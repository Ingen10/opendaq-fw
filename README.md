openDAQ firmware
================

This is the firmware of the [openDAQ](http://open-daq.com) board.
It uses the lastest [Arduino](http://arduino.cc/) environment.

OpenDAQ is an USB based measurement and automation device, which provides user
with multifunction data-logging capabilities such as analog inputs and outputs,
digital inputs and outputs, timer, counter and many more features.


How to setup the Arduino environment
-----------------------------------
A couple of files have to be modified in the Arduino system directoies to add
the openDAQ board:

 * Add the openDAQ board definition to the end of the `boards.txt` file
 * Create a new "variant" folder with the pin definition file `pins_arduino.h`

In Debian/Ubuntu (use superuser rights):

```bash
apt-get install arduino
mkdir /usr/share/arduino/hardware/arduino/variants/opendaq
cp arduino/pins_arduino.h /usr/share/arduino/hardware/arduino/variants/opendaq/
cat arduino/board.txt >> /usr/share/arduino/hardware/arduino/boards.txt
```

Now you can select the board "openDAQ" in the menu Tools->Boards of the IDE.

If you love the terminal way of living, the provided Makefile will let you
build the project and flash your board without opening the Arduino IDE.
Simply call:

```bash
make
make program
```

If it doesn't work, maybe you'll have to tweak some definitions inside the
Makefile for your specific environment.


Coding style
------------
Code is formatted with 4 space indentation. Tabs are expanded to spaces.

The code can be automatically formated using [astyle](http://astyle.sourceforge.net/):

```bash
astyle --style=stroustrup -p -c -s4 *.cpp *.h
```
