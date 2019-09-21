#include "waterLevelSensor.h"

void setupWaterLevel()
{
    waterLevelMeasures = xEventGroupCreate();
    if( waterLevelMeasures == NULL )
    {
            while(1);
    }

#ifdef CODE1
    // Configure Timer
    MAP_TimerConfigure(TIMERA2_BASE, TIMER_CFG_PERIODIC_UP);
    MAP_TimerEnable(TIMERA2_BASE,TIMER_A);

    // Configure Echo Pin
    MAP_GPIOIntEnable(GPIOA2_BASE,PIN_08);
    MAP_GPIOIntTypeSet(GPIOA2_BASE, PIN_08,GPIO_BOTH_EDGES);
    IntPrioritySet(INT_GPIOA2, configMAX_SYSCALL_INTERRUPT_PRIORITY);
    MAP_TimerIntRegister(TIMERA2_BASE,TIMER_A,echoInt);
    MAP_GPIOIntRegister(GPIOA2_BASE,echoInt);
#endif //CODE1

#ifdef CODE2

    //
    // Pongo el pin en modo timer
    //
    MAP_PinTypeTimer(PIN_05, PIN_MODE_12);

    //
    // Enable pull down  -Configure Pin output drive strength and Type
    //
    MAP_PinConfigSet(PIN_05, PIN_STRENGTH_6MA, PIN_TYPE_STD_PD);


    IntPrioritySet(INT_TIMERA2A, configMAX_SYSCALL_INTERRUPT_PRIORITY);     // Fijar la prioridad del timer que llama a las interrupciones

    //
    // Register timer interrupt hander
    //
    MAP_TimerIntRegister(TIMERA2_BASE,TIMER_A,TimerIntHandler);
    //
    // Configure the timer in edge count mode
    //
    MAP_TimerConfigure(TIMERA2_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME));
    //
    // Set the detection edge
    //
    MAP_TimerControlEvent(TIMERA2_BASE,TIMER_A,TIMER_EVENT_BOTH_EDGES);
    //
    // Set the reload value
    //
    MAP_TimerLoadSet(TIMERA2_BASE,TIMER_A,0xffff);
    //
    // Enable capture event interrupt
    //
    MAP_TimerIntEnable(TIMERA2_BASE,TIMER_CAPA_EVENT);
    //
    // Enable Timer
    //
    MAP_TimerEnable(TIMERA2_BASE,TIMER_A);

#endif //CODE2

}


uint32_t readWaterLevel()
{
    const double temp = 1.0/80.0; // 80 MHz
    double distance;
    if(echowait != 1)
    {
        //Se lanza el pulso de 10uS
        MAP_GPIOPinWrite(GPIOA1_BASE, PIN_64,PIN_64);
        UtilsDelay(266);                                // 266/80 Mhz * 3 ticks por ciclo = 10uS .No hay funcion en la osal para hacer espera no-bloqueante en us
        MAP_GPIOPinWrite(GPIOA1_BASE, PIN_64, ~PIN_64);

        // Se espera a que se termiel el pulso
        xEventGroupWaitBits(waterLevelMeasures, ECHO_DONE, pdFALSE, pdFALSE, portMAX_DELAY);                         // Funcion bloqueante que espera a que se active el flag,no resetea los flags al salir y no espera todos los flags. Espera infinto
        distance = timerValue/80.0;     // Pasamos a us
        distance = distance/58.0;       // Pasamos a cm
        #ifdef DEBUG_WATERLEVELSENSOR
        Report("\n Distance = %f cm \t " , (float)distance);
        Report("Timer Value = %lu \n\r" ,  timerValue);
#endif // DEBUG_WATERLEVELSENSOR
        xEventGroupClearBits(waterLevelMeasures, ECHO_DONE); // Limpiar flags
    }
}

#ifdef CODE1
void echoInt()
{
    BaseType_t xHigherPriorityTaskWoken=pdFALSE;
    UtilsDelay(8000);                                 // Debouncer
    MAP_GPIOIntClear(GPIOA2_BASE,PIN_08);             // Limpia flags de int
    if (GPIOPinRead(GPIOA2_BASE,PIN_08) == 2)         // Flanco de subida
    {
        HWREG(TIMERA2_BASE + TIMER_O_TAV ) = 0;       // Pone a 0 el timer
        TimerEnable(TIMERA2_BASE,TIMER_A);            // Habilita el timer
        echowait=1;
    }
    else                                              // Flanco de bajada
    {
        if(echowait)
        {
            timerValue = TimerValueGet(TIMERA2_BASE,TIMER_A);    // Coger valor del timer
            TimerDisable(TIMERA2_BASE,TIMER_A);                  // Deshabilita el timer
            xEventGroupSetBitsFromISR(waterLevelMeasures, ECHO_DONE, &xHigherPriorityTaskWoken);        // Levantar flag de pulso capturado
            echowait=0;
        }
    }
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );         // Miramos si hay otra tarea de mayor prioridad
}
#endif CODE1


#ifdef CODE2
static void TimerIntHandler()
{
    BaseType_t xHigherPriorityTaskWoken=pdFALSE;

    //
    // Clear the interrupt
    //
    MAP_TimerIntClear(TIMERA2_BASE,TIMER_CAPA_EVENT);

    //
    // Get the samples and compute the frequency
    //
    timerValue1 = timerValue2;
    timerValue2 = MAP_TimerValueGet(TIMERA2_BASE,TIMER_A);
    if(echowait)
    {
        xEventGroupSetBitsFromISR(waterLevelMeasures, ECHO_DONE, &xHigherPriorityTaskWoken);        // Levantar flag de pulso capturado
        echowait = 0;
        timerValue = ((timerValue1 - timerValue2));
    }
    else
    {
        timerValue = 0;
    }
    echowait = 1;

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );         // Miramos si hay otra tarea de mayor prioridad
}
#endif CODE2
