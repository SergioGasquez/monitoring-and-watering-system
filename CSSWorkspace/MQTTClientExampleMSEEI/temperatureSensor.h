#ifndef __TEMPERATURE_SENSOR__
#define __TEMPERATURE_SENSOR__

/************************************************/
/*                    PINOUT                    */
/*
 *    SHT31                   CC3200
 *    ----------------------------------
 *    VIN                       VCC
 *    GND                       GND
 *    SCL                       PIN_01
 *    SDA                       PIN_02
 */

#include "i2c_if.h"
#include "stdint.h"
#include "FreeRTOSConfig.h"
#include "portmacro.h"
#include "math.h"
#include "stdbool.h"

// Debug
//#define DEBUG_TEMPERATURESENSOR
// Pins
#define HARD_RESET_PIN
// Address
#define TEMPERATURE_DEV_ADDR 0x44 //(addrs pin 2 conected to Vss)

// Commands
#define SOFT_RESET 0x30A2
#define HEATER_ON 0x306D
#define HEATER_OFF 0x3066
#define READ_STATUS_REGISTER 0xF32D
#define CLEAR_STATUS_REGISTER 0x3041

// Modes
#define HIGHREP_CLOCKSTRECH 0x2C06
#define MEDREP_CLOCKSTRECH 0x2C0D
#define LOWREP_CLOCKSTRECH 0x2C10
#define HIGHREP_NOCLOCKSTRECH 0x2400
#define MEDREP_NOCLOCKSTRECH 0x240B
#define LOWREP_NOCLOCKSTRECH 0x2416


// Results
#define FAILURE                 -1
#define SUCCESS                 0
#define RET_IF_ERR(Func)        {int iRetVal = (Func); \
                                 if (SUCCESS != iRetVal) \
                                     return  iRetVal;}
/*
typedef enum SHT3xMode
{
    Single_HighRep_ClockStretch,
    Single_MediumRep_ClockStretch,
    Single_LowRep_ClockStretch,
    Single_HighRep_NoClockStretch,
    Single_MediumRep_NoClockStretch,
    Single_LowRep_NoClockStretch
}SHT3xModes;
*/

#ifdef __cplusplus
extern "C"
{
#endif

//void setMode(SHT3xMode mode);
void setupTemperatureSensor();
int getTemperature(float *temperature, float *humidity);
bool crcCheck(unsigned char MSB, unsigned char LSB, unsigned char CRC);


#ifdef __cplusplus
}
#endif

#endif //  __TEMPERATURE_SENSOR__
