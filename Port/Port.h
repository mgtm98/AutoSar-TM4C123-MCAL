#include "include/tm4c123gh6pm.h"
#include "MCAL/sysctrl.h"

// Autosar Port Module:
// - used for initialize the pins for various modes
// - now supports initializers for DIO, PWM, UART

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

#define    PORT_A_PIN_0                 0
#define    PORT_A_PIN_1                 1
#define    PORT_A_PIN_2                 2
#define    PORT_A_PIN_3                 3
#define    PORT_A_PIN_4                 4
#define    PORT_A_PIN_5                 5
#define    PORT_A_PIN_6                 6
#define    PORT_A_PIN_7                 7

#define    PORT_B_PIN_0                 8
#define    PORT_B_PIN_1                 9
#define    PORT_B_PIN_2                 10
#define    PORT_B_PIN_3                 11
#define    PORT_B_PIN_4                 12
#define    PORT_B_PIN_5                 13
#define    PORT_B_PIN_6                 14
#define    PORT_B_PIN_7                 15

#define    PORT_C_PIN_0                 16
#define    PORT_C_PIN_1                 17
#define    PORT_C_PIN_2                 18
#define    PORT_C_PIN_3                 19
#define    PORT_C_PIN_4                 20
#define    PORT_C_PIN_5                 21
#define    PORT_C_PIN_6                 22
#define    PORT_C_PIN_7                 23

#define    PORT_D_PIN_0                 24
#define    PORT_D_PIN_1                 25
#define    PORT_D_PIN_2                 26
#define    PORT_D_PIN_3                 27
#define    PORT_D_PIN_4                 28
#define    PORT_D_PIN_5                 29
#define    PORT_D_PIN_6                 30
#define    PORT_D_PIN_7                 31

#define    PORT_E_PIN_0                 32
#define    PORT_E_PIN_1                 33
#define    PORT_E_PIN_2                 34
#define    PORT_E_PIN_3                 35
#define    PORT_E_PIN_4                 36
#define    PORT_E_PIN_5                 37

#define    PORT_F_PIN_0                 40
#define    PORT_F_PIN_1                 41
#define    PORT_F_PIN_2                 42
#define    PORT_F_PIN_3                 43
#define    PORT_F_PIN_4                 44

#define PORT_PIN0                       0x01
#define PORT_PIN1                       0x02
#define PORT_PIN2                       0x04
#define PORT_PIN3                       0x08
#define PORT_PIN4                       0x10
#define PORT_PIN5                       0x20
#define PORT_PIN6                       0x40
#define PORT_PIN7                       0x80
#define PORT_ALLPINS                    0xFF

#define PORT_DIO_MODE                   0x00
#define PORT_UART_MODE                  0x01
#define PORT_PWM_M0_MODE                0x02
#define PORT_PWM_M1_MODE                0x03


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

// Defines pin number
typedef uint8_t Port_PinType;

// Defines whether the pin is ADC, DIO, SPI
typedef uint8_t Port_PinModeType;

// Options field consisits of 16 bit fields each on responsible for a specific option in the Pin
//                -----------------------------------------
//                | 15 | 14 | 13-12 | 11-10 | 9 | 8 | 7-0 |
//                -----------------------------------------
// BIT 0-7:   Default value of the pin     (0x00 means LOW, 0xFF means HIGH)
// BIT 8:     Direction Changeable         (0 means Not Changeable, 1 means Changeable)
// BIT 9:     Mode Changeable              (0 means Not Changeable, 1 means Changeable)
// BIT 10-11: Specifies the pull status   
// BIT 12-13: Specifies the drive current 
// BIT 14:    Slew Rate                    (0 means not activated, 1 means activated)
// BIT 15:    Open drain                   (0 means not activated, 1 means activated)
//
// NOTE: Slew Rate can't be activated unless the drive current is PORT_8ma
typedef uint16_t Port_OptionsType;


typedef struct{
    Port_OptionsType        options;        // 16bit field containes options various options
    Port_PinType            pin;            // 8bit field represents the pin number (can represent more than one pin)
    Port_PinModeType        mode;           // defines the pin to be either DIO, ADC, PWM, etc.
    Port_PinDirectionType   dir;            // defines the pin direction either PORT_PIN_IN or PORT_PIN_OUT
} Port_ConfigType;


void Port_Init(const Port_ConfigType *ConfigPtr);
void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction);
void Port_RefreshPortDirection(void);
void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode);
