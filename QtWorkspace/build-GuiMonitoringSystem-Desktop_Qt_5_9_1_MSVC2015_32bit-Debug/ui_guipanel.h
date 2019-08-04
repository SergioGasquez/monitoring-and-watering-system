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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include "analogwidgets/thermometer.h"
#include "qwt_counter.h"
#include "qwt_plot.h"

QT_BEGIN_NAMESPACE

class Ui_GUIPanel
{
public:
    QPushButton *pingButton;
    QLineEdit *hostAddress;
    QPushButton *stateButton;
    QLabel *statusLabel;
    QLabel *hostLabel;
    QPushButton *startButton;
    QTabWidget *tabWidget;
    QWidget *generalTab;
    QGroupBox *groupAmbient;
    ThermoMeter *temperature;
    QwtCounter *tempFreq;
    QLabel *labelFreqTemp;
    QCheckBox *tempEnable;
    QLabel *labelMinutesTemp;
    QLabel *labelTemperature;
    QLabel *labelHumidity;
    QCheckBox *humEnable;
    ThermoMeter *humidity;
    QGroupBox *groupMoisture;
    ThermoMeter *moisture;
    QwtCounter *moistFreq;
    QLabel *labelMoistureFreq;
    QCheckBox *moistEnable;
    QLabel *labelMoisutreMInutes;
    QGroupBox *groupWaterLevel;
    ThermoMeter *waterLevel;
    QwtCounter *freqWaterLevel;
    QLabel *freqWaterLevel_2;
    QCheckBox *waterLevelEnable;
    QLabel *labelMinutesWaterLEvel;
    QWidget *ambienTab;
    QWidget *MoistureTab;
    QwtPlot *moistureGraph;

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
        startButton = new QPushButton(GUIPanel);
        startButton->setObjectName(QStringLiteral("startButton"));
        startButton->setGeometry(QRect(350, 0, 91, 41));
        startButton->setFont(font);
        tabWidget = new QTabWidget(GUIPanel);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(10, 60, 981, 531));
        generalTab = new QWidget();
        generalTab->setObjectName(QStringLiteral("generalTab"));
        groupAmbient = new QGroupBox(generalTab);
        groupAmbient->setObjectName(QStringLiteral("groupAmbient"));
        groupAmbient->setGeometry(QRect(10, 60, 451, 391));
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
        tempFreq = new QwtCounter(groupAmbient);
        tempFreq->setObjectName(QStringLiteral("tempFreq"));
        tempFreq->setGeometry(QRect(230, 320, 131, 31));
        QFont font4;
        font4.setPointSize(15);
        tempFreq->setFont(font4);
        tempFreq->setMaximum(90);
        tempFreq->setSingleStep(1);
        labelFreqTemp = new QLabel(groupAmbient);
        labelFreqTemp->setObjectName(QStringLiteral("labelFreqTemp"));
        labelFreqTemp->setGeometry(QRect(130, 320, 101, 31));
        QFont font5;
        font5.setPointSize(13);
        labelFreqTemp->setFont(font5);
        tempEnable = new QCheckBox(groupAmbient);
        tempEnable->setObjectName(QStringLiteral("tempEnable"));
        tempEnable->setGeometry(QRect(10, 60, 131, 41));
        QFont font6;
        font6.setPointSize(14);
        font6.setItalic(false);
        tempEnable->setFont(font6);
        labelMinutesTemp = new QLabel(groupAmbient);
        labelMinutesTemp->setObjectName(QStringLiteral("labelMinutesTemp"));
        labelMinutesTemp->setGeometry(QRect(280, 350, 51, 16));
        QFont font7;
        font7.setPointSize(11);
        labelMinutesTemp->setFont(font7);
        labelTemperature = new QLabel(groupAmbient);
        labelTemperature->setObjectName(QStringLiteral("labelTemperature"));
        labelTemperature->setGeometry(QRect(40, 30, 151, 41));
        labelTemperature->setFont(font1);
        labelHumidity = new QLabel(groupAmbient);
        labelHumidity->setObjectName(QStringLiteral("labelHumidity"));
        labelHumidity->setGeometry(QRect(250, 30, 151, 41));
        labelHumidity->setFont(font1);
        humEnable = new QCheckBox(groupAmbient);
        humEnable->setObjectName(QStringLiteral("humEnable"));
        humEnable->setGeometry(QRect(230, 60, 131, 41));
        humEnable->setFont(font6);
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
        groupMoisture->setGeometry(QRect(470, 60, 231, 391));
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
        moistFreq = new QwtCounter(groupMoisture);
        moistFreq->setObjectName(QStringLiteral("moistFreq"));
        moistFreq->setGeometry(QRect(40, 330, 141, 31));
        moistFreq->setFont(font4);
        moistFreq->setMaximum(90);
        moistFreq->setSingleStep(1);
        labelMoistureFreq = new QLabel(groupMoisture);
        labelMoistureFreq->setObjectName(QStringLiteral("labelMoistureFreq"));
        labelMoistureFreq->setGeometry(QRect(10, 300, 101, 31));
        labelMoistureFreq->setFont(font5);
        moistEnable = new QCheckBox(groupMoisture);
        moistEnable->setObjectName(QStringLiteral("moistEnable"));
        moistEnable->setGeometry(QRect(20, 40, 131, 41));
        moistEnable->setFont(font6);
        labelMoisutreMInutes = new QLabel(groupMoisture);
        labelMoisutreMInutes->setObjectName(QStringLiteral("labelMoisutreMInutes"));
        labelMoisutreMInutes->setGeometry(QRect(90, 360, 51, 16));
        labelMoisutreMInutes->setFont(font7);
        groupWaterLevel = new QGroupBox(generalTab);
        groupWaterLevel->setObjectName(QStringLiteral("groupWaterLevel"));
        groupWaterLevel->setGeometry(QRect(710, 60, 231, 391));
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
        freqWaterLevel = new QwtCounter(groupWaterLevel);
        freqWaterLevel->setObjectName(QStringLiteral("freqWaterLevel"));
        freqWaterLevel->setGeometry(QRect(40, 330, 141, 31));
        freqWaterLevel->setFont(font4);
        freqWaterLevel->setMaximum(90);
        freqWaterLevel->setSingleStep(1);
        freqWaterLevel_2 = new QLabel(groupWaterLevel);
        freqWaterLevel_2->setObjectName(QStringLiteral("freqWaterLevel_2"));
        freqWaterLevel_2->setGeometry(QRect(10, 300, 101, 31));
        freqWaterLevel_2->setFont(font5);
        waterLevelEnable = new QCheckBox(groupWaterLevel);
        waterLevelEnable->setObjectName(QStringLiteral("waterLevelEnable"));
        waterLevelEnable->setGeometry(QRect(20, 40, 131, 41));
        waterLevelEnable->setFont(font6);
        labelMinutesWaterLEvel = new QLabel(groupWaterLevel);
        labelMinutesWaterLEvel->setObjectName(QStringLiteral("labelMinutesWaterLEvel"));
        labelMinutesWaterLEvel->setGeometry(QRect(90, 360, 51, 16));
        labelMinutesWaterLEvel->setFont(font7);
        tabWidget->addTab(generalTab, QString());
        ambienTab = new QWidget();
        ambienTab->setObjectName(QStringLiteral("ambienTab"));
        ambienTab->setEnabled(true);
        tabWidget->addTab(ambienTab, QString());
        MoistureTab = new QWidget();
        MoistureTab->setObjectName(QStringLiteral("MoistureTab"));
        moistureGraph = new QwtPlot(MoistureTab);
        moistureGraph->setObjectName(QStringLiteral("moistureGraph"));
        moistureGraph->setGeometry(QRect(70, 60, 431, 281));
        tabWidget->addTab(MoistureTab, QString());

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
        startButton->setText(QApplication::translate("GUIPanel", "Start", Q_NULLPTR));
        groupAmbient->setTitle(QApplication::translate("GUIPanel", "Ambient Conditions", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        temperature->setToolTip(QApplication::translate("GUIPanel", "Shows the temperature", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        temperature->setWhatsThis(QApplication::translate("GUIPanel", "The bar meter widget displays the pressure attached to it", Q_NULLPTR));
#endif // QT_NO_WHATSTHIS
        temperature->setSuffix(QApplication::translate("GUIPanel", " [\302\272C]", Q_NULLPTR));
        labelFreqTemp->setText(QApplication::translate("GUIPanel", "Frequency:", Q_NULLPTR));
        tempEnable->setText(QApplication::translate("GUIPanel", "Enable", Q_NULLPTR));
        labelMinutesTemp->setText(QApplication::translate("GUIPanel", "Minutes", Q_NULLPTR));
        labelTemperature->setText(QApplication::translate("GUIPanel", "Temperature", Q_NULLPTR));
        labelHumidity->setText(QApplication::translate("GUIPanel", "Humidity Ratio", Q_NULLPTR));
        humEnable->setText(QApplication::translate("GUIPanel", "Enable", Q_NULLPTR));
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
        labelMoistureFreq->setText(QApplication::translate("GUIPanel", "Frequency:", Q_NULLPTR));
        moistEnable->setText(QApplication::translate("GUIPanel", "Enable", Q_NULLPTR));
        labelMoisutreMInutes->setText(QApplication::translate("GUIPanel", "Minutes", Q_NULLPTR));
        groupWaterLevel->setTitle(QApplication::translate("GUIPanel", "Water Level", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        waterLevel->setToolTip(QApplication::translate("GUIPanel", "Shows the temperature", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        waterLevel->setWhatsThis(QApplication::translate("GUIPanel", "The bar meter widget displays the pressure attached to it", Q_NULLPTR));
#endif // QT_NO_WHATSTHIS
        waterLevel->setSuffix(QApplication::translate("GUIPanel", " [%]", Q_NULLPTR));
        freqWaterLevel_2->setText(QApplication::translate("GUIPanel", "Frequency:", Q_NULLPTR));
        waterLevelEnable->setText(QApplication::translate("GUIPanel", "Enable", Q_NULLPTR));
        labelMinutesWaterLEvel->setText(QApplication::translate("GUIPanel", "Minutes", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(generalTab), QApplication::translate("GUIPanel", "Tab 1", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(ambienTab), QApplication::translate("GUIPanel", "Page", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(MoistureTab), QApplication::translate("GUIPanel", "Tab 2", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class GUIPanel: public Ui_GUIPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUIPANEL_H
