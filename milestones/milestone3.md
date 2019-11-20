# Milestone 3: Avoiding Robots and Traversing Mazes

## Introduction

This milestone involved having our robot successfully avoid other robots and explore a unique 4x5 maze with some algorithm such as DFS, BFS, Dijkstra, or A*. For this milestone, we relied on the Arduino code and hardware that we set up in previous labs throughout the semester, specifically line following and wall detection. In order to detect other robots, we tested different IR emitters and detectors and eventually chose to go with the wider angle detectors and the shorter ranged emitters, mounting three detectors and four emitters at exactly 5 inches off the ground. For maze exploration, we decided to implement a DFS algorithm which involved utilizing stacks and recursion.

## Robot Detection



<p align="center">
  <iframe width="560" height="315" src="https://www.youtube.com/embed/Ns-UvfSCByA" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

<p align="center">
  <iframe width="560" height="315" src="https://www.youtube.com/embed/A8EQLZdOH7E" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
  <iframe width="560" height="315" src="https://www.youtube.com/embed/fa2VKi1fIuM" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

## Maze Exploration

In this section of the milestone, we implemented a depth-first search in order to successfully traverse a 4x5 maze. In order to implement this algorithm, we used the StackArray Arduino library

```c
// This program allows the robot to nagivate any maze using a DFS

... // Declarations

... // Moving commands (i.e. forward, turn left/right/around, 
... // slight left/right to correct during straightaways, etc.)

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

// NOTE: location must be an open location for 
// us to go to and we must be facing that location.
void dfs( byte location ) { 

  movetoLocation(location); // move to the open location using same navigation code as Milestone 2
  determineWalls(location); // Determine what paths are open

  // determine locations in maze array (will be used to check what's been visited already)
  
  byte locfront;
  byte locleft;
  byte locright;

  if (current.dir == north) {
    locfront = byte( int(location) + 1);  // y+1
    locleft =  byte( int(location) - 16); // x-1
    locright = byte( int(location) + 16); // x+1
  } // Similar for other three directions but excluded here for brevity.

  direction curr_direct = current.dir; // Save initial direction
  
  // if there's no wall in front and the location above has not been visited 
  // (and it's not out of the maze; sanity check)
  
  if ( front_detect && ( int(maze[int(locfront)].visited) != 1 ) && checkRange(locfront)) {
    dfs(locfront);
  }
  
  // if there's no wall to left and the location to left has not been visited 
  // (and it's not out of the maze; sanity check)
  
  if (current.dir != curr_direct) { // coming in from opposite direction
    left_detect = digitalRead(right_ir_sensor); // 0 when detecting
  } else {
    left_detect = digitalRead(left_ir_sensor); // 0 when detecting
  }
  
  if (left_detect && ( int(maze[int(locleft)].visited) != 1 ) && checkRange(locleft) ) {
    if (current.dir != curr_direct) {
      right90Turn();
      current.dir = direction( (int(current.dir) + 1) % 4 ); // % doesn't work for negatives
    } else {
      left90Turn();
      current.dir = direction( (int(current.dir) - 1) ); // % doesn't work for negatives
      if (current.dir < 0) {
        current.dir = direction(current.dir + 4);
      }
    }
    dfs(locleft);
  }
  
  // if there's no wall to right and the location to right has not been visited
  // (and it's not out of the maze; sanity check)
  
  int dir_calc = current.dir - curr_direct;
  if (dir_calc < 0) {
      dir_calc = dir_calc + 4;
  }
  
  if (dir_calc == 1) {
    right_detect = digitalRead(front_ir_sensor);
  } else if (dir_calc == 2) {
    right_detect = digitalRead(left_ir_sensor);
  } else {
    right_detect = digitalRead(right_ir_sensor);
  }
  
  if ( right_detect && ( int(maze[int(locright)].visited) != 1 ) && checkRange(locright)) {
    if (dir_calc == 0) {
      right90Turn();
      current.dir = direction( (int(current.dir) + 1) % 4 );
    } else if (dir_calc == 2) {
      left90Turn();
      current.dir = direction( (int(current.dir) - 1) % 4 ); // % doesn't work for negatives
      if (current.dir < 0) {
        current.dir = direction(int(current.dir) + 4);
      }
    } // if dir_calc == 1, we are already facing the correct direction
    dfs(locright);
  }
  
  // All paths have now been explored

  path.pop();
  if (!(path.isEmpty())) { // If we have somewhere to walk back to
    walkBack();
  }
}

void setup() {
 ... // Pin Setup stuff (excluded for brevity)
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
  while (beginning) { // to wait for pushbutton
    if (digitalRead(START_BUTTON)) {
      beginning = 0;
    }
  }
  if (!(ending)) { // to make sure we don't keep doing stuff after we finish
    path.push(current.pos);
    front_detect = digitalRead(front_ir_sensor);
    if (front_detect) {
      dfs( byte( int(current.pos) + 1 ) ); // bits are 76543210, this will increment bit 0 by 1
    }
    if (current.dir == south) {
      left_detect = digitalRead(left_ir_sensor);
      if (left_detect && maze[int(B00010000)].visited == B00000000) {
        left90Turn();
        current.dir = east;
        dfs( byte( int(current.pos) + 16 ) ); // bits are 76543210, this will increment bit 4 by 1
        right90Turn();
      } else {
        right180Turn();
      }
    } //else we're facing north!
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
```

<p align="center">
  <iframe width="560" height="315" src="https://www.youtube.com/embed/UaM1cZHoPV0" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

## Conclusion

