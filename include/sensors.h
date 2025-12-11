#pragma once

float readCurrent();
float readTemperature();
float readVoltage();
float readVibrationRMS();

void sensors_init();
void sensors_update();   // lấy mẫu MPU6050
