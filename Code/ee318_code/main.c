#include <msp430.h> 
#include "driverlib.h"
/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    PMM_unlockLPM5();
    __enable_interrupt();
	P4DIR |= BIT0;
	P4OUT &= BIT0;
	
	while (1)
	{
	    P4OUT ^= BIT0;
	    __delay_cycles(200000);
	}

	return 0;
}
