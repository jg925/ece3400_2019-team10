# Milestone 3: Avoiding Robots and Traversing Mazes

## Introduction

This milestone involved having our robot successfully avoid other robots and 
explore a unique 4x5 maze with some algorithm such as DFS, BFS, Dijkstra, or A\*
. For this milestone, we relied on the Arduino code and hardware that we set up in previous labs throughout the semester, specifically line following and wall detection. In order to detect other robots, we tested different IR emitters and detectors and eventually chose to go with the wider angle detectors and the shorter ranged emitters, mounting three detectors and four emitters at exactly 5 inches off the ground. For maze exploration, we decided to implement a DFS algorithm which involved utilizing stacks and recursion.

## Robot Detection

This milestone involved using IR detectors and emitters to detect and avoid other robots. Per the lab specification, we mounted both the detectors and sensors at exactly 5 inches off the ground. Mounted detector shown below.

<p align="center">
  <img src="https://github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab3/phototransistor.jpg" height="350" width="350">
</p>

There were two options of IR detectors. After testing both with the same circuit below, we decided to use the wider angle detector to ensure our robot detection had the most range possible. When testing this IR detector circuit, we found that a higher resistance in the circuit allowed a larger range of analog read values to occur. With a 330Ω resistor, the analog read values were too small to have noticeable differences. So we upped the resistance to 10kΩ and obtained a solid range from 0 to 100 which is shown in the circuit and videos below.

<p align="center">
  <img src="https://github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab3/circuit.jpg" height="350" width="250">
</p>

<p align="center">
  <iframe width="560" height="315" src="https://www.youtube.com/embed/A8EQLZdOH7E" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
  <iframe width="560" height="315" src="https://www.youtube.com/embed/fa2VKi1fIuM" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

We decided to use three detectors for the front, right and left sides of our robot. As per the lab instructions, we mounted four emitters for each side of our robot. While we were testing our robot in a maze, however, we noticed that we were recieving a lot of noise from our detectors even after changing the threshold many times. We first concluded that it was probably due to the IR emitted from computer screens and the sun. But we found out through more testing that we were also picking up the IR being emitted from our own robot emitters as it bounced off of nearby walls. With the velcro added to the upper half of all maze walls, we had to reposition all of our wall sensors, IR detectors and IR emitters. We switched all our wall sensors to the lower level of our robot and made sure there was lots of space in between each detector and emitter pair on each side of our robot.

<p align="center">
  <iframe width="560" height="315" src="https://www.youtube.com/embed/Ns-UvfSCByA" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

Below is the "robot" that we avoided in our video above. The emitter circuit simply consists of one IR emitter and one 330Ω resistor connected to a power source.

<p align="center">
  <img src="https://github.coecis.cornell.edu/jg925/ece3400-2019-team10/milestones/ir.jpg" height="250" width="350">
</p>

<p align="center">
  <img src="https://github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab3/emittercircuit.jpg" height="250" width="150">
</p>

Our code for reading in the values from the phototransistors consisted of using a running average of a sample of 10 inputs. Every time we detect that the average of the sample is above our decided threshold, we turn on the green LED located on our protoboard and make a 180 degree turn. Originally, we were halting when the IR was detected, but after learning that halting as a avoidance technique would be ineffective for the actual competition, we chose to make a U-turn instead. Immediately once the average drops below this threshold, the LED turns off and the robot does normal right hand rule line following navigation.

```c
int photo_input = analogRead(left_robot_detect);
  
pi_arr[sample_size-1] = photo_input;

for (int i= 1; i < sample_size; i++) {
  pi_arr[i-1] = pi_arr[i];
}

sum = 0;
for (int i= 0; i < sample_size; i++) {
  Serial.println(pi_arr[i]);
  sum += pi_arr[i];
}

avg = sum/sample_size;

if (avg > threshold) {
  digitalWrite(robot_LED_pin, HIGH);
  right180Turn();
} else {
  digitalWrite(robot_LED_pin, LOW);
  navigate();
}
```

After soldering the IR detection circuit onto our protoboard, the IR detector readings were abnormal. We quickly realized that this was due to a lot of noise from the rest of our circuits on the protoboard, so we isolated the IR detection circuit on a separate protoboard and the readings were back to normal. We also decided to switch to a 100kΩ resistor because after further testing, we found that it gave us an even better range of 0 to 700 which really helped in our robot detection.

## Maze Exploration

In this section of the milestone, we implemented a depth-first search in order to successfully traverse a 4x5 maze. In order to implement this algorithm, we used the StackArray Arduino library. We created an enum called direction as well as a struct called node that keeps track of the robot's orientation and position. We used bytes to store the position to save space; the first four bits are for the x-coordinate and the last four are for the y-coordinate. We created another struct that we used to create a 100 element array to simulate the 10x10 maze. This node contains two bytes which tell if the location has been visited and where the walls are relative to where to robot entered the position. We initially set all the walls to be 00000111 in setup() because we use shifting to properly set the bits once we determine the locations using the wall sensors. When placing our robot into the corner of a maze, we always say it starts at (0,0) and that it is always facing north, so we always place it such that there are walls to the left and behind it as shown below. This greatly simplified the start-up code. 

<p align="center">
  <img src="https://github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab3/images/RobotInit.jpg" height="350" width="350">
</p>

When loop() is called for the first time, we wait for the start button that we integrated to be pressed. Then we check if (0,1) is open and we perform a DFS on that location. Our DFS is a little more complicated than that of a "standard" DFS because we always have to take the robot's orientation into account. First, we move to the open location, push it onto the stack, and determine where the walls are, modifying the bytes in our maze array as needed. Then we determine the coordinates of the neighbors, save the current orientation, and look to recursively call DFS on the location in front of the robot if there isn't a wall, it isn't visited, and it's in the maze. Then, depending on the orientation after that, we read the appropriate wall sensor to check the location to the left of the original orientation, check the coordinate to see if it's navigable, and perform another DFS if possible. Finally, we execute a similar process for the location to the right of the original orientation. After all that, we pop the current location off the stack and return to the previous location (if there is a location to return to). After the DFS to (0,1) fully completes, we look back into loop(), and do the same thing on (1,0). Again, because orientation matters, it gets a little more complicated determining which sensor to read from and where to turn. After both of those paths have been explored fully (or determined to be unexplorable), we return to our starting location, face north, halt the robot, and turn on our victory LED.

A video showing our robot navigating a complex 4x5 maze is shown below. Bits of our code are also shown below that, with many declarations and functions omitted for brevity. If a function was omitted, there is a comment next to its call in DFS explaining briefly what it does.

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


  movetoLocation(location); // move to the open location using similar navigation code as Milestone 2
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

We were able to successfully complete this milestone. Building the phototransistor circuit wasn't too hard, but there were many problems with the readings from the detectors due to external IR and circuit board noise. We considered a few different ways to filter noise and ended up using a running average rather than a hardware filter. We quickly realized that the phototransistor readings weren't very consistent–due to positioning, bouncing, etc.–and that we need to work on it more for the actual competition. Debugging the DFS took almost a full week of going to lab every day, which proved to be quite difficult despite all the time we put in. Many hours were spent figuring out why the direction and coordinates weren't updating correctly, which we eventually figured out to be caused by us not calling DFS on the correct location based on the sensor readings and current directions. However, we were able to get it working in the end. It is important to note that when we tried integrating everything for Lab 4 (including the FFT and Radio libraries), we ran out of dynamic memory on our Arduino (actually, we reached 87% but there were many issues occuring). A TA told us after we finished this milestone that using both the StackArray library and recursion on the Arduino is very dangerous, so we will need to completely rewrite the algorithm by the end of Lab 4. 
