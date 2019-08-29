#include "guipanel.h"
#include "ui_guipanel.h"
#include <QSerialPort>      // Comunicacion por el puerto serie
#include <QSerialPortInfo>  // Comunicacion por el puerto serie
#include <QMessageBox>      // Se deben incluir cabeceras a los componentes que se vayan a crear en la clase

#include <QJsonObject>
#include <QJsonDocument>

// y que no estén incluidos en el interfaz gráfico. En este caso, la ventana de PopUp <QMessageBox>
// que se muestra al recibir un PING de respuesta

#include<stdint.h>      // Cabecera para usar tipos de enteros con tamaño
#include<stdbool.h>     // Cabecera para usar booleanos
#include <iostream>
#include <fstream>
#include <string>


constexpr const char * TOPIC_PC                     = "/PC";       // Topic en el que se publican los mensajes de PC a CC3200
constexpr const char * TOPIC_CC3200                 = "/cc3200/#"; // Topic en el que recibiremos los mensajes de CC3200. Al usar #, nos subscribimos a todos sus subtopics

/*
// Subtopics (no se usan porque se subscribe solo al topic superior)
constexpr const char * TOPIC_LED                    = "/cc3200/LEDs";
constexpr const char * TOPIC_TEMP                   = "/cc3200/TEMP";
constexpr const char * TOPIC_ACC                    = "/cc3200/ACC";
constexpr const char * TOPIC_CHECK_BUTTONS          = "/cc3200/BUTTONS";
constexpr const char * TOPIC_CHECK_BUTTONS_ASYNC    = "/cc3200/BUTTONS_ASYNC";
constexpr const char * TOPIC_MODE                   = "/cc3200/MODE";
constexpr const char * TOPIC_PWM                    = "/cc3200/PWM";
constexpr const char * TOPIC_PING                   = "/cc3200/PING";
*/




GUIPanel::GUIPanel(QWidget *parent) :                // Constructor de la clase
    QWidget(parent),
    ui(new Ui::GUIPanel),                            // Indica que guipanel.ui es el interfaz grafico de la clase
   transactionCount(0),
   pinged_(false),                                   // Inicializamos las variables que controlan que topics esperamos
   temperatureEnabled_(false),
   moistureEnable_(false),
   waterLevelEnable_(false)
{
    ui->setupUi(this);                              // Conecta la clase con su interfaz gráfico.
    setWindowTitle(tr("Interfaz de Control"));      // Título de la ventana

    _client=new QMQTT::Client("localhost", 1883);   // Localhost y puerto del cliente

    connect(_client, SIGNAL(connected()), this, SLOT(onMQTT_Connected()));
    connect(_client, SIGNAL(connacked(quint8)), this, SLOT(onMQTT_Connacked(quint8)));
    connect(_client, SIGNAL(received(const QMQTT::Message &)), this, SLOT(onMQTT_Received(const QMQTT::Message &)));
    connect(_client, SIGNAL(subscribed(const QString &)), this, SLOT(onMQTT_subscribed(const QString &)));
    connect(_client, SIGNAL(subacked(quint16, quint8)), this, SLOT(onMQTT_subacked(quint16, quint8)));

    connected=false;                 // Todavía no hemos establecido la conexión USB

//    //Deshabilita la interfaz hasta que se establezca la conexion
      ui->tabWidget->setEnabled(false);
      ui->pingButton->setEnabled(false);
      ui->measurmentSwitch->setEnabled(false);


    //Configuramos la moistureGraph
    ui->moistureGraph->setAxisTitle(QwtPlot::xBottom,"Sample Number");   // Titulo de los ejes
    ui->moistureGraph->setAxisTitle(QwtPlot::yLeft, "%");
    ui->moistureGraph->setAxisScale(QwtPlot::yLeft, 0.0, 100.0);          // Con escala definida
    ui->moistureGraph->setAutoReplot(false);

    //Creamos una curva y la añadimos a la moistureGraph
    m_Moisture = new QwtPlotCurve();
    m_Moisture->setPen(QPen(Qt::red));
    mMoisture_Grid = new QwtPlotGrid();
    mMoisture_Grid->attach(ui->moistureGraph);
    m_Moisture->attach(ui->moistureGraph);

    //Inicializadmos los datos que se muestran en la moistureGraph
    for (int i=0; i<NMAX; i++)
    {
        yValMoisture[i]=0;
        xValMoisture[i]=i;
    }
    m_Moisture->setRawSamples(xValMoisture,yValMoisture,NMAX);
    ui->moistureGraph->replot();

//    //Configuramos la ambientGraph
    ui->ambientGraph->setAxisTitle(QwtPlot::xBottom,"Sample Number");   // Titulo de los ejes
    ui->ambientGraph->setAxisTitle(QwtPlot::yLeft, "ºC/%");
    ui->ambientGraph->setAxisScale(QwtPlot::yLeft, 0.0, 100.0);          // Con escala definida
    ui->ambientGraph->setAutoReplot(false);

//    ///Creamos una curva y la añadimos a la accGraph
    m_curve_temp = new QwtPlotCurve();
    m_curve_temp->setPen(QPen(Qt::red));
    m_Ambient_Grid = new QwtPlotGrid();
    m_Ambient_Grid->attach(ui->ambientGraph);
    m_curve_temp->attach(ui->ambientGraph);

//    //Creamos una curva y la añadimos a la accGraph
    m_curve_hum = new QwtPlotCurve();
    m_curve_hum->setPen(QPen(Qt::blue));
    m_curve_hum->attach(ui->ambientGraph);

//    //Inicializadmos los datos que se muestran en la accGraph
    for (int i=0; i<NMAX; i++) {
        yValTemp[i]=0;
        yValHum[i]=0;
        xVal[i]=i;
    }
    m_curve_temp->setRawSamples(xVal,yValTemp,NMAX);
    m_curve_hum->setRawSamples(xVal,yValHum,NMAX);
    ui->ambientGraph->replot();
}

GUIPanel::~GUIPanel()   // Destructor de la clase
{
    delete ui;          // Borra el interfaz gráfico asociado a la clase
}


// Establecimiento de la comunicación USB serie a través del interfaz seleccionado en la comboBox, tras pulsar el
// botón RUN del interfaz gráfico.
void GUIPanel::startClient()
{
    _client->setHost(ui->hostAddress->text());          // Obtenemos el texto de la interfaz
    _client->setPort(1883);                             // Fijamos el puerto (1883)
    _client->setKeepAlive(300);
    _client->setCleansess(true);

    _client->connect();                                 // Nos conectamos
}

// Funcion auxiliar de procesamiento de errores de comunicación (usada por startSlave)
void GUIPanel::processError(const QString &s)
{
    activateRunButton(); // Activa el botón RUN
    // Muestra en la etiqueta de estado la razón del error (notese la forma de pasar argumentos a la cadena de texto)
    ui->statusLabel->setText(tr("Status: Not running, %1.").arg(s));
}

//Funcion que saca un popup mostrando la respuesta del ping
void GUIPanel::pingRecived(bool response)
{
    if(response)
    {
        QMessageBox ventanaPopUp(QMessageBox::Information,tr("Evento"),tr("RESPUESTA A PING RECIBIDA"),QMessageBox::Ok,this,Qt::Popup);
        ventanaPopUp.exec();
    }
    else
    {
        QMessageBox ventanaPopUp(QMessageBox::Information,tr("Evento"),tr("NO SE HA RECIBIDO RESPUESTA"),QMessageBox::Ok,this,Qt::Popup);
        ventanaPopUp.exec();
    }

}

// Funcion de habilitacion del boton de inicio/conexion
void GUIPanel::activateRunButton()
{
    ui->connectButton->setEnabled(true);
    ui->tabWidget->setEnabled(false);
    ui->pingButton->setEnabled(false);
    ui->measurmentSwitch->setEnabled(false);
}

void GUIPanel::onMQTT_subscribed(const QString &topic)
{
     ui->statusLabel->setText(tr("subscribed %1").arg(topic));      // Mostramos el topic al que nos hemos subscrito en el texto de la interfaz
}

void GUIPanel::onMQTT_subacked(quint16 msgid, quint8 qos)
{
     //ui->statusLabel->setText(tr("subacked: msgid=%1, qos=%2").arg(msgid).arg(qos));      // Comentado para menor ruido en la interfaz
}

void GUIPanel::on_stateButton_clicked()
{
    ui->statusLabel->setText(tr(""));       // Al pulsar el boton de estado borramos el texto de la interfaz
}


void GUIPanel::onMQTT_Received(const QMQTT::Message &message)
{
    if (connected)
    {
        //Deshacemos el escalado
        QJsonParseError error;
        QJsonDocument mensaje=QJsonDocument::fromJson(message.payload(),&error);

        if ((error.error==QJsonParseError::NoError)&&(mensaje.isObject()))
        { //Tengo que comprobar que el mensaje es del tipo adecuado y no hay errores de parseo...
            QJsonObject objeto_json=mensaje.object();

            // Comprobar si la respuesta de ping
            if(pinged_)
            {
                pinged_ = false;
                QJsonValue pingResponse=objeto_json["ping"];
                pingRecived(pingResponse.toBool());
            }

//            QJsonValue checkButtonCommand=objeto_json["checkButtons"];              // Como estamos subscritos al topic de PC, recibe los propios mensajes que envia
//            bool checkButtonResponse = !(checkButtonCommand.toBool());              // Por lo que si no ponemos esta condicion al usar el boton check value, el primer
//            if(checkButtonResponse)                                                 // Comando que recibimos, es el propio que enviamos nosotros ({"checkButtons" : true})
//            {                                                                       // Por lo que este comando pondria checkButtons_ a false y no se actualizarian los estados
//                // Actualizar valor de los botones                                  // De los botones
//                if(async_ || checkButtons_)
//                {
//                    QJsonValue checkButton1=objeto_json["button1"];
//                    bool button1 = checkButton1.toBool();
//                    ui->button1State->setChecked(button1);
//                    QJsonValue checkButton2=objeto_json["button2"];
//                    bool button2 = checkButton2.toBool();
//                    ui->button2State->setChecked(button2);
//                    checkButtons_ = false;
//                }
//            }



            // Actualizar temperatura
            if(temperatureEnabled_)
            {
                QJsonValue temperature=objeto_json["temperature"];
                QJsonValue humidity=objeto_json["humidity"];
                if (temperature != QJsonValue::Null && humidity != QJsonValue::Null)
                {
                    ui->temperature->setValue(temperature.toDouble());
                    ui->humidity->setValue(humidity.toDouble());
                    memmove(yValTemp,yValTemp+1,sizeof(double)*(NMAX-1));     //Desplazamos las muestras hacia la izquierda
                    yValTemp[NMAX-1]=temperature.toDouble();    //Añadimos el último punto
                    m_curve_temp->setRawSamples(xVal,yValTemp,NMAX);          //Refrescamos..
                    memmove(yValHum,yValHum+1,sizeof(double)*(NMAX-1));     //Desplazamos las muestras hacia la izquierda
                    yValHum[NMAX-1]=humidity.toDouble();    //Añadimos el último punto
                    m_curve_hum->setRawSamples(xVal,yValHum,NMAX);
                    ui->ambientGraph->replot();

                }
            }

            if(moistureEnable_)
            {
                QJsonValue moisture=objeto_json["moisture"];
                if (moisture != QJsonValue::Null)
                {
                    ui->moisture->setValue(moisture.toDouble());
                    memmove(yValMoisture,yValMoisture+1,sizeof(double)*(NMAX-1));     //Desplazamos las muestras hacia la izquierda
                    yValMoisture[NMAX-1]=moisture.toDouble();    //Añadimos el último punto
                    m_Moisture->setRawSamples(xValMoisture,yValMoisture,NMAX);          //Refrescamos..
                    ui->moistureGraph->replot();

                    std::ofstream os("Moisture.txt", std::ios_base::app);
                    if(os.is_open()){
                     os << std::endl << std::to_string(moisture.toDouble())  ;
                     os.flush();
                    }
                }

            }


//            // Actualizar aceleración
//            if(acc_)
//            {
//                QJsonValue x_acc=objeto_json["x_acc"];
//                QJsonValue y_acc=objeto_json["y_acc"];
//                QJsonValue z_acc=objeto_json["z_acc"];
//                memmove(yVal1,yVal1+1,sizeof(double)*(NMAX-1));     //Desplazamos las muestras hacia la izquierda
//                memmove(yVal2,yVal2+1,sizeof(double)*(NMAX-1));
//                memmove(yVal3,yVal3+1,sizeof(double)*(NMAX-1));
//                yVal1[NMAX-1]=2.0*(double)(x_acc.toInt())/128.0;    //Añadimos el último punto
//                yVal2[NMAX-1]=2.0*(double)(y_acc.toInt())/128.0;
//                yVal3[NMAX-1]=2.0*(double)(z_acc.toInt())/128.0;
//                m_curve_1->setRawSamples(xVal,yVal1,NMAX);          //Refrescamos..
//                m_curve_2->setRawSamples(xVal,yVal2,NMAX);
//                m_curve_3->setRawSamples(xVal,yVal3,NMAX);
//                ui->accGraph->replot();
//            }

//            // En caso de que los valores de la interfaz se cambien de forma externa, la actualiamos:

//            // Actualizar el valor de los checks de los leds en caso de cambiarlos de forma externa
//            QJsonValue redLed=objeto_json["redLed"];
//            QJsonValue orangeLed=objeto_json["orangeLed"];
//            QJsonValue greenLed=objeto_json["greenLed"];
//            bool previousblockinstate,checked;
//            if (redLed.isBool())
//            {

//                checked=redLed.toBool();
//                previousblockinstate=ui->redLed->blockSignals(true);

//                ui->redLed->setChecked(checked);
//                ui->redLed->blockSignals(previousblockinstate);
//            }
//            if (orangeLed.isBool())
//            {

//                checked=orangeLed.toBool();
//                previousblockinstate=ui->orangeLed->blockSignals(true);

//                ui->orangeLed->setChecked(checked);
//                ui->orangeLed->blockSignals(previousblockinstate);
//            }
//            if (greenLed.isBool())
//            {

//                checked=greenLed.toBool();
//                previousblockinstate=ui->greenLed->blockSignals(true);

//                ui->greenLed->setChecked(checked);
//                ui->greenLed->blockSignals(previousblockinstate);
//            }
//            // Actualizar el valor del desplegable de modo
//            QJsonValue mode=objeto_json["mode"];
//            if(mode.isDouble())
//            {
//                int modeSelected = (int)mode.toDouble();
//                ui->modeSelect->setCurrentIndex(modeSelected);
//                mode_=(uint8_t)(modeSelected);
//            }
//            // Actualizar el valor de los botones
//            QJsonValue checkButtons=objeto_json["checkButtons"];
//            if (checkButtons.isBool())
//            {
//                checkButtons_=checkButtons.toBool();
//            }
//            // Actualizar el valor del modo async
//            QJsonValue async=objeto_json["checkButtonsAsync"];
//            if (async.isBool())
//            {
//                async_=async.toBool();
//                previousblockinstate=ui->async->blockSignals(true);

//                ui->async->setChecked(async_);
//                ui->async->blockSignals(previousblockinstate);
//            }
//            // Actualizar si se ha iniciado la medicion de temperatura
//            QJsonValue tempStart=objeto_json["tempStart"];
//            if (tempStart.isBool())
//            {
//                temp_=tempStart.toBool();
//                previousblockinstate=ui->start_Temp->blockSignals(true);

//                ui->start_Temp->setChecked(temp_);
//                ui->start_Temp->blockSignals(previousblockinstate);
//            }
//            // Actualizar si se ha iniciado la medicion de aceleracion
//            QJsonValue accStart=objeto_json["accStart"];
//            if (accStart.isBool())
//            {
//                acc_=accStart.toBool();
//                previousblockinstate=ui->start->blockSignals(true);

//                ui->start->setChecked(acc_);
//                ui->start->blockSignals(previousblockinstate);
//            }

        }
    }
}


/* -----------------------------------------------------------
 MQTT Client Slots
 -----------------------------------------------------------*/
void GUIPanel::onMQTT_Connected()
{
    QString topicCC3200(TOPIC_CC3200);
    QString topicPC(TOPIC_PC);
//    ui->runButton->setEnabled(false);
    // Se indica que se ha realizado la conexión en la etiqueta 'statusLabel'
    ui->statusLabel->setText(tr("Ejecucion, conectado al servidor"));
    connected=true;
    _client->subscribe(topicCC3200,0);  // Se subscribe a todos los topics de CC3200
    _client->subscribe(topicPC,0);      //Se suscribe al mismo topic en el que publica por si recibimos comandos desde otro cliente actualizar la interfaz
}

void GUIPanel::onMQTT_Connacked(quint8 ack)
{
    QString ackStatus;
    switch(ack) {
    case QMQTT::CONNACK_ACCEPT:
        ackStatus = "Connection Accepted";
        break;
    case QMQTT::CONNACK_PROTO_VER:
        ackStatus = "Connection Refused: unacceptable protocol version";
        break;
    case QMQTT::CONNACK_INVALID_ID:
        ackStatus = "Connection Refused: identifier rejected";
        break;
    case QMQTT::CONNACK_SERVER:
        ackStatus = "Connection Refused: server unavailable";
        break;
    case QMQTT::CONNACK_CREDENTIALS:
        ackStatus = "Connection Refused: bad user name or password";
        break;
    case QMQTT::CONNACK_AUTH:
        ackStatus = "Connection Refused: not authorized";
        break;
    }

    //ui->statusLabel->setText(tr("connacked: %1, %2").arg(ack).arg(ackStatus));        // Se ha comentado para reducir ruido en la interfaz
}

// Funcion que publica un mensaje (que recibe como parametro) en el topic PC
void GUIPanel::SendMessage(QJsonDocument mensaje)
{
    QMQTT::Message msg(0, TOPIC_PC, mensaje.toJson());              //Crea el mensaje MQTT contieniendo el mensaje en formato JSON y fijando el topic

    //Publica el mensaje
    _client->publish(msg);

}

// SLOT asociada a pulsación del botón Start
void GUIPanel::on_connectButton_clicked()
{
    startClient();
    // Habilitamos las partes del interfaz necesarias
    ui->connectButton->setEnabled(false);
    ui->pingButton->setEnabled(true);
    ui->tabWidget->setEnabled(true);
    ui->measurmentSwitch->setEnabled(true);
}

//Al pulsar el boton Ping , envia el comando de ping
void GUIPanel::on_pingButton_clicked()
{
    pinged_ = true;                             // Activamos la variable que observa si esperamos respuesta de este mensaje
    QJsonObject objeto_json;
    objeto_json["ping"]= true;                  // Enviamos el mensaje
    QJsonDocument mensaje(objeto_json);

    SendMessage(mensaje);
}

//Slots asociado a los checkboxes de los LEDs
void GUIPanel::on_tempEnable_toggled(bool checked)
{
    temperatureEnabled_ = checked;                           // Activamos o desactivamos la variable que observa si esperamos respuesta de este mensaje
    QJsonObject objeto_json;
    objeto_json["ambientEnable"]=checked;   // Creamos el mensaje y lo enviamos
    QJsonDocument mensaje(objeto_json);
    SendMessage(mensaje);
}

void GUIPanel::on_moistEnable_toggled(bool checked)
{
    moistureEnable_ = checked;                           // Activamos o desactivamos la variable que observa si esperamos respuesta de este mensaje
    QJsonObject objeto_json;
    objeto_json["moistureEnable"]=checked;   // Creamos el mensaje y lo enviamos
    QJsonDocument mensaje(objeto_json);
    SendMessage(mensaje);
}

//Envia el valor de frecuencia de medicion de la temperatura cuando se cambia su valor desde la interfaz
void GUIPanel::on_freq_valueChanged(double value)
{
    QJsonObject objeto_json;
    objeto_json["measuresFreq"]=value;              // Creamos el mensaje y lo enviamos con el valor de la rueda
    QJsonDocument mensaje(objeto_json);
    SendMessage(mensaje);
}



