How to setup the Arduino environment
====================================

First of all, you must get Arduino 1.0.4, or newer, installed.

Once you have it, go to the installation folder of Arduino. 

A couple of files have to be modified in the Arduino system directories to add
the openDAQ board and compile programs for it:

 * Add the openDAQ board definition to the end of the `\Arduino\hardware\arduino\boards.txt` file
 * Go to the `\Arduino\hardware\arduino\variants` folder and copy the two variant folders available in this repository (`\openDAQ_M` and `\openDAQ_S`)

After these steps, you should be able to compile the firmware of openDAQ in the Arduino IDE. Just select the appropiate board and the corresponding version will be compiled.

