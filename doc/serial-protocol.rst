
.. _`Serial protocol`:


Serial protocol
===============

For reasons of compatibility with Arduino_ platform, USB handler uses a VCP
(Virtual COM Port) configuration. Thus, when the device is connected to a
computer, a new COM port is created, and it can be accessed as any other serial
port. In order to communicate with openDAQ, the following settings must be
configured in the terminal program that you are using:

- Rate: 115200 baud
- Data Bits: 8
- Parity: None
- Stop Bits: 1
- Flow Control: None

Following is a description of openDAQ’s low level protocol. Device communicates
with host computer by means of a binary serial protocol. There are two kinds of
commands used by openDAQ:

- `Regular command format`_, used for most of the commands.
- `Stream data format`_, used by the STREAM_DATA_ packet to transmit large
  chunks of data when the device is running in Stream mode.

Regular command format
----------------------
Normal openDAQ commands (aka Regular Commands) use a minimum of 4 bytes to
transmit a message to the device. Maximum allowed length for these packets is
64 bytes.

First two bytes consist on a 16 bit checksum or CRC. This checksum is
calculated doing the sum of all other bytes in the packet into a 16 bit
integer. Then, the result is complemented.

Be aware that some commands transmit 16 bit integers, and they do it in
big-endian_ format (higher bytes first), while Arduino stores variables in its
memory in **little-endian** format (lower bytes first).

======= ======================= =========== ===========
Byte    Description             Value       Notes
------- ----------------------- ----------- -----------
0       CRC16H
1       CRC16L                              Sum of all bytes 2-63 complemented with ``0xFFFF``
2       Command number          1-255
3       Number of data bytes    0-60        Total amount of data bytes (4-63)
4:63    Data                                16 bit words represented in **big-endian** format
======= ======================= =========== ===========

Device will react to all these commands with a response with the same command
number and different fields of data depending on the command. (Command-Response
mode)

Stream data format
------------------
Stream Data packets differ from normal commands because the device sends them
without being asked to do it, and no further response is expected.

When openDAQ is performing one or more simultaneous experiments, recorded data
should be transmitted as fast as possible in order not to saturate limited
internal memory buffers. The device can't wait for the computer to ask for
data, but instead it will send that data in packets as fast as possible.  In
order to keep synchronization between openDAQ and the host computer, this
packets use a method called `escaping characters`. An escape character is a
byte of data which invokes an alternative interpretation on subsequent
characters in a sequence. It is a common technique, used in many other serial
protocols, like PPP (the protocol used to control phone modems). 

In this case, we are using hexadecimal number ``0x7E`` (decimal 126) to indicate
the start of a new packet. To ensure that no other byte in the packet has the
same value, we use another escape character, ``0x7D`` (decimal 125). When this
character appears, it indicates that following byte must be modified in order
to be processed adequately. In this case, the character following a ``0x7D`` must
be exor'ed with ``0x20``. For example, ``0x7E`` data byte will be transmitted in
two bytes: ``0x7D 0x5E``.

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       Init Byte           0x7E
1       CRC16H
2       CRC16L                              Sum of all bytes complemented with ``0xFFFF``
3       Command number      25
4       Nb: Number of bytes
5       Number              1-4             Number of the DataChannel used
6       P-input             1-8             Positive/SE analog input (default 5)
7       N-input             0, 5-8, 25      Negative analog input (default 0)
8       Gain                0-4             00:x1/2, 01:x1, 02:x2, 03:x10, 04:x100,  (default 1)
9-57    Data                                Data points (16 bit) stored for that DataChannel
======= =================== ==============  ====================================================

Resume: All bytes with value ``0x7E`` or ``0x7D`` must be escaped (``byte ^ 0x20``)
and preceded by ``0x7D``, except init byte.

List of commands
----------------

================ ==== ================================================================================
Command          Code Description
---------------- ---- --------------------------------------------------------------------------------
AIN_             1    Read ADC (with current settings)
AIN_CFG_         2    Read ADC after configuring analog settings: positive input, negative input, gain
PIO_             3    Write/read PIO output: 1 or 0
PIO_DIR_         5    Configure PIO direction: 1 output, 0 input
PORT_            7    Write/read the port including all PIOS
PORT_DIR_        9    Configure/read all PIOs direction
LED_W_           18   Set LED color: 0=off, 1=green, 2=red, 3=orange
SET_DAC_         13   Set output voltage (-4096 to +4096mV)
PWM_INIT_        10   Init PWM: period, duty
PWM_STOP_        11   Disable PWM
PWM_DUTY_        12   Configure PWM duty
CAPTURE_INIT_    14   Start capture mode around a given period
CAPTURE_STOP_    15   Stop capture mode
GET_CAPTURE_     16   Get current period length: 0 (low cycle), 1(high cycle), 2(full period)
ENCODER_INIT_    50   Init encoder function
ENCODER_STOP_    51   Stop encoder function
GET_ENCODER_     52   Get current encoder relative position
COUNTER_INIT_    41   Initialize the edge counter (0 h_to_l, 1 l_to_h)
GET_COUNTER_     42   Get counter value (>0 resets accumulator)
EEPROM_WRITE_    30   Write a byte in EEPROM memory position
EEPROM_READ_     31   Read byte from EEPROM memory position
STREAM_CREATE_   19   Create stream experiment
EXTERNAL_CREATE_ 20   Create external experiment
BURST_CREATE_    21   Create burst experiment
STREAM_START_    64   Start an automated measurement
STREAM_STOP_     80   Stop current measurement
CHANNEL_SETUP_   32   Configure Experiment number of points
CHANNEL_CFG_     22   Configure one of the experiments (analog +IN,-IN, GAIN)
TRIGGER_SETUP_   33   Configure experiment trigger
CHANNEL_DESTROY_ 57   Delete Datachannel structure
CHANNEL_FLUSH_   45   Reset buffer of data in the Datachannel
STREAM_DATA_     25   Device writes a packet with measured data coming from one of the channels (response only)
SIGNAL_LOAD_     23   Load an array of values to preload DAC output
RESET_           27   System reset and restart
WAIT_MS_         17   Do nothing until a time has elapsed (milliseconds)
ID_CONFIG_       39   Read device config: serial number, firmware version, hardware version
GET_CALIB_       36   Read device calibration
SET_CALB_        37   Set device calibration
RESET_CALIB_     38   Reset device calibration
ENABLE_CRC_      55   Enable/disable cyclic redundancy check.
NACK_                 Invalid command (response only)
================ ==== ================================================================================


AIN
---
Description of the command

AIN_CFG
-------
Description of the command

PIO
---

PIO_DIR
-------

PORT
----

PORT_DIR
--------


LED_W
-----

SET_DAC
-------

PWM_INIT
--------

PWM_STOP
--------


PWM_DUTY
--------


CAPTURE_INIT
------------


CAPTURE_STOP
------------


GET_CAPTURE
-----------


ENCODER_INIT
------------


ENCODER_STOP
------------


GET_ENCODER
-----------


COUNTER_INIT
------------


GET_COUNTER
-----------


EEPROM_WRITE
------------


EEPROM_READ
-----------


STREAM_CREATE
-------------


EXTERNAL_CREATE
---------------


BURST_CREATE
------------


STREAM_START
------------


STREAM_STOP
-----------


CHANNEL_SETUP
-------------


CHANNEL_CFG
-----------


TRIGGER_SETUP
-------------


CHANNEL_DESTROY
---------------


CHANNEL_FLUSH
-------------

STREAM_DATA
-----------
See `Stream data format`_.

SIGNAL_LOAD
-----------


RESET
-----


WAIT_MS
-------


ID_CONFIG
---------


GET_CALIB
---------


SET_CALB
--------


RESET_CALIB
-----------


ENABLE_CRC
----------


NACK
----


.. _Arduino: http://www.arduino.cc
.. _big-endian: http://en.wikipedia.org/wiki/Endianness
