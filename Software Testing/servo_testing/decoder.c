#include "decoder.h"

void decoder_pins_init()
{
    // Configure select output pins
    P1DIR |= SELA;
    P1DIR |= SELB;
    P1OUT &= ~SELA;
    P1DIR &= ~SELB;
}

void decoder_set_led(int led_number)
{
    switch(led_number)
    {
    case 0: // Sel = "00" light LED Y0
        P1OUT &= ~SELB;
        P1OUT &= ~SELA;
        break;
    case 1: // Sel = "01" light LED Y1
        P1OUT &- ~SELB;
        P1OUT |= SELA;
        break;
    case 2: // Sel = "10" light LED Y2
        P1OUT |= SELB;
        P1OUT &= ~SELA;
        break;
    case 3: // Sel = "11" light LED Y3
        P1OUT |= SELB;
        P1OUT |= SELA;
        break;
    case default: // Should never occur but light Y0 in this event
        P1OUT &= ~SELB;
        P1OUT &= ~SELA;
        break;
    }

}


