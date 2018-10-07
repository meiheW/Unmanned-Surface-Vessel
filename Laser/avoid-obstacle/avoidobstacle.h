//******avoidobstacle.h******
struct ProjectData{
	float azimuth;
	float distance;
};

float avoidObstacle(struct ProjectData* pdata, int pindex, float start_angle, float end_angle);
void printArray(int* , int);
