# Milestone 2: Wall Following

## Introduction

## Wall Following

<p align="center">
  <iframe width="560" height="315" src="https://www.youtube.com/embed/r7wxAMWEsIM" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

<p align="center">
  <iframe width="560" height="315" src="https://www.youtube.com/embed/ZjsPwd34qOY" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

```c
// Program to allow the robot to nagivate walls

... // Declarations

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

  // if both sensors on white
  if (left_sensor_value < threshold && right_sensor_value < threshold) {
    
    // Move forward and prepare to potentially turn.
    moveForward();
    delay(500);

    // right IR sensor read + distance calculation
    right_detect = digitalRead(right_ir_sensor);  // 0 when detecting
  
    // left IR sensor read + distance calculation
    left_detect = digitalRead(left_ir_sensor);   // 0 when detecting
  
    // front IR sensor read + distance calculation
    front_detect = digitalRead(front_ir_sensor);  // 0 when detecting

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

... // setup

void loop() {
  // put your main code here, to run repeatedly:  
  navigate();
}
```

## Conclusion
