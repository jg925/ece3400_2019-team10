# Milestone 4: Display, Communication, and Robot Detection

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
While this scheme worked, we thought it over a little more and realized a lot of the information we were sending was excessive. We don't need to draw robots so we don't need to send that information, and since we only send one square at a time when we're at an intersection, the visited bit was also useless. Thus, we came up with a new and improved scheme that sends the current coordinates (XXXXYYYY), the walls (NESW), and a sent bit (S) indicating if we had already sent the walls information for a given location. This sent bit is useful because we don't want to overwrite older information if we're in a walkback of the DFS. The final scheme is shown below.

<p align="center">
  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab4/NewScheme.png">
</p>

## Drawing a Tile
After Lab 3, we continued to work on our FPGA Verilog code to map out a tile. As a first stepping stone to the final goal of 
drawing out a full maze, we aimed to accurately draw a tile in a certain location. Since the robot sends data via 
the radio communication at each intersection in the maze, we wanted to be able to draw a tile representing each of those 
intersections. We set the visible screen size to be able to fit the entire 10x10 tile maze which would include 30x30 pixel 
tiles.

Our overall flow for drawing a tile at a location was to send information from the base station's Arduino about the 
coordinates of the robot in the maze. These coordinates would then directly map to a certain top-left corner pixel of a 30x30 
pixel tile that would be mapped. `DEO_NANO.v` and `IMAGE_PROCESSOR.v` were the two main files that were heavily modified to try different methods for mapping out the tiles. Ultimately, we had `DE0_NANO.v`, the main FPGA project module, convert the Arduino inputs indicating x and y location into its corresponding top-left corner pixel. Within `IMAGE_PROCESSOR.v`, an iterating `xpos` and `ypos` would be added on to the top-left corner x and y inputs to create the tile. Each pixel corresponded to an index in the memory, and the color of that pixel would be stored. To create the walls, we set the color of the outer border of the tile to a different color than the inside of the tile. In addition to the x and y position of the top left corner of the tile, the wall orientation of each tile would be known and would indicate which sides of the tiles would be colored differently.

Initially, we had problems writing to the memory in a way that would properly display on the screen. Eventually, we realized there were issues in the clocks we were using in `IMAGE_PROCESSOR.v` and that we needed to write to memory faster than we wrote to it. Once we set the clocks correctly, and worked through issues with memory overflow, we were able to draw a 30x30 pixel tile in any location on the screen. 

```c
code
```

## Drawing the Maze
The next 

## Full Robotic Integration
The last thing to do was to put it all together. 

The next thing that we integrated was radio communication to the base station Arduino. This was made extremely simple because we had done a lot of this in the earlier section of Lab 4. We just had to be careful with what we were sending because we used bytes, so some cleverness with bit shifting and bit-wise operations were required. The code is shown below for the final communicate method used on the robot. For reference, this gets called immediately after the determineWalls method is called in our DFS. We also update the sent bit of a location in the maze after the call to communicate is completed in DFS. There is also a video below showing our progress.

```c
void communicate() {
  // 000swwwwxxxxyyyy is the scheme we are using

  // zeroes out msg
  uint16_t msg = 0000000000000000;

  // walls already sent bit; NOTE: IF THIS IS 1, DO NOT DRAW THE WALLS AGAIN
  msg = (msg << 1) | ((maze[int(current.pos & B00001111) * 9 
    + int(current.pos >> 4)].vs_came & B01000000) >> 6);

  // walls bits
  msg = (msg << 4) | (maze[int(current.pos & B00001111) * 9
    + int(current.pos >> 4)].walls_neighbors >> 4);

  // x pos bits
  msg = (msg << 4) | (current.pos >> 4);

  // y pos bits
  msg = (msg << 4) | (current.pos & B00001111);

  // First, stop listening so we can talk.
  radio.stopListening();
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
    // Grab the response, compare, and send to debugging spew
    uint16_t got_msg;
    radio.read( &got_msg, sizeof(uint16_t) );
    
    // Spew it
    printf("Got response %x \n\r", msg);
  }
}
```
<p align="center">
  <iframe width="560" height="315" src="https://www.youtube.com/embed/CR6a_61peiU" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

The next thing we integrated was robot detection.

## Conclusion
