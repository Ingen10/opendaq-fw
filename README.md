openDAQ firmware
================

This is the firmware of the [openDAQ](http://open-daq.com) board. It is based
on [Arduino](http://arduino.cc/).

The complete documentation is [here](http://opendaq.readthedocs.io/en/latest/).

OpenDAQ is an USB measurement and automation device which provides
multifunction data-logging capabilities, analog inputs and outputs, digital
inputs and outputs, timer, counter and many more features.

You can control openDAQ using the
[opendaq-python](http://opendaq-python.readthedocs.org/en/latest/opendaq.html)
package, or using any of the [GUI demos](https://github.com/openDAQ/opendaq-gui).

How to setup the Arduino IDE
------------------------------------
First of all, you must have Arduino 1.6.0 or newer installed.

* In the Arduino IDE add the following link to the
  File->Preferences->Additional Boards Manager URLs:
  https://raw.githubusercontent.com/openDAQ/opendaq-fw/master/package_opendaq.com_index.json

* Now go to Tools->Board->Board Manager, select openDAQ in "Contributed" and
  click install

* Select the board "openDAQ" in the menu Tools->Boards of the IDE.

Using Make (optional)
---------------------
If you prefer using the command line, the provided Makefile will let you build
the project and flash your board without opening the Arduino IDE. You will need
to have the Arduino IDE and
[Arduino-Makefile](https://github.com/sudar/Arduino-Makefile) installed.

In Debian/Ubuntu, you can install Arduino-Makefile from the default apt repository:

    sudo apt-get install arduino-mk

You may need to modify some variables inside de provided Makefile depending on
your OS and Arduino setup. Please read the
[documentation](https://github.com/sudar/Arduino-Makefile) of Arduino-Makefile.

If everything is OK, type `make upload`. The code should be compiled and your
OpenDAQ programmed.

Coding style
------------
Code is formatted with 4 space indentation. Tabs are expanded to spaces.

The code can be automatically formated using [astyle](http://astyle.sourceforge.net/):

```
astyle --style=stroustrup -p -c -s4 *.cpp *.h
```
