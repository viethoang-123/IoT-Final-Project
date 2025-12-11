#include "sensors_config.h"

// ===== DS18B20 =====
OneWire oneWire(33);
DallasTemperature sensors(&oneWire);

// ===== MPU =====
MPU6050 mpu(Wire);

// ===== ADC / CURRENT =====
const int ACS_PIN = 32;
const float ADC_VREF = 3.3;
const int ADC_RES = 4095;
const float SENSITIVITY = 0.100;
float offsetVoltage = 2.5;

// ===== VOLTAGE DIVIDER =====
const int adcPin = 34;

// ===== MPU BUFFER =====
const int SAMPLE_RATE = 200;
const int BUFFER_N = 256;

float axBuf[BUFFER_N];
float ayBuf[BUFFER_N];
float azBuf[BUFFER_N];

int bufIndex = 0;
unsigned long lastSampleUs = 0;
unsigned long sampleIntervalUs = 1000000UL / SAMPLE_RATE;

// ===== ALARM PINS =====
const int LED_PIN = 2;
const int BUZZER_PIN = 15;
const int RELAY_PIN = 27;

// ===== FLAGS =====
bool alarmTemp = false;
bool alarmCurrent = false;
bool alarmPower = false;
bool alarmRMS = false;
bool alarmVin = false;
bool userLedOff = false;

bool relayState = true;
bool buzzerState = false;
bool alarmLatched = false;
