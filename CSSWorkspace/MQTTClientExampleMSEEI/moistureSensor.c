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

    uint32_t ulSample;
    //uint64_t measure;
    if( ADCFIFOLvlGet(ADC_BASE, ADC_CH_2) )
    {
        ulSample = ADCFIFORead(ADC_BASE, ADC_CH_2);
    }
    ulSample = ulSample  & 0x3FFF;
    ulSample = ulSample >>2;
    return ulSample;
}
