#include <IRremote.h>
#include <IRremoteInt.h>



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
IRrecv irrecv(RECEIVER);
decode_results results;

//LCD
LiquidCrystal lcd(1, 2, 10, 11, 12, 13);

String oldMouve = "";
bool reverse = false;
bool manualMode = false;
bool start = false;
String manualDirection = "a";
int maxSpeed = 180;
int minSpeed = 140;
int manuelSpeed = maxSpeed;
bool manualStart = true;

void translateIR(){
  lcd.clear();
  lcd.print("interrupt");
  if(irrecv.decode(&results)) {
    lcd.clear();
    lcd.print("Commande");
    lcd.setCursor(0, 1);
    switch(results.value) {
      case 0xFFA25D:
        start = !start;
        lcd.clear();
        lcd.setCursor(0, 0);
        if(start){
          lcd.print("Start");
        }else{
          lcd.print("Stop");
        }
        lcd.setCursor(0, 1);
        lcd.print("Robot");
        break;
      case 0xFFE21D:
        manualMode = !manualMode;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Change mode");
        break;
      case 0xFF629D: 
        manualDirection = "a";
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Avancer");
        break;
      case 0xFF22DD:
        if(manualDirection == "g1"){
          manualDirection = "g2";
        }else if(manualDirection == "g2"){
          manualDirection = "g3";
        }else{
          manualDirection = "g1";
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Braquage gauche");
      
        break;
      case 0xFF02FD:
        manualStart = !manualStart;
        lcd.clear();
        lcd.setCursor(0, 0);
        if(manualStart){
          lcd.print("Resume robot");
        }else{
          lcd.print("Mise en pause");
        }
        
        break;
      case 0xFFC23D:
        if(manualDirection == "d1"){
          manualDirection = "d2";
        }else if(manualDirection == "d2"){
          manualDirection = "d3";
        }else{
          manualDirection = "d1";
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Braquage droite");
      
        break;
      case 0xFFE01F:
        if(manuelSpeed >= minSpeed+10){
          manuelSpeed = manuelSpeed - 10;
        }else{
          manuelSpeed = minSpeed;
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Freiner");
        break;
      case 0xFFA857:
        manualDirection = "r";
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Reculer");
        break;
      case 0xFF906F:
        if(manuelSpeed <= maxSpeed - 10){
          manuelSpeed = manuelSpeed + 10;
        }else{
          manuelSpeed = maxSpeed;
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Accelerer");
        break;
      case 0xFF9867: lcd.print("EQ");    break;
      case 0xFFB04F: lcd.print("ST/REPT");    break;
      case 0xFF6897: lcd.print("0");    break;
      case 0xFF30CF: lcd.print("1");    break;
      case 0xFF18E7: lcd.print("2");    break;
      case 0xFF7A85: lcd.print("3");    break;
      case 0xFF10EF: lcd.print("4");    break;
      case 0xFF38C7: lcd.print("5");    break;
      case 0xFF5AA5: lcd.print("6");    break;
      case 0xFF42BD: lcd.print("7");    break;
      case 0xFF4AB5: lcd.print("8");    break;
      case 0xFF52AD: lcd.print("9");    break;
      case 0xFFFFFFFF: lcd.print(" REPEAT");break;  
      default: 
        lcd.print(" other button   ");
    }
    delay(500);
  }
  irrecv.resume();
}

void setup() {
  //---set pin direction
  pinMode(ENABLE_RIGHT,OUTPUT);
  pinMode(DIRA_RIGHT,OUTPUT);
  pinMode(DIRB_RIGHT,OUTPUT);
  pinMode(ENABLE_LEFT,OUTPUT);
  pinMode(DIRA_LEFT,OUTPUT);
  pinMode(DIRB_LEFT,OUTPUT);
  //Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Debut");
  lcd.setCursor(0, 1);
  lcd.print("calibration");
  //Serial.println("calibration");
  int i;
  for (i = 0; i < 500; i++)  // make the calibration take about 5 seconds
  {
    qtr.calibrate();
    delay(20);
  }
  //Serial.println("fin calibration");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibration");
  lcd.setCursor(0, 1);
  lcd.print("terminee");
  irrecv.enableIRIn();
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
    actionnerMoteur1(maxSpeed);
    actionnerMoteur2(maxSpeed * VIRAGE_1);
  } else if(niveauBraquage == 2) {
    actionnerMoteur1(maxSpeed);
    actionnerMoteur2(maxSpeed * VIRAGE_2);
  } else if(niveauBraquage == 3) {
    actionnerMoteur1(maxSpeed);
    actionnerMoteur2(maxSpeed * -1 * VIRAGE_3);
  }
}

void avancerGauche(int niveauBraquage) {
  if(niveauBraquage == 1) {
    actionnerMoteur2(maxSpeed);
    actionnerMoteur1(maxSpeed * VIRAGE_1);
  } else if(niveauBraquage == 2) {
    actionnerMoteur2(maxSpeed);
    actionnerMoteur1(maxSpeed * VIRAGE_2);
  } else if(niveauBraquage == 3) {
    actionnerMoteur2(maxSpeed);
    actionnerMoteur1(maxSpeed * -1 * VIRAGE_3);
  }
}

void loop() {
 if(irrecv.decode(&results)) {
    translateIR();
    irrecv.resume();
  }
  
  if(start){
     if(manualMode){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Mode manuel");
        lcd.setCursor(0, 1);
        if(manualStart){
          if(manualDirection=="a"){
            rouler(manuelSpeed);
            float pourcentageVitesse = float(manuelSpeed)/maxSpeed*100;
            String vitesse = String(pourcentageVitesse) + "%";
            lcd.print("Avance : " + vitesse);
          }else if(manualDirection=="r"){
            rouler(manuelSpeed*-1);
            float pourcentageVitesse = float(manuelSpeed)/maxSpeed*100;
            String vitesse = String(pourcentageVitesse) + "%";
            lcd.print("Recule : "+ vitesse);
          }else if(manualDirection=="d1"){
            avancerDroite(1);
            lcd.print("Virage droite 1");
          }else if(manualDirection=="d2"){
            avancerDroite(2);
            lcd.print("Virage droite 2");
          }else if(manualDirection=="d3"){
            avancerDroite(3);
            lcd.print("Virage droite 3");
          }else if(manualDirection=="g1"){
            avancerGauche(1);
            lcd.print("Virage gauche 1");
          }else if(manualDirection=="g2"){
            avancerGauche(2);
            lcd.print("Virage gauche 2");
          }else if(manualDirection=="g3"){
            avancerGauche(3);
            lcd.print("Virage gauche 3");
          }
        }else{
          rouler(0);
          lcd.print("Pause");
        }
        
        
      }else{
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Mode auto");
        unsigned int sensors[3];
      // get calibrated sensor values returned in the sensors array, along with the line
      // position, which will range from 0 to 2000, with 1000 corresponding to the line
      // over the middle sensor.
      int position = qtr.readLine(sensors);
      getDirectionFromPosition(position);
      //String positionStr = "Position ";
      //Serial.print(positionStr);
      //Serial.println(position);
      ///lcd.clear();
      //lcd.setCursor(0, 0);
      //lcd.print(positionStr);
      //lcd.setCursor(0, 1);
      //lcd.print(position);
      }
  }else{
    rouler(0);
  }
 
  
  delay(10);
}

void getDirectionFromPosition(int position) {
  if(position >= 900 && position <= 1100) {
    rouler(maxSpeed);
    reverse = false;
    oldMouve = "A";
    lcd.setCursor(0, 1);
    lcd.print("cmd a, pos:" +String(position));
  } else if(position >= 700 && position < 900) {
    avancerDroite(1);
    reverse = false;
    oldMouve = "D1";
    lcd.setCursor(0, 1);
    lcd.print("cmd d1, pos:" +String(position));
  } else if(position >= 400 && position < 700) {
    avancerDroite(2);
    reverse = false;
    oldMouve = "D2";
    lcd.setCursor(0, 1);
    lcd.print("cmd d2, pos:" +String(position));
  } else if(position > 0 && position < 400) {
    avancerDroite(3);
    reverse = false;
    oldMouve = "D3";
    lcd.setCursor(0, 1);
    lcd.print("cmd d3, pos:"+ String(position));
  } else if(position > 1100 && position <= 1300) {
    avancerGauche(1);
    reverse = false;
    oldMouve = "G1";
    lcd.setCursor(0, 1);
    lcd.print("cmd g1, pos:"+ String(position));
  } else if(position > 1300 && position <= 1600) {
    avancerGauche(2);
    reverse = false;
    oldMouve = "G2";
    lcd.setCursor(0, 1);
    lcd.print("cmd g2, pos:"+ String(position));
  } else if(position > 1600 && position < 2000) {
    avancerGauche(3);
    reverse = false;
    oldMouve = "G3";
    lcd.setCursor(0, 1);
    lcd.print("cmd g3, pos:"+ String(position));
  } else if(position == 0 || position == 2000) {
    lcd.setCursor(0, 1);
    lcd.print("No line found");
    delay(100);
    findLine();
  }
}

void findLine() {
  if(! reverse) {
    if(oldMouve == "A") {
      Serial.println("find line : marche arrière");
      rouler(maxSpeed * -1);
      oldMouve = "R";
    } else if(oldMouve == "R") {
      Serial.println("find line : marche avant");
      rouler(maxSpeed);
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
  lcd.setCursor(0, 1);
  lcd.print("Error stop !   ");
  rouler(0);
}
