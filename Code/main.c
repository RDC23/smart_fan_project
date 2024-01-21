#include <msp430.h> 
#include <driverlib.h>
#include <math.h>
#include <stdbool.h>
#include "hal_LCD.h"

// Custom mode types
typedef enum {
    SWEEP = 0,
    TRACK = 1,
    STATIC = 2
} Fanmode_t;

// ADC Globals
float ADCValue_ = 0;
float temperature = 0;
float vrt = 0;
float rt = 0;

// Mode variables
Fanmode_t current_mode = STATIC;
bool has_toggled_mode = false;

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
    // Setup LED P4.0 to glow if temp passes 18 degrees c
    P4DIR |= BIT0;
    P4OUT &= ~BIT0;

    // Setup button to toggle fan mode using P1.2
    P1DIR |= BIT2;
    P1IN |= BIT2;
    P1REN |= BIT2;
    P1OUT |= BIT2;
    P1IE |= BIT2;
    P1IES |= BIT2;
    P1IFG &= BIT2;
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

	// Enable global interrupts
    __enable_interrupt();

	while (1)
	{
	    if (has_toggled_mode)
	    {
	        // Update the LCD to display the new mode
	        displayFanMode(current_mode);
	        __delay_cycles(200000);
	        has_toggled_mode = false;
	    }


	    /*ADC_startConversion(ADC_BASE, ADC_SINGLECHANNEL);

	    if (temperature > 18.0)
	    {
	        P4OUT |= BIT0;
	    }
	    else
	    {
	        P4OUT &= ~BIT0;
	    }*/
	//}
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
    case P1IV_P1IFG2:
        // Cycle through modes
        if (current_mode == STATIC && !has_toggled_mode)
        {
            current_mode = SWEEP;
        }
        else if (current_mode == SWEEP && !has_toggled_mode)
        {
            current_mode = TRACK;
        }
        else if (current_mode == TRACK && !has_toggled_mode)
        {
            current_mode = STATIC;
        }
        // Set the mode toggle flag
        has_toggled_mode = true;
        // Clear the interrupt
        P1IFG &= ~BIT2;
        break;
    }
}
