// thermistor.c
// Author: Ross Cathcart
// Module: Smart Fan Project
// Last update: 29/02/2024 - updated r_divider value for 4.7 kOhm thermistor
// Description: Provides function definitions to interface MSP430 with thermistor potential divider circuit,
// including ADC setup, interrupt configuration, and ISR for temperature conversion using Steinhart-Hart equation.

#include "thermistor.h"
#include <math.h>

static float ADCValue_ = 0;
static float temperature = 0;
static float vrt = 0;
static float rt = 0;

void init_thermistor()
{
    SYSCFG2 |= ADCPCTL9;   // ADC Channel A9 (P8.1) select
    ADCCTL0 |= ADCON;       // Switch ADC on, S&H to 4 clock cycles
    ADCCTL1 |= ADCSHP;      // Sample and hold pulse mode
    ADCCTL2 |= ADCRES_1;    // 10-bit conversion resolution
    ADCMCTL0 |= ADCSREF_0 | ADCINCH_9; // Set reference voltage and channel to A9
    ADCIFG &= ~0x01;        // Clear interrupt flag
    ADCIE |= ADCIE0;        // Enable interrupts
}

float measure_temperature()
{
    ADC_startConversion(ADC_BASE, ADC_SINGLECHANNEL);
    return temperature;
}

#pragma vector = ADC_VECTOR
__interrupt void ADC_ISR(void)
{
    #define beta 3950
    #define kelvin 273.15
    #define adc_resolution 1023
    #define avcc 3.3            // Supply reference voltage
    #define r_divider 4.7       // kOhms

    switch (__even_in_range(ADCIV, ADCIV_ADCIFG))
    {
    case ADCIV_ADCIFG: // Conversion complete
        ADCValue_ = ADCMEM0;
        vrt = ADCValue_ * avcc / adc_resolution;
        rt = r_divider * vrt / (avcc - vrt);
        temperature = (1 / (log(rt / r_divider) / beta + 1 / (kelvin + 25))) - kelvin;
        ADC_clearInterrupt(ADC_BASE, ADC_COMPLETED_INTERRUPT_FLAG);
        break;
    }
}
