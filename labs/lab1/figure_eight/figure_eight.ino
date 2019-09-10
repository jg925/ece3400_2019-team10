// This program will eventually allow the robot to autonomously move in a figure eight

#include <Servo.h>

Servo right;
Servo left;
int right_pin = 5;
int left_pin = 3;

void rightTurn() {
  left.write(180);
  right.write(120);
}

void leftTurn() {
  left.write(120);
  right.write(180)
}

void moveForward() {
  left.write(180);
  right.write(180);
}

void setup() {
  // put your setup code here, to run once:
  right.attach(right_pin);
  left.attach(left_pin);
}

void loop() {
  // put your main code here, to run repeatedly:
  moveForward();
  delay(1000);
  leftTurn();
  delay(500);
  moveForward();
  delay(1000);
  rightTurn();
  delay(500);
}
