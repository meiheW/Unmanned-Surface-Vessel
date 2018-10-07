//******avoid1.h******
struct ProjectData{
	float azimuth;
	float distance;
};


void setBubbleSize(float start_angle, float end_angle);
int avoidObstacle(struct ProjectData* pdata, int pindex, float start_angle, float end_angle);

