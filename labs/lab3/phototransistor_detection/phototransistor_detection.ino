#include <Servo.h>

Servo left;
Servo right;

// ============================================================================================

// Variables

// ============================================================================================

// servo pins
int right_pin = 6;
int left_pin = 5;

// line sensor pins
int LEFT_LINE_SENSOR = A5;
int RIGHT_LINE_SENSOR = A4;

// initialization of line sensor values
int left_sensor_value = 0;
int right_sensor_value = 0;

// IR sensor pins
int right_ir_sensor = 2;
int left_ir_sensor = 3;
int front_ir_sensor = 4;

// boundary between "white" and "black"
int line_threshold = 650;

// initialization of IR sensor values
int right_detect;
int left_detect;
int front_detect;

// phototransistor pins
int left_robot_detect = A3;

// detected robot LED
int robot_LED_pin = 12;

// robot detection vars
int pi_arr[10];
int sum = 0;
int count = 0;
int threshold = 100;
int count_threshold = 5;
int sample_size = 10;
int robot_done = 0;
int avg = 0;

// start
int START_BUTTON = 8;

// initialize "beginning" which waits for button press or 950 Hz signal and "ending" which goes HIGH when we finished traversing the maze
int beginning = 1;
int ending = 0;

// ============================================================================================

// Helper functions

// ============================================================================================

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

  robot_done = 1;
}

// ============================================================================================

// Line following code

// ============================================================================================

void navigate() {
  left_sensor_value = analogRead(LEFT_LINE_SENSOR);
  right_sensor_value = analogRead(RIGHT_LINE_SENSOR);

  Serial.println("navigate");

  if (left_sensor_value < line_threshold && right_sensor_value < line_threshold) {
    // left on white, right on white --> check for walls

    moveForward();
    delay(500);

    right_detect = digitalRead(right_ir_sensor);  // 0 when detecting
    left_detect = digitalRead(left_ir_sensor);   // 0 when detecting
    front_detect = digitalRead(front_ir_sensor);   // 0 when detecting

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
    
  } else if (left_sensor_value < line_threshold && right_sensor_value >= line_threshold) {
    // left on white, right on black --> slight left adjust
    while (left_sensor_value < line_threshold && right_sensor_value >= line_threshold) {
      slightLeft();
      left_sensor_value = analogRead(LEFT_LINE_SENSOR);
      right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
    }
    
  } else if (left_sensor_value >= line_threshold && right_sensor_value < line_threshold) {
    // right on white, left on black --> slight right adjust
    while (right_sensor_value < line_threshold && left_sensor_value >= line_threshold ) {
      slightRight();
      left_sensor_value = analogRead(LEFT_LINE_SENSOR);
      right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
    }
    
  //} else if (left_sensor_value >= line_threshold && right_sensor_value >= line_threshold) {
  } else {
    Serial.println("FORWARD ELSE");
    // both sensors detect black --> go straight
    moveForward();
  }
  
}

// ============================================================================================

// Robot detection code take 1: running count

// ============================================================================================

void robot_detect_take1() {
  
  int photo_input = analogRead(left_robot_detect);

  pi_arr[sample_size-1] = photo_input;

  Serial.println("ARRAY START");
  count = 0;
  for (int i= 0; i < sample_size; i++) {
    Serial.println(pi_arr[i]);
    if (pi_arr[i] > threshold) {
      count++;
    }
  }
  Serial.println("ARRAY END");
  
  for (int i= 1; i < sample_size; i++) {
    pi_arr[i-1] = pi_arr[i];
  }

  Serial.println(count);
  
  if (count >= count_threshold) {
    Serial.println("DETECT");
    digitalWrite(robot_LED_pin, HIGH);
    right180Turn();
  } else {
    //halt();
    digitalWrite(robot_LED_pin, LOW);   
    navigate();
  }
  //delay(1000);

}

// ============================================================================================

// Robot detection code take 2: moving average

// ============================================================================================

void robot_detect_take2() {

<<<<<<< HEAD
  //if (robot_done == 0) {
    int photo_input = analogRead(left_robot_detect);
  
    pi_arr[sample_size-1] = photo_input;
  
    for (int i= 1; i < sample_size; i++) {
      pi_arr[i-1] = pi_arr[i];
    }
  
    sum = 0;
    Serial.println("ARRAY START");
    for (int i= 0; i < sample_size; i++) {
      Serial.println(pi_arr[i]);
      sum += pi_arr[i];
    }
    Serial.println("ARRAY END");
  
    avg = sum/sample_size;
    Serial.println("AVERAGE");
    Serial.println(avg);
  //}

  //if (avg > threshold && robot_done == 0) {
  if (avg > threshold) {
    Serial.println("DETECT!");
    delay(1000);
    digitalWrite(robot_LED_pin, HIGH);
    //right180Turn();
  } else {
    Serial.println("ELSE");
    digitalWrite(robot_LED_pin, LOW);
=======
  int photo_input = analogRead(left_robot_detect);

  pi_arr[sample_size-1] = photo_input;

  for (int i= 1; i < sample_size; i++) {
    pi_arr[i-1] = pi_arr[i];
  }

  sum = 0;
  Serial.println("ARRAY START");
  for (int i= 0; i < sample_size; i++) {
    Serial.println(pi_arr[i]);
    sum += pi_arr[i];
  }
  Serial.println("ARRAY END");

  avg = sum/sample_size;
  Serial.println("AVERAGE");
  Serial.println(avg);

  if (avg > threshold) {
    Serial.println("DETECT");
    //digitalWrite(robot_LED_pin, HIGH);
    //right180Turn();
  } else {
    Serial.println("ELSE");
    //digitalWrite(robot_LED_pin, LOW);
>>>>>>> 5170c4b849c9b48bf2d30e593b8ec2c58b52546b
    //navigate();
    //robot_done = 0;
  }
  
}

// ============================================================================================

// Setup

// ============================================================================================

void setup() {
  // put your setup code here, to run once:
  pinMode(left_robot_detect, INPUT);
  Serial.begin(9600);
  right.attach(right_pin);
  left.attach(left_pin);
  pinMode(LEFT_LINE_SENSOR, INPUT);
  pinMode(RIGHT_LINE_SENSOR, INPUT);
  pinMode(left_ir_sensor, INPUT);
  pinMode(front_ir_sensor, INPUT);
  pinMode(robot_LED_pin, OUTPUT);
  pinMode(START_BUTTON, INPUT);
  
}

// ============================================================================================

// Loop

// ============================================================================================

void loop() {
<<<<<<< HEAD
  halt();
//
=======
  //halt();

>>>>>>> 5170c4b849c9b48bf2d30e593b8ec2c58b52546b
//  while (beginning) { // to wait for pushbutton/950 Hz tone
//    halt();
//    if (digitalRead(START_BUTTON)) {
//      beginning = 0;
//      /*
//      for (int i = 0; i < 2; i++) { // WARNING: PLEASE STEP A SAFE DISTANCE AWAY FROM THE ROBOT ;)
//        digitalWrite(DONE_LED, HIGH);
//        delay(500);
//        digitalWrite(DONE_LED, LOW);
//        delay(500);
//      }*/
//    }
//  }

  //robot_detect_take1();
  robot_detect_take2();
  
}
