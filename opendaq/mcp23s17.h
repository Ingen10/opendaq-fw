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
 * @file mcp23s17.h
 * Header file 

 */

#ifndef MCP23S17_H
#define MCP23S17_H

#include <pins_arduino.h>

// Register addresses
#define IODIRA 0x00
#define IODIRB 0x01
#define IODIR  IODIRA
#define GPPUA  0x0C
#define GPPUB  0x0D
#define GPPU   GPPUA
#define GPIOA  0x12
#define GPIOB  0x13
#define GPIO   GPIOA

// Use PIO pins to implement a "bitbang" SPI port:
#define SPI_CLK   PIO1  // clock pin
#define SPI_MOSI  PIO2  // master out, slave in

void mcp23s17_write(int ss_pin, uint8_t addr, uint16_t data);

#endif
