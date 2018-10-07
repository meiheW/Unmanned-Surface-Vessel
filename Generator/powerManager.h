#ifndef _POWER_MANAGER_H_
#define _POWER_MANAGER_H_
#include <stddef.h>
//#include <inttypes.h>
#include "modbus.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	PMNONE = 0,
	PMOPEN,
	PMCLOSE,
	PMREAD
}POWER_CMD;

typedef enum
{
	SNONE = 0,
	ADDR1,
	ADDR2,
	ADDR3,
	ADDR4
}SLAVE_ID;

typedef struct __powermang_t_
{
    modbus_t *ctx;
    int       addrid1;
    int       addrid2;
    int       addrid3;
    int       addrid4;

}powermang_t_;


int open_modbus_rtu_powermang(const char *device, int baud, char parity, int data_bit, int stop_bit, int slave);
void close_modbus_rtu_powermang();
int get_addr();
void set_addr(int zqwl_addr);
int read_coil_status(modbus_t *ctx, int coil_index);
int write_single_coil(modbus_t *ctx, int coil_index, int status);
int open_powermang(int mang_index, int coil_index);
int close_powermang(int mang_index, int coil_index);
int read_powermang(int mang_index, int coil_index);
int powermang_init(char* port,int * addrid );
int powermang_connect(SLAVE_ID slave_id);
int powermang_disconnect();
int powermang_ctrl (POWER_CMD cmd ,SLAVE_ID slave_id, int coil_index);
int powermang_getinfo(SLAVE_ID slave_id, int coil_index);
int powermang_close();
int test();
int mytest();
//int powermang_init(char* port,int * addrid );
//int powermang_ctrl (powermang_t_ * pm, int cmd );
//int powermang_getinfo(powermang_t_ * pm);
//int powermang_close(powermang_t_ * pm);


#ifdef __cplusplus
}
#endif
#endif
