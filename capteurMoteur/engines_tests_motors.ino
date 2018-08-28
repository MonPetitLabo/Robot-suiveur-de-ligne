#include <QTRSensors.h>

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

// Power
// 8 && 16
// GND
// 4 && 13
// MotorG
// 3 && 6
// MotorD
// 14 && 11

const int sensorInput1 = 0;
const int sensorInput2 = 1;
const int sensorInput3 = 2;

QTRSensorsAnalog sensors ((unsigned char[]) {sensorInput1, sensorInput2, sensorInput3}, 3);

void setup() {
  // Power pins 
  pinMode(ENABLEG,OUTPUT);
  pinMode(ENABLED,OUTPUT);

  // Rotation pins
  pinMode(DIRAG,OUTPUT);
  pinMode(DIRBG,OUTPUT);
  pinMode(DIRAD,OUTPUT);
  pinMode(DIRBD,OUTPUT);

  // Use log output
  Serial.begin(9600);

  Serial.println(">>Calibration...");
  for(int i =0; i < 250; i++){
     
    sensors.calibrate( );
    delay(20);
  }
  Serial.println(">>Calibration end...");
}

void loop() {
  unsigned int sensorsValues[3];
  int position = sensors.readLine(sensorsValues );
  position = position - 1000;
  
  Serial.print(">>");
  Serial.println(position);

  if(position < -250){
    analogWrite(ENABLEG,0);
    analogWrite(ENABLED,255); 
    Serial.println(">DD");
  }else if(position > 250){
    analogWrite(ENABLEG,255); 
    analogWrite(ENABLED,0); 
    Serial.println(">GG");
  }else{
    analogWrite(ENABLEG,255); 
    analogWrite(ENABLED,255);
    Serial.println(">GD");
  }
    
  digitalWrite(DIRAG,HIGH);
  digitalWrite(DIRBG,LOW);

  digitalWrite(DIRAD,HIGH);
  digitalWrite(DIRBD,LOW);
  
  delay(50);
}
