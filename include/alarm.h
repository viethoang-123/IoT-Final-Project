#pragma once

bool checkAlarms(float tempC, float current, float power, float rms, float Vin);
void applyAlarmLogic(bool alarmGlobal);
