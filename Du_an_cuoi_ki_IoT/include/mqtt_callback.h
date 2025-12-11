#pragma once
#include <Arduino.h>
/******** ALARM *********/
const int LED_PIN = 2;
const int BUZZER_PIN = 15;
const int RELAY_PIN = 27;
void mqtt_message_handler(const char* topic, const String &msg);
