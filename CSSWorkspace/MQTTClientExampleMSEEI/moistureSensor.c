#include "moistureSensor.h"


void setupAdc()
{
    PinTypeADC(MOISTURE_SENSOR_PIN, 0xFF);
    ADCChannelEnable(ADC_BASE, ADC_CH_2);
    ADCTimerConfig(ADC_BASE, 2^17);
    ADCTimerEnable(ADC_BASE);
    ADCEnable(ADC_BASE);
}

uint32_t analogReadMoisture()
{
    uint64_t measure = 0;
    uint32_t ulSample = 0;
    uint8_t i;
    for (i = 0; i <100 ;i++)
    {
        if( ADCFIFOLvlGet(ADC_BASE, ADC_CH_2) )
        {
            ulSample = ADCFIFORead(ADC_BASE, ADC_CH_2);
        }
        ulSample = ulSample  & 0x3FFF; //  Los bits en los que devuelve la respuesta son del 13 al 2. Por eso esta mascara, para qutiarse los de arriba
        ulSample = ulSample >>2;        // Quitamos los 2 primeros bits
        measure += ulSample;
    }
    measure = measure/10;       // Duda
    ulSample = measure;
    return ulSample;
}
