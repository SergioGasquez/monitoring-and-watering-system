/********************************************************************************
** Form generated from reading UI file 'guipanel.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUIPANEL_H
#define UI_GUIPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include "analogwidgets/thermometer.h"
#include "qwt_counter.h"
#include "qwt_plot.h"
#include "qwt_slider.h"
#include "qwt_text_label.h"

QT_BEGIN_NAMESPACE

class Ui_GUIPanel
{
public:
    QPushButton *pingButton;
    QLineEdit *hostAddress;
    QPushButton *stateButton;
    QLabel *statusLabel;
    QLabel *hostLabel;
    QPushButton *connectButton;
    QTabWidget *tabWidget;
    QWidget *generalTab;
    QGroupBox *groupAmbient;
    ThermoMeter *temperature;
    QLabel *labelTemperature;
    QLabel *labelHumidity;
    ThermoMeter *humidity;
    QGroupBox *groupMoisture;
    ThermoMeter *moisture;
    QGroupBox *groupWaterLevel;
    ThermoMeter *waterLevel;
    QLabel *freqLabel;
    QwtCounter *freq;
    QLabel *labelMinutesWaterLEvel;
    QWidget *ambienTab;
    QwtPlot *ambientGraph;
    QWidget *moistureTab;
    QwtPlot *moistureGraph;
    QWidget *waterTab;
    QPushButton *waterButton;
    QPushButton *waterParamsSet;
    QwtTextLabel *waterTimeLabel;
    QwtTextLabel *waterParamsLabel;
    QwtSlider *waterTimeSlider;
    QwtSlider *waterTemperatureSlider;
    QwtSlider *waterMoistureSlider;
    QwtTextLabel *temperatureLabel2;
    QwtTextLabel *soilMostLabel;
    QwtTextLabel *secondsLabel;
    QwtTextLabel *tempUnitsLabel;
    QwtTextLabel *moistUnitsLabel;
    QSlider *measurmentSwitch;
    QwtTextLabel *startLabel;
    QwtTextLabel *onLabel;
    QwtTextLabel *offLabel;

    void setupUi(QWidget *GUIPanel)
    {
        if (GUIPanel->objectName().isEmpty())
            GUIPanel->setObjectName(QStringLiteral("GUIPanel"));
        GUIPanel->resize(1011, 658);
        pingButton = new QPushButton(GUIPanel);
        pingButton->setObjectName(QStringLiteral("pingButton"));
        pingButton->setGeometry(QRect(450, 0, 91, 41));
        QFont font;
        font.setPointSize(12);
        pingButton->setFont(font);
        hostAddress = new QLineEdit(GUIPanel);
        hostAddress->setObjectName(QStringLiteral("hostAddress"));
        hostAddress->setGeometry(QRect(160, 10, 171, 31));
        hostAddress->setFont(font);
        stateButton = new QPushButton(GUIPanel);
        stateButton->setObjectName(QStringLiteral("stateButton"));
        stateButton->setGeometry(QRect(10, 610, 71, 31));
        stateButton->setFont(font);
        statusLabel = new QLabel(GUIPanel);
        statusLabel->setObjectName(QStringLiteral("statusLabel"));
        statusLabel->setGeometry(QRect(90, 620, 331, 21));
        statusLabel->setFont(font);
        hostLabel = new QLabel(GUIPanel);
        hostLabel->setObjectName(QStringLiteral("hostLabel"));
        hostLabel->setGeometry(QRect(20, 0, 151, 41));
        QFont font1;
        font1.setPointSize(16);
        hostLabel->setFont(font1);
        connectButton = new QPushButton(GUIPanel);
        connectButton->setObjectName(QStringLiteral("connectButton"));
        connectButton->setGeometry(QRect(350, 0, 91, 41));
        connectButton->setFont(font);
        tabWidget = new QTabWidget(GUIPanel);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(10, 60, 981, 531));
        generalTab = new QWidget();
        generalTab->setObjectName(QStringLiteral("generalTab"));
        groupAmbient = new QGroupBox(generalTab);
        groupAmbient->setObjectName(QStringLiteral("groupAmbient"));
        groupAmbient->setGeometry(QRect(10, 50, 451, 321));
        QFont font2;
        font2.setPointSize(18);
        groupAmbient->setFont(font2);
        temperature = new ThermoMeter(groupAmbient);
        temperature->setObjectName(QStringLiteral("temperature"));
        temperature->setGeometry(QRect(10, 90, 201, 201));
        QFont font3;
        font3.setPointSize(13);
        font3.setStrikeOut(false);
        font3.setKerning(true);
        temperature->setFont(font3);
        temperature->setMinimum(5);
        temperature->setMaximum(60);
        temperature->setValue(0);
        temperature->setValueOffset(275);
        temperature->setDigitOffset(10);
        labelTemperature = new QLabel(groupAmbient);
        labelTemperature->setObjectName(QStringLiteral("labelTemperature"));
        labelTemperature->setGeometry(QRect(50, 60, 151, 41));
        labelTemperature->setFont(font1);
        labelHumidity = new QLabel(groupAmbient);
        labelHumidity->setObjectName(QStringLiteral("labelHumidity"));
        labelHumidity->setGeometry(QRect(260, 60, 151, 41));
        labelHumidity->setFont(font1);
        humidity = new ThermoMeter(groupAmbient);
        humidity->setObjectName(QStringLiteral("humidity"));
        humidity->setGeometry(QRect(220, 90, 201, 201));
        humidity->setFont(font3);
        humidity->setMinimum(20);
        humidity->setMaximum(80);
        humidity->setValue(0);
        humidity->setValueOffset(276);
        humidity->setDigitOffset(10);
        groupMoisture = new QGroupBox(generalTab);
        groupMoisture->setObjectName(QStringLiteral("groupMoisture"));
        groupMoisture->setGeometry(QRect(470, 50, 231, 321));
        groupMoisture->setFont(font2);
        moisture = new ThermoMeter(groupMoisture);
        moisture->setObjectName(QStringLiteral("moisture"));
        moisture->setGeometry(QRect(10, 90, 201, 201));
        moisture->setFont(font3);
        moisture->setMinimum(0);
        moisture->setMaximum(100);
        moisture->setValue(0);
        moisture->setValueOffset(275);
        moisture->setDigitOffset(10);
        groupWaterLevel = new QGroupBox(generalTab);
        groupWaterLevel->setObjectName(QStringLiteral("groupWaterLevel"));
        groupWaterLevel->setGeometry(QRect(710, 50, 231, 321));
        groupWaterLevel->setFont(font2);
        waterLevel = new ThermoMeter(groupWaterLevel);
        waterLevel->setObjectName(QStringLiteral("waterLevel"));
        waterLevel->setGeometry(QRect(10, 90, 201, 201));
        waterLevel->setFont(font3);
        waterLevel->setMinimum(0);
        waterLevel->setMaximum(100);
        waterLevel->setValue(0);
        waterLevel->setValueOffset(275);
        waterLevel->setDigitOffset(10);
        freqLabel = new QLabel(generalTab);
        freqLabel->setObjectName(QStringLiteral("freqLabel"));
        freqLabel->setGeometry(QRect(190, 400, 241, 31));
        freqLabel->setFont(font1);
        freq = new QwtCounter(generalTab);
        freq->setObjectName(QStringLiteral("freq"));
        freq->setGeometry(QRect(440, 400, 141, 31));
        QFont font4;
        font4.setPointSize(15);
        freq->setFont(font4);
        freq->setMinimum(1);
        freq->setMaximum(90);
        freq->setSingleStep(1);
        labelMinutesWaterLEvel = new QLabel(generalTab);
        labelMinutesWaterLEvel->setObjectName(QStringLiteral("labelMinutesWaterLEvel"));
        labelMinutesWaterLEvel->setGeometry(QRect(590, 410, 91, 16));
        labelMinutesWaterLEvel->setFont(font4);
        tabWidget->addTab(generalTab, QString());
        ambienTab = new QWidget();
        ambienTab->setObjectName(QStringLiteral("ambienTab"));
        ambienTab->setEnabled(true);
        ambientGraph = new QwtPlot(ambienTab);
        ambientGraph->setObjectName(QStringLiteral("ambientGraph"));
        ambientGraph->setGeometry(QRect(40, 30, 871, 431));
        tabWidget->addTab(ambienTab, QString());
        moistureTab = new QWidget();
        moistureTab->setObjectName(QStringLiteral("moistureTab"));
        moistureGraph = new QwtPlot(moistureTab);
        moistureGraph->setObjectName(QStringLiteral("moistureGraph"));
        moistureGraph->setGeometry(QRect(40, 30, 871, 431));
        tabWidget->addTab(moistureTab, QString());
        waterTab = new QWidget();
        waterTab->setObjectName(QStringLiteral("waterTab"));
        waterButton = new QPushButton(waterTab);
        waterButton->setObjectName(QStringLiteral("waterButton"));
        waterButton->setGeometry(QRect(640, 100, 121, 51));
        waterButton->setFont(font);
        waterParamsSet = new QPushButton(waterTab);
        waterParamsSet->setObjectName(QStringLiteral("waterParamsSet"));
        waterParamsSet->setGeometry(QRect(620, 320, 171, 71));
        waterParamsSet->setFont(font);
        waterTimeLabel = new QwtTextLabel(waterTab);
        waterTimeLabel->setObjectName(QStringLiteral("waterTimeLabel"));
        waterTimeLabel->setGeometry(QRect(20, 30, 201, 41));
        QFont font5;
        font5.setPointSize(16);
        font5.setBold(true);
        font5.setWeight(75);
        waterTimeLabel->setFont(font5);
        waterTimeLabel->setIndent(6);
        waterParamsLabel = new QwtTextLabel(waterTab);
        waterParamsLabel->setObjectName(QStringLiteral("waterParamsLabel"));
        waterParamsLabel->setGeometry(QRect(30, 170, 251, 41));
        waterParamsLabel->setFont(font5);
        waterParamsLabel->setIndent(6);
        waterTimeSlider = new QwtSlider(waterTab);
        waterTimeSlider->setObjectName(QStringLiteral("waterTimeSlider"));
        waterTimeSlider->setGeometry(QRect(130, 80, 431, 61));
        waterTimeSlider->setUpperBound(60);
        waterTimeSlider->setOrientation(Qt::Horizontal);
        waterTemperatureSlider = new QwtSlider(waterTab);
        waterTemperatureSlider->setObjectName(QStringLiteral("waterTemperatureSlider"));
        waterTemperatureSlider->setGeometry(QRect(150, 260, 71, 191));
        waterTemperatureSlider->setLowerBound(5);
        waterTemperatureSlider->setUpperBound(60);
        waterTemperatureSlider->setOrientation(Qt::Vertical);
        waterMoistureSlider = new QwtSlider(waterTab);
        waterMoistureSlider->setObjectName(QStringLiteral("waterMoistureSlider"));
        waterMoistureSlider->setGeometry(QRect(430, 260, 71, 191));
        waterMoistureSlider->setLowerBound(0);
        waterMoistureSlider->setUpperBound(100);
        waterMoistureSlider->setOrientation(Qt::Vertical);
        temperatureLabel2 = new QwtTextLabel(waterTab);
        temperatureLabel2->setObjectName(QStringLiteral("temperatureLabel2"));
        temperatureLabel2->setGeometry(QRect(10, 310, 151, 41));
        temperatureLabel2->setFont(font1);
        temperatureLabel2->setIndent(6);
        soilMostLabel = new QwtTextLabel(waterTab);
        soilMostLabel->setObjectName(QStringLiteral("soilMostLabel"));
        soilMostLabel->setGeometry(QRect(290, 310, 151, 41));
        soilMostLabel->setFont(font1);
        soilMostLabel->setIndent(6);
        secondsLabel = new QwtTextLabel(waterTab);
        secondsLabel->setObjectName(QStringLiteral("secondsLabel"));
        secondsLabel->setGeometry(QRect(290, 140, 100, 20));
        tempUnitsLabel = new QwtTextLabel(waterTab);
        tempUnitsLabel->setObjectName(QStringLiteral("tempUnitsLabel"));
        tempUnitsLabel->setGeometry(QRect(220, 260, 31, 20));
        moistUnitsLabel = new QwtTextLabel(waterTab);
        moistUnitsLabel->setObjectName(QStringLiteral("moistUnitsLabel"));
        moistUnitsLabel->setGeometry(QRect(500, 260, 31, 20));
        tabWidget->addTab(waterTab, QString());
        measurmentSwitch = new QSlider(GUIPanel);
        measurmentSwitch->setObjectName(QStringLiteral("measurmentSwitch"));
        measurmentSwitch->setGeometry(QRect(780, 10, 61, 31));
        measurmentSwitch->setMaximum(1);
        measurmentSwitch->setOrientation(Qt::Horizontal);
        startLabel = new QwtTextLabel(GUIPanel);
        startLabel->setObjectName(QStringLiteral("startLabel"));
        startLabel->setGeometry(QRect(610, 0, 151, 51));
        startLabel->setFont(font4);
        onLabel = new QwtTextLabel(GUIPanel);
        onLabel->setObjectName(QStringLiteral("onLabel"));
        onLabel->setGeometry(QRect(810, 40, 51, 20));
        QFont font6;
        font6.setPointSize(10);
        onLabel->setFont(font6);
        offLabel = new QwtTextLabel(GUIPanel);
        offLabel->setObjectName(QStringLiteral("offLabel"));
        offLabel->setGeometry(QRect(760, 40, 51, 20));
        offLabel->setFont(font6);

        retranslateUi(GUIPanel);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(GUIPanel);
    } // setupUi

    void retranslateUi(QWidget *GUIPanel)
    {
        GUIPanel->setWindowTitle(QApplication::translate("GUIPanel", "GUIPanel", Q_NULLPTR));
        pingButton->setText(QApplication::translate("GUIPanel", "Ping", Q_NULLPTR));
        hostAddress->setText(QApplication::translate("GUIPanel", "localhost", Q_NULLPTR));
        stateButton->setText(QApplication::translate("GUIPanel", "State:", Q_NULLPTR));
        statusLabel->setText(QApplication::translate("GUIPanel", "Stoped", Q_NULLPTR));
        hostLabel->setText(QApplication::translate("GUIPanel", "MQTT Broker: ", Q_NULLPTR));
        connectButton->setText(QApplication::translate("GUIPanel", "Connect", Q_NULLPTR));
        groupAmbient->setTitle(QApplication::translate("GUIPanel", "Ambient Conditions", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        temperature->setToolTip(QApplication::translate("GUIPanel", "Shows the temperature", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        temperature->setWhatsThis(QApplication::translate("GUIPanel", "The bar meter widget displays the pressure attached to it", Q_NULLPTR));
#endif // QT_NO_WHATSTHIS
        temperature->setSuffix(QApplication::translate("GUIPanel", " [\302\272C]", Q_NULLPTR));
        labelTemperature->setText(QApplication::translate("GUIPanel", "Temperature", Q_NULLPTR));
        labelHumidity->setText(QApplication::translate("GUIPanel", "Humidity Ratio", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        humidity->setToolTip(QApplication::translate("GUIPanel", "Shows the temperature", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        humidity->setWhatsThis(QApplication::translate("GUIPanel", "The bar meter widget displays the pressure attached to it", Q_NULLPTR));
#endif // QT_NO_WHATSTHIS
        humidity->setSuffix(QApplication::translate("GUIPanel", " [%]", Q_NULLPTR));
        groupMoisture->setTitle(QApplication::translate("GUIPanel", "Moisture", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        moisture->setToolTip(QApplication::translate("GUIPanel", "Shows the temperature", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        moisture->setWhatsThis(QApplication::translate("GUIPanel", "The bar meter widget displays the pressure attached to it", Q_NULLPTR));
#endif // QT_NO_WHATSTHIS
        moisture->setSuffix(QApplication::translate("GUIPanel", " [%]", Q_NULLPTR));
        groupWaterLevel->setTitle(QApplication::translate("GUIPanel", "Water Level", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        waterLevel->setToolTip(QApplication::translate("GUIPanel", "Shows the temperature", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        waterLevel->setWhatsThis(QApplication::translate("GUIPanel", "The bar meter widget displays the pressure attached to it", Q_NULLPTR));
#endif // QT_NO_WHATSTHIS
        waterLevel->setSuffix(QApplication::translate("GUIPanel", " [%]", Q_NULLPTR));
        freqLabel->setText(QApplication::translate("GUIPanel", "Measurement Frequency:", Q_NULLPTR));
        labelMinutesWaterLEvel->setText(QApplication::translate("GUIPanel", "Minutes", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(generalTab), QApplication::translate("GUIPanel", "General", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(ambienTab), QApplication::translate("GUIPanel", "Ambient Graph", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(moistureTab), QApplication::translate("GUIPanel", "Moisture Graph", Q_NULLPTR));
        waterButton->setText(QApplication::translate("GUIPanel", "Water", Q_NULLPTR));
        waterParamsSet->setText(QApplication::translate("GUIPanel", "Set water parameters", Q_NULLPTR));
        waterTimeLabel->setPlainText(QApplication::translate("GUIPanel", "Water over time:", Q_NULLPTR));
        waterParamsLabel->setPlainText(QApplication::translate("GUIPanel", "Water with thresholds: ", Q_NULLPTR));
        temperatureLabel2->setPlainText(QApplication::translate("GUIPanel", "Temperature:", Q_NULLPTR));
        soilMostLabel->setPlainText(QApplication::translate("GUIPanel", "Soil Moisture:", Q_NULLPTR));
        secondsLabel->setPlainText(QApplication::translate("GUIPanel", "Seconds", Q_NULLPTR));
        tempUnitsLabel->setPlainText(QApplication::translate("GUIPanel", "\302\272C", Q_NULLPTR));
        moistUnitsLabel->setPlainText(QApplication::translate("GUIPanel", "%", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(waterTab), QApplication::translate("GUIPanel", "Watering", Q_NULLPTR));
        startLabel->setPlainText(QApplication::translate("GUIPanel", "Measurements: ", Q_NULLPTR));
        onLabel->setPlainText(QApplication::translate("GUIPanel", "On", Q_NULLPTR));
        offLabel->setPlainText(QApplication::translate("GUIPanel", "Off", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class GUIPanel: public Ui_GUIPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUIPANEL_H
