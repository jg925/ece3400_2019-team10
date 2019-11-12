#include <StackArray.h>
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
int right_ir_sensor = 2;
int left_ir_sensor = 3;
int front_ir_sensor = 4;

// IR Robot Detect Pins
int left_robot_detect = A3;
int center_robot_detect = A2;
int right_robot_detect = A1;

int left_pi_arr[10];
int center_pi_arr[10];
int right_pi_arr[10];
int leftcount = 0;
int centercount = 0;
int rightcount = 0;
int robot_threshold = 75;
int count_threshold = 5;

// boundary between "white" and "black"
int line_threshold = 650;

// initialization of line sensor values
int left_sensor_value = 0;
int right_sensor_value = 0;

// initialization of IR sensor values
int right_detect;
int left_detect;
int front_detect;

// initialize green "done LED" and push button "start button"
int DONE_LED = 12;
int START_BUTTON = 8;

// initialize "beginning" which waits for button press or 950 Hz signal and "ending" which goes HIGH when we finished traversing the maze
int beginning;
int ending;

// initialization of Stack path, dfs stuff

enum direction {
  north,
  east,
  south,
  west
};

struct node {
  byte pos;            // FIRST FOUR BITS ARE X, LAST FOUR BITS ARE Y
  direction dir;
};

struct box {
  byte visited;
  byte walls;
};

StackArray <byte> path;
box maze[100];
node current;

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


void dfs( byte location ) { // NOTE: location must be an open location for us to go to and we must be facing that location.

  int go_on = 0;
  while ( go_on != 1 ) { // Want to navigate to next intersection at location
    go_on = navigate();
  }
  halt(); // Just so we don't go anywhere ;)

  current.pos = location; // Our current location is now location, facing same direction as we were at the time of calling this function
  path.push(location); // Add current location to the path
  maze[int(location)].visited = B00000001; // Mark location as visited

  // Properly assign wall bits (same as old DFS)

  maze[int(location)].walls = maze[int(location)].walls << 1;
  left_detect = digitalRead(left_ir_sensor);  // 0 when detecting
  if ( !(left_detect) ) {
    maze[int(location)].walls = maze[int(location)].walls + B001;
  }

  maze[int(location)].walls = maze[int(location)].walls << 1;
  front_detect = digitalRead(front_ir_sensor);  // 0 when detecting
  if ( !(front_detect) ) {
    maze[int(location)].walls = maze[int(location)].walls + B001;
  }

  maze[int(location)].walls = maze[int(location)].walls << 1;
  right_detect = digitalRead(right_ir_sensor);  // 0 when detecting
  if ( !(right_detect) ) {
    maze[int(location)].walls = maze[int(location)].walls + B001;
  }

  // determine locations in maze array (will be used to check what's been visited already)

  byte locfront;
  byte locleft;
  byte locright;

  if (current.dir == north) {
    locfront = byte( int(location) + 1);  // y+1
    locleft =  byte( int(location) - 16); // x-1
    locright = byte( int(location) + 16); // x+1
  } else if (current.dir == east) {
    locfront = byte( int(location) + 16); // x+1
    locleft =  byte( int(location) + 1);  // y+1;
    locright = byte( int(location) - 1);  // y-1;
  } else if (current.dir == south) {
    locfront = byte( int(location) - 1);  // y-1
    locleft =  byte( int(location) + 16); // x+1
    locright = byte( int(location) - 16); // x-1
  } else { // current.dir == west
    locfront = byte( int(location) - 16); // x-1
    locleft =  byte( int(location) - 1);  // y-1;
    locright = byte( int(location) + 1);  // y+1;
  }
  //Serial.println(current.pos);
  //Serial.println(current.dir);
  //Serial.println(locfront);

  direction curr_direct = current.dir;
  Serial.println("POSITION: ");
  Serial.println(current.pos, BIN);
  Serial.println("DIRECTION: ");
  Serial.println(current.dir);

  Serial.println("walls");
  Serial.println(maze[int(current.pos)].walls, BIN);
  
  // if there's no wall in front and the location above has not been visited (and it's not out of the maze; sanity check)
  if ( front_detect && ( int(maze[int(locfront)].visited) != 1 ) && checkRange(locfront)) {
    dfs(locfront);
  }
  Serial.println("POSITION: ");
  Serial.println(current.pos, BIN);
  Serial.println("DIRECTION: ");
  Serial.println(current.dir);

  Serial.println("walls");
  Serial.println(maze[int(current.pos)].walls, BIN);
  
  // if there's no wall to left and the location to left has not been visited (and it's not out of the maze; sanity check)
  Serial.println("Locleft:");
  Serial.println(locleft);
  Serial.println("Locright:");
  Serial.println(locright);
  
  if (current.dir != curr_direct) { // coming in from opposite direction
    right_detect = digitalRead(right_ir_sensor); // 0 when detecting
    if ( right_detect  && ( int(maze[int(locleft)].visited) != 1 ) && checkRange(locleft) ) {
      right90Turn();
      current.dir = direction( (int(current.dir) + 1) % 4 ); // % doesn't work for negatives
      dfs(locleft);
    }
    // if there's no wall to right and the location to right has not been visited (and it
  } else {
    left_detect = digitalRead(left_ir_sensor);  // 0 when detecting
    if ( left_detect  && ( int(maze[int(locleft)].visited) != 1 ) && checkRange(locleft) ) {
      left90Turn();
      current.dir = direction( (int(current.dir) - 1) ); // % doesn't work for negatives
      if (current.dir < 0) {
        current.dir = direction(current.dir + 4);
      }
      dfs(locleft);
    }
  }
  //Serial.println("Locright:");

  //  Serial.println(locright);
  Serial.println("POSITION: ");
  Serial.println(current.pos, BIN);
  Serial.println("DIRECTION: ");
  Serial.println(current.dir);

  Serial.println("walls");
  Serial.println(maze[int(current.pos)].walls, BIN);
  
  // if there's no wall to right and the location to right has not been visited (and it's not out of the maze; sanity check)
  if (current.dir != curr_direct) {
    int dir_calc = current.dir - curr_direct;
    if (dir_calc < 0) {
      dir_calc = dir_calc + 4;
    }
    if (dir_calc == 1) {
      //    Serial.println("in 1");
      front_detect = digitalRead(front_ir_sensor);
      if ( front_detect && ( int(maze[int(locright)].visited) != 1 ) && checkRange(locright)) {
        dfs(locright);
      }
    } else if ( dir_calc == 2 ) {
      //  Serial.println("in 2");

      left_detect = digitalRead(left_ir_sensor);
      if ( left_detect  && ( int(maze[int(locright)].visited) != 1 ) && checkRange(locright) ) {
        left90Turn();
        current.dir = direction( (int(current.dir) - 1) % 4 ); // % doesn't work for negatives
        if (current.dir < 0) {
          current.dir = current.dir + 4;
        }
        dfs(locright);
      }
    }
  } else {
    right_detect = digitalRead(right_ir_sensor);  // 0 when detecting
    if ( right_detect && ( int(maze[int(locright)].visited) != 1 ) && checkRange(locright) ) {
      right90Turn();
      current.dir = direction( (int(current.dir) + 1) % 4 );
      dfs(locright);
    }
  }

  // All paths have now been explored

/*
  for (int i = 0; i < 2; i++) { // WARNING: PLEASE STEP A SAFE DISTANCE AWAY FROM THE ROBOT ;)
    digitalWrite(DONE_LED, HIGH);
    delay(500);
    digitalWrite(DONE_LED, LOW);
    delay(500);
  }
*/

  path.pop();
  if (!(path.isEmpty())) { // If we have somewhere to walk back to
    byte return_to = path.peek(); // find where we need to go back to but don't pop it in case it has other neighbors to visit
    direction face;

    if ( (int(current.pos >> 4) - int(return_to >> 4)) == 1) { // if current_x - return_to_x == 1, want to face west
      face = west;
    }
    else if ( (int(current.pos >> 4) - int(return_to >> 4))  == -1 ) { // if current_x - return_to_x == -1, want to face east
      face = east;
    }
    else if ( (int(current.pos & B00001111) - int(return_to & B00001111)) == 1 ) { // if current_y - return_to_y == 1, want to face south
      face = south;
    }
    else if ( (int(current.pos & B00001111) - int(return_to & B00001111)) == -1) { // if current_y - return_to_y == 1, want to face north
      face = north;
    }

    // Turn to face, update current direction - need tempa and tempb because % doesn't work for negatives apparently : /

    int temp = current.dir - face;
    if (temp < 0) {
      temp = temp + 4;
    }

    if ( temp == 1 ) {
      left90Turn();
    } else if ( temp == 2 ) {
      right180Turn();
    } else if ( temp == 3 ) {
      right90Turn();
    } //else keep on keepin' on! :)
    current.dir = face;

    // Navigate to return_to, update current position

    go_on = 0;
    while ( go_on != 1 ) { // Want to get to next intersection (but to the last place in the path)
      go_on = navigate();
    }
    halt(); // Just so we don't go anywhere ;)
    current.pos = return_to;
  }
  Serial.println("POSITION: ");
  Serial.println(current.pos, BIN);
  Serial.println("DIRECTION: ");
  Serial.println(current.dir);
}

void setup() {
  pinMode(LEFT_LINE_SENSOR, INPUT);
  pinMode(RIGHT_LINE_SENSOR, INPUT);
  pinMode(left_ir_sensor, INPUT);
  pinMode(front_ir_sensor, INPUT);
  pinMode(right_ir_sensor, INPUT);
  pinMode(left_robot_detect, INPUT);
  pinMode(center_robot_detect, INPUT);
  pinMode(right_robot_detect, INPUT);
  pinMode(START_BUTTON, INPUT);
  pinMode(DONE_LED, OUTPUT);
  Serial.begin(9600);
  right.attach(right_pin);
  left.attach(left_pin);
  halt();
  digitalWrite(DONE_LED, LOW);
  beginning = 1;
  ending = 0;
  current = { B00000000, north };
  for (int i = 0; i < 100; i++) {
    maze[i].visited = B00000000; // initialize all locations to be not visited
    maze[i].walls = B00000111;  // initialize all walls to be 111
  }
  maze[0].visited = B00000001;
}

void loop() {
  while (beginning) { // to wait for pushbutton/950 Hz tone
    if (digitalRead(START_BUTTON)) {
      beginning = 0;
      for (int i = 0; i < 2; i++) { // WARNING: PLEASE STEP A SAFE DISTANCE AWAY FROM THE ROBOT ;)
        digitalWrite(DONE_LED, HIGH);
        delay(500);
        digitalWrite(DONE_LED, LOW);
        delay(500);
      }
    }
  }
  if (!(ending)) { // to make sure we don't keep doing stuff after we finish
    path.push(current.pos);
    front_detect = digitalRead(front_ir_sensor);
    if (front_detect) {
      dfs( byte( int(current.pos) + 1 ) ); // bits are 76543210, this will increment bit 0 by 1
    }
    if (current.dir == east) {
      left90Turn();
    } else if (current.dir == south) {
      right180Turn();
    } else if (current.dir == west) {
      right90Turn();
    } //else we're facing the right direction!
    current.dir = north;
    right_detect = digitalRead(right_ir_sensor);
    if (right_detect && maze[int(B00010000)].visited == B00000000) {
      right90Turn();
      current.dir = east;
      dfs( byte( int(current.pos) + 16 ) ); // bits are 76543210, this will increment bit 4 by 1
      right90Turn();
    }
    halt();
    ending = 1; // We're done; switch ending to 1 so we don't keep doing stuffs
    digitalWrite(DONE_LED, HIGH); // Victory LED
  }
}
