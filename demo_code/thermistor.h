// thermistor.h
// Author: Ross Cathcart
// Module: Smart Fan Project
// Last update: 29/02/2024 - updated thermistor pin to use P8.1 for ADC channel A9
// Description: Provides function prototypes for using a thermistor-based circuit to record and
// process external temperature readings using the ADC.

#ifndef THERMISTOR_H_
#define THERMISTOR_H_

#include <msp430fr4133.h>
#include <driverlib.h>

#define THERMISTOR_PIN BIT1 // P8.1

void init_thermistor();       // Setup ADC for thermistor

float measure_temperature();   // Take ADC measurement and return temperature

#endif /* THERM_H_ */
