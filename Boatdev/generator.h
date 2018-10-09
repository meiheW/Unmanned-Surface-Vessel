#ifndef _GENERATOR_MANAGER_H_
#define _GENERATOR_MANAGER_H_
#include <stddef.h>
#include <inttypes.h>
#include "modbus.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OPLOOPS				 (2)


//Ö»¶Á¼Ä´æÆ÷µØÖ·


//¶ÁÐ´¼Ä´æÆ÷µØÖ·
				

typedef enum
{
	motorOFF = 0,
	motorPREHEAT,
	motorSTART,
	motorFUNCTION,
	motorSHUTDOWN,
	motorCOOLING,
	motorOFFPERMANENT
}MOTORSTATUS;


typedef struct __mase_gen_t_
{
    modbus_t *ctx;
    int     addrid;

}masegen_t;

int generator_init(char* port,int addrid );
int generator_ctrl (masegen_t * masegen, int cmd );
int generator_getinfo(masegen_t * masegen);
int generator_close(masegen_t * masegen);

#ifdef __cplusplus
}
#endif

#endif
