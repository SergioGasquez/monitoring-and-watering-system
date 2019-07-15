#ifndef __MOISTURE_SENSOR__
#define __MOISTURE_SENSOR__

#include<stdint.h>
#include "pin.h"
#include "adc.h"
#include "hw_memmap.h"
#include "hw_types.h"
#include "uart_if.h"

#define MOISTURE_SENSOR_PIN PIN_59

#ifdef __cplusplus
extern "C"
{
#endif

void setupAdc();
uint32_t analogReadMoisture();



#ifdef __cplusplus
}
#endif

#endif //  __MOISTURE_SENSOR__
