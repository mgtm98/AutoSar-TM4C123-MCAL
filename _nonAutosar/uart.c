#include "uart.h"

char buffer[UART_BUFFER_SIZE];
volatile uint8_t buffer_index = 0;
volatile UART_Type *uartAttachedToBuffer;

void __UART_InitPin(__UART_PIN_CONFIG *uart){
		SYSCTRL_initGPIO(uart->pinControl);
		GPIO_initPin(uart->Port, uart->RX_Pin, GPIO_CONF_PREPH);
		GPIO_initPin(uart->Port, uart->TX_Pin, GPIO_CONF_PREPH);
		GPIO_setPCTL(uart->Port, uart->RX_Pin, UART_FUNC);
		GPIO_setPCTL(uart->Port, uart->TX_Pin, UART_FUNC);
}

void UART_Init(UART_Type *uart, UART_BAUDRATE baudrate){
    if(uart == UART0) 		 __UART_InitPin(__UART0_PIN_CONFIG);
    else if(uart == UART1) __UART_InitPin(__UART1_PIN_CONFIG);
    else if(uart == UART2) __UART_InitPin(__UART2_PIN_CONFIG);
    else if(uart == UART3) __UART_InitPin(__UART3_PIN_CONFIG);
    else if(uart == UART4) __UART_InitPin(__UART4_PIN_CONFIG);
    else if(uart == UART5) __UART_InitPin(__UART5_PIN_CONFIG);
    else if(uart == UART6) __UART_InitPin(__UART6_PIN_CONFIG);
    else if(uart == UART7) __UART_InitPin(__UART7_PIN_CONFIG);
    uart->CTL  &= ~0x00000001;						// Disable UART Module
    uart->IBRD  = BAUD_IDIV(baudrate);			    // writing integer baudrate divisor
    uart->FBRD  = BAUD_FDIV(baudrate);			    // Fractional integer baudrate divisor
    uart->CC   &= ~0x0000000F; 						// clock source is system clock
    uart->LCRH  = 0x00000060;  						// 8 bit, no parity bits, one stop, no FIFOs
    uart->CTL  |= 0x00000001;						// enable UART
}

uint8_t UART_readChar(UART_Type *uart){
  while((uart->FR & 0x0010) != 0);     // wait until RXFE is 0 (reciver is empty)
  return (uint8_t)uart->DR;
}

bool UART_available(UART_Type *uart){
		return (uart->FR & 0x0010) == 0;
}

void UART_sendChar(UART_Type *uart, const char data){
		while((uart->FR&0x0020) != 0);			// wait until TXFF is 0 (transmitter is  full)
		uart->DR = data;
}

void UART_sendString(UART_Type *uart, const char *data){
	uint8_t j = 0;
	while(data[j] != '\0'){
		UART_sendChar(uart,data[j]);
		j++;
	}
	UART_sendChar(uart,'\0');
}


void UART_attachBuffer(UART_Type *uart){
    uartAttachedToBuffer = uart;
    if(uart == UART0) NVIC_EnableIRQ(UART0_IRQn);			    //enable interrupts for UART0
    else if(uart == UART1) NVIC_EnableIRQ(UART1_IRQn);			//enable interrupts for UART1
    else if(uart == UART2) NVIC_EnableIRQ(UART2_IRQn);			//enable interrupts for UART2 
    else if(uart == UART3) NVIC_EnableIRQ(UART3_IRQn);			//enable interrupts for UART3 
    else if(uart == UART4) NVIC_EnableIRQ(UART4_IRQn);			//enable interrupts for UART4 
    else if(uart == UART5) NVIC_EnableIRQ(UART5_IRQn);			//enable interrupts for UART5 
    else if(uart == UART6) NVIC_EnableIRQ(UART6_IRQn);			//enable interrupts for UART6 
    else if(uart == UART7) NVIC_EnableIRQ(UART7_IRQn);			//enable interrupts for UART7 
  	uart->IFLS	&= 0xFFFFFF00;
    uart->IM	  |= 0x00000010;
}


void UART_readString(char *dest){
    strncpy(dest,buffer,buffer_index);
dest[buffer_index] = '\0';
    buffer_index = 0;
}

bool UART_bufferAvailable(){
    if (buffer_index > 0) return true;
    else return false;
}

void __UART_interruptHandler(UART_Type *uart){
	buffer[buffer_index] = UART_readChar(uart);
	buffer_index++;
}

void UART0_Handler(){
	if(uartAttachedToBuffer == UART0) __UART_interruptHandler(UART0);
}

void UART1_Handler(){
	if(uartAttachedToBuffer == UART1) __UART_interruptHandler(UART1);
}

void UART2_Handler(){
	if(uartAttachedToBuffer == UART2) __UART_interruptHandler(UART2);
}

void UART3_Handler(){
	if(uartAttachedToBuffer == UART3) __UART_interruptHandler(UART3);
}

void UART4_Handler(){
	if(uartAttachedToBuffer == UART4) __UART_interruptHandler(UART4);
}

void UART5_Handler(){
	if(uartAttachedToBuffer == UART5) __UART_interruptHandler(UART5);
}

void UART6_Handler(){
	if(uartAttachedToBuffer == UART6) __UART_interruptHandler(UART6);
}

void UART7_Handler(){
	if(uartAttachedToBuffer == UART7) __UART_interruptHandler(UART7);
}
