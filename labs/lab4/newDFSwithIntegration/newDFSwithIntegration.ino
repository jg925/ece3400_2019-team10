#include <Servo.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

// =====================================================================================================

// communication: hardware setup

// =====================================================================================================

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9,10);

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
int left_sensor_value = 0;
int right_sensor_value = 0;

// =====================================================================================================

// robot detection variables inits

// =====================================================================================================

// phototransistor pins
int left_robot_detect = A3;
int center_robot_detect = A2;
int right_robot_detect = A1;

// ir robot detection variables
int left_pi_arr[10];
int center_pi_arr[10];
int right_pi_arr[10];
int robot_threshold = 100;
int sum = 0;
int avg = 0;

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
  byte visited_sent; // v000xxxx, tells if visited with v bit, xxxx tells which direction we came from.
  byte neighbors; // s000xxxx, tells if the walls were sent with the s bit which locations are free to be moved to.
                  // 1111 means everywhere has been traversed or there are walls (a.k.a. return).
};

box maze[81]; // 9x9
node current;

// =====================================================================================================

// fft variables inits

// =====================================================================================================

#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

// #include <FFT.h> // include the library UNCOMMENT THIS LINE TO INCLUDE FFT LIBRARY

int detect_count = 0;
int flag_950 = 0;

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
    // Serial.println("LEFT ON WHITE");
    while (left_sensor_value < line_threshold && right_sensor_value >= line_threshold) {
      slightLeft();
      left_sensor_value = analogRead(LEFT_LINE_SENSOR);
      right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
    }
  }

  // right on white, left on black --> slight right adjust
  else if (left_sensor_value >= line_threshold && right_sensor_value < line_threshold) {
    // left detects black, right detects white
    //Serial.println("RIGHT ON WHITE");
    while (right_sensor_value < line_threshold && left_sensor_value >= line_threshold ) {
      slightRight();
      left_sensor_value = analogRead(LEFT_LINE_SENSOR);
      right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
    }
  }

  // both sensors detect black --> go straight
  else if (left_sensor_value >= line_threshold && right_sensor_value >= line_threshold) {
    // Serial.println("ALL BLACK");
    moveForward();
  }
  return 0;
}

// =====================================================================================================

// robot detection

// =====================================================================================================

void detect_robots() {
  // takes a running average of 10 inputs
  
  int left_input = analogRead(left_robot_detect);
  left_pi_arr[9] = left_input;

  // updates array for left phototransistor
  for (int i= 1; i < 10; i++) {
    left_pi_arr[i-1] = left_pi_arr[i];
  }

  // sums sample
  sum = 0;
  Serial.println("ARRAY START");
  for (int i= 0; i < 10; i++) {
    Serial.println(left_pi_arr[i]);
    sum += left_pi_arr[i];
  }
  Serial.println("ARRAY END");

  // averages sample
  avg = sum/10;
  Serial.println("AVERAGE");
  Serial.println(avg);

  if (avg > robot_threshold) {
    Serial.println("DETECT");
    //digitalWrite(robot_LED_pin, HIGH);
    right180Turn();
  } else {
    Serial.println("ELSE");
    //digitalWrite(robot_LED_pin, LOW);
    navigate();
  }
}

// =====================================================================================================

// fft helper

// =====================================================================================================

int is_maximum( int five, int six, int seven, int eight, int FFT_threshold ) {
  if ( six > FFT_threshold && seven > FFT_threshold ) {
    //if ( six > five && six > eight && seven > five && seven > eight ) {
      // checking that six and seven are a local maximum
      //if ( six - seven < 10 && six - five > 10 && six - seven > 0) {
        // checking that shape of curve is correct
        return 1;
      //}
    //}
  }
  else {
    return 0;
  }
}

// =====================================================================================================

// fft main

// =====================================================================================================


//void fft() {
//  while (1) { // reduces jitter
//    cli();  // UDRE interrupt slows this way down on arduino1.0
//    for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
//      while (!(ADCSRA & 0x10)); // wait for adc to be ready
//      ADCSRA = 0xf5; // restart adc
//      byte m = ADCL; // fetch adc data
//      byte j = ADCH;
//      int k = (j << 8) | m; // form into an int
//      k -= 0x0200; // form into a signed int
//      k <<= 6; // form into a 16b signed int
//      fft_input[i] = k; // put real data into even bins
//      fft_input[i + 1] = 0; // set odd bins to 0
//    }
//    fft_window(); // window the data for better frequency response
//    fft_reorder(); // reorder the data before doing the fft
//    fft_run(); // process the data in the fft
//    fft_mag_log(); // take the output of the fft
//    sei();
//    int max = is_maximum( fft_log_out[5], fft_log_out[6], fft_log_out[7], fft_log_out[8], 124 );
//    if (max == 1 && detect_count >= 6)
//    {
//      Serial.println("950 Hz");
//      flag_950 = 1;
//      detect_count = 0;
//      break;
//      //Serial.println(detect_count);
//    }
//    else if (digitalRead(START_BUTTON)) {
//      Serial.println("Button Press");
//      break;
//    }
//    else if ( max == 1 ) {
//      detect_count++;
//    }
//  }
//}

// =====================================================================================================

// communication

// =====================================================================================================

void communicate() {
  // 0rvswwwwxxxxyyyy 

  // zeroes out msg
  uint16_t msg = 0000000000000000;

  // robot detection bit
  msg = (msg << 1) | 1;
  // visited node bit
  msg = (msg << 1) | (maze[int(current.pos)].visited_sent >> 7);
  // walls already sent bit; NOTE: IF THIS IS 1, DO NOT DRAW THE WALLS AGAIN
  msg = (msg << 1) | (maze[int(current.pos)].neighbors >> 7);
  // walls bits
  msg = (msg << 4) | ((maze[int(current.pos)].neighbors << 4) >> 4);
  // x pos bits
  msg = (msg << 4) | (current.pos >> 4);
  // y pos bits
  msg = (msg << 4) | ((current.pos << 4) >> 4);

//  printf("\ncoord: ");
//  printf("%d", x);
//  printf(", %d \n", y);
//  printf("\nmsg: ");
//  printf("%x", msg);
//  printf("\n");

  // First, stop listening so we can talk.
  radio.stopListening();

  //printf("Now sending %x...",msg);
  bool ok = radio.write( &msg, sizeof(uint16_t) );

//  if (ok)
//    printf("ok...");
//  else
//    printf("failed.\n\r");

  // Now, continue listening
  radio.startListening();

  // Wait here until we get a response, or timeout (250ms)
  unsigned long started_waiting_at = millis();
  
  bool timeout = false;
  while ( ! radio.available() && ! timeout )
    if (millis() - started_waiting_at > 200 )
      timeout = true;

  // Describe the results
//  if ( timeout )
//  {
//    printf("Failed, response timed out.\n\r");
//  }
//  else
//  {
//    // Grab the response, compare, and send to debugging spew
//    byte got_msg;
//    radio.read( &got_msg, sizeof(uint16_t) );
//
//    // Spew it
//    printf("Got response %x \n\r",msg);
//    
//  }

  // Try again 1s later
  //delay(1000);
}

// =====================================================================================================

// dfs helpers      FIX DFS!!! DO NOT USE STACK ARRAYS OR RECURSION

// =====================================================================================================

bool checkRange(byte location) {
  int temp1 = int(location) >> 4; //x
  if (temp1 < 0 || temp1 >= 10) {
    return false;
  }
  int temp2 = int(location & B00001111); //y
  if (temp2 < 0 || temp2 >= 10) {
    return false;
  }
  return true;
}

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

  byte original_dir = current.dir;

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
  //halt(); // Just so we don't go anywhere ;)

  current.pos = location; // Our current location is now location, facing same direction as we were at the time of calling this function
  maze[int(location)].visited_sent = B10000000 | original_dir; // Mark location as visited and update "came" direction
}

int moveNorth() {
  byte location = current.pos;
  if (int(((location << 4) >> 4) + B00000001) < 9) { // if y-to-be < 9
    if ( (int((maze[int(location)].neighbors << 4) >> 7) == 0) && ((maze[int(location + B00000001)].visited_sent >> 7) == 0) ) { // free to move north
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
    if ( (int((maze[int(location)].neighbors << 5) >> 7) == 0) && ((maze[int(location + B00010000)].visited_sent >> 7) == 0) ) { // free to move east
      movetoLocation(location + B00010000);
      return 1;
    }
    return 0;
  }
  return 0;
}

int moveSouth() {
  byte location = current.pos;
  if (int(((location << 4) >> 4) - B00000001) >= 0) { // if y-to-be >=0
    if ( (int((maze[int(location)].neighbors << 6) >> 7) == 0) && ((maze[int(location - B00000001)].visited_sent >> 7) == 0) ){ // free to move south
      movetoLocation(location - B00000001);
      return 1;
    }
    return 0;
  }
  return 0;
}

int moveWest() {
  byte location = current.pos;
  if (int((location >> 4) + B00000001) >= 0) { // if x-to-be >= 0
    if ( (int((maze[int(location)].neighbors << 7) >> 7) == 0) && ((maze[int(location - B00010000)].visited_sent >> 7) == 0) ) { // free to move west
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
  byte to_return_dir = (maze[int(current.pos)].visited_sent << 4) >> 4;
  byte to_return_pos;
  byte face;

  if (int(to_return_dir) == 8) {
    face = B00000010;
    to_return_pos = current.pos + B00000001;
  } else if (int(to_return_dir) == 4) {
    face = B00000001;
    to_return_pos = current.pos + B00010000;
  } else if (int(to_return_dir) == 2) {
    face = B00001000;
    to_return_pos = current.pos - B00000001;
  } else { // (int(to_return_dir) == 1)
    face = B00000100;
    to_return_pos = current.pos - B00010000;
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
  if (int(maze[int(location)].visited_sent >> 7) != 1) { // don't redo walls if we've already been here
    determineWalls(location);
  }

  // transmit info
  communicate();
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

  if (!wemoved) { // if we didn't move, i.e. all neighbors have been visited and/or have walls
    walkBack();
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
  pinMode(left_robot_detect, INPUT);
  pinMode(center_robot_detect, INPUT);
  pinMode(right_robot_detect, INPUT);

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
  beginning = 1;
  ending = 0;
  current = { B00000000, B00001000 }; // set at (0,0), facing north
  for (int i = 0; i < 81; i++) {
    maze[i].visited_sent = B00000000; // initialize all locations to be not visited
    maze[i].neighbors = B00000000;  // initialize all neighbors to be open
  }
  maze[0].visited_sent = B10000000; // mark (0,0) as visited

  //
  // fft setup
  //
  
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0

  //
  // communication setup
  //
  
  // setup and configure rf radio
  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
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
  radio.openReadingPipe(1,pipes[0]);
  
  // Start listening
  radio.startListening();

  // Dump the configuration of the rf unit for debugging
  radio.printDetails();
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
  
  if (!(ending)) { // to make sure we don't keep doing stuff after we finish
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
