//Sarah Vogelzang.
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
const double minDistance = 12;                                                                  //minimale aftsand wat de robot moet houden
//wheels
const byte reverseRight = 9;
const byte forwardRight = 10;
const byte reverseLeft = 6;
const byte forwardLeft = 11;
const int startWheels = 255;                                                                    //de 'wake up' voor de motoren
const int wheelSpeedRight = 180;                                                                //gewenste snelheid rechter wiel
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
  servo(gripper, 400);
  //playMusic(); //functie, noten en lied staat in music.h
  moveForwardwithoutusonic(75);
  turnRight(30);
}

void loop() {
  readGhost();
  moveForward(40);
  readGhost();
  lookAround();
  readGhost();

  if (distanceForward <= minDistance && distanceForward < distanceLeft && distanceRight < distanceLeft) {
    turnLeft(20);
  } else if (distanceForward <= minDistance && distanceForward < distanceRight && distanceRight > distanceLeft) {
    turnRight(20);
  } else if (distanceForward < 18 && distanceRight < 10 && distanceLeft < 15) {
    Turn180DLeft();
  }  else if (distanceForward < 18 && distanceRight < 10 && distanceLeft < 15) {
    Turn180DRight();
 }

  readGhost();
}

void moveForward(int pulsecount) {
  while (counterA <= pulsecount && counterB <= pulsecount) {
   readGhost();
   scanDistance();
    if(distance < 7){
      stopVehicle();
      moveBackward(10);
      lookAround();
        if(distanceRight > distanceLeft){
          turnRight(20);
        } else{
          turnLeft(20);
        }
      } else{
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
  }
  stopVehicle();
  counterA = 0;
  counterB = 0;
 
}

void moveForwardwithoutusonic(int pulsecount) {
  while (/*counterA <= pulsecount &&*/ counterB <= pulsecount) {
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
  //counterA = 0;
  counterB = 0;
}


void lookAround(){
  stopVehicle();
   scanDistance();
  //Serial.println(distance);
  //Serial.println("vooruit");
  distanceForward = distance;
  Serial.println(distanceForward);
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
}


void moveBackward(int pulsecount) {
  while (counterA <= pulsecount && counterB <= pulsecount) {
    analogWrite(reverseLeft, startWheels);
    analogWrite(reverseRight, startWheels);
    analogWrite(forwardLeft, stopWheels);
    analogWrite(forwardRight, stopWheels);
    analogWrite(reverseLeft, wheelSpeedLeft);
    analogWrite(reverseRight, wheelSpeedRight);
  }
  stopVehicle();
  counterA = 0;
  counterB = 0;
}

void Turn180DRight() {
  turnRight(20);
  moveBackward(10);
  turnRight(25);
}

void Turn180DLeft() {
  turnLeft(20);
  moveBackward(10);
  turnLeft(25);
}

void turnRight(int pulsecount) {
  while (counterA <= pulsecount) {
    analogWrite(forwardLeft, stopWheels);
    analogWrite(forwardRight, wheelSpeedRight);
    analogWrite(reverseLeft, wheelSpeedLeft);
    analogWrite(reverseRight, stopWheels);
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
    analogWrite(forwardLeft, wheelSpeedLeft);
    analogWrite(forwardRight, stopWheels);
    analogWrite(reverseLeft, stopWheels);
    analogWrite(reverseRight, wheelSpeedRight);
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
  analogWrite(forwardLeft, stopWheels);
  analogWrite(forwardRight, stopWheels);
  analogWrite(reverseLeft, stopWheels);
  analogWrite(reverseRight, stopWheels);
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
  counterA = 0;
  counterB = 0;
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

void readGhost(){
  if(sensorValues[2] > 170 && sensorValues[2] < 175 && sensorValues[5] > 170 && sensorValues[5] < 175){ //blauwe spook
    stopVehicle();
    turnLeft(20);
    moveForwardwithoutusonic(80);
  }
  if(sensorValues[2] > 280 && sensorValues[2] < 285 && sensorValues[5] > 280 && sensorValues[5] < 285 ){ //donker roze spook
    Turn180DRight();
  }

  if(sensorValues[2] > 308 && sensorValues[2] < 312 && sensorValues[5] > 308 && sensorValues[5] < 312){ //lila spook 311 (312) en 309 (308)
    stopVehicle();
  }

  if(sensorValues[2] > 321 && sensorValues[2] < 328 && sensorValues[5] > 321 && sensorValues[5] < 328){ // donker paars spook 322 327
    stopVehicle();
    moveBackward(20);
    turnRight(25);
  }
}
