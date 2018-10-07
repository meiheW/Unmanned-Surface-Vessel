#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "powerManager.h"

modbus_t *ctx = NULL;

//open modbus tcp --ip:"192.168.1.253", port:1031
int open_modbus_tcp_powermang(const char *ip_address, int port)
{
	//ctx = modbus_new_tcp("192.168.1.253", 1031);
	ctx = modbus_new_tcp(ip_address, port);
    modbus_set_debug(ctx, TRUE);

    if (modbus_connect(ctx) == -1) 
    {
        fprintf(stderr, "Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }
	return 0;
}

//close modbus tcp
void close_modbus_tcp_powermang()
{
	modbus_close(ctx);
	modbus_free(ctx);
}

//open power index_m coil index_n.
int open_powermang(int mang_index, int coil_index)
{
	int status = 1;
	int ret = 0;
	int rc = 0;
	//1:1CNRC8;  2:1CX0C4; 
	rc = modbus_set_slave(ctx, mang_index);
	if(-1 == rc){
		modbus_free(ctx);
		return -1;
	}
	ret = write_single_coil(ctx, coil_index, status);

	return ret;
}

//close power index_m coil index_n. 
int close_powermang(int mang_index, int coil_index)
{
	int status = 0;
	int ret = 0;
	int rc = 0;
	//1:1CNRC8; 2:1CX0C4; 
	rc = modbus_set_slave(ctx, mang_index);
	if(-1 == rc){
		modbus_free(ctx);
		return -1;
	}
	ret = write_single_coil(ctx, coil_index, status);
	
	return ret;
}

//read status power index_m coil index_n. 
int read_powermang(int mang_index, int coil_index)
{
	int ret;
	int rc = 0;
	rc = modbus_set_slave(ctx, mang_index);
	if(-1 == rc){
		modbus_free(ctx);
		return -1;
	}
	ret = read_coil_status(ctx, coil_index);

	return ret;
}

//read or write coil status
int read_coil_status(modbus_t *ctx, int coil_index)
{
	if(coil_index < 1 || coil_index > 8)
	{
		printf("read coil index wrong.\n");
		return -1;
	}
	int addr_h = 0x00;
	int addr_l = coil_index-1;
	int addr = (addr_h << 8) + addr_l;

	uint8_t tab_rp_bits;
	if(1 != modbus_read_bits(ctx, addr, 1, &tab_rp_bits))
	{
		printf("Modbus read bits failed.\n");
		return -1;
	}
	int ret = tab_rp_bits;
	//printf("index %d, ret: %d\n", coil_index, ret);
	//printf("read bits: %0x", tab_rp_bits);
	//printf("read 0 : %02x\n", tab_rp_bits[0]);
	//printf("read 1 : %02x\n", tab_rp_bits[1]);
	//int all_coil_status = tab_rp_bits[1];
	//int index = 1 << (coil_index - 1);
	//int coil_status = all_coil_status & index;	//
	//int ret = (coil_status != 0) ? 1 : 0;

	return ret;
}

int write_single_coil(modbus_t *ctx, int coil_index, int status)
{
	if(coil_index < 1 || coil_index > 8)
	{
		printf("write coil index wrong.\n");
		return -1;
	}

	int addr_h = 0x00;
	int addr_l  = coil_index-1; //Y4-->0x03
	int addr = (addr_h << 8) + addr_l;
	
	printf("write addr: %x\n", addr);

	int status_h = (1 == status) ? 0xFF : 0x00;	//0xFF:open
	int status_l = 0x00;
	int coil_status = (status_h << 8) + status_l;
	
	if(1 != modbus_write_bit(ctx, addr, coil_status))
	{
		printf("modbus write 4 bits failed");
		return -1;
	}

	return 0;
}



//attention:only used when set a new powermang
void set_addr(int zqwl_addr)  //zqwl_addr such as: 0x0001
{
	if(zqwl_addr < 0x00 || zqwl_addr > 0xFF)
	{
		printf("zqwl addr num wrong.\n");
		return;
	}
	int addr = 0x0000;

	if(1 != modbus_write_register(ctx, addr,zqwl_addr))
	{
		printf("modbus write register failed.\n");
		return;
	}
}
//get baud or addr of a powermang etc.
int read_baud(int mang_index)
{
	int rc = 0;
	rc = modbus_set_slave(ctx, mang_index);
	if(-1 == rc){
		modbus_free(ctx);
		return -1;
	}
	int addr = 0x0001;
	uint16_t rp;
	modbus_read_registers(ctx, addr, 1, &rp);
	int ret = rp;
	//printf("baud: %d\n", ret);
	return ret;
}

int get_addr(int mang_index)
{
	int rc = 0;
	rc = modbus_set_slave(ctx, mang_index);
	if(-1 == rc){
		modbus_free(ctx);
		return -1;
	}
	int addr = 0x0000;
	uint16_t rp;
	modbus_read_registers(ctx, addr, 1, &rp);
	int ret = rp;
	return ret;
}

void closeall()
{
	for(int i=1;i<=4; i++)
	{
		close_powermang(1,i);
		usleep(50000);
	}
	for(int i=2; i<=4; i++)
	{
		for(int j=1; j<=8; j++)
		{
			close_powermang(i,j);
			usleep(50000);
		}
	}
	printf("all closed\n");
}

void openall()
{
	for(int i=1;i<=4; i++)
	{
		open_powermang(1,i);
		usleep(500000);
	}
	for(int i=2; i<=4; i++)
	{
		for(int j=1; j<=8; j++)
		{
			open_powermang(i,j);
			usleep(500000);
		}
	}
	printf("all open...\n");
}

void cruise()
{
	for(int i=1; i<=4; i++)
	{
		open_powermang(1,i);
		sleep(2);
		close_powermang(1,i);
		sleep(2);
	}

	for(int i=2; i<=4; i++)
	{
		for(int j=1; j<=8; j++)
		{
			open_powermang(i,j);
			sleep(2);
			close_powermang(i,j);
			sleep(2);
		}
	}

}

int main(int arg, char** argv)
{
    //ctx = modbus_new_tcp("192.168.1.253", 1030);//banzi,no transporter
	open_modbus_tcp_powermang("192.168.1.253", 1031);
	

	//closeall();	
	
	char* para1 = argv[1]; 	//open 1/ close 0
	char* para2 = argv[2];	//box i
	char* para3 = argv[3];	//index j
	

	if(!strcmp(para1,"cru"))
	{
		cruise();
		return 0;
	}

	if(!strcmp(para1, "close")){
		closeall();
		return 0;
	}
	
	if(!strcmp(para1, "open"))
	{
		openall();
		return 0;
	}
	int p1 = atoi(para1);
	int p2 = atoi(para2);
	int p3 = atoi(para3);	
	if(p2==1 && p3>4)
	{	
		printf("box1 index out of boundary.\n");
		return 0;
	}
	if( p2>4 ||(p2>1 && p3>8) ) 	
	{
		printf("box2~4 index out of boundary.\n");
		return 0;
	}
	printf("p1=%d, p2=%d, p3=%d\n", p1, p2, p3);
	if(p1==1)
		open_powermang(p2, p3);
	if(p1==0)
		close_powermang(p2, p3);
	//close_powermang(2,1);
	
//	if(!strcmp(para1,"2") && !strcmp(para2,"2") )
//		open_powermang(2,2);
	//int baud = read_baud(1) * 100;
	//int addr = get_addr(1);
	//printf("baud rate = %d\n", baud);
	//printf("addr = %d\n", addr);
	
	close_modbus_tcp_powermang();
    printf("exit\n");
    return 0;
}
