#ifndef ODSTREAM_CPP
#define ODSTREAM_CPP

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
		TIMSK2 = _BV(OCIE2A);
	}

	if( (Channel1.dctype == EXTERNAL_TYPE) || (Channel2.dctype == EXTERNAL_TYPE) || 
		(Channel3.dctype == EXTERNAL_TYPE) || (Channel4.dctype == EXTERNAL_TYPE) ){
		PCICR |= _BV(PCIE3);						//Pin Change Interrupt Enable 3 (PCINT31..24)
	}
	
	if(Channel1.dctype == BURST_TYPE){
		Channel1.Activate();
		usingch = 1;
	}
		
	CheckTriggers();
}

void DStream::Stop()
{
	TIMSK2 &= ~_BV(OCIE2A);
	PCICR &= ~_BV(PCIE3);
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
	OCR2A = 249;              //Prescaler: 250us per interrupt

	tstreamCallback = stream_sm;
	usingch = 0;
}

void DStream::CreateExternalChannel(uint8_t nb, uint8_t edge)
{

	if(nb==1){
		Channel1.Destroy();
		Channel1=DataChannel(EXTERNAL_TYPE,1,edge);
	}
	else if(nb==2){
		Channel2.Destroy();
		Channel2=DataChannel(EXTERNAL_TYPE,2,edge);
	}
	else if(nb==3){
		Channel3.Destroy();
		Channel3=DataChannel(EXTERNAL_TYPE,3,edge);
	}
	else if(nb==4){
		Channel4.Destroy();
		Channel4=DataChannel(EXTERNAL_TYPE,4,edge);
	}
  
  //TODO: assign to real PIO entries
	PCMSK3 |= _BV(PCINT27);  				//Pin Change Enable Mask 31:24 (PCINT31 & PCINT30 enabled)
	
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

void DStream::DeleteAllExperiments()
{
	Channel1.Destroy();
	Channel2.Destroy();
	Channel3.Destroy();
	Channel4.Destroy();
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
	static unsigned long nTemp=0;
	static int blocked=0;
	int p;
	int i,datapack=0;
	
	//ACTIVATIONS
	if( (Channel1.state == CH_RUN) && (Channel1.dctype==STREAM_TYPE) ){
		if(((nTemp+10)%Channel1.period)==0){
			Channel1.ready = 1;
			ActivateAnalogInput(1);
		}
		if((Channel1.ready)){ 
			if(ODStream.usingch != 1){
				if(!blocked){
					Channel1.Activate();
					ODStream.usingch = 1;
					blocked = 1;
				}
			}
			else{
				Channel1.Action();
				Channel1.ready = 0;
				blocked = 0;
				ActivateAnalogInput(0);
			}
		}	
	}
	
	if( (Channel2.state == CH_RUN) && (Channel2.dctype==STREAM_TYPE) ){
		if(((nTemp+10)%Channel2.period)==0){
			Channel2.ready = 1;
			ActivateAnalogInput(2);
		}
		if((Channel2.ready)){ 
			if(ODStream.usingch != 2){
				if(!blocked){
					Channel2.Activate();
					ODStream.usingch = 2;
					blocked = 1;
				}
			}
			else{
				Channel2.Action();
				Channel2.ready = 0;
				blocked = 0;
				ActivateAnalogInput(0);
			}
		}	
	}
	
	if( (Channel3.state == CH_RUN) && (Channel3.dctype==STREAM_TYPE) ){
		if(((nTemp+10)%Channel3.period)==0){
			Channel3.ready = 1;
			ActivateAnalogInput(3);
		}
		if((Channel3.ready)){ 
			if(ODStream.usingch != 3){
				if(!blocked){
					Channel3.Activate();
					ODStream.usingch = 3;
					blocked = 1;
				}
			}
			else{
				Channel3.Action();
				Channel3.ready = 0;
				blocked = 0;
				ActivateAnalogInput(0);
			}
		}	
	}
	
	if( (Channel4.state == CH_RUN) && (Channel4.dctype==STREAM_TYPE) ){
		if(((nTemp+10)%Channel4.period)==0){
			Channel4.ready = 1;
			ActivateAnalogInput(4);
		}
		if((Channel4.ready)){ 
			if(ODStream.usingch != 4){
				if(!blocked){
					Channel4.Activate();
					ODStream.usingch = 4;
					blocked = 1;
				}
			}
			else{
				Channel4.Action();
				Channel4.ready = 0;
				blocked = 0;
				ActivateAnalogInput(0);
			}
		}	
	}

  ActivateAnalogInput(0);
	
	nTemp++;
}

void burst_sm()
{
	if(Channel1.state == CH_RUN){
		ActivateAnalogInput(1);
		Channel1.Action();
		ActivateAnalogInput(0);
	}
}

void ext_sm()
{	
	int p;
	int i,datapack=0;
	
	//ACTIVATIONS
	if( (Channel1.state == CH_RUN) && (Channel1.dctype==EXTERNAL_TYPE) ){
		if(pioRead(5) == Channel1.edge){//TODO:COMPROBAR QUE EFECTIVAMENTE HUBO FLANCO EN ESTE PIO CONCRETO
			if(ODStream.usingch != 1)
				Channel1.Activate();
			ActivateAnalogInput(1);
			Channel1.Action();
			ODStream.usingch = 1;
		}	
	}
	
	if( (Channel2.state == CH_RUN) && (Channel2.dctype==EXTERNAL_TYPE) ){
		if(pioRead(5) == Channel2.edge){
			if(ODStream.usingch != 2)
				Channel2.Activate();
			ActivateAnalogInput(2);
			Channel2.Action();
			ODStream.usingch = 2;
		}	
	}
	
	if( (Channel3.state == CH_RUN) && (Channel3.dctype==EXTERNAL_TYPE) ){
		if(pioRead(5) == Channel3.edge){
			if(ODStream.usingch != 3)
				Channel3.Activate();
			ActivateAnalogInput(3);
			Channel3.Action();
			ODStream.usingch = 3;
		}	
	}
	
	if( (Channel4.state == CH_RUN) && (Channel4.dctype==EXTERNAL_TYPE) ){
		if(pioRead(5) == Channel4.edge){
			if(ODStream.usingch != 4)
				Channel4.Activate();
			ActivateAnalogInput(4);
			Channel4.Action();
			ODStream.usingch = 4;
		}	
	}

  ActivateAnalogInput(0);	
}

/////////////////////////////////////////////////////////////////////////////
// INTERRUPT GENERAL ROUTINES:
/////////////////////////////////////////////////////////////////////////////


ISR(TIMER2_COMPA_vect)          
{
  ODStream.tstreamCallback();
}


ISR(PCINT3_vect) 
{
	ext_sm();

}


#endif

