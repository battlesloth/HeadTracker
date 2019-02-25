#include <Arduino.h>
#include <NXPMotionSense.h>
#include <MahonyAHRS.h>
#include <Wire.h>
#include <EEPROM.h>


NXPMotionSense imu;
Mahony filter;


void setup() {
  Serial.begin(9600);
  imu.begin();
  filter.begin(100);
}

void loop() {
  // put your main code here, to run repeatedly:
}