#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft
#include <Servo.h>
#include <FFT.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

// =====================================================================================================

// communication: hardware setup

// =====================================================================================================

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9, 10);

// =====================================================================================================

// communication: topology

// =====================================================================================================

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0x0000000016LL, 0x0000000017LL };

// =====================================================================================================

// robot motion variables inits

// =====================================================================================================

Servo left;
Servo right;

// servo pins
#define right_pin 5
#define left_pin  6

// line sensor pins
#define LEFT_LINE_SENSOR A5
#define RIGHT_LINE_SENSOR A4

// wall sensor pins
#define right_ir_sensor 2
#define left_ir_sensor 3
#define front_ir_sensor 4

// initialization of ir wall sensor values
int right_detect;
int left_detect;
int front_detect;

// boundary between "white" and "black"
#define line_threshold 775

// =====================================================================================================

// robot detection variables inits

// =====================================================================================================

// phototransistor pins
#define center_robot_detect A2

// ir robot detection variables
#define robot_threshold 825
int robot_detected = 0;

// =====================================================================================================

// state variables inits

// =====================================================================================================

// green "done LED" and push button "start button"
#define DONE_LED A1
#define START_BUTTON 7

// initialize "beginning" which waits for button press or 950 Hz signal and "ending"
// which goes HIGH when we finished traversing the maze
int beginning;
int ending;

#define maze_size 81 // 9x9

// =====================================================================================================

// dfs inits

// =====================================================================================================

struct node {
  byte pos; // xxxxyyyy
  byte dir; // 0000dddd, where first bit is north, second is east, third is south, and fourth is west.
};

struct box {
  byte vs_came; // vs00dddd, tells if visited with v bit, the walls were sent with the s bit,
  // dddd tells which direction we came from where 1000 is N, 0100 is E, 0010 is S, and 0001 is W

  byte walls_neighbors; // wwwwnnnn, tells where walls are (using cardinal directions) with wwww, where is available to move/not move with nnnn (using cardinal directions)
  // where 1000 is N, 0100 is E, 0010 is S, and 0001 is W. when nnnn is 1111, everywhere has been traversed and/or there are walls.
};

box maze[maze_size];
node current;

// =====================================================================================================

// robot motion helpers

// =====================================================================================================

int going_fast = 0; // to help with jerking

void halt() {
  left.write(96);   // normal servos: 90
  right.write(96);  // normal servos: 90
  going_fast = 0;
}

void moveForward() {
  if (!going_fast) { // trying to reduce jerk
    left.write(97);
    right.write(95);
    delay(30);
    left.write(98);
    right.write(94);
    delay(30);
    left.write(99);
    right.write(93);
    delay(30);
    left.write(100);
    right.write(92);
    delay(30);
  }
  going_fast = 1;
  left.write(101);
  right.write(91);
}

void slightRight() { // for adjustments when line-following
  left.write(103);
  right.write(92);
  //going_fast = 0;
}

void slightLeft() { // for adjustments when line-following
  left.write(100);
  right.write(89);
  //going_fast = 0;
}

void rotateRight() {
  left.write(99); //101
  right.write(99); //101
  going_fast = 0;
}

void rotateLeft() {
  left.write(93); //91
  right.write(93); //91
  going_fast = 0;
}

void right90Turn() {
  if (!beginning && !robot_detected) { // Don't want to move forward if starting in tunnel or there is a robot in front of us
    moveForward();
    delay(100);
  }
  robot_detected = 0;
  rotateRight();
  delay(200); // to give robot time to rotate before checking if turn is complete
  while (analogRead(RIGHT_LINE_SENSOR) >= line_threshold) { // keep rotating while right not on white
    continue;
  }
  while (analogRead(LEFT_LINE_SENSOR) >= line_threshold) { // keep rotating while left not on white
    continue;
  }
  going_fast = 0;
}

void left90Turn() {
  if (!robot_detected) { // Don't want to move forward if there is a robot in front of us
    moveForward();
    delay(100);
  }
  robot_detected = 0;
  rotateLeft();
  delay(200); // to give robot time to rotate before checking if turn is complete
  while (analogRead(LEFT_LINE_SENSOR) >= line_threshold) { // keep rotating while left not on white
    continue;
  }
  while (analogRead(RIGHT_LINE_SENSOR) >= line_threshold) { // keep rotating while right not on white
    continue;
  }
  going_fast = 0;
}

void right180Turn() {
  if (!robot_detected) { // Don't want to move forward if there is a robot in front of us
    moveForward();
    delay(100);
  }
  robot_detected = 0;
  rotateRight();
  delay(200); // to give robot time to rotate before checking if turn is complete
  for (int w = 0; w < 2; w++) { // turn until we hit an intersection, continue turning until we hit another intersection
    while (analogRead(RIGHT_LINE_SENSOR) >= line_threshold) { // keep rotating while right not on white
      continue;
    }
    while (analogRead(LEFT_LINE_SENSOR) >= line_threshold) { // keep rotating while left not on white
      continue;
    }
  }
  going_fast = 0;
}

// =====================================================================================================

// line following

// =====================================================================================================

int navigate() {
  int left_sensor_value = analogRead(LEFT_LINE_SENSOR);
  int right_sensor_value = analogRead(RIGHT_LINE_SENSOR);

  // if both sensors on white
  if (left_sensor_value < line_threshold && right_sensor_value < line_threshold) {
    moveForward();
    delay(100); // move a little past the intersection
    halt(); // stop
    delay(50); // slight delay to let communication catch up
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

// robot detection

// =====================================================================================================

int detect_robots() {
  int center_input = analogRead(center_robot_detect);
  return center_input > robot_threshold;
}

// =====================================================================================================

// fft helper

// =====================================================================================================

int freq_detect( int five, int six, int seven ) {
  return ( six > 122 && six < 137 && seven > 120 && seven < 126 && five < 100 ); // digital filter on fft output
}

// =====================================================================================================

// fft main

// =====================================================================================================

int fftboi() {
  halt();
  int flag_950 = 0;
  right.detach(); // for stability
  left.detach(); // for statbility

  cli();  // UDRE interrupt slows this way down on arduino1.0

  int tempTIM = TIMSK0; // store old value
  int tempSRA = ADCSRA; // store old value
  int tempMUX = ADMUX; // store old value
  int tempDID = DIDR0; // store old value

  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0

  // sample fft algorithm from fft website

  byte m;
  byte j;
  int k;
  for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
    while (!(ADCSRA & 0x10)); // wait for adc to be ready
    ADCSRA = 0xf5; // restart adc
    m = ADCL; // fetch adc data
    j = ADCH;
    k = (j << 8) | m; // form into an int
    k -= 0x0200; // form into a signed int
    k <<= 6; // form into a 16b signed int
    fft_input[i] = k; // put real data into even bins
    fft_input[i + 1] = 0; // set odd bins to 0
  }
  fft_window(); // window the data for better frequency response
  fft_reorder(); // reorder the data before doing the fft
  fft_run(); // process the data in the fft
  fft_mag_log(); // take the output of the fft
  sei();

  int detect = freq_detect( fft_log_out[5], fft_log_out[6], fft_log_out[7] ); // determine if 950 Hz detected
  if (detect || digitalRead(START_BUTTON)) { // if we detect the frequency or the push button is pressed
    flag_950 = 1;
  }

  TIMSK0 = tempTIM; // restore old value
  ADCSRA = tempSRA; // restore old value
  ADMUX = tempMUX; // restore old value
  DIDR0 = tempDID; // restore old value

  right.attach(right_pin); // reattach the servo
  left.attach(left_pin); // reattach the servo

  return flag_950;
}

// =====================================================================================================

// communication

// =====================================================================================================

void communicate() {
  // 000swwwwxxxxyyyy is the scheme we are using

  // zeroes out msg
  uint16_t msg = 0000000000000000;

  // walls already sent bit; NOTE: IF THIS IS 1, DO NOT DRAW THE WALLS AGAIN
  msg = (msg << 1) | ((maze[int(current.pos & B00001111) * 9 + int(current.pos >> 4)].vs_came & B01000000) >> 6);

  // walls bits
  msg = (msg << 4) | (maze[int(current.pos & B00001111) * 9 + int(current.pos >> 4)].walls_neighbors >> 4);

  // x pos bits
  msg = (msg << 4) | (current.pos >> 4);

  // y pos bits
  msg = (msg << 4) | (current.pos & B00001111);

  // First, stop listening so we can talk.
  radio.stopListening();

  //printf("Now sending %x...",msg);
  bool ok = radio.write( &msg, sizeof(uint16_t) );

  // For debugging purposes:
  if (ok)
    printf("ok...");
  else
    printf("failed.\n\r");

  // Now, continue listening
  radio.startListening();

  //  Wait here until we get a response, or timeout (250ms)
  unsigned long started_waiting_at = millis();
  //
  bool timeout = false;
  while ( ! radio.available() && ! timeout ) {
    if (millis() - started_waiting_at > 250 ) {
      timeout = true;
    }
  }

  // Describe the results (we kept this because it didn't work when we got rid of it
  if ( !timeout ) {
    //    // Grab the response, compare, and send to debugging spew
    uint16_t got_msg;
    radio.read( &got_msg, sizeof(uint16_t) );
    //
    //    // Spew it
    printf("Got response %x \n\r", msg);
    //
  }
  else {
    printf(" We got issues");
  }
}

// =====================================================================================================

// dfs helpers

// =====================================================================================================

int movetoLocation (byte location) {
  int diffx = int(current.pos >> 4) - int(location >> 4);
  int diffy = int(current.pos & B00001111) - int(location & B00001111);
  byte face;

  // determine where to face based on difference of current position/position to move to
  if (diffx == 1) {
    face = B00000001;
  } else if (diffx == -1) {
    face = B00000100;
  } else if (diffy == 1) {
    face = B00000010;
  } else { // diffy == -1
    face = B00001000;
  }

  // Figure out how to turn the fastest
  int curr_dir = current.dir;
  int i = 1;
  for (i; i < 4; i++) {
    curr_dir = curr_dir << 1;
    if (curr_dir > 8) {
      curr_dir = B00000001;
    }
    if (curr_dir == face) {
      break;
    }
  }

  // Turn to face the destination
  if (i == 1) {
    left90Turn();
  } else if (i == 2) {
    right180Turn();
  } else if (i == 3) {
    right90Turn();
  }
  current.dir = face;

  if (detect_robots()) { // If we detect, don't move forward!!
    robot_detected = 1;
    return 0;
  }

  // Update current pos with destination as unnavigable (because we are moving there/don't want repeats)
  maze[int(current.pos & B00001111) * 9 + int(current.pos >> 4)].walls_neighbors = maze[int(current.pos & B00001111) * 9 + int(current.pos >> 4)].walls_neighbors | face;

  // Actually move to destination
  int go_on = 0;
  while ( go_on != 1 ) { // Want to navigate to next intersection at location
    go_on = navigate();
  }
  current.pos = location; // Our current location is now location, facing same direction as we were at the time of calling this function
  maze[int(current.pos & B00001111) * 9 + int(current.pos >> 4)].vs_came = B10000000 | face; // Mark new current location as visited and update "came" direction
  return 1;
}

void movetoLocation2 (byte location) {
  // EXACTLY THE SAME AS movetoLocation BUT DOESN'T UPDATE MAZE
  // USEFUL FOR ROBOT DETECTION IN A WALK BACK

  int diffx = int(current.pos >> 4) - int(location >> 4);
  int diffy = int(current.pos & B00001111) - int(location & B00001111);
  byte face;

  // determine where to face based on difference of current position/position to move to
  if (diffx == 1) {
    face = B00000001;
  } else if (diffx == -1) {
    face = B00000100;
  } else if (diffy == 1) {
    face = B00000010;
  } else { // diffy == -1
    face = B00001000;
  }

  // Figure out how to turn the fastest
  int curr_dir = current.dir;
  int i = 1;
  for (i; i < 4; i++) {
    curr_dir = curr_dir << 1;
    if (curr_dir > 8) {
      curr_dir = B00000001;
    }
    if (curr_dir == face) {
      break;
    }
  }

  // Turn to face the destination
  if (i == 1) {
    left90Turn();
  } else if (i == 2) {
    right180Turn();
  } else if (i == 3) {
    right90Turn();
  }
  current.dir = face;

  // Actually move to destination
  int go_on = 0;
  while ( go_on != 1 ) { // Want to navigate to next intersection at location
    go_on = navigate();
  }

  current.pos = location; // Our current location is now location, facing same direction as we were at the time of calling this function
}

int moveNorth() {
  byte location = current.pos;
  byte location_plus_one = location + B00000001; // location directly north of current location
  if (int(location_plus_one & B00001111) < 9) { // if y-to-be < 9 (i.e. within the maze)
    if ( (int((maze[int(location & B00001111) * 9 + int(location >> 4)].walls_neighbors & B00001000) >> 3) == 0) &&
         ((maze[int(location_plus_one & B00001111) * 9 + int(location_plus_one >> 4)].vs_came >> 7) == 0) ) { // if there isn't a wall to the north and the location hasn't been visited
      return movetoLocation(location_plus_one);
    }
    return 0;
  }
  return 0;
}

int moveEast() {
  byte location = current.pos;
  byte location_plus_sixteen = location + B00010000;  // location directly east of current location
  if (int(location_plus_sixteen >> 4) < 9) { // if x-to-be < 9 (i.e. within the maze)
    if ( (int((maze[int(location & B00001111) * 9 + int(location >> 4)].walls_neighbors & B00000100) >> 2) == 0) &&
         ((maze[int(location_plus_sixteen & B00001111) * 9 + int(location_plus_sixteen >> 4)].vs_came >> 7) == 0) ) { // if there isn't a wall to the east and the location hasn't been visited
      return movetoLocation(location_plus_sixteen);
    }
    return 0;
  }
  return 0;
}

int moveSouth() {
  byte location = current.pos;
  byte location_minus_one = location - B00000001; // location directly south of current location
  if (int(location_minus_one & B00001111) >= 0) { // if y-to-be >= 0 (i.e. within the maze)
    if ( (int((maze[int(location & B00001111) * 9 + int(location >> 4)].walls_neighbors & B00000010) >> 1) == 0) &&
         ((maze[int(location_minus_one & B00001111) * 9 + int(location_minus_one >> 4)].vs_came >> 7) == 0) ) { // if there isn't a wall to the south and the location hasn't been visited
      return movetoLocation(location_minus_one);
    }
    return 0;
  }
  return 0;
}

int moveWest() {
  byte location = current.pos;
  byte location_minus_sixteen = location - B00010000;  // location directly east of current location
  if (int(location_minus_sixteen >> 4) >= 0) { // if x-to-be >= 0 (i.e. within the maze)
    if ( (int(maze[int(location & B00001111) * 9 + int(location >> 4)].walls_neighbors & B00000001) == 0) &&
         ((maze[int(location_minus_sixteen & B00001111) * 9 + int(location_minus_sixteen >> 4)].vs_came >> 7) == 0) ) { // if there isn't a wall to the west and the location hasn't been visited
      return movetoLocation(location_minus_sixteen);
    }
    return 0;
  }
  return 0;
}

void determineWalls( byte location ) {
  // location is current location
  // used to update information about surroundings

  left_detect = digitalRead(left_ir_sensor);  // 0 when detecting
  front_detect = digitalRead(front_ir_sensor);  // 0 when detecting
  right_detect = digitalRead(right_ir_sensor);  // 0 when detecting

  int mazepos = int(location & B00001111) * 9 + int(location >> 4); // current index of maze (our current location)

  if (int(current.dir) == 8) { // north
    if (!left_detect) {
      maze[mazepos].walls_neighbors = maze[mazepos].walls_neighbors | B00010001;
    }
    if (!front_detect) {
      maze[mazepos].walls_neighbors = maze[mazepos].walls_neighbors | B10001000;
    }
    if (!right_detect) {
      maze[mazepos].walls_neighbors = maze[mazepos].walls_neighbors | B01000100;
    }
  } else if (int(current.dir) == 4) { // east
    if (!left_detect) {
      maze[mazepos].walls_neighbors = maze[mazepos].walls_neighbors | B10001000;
    }
    if (!front_detect) {
      maze[mazepos].walls_neighbors = maze[mazepos].walls_neighbors | B01000100;
    }
    if (!right_detect) {
      maze[mazepos].walls_neighbors = maze[mazepos].walls_neighbors | B00100010;
    }
  } else if (int(current.dir) == 2) { // south
    if (!left_detect) {
      maze[mazepos].walls_neighbors = maze[mazepos].walls_neighbors | B01000100;
    }
    if (!front_detect) {
      maze[mazepos].walls_neighbors = maze[mazepos].walls_neighbors | B00100010;
    }
    if (!right_detect) {
      maze[mazepos].walls_neighbors = maze[mazepos].walls_neighbors | B00010001;
    }
  } else { // west
    if (!left_detect) {
      maze[mazepos].walls_neighbors = maze[mazepos].walls_neighbors | B00100010;
    }
    if (!front_detect) {
      maze[mazepos].walls_neighbors = maze[mazepos].walls_neighbors | B00010001;
    }
    if (!right_detect) {
      maze[mazepos].walls_neighbors = maze[mazepos].walls_neighbors | B10001000;
    }
  }
}

void miniWalk() {
  // Walkback used if we detect a robot during a walk back

  byte loc = current.pos;
  byte dir = current.dir;
  byte path[3]; // Create small path for us to move to in order to avoid robots

  int i = 0;
  while (i < 3) { // Find 3 open squares and move there, adding them to path
    int left = digitalRead(left_ir_sensor);
    int right = digitalRead(right_ir_sensor);
    int front = digitalRead(front_ir_sensor);
    if (front && i > 0) { // Don't want to move straight when i=0 because that's where the robot is!
      if (current.dir == B00001000) {
        movetoLocation2(current.pos + 1);
      } else if (current.dir == B00000100) {
        movetoLocation2(current.pos + 16);
      } else if (current.dir == B00000010) {
        movetoLocation2(current.pos - 1);
      } else {
        movetoLocation2(current.pos - 16);
      }
    } else if (left) {
      if (current.dir == B00001000) {
        movetoLocation2(current.pos - 16);
      } else if (current.dir == B00000100) {
        movetoLocation2(current.pos + 1);
      } else if (current.dir == B00000010) {
        movetoLocation2(current.pos + 16);
      } else {
        movetoLocation2(current.pos - 1);
      }
    } else if (right) {
      if (current.dir == B00001000) {
        movetoLocation2(current.pos + 16);
      } else if (current.dir == B00000100) {
        movetoLocation2(current.pos - 1);
      } else if (current.dir == B00000010) {
        movetoLocation2(current.pos - 16);
      } else {
        movetoLocation2(current.pos + 1);
      }
    } else {
      if (current.dir == B00001000) {
        movetoLocation2(current.pos - 1);
      } else if (current.dir == B00000100) {
        movetoLocation2(current.pos + 16);
      } else if (current.dir == B00000010) {
        movetoLocation2(current.pos + 1);
      } else {
        movetoLocation2(current.pos - 16);
      }
    }
    path[i] = current.pos; // Add new location to path
    i++;
  }
  i--; // Decrement i by 1 to start at i=2
  while (i > 0) { // Walk back the path
    movetoLocation2(path[i - 1]);
    current.pos = path[i - 1];
    i--;
  }
  movetoLocation2(loc); // return to original location where we detected another robot

  // Figure out how to turn the fastest
  int curr_dir = current.dir;
  int u = 1;
  for (u; u < 4; u++) {
    curr_dir = curr_dir << 1;
    if (curr_dir > 8) {
      curr_dir = B00000001;
    }
    if (curr_dir == dir) {
      break;
    }
  }

  // Actually turn to face that direction
  if (u == 1) {
    left90Turn();
  } else if (u == 2) {
    right180Turn();
  } else if (u == 3) {
    right90Turn();
  }
  current.dir = dir;
}


void walkBack() {
  byte to_return_dir = maze[int(current.pos & B00001111) * 9 + int(current.pos >> 4)].vs_came & B00001111;
  byte to_return_pos;
  byte face;

  // Find location to walk back to (and the required direction to face in order to get there)
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

  // Figure out how to turn the fastest
  int curr_dir = current.dir;
  int i = 1;
  for (i; i < 4; i++) {
    curr_dir = curr_dir << 1;
    if (curr_dir > 8) {
      curr_dir = B00000001;
    }
    if (curr_dir == face) {
      break;
    }
  }

  // Actually turn to face that direction
  if (i == 1) {
    left90Turn();
  } else if (i == 2) {
    right180Turn();
  } else if (i == 3) {
    right90Turn();
  }
  current.dir = face;

  if (detect_robots()) { // if we detect a robot in a walk back, normal robot detection code won't work so go to special avoidance mechanism
    robot_detected = 1;
    miniWalk();
  } else { // else, just move to walk back location
    int go_on = 0;
    while ( go_on != 1 ) { // Want to get to next intersection (but to the last place in the path)
      go_on = navigate();
    }
    current.pos = to_return_pos;
  }
}

int determineDone() { // For determining when to turn on done LED
  for (int i = 0; i < maze_size; i++) {
    if (((maze[i].walls_neighbors & B00001111) != 15) && (maze[i].vs_came & B10000000) >> 7 == 1) { // if there are still open spots to move to from a location that has already been visited
      return 0;
    }
  }
  return 1;
}

void determineNav( byte location ) { // For updating neighbors of location, marking location as unnavigable (don't want repeats)
  int n = int(location & B00001111) + 1;
  int e = (int(location) >> 4) + 1;
  int s = int(location & B00001111) - 1;
  int w = (int(location) >> 4) - 1;
  if (n >= 0 && n < 9) { // if north is in-bounds, mark south as unnavigable
    maze[int((location + 1) & B00001111) * 9 + int((location + 1) >> 4)].walls_neighbors = maze[int((location + 1) & B00001111) * 9 + int((location + 1) >> 4)].walls_neighbors | B00000010;
  }
  if (e >= 0 && e < 9) { // if east is in-bounds, mark west as unnavigable
    maze[int((location + 16) & B00001111) * 9 + int((location + 16) >> 4)].walls_neighbors = maze[int((location + 16) & B00001111) * 9 + int((location + 16) >> 4)].walls_neighbors | B00000001;
  }
  if (s >= 0 && s < 9) { // if south is in-bounds, mark north as unnavigable
    maze[int((location - 1) & B00001111) * 9 + int((location - 1) >> 4)].walls_neighbors = maze[int((location - 1) & B00001111) * 9 + int((location - 1) >> 4)].walls_neighbors | B00001000;
  }
  if (w >= 0 && w < 9) { // if west is in-bounds, , mark east as unnavigable
    maze[int((location - 16) & B00001111) * 9 + int((location - 16) >> 4)].walls_neighbors = maze[int((location - 16) & B00001111) * 9 + int((location - 16) >> 4)].walls_neighbors | B00000100;
  }
}


// =====================================================================================================

// dfs main

// =====================================================================================================

void dfs() {
  byte location = current.pos;

  determineWalls(location); // determine the walls around the current location

  communicate(); // transmit info
  maze[int(location & B00001111) * 9 + int(location >> 4)].vs_came = maze[int(location & B00001111) * 9 + int(location >> 4)].vs_came | B01000000; // set walls sent to 1

  determineNav(location); // set all neighbors such that location is now unnavigable (to avoid repeats)

  if (determineDone()) { // if we've navigated everywhere we can, turn the done LED on.
    digitalWrite(DONE_LED, HIGH);
  }

  int wemoved = 1;
  if ((maze[int(location & B00001111) * 9 + int(location >> 4)].walls_neighbors & B00001111) != 15) { // if everywhere isn't unnavigable around location

    // Now move based on movement priority: try in front of us, then to left, then to right, then behind

    if (int(current.dir) == 8) { // facing north
      if (!moveNorth()) {
        if (!moveWest()) {
          if (!moveEast()) {
            wemoved = moveSouth();
          }
        }
      }
    } else if (int(current.dir) == 4) { // facing east
      if (!moveEast()) {
        if (!moveNorth()) {
          if (!moveSouth()) {
            wemoved = moveWest();
          }
        }
      }
    } else if (int(current.dir) == 2) { // facing south
      if (!moveSouth()) {
        if (!moveEast()) {
          if (!moveWest()) {
            wemoved = moveNorth();
          }
        }
      }
    } else { // facing west
      if (!moveWest()) {
        if (!moveSouth()) {
          if (!moveNorth()) {
            wemoved = moveEast();
          }
        }
      }
    }
  } else { // if all neighbors are unnavigable
    wemoved = 0;
  }

  if (!wemoved && current.pos != 0) { // if we didn't move, i.e. all neighbors have been visited and/or have walls, and we aren't at the start
    walkBack();
  } else if (!wemoved && current.pos == 0 && current.dir != 8) { // if we didn't move and we're at the start and we aren't facing north

    // Figure out how to turn the fastest
    int i = 1;
    int curr_dir = current.dir;
    for (i; i < 4; i++) {
      curr_dir = curr_dir << 1;
      if (curr_dir > 8) {
        curr_dir = B00000001;
      }
      if (curr_dir == B00001000) {
        break;
      }
    }

    // Actually turn to face north
    if (i == 1) {
      left90Turn();
    } else if (i == 2) {
      right180Turn();
    } else if (i == 3) {
      right90Turn();
    }
    current.dir = B00001000;
  }
}

// =====================================================================================================

// setup

// =====================================================================================================

void setup() {
  // line sensors
  pinMode(LEFT_LINE_SENSOR, INPUT);
  pinMode(RIGHT_LINE_SENSOR, INPUT);

  // wall sensors
  pinMode(left_ir_sensor, INPUT);
  pinMode(front_ir_sensor, INPUT);
  pinMode(right_ir_sensor, INPUT);

  // ir detectors
  pinMode(center_robot_detect, INPUT);

  // button/led
  pinMode(START_BUTTON, INPUT);
  pinMode(DONE_LED, OUTPUT);

  // serial and print setup
  Serial.begin(57600);
  printf_begin();

  // servo setup
  right.attach(right_pin);
  left.attach(left_pin);

  // initial state setup
  halt();
  digitalWrite(DONE_LED, LOW);
  beginning = 1; // to stop FFT/button press search
  ending = 0; // to stop infinitely many DFS calls if we're already done and at (0,0)
  current = { B00000000, B00001000 }; // set starting information to be at (0,0), facing north
  maze[0].vs_came = B10000000; // mark (0,0) as visited
  maze[0].walls_neighbors = B00100010; // Mark back wall as unnavigable
  for (int i = 1; i < maze_size; i++) { // initialize all other locations to be not visited with all neighbors open
    maze[i].vs_came = B00000000;
    maze[i].walls_neighbors = B00000000;
  }

  // =====================================================================================
  //
  // communication setup (copied from sample radio code from course website
  //
  // =====================================================================================

  // setup and configure rf radio
  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15, 15);
  radio.setAutoAck(true);
  // set the channel
  radio.setChannel(0x50);
  // set the power
  // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  radio.setPALevel(RF24_PA_HIGH);
  //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_250KBPS);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  radio.setPayloadSize(2);

  // Open pipes to other nodes for communication

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1, pipes[0]);

  // Start listening
  radio.startListening();

  // Dump the configuration of the rf unit for debugging
  radio.printDetails();
}

// =====================================================================================================

// main loop

// =====================================================================================================

int stopped[3] = {0, 0, 0}; // to help stop calling dfs once we return to the start and are done

void loop() {
  if (beginning) { // wait for pushbutton/950 Hz tone
    if (fftboi()) {
      dfs();
      beginning = 0; // stop calling fft if 950 Hz detected or button pressed
    }
  }
  else if (!ending) { // to make sure we don't keep doing stuff after we finish
    dfs();
    stopped[0] = stopped[1]; // shift element over
    stopped[1] = stopped[2]; // shift element over
    if (int(current.pos) == 0) { // fill in new opening
      stopped[2] = 1;
      if (stopped[0] == 1 && stopped[1] == 1) {
        ending = 1; // if all three elements are 1, we've been at (0,0) for three dfs calls in a row (a.k.a. we're done)
      }
    } else {
      stopped[2] = 0;
    }
  } else { // Permanently halt at (0,0) when done.
    halt();
  }
}
