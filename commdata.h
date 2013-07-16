/*
 *  Copyright (C) 2012 INGEN10 Ingenieria SL
 *  http://www.ingen10.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 2.1 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Version:  120507
 *  Author:   JRB
 */

#ifndef COMMDATA_H
#define COMMDATA_H

//#define SERIAL_DEBUG

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "HardwareSerial.h"
#include "daqhw.h"
#include "calibration.h"
#include "odstream.h"
#include "encoder.h"

extern DataChannel Channel1, Channel2, Channel3, Channel4;


#define MAX_DATA_BYTES 128 // max number of data bytes in messages

#define NACK            0xA0

#define C_AIN           1
#define C_AIN_CFG       2

#define C_PIO           3
#define C_PIO_DIR       5
#define C_PORT          7
#define C_PORT_DIR      9

#define C_LED_W         18          //Configure device LED status: bit 1 sets green LED, bit 2 sets red LED

#define C_SET_DAC       13
#define C_SET_ANALOG    24
#define C_EEPROM_WRITE  30
#define C_EEPROM_READ   31

#define C_COUNTER_INIT  41
#define C_GET_COUNTER   42

#define C_CAPTURE_INIT  14
#define C_CAPTURE_STOP  15
#define C_GET_CAPTURE   16

#define C_ENCODER_INIT  50
#define C_ENCODER_STOP  51
#define C_GET_ENCODER   52

#define C_PWM_INIT      10
#define C_PWM_STOP      11
#define C_PWM_DUTY      12

#define C_STREAM_CREATE   19    //Create stream channel
#define C_EXTERNAL_CREATE 20    //Create external channel
#define C_BURST_CREATE    21    //Create burst experiment

#define C_STREAM_START  64  //Start an automated measurement
#define C_STREAM_STOP   80  //Stop actual measurement
#define C_CHANNEL_FLUSH 45  //Resets buffer of data in the channel 

#define C_CHANNEL_CFG   22  //Configure one of the measurement experiments (analog +IN,-IN, GAIN)
#define C_CHANNEL_SETUP 32  //Configure experiment number of points and repeat mode
#define C_TRIGGER_SETUP 33  //Configure experiment initial trigger mode and conditions

#define C_SIGNAL_LOAD   23  //Load an array of values to preload DAC output

#define C_RESET         27  //System reset and restart                                                              
#define C_WAIT_MS       17  //Do nothing until a time has elapsed (milliseconds)                                                                

#define C_ID_CONFIG     39  //Read device config:serial number, firmware version, hardware version

#define C_GET_CALIB     36  //Read device calibration
#define C_SET_CALIB     37  //Set device calibration
#define C_RESET_CALIB   38  //Reset device calibration

#define C_CHANNEL_DESTROY 57    //Delete channel structure

#define C_STREAM_DATA   25  //Device writes a packet with measured data coming from one of the channels

#define C_ENABLE_CRC    55

//other constants
#define ID_OVERWRITE            4
#define CALIBRATION_OVERWRITE   5
#define CALIBRATION_RESET       3

/******************************************************************************
 * Class
 ******************************************************************************/

/* CommData Class defines all the variables and functions used for managing the
 * serial communications for openDAQ */

class CommDataClass {
private:
    // properties
    int checkCRC();
    byte receivedBytes;
    byte waitForData;   // shows how many input bytes are left in the message
    byte maxData;
    uint16_t my_crc16;  // received 16bit crc
    byte next_command;  // short command being executed
    uint8_t my_chp, my_chn, my_gain, my_nsamples, my_mode;
    int16_t my_vout;

    uint32_t my_id;

    int crcEnabled;

    //byte* response;
    //byte response[MAX_DATA_BYTES];

    byte storedInputData[MAX_DATA_BYTES]; // multi-byte data

    // methods
    void systemReset(void);
    uint16_t CRC_16(int size, byte* packet);

    uint16_t make16(byte* a);
    uint32_t make32(byte* a);

    byte make8(uint16_t a, byte position);

    void Process_Command(void);

    void Send_Command(byte* response, int size);

public:
    //! class constructor
    CommDataClass();

    //serial receive handling
    void parseInput(int fl);

    //serial stream transfer
    void Process_Stream(void);

    // override general serial communication functions
    void begin(long speed);
    void begin(void);
    void end();

    int available(void);
    int read(void);
    void flush(void);
    void write(uint8_t c);
};

extern CommDataClass Comm;
extern DStream ODStream;
extern Encoder encoder;

#endif
