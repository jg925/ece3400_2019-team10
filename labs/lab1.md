# Lab 1: Microcontroller

## Overview
The goal of Lab 1 was to accustom us to the many functionalities of the Arduino UNO 
and the Arduino IDE, such as using analog and digital input/outputs and driving 
peripheral devices (LED, Servos, etc). The finished product was to create 
an assembled robot that drives autonomously.


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
and turn it off for 1 second, looping repeatedly. 

<iframe width="560" height="315" src="https://www.youtube.com/embed/ZpkMIolsPc0" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

Next, we took the Blink sketch and modified it to light up an external red LED.
To do this, we changed the setup function, placed a red LED on the breadboard,
wired it through a 330Ω, and then connected it to an output pin on the Arduino.
The final code for this is shown below.

```c
// A simple Arduino program to make an external LED blink

int EXTERNAL_LED = 13;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
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

<iframe width="560" height="315" src="https://www.youtube.com/embed/X57n8F3q364" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

## Reading from a Potentiometer

In the second section of the lab, we were tasked with assembling the robot with 
a potentiometer. We first set up the potentiometer like in the following picture 
and tested the output readings through serial communication.

![potentiometer setup](lab1/media/potentiometer_setup.jpg)

```c
// This program allows us to continuous read the voltage across a voltage divider by alternating the potentiometer

int VOLT_DIVIDE = A0;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin VOLT_DIVIDE as an output.
  pinMode(VOLT_DIVIDE, INPUT);
  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
  Serial.println(analogRead(VOLT_DIVIDE));    // Print out the value being read from the analog pin
  delay(500);                                 // wait for half a second
}
```

A snippet of the outputs printed to COM3 using the code above is shown below.

![potentiometer output](lab1/media/potentiometer_output.jpg)

We were also tasked with adjusting the brightness of an LED with the potentiometer.
A clip of that is shown below, along with the modified code that was used.

<iframe width="560" height="315" src="https://www.youtube.com/embed/P90ZEs6cvP0" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

```c
// This program allows us to control the brightness of an LED by turning the potentiometer

int VOLT_DIVIDE = A5;
int LED = 3;
int v = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(VOLT_DIVIDE, INPUT);
  pinMode(LED, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  v = analogRead(VOLT_DIVIDE);
  analogWrite(LED, v/2);
}
```
## Driving a Servos
In this next portion of the lab, we were tasked with driving a servos using the Arduino UNO.
The Servos motors would be the motors running the wheels of our robot later on.

<iframe width="560" height="315" src="https://www.youtube.com/embed/WLWtf4ng6Ug" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

After testing the Servos alone, we used a potentiometer to adjust the speed and direction 
at which the Servos motor was rotating.

<iframe width="560" height="315" src="https://www.youtube.com/embed/ou17E-c4ugQ" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

## Driving Autonomously
 


