//#include <IRremote.h>

#include <LiquidCrystal.h>


#include <QTRSensors.h>

//www.elegoo.com
//2016.12.12

/************************
Exercise the motor using
the L293D chip
************************/

// moteurs
#define ENABLE_RIGHT 5
#define DIRA_RIGHT 3
#define DIRB_RIGHT 4
#define ENABLE_LEFT 6
#define DIRA_LEFT 7
#define DIRB_LEFT 8

#define VIRAGE_1 .5
#define VIRAGE_2 .0
#define VIRAGE_3 .5

// infrarouge
#define RECEIVER 9

// capteur
#define NUM_SENSORS 3
// définition des ports analogiques
QTRSensorsAnalog qtr((char[]) {0, 1, 2}, NUM_SENSORS);
//infrarouge
//IRrecv irrecv(RECEIVER);
//decode_results results;

//LCD
LiquidCrystal lcd(1, 2, 10, 11, 12, 13);

String oldMouve = "";
bool reverse = false;
bool manualMode = false;


void setup() {
  //---set pin direction
  /*pinMode(ENABLE_RIGHT,OUTPUT);
  pinMode(DIRA_RIGHT,OUTPUT);
  pinMode(DIRB_RIGHT,OUTPUT);
  pinMode(ENABLE_LEFT,OUTPUT);
  pinMode(DIRA_LEFT,OUTPUT);
  pinMode(DIRB_LEFT,OUTPUT);*/
  //Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Debut");
  lcd.setCursor(0, 1);
  lcd.print("calibration");
  int i;
  for (i = 0; i < 250; i++)  // make the calibration take about 5 seconds
  {
    qtr.calibrate();
    delay(20);
  }
  // print the calibration minimum values measured when emitters were on
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtr.calibratedMinimumOn[i]);
    Serial.print(' ');
  }
  Serial.println();

  // print the calibration maximum values measured when emitters were on
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtr.calibratedMaximumOn[i]);
    Serial.print(' ');
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibration");
  lcd.setCursor(0, 1);
  lcd.print("terminee");
  //irrecv.enableIRIn();
  delay(1000);
}

void rouler(int vitesse) {
    actionnerMoteur1(vitesse);
    actionnerMoteur2(vitesse);
}

void actionnerMoteur1(int vitesse) {
  digitalWrite(ENABLE_RIGHT,HIGH);
  if(vitesse < 0) {
    digitalWrite(DIRA_RIGHT,LOW);
    digitalWrite(DIRB_RIGHT,HIGH);
    analogWrite(ENABLE_RIGHT,vitesse * -1);
  } else if(vitesse > 0) {
    digitalWrite(DIRA_RIGHT,HIGH);
    digitalWrite(DIRB_RIGHT,LOW);
    analogWrite(ENABLE_RIGHT,vitesse);
  } else {
    digitalWrite(ENABLE_RIGHT,LOW);
  }
}

void actionnerMoteur2(int vitesse) {
  digitalWrite(ENABLE_LEFT,HIGH);
  if(vitesse < 0) {
    digitalWrite(DIRA_LEFT,LOW);
    digitalWrite(DIRB_LEFT,HIGH);
    analogWrite(ENABLE_LEFT,vitesse * -1);
  } else if(vitesse > 0) {
    digitalWrite(DIRA_LEFT,HIGH);
    digitalWrite(DIRB_LEFT,LOW);
    analogWrite(ENABLE_LEFT,vitesse);
  } else {
    digitalWrite(ENABLE_LEFT,LOW);
  }
}

void avancerDroite(int niveauBraquage) {
  if(niveauBraquage == 1) {
    actionnerMoteur1(255);
    actionnerMoteur2(255 * VIRAGE_1);
  } else if(niveauBraquage == 2) {
    actionnerMoteur1(255);
    actionnerMoteur2(255 * VIRAGE_2);
  } else if(niveauBraquage == 3) {
    actionnerMoteur1(255);
    actionnerMoteur2(-255 * VIRAGE_3);
  }
}

void avancerGauche(int niveauBraquage) {
  if(niveauBraquage == 1) {
    actionnerMoteur2(255);
    actionnerMoteur1(255 * VIRAGE_1);
  } else if(niveauBraquage == 2) {
    actionnerMoteur2(255);
    actionnerMoteur1(255 * VIRAGE_2);
  } else if(niveauBraquage == 3) {
    actionnerMoteur2(255);
    actionnerMoteur1(-255 * VIRAGE_3);
  }
}

void loop() {
  /*if(irrecv.decode(&results)) {
    Serial.print("Telecommande : ");
    Serial.println(results.value);
    delay(100);
    irrecv.resume();
  }*/

  if(manualMode){
    Serial.print("Mode manuel");
  }else{
    unsigned int sensors[3];
  // get calibrated sensor values returned in the sensors array, along with the line
  // position, which will range from 0 to 2000, with 1000 corresponding to the line
  // over the middle sensor.
  int position = qtr.readLine(sensors);
  //getDirectionFromPosition(position);
  String positionStr = "Position ";
  Serial.print(positionStr);
  Serial.println(position);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(positionStr);
  lcd.setCursor(0, 1);
  lcd.print(position);
  }
  
  delay(2000);
  
}

void getDirectionFromPosition(int position) {
  if(position >= 900 && position <= 1100) {
    Serial.println("Marche avant");
    rouler(255);
    reverse = false;
    oldMouve = "A";
  } else if(position >= 700 && position < 900) {
    Serial.println("Droite 1");
    avancerDroite(1);
    reverse = false;
    oldMouve = "D1";
  } else if(position >= 400 && position < 700) {
    Serial.println("Droite 2");
    avancerDroite(2);
    reverse = false;
    oldMouve = "D2";
  } else if(position > 0 && position < 400) {
    Serial.println("Droite 3");
    avancerDroite(3);
    reverse = false;
    oldMouve = "D3";
  } else if(position > 1100 && position <= 1300) {
    Serial.println("Gauche 1");
    avancerGauche(1);
    reverse = false;
    oldMouve = "G1";
  } else if(position > 1300 && position <= 1600) {
    Serial.println("Gauche 2");
    avancerGauche(2);
    reverse = false;
    oldMouve = "G2";
  } else if(position > 1600 && position < 2000) {
    Serial.println("Gauche 3");
    avancerGauche(3);
    reverse = false;
    oldMouve = "G3";
  } else if(position == 0 || position == 2000) {
    findLine();
  }
}

void findLine() {
  if(! reverse) {
    if(oldMouve == "A") {
      Serial.println("find line : marche arrière");
      rouler(-255);
      oldMouve = "R";
    } else if(oldMouve == "R") {
      Serial.println("find line : marche avant");
      rouler(255);
      oldMouve = "A";
    } else if(oldMouve.startsWith("D")) {
      Serial.println("find line : gauche 1");
      avancerGauche(1);
      oldMouve = "G1";
    } else if(oldMouve.startsWith("G")) {
      Serial.println("find line : droite 1");
      avancerDroite(1);
      oldMouve = "D1";
    }
  } else {
    if(oldMouve == "A" || oldMouve == "R" || oldMouve.endsWith("3")) {
      Serial.println("find line : last chance");
      lastChance();
    } else if(oldMouve == "D1") {
      Serial.println("find line : droite 2");
      avancerDroite(2);
      oldMouve = "D2";
    } else if(oldMouve == "D2") {
      Serial.println("find line : droite 3");
      avancerDroite(3);
      oldMouve = "D3";
    } else if(oldMouve == "G1") {
      Serial.println("find line : gauche 2");
      avancerGauche(2);
      oldMouve = "G2";
    } else if(oldMouve == "G2") {
      Serial.println("find line : gauche 3");
      avancerGauche(3);
      oldMouve = "G3";
    }
  }
  reverse = true;
}

void lastChance() {
  //TODO
  Serial.println("last chance : stop");
  rouler(0);
}
