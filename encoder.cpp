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
 *  Version:    140207
 *  Author:     JRB
 *  Revised by: AV (07/02/14)
 */

#include "encoder.h"
#include "commdata.h"

Encoder encoder;

// Public Methods //////////////////////////////////////////////////////////////

/** \brief
 *  Start the encoder
 *
 *  \param
 *  res: encoder resolution
 */
void Encoder::Start(unsigned int res) {
    position = 0;
    resolution = res;
    stabilizationTime = 100;
    //Digital pin 5 as input
    DDRD &= ~(_BV(DDD5)); //input direction
    //  PORTD &= ~(_BV(PD5));

    EICRA = _BV(ISC11);
    EIMSK |= _BV(INT1);
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
int Encoder::get_position() {
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
    position--;
    if (position < 0 && resolution > 0)
        position = resolution;
}

/** \brief
 *  Wait for the encoder stabilization
 *
 */
int Encoder::waitStabilization() {
    int dummy, i, j;
    for (i = 0; i < stabilizationTime; i++) {
        for (j = 0; j < 10; j++) {
            if (UCSR0A & _BV(RXC0)) {
                Comm.parseInput(1);
                ledSet(LEDGREEN, 0);
                ledSet(LEDRED, 1);
            }
        }
    }
    return dummy;
}

/////////////////////////////////////////////////////////////////////////////
// INTERRUPT GENERAL ROUTINES:
/////////////////////////////////////////////////////////////////////////////

/** \brief
 *  Routine to process interrupts
 *
 */
ISR(INT1_vect) {
    static unsigned interrupt;
    if (interrupt) {
        interrupt = 0;
        return;
    }

    encoder.waitStabilization();

    if (PIND & _BV(PIND5)) {
        encoder.increment_position();
    } else {
        encoder.decrement_position();
    }
    while (!(PIND & _BV(PIND3))) {
        if (UCSR0A & _BV(RXC0)) {
            Comm.parseInput(1);
            ledSet(LEDGREEN, 0);
            ledSet(LEDRED, 1);
        }
    }

    encoder.waitStabilization();

    //if a interrupt was caused, next re-entry will return inmediatly
    if (EIFR != 0) {
        interrupt = 1;
    } else
        interrupt = 0;

    ledSet(LEDGREEN, 1);
    ledSet(LEDRED, 0);
}
