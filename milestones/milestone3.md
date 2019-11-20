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

In this section of the milestone, we implemented a depth-first search in order to successfully traverse a 4x5 maze. In order to implement this algorithm, we used the StackArray Arduino library. We created an enum called direction as well as a struct called node that keeps track of the robot's orientation and position. We used bytes to store the position to save space; the first four bits are for the x-coordinate and the last four are for the y-coordinate. We created another struct that we used to create a 100 element array to simulate the 10x10 maze. This node contains two bytes which tell if the location has been visited and where the walls are relative to where to robot entered the position. We initially set all the walls to be 00000111 in setup() because we use shifting to properly set the bits once we determine the locations using the wall sensors. When placing our robot into the corner of a maze, we always say it starts at (0,0) and that it is always facing north, so we always place it such that there are walls to the left and behind it as shown below. This greatly simplified the start-up code. 

<p align="center"><img src="https://github.coecis.cornell.edu/jg925/ece3400-2019-team10/blob/master/labs/lab3/images/RobotInit.png?raw=true" height="350" width="350"></p>

When loop() is called for the first time, we wait for the start button that we integrated to be pressed. Then we check if (0,1) is open and we perform a DFS on that location. Our DFS is a little more complicated than that of a "standard" DFS because we always have to take the robot's orientation into account. First, we move to the open location, push it onto the stack, and determine where the walls are, modifying the bytes in our maze array as needed. Then we determine the coordinates of the neighbors, save the current orientation, and look to recursively call DFS on the location in front of the robot if there isn't a wall, it isn't visited, and it's in the maze. Then, depending on the orientation after that, we read the appropriate wall sensor to check the location to the left of the original orientation, check the coordinate to see if it's navigable, and perform another DFS if possible. Finally, we execute a similar process for the location to the right of the original orientation. After all that, we pop the current location off the stack and return to the previous location (if there is a location to return to). After the DFS to (0,1) fully completes, we look back into loop(), and do the same thing on (1,0). Again, because orientation matters, it gets a little more complicated determining which sensor to read from and where to turn. After both of those paths have been explored fully (or determined to be unexplorable), we return to our starting location, face north, halt the robot, and turn on our victory LED.

A video showing our robot navigating a complex 4x5 maze along with part of our code is shown below.

<p align="center">
  <iframe width="560" height="315" src="https://www.youtube.com/embed/UaM1cZHoPV0" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

```c
// This program allows the robot to nagivate any maze using a DFS

... // Declarations

... // Moving commands (i.e. forward, turn left/right/around, 
... // slight left/right to correct during straightaways, etc.)

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
  byte visited; // bytes to save space
  byte walls; // bytes to save space
};

StackArray <byte> path; // bytes to save space
box maze[100];
node current;

void dfs( byte location ) { 
  // NOTE: location must be an open location for 
  // us to go to and we must be facing that location.


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
    if (current.dir != curr_direct) { // coming in from opposite direction
      right90Turn();
      current.dir = direction( (int(current.dir) + 1) % 4 );
    } else { // facing right direction
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
    if (dir_calc == 0) { // coming in from right direction
      right90Turn();
      current.dir = direction( (int(current.dir) + 1) % 4 );
    } else if (dir_calc == 2) { // coming in from opposite direction
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
    walkBack(); // Return to previous location
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
      dfs( byte( int(current.pos) + 1 ) )
    }
    if (current.dir == south) { // If we are returning from dfs toward (0,1)
      left_detect = digitalRead(left_ir_sensor);
      if (left_detect && maze[int(B00010000)].visited == B00000000) {
      // If (1,0) is open
        left90Turn();
        current.dir = east;
        dfs( byte( int(current.pos) + 16 ) );
        right90Turn();
      } else { // else just turn and face north
        right180Turn();
      }
    } // else we're facing north
    right_detect = digitalRead(right_ir_sensor);
    if (right_detect && maze[int(B00010000)].visited == B00000000) {
      right90Turn();
      current.dir = east;
      dfs( byte( int(current.pos) + 16 ) );
      right90Turn();
    }
    halt();
    ending = 1; // We're done; switch ending to 1 so we don't keep doing stuffs
    digitalWrite(DONE_LED, HIGH); // Victory LED
  }
}
```

## Conclusion

We were able to successfully complete this milestone. TALK ABOUT ROBOT DETECTION STUFF BEING EASY OR HARD OR SO-SO OR WHATEVER LOL. Debugging the DFS took almost a full week of going to lab every day, which proved to be quite difficult despite all the time we put in. However, we were able to get it working in the end. It is important to note that when we tried integrating everything for Lab 4 (including the FFT and Radio libraries), we ran out of dynamic memory on our Arduino (actually, we reached 87% but there were many issues occuring). Thus, we will need to rewrite the DFS by the end of Lab 4 so that we don't use the StackArray library and so that we don't use recursion. A TA told us that using both of these things were very dangerous on the Arduino after we had finished this milestone so, again, we will need to completely redo the algorithm. 
