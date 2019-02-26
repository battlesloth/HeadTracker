#include <Arduino.h>
#include <NXPMotionSense.h>
#include <SensorFusion.h>

NXPMotionSense imu;
SF fusion;


void setup() {
  Serial.begin(9600);
  imu.begin();
  
}

void loop() {
  // put your main code here, to run repeatedly:
}


int getHeading(const int center, const int reading)
{
	const int modDiff = (reading - center) % 360;
	int distance = 180 - abs(abs(modDiff) - 180);
	return (modDiff + 360) % 360 < 180 ? distance : distance *= -1;
}