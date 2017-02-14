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
 *  Version:    150701
 *  Author:     JRB
 *  Revised by: AV (17/07/15)
 */


/**
 * @file Timer1.cpp
 * Source code for Timer1 class
 */

#include "Timer1.h"

TimerOne Timer1; // preinstatiate
/** \brief
 *  Interrupt service routine that wraps a user defined function supplied by
 *  attachInterrupt
 *
 */
ISR(TIMER1_OVF_vect) {
    Timer1.nOverflows++; // incriment overflow count
    if (Timer1.isrCallback != NULL)
        Timer1.isrCallback();
}

/** 
 *  Initialize the timer
 *
 *  \param
 *  microseconds: microseconds to set the timer period
 */
void TimerOne::initialize(long microseconds) {
    TCCR1A = 0; // clear control register A
    TCCR1B = _BV(WGM13); // set mode 8: phase and frequency correct pwm, stop the timer
    setPeriod(microseconds);
}

/** 
 *  Set the timer period
 *
 *  \param
 *  microseconds: microseconds to set the timer period
 */
void TimerOne::setPeriod(unsigned long microseconds) // AR modified for atomic access
{

    long cycles = (F_CPU / 2000000) * microseconds; // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
    if (cycles < RESOLUTION) clockSelectBits = _BV(CS10); // no prescale, full xtal
    else if ((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11); // prescale by /8
    else if ((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11) | _BV(CS10); // prescale by /64
    else if ((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12); // prescale by /256
    else if ((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12) | _BV(CS10); // prescale by /1024
    else cycles = RESOLUTION - 1, clockSelectBits = _BV(CS12) | _BV(CS10); // request was out of bounds, set as maximum

    oldSREG = SREG;
    cli(); // Disable interrupts for 16 bit register access
    ICR1 = pwmPeriod = cycles; // ICR1 is TOP in p & f correct pwm mode
    SREG = oldSREG;

    TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
    TCCR1B |= clockSelectBits; // reset clock select register, and starts the clock
}

/** \brief
 *  Set the pwm duty
 *
 *  \param   pin: timer pin
 *  \param duty: duty to set up
 */
void TimerOne::setPwmDuty(char pin, int duty) {
    unsigned long dutyCycle = pwmPeriod;

    dutyCycle *= duty;
    dutyCycle >>= 10;

    oldSREG = SREG;
    cli();
    if (pin == 1 || pin == 5) OCR1A = dutyCycle;
    else if (pin == 2 || pin == 4) OCR1B = dutyCycle;
    SREG = oldSREG;
}

/** \brief
 *  Set the pwm duty and the timer period
 *
 *  \param  pin: timer pin
 *  \param duty: duty to set up
 *  \param microseconds: microseconds to set the timer period
 */
void TimerOne::pwm(char pin, int duty, long microseconds) // expects duty cycle to be 10 bit (1024 max)
{
    TCCR1A = 0;
    TCCR1B = 0;

    if (microseconds > 0) setPeriod(microseconds);

    setPwmDuty(pin, duty);

    TCCR1A = 0X82;
    TCCR1B |= 0x18; //TIMER 1 MODE 14 (FAST PWM)

    if (pin == 1 || pin == 5) {
        DDRD |= _BV(DDD5); // sets data direction register for pwm output pin                                 // activates the output pin
        TCCR1A |= _BV(COM1A1);
    } else if (pin == 2 || pin == 4) {
        DDRD |= _BV(DDD4);
        TCCR1A |= _BV(COM1B1);
    }

    resume(); // Lex - make sure the clock is running.  We don't want to restart the count, in case we are starting the second WGM
    // and the first one is in the middle of a cycle
}

/** \brief
 *  Disable pwm
 *
 *  \param
 *  pin: timer pin
 */
void TimerOne::disablePwm(char pin) {
    if (pin == 1 || pin == 5) TCCR1A &= ~_BV(COM1A1); // clear the bit that enables pwm on PB1
    else if (pin == 2 || pin == 4) TCCR1A &= ~_BV(COM1B1); // clear the bit that enables pwm on PB2
}

/** \brief
 *  Attach an interruption
 *
 *  \param  isr: the real isr
 *  \param microseconds: microseconds to set the timer period
 */
void TimerOne::attachInterrupt(void (*isr)(), long microseconds) {
    if (microseconds > 0) setPeriod(microseconds);
    isrCallback = isr; // register the user's callback with the real ISR
    TIMSK1 = _BV(TOIE1); // sets the timer overflow interrupt enable bit
    // AR - remove sei() - might be running with interrupts disabled (eg inside an ISR), so leave unchanged
    //  sei();                                                   // ensures that interrupts are globally enabled
    resume();
}

/** \brief
 *  Detach an interruption
 *
 */
void TimerOne::detachInterrupt() {
    TIMSK1 &= ~_BV(TOIE1); // clears the timer overflow interrupt enable bit
}

/** \brief
 *  Resume the timer
 *
 */
void TimerOne::resume() // AR suggested
{
    TCCR1B |= clockSelectBits;
}

/** \brief
 *  Restart the timer
 *
 */
void TimerOne::restart() // Depricated - Public interface to start at zero - Lex 10/9/2011
{
    start();
}

/** \brief
 *  Start the timer
 *
 */
void TimerOne::start() // AR addition, renamed by Lex to reflect it's actual role
{
    unsigned int tcnt1;

    TIMSK1 &= ~_BV(TOIE1); // AR added
    GTCCR |= _BV(PSRSYNC); // AR added - reset prescaler (NB: shared with all 16 bit timers);

    oldSREG = SREG; // AR - save status register
    cli(); // AR - Disable interrupts
    TCNT1 = 0;
    SREG = oldSREG; // AR - Restore status register

    do { // Nothing -- wait until timer moved on from zero - otherwise get a phantom interrupt
        oldSREG = SREG;
        cli();
        tcnt1 = TCNT1;
        SREG = oldSREG;
    } while (tcnt1 == 0);

    TIFR1 = 0xff;                   // AR - Clear interrupt flags
    //TIMSK1 = _BV(TOIE1);              // sets the timer overflow interrupt enable bit
}

/** \brief
 *  Stop the timer
 *
 */
void TimerOne::stop() {
    TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12)); // clears all clock selects bits
}

/** \brief
 *  Read the timer value
 *
 *  \return
 *  The value of the timer in microseconds
 */
unsigned long TimerOne::read() //returns the value of the timer in microseconds
{
    //rember! phase and freq correct mode counts up to then down again
    unsigned long tmp; // AR amended to hold more than 65536 (could be nearly double this)
    unsigned int tcnt1; // AR added

    oldSREG = SREG;
    cli();
    tmp = TCNT1;
    SREG = oldSREG;

    char scale = 0;
    switch (clockSelectBits) {
        case 1:// no prescalse
            scale = 0;
            break;
        case 2:// x8 prescale
            scale = 3;
            break;
        case 3:// x64
            scale = 6;
            break;
        case 4:// x256
            scale = 8;
            break;
        case 5:// x1024
            scale = 10;
            break;
    }

    do { // Nothing -- max delay here is ~1023 cycles.  AR modified
        oldSREG = SREG;
        cli();
        tcnt1 = TCNT1;
        SREG = oldSREG;
    } while (tcnt1 == tmp); //if the timer has not ticked yet

    //if we are counting down add the top value to how far we have counted down
    tmp = ((tcnt1 > tmp) ? (tmp) : (long) (ICR1 - tcnt1) + (long) ICR1); // AR amended to add casts and reuse previous TCNT1
    return ((tmp * 1000L) / (F_CPU / 1000L)) << scale;
}

/** \brief
 *  Start the timer in capture mode
 *
 *  \param
 *  microseconds: microseconds to set the timer period
 */
void TimerOne::startCapture(unsigned long microseconds)
//Capture mode. Approximate period in argument to set clock divider
{

    TCCR1A = 0; //Reset registers
    TCCR1B = 0;

    DDRD &= ~(_BV(DDD6)); //input direction for ICP1 pin

    TCCR1B = _BV(CS12); // NORMAL MODE!!, prescaller 8, rising edge ICP1 - this works
    if (microseconds > 0) setPeriod(microseconds << 1);
    TCCR1B |= _BV(ICES1); // enable input capture

    hValue = 0; // Init counters
    lValue = 0;
    nOverflows = 0;

    TIMSK1 = _BV(ICIE1); // enable input capture interrupt for timer 1
    TIMSK1 |= _BV(TOIE1); // enable overflow interrupt to detect missing input pulses

    resume();
}

/** \brief
 *  Stop capture mode
 *
 */
void TimerOne::stopCapture()
//Stops capture mode
{
    TIMSK1 &= ~(_BV(ICIE1) | _BV(TOIE1));
    stop();
}

ISR(TIMER1_CAPT_vect)
//ICR interrupt vector
{
    Timer1.icpCallback();
}

/** \brief
 *  ICR capture interrupt callback
 *
 */
void TimerOne::icpCallback()
//ICR capture interrupt callback
{

    TCNT1 = 0; // reset the counter
    if (bit_is_set(TCCR1B, ICES1)) { // rising edge detected
        lValue = ICR1;
        if (nOverflows > 0) {
            lValue += nOverflows * 65536;
        }
        //pioWrite(2, HIGH);

    } else { // falling edge detected
        hValue = ICR1; // save the input capture value
        if (nOverflows > 0) {
            hValue += nOverflows * 65536;
        }
        //pioWrite(2, LOW);
    }
    nOverflows = 0;
    TCCR1B ^= _BV(ICES1); // toggle bit value to trigger on the other edge

}

/** \brief
 *  Returns semi-period length, in microseconds
 *
 *  \param
 *  state: high (1) or low (0)
 *  \return
 *  Semi-period length, in microseconds
 */
unsigned long TimerOne::getCapture(int state)
//Returns semi-period length, in microseconds: high (1) or low (0) state in argument
{
    unsigned long result;

    if (state == 1)
        result = hValue; //High duty period
    else if (state == 0)
        result = lValue; //Low duty period
    else
        result = hValue + lValue; //Total period

    char scale = 0;
    switch (clockSelectBits) {
        case 1:// no prescalse
            scale = 0;
            break;
        case 2:// x8 prescale
            scale = 3;
            break;
        case 3:// x64
            scale = 6;
            break;
        case 4:// x256
            scale = 8;
            break;
        case 5:// x1024
            scale = 10;
            break;
    }

    return ((result * 1000L) / (F_CPU / 1000L)) << scale;

}

/** \brief
 *  Start the timer counter
 *
 *  \param
 *  edge: timer edge
 */
void TimerOne::startCounter(int edge) {
    TCCR1A = 0; //Reset registers
    TCCR1B = 0;

    DDRB &= ~(_BV(DDB1)); //input direction for T1 pin

    clockSelectBits = _BV(CS12);
    clockSelectBits |= _BV(CS11);

    if (edge > 0)
        clockSelectBits |= _BV(CS10); // clock on rising edge

    oldSREG = SREG; // AR - save status register
    cli(); // AR - Disable interrupts
    TCNT1 = 0;
    nOverflows = 0;
    TIMSK1 = _BV(TOIE1); // enable overflow interrupt to detect missing input pulses
    SREG = oldSREG; // AR - Restore status register


    resume();
}

/** \brief
 *  Get the timer counter
 *
 *  \param
 *  reset: flag to reset the counter
 *  \return
 *  Timer counter
 */
unsigned long TimerOne::getCounter(int reset) {
    unsigned long tcnt1_val,aux;

    oldSREG = SREG;
    cli();

    tcnt1_val = TCNT1;

    if (nOverflows > 0) {
        aux = nOverflows;
        tcnt1_val += aux * 65536;
    }

    if (reset > 0) {
        TCNT1 = 0;
        nOverflows = 0;
    }

    SREG = oldSREG;

    resume();

    return tcnt1_val;
}
