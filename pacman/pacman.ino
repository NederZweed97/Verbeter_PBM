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
byte stripA[] = {4,6,8,10}; //roze undeglow
byte stripB[] = {5,7,9,11}; //blauw underglow
byte stripC[] = {0,2}; //roze top neopixels
byte stripD[] = {1,3}; //blauw top neopixels

//ultrasonic
int echoPin = 13;
int triggerPin = 12; 
double duration; 
double distance;
double distanceLeft;
double distanceRignt;
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
const uint8_t SensorCount = 6;
uint16_t sensorValues[SensorCount];
const int sensorMax0 = 800;
const int sensorMax6 = 800;


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
  qtr.setSensorPins((const uint8_t[]){A0, A1, A2, A3, A4, A5}, SensorCount);

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
   pixels->clear(); // Set all pixel colors to 'off'

  
  qtr.read(sensorValues);
//waardes van de line tracker, voor dat de game begint deze uitcommenten en de waardes lezen. 
 //Serial.println(sensorValues[0]);
 //Serial.println(sensorValues[5]);
 

  
moveForward(15);
 qtr.read(sensorValues);
scanDistance();
delay(500);
//Serial.println(distance);



  distanceForward = distance;
  stopVehicle();
  servo(neck, 2400);
  scanDistance();
  distanceRignt = distance;
  //Serial.println(distanceRight);
  delay(800);
  servo(neck, 400);
  delay(800);
  scanDistance();
  distanceLeft = distance;
 // Serial.println(distanceLeft);
  servo(neck, 1400);
  //Serial.println("keuze maken");
  
    if(distanceForward <= 15){
      if(distanceRignt < distanceLeft){
      //Serial.println("Links");
      turnLeft(25);
    }else if(distanceRignt > distanceLeft){
      //Serial.println("rechts");
      turnRight(25);
    }
    }

 
 qtr.read(sensorValues);

//if(sensorValues[0] >= sensorMax0 && sensorValues[5] >= sensorMax6){
//  stopVehicle();
//  servo(gripper, 1500);
//  moveBackward(20);
//  delay(1000);
//  exit(0);
//  
//}

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

  

void moveForward(int pulsecount) {
  while(counterA <= pulsecount){
  analogWrite(forwardLeft, 200);
  analogWrite(forwardRight, 210);
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
  counterA = 0;
}

   
void moveBackward(int pulsecount) {
  while(counterA <= pulsecount){
    analogWrite(forwardLeft, LOW);
    analogWrite(forwardRight, LOW);
    analogWrite(reverseLeft, 200);
    analogWrite(reverseRight, 210);
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
