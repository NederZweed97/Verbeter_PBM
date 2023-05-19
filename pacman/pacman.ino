//Sarah Vogelzang
//Vebetering van BattleBot
#include <Adafruit_NeoPixel.h>
#include <QTRSensors.h>
#include "music.h"
//defineer alle pinnen en globale variabelen
//Neopixels
int neopin =  4; 
int numPixels  = 12; 
int pixelFormat = NEO_RGB + NEO_KHZ800;
Adafruit_NeoPixel *pixels;
 byte ll = 0;
 byte lr = 1;
 byte rr = 2;
 byte rl = 3;
 byte uglow1 = 4;
 byte uglow2 = 5;
 byte uglow3 = 6;
 byte uglow4 = 7;
 byte uglow5 = 8;
 byte uglow6 = 9;
 byte uglow7 = 10;
 byte uglow8 = 11;
//ultrasonic
int echoPin = 13;
int triggerPin = 12; 
double duration; 
double distance;
double distanceLeft;
double distanceRight;
double distanceForward;
//wheels
int reverseRight = 9;
int forwardRight = 10;
int reverseLeft = 6;
int forwardLeft = 11;
//pulsesensor
int speedSensorA = 2; //rechts
int speedSensorB = 3; //links
int counterA = 0;
int counterB = 0;
//servos
int gripper = 8;
int neck = 7;
//line tracker
QTRSensors qtr;
const int SensorCount = 8;
int sensorValues[SensorCount];

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
  pixels = new Adafruit_NeoPixel(numPixels, neopin, pixelFormat);
  pixels->begin();
  digitalWrite(neck, LOW);
  qtr.setTypeAnalog();
  //zet alle analoge pinnen als input pinnen voor de line tracker
  qtr.setSensorPins((const byte[]){A0, A1, A2, A3, A4, A5, A6, A7}, SensorCount);
  //calibrateSensors();
//  moveForward(60);  
//  servo(gripper, 500);
//  turnRight(25);
//  playMusic(); //functie, noten en lied staat in music.h
}


void loop() {
   pixels->clear(); // Set all pixel colors to 'off'

  moveForward(40);  
  scanDistance();
  Serial.println(distance);
  distanceForward = distance;
  stopVehicle();
  servo(neck, 2400);
  scanDistance();
  distanceRight = distance;
  Serial.println(distanceRight);
  delay(200);
  servo(neck, 400);
  delay(200);
  scanDistance();
  distanceLeft = distance;
  Serial.println(distanceLeft);
  servo(neck, 1400);
  Serial.println("keuze maken");
  
    if(distanceForward < 30){
      if(distanceRight < distanceLeft){
      Serial.println("Links");
      turnLeft(25);
      }else if(distanceRight > distanceLeft){
        Serial.println("rechts");
        turnRight(25);
      } 
    }
}




void moveForward(int pulsecount) {
  while(counterA <= pulsecount && counterB <= pulsecount){
  analogWrite(forwardLeft, 180);
  analogWrite(forwardRight, 182);
  analogWrite(reverseLeft, LOW);
  analogWrite(reverseRight, LOW);
    pixels->setBrightness(85); //helderheid omlaag, deze stond eerst op 100%. 
    pixels->setPixelColor(ll, pixels->Color(255,0,50));
    pixels->setPixelColor(lr, pixels->Color(0,0,150));
    pixels->setPixelColor(rr, pixels->Color(255,0,50));
    pixels->setPixelColor(rl, pixels->Color(0,0,150));
    //underglow neopixels    
    pixels->setPixelColor(uglow1, pixels->Color(0,255,50));
    pixels->setPixelColor(uglow2, pixels->Color(0,0,250));
    pixels->setPixelColor(uglow3, pixels->Color(0,255,50));
    pixels->setPixelColor(uglow4, pixels->Color(0,0,250));
    pixels->setPixelColor(uglow5, pixels->Color(0,255,50));
    pixels->setPixelColor(uglow6, pixels->Color(0,0,250));
    pixels->setPixelColor(uglow7, pixels->Color(0,255,50));
    pixels->setPixelColor(uglow8, pixels->Color(0,0,250));
  pixels->show();
  }
  stopVehicle();
  counterA = 0;
  counterB = 0;
}

   
void moveBackward(int pulsecount) {
  while(counterA <= pulsecount && counterB <= pulsecount){
    analogWrite(forwardLeft, LOW);
    analogWrite(forwardRight, LOW);
    analogWrite(reverseLeft, 210);
    analogWrite(reverseRight, 188);
  } 
    stopVehicle();
    counterA = 0;
    counterB = 0;
}

void turnRight(int pulsecount) {
  while(counterA <= pulsecount){
    analogWrite(forwardLeft, LOW);
    analogWrite(forwardRight, 180);
    analogWrite(reverseLeft, 170);
    analogWrite(reverseRight, LOW);
  } 
    stopVehicle();
     /*
     * Beide counters worden gereset ondanks dat er in de while maar 1 wordt aangeroepen, dit is omdat beide wielen draaien.
     * Als counterB niet gereset wordt dan staan beide counter niet meer gelijk, dit kan nadelig effect hebben op bijvoorbeld moveForward. 
     * Zelfde voor turnLeft.
     */
    counterA = 0;
    counterB = 0;  
} 

void turnLeft(int pulsecount) {
  while(counterB <= pulsecount){
    analogWrite(forwardLeft, 180);
    analogWrite(forwardRight, LOW);
    analogWrite(reverseLeft, LOW);
    analogWrite(reverseRight, 170);
  } 
    stopVehicle();
    counterA = 0;
    counterB = 0; 
} 
 
void countA(){
  counterA++;
}

void countB(){
  counterB++;
}
 
void stopVehicle(){
  analogWrite(forwardLeft, LOW);
  analogWrite(forwardRight, LOW); 
  analogWrite(reverseLeft, LOW);   
  analogWrite(reverseRight, LOW);
    pixels->setBrightness(70);
    pixels->setPixelColor(ll, pixels->Color(255,0,0));
    pixels->setPixelColor(lr, pixels->Color(255,0,0));
    pixels->setPixelColor(rr, pixels->Color(255,0,0));
    pixels->setPixelColor(rl, pixels->Color(255,0,0));
    //underglow neopixels
    pixels->setPixelColor(uglow1, pixels->Color(0,255,0));
    pixels->setPixelColor(uglow2, pixels->Color(0,255,0));
    pixels->setPixelColor(uglow3, pixels->Color(0,255,0));
    pixels->setPixelColor(uglow4, pixels->Color(0,255,0));
    pixels->setPixelColor(uglow5, pixels->Color(0,255,0));
    pixels->setPixelColor(uglow6, pixels->Color(0,255,0));
    pixels->setPixelColor(uglow7, pixels->Color(0,255,0));
    pixels->setPixelColor(uglow8, pixels->Color(0,255,0));
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
void servo(int PIN, int pulse){
  for(int i=0; i < 10; i++){
    digitalWrite(PIN, HIGH);
    delayMicroseconds(pulse);
    digitalWrite(PIN, LOW);
    delay(20);
  }  
}

void calibrateSensors(){
  for(int i = 1; i <= 20; i++){
    qtr.calibrate();
    for(int j = 1; j <= 5; j++){
      moveForward(1);
      counterA = 0;
      counterB = 0;
    }
    stopVehicle();
    for(int k = 1; k <= 5; k++){
      moveBackward(2);
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
