#include "alarm.h"
#include <Arduino.h>

extern bool alarmTemp;
extern bool alarmCurrent;
extern bool alarmPower;
extern bool alarmRMS;
extern bool alarmVin;
extern bool alarmLatched;

extern bool relayState;
extern bool buzzerState;
extern bool userLedOff;

extern const int LED_PIN;
extern const int RELAY_PIN;
extern const int BUZZER_PIN;

bool checkAlarms(float tempC, float current, float power, float rms, float Vin)
{
    if (!alarmLatched)
    {
        if (!isnan(tempC) && tempC > 30) alarmTemp = true;
        if (current > 2.0) alarmCurrent = true;
        if (power > 120) alarmPower = true;
        if (rms > 1.5) alarmRMS = true;
        if (Vin < 0 || Vin > 26) alarmVin = true;
    }

    return alarmTemp || alarmCurrent || alarmPower || alarmRMS || alarmVin;
}

void applyAlarmLogic(bool alarmGlobal)
{
    if (alarmGlobal)
    {
        alarmLatched = true;

        digitalWrite(RELAY_PIN, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);

        relayState = false;
        buzzerState = true;
    }
    else
    {
        digitalWrite(RELAY_PIN, relayState ? LOW : HIGH);
        digitalWrite(BUZZER_PIN, buzzerState ? HIGH : LOW);
    }

    digitalWrite(LED_PIN, (alarmGlobal && !userLedOff) ? HIGH : LOW);
}
