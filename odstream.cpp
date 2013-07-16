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
 *  Version:  120507
 *  Author:   JRB
 */

#include "odstream.h"

DStream ODStream;              // preinstatiate

DataChannel Channel1=DataChannel(INACTIVE_TYPE);
DataChannel Channel2=DataChannel(INACTIVE_TYPE);
DataChannel Channel3=DataChannel(INACTIVE_TYPE);
DataChannel Channel4=DataChannel(INACTIVE_TYPE);



// Public Methods //////////////////////////////////////////////////////////////
void DStream::Initialize()
{
  TCCR2A = 0;                 // clear control register A 
  TCCR2B = _BV(WGM13);        // set mode 8: phase and frequency correct pwm, stop the timer
}

void DStream::Start()
{
	if( (Channel1.dctype >= STREAM_TYPE) || (Channel2.dctype >= STREAM_TYPE) || 
			(Channel3.dctype >= STREAM_TYPE) || (Channel4.dctype >= STREAM_TYPE) ) {
		TCNT2 = 0;
		TIMSK2 = _BV(OCIE2A);
	}

	if( (Channel1.dctype == EXTERNAL_TYPE) || (Channel2.dctype == EXTERNAL_TYPE) || 
		(Channel3.dctype == EXTERNAL_TYPE) || (Channel4.dctype == EXTERNAL_TYPE) ){
		PCICR |= _BV(PCIE0);						//Pin Change Interrupt Enable 0 (PCINT31..24)
	}
	
	if(Channel1.dctype == BURST_TYPE){
		Channel1.Activate();
		usingch = 1;
	}
		
	CheckTriggers();
	ledSet(LEDGREEN,1);
	ledSet(LEDRED,0);
	stream_sm();
}

void DStream::Stop()
{
	Channel1.reset();
	Channel2.reset();
	Channel3.reset();
	Channel4.reset();
	
	TIMSK2 &= ~_BV(OCIE2A);
	PCICR &= ~_BV(PCIE0);
	ledSet(LEDGREEN,1);
	ledSet(LEDRED,0);
}

void DStream::Pause()
{
}

void DStream::Resume()
{
}


void DStream::CreateStreamChannel(uint8_t nb, unsigned long ms_period)
{
	if(nb==1){
		Channel1.Destroy();
		Channel1=DataChannel(STREAM_TYPE,(unsigned long) ms_period);
	}
	else if(nb==2){
		Channel2.Destroy();
		Channel2=DataChannel(STREAM_TYPE,(unsigned long) ms_period);
	}
	else if(nb==3){
		Channel3.Destroy();
		Channel3=DataChannel(STREAM_TYPE,(unsigned long) ms_period);
	}
	else if(nb==4){
		Channel4.Destroy();
		Channel4=DataChannel(STREAM_TYPE,(unsigned long) ms_period);
	}

	if(Channel1.dctype == BURST_TYPE){
			Channel1.Destroy();
	}

	TCCR2A = 0X82;            //CTC MODE, CLEAR OC2A ON COMPARE
	TCCR2B = 4;               //Main clock /64 (2MHz)
	OCR2A = 250;              //Prescaler: 250us per interrupt

	tstreamCallback = stream_sm;
	usingch = 0;
}

void DStream::CreateExternalChannel(uint8_t nb, uint8_t edge)
{

	if(nb==1){
		Channel1.Destroy();
		Channel1=DataChannel(EXTERNAL_TYPE,1,edge);  
		PCMSK0 |= _BV(PCINT7);  				//Pin Change Enable Mask
		DDRA &= ~(_BV(DDA7));					//input direction
	}
	else if(nb==2){
		Channel2.Destroy();
		Channel2=DataChannel(EXTERNAL_TYPE,2,edge);
		PCMSK0 |= _BV(PCINT6);  				//Pin Change Enable Mask
		DDRA &= ~(_BV(DDA6));					//input direction
	}
	else if(nb==3){
		Channel3.Destroy();
		Channel3=DataChannel(EXTERNAL_TYPE,3,edge);
		PCMSK0 |= _BV(PCINT5);  				//Pin Change Enable Mask
		DDRA &= ~(_BV(DDA5));					//input direction
	}
	else if(nb==4){
		Channel4.Destroy();
		Channel4=DataChannel(EXTERNAL_TYPE,4,edge);
		PCMSK0 |= _BV(PCINT4);  				//Pin Change Enable Mask
		DDRA &= ~(_BV(DDA4));					//input direction
	}

	
  //stores variable with actual external input
	usingch = 0;

}

void DStream::CreateExternalChannel(uint8_t nb)
{
	CreateExternalChannel(nb,L_TO_H);
}


#define T2_RESOLUTION 256    // Timer2 is 8 bit
void DStream::CreateBurstChannel(unsigned long us_period)
{
	uint8_t t2bits;
	
	Channel1.Destroy();
	Channel2.Destroy();
	Channel3.Destroy();	
	Channel4.Destroy();

	Channel1=DataChannel(BURST_TYPE,us_period);
	
  TCCR2A = 0X82;            //CTC MODE, CLEAR OC2A ON COMPARE
  
  long cycles = (F_CPU / 1000000) * us_period;                         // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
	if((cycles >>= 3) < T2_RESOLUTION) t2bits = _BV(CS21);              // prescale by /8
  else if((cycles >>= 2) < T2_RESOLUTION) t2bits = _BV(CS21) | _BV(CS20);  // prescale by /32
  else if((cycles >>= 1) < T2_RESOLUTION) t2bits = _BV(CS22);              // prescale by /64
  else if((cycles >>= 2) < T2_RESOLUTION) t2bits = _BV(CS21) | _BV(CS22);              // prescale by /256
  else if((cycles >>= 2) < T2_RESOLUTION) t2bits = _BV(CS20) | _BV(CS21) | _BV(CS22);  // prescale by /1024
  else        cycles = T2_RESOLUTION - 1, t2bits = _BV(CS20) | _BV(CS21) | _BV(CS22);  // request was out of bounds, set as maximum
  
  OCR2A = cycles;                                          // ICR1 is TOP in p & f correct pwm mode  
  TCCR2B &= ~(_BV(CS20) | _BV(CS21) | _BV(CS22));
  TCCR2B |= t2bits;                                          // reset clock select register, and starts the clock

	tstreamCallback = burst_sm;
}

void DStream::DeleteExperiments(uint8_t nb)
{
	if(nb == 1){
		Channel1.Destroy();
	}
	else if(nb == 2){
		Channel2.Destroy();	
	}
	else if(nb == 3){
		Channel3.Destroy();
	
	}
	else if(nb == 4){
		Channel4.Destroy();	
	}
	else{
		Channel1.Destroy();
		Channel2.Destroy();
		Channel3.Destroy();
		Channel4.Destroy();	
	}

}

void DStream::CheckTriggers()
{	
	if(Channel1.state == CH_READY){
		if(Channel1.CheckMyTrigger())
			Channel1.Enable();
	}
	if(Channel2.state == CH_READY){
		if(Channel2.CheckMyTrigger())
			Channel2.Enable();
	}	
	if(Channel3.state == CH_READY){
		if(Channel3.CheckMyTrigger())
			Channel3.Enable();
	}	
	if(Channel4.state == CH_READY){
		if(Channel4.CheckMyTrigger())
			Channel4.Enable();
	}	
}

void DStream::SetupChan(uint8_t nb, unsigned long maxpoints, int repeat)
{
	switch(nb){
		case 4: 
			Channel4.Setup(maxpoints,repeat);
		break;
		case 3: 
			Channel3.Setup(maxpoints,repeat);
		break;
		case 2: 
			Channel2.Setup(maxpoints,repeat);
		break;
		case 1:
		default:
			Channel1.Setup(maxpoints,repeat);
		break;
	}
}

void DStream::TriggerMode(uint8_t nb, int trigger_mode, int16_t trigger_value)
{
	switch(nb){
		case 4: 
			Channel4.TriggerConfig(trigger_mode,trigger_value);
		break;
		case 3: 
			Channel3.TriggerConfig(trigger_mode,trigger_value);
		break;
		case 2: 
			Channel2.TriggerConfig(trigger_mode,trigger_value);
		break;
		case 1:
		default:
			Channel1.TriggerConfig(trigger_mode,trigger_value);
		break;
	}
}


void DStream::ConfigChan(uint8_t nb, int mode)
{
	switch(nb){
		case 4: 
			Channel4.Configure(mode);
		break;
		case 3: 
			Channel3.Configure(mode);
		break;
		case 2: 
			Channel2.Configure(mode);
		break;
		case 1:
		default:
			Channel1.Configure(mode);
		break;
	}
}

void DStream::ConfigChan(uint8_t nb, int mode, int pchan, int nchan)
{
	switch(nb){
		case 4: 
			Channel4.Configure(mode, pchan, nchan);
		break;
		case 3: 
			Channel3.Configure(mode, pchan, nchan);
		break;
		case 2: 
			Channel2.Configure(mode, pchan, nchan);
		break;
		case 1:
		default:
			Channel1.Configure(mode, pchan, nchan);
		break;
	}

}

void DStream::ConfigChan(uint8_t nb, int mode, int pchan, int nchan, int gain)
{
	switch(nb){
		case 4: 
			Channel4.Configure(mode, pchan, nchan, gain);
		break;
		case 3: 
			Channel3.Configure(mode, pchan, nchan, gain);
		break;
		case 2: 
			Channel2.Configure(mode, pchan, nchan, gain);
		break;
		case 1:
		default:
			Channel1.Configure(mode, pchan, nchan, gain);
		break;
	}
	
}

void DStream::ConfigChan(uint8_t nb, int mode, int pchan, int nchan, int gain, int nsamples)
{
	switch(nb){
		case 4: 
			Channel4.Configure(mode, pchan, nchan, gain, nsamples);
		break;
		case 3: 
			Channel3.Configure(mode, pchan, nchan, gain, nsamples);
		break;
		case 2: 
			Channel2.Configure(mode, pchan, nchan, gain, nsamples);
		break;
		case 1:
		default:
			Channel1.Configure(mode, pchan, nchan, gain, nsamples);
		break;
	}
	
}

void DStream::ConfigChan(uint8_t nb, int mode, int channel)
{
	switch(nb){
		case 4: 
			Channel4.Configure(mode, channel);
		break;
		case 3: 
			Channel3.Configure(mode, channel);
		break;
		case 2: 
			Channel2.Configure(mode, channel);
		break;
		case 1:
		default:
			Channel1.Configure(mode, channel);
		break;
	}
}
	
void DStream::FlushChan(uint8_t nb)
{
	switch(nb){
		case 4: 
			Channel4.Flush();
		break;
		case 3: 
			Channel3.Flush();
		break;
		case 2: 
			Channel2.Flush();
		break;
		case 1:
			Channel1.Flush();
		break;
		case 0:
		default:
			Channel1.Flush();
			Channel2.Flush();
			Channel3.Flush();
			Channel4.Flush();
		break;

	}

}




// External functions //////////////////////////////////////////////////////////


void stream_sm()
{
	//nTemp--> variable for counting slots of 500us
	static int ledStatus=0;
	static unsigned long nTemp=0;
	static int blocked=0;
	int p;
	int i,datapack=0;
	//ACTIVATIONS
	if( (Channel1.state == CH_RUN) && (Channel1.dctype==STREAM_TYPE) ){
		if(((nTemp)%(Channel1.period))==0){
			ledStatus = ~ledStatus;
			ledSet(LEDRED,ledStatus);
			pioWrite(0, ledStatus);
			Channel1.Activate();
			Channel1.waitStabilization();
			Channel1.Action();
		}
	}
	
	if( (Channel2.state == CH_RUN) && (Channel2.dctype==STREAM_TYPE) ){
		if(((nTemp)%(Channel2.period))==0){
			Channel2.Activate();
			Channel2.waitStabilization();
			Channel2.Action();
		}
	}
	
	if( (Channel3.state == CH_RUN) && (Channel3.dctype==STREAM_TYPE) ){
		if(((nTemp)%(Channel3.period))==0){
			Channel3.Activate();
			Channel3.waitStabilization();
			Channel3.Action();
		}	
	}
	
	if( (Channel4.state == CH_RUN) && (Channel4.dctype==STREAM_TYPE) ){
		if(((nTemp)%(Channel4.period))==0){
			Channel4.Activate();
			Channel4.waitStabilization();
			Channel4.Action();
		}	
	}
	
	nTemp++;
}

void burst_sm()
{
	if(Channel1.state == CH_RUN){
		Channel1.Activate();
		Channel1.Action();
	}
}

void ext_sm(int currentValue)
{	
	int p,pio;
	int i,datapack=0;
	//ACTIVATIONS
	if( (Channel1.state == CH_RUN) && (Channel1.dctype==EXTERNAL_TYPE) ){
		if(((currentValue & 0x80)>0) == Channel1.edge){//TODO:COMPROBAR QUE EFECTIVAMENTE HUBO FLANCO EN ESTE PIO CONCRETO
			Channel1.Activate();
			Channel1.waitStabilization();
			Channel1.Action();
		}	
	}
	
	if( (Channel2.state == CH_RUN) && (Channel2.dctype==EXTERNAL_TYPE) ){
		if(((currentValue & 0x40)>0) == Channel2.edge){
			Channel2.Activate();
			Channel2.waitStabilization();
			Channel2.Action();
		}	
	}
	
	if( (Channel3.state == CH_RUN) && (Channel3.dctype==EXTERNAL_TYPE) ){
		if(((currentValue & 0x20)>0) == Channel3.edge){
			Channel3.Activate();
			Channel3.waitStabilization();
			Channel3.Action();
		}	
	}
	
	if( (Channel4.state == CH_RUN) && (Channel4.dctype==EXTERNAL_TYPE) ){
		if(((currentValue & 0x10)>0) == Channel4.edge){
			Channel4.Activate();
			Channel4.waitStabilization();
			Channel4.Action();
		}	
	}
}

/////////////////////////////////////////////////////////////////////////////
// INTERRUPT GENERAL ROUTINES:
/////////////////////////////////////////////////////////////////////////////


ISR(TIMER2_COMPA_vect)          
{
	
	ODStream.tstreamCallback();
}


ISR(PCINT0_vect) 
{
	static unsigned interrupt;
	unsigned int i,j;
	int currentValue,refreshValue;
	if(PCIFR !=0)
		return;
	if(interrupt)
	{
		interrupt=0;
		return;
	}
	//This is a bucle for wait and avoid fakes edges
	for(i=0;i<200;i++)
	{
		refreshValue = PINA;
		for(j=0;j<200;j++)
		{
			refreshValue+=PINA;
		}
	}
	
	ext_sm(refreshValue);
	
	//if a interrupt was caused, next re-entry will return inmediatly
	if(PCIFR!=0)
	{
		interrupt=1;
	}
	else
		interrupt=0;
}
