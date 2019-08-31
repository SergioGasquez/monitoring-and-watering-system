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

#include <time.h>
#include <iomanip>



constexpr const char * TOPIC_PC                     = "/PC";       // Topic en el que se publican los mensajes de PC a CC3200
constexpr const char * TOPIC_CC3200                 = "/cc3200/#"; // Topic en el que recibiremos los mensajes de CC3200. Al usar #, nos subscribimos a todos sus subtopics

/*
// Subtopics (no se usan porque se subscribe solo al topic superior)
constexpr const char * TOPIC_SENSORS                    = "/cc3200/SENSORS";
constexpr const char * TOPIC_PING                   = "/cc3200/PING";
*/




GUIPanel::GUIPanel(QWidget *parent) :                // Constructor de la clase
    QWidget(parent),
    ui(new Ui::GUIPanel),                            // Indica que guipanel.ui es el interfaz grafico de la clase
   transactionCount(0),
   pinged_(false),                                   // Inicializamos las variables que controlan que topics esperamos
   fileInitialized_(false),
   filename_(""),
   wateringTime_(0),
   waterMoistureSlider_(0),
   waterTemperatureSlider_(0)
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

    // Deshabilita la interfaz hasta que se establezca la conexion
    ui->tabWidget->setEnabled(false);
    ui->pingButton->setEnabled(false);
    ui->measurmentSwitch->setEnabled(false);

    ui->temperature->setValue(0);
    ui->humidity->setValue(0);
    ui->moisture->setValue(0);
    ui->waterLevel->setValue(0);

    // Configuramos la moistureGraph
    ui->moistureGraph->setAxisTitle(QwtPlot::xBottom,"Sample Number");   // Titulo de los ejes
    ui->moistureGraph->setAxisTitle(QwtPlot::yLeft, "%");
    ui->moistureGraph->setAxisScale(QwtPlot::yLeft, 0.0, 100.0);          // Con escala definida
    ui->moistureGraph->setAutoReplot(false);

    // Creamos una curva y la añadimos a la moistureGraph
    m_Moisture = new QwtPlotCurve();
    m_Moisture->setPen(QPen(Qt::red));
    mMoisture_Grid = new QwtPlotGrid();
    mMoisture_Grid->attach(ui->moistureGraph);
    m_Moisture->attach(ui->moistureGraph);

    // Inicializadmos los datos que se muestran en la moistureGraph
    for (int i=0; i<NMAX; i++)
    {
        yValMoisture[i]=0;
        xValMoisture[i]=i;
    }
    m_Moisture->setRawSamples(xValMoisture,yValMoisture,NMAX);
    ui->moistureGraph->replot();

    // Configuramos la ambientGraph
    ui->ambientGraph->setAxisTitle(QwtPlot::xBottom,"Sample Number");   // Titulo de los ejes
    ui->ambientGraph->setAxisTitle(QwtPlot::yLeft, "ºC/%");
    ui->ambientGraph->setAxisScale(QwtPlot::yLeft, 0.0, 100.0);          // Con escala definida
    ui->ambientGraph->setAutoReplot(false);

    // Creamos una curva y la añadimos a la moistureGraph
    m_curve_temp = new QwtPlotCurve();
    m_curve_temp->setPen(QPen(Qt::red));
    m_Ambient_Grid = new QwtPlotGrid();
    m_Ambient_Grid->attach(ui->ambientGraph);
    m_curve_temp->attach(ui->ambientGraph);

    m_curve_hum = new QwtPlotCurve();
    m_curve_hum->setPen(QPen(Qt::blue));
    m_curve_hum->attach(ui->ambientGraph);

    // Inicializadmos los datos que se muestran en la moistureGraph
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

            // Actualizar temperatura

            QJsonValue temperature=objeto_json["temperature"];
            QJsonValue humidity=objeto_json["humidity"];
            QJsonValue moisture=objeto_json["moisture"];
            if ((temperature != QJsonValue::Null) || (humidity != QJsonValue::Null) || (moisture != QJsonValue::Null))
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
                ui->moisture->setValue(moisture.toDouble());
                memmove(yValMoisture,yValMoisture+1,sizeof(double)*(NMAX-1));     //Desplazamos las muestras hacia la izquierda
                yValMoisture[NMAX-1]=moisture.toDouble();    //Añadimos el último punto
                m_Moisture->setRawSamples(xValMoisture,yValMoisture,NMAX);          //Refrescamos..
                ui->moistureGraph->replot();

                saveData(temperature.toDouble(), humidity.toDouble(), moisture.toDouble());

            }

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

void GUIPanel::saveData(double temperature, double humidity, double moisture)
{
    if (!fileInitialized_)
    {
        std::time_t t = std::time(nullptr);
        char aux[100];
        if (std::strftime(aux, sizeof(aux), " %D %I %M %S", std::localtime(&t))) {
            std::cout << aux << '\n';
        }
        aux[3] = '-';
        aux[6] = '-';
        aux[9] = '_';
        aux[12] = '-';
        aux[15] = '-';
        std::string buffer;
        buffer.append(aux);
        filename_ = buffer + ".txt";
        std::ofstream os(filename_.c_str(), std::ios_base::app);
        if(os.is_open())
        {
            os << "temperature, humidity, moisture \n";
            os.flush();
        }
        fileInitialized_ = true;
    }

    std::ofstream os(filename_.c_str(), std::ios_base::app);
    if(os.is_open())
    {
        os << std::endl << std::to_string(temperature) << ", " << std::to_string(humidity) << ", " << std::to_string(moisture);
        os.flush();
    }
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

void GUIPanel::on_measurmentSwitch_valueChanged(int  value)
{
    QJsonObject objeto_json;
    objeto_json["measurementSwitch"]=value;              // Creamos el mensaje y lo enviamos con el valor de la rueda
    QJsonDocument mensaje(objeto_json);
    SendMessage(mensaje);
}

void GUIPanel::on_waterTimeSlider_valueChanged(double value)
{
    wateringTime_ = value;
}
void GUIPanel::on_waterButton_clicked()
{
    QJsonObject objeto_json;
    objeto_json["wateringTime"]=wateringTime_;              // Creamos el mensaje y lo enviamos con el valor de la rueda
    QJsonDocument mensaje(objeto_json);
    SendMessage(mensaje);
}

void GUIPanel::on_waterMoistureSlider_valueChanged(double value)
{
    waterMoistureSlider_ = value;
}
void GUIPanel::on_waterTemperatureSlider_valueChanged(double value)
{
    waterTemperatureSlider_ = value;
}
void GUIPanel::on_waterParamsSet_clicked()
{
    if(waterMoistureSlider_ > 0 && waterTemperatureSlider_ > 0)     // TODO: FIJAR LOS VALORES ESTOS CON SENTIDO
    {
        QJsonObject objeto_json;
        objeto_json["moistureThreshold"]=waterMoistureSlider_;              // Creamos el mensaje y lo enviamos con el valor de la rueda
        QJsonDocument mensaje(objeto_json);
        //SendMessage(mensaje);
        objeto_json["temperatureThreshold"]=waterTemperatureSlider_;              // TENGO QUE HACER ALGO PARA QUE SEA SOLO UN MENSAJE CON LOS DOS TH
        mensaje = QJsonDocument (objeto_json);
        SendMessage(mensaje);
    }
    else
    {
        QMessageBox ventanaPopUp(QMessageBox::Information,tr("Evento"),tr("Thresholds cant be the minimum"),QMessageBox::Ok,this,Qt::Popup);
        ventanaPopUp.exec();
    }
}

//Envia el valor de frecuencia de medicion de la temperatura cuando se cambia su valor desde la interfaz
void GUIPanel::on_freq_valueChanged(double value)
{
    QJsonObject objeto_json;
    objeto_json["measuresFreq"]=value;              // Creamos el mensaje y lo enviamos con el valor de la rueda
    QJsonDocument mensaje(objeto_json);
    SendMessage(mensaje);
}



