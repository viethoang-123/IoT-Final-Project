#include "sensors.h"
#include <Arduino.h>
#include <Wire.h>
#include <DallasTemperature.h>
#include <MPU6050_light.h>

extern const int ACS_PIN;
extern const float ADC_VREF;
extern const int ADC_RES;
extern const float SENSITIVITY;
extern float offsetVoltage;

extern MPU6050 mpu;
extern DallasTemperature sensors;

extern float axBuf[], ayBuf[], azBuf[];
extern int bufIndex;
extern unsigned long lastSampleUs;
extern unsigned long sampleIntervalUs;
extern const int BUFFER_N;

void sensors_init()
{
    sensors.begin();
    mpu.begin();
    lastSampleUs = micros();
}

void sensors_update()
{
    unsigned long nowUs = micros();
    if (nowUs - lastSampleUs >= sampleIntervalUs)
    {
        lastSampleUs += sampleIntervalUs;
        mpu.update();

        axBuf[bufIndex] = mpu.getAccX();
        ayBuf[bufIndex] = mpu.getAccY();
        azBuf[bufIndex] = mpu.getAccZ();

        bufIndex = (bufIndex + 1) % BUFFER_N;
    }
}

float readTemperature()
{
    sensors.requestTemperatures();
    return sensors.getTempCByIndex(0);
}

float readVoltage()
{
    return analogRead(34) * (3.3 / 4095.0) * 11.0;
}

float readCurrent()
{
    const int N = 200;
    long sum = 0;

    for (int i = 0; i < N; i++)
    {
        sum += analogRead(ACS_PIN);
        delayMicroseconds(200);
    }

    float raw = sum / (float)N;
    float v = raw * (ADC_VREF / ADC_RES);
    float diff = v - offsetVoltage;

    float current = -(diff / SENSITIVITY);
    return (fabs(current) < 0.05) ? 0 : current;
}

float readVibrationRMS()
{
    double s = 0;
    for (int i = 0; i < BUFFER_N; i++)
    {
        float x = axBuf[i], y = ayBuf[i], z = azBuf[i];
        s += (x*x + y*y + z*z);
    }
    return sqrt(s / BUFFER_N);
}
