#include "thermistor.h"
#include <msp430fr4133.h>
#include <driverlib.h>

// ADC variables
static float ADCValue_ = 0;
static float temperature = 0;
static float vrt = 0;
static float rt = 0;

// Configure the ADC using registers for thermistor use
void init_thermistor()
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
float measure_temperature()
{
    ADC_startConversion(ADC_BASE, ADC_SINGLECHANNEL);
    return temperature;
}

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