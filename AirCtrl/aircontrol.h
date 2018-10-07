#ifndef _AIR_CONTROL_H_
#define _AIR_CONTROL_H_
#include <stddef.h>
#include <inttypes.h>
#include "modbus.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OPLOOPS				 (2)


//read
#define ROOMTEMP_ad				(0x101F)	//room temperature
#define PIPETEMP_ad				(0x1020)	//pipe temperature
#define WATERTEMP_ad			(0x1021)	//water temperature

//Compressor
#define HPREALARM_ad			(0x0864)	//high pressure alarm
#define LPREALARM_ad			(0x0865)	//low  pressure alarm
#define OPENELEALARM_ad		    (0x0866)    //open electricity to heat alarm

//write
#define COLDING_ad				(0x0801)	//colding
#define HEATING_ad				(0x0802)	//heating
#define ELEHEATING_ad			(0x0803)	//electricity heating
#define RESET_ad				(0x0805)	//reset
#define ROOMTEMPSET_ad			(0x11B1)	//room temperature set


int get_room_temperature();
int get_pipe_temperature();
int get_water_temperature();
int compressor_high_pressure_alarm();
int compressor_low_pressure_alarm();
int turn_on_heat_alarm();
int set_cooling_status(int status);
int cool_on();
int cool_off();
int set_heating_status(int status);
int heat_on();
int heat_off();
int set_ele_heating_status(int status);
int ele_heat_on();
int ele_heat_off();
int fault_reset();
int set_room_temperature(int temp);
int read_single_register(modbus_t *pctx, int addr);
int write_single_register(modbus_t *pctx, int addr, uint16_t val);
void write_register(modbus_t *pctx, int addr, uint16_t val0, uint16_t val1);
int airctrl_init(char* port,int addrid);
int aurctrl_close();


#ifdef __cplusplus
}
#endif

#endif
