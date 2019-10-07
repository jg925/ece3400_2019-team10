# Lab 2: Analog & Digital Circuitry and FFTs

## Overview
The goal of Lab 2 was to be able to detect a 950 Hz note using the FFT algorithm.
In order to do this, we designed an amplifying band pass circuit to take the output
of an electret microphone and boost the signal while blocking undesired frequencies.
This signal is then sent into the Arduino, where the FFT algorithm looks to find the right
frequency. At that point, our robot is to begin moving. In addition, we also designed a
Schmitt Trigger circuit to be used in our wall detection. We wire the output of an IR sensor
to the trigger, and it completely converts the analog signal into a digital one, thus freeing
up some analog ports for future use. In working on this lab, we broke up into two subteams:
Jinny and Ryan worked on the FFT algorithm and analysis, while Joy and Dan worked on designing
and building the circuitry for both the band pass amplifier and the Schmitt Trigger.

### Materials
* 1 Arduino Uno
* 1 USB A/B Cable
* Electret Microphone
* Multiple Capacitors (nF and uF range)
* Several Resistors (kΩ range)
* LM358P Operation Amplifiers

## Amplifying Band Pass Circuit

The first part of this lab involved building a circuit for the electret microphone. A picture of one along with a representative
circuit schematic are shown below.

<img src="https://www.google.com/url?sa=i&source=images&cd=&cad=rja&uact=8&ved=2ahUKEwjSnOW8h4nlAhVCtlkKHSLfB_8QjRx6BAgBEAQ&url=%2Furl%3Fsa%3Di%26source%3Dimages%26cd%3D%26ved%3D%26url%3Dhttps%253A%252F%252Fen.wikipedia.org%252Fwiki%252FElectret_microphone%26psig%3DAOvVaw1tviBlI7mIagDRgXzEHLrH%26ust%3D1570500246672498&psig=AOvVaw1tviBlI7mIagDRgXzEHLrH&ust=1570500246672498">
<img src="https://www.google.com/url?sa=i&source=images&cd=&cad=rja&uact=8&ved=2ahUKEwjcpZjeh4nlAhXEzlkKHdZVBgkQjRx6BAgBEAQ&url=%2Furl%3Fsa%3Di%26source%3Dimages%26cd%3D%26ved%3D%26url%3D%252Furl%253Fsa%253Di%2526source%253Dimages%2526cd%253D%2526ved%253D%2526url%253Dhttps%25253A%25252F%25252Fwww.sparkfun.com%25252Fproducts%25252F8635%2526psig%253DAOvVaw1tviBlI7mIagDRgXzEHLrH%2526ust%253D1570500246672498%26psig%3DAOvVaw1tviBlI7mIagDRgXzEHLrH%26ust%3D1570500246672498&psig=AOvVaw1tviBlI7mIagDRgXzEHLrH&ust=1570500246672498">


<p align="center">
<iframe width="560" height="315" src="https://www.youtube.com/embed/1Nl6ceJsTIU" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

## The FFT Algorithm

## Implementing Schmitt Trigger

<p align="center">
<iframe width="560" height="315" src="https://www.youtube.com/embed/KV99awF2qbQ" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

<iframe width="560" height="315" src="https://www.youtube.com/embed/hOAEnnWUPrM" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

## Conclusion
