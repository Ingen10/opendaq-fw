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
 *  Version:    160901
 *  Author:     JRB
 *  Revised by: 
 */



#include "encoder.h"
#include "commdata.h"

Encoder encoder;
/**
 * @file encoder.cpp
 * Source code for Encoder Class
*/
/** **************************************************************************************
 * \section Public Public methods
 *
 */

/** \brief
 *  Start the encoder
 *
 *  \param
 *  res: encoder resolution
 */
void Encoder::Start(uint16_t res) {
    position = 0;
    resolution = res;
    stabilizationTime = 100;
    //Digital pin 5 as input for direction sense
    DDRD &= ~(_BV(DDD5)); //input direction

    EICRA = _BV(ISC11); //The rising edge of INT1 generates an interrupt request.
    EIMSK |= _BV(INT1); //INT1: External Interrupt Request 1 Enable (INT1->PD3/PIO6)
}

/** \brief
 *  Stop the encoder
 *
 */
void Encoder::Stop() {
    EIMSK = 0;
}

/** \brief
 *  Pause the encoder
 *
 */
void Encoder::Pause() {
    EIMSK = 0;
}

/** \brief
 *  Resume the encoder
 *
 */
void Encoder::Resume() {
    EIMSK = 1;
}

/** \brief
 *  Get the encoder position
 *
 *  \return
 *  The encoder position
 */
uint32_t Encoder::get_position() {
    return position;
}

/** \brief
 *  Increment the encoder position
 *
 */
void Encoder::increment_position() {
    position++;
    if (position > resolution && resolution > 0)
        position = 0;
}

/** \brief
 *  Decrement the encoder position
 *
 */
void Encoder::decrement_position() {
    if (position == 0 && resolution > 0)
        position = resolution;
    else
        position--;
}



/** **************************************************************************************
 * \section Interrupt Interrupt general routines
 *
 */

/** \brief
 *  Routine to process INT1 (PIO6) interrupts
 *  If PIO5=0 decrement, else increment
 */
ISR(INT1_vect) {
    static uint8_t interrupt = 0;
    
    if (interrupt) {
        interrupt = 0;
        return;
    }
    
    delayMicroseconds(50);
    
    if (PIND & _BV(PIND5)) {    //If PIO5=0 decrement, else increment
        encoder.increment_position();
    } else {
        encoder.decrement_position();
    }
    
    if (EIFR != 0) 
        interrupt = 1;
    else
        interrupt = 0;
}


