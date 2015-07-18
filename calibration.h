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

#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <inttypes.h>
#include "HardwareSerial.h"

#define CW_MARK     6
#define CW_IND      0X55

#define IW_MARK     4
#define IW_IND      0XAA

#define CG0_POS		10
#define OFFSET_POS	36

#define CID_POS 	80

#define NCAL_POS    17
/**
 * @file calibration.h
 * Header file for Calibration Class
 */
class CalibrationClass {
public:
    // properties
    unsigned long gain_m[NCAL_POS + 1];
    long gain_b[NCAL_POS + 1];

    uint32_t my_id;

    // methods

    // class constructor
    CalibrationClass();

    uint8_t read(int);
    int write(int, uint8_t);

    uint32_t ID_Recall();
    int ID_Save(uint32_t device_id);

    int SaveCalibration();
    int RecallCalibration();


    int Reset_calibration();
};

extern CalibrationClass Cal;

#endif
