# Milestone 4: Display, Communication, and Robot Detection

## Overview
For milestone 4, we had three requirements to fulfill. First, we had to update the base station display. We did this by simulating maze data and uploading it directly to our Arduino. Our FPGA then took this data and displayed it on the display using our Verilog code from lab 3. After this was completed, we worked on wirelessly communicating information from the robot to the base station. The radio communication that we worked on in lab 4 enabled us to do this. Once we did this, we are able to send real maze data and draw it on the screen. Finally, we worked on robot detection. This required adding several IR emitters and detectors to our robot. Since the other robots will also have emitters and detectors, we will be able to determine when other robots are nearby. When a robot is detected, we modified our DFS algorithm to avoid a collision.

## Materials
 * 2 Nordic nRF24L01+ transceivers
 * 2 Arduino Unos
 * 2 radio breakout boards with headers
 * 1 DE0-Nano Cyclone IV FPGA Board

## Changing the Display

For lab 3, the requirements were relaxed, so we only had to change the color on the screen by alternating data we sent from the Arduino. We simply alternated a one-bit value and used that to write an alternating color to the display. However, we knew the ultimate goal was to draw a complete maze, so we decided to use tiles with 30 rows of pixels and 30 columns. Once we had this working, we would then be able to draw an entire square on the screen just by providing the x and y location of where it should go.

### Drawing a Tile
After Lab 3, we continued to work on our FPGA Verilog code to map out a tile. As a first stepping stone to the final goal of 
drawing out a full maze, we aimed to accurately draw a tile in a certain location. Since the robot sends data via 
the radio communication at each intersection in the maze, we wanted to be able to draw a tile representing each of those 
intersections. We set the visible screen size to be able to fit the entire 10x10 tile maze which would include 30x30 pixel 
tiles.

Our overall flow for drawing a tile at a location was to send information from the base station's Arduino about the 
coordinates of the robot in the maze. These coordinates would then directly map to a certain top-left corner pixel of a 30x30 
pixel tile that would be mapped. `DEO_NANO.v` and `IMAGE_PROCESSOR.v` were the two main files that were heavily modified to try different methods for mapping out the tiles. Ultimately, we had `DE0_NANO.v`, the main FPGA project module, convert the Arduino inputs indicating x and y location into its corresponding top-left corner pixel. Within `IMAGE_PROCESSOR.v`, an iterating `xpos` and `ypos` would be added on to the top-left corner x and y inputs to create the tile. Based on the current value of xpos and ypos, we drew a pixel in the correct location, and then incremented the values until the full square was drawn. We used a direct mapping between each pixels location and its memory address to keep previously drawn tiles on the screen.

Initially, we had problems writing to the memory in a way that would properly display on the screen. Eventually, we realized there were issues in the clocks we were using in `IMAGE_PROCESSOR.v` and that we needed to write to memory faster than we wrote to it. Once we set the clocks correctly, and worked through issues with memory overflow, we were able to draw a 30x30 pixel tile in any location on the screen.

```c
if (valid && count > 5) begin

  ...
  
  // Incrementing xpos and ypos
  if (xpos >= 30) begin
    if (ypos < 30) begin
      ypos <= ypos + 5'b1;	
      xpos <= 5'b0;
      W_EN <= 1;
    end
  end

  else begin
    W_EN <= 0;
    ypos <= 5'b0;
    xpos <= 5'b0;
    count <= 0;
  end
  
  else begin
    W_EN <= 1;
    ypos <= ypos;
    xpos <= xpos + 5'b1;
  end
  
  ...
  
end
```
In the above code snippet, we can see how pixels are only drawn when valid is high. We also added a counter to allow the signal to propagate before we write anything. In the first if statement, we reached the end of a tile, so ypos is incremented and xpos is set back to 0, so we can start drawing in the beginning of the next row. W_EN is also set to 1 so we can draw. In the second conditonal statement, we have reached the bottom right corner of a tile, so we want to set all the values back to 0, and set W_EN to 0 so we don't draw anything outside of the defined tile. The else statement is the default case, where we are in the middle of a row, in which case we just increment xpos by 1 and draw there. 

### Next Steps

After we were able to draw squares in different locations, we had to work on drawing several squares in succession with walls. We completed this in Lab 4, as part of our full robotic integration. A video of our Verilog code drawing a maze with synthesized data is attached below.

## Radio Communication
While the scheme we selected in Lab 4 worked, we thought it over a little more and realized a lot of the information we were sending was excessive. We don't need to draw robots so we don't need to send that information, and since we only send one square at a time when we're at an intersection, the visited bit was also useless. Thus, we came up with a new and improved scheme that sends the current coordinates (XXXXYYYY), the walls (NESW), and a sent bit (S) indicating if we had already sent the walls information for a given location. This sent bit is useful because we don't want to overwrite older information if we're in a walkback of the DFS. The final scheme is shown below.

<p align="center">
  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab4/NewScheme.png">
</p>

The next thing to do was integrate the radio communication to the base station Arduino. This was made extremely simple because we had done a lot of this in the earlier section of Lab 4. We just had to be careful with what we were sending because we used bytes, so some cleverness with bit shifting and bit-wise operations were required. The code is shown below for the final communicate method used on the robot. For reference, this gets called immediately after the determineWalls method is called in our DFS. We also update the sent bit of a location in the maze after the call to communicate is completed in DFS. There is also a video below showing our progress.

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
  <iframe width="560" height="315" src="https://www.youtube.com/embed/X9fH-sHu8DY" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

## Conclusion
This milestone proved to be relatively simple because of our previous work in Lab 4 and every Lab and Milestone prior to this. For the accurate transmission of maze information, we had to make small adjustments to current code. Similarly, we knew how to receive and draw before, so we just had to make slight updates to the base station Arduino’s code in order to get everything working properly.

With the completion of this Milestone, we were ready to take on the other robots at the competition, where we won first place. See our home page to see Axel performing in the final round.
