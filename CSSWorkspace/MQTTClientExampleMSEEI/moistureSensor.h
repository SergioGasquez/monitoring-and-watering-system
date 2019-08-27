#ifndef __MOISTURE_SENSOR__
#define __MOISTURE_SENSOR__

#include<stdint.h>
#include "pin.h"
#include "adc.h"
#include "hw_memmap.h"
#include "hw_types.h"
#include "uart_if.h"

//#define AVERAGE
#define SINGLE_MEASURE

#define FAILURE                 -1
#define SUCCESS                 0

#define MOISTURE_SENSOR_PIN PIN_59

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
