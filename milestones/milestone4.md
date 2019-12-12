# Lab 4/Milestone 4: Radio Communication and Full Robot Integration

## Overview
One goal of lab 4 was to enable communication between the robot and video controller. After completing this lab, we were able to send information from the robot's Arduino to our basestation Arduino through a radio. This information included the robot's current position in the maze, as well as the position of walls at each square. Once our basestation Arduino received this information, we used the Verilog code from lab 3 to correctly display the maze on the screen.

We also worked on full robotic integration for lab 4. Throughout the semester, we have worked on various smaller projects, including motor control, line sensing, the FFT algorithm, the DFS algorithm, robot detection, wall sensing, the FPGA/VGA display, and wireless communication, among others. The other main focus of this lab was to combine all of these components into one working robot and one basestation in preparation for the final competition.

For milestone 4, we had three requirements to fulfill. First, we had to update the base station display. We did this by simulating maze data and uploading it directly to our Arduino. Our FPGA then took this data and displayed it on the display using our Verilog code from lab 3. After this was completed, we worked on wirelessly communicating information from the robot to the base station. The radio communication that we worked on in lab 4 enabled us to do this. Once we did this, we are able to send real maze data and draw it on the screen. Finally, we worked on robot detection. This required adding several IR emitters and detectors to our robot. Since the other robots will also have emitters and detectors, we will be able to determine when other robots are nearby. When a robot is detected, we modified our DFS algorithm to avoid a collision.

## Materials
 * 2 Nordic nRF24L01+ transceivers
 * 2 Arduino Unos
 * 2 radio breakout boards with headers
 * 1 DE0-Nano Cyclone IV FPGA Board
 
## Radio Communication
When we obtained the two radio breakout boards, everything was already soldered for us except for the power wires, so we quickly soldered those to the boards and moved on. We then downloaded the RF24 Arduino library and the "Getting Started" sketch, put the radios into two separate Arduinos, changed the channels over which we want to transmit (so that we don't also receive signals from other groups), and the radios transmitted and received without a problem. We also walked around with the Arduinos and radios and played with the power transmission settings. We found that RF24_PA_HIGH suited our needs (anything below that was too weak, but we didn't quite need max power). 

We then wanted to simulate our robot traversing a maze, so we made up a 10x10 array of random wall values and transmitted the message as seen below. Our original scheme involved sending if a robot was detected (r), if the location was visited (v), the walls (www), and the coordinates (xxxxyyyy). For the simulation, we always assumed no robot was detected and the location was always visited. Thus, the scheme looked like the the image below, and part of the code we used to simulate the robot is shown below, as is a video showing the results,.

<p align="center">
  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab4/OldScheme.png">
</p>

```c
byte maze[10][10] =

{
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
};

...
...

msg = 0000000000000000;
msg = (msg << 1) | 1;
msg = (msg << 1) | 0;
msg = (msg << 3) | maze[x][y];
msg = (msg << 4) | x;
msg = (msg << 4) | y;

printf("\ncoord: ");
printf("%d", x);
printf(", %d \n", y);
printf("walls: ");
printf("%x", robot);
printf("\nmsg: ");
printf("%x", msg);
printf("\n");

// simulate robot in maze
if (x == 9 && y == 9) {
  x = 0;
  y = 0;
} else if (x %2 == 0) {
  if (y == 9) {
    x++;
  } else {
    y++;
  }
} else {
  if (y == 0) {
    x++;
  } else {
    y--;
  }
} 
```

## INSERT VIDEO SHOWING RADIO COMMUNICATION RESULTS

While this scheme worked, we thought it over a little more and realized a lot of the information we were sending was excessive. We don't need to draw robots so we don't need to send that information, and since we only send one square at a time when we're at an intersection, the visited bit was also useless. Thus, we came up with a new and improved scheme that sends the current coordinates (XXXXYYYY), the walls (NESW), and a sent bit (S) indicating if we had already sent the walls information for a given location. This sent bit is useful because we don't want to overwrite older information if we're in a walkback of the DFS. The final scheme is shown below.

<p align="center">
  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab4/NewScheme.png">
</p>

## Drawing a Tile

30x30 tile boy
clocks were an issue
I hate Verilog

## Drawing the Maze

## Full Robotic Integration
The last thing to do was to put it all together. 
### Hardware

We had created the pushbutton circuit in a previous lab, but for completeness, we have shown the circuit below. It's a simple pushbutton held low with a pulldown resistor until the button is pressed.

<p align="center">
  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab4/pushbutton.png">
</p>

The wall sensors, also working from a previous lab, utilize Inverting Schmitt Triggers so that we could use digital pins rather than analog pins in order to save the pins for other inputs that only work with analog pins. The same schematic from Lab 2 is copied below. We made three of these circuits: one for each wall sensor.

<p align="center">
  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab2/images/SchmittTrigger.png">
</p>

In order to detect other robots, we built another simple circuit that is shown below. We used 940nm IR detectors (shown as the phototransistor below) and played around with the resistor value until we had a large enough range that allowed for easy differentiation between ambient noise (i.e. radiation from the Sun and computer monitors) and the emitters that go on every robot.

<p align="center">
  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab4/Detector.png">
</p>

The next circuit we needed to build was an LED circuit that would turn on when we finished mapping the maze. It was yet another simple circuit to build, with the schematic shown below. The green LED is controlled by the Arduino rather than always being at +5V, unlike the previous circuits shown above. 

This circuit is exactly the same as our four IR emitter circuits, except that the IR LEDs are powered by +5V rather than the Arduino output.

<p align="center">
  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab4/LED.png">
</p>

The final thing we needed to add hardware-wise was support for our fast servos. Powering them off +5V from the Arduino just wasn't going to cut it. Thus, we used a 9V battery for each servo. The circuit isn't drawn here because it's the same thing as powering regular servos, but we wanted to make note that we wired the ground of the battery to the Arduino ground so that we had a common ground, and the +9V terminal went to the power wire on the Servo. The signal that went through the 330Ω resistor from the Arduino to the control wire remained exactly the same.

### Software

The software proved far more difficult than anticipated despite the hardware working properly. We integrated the software incrementally in order to aid in the debugging process. 

The first thing we did was completely revamp our DFS algorithm. As noted in the previous milestone, the TAs told us that using recursion and additional data structures like a stack array would probably cause us to run out of memory and cause our robot to fail. Thus, we wrote an iterative DFS using only structs that we defined. Similar to Milestone 3, we created a node struct that holds the position and direction that we always use to keep track of our current location and orientation, and a maze struct that holds a visited bit, a sent bit (for radio communication), and the direction the robot came from in order to reach that location, as well as the location of walls and navigable neighbors around the location. All of these fields use bytes so we can save space. The maze struct is used to make an 81 element array (because the final competition has mazes that are 9x9), with each element representing the location of a square in the maze. Part of the code is shown below. 

It is an iterative DFS, so we call dfs() once every time loop gets called. We only want to start moving if the button is pressed so that's why we have that first conditional in loop. Once the button is pressed, then we want to start the dfs. Then in dfs, we determine our current position and the walls around us, change the neighbors to recognize that we've visited the location, determine if we're done mapping the maze, and then look to move. If we can move, then we do; otherwise, we walk back using the information we've saved into the maze. Finally, in loop, when we return to (0,0), we halt. This integrates many things already: the DFS algorithm, the line sensors, the wall sensors, the green done LED, and the pushbutton. A video is shown below the code of this code working on a 4x4 maze.

```c
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

void dfs() {
  byte location = current.pos;

  // determine the walls around the current location
  determineWalls(location);

  // set all neighbors such that location is now unnavigable (to avoid repeats)
  determineNav(location);

  if (determineDone()) { // if we've navigated everywhere we can, turn the done LED on.
    digitalWrite(DONE_LED, HIGH);
  }

  int wemoved = 1;
  
  // if everywhere isn't unnavigable around location
  if ((maze[int(location & B00001111) * 9 + int(location >> 4)].walls_neighbors & B00001111) != 15) {

    // Now move based on movement priority:
    // try in front of us, then to left, then to right, then behind

    if (int(current.dir) == 8) { // facing north
      if (!moveNorth()) {
        if (!moveWest()) {
          if (!moveEast()) {
            wemoved = moveSouth();
          }
        }
      }
    } 
    // Similar code for other three directions
  } else { // if all neighbors are unnavigable
    wemoved = 0;
  }

  if (!wemoved && current.pos != 0) { // if we didn't move, 
  // i.e. all neighbors have been visited and/or have walls, and we aren't at the start
    walkBack();
  }
}

... // setup and DFS helpers

void loop() {
  if (beginning) { // wait for pushbutton
    if (digitalRead(START_BUTTON) {
      dfs();
      beginning = 0; // stop calling fft if 950 Hz detected or button pressed
    }
  }
  else if (!ending) { // to make sure we don't keep doing stuff after we finish
    dfs();
  } else { // Permanently halt at (0,0) when done.
    halt();
  }
}
```

## INSERT 4x4newDFStest VIDEO HERE

## Conclusion
