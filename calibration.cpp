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

#ifndef CALIBRATION_CPP
#define CALIBRATION_CPP

#include <avr/eeprom.h>
#include "calibration.h"

// Constructors ////////////////////////////////////////////////////////////////
CalibrationClass::CalibrationClass(void)
{
	Reset_calibration();
 
  my_id = 0;
}

void CalibrationClass::Reset_calibration()
{
	//AIN CALIBRATION DEFAULTS
  gain_m[0] = 37500;
  gain_m[1] = 12500;
  gain_m[2] =  6250;
  gain_m[3] =  1250;
  gain_m[4] =   125;
	
  gain_b[0] = 0;
  gain_b[1] = 0;
  gain_b[2] = 0;
  gain_b[3] = 0;
  gain_b[4] = 0;
  
	//DAC CALIBRATION DEFAULTS
  gain_m[5] = 2000;
  gain_b[5] = 0;
}


// Public Methods //////////////////////////////////////////////////////////////

///////ID-SERIAL NB STORAGE FUNCTIONS
uint32_t CalibrationClass::ID_Recall()
{
	unsigned char p;
	uint32_t i32;
	int i;
	
	i32= 0;
	
	p = read(IW_MARK);
	if(p!=IW_IND)
		return 0;

	for(i=0;i<4;i++){
		i32<<=8;
		p = read(CID_POS+i);
		i32|=p;
	}
	
	return i32;
}


void CalibrationClass::ID_Save(uint32_t device_id)
{
	unsigned char p;
	uint32_t i32;
	int i;
	
	i32= device_id;
	
	write(IW_MARK,IW_IND);

	for(i=0;i<4;i++){
		p = i32&0x000000FF;
		write(CID_POS+3-i,p);
		i32>>=8;
	}
}

/////////////////////////////////////////////////////////////////////
////CALIBRATION STORAGE FUNCTIONS 

void CalibrationClass::SaveCalibration()
{
	unsigned char *p;
	int i;
	
	
	p = (unsigned char*) gain_m;
	
	for(i=0;i<sizeof(gain_m);i++){
		write(CG0_POS + i,p[i]);
	}

	p = (unsigned char*) gain_b;
	
	for(i=0;i<sizeof(gain_b);i++){
		write(CG0_POS +OFFSET_POS + i,p[i]);
	}
	
	write(CW_MARK,CW_IND);
	
}

int CalibrationClass::RecallCalibration()
{
	unsigned char *p;
	int i;
	int a;
	
	
	a = read(CW_MARK);	
	if(a!= CW_IND){
		Reset_calibration();
		return 0;
	}
	
	p = (unsigned char*) gain_m;
	
	for(i=0;i<sizeof(gain_m);i++){
		p[i] = read(CG0_POS + i);
	}

	p = (unsigned char*) gain_b;
	
	for(i=0;i<sizeof(gain_b);i++){
		p[i] = read(CG0_POS +OFFSET_POS + i);
	}

	return a;
}



/////////////////////////////////////////////////////////////////////
// LOW LEVEL EEPROM CALLS

uint8_t CalibrationClass::read(int address)
{
	return eeprom_read_byte((unsigned char *) address);
}

void CalibrationClass::write(int address, uint8_t value)
{
	eeprom_write_byte((unsigned char *) address, value);
}

CalibrationClass Cal;

#endif
