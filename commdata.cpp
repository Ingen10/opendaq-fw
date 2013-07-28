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
#include "mcp23s17.h"

extern "C" {
#include <string.h>
#include <stdlib.h>
}

#define MAXSIZE 60

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

// begin method
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
    int i;
    unsigned int check, suma = 0;

    if(!crc_enabled)
        return 1;

    for(i = 2; i < received_bytes; i++)
        suma += input_data[i];

    return (my_crc16 == suma);
}


void CommDataClass::parseInput(int fl)
{
    int inputData = read();
    int i;
    if(fl)
        inputData = UDR0;

#ifdef SERIAL_DEBUG
    Serial.print(inputData, HEX);
    Serial.print(" ");
#endif

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
        max_data = wait_for_data;
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
    byte resp_len = 0;
    int16_t value;
    unsigned long longvalue;
    byte response[MAX_DATA_BYTES];
    int len;
    int i, j;
#ifdef SERIAL_DEBUG
    char text[50];
#endif

    for (i=1; i<5; i++) {
        if ((i == 1 || i == 4) && (channels[i].dcmode != ANALOG_OUTPUT))
            continue;

        if(channels[i].readindex < channels[i].writeindex) {
            len = channels[i].writeindex - channels[i].readindex;
            len = (len > MAXSIZE) ? MAXSIZE : len;

#ifdef SERIAL_DEBUG
            Serial.println("Processing Stream");
            Serial.print(len, DEC);
            sprintf(text, " *%d-(w%d, r%d, ndata:%d, state:%d): [", i,
                    channels[i].writeindex, channels[i].readindex,
                    channels[i].ndata, channels[i].state);
            Serial.write(text);

            for(j = 0; j < len; j++) {
                value = channels[i].Get();
                Serial.print(value, DEC);
                Serial.print(" ");
            }
            Serial.println("]");
#else
            resp_len = 4 + 2 * len;

            for(j = 0; j < len; j++) {
                value = channels[i].Get();
                response[8 + 2 * j] = make8(value, 1);
                response[9 + 2 * j] = make8(value, 0);
            }

            response[2] = 25;
            response[3] = resp_len;  //number of bytes
            response[4] = i;         //channel #
            response[5] = channels[i].pch;
            response[6] = channels[i].nch;
            response[7] = channels[i].g;

            my_crc16 = CRC_16(resp_len + 2, response + 2);
            response[0] = make8(my_crc16, 1);
            response[1] = make8(my_crc16, 0);

            PORTA = PORTA | 0x40;
            sendCommand(response, resp_len + 4);
            PORTA = PORTA & ~0x40;
#endif
        }
    }

    //End-of-experiment check
    for (i=1; i<5; i++) {
        if(channels[i].writeindex > 0  &&
                channels[i].writeindex == channels[i].readindex &&
                channels[i].endReached()) {
            //send stop
            resp_len = 1;
            response[2] = 80;
            response[3] = resp_len;  //number of bytes
            response[4] = 1;         //channel #1

            my_crc16 = CRC_16(resp_len + 2, response + 2);
            response[0] = make8(my_crc16, 1);
            response[1] = make8(my_crc16, 0);
            sendCommand(response, resp_len + 4);

            channels[i].reset();
        }
    }
}



// Private Methods /////////////////////////////////////////////////////////////

void CommDataClass::processCommand(void)
{
    byte i, j;
    uint16_t tdata;
    uint16_t temp;
    byte resp_len = 0;
    int16_t value;
    uint32_t aux32;
    byte response[MAX_DATA_BYTES];
#ifdef SERIAL_DEBUG
    char debug_text[50];
#endif

#ifdef SERIAL_DEBUG
    float f;

    Serial.println();

    Serial.print("crc16: ");
    Serial.print(my_crc16, HEX);
    Serial.print(" <> ");
    Serial.println(CRC_16(max_data + 2, input_data + 2), HEX);

    Serial.print("Command: ");
    Serial.println(next_command, HEX);
    Serial.print("bytes: ");
    Serial.println(max_data, HEX);

    for(i = 0; i < max_data; i += 2) {
        tdata = make16(input_data + i + 4);
        Serial.print(tdata, HEX);
        Serial.print(" ");
    }
    Serial.println();
#endif

    //response = (byte*) calloc(16,sizeof(byte));

    switch(next_command) {
    case C_AIN:
        resp_len = 2;

        //value = ReadAnalogIn(my_nsamples);
        value = ReadNADC(my_nsamples);
        response[4] = make8(value, 1);
        response[5] = make8(value, 0);
#ifdef SERIAL_DEBUG
        print_debug("AIN: %d\nP: %d\nN: %d\nGAIN: %d\nNSAMPLES: %d 
        Serial.print("AIN: ");
        Serial.println(value, DEC);
        Serial.print("P: ");
        Serial.print(my_chp, DEC);
        Serial.print(" N: ");
        Serial.print(my_chn, DEC);
        Serial.print(" GAIN: ");
        Serial.print(my_gain, DEC);
        Serial.print(" nSAMPLES: ");
        Serial.println(my_nsamples, DEC);
#endif
        break;

    case C_AIN_CFG:
        resp_len = 6;

        my_chp = input_data[4];
        my_chn = input_data[5];
        if(max_data > 2)
            my_gain = input_data[6];
        if(max_data > 3)
            my_nsamples = input_data[7];

        ConfigAnalog(my_chp, my_chn, my_gain);

        ReadNADC(my_nsamples);
        value = ReadNADC(my_nsamples);
        //value = ReadAnalogIn(my_nsamples);
        response[4] = make8(value, 1);
        response[5] = make8(value, 0);
        response[6] = my_chp;
        response[7] = my_chn;
        response[8] = my_gain;
        response[9] = my_nsamples;

#ifdef SERIAL_DEBUG
        Serial.print("AIN_CFG: ");
        Serial.println(value, DEC);
        Serial.print("P: ");
        Serial.print(my_chp, DEC);
        Serial.print(" N: ");
        Serial.print(my_chn, DEC);
        Serial.print(" GAIN: ");
        Serial.print(my_gain, DEC);
        Serial.print(" nSAMPLES: ");
        Serial.println(my_nsamples, DEC);
#endif
        break;

    case C_SET_DAC:
        resp_len = 2;

        my_vout = make16(input_data + 4);
        SetAnalogVoltage(my_vout);
        response[4] = make8(my_vout, 1);
        response[5] = make8(my_vout, 0);

#ifdef SERIAL_DEBUG
        Serial.print("C_SET_DAC: ");
        Serial.println(my_vout, DEC);
#endif
        break;

    case C_SET_ANALOG:
        resp_len = 2;

        my_vout = make16(input_data + 4);
        SetDacOutput(my_vout);
        response[4] = make8(my_vout, 1);
        response[5] = make8(my_vout, 0);

#ifdef SERIAL_DEBUG
        Serial.print("C_SET_Analog: ");
        Serial.println(my_vout, DEC);
#endif
        break;

        ///// PIO & PORT COMMANDS  /////////////////////////////////////

    case C_PIO:
        resp_len = 2;

        i = input_data[4];
        if(max_data > 1) {
            j = input_data[5];
            pioWrite(i - 1, j);
        } else {
            j = pioRead(i - 1);
        }
        response[4] = i;
        response[5] = j;

#ifdef SERIAL_DEBUG
        Serial.print("C_PIO ");
        Serial.print(i, DEC);
        Serial.print(": ");
        Serial.println(j, DEC);
#endif
        break;

    case C_PIO_DIR:
        resp_len = 2;

        i = input_data[4];
        if(max_data > 1) {
            j = input_data[5];
            if(j != 0)
                SetpioMode(i - 1, OUTPUT);
            else
                //pinMode(8,INPUT);
                SetpioMode(i - 1, INPUT);
        } else {
            j = GetpioMode(tdata - 1);
        }

        response[4] = i;
        response[5] = j;

#ifdef SERIAL_DEBUG
        Serial.print("C_PIO_DIR ");
        Serial.print(i, DEC);
        Serial.print(": ");
        Serial.println(j, DEC);
#endif
        break;


    case C_PORT:
        resp_len = 1;

        if(max_data > 0) {
            //TODO: verificar si los 1 o los 0 son las salidas
            j = input_data[4];
            OutputDigital(j);
            j = ReadDigital();
        } else
            j = ReadDigital();

        response[4] = j;

#ifdef SERIAL_DEBUG
        Serial.print("C_PORT: ");
        Serial.println(j, HEX);
#endif
        break;

    case C_PORT_DIR:
        resp_len = 1;

        if(max_data > 0) {
            j = input_data[4];
            SetDigitalDir(j);
        } else
            j = GetDigitalDir();

        response[4] = j;

#ifdef SERIAL_DEBUG
        Serial.print("C_PORT_DIR: ");
        Serial.println(j, HEX);
#endif
        break;

        ///// EEPROM COMMANDS         /////////////////////////////////////

    case C_EEPROM_WRITE:
        resp_len = 2;

        i = input_data[4];
        j = input_data[5];
        Cal.write(i, j);

        response[4] = i;
        response[5] = j;

#ifdef SERIAL_DEBUG
        Serial.print("EEPROM_WRITE: [ ");
        Serial.print(i, DEC);
        Serial.print(" ] = ");
        Serial.println(j, DEC);
#endif
        break;

    case C_EEPROM_READ:
        resp_len = 2;

        i = input_data[4];
        j = Cal.read(i);

        response[4] = i;
        response[5] = j;

#ifdef SERIAL_DEBUG
        Serial.print("EEPROM_READ: [ ");
        Serial.print(i, DEC);
        Serial.print(" ] = ");
        Serial.println(j, DEC);
#endif
        break;

    case C_ID_CONFIG:
        resp_len = 6;

        if(max_data == ID_OVERWRITE) {
            my_id = make32(input_data + 4);
            Cal.ID_Save(my_id);
        }

        my_id = Cal.ID_Recall();

        response[4] = HVER;
        response[5] = FVER;

        aux32 = (my_id & 0XFF000000) >> 24;
        response[6] = aux32;
        aux32 = (my_id & 0X00FF0000) >> 16;
        response[7] = aux32;
        aux32 = (my_id & 0X0000FF00) >> 8;
        response[8] = aux32;
        aux32 = (my_id & 0X000000FF);
        response[9] = aux32;

#ifdef SERIAL_DEBUG
        Serial.println("C_ID_CONFIG: ");
        Serial.print("ID =  ");
        Serial.print(my_id, DEC);
        Serial.print("   x");
        Serial.println(my_id, HEX);
#endif
        break;

    case C_GET_CALIB:
    case C_SET_CALIB:
    case C_RESET_CALIB:
        resp_len = 5;

        i = input_data[4];       //gain channel
        response[4] = i;

        if(next_command == C_SET_CALIB) {
            tdata = make16(input_data + 5);
            Cal.gain_m[i] = tdata;
            value = make16(input_data + 7);
            Cal.gain_b[i] = value;
            Cal.SaveCalibration();
        } else if(next_command == C_RESET_CALIB) {
            Cal.Reset_calibration();
            Cal.SaveCalibration();
        }

        tdata = Cal.gain_m[i];
        value = Cal.gain_b[i];

        response[5] = make8(tdata, 1);
        response[6] = make8(tdata, 0);
        response[7] = make8(value, 1);
        response[8] = make8(value, 0);

#ifdef SERIAL_DEBUG
        Serial.println("C_xxx_CALIB: ");
        Serial.print("g");
        Serial.print(i, DEC);
        Serial.print(":  m=");
        f = tdata;
        f /= 10000.0;
        Serial.print(f, 5);
        Serial.print(" (m=");
        Serial.print(tdata);
        Serial.print(") : b=");
        Serial.println(value);
#endif
        break;

        ///// COUNTER COMMANDS        /////////////////////////////////////
    case C_COUNTER_INIT:
        resp_len = 1;

        i = input_data[4];
        counterInit(i);
        response[4] = i;

#ifdef SERIAL_DEBUG
        Serial.print("C_COUNTER_INIT: f_");
        Serial.println(i, DEC);
#endif
        break;

    case C_GET_COUNTER:
        resp_len = 2;

        i = input_data[4];
        tdata = getCounter(i);
        response[4] = make8(tdata, 1);
        response[5] = make8(tdata, 0);

#ifdef SERIAL_DEBUG
        Serial.print("C_GET_COUNTER: ");
        Serial.println(tdata, DEC);
#endif
        break;

        ///// PWM COMMANDS       /////////////////////////////////////
    case C_PWM_INIT:
        resp_len = 4;

        value = make16(input_data + 4);
        tdata = make16(input_data + 6);
        pwmInit(value, tdata);

        response[4] = make8(value, 1);
        response[5] = make8(value, 0);
        response[6] = make8(tdata, 1);
        response[7] = make8(tdata, 0);

#ifdef SERIAL_DEBUG
        Serial.print("C_PWM_INIT: f_ ");
        Serial.print(tdata, DEC);
        Serial.print(" , duty_ ");
        Serial.println(value, DEC);
#endif
        break;

    case C_PWM_DUTY:
        resp_len = 2;

        value = make16(input_data + 4);
        setPwmDuty(value);

        response[4] = make8(value, 1);
        response[5] = make8(value, 0);

#ifdef SERIAL_DEBUG
        Serial.print("C_PWM_DUTY: ");
        Serial.println(value, DEC);
#endif
        break;

    case C_PWM_STOP:
        resp_len = 0;

        PwmStop();
#ifdef SERIAL_DEBUG
        Serial.println("C_PWM_STOP");
#endif
        break;
        ///// CAPTURE COMMANDS        //////////////////////////////////////
    case C_CAPTURE_INIT:
        resp_len = 2;

        tdata = make16(input_data + 4);
        response[4] = make8(tdata, 1);
        response[5] = make8(tdata, 0);

        captureInit(tdata);
#ifdef SERIAL_DEBUG
        Serial.print("C_CAPTURE_INIT: T = ");
        Serial.println(tdata, DEC);
#endif
        break;

    case C_CAPTURE_STOP:
        resp_len = 0;

        captureStop();
#ifdef SERIAL_DEBUG
        Serial.println("C_CAPTURE_STOP");
#endif
        break;

    case C_GET_CAPTURE:
        resp_len = 3;

        i = input_data[4];
        tdata = getCapture(i);
        response[4] = i;
        response[5] = make8(tdata, 1);
        response[6] = make8(tdata, 0);

#ifdef SERIAL_DEBUG
        Serial.print("C_GET_CAPTURE (");
        Serial.print(i, DEC);
        Serial.print("):  ");
        Serial.println(tdata, DEC);
#endif
        break;

        ///// ENCODER COMMANDS        //////////////////////////////////////
    case C_ENCODER_INIT:
        resp_len = 1;

        value = input_data[4];
        response[4] = value;

        encoder.Start(value);
#ifdef SERIAL_DEBUG
        Serial.print("C_ENCODEER_INIT: T = ");
        Serial.println(tdata, DEC);
#endif
        break;

    case C_ENCODER_STOP:
        resp_len = 0;

        encoder.Stop();
#ifdef SERIAL_DEBUG
        Serial.println("C_ENCODER_STOP");
#endif
        break;

    case C_GET_ENCODER:
        resp_len = 2;

        tdata = encoder.get_position();
        response[4] = make8(tdata, 1);
        response[5] = make8(tdata, 0);

#ifdef SERIAL_DEBUG
        Serial.print("C_GET_ENCODER (");
        Serial.print(i, DEC);
        Serial.print("):  ");
        Serial.println(tdata, DEC);
#endif
        break;

        ///// STREAM CONTROL COMMANDS      ///////////////////////////////////
    case C_STREAM_CREATE:
        resp_len = 3;

        i = input_data[4];
        tdata = make16(input_data + 5);

        response[4] = i;
        response[5] = make8(tdata, 1);
        response[6] = make8(tdata, 0);

        ODStream.CreateStreamChannel(i, tdata);

#ifdef SERIAL_DEBUG
        Serial.print("C_STREAM_CREATE [ ");
        Serial.print(i, DEC);
        Serial.print(" ] => f(ms)= ");
        Serial.println(tdata, DEC);
#endif
        break;

    case C_BURST_CREATE:
        resp_len = 2;

        tdata = make16(input_data + 4);

        response[4] = make8(tdata, 1);
        response[5] = make8(tdata, 0);

        ODStream.CreateBurstChannel(tdata);

#ifdef SERIAL_DEBUG
        Serial.print("C_BURST_CREATE => f(us)= ");
        Serial.println(tdata, DEC);
#endif
        break;

    case C_EXTERNAL_CREATE:
        resp_len = 2;

        i = input_data[4];
        if(max_data > 1)
            j = input_data[5];
        else
            j = 1;

        response[4] = i;
        response[5] = j;

        ODStream.CreateExternalChannel(i, j);

#ifdef SERIAL_DEBUG
        Serial.print("C_EXTERNAL_CREATE [ ");
        Serial.print(i, DEC);
        Serial.print(" ] => edge= ");
        Serial.println(j, DEC);
#endif
        break;

    case C_STREAM_START:
        resp_len = 0;

        ODStream.Start();
#ifdef SERIAL_DEBUG
        Serial.println("C_STREAM_START");
#endif
        break;

    case C_STREAM_STOP:
        resp_len = 0;

        ODStream.Stop();
#ifdef SERIAL_DEBUG
        Serial.println("C_STREAM_STOP");
#endif
        break;


    case C_CHANNEL_FLUSH:
        resp_len = 1;

        i = input_data[4];
        response[4] = i;

        ODStream.FlushChan(i);
#ifdef SERIAL_DEBUG
        Serial.println("C_CHANNEL_FLUSH: ");
        Serial.println(i, DEC);
#endif
        break;


    case C_CHANNEL_DESTROY:
        resp_len = 1;

        i = input_data[4];
        response[4] = i;

        ODStream.DeleteExperiments(i);
#ifdef SERIAL_DEBUG
        Serial.println("C_CHANNEL_DESTROY: ");
        Serial.println(i, DEC);
#endif
        break;

        ///// DATA CHANNEL CONFIGURATION        //////////////////////////////////////
    case C_CHANNEL_SETUP:
        resp_len = 4;

        i = input_data[4]; //Nb of channel
        tdata = make16(input_data + 5);         //nb of total points
        j = input_data[7];  //repetition setup

        response[4] = i;
        response[5] = make8(tdata, 1);
        response[6] = make8(tdata, 0);
        response[4] = j;

        ODStream.SetupChan(i, tdata, j);

#ifdef SERIAL_DEBUG
        Serial.print("C_CHANNEL_SETUP [ ");
        Serial.print(i, DEC);
        Serial.print(" ] => Nb= ");
        Serial.print(tdata, DEC);
        Serial.print(" repeat= ");
        Serial.println(j, DEC);
#endif
        break;


    case C_CHANNEL_CFG:
        resp_len = 6;

        i = input_data[4]; //Nb of channel
        my_mode = input_data[5];
        my_chp = input_data[6];
        my_chn = input_data[7];
        if(max_data > 4)
            my_gain = input_data[8];
        if(max_data > 5)
            my_nsamples = input_data[9];

        response[4] = i;
        response[6] = my_mode;
        response[6] = my_chp;
        response[7] = my_chn;
        response[8] = my_gain;
        response[9] = my_nsamples;

        ODStream.ConfigChan(i, my_mode, my_chp, my_chn, my_gain);

#ifdef SERIAL_DEBUG
        Serial.print("C_CHANNEL_CFG [ ");
        Serial.print(i, DEC);
        Serial.print(" ] => mode: ");
        Serial.print(my_mode, DEC);
        Serial.print(" P: ");
        Serial.print(my_chp, DEC);
        Serial.print(" N: ");
        Serial.print(my_chn, DEC);
        Serial.print(" GAIN: ");
        Serial.println(my_gain, DEC);
#endif
        break;

    case C_TRIGGER_SETUP:
        resp_len = 4;

        i = input_data[4]; //Nb of channel
        j = input_data[5];  //repetition setup
        tdata = make16(input_data + 6);         //nb of total points

        response[4] = i;
        response[5] = j;
        response[6] = make8(tdata, 1);
        response[7] = make8(tdata, 0);

        ODStream.TriggerMode(i, j, tdata);


#ifdef SERIAL_DEBUG
        Serial.print("C_TRIGGER_SETUP [ ");
        Serial.print(i, DEC);
        Serial.print(" ] => mode= ");
        Serial.print(j, DEC);
        Serial.print(" value= ");
        Serial.println(tdata, DEC);
#endif
        break;

    case C_SIGNAL_LOAD:
        resp_len = 3;

        tdata = make16(input_data + 4);         //index init

        response[4] = make8(tdata, 1);
        response[5] = make8(tdata, 0);

        temp = input_data[3];
        temp -= 2;
        temp /= 2;
        temp += tdata;
        for(i = 0; i < temp - tdata; i++) {
            value = make16(input_data + 6 + 2 * i);      //nb of total points
            channels[3].databuffer[i + tdata] = value;
            channels[0].databuffer[i + tdata] = value;
        }
        break;

        ///// ALTERNATIVE COMMANDS         //////////////////////////////////////
    case C_ENABLE_CRC:
        resp_len = 1;

        i = input_data[4];
        response[4] = i;

        crc_enabled = (i > 0);

        break;

    case C_LED_W:
        resp_len = 1;

        i = input_data[4];
        response[4] = i;
        ledSet(LEDGREEN, i & 0x01);
        ledSet(LEDRED, i & 0x02);
#ifdef SERIAL_DEBUG
        Serial.print("C_LED_W: ");
        Serial.println(i, HEX);
#endif
        break;

    case C_RESET:
        resp_len = 0;

        response[0] = 0;
        response[1] = C_RESET;
        response[2] = C_RESET;
        response[3] = 0;

#ifdef SERIAL_DEBUG
        Serial.print("C_LED_W: ");
        Serial.println(i, HEX);
#else
        sendCommand(response, resp_len + 4);
#endif

        systemReset();
        break;

    case C_WAIT_MS:
        resp_len = 2;

        tdata = make16(input_data + 4);         //delay in ms
        response[4] = make8(tdata, 1);
        response[5] = make8(tdata, 0);

        for (i = 0; i < tdata; i++)
            _delay_ms(1);

#ifdef SERIAL_DEBUG
        Serial.print("C_WAIT_MS: ");
        Serial.println(tdata);
#endif

        break;

    case C_MCP23S17:
	/* Use some PIOs as a SPI port
	 * for controlling a MCP23S17 port expander:
	 * PIO1: CLK
	 * PIO2: MOSI
	 * PIO4: SS
	 */
        tdata = make16(input_data + 4);

	SetDigitalDir(0X0F);		    // set PIO 1-4 as outputs
	mcp23s17_write(PIO4, IODIR, 0);	    // set all GPIOs as outputs
	mcp23s17_write(PIO4, GPIO, tdata);  // write 16 bits to GPIOs

        resp_len = 2;
        response[4] = make8(tdata, 1);
        response[5] = make8(tdata, 0);
        break;

    case NACK:
    default:
        break;
    }

    response[2] = next_command;
    response[3] = resp_len;

    my_crc16 = CRC_16(resp_len + 2, response + 2);
    response[0] = make8(my_crc16, 1);
    response[1] = make8(my_crc16, 0);

#ifdef SERIAL_DEBUG
    Serial.print("Response: ");
    for(i = 0; i < resp_len + 4; i++) {
        Serial.print(response[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    Serial.println("*************");
    Serial.println();
#else
    //sendCommand(resp_len+4);
    for(i = 0; i < resp_len + 4; i++) {
        Serial.write(response[i]);
    }
#endif
    //free(response);
    //response=NULL;
}


// sends output buffer taking account of initial character x7E and escape character x7D
void CommDataClass::sendCommand(byte* response, int size)
{
    int i;
    byte a;
    pioWrite(1, 1);
    Serial.write(0x7E);

    for(i = 0; i < size; i++) {
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
    byte i;

    for(i = 0; i < MAX_DATA_BYTES; i++)
        input_data[i] = 0;

    wait_for_data = 0;
    max_data = 0;
    my_crc16 = 0;
    next_command = 0;
    received_bytes = 0;

}

uint16_t CommDataClass::CRC_16(int size, byte *packet)
{
    uint16_t crccode = 0;
    byte i;

    //Iterate through packet
    for(i = 0; i < size; i++)
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
