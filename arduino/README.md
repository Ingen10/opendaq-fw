How to setup the Arduino environment
====================================

First of all, you must get Arduino 1.0.4, or newer, installed.

Once you have it, go to the installation folder of Arduino. 

A couple of files have to be modified in the Arduino system directories to add the openDAQ board definitions and compile programs for it:

 * Add the openDAQ board definition (content of `boards.txt`) to the end of the `\Arduino\hardware\arduino\avr\boards.txt` file
 * Go to the `\Arduino\hardware\arduino\avr\variants\` folder and copy the two variant folders available in this repository (`\openDAQ_M` and `\openDAQ_S`)

Now, launch the Arduino IDE, and you should be able to select the board "openDAQ" in the menu `Tools->Boards`.

To compile the openDAQ package in Windows, you will need to download the project code directly to the Arduino work directory (usually in "My Documents"), where it stores all the user sketches. 
In any other case, you would face problems with the dependencies between files.

After these steps, you should be able to compile the firmware of openDAQ in the Arduino IDE. 

*This was tested with Arduino 1.6.0*


