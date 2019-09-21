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

//#define CODE1
#define CODE2

#define DEBUG_WATERLEVELSENSOR

extern int echowait ;
extern volatile int pulse;
extern volatile unsigned long timerValue;

//#ifdef CODE2
extern volatile unsigned long timerValue1;
extern volatile unsigned long timerValue2;
///#endif // CODE2



static  EventGroupHandle_t waterLevelMeasures;
#define ECHO_DONE          0x0001


#ifdef __cplusplus
extern "C"
{
#endif

void setupWaterLevel();
uint32_t readWaterLevel();
void echoInt();
#ifdef CODE2
static void TimerIntHandler();
#endif // CODE2

#ifdef __cplusplus
}
#endif

#endif //  __WATER_LEVEL_SENSOR__
