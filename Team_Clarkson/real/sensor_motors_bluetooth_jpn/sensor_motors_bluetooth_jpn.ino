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

//////////////////////////////////////
// BLUETOOTH
/////////////////////////////////////
const int BLUETOOTH_RX = 6; // TX3
const int BLUETOOTH_TX = 7; // RX3
const int BLUETOOTH_KEY = 12; // KEY
const int AT_COMMAND_WAIT_TIME = 250; // time in ms to wait after command printed
SoftwareSerial Serial3(BLUETOOTH_RX, BLUETOOTH_TX);

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
int currentMotorsSpeed = 255;
int choosedMotorsSpeed = 255;
int currentPosition = 0;

void setup() {
  // Power pins 
  pinMode(ENABLEG, OUTPUT);
  pinMode(ENABLED, OUTPUT);

  // Rotation pins
  pinMode(DIRAG, OUTPUT);
  pinMode(DIRBG, OUTPUT);
  pinMode(DIRAD, OUTPUT);
  pinMode(DIRBD, OUTPUT);

  // Bluetooth pins
  pinMode(BLUETOOTH_KEY, OUTPUT);
  digitalWrite(BLUETOOTH_KEY, LOW);
  
  // serials initializations
  Serial.begin(9600);
  Serial3.begin(9600); // for bluetooth

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
  Serial.println("== Bluetooth configuration ==");
  delay(1000);
  
  runCommand("AT+NAME=FABLAB", true);
  runCommand("AT+NAME", true);
  runCommand("AT+PSWD=2018", true);
  runCommand("AT+PSWD?", true);
  Serial.println("== end bluetooth configuration ==");
  
  Serial.println("Robot ready !!");
  
  for(int i = 5; i > 0; i--){
    Serial.print(">>");
    Serial.print(i);
    Serial.println("<<");
    delay(1000);
  }
}

void loop() {
  // check serial input
  checkSerialInput();

  // check bluetooth input
  checkBluetoothInput();

  // position calculation
  currentPosition = calculateCurrentPosition();
    
  if(isAutoMode){  
    Serial.print(">>current position : ");
    Serial.println(currentPosition);
    runAutoMode(currentPosition, currentMotorsSpeed);
  }else{
    runMotors(currentMotorsSpeed, true, true, false);
  }

  delay(50);
}

int calculateCurrentPosition(){
  unsigned int sensorsValues[3];
  int position = sensors.readLine(sensorsValues );
  position = position - 1000;

  return position;
}

void runAutoMode(int currentPosition, int motorSpeed){
  if(currentPosition < -250){
    runMotors(motorSpeed, true, false, true);
    Serial.println(">DD");
  }else if(currentPosition > 250){
    runMotors(motorSpeed, false, true, true);
    Serial.println(">GG");
  }else{
    runMotors(motorSpeed, true, true, false);
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
void runMotors(int motorSpeed, boolean isMotorLeft, boolean isMotorRight, boolean stopOther){
  // Rotation direction
  digitalWrite(DIRAG,HIGH);
  digitalWrite(DIRBG,LOW);

  digitalWrite(DIRAD,HIGH);
  digitalWrite(DIRBD,LOW);

  // controls for motors speed
  if(motorSpeed < 0){
    motorSpeed = 0;
  }else if(motorSpeed > 255){
    motorSpeed = 255;
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

/**
 * Send a commond to the bluetooth module
 * 
 * command : command to send
 * isATCommand : specify if the command is for the module configuration or is to send to connected devices
 */
void runCommand(String command, boolean isATCommand){
  if(command.length() > 0){
    Serial.print("BM>>'");
    Serial.print(command);
    Serial.println("'");

    // activate command mode
    if(isATCommand){
      digitalWrite(BLUETOOTH_KEY, HIGH);
    }

    // print command
    Serial3.print(command);
    Serial3.print("\r\n");

    // wait for answer
    delay(300);

    // get and print answer
    if(Serial3.available()){
      String input = Serial3.readString();
      Serial.print("BM<<");
      Serial.println(input);
    }

    // unactive command mode
    if(isATCommand){
      digitalWrite(BLUETOOTH_KEY, LOW);
    }
  }
}

/**
 * Check if a command was send by us and treat it
 */
void checkSerialInput(){
  if(Serial.available()){
    String input;    
    input = Serial.readString();
    
    if(input.length() >= 2){
      if(input.startsWith("AT")){
        runCommand(input, true);
      }
    }
  }
}

/**
 * Check if the module sent us a value and treat it
 * 
 */
void checkBluetoothInput(){
  if(Serial3.available()){
    String input;    
    input = Serial3.readString();

    Serial.print("<<");
    Serial.println(input);

    if(input.startsWith(MANUAL_MODE)){
      isAutoMode = false;
      runMotors(currentMotorsSpeed, false, false, true); 
    }else if(input.startsWith(AUTO_MODE)){
      isAutoMode = true;
      runMotors(currentMotorsSpeed, true, true, false);
    }else if(input.startsWith(SPEED_CHOOSE_PREFIX) && input.endsWith(SPEED_CHOOSE_PREFIX)){
      currentMotorsSpeed = input.substring(SPEED_CHOOSE_PREFIX.length(), input.length() - SPEED_CHOOSE_PREFIX.length()).toInt();

      String speedValue = "*" + SPEED_VALUE_PREFIX + currentMotorsSpeed + "*";
      choosedMotorsSpeed = currentMotorsSpeed;
      runCommand(speedValue, false);
    }else if(input.startsWith(DIRECTION_UP)){
      Serial.println("== GO ==");
      currentMotorsSpeed = choosedMotorsSpeed;
      runMotors(currentMotorsSpeed, true, true, false);
    }else if(input.startsWith(DIRECTION_UP_CANCEL)){
      Serial.println("== STOP ==");
      currentMotorsSpeed = 0;
      runMotors(currentMotorsSpeed, false, false, true);
    }else if(input.startsWith(DIRECTION_STOP)){
        Serial.println("== STOP ==");
        currentMotorsSpeed = 0;
        runMotors(currentMotorsSpeed, false, false, true);
    }else if(!isAutoMode){
      // checks manual directions only if in manual mode
      if(input.startsWith(DIRECTION_LEFT)){
        Serial.println("== LEFT ==");
        runMotors(currentMotorsSpeed, true, false, true);
      }else if(input.startsWith(DIRECTION_LEFT_CANCEL)){
        Serial.println("== LEFT STOP ==");
        runMotors(currentMotorsSpeed, true, true, false);
      }else if(input.startsWith(DIRECTION_RIGHT)){
        Serial.println("== RIGHT ==");
        runMotors(currentMotorsSpeed, false, true, true);
      }else if(input.startsWith(DIRECTION_RIGHT_CANCEL)){
        Serial.println("== RIGHT STOP ==");
        runMotors(currentMotorsSpeed, false, true, false);
      }else if(input.startsWith(DIRECTION_BACK)){
        Serial.println("== BACK ==");
        // TODO : traitement sens inverse
      }else if(input.startsWith(DIRECTION_BACK_CANCEL)){
        Serial.println("== BACK_STOP ==");
        // TODO : traitement sens inverse
      }
    }
  }
}
