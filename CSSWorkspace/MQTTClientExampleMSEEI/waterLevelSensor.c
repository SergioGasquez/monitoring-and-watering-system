#include "waterLevelSensor.h"


void setupWaterLevel()
{
    MAP_TimerConfigure(TIMERA2_BASE, TIMER_CFG_PERIODIC_UP);
    MAP_TimerEnable(TIMERA2_BASE,TIMER_A);

    MAP_GPIOIntEnable(GPIOA2_BASE,PIN_08);
    MAP_GPIOIntTypeSet(GPIOA2_BASE, PIN_08,GPIO_BOTH_EDGES);
    MAP_TimerIntRegister(TIMERA2_BASE,TIMER_A,inputInt);
    MAP_GPIOIntRegister(GPIOA2_BASE,inputInt);

}

uint32_t readWaterLevel()
{
//    TickType_t xLastWakeTime;
    const double temp = 1.0/80.0; // 80 MHZ ?
    //Checks if a pulse read is in progress
//    if(waterLevelMeasures != 0x01 )
    if(echowait!=1)
    {
        //Does the required pulse of 10uS
        MAP_GPIOPinWrite(GPIOA1_BASE, PIN_64,PIN_64);           // Pone en el pin en alto
//        xLastWakeTime=xTaskGetTickCount();
//        vTaskDelayUntil(&xLastWakeTime,266); ///0.00001*configTICK_RATE_HZ
        UtilsDelay(266);                                        // Se espera 10uS (creo que de forma activa--> MALO)  1/80 Mhz * 3 ticks por ciclo  * 266 = 10 uS
        MAP_GPIOPinWrite(GPIOA1_BASE, PIN_64, ~PIN_64);         // Pone en el pin en bajo

        /*
                This makes the code wait for a reading to finish
                You can omit this part if you want the code to be non-blocking but
                reading is only ready when echowait=0.
         */
        while(echowait != 0);           // CODIGO BLOQUEANTE, SE DEBERIA USAR UN FLAG, por ejemplo
//        xEventGroupWaitBits(waterLevelMeasures, WAITING_ECHO,pdFALSE,pdFALSE,portMAX_DELAY);                         // Funcion bloqueante que espera a que se active el flag,no resetea los flags al salir y no espera todos los flags. Espera infinto

        float percentage =0.0;
        int ad;
        float total =40.0;

        //Converts the counter value to cm. TempValueC = (uint32_t)(147.5 - ((75.0*3.3 *(float)ADCValues[0])) / 4096.0);
        pulse =(uint32_t)(temp * pulse);        // Supongo que lo pasa de ticks a tiempo
        pulse = pulse / (340/2); // 58?

        //Percentage calculatin
        percentage =(float)(pulse/total);
        percentage= percentage*100;
        ad=(int)percentage;


        //Prints out the distance measured.
        Report("distance = %2dcm \n\r" , pulse);
//        Report("distance = %2dcm \n\r" , ad);
//        Report("Percentage = %f -/- \n\r" , percentage);
    }
//    xLastWakeTime=xTaskGetTickCount();
//    vTaskDelayUntil(&xLastWakeTime,3*configTICK_RATE_HZ);
    UtilsDelay(8000000);            // Espera 3 secs
    return 0; //ulSample;
}

void inputInt()
{
    //Clear interrupt flag. Since we only enabled on this is enough
    MAP_GPIOIntClear(GPIOA2_BASE,PIN_08);

    /*
    If it's a rising edge then set he timer to 0
    It's in periodic mode so it was in some random value
     */
    if (GPIOPinRead(GPIOA2_BASE,PIN_08) == 2)          // If risign edge
    {
        HWREG(TIMERA2_BASE + TIMER_O_TAV ) = 0; //Loads value 0 into the timer.

        long ad = MAP_TimerLoadGet(TIMERA2_BASE,TIMER_A);           // Gets the timer load value. Debe ser 0?
        TimerEnable(TIMERA2_BASE,TIMER_A);                          // Activa el timer
//        xEventGroupSetBitsFromISR(waterLevelMeasures, WAITING_ECHO, ( ( BaseType_t ) 0 ));
        echowait=1;                                                 // Esperamos un echo
    }
    /*
    If it's a falling edge that was detected, then get the value of the counter
     */
    else
    {
        pulse = TimerValueGet(TIMERA2_BASE,TIMER_A);            // Gets the current timer value.
        long af = GPIOPinRead(GPIOA2_BASE,PIN_08);              // Lee el pin
        //Report("pin = %dcm \n\r" , af);
        // Report("distance = %dcm \n\r" , pulse);//record value
        TimerDisable(TIMERA2_BASE,TIMER_A);                     // Desactiva el timer
        //xEventGroupClearBitsFromISR(waterLevelMeasures, WAITING_ECHO);
        echowait=0;                                             // Ya no esperamos un echo
    }


}
