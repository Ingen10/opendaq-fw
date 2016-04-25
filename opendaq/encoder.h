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

#ifndef ENCODER_H
#define ENCODER_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "datachannel.h"

#include "HardwareSerial.h"

//EDGE MODES
#define H_TO_L      0
#define L_TO_H      1


//******************************************************************************
// Class
//******************************************************************************

// Encoder Class
/**
 * @file encoder.h
 * Encoder Class
*/
class Encoder {
private:
    int position;
    int resolution;
    unsigned int stabilizationTime;

public:
    void Start(unsigned int res);
    void Stop();
    void Pause();
    void Resume();

    int get_position();
    void increment_position();
    void decrement_position();
    int waitStabilization();
};

#endif
