#ifndef _KSERIALS_PORT_H_
#define _KSERIALS_PORT_H_

#include <stddef.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*uart_callback_t)( int id,          /* id passed to callback */
			                     uint8_t *buffer, /* data received */
			                     int len);        /* length of data in bytes */

struct  uart_t;
typedef struct uart_t uart_t;

struct uart_t 
{
    int  fd;
    int  id;
    int  loop;   
    char device[32];
    uart_callback_t callback;
                     
};


uart_t *uart_open(const char *fname, // pathname of port file,				     
	              uart_callback_t callback, //callback function
	              int id, 
	              int baud,   // baudrate, integer, for example 19200 */
	              int bits,   // data bits: 7 or 8 */
	              int parity, //parity: 0 - none, 1-odd, 2-even */
	              int stop);  // stop bits: 1 or 2 */



int  uart_write( uart_t* uarthandle, uint8_t *buffer, int len );
void uart_close( uart_t* uarthandle );

#ifdef __cplusplus
}
#endif

#endif	//
