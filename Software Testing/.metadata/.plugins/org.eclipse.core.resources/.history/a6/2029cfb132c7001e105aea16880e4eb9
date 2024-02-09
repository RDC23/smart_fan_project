// The decoder uses ACTIVE-lOW outputs, a select A and select B input.
// SELB = MSB & SELA = LSB
// "00" => Output Y0 low
// "01" => Output Y1 low
// "10" => Output Y2 low
// "11" => Output Y3 low

#ifndef FANCONTROLLER_H_
#define FANCONTROLLER_H_

/******************************************
 * INCLUDES
 ******************************************/

#include <msp430.h>

/******************************************
 * DEFINES
 ******************************************/

#define SELB BIT4 // P1.4 Select input for the MSB
#define SELA BIT5 // P1.5 Select input for the LSB

/******************************************
 * API FUNCTION DECLARATIONS
 ******************************************/

void decoder_pins_init();

void decoder_set_led(int led_number);

#endif
