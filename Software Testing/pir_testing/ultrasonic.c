#include <driverlib.h> // For __delay_cycles
#include "ultrasonic.h"

// Static used to limit global scope of variables to this compilation unit.
static volatile unsigned long validated_dist = 0;

void ultrasonic_fire_pulse()
{
    // Clear the timer for the ECHO pin
    TA1CTL |= TACLR;
    // Send out a short 10us (logic high) trigger pulse
    P1OUT &= ~TRIG_PIN;
    __delay_cycles(2);
    P1OUT |= TRIG_PIN;
    __delay_cycles(10);
    P1OUT &= ~TRIG_PIN;
    // Wait for 60 ms before allowing function to be called again.
    __delay_cycles(60000);
}

void ultrasonic_setup_pins()
{
    // P8.2 trigger pulse. GPIO output
    P8DIR |= TRIG_PIN;
    P8OUT &= ~TRIG_PIN;

    // P8.3 echo pulse and timer input. Secondary function, TA1 CCI2A input
    P8DIR &= ~ECHO_PIN;
    P8SEL0 |= ECHO_PIN;
    TA1CTL |= TASSEL_2 | MC_2 | TACLR; // SMCLK input source for 1MHz, continuous mode
    TA1CCTL2 |= CM_3 | SCS | CAP | CCIS_0 | CCIE; // Capture on both edges, sync capture, capture mode, input select CCIxA, enable interrupts
}

unsigned long ultrasonic_get_distance()
{
    return validated_dist;
}

// ISR for Timer A1 (ECHO signal capture)
#pragma vector = TIMER1_A1_VECTOR
__interrupt void TIMERA1_ISR1(void)
{
    // Declared 'static' to maintain state between ISRQ calls
    static volatile unsigned long start_time;
    static volatile unsigned long end_time;
    static volatile unsigned long delta_time;
    static volatile unsigned long distance;

    TA1CTL &= ~CCIFG; // Clear the interrupt flag
    switch(TA1IV)
    {
    case 10: // Timer overflow
        break;
    default:
       // Read the CCI bit (ECHO signal) in CCTL1
       // If ECHO is HIGH then start counting (rising edge)
       if (TA1CCTL2 & CCI)
       {
         start_time = TA1CCR2;
       }
       // If ECHO is LOW then stop counting (falling edge)
       else
       {
         end_time = TA1CCR2;
         delta_time = end_time - start_time;
         distance = (unsigned long)(delta_time / 58.3090379); // 58.3 = speed of sound cm/s, halved for echo time bounce

         //only accept values within HC-SR04 valid range
         if (distance  >= 0.0 && distance <= MAX_RANGE)
         {
           validated_dist = distance;
         }
       }
       break;
     }

   }
