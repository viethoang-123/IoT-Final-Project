#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "ca_cert_emqx.h"

// ---------------- WIFI ----------------
char ssid[] = "NI-Elvis-216";
char password[] = "votuyen@216";

const char* mqtt_host = "nd022d10.ala.asia-southeast1.emqxsl.com";
const int mqtt_port = 8883;
const char* mqtt_user = "deskop_client";
const char* mqtt_pass = "1";

// ---------------- SENSORS ----------------
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <MPU6050_light.h>

/******** ACS712 *********/
const int ACS_PIN = 32;
const float ADC_VREF = 3.3;
const int ADC_RES = 4095;
const float SENSITIVITY = 0.100;
float offsetVoltage = 2.5;

/******** Voltage divider *********/
const int adcPin = 34;

/******** DS18B20 *********/
#define ONE_WIRE_BUS 33
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

/******** MPU6050 *********/
MPU6050 mpu(Wire);

const int SAMPLE_RATE = 200;
const int BUFFER_N = 256;

float axBuf[BUFFER_N], ayBuf[BUFFER_N], azBuf[BUFFER_N];
int bufIndex = 0;

unsigned long lastSampleUs = 0;
unsigned long sampleIntervalUs = 1000000UL / SAMPLE_RATE;

/******** ALARM *********/
const int LED_PIN = 2;
const int BUZZER_PIN = 15;
const int RELAY_PIN = 27;

/******** MQTT *********/
WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

/******** FLAGS *********/
bool alarmTemp = false;
bool alarmCurrent = false;
bool alarmPower = false;
bool alarmRMS = false;
bool alarmVin = false;
bool userLedOff = false;

bool relayState = true;
bool buzzerState = false;
bool alarmLatched = false;

/******** PROTOTYPES ********/
void mqttReconnect();
float readCurrent();
float calcRMS(float *buf, int N);


/******************** MQTT CALLBACK ********************/
void mqtt_callback(char* topic, byte* payload, unsigned int len) {
    String msg;
    for (int i = 0; i < len; i++) msg += (char)payload[i];

    // -------- LED CONTROL --------
    if (String(topic) == "esp32/led/control") {
        if (msg == "OFF") {
            userLedOff = true;
            digitalWrite(LED_PIN, LOW);
        }
    }

    // -------- RELAY CONTROL --------
    if (String(topic) == "esp32/relay/control") {
        if (msg == "ON") {
            relayState = true;
            digitalWrite(RELAY_PIN, LOW);
            alarmLatched = false;
        }
        else if (msg == "OFF") {
            relayState = false;
            digitalWrite(RELAY_PIN, HIGH);
        }
    }

    // -------- RESET ALARM --------
    if (strcmp(topic, "esp32/reset/alarm") == 0) {
        if (msg == "ON") {
            alarmLatched = false;
            alarmTemp = false;
            alarmCurrent = false;
            alarmPower = false;
            alarmRMS = false;
            alarmVin = false;

            userLedOff = false;

            buzzerState = false;
            digitalWrite(BUZZER_PIN, LOW);

            relayState = true;
            digitalWrite(RELAY_PIN, LOW);

        }
    }
}


/******************** SETUP ********************/
void setup() {
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);

    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(RELAY_PIN, LOW);

    analogReadResolution(12);

    // -------- WIFI --------
    WiFi.begin(ssid, password);
    Serial.print("WiFi connecting");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(150);
    }
    Serial.println("\nWiFi Connected!");

    // -------- TLS --------
    espClient.setCACert(emqx_ca_cert);

    // -------- MQTT --------
    mqttClient.setServer(mqtt_host, mqtt_port);
    mqttClient.setCallback(mqtt_callback);

    // -------- CALIB ACS712 --------
    long sum = 0;
    for (int i = 0; i < 300; i++) {
        sum += analogRead(ACS_PIN);
        delay(2);
    }
    offsetVoltage = (sum / 300.0) * (ADC_VREF / ADC_RES);

    // -------- SENSORS --------
    Wire.begin(21, 22);
    Wire.setClock(100000);

    if (mpu.begin() != 0) {
        Serial.println("MPU INIT FAILED!");
        while (1);
    }

    sensors.begin();
    lastSampleUs = micros();
}


/******************** LOOP ********************/
void loop() {
    if (!mqttClient.connected()) mqttReconnect();
    mqttClient.loop();

    // -------- MPU sampling --------
    unsigned long nowUs = micros();
    if (nowUs - lastSampleUs >= sampleIntervalUs) {
        lastSampleUs += sampleIntervalUs;

        mpu.update();
        axBuf[bufIndex] = mpu.getAccX();
        ayBuf[bufIndex] = mpu.getAccY();
        azBuf[bufIndex] = mpu.getAccZ();

        bufIndex = (bufIndex + 1) % BUFFER_N;
    }

    // -------- SEND EVERY 2 SEC --------//
    static unsigned long lastSend = 0;
    if (millis() - lastSend < 2000) return;
    lastSend = millis();

    float current = readCurrent();
    float power = current * 24.0;

    float Vin = analogRead(adcPin) * (3.3 / 4095.0) * 11.0;

    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0);
    if (tempC == 85 || tempC == -127) tempC = NAN;

    float magBuf[BUFFER_N];
    for (int i = 0; i < BUFFER_N; i++) {
        float x = axBuf[i], y = ayBuf[i], z = azBuf[i];
        magBuf[i] = sqrt(x*x + y*y + z*z);
    }

    float rms = calcRMS(magBuf, BUFFER_N);

    // -------- ALARM CHECK --------
    bool alarmGlobal = false;

    if (!alarmLatched) {
        if (!isnan(tempC) && tempC > 31) alarmTemp = true;
        if (current > 1.0) alarmCurrent = true;
        if (power > 120) alarmPower = true;
        if (rms > 1.5) alarmRMS = true;
        if (Vin < 5 || Vin > 26) alarmVin = true;
    }

    alarmGlobal = alarmTemp || alarmCurrent || alarmPower || alarmRMS || alarmVin;

    // -------- RELAY + BUZZER LOGIC --------
    if (alarmGlobal) {
        alarmLatched = true;

        digitalWrite(RELAY_PIN, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);
        relayState = false;
        buzzerState = true;
    }
    else {
        digitalWrite(RELAY_PIN, relayState ? LOW : HIGH);
        digitalWrite(BUZZER_PIN, buzzerState ? HIGH : LOW);
    }

    digitalWrite(LED_PIN, (alarmGlobal && !userLedOff) ? HIGH : LOW);
    /******** SERIAL DEBUG ********/ 
     Serial.println("------------------------------");
      Serial.printf("Vin: %.2f V | I: %.3f A | P: %.2f W\n", Vin, current, power);
     Serial.printf("Temp: %.2f C | RMS(g): %.4f\n", tempC, rms);

    // -------- MQTT PUBLISH --------
    mqttClient.publish("esp32/current", String(current).c_str());
    mqttClient.publish("esp32/power", String(power).c_str());
    mqttClient.publish("esp32/vin", String(Vin).c_str());
    mqttClient.publish("esp32/temperature", String(tempC).c_str());
    mqttClient.publish("esp32/vibration", String(rms).c_str());

    mqttClient.publish("esp32/led/state", digitalRead(LED_PIN) ? "1" : "0");
    mqttClient.publish("esp32/relay/state", relayState ? "1" : "0");
    

}

//     return current;
// }
float readCurrent() {
    const int N = 200;        // Lấy nhiều mẫu cho mượt hơn
    long sum = 0;

    for (int i = 0; i < N; i++) {
        sum += analogRead(ACS_PIN);
        delayMicroseconds(200);   // ~0.2 ms giữa 2 mẫu
    }

    float raw = sum / (float)N;               // giá trị ADC trung bình
    float v   = raw * (ADC_VREF / ADC_RES);   // điện áp tại chân ACS
    float diff = v - offsetVoltage;           // lệch so với điểm 0A

    // TÍNH DÒNG: I = V / SENSITIVITY
    float current = diff / SENSITIVITY;

    //

    // Ngưỡng chết để bỏ nhiễu vùng gần 0A
    if (fabs(current) < 0.05) current = 0;

    return current;
}

float calcRMS(float *buf, int N) {
    double s = 0;
    for (int i = 0; i < N; i++) s += buf[i] * buf[i];
    return sqrt(s / N);
}

void mqttReconnect() {
    while (!mqttClient.connected()) {
        Serial.println("MQTT reconnecting...");
        mqttClient.connect("ESP32_Client", mqtt_user, mqtt_pass);
        delay(400);
    }

    mqttClient.subscribe("esp32/led/control");
    mqttClient.subscribe("esp32/relay/control");
    mqttClient.subscribe("esp32/reset/alarm");
}
