// INCLUDES
#include <msp430.h>
#include <stdbool.h>
#include "driverlib.h"
#include "servo.h"
#include "fancontroller.h"

// GLOBALS
volatile int ADCValue_ = 0;
volatile int adc_angle = 0;
volatile int button1 = 0;
volatile bool servo = 0; // are we considering servo? If not, we are considering fan pwm


// Mapping function for # of delays
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void msp_setup()
{
    WDTCTL = WDTPW | WDTHOLD;
    PMM_unlockLPM5();
    __enable_interrupt();
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

// Setup the button GPIO
void init_buttons()
{
    P1SEL0 &= (~BIT3);
    P1DIR &= (~BIT3);
    P1IES |= (BIT3);
    P1REN |= (BIT3); // ESSENTIAL to set a defined starting logic level
    P1IFG &= (~BIT3); // Clear interrupt flag for P1.3
    P1IE |= (BIT3); // Enable interrupt for P1.3
}


// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    switch(__even_in_range(P1IV,16))
    {
        case P1IV_P1IFG3:
          // Toggle PWM pin to consider either servo or fan
          servo = !servo;
          GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN3);
          break;
    }
}

// Define the interrupt service routine for the ADC
#pragma vector = ADC_VECTOR
__interrupt void ADC_ISR(void)
{
    switch (__even_in_range(ADCIV, ADCIV_ADCIFG))
    {
    // Conversion complete
    case ADCIV_ADCIFG:
        ADCValue_ = ADCMEM0; //When a conversion result is written to ADCMEM0, the ADCIFG0 is set.

        // Map ADC value (0-1023) to angle for either servo or fan pwm

        // Servo
        if (servo)
        {
            adc_angle = map(ADCValue_, 0, 1023, 0, 180);
            ADC_clearInterrupt(ADC_BASE, ADC_COMPLETED_INTERRUPT_FLAG);
        }

        // Fan
        else
        {
            adc_angle = map(ADCValue_, 0, 1023, FAN_PWM_MIN_DUTY, FAN_PWM_MAX_DUTY);
            ADC_clearInterrupt(ADC_BASE, ADC_COMPLETED_INTERRUPT_FLAG);

        }

        break;
    }
}



int main(void)
{
    // General Setup
    msp_setup();
    init_adc();
    servo_init();
    fan_init();
    servo_reset_pos();
    servo_to_angle(90); //start at 90 degrees
    init_buttons();

    while(1)
    {
        ADC_startConversion(ADC_BASE, ADC_SINGLECHANNEL);
        if (servo)
        {
            servo_to_angle(adc_angle);
        }
        else
        {
            TA0CCR2 = adc_angle;
        }
    }
}
