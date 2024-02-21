#include "pir.h"

extern volatile PIR_direction activated_direction = MID;

void pir_init()
{
    // Setup the PIR pins
    P1DIR &= ~(PIR_LEFT | PIR_MID | PIR_RIGHT);
    P1REN |= (PIR_LEFT | PIR_MID | PIR_RIGHT);
    P1OUT |= (PIR_LEFT | PIR_MID | PIR_RIGHT);
    P1IE |= (PIR_LEFT | PIR_MID | PIR_RIGHT);
    P1IES &= ~(PIR_LEFT | PIR_MID | PIR_RIGHT); // Low to high transition interrupt for PIR sensor
    P1IFG &= ~(PIR_LEFT | PIR_MID | PIR_RIGHT);
}