# Milestone 1: Line Following and Figure Eight

## Introduction

The milestone that was achieved involved having our robot first follow and traverse a 
line of white tape on a black floor and then traverse a figure eight while still following
the tape on the ground. In order to reach this milestone, we had to rely on the Arduino coding 
and breadboard wiring skills learned in Lab 1 to utilize input from line sensors to ensure the robot stayed on track.
We chose to use two line sensors, and attached
them as close to the ground as possible near the front of the robot. We made sure
to space them apart so that they straddled the tape. You can
see our placement in the photo below; the sensors are attached to the extenders on either side
of the ball bearing.

<p align="center">
  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/milestones/teamphoto.jpg" height="420" width="360">
</p>

Since this milestone involved almost entirely software, we all contributed to the creation of the algorithms through the entirety of this project. However, Jinny and Joy primarily debugged the figure eight as Dan and Ryan moved to begin work on the second lab.

## Line Following

Since we were using two line sensors, we used multiple conditional statements to check each possible
condition for the robot's placement relative to the white lines. If both line sensors were detecting
black, it meant that the robot was on the correct path and would move forward. If only the right
sensor was detecting black, then the left sensor must be drifting right over the line, so the robot
would  make a slight left turn until the left sensor was also detecting black. Likewise, if only the
left sensor was detecting black, then the right sensor must be drifting left over the line, so the
robot would make a slight right turn until both sensors were detecting black again. These slight
turns are executed while the robot is still moving forward so that speed is not sacrificed. Finally, we have a condition that checks
for both sensors on white, indicating the robot has reached an intersection. This was useful for
debugging and became more prevalent in the second part of the milestone, but we tell
the robot to keep moving forward if this condition exists for this section. A video showing a successful
line traversal as well as code showing the algorithm we used to help the robot navigate are below.

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
      left_sensor_value = analogRead(LEFT_LINE_SENSOR);
    }    
  } else if (left_sensor_value >= threshold && right_sensor_value < threshold) {
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

## Figure Eight

To have our robot navigate a figure eight, we set up a series of conditional statements within the
first if statement that checks if both line sensors detect white – meaning that the robot is at a
junction. Outside of this function, we declared a global variable called count that would keep track
of what type of turn we wanted the robot to perform: right or left. As a figure eight can be simply
broken down into four right turns and then four left turns (or vice versa), the robot would make right
turns for count values 0-3 and left turns for count values 4-7. After every sequence of four rights and
four lefts, we zeroed out the count variable to allow the navigation of multiple figure eights. To have
the robot execute a turn, we created helper functions rotateRight and rotateLeft which write the same
value to both servos, allowing the robot's wheels to rotate in opposite directions with the same speed.
Since every junction is made up of 90 degree angles, we let the robot rotate for 700 milliseconds which
is about the amount of time needed to make a 90 degree turn. After the turn is finished, the count is
incremented and the figureEight function is done executing. On the next call to this function, the robot
will simply perform the line-following logic that we previously implemented. This is because now that
the turn is finished, the line sensors are no longer detecting white and all of the turn logic will be
skipped. A video showing a successful figure eight traversal as well as code showing the algorithm we used to 
help the robot navigate are below.

<p align="center">
  <iframe width="560" height="315" src="https://www.youtube.com/embed/FR61TYuzD8M" frameborder="0" allow="accelerometer; autoplay; 
  encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>


```c
// This program allows the robot to traverse a figure eight.

#include <Servo.h>

// Servo and connection setup.
Servo left;
Servo right;
int right_pin = 6;
int left_pin = 5;
int LEFT_LINE_SENSOR = A5;
int RIGHT_LINE_SENSOR = A4;

// Initiate counter to determine turn direction.
int count = 0;

// this integer is the boundary between "white" and "not white"
// (NB: values less than the threshold indicate white).
int threshold = 666;

int left_sensor_value;
int right_sensor_value;

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

void figureEight() {
  left_sensor_value = analogRead(LEFT_LINE_SENSOR);
  right_sensor_value = analogRead(RIGHT_LINE_SENSOR);

  if (left_sensor_value < threshold && right_sensor_value < threshold) {
    //if both sensors on white
    moveForward();
    delay(500);

    if (count == 8) {
    // zero out count
      count=0;
    } 
    if (count < 4) {
    // right turn rotate 
      rotateRight();
      delay(700);
    } else {
    // left turn rotate
      rotateLeft();
      delay(700);
    }
    count++;
  }
  
  else if (left_sensor_value < threshold && right_sensor_value >= threshold) {
    // left detects white, right detects black
    while (left_sensor_value < threshold) {
      slightLeft();
      left_sensor_value = analogRead(LEFT_LINE_SENSOR);
    }    
  }
  
  else if (left_sensor_value >= threshold && right_sensor_value < threshold) {
    // left detects black, right detects white
    while (right_sensor_value < threshold) {
      slightRight();
      right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
    }
  }
  
  else {
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
  figureEight();
}
```

## Conclusion

Overall, this milestone wasn't too difficult. We did, however, encounter a few problems. For a decent amount of time, our motors weren't behaving as expected and were spinning when we thought they should have stopped. We realized the values we were writing to the servos were incorrect and quickly fixed that mistake. Another issue was not being able to detect an intersection. After many trials and postulations, we came to the conclusion that a combination of dirty tape and shadows were the perpetrators. Once we moved to a cleaner area of the floor and didn't stand too close and block light, the detection worked without a problem. Our final issue was that we would be infinitely turning right once we hit the intersection. After some troubleshooting, we realized this was because we didn't update our sensor readings once we hit a while loop and thus were stuck forever. We fixed that and finally our robot was able to do what it was born to do.

At the end of this milestone, our robot, Axel, was successfully able to track lines and complete figure eights.
