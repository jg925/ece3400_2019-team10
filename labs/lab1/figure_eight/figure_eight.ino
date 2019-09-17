// This program will eventually allow the robot to autonomously move in a figure eight

#include <Servo.h>

Servo right;
Servo left;

int right_pin = 11;
int left_pin = 10;

void leftTurn(int t) {
  left.write(0);
  right.write(60);
  delay(t);
}

void rightTurn(int t) {
  left.write(120);
  right.write(180);
  delay(t);
}

void moveForward(int t) {
  left.write(180);
  right.write(0);
  delay(t);
}

void rightSquare() {
  for (int i = 0; i < 4; i++) {
    moveForward(3000);
    rightTurn(670);
  }
}

void leftSquare() {
  for (int i = 0; i < 4; i++) {
    moveForward(3000);
    leftTurn(670);
  }
}

void figureEight() {
  for (int i = 0; i < 4; i++) {
    moveForward(3000);
    leftTurn(668);
  }

  for (int i = 0; i < 4; i++) {
    moveForward(3000);
    rightTurn(668);
  } 
}

void setup() {
  // put your setup code here, to run once:
  right.attach(right_pin);
  left.attach(left_pin);
}

void loop() {
  // put your main code here, to run repeatedly:
  figureEight();
}
