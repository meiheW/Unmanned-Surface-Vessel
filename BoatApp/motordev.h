#ifndef _MOTOR_DEV_H_
#define _MOTOR_DEV_H_
#include <stddef.h>
#include <inttypes.h>
#include "modbus.h"
#ifdef __cplusplus
extern "C" {
#endif

#define OPLOOPS          (2)
#define SPEED_RATIO      (10)

//寄存器地址
#define FORMAT_ad        (0x6000)
#define CONTROL_ad       (0x6040)
#define QUICKSTOP_ad     (0x605A)
#define OPERATION_ad     (0x6060)
#define DESTINATION_ad   (0x607A)
#define IMME_SPEED_ad    (0x6081)
#define IMME_ACC_ad      (0x6083)
#define IMME_DEC_ad      (0x6084)
#define QUICK_DEC_ad     (0x6085)

//控制指令用于对寄存器赋值
#define INIT_DRIVER      (0x0001)
#define RUN_DRIVER       (0X0003)
#define INIT_MOTOR       (0x000F)
#define RUN_MOTOR        (0x001F)
#define STOP_MOTOR       (0x010F)

typedef enum
{
	OPNONE = 0,
	LOCATIONmode,
	SPEEDmode,
	TORQUEmode,
	TOZEROmode
}OPMODE;

typedef enum
{
	QUNONE = 0,
	CURDEC,
	QUICKDEC,
	IMMEDEC
}QUICKSTOPMODE;



typedef struct __motor_t_
{
    modbus_t *ctx;
    int     addrid;

}motor_t;

uint16_t get_format();
int init_driver();
int run_driver();
int run_motor();
int set_operation_mode(OPMODE mode);
int set_cruise_speed(uint32_t speed);
uint32_t get_cruise_speed();
int send_cruise_speed();
int set_acce_speed(uint16_t speed);
uint16_t get_acce_speed();
int send_acce_speed();
int set_dec_speed(uint16_t speed);
uint16_t get_dec_speed();
int send_dec_speed();
int set_destination(uint32_t position);
int run_to_destination();
int stop_run();
int set_quick_dec_speed(uint16_t speed);
uint16_t get_quick_dec_speed();
int send_quick_dec_speed();
int quick_stop(QUICKSTOPMODE mode);


void write_register(modbus_t *pctx, int addr, uint16_t val0, uint16_t val1);

int motor_init(char* port,int addrid );

int motor_close(motor_t * mt);

#ifdef __cplusplus
}
#endif
#endif
