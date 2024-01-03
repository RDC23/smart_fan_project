#include "driverlib.h"

#define LED_PIN BIT0;


int main()
{
    P4DIR |= LED_PIN;
    P4OUT &= ~LED_PIN;

    while(1)
    {
        P4OUT ^= LED_PIN;
        __delay_cycles(2000000);
    }

}



