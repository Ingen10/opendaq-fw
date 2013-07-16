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
 *  Version:  120407
 *  Author:   JRB
 */

#ifndef DATACHANNEL_H
#define DATACHANNEL_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "daqhw.h"

#include "HardwareSerial.h"


//DATACHANNEL TYPES
#define INACTIVE_TYPE   0
#define EXTERNAL_TYPE   1
#define STREAM_TYPE 2
#define BURST_TYPE  3

//DATACHANNEL STATES
#define CH_IDLE     0
#define CH_READY    1
#define CH_RUN      2
#define CH_STOP     3

//DATACHANNEL MODES
#define ANALOG_INPUT        0
#define ANALOG_OUTPUT       1
#define DIGITAL_INPUT       2
#define DIGITAL_OUTPUT  3
#define COUNTER_INPUT       4
#define CAPTURE_INPUT       5

//SETUP MODES
#define R_CONTINUOUS    0
#define R_ONCE              1

//TRIGGER MODES
#define Sw_TRG      0           //Software trigger (run on start)
#define DIN1_TRG    1           //Digital triggers
#define DIN2_TRG    2
#define DIN3_TRG    3
#define DIN4_TRG    4
#define DIN5_TRG    5
#define DIN6_TRG    6
#define ABIG_TRG    10      //Analog triggers (use current channel configuration: chp, chm, gain)
#define ASML_TRG    20


/******************************************************************************
 * Class
 ******************************************************************************/

/* DataChannel Class defines all the variables and functions used for managing
 * the automated data acquisition for openDAQ */

class DataChannel {
private:
    // properties
    int option;

    int stabilizationTime;
    int endReachedFlag;

    // methods
    void Initialize();
    void Begin();
    signed int (*actionCallback)(signed int n); ///USAR FUNCION CON ARGUMENTO INT

public:
    int dctype;
    int dcmode;
    int ready;
    int state;
    int period;
    int edge;

    //trigger controlling variables
    int trg_mode;
    int16_t  trg_value;

    unsigned int bufferlen;
    signed int *databuffer;

    unsigned int writeindex, readindex;
    unsigned long ndata, maxndata;
    unsigned int nrepeat, maxnrepeat;

    int pch, nch, g;

    unsigned int npoints;
    signed long  addsum;

    // methods
    DataChannel(int dtype);
    DataChannel(int dtype, unsigned long dperiod);
    DataChannel(int dtype, int dpin);
    DataChannel(int dtype, int dpin, int dedge);

    signed int Get();
    void Action();

    void Read();
    void Write();

    int waitStabilization();

    void Setup(unsigned long maxpoints, int maxrepeat);

    void TriggerConfig(int trigger_mode, int16_t trigger_value);
    int CheckMyTrigger();

    void Configure(int mode);
    void Configure(int mode, int pchan, int nchan);
    void Configure(int mode, int pchan, int nchan, int gain);
    void Configure(int mode, int pchan, int nchan, int gain, int nsamples);
    void Configure(int mode, int channel);

    void Activate();

    int endReached();
    void reset();

    void Enable();
    void Disable();
    void Destroy();

    void Flush();

    int Datalen();
};

#endif
