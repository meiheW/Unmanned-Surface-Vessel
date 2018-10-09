#ifndef _BOAT_DEV_H_
#define _BOAT_DEV_H_
#include <stddef.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

// struct  boatdev_t;
// typedef struct boatdev_t boatdev_t;

// struct boatdev_t 
// {
//     char        portname[32];// serials port R232 
                
// };
typedef struct __boatstatus_t 
{
    int16_t     angled;
    int16_t     anglen;
    uint32_t    mot_speed;   //motor speed
    int16_t     water_temperature;
    int16_t     oil_pressure;
    int16_t     oil_bin;
    uint16_t    vol_12;
    uint16_t    vol_24;
    uint32_t    gen_speed;
    uint16_t    op_mode;
    int16_t     angled2;
    int16_t     anglen2;
	int16_t     mot_speed2;
    uint32_t    dev_io;    
}boatstatus_t;


typedef struct __boatcontrol_t 
{
    int16_t     angled;
    int16_t     anglen;
    int16_t     mot_speed;   
    uint16_t    op_mode;
    uint32_t    dev_io;    
}boatcontrol_t;



int boat_init( const char* port );
int boat_getinfo(boatstatus_t * info);
int boat_sendcmd( uint8_t * buf, int len );
int boat_close( );

#ifdef __cplusplus
}
#endif

#endif
