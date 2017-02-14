.. _`Serial protocol`:

Serial protocol
===============

For reasons of compatibility with Arduino_ platform, USB handler uses a VCP
(Virtual COM Port) configuration. Thus, when the device is connected to a
computer, a new serial port is created, and it can be accessed as any other
serial port. In order to communicate with openDAQ, the following settings must
be configured in the terminal program that you are using:

- Rate: 115200 baud
- Data Bits: 8
- Parity: None
- Stop Bits: 1
- Flow Control: None

Following is a description of openDAQ’s low level protocol. Device communicates
with host computer by means of a binary serial protocol. There are two kinds of
commands used by openDAQ:

- `Regular command format`_, used for most of the commands.
- `Stream data format`_, used by the STREAMDATA_ packet to transmit large
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
big-endian format (higher bytes first), while Arduino stores variables in its
memory in **little-endian** format (lower bytes first).

======= ==================== =============  ====================================================
Byte    Description             Value       Notes
------- -------------------- -------------  ----------------------------------------------------
0       CRC16H
1       CRC16L                              Sum of all bytes 2-63 complemented with ``0xFFFF``
2       Command number          1-255
3       Number of data bytes    0-60        Total amount of data bytes excluding header
4:63    Data                                16 bit words represented in **big-endian** format
======= ==================== =============  ====================================================

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
data, but instead it will send that data in packets as fast as possible. In
order to keep synchronization between openDAQ and the host computer, this
packets use a special byte (``0x7E``) for marking the start of a packet.
No other byte in the packet must have this same value, so we have to `escape`
this value. When another ``0x7E`` must be transmitted inside the packet, the
openDAQ will substitute it by ``0x7D 0x5E``. In the same way, the byte ``0x7D``
will be transmitted as ``0x7D 0x5D``.

**Stream data packet:**

======= =================== ===========  ====================================================
Byte    Description         Value        Notes
------- ------------------- -----------  ----------------------------------------------------
0       Init byte           0x7E
1       unused              0
2       unused              0
3       Command number      25
4       Number of bytes                  Size of the packet excluding this header (5 bytes)
5       Channel number      1-4          Number of the DataChannel used
6       Positive input      1-8          Positive/SE analog input (default 5)
7       Negative input      0, 5-8, 25   Negative analog input (default 0)
8       Gain index          0-4
9-57    Data                             Data points (16 bit) stored for that DataChannel
======= =================== ===========  ====================================================

**Stream stop packet:**

======= =================== =======
Byte    Description         Value
------- ------------------- -------
0       Init byte           0x7E
1       unused              0
2       unused              0
3       Command number      80
4       Number of bytes     0
======= =================== =======
