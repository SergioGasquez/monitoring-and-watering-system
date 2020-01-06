#ifndef __MOISTURE_SENSOR__
#define __MOISTURE_SENSOR__


/************************************************/
/*                    PINOUT                    */
/*
 *    SEN0193                   CC3200
 *    ----------------------------------
 *    GND                       GND
 *    VCC                       VCC
 *    AOUT                      PIN_58
 *
 *                      CONFIG
 *
 *
 *        AOUT ---R1---X PIN_58
 *                     |
 *                     R2       R1 = R2 = 470 kOhm
 *                     |
 *                    GND
 */


#include<stdint.h>
#include "pin.h"
#include "adc.h"
#include "hw_memmap.h"
#include "hw_types.h"
#include "uart_if.h"
#include "gpio.h"


#define FAILURE                 -1
#define SUCCESS                 0

#define MOISTURE_SENSOR_PIN     PIN_58

// #define DEBUG_MOISTURESENSOR


#ifdef __cplusplus
extern "C"
{
#endif

void setupAdc();
int analogReadMoisture(float *moisture);



#ifdef __cplusplus
}
#endif

#endif //  __MOISTURE_SENSOR__
