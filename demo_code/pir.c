// pir.c
// Author : Ross Cathcart
// Last update : 07/03/2024 - added configuration of PIR pins
// This is the implementation file for pir.c, containing a function to initialise the PIR sensor pins.

#include "pir.h"

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