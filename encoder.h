#ifndef ENCODER_H
#define ENCODER_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "datachannel.h"

#include "HardwareSerial.h"


//EDGE MODES
#define H_TO_L		0
#define L_TO_H		1



	
/******************************************************************************
 * Class
 ******************************************************************************/
 
//! Encoder Class

class Encoder
{
  private:
  // properties
  unsigned int position;
  unsigned int resolution;
  unsigned int stabilizationTime;
  // methods

  public:
	
  // properties
	//! Variable : specifies ...
  	/*!
   	*/

	

  // methods

	//! class constructor
  /*!
	They initialize the channel
	\param void
	\return void
	*/
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

