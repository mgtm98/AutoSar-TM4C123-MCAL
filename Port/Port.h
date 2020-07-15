#include "include/tm4c123gh6pm.h"
#include "MCAL/sysctrl.h"
    
#define    LOG(X)                      (31 - __clz(X))

#define    STD_ON                       0x00
#define    STD_OFF                      0xff

#define    PORT_OPTIONS_DEFAULTVAL_MASK  0b0000000011111111
#define    PORT_OPTIONS_DIRCHANGE_MASK   0b0000000100000000
#define    PORT_OPTIONS_MODCHANGE_MASK   0b0000001000000000
#define    PORT_OPTIONS_PULLSTATUS_MASK  0b0000110000000000
#define    PORT_OPTIONS_DRCURRENT_MASK   0b0011000000000000
#define    PORT_OPTIONS_SLEWRATE_MASK    0b0100000000000000
#define    PORT_OPTIONS_OPENDRAIN_MASK   0b1000000000000000

#define PORT_PIN0                       0x01
#define PORT_PIN1                       0x02
#define PORT_PIN2                       0x04
#define PORT_PIN3                       0x08
#define PORT_PIN4                       0x10
#define PORT_PIN5                       0x20
#define PORT_PIN6                       0x40
#define PORT_PIN7                       0x80
#define PORT_ALLPINS                    0xFF

#define PORT_DIOMODE                    0x00
#define PORT_UARTMODE                   0x01
#define PORT_PWMMODE                    0x02


typedef enum {
    PORT_PIN_IN        = 0x00,
    PORT_PIN_OUT       = 0xFF,
} Port_PinDirectionType;

// Pad Resistor
typedef enum {
    PORT_PULLUP     = 0b0000100000000000,
    PORT_PULLDOWN   = 0b0000010000000000,
    PORT_FLOAT      = 0b0000000000000000,
} Port_PullType;

// Pad Drive Current
typedef enum {
    PORT_2ma        = 0b0011000000000000,
    PORT_4ma        = 0b0010000000000000,
    PORT_8ma        = 0b0001000000000000,
} Port_DriveCurrentType;

// Defines whether the pin is ADC, DIO, SPI
typedef uint8_t Port_PinType;

// Pin MASK to  access a specific pin in the port
typedef uint8_t Port_PinModeType;

// Options field consisits of 16 bit fields each on responsible for a specific option in the Pin
//                -----------------------------------------
//                | 15 | 14 | 13-12 | 11-10 | 9 | 8 | 7-0 |
//                -----------------------------------------
// BIT 0-7:   Default value of the pin    (0x00 means LOW, 0xFF means HIGH)
// BIT 8:   Direction Changeable          (0 means Not Changeable, 1 means Changeable)
// BIT 9:   Mode Changeable               (0 means Not Changeable, 1 means Changeable)
// BIT 10-11: Specifies the pull status   
// BIT 12-13: Specifies the drive current 
// BIT 14:   Slew Rate                    (0 means not activated, 1 means activated)
// BIT 15:   Open drain                   (0 means not activated, 1 means activated)
//
// NOTE: Slew Rate can't be activated unless the drive current is PORT_8ma
typedef uint16_t Port_OptionsType;


typedef struct{
    GPIO_Type              *port;           // Pointer to GPIO struct in memory CMSIS style
    Port_OptionsType        options;        // 16bit field containes options various options
    Port_PinType            pin;            // 8bit field represents the pin number (can represent more than one pin)
    Port_PinModeType        mode;           // defines the pin to be either DIO, ADC, PWM, etc.
    Port_PinDirectionType   dir;            // defines the pin direction either PORT_PIN_IN or PORT_PIN_OUT
} Port_ConfigType;


void Port_Init(const Port_ConfigType *ConfigPtr);
void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction);
void Port_RefreshPortDirection(void);
void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode);