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
#define VIRAGE_3 1

// infrarouge
#define RECEIVER 9

// capteur
#define NUM_SENSORS 3
// définition des ports analogiques
QTRSensorsAnalog qtr((char[]) {0, 1, 2}, NUM_SENSORS);
//infrarouge
IRrecv irrecv(RECEIVER);
decode_results results;

//////////////////////////////////////
// BLUETOOTH
/////////////////////////////////////
const int BLUETOOTH_RX = 1; // TX3
const int BLUETOOTH_TX = 0; // RX3
const int BLUETOOTH_KEY = 2; // KEY
const int BLUETOOTH_STATE = 9; // STATE
const int AT_COMMAND_WAIT_TIME = 250; // time in ms to wait after command printed


bool manualMode = false;
bool start = true;
String manualDirection = "a";
int maxSpeed = 20;
int minSpeed = 0;
int manuelSpeed = maxSpeed;
bool manualStart = true;
int affichageTime=0;
unsigned int sensors[3];


/**
 * Send a commond to the bluetooth module
 * 
 * command : command to send
 * isATCommand : specify if the command is for the module configuration or is to send to connected devices
 */
void runCommand(String command, boolean isATCommand){
  if(command.length() > 0){

    // activate command mode
    if(isATCommand){
      digitalWrite(BLUETOOTH_KEY, HIGH);
    }

    // print command
    Serial.print(command);
    if(isATCommand){
      Serial.print("\r\n");
    }else{
      Serial.print("@");
    }

    // unactive command mode
    if(isATCommand){
      digitalWrite(BLUETOOTH_KEY, LOW);
    }
  }
}

void sendMessage(String msg){
  if(digitalRead(BLUETOOTH_STATE)== HIGH){
      runCommand(msg, false);
  }
}

void setup() {
  //---set pin direction
  pinMode(ENABLE_RIGHT,OUTPUT);
  pinMode(DIRA_RIGHT,OUTPUT);
  pinMode(DIRB_RIGHT,OUTPUT);
  pinMode(ENABLE_LEFT,OUTPUT);
  pinMode(DIRA_LEFT,OUTPUT);
  pinMode(DIRB_LEFT,OUTPUT);

  pinMode(BLUETOOTH_STATE,INPUT);

  // Bluetooth pins
  pinMode(BLUETOOTH_KEY, OUTPUT);
  digitalWrite(BLUETOOTH_KEY, LOW);
  Serial.begin(9600);

  runCommand("AT+NAME=FABLAB", true);
  runCommand("AT+NAME", true);
//  runCommand("AT+PSWD=2018", true);
//  runCommand("AT+PSWD?", true);

  delay(2000);
  
  calibrate();
  irrecv.enableIRIn();
}



void calibrate() {
//  lcd.clear();
//  lcd.setCursor(0, 0);
//  lcd.print("Calibration");
//  lcd.setCursor(0, 1);
//  lcd.print("dans 4s");
  sendMessage("Calibration dans 4s");
  delay(4000);
//  lcd.clear();
//  lcd.print("Debut");
//  lcd.setCursor(0, 1);
//  lcd.print("calibration");
  //Serial.println("calibration");
  sendMessage("Début calibration");
  int i;
  for (i = 0; i < 500; i++)  // make the calibration take about 5 seconds
  {
    qtr.calibrate(QTR_EMITTERS_ON);
    delay(20);
  }
  sendMessage("Fin calibration");
  //Serial.println("fin calibration");
//  lcd.clear();
//  lcd.setCursor(0, 0);
//  lcd.print("Calibration");
//  lcd.setCursor(0, 1);
//  lcd.print("terminee");
  delay(1000);
}

void rouler(int vitesse) {
    actionnerMoteurGauche(vitesse);
    actionnerMoteurDroite(vitesse);
}

void actionnerMoteurGauche(int vitesse) {
String vitesseStr = String(vitesse);
//  sendMessage("G " + vitesseStr);
  digitalWrite(ENABLE_LEFT,HIGH);
  if(vitesse < 0) {
    digitalWrite(DIRA_LEFT,LOW);
    digitalWrite(DIRB_LEFT,HIGH);
    analogWrite(ENABLE_LEFT,(vitesse * -1)+140);
  } else if(vitesse > 0) {
    digitalWrite(DIRA_LEFT,HIGH);
    digitalWrite(DIRB_LEFT,LOW);
    analogWrite(ENABLE_LEFT,vitesse+140);
  } else {
    digitalWrite(ENABLE_LEFT,LOW);
  }
}

void actionnerMoteurDroite(int vitesse) {
String vitesseStr = String(vitesse);
//  sendMessage("D " + vitesseStr);
  digitalWrite(ENABLE_RIGHT,HIGH);
  if(vitesse < 0) {
    digitalWrite(DIRA_RIGHT,LOW);
    digitalWrite(DIRB_RIGHT,HIGH);
    analogWrite(ENABLE_RIGHT,(vitesse * -1)+140);
  } else if(vitesse > 0) {
    digitalWrite(DIRA_RIGHT,HIGH);
    digitalWrite(DIRB_RIGHT,LOW);
    analogWrite(ENABLE_RIGHT,vitesse+140);
  } else {
    digitalWrite(ENABLE_RIGHT,LOW);
  }
}

//void displayPosition(int position) {
//  affichageTime++;
//  if(affichageTime==255){
//    lcd.clear();
//    lcd.setCursor(0, 0);
//    lcd.print("1:" +String(sensors[0]));
//    lcd.setCursor(0, 1);
//    lcd.print("2:" +String(sensors[1]));
//    lcd.setCursor(8, 1);
//    lcd.print("3:" +String(sensors[2]));
//    affichageTime=0;
//  }
//}

void manageManualMode() {
    if(manualStart){
      if(manualDirection=="a"){
        rouler(manuelSpeed);
        float pourcentageVitesse = float(manuelSpeed)/maxSpeed*100;
        String vitesse = String(pourcentageVitesse) + "%";
//        lcd.print("Avance : " + vitesse);
      }else if(manualDirection=="r"){
        rouler(manuelSpeed*-1);
        float pourcentageVitesse = float(manuelSpeed)/maxSpeed*100;
        String vitesse = String(pourcentageVitesse) + "%";
//        lcd.print("Recule : "+ vitesse);
      }else if(manualDirection=="d1"){
        getVitesseFromPosition(750);
  //      lcd.print("Virage droite 1");
      }else if(manualDirection=="d2"){
        getVitesseFromPosition(500);
  //      lcd.print("Virage droite 2");
      }else if(manualDirection=="d3"){
        getVitesseFromPosition(0);
  //      lcd.print("Virage droite 3");
      }else if(manualDirection=="g1"){
        getVitesseFromPosition(1250);
  //      lcd.print("Virage gauche 1");
      }else if(manualDirection=="g2"){
        getVitesseFromPosition(1500);
  //      lcd.print("Virage gauche 2");
      }else if(manualDirection=="g3"){
        getVitesseFromPosition(2000);
  //      lcd.print("Virage gauche 3");
      }
    }else{
      rouler(0);
  //    lcd.print("Pause");
    }
}

void getVitesseFromPosition(int position){
  position = position - 1000;
 String positionStr = String(position);
sendMessage("P " + positionStr);

  //Gestion moteur droit
  if(position>=0){
    actionnerMoteurDroite(maxSpeed);
  }else{
    int vitesse = maxSpeed + int((float(position)/float(500/maxSpeed)));
    actionnerMoteurDroite(vitesse);
  }

//Gestion moteur gauche
  if(position<=0){
    actionnerMoteurGauche(maxSpeed);
  }else{
    int vitesse = maxSpeed - int((float(position)/float(500/maxSpeed)));
    actionnerMoteurGauche(vitesse);
  }
}




void translateIR(){
  if(irrecv.decode(&results)) {
//    lcd.clear();
//    lcd.print("Commande");
//    lcd.setCursor(0, 1);
    switch(results.value) {
      case 0xFFA25D:
        start = !start;
//        lcd.clear();
//        lcd.setCursor(0, 0);
        if(start){
     //     lcd.print("Start");
        }else{
     //     lcd.print("Stop");
        }
     //   lcd.setCursor(0, 1);
     //   lcd.print("Robot");
        break;
      case 0xFFE21D:
        manualMode = !manualMode;
//        lcd.clear();
//        lcd.setCursor(0, 0);
//        lcd.print("Change mode");
        break;
      case 0xFF629D: 
        manualDirection = "a";
//        lcd.clear();
//        lcd.setCursor(0, 0);
//        lcd.print("Avancer");
        break;
      case 0xFF22DD:
        if(manualDirection == "g1"){
          manualDirection = "g2";
        }else if(manualDirection == "g2"){
          manualDirection = "g3";
        }else{
          manualDirection = "g1";
        }
//        lcd.clear();
//        lcd.setCursor(0, 0);
//        lcd.print("Braquage gauche");
      
        break;
      case 0xFF02FD:
        manualStart = !manualStart;
//        lcd.clear();
//        lcd.setCursor(0, 0);
//        if(manualStart){
//          lcd.print("Resume robot");
//        }else{
//          lcd.print("Mise en pause");
//        }
        
        break;
      case 0xFFC23D:
        if(manualDirection == "d1"){
          manualDirection = "d2";
        }else if(manualDirection == "d2"){
          manualDirection = "d3";
        }else{
          manualDirection = "d1";
        }
//        lcd.clear();
//        lcd.setCursor(0, 0);
//        lcd.print("Braquage droite");
      
        break;
      case 0xFFE01F:
        if(manuelSpeed >= minSpeed+10){
          manuelSpeed = manuelSpeed - 10;
        }else{
          manuelSpeed = minSpeed;
        }
//        lcd.clear();
//        lcd.setCursor(0, 0);
//        lcd.print("Freiner");
        break;
      case 0xFFA857:
        manualDirection = "r";
//        lcd.clear();
//        lcd.setCursor(0, 0);
//        lcd.print("Reculer");
        break;
      case 0xFF906F:
        if(manuelSpeed <= maxSpeed - 10){
          manuelSpeed = manuelSpeed + 10;
        }else{
          manuelSpeed = maxSpeed;
        }
//        lcd.clear();
//        lcd.setCursor(0, 0);
//        lcd.print("Accelerer");
        break;
      case 0xFF9867:
        start = false;
        calibrate();
        break;
//      case 0xFFB04F: lcd.print("ST/REPT");    break;
//      case 0xFF6897: lcd.print("0");    break;
//      case 0xFF30CF: lcd.print("1");    break;
//      case 0xFF18E7: lcd.print("2");    break;
//      case 0xFF7A85: lcd.print("3");    break;
//      case 0xFF10EF: lcd.print("4");    break;
//      case 0xFF38C7: lcd.print("5");    break;
//      case 0xFF5AA5: lcd.print("6");    break;
//      case 0xFF42BD: lcd.print("7");    break;
//      case 0xFF4AB5: lcd.print("8");    break;
//      case 0xFF52AD: lcd.print("9");    break;
//      case 0xFFFFFFFF: lcd.print(" REPEAT");break;  
//      default: 
//        lcd.print(" other button   ");
    }
    delay(500);
    irrecv.resume();
  }
  
}






void loop() {
  //translateIR();
  
  int position = qtr.readLine(sensors,QTR_EMITTERS_ON);
  
  
  if(start){
     if(manualMode){
        
        manageManualMode();
      }else{
        getVitesseFromPosition(position);
      }
  }else{
    rouler(0);
  }
}
