#include <Arduino.h>
#include <NXPMotionSense.h>
#include <MahonyAHRS.h>
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



float pitchFactor;
float rollFactor;

void setup() {
  Serial.begin(9600);
  imu.begin();

  // "value" is from 0 to 1023
  //   512 is resting position
  pitchFactor = 1024 / pitchRange;
  rollFactor = 1024 / rollRange;
  headingFactor = 1024 / headingRange;

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
    Serial.print(" ; ");
    
    setJoyHeading(yaw);
    setJoyPitch(pitch);
    setJoyRoll(roll);
  }
}



int getHeading(const int center, const int reading)
{
	const int modDiff = (reading - center) % 360;
	int distance = 180 - abs(abs(modDiff) - 180);
	return (modDiff + 360) % 360 < 180 ? distance : distance *= -1;
}

void setJoyHeading(float z){

  int head = getHeading(headingCenter, z);
  int temp = head * headingFactor;

  if (temp < 0){
    temp = 0;
  } else if (temp > 1023){
    temp = 1023;
  }

  Serial.print("Heading: ")
  Serial.print(head);
  Serial.print(" : ")
  Serial.print(temp)
  Serial.print(" ");

  Joystick.Z(temp);
}

void setJoyPitch(float y){
  int temp = (x - pitchCenter) * pitchFactor;

  if (temp < 0){
    temp = 0;
  } else if (temp > 1023){
    temp = 1023;
  }

  Serial.print("Pitch: ")
  Serial.print(temp)
  Serial.print(" ");

  Joystick.Y(temp);
}

void setJoyRoll(float x){
  int temp = (x - rollCenter) * rollFactor;

  if (temp < 0){
    temp = 0;
  } else if (temp > 1023){
    temp = 1023;
  }

  Serial.print("Roll: ")
  Serial.print(temp)
  Serial.print(" ");

  Joystick.X(temp);
}


