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
int threshold = 250;
int count_threshold = 5;
int sample_size = 20;
int robot_done = 0;

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
    
  } else if (left_sensor_value >= line_threshold && right_sensor_value >= line_threshold) {
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
  
  int photo_input = analogRead(left_robot_detect);

  pi_arr[sample_size-1] = photo_input;

  for (int i= 1; i < sample_size; i++) {
    pi_arr[i-1] = pi_arr[i];
  }

  for (int i= 0; i < sample_size; i++) {
    sum += pi_arr[i];
  }

  avg = sum/sample_size;

  if (avg > threshold && robot_done == 0) {
    Serial.println("DETECT");
    digitalWrite(robot_LED_pin, HIGH);
    right180Turn();
  } else {
    digitalWrite(robot_LED_pin, LOW);
    robot_done = 0;  
    navigate();
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
  
}

// ============================================================================================

// Loop

// ============================================================================================

void loop() {
  halt();

  //robot_detect_take1();
  robot_detect_take2();
  
}
