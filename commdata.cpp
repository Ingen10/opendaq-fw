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

#include "commdata.h"
#include "debug.h"
#include "mcp23s17.h"

extern "C" {
#include <string.h>
#include <stdlib.h>
}

#define MAXSIZE 60

extern DStream ODStream;


// Constructors ////////////////////////////////////////////////////////////////

CommDataClass::CommDataClass(void)
{
    my_nsamples = 20;
    my_chp = 5;
    my_chn = 0;
    my_gain = 1;
    my_mode = 0;

    crc_enabled = 0;

    systemReset();
}


// Public Methods //////////////////////////////////////////////////////////////

// override general serial communication functions:

void CommDataClass::begin(void)
{
    begin(115200);
}


// begin method for overriding default serial bitrate
void CommDataClass::begin(long speed)
{
    Serial.begin(speed);
    //delay(300);
}


void CommDataClass::end()
{
    Serial.end();
}


int CommDataClass::available(void)
{
    return Serial.available();
}


int CommDataClass::read(void)
{
    return Serial.read();
}


void CommDataClass::flush(void)
{
    Serial.flush();
}


void CommDataClass::write(uint8_t c)
{
    Serial.write(c);
}


int CommDataClass::checkCRC()
{
    unsigned int check, suma = 0;

    if(!crc_enabled)
        return 1;

    for(int i = 2; i < received_bytes; i++)
        suma += input_data[i];

    return (my_crc16 == suma);
}


void CommDataClass::parseInput(int fl)
{
    int inputData = read();

    if(fl)
        inputData = UDR0;

    _DEBUG("%X ", inputData);

    input_data[received_bytes] = inputData;
    received_bytes++;
    received_bytes %= MAX_DATA_BYTES;

    if(received_bytes == 1)
        my_crc16 = inputData << 8;
    else if(received_bytes == 2)
        my_crc16 += inputData;
    else if(received_bytes == 3)
        next_command = inputData;
    else if(received_bytes == 4) {
        wait_for_data = inputData;
        if(wait_for_data > MAX_DATA_BYTES - 4)
            wait_for_data = 0;
        if ((next_command != C_SIGNAL_LOAD) && (wait_for_data > 10))
            wait_for_data = 0;
        data_len = wait_for_data;
        if(wait_for_data == 0) {
            processCommand();
            systemReset();
        }
    } else {
        if(wait_for_data > 0)
            wait_for_data--;
        if(wait_for_data == 0) {
            if(!checkCRC()) {
                ledSet(LEDGREEN, 0);
                ledSet(LEDRED, 1);
                systemReset();
                return;
            }
            processCommand();
            systemReset();
        }
    }
}


void CommDataClass::processStream(void)
{
    byte resp_len;
    int16_t value;
    byte response[MAX_DATA_BYTES];
    int len;
    int i, j;

    for (i=1; i<5; i++) {
    
        if(ODStream.ReadIndex(i) < ODStream.WriteIndex(i)) {
        //if(i==0){
            len = ODStream.WriteIndex(i) - ODStream.ReadIndex(i);
            len = (len > MAXSIZE) ? MAXSIZE : len;

            _DEBUG(" \r\n*%d-(w%d, r%d, ndata:%d, state:%d): [", i,
                    ODStream.WriteIndex(i), ODStream.ReadIndex(i),
                    ODStream.Ndata(i), ODStream.State(i));
#ifdef SERIAL_DEBUG
            for(j = 0; j < len; j++)
                _DEBUG("%d ", ODStream.Get(i));
            _DEBUG("]\r\n");
#else
            resp_len = 4 + 2 * len;

            for(j = 0; j < len; j++) {
                value = ODStream.Get(i);
                response[8 + 2 * j] = make8(value, 1);
                response[9 + 2 * j] = make8(value, 0);
            }

            response[2] = 25;
            response[3] = resp_len;  //number of bytes
            response[4] = i;         //channel #
            response[5] = ODStream.Pchan(i);
            response[6] = ODStream.Nchan(i);
            response[7] = ODStream.Gain(i);

            my_crc16 = crc16(resp_len + 2, response + 2);
            response[0] = make8(my_crc16, 1);
            response[1] = make8(my_crc16, 0);

            sendCommand(response, resp_len + 4);
#endif
        }
    }

    //End-of-experiment check
    for (i=1; i<5; i++) {
        if(ODStream.WriteIndex(i) > 0  &&
                ODStream.WriteIndex(i) == ODStream.ReadIndex(i) &&
                ODStream.endReached(i)) {
            //send stop
            resp_len = 1;
            response[2] = 80;
            response[3] = resp_len;  //number of bytes
            response[4] = 1;         //channel #1

            my_crc16 = crc16(resp_len + 2, response + 2);
            response[0] = make8(my_crc16, 1);
            response[1] = make8(my_crc16, 0);
            sendCommand(response, resp_len + 4);

            ODStream.Reset(i);
        }
    }
    
}


// Private Methods /////////////////////////////////////////////////////////////

void CommDataClass::processCommand(void)
{
    uint16_t word1, word2, word3;
    byte byte1, byte2, i;
    byte resp_len = 0;
    byte response[MAX_DATA_BYTES];

#ifdef SERIAL_DEBUG
    float f;

    _DEBUG("crc16: %X <> %X\n", my_crc16, crc16(data_len + 2, input_data + 2));
    _DEBUG("command: %X\n", next_command);
    _DEBUG("bytes: %X\n", data_len);
    for(i = 0; i < data_len; i += 2)
        _DEBUG("%X ", make16(input_data + i + 4));
    _DEBUG("\n");
#endif

    switch(next_command) {
    case C_AIN:
        word1 = ReadNADC(my_nsamples);

        response[4] = make8(word1, 1);
        response[5] = make8(word1, 0);
        resp_len = 2;

        _DEBUG("AIN: %d\n", word1);
        _DEBUG("P: %d ", my_chp);
        _DEBUG("N: %d ", my_chn);
        _DEBUG("GAIN: %d ", my_gain);
        _DEBUG("NSAMPLES: %d\n", my_nsamples);
        break;

    case C_AIN_CFG:
        my_chp = input_data[4];
        my_chn = input_data[5];
        if(data_len > 2)
            my_gain = input_data[6];
        if(data_len > 3)
            my_nsamples = input_data[7];

        ConfigAnalog(my_chp, my_chn, my_gain);
        ReadNADC(my_nsamples);
        word1 = ReadNADC(my_nsamples);

        response[4] = make8(word1, 1);
        response[5] = make8(word1, 0);
        response[6] = my_chp;
        response[7] = my_chn;
        response[8] = my_gain;
        response[9] = my_nsamples;
        resp_len = 6;

        _DEBUG("AIN_CFG: %d\n", word1);
        _DEBUG("P: %d ", my_chp);
        _DEBUG("N: %d ", my_chn);
        _DEBUG("GAIN: %d ", my_gain);
        _DEBUG("NSAMPLES: %d\n", my_nsamples);
        break;

    case C_SET_DAC:
        my_vout = make16(input_data + 4);
        SetAnalogVoltage(my_vout);

        response[4] = make8(my_vout, 1);
        response[5] = make8(my_vout, 0);
        resp_len = 2;

        _DEBUG("C_SET_DAC: %d\n", my_vout);
        break;

    case C_SET_ANALOG:
        my_vout = make16(input_data + 4);
        SetDacOutput(my_vout);

        response[4] = make8(my_vout, 1);
        response[5] = make8(my_vout, 0);
        resp_len = 2;

        _DEBUG("C_SET_ANALOG: %d\n", my_vout);
        break;

        ///// PIO & PORT COMMANDS  /////////////////////////////////////

    case C_PIO:
        byte1 = input_data[4];
        if(data_len > 1) {
            byte2 = input_data[5];
            pioWrite(byte1 - 1, byte2);
        } else
            byte2 = pioRead(byte1 - 1);

        response[4] = byte1;
        response[5] = byte2;
        resp_len = 2;

        _DEBUG("C_PIO: %d: %d\n", byte1, byte2);
        break;

    case C_PIO_DIR:
        byte1 = input_data[4];
        if(data_len > 1) {
            byte2 = input_data[5];
            if(byte2 != 0)
                SetpioMode(byte1 - 1, OUTPUT);
            else
                //pinMode(8,INPUT);
                SetpioMode(byte1 - 1, INPUT);
        } else
            byte2 = GetpioMode(word1 - 1);

        response[4] = byte1;
        response[5] = byte2;
        resp_len = 2;

        _DEBUG("C_PIO_DIR: %d: %d\n", byte1, byte2);
        break;

    case C_PORT:
        if(data_len > 0) {
            //TODO: verificar si los 1 o los 0 son las salidas
            byte1 = input_data[4];
            OutputDigital(byte1);
            byte1 = ReadDigital();
        } else
            byte1 = ReadDigital();

        response[4] = byte1;
        resp_len = 1;

        _DEBUG("C_PORT: %d\n", byte1);
        break;

    case C_PORT_DIR:
        if(data_len > 0) {
            byte1 = input_data[4];
            SetDigitalDir(byte1);
        } else
            byte1 = GetDigitalDir();

        response[4] = byte1;
        resp_len = 1;

        _DEBUG("C_PORT_DIR: %d\n", byte1);
        break;

        ///// EEPROM COMMANDS         /////////////////////////////////////

    case C_EEPROM_WRITE:
        byte1 = input_data[4];
        byte2 = input_data[5];
        Cal.write(byte1, byte2);

        memcpy(&response[4], &input_data[4], 2);
        resp_len = 2;

        _DEBUG("EEPROM_WRITE: [%d] = %d\n", byte1, byte2);
        break;

    case C_EEPROM_READ:
        byte1 = input_data[4];
        byte2 = Cal.read(byte1);

        memcpy(&response[4], &input_data[4], 2);
        resp_len = 2;

        _DEBUG("EEPROM_READ: [%d] = %d\n", byte1, byte2);
        break;

    case C_ID_CONFIG:
        if(data_len == ID_OVERWRITE) {
            my_id = make32(input_data + 4);
            Cal.ID_Save(my_id);
        }

        my_id = Cal.ID_Recall();

        response[4] = HW_VERSION;
        response[5] = FVER;
        memcpy(&response[6], &my_id, 4);
        resp_len = 6;

        _DEBUG("C_ID_CONFIG: ID = %d  x%X\n", my_id, my_id);
        break;

    case C_GET_CALIB:
    case C_SET_CALIB:
    case C_RESET_CALIB:
        resp_len = 5;

        byte1 = input_data[4];       //gain channel
        response[4] = byte1;

        if(next_command == C_SET_CALIB) {
            word1 = make16(input_data + 5);
            Cal.gain_m[byte1] = word1;
            word2 = make16(input_data + 7);
            Cal.gain_b[byte1] = word2;
            Cal.SaveCalibration();
        } else if(next_command == C_RESET_CALIB) {
            Cal.Reset_calibration();
            Cal.SaveCalibration();
        }

        word1 = Cal.gain_m[byte1];
        word2 = Cal.gain_b[byte1];

        response[5] = make8(word1, 1);
        response[6] = make8(word1, 0);
        response[7] = make8(word2, 1);
        response[8] = make8(word2, 0);

#ifdef SERIAL_DEBUG
        f = word1/10000.0;
        _DEBUG("C_xxx_CALIB: g%d: m=%f (m=%d) : b=%d\n", byte1, f, word1, word2);
#endif
        break;

        ///// COUNTER COMMANDS        /////////////////////////////////////
    case C_COUNTER_INIT:
        byte1 = input_data[4];
        counterInit(byte1);

        response[4] = byte1;
        resp_len = 1;

        _DEBUG("C_COUNTER_INIT: f_%d\n", byte1);
        break;

    case C_GET_COUNTER:
        byte1 = input_data[4];
        word1 = getCounter(byte1);

        memcpy(&response[4], &input_data[4], 2);
        resp_len = 2;

        _DEBUG("C_GET_COUNTER: %d\n", word1);
        break;

        ///// PWM COMMANDS       /////////////////////////////////////
    case C_PWM_INIT:
        word1 = make16(input_data + 4);
        word2 = make16(input_data + 6);
        pwmInit(word1, word2);

        memcpy(&response[4], &input_data[4], 4);
        resp_len = 4;

        _DEBUG("C_PWM_INIT: f_ %d , duty_ %d\n", word2, word1);
        break;

    case C_PWM_DUTY:
        word1 = make16(input_data + 4);
        setPwmDuty(word1);

        memcpy(&response[4], &input_data[4], 4);
        resp_len = 2;

        _DEBUG("C_PWM_DUTY: %d\n", word1);
        break;

    case C_PWM_STOP:
        PwmStop();

        resp_len = 0;

        _DEBUG("C_PWM_STOP");
        break;

    ///// CAPTURE COMMANDS        //////////////////////////////////////
    case C_CAPTURE_INIT:
        word1 = make16(input_data + 4);

        captureInit(word1);

        memcpy(&response[4], &input_data[4], 4);
        resp_len = 2;

        _DEBUG("C_CAPTURE_INIT: T = %d\n", word1);
        break;

    case C_CAPTURE_STOP:
        captureStop();

        resp_len = 0;

        _DEBUG("C_CAPTURE_STOP");
        break;

    case C_GET_CAPTURE:
        byte1 = input_data[4];
        word1 = getCapture(byte1);

        response[4] = byte1;
        response[5] = make8(word1, 1);
        response[6] = make8(word1, 0);
        resp_len = 3;

        _DEBUG("C_GET_CAPTURE (%d): %d\n", byte1, word1);
        break;

    ///// ENCODER COMMANDS        //////////////////////////////////////
    case C_ENCODER_INIT:
        word1 = input_data[4];

        response[4] = word1;
        resp_len = 1;

        encoder.Start(word1);
        _DEBUG("C_ENCODER_INIT: T = %d\n", byte1, word1);
        break;

    case C_ENCODER_STOP:
        encoder.Stop();

        resp_len = 0;

        _DEBUG("C_ENCODER_STOP");
        break;

    case C_GET_ENCODER:
        word1 = encoder.get_position();

        response[4] = make8(word1, 1);
        response[5] = make8(word1, 0);
        resp_len = 2;

        _DEBUG("C_GET_ENCODER: (%d): %d\n", byte1, word1);
        break;

    ///// STREAM CONTROL COMMANDS      ///////////////////////////////////
    case C_STREAM_CREATE:
        byte1 = input_data[4];
        word1 = make16(input_data + 5);

        memcpy(&response[4], &input_data[4], 3);
        resp_len = 3;

        ODStream.CreateStreamChannel(byte1, word1);

        _DEBUG("C_STREAM_CREATE [ %d ] => f(ms)= %d\n", byte1, word1);
        break;

    case C_BURST_CREATE:
        word1 = make16(input_data + 4);

        memcpy(&response[4], &input_data[4], 2);
        resp_len = 2;

        ODStream.CreateBurstChannel(word1);

        _DEBUG("C_BURST_CREATE => f(us)= %d\n", word1);
        break;

    case C_EXTERNAL_CREATE:
        byte1 = input_data[4];
        byte2 = (data_len > 1) ? input_data[5] : 1;
        ODStream.CreateExternalChannel(byte1, byte2);

        response[4] = byte1;
        response[5] = byte2;
        resp_len = 2;

        _DEBUG("C_EXTERNAL_CREATE [ %d ] => edge= %d\n", byte1, byte2);
        break;

    case C_STREAM_START:
        ODStream.Start();
        resp_len = 0;

        _DEBUG("C_STREAM_START\n");
        break;

    case C_STREAM_STOP:
        ODStream.Stop();
        resp_len = 0;

        _DEBUG("C_STREAM_STOP\n");
        break;


    case C_CHANNEL_FLUSH:
        byte1 = input_data[4];
        ODStream.FlushChan(byte1);

        response[4] = byte1;
        resp_len = 1;

        _DEBUG("C_CHANNEL_FLUSH: %d\n", byte1);
        break;


    case C_CHANNEL_DESTROY:
        byte1 = input_data[4];
        ODStream.DeleteExperiments(byte1);

        response[4] = byte1;
        resp_len = 1;

        _DEBUG("C_CHANNEL_DESTROY: %d\n", byte1);
        break;

    ///// DATA CHANNEL CONFIGURATION        //////////////////////////////////////
    case C_CHANNEL_SETUP:
        byte1 = input_data[4]; //Nb of channel
        word1 = make16(input_data + 5);         //nb of total points
        byte2 = input_data[7];  //repetition setup
        ODStream.SetupChan(byte1, word1, byte2);

        memcpy(&response[4], &input_data[4], 4);
        resp_len = 4;

        _DEBUG("C_CHANNEL_SETUP [ %d ] => Nb= %d repeat=%d\n", byte1, word1, byte2);
        break;

    case C_CHANNEL_CFG:
        byte1 = input_data[4]; //Nb of channel
        my_mode = input_data[5];
        my_chp = input_data[6];
        my_chn = input_data[7];
        if(data_len > 4)
            my_gain = input_data[8];
        if(data_len > 5)
            my_nsamples = input_data[9];
        ODStream.ConfigChan(byte1, my_mode, my_chp, my_chn, my_gain);

        memcpy(&response[4], &input_data[4], 4);
        response[8] = my_gain;
        response[9] = my_nsamples;
        resp_len = 6;

        _DEBUG("C_CHANNEL_CFG [ %d ] => mode: %d P: %d N: %d GAIN: %d\n",
                byte1, my_mode, my_chp, my_chn, my_gain);
        break;

    case C_TRIGGER_SETUP:
        byte1 = input_data[4]; //Nb of channel
        byte2 = input_data[5];  //repetition setup
        word1 = make16(input_data + 6);         //nb of total points
        ODStream.TriggerMode(byte1, byte2, word1);

        memcpy(&response[4], &input_data[4], 4);
        resp_len = 4;

        _DEBUG("C_TRIGGER_SETUP [ %d ] => mode: %d value: %d\n", byte1, byte2, word1);
        break;

    case C_SIGNAL_LOAD:

        word1 = make16(input_data + 4);         //index init
        word2 = (input_data[3] - 2)/2;

        for(i = 0; i < word2; i++) {
            word3 = make16(input_data + 6 + 2*i);      //nb of total points
            ODStream.Put(4,i + word1,word3);            //fills channel 4 buffer with the input data
        }

        memcpy(&response[4], &input_data[4], 2);
        resp_len = 3;

        break;

        ///// ALTERNATIVE COMMANDS         //////////////////////////////////////
    case C_ENABLE_CRC:
        byte1 = input_data[4];
        crc_enabled = (byte1 > 0);

        response[4] = byte1;
        resp_len = 1;

        break;

    case C_LED_W:
        byte1 = input_data[4];
        ledSet(LEDGREEN, byte1 & 0x01);
        ledSet(LEDRED, byte1 & 0x02);

        response[4] = byte1;
        resp_len = 1;

        _DEBUG("C_LED_W: %d\n", byte1);
        break;

    case C_RESET:
        response[0] = 0;
        response[1] = C_RESET;
        response[2] = C_RESET;
        response[3] = 0;
        resp_len = 0;

        _DEBUG("C_RESET\n");
#ifndef SERIAL_DEBUG
        sendCommand(response, resp_len + 4);
#endif
        systemReset();
        break;

    case C_WAIT_MS:
        word1 = make16(input_data + 4);         //delay in ms
        for (i = 0; i < word1; i++)
            _delay_ms(1);

        response[4] = make8(word1, 1);
        response[5] = make8(word1, 0);
        resp_len = 2;

        _DEBUG("C_WAIT_MS: %d\n", word1);
        break;

    case C_MCP23S17:
        /* Emulate an SPI port using some PIOs for controlling a MCP23S17
         * port expander: PIO1=CLK, PIO2=MOSI, PIO4=SS */
        word1 = make16(input_data + 4);
        SetDigitalDir(0X0F);		    // set PIO 1-4 as outputs
        mcp23s17_write(PIO4, IODIR, 0);	    // set all GPIOs as outputs
        mcp23s17_write(PIO4, GPIO, word1);  // write 16 bits to GPIOs

        response[4] = make8(word1, 1);
        response[5] = make8(word1, 0);
        resp_len = 2;

        _DEBUG("C_MCP23S17: %d\n", word1);
        break;

    case NACK:
    default:
        break;
    }

    response[2] = next_command;
    response[3] = resp_len;

    my_crc16 = crc16(resp_len + 2, response + 2);
    response[0] = make8(my_crc16, 1);
    response[1] = make8(my_crc16, 0);

#ifdef SERIAL_DEBUG
    _DEBUG("Response: ");
    for(i = 0; i < resp_len + 4; i++)
        _DEBUG("%X ", response[i]);

    _DEBUG("\n*************\n");

#else
    for(i = 0; i < resp_len + 4; i++)
        Serial.write(response[i]);
#endif
}


// sends output buffer taking account of initial character x7E and escape character x7D
void CommDataClass::sendCommand(byte* response, int size)
{
    byte a;
    pioWrite(1, 1);
    Serial.write(0x7E);

    for(byte i = 0; i < size; i++) {
        a = response[i];

        if( (a == 0x7D) || (a == 0x7E) ) {
            Serial.write(0x7D);
            Serial.write(a ^ 0x20);
        } else {
            Serial.write(a);
        }
    }
    pioWrite(1, 0);
}


// resets the system state upon a SYSTEM_RESET message from the host software
void CommDataClass::systemReset(void)
{
    for(byte i = 0; i < MAX_DATA_BYTES; i++)
        input_data[i] = 0;

    wait_for_data = 0;
    data_len = 0;
    my_crc16 = 0;
    next_command = 0;
    received_bytes = 0;

}


uint16_t CommDataClass::crc16(int size, byte *packet)
{
    uint16_t crccode = 0;

    //Iterate through packet
    for(byte i = 0; i < size; i++)
        crccode += packet[i];

    //crccode ^= 0xFFFF;

    return crccode;
}


uint16_t CommDataClass::make16(byte* a)
{
    return ((uint16_t)(a[0]) << 8) | (uint16_t)(a[1]);
}


uint32_t CommDataClass::make32(byte* a)
{
    return ((uint32_t)(a[0]) << 24) | ((uint32_t)(a[1]) << 16) | \
           ((uint32_t)(a[2]) << 8)  | (uint32_t)(a[3]);
}


byte CommDataClass::make8(uint16_t a, byte position)
{
    byte aux;

    if(position == 1)
        aux = (a & 0xFF00) >> 8;
    else
        aux = a & 0x00FF;

    return aux;
}


CommDataClass Comm;
