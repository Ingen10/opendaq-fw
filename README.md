openDAQ firmware
================

This is the firmware of the [openDAQ](http://open-daq.com) board. It is based
on [Arduino](http://arduino.cc/).

OpenDAQ is an USB measurement and automation device which provides
multifunction data-logging capabilities, analog inputs and outputs, digital
inputs and outputs, timer, counter and many more features.


How to setup the Arduino environment
------------------------------------
A couple of files have to be modified in the Arduino system directories to add
the openDAQ board:

 * Add the openDAQ board definition to the end of the `boards.txt` file
 * Create two new "variant" folders containing the pin definition files
   of the two openDAQ models ("M" and "S")

In Debian/Ubuntu (using superuser rights):

```
apt-get install arduino
cp -r arduino/variants /usr/share/arduino/hardware/arduino/
cp bootloader/opendaq_boot.hex /usr/share/arduino/hardware/arduino/bootloaders/atmega/
cat arduino/boards.txt >> /usr/share/arduino/hardware/arduino/boards.txt
```

Now you can select the board "openDAQ" in the menu Tools->Boards of the IDE.

Using Make (optional)
---------------------
If you prefer using the command-line, the provided Makefile will let you build
the project and flash your board without opening the Arduino IDE. You will
need to have the Arduino IDE and
[Arduino-Makefile](https://github.com/sudar/Arduino-Makefile) installed.

The provided Makefile was tested in Ubuntu 14.04 with the Arduino and
Arduino-Makefile installed from the default APT packages.

You may need modify some variables inside de provided Makefile depending on your
OS and Arduino setup. Please read the documentation of the
[Arduino-Makefile project](https://github.com/sudar/Arduino-Makefile).

If everything is OK, type `make upload`. The code should be compiled and your
OpenDAQ programmed.


Coding style
------------
Code is formatted with 4 space indentation. Tabs are expanded to spaces.

The code can be automatically formated using [astyle](http://astyle.sourceforge.net/):

```
astyle --style=stroustrup -p -c -s4 *.cpp *.h
```
