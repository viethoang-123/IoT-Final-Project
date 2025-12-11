#include "mqtt_callback.h"
#include <Arduino.h>

// Dùng biến global từ main:
extern bool userLedOff;
extern bool relayState;
extern bool buzzerState;
extern bool alarmLatched;

extern bool alarmTemp;
extern bool alarmCurrent;
extern bool alarmPower;
extern bool alarmRMS;
extern bool alarmVin;

extern const int LED_PIN;
extern const int RELAY_PIN;
extern const int BUZZER_PIN;

void mqtt_message_handler(const char* topic, const String &msg)
{
    // ========== LED CONTROL ==========
    if (strcmp(topic, "esp32/led/control") == 0) 
    {
        if (msg == "OFF") {
            userLedOff = true;
            digitalWrite(LED_PIN, LOW);
        }
        return;
    }

    // ========== RELAY CONTROL ==========
    if (strcmp(topic, "esp32/relay/control") == 0)
    {
        if (msg == "ON") {
            relayState = true;
            digitalWrite(RELAY_PIN, LOW);
            alarmLatched = false;
        }
        else if (msg == "OFF") {
            relayState = false;
            digitalWrite(RELAY_PIN, HIGH);
        }
        return;
    }

    // ========== RESET ALARM ==========
    if (strcmp(topic, "esp32/reset/alarm") == 0)
    {
        if (msg == "ON") 
        {
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
        return;
    }
}
