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
int speedSensorA = 2; 
int speedSensorB = 3; 
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
  calibrateSensors();
//  moveForward(60);  
//  servo(gripper, 500);
//  turnRight(25);
//  playMusic(); //functie, noten en lied staat in music.h
}

void loop() {
//   pixels->clear(); // Set all pixel colors to 'off'
//  qtr.read(sensorValues);
////waardes van de line tracker, voor dat de game begint deze uitcommenten en de waardes lezen. 
// //Serial.println(sensorValues[0]);
// //Serial.println(sensorValues[7]); 
//moveForward(15);
// qtr.read(sensorValues);
//scanDistance();
//delay(500);
////Serial.println(distance);
//  distanceForward = distance;
//  stopVehicle();
//  servo(neck, 2400);
//  scanDistance();
//  distanceRight = distance;
//  Serial.println(distanceRight);
//  delay(800);
//  servo(neck, 400);
//  delay(800);
//  scanDistance();
//  distanceLeft = distance;
//  Serial.println(distanceLeft);
//  servo(neck, 1400);
//  Serial.println("keuze maken");
//  
//    if(distanceForward < 30){
//      if(distanceRight < distanceLeft){
//      //Serial.println("Links");
//      turnLeft(25);
//    }else if(distanceRight > distanceLeft){
//      //Serial.println("rechts");
//      turnRight(25);
//    }
//qtr.read(sensorValues);
//if(sensorValues[0] >= sensorMax0 && sensorValues[7] >= sensorMax6){
//  stopVehicle();
//  servo(gripper, 1500);
//  moveBackward(20);
//  delay(1000);
//  exit(0);
//}

  int position = qtr.readLineBlack(sensorValues);
  moveForward(40);
  distanceForward = distance;
  stopVehicle();
  servo(neck, 2400);
  scanDistance();
  distanceRight = distance;
  //Serial.println(distanceRight);
  delayMicroseconds(80);
  servo(neck, 400);
  delayMicroseconds(80);
  scanDistance();
  distanceLeft = distance;
  //Serial.println(distanceLeft);
  servo(neck, 1400);
  //Serial.println("keuze maken");

  if(distanceForward <= 20){
    if(distanceRight > distanceLeft){
      turnRight(10);
    } else{
      turnLeft(10);
    }
  }

  for(int i = 0; i < SensorCount; i++){
    if(SensorValues[i] == qtr.calibrationOn.maximum[i]){
      StopVehicle();
      exit(0);
    }
  }
}



void moveForward(int pulsecount) {
  while(counterA <= pulsecount){
  analogWrite(forwardLeft, 180);
  analogWrite(forwardRight, 182);
  analogWrite(reverseLeft, LOW);
  analogWrite(reverseRight, LOW);
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
}

   
void moveBackward(int pulsecount) {
  while(counterB <= pulsecount){
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
    counterA = 0;  
} 

void turnLeft(int pulsecount) {
  while(counterB <= pulsecount){
    analogWrite(forwardLeft, 180);
    analogWrite(forwardRight, LOW);
    analogWrite(reverseLeft, LOW);
    analogWrite(reverseRight, 170);
  } 
    stopVehicle();
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
  counterA = 0;
  counterB = 0;
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
