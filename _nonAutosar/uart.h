#ifndef _UART_H
#define _UART_H

#include "../include/common.h"
#include "gpio.h"
#include "sysctrl.h"
#include <string.h>
#include <stdlib.h>

#define CLK_SPEED				 	 50000000
#define UART_BUFFER_SIZE 	 64
#define BAUD_IDIV(B)		 	 (int)(CLK_SPEED/(16*B))
#define BAUD_FDIV(B) 		 	 (int)(((CLK_SPEED/(16*B)) - BAUD_IDIV(B))*64)
#define UART_FUNC					 0x1

typedef enum UART_BAUDRATE{
	UART_BAUD_300 			  				= 300,
	UART_BAUD_1200 		  					= 1200,
	UART_BAUD_2400 			  				= 2400,
	UART_BAUD_4800 			  				= 4800,
	UART_BAUD_9600 			  				= 9600,
	UART_BAUD_19200 						= 19200,
	UART_BAUD_38400 						= 38400,
	UART_BAUD_57600 						= 57600,
	UART_BAUD_74880							= 74880,
	UART_BAUD_115200						= 115200,
} UART_BAUDRATE;

typedef struct{
		GPIO_Type 		*Port;
		Pin 	    	RX_Pin;
		Pin 	    	TX_Pin;
	 GPIO_SYSCTRL 		pinControl;
} __UART_PIN_CONFIG;

#define __UART0_PIN_CONFIG   &((__UART_PIN_CONFIG){GPIOA, PIN0, PIN1, SYSCTRL_GPIOA})
#define __UART1_PIN_CONFIG   &((__UART_PIN_CONFIG){GPIOB, PIN0, PIN1, SYSCTRL_GPIOB})
#define __UART2_PIN_CONFIG   &((__UART_PIN_CONFIG){GPIOD, PIN6, PIN7, SYSCTRL_GPIOD})
#define __UART3_PIN_CONFIG   &((__UART_PIN_CONFIG){GPIOC, PIN6, PIN7, SYSCTRL_GPIOC})
#define __UART4_PIN_CONFIG   &((__UART_PIN_CONFIG){GPIOC, PIN4, PIN5, SYSCTRL_GPIOC})
#define __UART5_PIN_CONFIG   &((__UART_PIN_CONFIG){GPIOE, PIN4, PIN5, SYSCTRL_GPIOE})
#define __UART6_PIN_CONFIG   &((__UART_PIN_CONFIG){GPIOD, PIN4, PIN5, SYSCTRL_GPIOD})
#define __UART7_PIN_CONFIG   &((__UART_PIN_CONFIG){GPIOE, PIN0, PIN1, SYSCTRL_GPIOE})

void UART_Init(UART_Type *uart, UART_BAUDRATE baudrate);
uint8_t UART_readChar(UART_Type *uart);
bool UART_available(UART_Type *uart);
void UART_sendChar(UART_Type *uart, const char data);
void UART_sendString(UART_Type *uart, const char *data);
void UART_attachBuffer(UART_Type *uart);
void UART_readString(char *dest);
bool UART_bufferAvailable(void);
#endif
