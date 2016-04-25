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

#ifndef BBSPI_H
#define BBSPI_H
/** 
 * @file bbspi.h 
 * Header for the BbspiClass  
 */
class BbspiClass {

/*
 * Private variables 
 */
private:
    // Use PIO pins to implement a "bitbang" SPI port:
    int bb_clk; // clock pin
    int bb_mosi; // master out, slave in
    int bb_miso; // master in, slave out

    int bb_cpol; // clock polarity: clk value when inactive
    int bb_cpha; // clock phase: data valid on clock leading (0) or trailing (1) edges

/*
 * Public Methods
 */

public:
    
    BbspiClass();  
    int setup(); 
    int setup(uint8_t my_clk, uint8_t my_mosi, uint8_t my_miso);
    int configure(uint8_t my_cpol, uint8_t my_cpha); 
    uint8_t transfer(uint8_t data);
};

extern BbspiClass spisw;

#endif
