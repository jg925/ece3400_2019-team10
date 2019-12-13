# Milestone 4: Display, Communication, and Robot Detection

## Overview
For milestone 4, we had three requirements to fulfill. First, we had to update the base station display. We did this by simulating maze data and uploading it directly to our Arduino. Our FPGA then took this data and displayed it on the display using our Verilog code from lab 3. After this was completed, we worked on wirelessly communicating information from the robot to the base station. The radio communication that we worked on in lab 4 enabled us to do this. Once we did this, we are able to send real maze data and draw it on the screen. Finally, we worked on robot detection. This required adding several IR emitters and detectors to our robot. Since the other robots will also have emitters and detectors, we will be able to determine when other robots are nearby. When a robot is detected, we modified our DFS algorithm to avoid a collision.

## Materials
 * 2 Nordic nRF24L01+ transceivers
 * 2 Arduino Unos
 * 2 radio breakout boards with headers
 * 1 DE0-Nano Cyclone IV FPGA Board

## FPGA STUFF

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

The next thing we integrated was robot detection.

## Conclusion
