# Lab 3: FPGA Video Controller

## Overview
The goal of this lab was to enable communication between an Arduino
and a DE0_NANO FPGA board. We decided to use parallel communication 
to send data from the Arduino to the FPGA. Eventually, this data will
allow us to draw important information on a computer display, such
as wall locations and visited squares. Ryan and Jinny focused on 
meeting the requirements for Lab 3, whereas Joy and Daniel were
working on the third milestone.

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

The Image_processor took in the data from the Arduino and parsed the 
concatenated bits into its separate parts. Then it would calculate the
proper result to store into memory as well as the write address in memory
at which to store this result. In addition, the Image_processor controls
when write to the memory is enabled.

## Communicating Between Arduino and FPGA
Initially, we planned to use serial communication, or SPI, because
we thought it would allow us to send data back and forth faster.
However, we eventually realized parallel communication was much easier
to implement. The circuit we constructed only consisted of several
voltage dividers (1 per bit of communication). This was required 
because the Arduino operates at 5 V and the FPGA operates at 3.3 V.

<p align="center">
  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab3/images/parallel_comm.jpg" height="400">
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

To change the color on the screen, we just wrote the necessary Arduino 
code to set the desired digital pin outputs to high or low. Then, on the
FPGA's side, in Verilog, DE0_NANO was modified to read from the same memory 
address that was being written. This ensured that the entire screen would
flash a different color according to the Arduino input to the FPGA. The memory
would contain the bit of data that was being transferred from the Arduino to 
the FPGA to eventually control what color each pixel on the screen would be.

The color was adjusted with the following code:
```verilog
always @ (posedge c2) begin
    if (MEM_OUTPUT) begin
        pixel_color <= GREEN;
    end

    else begin
        pixel_color <= BLUE;
    end
end
```

## Conclusion
After following the guidelines provided by the previous year's lab, the
objectives of this lab did not seem as daunting as they were when first
introduced. The most challenging parts of this lab were figuring out what
each module in the verilog code was doing and choosing a communication
method to implement. Although just changing the color of the screen is an
accomplishment, it isn't the ultimate goal, and the next obstacle is 
properly implementing the Image_processor so that the screen can display
tiles and walls in specific location. 
