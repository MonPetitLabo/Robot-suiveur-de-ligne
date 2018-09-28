#include <string.h>
#include <QTRSensors.h>
#include <SoftwareSerial.h>

//////////////////////////////////////
// MOTORS
/////////////////////////////////////
// M1 PWM 1
#define ENABLEG 11
// M1 Direction 0/1 2
#define DIRAG 10
// M1 Direction 1/0 7
#define DIRBG 9
// M2 PWN 9
#define ENABLED 5
// M2 Direction 0/1 15
#define DIRAD 3
// M2 Direction 1/0 10
#define DIRBD 4

// VCC 8 && 16
// GND 4 && 13
// MotorG 3 && 6
// MotorD 14 && 11

//////////////////////////////////////
// SENSORS
/////////////////////////////////////
const int sensorInput1 = 0;
const int sensorInput2 = 1;
const int sensorInput3 = 2;
const int SENSOR_DELTA = 250;
QTRSensorsAnalog sensors ((unsigned char[]) {sensorInput1, sensorInput2, sensorInput3}, 3);


// prefix for actions
const String SPEED_CHOOSE_PREFIX = "A";
const String SPEED_VALUE_PREFIX = "D";
const String DIRECTION_UP = "GO";
const String DIRECTION_BACK = "BACK";
const String DIRECTION_LEFT = "LEFT";
const String DIRECTION_RIGHT = "RIGHT";
const String DIRECTION_STOP = "STOP";
const String DIRECTION_BACK_CANCEL = "back";
const String DIRECTION_UP_CANCEL = "go";
const String DIRECTION_LEFT_CANCEL = "left";
const String DIRECTION_RIGHT_CANCEL = "right";
const String MANUAL_MODE = "MANUAL";
const String AUTO_MODE = "AUTO";

// Global vars
boolean isAutoMode = true;
int currentMotorsSpeed = 150;
int maxMotorsSpeed = 200;
int currentPosition = 0;
int oldPosition = 0;

void setup() {
  // Power pins 
  pinMode(ENABLEG, OUTPUT);
  pinMode(ENABLED, OUTPUT);

  // Rotation pins
  pinMode(DIRAG, OUTPUT);
  pinMode(DIRBG, OUTPUT);
  pinMode(DIRAD, OUTPUT);
  pinMode(DIRBD, OUTPUT);
  
  // serials initializations
  Serial.begin(9600);

  Serial.println("== Robot starting sequence ==");
  delay(2500);
  
  Serial.println("== Sensors calibration ==");
  delay(2500);
  for(int i =0; i < 250; i++){
    Serial.print(i);
    Serial.print("/");
    Serial.println(250);
    
    sensors.calibrate();
    delay(20);
  }
  Serial.println("== end of sensors calibration ==");
  
  Serial.println("Robot ready !!");
  
  for(int i = 5; i > 0; i--){
    Serial.print(">>");
    Serial.print(i);
    Serial.println("<<");
    delay(1000);
  }
}

void loop() {

  // position calculation
  oldPosition = currentPosition;
  currentPosition = calculateCurrentPosition();
    
  Serial.print(">>current position : ");
  Serial.println(currentPosition);
  runAutoMode(currentPosition, oldPosition, currentMotorsSpeed);
  delay(10);
  analogWrite(ENABLEG, 0); 
  analogWrite(ENABLED, 0);
}

int calculateCurrentPosition(){
  unsigned int sensorsValues[3];
  int position = sensors.readLine(sensorsValues );
  position = position - 1000;

  return position;
}

void runAutoMode(int currentPosition, int oldPosition, int motorSpeed){
  if(currentPosition < -250){
    if(oldPosition > 250){
      runMotors(maxMotorsSpeed, false, false, false, true, true);
    }
    runMotors(maxMotorsSpeed, true, true, true, true, false);
    Serial.println(">DD");
  }else if(currentPosition > 250){
    if(oldPosition < 250){
      runMotors(maxMotorsSpeed, false, false, false, true, true);
    }
    runMotors(maxMotorsSpeed, true, true, true, false, true);
    Serial.println(">GG");
  }else{
    runMotors(maxMotorsSpeed, true, true, false, true, true);
    Serial.println(">GD");
  }
}

/**
 * Run a motor (left / right / both)
 * 
 * motorSpeed : motor speed (value between 0 and 255)
 * isMotorLeft : run the left motor
 * isMotorRight : run the right motor
 * stopOther : stop the other motor (if one of the two motors must be stopped)
 */
void runMotors(int motorSpeed, boolean isMotorLeft, boolean isMotorRight, boolean stopOther, boolean leftMotorUp, boolean rightMotorUp){
  // controls for motors speed
  if(motorSpeed < 0){
    motorSpeed = 0;
  }else if(motorSpeed > 255){
    motorSpeed = 255;
  }

  // Rotation direction
  if(leftMotorUp){
      digitalWrite(DIRAG,HIGH);
      digitalWrite(DIRBG,LOW);
  }else{
      digitalWrite(DIRAG,LOW);
      digitalWrite(DIRBG,HIGH);
  }
  if(rightMotorUp){
    digitalWrite(DIRAD,HIGH);
    digitalWrite(DIRBD,LOW);
  }else{
    digitalWrite(DIRAD,LOW);
    digitalWrite(DIRBD,HIGH);
  }
  
  if(isMotorLeft && isMotorRight){
    analogWrite(ENABLEG, motorSpeed); 
    analogWrite(ENABLED, motorSpeed);
  }else if(isMotorLeft && !isMotorRight){
    analogWrite(ENABLEG, motorSpeed);
    if(stopOther){
      analogWrite(ENABLED, 0);
    }
  }else if(isMotorRight && !isMotorLeft){
    analogWrite(ENABLED, motorSpeed);
    if(stopOther){
      analogWrite(ENABLEG, 0);
    }
  }else{
    analogWrite(ENABLEG, 0); 
    analogWrite(ENABLED, 0);
  }
}
