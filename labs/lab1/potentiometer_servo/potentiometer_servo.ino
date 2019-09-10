// This program allows us to control the rate at which the Servo rotates by twisting the potentiometer

#include <Servo.h>

Servo SERVO;
int VOLT_DIVIDE = A5;
int v = 0;

void setup() {
  // put your setup code here, to run once:
  SERVO.attach(3);
  pinMode(VOLT_DIVIDE, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  v = analogRead(VOLT_DIVIDE);
  SERVO.write(v*2);
}
