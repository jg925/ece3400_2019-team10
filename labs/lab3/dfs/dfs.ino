#include <Servo.h>

Servo left;
Servo right;

// manual go
int button_pin = 7;

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

// boundary between "white" and "black"
int threshold = 650;

// initialization of line sensor values
int left_sensor_value = 0;
int right_sensor_value = 0;

// initialization of IR sensor values
int right_detect;
int left_detect;
int front_detect;

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

void navigate() {
  left_sensor_value = analogRead(LEFT_LINE_SENSOR);
  right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
//  Serial.println("LEFTSENSOR");
//  Serial.println(left_sensor_value);
//  Serial.println("RIGHTSENSOR");
//  Serial.println(right_sensor_value);

  // if both sensors on white
  if (left_sensor_value < threshold && right_sensor_value < threshold) {

    // to move into corresponding turn cases after debug
    halt();
    delay(1000);
//    Serial.println("check");
    moveForward();
    delay(500);
  }
  //else {
    while (!(analogRead(LEFT_LINE_SENSOR) < threshold && analogRead(RIGHT_LINE_SENSOR) < threshold)) {
    
      // left on white, right on black --> slight left adjust
      if (left_sensor_value < threshold && right_sensor_value >= threshold) {
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
  //}
}

enum direction {
  north,
  east,
  south,
  west
};

struct node {
  byte x;
  byte y;
  direction dir; // which way robot is facing
  byte walls; // whether there is a wall (1) or not (0), left-front-right
  byte frontier[3];
};

struct nodesmall {
  byte x;
  byte y;
};

node visited [100];
nodesmall path [100];
int visitedi = 0;
int pathi = 0;
node current;
node avail;
int clr[3] = { 1, 0, 2 };

void dfs() {
  // wall detection
  int right_detect = digitalRead( right_ir_sensor );
  int left_detect = digitalRead( left_ir_sensor );
  int front_detect = digitalRead( front_ir_sensor );

  current.walls = current.walls << 1;
  if ( !left_detect ) {
    current.walls = current.walls + B001;
  }
  current.walls = current.walls << 1;
  if ( !front_detect ) {
    current.walls = current.walls + B001;
  }
  current.walls = current.walls << 1;
  if ( !right_detect ) {
    current.walls = current.walls + B001;
  }
  
  int w;
  // left wall not detected
  if ( !( ( current.walls & B100 ) && B100 ) ) {
    nodesmall avail = { current.x - B1, current.y };
    for (w = 0; w < visitedi; w++)
    {
      if ( avail.x == visited[w].x && avail.y == visited[w].y ) {
        break;
      }
    }
    if ( w == visitedi ) { // if not found in visited set, add to frontier
      current.frontier[0] = avail.x << 4 + avail.y;
    }
  }

  // front wall not detected
  if ( !( ( current.walls & B010 ) && B010 ) ) {
    nodesmall avail = { current.x, current.y + B1 };
    for (w = 0; w < visitedi; w++)
    {
      if ( avail.x == visited[w].x && avail.y == visited[w].y ) {
        break;
      }
    }
    if ( w == visitedi ) { // if not found in visited set, add to frontier
      current.frontier[1] = avail.x << 4 + avail.y;
    }
  }

  // right wall not detected
  if ( !( ( current.walls & B001 ) && B001 ) ) {
    nodesmall avail = { current.x + B1, current.y };
    for ( w = 0; w < visitedi; w++ )
    {
      if ( avail.x == visited[w].x && avail.y == visited[w].y ) {
        break;
      }
    }
    if ( w == visitedi ) { // if not found in visited set, add to frontier
      current.frontier[2] = avail.x << 4 + avail.y;
    }
  }

  // Don't want to add repeats into visited node

  byte flag = B0;
  for ( int z = 0; z < visitedi; z++ ) {
    if ( visited[z].x == current.x && visited[z].y == current.y ) {
      flag = B1;
      break;
    }
  }
  if ( flag == B0 || visitedi == 0 ) {
    visited[visitedi] = current;
    visitedi++;
  }

  // Next direction, next place to move
  int q;
  byte nextx;
  byte nexty;
  direction face = current.dir;
  for ( q = 0; q < 3; q++ ) {
    int temp = clr[q];
    if ( current.frontier[temp] != B0 ) {

      if (temp == 1) {                     // open space in front of robot
        face = current.dir;
        if ( current.dir == north ) {
          nextx = current.x;
          nexty = current.y + B1;
        }
        else if ( current.dir == east) {
          nextx = current.x + B1;
          nexty = current.y;
        }
        else if ( current.dir == south ) {
          nextx = current.x;
          nexty = current.y - B1;
        }
        else {
          nextx = current.x - B1;
          nexty = current.y ;
        }
      }
      else if (temp == 0) {                // open space to left of robot
        face = direction( ( int( current.dir ) - 1 ) % 4 );
        if ( current.dir == north ) {
          nextx = current.x - B1;
          nexty = current.y;
        }
        else if ( current.dir == east) {
          nextx = current.x;
          nexty = current.y + B1;
        }
        else if ( current.dir == south ) {
          nextx = current.x + B1;
          nexty = current.y;
        }
        else {
          nextx = current.x;
          nexty = current.y - B1;
        }
        left90Turn();
      }
      else if (temp == 2) {                // open space to right of robot
        face = direction( ( int( current.dir ) + 1 ) % 4 );
        if ( current.dir == north ) {
          nextx = current.x + B1;
          nexty = current.y;
        }
        else if ( current.dir == east) {
          nextx = current.x;
          nexty = current.y - B1;
        }
        else if ( current.dir == south ) {
          nextx = current.x - B1;
          nexty = current.y;
        }
        else {
          nextx = current.x;
          nexty = current.y + B1;
        }
        right90Turn();
      }
      navigate();
      break;
    }
  }
  if (q == 3) {
    // nextx and nexty --> node behind the robot
    byte return_to_x = path[pathi].x;
    byte return_to_y = path[pathi].y;

    if (current.x-return_to_x == 1) {
      face = west;
    }
    else if (current.x-return_to_x == -1) {
      face = east;
    }
    else if (current.y-return_to_y == 1) {
      face = south;
    }
    else if (current.y-return_to_y == -1) {
      face = north;
    }

    while (current.dir != face) {
      right90Turn();
      current.dir = direction( ( int( current.dir ) + 1 ) % 4 );
    }
    navigate();
    nextx = return_to_x;
    nexty = return_to_y;
    pathi--;
  }
  else {
    path[pathi].x = current.x;
    path[pathi].y = current.y;
    pathi++;
  }

  current = { nextx, nexty, face, B111, { B0, B0, B0 } };
}

void setup() {
  Serial.begin( 9600 );

  pinMode( LEFT_LINE_SENSOR, INPUT );
  pinMode( RIGHT_LINE_SENSOR, INPUT );
  pinMode( left_ir_sensor, INPUT );
  pinMode( front_ir_sensor, INPUT );
  pinMode( right_ir_sensor, INPUT );
  pinMode( button_pin, INPUT);
  right.attach( right_pin );
  left.attach( left_pin );

  // init starting position
  current = { B0, B0, north, B111, { B0, B0, B0 } }; // Must use B111 because we can't shift in 1's.
}

void loop() {
  dfs();
}
