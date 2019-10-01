# Milestone 1: Line Following and Figure Eight

This first milestone of the final project required us to rely on the skills
learned in Lab 1 and utilize line sensors to have the robot follow a while line
made with tape on a black background. We chose to use two line sensors, and attached
them as close to the ground as possible near the front of the robot. We made sure
to space them apart so that they were a bit more than the width of the tape apart.

<p align="center">
  <img src="milestones/teamphoto.jpg">
</p>

The following code block shows the algorithm we used to help the robot navigate. 
Since we were using 2 line sensors, we had if, else if statements to check each 
possible condition for the robot's placement relative to the white lines. If both
line sensors were detecting black, it meant that the robot was on the line and would
move forward. If only the right sensor was detecting black, then the robot would 
make a slight left turn until the left sensor was also detecting black. Likewise, 
if only the left sensor was detecting black, then the robot would make a slight
right turn until both sensors were detecting black again. These slight turns were
done while the robot was still moving forward.  

<p align="center">
<iframe width="560" height="315" src="https://www.youtube.com/embed/-ANTz5VJQi0" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

The following video shows Axel relying on line sensors to follow a Figure Eight.

<p align="center">
<iframe width="560" height="315" src="https://www.youtube.com/embed/FR61TYuzD8M" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>
