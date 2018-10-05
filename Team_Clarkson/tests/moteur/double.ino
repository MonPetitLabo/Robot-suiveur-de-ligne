//origin
//www.elegoo.com
//2016.12.12

/************************
Run two motors at the
same speed using L293D
Needed
2 Motors 
1 l293D
1 9v battery
************************/

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

void setup() {
  // Power pins 
  pinMode(ENABLEG,OUTPUT);
  pinMode(ENABLED,OUTPUT);

  // Rotation pins
  pinMode(DIRAG,OUTPUT);
  pinMode(DIRBG,OUTPUT);
  pinMode(DIRAD,OUTPUT);
  pinMode(DIRBD,OUTPUT);

  // Use log ouutput
  Serial.begin(9600);
}

void loop() {

  Serial.println("PWM full on the 2 motors");

  //---PWM example, full on the 2 motors
  analogWrite(ENABLEG,255); 
  analogWrite(ENABLED,255); 

  digitalWrite(DIRAG,HIGH);
  digitalWrite(DIRBG,LOW);

  digitalWrite(DIRAD,HIGH);
  digitalWrite(DIRBD,LOW);

  delay(2000);
}
   

