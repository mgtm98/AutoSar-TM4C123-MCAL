#include "Port.h"
/****************************************************************************************************************************************/
/************************************************ GLOBAL VARIABLES **********************************************************************/
/****************************************************************************************************************************************/
// Container for the pins configs structs pointers. 
const Port_ConfigType* pinsConfigs [48] = {(Port_ConfigType*)0x00};

// Container to get the port GPIO struct by the port number
GPIO_Type*       PORT_ARR [] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF};

// Container to get the pin mask by pin number
const uint8_t    PIN_MASK [] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

// Array to detect each pin prephirals support.
uint8_t UART_Pins   [] = {0, 1, 8, 9, 20, 21, 22, 23, 28, 29, 30, 31, 32, 33, 36, 37};
uint8_t PWM_M0_Pins [] = {12, 13, 14, 15, 20, 21, 24, 25, 36, 37}; 
uint8_t PWM_M1_Pins [] = {6, 7, 24, 25, 36, 37, 40, 41, 42, 43}; 
/****************************************************************************************************************************************/
/****************************************************************************************************************************************/


/*
    NOTE: field pin Port_ConfigType represents the pin number not the pin MASK
    NOTE: to get the GPIO struct use __getPort
    NOTE: to get the Pin mask use __getPinMask
*/

/****************************************************************************************************************************************/
/**************************************************** HELPERS FUNCTIONS *****************************************************************/
/****************************************************************************************************************************************/
static __inline GPIO_Type *__getPort(Port_PinType pin){
    return PORT_ARR[pin/8];
}

static __inline uint8_t __getPinMask(Port_PinType pin){
    return PIN_MASK[pin%8];
}

// Initialize the port clock if it's not initialized before
void __initPortClock(const uint8_t portNumber){
    uint32_t mask = 1 << (portNumber/8);
    if (!(mask && SYSCTL->RCGCGPIO )){
        SYSCTL->RCGCGPIO |= mask;
        while(!(SYSCTL->PRGPIO & mask));
    }
}

// Init rotuine for the DIO mode
void __initPortDIOMode(const Port_ConfigType *ptr){
    GPIO_Type *port             = __getPort(ptr->pin);
    uint8_t mask                = __getPinMask(ptr->pin);
    port->DATA_Bits[mask]       = ptr->options & PORT_OPTIONS_DEFAULTVAL_MASK;
    port->DEN                   = port->DEN | mask;
    port->DIR                   = (port->DIR & ~mask) | (mask & ptr->dir);
    port->AMSEL                 = (port->AMSEL & ~mask);
    port->AFSEL                 = (port->AFSEL & ~mask);
}

// Check if the pin supports the mode in the config struct
uint8_t __checkPrephiral(uint8_t pinNumber, uint8_t *prepheralPins){
    for(uint8_t counter = 0; counter < sizeof(UART_Pins)/sizeof(UART_Pins[0]); counter++){
        if(pinNumber == prepheralPins[counter]) return 0xFF;
    }
    return 0x00;
}

// Init rotuine for the UART mode
void __initPortUARTMode(const Port_ConfigType *ptr){
    GPIO_Type *port             = __getPort(ptr->pin);
    uint8_t mask                = __getPinMask(ptr->pin);
    port->DEN                   = port->DEN | mask;
    port->AFSEL                 = port->AFSEL | mask;
    port->AMSEL                 = port->AMSEL & ~mask;
    
    uint32_t temp = port->PCTL & ~(0x0000000F << ((ptr->pin%8)*4));
    port->PCTL = temp | (1 << (ptr->pin%8)*4);
}

// Init rotuine for the PWM mode
void __initPortPWMMode(const Port_ConfigType *ptr, uint8_t func){
    GPIO_Type *port             = __getPort(ptr->pin);
    uint8_t mask                = __getPinMask(ptr->pin);
    port->DEN                   = port->DEN | mask;
    port->AFSEL                 = port->AFSEL | mask;
    port->AMSEL                 = port->AMSEL & ~mask;
    
    uint32_t temp = port->PCTL & ~(0x0000000F << ((ptr->pin%8)*4));
    port->PCTL = temp | (func << (ptr->pin%8)*4);
}

// Apply options to the port like Open drain and Slew rate
void __applyOptions(const Port_ConfigType *ptr){
    GPIO_Type *port             = __getPort(ptr->pin);
    uint8_t mask                = __getPinMask(ptr->pin);
    // 1- pullup and pulldown resistors
    if(ptr->options && PORT_OPTIONS_PULLSTATUS_MASK == PORT_PULLUP){
        port->PUR |= mask;
        port->PDR &= mask;
    }else if(ptr->options && PORT_OPTIONS_PULLSTATUS_MASK == PORT_PULLUP){
        port->PDR |= mask;
        port->PUR &= ~mask;
    }else if(ptr->options && PORT_OPTIONS_PULLSTATUS_MASK == PORT_FLOAT){
        port->PDR &= ~mask;
        port->PUR &= ~mask;
    }
    
    // 2- Drive Current
    if(ptr->options && PORT_OPTIONS_DRCURRENT_MASK == PORT_2ma){
        port->DR2R |=  mask;
        port->DR4R &= ~mask;
        port->DR8R &= ~mask;
    }else if(ptr->options && PORT_OPTIONS_PULLSTATUS_MASK == PORT_4ma){
        port->DR2R &= ~mask;
        port->DR4R |= mask;
        port->DR8R &= ~mask;
    }else if(ptr->options && PORT_OPTIONS_PULLSTATUS_MASK == PORT_8ma){
        port->DR2R &= ~mask;
        port->DR4R &= ~mask;
        port->DR8R |= mask;
    }else{
        port->DR2R &= ~mask;
        port->DR4R &= ~mask;
        port->DR8R &= ~mask;
    }
    
    // 3- Slew Rate
    if(ptr->options & PORT_OPTIONS_SLEWRATE_MASK){
        port->SLR |= mask;
    }else{
        port->SLR &= ~mask;
    }
    
     // 4- Open Drain
    if(ptr->mode == PORT_DIO_MODE){
        if(ptr->options & PORT_OPTIONS_OPENDRAIN_MASK){
            port->ODR |= mask;
        }else{
            port->ODR &= ~mask;
        }
    }

}

// Initialize the pin with its mode initializer
uint8_t __initMode(const Port_ConfigType *ConfigPtr, Port_PinModeType Mode){
    if(Mode == PORT_DIO_MODE){
        __initPortDIOMode(ConfigPtr);
    }else if(Mode == PORT_UART_MODE){
        if(!__checkPrephiral(ConfigPtr->pin, UART_Pins)) return 0;
        __initPortUARTMode(ConfigPtr);
    }else if(Mode == PORT_PWM_M0_MODE){
        if(!__checkPrephiral(ConfigPtr->pin, PWM_M0_Pins)) return 0;
        __initPortPWMMode(ConfigPtr, 4);
    }else if(Mode == PORT_PWM_M1_MODE){
        if(!__checkPrephiral(ConfigPtr->pin, PWM_M1_Pins)) return 0;
        __initPortPWMMode(ConfigPtr, 5);
    }
    return 1;
}
/****************************************************************************************************************************************/
/****************************************************************************************************************************************/


/****************************************************************************************************************************************/
/**************************************************** AUTOSAR APIs **********************************************************************/
/****************************************************************************************************************************************/
void Port_Init(const Port_ConfigType *ConfigPtr){    
    pinsConfigs[ConfigPtr->pin] = ConfigPtr;
    __initPortClock(ConfigPtr->pin);
    if(!__initMode(ConfigPtr, ConfigPtr->mode))return;
    __applyOptions(ConfigPtr);
}

void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction){
    const Port_ConfigType *config = pinsConfigs[Pin];
    if(config != (Port_ConfigType*)0x00 && config->options & PORT_OPTIONS_DIRCHANGE_MASK){
        uint8_t mask = __getPinMask(config->pin);
        GPIO_Type *port = __getPort(config->pin);
        port->DIR = (port->DIR & ~mask) | (mask & Direction);
    }        
}

void Port_RefreshPortDirection(void){
    for(int a = 0; a < 48; a++){
        const Port_ConfigType *config = pinsConfigs[a];
        if(config != (Port_ConfigType*)0x00 && !(config->options & PORT_OPTIONS_DIRCHANGE_MASK)){
            uint8_t mask = __getPinMask(config->pin);
            GPIO_Type *port = __getPort(config->pin);
            port->DIR = (port->DIR & ~mask) | (mask & config->dir);
        }
    }
}

void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode){
    const Port_ConfigType *config = pinsConfigs[Pin];
    if(config != (Port_ConfigType*)0x00 && config->options & PORT_OPTIONS_MODCHANGE_MASK){
        __initMode(config, Mode);
    }  
}
/****************************************************************************************************************************************/
/****************************************************************************************************************************************/
