#include <driverlib.h> // For __delay_cycles
#include "ultrasonic.h"

// Static used to limit global scope of variables to this compilation unit.
static volatile unsigned long validated_dist = 0;

void ultrasonic_fire_pulse()
{
    // Clear the timer for the ECHO pin
    TA0CTL |= TACLR;
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
    // P1.5 trigger pulse. GPIO output
    P1DIR |= TRIG_PIN;
    P1OUT &= ~TRIG_PIN;

    // P1.6 echo pulse and timer input. Secondary function, TA0 CCI2A input
    P1DIR &= ~ECHO_PIN;
    P1SEL0 |= ECHO_PIN;
    TA0CTL &= ~(0xFFFF); // Clear current config
    TA0CTL |= TASSEL_2 | MC_2 | TACLR; // SMCLK input source for 1MHz, continuous mode
    TA0CCTL2 |= CM_3 | SCS | CAP | CCIS_0 | CCIE; // Capture on both edges, sync capture, capture mode, input select CCIxA, enable interrupts

}

unsigned long ultrasonic_get_distance()
{
    return validated_dist;
}

// ISR for Timer A1 (ECHO signal capture)
#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMERA0_ISR1(void)
{
    // Declared 'static' to maintain state between ISRQ calls
    static volatile unsigned long start_time;
    static volatile unsigned long end_time;
    static volatile unsigned long delta_time;
    static volatile unsigned long distance;

    TA0CTL &= ~CCIFG; // Clear the interrupt flag
    switch(TA0IV)
    {
    case 10: // Timer overflow
        break;
    default:
       // Read the CCI bit (ECHO signal) in CCTL0
       // If ECHO is HIGH then start counting (rising edge)
       if (TA0CCTL2 & CCI)
       {
         start_time = TA0CCR2;
       }
       // If ECHO is LOW then stop counting (falling edge)
       else
       {
         end_time = TA0CCR2;
         delta_time = end_time - start_time;
         distance = (unsigned long)(delta_time / (0.00583090379 * 10000));

         //only accept values within HC-SR04 valid range
         if (distance  >= 0.0 && distance <= MAX_RANGE)
         {
             validated_dist = distance;
         }
         else
         {
             validated_dist - MAX_RANGE;
         }
       }
       break;
     }

   }
