
Serial commands
===============

.. _`List of commands`:


List of commands
----------------

===============  ========  ==================================================================================
Name               Number  Description
===============  ========  ==================================================================================
AIN_                    1  Read ADC with current settings
AINALL_                 4  Read all analog inputs
AINCFG_                 2  Read ADC after configuring analog settings
BURSTCREATE_           21  Create a burst experiment
CAPTUREINIT_           14  Initialize the capture mode
CAPTURESTOP_           15  Disable the the capture mode
CHANNELCFG_            22  Configure an experiment
CHANNELDESTROY_        57  Delete an experiment
CHANNELFLUSH_          45  Reset the data buffer of an experiment
CHANNELSETUP_          32  Setup an experiment
COUNTERINIT_           41  Initialize the edge counter
EEPROMREAD_            31  Read a byte from the EEPROM at a given position
EEPROMWRITE_           30  Write a byte in the EEPROM at a given position
ENABLECRC_             55  Enable/disable cyclic redundancy check
ENCODERINIT_           50  Initialize the encoder mode
ENCODERSTOP_           51  Disable the the encoder mode
EXTERNALCREATE_        20  Create an external experiment
GETCALIB_              36  Read a calibration register
GETCAPTURE_            16  Get current period length
GETCOUNTER_            42  Get current counter value
GETENCODER_            52  Get current encoder position
IDCONFIG_              39  Read device information
LEDW_                  18  Set LED color
NAK_                  160  Invalid command (response only)
PIO_                    3  Read/write PIO value
PIODIR_                 5  Read/write PIO direction
PORT_                   7  Read/write all PIOs
PORTDIR_                9  Read/write all PIO directions
PWMDUTY_               12  Configure PWM duty cycle
PWMINIT_               10  Initialize PWM
PWMSTOP_               11  Disable the PWM
RESET_                 27  Restart the device
RESETCALIB_            38  Reset a calibration register
SETCALIB_              37  Write a calibration register
SETDAC_                13  Set DAC output voltage
SIGNALLOAD_            23  Load an array of values to preload DAC output
SPISWCONFIG_           26  Configure bit-bang SPI (clock properties)
SPISWSETUP_            28  Configure bit-bang SPI (clock properties)
SPISWTRANSFER_         29  Transfer SPI data (send and receive)
STREAMCREATE_          19  Create a stream experiment
STREAMDATA_            25  This packet transmits the data captured by a running experiment (stream mode only)
STREAMSTART_           64  Start an automated experiment
STREAMSTOP_            80  This packet marks the end of a running experiment (stream mode only)
TRIGGERSETUP_          33  Setup a trigger experiment
===============  ========  ==================================================================================

.. _AIN:

AIN
---

Read ADC with current settings

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number        1
         3       1  Packet size           0  Number of bytes, excluding the header
==========  ======  ==============  =======  =====================================

**Response:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number        1
         3       1  Packet size          16  Number of bytes, excluding the header
         4      16  Value                    Raw value of analog input
==========  ======  ==============  =======  =====================================

.. _AINALL:

AINALL
------

Read all analog inputs

**Command:**

==========  ======  ==============  =======  ======================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  ======================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  4
         3       1  Packet size     2        Number of bytes, excluding the header
         4       1  Samples         1-255    Number of averaged samples per measure
         5       1  Gain index      0-4
==========  ======  ==============  =======  ======================================

**Response:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number        4
         3       1  Packet size          16  Number of bytes, excluding the header
         4      16  Values                   Raw value of all analog inputs
==========  ======  ==============  =======  =====================================

.. _AINCFG:

AINCFG
------

Read ADC after configuring analog settings

**Command:**

==========  ======  ==============  =======  ======================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  ======================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  2
         3       1  Packet size     4        Number of bytes, excluding the header
         4       1  Positive input           Positive/Single-ended analog input
         5       1  Negative input           Negative analog input
         6       1  Gain index      0-4
         7       1  Samples         1-255    Number of averaged samples per measure
==========  ======  ==============  =======  ======================================

**Response:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number        2
         3       1  Packet size           2  Number of bytes, excluding the header
         4       2  Read value               ADC response
==========  ======  ==============  =======  =====================================

.. _BURSTCREATE:

BURSTCREATE
-----------

Create a burst experiment

**Command:**

==========  ======  ==============  =========  =====================================
  Position    Size  Name            Value      Description
==========  ======  ==============  =========  =====================================
         0       2  CRC                        Cyclic redundancy check
         2       1  Command number  21
         3       1  Packet size     2          Number of bytes, excluding the header
         4       2  Period          100-65535  Period in microseconds
==========  ======  ==============  =========  =====================================

**Response:**

Same as command

.. _CAPTUREINIT:

CAPTUREINIT
-----------

Initialize the capture mode

**Command:**

==========  ======  ==============  =======  =======================================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =======================================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number       14
         3       1  Packet size           4  Number of bytes, excluding the header
         4       4  Period                   Approximate period of the wave, in microseconds (int32)
==========  ======  ==============  =======  =======================================================

**Response:**

Same as command

.. _CAPTURESTOP:

CAPTURESTOP
-----------

Disable the the capture mode

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number       15
         3       1  Packet size           0  Number of bytes, excluding the header
==========  ======  ==============  =======  =====================================

**Response:**

Same as command

.. _CHANNELCFG:

CHANNELCFG
----------

Configure an experiment

**Command:**

==========  ======  ==============  =======  ====================================================================================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  ====================================================================================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  22
         3       1  Packet size     6        Number of bytes, excluding the header
         4       1  Number          1-4      DataChannel number
         5       1  Mode            0-5      0=ANALOG_INPUT, 1=ANALOG_OUTPUT, 2=DIGITAL_INPUT, 3=DIGITAL_OUTPUT, 4=COUNTER_INPUT, 5=CAPTURE_INPUT
         6       1  Positive input           Positive/Single-ended analog input
         7       1  Negative input           Negative analog input
         8       1  Gain index      0-4
         9       1  Samples         1-255    Number of averaged samples per measure
==========  ======  ==============  =======  ====================================================================================================

**Response:**

Same as command

.. _CHANNELDESTROY:

CHANNELDESTROY
--------------

Delete an experiment

**Command:**

==========  ======  ==============  =======  =============================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =============================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  57
         3       1  Packet size     1        Number of bytes, excluding the header
         4       1  Number          0-4      DataChannel number. 0: delete all experiments
==========  ======  ==============  =======  =============================================

**Response:**

Same as command

.. _CHANNELFLUSH:

CHANNELFLUSH
------------

Reset the data buffer of an experiment

**Command:**

==========  ======  ==============  =======  =============================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =============================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  45
         3       1  Packet size     1        Number of bytes, excluding the header
         4       1  Number          0-4      DataChannel number. 0: flush all data buffers
==========  ======  ==============  =======  =============================================

**Response:**

Same as command

.. _CHANNELSETUP:

CHANNELSETUP
------------

Setup an experiment

**Command:**

==========  ======  ===============  =======  ===========================================
  Position    Size  Name             Value    Description
==========  ======  ===============  =======  ===========================================
         0       2  CRC                       Cyclic redundancy check
         2       1  Command number   32
         3       1  Packet size      4        Number of bytes, excluding the header
         4       1  Number           1-4      DataChannel number
         5       2  Points           0-65535  Number of points. 0: continuous acquisition
         7       1  Repetition mode  0, 1     0: continuous, 1: run once
==========  ======  ===============  =======  ===========================================

**Response:**

Same as command

.. _COUNTERINIT:

COUNTERINIT
-----------

Initialize the edge counter

**Command:**

==========  ======  ==============  =======  ================================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  ================================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  41
         3       1  Packet size     1        Number of bytes, excluding the header
         4       1  Edge            0, 1     Which edge increments the couter: 0=H->L, 1=L->H
==========  ======  ==============  =======  ================================================

**Response:**

Same as command

.. _EEPROMREAD:

EEPROMREAD
----------

Read a byte from the EEPROM at a given position

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  31
         3       1  Packet size     1        Number of bytes, excluding the header
         4       1  Address         16-2000  Memory address
==========  ======  ==============  =======  =====================================

**Response:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number       31
         3       1  Packet size           2  Number of bytes, excluding the header
         4       1  Address                  Memory address
         5       1  Value                    Data value
==========  ======  ==============  =======  =====================================

.. _EEPROMWRITE:

EEPROMWRITE
-----------

Write a byte in the EEPROM at a given position

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number       30
         3       1  Packet size           2  Number of bytes, excluding the header
         4       1  Address                  Memory address
         5       1  Value                    Data value
==========  ======  ==============  =======  =====================================

**Response:**

Same as command

.. _ENABLECRC:

ENABLECRC
---------

Enable/disable cyclic redundancy check

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  55
         3       1  Packet size     1        Number of bytes, excluding the header
         4       1  Number          0-4      0: disabled, 1: enabled
==========  ======  ==============  =======  =====================================

**Response:**

Same as command

.. _ENCODERINIT:

ENCODERINIT
-----------

Initialize the encoder mode

**Command:**

==========  ======  ==============  =======  =======================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =======================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number       50
         3       1  Packet size           4  Number of bytes, excluding the header
         4       4  Resolution               Maximum number of encoder edges (int32)
==========  ======  ==============  =======  =======================================

**Response:**

Same as command

.. _ENCODERSTOP:

ENCODERSTOP
-----------

Disable the the encoder mode

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number       51
         3       1  Packet size           0  Number of bytes, excluding the header
==========  ======  ==============  =======  =====================================

**Response:**

Same as command

.. _EXTERNALCREATE:

EXTERNALCREATE
--------------

Create an external experiment

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  20
         3       1  Packet size     2        Number of bytes, excluding the header
         4       1  Number          1-4      DataChannel number
         5       1  Edge            0, 1
==========  ======  ==============  =======  =====================================

**Response:**

Same as command

.. _GETCALIB:

GETCALIB
--------

Read a calibration register

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number       36
         3       1  Packet size           1  Number of bytes, excluding the header
         4       1  Address                  Calibration register address
==========  ======  ==============  =======  =====================================

**Response:**

==========  ======  ==============  ===============  =====================================
  Position    Size  Name            Value            Description
==========  ======  ==============  ===============  =====================================
         0       2  CRC                              Cyclic redundancy check
         2       1  Command number  36
         3       1  Packet size     5                Number of bytes, excluding the header
         4       1  Address                          Calibration register address
         5       2  Gain            -32768 to 32767  Gain calibration
         7       2  Offset          -32768 to 32767  Offset calibration
==========  ======  ==============  ===============  =====================================

.. _GETCAPTURE:

GETCAPTURE
----------

Get current period length

**Command:**

==========  ======  ==============  =======  ===========================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  ===========================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  16
         3       1  Packet size     1        Number of bytes, excluding the header
         4       1  Edge            0, 1, 2  0: low cycle, 1: high cycle, 2: full period
==========  ======  ==============  =======  ===========================================

**Response:**

==========  ======  ==============  =======  ===========================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  ===========================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  16
         3       1  Packet size     5        Number of bytes, excluding the header
         4       1  Edge            0, 1, 2  0: low cycle, 1: high cycle, 2: full period
         5       4  Value                    Period of the wave, in microseconds (int32)
==========  ======  ==============  =======  ===========================================

.. _GETCOUNTER:

GETCOUNTER
----------

Get current counter value

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  42
         3       1  Packet size     1        Number of bytes, excluding the header
         4       1  Reset count     0, 1     Reset the counter after measuring
==========  ======  ==============  =======  =====================================

**Response:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number       42
         3       1  Packet size           4  Number of bytes, excluding the header
         4       4  Count                    Number of counted edges (int32)
==========  ======  ==============  =======  =====================================

.. _GETENCODER:

GETENCODER
----------

Get current encoder position

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number       52
         3       1  Packet size           0  Number of bytes, excluding the header
==========  ======  ==============  =======  =====================================

**Response:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number       52
         3       1  Packet size           4  Number of bytes, excluding the header
         4       4  Position                 Current encoder value (int32)
==========  ======  ==============  =======  =====================================

.. _IDCONFIG:

IDCONFIG
--------

Read device information

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number       39
         3       1  Packet size           0  Number of bytes, excluding the header
==========  ======  ==============  =======  =====================================

**Response:**

==========  ======  ================  =======  =====================================
  Position    Size  Name              Value    Description
==========  ======  ================  =======  =====================================
         0       2  CRC                        Cyclic redundancy check
         2       1  Command number    39
         3       1  Packet size       4        Number of bytes, excluding the header
         4       1  Hardware version           Hardware version
         5       1  Firmware version           Firmware version
         6       2  Serial number     0-65535  Serial number
==========  ======  ================  =======  =====================================

.. _LEDW:

LEDW
----

Set LED color

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  18
         3       1  Packet size     2        Number of bytes, excluding the header
         4       1  LED color       0-3      0: off, 1: green, 2: red, 3: orange
         5       1  LED number      0        LED number (not used)
==========  ======  ==============  =======  =====================================

**Response:**

Same as command

.. _NAK:

NAK
---

Invalid command (response only)

**Response:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number      160
         3       1  Packet size           0  Number of bytes, excluding the header
==========  ======  ==============  =======  =====================================

.. _PIO:

PIO
---

Read/write PIO value

**Command (read):**

==========  ======  ==============  =======  =====================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  3
         3       1  Packet size     1        Number of bytes, excluding the header
         4       1  PIO number      1-6
==========  ======  ==============  =======  =====================================

**Command (write):**

==========  ======  ==============  =======  =====================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  3
         3       1  Packet size     2        Number of bytes, excluding the header
         4       1  PIO number      1-6
         5       1  Value           0, 1     PIO value
==========  ======  ==============  =======  =====================================

**Response:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  3
         3       1  Packet size     2        Number of bytes, excluding the header
         4       1  PIO number      1-6
         5       1  Value           0, 1     PIO value
==========  ======  ==============  =======  =====================================

.. _PIODIR:

PIODIR
------

Read/write PIO direction

**Command (read):**

==========  ======  ==============  =======  =====================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  5
         3       1  Packet size     1        Number of bytes, excluding the header
         4       1  PIO number      1-6
==========  ======  ==============  =======  =====================================

**Command (write):**

==========  ======  ==============  =======  =====================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  5
         3       1  Packet size     2        Number of bytes, excluding the header
         4       1  PIO number      1-6
         5       1  Direction       0, 1     PIO direction: 0=input, 1=output
==========  ======  ==============  =======  =====================================

**Response:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  5
         3       1  Packet size     2        Number of bytes, excluding the header
         4       1  PIO number      1-6
         5       1  Direction       0, 1     PIO direction: 0=input, 1=output
==========  ======  ==============  =======  =====================================

.. _PORT:

PORT
----

Read/write all PIOs

**Command (read):**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number        7
         3       1  Packet size           0  Number of bytes, excluding the header
==========  ======  ==============  =======  =====================================

**Command (write):**

==========  ======  ==============  =======  =====================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  7
         3       1  Packet size     1        Number of bytes, excluding the header
         4       1  Value           0, 1     Value of all PIOs
==========  ======  ==============  =======  =====================================

**Response:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  7
         3       1  Packet size     1        Number of bytes, excluding the header
         4       1  Value           0, 1     Value of all PIOs
==========  ======  ==============  =======  =====================================

.. _PORTDIR:

PORTDIR
-------

Read/write all PIO directions

**Command (read):**

==========  ======  ==============  =======  =====================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  9
         3       1  Packet size     1        Number of bytes, excluding the header
         4       1  PIO number      1-6
==========  ======  ==============  =======  =====================================

**Command (write):**

==========  ======  ==============  =======  ========================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  ========================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  9
         3       1  Packet size     2        Number of bytes, excluding the header
         4       1  PIO number      1-6
         5       1  Directions      0, 1     Directions of all PIOs: 0=input 1=output
==========  ======  ==============  =======  ========================================

**Response:**

==========  ======  ==============  =======  ========================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  ========================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  9
         3       1  Packet size     2        Number of bytes, excluding the header
         4       1  PIO number      1-6
         5       1  Directions      0, 1     Directions of all PIOs: 0=input 1=output
==========  ======  ==============  =======  ========================================

.. _PWMDUTY:

PWMDUTY
-------

Configure PWM duty cycle

**Command:**

==========  ======  ==============  =======  =======================================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =======================================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  12
         3       1  Packet size     2        Number of bytes, excluding the header
         4       2  Duty cycle      0-1023   High time of the signal: 0=always low, 1023=always high
==========  ======  ==============  =======  =======================================================

**Response:**

Same as command

.. _PWMINIT:

PWMINIT
-------

Initialize PWM

**Command:**

==========  ======  ==============  =======  =======================================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =======================================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  10
         3       1  Packet size     4        Number of bytes, excluding the header
         4       2  Frequency       0-65535  Frequency of the signal, in microseconds
         6       2  Duty cycle      0-1023   High time of the signal: 0=always low, 1023=always high
==========  ======  ==============  =======  =======================================================

**Response:**

Same as command

.. _PWMSTOP:

PWMSTOP
-------

Disable the PWM

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number       11
         3       1  Packet size           0  Number of bytes, excluding the header
==========  ======  ==============  =======  =====================================

**Response:**

Same as command

.. _RESET:

RESET
-----

Restart the device

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number       27
         3       1  Packet size           0  Number of bytes, excluding the header
==========  ======  ==============  =======  =====================================

**Response:**

Same as command

.. _RESETCALIB:

RESETCALIB
----------

Reset a calibration register

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number       38
         3       1  Packet size           1  Number of bytes, excluding the header
         4       1  Address                  Calibration register address
==========  ======  ==============  =======  =====================================

**Response:**

==========  ======  ==============  ===============  =====================================
  Position    Size  Name            Value            Description
==========  ======  ==============  ===============  =====================================
         0       2  CRC                              Cyclic redundancy check
         2       1  Command number  38
         3       1  Packet size     5                Number of bytes, excluding the header
         4       1  Address                          Calibration register address
         5       2  Gain            -32768 to 32767  Gain calibration
         7       2  Offset          -32768 to 32767  Offset calibration
==========  ======  ==============  ===============  =====================================

.. _SETCALIB:

SETCALIB
--------

Write a calibration register

**Command:**

==========  ======  ==============  ===============  =====================================
  Position    Size  Name            Value            Description
==========  ======  ==============  ===============  =====================================
         0       2  CRC                              Cyclic redundancy check
         2       1  Command number  37
         3       1  Packet size     5                Number of bytes, excluding the header
         4       1  Address                          Calibration register address
         5       2  Gain            -32768 to 32767  Gain calibration
         7       2  Offset          -32768 to 32767  Offset calibration
==========  ======  ==============  ===============  =====================================

**Response:**

Same as command

.. _SETDAC:

SETDAC
------

Set DAC output voltage

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number       13
         3       1  Packet size           2  Number of bytes, excluding the header
         4       2  Value                    Raw DAC value. 16 bits, signed
==========  ======  ==============  =======  =====================================

**Response:**

Same as command

.. _SIGNALLOAD:

SIGNALLOAD
----------

Load an array of values to preload DAC output

**Command:**

==========  ======  =================  =======  =================================================
  Position    Size  Name               Value    Description
==========  ======  =================  =======  =================================================
         0       2  CRC                         Cyclic redundancy check
         2       1  Command number     23
         3       1  Packet size        4        Number of bytes, excluding the header
         4       2  Number of samples  1-400    The packet size depends of this number of samples
         6       2  Sample list                 List of samples. Each one is a signed int16
==========  ======  =================  =======  =================================================

**Response:**

==========  ======  =================  =======  =====================================
  Position    Size  Name               Value    Description
==========  ======  =================  =======  =====================================
         0       2  CRC                         Cyclic redundancy check
         2       1  Command number     23
         3       1  Packet size        2        Number of bytes, excluding the header
         4       2  Number of samples  1-400    Number of uploaded samples
==========  ======  =================  =======  =====================================

.. _SPISWCONFIG:

SPISWCONFIG
-----------

Configure bit-bang SPI (clock properties)

**Command:**

==========  ======  ==============  =======  =================================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =================================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  26
         3       1  Packet size     2        Number of bytes, excluding the header
         4       1  CPOL            0, 1     Clock polarity: clock pin state when inactive
         5       1  CPHA            0, 1     Clock phase (reading edge): 0=leading, 1=trailing
==========  ======  ==============  =======  =================================================

**Response:**

Same as command

.. _SPISWSETUP:

SPISWSETUP
----------

Configure bit-bang SPI (clock properties)

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  28
         3       1  Packet size     3        Number of bytes, excluding the header
         4       1  BBSCK pin       1-6      Clock pin for bit-bang SPI transfer
         5       1  BBMOSI pin      1-6      MOSI pin
         6       1  BBMISO pin      1-6      MISO pin
==========  ======  ==============  =======  =====================================

**Response:**

Same as command

.. _SPISWTRANSFER:

SPISWTRANSFER
-------------

Transfer SPI data (send and receive)

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number       29
         3       1  Packet size           1  Number of bytes, excluding the header
         4       1  Data to send             Array of bytes to transfer
==========  ======  ==============  =======  =====================================

**Response:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number       29
         3       1  Packet size           1  Number of bytes, excluding the header
         4       1  Received data            Array of received bytes
==========  ======  ==============  =======  =====================================

.. _STREAMCREATE:

STREAMCREATE
------------

Create a stream experiment

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  19
         3       1  Packet size     3        Number of bytes, excluding the header
         4       1  Number          1-4      DataChannel number
         5       2  Period          1-65535  Reading period in microseconds
==========  ======  ==============  =======  =====================================

**Response:**

Same as command

.. _STREAMDATA:

STREAMDATA
----------

This packet transmits the data captured by a running experiment (stream mode only)

**Command:**

This packet is always sent by the openDAQ

**Response:**

==========  ======  ==============  =======  ====================================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  ====================================================
         0       1  Init byte       0x7e
         1       2  Unused
         3       1  Command number  25
         4       1  Packet size     6        Number of bytes, excluding the header
         5       1  Number          1-4      DataChannel number
         6       1  Positive input           Positive/Single-ended analog input
         7       1  Negative input           Negative analog input
         8       1  Gain index      0-4
         9       2  Sample list              List of captured samples. Each one is a signed int16
==========  ======  ==============  =======  ====================================================

.. _STREAMSTART:

STREAMSTART
-----------

Start an automated experiment

**Command:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name              Value  Description
==========  ======  ==============  =======  =====================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number       64
         3       1  Packet size           0  Number of bytes, excluding the header
==========  ======  ==============  =======  =====================================

**Response:**

Same as command

.. _STREAMSTOP:

STREAMSTOP
----------

This packet marks the end of a running experiment (stream mode only)

**Command:**

This packet is always sent by the openDAQ

**Response:**

==========  ======  ==============  =======  =====================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  =====================================
         0       1  Init byte       0x7e
         1       2  Unused
         3       1  Command number  80
         4       1  Packet size     1        Number of bytes, excluding the header
         5       1  Number          1-4      DataChannel number
==========  ======  ==============  =======  =====================================

.. _TRIGGERSETUP:

TRIGGERSETUP
------------

Setup a trigger experiment

**Command:**

==========  ======  ==============  =======  ====================================================================================================================================================================================================================================
  Position    Size  Name            Value    Description
==========  ======  ==============  =======  ====================================================================================================================================================================================================================================
         0       2  CRC                      Cyclic redundancy check
         2       1  Command number  33
         3       1  Packet size     4        Number of bytes, excluding the header
         4       1  Number          1-4      DataChannel number
         5       1  Trigger mode    1-5      SW_TRG 0-->Software trigger (run on start) DIN1_TRG 1-->Digital triggers, DIN2_TRG 2, DIN3_TRG 3, DIN4_TRG 4, DIN5_TRG 5, DIN6_TRG 6, ABIG_TRG 10-->Analog triggers (use current channel configuration: chp, chm, gain), ASML_TRG 20
         6       2  Trigger value   1-65535
==========  ======  ==============  =======  ====================================================================================================================================================================================================================================

**Response:**

Same as command
