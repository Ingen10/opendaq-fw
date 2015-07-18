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
 *  Version:    150717
 *  Author:     JRB
 *  Revised by: AV (17/07/15)
 */

#include <pins_arduino.h>
#include <inttypes.h>

#include "daqhw.h"
#include "bbspi.h"
#define NPIOPINS 6

// Constructors ////////////////////////////////////////////////////////////////
/**
 * @file bbspi.cpp
 */ 
/** 
 *  Bbspi constructor
 *
 *  \return
 *  Bbspi object created
 */
BbspiClass::BbspiClass() {
    bb_clk = 0; /* clock pin*/
    bb_mosi = 1; /* master out, slave in*/
    bb_miso = 2; /* master in, slave out*/

    bb_cpol = 0; /* clock output low when inactive*/
    bb_cpha = 1; /* data valid on clock trailing edges*/
}

// Public Methods //////////////////////////////////////////////////////////////

/**
 *  Setup Bbspi (pin numbers and functions)
 *
 * 
 * \param my_clk: clock pin number
 *  \param my_mosi: mosi pin number
 *  \param my_miso: miso pin number
 *  \return
 *  Error flag (0 -> right)
 */
int BbspiClass::setup(uint8_t my_clk, uint8_t my_mosi, uint8_t my_miso) {
    /* 
     * assign pin numbers*/
    if ((my_clk < NPIOPINS + 1)&&(my_clk > 0))
        bb_clk = my_clk - 1;
    if ((my_mosi < NPIOPINS + 1)&&(my_mosi > 0))
        bb_mosi = my_mosi - 1;
    if ((my_miso < NPIOPINS + 1)&&(my_miso > 0))
        bb_miso = my_miso - 1;

    //error check
    if (bb_clk == bb_mosi)
        return -1;
    if (bb_clk == bb_miso)
        return -1;
    if (bb_miso == bb_mosi)
        return -1;

    //set pin direction
    SetpioMode(bb_clk, 1);
    SetpioMode(bb_mosi, 1);
    SetpioMode(bb_miso, 0);
    pioWrite(bb_clk, bb_cpol);
    return 0;
}

/** 
 *  Set default pin numbers and functions
 *
 *  \return  0
 */
int BbspiClass::setup() {
    SetpioMode(bb_clk, 1);
    SetpioMode(bb_mosi, 1);
    SetpioMode(bb_miso, 0);
    pioWrite(bb_clk, bb_cpol);
    return 0;
}

/** 
 *  Set SPI transfer clock configuration
 *
 *  \param   my_cpol: clock output
 *  \param my_cpha: data valid on clock trailing edges
 *  \return bb_cpol
 * 
 */
int BbspiClass::configure(uint8_t my_cpol, uint8_t my_cpha) {
    bb_cpol = my_cpol;
    bb_cpha = my_cpha;
    return bb_cpol;
}

/** 
 *  Bit shift the data out
 *
 *  \param   data: input data
 *  \return  Received data
 */
uint8_t BbspiClass::transfer(uint8_t data) {
    uint8_t recv_data = 0;

    pioWrite(bb_clk, bb_cpol);
    pioWrite(bb_mosi, 0);

    for (int i = 0; i < 8; i++) {
        recv_data = recv_data << 1;
        if (data > 127)
            pioWrite(bb_mosi, 1);
        else
            pioWrite(bb_mosi, 0);

        pioWrite(bb_clk, !bb_cpol);
        if (bb_cpha == 0)
            recv_data |= pioRead(bb_miso);
        data = data << 1;
        pioWrite(bb_clk, bb_cpol);
        if (bb_cpha == 1)
            recv_data |= pioRead(bb_miso);
    }
    return recv_data;
}


BbspiClass spisw;
