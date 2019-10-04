// Program to allow the robot to trace a white line

#include <Servo.h>

Servo left_motor;
Servo right_motor;
int right_pin = 6;
int left_pin = 5;

int LEFT_LINE_SENSOR = A5;
int RIGHT_LINE_SENSOR = A4;

// this integer is the boundary between "white" and "not white"
int threshold = 666;

int left_sensor_value;
int right_sensor_value;

void move() {
  left_sensor_value = analogRead(LEFT_LINE_SENSOR);
  right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
  if (left_sensor_value < threshold && right_sensor_value < threshold) {
    // both sensors detect white
    moveForward();
    halt();
  } else if (left_sensor_value < threshold && right_sensor_value >= threshold) {
    // left detects white, right detects black
    while (left_sensor_value < threshold) {
      slightLeft();
    }    
  } else if (left_sensor_value >= threshold && right_sensor_value < threshold) {
    // left detects black, right detects white
    while (right_sensor_value < threshold) {
      slightRight();
    }
  } else {
    // both sensors detect black
    moveForward();
  }
}

void slightRight() {
  left_motor.write(180);
  right_motor.write(85);
}

void slightLeft() {
  left_motor.write(95);
  right_motor.write(0);
}

void moveForward() {
  left_motor.write(180);
  right_motor.write(0);
}

void halt() {
  left_motor.write(90);
  right_motor.write(90);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LEFT_LINE_SENSOR, INPUT);
  pinMode(RIGHT_LINE_SENSOR, INPUT);
  Serial.begin(9600);
  right_motor.attach(right_pin);
  left_motor.attach(left_pin);
}

void loop() {
  // put your main code here, to run repeatedly:
  //move();
  moveForward();
  delay(1000);
  halt();
  delay(1000);
}










