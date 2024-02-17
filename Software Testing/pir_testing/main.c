// This program tests the interfacing of the HC-SR501 PIR sensor array and HC-SR04 ultrasonic sensor with the MSP430 to control servo position.
// When a PIR sensor is activated, and an ultrasonic 'distance event' occurs, the servo will rotate to face the trigger direction.

#include <msp430.h> 
#include <stdbool.h>
#include <stdio.h>
#include "driverlib.h"
#include "servo.h"
#include "ultrasonic.h"
#include "hal_LCD.h"

// Pin defines
#define PIR_LEFT BIT3 // P1.5
#define PIR_MID BIT4 // P1.4
#define PIR_RIGHT BIT3 // P1.3

// PIR sensors
typedef enum {
    LEFT = 180,
    MID = 90,
    RIGHT = 0
} PIR_direction;

volatile PIR_direction activated_direction = MID;

// Ultrasonic
volatile int last_measured_distance = 0;
#define NUM_SAMPLES 5
volatile int distance_samples[NUM_SAMPLES];
volatile int sample_index = 0;

void pir_init()
{
    // Setup the PIR pins
    P1DIR &= ~(PIR_LEFT | PIR_MID | PIR_RIGHT);
    P1REN |= (PIR_LEFT | PIR_MID | PIR_RIGHT);
    P1OUT |= (PIR_LEFT | PIR_MID | PIR_RIGHT);
    P1IE |= (PIR_LEFT | PIR_MID | PIR_RIGHT);
    P1IES &= ~(PIR_LEFT | PIR_MID | PIR_RIGHT); // Low to high transition interrupt for PIR sensor
    P1IFG &= ~(PIR_LEFT | PIR_MID | PIR_RIGHT);
}

bool movement_event(int cdist)
{
    if ((activated_direction != cur_servo_ang) && (abs(cdist - last_measured_distance) > SIG_DELTA_DISTANCE))
    {
        return true;
    }
    else
    {
        return false;
    }
}

int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PMM_unlockLPM5();

	// Initialise peripherals
	Init_LCD();
	pir_init();
	ultrasonic_setup_pins();
	servo_init();
	servo_to_angle(90);
	__enable_interrupt();

	
	while(1)
	{
	    ultrasonic_fire_pulse();
	    int current_distance = ultrasonic_get_distance();

	    // If a 'movement event' is detected, turn the servo to face the PIR
	    if(movement_event(current_distance))
	    {
	        // DEBUG LCD USE
	        clearLCD();
            char temp_str[6];
            if (activated_direction == MID)
            {
                sprintf(temp_str, "MID", current_distance);
                displayShortMessage(temp_str);
            }
            if (activated_direction == LEFT)
            {
                sprintf(temp_str, "LEFT", current_distance);
                displayShortMessage(temp_str);
            }
            if (activated_direction == RIGHT)
            {
                sprintf(temp_str, "RIGHT", current_distance);
                displayShortMessage(temp_str);
            }

	        servo_cycle_gradual(cur_servo_ang, activated_direction);
	        // Update the last distance with the target position in the new servo frame
	        ultrasonic_fire_pulse();
	        last_measured_distance = ultrasonic_get_distance();
	    }
	}
	return 0;
}

/*---------------------------------------------------------------------------*/
/* Interrupt Service Routines                                                */
/*---------------------------------------------------------------------------*/

// Define the interrupt service routine for port 1. Contains PIR sensors.
#pragma vector = PORT1_VECTOR
__interrupt void P1_ISR(void)
{
    switch (__even_in_range(P1IV, P1IV_P1IFG7))
    {
    // Left PIR sensor
    case P1IV_P1IFG5:
        activated_direction = LEFT;
        P1IFG &= ~PIR_LEFT;
        break;
    // Middle PIR sensor
    case P1IV_P1IFG4:
        activated_direction = MID;
        P1IFG &= ~PIR_MID;
        break;
    // Right PIR sensor
    case P1IV_P1IFG3:
        activated_direction = RIGHT;
        P1IFG &= ~PIR_RIGHT;
        break;
    }
}
