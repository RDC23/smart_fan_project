#ifndef THERMISTOR_H_
#define THERMISTOR_H_

/******************************************
 * INCLUDES                 
 ******************************************/

#include <msp430fr4133.h>
#include <driverlib.h>

/******************************************
 * DEFINES                  
 ******************************************/

#define THERMISTOR_PIN BIT1 //P8.1

/******************************************
 * API FUNCTION DECLARATIONS      
 ******************************************/

void init_thermistor(); // Setup the ADC for using the thermistor

float measure_temperature(); // Take an ADC measurement then return the temperature

#endif /* THERM_H_ */
