#ifndef __WATER_LEVEL_SENSOR__
#define __WATER_LEVEL_SENSOR__

#include<stdint.h>
#include "pin.h"
#include "gpio.h"
#include "adc.h"
#include "hw_memmap.h"
#include "hw_types.h"
#include "uart_if.h"
#include "timer.h"
#include "hw_timer.h"
#include "rom_map.h"
extern volatile int pulse;
extern int echowait;
#define ECHO_PIN PIN_08
#define TRIGGER_PIN PIN_64

#ifdef __cplusplus
extern "C"
{
#endif

void setupWaterLevel();
uint32_t readWaterLevel();
void inputInt();


#ifdef __cplusplus
}
#endif

#endif //  __WATER_LEVEL_SENSOR__
