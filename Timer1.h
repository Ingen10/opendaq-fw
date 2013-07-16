/*
 *  TIMER1 utilities for ATmega644P
 *
 *  Original code by Jesse Tane for http://labs.ideo.com August 2008
 *  Modified March 2009 by Jérôme Despatis and Jesse Tane for ATmega328 support
 *  Modified June 2009 by Michael Polli and Jesse Tane to fix a bug in setPeriod() which caused the timer to stop
 *  Modified June 2011 by Lex Talionis to add a function to read the timer
 *  Modified Oct 2011 by Andrew Richards to avoid certain problems:
 *  Modified 7:26 PM Sunday, October 09, 2011 by Lex Talionis
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 2.1 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
  
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#ifndef TIMERONE_h
#define TIMERONE_h

#include <avr/io.h>
#include <avr/interrupt.h>

#include "daqhw.h"


#define RESOLUTION 65536    // Timer1 is 16 bit

class TimerOne
{
  public:
  
	// properties
	unsigned long hValue,lValue;	//capture mode periods
	unsigned long nOverflows;		//number of timer overflows
	unsigned int pwmPeriod;
	unsigned char clockSelectBits;
	char oldSREG;					// To hold Status Register while ints disabled

	// methods
	void initialize(long microseconds=1000000);
	void start();
	void stop();
	void restart();
	void resume();
	unsigned long read();
	void attachInterrupt(void (*isr)(), long microseconds=-1);
	void detachInterrupt();
	void setPeriod(unsigned long microseconds);
	void pwm(char pin, int duty, long microseconds=-1);
	void disablePwm(char pin);
	void setPwmDuty(char pin, int duty);
	void startCapture(unsigned long microseconds);
	void stopCapture();
	unsigned long  getCapture(int state);
	void startCounter(int edge);
	unsigned long getCounter(int reset);
	void (*isrCallback)();
	void icpCallback();
};

extern TimerOne Timer1;
#endif
