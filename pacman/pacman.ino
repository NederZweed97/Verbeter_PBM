//Sarah Vogelzang
//Vebetering van BattleBot
#include <Adafruit_NeoPixel.h>
#include <QTRSensors.h>
#include "music.h"
//defineer alle pinnen en globale variabelen
//Neopixels
const byte neopin =  4;                                                                         //neopin
const byte numPixels  = 12;                                                                     //num neopixels
const int pixelFormat = NEO_RGB + NEO_KHZ800;                                                   //zet het pixelformat. LET OP! De neopixels op de robot zelf is RGB, de strip voor de underglow is GBR!
Adafruit_NeoPixel *pixels;                                                                      //maak een pointer met van type adrafruit neopixels
//om de neopixels individeel aan te sturen geef je ze een nummer.
//---- neopixels op de robot ------
const byte ll = 0;
const byte lr = 1;
const byte rr = 2;
const byte rl = 3;
//------------------------------------
//----- neopixels undeglow strip ------
const byte uglow1 = 4;
const byte uglow2 = 5;
const byte uglow3 = 6;
const byte uglow4 = 7;
const byte uglow5 = 8;
const byte uglow6 = 9;
const byte uglow7 = 10;
const byte uglow8 = 11;
//---------------------------------
//ultrasonic
const byte echoPin = 13;                                                                        //echopin
const byte triggerPin = 12;                                                                     //triggerpin
double duration;                                                                                //tijd tussen trigger en echo
double distance;                                                                                //gemeten afstand
double distanceLeft;                                                                            //gemeten afstand vooruit
double distanceRight;                                                                           //gemeten afstand rechts van de robot
double distanceForward;                                                                         //gemeten afstand links van de robot
const int minDistance = 45;                                                                     //minimale aftsand wat de robot moet houden
//wheels
const byte reverseRight = 9;
const byte forwardRight = 10;
const byte reverseLeft = 6;
const byte forwardLeft = 11;
const int startWheels = 255;                                                                    //de 'wake up' voor de motoren
const int wheelSpeedRight = 182;                                                                //gewenste snelheid rechter wiel
const int wheelSpeedLeft = 180;                                                                 //gewenste snelheid linker wiel
const int stopWheels = 0;                                                                       //stop de motoren
//pulsesensor
const byte speedSensorA = 2;                                                                    //sensor rechts
const byte speedSensorB = 3;                                                                    //sensor links
int counterA = 0;                                                                               //counter voor sensor rechts
int counterB = 0;                                                                               //counter voor sensor links
//servos
const byte gripper = 8;                                                                         //servo voor de gripper
const byte neck = 7;                                                                            //servo voor de neck met de ultrasonic
//line tracker
QTRSensors qtr;                                                                                 //variable met het tyoe QTRSensors
const byte SensorCount = 8;                                                                     //num sensors op line tracker
int sensorValues[SensorCount];                                                                  //sensorwaardes van de line tracker. wordt gelezen met een for loop of per sensor.

void setup() {
  Serial.begin(9600);
  pinMode(echoPin, INPUT);
  pinMode(triggerPin, OUTPUT);
  pinMode(forwardLeft, OUTPUT);
  pinMode(forwardRight, OUTPUT);
  pinMode(reverseLeft, OUTPUT);
  pinMode(reverseRight, OUTPUT);
  pinMode(speedSensorA, INPUT_PULLUP);
  pinMode(speedSensorB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(speedSensorA), countA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(speedSensorB), countB, CHANGE);
  pixels = new Adafruit_NeoPixel(numPixels, neopin, pixelFormat);                               //koppel een nieuw adafruit neopixels object aan de pointer "pixels"
  pixels->begin();                                                                              //zet de neopixels "aan"
  digitalWrite(neck, LOW);                                                                      //zet de servo van de neck standaard op LOW, dan schiet deze niet terug na het draaien
  qtr.setTypeAnalog();                                                                          //zet de meettype op analoog
  qtr.setSensorPins((const byte[]) {A0, A1, A2, A3, A4, A5, A6, A7}, SensorCount);               //zet alle analoge pinnen als input pinnen voor de line tracker
  calibrateSensors();
  moveForward(50);
  servo(gripper, 500);
  turnRight(35);
  playMusic(); //functie, noten en lied staat in music.h

}


void loop() {
  //   pixels->clear();                                                                            //zet alle neopixels op neutraal
  //
  moveForward(60);
  scanDistance();
  Serial.println(distance);
  //Serial.println("vooruit");
  distanceForward = distance;
  stopVehicle();
  servo(neck, 2400);
  scanDistance();
  distanceRight = distance;
  //  Serial.println(distanceRight);
  //  Serial.println("rechts");
  delay(300);
  servo(neck, 400);
  delay(300);
  scanDistance();
  distanceLeft = distance;
  //  Serial.println(distanceLeft);
  //  Serial.println("links");
  servo(neck, 1400);
  //Serial.println("keuze maken");


  if (distanceForward <= minDistance && distanceForward < distanceLeft && distanceRight < distanceLeft) {
    turnLeft(25);
  } else if (distanceForward <= minDistance && distanceForward < distanceRight && distanceRight > distanceLeft) {
    turnRight(25);
  } else if (distanceForward < 30 && distanceRight < 20 && distanceLeft < 30) {
    Turn180DLeft();
  }  else if (distanceForward < 30 && distanceRight < 30 && distanceLeft < 20) {
    Turn180DRight();
  }
}

/*
   moveForward en moveBackward hebben 2 extra lijnen code. Beide functies hebben eerst een soort van wake up (de 2 analogwrites met 255).
   Tijdens het maken van de calibratie bleef de robot niet stabiel, dit kwam omdat, vrij logisch, de robot eerst alle gewicht moet meetrekken.
   Toen de 'wake up" er niet inzat bleven de wielen vaak hangen en ging hij schreef.
*/
void moveForward(int pulsecount) {
  while (counterA <= pulsecount && counterB <= pulsecount) {
    analogWrite(forwardLeft, startWheels);
    analogWrite(forwardRight, startWheels);
    analogWrite(forwardLeft, wheelSpeedLeft);
    analogWrite(forwardRight, wheelSpeedRight);
    analogWrite(reverseLeft, stopWheels);
    analogWrite(reverseRight, stopWheels);
    pixels->setBrightness(85); //helderheid omlaag, deze stond eerst op 100%.
    pixels->setPixelColor(ll, pixels->Color(255, 0, 50));
    pixels->setPixelColor(lr, pixels->Color(0, 0, 150));
    pixels->setPixelColor(rr, pixels->Color(255, 0, 50));
    pixels->setPixelColor(rl, pixels->Color(0, 0, 150));
    //underglow neopixels
    pixels->setPixelColor(uglow1, pixels->Color(0, 255, 50));
    pixels->setPixelColor(uglow2, pixels->Color(0, 0, 250));
    pixels->setPixelColor(uglow3, pixels->Color(0, 255, 50));
    pixels->setPixelColor(uglow4, pixels->Color(0, 0, 250));
    pixels->setPixelColor(uglow5, pixels->Color(0, 255, 50));
    pixels->setPixelColor(uglow6, pixels->Color(0, 0, 250));
    pixels->setPixelColor(uglow7, pixels->Color(0, 255, 50));
    pixels->setPixelColor(uglow8, pixels->Color(0, 0, 250));
    pixels->show();

  }
  stopVehicle();
  counterA = 0;
  counterB = 0;
}


void moveBackward(int pulsecount) {
  while (counterA <= pulsecount && counterB <= pulsecount) {
    analogWrite(reverseLeft, 255);
    analogWrite(reverseRight, 255);
    analogWrite(forwardLeft, 0);
    analogWrite(forwardRight, 0);
    analogWrite(reverseLeft, 180);
    analogWrite(reverseRight, 175);
  }
  stopVehicle();
  counterA = 0;
  counterB = 0;
}

void Turn180DRight() {
  turnRight(20);
  moveBackward(10);
  turnRight(20);
}

void Turn180DLeft() {
  turnLeft(20);
  moveBackward(10);
  turnLeft(20);
}

void turnRight(int pulsecount) {
  while (counterA <= pulsecount) {
    analogWrite(forwardLeft, LOW);
    analogWrite(forwardRight, 180);
    analogWrite(reverseLeft, 170);
    analogWrite(reverseRight, LOW);
    pixels->setBrightness(85); //helderheid omlaag, deze stond eerst op 100%.
    pixels->setPixelColor(ll, pixels->Color(0, 0, 150));
    pixels->setPixelColor(lr, pixels->Color(255, 0, 50));
    pixels->setPixelColor(rr, pixels->Color(255, 0, 50));
    pixels->setPixelColor(rl, pixels->Color(0, 0, 150));
    //underglow neopixels
    pixels->setPixelColor(uglow1, pixels->Color(0, 255, 50));
    pixels->setPixelColor(uglow2, pixels->Color(0, 255, 50));
    pixels->setPixelColor(uglow3, pixels->Color(0, 255, 50));
    pixels->setPixelColor(uglow4, pixels->Color(0, 255, 50));
    pixels->setPixelColor(uglow5, pixels->Color(0, 0, 150));
    pixels->setPixelColor(uglow6, pixels->Color(0, 0, 150));
    pixels->setPixelColor(uglow7, pixels->Color(0, 0, 150));
    pixels->setPixelColor(uglow8, pixels->Color(0, 0, 150));
    pixels->show();
  }
  stopVehicle();
  /*
    Beide counters worden gereset ondanks dat er in de while maar 1 wordt aangeroepen, dit is omdat beide wielen draaien.
    Als counterB niet gereset wordt dan staan beide counters niet meer gelijk, dit kan nadelig effect hebben op alle nakomemde function calls.
    Zelfde voor turnLeft.
  */
  counterA = 0;
  counterB = 0;
}

void turnLeft(int pulsecount) {
  while (counterB <= pulsecount) {
    analogWrite(forwardLeft, 180);
    analogWrite(forwardRight, LOW);
    analogWrite(reverseLeft, LOW);
    analogWrite(reverseRight, 170);
    pixels->setBrightness(85); //helderheid omlaag, deze stond eerst op 100%.
    pixels->setPixelColor(ll, pixels->Color(255, 0, 50));
    pixels->setPixelColor(lr, pixels->Color(0, 0, 150));
    pixels->setPixelColor(rr, pixels->Color(0, 0, 150));
    pixels->setPixelColor(rl, pixels->Color(255, 0, 50));
    //underglow neopixels
    pixels->setPixelColor(uglow1, pixels->Color(0, 0, 150));
    pixels->setPixelColor(uglow2, pixels->Color(0, 0, 150));
    pixels->setPixelColor(uglow3, pixels->Color(0, 0, 150));
    pixels->setPixelColor(uglow4, pixels->Color(0, 0, 150));
    pixels->setPixelColor(uglow5, pixels->Color(0, 255, 50));
    pixels->setPixelColor(uglow6, pixels->Color(0, 255, 50));
    pixels->setPixelColor(uglow7, pixels->Color(0, 255, 50));
    pixels->setPixelColor(uglow8, pixels->Color(0, 255, 50));
    pixels->show();
  }

  stopVehicle();
  counterA = 0;
  counterB = 0;
}

void countA() {
  counterA++;
}

void countB() {
  counterB++;
}

void stopVehicle() {
  analogWrite(forwardLeft, LOW);
  analogWrite(forwardRight, LOW);
  analogWrite(reverseLeft, LOW);
  analogWrite(reverseRight, LOW);
  pixels->setBrightness(70);
  pixels->setPixelColor(ll, pixels->Color(255, 0, 50));
  pixels->setPixelColor(lr, pixels->Color(255, 0, 50));
  pixels->setPixelColor(rr, pixels->Color(255, 0, 50));
  pixels->setPixelColor(rl, pixels->Color(255, 0, 50));
  //underglow neopixels
  pixels->setPixelColor(uglow1, pixels->Color(0, 255, 50));
  pixels->setPixelColor(uglow2, pixels->Color(0, 255, 50));
  pixels->setPixelColor(uglow3, pixels->Color(0, 255, 50));
  pixels->setPixelColor(uglow4, pixels->Color(0, 255, 50));
  pixels->setPixelColor(uglow5, pixels->Color(0, 255, 50));
  pixels->setPixelColor(uglow6, pixels->Color(0, 255, 50));
  pixels->setPixelColor(uglow7, pixels->Color(0, 255, 50));
  pixels->setPixelColor(uglow8, pixels->Color(0, 255, 50));
  pixels->show();
}



void scanDistance() {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(10);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  //Serial.println(distance);
}

//credits to Bart Oerlemans
void servo(int PIN, int pulse) {
  for (int i = 0; i < 10; i++) {
    digitalWrite(PIN, HIGH);
    delayMicroseconds(pulse);
    digitalWrite(PIN, LOW);
    delay(20);
  }
}

void calibrateSensors() {
  for (int i = 1; i <= 10; i++) {
    qtr.calibrate();
    for (int j = 1; j <= 5; j++) {
      moveForward(3);
      counterA = 0;
      counterB = 0;
    }
    stopVehicle();
    for (int k = 1; k <= 5; k++) {
      moveBackward(3);
      counterA = 0;
      counterB = 0;
    }
  }
  stopVehicle();

  for (int i = 0; i < SensorCount; i++)
  {
    Serial.print(qtr.calibrationOn.minimum[i]);
    Serial.print(' ');
  }
  Serial.println();
  // print the calibration maximum values measured when emitters were on
  for (int i = 0; i < SensorCount; i++)
  {
    Serial.print(qtr.calibrationOn.maximum[i]);
    Serial.print(' ');
  }
}
