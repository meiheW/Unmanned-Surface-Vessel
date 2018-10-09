#include     <stdio.h> 
#include     <stdlib.h>
#include     <string.h>
#include     <unistd.h>
#include     <sys/time.h>
#include     <sys/types.h>
#include     <sys/stat.h>  
#include     <fcntl.h>  
#include     <termios.h>
#include     <errno.h> 
#include     <stddef.h>
#include     <pthread.h> 
#include     <sys/ioctl.h> 
#include     <math.h>
#include     "kserialsport.h"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif
// 用于变量赋值的互斥
//pthread_mutex_t mutex_encoder = PTHREAD_MUTEX_INITIALIZER;

static int speed_arr[] = {B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200};
static int name_arr[] =  {115200, 57600, 38400,  19200,  9600,  4800,  2400, 1200};

#define debugnum(data,len,prefix)  \
{ \
        unsigned int i;   \
        for (i = 0;i < len;i++) { \
                if(prefix)  \
                        printf("0x%02x ",data[i]); \
                else  \
                        printf("%02x ",data[i]); \
        } \
}

static void set_speed(int fd, int speed)
{
    int   i;
    int   status;
    struct termios   Opt;
    tcgetattr(fd, &Opt);
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++){
    
       	if (speed == name_arr[i]){
       	
           	    tcflush(fd, TCIOFLUSH);
	            cfsetispeed(&Opt, speed_arr[i]);
	            cfsetospeed(&Opt, speed_arr[i]);
	            status = tcsetattr(fd, TCSANOW, &Opt);
	            
	            if (status != 0)
		            perror("tcsetattr fd1");
		            
	            return;
         	}
         	
	    tcflush(fd,TCIOFLUSH);
    }
}
static int set_Parity(int fd,int databits,int stopbits,int parity)
{
    struct termios options;
    if  ( tcgetattr( fd,&options)  !=  0)
    {
      	perror("SetupSerial 1");
      	return(FALSE);
    }
    options.c_cflag &= ~CSIZE;
    switch (databits)
    {
        case 7:
	    options.c_cflag |= CS7;
	    break;
        case 8:
	    options.c_cflag |= CS8;
	    break;
        default:
	    fprintf(stderr,"Unsupported data size\n");
	    return (FALSE);
    }
    switch (parity)
    {
        case 'n':
        case 'N':
	    options.c_cflag &= ~PARENB;   
	    options.c_iflag &= ~INPCK;   
	    break;
        case 'o':
        case 'O':
	    options.c_cflag |= (PARODD | PARENB); 
	    options.c_iflag |= INPCK;           
	    break;
        case 'e':
        case 'E':
	    options.c_cflag |= PARENB;     
	    options.c_cflag &= ~PARODD;
	    options.c_iflag |= INPCK;     
	    break;
        case 'S':
        case 's':  
	    options.c_cflag &= ~PARENB;
	    options.c_cflag &= ~CSTOPB;
	    break;
        default:
	    fprintf(stderr,"Unsupported parity\n");
	    return (FALSE);
    }

    switch (stopbits)
    {
        case 1:
	    options.c_cflag &= ~CSTOPB;
	    break;
        case 2:
	    options.c_cflag |= CSTOPB;
	    break;
        default:
	    fprintf(stderr,"Unsupported stop bits\n");
	    return (FALSE);
    }


	options.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
	options.c_oflag &= ~OPOST;
	options.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);

    /* Set input parity option */
    
    if (parity != 'n')
    options.c_iflag |= INPCK;
    options.c_cc[VTIME] = 150; // 15 seconds
    options.c_cc[VMIN] = 0;
    
    
    tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */

    if (tcsetattr(fd,TCSANOW,&options) != 0)
    {
	    perror("SetupSerial");
	    return (FALSE);
    }
    return (TRUE);
}

// 编码器监听线程
//*************************************************************************************************************************************
static void receivethread( void* pParam )
{
    int  nread;
    char buff[1024];

    uart_t *  puart = (uart_t *)pParam;
    
    struct timeval t_last, t_now;
    gettimeofday(&t_now,NULL);
    t_last = t_now;

    puart->loop = 1;

    printf("fd=%d\n",puart->fd);
    printf("com=%s\b",puart->device);
    
    // 监听
    while(puart->loop) 
    {
    
	    nread = read(puart->fd,buff,sizeof(buff)-1);
	    nread = (nread > 0) ? nread : 0;

	    
        if( nread > 0 ){

            printf("[RECEIVE LEN] is %d, content is:\n",nread);
	        for(int i = 0; i < nread; i++){

	        	fprintf(stderr,"%.2x ",buff[i]);
	        }

            if( puart->callback ){
               
                puart->callback(puart->id,buff,nread); 
            }
        }
    	// 帧间延迟
		usleep(100010);//10ms

	    

    } 
    return;
}

void uart_close(uart_t* uarthandle)
{
    if( uarthandle != NULL){
        
        uarthandle->loop=0;
        sleep(1);
        tcflush(uarthandle->fd,TCOFLUSH); 
        tcflush(uarthandle->fd,TCIFLUSH);
        close(uarthandle->fd);
    }
}

//
int uart_write( uart_t* uarthandle, uint8_t *buffer, int len )
{
    if( uarthandle !=0 ){

        int r = write(uarthandle->fd, buffer, len ); 
        return r;

    } 


}

uart_t *uart_open(const char *fname, 		     
	              uart_callback_t callback, 
	              int id, 
	              int baud,   
	              int bits,   
	              int parity,
	              int stop)  
{
    pthread_t receiveid;
    uart_t *  uarthandle = NULL;
    int fd = -1;
    
    printf("******%s*******\n",fname);

    if( callback ){
        fd = open(fname, O_RDWR|O_NDELAY); // 非阻塞
        if (fd < 0){        
	        return NULL;
        }
        fcntl(fd,F_SETFL,FNDELAY); // 非阻塞
        //fcntl(fd,F_SETSIG,CSSL_SIGNAL);
	    //fcntl(fd,F_SETOWN,getpid());
	    //fcntl(fd,F_SETFL,O_ASYNC|O_NONBLOCK);
    }
    else
    {
        fd = open(fname, O_RDWR);//O_NOCTTY
        if (fd < 0){        
	        return NULL;
        }
    }
    
    set_speed(fd,baud);
    set_Parity(fd,bits,stop,parity);

        
    
    uarthandle = malloc(sizeof(uart_t));
    
    uarthandle->fd=fd;
    uarthandle->callback=callback;
    uarthandle->id=id;
    strcpy( uarthandle->device,fname);
    
    printf("******fd:%d*******\n",uarthandle->fd);  
    printf("******fd:%d*******\n",baud);
    printf("******fd:%d*******\n",bits);
    printf("******fd:%d*******\n",stop);
    printf("******fd:%d*******\n",parity);  

    if( callback ){

        if(0 != pthread_create( &receiveid,NULL,
                    (void*)receivethread,uarthandle)){ 
            fprintf(stderr,"error when create thread\n");;
        }

        if(0 != pthread_detach( receiveid) ){ 
            fprintf(stderr,"error when detach thread\n");;
        }

    }

    return uarthandle;
}


