#include <msp430.h> 
#include <stdbool.h>
#include <stdio.h>
#include "driverlib.h"
#include "servo.h"
#include "hal_LCD.h"
#include "ultrasonic.h"

#define MOTOR_PWM_PIN BIT6 // P1.6
#define MOTOR_ENABLE_PIN BIT3 // P1.3
#define PWM_PERIOD 640 // ACLK (32kHz, divider 1) must count to 640 to generate a 20ms output signal)
#define MOTOR_DUTY 100

void setupMotor()
{
    // Setup the pins
     P1DIR |= MOTOR_PWM_PIN; // Output
     P1SEL0 |= MOTOR_PWM_PIN; // Select primary pin function (timer output)
     P1DIR |= MOTOR_ENABLE_PIN;
     P1OUT |= MOTOR_ENABLE_PIN;

     // Setup the timer for PWM (uses CCR1 and CCR0)
     TA0CTL |= TASSEL_1 | MC_1 | ID_0 | TACLR; // ACLK, up to CCR0, divider = 1
     TA0CCTL2 |= OUTMOD_7; // PWM reset/set
     TA0CCR0 |= PWM_PERIOD;
     TA0CCR2 |= MOTOR_DUTY;
}


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    PMM_unlockLPM5();
    __enable_interrupt();
    servo_init();
    servo_to_angle(0);
    Init_LCD();
    int curang = 0;

    while (1)
	{
      //clear screen
      clearLCD();
      //move clockwise
	  while (curang < 180){
	      curang++;
	      servo_to_angle(curang);
	      __delay_cycles(20000);
	  }
	  //move anticlockwise
	  while (curang > 0){
	      curang--;
          servo_to_angle(curang);
          __delay_cycles(20000);
	  }
	  // display complete message
	  displayScrollText("SERVO");
	}

	return 0;
}
