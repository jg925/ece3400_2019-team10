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

// navigate
int navigate_flag;

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
    Serial.println("In navigate");
//    Serial.println("LEFTSENSOR");
//    Serial.println(left_sensor_value);
//    Serial.println("RIGHTSENSOR");
//    Serial.println(right_sensor_value);

  //else {
  //while (!(analogRead(LEFT_LINE_SENSOR) < threshold && analogRead(RIGHT_LINE_SENSOR) < threshold)) {
    //Serial.println("We be out here");
    left_sensor_value = analogRead(LEFT_LINE_SENSOR);
    right_sensor_value = analogRead(RIGHT_LINE_SENSOR);

    // left and right on white
    if (left_sensor_value < threshold && right_sensor_value < threshold) {
      navigate_flag = 0;

      halt();
      delay(1000);
      Serial.println("Finna halt");
      moveForward();
      delay(500);
    } else {

      left_sensor_value = analogRead(LEFT_LINE_SENSOR);
      right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
      
      // left on white, right on black --> slight left adjust
      if (left_sensor_value < threshold && right_sensor_value >= threshold) {
        // left detects white, right detects black
        navigate_flag = 1;
        
        while (left_sensor_value < threshold) {
          slightLeft();
          left_sensor_value = analogRead(LEFT_LINE_SENSOR);
        }
      }
  
      left_sensor_value = analogRead(LEFT_LINE_SENSOR);
      right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
      
      // right on white, left on black --> slight right adjust
      if (left_sensor_value >= threshold && right_sensor_value < threshold) {
        navigate_flag = 1;
        
        while (right_sensor_value < threshold) {
          slightRight();
          right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
        }
      }
  
      left_sensor_value = analogRead(LEFT_LINE_SENSOR);
      right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
      
      // both sensors detect black --> go straight
      if (left_sensor_value >= threshold && right_sensor_value >= threshold) {
       //halt();
       //delay(5000);
        //Serial.println("FORWARD");
  
        navigate_flag = 1;
        
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
    Serial.println("left");
  }
  current.walls = current.walls << 1;

  right_detect = digitalRead( right_ir_sensor );
  left_detect = digitalRead( left_ir_sensor );
  front_detect = digitalRead( front_ir_sensor );
  
  if ( !front_detect ) {
    current.walls = current.walls + B001;
    Serial.println("center");
  }
  current.walls = current.walls << 1;

  right_detect = digitalRead( right_ir_sensor );
  left_detect = digitalRead( left_ir_sensor );
  front_detect = digitalRead( front_ir_sensor );
  
  if ( !right_detect ) {
    current.walls = current.walls + B001;
    Serial.println("right");
  }

  Serial.println("WALLS: ");
  Serial.println(current.walls, BIN);

  int w;
  byte xtobe;
  byte ytobe;
  // left wall not detected
  if (  ( current.walls & B00000100 ) != B00000100 ) {
    Serial.println("ADD LEFT IN FRONTIER");
    if (current.dir == north) {
      xtobe = current.x - B1;
      ytobe = current.y;
    }
    else if (current.dir == east) {
      xtobe = current.x;
      ytobe = current.y + B1;
    }
    else if (current.dir == south) {
      xtobe = current.x + B1;
      ytobe = current.y;
    }
    else {
      xtobe = current.x;
      ytobe = current.y - B1;
    }
    nodesmall avail = { xtobe, ytobe };
    for (w = 0; w < visitedi; w++)
    {
      if ( avail.x == visited[w].x && avail.y == visited[w].y ) {
        break;
      }
    }
    if ( w == visitedi ) { // if not found in visited set, add to frontier
      current.frontier[0] = (byte)(int(avail.x << 4) + int(avail.y));
    }
  }

  // front wall not detected
  if ( ( current.walls & B00000010 ) != B00000010 ) {
    Serial.println("ADD FRONT IN FRONTIER");
    if (current.dir == north) {
      xtobe = current.x;
      ytobe = current.y + B1;
    }
    else if (current.dir == east) {
      xtobe = current.x + B1;
      ytobe = current.y;
    }
    else if (current.dir == south) {
      xtobe = current.x;
      ytobe = current.y - B1;
    }
    else {
      xtobe = current.x - B1;
      ytobe = current.y;
    }
//    Serial.println("YTOBE");
//    Serial.println(ytobe, BIN);
//    Serial.println("XTOBE");
//    Serial.println(xtobe, BIN);
    nodesmall avail = { xtobe, ytobe };
    for (w = 0; w < visitedi; w++)
    {
      if ( avail.x == visited[w].x && avail.y == visited[w].y ) {
        break;
      }
    }
    if ( w == visitedi ) { // if not found in visited set, add to frontier
//      Serial.println("AVAIL SHIFT");
//      Serial.println(avail.x<<4, BIN);
//      Serial.println("TEST SHIFT");
//      Serial.println(avail.y, BIN);
//      Serial.println(byte(int(avail.x << 4) + int(avail.y)), BIN);
      
      current.frontier[1] = (byte)(int(avail.x << 4) + int(avail.y));
    }
  }

  // right wall not detected
  if ( ( current.walls & B00000001 ) != B00000001 ) {
    Serial.println("ADD RIGHT IN FRONTIER");
    if (current.dir == north) {
      xtobe = current.x + B1;
      ytobe = current.y;
    }
    else if (current.dir == east) {
      xtobe = current.x;
      ytobe = current.y - B1;
    }
    else if (current.dir == south) {
      xtobe = current.x - B1;
      ytobe = current.y;
    }
    else {
      xtobe = current.x;
      ytobe = current.y + B1;
    }
    nodesmall avail = { xtobe, ytobe };
    for ( w = 0; w < visitedi; w++ )
    {
      if ( avail.x == visited[w].x && avail.y == visited[w].y ) {
        break;
      }
    }
    if ( w == visitedi ) { // if not found in visited set, add to frontier
      current.frontier[2] = (byte)(int(avail.x << 4) + int(avail.y));
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

  Serial.println("FRONTIER LIST");
  Serial.println(current.frontier[0], BIN);
  Serial.println(current.frontier[1], BIN);
  Serial.println(current.frontier[2], BIN);

  // Next direction, next place to move
  int q;
  byte nextx;
  byte nexty;
  direction face = current.dir;
  for ( q = 0; q < 3; q++ ) {
    int temp = clr[q];
    if ( current.frontier[temp] != B0 ) {
      Serial.println("NAVIGATE BOIIII");
      navigate_flag = 1;
      while (navigate_flag) {
        navigate();
      }
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
        Serial.println("Straight open; going straight");
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
        Serial.println("Left open; going left");
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
        Serial.println("Right open; going right");
      }
      break;
    }
  }
  if (q == 3) {
    // nextx and nexty --> node behind the robot
    byte return_to_x = path[pathi].x;
    byte return_to_y = path[pathi].y;

    if (current.x - return_to_x == 1) {
      face = west;
    }
    else if (current.x - return_to_x == -1) {
      face = east;
    }
    else if (current.y - return_to_y == 1) {
      face = south;
    }
    else if (current.y - return_to_y == -1) {
      face = north;
    }

    while (current.dir != face) {
      right90Turn();
      current.dir = direction( ( int( current.dir ) + 1 ) % 4 );
    }
    navigate_flag = 1;
    while (navigate_flag) {
      navigate();
    }
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
