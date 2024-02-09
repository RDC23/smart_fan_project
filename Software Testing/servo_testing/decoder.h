// The decoder uses ACTIVE-lOW outputs, a select A and select B input.
// SELB = MSB & SELA = LSB
// "00" => Output Y0 low
// "01" => Output Y1 low
// "10" => Output Y2 low
// "11" => Output Y3 low

#ifndef DECODER_H_
#define DECODER_H_

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


/*
 * decoder_pins_init - Initialises the GPIO pins for the 2-to-4 decoder selection lines.
 *
 * This function configures the GPIO pins corresponding to the selection lines (SelA and SelB) of the 2-to-4 decoder.
 * It sets these pins as outputs and initialises their initial states.
 *
 * @param None
 *
 * @return None
 *
 * Note: This function assumes that the selection lines SelA and SelB are connected to GPIO pins (such as P1OUT)
 * and need to be configured prior to use.
 *
 * Example usage:
 *      Call this function once at the beginning of the program to initialise the decoder pins.
 */
void decoder_pins_init();

/*
 * decoder_set_led - Sets the LED based on the input LED number using a 2-to-4 decoder logic.
 *
 * This function sets the LED corresponding to the input LED number using a 2-to-4 decoder logic. The LED number
 * is used to determine the selection lines (SelA and SelB) required to activate the desired LED output.
 *
 * @param led_number An integer specifying the LED number to be lit up (0 to 3).
 *
 * @return None
 *
 * Note: This function assumes that the selection lines SelA and SelB are connected to GPIO pins (such as P1OUT)
 * and have been configured as outputs prior to its invocation.
 *
 * Example usage:
 *      To light up LED Y2:
 *          decoder_set_led(2);
 */
void decoder_set_led(int led_number);


#endif
