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
 *  Version:    161203
 *  Author:     JRB
 *  Revised by: 
 */

/**
 * @file opendaq.ino
 * Main loop and device initialization with Arduino type extension
 */

#define SERIAL_DEBUG

#include <avr/wdt.h>
#include "calibration.h"
#include "datachannel.h"
#include "odstream.h"
#include "commdata.h"
#include "debug.h"

extern DStream ODStream;

void setup()
{
    int i;

    daqInit();
    i = Cal.RecallCalibration();
    Comm.begin();

    delay(100);
    SetAnalogVoltage(0);

    _DEBUG("memory < %d\r\n", availableMemory());
    _DEBUG("VERSION %d\r\n", HW_VERSION);

    ledSet(LEDGREEN, 1);
    ledSet(LEDRED, 0);

    wdt_enable(WDTO_2S);
    SetDacOutput(20000);
#ifdef SERIAL_DEBUG 
    /*PORTA &= ~(0X01 << 1);
    SetupMcp6s26_Channel(1); 
    PORTA |= 0X01 << 1;*/
    TestConfig(7,0,2);
#endif
}


void loop()
{
    static int i = 0;
#ifdef SERIAL_DEBUG
    delay(500);
    i++;
    _DEBUG("Read2= %d\r\n", ReadADC());
#endif
    ODStream.CheckTriggers();
    Comm.processStream();   
    
    while(Comm.available())
        Comm.parseInput(0);

    wdt_reset();
}
