#include "msp.h"
#include "BoardSupportPackage\inc\Clock.h" /* Make Sure Correct Path To Where You Include */

/* Initialize SysTick With Busy Wait Running At Bus Clock */
void SysTick_Init(void)
{
    SysTick->LOAD = 0x00FFFFFF;     /* Max Reload Value */
    SysTick->CTRL = 0x00000005;     /* Enable SysTick With No Interrupts */
}

/* Time Delay Using Busy Wait With Delay Parameter In Terms Of Core Clock Assuming 48Mhz */
void SysTick_Wait(uint32_t delay)
{
    if(delay <= 1){return;}
    SysTick->LOAD = (delay - 1);    /* Count Down To Zero */
    SysTick->VAL = 0;               /* Any Write To CVR Clears It And COUNTFLAG In CSR */
    while((SysTick->CTRL & 0x00010000) == 0){};
}

/* Time Delay Using Busy Wait For 48 MHz */
void SysTick_Wait1us(uint32_t delay)
{
    /* Wait i*48ms */
    for(uint32_t i = 0; i < delay; i++){SysTick_Wait(48);}
}

/*
 * Red Servo Wire    : 5V
 * Black Servo Wire  : GND
 * Yellow Servo Wire : GPIO 6.7
 */
int main(void)
{
    /* Initialize Clock And Timer */
    Clock_Init48MHz();
    SysTick_Init();

    /* 6.7 as PWM */
    P6->SEL0 &= ~0x80;
    P6->SEL1 &= ~0x80;
    P6->DIR  |=  0x80;
    P6->OUT  &= ~0x80;

    /* 7741 = 20ms
     * 388  = 1ms
     * 775  = 2ms
     */
    uint32_t start = 388;
    uint32_t end = 775;
    while(1)
    {
        for (uint32_t duty = start; duty <= end; duty++)
        {
            /* PWM High */
            P6->OUT |= 0x80;

            /* High Cycle (1 - 2 ms) */
            SysTick_Wait1us(duty);

            /* PWM Low */
            P6->OUT &= ~0x80;

            /* Low Cycle (19 - 18 ms) */
            SysTick_Wait1us(7741 - duty);
        }

        for (uint32_t duty = end; duty >= start; duty--)
        {
            /* PWM High */
            P6->OUT |= 0x80;

            /* High Cycle (2 - 1 ms) */
            SysTick_Wait1us(duty);

            /* PWM Low */
            P6->OUT &= ~0x80;

            /* Low Cycle (18 - 19 ms) */
            SysTick_Wait1us(7741 - duty);
        }
    }
}