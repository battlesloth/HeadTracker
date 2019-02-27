#include <Arduino.h>
#include <NXPMotionSense.h>
#include <SensorFusion.h>
#include <Wire.h>
#include <EEPROM.h>

NXPMotionSense imu;
SF fusion;

float gx, gy, gz, ax, ay, az, mx, my, mz, temp;
float pitch, roll, yaw;
float deltat;


const int headingCenter = 0;
const int pitchCenter = 0;
const int rollCenter = 0;

const int headingRange = 70;
const int pitchRange = 40;
const int rollRange = 40;


void setup() {
  Serial.begin(9600);
  imu.begin();  
}

void loop() {

  if (imu.available()){
    imu.readMotionSensor(ax, ay, az, gx, gy, gz, mx, my, mz);

    deltat = fusion.deltatUpdate();
    //fusion.MahonyUpdate(gx, gy, gz, ax, ay, az, mx, my, mz, deltat);  //mahony is suggested if there isn't the mag
    fusion.MadgwickUpdate(gx, gy, gz, ax, ay, az, mx, my, mz, deltat);  //else use the magwick

    yaw = fusion.getYaw();
    roll = fusion.getRoll();
    pitch = fusion.getPitch();
    

    Serial.print("Orientation: ");
    Serial.print(yaw);
    Serial.print(" ");
    Serial.print(pitch);
    Serial.print(" ");
    Serial.println(roll);

    

    //Joystick.X(value);            // "value" is from 0 to 1023
    //Joystick.Y(value);            //   512 is resting position
    //Joystick.Z(value);
    //Joystick.Zrotate(value);
  }
}



int getHeading(const int center, const int reading)
{
	const int modDiff = (reading - center) % 360;
	int distance = 180 - abs(abs(modDiff) - 180);
	return (modDiff + 360) % 360 < 180 ? distance : distance *= -1;
}