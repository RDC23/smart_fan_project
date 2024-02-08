#include "decoder.h"

void decoder_pins_init()
{
    // Configure select output pins
    P1DIR |= SEL0;
    P1DIR |= SEL1;
    P1OUT &= ~SEL0;
    P1DIR &= ~SEL1;
}

void decoder_set_led(int led_number)
{
    switch(led_number)
    {
    case 0:
        // finish, set corresponding sela/selb high or low based on TT


    }

}


