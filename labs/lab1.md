# Lab 1: Microcontrollers

## Overview
The goal of Lab 1 was to accustom us to the many functionalities of the Arduino UNO 
and the Arduino IDE, such as using analog and digital input/outputs and driving 
peripheral devices (LED, Servos, etc). The finished product was to create 
an assembled robot that drives autonomously. Through this lab's entirety, all four 
of us worked together as one team. We didn't break into subteams of two but we did 
tend to work in our area of specialty. For example, Ryan and Joy worked primarily 
on the code, Dan and Jinny worked heavily on the circuits and wiring, and we all 
helped assemble the robot. The following report is an in-depth review of our work 
on this first lab.

### Materials
* 1 Arduino Uno
* 1 USB A/B Cable
* 1 Continuous rotation servos
* 1 Pushbutton
* 1 LED (red)
* 1 Potentiometer
* Several resistors (kΩ range)
* 1 Solderless breadboard

## Using and Modifying the Blink Sketch

The first task was to use the provided Blink example code to light up an internal LED for 1 second 
and turn it off for 1 second, looping repeatedly. A video is displayed below showing the results.

<p align="center"><iframe width="560" height="315" src="https://www.youtube.com/embed/ZpkMIolsPc0" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe></p>

Next, we took the Blink sketch and modified it to light up an external red LED.
To do this, we placed a red LED on the breadboard, wired it through a 330Ω resistor to limit the current through the LED,
and connected it to an output pin on the Arduino. We also changed the setup function. 
The a video showing the results is displayed below, along with final code.

<p align="center"><iframe width="560" height="315" src="https://www.youtube.com/embed/X57n8F3q364" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe></p>

```c
// A simple Arduino program to make an external LED blink

int EXTERNAL_LED = 13;
// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(EXTERNAL_LED, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(EXTERNAL_LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(EXTERNAL_LED, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
```

## Reading from a Potentiometer

In the second section of the lab, we were tasked with using a potentiometer to figure out 
how to operate the analog input ports of the Arduino. We first wired up the potentiometer 
as shown in the following picture: power to 5V, ground to ground, and the output signal 
through a parallel combination of 330Ω resistors. The resistor that connects to ground 
acts as the other half of a voltage divider, while the resistor that connects to an 
analog input port on the Arduino board acts as a current limiter. A simple schematic 
is also shown below. 

<p align="center">
  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab1/media/potentiometer_setup.jpg" height="320" width="480">

  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab1/media/circuit.png" height="320" width="480">
</p>

We then tested the output readings through serial communication. A snippet of the output 
stream printed to COM3 as we turned the potentiometer is shown below, as is the code we 
used to read the voltage level.

<p align="center">
  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab1/media/potentiometer_output.jpg" height="290" width="530">
</p>


```c
// This program allows us to continuous read the voltage 
// across a voltage divider by turning the potentiometer

int VOLT_DIVIDE = A0;

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(VOLT_DIVIDE, INPUT);
  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
  Serial.println(analogRead(VOLT_DIVIDE));    // Print out the value being read from the analog pin
  delay(500);                                 // wait for half a second
}
```

We were also tasked with adjusting the brightness of an LED with the potentiometer.
To do this, we left our potentiometer on the board as it was in the previous section. We then
wired a red LED to digital output 3 through a 330Ω resistor. We selected port 3 because it has pulse width modulation
capabilities, which we need to vary the brightness of the LED as we turn the potentiometer. 
Finally, we modified the code from the previous section so that we now write to port 3 
based on the voltage reading. We divided the voltage by 2 and wrote that value to the 
LED because based on some experimenting, that mapping produced the greatest change in 
brightness as the potentiometer was turned.
A clip of the results are shown below, along with the modified code that was used.

<p align="center"><iframe width="560" height="315" src="https://www.youtube.com/embed/P90ZEs6cvP0" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe></p>

```c
// This program allows us to control the brightness of an LED by turning the potentiometer

int VOLT_DIVIDE = A5;
int LED = 3;
int v = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(VOLT_DIVIDE, INPUT);
  pinMode(LED, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  v = analogRead(VOLT_DIVIDE);
  analogWrite(LED, v/2);
}
```
## Driving A Servo
In this next portion of the lab, we were tasked with driving a servo using the Arduino.
The servos will be the driving the wheels of our robot later on.

For our first task, we wired a servo up to the Arduino as follows: power to 5V, ground to ground, 
and input of the servo through a 330Ω resistor to a PWM-capable digital output on the Arduino. 
We then played with some values to send from the Arduino to the servo to see how it works. We 
found that from 0-80 and from 100-180, the servo was at roughly the same speed (but in opposite 
directions for the two ranges), and that from 80-100, each value noticeably altered the speed 
of the servo. Finally, we created a for loop to sweep the full range of values to the servo. 
The results of this sweep, as well as the code used, are shown below.

<p align="center"><iframe width="560" height="315" src="https://www.youtube.com/embed/WLWtf4ng6Ug" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe></p>

```c
// We used this program to experiment with how the Servo motor rotates to figure out how it functions

#include <Servo.h>

Servo SERVO;

void setup() {
  // put your setup code here, to run once:
  SERVO.attach(3);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int pos = 180; pos >=0; pos -=1) { // to sweep the voltage, found on Arduino Servo Library
    SERVO.write(pos);
    delay(40);
  }
}
```

After testing the servo alone, we used a potentiometer to adjust the speed and direction 
at which the servo motor was rotating. We wired up the potentiometer up exactly as it was 
in the previous section. We also took our code from the previous section that was used to 
change the LED's brightness using the potentiometer and modified it slightly. We chose to 
multiply the voltage by 2 and write that value since the Arduino read voltages in the 
30-60 range. Thus, doubling the voltage would put us in the 60-120 range, enabling the 
servo to rotate in both directions. A clip of the results are shown below, along with 
the modified code that was used.


<p align="center"><iframe width="560" height="315" src="https://www.youtube.com/embed/ou17E-c4ugQ" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe></p>

```c
// This program allows us to control the rate at which 
// the Servo rotates by turning the potentiometer

#include <Servo.h>

Servo SERVO;
int VOLT_DIVIDE = A5;
int v = 0;

void setup() {
  // put your setup code here, to run once:
  SERVO.attach(3);
  pinMode(VOLT_DIVIDE, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  v = analogRead(VOLT_DIVIDE);
  SERVO.write(v*2);
}
```

## Driving Autonomously
The last task in this lab was to build a robot that drives autonomously. 

We first had to assemble the robot. This was not a very enjoyable experience 
since we were in the last lab section and had to fight for the best of the scraps. 
For example, there were two ball bearings left for us to choose from, but one was 
too short and the other one was too tall. We had originally attached the smaller 
one to our robot but eventually found a saw to cut the tall one down to the right 
size so we ended up using that. Furthermore, there seemed to be a shortage of nuts 
and screws of varying sizes, and the allen keys that would have been used to screw 
them in were also missing, so we had to screw everything together with our hands 
and a set of pliers. All of this took a lot of time to sort out and inhibited us from performing at a higher level as a team.

Once we had painstakingly assembled Axel (we named our robot Axel), the rest was 
relatively straightforward. We ultimately decided to have him drive in a figure eight. 
We had to determine the correct servo speeds to have the robot make 90 degree left 
and right turns that followed a grid. It took a little trial and error, but it didn't 
take long at all. When coding the program, we decided to create simple functions and 
then build upon them in larger functions so that we wouldn't have to change much through 
our experimentation. This helped to make the code extremely readable and reusable. The final code and video results are shown below.

<p align="center"><iframe width="560" height="315" src="https://www.youtube.com/embed/kf4MASRxF84" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe></p>

```c
// This program allows the robot to "autonomously" move in a figure eight

#include <Servo.h>

Servo right;
Servo left;

int right_pin = 11;
int left_pin = 10;

void leftTurn(int t) {
  left.write(0);
  right.write(60);
  delay(t);
}

void rightTurn(int t) {
  left.write(120);
  right.write(180);
  delay(t);
}

void moveForward(int t) {
  left.write(180);
  right.write(0);
  delay(t);
}

void rightSquare() {
  for (int i = 0; i < 4; i++) {
    moveForward(3000);
    rightTurn(670);
  }
}

void leftSquare() {
  for (int i = 0; i < 4; i++) {
    moveForward(3000);
    leftTurn(670);
  }
}

void figureEight() {
  for (int i = 0; i < 4; i++) {
    moveForward(3000);
    leftTurn(668);
  }

  for (int i = 0; i < 4; i++) {
    moveForward(3000);
    rightTurn(668);
  } 
}

void setup() {
  // put your setup code here, to run once:
  right.attach(right_pin);
  left.attach(left_pin);
}

void loop() {
  // put your main code here, to run repeatedly:
  figureEight();
}
```

## Conclusion
While this lab was relatively straightforward and even fun at times, the assembly part was extremely frustrating. However, the hardest part is over and we're excited to continue work on Axel.
