//*****************************************************************************
// Copyright (C) 2016 Texas Instruments Incorporated
//
// All rights reserved. Property of Texas Instruments Incorporated.
// Restricted rights to use, duplicate or disclose this code are
// granted through contract.
// The program may not be used without the written permission of
// Texas Instruments Incorporated or against the terms and conditions
// stipulated in the agreement under which this program has been supplied,
// and under no circumstances can it be used with non-TI connectivity device.
//
//*****************************************************************************
//*****************************************************************************
// Modificado, adaptado y ampliado por Jose Manuel Cano, Eva Gonzalez, Ignacio Herrero
// Departamento de Tecnologia Electronica
// Universidad de Malaga
//*****************************************************************************
//
//*****************************************************************************

//*****************************************************************************
//

//! \addtogroup mqtt_client
//! @{
//
//*****************************************************************************

#include<stdint.h>
#include<stdbool.h>

// Standard includes
#include <stdlib.h>

// simplelink includes
#include "simplelink.h"

// driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "interrupt.h"
#include "rom_map.h"
#include "prcm.h"
#include "uart.h"
#include "gpio.h"
#include "pin.h"
#include "timer.h"
#include "adc.h"
//#include "queue.h"

// common interface includes
#include "network_if.h"
#ifndef NOTERM
#include "uart_if.h"
#endif

#include "button_if.h"
#include "gpio_if.h"
#include "timer_if.h"
#include "common.h"
#include "utils.h"


#include "utils/uartstdio.h"
#include "i2c_if.h"
#include "pwm_if.h"

//Sensores
#include "tmp006drv.h"
#include "bma222drv.h"
#include "moistureSensor.h"
#include "waterLevelSensor.h"
#include "temperatureSensor.h"

//FreeRTOS
#include"FreeRTOS.h"

#include "event_groups.h"               // Libreria de grupos de eventos
#include "task.h"
#include "queue.h"

#include "sl_mqtt_client.h"

// application specific includes
#include "pinmux.h"

#include "frozen.h" //Cabecera JSON

#define APPLICATION_VERSION     "1.1.1"


#define DEBUG

/*Operate Lib in MQTT 3.1 mode.*/
#define MQTT_3_1_1              false /*MQTT 3.1.1 */
#define MQTT_3_1                true /*MQTT 3.1*/

#define WILL_TOPIC              "Client"
#define WILL_MSG                "Client Stopped"
#define WILL_QOS                QOS2
#define WILL_RETAIN             false

/*Defining Broker IP address and port Number*/
#define SERVER_ADDRESS          "192.168.1.38"      // IP del PC (Broker MQTT)
#define PORT_NUMBER             1883

#define MAX_BROKER_CONN         1

#define SERVER_MODE             MQTT_3_1
/*Specifying Receive time out for the Receive task*/
#define RCV_TIMEOUT             30

/*Background receive task priority*/
#define TASK_PRIORITY           3

/* Keep Alive Timer value*/
#define KEEP_ALIVE_TIMER        25

/*Clean session flag*/
#define CLEAN_SESSION           true

/*Retain Flag. Used in publish message. */
#define RETAIN                  1

/*Topics a los que va a publicar*/
#define PUB_TOPIC_SENSORS  "/cc3200/Station1"
#define PUB_TOPIC_LOSANT    "losant/5d8236ee68f6a70006d4a026/state"

/*Numero de topics a los que se va a subscribir*/
#define TOPIC_COUNT             2

/*Defining Subscription Topic Values*/
#define TOPIC_PC                "/PC"
#define TOPIC_LOSANT            "losant/5d8236ee68f6a70006d4a026/command"


#define RELAY_PIN PIN_59



/*Defining QOS levels*/
#define QOS0                    0
#define QOS1                    1
#define QOS2                    2

/*Spawn task priority and OSI Stack Size*/
#define OSI_STACK_SIZE          2048
#define UART_PRINT              Report


typedef struct connection_config{
    SlMqttClientCtxCfg_t broker_config;
    void *clt_ctx;
    unsigned char *client_id;
    unsigned char *usr_name;
    unsigned char *usr_pwd;
    bool is_clean;
    unsigned int keep_alive_time;
    SlMqttClientCbs_t CallBAcks;
    int num_topics;
    char *topic[TOPIC_COUNT];
    unsigned char qos[TOPIC_COUNT];
    SlMqttWill_t will_params;
    bool is_connected;
}connect_config;

typedef enum events
{
    SENSORS_REPORT,
    WATER_REPORT,
    BROKER_DISCONNECTION
}osi_messages;

#ifdef USE_FREERTOS
void vUARTTask( void *pvParameters );
#endif


/////////////////////////////////////////////////////

volatile unsigned long timerValue = 0;

static QueueHandle_t freqQueue;             // Cola que almacena la frecuencia con la que se mide
static QueueHandle_t thresholdsQueue;       // Cola que almacena los thresholds
static QueueHandle_t waterQueue;            // Cola que almacena el tiempo que se riega

static EventGroupHandle_t measurementFlag;  // Grupo de eventos que controla cuando se debe medir
#define MEASUREMENT_START 0x0001

static EventGroupHandle_t waterFlag;        // Grupo de eventos que controla cuando se debe regar
#define WATERING_START 0x0001



typedef struct {                                    // Struct para los umbrales que activan el riego
    char temperature;
    char moisture;
}systemThresholds;

typedef struct {                                    // Struct para almacenar los valores del sensor de ambiente
    float temperature;
    float humidity;
}sht31Data;
typedef struct {                                    // Struct para almacenar los envios
    sht31Data sht31Data_;
    float moisture_;
}sensorsData;

////////////////////////////////////////////////////

//*****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES
//*****************************************************************************
static void
Mqtt_Recv(void *app_hndl, const char  *topstr, long top_len, const void *payload,
          long pay_len, bool dup,unsigned char qos, bool retain);
static void sl_MqttEvt(void *app_hndl,long evt, const void *buf,
                       unsigned long len);
static void sl_MqttDisconnect(void *app_hndl);
void ToggleLedState(ledEnum LedNum);
void TimerPeriodicIntHandler(void);
void LedTimerConfigNStart();
void LedTimerDeinitStop();
void BoardInit(void);
static void DisplayBanner(char * AppName);
void ConnectWiFI(void *pvParameters);
void measurementsTask(void *pvParameters);
void wateringTask (void *pvParameters);


//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
#ifdef USE_FREERTOS
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#endif

unsigned short g_usTimerInts;
/* AP Security Parameters */
SlSecParams_t SecurityParams = {0};

/*Message Queue*/
OsiMsgQ_t g_PBQueue;                            // Cola de mensajes donde recibiremos los mensajes mqtt

/* connection configuration */
connect_config usr_connect_config[] =
{
    {
        {
            {
                SL_MQTT_NETCONN_URL,
                SERVER_ADDRESS,
                PORT_NUMBER,
                0,
                0,
                0,
                NULL
            },
            SERVER_MODE,
            true,
        },
        NULL,
        (unsigned char *)"user1",
        NULL,
        NULL,
        true,
        KEEP_ALIVE_TIMER,
        {Mqtt_Recv, sl_MqttEvt, sl_MqttDisconnect},
        TOPIC_COUNT,
        {TOPIC_PC,TOPIC_LOSANT},             // Nos subscribimos al topic del PC
        {QOS2},                 // Con calidad de servicio 2
        {WILL_TOPIC,WILL_MSG,WILL_QOS,WILL_RETAIN},
        false
    }
};

/* library configuration */
SlMqttClientLibCfg_t Mqtt_Client={
    1882,
    TASK_PRIORITY,
    30,
    true,
    (long(*)(const char *, ...))UART_PRINT
};

/*Conversion a char* de los topics en los que publicaremos*/
const char *pub_topic_sensors = PUB_TOPIC_SENSORS;
const char *pub_topic_losant = PUB_TOPIC_LOSANT;







void *app_hndl = (void*)usr_connect_config;
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

//****************************************************************************
//! Defines Mqtt_Pub_Message_Receive event handler.
//! Client App needs to register this event handler with sl_ExtLib_mqtt_Init 
//! API. Background receive task invokes this handler whenever MQTT Client 
//! receives a Publish Message from the broker.
//!
//!\param[out]     topstr => pointer to topic of the message
//!\param[out]     top_len => topic length
//!\param[out]     payload => pointer to payload
//!\param[out]     pay_len => payload length
//!\param[out]     retain => Tells whether its a Retained message or not
//!\param[out]     dup => Tells whether its a duplicate message or not
//!\param[out]     qos => Tells the Qos level
//!
//!\return none
//****************************************************************************

// Funcio�n que se ejecuta cuando recibimos un mensaje
static void
Mqtt_Recv(void *app_hndl, const char  *topstr, long top_len, const void *payload,
                       long pay_len, bool dup,unsigned char qos, bool retain)
{
    
    char *output_str=(char*)pvPortMalloc(top_len+1);
    memset(output_str,'\0',top_len+1);
    strncpy(output_str, (char*)topstr, top_len);
    output_str[top_len]='\0';
    if (strncmp(output_str,TOPIC_PC, top_len) == 0)                         // Si recibimos un mensaje de la interfaz de PC
    {
        systemThresholds systemThresholds_;
        uint32_t value;
        if (json_scanf((const char *)payload, pay_len, "{ measuresFreq: %d }", &value)>0)         // Si recibimos un mensaje de cambio de frecuencia de muestreo
        {
            value = value*1000*60;
            xQueueOverwrite(freqQueue,&value);
        }
        if (json_scanf((const char *)payload, pay_len, "{ measurementSwitch: %d }", &value)>0)         // Si recibimos un mensaje de pausar/reunadar las medidas
        {
            if(value)
            {
                xEventGroupSetBits(measurementFlag, MEASUREMENT_START);
            }
            else
            {
                xEventGroupClearBits(measurementFlag, MEASUREMENT_START);
            }
        }
        if (json_scanf((const char *)payload, pay_len, "{ wateringTime: %d }", &value)>0)         // Si recibimos un mensaje de cambio de tiempo de riego
        {
            value = value *1000;
            xQueueOverwrite(waterQueue,&value);
            xEventGroupSetBits(waterFlag,WATERING_START);
        }
        if (json_scanf((const char *)payload, pay_len, "{ temperatureThreshold: %d }", &value)>0)         // Si recibimos un mensaje de cambio de umbral de temperatura
        {
            systemThresholds_.temperature = value;
            xQueueOverwrite(thresholdsQueue,&systemThresholds_);
        }
        if (json_scanf((const char *)payload, pay_len, "{ moistureThreshold: %d }", &value)>0)         // Si recibimos un mensaje de cambio de umbral de humedad en suelo
        {
            systemThresholds_.moisture = value;
            xQueueOverwrite(thresholdsQueue,&systemThresholds_);
        }

    }
    if (strncmp(output_str,TOPIC_LOSANT, top_len) == 0)                                     // Si recibimos un mensaje de Losant - Realizamos las mismas comprobaciones
    {
        systemThresholds systemThresholds_;
        uint32_t value;
        uint32_t freq;
        bool boolean;
        char *aux = NULL;
        if (json_scanf((const char *)payload, pay_len, "{ payload: %Q }", &aux)>0)
        {
            if (json_scanf((const char *)aux, pay_len, "{ measurementSwitch: %B }", &boolean)>0)
            {
                if(boolean)
                {
                    xEventGroupSetBits(measurementFlag, MEASUREMENT_START);
                }
                else
                {
                    xEventGroupClearBits(measurementFlag, MEASUREMENT_START);
                }
#ifdef DEBUG
                UART_PRINT("\measurementSwitch: %d", (int)boolean);
#endif //DEBUG

            }
            if (json_scanf((const char *)aux, pay_len, "{ measuresFreq: %d }", &freq)>0)
            {
                freq = freq*1000*60;
                xQueueOverwrite(freqQueue,&freq);
#ifdef DEBUG
                UART_PRINT("\nmeasuresFreq: %d", freq);
#endif //DEBUG

            }
            if (json_scanf((const char *)aux, pay_len, "{ wateringTime: %d }", &value)>0)
            {
                value = value *1000;
                xQueueOverwrite(waterQueue,&value);
                xEventGroupSetBits(waterFlag,WATERING_START);
#ifdef DEBUG
                UART_PRINT("\Watering Time: %d", value);
#endif //DEBUG
            }
            if (json_scanf((const char *)aux, pay_len, "{ temperatureThreshold: %d }", &value)>0)
            {
                systemThresholds_.temperature = value;
                xQueueOverwrite(thresholdsQueue,&systemThresholds_);
#ifdef DEBUG
        UART_PRINT("\nsystemThresholds temeperature: %d", systemThresholds_.temperature);
#endif //DEBUG
            }
            if (json_scanf((const char *)aux, pay_len, "{ moistureThreshold: %d }", &value)>0)
            {
                systemThresholds_.moisture = value;
                xQueueOverwrite(thresholdsQueue,&systemThresholds_);
#ifdef DEBUG
        UART_PRINT("\nsystemThresholds moisture: %d", systemThresholds_.moisture);
#endif //DEBUG
            }
        }

    }

    // Publicamos por la consola el mensaje publicado
    UART_PRINT("\n\rPublish Message Received");
    UART_PRINT("\n\rTopic: ");
    UART_PRINT("%s",output_str);
    vPortFree(output_str);
    UART_PRINT(" [Qos: %d] ",qos);
    if(retain)
      UART_PRINT(" [Retained]");
    if(dup)
      UART_PRINT(" [Duplicate]");
    
    output_str=(char*)pvPortMalloc(pay_len+1);
    memset(output_str,'\0',pay_len+1);
    strncpy(output_str, (char*)payload, pay_len);
    output_str[pay_len]='\0';
    UART_PRINT("\n\rData is: ");
    UART_PRINT("%s",(char*)output_str);
    UART_PRINT("\n\r");
    vPortFree(output_str);
    
    return;
}

//****************************************************************************
//! Defines sl_MqttEvt event handler.
//! Client App needs to register this event handler with sl_ExtLib_mqtt_Init 
//! API. Background receive task invokes this handler whenever MQTT Client 
//! receives an ack(whenever user is in non-blocking mode) or encounters an error.
//!
//! param[out]      evt => Event that invokes the handler. Event can be of the
//!                        following types:
//!                        MQTT_ACK - Ack Received 
//!                        MQTT_ERROR - unknown error
//!                        
//!  
//! \param[out]     buf => points to buffer
//! \param[out]     len => buffer length
//!       
//! \return none
//****************************************************************************
static void
sl_MqttEvt(void *app_hndl, long evt, const void *buf,unsigned long len)
{
    int i;
    switch(evt)
    {
      case SL_MQTT_CL_EVT_PUBACK:
        UART_PRINT("PubAck:\n\r");
        UART_PRINT("%s\n\r",buf);
        break;
    
      case SL_MQTT_CL_EVT_SUBACK:
        UART_PRINT("\n\rGranted QoS Levels are:\n\r");
        
        for(i=0;i<len;i++)
        {
          UART_PRINT("QoS %d\n\r",((unsigned char*)buf)[i]);
        }
        break;
        
      case SL_MQTT_CL_EVT_UNSUBACK:
        UART_PRINT("UnSub Ack \n\r");
        UART_PRINT("%s\n\r",buf);
        break;
    
      default:
        break;
  
    }
}

//****************************************************************************
//
//! callback event in case of MQTT disconnection
//!
//! \param app_hndl is the handle for the disconnected connection
//!
//! return none
//
//****************************************************************************
static void
sl_MqttDisconnect(void *app_hndl)
{
    connect_config *local_con_conf;
    osi_messages var = BROKER_DISCONNECTION;
    local_con_conf = (connect_config *)app_hndl;
    sl_ExtLib_MqttClientUnsub(local_con_conf->clt_ctx, local_con_conf->topic,
                              TOPIC_COUNT);
    UART_PRINT("disconnect from broker %s\r\n",
           (local_con_conf->broker_config).server_info.server_addr);
    local_con_conf->is_connected = false;
    sl_ExtLib_MqttClientCtxDelete(local_con_conf->clt_ctx);

    //
    // write message indicating publish message
    //
    osi_MsgQWrite(&g_PBQueue,&var,OSI_NO_WAIT);

}




//****************************************************************************
//
//!    Toggles the state of GPIOs(LEDs)
//!
//! \param LedNum is the enumeration for the GPIO to be toggled
//!
//!    \return none
//
//****************************************************************************
void ToggleLedState(ledEnum LedNum)
{
    unsigned char ledstate = 0;
    switch(LedNum)
    {
    case LED1:
        ledstate = GPIO_IF_LedStatus(MCU_RED_LED_GPIO);
        if(!ledstate)
        {
            GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        }
        else
        {
            GPIO_IF_LedOff(MCU_RED_LED_GPIO);
        }
        break;
    case LED2:
        ledstate = GPIO_IF_LedStatus(MCU_ORANGE_LED_GPIO);
        if(!ledstate)
        {
            GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
        }
        else
        {
            GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);
        }
        break;
    case LED3:
        ledstate = GPIO_IF_LedStatus(MCU_GREEN_LED_GPIO);
        if(!ledstate)
        {
            GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
        }
        else
        {
            GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
        }
        break;
    default:
        break;
    }
}

//*****************************************************************************
//
//! Periodic Timer Interrupt Handler
//!
//! \param None
//!
//! \return None
//
//*****************************************************************************
void
TimerPeriodicIntHandler(void)
{
    unsigned long ulInts;

    //
    // Clear all pending interrupts from the timer we are
    // currently using.
    //
    ulInts = MAP_TimerIntStatus(TIMERA0_BASE, true);
    MAP_TimerIntClear(TIMERA0_BASE, ulInts);

    //
    // Increment our interrupt counter.
    //
    g_usTimerInts++;
    if(!(g_usTimerInts & 0x1))
    {
        //
        // Off Led
        //
        GPIO_IF_LedOff(MCU_RED_LED_GPIO);
    }
    else
    {
        //
        // On Led
        //
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
    }
}

//****************************************************************************
//
//! Function to configure and start timer to blink the LED while device is
//! trying to connect to an AP
//!
//! \param none
//!
//! return none
//
//****************************************************************************
void LedTimerConfigNStart()
{
    //
    // Configure Timer for blinking the LED for IP acquisition
    //
    Timer_IF_Init(PRCM_TIMERA0,TIMERA0_BASE,TIMER_CFG_PERIODIC,TIMER_A,0);
    Timer_IF_IntSetup(TIMERA0_BASE,TIMER_A,TimerPeriodicIntHandler);
    Timer_IF_Start(TIMERA0_BASE,TIMER_A,100);
}

//****************************************************************************
//
//! Disable the LED blinking Timer as Device is connected to AP
//!
//! \param none
//!
//! return none
//
//****************************************************************************
void LedTimerDeinitStop()
{
    //
    // Disable the LED blinking Timer as Device is connected to AP
    //
    Timer_IF_Stop(TIMERA0_BASE,TIMER_A);
    Timer_IF_DeInit(TIMERA0_BASE,TIMER_A);

}

//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
void BoardInit(void)
{
    /* In case of TI-RTOS vector table is initialize by OS itself */
    #ifndef USE_TIRTOS
    //
    // Set vector table base
    //
    #if defined(ccs)
        IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
    #endif
    #if defined(ewarm)
        IntVTableBaseSet((unsigned long)&__vector_table);
    #endif
    #endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

//*****************************************************************************
//
//! Application startup display on UART
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
static void
DisplayBanner(char * AppName)
{

    UART_PRINT("\n\n\n\r");
    UART_PRINT("\t\t *************************************************\n\r");
    UART_PRINT("\t\t    CC3200 %s Application       \n\r", AppName);
    UART_PRINT("\t\t *************************************************\n\r");
    UART_PRINT("\n\n\n\r");
}
  


//GLOBAL PARA DE MOMENTO NO GASTAR PILA (CUIDADO!!!)
char json_buffer[110];
//struct json_out out1 = JSON_OUT_BUF(json_buffer, sizeof(json_buffer));






//*****************************************************************************
//
//! Task implementing MQTT client communication to other web client through
//!    a broker
//!
//! \param  none
//!
//! This function
//!    1. Initializes network driver and connects to the default AP
//!    2. Initializes the mqtt library and set up MQTT connection configurations
//!    3. set up the button events and their callbacks(for publishing)
//!    4. handles the callback signals
//!
//! \return None
//!
//*****************************************************************************
void ConnectWiFI(void *pvParameters)
{
    
    long lRetVal = -1;
#ifdef STATION_MODE
    //These are only used in that mode
    int iCount = 0;
    int iNumBroker = 0;
    int iConnBroker = 0;
    osi_messages RecvQue;
    connect_config *local_con_conf = (connect_config *)app_hndl;
#endif
    //
    // Configure LED
    //
    GPIO_IF_LedConfigure(LED1|LED2|LED3);

    GPIO_IF_LedOff(MCU_RED_LED_GPIO);
    GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);

    //
    // Reset The state of the machine
    //
    Network_IF_ResetMCUStateMachine();

    //
    // Start the driver
    //
    lRetVal = Network_IF_InitDriver(ROLE_STA);
    if(lRetVal < 0)
    {
       UART_PRINT("Failed to start SimpleLink Device\n\r",lRetVal);
       LOOP_FOREVER();
    }

    // switch on Green LED to indicate Simplelink is properly up
    GPIO_IF_LedOn(MCU_ON_IND);

    // Start Timer to blink Red LED till AP connection
    LedTimerConfigNStart();

    // Initialize AP security params
    SecurityParams.Key = (signed char *)SECURITY_KEY;
    SecurityParams.KeyLen = strlen(SECURITY_KEY);
    SecurityParams.Type = SECURITY_TYPE;

    //
    // Connect to the Access Point
    //
    lRetVal = Network_IF_ConnectAP(SSID_NAME, SecurityParams);
    if(lRetVal < 0)
    {
       UART_PRINT("Connection to an AP failed\n\r");
       LOOP_FOREVER();
    }

    //
    // Disable the LED blinking Timer as Device is connected to AP
    //
    LedTimerDeinitStop();

    //
    // Switch ON RED LED to indicate that Device acquired an IP
    //
    GPIO_IF_LedOn(MCU_IP_ALLOC_IND);


    osi_Sleep(1000);

    GPIO_IF_LedOff(MCU_RED_LED_GPIO);
    GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);
    GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);


    //
    // Inicializa la biblioteca que gestiona el bus I2C. (movido desde main(...))
    //
//    if(I2C_IF_Open(I2C_MASTER_MODE_FST) < 0)
//    {
//     while (1);
//    }
//    osi_Sleep(10); //Espera un poco


    //Esto tiene que ser realizado en una tarea. De momento lo pongo aqui
    //Init Temperature Sensor
    if(TMP006DrvOpen() < 0)
    {
        UARTprintf("TMP006 open error\n");
    }

    //Esto tiene que ser realizado en una tarea. De momento lo pongo aqui
    //Init Accelerometer Sensor
//    if(BMA222Open() < 0)
//    {
//        UARTprintf("BMA222 open error\n");
//    }


    osi_Sleep(10); //Espera un poco


    //Habilitamos los timers en modo PWM (pero NO habilitamos los pines como PWM)
    PWM_IF_Init(0);


    //
    // Display Application Banner
    //
    DisplayBanner("TFM - Sergio Gasquez");
   
    //
    // Init Push Button, enable ISRs
    //
//    ButtonsInit();
    
    //Lanza el interprete de comandos...
    lRetVal = osi_TaskCreate(vUARTTask,
                        (const signed char *)"CmdLine",
                        OSI_STACK_SIZE, NULL, 2, NULL );

    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }

    //
    // Initialze MQTT client lib
    //
    lRetVal = sl_ExtLib_MqttClientInit(&Mqtt_Client);
    if(lRetVal != 0)
    {
        // lib initialization failed
        UART_PRINT("MQTT Client lib initialization failed\n\r");
        LOOP_FOREVER();
    }
    
    /******************* connection to the broker ***************************/
    iNumBroker = sizeof(usr_connect_config)/sizeof(connect_config);
    if(iNumBroker > MAX_BROKER_CONN)
    {
        UART_PRINT("Num of brokers are more then max num of brokers\n\r");
        LOOP_FOREVER();
    }

    while(iCount < iNumBroker)
    {
        //create client context
        local_con_conf[iCount].clt_ctx =
        sl_ExtLib_MqttClientCtxCreate(&local_con_conf[iCount].broker_config,
                                      &local_con_conf[iCount].CallBAcks,
                                      &(local_con_conf[iCount]));

        //
        // Set Client ID
        //
        sl_ExtLib_MqttClientSet((void*)local_con_conf[iCount].clt_ctx,
                            SL_MQTT_PARAM_CLIENT_ID,
                            local_con_conf[iCount].client_id,
                            strlen((char*)(local_con_conf[iCount].client_id)));

        //
        // Set will Params
        //
        if(local_con_conf[iCount].will_params.will_topic != NULL)
        {
            sl_ExtLib_MqttClientSet((void*)local_con_conf[iCount].clt_ctx,
                                    SL_MQTT_PARAM_WILL_PARAM,
                                    &(local_con_conf[iCount].will_params),
                                    sizeof(SlMqttWill_t));
        }

        //
        // setting username and password
        //
        if(local_con_conf[iCount].usr_name != NULL)
        {
            sl_ExtLib_MqttClientSet((void*)local_con_conf[iCount].clt_ctx,
                                SL_MQTT_PARAM_USER_NAME,
                                local_con_conf[iCount].usr_name,
                                strlen((char*)local_con_conf[iCount].usr_name));

            if(local_con_conf[iCount].usr_pwd != NULL)
            {
                sl_ExtLib_MqttClientSet((void*)local_con_conf[iCount].clt_ctx,
                                SL_MQTT_PARAM_PASS_WORD,
                                local_con_conf[iCount].usr_pwd,
                                strlen((char*)local_con_conf[iCount].usr_pwd));
            }
        }

        //
        // connectin to the broker
        //
        if((sl_ExtLib_MqttClientConnect((void*)local_con_conf[iCount].clt_ctx,
                            local_con_conf[iCount].is_clean,
                            local_con_conf[iCount].keep_alive_time) & 0xFF) != 0)
        {
            UART_PRINT("\n\rBroker connect fail for conn no. %d \n\r",iCount+1);
            
            //delete the context for this connection
            sl_ExtLib_MqttClientCtxDelete(local_con_conf[iCount].clt_ctx);
            
            break;
        }
        else
        {
            UART_PRINT("\n\rSuccess: conn to Broker no. %d\n\r ", iCount+1);
            local_con_conf[iCount].is_connected = true;
            iConnBroker++;
        }

        //
        // Subscribe to topics
        //

        if(sl_ExtLib_MqttClientSub((void*)local_con_conf[iCount].clt_ctx,
                                   local_con_conf[iCount].topic,
                                   local_con_conf[iCount].qos, TOPIC_COUNT) < 0)
        {
            UART_PRINT("\n\r Subscription Error for conn no. %d\n\r", iCount+1);
            UART_PRINT("Disconnecting from the broker\r\n");
            sl_ExtLib_MqttClientDisconnect(local_con_conf[iCount].clt_ctx);
            local_con_conf[iCount].is_connected = false;
            
            //delete the context for this connection
            sl_ExtLib_MqttClientCtxDelete(local_con_conf[iCount].clt_ctx);
            iConnBroker--;
            break;
        }
        else
        {
            int iSub;
            UART_PRINT("Client subscribed on following topics:\n\r");
            for(iSub = 0; iSub < local_con_conf[iCount].num_topics; iSub++)
            {
                UART_PRINT("%s\n\r", local_con_conf[iCount].topic[iSub]);
            }
        }
        iCount++;
    }

    if(iConnBroker < 1)
    {
        //
        // no succesful connection to broker
        //
        goto end;
    }

    iCount = 0;

    for(;;)
    {
        osi_MsgQRead( &g_PBQueue, &RecvQue, OSI_WAIT_FOREVER);                  // Funcion bloqueante que espera a que haya algun mensaje en la cola de salida
                                                                                // Cuando llega algun mensaje lo guardamos en RecvQue
        // Comrpobamosque tipo de mensaje es
        if(SENSORS_REPORT== RecvQue)              // Si es porque se ha pulsado el boton SW2, se envia el mensaje corresponiente
        {
            sensorsData sensorsData_;
            systemThresholds systemThresholds_;
            uint8_t i = 0;
            // Moisture
            struct json_out out1 = JSON_OUT_BUF(json_buffer, sizeof(json_buffer));
            getTemperature(&sensorsData_.sht31Data_.temperature, &sensorsData_.sht31Data_.humidity);
            analogReadMoisture(&sensorsData_.moisture_);
            xQueuePeek(thresholdsQueue, &systemThresholds_, ( TickType_t )10);
            if((float)(systemThresholds_.temperature) <= sensorsData_.sht31Data_.temperature && (float)systemThresholds_.moisture >= sensorsData_.moisture_)
            {
                xEventGroupSetBits(waterFlag,WATERING_START);
            }

            json_printf(&out1,"{moisture : %f, temperature : %f,  humidity : %f}", (float)sensorsData_.moisture_, (float)sensorsData_.sht31Data_.temperature, (float)sensorsData_.sht31Data_.humidity);
            sl_ExtLib_MqttClientSend((void*)local_con_conf[iCount].clt_ctx,
                                     pub_topic_sensors,json_buffer,strlen((char*)json_buffer),QOS2,RETAIN);
#ifdef DEBUG
            UART_PRINT("\n\r CC3200 Publishes the following message\n\r");
            UART_PRINT("Topic: %s\n\r",pub_topic_sensors);
            UART_PRINT("Data: %s\n\r",json_buffer);
#endif //DEBUG
            // Reenviamos el mensaje para Losant
            json_buffer[0] = 0;
            struct json_out out2 = JSON_OUT_BUF(json_buffer, sizeof(json_buffer));
            json_printf(&out2,"{data: {moisture : %f, temperature : %f,  humidity : %f}}", (float)sensorsData_.moisture_, (float)sensorsData_.sht31Data_.temperature, (float)sensorsData_.sht31Data_.humidity);
            sl_ExtLib_MqttClientSend((void*)local_con_conf[iCount].clt_ctx,
                             pub_topic_losant,json_buffer,strlen((char*)json_buffer),QOS0,0);
#ifdef DEBUG
            UART_PRINT("Topic: %s\n\r",pub_topic_losant);
            UART_PRINT("Data: %s\n\r",json_buffer);
#endif //DEBUG


        }
        if(WATER_REPORT== RecvQue)              // Si es porque se ha pulsado el boton SW2, se envia el mensaje corresponiente
        {
            float waterLevel = 0;
            uint8_t i = 0;
            for ( i =0; i < 5; i++)
            {
                waterLevel += readWaterLevel();
                osi_Sleep(200);
            }

            waterLevel = waterLevel/5;
            struct json_out out1 = JSON_OUT_BUF(json_buffer, sizeof(json_buffer));
            json_printf(&out1,"{waterLevel : %f}", waterLevel);
            sl_ExtLib_MqttClientSend((void*)local_con_conf[iCount].clt_ctx,
                                    pub_topic_sensors,json_buffer,strlen((char*)json_buffer),QOS2,RETAIN);
#ifdef DEBUG
            UART_PRINT("\n\r CC3200 Publishes the following message\n\r");
            UART_PRINT("Topic: %s\n\r",pub_topic_sensors);
            UART_PRINT("Data: %s\n\r",json_buffer);
#endif //DEBUG
            // Reenviamos el mensaje para Losant
            json_buffer[0] = 0;
            struct json_out out2 = JSON_OUT_BUF(json_buffer, sizeof(json_buffer));
            json_printf(&out2,"{data: {waterLevel : %f}}", waterLevel);
            sl_ExtLib_MqttClientSend((void*)local_con_conf[iCount].clt_ctx,
                            pub_topic_losant,json_buffer,strlen((char*)json_buffer),QOS0,0);
#ifdef DEBUG
            UART_PRINT("Topic: %s\n\r",pub_topic_losant);
            UART_PRINT("Data: %s\n\r",json_buffer);
#endif //DEBUG

        }
        else if(BROKER_DISCONNECTION == RecvQue)    // Si es porque se ha producido una desconexion se termina
        {
            iConnBroker--;
            if(iConnBroker < 1)
            {
                //
                // device not connected to any broker
                //
                goto end;
            }
        }
    }
end:
    //
    // Deinitializating the client library
    //
    sl_ExtLib_MqttClientExit();
    UART_PRINT("\n\r Exiting the Application\n\r");
    
    LOOP_FOREVER();


} //Fin de la funcion de la tarea ConnectWIFI




void measurementsTask (void *pvParameters)
{
    for( ;; )                                                                       // Debemos tener un bucle infinito
    {
        uint32_t samplingFreq = 60000;
        xEventGroupWaitBits(measurementFlag, MEASUREMENT_START, pdFALSE, pdFALSE, portMAX_DELAY);                         // Funcion bloqueante que espera a que se active el flag,no resetea los flags al salir y no espera todos los flags. Espera infinto
        osi_messages var = SENSORS_REPORT;
        osi_MsgQWrite(&g_PBQueue,&var,OSI_NO_WAIT);
        xQueuePeek(freqQueue,&samplingFreq, ( TickType_t )10);
        osi_Sleep(samplingFreq);
    }
}


void wateringTask (void *pvParameters)
{
    osi_messages var = WATER_REPORT;
    osi_MsgQWrite(&g_PBQueue,&var,OSI_NO_WAIT);
    for( ;; )                                                                       // Debemos tener un bucle infinito
    {
        uint32_t wateringTime = 5000;
        xEventGroupWaitBits(waterFlag, WATERING_START, pdFALSE, pdFALSE, portMAX_DELAY);                         // Funcion bloqueante que espera a que se active el flag,no resetea los flags al salir y no espera todos los flags. Espera infinto
        xQueuePeek(waterQueue, &wateringTime, ( TickType_t )10);
        GPIOPinWrite(GPIOA0_BASE, 0x10, 0x10);
        osi_Sleep(( TickType_t )wateringTime);
        GPIOPinWrite(GPIOA0_BASE, 0x10, 0);
        UART_PRINT("\n Watering for %d secs", wateringTime/1000);
        osi_messages var = WATER_REPORT;
        osi_MsgQWrite(&g_PBQueue,&var,OSI_NO_WAIT);
        xEventGroupClearBits(waterFlag,WATERING_START);
    }
}


//*****************************************************************************
//
//! Main 
//!
//! \param  none
//!
//! This function
//!    1. Invokes the SLHost task
//!    2. Invokes the ConnectWiFI
//!
//! \return None
//!
//*****************************************************************************



void main()
{
    long lRetVal = -1;

    //
    // Initialize the board configurations
    //
    BoardInit();

    //
    // Pinmux for UART
    //
    PinMuxConfig();

    //
    // Configuring UART
    //
    InitTerm();

    //
    // Configure the ADC for the Moisture Sensor
    //
    setupAdc();

    //
    // Configure the timers for the Water Level Sensor
    //
    setupWaterLevel();

    //
    // Configure the I2C for the Water Level Sensor
    //
    setupTemperatureSensor();


    //
    // Configure relay pin
    //
    MAP_PinTypeGPIO(RELAY_PIN, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA0_BASE, 0x10, GPIO_DIR_MODE_OUT);
    GPIOPinWrite(GPIOA0_BASE, 0x10, 0);


    //
    // Start the SimpleLink Host
    //
    lRetVal = VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }

    //
    // Start the MQTT Client task
    //
    osi_MsgQCreate(&g_PBQueue,"g_PBQueue",sizeof(osi_messages),20);
    lRetVal = osi_TaskCreate(ConnectWiFI,
                            (const signed char *)"WifiApp",
                            OSI_STACK_SIZE, NULL, 2, NULL );

    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }


    //
    // Crear las colas
    //
    freqQueue = xQueueCreate(1,sizeof(uint32_t));
    if (freqQueue==NULL)
    {
        while(1);
    }

    thresholdsQueue = xQueueCreate(1,sizeof(systemThresholds));
    if (thresholdsQueue==NULL)
    {
        while(1);
    }

    waterQueue = xQueueCreate(1,sizeof(uint32_t));
    if (waterQueue==NULL)
    {
        while(1);
    }

    //
    // Crear los grupos de eventos
    //
    measurementFlag = xEventGroupCreate();
    if( measurementFlag == NULL )
    {
            while(1);
    }

    waterFlag = xEventGroupCreate();
    if( waterFlag == NULL )
    {
            while(1);
    }

    //
    // Crear las tareas
    //
    lRetVal = osi_TaskCreate(measurementsTask,
                                (const signed char *)"Measurements Task",
                                OSI_STACK_SIZE, NULL, 2, NULL );

    if(lRetVal < 0)
    {
       ERR_PRINT(lRetVal);
       LOOP_FOREVER();
    }

    lRetVal = osi_TaskCreate(wateringTask,
                                (const signed char *)"Watering Task",
                                OSI_STACK_SIZE, NULL, 2, NULL );

    if(lRetVal < 0)
    {
       ERR_PRINT(lRetVal);
       LOOP_FOREVER();
    }

    //
    // Start the task scheduler
    //
    osi_start();
}





