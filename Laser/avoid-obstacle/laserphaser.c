#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>
#include <time.h>

#include "laserphaser.h"
#include "avoidobstacle.h"

// 垂直角度数组
char vertical_angle[16] = {-15,1,-13,-3,-11,5,-9,7,-7,9,-5,11,-3,13,-1,15};

float max_distance = 1000;	//2m
float max_z = 1000.0;	//mm
float min_z = 0.0;

float start_angle = 0.0;//angle
float end_angle = 90.0;



int printBasicData(unsigned char* receive_buffer)
{
	
    printf("激光点z轴取值范围：%.1f~%.1f\n", min_z, max_z);

    printf("扫描范围：\t\%.1f~%.1f°\n", start_angle, end_angle);

    if (receive_buffer[1205] == 0x22)  printf("激光型号为 VLP-16\n");
    if (receive_buffer[1204] == 0x37)  printf("返回模式是 strongest\n");

	return 0;
}

float getAzimuthStep(unsigned char* receive_buffer)
{

    float azimuth_step;
    int azimuth0 = 0, azimuth1 = 0;
    memcpy(&azimuth0, receive_buffer + 2, 2);
    memcpy(&azimuth1, receive_buffer + 102, 2);
    if (azimuth1 > azimuth0)  azimuth_step = (azimuth1 - azimuth0)/2.0/100.0;
    else azimuth_step = (azimuth1 + 36000 - azimuth0)/2.0/100.0;
    printf("水平步距角：%3.2f\n", azimuth_step);
	
	return azimuth_step;
}


int main(int argc, char *argv[])
{
	struct Laser laser[384];
    int socket_id = InitiateUdp();

    unsigned char receive_buffer[2048] = {0};  // 一个包有 1206 字节.
    int bytes = recvfrom(socket_id, receive_buffer, 1206, 0, NULL, NULL);
	if(bytes==0)	return -1;

	printBasicData(receive_buffer);
    //attetion:the value of azimuth is related to the motor run frequency, and its better to get it from config
    float azimuth_step = 0.1;
    //float azimuth_step = getAzimuthStep(receive_buffer);
	
	sleep(2);

    // 一个扫描周期的数据包的数目
    int udp_nums_per_cycle = ceil((end_angle - start_angle) / (azimuth_step * 24));
	struct ProjectData* pdata = (struct ProjectData*)malloc(udp_nums_per_cycle * 12 * 2 * 16 * sizeof(struct ProjectData));

    int pindex = 0;
    int udp_nums = 0;

	int begint, endt;
	//start timing.
	begint = clock();
    
	while(1)
    {      
        bytes = recvfrom(socket_id, receive_buffer, 2048, 0, NULL, NULL);  
        if(bytes!=1206) continue;
	
	for (int i = 0; i < 12; ++i) 
        {
            for (int j = 0; j < 32; ++j)
            {
                // 激光点水平角azimuth
                int azimuth = 0;
                memcpy(&azimuth, receive_buffer + 2 + 100*i, 2);
                laser[i*32+j].azimuth = azimuth/100.0 + j/16*azimuth_step;

                // 激光点的垂直角度
                laser[i*32+j].vertical_angle = vertical_angle[j%16];

                // 激光点的距离distance
                int distance = 0;
                memcpy(&distance, receive_buffer + 4 + 100*i + 3*j, 2);
                laser[i*32+j].distance = distance*2;

                // 激光点的反射值reflectivity
                laser[i*32+j].reflectivity = receive_buffer[6 + 100*i + 3*j];

                // 激光点的笛卡尔坐标 x，y，z
                laser[i*32+j].x = laser[i*32+j].distance * cos(laser[i*32+j].vertical_angle*PI/180) * sin(laser[i*32+j].azimuth*PI/180);
                laser[i*32+j].y = laser[i*32+j].distance * cos(laser[i*32+j].vertical_angle*PI/180) * cos(laser[i*32+j].azimuth*PI/180); 
                laser[i*32+j].z = laser[i*32+j].distance * sin(laser[i*32+j].vertical_angle*PI/180);

                // 把符合要求的激光点（z 坐标范围）保存，projection_points[]保存当前周期的所有符合要求的点的投影长度和水平角
                if (laser[i*32 + j].z < max_z && laser[i*32 + j].z > min_z && laser[i*32 + j].distance < max_distance)
                { 
					pdata[pindex].distance = laser[i*32+j].distance * cos(laser[i*32+j].vertical_angle*PI/180);
					pdata[pindex].azimuth  = laser[i*32+j].azimuth;
                    pindex += 1;
                }
            }
        }  //一个数据包处理结束

        udp_nums++;  //UDP包计数

        if( udp_nums >= udp_nums_per_cycle ) //如果收满一周期的包
        {
			printf("udp_nums:%d\n", udp_nums_per_cycle);
			//***avoid obstacle	***//
			int boat_angle = avoidObstacle(pdata, pindex, start_angle, end_angle);
			printf("boat_angle = %d\n", boat_angle);
            
			udp_nums=0;       //当前扫描周期结束，UDP包数目清零
            pindex=0;         //
			
			//end timing.
			endt = clock();
			printf("processed time: %f ms\n",(endt - begint)*1000.0/CLOCKS_PER_SEC);
			begint = endt;
    
        }  //process cycle package 
    }  // while(1)-end

    return 0;
}

//initiate UDP
int InitiateUdp(void)
{
    int socket_id = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_id < 0) {puts("socket创建失败"); return -1;}

    struct sockaddr_in myaddr;
    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(2368); // 改为相应激光的端口

    if(bind(socket_id, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0)
    {
        perror("bind error");
        exit(-1);
    }
    
    return socket_id;
}

// 备用代码
// // 打印1个包的激光点数据
// for (int i = 0; i < 24; ++i)
// {
//     printf("\n\n/******* 第%d个数据包 *******/\n", udp_nums_per_cycle++);
//     printf("水平角：%6.2f°\n", laser[16*i].azimuth);
//     for (int j = 0; j < 16; ++j)
//     {
//         printf("\t垂直角：%3d°  %9.2fmm(x axis) %9.2fmm(y axis) %9.2fmm(z axis) %3d(反射值)\n",
//             laser[16*i + j].vertical_angle, laser[16*i + j].x, laser[16*i + j].y,
//             laser[16*i + j].z, laser[16*i + j].reflectivity);
//     }
// }
