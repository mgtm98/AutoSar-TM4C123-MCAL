#ifndef _CAMMAN_H
#define _CAMMAN_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "TM4C123GH6PM.h"

#define HIGH		0xFFFFFFFF
#define LOW 		0x00000000

#define CPU_CLK     50000000U


#define NVIC_EN0 (*((volatile uint32_t *)0xE000E100))
#define NVIC_EN1 (*((volatile uint32_t *)0xE000E104))
#define NVIC_EN2 (*((volatile uint32_t *)0xE000E108))
#define NVIC_EN3 (*((volatile uint32_t *)0xE000E10C))

#define Log(X) ((int)log2(X))

#endif //_CAMMAN_H

