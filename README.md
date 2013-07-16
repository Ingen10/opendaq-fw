openDAQ firmware
================

Howto setup the Arduino environment
-----------------------------------

In Debian/Ubuntu:

```bash
sudo su
apt-get install arduino
mkdir /usr/share/arduino/hardware/arduino/variants/opendaq
cp arduino/pins_arduino.h /usr/share/arduino/hardware/arduino/variants/opendaq/
cat arduino/board.txt >> /usr/share/arduino/hardware/arduino/boards.txt
exit
```

If you prefer the terminal way, the provided Makefile will let you build the
project and flash your hardware without using the Arduino IDE.

```bash
make
make program
```
