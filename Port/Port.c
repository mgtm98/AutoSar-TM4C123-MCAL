#include "Port.h"

uint8_t __PortClock = 0x00;                  // used to check whether the module have a clock or not
const Port_ConfigType* pinsConfigs [48];

uint8_t UART_Pins [16] = {0, 1, 8, 9, 20, 21, 22, 23, 28, 29, 30, 31, 32, 33, 36, 37};

uint8_t __getPortNumber(const GPIO_Type *port){
    if(port == GPIOA) return 0;
    else if(port == GPIOB) return 1;
    else if(port == GPIOC) return 2;
    else if(port == GPIOD) return 3;
    else if(port == GPIOE) return 4;
    else if(port == GPIOF) return 5;
}


void __initPortClock(const uint8_t portNumber){
    uint8_t mask = 1 << portNumber;
    if (!(mask && __PortClock)){
        __PortClock &= mask;
        SYSCTL->RCGCGPIO |= mask;
        while(!(SYSCTL->PRGPIO & mask));
    }
}


void __initPortDIOMode(const Port_ConfigType *ptr){
    ptr->port->DATA_Bits[ptr->pin]   = ptr->options & PORT_OPTIONS_DEFAULTVAL_MASK;
    ptr->port->DEN                   = (ptr->port->DEN & ~(ptr->pin)) | (ptr->pin & 0xFF);
    ptr->port->DIR                   = (ptr->port->DIR & ~(ptr->pin)) | (ptr->pin & ptr->dir);
    ptr->port->AMSEL                 = (ptr->port->AMSEL & ~(ptr->pin));
    ptr->port->AFSEL                 = (ptr->port->AFSEL & ~(ptr->pin));
}


uint8_t __checkPrephiral(uint8_t pinNumber, uint8_t *prepheralPins){
    for(uint8_t counter = 0; counter < sizeof(UART_Pins)/sizeof(UART_Pins[0]); counter++){
        if(pinNumber == prepheralPins[counter]) return 0xFF;
    }
    return 0x00;
}


void __initPortUARTMode(const Port_ConfigType *ptr){
    ptr->port->DEN                   = (ptr->port->DEN & ~(ptr->pin)) | (ptr->pin & 0xFF);
    ptr->port->DIR                   = (ptr->port->DIR & ~(ptr->pin));
    ptr->port->AMSEL                 = (ptr->port->AMSEL & ~(ptr->pin));
    ptr->port->AFSEL                 = (ptr->port->AFSEL & ~(ptr->pin)) | (ptr->pin & 0xFF);
    
    uint32_t temp = ptr->port->PCTL & ~(0x0000000F << (LOG(ptr->pin)*4));
    ptr->port->PCTL = temp | (1 << (LOG(ptr->pin)*4));
}

void __applyOptions(const Port_ConfigType *ptr){
    // 1- pullup and pulldown resistors
    if(ptr->options && PORT_OPTIONS_PULLSTATUS_MASK == PORT_PULLUP){
        ptr->port->PUR |= ptr->pin;
        ptr->port->PDR &= ~(ptr->pin);
    }else if(ptr->options && PORT_OPTIONS_PULLSTATUS_MASK == PORT_PULLUP){
        ptr->port->PDR |= ptr->pin;
        ptr->port->PUR &= ~(ptr->pin);
    }else if(ptr->options && PORT_OPTIONS_PULLSTATUS_MASK == PORT_FLOAT){
        ptr->port->PDR &= ~(ptr->pin);
        ptr->port->PUR &= ~(ptr->pin);
    }
    
    // 2- Drive Current
    if(ptr->options && PORT_OPTIONS_DRCURRENT_MASK == PORT_2ma){
        ptr->port->DR2R |= ptr->pin;
        ptr->port->DR4R &= ~(ptr->pin);
        ptr->port->DR8R &= ~(ptr->pin);
    }else if(ptr->options && PORT_OPTIONS_PULLSTATUS_MASK == PORT_4ma){
        ptr->port->DR2R |= ~(ptr->pin);
        ptr->port->DR4R &= ptr->pin;
        ptr->port->DR8R &= ~(ptr->pin);
    }else if(ptr->options && PORT_OPTIONS_PULLSTATUS_MASK == PORT_8ma){
        ptr->port->DR2R &= ~(ptr->pin);
        ptr->port->DR4R &= ~(ptr->pin);
        ptr->port->DR8R |= ptr->pin;
    }else{
        ptr->port->DR2R &= ~(ptr->pin);
        ptr->port->DR4R &= ~(ptr->pin);
        ptr->port->DR8R &= ~(ptr->pin);
    }
    
    // 3- Slew Rate
    if(ptr->options & PORT_OPTIONS_SLEWRATE_MASK){
        ptr->port->SLR |= ptr->pin;
    }else{
        ptr->port->SLR |= ~(ptr->pin);
    }
    
    // 4- Open Drain
    if(ptr->options & PORT_OPTIONS_OPENDRAIN_MASK){
        ptr->port->ODR |= ptr->pin;
    }else{
        ptr->port->ODR |= ~(ptr->pin);
    }
}


void Port_Init(const Port_ConfigType *ConfigPtr){
    uint8_t portNumber = __getPortNumber(ConfigPtr->port);
    uint8_t pinNumber = portNumber*8 + LOG(ConfigPtr->pin);
    pinsConfigs[pinNumber] = ConfigPtr;
    __initPortClock(portNumber);
    if(ConfigPtr->mode == PORT_DIOMODE){
        __initPortDIOMode(ConfigPtr);
    }else if(ConfigPtr->mode == PORT_UARTMODE){
        if(!__checkPrephiral(pinNumber, UART_Pins)) return;
        __initPortUARTMode(ConfigPtr);
    }else if(ConfigPtr->mode == PORT_PWMMODE){
//        __checkPrephiral(pinNumber, UART_Pins);
//        __initPortUARTMode(ConfigPtr);
    }
    __applyOptions(ConfigPtr);
}    