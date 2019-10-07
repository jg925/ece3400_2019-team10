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
* Oscilloscope
* DC Power Supply
* Digital Multimeter
* Frequency Generator App for Cell Phone

## Amplifying Band Pass Circuit

The first part of this lab involved building a circuit for the electret microphone. A picture of a microphone along with a representative circuit schematic are shown below. The pull-up resistor is required to help bias the FET and power the microphone, while the capacitor blocks DC signals from the microphone.

<p align="center">
  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab2/images/08635-03-L.jpg" height="300" width="300">
  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab2/images/220px-Electret_condenser_microphone_schematic.png" height="250" width="300">
</p>

The first consideration when designing the circuit was to amplify the output of the microphone. The output voltage
was so small that nothing registered on the oscilloscope except when we would tap the microphone with our fingers.
Thus, we would need a very large gain, so we decided to design an inverting amplifier, carefully
selecting the resistors as to generate a huge gain. In addition to the huge gain, we also wanted to be able to
lower the gain of undesired frequencies; after all, the end goal is to detect a 950 Hz signal and other frequencies
could trigger false positives. Thus, we converted our inverting amplifier into a second order band pass filter. We
carefully selected the capacitor values (and modified the resistor values) so that the break frequencies were around
550 Hz and 1050 Hz. While the lower break frequency is a little low, we didn't have the components to make a working
filter that had a pass band that was so narrow. Our final circuit design is shown below.

<p align="center">
  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab2/images/MicrophoneAmplifier.png">
</p>

We then built the circuit on a breadboard and tested it by playing different frequencies using an app on a cell phone.
The results are shown in the video below.

<p align="center">
<iframe width="560" height="315" src="https://www.youtube.com/embed/1Nl6ceJsTIU" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

## The FFT Algorithm

## Implementing Schmitt Trigger

For our other circuit, we decided to design a Schmitt Trigger that would be used in detecting walls.
We had very little experience with Schmitt Triggers so we used <a href="https://howtomechatronics.com/how-it-works/electrical-engineering/schmitt-trigger/">this website</a> as a reference. 

<p align="center">
  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab2/images/SchmittTrigger.png">
  <img src="https://pages.github.coecis.cornell.edu/jg925/ece3400-2019-team10/labs/lab2/images/SchmittExplanation.png">
</p>

<p align="center">
<iframe width="560" height="315" src="https://www.youtube.com/embed/KV99awF2qbQ" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

<iframe width="560" height="315" src="https://www.youtube.com/embed/hOAEnnWUPrM" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

## Conclusion
