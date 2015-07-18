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

/**
 * @file mcp23s17.cpp
 * 
*/



#include "mcp23s17.h"
#include "Arduino.h"
#include <avr/io.h>

#define CMD_BYTE 0x40

/** \brief
 *  Bit shift the data out
 *
 *  \param
 *  data: input data
 */
void spi_transfer(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        if (data > 127)
            digitalWrite(SPI_MOSI, 1);
        else
            digitalWrite(SPI_MOSI, 0);

        digitalWrite(SPI_CLK, 1);
        data = data << 1;
        digitalWrite(SPI_CLK, 0);
    }
}

/** \brief
 *  Send a command to the MCP23S17 port expander
 *
 *  \param  ss_pin to digital write
 *  \param addr: address to transfer
 *  \param data: input data
 */
void mcp23s17_write(int ss_pin, uint8_t addr, uint16_t data) {
    digitalWrite(ss_pin, 0);
    spi_transfer(CMD_BYTE);
    spi_transfer(addr);
    spi_transfer((uint8_t) (data >> 8));
    spi_transfer((uint8_t) (data & 0x00ff));
    digitalWrite(ss_pin, 1);
}
