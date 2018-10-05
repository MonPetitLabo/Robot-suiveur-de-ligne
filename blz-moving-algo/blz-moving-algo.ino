#include <string.h>
#include <QTRSensors.h>

#define ENABLE_MOTOR_LEFT 5
#define DIRA_MOTOR_LEFT 3
#define DIRB_MOTOR_LEFT 4

#define ENABLE_MOTOR_RIGHT 10
#define DIRA_MOTOR_RIGHT 11
#define DIRB_MOTOR_RIGHT 12

// Input data
#define MIN_INPUT 0
#define MAX_INPUT 2000
#define DEFAULT_ZERO 1000

// Motor data
#define MIN_MOTOR 0
#define MAX_MOTOR 255


#define MAX_VALUE_MOTOR 200 // Max value : 255

#define STOP_BEFORE_TURN 0

#define ZONE_MORT_POSITIF 500
#define ZONE_MORT_NEGATIF -500

const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output

const int CAPTEUR1 = 2;
const int CAPTEUR2 = 1;
const int CAPTEUR3 = 0;

const bool ENABLE_CALIBRATION = true;

QTRSensorsAnalog sensors ((unsigned char[]) {CAPTEUR1, CAPTEUR2, CAPTEUR3}, 3);


void setup() {

  Serial.begin(9600);
    
  // put your setup code here, to run once:
  pinMode(ENABLE_MOTOR_LEFT, OUTPUT);
  pinMode(DIRA_MOTOR_LEFT, OUTPUT);
  pinMode(DIRB_MOTOR_LEFT, OUTPUT);

  pinMode(ENABLE_MOTOR_RIGHT, OUTPUT);
  pinMode(DIRA_MOTOR_RIGHT, OUTPUT);
  pinMode(DIRB_MOTOR_RIGHT, OUTPUT);

  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);

  //if(true) {
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
  //}

  

}

int oldDirection = 0;
int newDirection = 0;

void loop() {
  // put your main code here, to run repeatedly:

  oldDirection = newDirection;
  newDirection = getDirection();

  Serial.print("Direction:");
  Serial.print(newDirection);

  if(newDirection >= ZONE_MORT_NEGATIF && 
    newDirection <= ZONE_MORT_POSITIF) {
      Serial.print("\t --> No turn"); 
      goStraight();
  } else {
    Serial.print("\t --> Turn required"); 
    if(newDirection <= ZONE_MORT_NEGATIF) {
      Serial.print("\t --> turn right");
      turnRight(needFastTurn(newDirection, oldDirection));
    } else if (newDirection >= ZONE_MORT_POSITIF) {
      Serial.print("\t --> turn left");
      turnLeft(needFastTurn(newDirection, oldDirection));
    }     
    delay(15);
    stop();
  }
}

boolean needFastTurn(int currentDirection, int previousDirection) {
  return true;
}

void stop() {
    analogWrite(ENABLE_MOTOR_LEFT, 0); //enable on
    analogWrite(ENABLE_MOTOR_RIGHT, 0); //enable on
}

void turnLeft(boolean isFastTurn) {
    analogWrite(ENABLE_MOTOR_LEFT, MAX_VALUE_MOTOR); //enable on
    digitalWrite(DIRA_MOTOR_LEFT, HIGH); //one way
    digitalWrite(DIRB_MOTOR_LEFT, LOW);
    if(isFastTurn) {
      analogWrite(ENABLE_MOTOR_RIGHT, MAX_VALUE_MOTOR); //enable on
    } else {
      analogWrite(ENABLE_MOTOR_RIGHT, 0); //enable on
    }
    digitalWrite(DIRA_MOTOR_RIGHT, LOW); //one way
    digitalWrite(DIRB_MOTOR_RIGHT, HIGH);
    Serial.println("\tTurn Left");
}

void turnRight(boolean isFastTurn) {
    analogWrite(ENABLE_MOTOR_RIGHT, MAX_VALUE_MOTOR); //enable on
    digitalWrite(DIRA_MOTOR_RIGHT, HIGH); //one way
    digitalWrite(DIRB_MOTOR_RIGHT, LOW);

    if(isFastTurn) {
      analogWrite(ENABLE_MOTOR_LEFT, MAX_VALUE_MOTOR); //enable on
    } else {
      analogWrite(ENABLE_MOTOR_LEFT, 0); //enable on
    }
    digitalWrite(DIRA_MOTOR_LEFT, LOW); //one way
    digitalWrite(DIRB_MOTOR_LEFT, HIGH);
    Serial.println("\tTurn Right");
}

void goStraight() {
    analogWrite(ENABLE_MOTOR_LEFT, MAX_VALUE_MOTOR); //enable on
    digitalWrite(DIRA_MOTOR_LEFT, HIGH); //one way
    digitalWrite(DIRB_MOTOR_LEFT, LOW);
    analogWrite(ENABLE_MOTOR_RIGHT, MAX_VALUE_MOTOR); //enable on
    digitalWrite(DIRA_MOTOR_RIGHT, HIGH); //one way
    digitalWrite(DIRB_MOTOR_RIGHT, LOW);
    Serial.println("Tout droit");
}

int getDirection() {
  int currentXAxis = readCapteur(); // Range -1000 - 1000 

  return currentXAxis;
}

int readCapteur() {
  unsigned int sensorsValues[3];
  int position = sensors.readLine(sensorsValues );
  position = position - 1000;

  Serial.print("Position : ");
  Serial.println(position);

  return position;
}

