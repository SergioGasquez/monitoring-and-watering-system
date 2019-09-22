#ifndef __WATER_LEVEL_SENSOR__
#define __WATER_LEVEL_SENSOR__

/************************************************/
/*                    PINOUT                    */
/*
 *    HC-SR04                   CC3200
 *    ----------------------------------
 *    GND                       GND
 *    Echo                      PIN_08
 *    Trigger                   PIN_64
 *    VCC                       VCC
 */


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
#include "portmacro.h"
#include "projdefs.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "osi.h"
#include "hw_ints.h"
#include "prcm.h"



//#define DEBUG_WATERLEVELSENSOR

extern volatile unsigned long timerValue;

static  EventGroupHandle_t waterLevelMeasures;
#define ECHO_DONE          0x0001
#define ECHO_WAIT          0x0002


#ifdef __cplusplus
extern "C"
{
#endif

void setupWaterLevel();
float readWaterLevel();
void echoInt();


#ifdef __cplusplus
}
#endif

#endif //  __WATER_LEVEL_SENSOR__
