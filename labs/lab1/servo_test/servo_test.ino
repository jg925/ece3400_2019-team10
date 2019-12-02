// We used this program to experiment with how the Servo motor rotates to figure out how it functions

//#include <Servo.h>
//
//Servo SERVO;
//
//void setup() {
//  // put your setup code here, to run once:
//  SERVO.attach(3);
//}
//
//void loop() {
//  // put your main code here, to run repeatedly:
//  for (int pos = 180; pos >=0; pos -=1) { // to sweep the voltage, found on Arduino Servo Library
//    SERVO.write(pos);
//    delay(40);
//  }
//}

// fast Servo test

#include <Servo.h>

Servo right;
int right_pin = 11;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  right.attach(right_pin);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i=0; i<=180; i++) {
    Serial.println(i);
    right.write(i);
    delay(500);
  }
  /*
  Serial.println(0);
  right.write(0);
  delay(5000);
  Serial.println(30);
  right.write(30);
  delay(5000);
  Serial.println(60);
  right.write(60);
  delay(5000);
  Serial.println(90);
  right.write(90);
  delay(5000);
  Serial.println(120);
  right.write(120);
  delay(5000);
  Serial.println(180);
  right.write(180);
  delay(5000);*/
}
