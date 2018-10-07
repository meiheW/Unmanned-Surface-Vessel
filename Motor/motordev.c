#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "motordev.h"


motor_t* pmotor = NULL;
uint32_t cruise_speed;
uint16_t acce_speed;
uint16_t dec_speed;
uint16_t quick_dec_speed;


//¿¿¿¿¿¿
uint16_t get_format()
{
	uint16_t format;
	if(-1==modbus_read_registers(pmotor->ctx, FORMAT_ad, 1, &format))
	{
		printf("modbus read format failed.\n"); 
		return -1;
	}
	return format; //format£º0--> h.l. ; 1--> l.h
}

//¿¿¿¿¿¿
int init_driver()
{
	for(int i=0; i< OPLOOPS && 1 != modbus_write_register(pmotor->ctx, CONTROL_ad, INIT_DRIVER); i++)
	{
		if(OPLOOPS-1 == i)
		{
			printf("init driver failed.\n");
			return -1;
		}
	}
	return 0;
}

//¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
int run_driver()
{
	for(int i=0; i< OPLOOPS && 1 != modbus_write_register(pmotor->ctx, CONTROL_ad, RUN_DRIVER); i++)
	{
		if(OPLOOPS-1 == i)
		{
			printf("run driver failed.\n");
			return -1;
		}
	}
	return 0;
}

//¿¿¿¿¿¿¿¿¿¿¿
int run_motor()
{
	for(int i=0; i< OPLOOPS && 1 != modbus_write_register(pmotor->ctx, CONTROL_ad, INIT_MOTOR); i++)
	{
		if(OPLOOPS-1 == i)
		{
			printf("run motor failed.\n");
			return -1;
		}
	}
	return 0;


}

//¿¿¿¿¿¿¿1.¿¿¿2.¿¿¿3.¿¿¿4.¿¿
int set_operation_mode(OPMODE mode)
{
	int mode_code = 0;
	switch(mode)
	{
		case LOCATIONmode:
			mode_code = 1;
			break;
		case SPEEDmode:
			mode_code = 3;
			break;
	    case TORQUEmode:
			mode_code = 4;
			break;
	    case TOZEROmode:
			mode_code = 6;
			break;
		default:
			break;
	}

	for(int i=0; i< OPLOOPS && 1 != modbus_write_register(pmotor->ctx, OPERATION_ad, mode_code); i++)
	{
		if(OPLOOPS-1 == i)
		{
			printf("send operation mode failed.\n");
			return -1;
		}
	}

	return 0;
}

//¿¿¿¿¿¿¿¿
int set_cruise_speed(uint32_t speed)
{
	cruise_speed = speed;
	return 0;
}

//¿¿¿¿¿¿¿¿
uint32_t get_cruise_speed()
{
	uint32_t speed = cruise_speed;
	return speed;
}

//¿¿¿¿¿¿¿¿¿
int send_cruise_speed()
{
	uint16_t tab_rq_registers[2];
	tab_rq_registers[0] = (cruise_speed >> 16) & 0xFFFF;
	tab_rq_registers[1] = cruise_speed & 0xFFFF;
	for(int i=0; i< OPLOOPS && 2 != modbus_write_registers(pmotor->ctx, IMME_SPEED_ad, 2, tab_rq_registers); i++)
	{
		if(OPLOOPS-1 == i)
		{
			printf("send cruise speed failed.\n");
			return -1;
		}
	}
	return 0;
}

//¿¿¿¿¿¿¿¿¿¿¿
int set_acce_speed(uint16_t speed)
{
	acce_speed = speed;
	return 0;
}

uint16_t get_acce_speed()
{
	uint16_t speed = acce_speed;
	return speed;
}

int send_acce_speed()
{
	acce_speed *= SPEED_RATIO;
	for(int i=0; i< OPLOOPS && 1 != modbus_write_register(pmotor->ctx, IMME_ACC_ad, acce_speed); i++)
	{
		if(OPLOOPS-1 == i)
		{
			printf("send acce speed failed.\n");
			return -1;
		}
	}
	return 0;

}

//¿¿¿¿¿¿¿¿¿¿¿
int set_dec_speed(uint16_t speed)
{
	dec_speed = speed;
	return 0;
}

uint16_t get_dec_speed()
{
	uint16_t speed = dec_speed;
	return speed;
}

int send_dec_speed()
{
	dec_speed *= SPEED_RATIO;
	for(int i=0; i< OPLOOPS && 1 != modbus_write_register(pmotor->ctx, IMME_DEC_ad, dec_speed); i++)
	{
		if(OPLOOPS-1 == i)
		{
			printf("send acce speed failed.\n");
			return -1;
		}
	}
	return 0;

}

//¿¿¿¿¿¿
int set_destination(uint32_t position)
{
	position = 200000;
	uint16_t destination[2];
	destination[1] = position & 0xFFFF;
	destination[0] = (position >> 16) & 0xFFFF;
	for(int i=0; i< OPLOOPS && 2 != modbus_write_registers(pmotor->ctx, DESTINATION_ad, 2, destination); i++)
	{
		if(OPLOOPS-1 == i)
		{
			printf("send destination failed.\n");
			return -1;
		}
	}
	return 0;


}

//¿¿¿¿¿¿¿¿
int run_to_destination()
{
	for(int i=0; i< OPLOOPS && 1 != modbus_write_register(pmotor->ctx, CONTROL_ad, RUN_MOTOR); i++)
	{
		if(OPLOOPS-1 == i)
		{
			printf("run to destination failed.\n");
			return -1;
		}
	}

	for(int i=0; i< OPLOOPS && 1 != modbus_write_register(pmotor->ctx, CONTROL_ad, INIT_MOTOR); i++)
	{
		if(OPLOOPS-1 == i)
		{
			printf("run to destination failed.\n");
			return -1;
		}
	}
	return 0;

}

//¿¿
int stop_run()
{
	for(int i=0; i< OPLOOPS && 1 != modbus_write_register(pmotor->ctx, CONTROL_ad, STOP_MOTOR); i++)
	{
		if(OPLOOPS-1 == i)
		{
			printf("stop motor run failed.\n");
			return -1;
		}
	}
	return 0;	
}

//¿¿¿¿¿¿¿¿¿¿¿¿¿
int set_quick_dec_speed(uint16_t speed)
{
	quick_dec_speed = speed;
	return 0;
}

uint16_t get_quick_dec_speed()
{
	uint16_t speed = quick_dec_speed;
	return speed;
}

int send_quick_dec_speed()
{
	dec_speed *= SPEED_RATIO;
	for(int i=0; i< OPLOOPS && 1 != modbus_write_register(pmotor->ctx, QUICK_DEC_ad, dec_speed); i++)
	{
		if(OPLOOPS-1 == i)
		{
			printf("send acce speed failed.\n");
			return -1;
		}
	}
	return 0;

}

//¿¿¿¿
int quick_stop(QUICKSTOPMODE mode)
{
	for(int i=0; i< OPLOOPS && 1 != modbus_write_register(pmotor->ctx, QUICKSTOP_ad, mode); i++)
	{
		if(OPLOOPS-1 == i)
		{
			printf("quick stop failed.\n");
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

//¿¿¿¿¿
int motor_init(char* port,int addrid )
{
    pmotor = (motor_t*)malloc(sizeof(motor_t));

    pmotor->addrid = addrid;
    pmotor->ctx = modbus_new_rtu(port, 19200, 'N', 8, 1);
    modbus_set_slave(pmotor->ctx, addrid);
    
	if (modbus_connect(pmotor->ctx) == -1) 
    {
        fprintf(stderr, "Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(pmotor->ctx);
        free(pmotor);
		return -1;
    }
	printf("connect success.\n");
	
	return 0;
}

//¿¿¿¿
int motor_close(motor_t * mt)
{
    if( mt!=NULL )
    {
        modbus_close(mt->ctx);
        modbus_free(mt->ctx);
        free(mt);

    }

    
    return 0;
}

int main()
{
	motor_init("/dev/ttyUSB0",1);	

	uint16_t res_format = get_format();
	printf("%0x\n", res_format);
//	set_operation_mode(1);
//	set_destination(1.1);
	run_motor();
	return 0;
}



