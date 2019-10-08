// Program to allow the robot to nagivate walls

#include <Servo.h>

Servo left;
Servo right;

// servo pins
int right_pin = 6;
int left_pin = 5;

// line sensor pins
int LEFT_LINE_SENSOR = A5;
int RIGHT_LINE_SENSOR = A4;

// IR sensor pins
#define right_ir_sensor 2
#define left_ir_sensor A2
#define front_ir_sensor A1

// boundary between "white" and "black"
int threshold = 650;

// initialization of line sensor values
int left_sensor_value = 0;
int right_sensor_value = 0;

// initialization of IR sensor values
int right_detect;
int left_detect;
int front_detect;

void halt() {
  left.write(90);
  right.write(90);
}

void moveForward() {
  left.write(180);
  right.write(0);
}

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
  right.write(120);
}

void rotateLeft() {
  left.write(60);
  right.write(60);
}

void right90Turn() {
  rotateRight();
  delay(725);
}

void left90Turn() {
  rotateLeft();
  delay(725);
}

void right180Turn() {
  rotateRight();
  delay(1450);
}

void navigate() {
  left_sensor_value = analogRead(LEFT_LINE_SENSOR);
  right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
  Serial.println("LEFTSENSOR");
  Serial.println(left_sensor_value);
  Serial.println("RIGHTSENSOR");
  Serial.println(right_sensor_value);

  // if both sensors on white
  if (left_sensor_value < threshold && right_sensor_value < threshold) {
    
    // to move into corresponding turn cases after debug
    //halt();
    //delay(1000);
    Serial.println("check");
    moveForward();
    delay(500);

    // left IR sensor read + distance calculation
    float lvolts = analogRead(left_ir_sensor)*0.0048828125;  // value from sensor * (5/1024)
    int ldistance = 13*pow(lvolts, -1); // worked out from datasheet graph
    //delay(1000); // slow down serial port

    Serial.println("DISTANCE");
    Serial.println(ldistance);
  
    if (ldistance <= 10) {
      left_detect = 0;
    } else {
      left_detect = 1;
    }

    // front IR sensor read + distance calculation
    float fvolts = analogRead(front_ir_sensor)*0.0048828125;  // value from sensor * (5/1024)
    int fdistance = 13*pow(fvolts, -1); // worked out from datasheet graph
    //delay(1000); // slow down serial port
  
    if (fdistance <= 20) {
      front_detect = 0;
    } else {
      front_detect = 1;
    }

    // right IR sensor read + distance calculation
    right_detect = digitalRead(right_ir_sensor);  // 0 when detecting
  
    // left IR sensor read + distance calculation
    //left_detect = digitalRead(right_ir_sensor);   // 0 when detecting
  
    // front IR sensor read + distance calculation
    //front_detect = digitalRead(front_ir_sensor);  // 0 when detecting

    Serial.println("RIGHT");
    Serial.println(right_detect);
    Serial.println("LEFT");
    Serial.println(left_detect);
    Serial.println("FRONT");
    Serial.println(fdistance);
    Serial.println(front_detect);

    // case 0: no walls --> turn right
    if (right_detect && left_detect && front_detect) {
      Serial.println("NO WALLS");
      right90Turn();
    }
    
    // case 1: right wall, left wall, front wall --> turn around
    else if (!right_detect && !left_detect && !front_detect) {
      Serial.println("180 TURN");
      right180Turn();
    }
    
    // case 2: left wall, front wall --> turn right
    else if (right_detect && !left_detect && !front_detect) {
      Serial.println("RIGHT TURN");
      right90Turn();
    }

    // case 3: right wall, front wall --> turn left
    else if (!right_detect && left_detect && !front_detect) {
      Serial.println("LEFT TURN");
      left90Turn();
    }

    // case 4: front wall only --> turn right
    else if (right_detect && left_detect && !front_detect) {
      right90Turn();
    }

    // case 5: left wall only --> turn right
    else if (right_detect && !left_detect && front_detect) {
      right90Turn();
    }
    
    // case 6: otherwise --> go straight
    else {
      Serial.println("STRIGHT");
      moveForward();
    }
  }
  
  // left on white, right on black --> slight left adjust
  else if (left_sensor_value < threshold && right_sensor_value >= threshold) {
    // left detects white, right detects black
    while (left_sensor_value < threshold) {
      slightLeft();
      left_sensor_value = analogRead(LEFT_LINE_SENSOR);
    }    
  }
  
  // right on white, left on black --> slight right adjust
  else if (left_sensor_value >= threshold && right_sensor_value < threshold) {
    // left detects black, right detects white
    while (right_sensor_value < threshold) {
      slightRight();
      right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
    }
  }
  
  // both sensors detect black --> go straight
  else if (left_sensor_value >= threshold && right_sensor_value >= threshold) {
    Serial.println("FORWARD");
    moveForward();
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LEFT_LINE_SENSOR, INPUT);
  pinMode(RIGHT_LINE_SENSOR, INPUT);
  pinMode(left_ir_sensor, INPUT);
  pinMode(front_ir_sensor, INPUT);
  Serial.begin(9600);
  right.attach(right_pin);
  left.attach(left_pin);
}

void loop() {
  // put your main code here, to run repeatedly:
  //halt();
  
  navigate();
}

