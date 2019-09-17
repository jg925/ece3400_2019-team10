// Program to allow the robot to trace a white line

#include <Servo.h>

Servo left;
Servo right;
int right_pin = 11;
int left_pin = 10;

int LEFT_LINE_SENSOR = A5;
int RIGHT_LINE_SENSOR = A4;

// this integer is the boundary between "white" and "not white"
int threshold = 450;

int left_sensor_value = 0;
int right_sensor_value = 0;

void move(int left, int right) {
  if (left < threshold && right < threshold) {
    // both sensors detect white
    moveForward();
    
  } else if (left < threshold && right >= threshold) {
    // left detects white, right detects black
    while (left < threshold) {
      slightLeft();
      left = analogRead(LEFT_LINE_SENSOR);
    }    
  } else if (left >= threshold && right < threshold) {
    // left detects black, right detects white
    while (right < threshold) {
      slightRight();
      right = analogRead(RIGHT_LINE_SENSOR);
    }
  } else {
    // both sensors detect black
    moveForward();
  }
}

void slightRight() {
  left.write(180);
  right.write(85);
}

void slightLeft() {
  left.write(95);
  right.write(0);
}

void moveForward() {
  left.write(180);
  right.write(0);
}

void halt() {
  left.write(90);
  right.write(90);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LEFT_LINE_SENSOR, INPUT);
  pinMode(RIGHT_LINE_SENSOR, INPUT);
  Serial.begin(9600);
  right.attach(right_pin);
  left.attach(left_pin);
}

void loop() {
  // put your main code here, to run repeatedly:
  left_sensor_value = analogRead(LEFT_LINE_SENSOR);
  right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
  move(left_sensor_value, right_sensor_value);
}
