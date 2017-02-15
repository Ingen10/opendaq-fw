.. _`Serial protocol`:

===============
Serial protocol
===============

For reasons of compatibility with Arduino platform, USB handler uses a VCP
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

- `Regular format`_, used for most of the commands.
- `Stream format`_, used by the :ref:`STREAMDATA` and :ref:`STREAMSTOP`
  packets when the device is running in stream mode.

Regular format
==============
Regular openDAQ commands (command-response mode) use a minimum of 4 bytes to
transmit a message to the device. Maximum allowed length for these packets is
64 bytes.

First two bytes consist on a 16 bit checksum or CRC. This checksum is
calculated doing the sum of all other bytes in the packet into a 16 bit
integer. Then, the result is complemented.

Be aware that some commands transmit 16 bit integers, and they do it in
**big-endian** format (higher bytes first), while Arduino stores variables in
its memory in **little-endian** format (lower bytes first).

Device will react to all these commands with a response with the same command
number and different fields of data depending on the command.

See the :ref:`List of commands` for a complete description of each one.

Stream format
=============
Stream packets differ from regular commands because the device sends them
without being asked to do it, and no further response from the host computer is
expected.

This format is only used by the :ref:`STREAMDATA` and :ref:`STREAMSTOP` packets.

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
