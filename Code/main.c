#include <msp430.h> 
#include <driverlib.h>
#include <math.h>
#include <stdbool.h>
#include "hal_LCD.h"
#include "servo.h"

#define MODE_BUTTON_PIN BIT5 //P1.5
#define THERMISTOR_PIN BIT1 //P8.1
#define GREEN_LED_PIN BIT4 //P1.4
#define PIR_LEFT BIT3 //P1.3

// Custom mode types
typedef enum {
    SWEEP = 0,
    TRACK = 1,
    STATIC = 2
} Fanmode_t;

// PIR position to angle
typedef enum {
    LEFT = 20,
    CENTRE = 90,
    RIGHT = 160
} PIR_t;

// ADC variables
float ADCValue_ = 0;
float temperature = 0;
float vrt = 0;
float rt = 0;

// Mode variables
#define BUTTON_DEBOUNCE() (__delay_cycles(90000))
Fanmode_t current_mode = STATIC;
bool has_toggled_mode = false;

// Servo variables
int current_angle = 90; // Degrees
bool going_cw = true;

// PIR sensors
bool pir_activated = false;

// Configure the ADC using registers
void init_adc()
{
    SYSCFG2 |= ADCPCTL9; //ADC Channel A9 (P8.1) select
    ADCCTL0 |= ADCON; //Switch ADC on and S&H to 4 clock cycles
    ADCCTL1 |= ADCSHP; //Sample and hold pulse mode
    ADCCTL2 |= ADCRES_1; //10 bit conversion resolution
    ADCMCTL0 |= ADCSREF_0 | ADCINCH_9; //Set the reference voltage and channel to A9
    ADCIFG &= ~0x01; //Clear the interrupt flag
    ADCIE |= ADCIE0; //Enable the interrupts
}

// Configure inputs and outputs used
void init_GPIO()
{
    // Setup LED to glow if temp passes 18 degrees celsius
    P1DIR |= GREEN_LED_PIN;
    P1OUT &= ~GREEN_LED_PIN;

    // Setup button to toggle fan mode
    P1DIR |= MODE_BUTTON_PIN;
    P1IN |= MODE_BUTTON_PIN;
    P1REN |= MODE_BUTTON_PIN;
    P1OUT |= MODE_BUTTON_PIN;
    P1IE |= MODE_BUTTON_PIN;
    P1IES |= MODE_BUTTON_PIN;
    P1IFG &= ~MODE_BUTTON_PIN;

    // Setup the PIR pins
    P1DIR &= ~PIR_LEFT;
    P1IN |= PIR_LEFT;
    P1REN |= PIR_LEFT;
    P1OUT |= PIR_LEFT;
    P1IE |= PIR_LEFT;
    P1IES &= ~PIR_LEFT; // Low to high transition interrupt for PIR sensor
    P1IFG &= ~PIR_LEFT;
}

// Handle cycling of fan modes
void cycle_fan_mode()
{
    switch(current_mode)
    {
    case STATIC:
        current_mode = SWEEP;
        break;
    case SWEEP:
        current_mode = TRACK;
        break;
    case TRACK:
        current_mode = STATIC;
        break;
    }
}

// Trigger the ADC to take temperature reading
void measure_temperature()
{
    ADC_startConversion(ADC_BASE, ADC_SINGLECHANNEL);
}


int main(void)
{
    // General Setup
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PMM_unlockLPM5();

	// Setup Peripherals
	init_adc();
	Init_LCD();
	init_GPIO();
	servo_init();
	servo_to_angle(90);
	displayFanMode(current_mode);

	// Enable global interrupts
    __enable_interrupt();

	while (1)
	{
	    // 1) Handle mode updates
	    if (has_toggled_mode)
	    {
	        BUTTON_DEBOUNCE();
	        cycle_fan_mode();
	        displayFanMode(current_mode);
	        has_toggled_mode = false;
	    }


	    // 2) Take temperature reading
	    measure_temperature();

	    // Handle temperature controlled actions
	    if (temperature > 18.0)
	    {
	        P1OUT |= GREEN_LED_PIN;
	    }
	    else
	    {
	        P1OUT &= ~GREEN_LED_PIN;
	    }

	    // 3) Handle mode determined servo actions
	    switch(current_mode)
	    {
	    case STATIC:
	        servo_to_angle(90);
	        current_angle = 90;
	        break;

	    case TRACK:
	        // handle PIR logic here
	        if (pir_activated)
	        {
	            servo_cycle_gradual(current_angle, LEFT);
	            current_angle = LEFT;
	            pir_activated = false;
	        }

	        break;

	    case SWEEP:

	        // Increment/ Decrement servo angle based on current angle
	        if(going_cw)
	        {
	            current_angle += 10;
	        }
	        else
	        {
	            current_angle -= 10;
	        }

	        // Handle direction change at bounds
	        if (current_angle >= 180)
	        {
	            going_cw = false;
	        }
	        else if (current_angle <= 0)
	        {
	            going_cw = true;
	        }

	        // Move the servo to the new angle (servo_to_angle handles angle under/overflow)
	        servo_to_angle(current_angle);

	        break;

	    }

	}

	return 0;
}


/*---------------------------------------------------------------------------*/
/* Interrupt Service Routines                                                */
/*---------------------------------------------------------------------------*/

// Define the interrupt service routine for the ADC
#pragma vector = ADC_VECTOR
__interrupt void ADC_ISR(void)
{
    #define beta 3950
    #define kelvin 273.15
    #define adc_resolution 1023
    #define avcc 3.3 // Supply reference voltage
    #define r_divider 10 // kOhms

    switch (__even_in_range(ADCIV, ADCIV_ADCIFG))
    {
    // Conversion complete
    case ADCIV_ADCIFG:

        ADCValue_ = ADCMEM0;
        // Map this to the temperature using Steinhart-Hart equation
        vrt = ADCValue_ * avcc /adc_resolution;
        rt = r_divider * vrt / (avcc- vrt);
        temperature = (1 / (log(rt / r_divider) / beta + 1 / (kelvin + 25))) - kelvin;
        ADC_clearInterrupt(ADC_BASE, ADC_COMPLETED_INTERRUPT_FLAG);
        break;
    }
}

// Define the interrupt service routine for port 1
#pragma vector = PORT1_VECTOR
__interrupt void P1_ISR(void)
{
    switch (__even_in_range(P1IV, P1IV_P1IFG7))
    {
    // Mode select button pressed
    case P1IV_P1IFG5:
        // Set the mode toggle flag
        has_toggled_mode = true;
        // Clear the interrupt
        P1IFG &= ~MODE_BUTTON_PIN;
        break;

    // Left PIR sensor
    case P1IV_P1IFG3:
        pir_activated = true;
        P1IFG &= ~PIR_LEFT;
        break;
    }
}
