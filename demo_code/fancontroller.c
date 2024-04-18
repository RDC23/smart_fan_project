// fancontroller.c
// Author: Ross Cathcart
// Module: Part of Smart Fan Project
// Description: Provides function definitions to control a fan with MSP430, set its speed, and calculate power output.

#include "fancontroller.h"

int fan_speed = 100;
Efficiency_t efficiency = LOW;

void fan_init()
{
    // Setup GPIO pins
    P1DIR |= FAN_PIN;      // Output
    P1SEL0 |= FAN_PIN;     // Select primary pin function (timer output)

    // Setup timer for PWM (uses CCR2 to adjust duty cycle)
    TA0CCTL2 |= OUTMOD_7;
    TA0CCR0 |= FAN_PWM_MAX_DUTY;
    TA0CCR2 |= FAN_PWM_MAX_DUTY; 
    TA0CTL |= TASSEL_1 | MC_1 | ID_0 | TACLR;
}

void fan_torque_boost()
{
    TA0CCR2 |= FAN_PWM_MAX_DUTY; // Max speed for high torque
    __delay_cycles(20000); // Delay for BJT gate switching
}

void fan_to_speed_percent(int speed)
{
    TA0CCR2 = speed * (FAN_PWM_MAX_DUTY - FAN_PWM_MIN_DUTY) / 100 + FAN_PWM_MIN_DUTY;
}

void fan_temp_to_speed(double temp)
{
    fan_speed = ((temp - FAN_MIN_TEMP) * 100) / (FAN_MAX_TEMP - FAN_MIN_TEMP);
    fan_to_speed_percent(fan_speed);
}

void fan_stop()
{
    TA0CCR2 = 0; // Stop fan in emergency
    fan_speed = 0;
}

double fan_calculate_power()
{
    // Map PWM value to power. At FAN_PWM_MAX_DUTY, power is P= VI = (12V * 0.25A) = 3000mW.
    return ((double)(TA0CCR2 - FAN_PWM_MIN_DUTY) * 3000) / (double)(FAN_PWM_MAX_DUTY - FAN_PWM_MIN_DUTY);

}

void fan_speed_to_power_LED()
{
#define LOW_SPEED_BOUNDARY 35
#define HIGH_SPEED_BOUNDARY 50

    if (fan_speed < LOW_SPEED_BOUNDARY)
    {
        P5OUT |= LOW_POWER_LED;
        P5OUT &= ~(MED_POWER_LED | HIGH_POWER_LED);
        efficiency = HIGH;
    }
    else if (fan_speed >= LOW_SPEED_BOUNDARY && fan_speed < HIGH_SPEED_BOUNDARY)
    {
        P5OUT |= MED_POWER_LED;
        P5OUT &= ~(HIGH_POWER_LED | LOW_POWER_LED);
        efficiency = MED;
    }
    else
    {
        P5OUT |= HIGH_POWER_LED;
        P5OUT &= ~(MED_POWER_LED | LOW_POWER_LED);
        efficiency = LOW;
    }
}
