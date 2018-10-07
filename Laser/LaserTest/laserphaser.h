//*****laserphaser.h*****
#define PI 3.1415926

struct Laser{
	unsigned char   laser_id;
    float           azimuth;
   	char            vertical_angle;
    float           distance;
    unsigned char   reflectivity;
    float x;
   	float y;
    float z;
};


int InitiateUdp(void);
int printBasicData(unsigned char* receive_buffer);
float getAzimuthStep(unsigned char* receive_buffer);

