#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>
#include <time.h>

#include "avoid1.h"
//#include "laserphaser.h"

int   sector_n = 18;
float sector_max_length = 800.0;  //mm
float sector_min_length = 400.0;


float sector_angle_step = 0.0;
float sector_length_step = 0.0;
float sector_length[18];

void setBubbleSize(float start_angle, float end_angle)
{

    sector_angle_step = (end_angle - start_angle) / sector_n;
    sector_length_step = (sector_max_length - sector_min_length) / (sector_n/2 - 1);
//    sector_length[sector_n];
    for (int i = 0; i < sector_n/2; i+=1)
    {
        sector_length[i] = sector_min_length + i*sector_length_step;
        sector_length[sector_n - 1 - i] = sector_length[i];
    }
}

int avoidObstacle(struct ProjectData* pdata, int pindex, float start_angle, float end_angle)
{
  int bubble_have_obstacle = 0;
  for (int i = 0; i < pindex; i++)
  {
      int index;
      index = (pdata[i].azimuth - start_angle)/sector_angle_step;
      
      if (pdata[i].distance < sector_length[index])
      {
          bubble_have_obstacle = 1;     
          break;
	  }
  }

  // 小车圈内有障碍物，则计算新的朝向角度（障碍物少的方向）
  if (bubble_have_obstacle == 1)
  {
      double denominator = 0;
      double numerator = 0;
      double new_angle = 0;
      for (int i = 0; i < pindex; i++)
      {            
          numerator   += (pdata[i].azimuth * pdata[i].distance);            
          denominator += pdata[i].distance;
      }
      
	  new_angle = numerator / denominator;
                printf("\t转向%.2f°以避开障碍物\n", new_angle);
  }
  else
  {
     // printf("\n当前扫描周期 bubble 内无障碍：没有\n");
  }

    return 0;
}

