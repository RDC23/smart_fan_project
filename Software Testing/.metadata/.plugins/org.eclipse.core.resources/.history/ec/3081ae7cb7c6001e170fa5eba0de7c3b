#include "fancontroller.h"

void fan_init()
{
    // Setup the GPIO pins
    P1DIR |= FAN_PIN; // Output
    P1SEL0 |= FAN_PIN; // Select primary pin function (timer output)

    // Setup the timer for PWM (uses CCR2 to adjust the duty cycle)
    TA0CCTL2 |= OUTMOD_7; // PWM reset/set
    TA0CCR2 |= FAN_PWM_MIN_DUTY;
}




