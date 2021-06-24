#include <stdint.h>
#include "msp.h"

uint32_t ClockFrequency = 3000000; /* Cycles / Second */
uint32_t Prewait = 0;              /* Loops Between BSP_Clock_InitFastest() Called And PCM Idle (Expect 0) */
uint32_t CPMwait = 0;              /* Loops Between Power Active Mode Request And Current Power Mode Matching Requested Mode (Expect Small) */
uint32_t Postwait = 0;             /* Loops Between Current Power Mode Matching Requested Mode And PCM Module Idle (Expect About 0) */
uint32_t IFlags = 0;               /* Non-Zero If Transition Is Invalid */
uint32_t Crystalstable = 0;        /* Loops Before The Crystal Stabilizes (Expect Small) */
void Clock_Init48MHz(void){
    /* Wait For The PCMCTL0 And Clock System To Be Writeable By Waiting For Power Control Manager To Be idle */
    while(PCM->CTL1 & 0x00000100)
    {
        Prewait = Prewait + 1;
        if(Prewait >= 100000){return;} /* Time Out Error */
    }
    /* Request Power Active Mode LDO VCORE1 To Support The 48 MHz Frequency */
    PCM->CTL0 = (PCM->CTL0 & ~0xFFFF000F) |     /* Clear PCMKEY Bit Field And AMR Bit Field */
                               0x695A0000 |     /* Write The Proper PCM Key To Unlock Write Access */
                               0x00000001;      /* Request Power Active Mode LDO VCORE1 */
    /* Check If The Transition Is Invalid (See Figure 7-3 On p344 Of Datasheet) */
    if(PCM->IFG & 0x00000004)
    {
        IFlags = PCM->IFG;                    /* Bit 2 Set On Active Mode Transition Invalid; Bits 1-0 Are For LPM Related Errors; Bit 6 Is For DC-DC Related Error */
        PCM->CLRIFG = 0x00000004;             /* Clear The Transition Invalid Flag */
        return;
    }
    /* Wait For The CPM (Current Power Mode) Bit Field To Reflect A Change To Active Mode LDO VCORE1 */
    while((PCM->CTL0 & 0x00003F00) != 0x00000100){
        CPMwait = CPMwait + 1;
        if(CPMwait >= 500000){return;} /* Time Out Error */
    }
    /* Wait For The PCMCTL0 And Clock System To Be Writeable By Waiting For Power Control Manager To Be Idle */
    while(PCM->CTL1 & 0x00000100)
    {
        Postwait = Postwait + 1;
        if(Postwait >= 100000){return;} /* Time Out Error */
    }
    /* Initialize PJ.3 And PJ.2 And Make Them HFXT (PJ.3 Built In 48 MHz Crystal Out; PJ.2 Built In 48 MHz Crystal In) */
    PJ->SEL0 |= 0x0C;
    PJ->SEL1 &= ~0x0C;                      /* Configure Built In 48 MHz Crystal For HFXT Operation */
    CS->KEY = 0x695A;                       /* Unlock CS Module For Register Access */
    CS->CTL2 = (CS->CTL2 & ~0x00700000) |   /* Clear HFXTFREQ Bit Field */
                             0x00600000 |   /* Configure For 48 MHz External Crystal */
                             0x00010000 |   /* HFXT Oscillator Drive Selection For Crystals >4 MHz */
                             0x01000000;    /* Enable HFXT */
    CS->CTL2 &= ~0x02000000;                /* Disable High-Frequency Crystal Bypass */
    /* Wait For The HFXT Clock To Stabilize */
    while(CS->IFG & 0x00000002)
    {
        CS->CLRIFG = 0x00000002;            /* Clear The HFXT Oscillator Interrupt Flag */
        Crystalstable = Crystalstable + 1;
        if(Crystalstable > 100000){return;} /* Time Out Error */
    }
    /* Configure For 2 Wait States (Minimum For 48 MHz Operation) For Flash Bank 0 */
    FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL&~0x0000F000)|FLCTL_BANK0_RDCTL_WAIT_2;
    /* Configure For 2 Wait States (Minimum For 48 MHz Operation) For Flash Bank 1 */
    FLCTL->BANK1_RDCTL = (FLCTL->BANK1_RDCTL&~0x0000F000)|FLCTL_BANK1_RDCTL_WAIT_2;
    CS->CTL1 = 0x20000000 |   /* Configure For SMCLK Divider /4 */
               0x00100000 |   /* Configure For HSMCLK Divider /2 */
               0x00000200 |   /* Configure For ACLK Sourced From REFOCLK */
               0x00000050 |   /* Configure For SMCLK And HSMCLK Sourced From HFXTCLK */
               0x00000005;    /* Configure For MCLK sourced From HFXTCLK */
    CS->KEY = 0;              /* Lock CS Module From Unintended Access */
    ClockFrequency = 48000000;
}

/* ------------Clock_GetFreq------------
 * Return the current system clock frequency for the LaunchPad.
 * Input: none
 * Output: system clock frequency in cycles/second
 */
uint32_t Clock_GetFreq(void){return ClockFrequency;}

/* ----------------delay-----------------
 * Delays about 6*ulCount cycles.
 * ulCount=8000 => 1ms = (8000 loops)*(6 cycles/loop)*(20.83 ns/cycle)
 */
void delay(unsigned long ulCount){
  __asm (  "pdloop:  subs    r0, #1\n"
      "    bne    pdloop\n");
}

/* ------------Clock_Delay1us------------
 * Simple delay function which delays about n microseconds.
 * Inputs: n, number of us to wait
 * Outputs: none
 */
void Clock_Delay1us(uint32_t n){
    n = (382 * n) / 100; /* 1 us, 48 MHz */
    while(n){n--;}
}

/* ------------Clock_Delay1ms------------
 * Simple delay function which delays about n milliseconds.
 * Inputs: n, number of ms to wait
 * Outputs: none
 */
void Clock_Delay1ms(uint32_t n){
    while(n)
    {
        delay(ClockFrequency/9162);   /* 1 ms, 48 MHz */
        n--;
    }
}