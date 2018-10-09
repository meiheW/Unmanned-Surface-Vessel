#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "generator.h"

masegen_t* mygen = NULL;


//VAC voltage
int get_VAC_voltage()
{
	int volt = read_single_register(mygen->ctx, VACVOLT_ad);
	return volt;
}

//VAC frequency
int get_VAC_frequence()
{
	int vacfreq = read_single_register(mygen->ctx, VACHZ_ad);	
	vacfreq /= 10;
	return vacfreq;

}

//oil pressure value(bar)
int get_oil_pressure()
{
	int oilpressure = read_single_register(mygen->ctx, OILPRESSBAR_ad);	
	return oilpressure;

}

//fuel level value(%)
int get_fuel_level()
{
	int fuellevel = read_single_register(mygen->ctx, FUELLEVEL_ad);	
	return fuellevel;
}

//motor status
MOTORSTATUS get_motor_status()
{
	int status = read_single_register(mygen->ctx, MOTORSTATUS_ad);
	if(status < 0 || status >  6)
		printf("read status not range from 0 to 6.\n");

	MOTORSTATUS curstatus;
	switch(status)
	{
		case 0:
			curstatus = motorOFF;
			break;
		case 1:
			curstatus = motorPREHEAT;
			break;
		case 2:
			curstatus = motorSTART;
			break;
		case 3:
			curstatus = motorFUNCTION;
			break;
		case 4:
			curstatus = motorSHUTDOWN;
			break;
		case 5:
			curstatus = motorCOOLING;
			break;
		case 6:
			curstatus = motorOFFPERMANENT;
			break;
		default:
			break;
	}

	return curstatus;
}

int generator_run()
{
	write_single_register(mygen->ctx, OUTPUTSTRUCT_ad, STARTGEN);

	return 0;
}

int generator_stop()
{
	write_single_register(mygen->ctx, OUTPUTSTRUCT_ad, STOPGEN);

	return 0;
}

int read_single_register(modbus_t *pctx, int addr)
{
	uint16_t tab_rp_register;

	for(int i=0; i< OPLOOPS && 1 != modbus_read_registers(pctx, addr, 1, &tab_rp_register); i++)
	{
		if(OPLOOPS-1 == i)
		{
			printf("stop motor run failed.\n");
			return -1;
		}
	}

	int ret = tab_rp_register;
	return ret;
}

int write_single_register(modbus_t *pctx, int addr, uint16_t val)
{
	for(int i=0; i< OPLOOPS && 1 != modbus_write_register(pctx, addr, val); i++)
	{
		if(OPLOOPS-1 == i)
		{
			printf("stop motor run failed.\n");
			return -1;
		}
	}
	return 0;
}

void write_register(modbus_t *pctx, int addr, uint16_t val0, uint16_t val1)
{
    uint16_t tab_rq_registers[2];
	tab_rq_registers[0] = val0;
	tab_rq_registers[1] = val1;
	modbus_write_registers(pctx,  addr,  2,  tab_rq_registers);
}

int generator_init(char* port,int addrid)
{
    mygen = (masegen_t*)malloc(sizeof(masegen_t));

    mygen->addrid = addrid;
    mygen->ctx = modbus_new_rtu(port, 19200, 'N', 8, 1);
    modbus_set_slave(mygen->ctx, addrid);
    //can be modified later
    //modbus_set_slave(ctx, SERVER_ID);
    if (modbus_connect(mygen->ctx) == -1) 
    {
        fprintf(stderr, "Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(mygen->ctx);
        free(mygen);
        return 0;
    }

    return 0;
}

int generator_close(masegen_t * masegen)
{
    if( masegen!=NULL )
    {
        modbus_close(masegen->ctx);
        modbus_free(masegen->ctx);
        free(masegen);
    }
    return 0;
}

int main(  )
{
    
   
    return 0;
}
