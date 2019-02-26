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
  //Joystick.X(value);            // "value" is from 0 to 1023
  //Joystick.Y(value);            //   512 is resting position
  //Joystick.Z(value);
  //Joystick.Zrotate(value);
}


int getHeading(const int center, const int reading)
{
	const int modDiff = (reading - center) % 360;
	int distance = 180 - abs(abs(modDiff) - 180);
	return (modDiff + 360) % 360 < 180 ? distance : distance *= -1;
}