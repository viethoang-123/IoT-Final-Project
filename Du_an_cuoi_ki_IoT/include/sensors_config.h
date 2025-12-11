#pragma once
#include <DallasTemperature.h>
#include <MPU6050_light.h>

// ===== SENSOR HARDWARE =====
extern OneWire oneWire;
extern DallasTemperature sensors;
extern MPU6050 mpu;

// ===== ACS712 =====
extern const int ACS_PIN;
extern const float ADC_VREF;
extern const int ADC_RES;
extern const float SENSITIVITY;
extern float offsetVoltage;

// ===== VOLTAGE DIVIDER =====
extern const int adcPin;

// ===== MPU BUFFER =====
extern const int SAMPLE_RATE;
extern const int BUFFER_N;

extern float axBuf[];
extern float ayBuf[];
extern float azBuf[];

extern int bufIndex;
extern unsigned long lastSampleUs;
extern unsigned long sampleIntervalUs;

// ===== ALARM PINS =====
extern const int LED_PIN;
extern const int BUZZER_PIN;
extern const int RELAY_PIN;

// ===== FLAGS =====
extern bool alarmTemp;
extern bool alarmCurrent;
extern bool alarmPower;
extern bool alarmRMS;
extern bool alarmVin;
extern bool userLedOff;

extern bool relayState;
extern bool buzzerState;
extern bool alarmLatched;
