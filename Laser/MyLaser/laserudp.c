/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2017  <>
 * 
 * motor-term is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * motor-term is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <math.h>

#include "laserudp.h"


#define BUFFSIZE     (64)	 
#define MY_PORT	     (2368)
#define SERVER_PORT  (2368)


int nprintline=0;

char* DataBlockFlag  = "\xff\xee"; 
const char * SERVER_IP = "192.168.1.201";
static int sock = -1;
volatile int finish = 1;
volatile float fsh_angle = 0;

struct sockaddr_in serveraddr;    
struct sockaddr_in myaddr;   

static int get_command();


static void  init_sock()
{
	
	 if( -1 == sock )
	 {
    
	    memset(&myaddr, 0, sizeof(myaddr));
	    myaddr.sin_family = AF_INET;
	    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	    myaddr.sin_port = htons(MY_PORT);
	    
	    
	    memset(&serveraddr, 0, sizeof(serveraddr));
	    serveraddr.sin_family = AF_INET;
	    serveraddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	   
	    serveraddr.sin_port = htons(SERVER_PORT);

	    // socket
	    sock = socket(AF_INET, SOCK_DGRAM, 0);
	    if(sock < 0)
	    {
	    	perror("sock");
	        exit(-1);
	    }

	    int flags = fcntl(sock, F_GETFL, 0); // 获取当前状态
	    fcntl(sock, F_SETFL, flags|O_NONBLOCK); // 非阻塞设置

	    // bind
	    if(bind(sock, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0){
	        perror("bind!!!!!!!!!!");
	        exit(-1);
	    }
    }

}

int sendbuf( char* buf, size_t len )
{
	init_sock();
	
	int r = sendto(sock, buf, len, 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr) );
	
	printf( "send:%s,%d\n",buf,r );
	
	return r;

}


void recvsocket(void)
{

   	init_sock();
	// Init Done
	int bytes = 0;
	char buf[1248];
	struct sockaddr_in addr; 
	int len = sizeof(addr);

	printf("start receive.\n");
	while(1)
	{
		//printf("enter while.\n");
		bytes = recvfrom(sock, buf, 1248, 0, (struct sockaddr*)&addr, (socklen_t*)&len);
		if(bytes > 0)
		{
			getReturnMode(buf);
			printf("bytes: %d\n", bytes);
			
		//	char* pstart = buf;
			
			
			//for(int i=0; i<bytes; i++)
				//printf("%.2x ", buf[i]);
			//printf("\nparse laser ... ...");
			parselaser(buf, bytes);
		}
		usleep(10); 
	}
	
	
}

void parselaser(char* p, int len)
{
	//find the data block.
	int flagindex = 0;
	int flagpos = 0;
	printf("parse begin.\n");
	
	double azimarr[12];
	double azimextend[24];
	double distarr[12][32];//12 Block,32 Data
	while(-1 != (flagpos= findstr(p, flagpos, len-1, DataBlockFlag, 2)) )
	{
		
		int azimuthpos = flagpos + FLAG_LEN;
		double azimuth = getAzimuth(p, azimuthpos);
		azimarr[flagindex] = azimuth;

		//printf("flagindex:%d, azimuth: %lf\n", flagcount, azimuth);	
		//all data in a Data Block
		int datapos = azimuthpos + AZIMUTH_LEN;
			int curdatapos = datapos;
		for(int dataindex=0; dataindex<32; dataindex++)
		{
			double d = getDistance(p, curdatapos);
			int r = getReflectivity(p, curdatapos);
			
			distarr[flagindex][dataindex] = d; 
			if(r>100)
				printf("dataid:%d , dist:%lf, refle:%d \n", dataindex, d, r);
			curdatapos +=(DISTANCE_LEN+REFLECT_LEN);
		}
		printf("\n");
	
		flagpos += BLOCK_LEN;
		flagindex++;
	}
	
	//azimarr[12]:    	12 azimuth data. -->24
	//distarr[12][32]:	12*2*16 dist data.
	//extend the azimarr[12] ---> azimextend[24]

	for(int i=0; i<12; i++)
	{
		azimextend[2*i] = azimarr[i]; 
		if(i!=11)
			azimextend[2*i+1] = azimextend[2*i]+azimextend[2*i+2];
		else
			azimextend[2*i+1] = 2*azimextend[2*i]-azimextend[2*i-1];
	}

	for(int i=0; i<12; i++)
	{
		for(int j=0; j<32; j++)
		{
			

		
		
		}
	
	
	}



	
	
	
	
	//printf("flagcount: %d\n", flagcount);
}

double getAzimuth(char* p, int azimuthpos)
{
	uint8_t azimuth_h = p[azimuthpos];
	uint8_t azimuth_l = p[azimuthpos + 1];

	int combinedbytes = (azimuth_l << 8) + azimuth_h;
	double ret = combinedbytes * 1.0 / 100;

	//printf("Azimuth ret: %lf\n", ret);i
	
	return ret;
}

double getDistance(char* p, int datapos)
{
	int distancepos = datapos;
	uint8_t distance_h = p[distancepos];
	uint8_t distance_l = p[distancepos + 1];
	//
//	printf("distance_h:%.2x", distance_h);
//	printf("distance_l:%.2x", distance_l);

	uint32_t combinedbytes = (distance_l << 8) + distance_h;
	//printf("combinedbytes:%d\n", combinedbytes);
	//int sum = 2 * combinedbytes;
	//printf("sum:%d\n", sum);
	double ret = combinedbytes * 2.0 / 1000;
	//printf("Distance ret: %lf\n", ret);
	return ret;
}

int getReflectivity(char* p, int datapos)
{
	int reflectpos = datapos + DISTANCE_LEN;
	uint8_t curreflect = p[reflectpos];
	int ret = curreflect;
	return ret;
}

int getReturnMode(char* p)
{
	int modepos = 0x4DE;
	//printf("mode:%d\n", returnmodepos);
	uint8_t value = p[modepos - HEAD_LEN];
	//printf("value:%d\n", value);

	switch(value)
	{
		case 0x37:
			printf("Strongest Return.\n");
			break;
		case 0x38:
			printf("Last Return.\n");
			break;
		case 0x39:
			printf("Dual Return.\n");
			break;
		default:
			return -1;
	}
	return 0;
}


int findstr(char* buf, int startpos, int endpos, char* subbuf, int sublen)
{
	if(endpos-startpos+1 < sublen) return -1;

	int i=0;
	int j=0;
	for(i=startpos; i <= endpos-sublen+1; i++)
	{
		for(j=0; j<sublen; j++)
		{
			if(*(buf+i+j) != *(subbuf+j))
			{
				break;
			}
		}
		if(j == sublen) return i;
	}

	return -1;

}

int main()
{
//	char* map = "\x33\x71";
//	double ret1 = getAzimuth(map, 0);	//test azimuth
//	char* mdp = "\x89\x59";
//	double ret2 = getDistance(mdp,0);	//test distance
	


	printf("ok!");
		
	pthread_t parsesocketid;

	if(0 != pthread_create(&parsesocketid,NULL,(void*)recvsocket,NULL)) return -1;
	if(0 != pthread_detach(parsesocketid)) return -1;
	
	
	

	for(;;)
	{

		


		get_command();
		
		usleep(1000);
		
	}
	return (0);
}

static int get_command()
{
    fd_set rfds;
    struct timeval tv;
    int n=0;
    char buf_show[BUFFSIZE];
	
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 10; //timeout
    //any key
	
    if (select(1, &rfds, NULL, NULL, &tv) > 0)
    {
	n = read(STDIN_FILENO, buf_show, BUFFSIZE);
	buf_show[n-1]=0;		

	if( strcmp(buf_show,"exit" ) ==0 ||
	     strcmp(buf_show,"q" ) ==0)
        {
			printf( "exit\n");
			exit(1);
		}
		else
		{
			sendbuf( buf_show, strlen(buf_show));
			
		}
	
	}

    return 0;
}
