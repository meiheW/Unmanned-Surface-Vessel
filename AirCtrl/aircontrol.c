#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "aircontrol.h"

modbus_t *actx=NULL;


//#define ROOMTEMP_ad				(0x101F)	//room temperature
//#define PIPETEMP_ad				(0x1020)	//pipe temperature
//#define WATERTEMP_ad				(0x1021)	//water temperature


//------- function code : H03 ---------//
//????? format ????
//room temperature
int get_room_temperature()
{
	uint16_t rq[2];
	modbus_read_registers(actx, ROOMTEMP_ad, 2 ,rq);
	int temp = (rq[0]<<16) + rq[1];
	return temp;
}

//pipe temperature
int get_pipe_temperature()
{
	uint16_t rq[2];
	modbus_read_registers(actx, PIPETEMP_ad, 2 ,rq);
	int temp = (rq[0]<<16) + rq[1];
	return temp;
}

//sea water temperature
int get_water_temperature()
{
	uint16_t rq[2];
	modbus_read_registers(actx, WATERTEMP_ad, 2 ,rq);
	int temp = (rq[0]<<16) + rq[1];
	return temp;
}

//------- function code : H01 ---------//
//????????????
int compressor_high_pressure_alarm()
{
	uint8_t res;
	if(1 != modbus_read_bits(actx, HPREALARM_ad, 1, &res) )
	{
		printf("read compressor_high_pressure_alarm wrong !!!\n");
		return -1;
	}

	//??what to return ????
	return res;
}

int compressor_low_pressure_alarm()
{
	uint8_t res;
	if(1 != modbus_read_bits(actx, LPREALARM_ad, 1, &res) )
	{
		printf("read compressor_high_pressure_alarm wrong !!!\n");
		return -1;
	}

	//??what to return ????
	return res;
}

int turn_on_heat_alarm()
{
	uint8_t res;
	if(1 != modbus_read_bits(actx, OPENELEALARM_ad, 1, &res) )
	{
		printf("read compressor_high_pressure_alarm wrong !!!\n");
		return -1;
	}

	//??what to return ???? need test
	return res;

}


//------- function code : H05 ---------//
//zhi leng
int set_cooling_status(int status)
{
	for(int i=0; i< OPLOOPS && 1 != modbus_write_bit(actx, COLDING_ad, status); i++)
	{
		if(OPLOOPS-1 == i)
		{
			printf("set cooling status failed.\n");
			return -1;
		}
	}
	return 0;
}

int cool_on()
{
	set_cooling_status(1);
	return 0;
}

int cool_off()
{
	set_cooling_status(0);
	return 0;
}

//zhi re.
int set_heating_status(int status)
{
	for(int i=0; i< OPLOOPS && 1 != modbus_write_bit(actx, HEATING_ad, status); i++)
	{
		if(OPLOOPS-1 == i)
		{
			printf("set heating status failed.\n");
			return -1;
		}
	}
	return 0;
}

int heat_on()
{
	set_heating_status(1);
	return 0;
}

int heat_off()
{
	set_heating_status(0);
	return 0;
}

//dain jia re.
int set_ele_heating_status(int status)
{
	for(int i=0; i< OPLOOPS && 1 != modbus_write_bit(actx, ELEHEATING_ad, status); i++)
	{
		if(OPLOOPS-1 == i)
		{
			printf("set ele heating status failed.\n");
			return -1;
		}
	}
	return 0;
}

int ele_heat_on()
{
	set_ele_heating_status(1);
	return 0;
}

int ele_heat_off()
{
	set_ele_heating_status(0);
	return 0;
}


//gu zhang fu wei .reset
int fault_reset()
{
	for(int i=0; i< OPLOOPS && 1 != modbus_write_bit(actx, RESET_ad, 1); i++)
	{
		if(OPLOOPS-1 == i)
		{
			printf("reset failed.\n");
			return -1;
		}
	}
	return 0;


}


//------- function code : H16 ---------//
//set room temperature. default:22
int set_room_temperature(int temp)
{
	write_register(actx, ROOMTEMPSET_ad, (temp>>16) & 0xFFFF, temp & 0xFFFF);
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

int airctrl_init(char* port,int addrid)
{
    actx = modbus_new_rtu(port, 9600, 'N', 8, 2);
    modbus_set_slave(actx, addrid);
    //can be modified later
    //modbus_set_slave(ctx, SERVER_ID);
    if (modbus_connect(actx) == -1) 
    {
        fprintf(stderr, "Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(actx);
        return -1;
    }

    return 0;
}

int aurctrl_close()
{
    if( actx!=NULL )
    {
        modbus_close(actx);
        modbus_free(actx);
    }
    return 0;
}


int main()
{
    //.......//


}


