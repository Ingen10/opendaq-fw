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
 *  Version:		120522
 *  Author:			JRB
 */

#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <inttypes.h>
//#include "pins_opendaq.h"
#include "HardwareSerial.h"


#define HVER	1
#define FVER 	1

#define	CW_MARK		6
#define CW_IND		0X55

#define IW_MARK		4
#define IW_IND		0XAA

#define CG0_POS		10
#define OFFSET_POS	20

#define CID_POS 	40

class CalibrationClass
{
  public:
  // properties
  uint16_t gain_m[6];
  int16_t gain_b[6];
  
  uint32_t my_id;

  // methods
  
	//! class constructor
  CalibrationClass();
  
  uint8_t read(int);
  void write(int, uint8_t);
  
  uint32_t ID_Recall();
  void ID_Save(uint32_t device_id);

  void SaveCalibration();
  int RecallCalibration();

	
  void Reset_calibration();
};

extern CalibrationClass Cal;

#endif

