#include "servo.h"

/******************************************
 *       GLOBALS                          *
 ******************************************/
extern int cur_servo_ang = 90;
extern bool going_cw = true;

/******************************************
 *       API FUNCTION DEFINITIONS         *
 ******************************************/

void servo_init()
{
    // Setup the GPIO pins
    P1DIR |= SERVO_PIN; // Output
    P1SEL0 |= SERVO_PIN; // Select primary pin function (timer output)

    // Setup the timer for PWM (uses CCR1 and CCR0)
    TA0CTL |= TASSEL_1 | MC_1 | ID_0 | TACLR; // ACLK, up to CCR0, divider = 1
    TA0CCTL1 |= OUTMOD_7; // PWM reset/set
    TA0CCR0 = PWM_PERIOD;
    TA0CCR1 = PWM_MIN_DUTY;
}

void servo_to_angle(double angle)
{
    // Validate input parameter
    if(angle>180)
    {
        angle = 180;
    }
    if(angle<0)
    {
        angle = 0;
    }

    // Map the input angle to PWM required
    TA0CCR1 = (int)(((angle * (PWM_MAX_DUTY - PWM_MIN_DUTY)) / 180) + PWM_MIN_DUTY);

    // Save new angle in 'cur_servo_ang' global variable
    cur_servo_ang = angle;
}

void servo_reset_pos()
{
    // Move to 0 degrees position
    TA0CCR1 = PWM_MIN_DUTY;

    // Save new angle in 'cur_servo_ang' global variable
    cur_servo_ang = 0;
}

void servo_cycle_gradual(int new_angle) {
    // Determine the direction of movement
    int increment = (cur_servo_ang < new_angle) ? 1 : -1;
    int inter_angle = cur_servo_ang;

    while (inter_angle != new_angle) {
        inter_angle += increment;
        servo_to_angle(inter_angle);
        SERVO_DELAY();
    }
    // The call to servo_to_angle already updates global 'cur_servo_ang' variable
}



