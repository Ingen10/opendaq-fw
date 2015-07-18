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
 *  Version:    150701
 *  Author:     JRB
 *  Revised by: AV (17/07/15)
 */

#include "datachannel.h"
#include "debug.h"

extern "C" {
#include <stdlib.h>
}

/**
 * @file datachannel.cpp
 * Source code for DataChannel Class
 */


// Constructors ////////////////////////////////////////////////////////////////

/** \brief
 *  DataChannel constructor
 *
 *  \param
 *  dtype: Type of DataChannel
 *  \return
 *  DataChannel created
 */
DataChannel::DataChannel(int dtype) {
    dctype = dtype;
    Initialize();
}

/** \brief
 *  DataChannel constructor
 *
 *  \param   dtype: type of DataChannel
 *  \param dperiod: period of DataChannel
 *  \return
 *  DataChannel created
 */
DataChannel::DataChannel(int dtype, unsigned long dperiod) {
    dctype = dtype;
    period = dperiod;
    Initialize();
}

/** \brief
 *  DataChannel constructor
 *
 *  \param  dtype: type of DataChannel
 *  \param dpin: unused
 *  \return
 *  DataChannel created
 */
DataChannel::DataChannel(int dtype, int dpin) {
    edge = 1;
    dctype = dtype;
    Initialize();
}

/** \brief
 *  DataChannel constructor
 *
 *  \param  dtype: type of DataChannel
 *  \param dpin: unused
 *  \param dedge: edge of DataChannel
 *  \return
 *  DataChannel created
 */
DataChannel::DataChannel(int dtype, int dpin, int dedge) {
    edge = dedge;
    dctype = dtype;
    Initialize();
}


// Public Methods //////////////////////////////////////////////////////////////

/** \brief
 *  Execute the DataChannel Action (write or read)
 * 
 */
void DataChannel::Action() {
    if (dcmode == ANALOG_OUTPUT)
        Write();
    else
        Read();

    if(stabilitation_points>0){
	stabilitation_points --;
	writeindex = 0;
	return;
    }
    ndata++;
    if (ndata == maxndata) {
        if (maxnrepeat == R_CONTINUOUS)
            ndata = 0;
        else {
            nrepeat++;
            if (maxnrepeat > nrepeat)
                ndata = 0;
            else
                Disable();
        }
    }
}

/** \brief
 *  Indicate if end of DataChannel has been reached
 *
 *  \return
 *  Indicate if end of DataChannel has been reached
 */
int DataChannel::endReached() {
    return end_reached;
}

/** \brief
 *  Read ADC and store the readed value in the DataChannel buffer
 *
 */
void DataChannel::Read() {
    signed int c;

    c = actionCallback(option); //ReadNADC(1);//
    databuffer[writeindex % bufferlen] = c;
    writeindex++;
}

/** \brief
 *  Set DAC using a value from the DataChannel buffer
 *
 */
void DataChannel::Write() {
    signed int c;
    unsigned int add;

    c = databuffer[writeindex % maxndata];
    SetDacOutput(c);
    writeindex++;
    readindex = writeindex;
}

/** \brief
 *  Read the buffer of the DataChannel and return the readed value
 *
 *  \return
 *  The readed value from DataChannel
 */
signed int DataChannel::Get() {
    signed int c;
    int i;

    if (dcmode == ANALOG_OUTPUT)
        c = databuffer[readindex % maxndata];
    else
        c = databuffer[readindex % bufferlen];

    if (readindex < writeindex)
        readindex++;

    return c;
}

/** \brief
 *  Write a value in the buffer of the DataChannel
 *
 *  \param  index: position of the buffer to write the value
 *  \param value: value to wrtie in the buffer of the DataChannel
 */
void DataChannel::Put(unsigned int index, signed int value) {
    databuffer[index] = value;
}

/** \brief
 *  Setup the max number of points and max number of repeats of DataChannel
 *
 *  \param  maxpoints: max number of points of the DataChannel
 *  \param maxrepeat: max number of repeats of the DataChannel
 */
void DataChannel::Setup(unsigned long maxpoints, int maxrepeat) {
    maxndata = maxpoints;
    if (dcmode == ANALOG_OUTPUT)
        maxndata %= bufferlen;

    maxnrepeat = maxrepeat;
}

/** \brief
 *  Configure the trigger of the DataChannel
 *
 *  \param  trigger_mode: mode of the trigger to setup in the DataChannel
 *  \param trigger_value_ value of the trigger to setup in the DataChannel
 */
void DataChannel::TriggerConfig(int trigger_mode, int16_t trigger_value) {
    trg_mode = trigger_mode;
    trg_value = trigger_value;
}

/** \brief
 *  Check the trigger of the DataChannel
 *
 *  \return
 *  Indicate if trigger has been triggered
 */
int DataChannel::CheckMyTrigger() {
    signed int value;

    if (state > CH_READY)
        return 1;

    if ((trg_mode >= 1) && (trg_mode <= 6)) {
        SetpioMode(trg_mode - 1, INPUT);
        return (pioRead(trg_mode - 1) == trg_value);
    } else if (trg_mode == ABIG_TRG) {
        ConfigAnalog(pch, nch, g);
        value = ReadNADC(10);
        return (value > trg_value);
    } else if (trg_mode == ASML_TRG) {
        ConfigAnalog(pch, nch, g);
        value = ReadNADC(10);
        return (value < trg_value);
    }

    return 1;
}

/** \brief
 *  Configure the mode of the DataChannel
 *
 *  \param
 *  mode: mode to configure the DataChannel
 */
void DataChannel::Configure(int mode) {
    dcmode = mode;
    pch = 1;
    nch = 0;
    g = 0;
}

/** \brief
 *  Configure the DataChannel
 *
 *  \param  mode: mode of the DataChannel
 *  \param pchan: channel of the DataChannel
 *  \param nchan: channel number
 */
void DataChannel::Configure(int mode, int pchan, int nchan) {
    dcmode = mode;
    pch = pchan;
    nch = nchan;
    g = 0;
}

/** \brief
 *  Configure the DataChannel
 *
 *  \param  mode: mode of the DataChannel
 *  \param pchan: channel of the DataChannel
 *  \param nchan: channel number
 *  \param gain: gain of the DataChannel
 */
void DataChannel::Configure(int mode, int pchan, int nchan, int gain) {
    dcmode = mode;
    pch = pchan;
    nch = nchan;
    g = gain;
}

/** \brief
 *  Configure the DataChannel
 *
 *  \param  mode: mode of the DataChannel
 *  \param pchan: channel of the DataChannel
 *  \param nchan: channel number
 *  \param gain: gain of the DataChannel
 *  \param nsamples: number of samples of the DataChannel
 */
void DataChannel::Configure(int mode, int pchan, int nchan, int gain, int nsamples) {
    dcmode = mode;
    pch = pchan;
    nch = nchan;
    g = gain;
    option = nsamples;
}

/** \brief
 *  Configure the DataChannel
 *
 *  \param  mode: mode of the DataChannel
 *  \param channel: channel of the DataChannel
 */
void DataChannel::Configure(int mode, int channel) {
    dcmode = mode;
    pch = channel;
    nch = 0;
    g = 0;
}

/** \brief
 *  Prepare the DataChannel to begin
 *
 */
void DataChannel::Begin() {
    stabilitation_points = 1;
    if (dcmode == ANALOG_INPUT)
        ConfigAnalog(pch, nch, g);
    else if (dcmode == ANALOG_OUTPUT)
	SetDacOutput(databuffer[0]);
    else if (dcmode == COUNTER_INPUT) {
        option = pch;
        counterInit(option);
    } else if (dcmode == DIGITAL_OUTPUT)
        SetDigitalDir(0xFF);
    else if (dcmode == DIGITAL_INPUT)
        SetDigitalDir(0);
    else if (dcmode == CAPTURE_INPUT) {
        option = pch;
        captureInit(nch);
    }
}

/** \brief
 *  Activate the DataChannel
 *
 */
void DataChannel::Activate() {
    if (dcmode == ANALOG_INPUT)
        ConfigAnalog(pch, nch, g);
}

/** \brief
 *  Get the length of the buffer of the DataChannel
 *
 *  \return
 *  Length of the buffer of the DataChannel
 */
int DataChannel::Datalen() {
    return bufferlen;
}

/** \brief
 *  Enable the DataChannel
 *
 */
void DataChannel::Enable() {
    end_reached = 0;

    switch (dcmode) {
        case ANALOG_INPUT:
#ifdef SERIAL_DEBUG
            actionCallback = ReadAnalogIn;
#else
            actionCallback = ReadNADC;
#endif
            break;
        case ANALOG_OUTPUT:
            actionCallback = SetDacOutput; //SetAnalogVoltage;
            break;
        case COUNTER_INPUT:
            actionCallback = getCounter;
            break;
        case DIGITAL_OUTPUT:
            //actionCallback = OutputDigital;
            break;
        case DIGITAL_INPUT:
            //actionCallback = pioRead;
            break;
        case CAPTURE_INPUT:
            actionCallback = getCapture;
            break;
        default:
            break;
    }

    state = CH_RUN;
    ndata = 0;
    nrepeat = 0;
    Begin();
}

/** \brief
 *  Disable the DataChannel
 *
 */
void DataChannel::Disable() {
    state = CH_STOP;
    end_reached = 1;
}

/** \brief
 *  Reset the DataChannel
 *
 */
void DataChannel::reset() {
    writeindex = 0;
    readindex = 0;
}

/** \brief
 *  Destroy the DataChannel
 *
 */
void DataChannel::Destroy() {
    if (dctype != INACTIVE_TYPE) { //disable only if channel is enabled
        state = CH_IDLE;
        dctype = INACTIVE_TYPE;
        free(databuffer);
    }
}

/** \brief
 *  Flush the DataChannel
 *
 */
void DataChannel::Flush() {
    writeindex = 0;
    readindex = 0;
    ndata = 0;
    nrepeat = 0;
}

/** \brief
 *  Double an int
 *
 *  \param
 *  j: int to be doubled
 *  \return
 *  Double of the param j
 */
int dummyfunction(int j) {
    return j * 2;
}

/** \brief
 *  Read ADC and wait to stabilization
 *
 *  \return
 *  The last residtual value of the dummy function (15998)
 */
int DataChannel::waitStabilization() {
    int dummy, i, j;

    //call ReadACD to adapt analog input
    ReadADC();
    for (i = 0; i < stabilization_time; i++) {
        for (j = 0; j < 8000; j++)
            dummy = dummyfunction(j);
    }

    return dummy;
}


// Private Methods /////////////////////////////////////////////////////////////

/** \brief
 *  Initialize the DataChannel
 *
 */
void DataChannel::Initialize() {
    stabilization_time = 100; // 100 times 0.508 us
    state = CH_READY;
    dcmode = ANALOG_INPUT;
    trg_mode = Sw_TRG;
    maxndata = 0;
    maxnrepeat = R_CONTINUOUS;
    writeindex = 0;
    readindex = 0;
    option = 1;
    ready = 0;
    bufferlen = 150;
    databuffer = (signed int*) malloc(bufferlen * sizeof (int));

    if (databuffer == NULL)
        _DEBUG("malloc error\n");
}
