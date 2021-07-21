#include <G8RTOS.h>
#include <Robot.h>
#include "msp.h"
#include "BoardSupportPackage\inc\Clock.h"
#include "BoardSupportPackage\DriverLib\wdt_a.h"
#include "CC3100SupportPackage\cc3100_usage\cc3100_usage.h"
#include "BoardSupportPackage\inc\Joystick.h"

/* Helper Functions */
extern playerType GetPlayerRole();

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

void main(void)
{
    /* Disable Watchdog */
    WDT_A_clearTimer();
    WDT_A_holdTimer();

    /* Initialize Clock And Timer */
    Clock_Init48MHz();
    SysTick_Init();

    /* Decide Which User We Are */
    playerType role = GetPlayerRole();

    /* Connect The Users And Initialize WiFi Accordingly */
    initCC3100(role);

    /* Send Something Over Wifi */
    DeviceInfo_t clientInfo = {0, false, false};
    State_t data = {0, 200};
    _i32 returnVal = -1;
    if(role == Host) /* Press 1.4 For Joystick Board */
    {
        /* Wait For Client To Sent Specific Player Information */
        while((returnVal == -1) || (clientInfo.acknowledge == false))
        {
            returnVal = ReceiveData((uint8_t *)&clientInfo, sizeof(clientInfo));
        }

        /* Acknowledge That We Have Connected With Client */
        SendData((uint8_t *)&clientInfo, clientInfo.IP_address, sizeof(clientInfo));
        clientInfo.acknowledge = false;

        /* Wait For Client To Join */
        returnVal = -1;
        while((returnVal == -1) || (clientInfo.joined == false))
        {
            returnVal = ReceiveData((uint8_t *)&clientInfo, sizeof(clientInfo));
        }

        /* Blue LED Indicating Good Connection */
        RED_LED_OFF;
        BLUE_LED_ON;


        /* Init joystick without interrupts */
        Joystick_Init_Without_Interrupt();

        uint16_t pos = 0;
        int16_t X = 0;
        int16_t Y = 0;
        int16_t delta = 0;
        int16_t counter = 0;
        while(1)
        {
            /* Read Joystick Value */
            GetJoystickCoordinates(&X, &Y);

            if(counter == 100)
            {
                /* Convert Value To Range */
                delta = X / 1000;
                if(((int16_t)pos >= (delta * -1)) && (((int16_t)pos + delta) <= (775 - 388)))
                {
                    pos += delta;
                }

                data.pulse = pos;
                counter = 0;
            }
            else
            {
                counter++;
            }

            SendData((uint8_t *)&data, clientInfo.IP_address, sizeof(data));
        }
    }
    else if(role == Client) /* Press 1.1 For Robot */
    {
        /* Initialize Client */
        clientInfo.IP_address = getLocalIP();

        /* Send Player To The Host */
        while((returnVal == -1) || (clientInfo.acknowledge == false))
        {
            clientInfo.acknowledge = true;
            SendData((uint8_t *) &clientInfo, HOST_IP_ADDR, sizeof(clientInfo));
            clientInfo.acknowledge = false;
            returnVal = ReceiveData((uint8_t *) &clientInfo, sizeof(clientInfo));
        }

        /* Join Game */
        clientInfo.joined = true;
        SendData((uint8_t *)&clientInfo, HOST_IP_ADDR, sizeof(clientInfo));

        /* If Joined, Acknowledge Joined To Host And Show Connection With Blue LED */
        RED_LED_OFF;
        BLUE_LED_ON;

        /* PWM Pin 4.6 */
        P4->SEL0 &= ~0x40;
        P4->SEL1 &= ~0x40;
        P4->DIR  |=  0x40;
        P4->OUT  &= ~0x40;

        while(1)
        {
            ReceiveData((uint8_t *) &data, sizeof(data));

            /* Update Duty Cycle */
            /* PWM High */
            P4->OUT |= 0x40;

            /* High Cycle (1 - 2 ms) */
            SysTick_Wait1us(data.pulse + 388);

            /* PWM Low */
            P4->OUT &= ~0x40;

            /* Low Cycle (19 - 18 ms) */
            SysTick_Wait1us(7741 - (data.pulse + 388));
        }
    }

    while(1);
}
