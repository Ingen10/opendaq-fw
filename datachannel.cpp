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
 *  Version:		120407
 *  Author:			JRB
 */

#ifndef DATACHANNEL_CPP
#define DATACHANNEL_CPP

#include "datachannel.h"

extern "C" {
  #include <stdlib.h>
}

// Constructors ////////////////////////////////////////////////////////////////

DataChannel::DataChannel(int dtype)
{
  dctype = dtype;
  Initialize();
}

DataChannel::DataChannel(int dtype, unsigned long dperiod)
{
  dctype = dtype;
	period = dperiod;
  Initialize();
}

DataChannel::DataChannel(int dtype, int dpin)
{
	edge = 1;
  dctype = dtype;
  Initialize();
}

DataChannel::DataChannel(int dtype, int dpin, int dedge)
{
 	edge = dedge;
	dctype = dtype;
  Initialize();
}


// Public Methods //////////////////////////////////////////////////////////////

void DataChannel::Action()
{

	if(dcmode == ANALOG_OUTPUT)
		Write();
	else 	//if(dcmode == ANALOG_INPUT)
		Read();

	ndata++;
  
	if(ndata == maxndata){
		if(maxnrepeat == R_CONTINUOUS)
			ndata = 0;
		else{
			nrepeat++;
			if(maxnrepeat>nrepeat)
				ndata = 0;
			else
				Disable();
		}
	}
}

void DataChannel::Read()
{
  signed int c;
  c = actionCallback(option);
  databuffer[writeindex%bufferlen] = c;
  writeindex++;
}

void DataChannel::Write()
{
  signed int c;
  
  c = databuffer[writeindex%maxndata];
  actionCallback(c);
  writeindex++;
	//readindex++;
}


signed int DataChannel::Get()
{
  signed int c;
	
	if(dcmode == ANALOG_OUTPUT){
		c = databuffer[readindex%maxndata];
	}
	else{
		c = databuffer[readindex%bufferlen];	
	}
  if(readindex<writeindex){
    readindex++;
  }
  
  return c;
}

void DataChannel::Setup(unsigned long maxpoints, int maxrepeat)
{
	maxndata = maxpoints;
	if(dcmode == ANALOG_OUTPUT){
		maxndata %= bufferlen;
	}
		
	maxnrepeat = maxrepeat;
}

void DataChannel::TriggerConfig(int trigger_mode,int16_t trigger_value)
{
	trg_mode = trigger_mode;
	trg_value = trigger_value;
}

int DataChannel::CheckMyTrigger()
{
	signed int value;
	
	if(state>CH_READY)
		return 1;
	
	if((trg_mode>=1)&&(trg_mode<=6)){
		SetpioMode(trg_mode-1,INPUT);
		if(pioRead(trg_mode-1)==trg_value)
			return 1;
		else
			return 0;
	}
	else if(trg_mode==ABIG_TRG){ 
		ConfigAnalog(pch, nch, g);
		value = ReadNADC(10);
		if(value>trg_value)
			return 1;
		else return 0;
	}
	else if(trg_mode==ASML_TRG){ 
		ConfigAnalog(pch, nch, g);
		value = ReadNADC(10);				
		if(value<trg_value)
			return 1;
		else return 0;
	}
	else
		return 1;

}

	
void DataChannel::Configure(int mode)
{
	dcmode = mode;
	pch = AIN1;
	nch = AGND;
	g = GAINX001;
}

void DataChannel::Configure(int mode, int pchan, int nchan)
{
	dcmode = mode;
	pch = pchan;
	nch = nchan;
	g = GAINX001;
}


void DataChannel::Configure(int mode, int pchan, int nchan, int gain)
{
	dcmode = mode;
	pch = pchan;
	nch = nchan;
	g = gain;
}

void DataChannel::Configure(int mode, int pchan, int nchan, int gain, int nsamples)
{
	dcmode = mode;
	pch = pchan;
	nch = nchan;
	g = gain;
	option = nsamples;
}


void DataChannel::Configure(int mode, int channel)
{
	dcmode = mode;
	pch = channel;
	nch = AGND;
	g = GAINX001;
}


void DataChannel::Begin()
{
	if(dcmode == ANALOG_INPUT){
		ConfigAnalog(pch, nch, g);
	}
	else if(dcmode == ANALOG_OUTPUT)
		SetDacOutput(0);
	else if(dcmode == COUNTER_INPUT){
		option = pch;
		counterInit(option);
	}
	else if(dcmode == DIGITAL_OUTPUT)
		SetDigitalDir(0xFF);
	else if(dcmode == DIGITAL_INPUT)
		SetDigitalDir(0);
	else if(dcmode == CAPTURE_INPUT){
		option = pch;
		captureInit(nch);
	}
}


void DataChannel::Activate()
{
	if(dcmode == ANALOG_INPUT){
		ConfigAnalog(pch,nch,	g);
	}
	else	return;
}


int DataChannel::Datalen()
{
	return bufferlen;
}


void DataChannel::Enable()
{
	if(dcmode == ANALOG_INPUT)
		//actionCallback = ReadAnalogIn;
		actionCallback = ReadNADC;
	else if(dcmode == ANALOG_OUTPUT)
		actionCallback = SetAnalogVoltage;
	else if(dcmode == COUNTER_INPUT)
		actionCallback = getCounter;
	else if(dcmode == DIGITAL_OUTPUT);
		//actionCallback = OutputDigital;
	else if(dcmode == DIGITAL_INPUT);
		//actionCallback = pioRead;
	else if(dcmode == CAPTURE_INPUT)
		actionCallback = getCapture;
		

	state = CH_RUN;
	ndata = 0;
	nrepeat = 0;
	Begin();
}

void DataChannel::Disable()
{
	state = CH_STOP;
}

void DataChannel::Destroy()
{
	state = CH_IDLE;
  dctype = INACTIVE_TYPE;

	free (databuffer);
}

void DataChannel::Flush()
{
  writeindex = 0;
  readindex = 0;
	ndata = 0;
	nrepeat = 0;
}


// Private Methods /////////////////////////////////////////////////////////////

void DataChannel::Initialize()
{
  state = CH_READY;
	
	dcmode = ANALOG_INPUT;
	
	trg_mode = Sw_TRG;
	
	maxndata = 0;
	maxnrepeat = R_CONTINUOUS;
	
  writeindex = 0;
  readindex = 0;
	
	option = 1;
	
	ready = 0;
	
  if(dctype==BURST_TYPE){
    bufferlen = 1200;
  }
  else{
    bufferlen = 400;
  }
	
	
  databuffer = (signed int*) malloc( bufferlen * sizeof(int));
  
}


#endif

