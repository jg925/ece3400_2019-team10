# Milestone 1: Line Following and Figure Eight

This first milestone of the final project required us to rely on the skills
learned in Lab 1 and utilize line sensors to have the robot follow a while line
made with tape on a black background. We chose to use two line sensors, and attached
them as close to the ground as possible near the front of the robot. We made sure
to space them apart so that they were a bit more than the width of the tape apart. You can
see our placement in the photo below; the sensors are attached to the extenders on either side
of the ball bearing.

<p align="center">
  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/milestones/teamphoto.jpg" height="420" width="360">
</p>

The following code block shows the algorithm we used to help the robot navigate. 
Since we were using two line sensors, we used multiple conditional statements to check each 
possible condition for the robot's placement relative to the white lines. If both
line sensors were detecting black, it meant that the robot was on the correct path
and would move forward. If only the right sensor was detecting black, then the
left sensor must be drifting right over the line, so the robot would 
make a slight left turn until the left sensor was also detecting black. Likewise, 
if only the left sensor was detecting black, then the right sensor must be drifting
left over the line, so the robot would make a slight right turn until both sensors were
detecting black again. These slight turns are executed while the robot is still moving forward.
Finally, we have a condition that checks for both sensors on white, indicating the robot has
reached an intersection. This was useful for debugging and became more prevalent in the second
part of the milestone.

<p align="center">
<iframe width="560" height="315" src="https://www.youtube.com/embed/-ANTz5VJQi0" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

```c
// This program allows the robot to trace a white line.

#include <Servo.h>

// Servo and connection setup.
Servo left_motor;
Servo right_motor;
int right_pin = 6;
int left_pin = 5;
int LEFT_LINE_SENSOR = A5;
int RIGHT_LINE_SENSOR = A4;

// This integer is the boundary between "white" and "not white"
// (NB: values less than the threshold indicate white).
int threshold = 666;

// To store the values read from the line sensors.
int left_sensor_value;
int right_sensor_value;

void move() {
  left_sensor_value = analogRead(LEFT_LINE_SENSOR);
  right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
  if (left_sensor_value < threshold && right_sensor_value < threshold) {
    // both sensors detect white (to be used in turning later)
    moveForward();
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
  move();
}
```

The following video shows Axel relying on line sensors to follow a Figure Eight.

<p align="center">
<iframe width="560" height="315" src="https://www.youtube.com/embed/FR61TYuzD8M" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>
