#include "systick.h"

volatile uint32_t SYSTick_ticks = 0;

void SYSTICK_start(uint32_t interval){
    SysTick->CTRL = 0x00000007;
    SysTick->LOAD = interval;
}

void SysTick_Handler(){
    SYSTick_ticks++;
    __disable_irq();
    OS_sched();
    __enable_irq();
}