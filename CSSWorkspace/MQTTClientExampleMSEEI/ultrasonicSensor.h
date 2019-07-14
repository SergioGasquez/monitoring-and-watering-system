#ifndef __ULTRASONIC_SENSOR__
#define __ULTRASONIC_SENSOR__

#include<stdint.h>
#include "pin.h"
#include "hw_memmap.h"
#include "hw_types.h"
#include "timer.h"
#include "hw_timer.h"
#include "gpio.h"
#include "uart_if.h"
#include "rom_map.h"

#define ECHO_PIN PIN_59
#define TRIGGER_PIN

#ifdef __cplusplus
extern "C"
{
#endif

void inputInt();
uint16_t analogReadWaterLevel();



#ifdef __cplusplus
}
#endif

#endif //  __ULTRASONIC_SENSOR__
