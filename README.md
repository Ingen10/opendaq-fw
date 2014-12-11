openDAQ firmware
================

This is the firmware of the [openDAQ](http://open-daq.com) board. It is based
on [Arduino](http://arduino.cc/).

OpenDAQ is an USB measurement and automation device which provides
multifunction data-logging capabilities, analog inputs and outputs, digital
inputs and outputs, timer, counter and many more features.


How to setup the Arduino environment
-----------------------------------
A couple of files have to be modified in the Arduino system directories to add
the openDAQ board:

 * Add the openDAQ board definition to the end of the `boards.txt` file
 * Create two new "variant" folders containing the pin definition files
   of the two openDAQ models ("M" and "S")

In Debian/Ubuntu (use superuser rights):

```bash
apt-get install arduino
cp -r arduino/variants /usr/share/arduino/hardware/arduino/
cat arduino/board.txt >> /usr/share/arduino/hardware/arduino/boards.txt
```

Now you can select the board "openDAQ" in the menu Tools->Boards of the IDE.

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
