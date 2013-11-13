/*
 *  Copyright (C) 2013 INGEN10 Ingenieria SL
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
 *  Author:   JRB
 *
 *  BbspiClass
 *  Software controlled SPI for driving external devices.
 *  Pin functions may be configured using setup function.
 *  Chip Select pin must be externally (software) controlled
 *
 */

#include <pins_arduino.h>
#include <inttypes.h>

#include "daqhw.h"

#include "bbspi.h"

#define NPIOPINS 6

// Constructors ////////////////////////////////////////////////////////////////

BbspiClass::BbspiClass()
{
    bb_clk = 0;     // clock pin
    bb_mosi = 1;    // master out, slave in
    bb_miso = 2;    // master in, slave out
}

// Public Methods //////////////////////////////////////////////////////////////

//setup: set pin numbers and functions
int BbspiClass::setup(uint8_t my_clk, uint8_t my_mosi, uint8_t my_miso)
{
    //assign pin numbers
    if((my_clk<NPIOPINS+1)&&(my_clk>0))
        bb_clk = my_clk - 1;
    if((my_mosi<NPIOPINS+1)&&(my_mosi>0))
        bb_mosi = my_mosi - 1;
    if((my_miso<NPIOPINS+1)&&(my_miso>0))
        bb_miso = my_miso - 1;
    
    //error check
    if(bb_clk == bb_mosi)
        return -1;
    if(bb_clk == bb_miso)
        return -1;
    if(bb_miso == bb_mosi)
        return -1;
    
    //set pin direction
    SetpioMode(bb_clk, 1);
    SetpioMode(bb_mosi, 1);
    SetpioMode(bb_miso, 0);    
    return 0;
}

//setup: set default pin numbers and functions
int BbspiClass::setup()
{
    SetpioMode(bb_clk, 1);
    SetpioMode(bb_mosi, 1);
    SetpioMode(bb_miso, 0);  
    return 0;
}

// Bit shift the data out
uint8_t BbspiClass::transfer(uint8_t data)
{
    uint8_t recv_data = 0;
    
    pioWrite (bb_clk, 0);
    pioWrite (bb_mosi, 0);
    
    for(int i = 0; i < 8; i++) {
        recv_data = recv_data << 1;
        if (data > 127)
            pioWrite (bb_mosi, 1);
        else
            pioWrite (bb_mosi, 0);
            
        pioWrite (bb_clk, 1);
        data = data << 1;
        recv_data |= pioRead (bb_miso);
        pioWrite (bb_clk, 0);
    }
    return recv_data;
}


BbspiClass spisw;
