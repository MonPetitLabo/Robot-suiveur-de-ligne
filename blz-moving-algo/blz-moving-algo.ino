#define ENABLE_MOTOR_LEFT 5
#define DIRA_MOTOR_LEFT 3
#define DIRB_MOTOR_LEFT 4

#define ENABLE_MOTOR_RIGHT 10
#define DIRA_MOTOR_RIGHT 11
#define DIRB_MOTOR_RIGHT 12

// Input data
#define MIN_INPUT 0
#define MAX_INPUT 1023
#define DEFAULT_ZERO 525

// Motor data
#define MIN_MOTOR 0
#define MAX_MOTOR 255

#define MAX_VALUE_MOTOR 255

const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output

void setup() {
  // put your setup code here, to run once:
  pinMode(ENABLE_MOTOR_LEFT, OUTPUT);
  pinMode(DIRA_MOTOR_LEFT, OUTPUT);
  pinMode(DIRB_MOTOR_LEFT, OUTPUT);

  pinMode(ENABLE_MOTOR_RIGHT, OUTPUT);
  pinMode(DIRA_MOTOR_RIGHT, OUTPUT);
  pinMode(DIRB_MOTOR_RIGHT, OUTPUT);

  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);

  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:

  int speed = readSpeed();

  /*if (speed < 0) {
    speed = speed * (-1);
    analogWrite(ENABLE_MOTOR_LEFT, speed); //enable on
    digitalWrite(DIRA_MOTOR_LEFT, HIGH); //one way
    digitalWrite(DIRB_MOTOR_LEFT, LOW);
    analogWrite(ENABLE_MOTOR_RIGHT, speed); //enable on
    digitalWrite(DIRA_MOTOR_RIGHT, LOW); //one way
    digitalWrite(DIRB_MOTOR_RIGHT, HIGH);
  } else if (speed > 0) {
    analogWrite(ENABLE_MOTOR_RIGHT, speed); //enable on
    digitalWrite(DIRA_MOTOR_RIGHT, HIGH); //one way
    digitalWrite(DIRB_MOTOR_RIGHT, LOW);
    analogWrite(ENABLE_MOTOR_LEFT, speed); //enable on
    digitalWrite(DIRA_MOTOR_LEFT, LOW); //one way
    digitalWrite(DIRB_MOTOR_LEFT, HIGH);
  } else { // speed = 0
    analogWrite(ENABLE_MOTOR_LEFT, MAX_VALUE_MOTOR); //enable on
    digitalWrite(DIRA_MOTOR_LEFT, HIGH); //one way
    digitalWrite(DIRB_MOTOR_LEFT, LOW);
    analogWrite(ENABLE_MOTOR_RIGHT, MAX_VALUE_MOTOR); //enable on
    digitalWrite(DIRA_MOTOR_RIGHT, HIGH); //one way
    digitalWrite(DIRB_MOTOR_RIGHT, LOW);
  }*/

    analogWrite(ENABLE_MOTOR_LEFT, MAX_VALUE_MOTOR); //enable on
    digitalWrite(DIRA_MOTOR_LEFT, HIGH); //one way
    digitalWrite(DIRB_MOTOR_LEFT, LOW);
    analogWrite(ENABLE_MOTOR_RIGHT, MAX_VALUE_MOTOR); //enable on
    digitalWrite(DIRA_MOTOR_RIGHT, HIGH); //one way
    digitalWrite(DIRB_MOTOR_RIGHT, LOW);

  delay(500);

}

int readSpeed() {
  int currentXAxis = analogRead(X_pin); // Range 0 - 1023
  Serial.print("Angle joystick:");
  Serial.print(currentXAxis);
  Serial.print("\t");
  int speed = currentXAxis - DEFAULT_ZERO;
  speed = speed / (MAX_INPUT/(2 * MAX_MOTOR));

  if (speed > MAX_VALUE_MOTOR) {
    speed = MAX_VALUE_MOTOR; // Max value of motor
  }

  if (speed < -MAX_VALUE_MOTOR) {
    speed = -MAX_VALUE_MOTOR;
  }

  Serial.print(speed);
  Serial.print("\n");

  return speed;
}

