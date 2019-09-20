// Program to allow the robot to trace a white line

#include <Servo.h>

Servo left;
Servo right;
int right_pin = 6;
int left_pin = 5;

int LEFT_LINE_SENSOR = A5;
int RIGHT_LINE_SENSOR = A4;

int count = 0;

// this integer is the boundary between "white" and "not white"
int threshold = 650;

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

void rotateLeft() {
  left.write(60);
  right.write(60);
}

void rotateRight() {
  left.write(120);
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
  Serial.println("LEFT");
  Serial.println(left_sensor_value);
  Serial.println("RIGHT");
  Serial.println(right_sensor_value);
  //Serial.println("out");
  //if both sensors on white
  if (left_sensor_value < threshold && right_sensor_value < threshold) {
    Serial.println("turn");
    
    moveForward();
    delay(500);

    // both sensors detect white
    if (count == 8) {
    // zero out count
      count=0;
    } 
    if (count < 4) {
    // right turn rotate 
      Serial.println("RIGHT TURN");

      rotateRight();
      delay(700);

      /*
      while (right_sensor_value > threshold) {
        right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
        rotateRight();
      }
      while (left_sensor_value > threshold) {
        left_sensor_value = analogRead(LEFT_LINE_SENSOR);
        rotateRight();
      }*/
      count++;
    } else {
    // left turn rotate
      Serial.println("LEFT TURN");
      
      rotateLeft();
      delay(700);

      /*
      while (left_sensor_value > threshold) {
        left_sensor_value = analogRead(LEFT_LINE_SENSOR);
        rotateLeft();
      }
      while (right_sensor_value > threshold) {
        right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
        rotateLeft();
      } */
      count++;
    }
    Serial.println("COUNT");
    Serial.println(count);
  }
  
  //left on white, right on black
  else if (left_sensor_value < threshold && right_sensor_value >= threshold) {
    Serial.println("first");
    // left detects white, right detects black
    while (left_sensor_value < threshold) {
      slightLeft();
      left_sensor_value = analogRead(LEFT_LINE_SENSOR);
    }    
  }
  
  //right on white, left on black
  else if (left_sensor_value >= threshold && right_sensor_value < threshold) {
    Serial.println("second");
    // left detects black, right detects white
    while (right_sensor_value < threshold) {
      slightRight();
      right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
    }
  }
  
  else if (left_sensor_value >= threshold && right_sensor_value >= threshold) {
    // both sensors detect black
    Serial.println("forward");
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
  figureEight();
}
