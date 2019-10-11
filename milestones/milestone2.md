# Milestone 2: Wall Following

## Introduction

The milestone that was achieved involved having our robot first successfully circle an arbitrary set of walls and then demonstrate that our robot does line tracking and avoids walls at the same time. In order to reach this milestone, we had to rely on the Arduino coding and breadboard wiring skills learned in Labs 1 and 2 to utilize input from line sensors as well as infrared wall sensors to ensure the robot stayed on track while not hitting a wall. We chose to use three IR sensors, and attached them beneath the top platform. We made sure to place them such that they are approximately at right angles from each other. You can see our placement in the photo below.

## INSERT IMAGE OF ROBOT WITH SENSORS HERE

Dan and Joy were the two that worked on this milestone. Jinny and Ryan worked on the FFT analysis of Lab 2 during this time. They also began working on the robot detection section of this milestone before that part was switched to Milestone 3.

## Wall Following

<p align="center">
  <iframe width="560" height="315" src="https://www.youtube.com/embed/r7wxAMWEsIM" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

<p align="center">
  <iframe width="560" height="315" src="https://www.youtube.com/embed/ZjsPwd34qOY" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

```c
// This program allows the robot to nagivate any arbitrary set of walls.

... // Declarations

... // Moving commands (i.e. forward, turn left/right/around, 
... // slight left/right to correct during straightaways, etc.)

void navigate() {

  // if both sensors on white
  if (left_sensor_value < threshold && right_sensor_value < threshold) {
    
    // Move forward and prepare to potentially turn.
    moveForward();
    delay(500);

    right_detect = digitalRead(right_ir_sensor);  // 0 when detecting
    left_detect = digitalRead(left_ir_sensor);   // 0 when detecting
    front_detect = digitalRead(front_ir_sensor);  // 0 when detecting

    // case 0: no walls --> turn right
    if (right_detect && left_detect && front_detect) {
      right90Turn();
    }
    
    // case 1: right wall, left wall, front wall --> turn around
    else if (!right_detect && !left_detect && !front_detect) {
      right180Turn();
    }
    
    // case 2: left wall, front wall --> turn right
    else if (right_detect && !left_detect && !front_detect) {
      right90Turn();
    }

    // case 3: right wall, front wall --> turn left
    else if (!right_detect && left_detect && !front_detect) {
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
