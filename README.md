# superlounge-chair-music
A chair with four pressure sensors embedded within the padding, and an LED strand that reacts to it. This was a project that spawned during [Prof. Hertzberg's Spring 2019 Aesthetics of Design class](http://www.aesdes.org/2019/05/08/luke-collier-summing-up-aesthetics/) (Click link for more details). 
The Apr 21 version simply prints the force of each resistor. 
The May 12 version attempts to output MIDI notes based on your position in the X and Y axes of where you're sitting. It also outputs a color in the corresponding section of a 60-LED strand. 
## Requirements:
* Arduino
* 4 Force Sensitive Resistors, hooked up as in [this tutorial](https://learn.sparkfun.com/tutorials/force-sensitive-resistor-hookup-guide/all).
* They are arragned as in the following:

| A3| A0|
|---|---|
| A2| A1|
