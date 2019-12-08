# Lab 4/Milestone 4: Radio Communication and Full Robot Integration

## Overview
One goal of lab 4 was to enable communication between the robot and video controller. After completing this lab, we were able to send information from the robot's Arduino to our basestation Arduino through a radio. This information included the robot's current position in the maze, as well as the position of walls at each square. Once our basestation Arduino received this information, we used the Verilog code from lab 3 to correctly display the maze on the screen.

We also worked on full robotic integration for lab 4. Throughout the semester, we have worked on various smaller projects, including motor control, line sensing, the FFT algorithm, the DFS algorithm, robot detection, wall sensing, the FPGA/VGA display, and wireless communication, among others. The other main focus of this lab was to combine all of these components into one working robot and one basestation in preparation for the final competition.

For milestone 4, we had three requirements to fulfill. First, we had to update the base station display. We did this by simulating maze data and uploading it directly to our Arduino. Our FPGA then took this data and displayed it on the display using our Verilog code from lab 3. After this was completed, we worked on wirelessly communicating information from the robot to the base station. The radio communication that we worked on in lab 4 enabled us to do this. Once we did this, we are able to send real maze data and draw it on the screen. Finally, we worked on robot detection. This required adding several IR emitters and detectors to our robot. Since the other robots will also have emitters and detectors, we will be able to determine when other robots are nearby. When a robot is detected, we modified our DFS algorithm to avoid a collision.

## Materials
 * 2 Nordic nRF24L01+ transceivers
 * 2 Arduino Unos
 * 2 radio breakout boards with headers
 
## Radio Communication
soldering PCB, installing RF24 Arduino library
 
## Simulating the Robot
data structure to encode maze information

sending simulated information to the base station (4 bits for x, 4 bits for y, 4 bits for walls)

protocol to send data from robot to base station?

sending the entire maze vs only sending new information

### Full Robotic Integration

## Drawing the Maze

## Conclusion
