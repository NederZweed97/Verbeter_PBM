#include <Adafruit_NeoPixel.h>
#include <QTRSensors.h>
#include "music.h"
//defineer alle pinnen en globale variabelen
//Neopixels
int neopin =  4; 
int numPixels  = 12; 
int pixelFormat = NEO_RGB + NEO_KHZ800;
Adafruit_NeoPixel *pixels;

//StripA is voor blauw en stripB is voor de roze bij de underglow
//stripC en D zijn voor de neopixels op de robot. Zijn gescheiden omdat de strip voor de underglow GBR is en de top RGB. Je krijgt anders andere kleuren
int stripA[] = {4,6,8,10}; //roze undeglow
int stripB[] = {5,7,9,11}; //blauw underglow
int stripC[] = {0,2}; //roze top neopixels
int stripD[] = {1,3}; //blauw top neopixels

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
  qtr.setSensorPins((const byte[]){A0, A1, A2, A3, A4, A5, A6, A7}, SensorCount);


  calibrateSensors();

 moveToCone(60);  
 servo(gripper, 500);
 turnRight(25);  

  //credits: https://github.com/robsoncouto/arduino-songs/blob/master/pacman/pacman.ino
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
    divider = melody[thisNote + 1];
    if (divider > 0) {
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; 
    }
    tone(buzzer, melody[thisNote], noteDuration * 0.9);
    delay(noteDuration);
    noTone(buzzer);
  }
}


void loop() {
//   pixels->clear(); // Set all pixel colors to 'off'
//
//  
//  qtr.read(sensorValues);
////waardes van de line tracker, voor dat de game begint deze uitcommenten en de waardes lezen. 
// //Serial.println(sensorValues[0]);
// //Serial.println(sensorValues[7]);
// 
//
//  
//moveForward(15);
// qtr.read(sensorValues);
//scanDistance();
//delay(500);
////Serial.println(distance);
//
//
//
//  distanceForward = distance;
//  stopVehicle();
//  servo(neck, 2400);
//  scanDistance();
//  distanceRight = distance;
  //Serial.println(distanceRight);
  delay(800);
  servo(neck, 400);
  delay(800);
  scanDistance();
  distanceLeft = distance;
  Serial.println(distanceLeft);
  servo(neck, 1400);
  Serial.println("keuze maken");
  
    if(distanceForward <= 15){
      if(distanceRight < distanceLeft){
      //Serial.println("Links");
      turnLeft(25);
    }else if(distanceRight > distanceLeft){
      //Serial.println("rechts");
      turnRight(25);
    }
    }

qtr.read(sensorValues);
if(sensorValues[0] >= sensorMax0 && sensorValues[7] >= sensorMax6){
  stopVehicle();
  servo(gripper, 1500);
  moveBackward(20);
  delay(1000);
  exit(0);
 
}

}

bool moveToCone(int pulsecount){
    while(counterA <= pulsecount){
    analogWrite(forwardLeft, 170);
    analogWrite(forwardRight, 180);
    analogWrite(reverseLeft, LOW);
    analogWrite(reverseRight, LOW);
  } 
    stopVehicle();
    counterA = 0;
}

  
void moveForward(int pulsecount, int snelheid) {
  while(counterB <= pulsecount){
  analogWrite(forwardLeft, snelheid);
  analogWrite(forwardRight, snelheid);
  analogWrite(reverseLeft, LOW);
  analogWrite(reverseRight, LOW);

  for(int i = 0; i < sizeof(stripA); i++){
     pixels->setPixelColor(stripA[i], pixels->Color(0,255,50));
  }

  for(int j = 0; j < sizeof(stripB); j++){
    pixels->setPixelColor(stripB[j], pixels->Color(0,0,150));
  }

  for(int k = 0; k < sizeof(stripC); k++){
    pixels->setPixelColor(stripC[k], pixels->Color(255,0,50));
  }

  for(int h = 0; h < sizeof(stripD); h++){
    pixels->setPixelColor(stripD[h], pixels->Color(0,0,150));    
  }
  pixels->show();
  }
  stopVehicle();
  counterB = 0;
}

   
void moveBackward(int pulsecount, int snelheid) {
  while(counterB <= pulsecount){
    analogWrite(forwardLeft, LOW);
    analogWrite(forwardRight, LOW);
    analogWrite(reverseLeft, snelheid);
    analogWrite(reverseRight, snelheid);
  } 
    stopVehicle();
    counterA = 0;
    counterB = 0;

}

void turnRight(int pulsecount) {
  while(counterB <= pulsecount){
    analogWrite(forwardLeft, LOW);
    analogWrite(forwardRight, 180);
    analogWrite(reverseLeft, 170);
    analogWrite(reverseRight, LOW);
  } 
    stopVehicle();
    counterB = 0;  
} 

void turnLeft(int pulsecount) {
  while(counterA <= pulsecount){
    analogWrite(forwardLeft, 180);
    analogWrite(forwardRight, LOW);
    analogWrite(reverseLeft, LOW);
    analogWrite(reverseRight, 170);
  } 
    stopVehicle();
    counterA = 0; 
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

  for(int i = 0; i < sizeof(stripA); i++){
    pixels->setPixelColor(stripA[i], pixels->Color(0,255,0));
  }

  for(int j = 0; j < sizeof(stripB); j++){
    pixels->setPixelColor(stripB[j], pixels->Color(0,255,0));
  }

  for(int k = 0; k < sizeof(stripC); k++){
    pixels->setPixelColor(stripC[k], pixels->Color(255,0,0));
  }

  for(int h = 0; h < sizeof(stripD); h++){
    pixels->setPixelColor(stripD[h], pixels->Color(255,0,0));    
  }
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
  for(int i = 1; i <= 10; i++){
  qtr.calibrate();
  for(int j = 1; j <= 10; j++){
    moveForward(2, 190);
  }
  stopVehicle();

  for(int k = 1; k <= 10; k++){
    moveBackward(2, 190);
  }
  stopVehicle();
  for(int z = 1; z <= 10; z++){
    moveForward(2, 190);
  }
  stopVehicle();
  for(int k = 1; k <= 10; k++){
    moveBackward(2, 190);
  }
  stopVehicle();

}
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
