# Lab 3: FPGA Video Controller

## Introduction
The goal of this lab was to enable communication between an Arduino
and a DE0_NANO FPGA board. We decided to use parallel communication 
to send data from the Arduino to the FPGA. Eventually, this data will
allow us to draw important information on a computer display, such
as wall locations and visited squares.

## Materials
 * DE0-Nano Development Board
 * Arduino Uno
 * Various resistors
 * Empty DAC circuit board
 * Solder and Soldering Iron
 * VGA Cable
 * Female header
 * VGA Connector

## Designing and Implementing Memory System
We decided to use M9K memory on the FPGA board because we can store 
much more data. Although this comes at the cost of a slower memory,
we decided the larger memory was worth it. We were provided complete
Verilog modules for the VGA Driver and M9K memory. The M9K memory 
stores information about which pixels on the screen should be colored, 
and the VGA driver converts this information into colors that can be
displayed on our monitor. We also added two more modules, Image_processor
and DE0_NANO, to make our design easier to understand and change in the 
future. DE0_NANO instantiated the other modules together, and
Image_processor converted the data from the Arduino into a form 
our M9K unit could use.

** add more about image_processor **

## Communicating Between Arduino and FPGA
Initially, we planned to use serial communication, or SPI, because
we thought it would allow us to send data back and forth faster.
However, we eventually realized parallel communication was much easier
to implement. The circuit we constructed only consisted of several
voltage dividers (1 per bit of communication). This was required 
because the Arduino operates at 5 V and the FPGA operates at 3.3 V.

<p align="center">
  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab3/images/parallel_comm.jpg">
</p>

Currently, we have 7 wires from the Arduino to the FPGA: the first
is an extra bit that we used for testing, the next 3 give us the x
coordinate, and the final 3 give us the y coordinate. Since we used
parallel communication, we can easily change which bits correspond 
to which values by moving wires or changing a few lines of code. In
addition, we can add more wires to increase the bandwidth of data
we are sending.

## Changing color on the screen

<p align="center">
  <iframe width="560" height="315" src="https://www.youtube.com/embed/4p3xNtZWUC0" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

we changed it.
