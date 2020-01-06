#include "moistureSensor.h"


void setupAdc()
{
    PinTypeADC(MOISTURE_SENSOR_PIN, 0xFF);
    ADCChannelEnable(ADC_BASE, ADC_CH_1);
    ADCTimerConfig(ADC_BASE, 2^17);
    ADCTimerEnable(ADC_BASE);
    ADCEnable(ADC_BASE);
}

int analogReadMoisture(float *moisture)
{
    uint64_t measure = 0;
    uint32_t ulSample = 0;
    uint8_t i = 0;;
    for( i = 0; i <= 63; i++)
    {
        if( ADCFIFOLvlGet(ADC_BASE, ADC_CH_1) )
        {
            ulSample = ADCFIFORead(ADC_BASE, ADC_CH_1);
        }
        ulSample = ulSample  & 0x3FFF; //  Los bits en los que devuelve la respuesta son del 13 al 2. Por eso esta mascara, para qutiarse los de arriba
        ulSample = ulSample >>2;        // Quitamos los 2 primeros bits
        measure += ulSample;
        osi_Sleep(100);
    }

    measure = measure >> 6;
    *moisture = measure * (1.467/4096);

    // 0.50 V cuando esta en agua
    // 0.20 V cuando no tiene humedad
    //*moisture = *moisture - 0.20;
    //*moisture = 100*(*moisture)/0.3;

    /*
    if(*moisture < 0 || *moisture > 1.5)
    {
        return FAILURE;
    }
    else
    {
#ifdef DEBUG_MOISTURESENSOR
        Report("\n Measure ave value = %d  \n" , measure);
        Report("\n moisture value = %d  \n" , ulSample);
        Report("\n moisture voltage = %f  \n" , *moisture);
        //Report("\n Percentage = %f  \n" , ((float)100 - ((*moisture*100))));
#endif //DEBUG_MOISTURESENSOR
        //*moisture =  (float)10 + ((*moisture*100));
        return SUCCESS;
    }
    */
    return FAILURE;
}
