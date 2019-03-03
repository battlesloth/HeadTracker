#include <Arduino.h>
#include <NXPMotionSense.h>
#include <Wire.h>
#include <EEPROM.h>

NXPMotionSense imu;
NXPSensorFusion filter;

float gx, gy, gz, ax, ay, az, mx, my, mz, temp;
int pitch, roll, yaw, joyPitch, joyRoll, joyYaw;

const int headingCenter = 346;
const int pitchCenter = -3;
const int rollCenter = -5;

const int headingRange = 70;
const int pitchRange = 40;
const int rollRange = 40;

float pitchFactor;
float rollFactor;
float headingFactor;

// 3.14159 /180.0
const float gyroScale = 0.017453; 

bool diagnosticMode = true;
// delay 3 minutes to settle the gyros
int bootUpDelay = 180;
bool measure = false;

long seconds;
long previousMillis;
const long interval = 1000;

int yawLast = 0;
int pitchLast = 0;
int rollLast = 0;

int yawDrift = 0;
int pitchDrift = 0;
int rollDrift = 0;

float yawDPS = 0;
float pitchDPS = 0;
float rollDPS = 0;

void setup() {
  Serial.begin(9600);
  imu.begin();
  filter.begin(100);

  // "value" is from 0 to 1023
  //   512 is resting position
  pitchFactor = 1024 / pitchRange;
  rollFactor = 1024 / rollRange;
  headingFactor = 1024 / headingRange;

  Serial.print("Setup Complete!");
}

void loop() {

  if (imu.available()){
    
    imu.readMotionSensor(ax, ay, az, gx, gy, gz, mx, my, mz);

    filter.update(gx, gy, gz, ax, ay, az, mx, my, mz); 

    // int is probably good enough for our needed precision
    yaw = (int) filter.getYaw();
    pitch = (int) filter.getPitch();
    roll = (int) filter.getRoll();
    
    //Serial.print("Orientation: ");
    //Serial.print(yaw);
    //Serial.print(" ");
    //Serial.print(pitch);
    //Serial.print(" ");
    //Serial.println(roll);
    //Serial.print(" ; ");
    
    joyYaw = getJoyYaw(yaw);
    // swap pitch and roll due to sensor position
    joyPitch = getJoyPitch(roll);
    joyRoll = getJoyRoll(pitch);

    //Serial.print("Yaw: ");
    //Serial.print(joyYaw);
    //Serial.print(" ");  
    //Serial.print("Pitch: ");
    //Serial.print(joyPitch);
    //Serial.print(" ");
    //Serial.print("Roll: ");
    //Serial.print(joyRoll);
    //Serial.print(" ");
  
    if (diagnosticMode){
      diagnosticLoop();
    }

    Joystick.X(joyRoll);
    Joystick.Y(joyPitch);
    Joystick.Z(joyYaw);
  }
}

void diagnosticLoop(){
      
  unsigned long currentMillis = millis();

  if (!measure){
    if (currentMillis - previousMillis > interval ){
      seconds++;
      previousMillis = currentMillis;
      if (seconds > bootUpDelay){
        Serial.print("Starting diagnostic...");
        measure = true;
        yawLast = yaw;
        pitchLast = pitch;
        rollLast = roll;
      }
    }
  }
  else if (currentMillis - previousMillis > interval){
    seconds++;
    previousMillis = currentMillis;

    yawDrift += (yaw - yawLast);
    pitchDrift += (pitch - pitchLast);
    rollDrift += (roll - rollLast);

    yawLast = yaw;
    pitchLast = pitch;
    rollLast = roll;

    if (yawDrift != 0){
      yawDPS = yawDrift / seconds;
    } else {
      yawDPS = 0;
    }

    if (pitchDrift != 0){
      pitchDPS = pitchDrift / seconds;
    } else {
      pitchDPS = 0;
    }

    if (rollDrift != 0){
      rollDPS = rollDrift / seconds;
    } else {
      rollDPS = 0;
    }

    Serial.print("Orientation: Y= ");
    Serial.print(yaw);
    Serial.print(" P= ");
    Serial.print(pitch);
    Serial.print(" R= ");
    Serial.println(roll);
    Serial.print(" ; ");

    Serial.print("Total seconds: ");
    Serial.print(seconds);
    Serial.print(" ; Y DPS= ");
    Serial.print(yawDPS); 
    Serial.print(" ; P DPS= ");
    Serial.print(pitchDPS);
    Serial.print(" ; R DPS= ");
    Serial.print(rollDPS);
    Serial.print(" ");
  }
}

int getHeading(const int center, const int reading)
{
	const int modDiff = (reading - center) % 360;
	int distance = 180 - abs(abs(modDiff) - 180);
	return (modDiff + 360) % 360 < 180 ? distance : distance *= -1;
}

int getJoyYaw(int z){

  int head = getHeading(headingCenter, z);
  int temp = head * headingFactor;

  if (temp < 0){
    temp = 0;
  } else if (temp > 1023){
    temp = 1023;
  }

  return temp;
}

int getJoyPitch(int y){
  int temp = (y - pitchCenter) * pitchFactor;

  if (temp < 0){
    temp = 0;
  } else if (temp > 1023){
    temp = 1023;
  }

  return temp;
}

int getJoyRoll(int x){
  int temp = (x - rollCenter) * rollFactor;

  if (temp < 0){
    temp = 0;
  } else if (temp > 1023){
    temp = 1023;
  }

  return temp;
}


