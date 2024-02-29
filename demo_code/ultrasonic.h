// ultrasonic.h
// Author: Ross Cathcart
// Module: Smart Fan Project
// Last update: 29/02/2024 - updated SIG_DELTA_DIST after lab tests to reduce false triggers
// Description: Provides function prototypes for interfacing MSP430 with HC-SR04 ultrasonic sensor,
// facilitating setup of trigger and echo pins, and measuring distance (0 - 400 cm).

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#include <driverlib.h>
#include <msp430fr4133.h>

#define TRIG_PIN BIT2            // P8.2
#define ECHO_PIN BIT3            // P8.3
#define CM_CONVERTION 0.0058309  // From us to cm
#define MAX_RANGE 400.0          // Max range of HC-SR04
#define SIG_DELTA_DISTANCE 10    // Distance to raise event flag

void ultrasonic_fire_pulse();   // Sends short 10 us pulse to trigger ultrasonic sensor.

void ultrasonic_setup_pins();   // Configures GPIO pins and timer for sensor operation.

unsigned long ultrasonic_get_distance(); // Gets last valid distance measured by sensor (in cm).

#endif

