// ultrasonic.c
// Author: Ross Cathcart
// Module: Smart Fan Project
// Last update: 25/03/2024 - updated distance conversion formula to simplify computation
// Description: Provides function definitions to setup HC-SR04 ultrasonic sensor and ISR code
// for timing and distance measurement using Timer A1.

#include "ultrasonic.h"

static volatile unsigned long validated_dist = 0;

void ultrasonic_fire_pulse()
{
    TA1CTL |= TACLR;      // Clear timer for ECHO pin
    P8OUT &= ~TRIG_PIN;   // Send 10us trigger pulse
    __delay_cycles(2);
    P8OUT |= TRIG_PIN;
    __delay_cycles(10);
    P8OUT &= ~TRIG_PIN;
    __delay_cycles(60000); // Wait 60ms before next call
}

void ultrasonic_setup_pins()
{
    P8DIR |= TRIG_PIN;    // Trigger pulse (GPIO output)
    P8OUT &= ~TRIG_PIN;

    P8DIR &= ~ECHO_PIN;   // Echo pulse and timer input (Secondary function, TA1 CCI2A input)
    P8SEL0 |= ECHO_PIN;
    TA1CTL |= TASSEL_2 | MC_2 | TACLR; // SMCLK input source (1MHz), continuous mode
    TA1CCTL2 |= CM_3 | SCS | CAP | CCIS_0 | CCIE; // Capture on both edges
}

unsigned long ultrasonic_get_distance()
{
    return validated_dist;
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void TIMERA1_ISR1(void)
{
    static volatile unsigned long start_time;
    static volatile unsigned long end_time;
    static volatile unsigned long delta_time;
    static volatile unsigned long distance;

    TA1CTL &= ~CCIFG; // Clear interrupt flag
    switch(TA1IV)
    {
    case 10: // Timer overflow
        break;
    default:
       if (TA1CCTL2 & CCI) // Rising edge
       {
         start_time = TA1CCR2;
       }
       else // Falling edge
       {
         end_time = TA1CCR2;
         delta_time = end_time - start_time;
         distance = delta_time / 58; // Speed of sound in cm/ms

         if (distance >= 0 && distance <= MAX_RANGE) // Accept values within HC-SR04 range
         {
           validated_dist = distance;
         }
       }
       break;
     }
}
