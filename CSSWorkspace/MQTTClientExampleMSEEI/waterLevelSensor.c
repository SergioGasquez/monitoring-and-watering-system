#include "waterLevelSensor.h"

void setupWaterLevel()
{
    waterLevelMeasures = xEventGroupCreate();
    if( waterLevelMeasures == NULL )
    {
            while(1);
    }

    // Configure Timer
    MAP_TimerConfigure(TIMERA2_BASE, TIMER_CFG_PERIODIC_UP);
    MAP_TimerEnable(TIMERA2_BASE,TIMER_A);

    // Configure Echo Pin
    MAP_GPIOIntEnable(GPIOA2_BASE,PIN_08);
    MAP_GPIOIntTypeSet(GPIOA2_BASE, PIN_08,GPIO_BOTH_EDGES);
    IntPrioritySet(INT_GPIOA2, configMAX_SYSCALL_INTERRUPT_PRIORITY);
    MAP_TimerIntRegister(TIMERA2_BASE,TIMER_A,echoInt);
    MAP_GPIOIntRegister(GPIOA2_BASE,echoInt);
    xEventGroupClearBits(waterLevelMeasures, ECHO_DONE | ECHO_WAIT);
}


float readWaterLevel()
{
    double waterLevelPerventage;
    EventBits_t uxBits;
    uxBits = xEventGroupGetBits(waterLevelMeasures);
    if (!(uxBits & ECHO_WAIT))
    {
        //Se lanza el pulso de 10uS
        MAP_GPIOPinWrite(GPIOA1_BASE, PIN_64,PIN_64);
        UtilsDelay(266);                                // 266/80 Mhz * 3 ticks por ciclo = 10uS .No hay funcion en la osal para hacer espera no-bloqueante en us
        MAP_GPIOPinWrite(GPIOA1_BASE, PIN_64, ~PIN_64);

        // Se espera a que se termiel el pulso
        xEventGroupWaitBits(waterLevelMeasures, ECHO_DONE, pdFALSE, pdFALSE, portMAX_DELAY);                         // Funcion bloqueante que espera a que se active el flag,no resetea los flags al salir y no espera todos los flags. Espera infinto
        if (timerValue < 3000)
        {
            timerValue = 3000;
        }
        else if(timerValue > 48500)
        {
            timerValue = 48500;
        }
        waterLevelPerventage = (timerValue - 3000)*100.0/45500.0;
#ifdef DEBUG_WATERLEVELSENSOR
        Report("\n Timer Value = %lu \n" ,  timerValue);
#endif // DEBUG_WATERLEVELSENSOR
        xEventGroupClearBits(waterLevelMeasures, ECHO_DONE); // Limpiar flags
        return (float)waterLevelPerventage;
    }
}

void echoInt()
{
    BaseType_t xHigherPriorityTaskWoken=pdFALSE;
    MAP_GPIOIntClear(GPIOA2_BASE,PIN_08);             // Limpia flags de int
    if (GPIOPinRead(GPIOA2_BASE,PIN_08) == 2)         // Flanco de subida
    {
        HWREG(TIMERA2_BASE + TIMER_O_TAV ) = 0;       // Pone a 0 el timer
        TimerEnable(TIMERA2_BASE,TIMER_A);            // Habilita el timer
        xEventGroupSetBitsFromISR(waterLevelMeasures, ECHO_WAIT, &xHigherPriorityTaskWoken);        // Levantar flag de pulso capturado
    }
    else                                              // Flanco de bajada
    {
        EventBits_t uxBits;
        uxBits = xEventGroupGetBitsFromISR(waterLevelMeasures);
        if((uxBits & ECHO_WAIT))
        {
            timerValue = TimerValueGet(TIMERA2_BASE,TIMER_A);    // Coger valor del timer
            TimerDisable(TIMERA2_BASE,TIMER_A);                  // Deshabilita el timer
            xEventGroupSetBitsFromISR(waterLevelMeasures, ECHO_DONE, &xHigherPriorityTaskWoken);        // Levantar flag de pulso capturado
            xEventGroupClearBitsFromISR(waterLevelMeasures, ECHO_WAIT);
        }
    }
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );         // Miramos si hay otra tarea de mayor prioridad
}

