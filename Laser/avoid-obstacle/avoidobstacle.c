#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>
#include <time.h>

#include "avoidobstacle.h"

float avoidObstacle(struct ProjectData* pdata, int pindex, float start_angle, float end_angle)
{
	//pdata:distance(float),azimuth(float)
	
	//step1:histogram
	printf("pindex--: %d\n", pindex);
	float range_angle = (end_angle>start_angle) ? (end_angle-start_angle) : (end_angle+180-start_angle);
	float unit_angle  = 2;
	int unit_num = ceil(range_angle/unit_angle);

	//record nums in each interval.
	int* num_each_unit  = (int*)malloc(unit_num * sizeof(int));
	//memset(num_each_uint, 0 , uint_num*sizeof(int));
	for(int i=0; i<unit_num; i++)
		num_each_unit[i]  = 0;
	//printf("赋值前：\n");
	//printArray(num_each_uint, uint_num);
	for(int i=0; i<pindex; i++)
	{
		float curangle = pdata[i].azimuth;
		curangle = (curangle<=start_angle) ? curangle+180-start_angle : curangle;
		int   curindex = (curangle-start_angle) / unit_angle;//0.1.2...
		num_each_unit[curindex]++;
	}	
	//printf("赋值后：\n");
	printArray(num_each_unit, unit_num);
	//step2:analysis the hist: 1.num_each_uint 2.unit_num
	
	int start = -1;
	int end = unit_num;
	int isfound = 0;
	int maxlen = 0;

	int first=-1;
	int second = -1; 
	
	for(int i=0; i<unit_num; i++)
	{
		int curnum = num_each_unit[i];
		if(curnum==0 && !isfound)
		{
			start = i;
			isfound = 1;
		}

		if( (curnum==0 && (i==unit_num-1 || num_each_unit[i+1]!=0)) && isfound)
		{
			end = i;
			isfound = 0;
			if(end - start + 1> maxlen)
			{
				maxlen = end - start + 1; 
				first = start;
				second = end;
			}
		
		}
	}
	if(first==-1 && second==-1)
	{
		printf("障碍物太多\n");
		return -1;
	}

	float dire = (first+second+1)*1.0/2;
	float des = start_angle + dire * unit_angle;
	des = (des >= 360) ? des-360 : des;
	
	printf("first = %d, end = %d\n", first, second);
	printf("最终角度: %f\n", des);

	return des;

}

void printArray(int* arr, int num)
{
	for(int i=0; i<num; i++)
		printf("%d ", arr[i]);
	printf("\n....print over....\n");
}


