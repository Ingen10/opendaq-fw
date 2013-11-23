
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
3       Number of data bytes    0-60        Total amount of data bytes excluding header
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

In this case, we use hexadecimal number ``0x7E`` (decimal 126) to indicate
the start of a new packet. To ensure that no other byte in the packet has the
same value, we use another escape character, ``0x7D`` (decimal 125). When this
character appears, it indicates that following byte must be modified in order
to be processed adequately. In this case, the character following a ``0x7D`` must
be xor'ed with ``0x20``. For example, ``0x7E`` data byte will be transmitted in
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

Summary: All bytes with value ``0x7E`` or ``0x7D`` must be escaped (``byte ^ 0x20``)
and preceded by ``0x7D``, except init byte.


List of commands
----------------

================ ==== ================================================================================
Command          Code Description
---------------- ---- --------------------------------------------------------------------------------
AIN_             1    Read ADC with current settings
AIN_CFG_         2    Read ADC after configuring analog settings: positive input, negative input, gain
PIO_             3    Write/read PIO output: 1 or 0
PIO_DIR_         5    Configure PIO direction: 1 output, 0 input
PORT_            7    Write/read the port including all PIOS
`PORT_DIR`_      9    Configure/read all PIOs direction
LED_W_           18   Set LED color. (0=off, 1=green, 2=red, 3=orange)
SET_DAC_         13   Set output voltage (-4096 to +4096mV)
SET_ANALOG_		 24   Set output voltage (-8192 to +8192)
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
SET_CALIB_       37   Set device calibration
RESET_CALIB_     38   Reset device calibration
ENABLE_CRC_      55   Enable/disable cyclic redundancy check.
SPISW_CONFIG_	 26   Bit bang spi configure (clock properties)
SPISW_SETUP_	 28	  Bit bang spi setup (pio numbers to use)
SPISW_TRANFER_	 29   Bit bang spi transfer (send+receive)
NACK_                 Invalid command (response only)
================ ==== ================================================================================


AIN
---
Read ADC with current settings.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      1
3       number of bytes     0
======= =================== ==============  ====================================================

**Response:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      1
3       number of bytes     2
4:5     value (byteH:byteL)                 ADC response (big-endian)
======= =================== ==============  ====================================================

AIN_CFG
-------
Read ADC after configuring analog settings: positive input, negative input, gain.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      2
3       number of bytes     4
4       p-input             1-8             Positive/SE analog input (default 5)
5       n-input             0, 5-8, 25      Negative analog input (default 0)
6       gain                0-4             00:x1/3, 01:x1, 02:x2, 03:x10, 04:x100, (default 1)
7       nsamples            1-255           Number of samples per point (default 20)
======= =================== ==============  ====================================================

**Response:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H
1       CRC16L
2       command number      2
3       number of bytes     6
4,5     value (byteH:byteL)                 ADC response (big-endian)
6       p-input             1-8
7       n-input             0, 5-8, 25
8       gain                0-4
9       nsamples            1-255
======= =================== ==============  ====================================================

PIO
---
Write/read PIO output: 1 or 0.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      3
3       number of bytes     1-2             1: read, 2: write
4       PIO number          1-6
5       value               0, 1            Low or high (+5 V) voltage output. Only when writing.
======= =================== ==============  ====================================================

**Response:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L
2       command number      3
3       number of bytes     2
4       PIO number          1-6
5       value               0, 1
======= =================== ==============  ====================================================

PIO_DIR
-------
Configure/read PIO direction: 1 output, 0 input.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      5
3       number of bytes     1-2             1: read, 2: configure
4       PIO number          1-6
5       direction           0, 1            Input (0) or output (1). Only when writing.
======= =================== ==============  ====================================================

**Response:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      5
3       number of bytes     2
4       PIO number          1-6
5       direction           0, 1
======= =================== ==============  ====================================================

PORT
----
Write/read all PIOS in a port.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      7
3       number of bytes     0-1             0: read port, 1: write port
4       value               0x00-0x3F       Output value of PIOs. Only when writing.
======= =================== ==============  ====================================================

**Response:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L
2       command number      7
3       number of bytes     1
4       value               0x00-0x3F       Value of PIOs
======= =================== ==============  ====================================================

PORT_DIR
--------
Configure/read all PIOs direction.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      9
3       number of bytes     0-1             0: read directions, 1: write directions
4       directions          0x00-0x3F       Direction of PIOs (0: input, 1: output). Only when writing.
======= =================== ==============  ====================================================

**Response:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L
2       command number      9
3       number of bytes     1
4       directions          0x00-0x3F
======= =================== ==============  ====================================================

LED_W
-----
Set LED color (0=off, 1=green, 2=red, 3=orange).

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      18
3       number of bytes     1
4       LED color           0-3             0: off, 1: green, 2: red, 3: orange
======= =================== ==============  ====================================================

**Response:** Same as command.

SET_DAC
-------
Set DAC output voltage (RAW value). DAC resolution depends on device model (14 bits for openDAQ [M], 12bits for openDAQ[S]).

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      13
3       number of bytes     2
4,5     value               -8192 to 8191   Signed word (16 bit) value for output voltage
======= =================== ==============  ====================================================

**Response**: Same as command.

SET_ANALOG
-------
Set DAC output voltage (RAW value). DAC resolution depends on device model (14 bits for openDAQ [M], 12bits for openDAQ[S]).

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      24
3       number of bytes     2
4,5     value               -8192 to 8191   Signed word (16 bit) value for output voltage
======= =================== ==============  ====================================================

**Response**: Same as command.

PWM_INIT
--------
Init PWM: period, duty.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      10              Starts PWM at the given frecuency and duty cycle
3       number of bytes     4               
4,5     PIO number          0:65535         Frecuency of the signal (microseconds)
5,6     duty                0:1023          High time for signal: 0 always low, 1023 always high
======= =================== ==============  ====================================================

**Response**: Same as command.

PWM_STOP
--------
Disable PWM.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      11				Stops PWM output
3       number of bytes     0
======= =================== ==============  ====================================================

**Response**: Same as command.

PWM_DUTY
--------
Configure PWM duty.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      10				
3       number of bytes     4
4,5     duty				0:1023			High time for signal: 0 always low, 1023 always high
======= =================== ==============  ====================================================

**Response**: Same as command.

SPISW_CONFIG
--------
Bit bang spi configure (clock properties).

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      26
3       number of bytes     2
4     	CPOL option         0:1             Clock polarity: clock pin state when inactive
5		CPHA option			0:1				Clock phase: data valid on clock leading (0) or trailing (1) edges   
======= =================== ==============  ====================================================

**Response**: Same as command.

SPISW_SETUP
--------
Bit bang spi setup (pio numbers to use).

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      28
3       number of bytes     0:3				0: Use default pin values (BBSCK=PIO1, BBMOSI=PIO2, BBMISO=PIO3)
4     	BBSCK pin number    1:6             Clock pin for bit bang SPI transfer
5		BBMOSI pin number 	1:6				Master out-Slave in pin for bit bang SPI transfer
6   	BBMISO pin number   1:6				Master in-Slave out pin for bit bang SPI transfer
======= =================== ==============  ====================================================

**Response**: Same as command.

SPISW_TRANSFER
--------
Bit bang spi transfer (send+receive).

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      29
3       number of bytes     1:64            Number of bytes to transmit via SPI (transmit or receive)
4:64    data to send		HEX				Bytes to transmit (MOSI output)
======= =================== ==============  ====================================================

**Response:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L
2       command number      29
3       number of bytes     1:64
4:64    data received		HEX				Bytes received in the transmission (MISO input)
======= =================== ==============  ====================================================

CAPTURE_INIT
------------
Start capture mode around a given period.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      14
3       number of bytes     2
4       period              0-65535         Aproximate period of the wave (microseconds)
======= =================== ==============  ====================================================

**Response:** Same as command.

CAPTURE_STOP
------------
Stop capture mode.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      15
3       number of bytes     0
======= =================== ==============  ====================================================

**Response:** Same as command.

GET_CAPTURE
-----------
Get current period length: 0 (low cycle), 1(high cycle), 2(full period).

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      16
3       number of bytes     1
4       edge                0-1-2           0 Low cycle, 1 High cycle, 2 Full period
======= =================== ==============  ====================================================

**Response:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      14
3       number of bytes     2
4       edge                0-1-2
5       period              0:65535         Period (microseconds)
======= =================== ==============  ====================================================

ENCODER_INIT
------------
Init encoder function.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      50
3       number of bytes     1
4       resolution          0:255           Max. number of ticks per round
======= =================== ==============  ====================================================

**Response:** Same as command.

ENCODER_STOP
------------
Stop encoder function.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      51
3       number of bytes     0
======= =================== ==============  ====================================================

**Response:** Same as command.

GET_ENCODER
-----------
Get current encoder relative position.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      52
3       number of bytes     0
======= =================== ==============  ====================================================

**Response:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      52
3       number of bytes     2
4		position			0:65535			Actual encoder value (must be<resolution)
======= =================== ==============  ====================================================

COUNTER_INIT
------------
Initialize the edge counter (0 h_to_l, 1 l_to_h).

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      41
3       number of bytes     1
4       edge                0-1             Which edge increments the count (1 L_to_H, 0 H_to_L)
======= =================== ==============  ====================================================

**Response:** Same as command.

GET_COUNTER
-----------
Get counter value (>0 resets accumulator).

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      42
3       number of bytes     1
4       reset count         0:1             1 reset accumulator
======= =================== ==============  ====================================================

**Response:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      42
3       number of bytes     2
4       count               0:65535         Number of edges actually detected
======= =================== ==============  ====================================================

EEPROM_WRITE
------------
Write a byte in EEPROM memory position.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      30
3       number of bytes     2
4       address             16:2000         Address of byte to write
5       data byte
======= =================== ==============  ====================================================

**Response:** Same as command.

EEPROM_READ
-----------
Read byte from EEPROM memory position.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      31
3       number of bytes     1
4       address             16:2000         Address of byte to be read
======= =================== ==============  ====================================================

**Response:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L
2       command number      31
3       number of bytes     2
4       address             16:2000         Address of byte
5       data byte                           Value of byte
======= =================== ==============  ====================================================

STREAM_CREATE
-------------
Create stream experiment.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      19
3       number of bytes     3
4       number              1:4             Number of DataChannel to assign
5:6     period	            1:65536         Period
======= =================== ==============  ====================================================

**Response:** Same as command.

EXTERNAL_CREATE
---------------
Create external experiment.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      20
3       number of bytes     2
4       number              1:4             Number of DataChannel to assign
5       edge                0-1
======= =================== ==============  ====================================================

**Response:** Same as command.

BURST_CREATE
------------
Create burst experiment.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      21				
3       number of bytes     2
4,5     period				100:65535		Period (microseconds)
======= =================== ==============  ====================================================

**Response**: Same as command.

STREAM_START
------------
Start an automated measurement.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      64
3       number of bytes     0
======= =================== ==============  ====================================================

**Response:** Same as command.

STREAM_STOP
-----------
Stop current measurement.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      80
3       number of bytes     0
======= =================== ==============  ====================================================

**Response:** Same as command.

CHANNEL_SETUP
-------------
Configure Experiment number of points.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      32
3       number of bytes     4
4       number              1:4             Number of DataChannel to assign
5:6     total of points     0:65536         0 indicates continuous acquisition
7       repetition mode     0:1             0:continuous, 1:run once
======= =================== ==============  ====================================================

**Response:** Same as command.

CHANNEL_CFG
-----------
Configure one of the experiments (analog +IN,-IN, GAIN).

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      22
3       number of bytes     5
4       number              1:4             Number of DataChannel to assign
5       mode                0:5             ANALOG_INPUT 0, ANALOG_OUTPUT 1, DIGITAL_INPUT 2, DIGITAL_OUTPUT 3, COUNTER_INPUT 4, CAPTURE_INPUT 5
6       p-input             1:8             Positive/SE analog input (default 5)
7       n-input             0, 25, 5:8      Negative analog input (default 0)
8       gain	            0:4             00:x1/3, 01:x1, 02:x2, 03:x10, 04:x100, (default 1)
9       number of samples   1:255           Number of samples per point
======= =================== ==============  ====================================================

**Response:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      22
3       number of bytes     5
4       number              1:4             
5       mode                1:5             
6       p-input             1:8             
7       n-input             0, 25, 5:8      
8       gain	            0:3, 8          
9       number of samples   1:255           
======= =================== ==============  ====================================================

TRIGGER_SETUP
-------------
Configure experiment trigger.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      33
3       number of bytes     4
4       number              1:4             Number of DataChannel to assign
5       trigger mode        1:5             SW_TRG 0-->Software trigger (run on start) DIN1_TRG 1-->Digital triggers, DIN2_TRG 2, DIN3_TRG 3, DIN4_TRG 4, DIN5_TRG 5, DIN6_TRG 6, ABIG_TRG 10-->Analog triggers (use current channel configuration: chp, chm, gain), ASML_TRG 20
6       trigger value       1:65535
======= =================== ==============  ====================================================

**Response:** Same as command.

CHANNEL_DESTROY
---------------
Delete Datachannel structure.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              
2       command number      57
3       number of bytes    	1
4		number				0:4				Number of DataChannel to clear (0=reset all DataChannels)
======= =================== ==============  ====================================================

**Response:** 

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              
2       command number      57
3       number of bytes    	3
4		number				0:4
======= =================== ==============  ====================================================

CHANNEL_FLUSH
-------------
Reset buffer of data in the Datachannel.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      45				Flush channel (empty all the buffers and reinitiale)
3       number of bytes     1
4       number				0:4				Number of DataChannel to flush(0 =reset all DataChannel)
======= =================== ==============  ====================================================

**Response**: Same as command.

STREAM_DATA
-----------
See `Stream data format`_.

SIGNAL_LOAD
-----------
Load an array of values to preload DAC output.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      23
3       number of bytes     2
4,5     number of data		1:400
6,N		data points		
======= =================== ==============  ====================================================

**Response:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      23
3       number of bytes     5
4,5		number of data		1:400
======= =================== ==============  ====================================================

RESET
-----
System reset and restart.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              
2       command number      27
3       number of bytes    	0
======= =================== ==============  ====================================================

**Response:** Same as command.

WAIT_MS
-------
Do nothing until a time has elapsed (milliseconds).

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              
2       command number      17
3       number of bytes    	2
4,5		time				0:65535			Period of time to wait without doing anything (miliseconds)
======= =================== ==============  ====================================================

**Response:** Same as command.

ID_CONFIG
---------
Read device config: serial number, firmware version, hardware version.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              
2       command number      39
3       number of bytes    	0
======= =================== ==============  ====================================================

**Response:** 

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              
2       command number      39
3       number of bytes    	4
4		hardware version	0:255
5		firmware version	0:255
6:9		Device serial #		0:65535
======= =================== ==============  ====================================================

GET_CALIB
---------
Read device calibration.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              
2       command number      36
3       number of bytes    	1
4		gain channel		0:4				00:x1/2, 01:x1, 02:x2, 03:x10, 04:x100, (default 1)
======= =================== ==============  ====================================================

**Response:** 

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              
2       command number      36
3       number of bytes    	5
4		gain channel		0:4
5,6     slope				0:65535 		Gain multiplied by 100000 (m=Slope/100000, 0 to 0.65)
7,8		offset				-32768:32768	Offset raw value
======= =================== ==============  ====================================================

SET_CALIB
--------
Set device calibration.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              
2       command number      37
3       number of bytes    	5
4		gain channel		0:4 			00:x1/2, 01:x1, 02:x2, 03:x10, 04:x100, (default 1)
5,6     slope				0:65535 		Gain multiplied by 100000 (m=Slope/100000, 0 to 0.65)
7,8		offset				-32768:32768	Offset raw value
======= =================== ==============  ====================================================

**Response:** Same as command.

RESET_CALIB
-----------
Reset device calibration.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              
2       command number      38
3       number of bytes    	1
4		gain channel		0:4				00:x1/2, 01:x1, 02:x2, 03:x10, 04:x100, (default 1)
======= =================== ==============  ====================================================

**Response:** 

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              
2       command number      38
3       number of bytes    	5
4		gain channel		0:4
5,6     slope				0:65535 		Gain multiplied by 100000 (m=Slope/100000, 0 to 0.65)
7,8		offset				-32768:32768	Offset raw value
======= =================== ==============  ====================================================

ENABLE_CRC
----------
Enable/disable cyclic redundancy check.

**Command:**

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              Sum of all bytes complemented with 0xFFFF
2       command number      55				Enables CRC16 validation for commands received in openDAQ
3       number of bytes     1
4      	CRC active			0:1				0: disabled 1: enabled
======= =================== ==============  ====================================================

**Response**: Same as command.

NACK
----
Invalid command (response only).

**Response:** 

======= =================== ==============  ====================================================
Byte    Description         Value           Notes
------- ------------------- --------------  ----------------------------------------------------
0       CRC16H              
1       CRC16L                              
2       command number      160
3       number of bytes    	0
======= =================== ==============  ====================================================


.. _Arduino: http://www.arduino.cc
.. _big-endian: http://en.wikipedia.org/wiki/Endianness
