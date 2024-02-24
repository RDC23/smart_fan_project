#include "fancontroller.h"

int fan_speed = 100;

void fan_init()
{
    // Setup the GPIO pins
    P1DIR |= FAN_PIN; // Output
    P1SEL0 |= FAN_PIN; // Select primary pin function (timer output)

    // Setup the timer for PWM (uses CCR2 to adjust the duty cycle)
    TA0CCTL2 |= OUTMOD_7; // PWM reset/set
    TA0CCR0 |= FAN_PWM_MAX_DUTY;
    TA0CCR2 |= FAN_PWM_MAX_DUTY; // Startup at max speed to provide high initial torque
    TA0CTL |= TASSEL_1 | MC_1 | ID_0 | TACLR; // ACLK, up to CCR0, divider = 1
}

void fan_torque_boost()
{
    TA0CCR2 |= FAN_PWM_MAX_DUTY; // Boost fan to max speed to provide high initial torque
    __delay_cycles(20000); // Delay to allow BJT gate switching and fan startup
}

void fan_to_speed_percent(int speed)
{
    TA0CCR2 = speed * (FAN_PWM_MAX_DUTY - FAN_PWM_MIN_DUTY) / (100) + FAN_PWM_MIN_DUTY;
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
    // Map the PWM value to a power value. At FAN_PWM_MAX_DUTY the power is P= VI = (12V * 0.25A) = 3W.
    return (((double)TA0CCR2 - FAN_PWM_MIN_DUTY)*3)/(FAN_PWM_MAX_DUTY - FAN_PWM_MIN_DUTY);
}

void fan_speed_to_power_LED()
{
    #define LOW_SPEED_BOUNDARY 33
    #define HIGH_SPEED_BOUNDARY 55

    if (fan_speed < LOW_SPEED_BOUNDARY)
    {
        P5OUT |= LOW_POWER_LED;
        P5OUT &= ~(MED_POWER_LED | HIGH_POWER_LED);
    }
    else if (fan_speed >= LOW_SPEED_BOUNDARY && fan_speed < HIGH_SPEED_BOUNDARY)
    {
        P5OUT |= MED_POWER_LED;
        P5OUT &= ~(HIGH_POWER_LED | LOW_POWER_LED);
    }
    else
    {
        P5OUT |= HIGH_POWER_LED;
        P5OUT &= ~(MED_POWER_LED | LOW_POWER_LED);
    }
}




