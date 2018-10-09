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
#include <stddef.h>
#include "kserialsport.h"
#include "boatdev.h"

#define BUFFSIZE 64

static uart_t * g_myuart =NULL;
static boatstatus_t  g_mystatus;
static boatcontrol_t g_mycontrol;
static uint8_t boatloop = 1;

//parse upload data.
void myuart_callback( int id, uint8_t *buffer, int len)
{ 
    printf("\ncallback:len=%d,", len );

    uint8_t* xbuf= malloc( len + 1 );  
    memcpy( xbuf,buffer,len);
    xbuf[len]=0;
    printf("buf:%s\n",xbuf );

	//data need to get 1-14,sum-15
	int16_t angled;
	int16_t  anglen;
	uint32_t mot_speed;
	int16_t water_temperature;
	int16_t oil_pressure;
	int16_t oil_bin;
	uint16_t vol_12;
	uint16_t vol_24;
	uint32_t gen_speed;
	uint16_t op_mode;
	int16_t angled2;
	int16_t anglen2;
	int16_t mot_speed2;
	uint32_t dev_io;
	uint16_t sum;//check_sum

    //parse uart data.
	uint8_t* pd = xbuf;
	pd = pd + 4;
	//01--int16_t--angled;
	memcpy(&angled, pd, 2);
	pd = pd + 2;
	//02--int16_t--anglen;
	memcpy(&anglen, pd, 2);
	pd = pd + 2;
	//03--uint32_t--mot_speed
	memcpy(&mot_speed, pd, 4);
	pd = pd + 4;
	//04--int16_t--water_temperature
	memcpy(&water_temperature, pd, 2);
	pd = pd + 2;
	//05--int16_t--oil_pressure
	memcpy(&oil_pressure, pd, 2);
	pd = pd + 2;
    //06--int16_t--oil_bin
	memcpy(&oil_bin, pd, 2);
	pd = pd + 2;
    //07--uint16_t--vol_12
	memcpy(&vol_12, pd, 2);
	pd = pd + 2;
    //08--uint16_t--vol_24
	memcpy(&vol_24, pd, 2);
	pd = pd + 2;
	//09--uint32_t--gen_speed
	memcpy(&gen_speed, pd, 4);
	pd = pd + 4;
    //10--uint16_t--op_mode
	memcpy(&op_mode, pd, 2);
	pd = pd + 2;
    //11--int16_t--angled2
	memcpy(&angled2, pd, 2);
	pd = pd + 2;
    //12--int16_t--anglen2
	memcpy(&anglen2, pd, 2);
	pd = pd + 2;
	//13--mot_speed2--int16_t
	memcpy(&mot_speed2, pd, 2);
	pd = pd + 2;
    //14--dev_io--uint32_t
	memcpy(&dev_io, pd, 4);
	pd = pd + 4;
    
	//CHECK_SUM
	
	memcpy(&sum, pd, 2);

	uint16_t res=0;
	uint8_t tmp;
	for(int i=0; i<38; i++)
	{
		memcpy(&tmp, xbuf+i, 1);
		res += tmp;
	}

	if(res != sum)
	{
		printf("check sum wrong.\n");
		free(xbuf);
		return;
	}
	//if crc ... g_mystatus update.
	//g_mystatus
	g_mystatus.angled = angled;
    g_mystatus.anglen = anglen;
    g_mystatus.mot_speed = mot_speed;
    g_mystatus.water_temperature = water_temperature;
    g_mystatus.oil_pressure = oil_pressure;
    g_mystatus.oil_bin = oil_bin;
    g_mystatus.vol_12 = vol_12;
    g_mystatus.vol_24 = vol_24;
    g_mystatus.gen_speed = gen_speed;
    g_mystatus.op_mode = op_mode;
    g_mystatus.angled2 = angled2;
    g_mystatus.anglen2 = anglen2;
	g_mystatus.mot_speed2 = mot_speed2;
    g_mystatus.dev_io = dev_io;    

    free(xbuf);
} 

void sendstatus()
{
	uint8_t mycontrol[18];
	int16_t angled = g_mycontrol.angled;
	int16_t anglen = g_mycontrol.anglen;
	int16_t mot_speed = g_mycontrol.mot_speed;
	uint16_t op_mode = g_mycontrol.op_mode;
	uint32_t dev_io = g_mycontrol.dev_io;
	
	uint8_t* dp = mycontrol;
	//prefix : 0xFFFFFFXX
	boatloop %= 256;
	uint8_t prefix[4] ={0xFF, 0xFF, 0xFF, boatloop++}; 
	memcpy(dp, prefix, 4);
	dp = dp+4; 
	
	//int16_t     angled;
	memcpy(dp, &angled, 2);
	dp = dp+2;
    //int16_t     anglen;
	memcpy(dp, &anglen, 2);
	dp = dp+2;
    //int16_t    mot_speed;   
	memcpy(dp, &mot_speed, 2);
	dp = dp+2;
    //uint16_t    op_mode;
	memcpy(dp, &op_mode, 2);
	dp = dp+2;
    //uint32_t    dev_io;   
	memcpy(dp, &dev_io, 4);
	dp = dp+4;

	//check_sum
	uint16_t sum=0;
	uint8_t tmp = 0;
	for(int i=0; i<16; i++)	//first 16 bytes get sum.
	{
		memcpy(&tmp, mycontrol+i, 1);
		sum += tmp;
	}
	memcpy(dp, &sum, 2);
	
	boat_sendcmd(mycontrol, 18);

}


int boat_init( const char* port )
{

    g_myuart = uart_open( port,myuart_callback,1,115200,8,'N',1);
    if( g_myuart == NULL ) return -1;
    return 0;
}
int boat_getinfo(boatstatus_t * info)
{
    return 0;
}
int boat_sendcmd( uint8_t * buf, int len )
{
    uart_write( g_myuart, buf,  len );
    return 0;
}
int boat_close( )
{
    uart_close(g_myuart);
    return 0;
}


int main(){

    return 0;

}

