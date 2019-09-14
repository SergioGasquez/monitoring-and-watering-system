#include "temperatureSensor.h"



void setupTemperatureSensor()
{
    //
    // Inicializa la biblioteca que gestiona el bus I2C. (movido desde main(...))
    //
    if(I2C_IF_Open(I2C_MASTER_MODE_FST) < 0)
    {
     while (1);
    }
}


int getTemperature(float *temperature, float *humidity)
{
    unsigned char command[2];
    command[0] = 0x2C;
    command[1] = 0x06;

    unsigned char readData[6];
    I2C_IF_Write(TEMPERATURE_DEV_ADDR, &command[0], 2, 1);
    osi_Sleep(10);                                              // Sin esto me da errores.
    I2C_IF_Read(TEMPERATURE_DEV_ADDR, &readData[0], 6);
    bool checkMeasureTemp = crcCheck(readData[0], readData[1], readData[2]);
    bool checkMeasureHum = crcCheck(readData[3], readData[4], readData[5]);
#ifdef DEBUG_TEMPERATURESENSOR
    unsigned char i;
    for (i =0; i < 6; i++)
    {
        UARTprintf("\nDatos Leidos[%d] = %X", i, readData[i]);
    }
#endif //DEBUG_TEMPERATURESENSOR
    if(checkMeasureTemp && checkMeasureHum)
    {
        int temp = (int)(readData[0] << 8) | readData[1];
        int hum = (int)(readData[3] << 8) | readData[4];
        *temperature = (((float)temp/65535)*175)-45;
        *humidity = ((float)hum/65535)*100;
        return SUCCESS;
    }
    else
    {
        return FAILURE;
    }

    return FAILURE;
}


bool crcCheck(unsigned char MSB, unsigned char LSB, unsigned char CRC)
{
    unsigned char crc = 0xFF;
    unsigned char i;
    crc ^= MSB;
    for (i = 0; i < 8; i++)
    {
        crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
    }
    crc ^= LSB;
    for (i = 0; i < 8; i++)
    {
        crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
    }
    if (crc==CRC)
    {
        return true;
    }
    else
    {
        return false;
    }
}


