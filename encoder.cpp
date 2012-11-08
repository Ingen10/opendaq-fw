#ifndef ENCODER_CPP
#define ENCODER_CPP

#include "encoder.h"
#include "commdata.h"

Encoder encoder;

// Public Methods //////////////////////////////////////////////////////////////
void Encoder::Start(unsigned int res)
{
	position=0;
	resolution = res;
	stabilizationTime=100;
	//Digital pin 5 as input
	DDRD &= ~(_BV(DDD5));					//input direction
	PORTD &= ~(_BV(PD5));
	
	EICRA = _BV(ISC11);
	EIMSK |= _BV(INT1);
	
}

void Encoder::Stop()
{
	EIMSK = 0;
}

void Encoder::Pause()
{
	EIMSK = 0;
}

void Encoder::Resume()
{
	EIMSK = 1;
}

int Encoder::get_position()
{
	return position;
}
void Encoder::increment_position()
{
	position++;
	if(position>resolution)
		position=0;
}

void Encoder::decrement_position()
{
	position--;
	if(position<0)
		position=resolution;
}


int dummyfunction(int j);

int Encoder::waitStabilization()
{
	int dummy,i,j;
	for(i=0;i<stabilizationTime;i++)
	{
		for(j=0;j<8000;j++)
		{
			dummyfunction(j);
		}
	}
	return dummy;
}

/////////////////////////////////////////////////////////////////////////////
// INTERRUPT GENERAL ROUTINES:
/////////////////////////////////////////////////////////////////////////////

ISR(INT1_vect) 
{
	if(PORTD && _BV(PD5))
	{
		Serial.print("Incrementando");
		encoder.increment_position();
	}
	else
	{
		Serial.print("Decrementando");
		encoder.decrement_position();
	}
	encoder.waitStabilization();
}


#endif

