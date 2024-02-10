// This code tests the system integration of the thermistor circuit to control fan speed.

#include <msp430.h> 
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "driverlib.h"
#include "hal_LCD.h"
#include "fancontroller.h"

#define THERMISTOR_PIN BIT1 //P8.1
#define BUTTON_TOGGLE BIT2 // P1.2
#define ON_OFF_BUTTON  BIT6 // P2.6

// Globals
bool show_temp = true;
bool fan_on = true;


// ADC variables
double ADCValue_ = 0;
double temperature = 0;
double vrt = 0;
double rt = 0;

// Setup GPIO
void init_GPIO()
{
    // Setup button to toggle fan mode
    P1DIR |= BUTTON_TOGGLE;
    P1REN |= BUTTON_TOGGLE;
    P1OUT |= BUTTON_TOGGLE;
    P1IE |= BUTTON_TOGGLE;
    P1IES |= BUTTON_TOGGLE;
    P1IFG &= ~BUTTON_TOGGLE;

    // Setup on/ off button to stop/start fan
    P2DIR |= ON_OFF_BUTTON;
    P2REN |= ON_OFF_BUTTON;
    P2OUT |= ON_OFF_BUTTON;
    P2IE |= ON_OFF_BUTTON;
    P2IES |= ON_OFF_BUTTON;
    P2IFG &= ~ON_OFF_BUTTON;
}

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

// Trigger the ADC to take temperature reading
void measure_temperature()
{
    ADC_startConversion(ADC_BASE, ADC_SINGLECHANNEL);
}

int main(void)
{
    // General Setup
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PMM_unlockLPM5();

    // Setup Peripherals
    init_GPIO();
    init_adc();
    Init_LCD();
    fan_init();
    fan_torque_boost();

    // Enable global interrupts
    __enable_interrupt();

    while (1)
    {
        // Take temperature reading
        measure_temperature();

        // Update fan PWM
        if (fan_on)
        {
            fan_temp_to_speed(temperature);
        }

        else
        {
            fan_stop();
        }

        // Display temperature or fan speed on LCD
        if (show_temp)
        {
            clearLCD();
            char temp_str[6];
            sprintf(temp_str, "%d DEG", (int)temperature);
            displayShortMessage(temp_str);
        }

        else
        {
            clearLCD();
            char fan_speed_str[6];
            sprintf(fan_speed_str, "%d PER", fan_speed);
            displayShortMessage(fan_speed_str);
        }



    }

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
    case P1IV_P1IFG2:
        show_temp = !show_temp;
        // Clear the interrupt
        P1IFG &= ~BUTTON_TOGGLE;
        break;
    }
}

// Define the interrupt service routine for port 2
#pragma vector = PORT2_VECTOR
__interrupt void P2_ISR(void)
{
    switch (__even_in_range(P2IV, P2IV_P2IFG7))
    {
    case P2IV_P2IFG6:
          fan_on = !fan_on;
          // Clear the interrupt
          P2IFG &= ~BUTTON_TOGGLE;
          break;
      }
}
