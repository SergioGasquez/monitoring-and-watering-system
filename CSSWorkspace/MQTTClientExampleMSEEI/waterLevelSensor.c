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
    const double temp = 1.0/80.0;
    //Checks if a pulse read is in progress
    if(echowait != 1)
    {
        //Does the required pulse of 10uS
        MAP_GPIOPinWrite(GPIOA1_BASE, PIN_64,PIN_64);
        UtilsDelay(266);
        MAP_GPIOPinWrite(GPIOA1_BASE, PIN_64, ~PIN_64);

        /*
                This makes the code wait for a reading to finish
                You can omit this part if you want the code to be non-blocking but
                reading is only ready when echowait=0.
         */
        while(echowait != 0);

        float percentage =0.0;
        int ad;
        float total =40.0;

        //Converts the counter value to cm. TempValueC = (uint32_t)(147.5 - ((75.0*3.3 *(float)ADCValues[0])) / 4096.0);
        //Report("distance = %dcm \n" , pulse);
        pulse =(uint32_t)(temp * pulse);
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
    UtilsDelay(8000000);

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
    if (GPIOPinRead(GPIOA2_BASE,PIN_08) == 2){
        HWREG(TIMERA2_BASE + TIMER_O_TAV ) = 0; //Loads value 0 into the timer.

        // TimerLoadSet(TIMERA2_BASE,TIMER_A,0xFFFFFFFF);
        long ad = MAP_TimerLoadGet(TIMERA2_BASE,TIMER_A);
        //Report("load = %dcm \n\r" , ad);
        TimerEnable(TIMERA2_BASE,TIMER_A);
        echowait=1;
    }
    /*
    If it's a falling edge that was detected, then get the value of the counter
     */
    else{
        pulse = TimerValueGet(TIMERA2_BASE,TIMER_A);
        long af = GPIOPinRead(GPIOA2_BASE,PIN_08);
        //Report("pin = %dcm \n\r" , af);
        // Report("distance = %dcm \n\r" , pulse);//record value
        TimerDisable(TIMERA2_BASE,TIMER_A);
        echowait=0;
    }


}
