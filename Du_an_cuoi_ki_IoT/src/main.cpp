#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#include "ca_cert_emqx.h"
#include "Bi'smat/mqtt.h"
#include "Bi'smat/Wifi.h"

#include "mqtt_callback.h"
#include "sensors.h"
#include "alarm.h"
#include "sensors_config.h"

// ================= MQTT CLIENT =================
WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

// ================= MQTT CALLBACK WRAPPER =================
void mqtt_callback(char* topic, byte* payload, unsigned int len)
{
    String msg;
    for (int i = 0; i < len; i++) msg += (char)payload[i];

    mqtt_message_handler(topic, msg);
}

// ====================== MQTT RECONNECT ======================
void mqttReconnect() {
    while (!mqttClient.connected()) {
        Serial.println("MQTT reconnecting...");
        mqttClient.connect("ESP32_Client", EMQX::username, EMQX::password);
        delay(400);
    }

    mqttClient.subscribe("esp32/led/control");
    mqttClient.subscribe("esp32/relay/control");
    mqttClient.subscribe("esp32/reset/alarm");
}

// =========================== SETUP ===========================
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
    WiFi.begin(WiFiSecrets::ssid, WiFiSecrets::pass);
    Serial.print("WiFi connecting");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(150);
    }
    Serial.println("\nWiFi Connected!");

    // -------- TLS --------
    espClient.setCACert(emqx_ca_cert);

    // -------- MQTT --------
    mqttClient.setServer(EMQX::broker, EMQX::port);
    mqttClient.setCallback(mqtt_callback);

    // -------- INIT SENSOR MODULE --------
    sensors_init();

    Serial.println("Setup completed!");
}

// ============================ LOOP ============================
void loop() {

    // -------- MQTT STATUS --------
    if (!mqttClient.connected()) mqttReconnect();
    mqttClient.loop();

    // -------- UPDATE MPU + BUFFER --------
    sensors_update();

    // ============ SEND DATA EVERY 2 SECONDS ============
    static unsigned long lastSend = 0;
    if (millis() - lastSend < 2000) return;
    lastSend = millis();

    // -------- READ SENSOR VALUES --------
    float tempC   = readTemperature();
    float current = readCurrent();
    float power   = current * 24.0;
    float Vin     = readVoltage();
    float rms     = readVibrationRMS();

    // -------- CHECK ALARMS --------
    bool alarmGlobal = checkAlarms(tempC, current, power, rms, Vin);

    // -------- APPLY ALARM LOGIC --------
    applyAlarmLogic(alarmGlobal);

    // ================= SERIAL DEBUG =================
    Serial.println("------------------------------");
    Serial.printf("Vin: %.2f V | I: %.3f A | P: %.2f W\n", Vin, current, power);
    Serial.printf("Temp: %.2f C | RMS(g): %.4f\n", tempC, rms);

    // ============== MQTT PUBLISH ==============
    mqttClient.publish("esp32/current",      String(current).c_str());
    mqttClient.publish("esp32/power",        String(power).c_str());
    mqttClient.publish("esp32/vin",          String(Vin).c_str());
    mqttClient.publish("esp32/temperature",  String(tempC).c_str());
    mqttClient.publish("esp32/vibration",    String(rms).c_str());

    mqttClient.publish("esp32/led/state",    digitalRead(LED_PIN) ? "1" : "0");
    mqttClient.publish("esp32/relay/state",  relayState ? "1" : "0");
}
