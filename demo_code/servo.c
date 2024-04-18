// servo.c
// Author: Ross Cathcart
// Module: Smart Fan Project
// Last update: 29/02/2024 - added "going_cw" variable to track servo direction
// Description: Implementation file for servo.h, defining functions to interact with MG-946R servo motor.

#include "servo.h"

int cur_servo_ang = 90;
bool going_cw = true;

void servo_init()
{
    P1DIR |= SERVO_PIN;     // GPIO output
    P1SEL0 |= SERVO_PIN;    // Select primary pin function (timer output)

    TA0CTL |= TASSEL_1 | MC_1 | ID_0 | TACLR; // ACLK, up to CCR0, divider = 1
    TA0CCTL1 |= OUTMOD_7;   // PWM reset/set
    TA0CCR0 = PWM_PERIOD;
    TA0CCR1 = PWM_MIN_DUTY;
}

void servo_to_angle(double angle)
{
    if(angle > SERVO_MAX_ANGLE)
        angle = SERVO_MAX_ANGLE;
    if(angle < SERVO_MIN_ANGLE)
        angle = SERVO_MIN_ANGLE;

    TA0CCR1 = (int)(((angle * (PWM_MAX_DUTY - PWM_MIN_DUTY)) / 180) + PWM_MIN_DUTY);
    cur_servo_ang = (int)angle;
}

void servo_reset_pos()
{
    TA0CCR1 = PWM_MIN_DUTY;
    cur_servo_ang = 0;
}

void servo_cycle_gradual(int new_angle) {
    int increment = (cur_servo_ang < new_angle) ? 1 : -1;
    int inter_angle = cur_servo_ang;

    while (inter_angle != new_angle) {
        inter_angle += increment;
        servo_to_angle(inter_angle);
        SERVO_DELAY();
    }
    cur_servo_ang = inter_angle;
}
