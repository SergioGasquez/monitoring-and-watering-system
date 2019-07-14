#include "moistureSensor.h"



uint16_t analogReadMoisture()
{
    uint16_t val;

    while(ADCFIFOLvlGet(ADC_BASE, ADC_CH_2)) {       // flush the channel's FIFO if not empty
        ADCFIFORead(ADC_BASE, ADC_CH_2);
    }

    PinTypeADC(MOISTURE_SENSOR_PIN,PIN_MODE_255);                  // Tenia puesto el dummy, why? Pinmodes en Cc3200-Hardware.pdf pag9/72 (Dig. Pin Mux Config Mode Value)
    ADCChannelEnable(ADC_BASE, ADC_CH_2);
    ADCTimerConfig(ADC_BASE,0x1ffff);               // Es el valor del timer. Usan 2^17 o 0x1ffff
    ADCTimerEnable(ADC_BASE);
    ADCEnable(ADC_BASE);

    while(!ADCFIFOLvlGet(ADC_BASE, ADC_CH_2));
    val = ADCFIFORead(ADC_BASE, ADC_CH_2) & 0x3FFF;  // & 0x3FFF <- Mascara para quitar los 2 bits altos. Deben contener el time stamp o otras cosas?

    ADCDisable(ADC_BASE);
    ADCChannelDisable(ADC_BASE, ADC_CH_2);
    ADCTimerDisable(ADC_BASE);

    val = val >> 2;                                 // Note that the bits 13:2 are the actual ADC codes. Estamos quitando los bits 0 y 1
    return val;
}
