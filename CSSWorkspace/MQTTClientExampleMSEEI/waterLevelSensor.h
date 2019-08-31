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
#include "event_groups.h"               // Libreria de grupos de eventos
#include "task.h"
#include "osi.h"

static  EventGroupHandle_t waterLevelMeasures;
#define WAITING_ECHO          0x0001

extern int echowait ;
extern volatile int pulse;

//#define DEFAULT_CODE // El de la web
//#define CODE_1        // El de la web cambiado un poco, cambio de tiempos y conversiones
#define CODE_2          // El de la web sin codigo bloqueante

//#define ECHO_PIN PIN_08
//#define TRIGGER_PIN PIN_64

#ifdef __cplusplus
extern "C"
{
#endif

void setupWaterLevel();
uint32_t readWaterLevel();
void echoInt();


#ifdef __cplusplus
}
#endif

#endif //  __WATER_LEVEL_SENSOR__
