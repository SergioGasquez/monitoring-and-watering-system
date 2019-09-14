#ifndef GUIPANEL_H
#define GUIPANEL_H

#include <QWidget>
#include <QtSerialPort/qserialport.h>
#include "qmqtt.h"

#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>

#define NMAX 300

namespace Ui {
class GUIPanel;
}

class GUIPanel : public QWidget
{
    Q_OBJECT

public:
    explicit GUIPanel(QWidget *parent = 0);                     // Constructor
    ~GUIPanel();                                                // Destructor

private slots:
    void onMQTT_Received(const QMQTT::Message &message);        // Funcion que se ejcuta cuando recivimos un mensaje
    void onMQTT_Connected(void);                                // Funcion que se ejecuta cuando se conecta al broker
    void onMQTT_Connacked(quint8 ack);                          // Funcion que se ejecuta al recibir el ack del Conect
    void onMQTT_subscribed(const QString &topic);               // Funcion que se ejecuta al subscribirse a algun topic
    void onMQTT_subacked(quint16 msgid, quint8 qos);            // Funcion que se ejecuta al recibir el ack del Subscribe

    //Slots botones (creados automaticamente)
    void on_connectButton_clicked();                            // Funcion que se ejecuta al pulsar el boton "Connect"
    void on_stateButton_clicked();                              // Funcion que se ejecuta al pulsar el boton "State"
    void on_pingButton_clicked();                               // Funcion que se ejecuta al pulsar el boton "Ping"
    void on_measurmentSwitch_valueChanged(int value);           // Funcion que se ejecuta al cambiar el valor del swith de iniciar medidas
    void on_waterTimeSlider_valueChanged(double value);         // Funcion que se ejecuta al cambiar el valor del slider de tiempo de riego
    void on_waterButton_clicked();                              // Funcion que se ejecuta al pulsar el boton "Water"
    void on_waterMoistureSlider_valueChanged(double value);     // Funcion que se ejecuta al cambiar el valor del slider de threshold de humedad
    void on_waterTemperatureSlider_valueChanged(double value);  // Funcion que se ejecuta al cambiar el valor del slider de threshold de temperetura
    void on_waterParamsSet_clicked();                           // Funcion que se ejecuta al pulsar el boton "Set Thresholds"
    void on_freq_valueChanged(double value);                    // Funcion que se ejecuta al cambiar la frecuencia con la que se miden los sensores

private:
    // Funciones privadas
    void pingRecived(bool response = false);                    // Comprueba la respuesta del ping
    void startClient();                                         // Funcion que se conecta con la direccion que introducimos
    void processError(const QString &s);                        // Procesa los errores, en caso de que ocurran
    void activateRunButton();                                   // Habilita el boton de "Connect" y deshabilita el resto
    void SendMessage(QJsonDocument mensaje);                    // Envia un mensaje a la red MQTT
    void saveData(double temperature, double humidity, double moisture);    // Almacena los datos en un archivo .txt

    Ui::GUIPanel *ui;
    int transactionCount;
    QMQTT::Client *_client;
    bool connected;

    // Variables que controlan las respuestas que esperamos
    bool pinged_;
    bool fileInitialized_;
    std::string filename_;
    char wateringTime_;
    char waterMoistureSlider_;
    char waterTemperatureSlider_;

    // Moisture graph
    double xValMoisture[NMAX];          // Sample number
    double yValMoisture[NMAX];          // Moisture
    QwtPlotGrid  *mMoisture_Grid;
    QwtPlotCurve *m_Moisture;

    // Grafica Ambient
    double xVal[NMAX];              // Sample number
    double yValTemp[NMAX];          // Temperature
    double yValHum[NMAX];           // Humidity ratio
    QwtPlotGrid  *m_Ambient_Grid;
    QwtPlotCurve *m_curve_temp;
    QwtPlotCurve *m_curve_hum;




};

#endif // GUIPANEL_H
