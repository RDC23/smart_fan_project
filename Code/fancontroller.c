/*
 * fancontroller.c
 *
 *  Created on: 29 Jan 2024
 *      Author: rdcsc
 */
#include "fancontroller.h"

void fan_init()
{
    // Setup the GPIO pins
    P8DIR |= FAN_PIN; // Output
    P8SEL0 |= FAN_PIN; // Select primary pin function (timer output)

    // Setup the timer for PWM (uses CCR2 and CCR0)
    TA1CTL |= TASSEL_2 | MC_1 | ID_0 | TACLR; // SMCLK, up to CCR0, divider = 1
    TA1CCTL2 |= OUTMOD_7; // PWM reset/set
    TA1CCR0 |= FAN_PWM_PERIOD;
    TA1CCR2 |= FAN_PWM_MIN_DUTY;
}




