#include <Servo.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

// =====================================================================================================

// robot motion variables inits

// =====================================================================================================

Servo left;
Servo right;

// servo pins
int right_pin = 6;
int left_pin = 5;

// line sensor pins
int LEFT_LINE_SENSOR = A5;
int RIGHT_LINE_SENSOR = A4;

// wall sensor pins
int right_ir_sensor = 2;
int left_ir_sensor = 3;
int front_ir_sensor = 4;

// initialization of ir wall sensor values
int right_detect;
int left_detect;
int front_detect;

// boundary between "white" and "black"
int line_threshold = 650;

// initialization of line sensor values
int left_sensor_value;
int right_sensor_value;

// =====================================================================================================

// state variables inits

// =====================================================================================================

// initialize green "done LED" and push button "start button"
int DONE_LED = 8;
int START_BUTTON = 7;

// initialize "beginning" which waits for button press or 950 Hz signal and "ending"
// which goes HIGH when we finished traversing the maze
int beginning;
int ending;

// =====================================================================================================

// dfs inits

// =====================================================================================================

struct node {
  byte pos; // xxxxyyyy
  byte dir; // 0000dddd, where first bit is north, second is east, third is south, and fourth is west.
};

struct box {
  byte visited_came; // v000xxxx, tells if visited with v bit, xxxx tells which direction we came from.
  byte neighbors; // s000xxxx, tells if the walls were sent with the s bit which locations are free to be moved to.
                  // 1111 means everywhere has been traversed or there are walls (a.k.a. return).
};

box maze[81]; // 9x9
node current;

// =====================================================================================================

// robot motion helpers

// =====================================================================================================

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

// =====================================================================================================

// line following

// =====================================================================================================

int navigate() {
  left_sensor_value = analogRead(LEFT_LINE_SENSOR);
  right_sensor_value = analogRead(RIGHT_LINE_SENSOR);

  // if both sensors on white
  if (left_sensor_value < line_threshold && right_sensor_value < line_threshold) {
    moveForward();
    delay(500);
    return 1; // return 1 here, else we return 0 to keep navigating
  }

  // left on white, right on black --> slight left adjust
  else if (left_sensor_value < line_threshold && right_sensor_value >= line_threshold) {
    // left detects white, right detects black
    while (left_sensor_value < line_threshold && right_sensor_value >= line_threshold) {
      slightLeft();
      left_sensor_value = analogRead(LEFT_LINE_SENSOR);
      right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
    }
  }

  // right on white, left on black --> slight right adjust
  else if (left_sensor_value >= line_threshold && right_sensor_value < line_threshold) {
    // left detects black, right detects white
    while (right_sensor_value < line_threshold && left_sensor_value >= line_threshold ) {
      slightRight();
      left_sensor_value = analogRead(LEFT_LINE_SENSOR);
      right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
    }
  }

  // both sensors detect black --> go straight
  else if (left_sensor_value >= line_threshold && right_sensor_value >= line_threshold) {
    moveForward();
  }
  return 0;
}

// =====================================================================================================

// dfs helpers

// =====================================================================================================

void movetoLocation (byte location) {
  int diffx = int(current.pos >> 4) - int(location >> 4);
  int diffy = (int(current.pos << 4) >> 4) - (int(location << 4) >> 4);
  byte face;
  if (diffx == 1) {
    face = B00000001;
  } else if (diffx == -1) {
    face = B00000100;
  } else if (diffy == 1) {
    face = B00000010;
  } else { // diffy == -1
    face = B00001000;
  }

  while (current.dir != face) {
    left90Turn();
    current.dir = current.dir << 1;
    if (int(current.dir) == 16) {
      current.dir = B00000001;
    }
  }
  
  int go_on = 0;
  while ( go_on != 1 ) { // Want to navigate to next intersection at location
    go_on = navigate();
  }
  halt(); // Just so we don't go anywhere ;)

  current.pos = location; // Our current location is now location, facing same direction as we were at the time of calling this function
  maze[int(location)].visited_came = B10000000 | face; // Mark location as visited and update "came" direction
}

int moveNorth() {
  byte location = current.pos;
  if (int((location & B00001111) + B00000001) < 9) { // if y-to-be < 9
    if ( (int((maze[int(location)].neighbors & B00001111) >> 3) == 0) && ((maze[int(location + B00000001)].visited_came >> 7) == 0) ) { // free to move north
      movetoLocation(location + B00000001);
      return 1;
    }
    return 0;
  }
  return 0;
}

int moveEast() {
  byte location = current.pos;
  if (int((location >> 4) + B00000001) < 9) { // if x-to-be < 9
    if ( (int((maze[int(location)].neighbors & B00000100) >> 2) == 0) && ((maze[int(location + B00010000)].visited_came >> 7) == 0) ) { // free to move east
      movetoLocation(location + B00010000);
      return 1;
    }
    return 0;
  }
  return 0;
}

int moveSouth() {
  byte location = current.pos;
  if (int((location & B00001111) - B00000001) >= 0) { // if y-to-be >=0
    if ( (int((maze[int(location)].neighbors & B00000010) >> 1) == 0) && ((maze[int(location - B00000001)].visited_came >> 7) == 0) ){ // free to move south
      movetoLocation(location - B00000001);
      //maze[int(location - B000000010].
      return 1;
    }
    return 0;
  }
  return 0;
}

int moveWest() {
  byte location = current.pos;
  if (int((location >> 4) - B00000001) >= 0) { // if x-to-be >= 0
    if ( (int((maze[int(location)].neighbors & B0000001)) == 0) && ((maze[int(location - B00010000)].visited_came >> 7) == 0) ) { // free to move west
      movetoLocation(location - B00010000);
      return 1;
    }
    return 0;
  }
  return 0;
}

void determineWalls( byte location ) { // location is current location
  left_detect = digitalRead(left_ir_sensor);  // 0 when detecting
  front_detect = digitalRead(front_ir_sensor);  // 0 when detecting
  right_detect = digitalRead(right_ir_sensor);  // 0 when detecting

  if (int(current.dir) == 8) { // north
      if (!left_detect) {
        maze[int(location)].neighbors = maze[int(location)].neighbors | B00000001;
      }
      if (!front_detect) {
        maze[int(location)].neighbors = maze[int(location)].neighbors | B00001000;
      }
      if (!right_detect) {
        maze[int(location)].neighbors = maze[int(location)].neighbors | B00000100;
      }  
  } else if (int(current.dir) == 4) { // east
      if (!left_detect) {
        maze[int(location)].neighbors = maze[int(location)].neighbors | B00001000;
      }
      if (!front_detect) {
        maze[int(location)].neighbors = maze[int(location)].neighbors | B00000100;
      }
      if (!right_detect) {
        maze[int(location)].neighbors = maze[int(location)].neighbors | B00000010;
      }
  } else if (int(current.dir) == 2) { // south
      if (!left_detect) {
        maze[int(location)].neighbors = maze[int(location)].neighbors | B00000100;
      }
      if (!front_detect) {
        maze[int(location)].neighbors = maze[int(location)].neighbors | B00000010;
      }
      if (!right_detect) {
        maze[int(location)].neighbors = maze[int(location)].neighbors | B00000001;
      }
  } else { // west
      if (!left_detect) {
        maze[int(location)].neighbors = maze[int(location)].neighbors | B00000010;
      }
      if (!front_detect) {
        maze[int(location)].neighbors = maze[int(location)].neighbors | B00000001;
      }
      if (!right_detect) {
        maze[int(location)].neighbors = maze[int(location)].neighbors | B00001000;
      }
  }
}

void walkBack() {
  byte to_return_dir = maze[int(current.pos)].visited_came & B00001111;
  byte to_return_pos;
  byte face;

  if (int(to_return_dir) == 8) {
    face = B00000010;
    to_return_pos = current.pos - B00000001;
  } else if (int(to_return_dir) == 4) {
    face = B00000001;
    to_return_pos = current.pos - B00010000;
  } else if (int(to_return_dir) == 2) {
    face = B00001000;
    to_return_pos = current.pos + B00000001;
  } else { // (int(to_return_dir) == 1)
    face = B00000100;
    to_return_pos = current.pos + B00010000;
  }

  while (current.dir != face) {
    left90Turn();
    current.dir = current.dir << 1;
    if (int(current.dir) == 16) {
      current.dir = B00000001;
    }
  }

  // Navigate to return_to, update current position

  int go_on = 0;
  while ( go_on != 1 ) { // Want to get to next intersection (but to the last place in the path)
    go_on = navigate();
  }
  //halt(); // Just so we don't go anywhere ;)
  current.pos = to_return_pos;
}

// =====================================================================================================

// dfs main

// =====================================================================================================

void dfs() {
  byte location = current.pos;
  
  determineWalls(location);

  // transmit info
  //communicate();
  maze[int(current.pos)].neighbors = maze[int(current.pos)].neighbors | B10000000; // set walls sent to 1

  // Movement priority

  int wemoved = 1;
  
  if (int(current.dir) == 8) { // facing north
    if (!moveNorth()) {
      if (!moveWest()) {
        if (!moveEast()) {
          wemoved= moveSouth();
        }
      }
    }
  } else if (int(current.dir) == 4) { // facing east
    if (!moveEast()) {
      if (!moveNorth()) {
        if (!moveSouth()) {
          wemoved= moveWest();
        }
      }
    }
  } else if (int(current.dir) == 2) { // facing south
    if (!moveSouth()) {
      if (!moveEast()) {
        if (!moveWest()) {
          wemoved= moveNorth();
        }
      }
    }
  } else { // facing west
    if (!moveWest()) {
      if (!moveSouth()) {
        if (!moveNorth()) {
          wemoved= moveEast();
        }
      }
    }
  }
  if (!wemoved && current.pos != 0) { // if we didn't move, i.e. all neighbors have been visited and/or have walls
    walkBack();
  } else if (current.pos == 0 && current.dir != 8) {
    left90Turn();
    current.dir = current.dir << 1;
  }
}

// =====================================================================================================

// setup

// =====================================================================================================

void setup() {
  //
  // pins setup
  //
  
  // line sensors
  pinMode(LEFT_LINE_SENSOR, INPUT);
  pinMode(RIGHT_LINE_SENSOR, INPUT);

  // wall sensors
  pinMode(left_ir_sensor, INPUT);
  pinMode(front_ir_sensor, INPUT);
  pinMode(right_ir_sensor, INPUT);

  // ir detectors
//  pinMode(left_robot_detect, INPUT);
  //pinMode(center_robot_detect, INPUT);
  //pinMode(right_robot_detect, INPUT);

  // button/led
  pinMode(START_BUTTON, INPUT);
  pinMode(DONE_LED, OUTPUT);

  // serial and print setup
  Serial.begin(57600);

  // servo setup
  right.attach(right_pin);
  left.attach(left_pin);

  // initial state setup
  halt();
  digitalWrite(DONE_LED, LOW);
  beginning = 1;
  ending = 0;
  current = { B00000000, B00001000 }; // set at (0,0), facing north
  for (int i = 0; i < 81; i++) {
    maze[i].visited_came = B00000000; // initialize all locations to be not visited
    maze[i].neighbors = B00000000;  // initialize all neighbors to be open
  }
  maze[0].visited_came = B10000000; // mark (0,0) as visited
}

// =====================================================================================================

// main loop

// =====================================================================================================

int stopped[3] = {0, 0, 0};

void loop() {
  while (beginning) { // wait for pushbutton/950 Hz tone
    if (digitalRead(START_BUTTON)) {// fft();
      beginning = 0;
      for (int i = 0; i < 2; i++) { // Signal we are about to begin
        digitalWrite(DONE_LED, HIGH);
        delay(500);
        digitalWrite(DONE_LED, LOW);
        delay(500);
      }
    }
  }
  
  if (!ending) { // to make sure we don't keep doing stuff after we finish
    dfs();
    stopped[0] = stopped[1]; // shift element over
    stopped[1] = stopped[2]; // shift element over
    if (int(current.pos) == 0) { // fill in new opening
      stopped[2] = 1;
      if (stopped[0] == 1 && stopped[1] == 1) {
        ending = 1;
      }
    } else {
      stopped[2] = 0;
    }
  } else {
    digitalWrite(DONE_LED, HIGH);
  }
}
