#ifndef ODSTREAM_H
#define ODSTREAM_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "datachannel.h"

#include "HardwareSerial.h"


//EDGE MODES
#define H_TO_L		0
#define L_TO_H		1


void stream_sm();
void burst_sm();
void ext_sm();


	
/******************************************************************************
 * Class
 ******************************************************************************/
 
//! DStream Class
/*!
	DStream Class defines all the variables and functions used for managing the automated data acquisition for openDAQ
 */

class DStream
{
  private:
  // properties
  
  // methods

  public:
	
	void (*tstreamCallback)();
	void ext_sm();
	
  // properties
	//! Variable : specifies ...
  	/*!
   	*/
  int usingch;

	

  // methods

	//! class constructor
  /*!
	They initialize the channel
	\param void
	\return void
	*/
	void Initialize();
	void Start();
	void Stop();
	void Pause();
	void Resume();

	
	void CreateStreamChannel(uint8_t nb, unsigned long ms_period);		//TODO: change second parameter to ms_period
	void CreateExternalChannel(uint8_t nb, uint8_t edge);
	void CreateExternalChannel(uint8_t nb);
	void CreateBurstChannel(unsigned long us_period);
	
	void ConfigChan(uint8_t nb, int mode);
	void ConfigChan(uint8_t nb, int mode, int pchan, int nchan);
	void ConfigChan(uint8_t nb, int mode, int pchan, int nchan, int gain);
	void ConfigChan(uint8_t nb, int mode, int pchan, int nchan, int gain, int nsamples);
	void ConfigChan(uint8_t nb, int mode, int channel);
	
	void TriggerMode(uint8_t nb, int trigger_mode, int16_t trigger_value);
	void CheckTriggers();
	
	void SetupChan(uint8_t nb, unsigned long maxpoints, int repeat);
	
	void DeleteExperiments(uint8_t nb);
	void FlushChan(uint8_t nb);

};

#endif

