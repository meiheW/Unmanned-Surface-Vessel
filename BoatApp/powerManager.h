#ifndef _POWER_MANAGER_H_
#define _POWER_MANAGER_H_
#include <stddef.h>
#include <inttypes.h>
#include "modbus.h"
#include "modbus-tcp.h"

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

int open_modbus_tcp_powermang(const char *ip_address, int port);
void close_modbus_tcp_powermang();
int open_powermang(int mang_index, int coil_index);
int close_powermang(int mang_index, int coil_index);
int read_powermang(int mang_index, int coil_index);
int read_coil_status(modbus_t *ctx, int coil_index);
int write_single_coil(modbus_t *ctx, int coil_index, int status);
void set_addr(int zqwl_addr);
int read_baud(int mang_index);
int get_addr(int mang_index);
void closeall();
void openall();


#ifdef __cplusplus
}
#endif
#endif
