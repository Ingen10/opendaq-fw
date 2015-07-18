openDAQ firmware
================

This is the firmware of the [openDAQ](http://www.open-daq.com) board. It is based
on [Arduino](http://arduino.cc/).

OpenDAQ is an USB measurement and automation device which provides
multifunction data-logging capabilities, analog inputs and outputs, digital
inputs and outputs, timer, counter and many more features.

You can control openDAQ using the [opendaq-python](http://opendaq-python.readthedocs.org/en/latest/opendaq.html) package, and using any of our GUI demos. 


How to setup the Arduino environment
-----------------------------------
First of all, you must get Arduino 1.0.4, or newer, installed. Once you have it, go to the installation folder of Arduino. 

A couple of files have to be modified in the Arduino system directories to add the openDAQ board definitions and compile programs for it:

 * Add the openDAQ board definition (content of `boards.txt`) to the end of the `\Arduino\hardware\arduino\avr\boards.txt` file
 * Go to the `\Arduino\hardware\arduino\avr\variants\` folder and copy the two variant folders available in this repository (`\openDAQ_M` and `\openDAQ_S`)

Now, launch the Arduino IDE, and you should be able to select the board "openDAQ" in the menu `Tools->Boards`.

To compile the openDAQ package in Windows, you will need to download the project code directly to the Arduino work directory (usually in "My Documents"), where it stores all the user sketches. 
In any other case, you would face problems with the dependencies between files.

After these steps, you should be able to compile the firmware of openDAQ in the Arduino IDE. 

*This was tested with Arduino 1.6.0*

In Debian/Ubuntu (use superuser rights):

```bash
apt-get install arduino
cp -r arduino/variants /usr/share/arduino/hardware/arduino/
cat arduino/board.txt >> /usr/share/arduino/hardware/arduino/boards.txt
```

If you love the command-line way of living, the provided Makefile will let you
build the project and flash your board without opening the Arduino IDE.
Simply call:

```bash
make
make program
```

If it doesn't work, maybe you'll have to tweak some definitions in the
Makefile for your specific environment.

Coding style
------------
Code is formatted with 4 space indentation. Tabs are expanded to spaces.

The code can be automatically formated using [astyle](http://astyle.sourceforge.net/):

```bash
astyle --style=stroustrup -p -c -s4 *.cpp *.h
```
