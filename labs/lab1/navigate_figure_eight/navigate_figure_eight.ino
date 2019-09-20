// Program to allow the robot to trace a white line

#include <Servo.h>

Servo left;
Servo right;
int right_pin = 10;
int left_pin = 11;

int LEFT_LINE_SENSOR = A5;
int RIGHT_LINE_SENSOR = A4;

int count = 0;

// this integer is the boundary between "white" and "not white"
int threshold = 666;

int left_sensor_value = 0;
int right_sensor_value = 0;

void slightRight() {
  left.write(180);
  right.write(85);
}

void slightLeft() {
  left.write(95);
  right.write(0);
}

void rotateRight() {
  left.write(120);
  right.write(60);
}

void rotateLeft() {
  left.write(60);
  right.write(120);
}

void moveForward() {
  left.write(180);
  right.write(0);
}

void halt() {
  left.write(90);
  right.write(90);
}

void stopOnWhite() {
  if (analogRead(LEFT_LINE_SENSOR) < threshold && analogRead(RIGHT_LINE_SENSOR) < threshold) {
    halt();
  } else {
    moveForward();
  }
}


void rightTurn() {
}

void leftTurn(){
}

void figureEight() {
  left_sensor_value = analogRead(LEFT_LINE_SENSOR);
  right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
  //if both sensors on white
  if (left_sensor_value < threshold && right_sensor_value < threshold) {
    moveForward();
    delay(100);
    Serial.println("in");
    // both sensors detect white
    if (count == 8) {
    // zero out count
      count=0;
    } 
    if (count < 4) {
    // right turn rotate 
      while (right_sensor_value > threshold) {
        right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
        rotateRight();
      }
      while (left_sensor_value > threshold) {
        left_sensor_value = analogRead(LEFT_LINE_SENSOR);
        rotateRight();
      }
      count++;
    } else {
    // left turn rotate
      while (left_sensor_value > threshold) {
        left_sensor_value = analogRead(LEFT_LINE_SENSOR);
        rotateLeft();
      }
      while (right_sensor_value > threshold) {
        right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
        rotateLeft();
      } 
      count++;
    }
    Serial.println(count);
  }
  //left on white, right on black
  else if (left_sensor_value < threshold && right_sensor_value >= threshold) {
    // left detects white, right detects black
    while (left_sensor_value < threshold) {
      slightLeft();
      left_sensor_value = analogRead(LEFT_LINE_SENSOR);
    }    
  } 
  //right on white, left on black
  else if (left_sensor_value >= threshold && right_sensor_value < threshold) {
    // left detects black, right detects white
    while (right_sensor_value < threshold) {
      slightRight();
      right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
    }
  } else {
    // both sensors detect black
    moveForward();
  }
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
  //left_sensor_value = analogRead(LEFT_LINE_SENSOR);
  //right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
  //rightTurn();
  //moveForward();
  //stopOnWhite();
  //figureEight();
}
