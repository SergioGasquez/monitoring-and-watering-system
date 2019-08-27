#include "waterLevelSensor.h"

void setupWaterLevel()
{
    // Configure Timer
    MAP_TimerConfigure(TIMERA2_BASE, TIMER_CFG_PERIODIC_UP);
    MAP_TimerEnable(TIMERA2_BASE,TIMER_A);

    // Configure Echo Pin
    MAP_GPIOIntEnable(GPIOA2_BASE,PIN_08);
    MAP_GPIOIntTypeSet(GPIOA2_BASE, PIN_08,GPIO_BOTH_EDGES);
    MAP_TimerIntRegister(TIMERA2_BASE,TIMER_A,echoInt);
    MAP_GPIOIntRegister(GPIOA2_BASE,echoInt);

}
#ifdef DEFAULT_CODE
uint32_t readWaterLevel()
{
    const double temp = 1.0/80.0; // 80 MHZ ?
    if(echowait != 1)
    {
        //Does the required pulse of 10uS
        MAP_GPIOPinWrite(GPIOA1_BASE, PIN_64,PIN_64);
        UtilsDelay(266);
        MAP_GPIOPinWrite(GPIOA1_BASE, PIN_64, ~PIN_64);
        while(echowait != 0);
        float percentage =0.0;
        int ad;
        float total =40.0;
        pulse =(uint32_t)(temp * pulse);
        pulse = pulse / 58;
        percentage =(float)(pulse/total);
        percentage= percentage*100;
        ad=(int)percentage;
        Report("distance = %2dcm \n\r" , pulse);
        Report("distance = %2dcm \n\r" , ad);
        Report("Percentage = %f -/- \n\r" , percentage);
    }
    UtilsDelay(8000000);
}
void inputInt()
{
  MAP_GPIOIntClear(GPIOA2_BASE,PIN_08);
  if (GPIOPinRead(GPIOA2_BASE,PIN_08) == 2)
  {
      HWREG(TIMERA2_BASE + TIMER_O_TAV ) = 0; //Loads value 0 into the timer.
      long ad = MAP_TimerLoadGet(TIMERA2_BASE,TIMER_A);
      TimerEnable(TIMERA2_BASE,TIMER_A);
      echowait=1;
  }
  else
  {
    pulse = TimerValueGet(TIMERA2_BASE,TIMER_A);
    long af = GPIOPinRead(GPIOA2_BASE,PIN_08);
    TimerDisable(TIMERA2_BASE,TIMER_A);
    echowait=0;
  }
}
#endif // DEFAULT_CODE

#ifdef CODE_1
uint32_t readWaterLevel()
{
    const double temp = 1.0/80.0; // 80 MHZ ?
    //Checks if a pulse read is in progress
    if(echowait!=1)
    {
        //Does the required pulse of 10uS
        MAP_GPIOPinWrite(GPIOA1_BASE, PIN_64,PIN_64);           // Pone en el pin en alto
        UtilsDelay(266);                                        // Se espera 10uS (creo que de forma activa--> MALO)  1/80 Mhz * 3 ticks por ciclo  * 266 = 10 uS
        MAP_GPIOPinWrite(GPIOA1_BASE, PIN_64, ~PIN_64);         // Pone en el pin en bajo


        //        This makes the code wait for a reading to finish
        //        You can omit this part if you want the code to be non-blocking but
        //        reading is only ready when echowait=0.

        while(echowait != 0);           // CODIGO BLOQUEANTE, SE DEBERIA USAR UN FLAG, por ejemplo

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
        Report("distance = %2dcm \n\r" , ad);
        Report("Percentage = %f -/- \n\r" , percentage);
    }
    UtilsDelay(8000000);            // Espera 3 secs
    return 0; //ulSample;
}

void echoInt()
{
    //Clear interrupt flag. Since we only enabled on this is enough
    MAP_GPIOIntClear(GPIOA2_BASE,PIN_08);


   // If it's a rising edge then set he timer to 0
    //It's in periodic mode so it was in some random value

    if (GPIOPinRead(GPIOA2_BASE,PIN_08) == 2)          // If risign edge
    {
        HWREG(TIMERA2_BASE + TIMER_O_TAV ) = 0; //Loads value 0 into the timer.
        long ad = MAP_TimerLoadGet(TIMERA2_BASE,TIMER_A);           // Gets the timer load value. Debe ser 0?
        TimerEnable(TIMERA2_BASE,TIMER_A);                          // Activa el timer
        echowait=1;                                                 // Esperamos un echo
    }
    //    If it's a falling edge that was detected, then get the value of the counter
    else
    {
        pulse = TimerValueGet(TIMERA2_BASE,TIMER_A);            // Gets the current timer value.
        long af = GPIOPinRead(GPIOA2_BASE,PIN_08);              // Lee el pin
        TimerDisable(TIMERA2_BASE,TIMER_A);                     // Desactiva el timer
        echowait=0;                                             // Ya no esperamos un echo
    }
}

#endif // CODE_1

#ifdef CODE_2
uint32_t readWaterLevel()
{
    TickType_t xLastWakeTime;
    const double temp = 1.0/80.0; // 80 MHZ ?
    //Checks if a pulse read is in progress
    if(waterLevelMeasures != WAITING_ECHO )     // Not waiting echo
    {
        //Does the required pulse of 10uS
        MAP_GPIOPinWrite(GPIOA1_BASE, PIN_64,PIN_64);           // Pone en el pin en alto
        xLastWakeTime=xTaskGetTickCount();
        vTaskDelayUntil(&xLastWakeTime,266);                    // 0.00001*configTICK_RATE_HZ
        MAP_GPIOPinWrite(GPIOA1_BASE, PIN_64, ~PIN_64);         // Pone en el pin en bajo

        // Wait echo
        xEventGroupWaitBits(waterLevelMeasures, WAITING_ECHO,pdFALSE,pdFALSE,portMAX_DELAY);                         // Funcion no bloqueante que espera a que se active el flag,no resetea los flags al salir y no espera todos los flags. Espera infinto

        float percentage =0.0;
        int ad;
        float total =40.0;

        //Converts the counter value to cm. TempValueC = (uint32_t)(147.5 - ((75.0*3.3 *(float)ADCValues[0])) / 4096.0);
        pulse =(uint32_t)(temp * pulse);        // Supongo que lo pasa de ticks a tiempo
        //pulse = pulse / (340/2); // 58?
        pulse = pulse / 58;

        //Percentage calculatin
        percentage =(float)(pulse/total);
        percentage= percentage*100;
        ad=(int)percentage;


        //Prints out the distance measured.
        Report("distance = %2dcm \n\r" , pulse);
        Report("distance = %2dcm \n\r" , ad);
        Report("Percentage = %f -/- \n\r" , percentage);
    }
    xLastWakeTime=xTaskGetTickCount();
    vTaskDelayUntil(&xLastWakeTime,3*configTICK_RATE_HZ);
    return 0; //ulSample;
}

void echoInt()
{
    //Clear interrupt flag. Since we only enabled on this is enough
    MAP_GPIOIntClear(GPIOA2_BASE,PIN_08);


   // If it's a rising edge then set he timer to 0
    //It's in periodic mode so it was in some random value

    if (GPIOPinRead(GPIOA2_BASE,PIN_08) == 2)          // If risign edge
    {
        HWREG(TIMERA2_BASE + TIMER_O_TAV ) = 0; //Loads value 0 into the timer.

        long ad = MAP_TimerLoadGet(TIMERA2_BASE,TIMER_A);           // Gets the timer load value. Debe ser 0?
        TimerEnable(TIMERA2_BASE,TIMER_A);                          // Activa el timer
        xEventGroupSetBitsFromISR(waterLevelMeasures, WAITING_ECHO, ( ( BaseType_t ) 0 )); // Esperamos un echo
//        xEventGroupSetBits(waterLevelMeasures, WAITING_ECHO);
    }
    //    If it's a falling edge that was detected, then get the value of the counter
    else
    {
        pulse = TimerValueGet(TIMERA2_BASE,TIMER_A);            // Gets the current timer value.
        long af = GPIOPinRead(GPIOA2_BASE,PIN_08);              // Lee el pin
        //Report("pin = %dcm \n\r" , af);
        // Report("distance = %dcm \n\r" , pulse);//record value
        TimerDisable(TIMERA2_BASE,TIMER_A);                     // Desactiva el timer
        xEventGroupClearBitsFromISR(waterLevelMeasures, WAITING_ECHO); // Ya no esperamos un echo
//        xEventGroupClearBits(waterLevelMeasures, WAITING_ECHO);
    }
}

#endif // CODE_2
