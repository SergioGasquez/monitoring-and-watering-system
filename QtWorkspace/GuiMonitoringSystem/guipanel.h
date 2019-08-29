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

//QT4:QT_USE_NAMESPACE_SERIALPORT

class GUIPanel : public QWidget
{
    Q_OBJECT

public:
    explicit GUIPanel(QWidget *parent = 0);
    ~GUIPanel(); // Da problemas

private slots:
    void onMQTT_Received(const QMQTT::Message &message);
    void onMQTT_Connected(void);
    void onMQTT_Connacked(quint8 ack);
    void onMQTT_subscribed(const QString &topic);
    void onMQTT_subacked(quint16 msgid, quint8 qos);

    //Slots botones (creados automaticamente)
    void on_connectButton_clicked();            // Funcion que se ejecuta al pulsar el boton "Start"
    void on_stateButton_clicked();           // Funcion que se ejecuta al pulsar el boton "State"
    void on_pingButton_clicked();           // Funcion que se ejecuta al pulsar el boton "Ping"
    void on_tempEnable_toggled(bool checked);
    void on_moistEnable_toggled(bool checked);
    void on_freq_valueChanged(double value);

private:
    // Funciones privadas
    void pingRecived(bool response = false);
    void startClient();
    void processError(const QString &s);
    void activateRunButton();
    void SendMessage(QJsonDocument mensaje);

    Ui::GUIPanel *ui;
    int transactionCount;
    QMQTT::Client *_client;
    bool connected;

    // Variables que controlan las respuestas que esperamos
    bool pinged_;
    bool temperatureEnabled_;
    bool moistureEnable_;
    bool waterLevelEnable_;


    // Moisture graph
    double xValMoisture[NMAX];          // Sample number
    double yValMoisture[NMAX];          // Moisture
    QwtPlotGrid  *mMoisture_Grid;
    QwtPlotCurve *m_Moisture;

    // Grafica Ambient
    double xVal[NMAX];          // Sample number
    double yValTemp[NMAX];         // Temperature
    double yValHum[NMAX];         // Humidity ratio
    QwtPlotGrid  *m_Ambient_Grid;
    QwtPlotCurve *m_curve_temp;
    QwtPlotCurve *m_curve_hum;




};

#endif // GUIPANEL_H
