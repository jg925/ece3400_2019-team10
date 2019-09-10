// We used this program to experiment with how the Servo motor rotates to figure out how it functions

#include <Servo.h>

Servo SERVO;

void setup() {
  // put your setup code here, to run once:
  SERVO.attach(3);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int pos = 180; pos >=0; pos -=1) { // to sweep the voltage, found on Arduino Servo Library
    SERVO.write(pos);
    delay(40);
  }
}
