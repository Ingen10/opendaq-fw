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
 *  Version:    140207
 *  Author:     JRB
 *  Revised by: AV (07/02/14)
 */

#include "odstream.h"

#define T2_RESOLUTION 256   // Timer2 is 8 bit


const int DC_PCINT[] = {PCINT7, PCINT6, PCINT5, PCINT4};
const int DC_DD[] = {DDA7, DDA6, DDA5, DDA4};
const int DC_PTNB[] = {0X80, 0X40, 0X20, 0X10};

DStream ODStream;

DataChannel Channel1 = DataChannel(INACTIVE_TYPE);
DataChannel Channel2 = DataChannel(INACTIVE_TYPE);
DataChannel Channel3 = DataChannel(INACTIVE_TYPE);
DataChannel Channel4 = DataChannel(INACTIVE_TYPE);

DataChannel channels[] = {Channel1, Channel2, Channel3, Channel4};



// Public Methods //////////////////////////////////////////////////////////////

/** \brief
 *  Initialize the stream channel
 *
 */
void DStream::Initialize() {
    TCCR2A = 0; // clear control register A
    // set mode 8: phase and frequency correct pwm, stop the timer
    TCCR2B = _BV(WGM13);

}

/** \brief
 *  Start the stream channel
 *
 */
void DStream::Start() {

    for (int i = 0; i < 4; i++)
        channels[i].Flush();

    if ((channels[0].dctype >= STREAM_TYPE) ||
            (channels[1].dctype >= STREAM_TYPE) ||
            (channels[2].dctype >= STREAM_TYPE) ||
            (channels[3].dctype >= STREAM_TYPE)) {
        TCNT2 = 0;
        TIMSK2 = _BV(OCIE2A);
    }

    if ((channels[0].dctype == EXTERNAL_TYPE) ||
            (channels[1].dctype == EXTERNAL_TYPE) ||
            (channels[2].dctype == EXTERNAL_TYPE) ||
            (channels[3].dctype == EXTERNAL_TYPE)) {
        PCICR |= _BV(PCIE0); //Pin Change Interrupt Enable 0 (PCINT31..24)
    }

    if (channels[0].dctype == BURST_TYPE) {
        channels[0].Activate();
        usingch = 1;
    }

    CheckTriggers();
    ledSet(LEDGREEN, 1);
    ledSet(LEDRED, 1);
    stream_sm();
}

/** \brief
 *  Stop the stream channel
 *
 */
void DStream::Stop() {
    for (int i = 0; i < 4; i++)
        channels[i].reset();

    TIMSK2 &= ~_BV(OCIE2A);
    PCICR &= ~_BV(PCIE0);
    ledSet(LEDGREEN, 1);
    ledSet(LEDRED, 0);
}

void DStream::Pause() {
}

void DStream::Resume() {
}

/** \brief
 *  Create a stream channel
 *
 *  \param
 *  nb: channel number
 *  ms_period: channel period
 */
void DStream::CreateStreamChannel(uint8_t nb, unsigned long ms_period) {
    channels[nb - 1].Destroy();
    channels[nb - 1] = DataChannel(STREAM_TYPE, (unsigned long) ms_period);

    if (channels[0].dctype == BURST_TYPE)
        channels[0].Destroy(); //if any channel configured as stream, burst mode is disabled

    TCCR2A = 0X82; //CTC MODE, CLEAR OC2A ON COMPARE
    TCCR2B = 4; //Main clock /64 (2MHz)
    OCR2A = 250; //Prescaler: 250us per interrupt

    tstreamCallback = stream_sm;
    usingch = 0;
}

/** \brief
 *  Create an external channel
 *
 *  \param
 *  nb: channel number
 *  edge: channel edge
 */
void DStream::CreateExternalChannel(uint8_t nb, uint8_t edge) {

    channels[nb - 1].Destroy();
    channels[nb - 1] = DataChannel(EXTERNAL_TYPE, nb - 1, edge);

    if (channels[0].dctype == BURST_TYPE)
        channels[0].Destroy(); //if any channel configured as external, burst mode is disabled

    //PCMSK0 |= _BV(PCINT7);  				//Pin Change Enable Mask
    //	DDRA &= ~(_BV(DDA7));					//input direction
    DDRA &= ~(_BV(DC_DD[nb - 1])); //input direction
    PCMSK0 |= _BV(DC_PCINT[nb - 1]); //Pin Change Enable Mask

    //stores variable with actual external input
    usingch = 0;
}

/** \brief
 *  Create an external channel with a default edge
 *
 *  \param
 *  nb: channel number
 */
void DStream::CreateExternalChannel(uint8_t nb) {
    CreateExternalChannel(nb, L_TO_H);
}

/** \brief
 *  Create a burst channel
 *
 *  \param
 *  us_period: channel period in microseconds
 */
void DStream::CreateBurstChannel(unsigned long us_period) {
    uint8_t t2bits;

    for (int i = 0; i < 4; i++)
        channels[i].Destroy();

    channels[0] = DataChannel(BURST_TYPE, us_period);

    TCCR2A = 0X82; //CTC MODE, CLEAR OC2A ON COMPARE

    // the counter runs backwards after TOP, interrupt is at BOTTOM so divide
    // microseconds by 2 
    long cycles = (F_CPU / 1000000) * us_period;

    if ((cycles >>= 3) < T2_RESOLUTION)
        t2bits = _BV(CS21); // prescale by /8
    else if ((cycles >>= 2) < T2_RESOLUTION)
        t2bits = _BV(CS21) | _BV(CS20); // prescale by /32
    else if ((cycles >>= 1) < T2_RESOLUTION)
        t2bits = _BV(CS22); // prescale by /64
    else if ((cycles >>= 2) < T2_RESOLUTION)
        t2bits = _BV(CS21) | _BV(CS22); // prescale by /256
    else if ((cycles >>= 2) < T2_RESOLUTION)
        t2bits = _BV(CS20) | _BV(CS21) | _BV(CS22); // prescale by /1024
    else {
        cycles = T2_RESOLUTION - 1;

	// request was out of bounds, set as maximum
	t2bits = _BV(CS20) | _BV(CS21) | _BV(CS22);
    }

    OCR2A = cycles; // ICR1 is TOP in p & f correct pwm mode
    TCCR2B &= ~(_BV(CS20) | _BV(CS21) | _BV(CS22));
    TCCR2B |= t2bits; // reset clock select register, and starts the clock

    tstreamCallback = burst_sm;
}

/** \brief
 *  Delete an experiment
 *
 *  \param
 *  nb: channel number to delete
 */
void DStream::DeleteExperiments(uint8_t nb) {
    if ((nb > 0) && (nb < 5))
        channels[nb - 1].Destroy();
    else {
        for (int i = 0; i < 4; i++)
            channels[i].Destroy();
    }
}

/** \brief
 *  Check the triggers of the channels
 */
void DStream::CheckTriggers() {
    for (int i = 0; i < 4; i++) {
        if (channels[i].state == CH_READY) {
            if (channels[i].CheckMyTrigger())
                channels[i].Enable();
        }
    }
}

/** \brief
 *  Set the max points of the channel and the repeats number
 *
 *  \param
 *  maxpoints: max number of points of the channel
 *  repeat: number of repeats
 */
void DStream::SetupChan(uint8_t nb, unsigned long maxpoints, int repeat) {
    channels[nb - 1].Setup(maxpoints, repeat);
}

/** \brief
 *  Set the trigger mode of the channel
 *
 *  \param
 *  nb: channel number
 *  trigger_mode: mode of the trigger
 *  trigger_value: value of the trigger
 */
void DStream::TriggerMode(uint8_t nb, int trigger_mode, int16_t trigger_value) {
    channels[nb - 1].TriggerConfig(trigger_mode, trigger_value);
}

/** \brief
 *  Set the mode of the channel
 *
 *  \param
 *  nb: channel number
 *  mode: mode to configure the channel
 */
void DStream::ConfigChan(uint8_t nb, int mode) {
    channels[nb - 1].Configure(mode);
}

/** \brief
 *  Set the mode of the channel
 *
 *  \param
 *  nb: channel number
 *  mode: mode to configure the channel
 *  pchan: channel of the stream
 */
void DStream::ConfigChan(uint8_t nb, int mode, int pchan) {
    channels[nb - 1].Configure(mode, pchan);
}

/** \brief
 *  Set the mode of the channel
 *
 *  \param
 *  nb: channel number
 *  mode: mode to configure the channel
 *  pchan: channel of the stream
 *  nchan: channel number
 */
void DStream::ConfigChan(uint8_t nb, int mode, int pchan, int nchan) {
    channels[nb - 1].Configure(mode, pchan, nchan);
}

/** \brief
 *  Set the mode and gain of the channel
 *
 *  \param
 *  nb: channel number
 *  mode: mode to configure the channel
 *  pchan: channel of the stream
 *  nchan: channel number
 *  gain: gain to set to the channel
 */
void DStream::ConfigChan(uint8_t nb, int mode, int pchan, int nchan, int gain) {
    channels[nb - 1].Configure(mode, pchan, nchan, gain);
}

/** \brief
 *  Set the modem, gain and number of samples of the channel
 *
 *  \param
 *  nb: channel number
 *  mode: mode to configure the channel
 *  pchan: channel of the stream
 *  nchan: channel number
 *  gain: gain to set to the channel
 *  nsamples: number of samples of the channel
 */
void DStream::ConfigChan(uint8_t nb, int mode, int pchan, int nchan, int gain, int nsamples) {
    channels[nb - 1].Configure(mode, pchan, nchan, gain, nsamples);
}

/** \brief
 *  Flush the channel
 *
 *  \param
 *  nb: channel number
 */
void DStream::FlushChan(uint8_t nb) {
    if ((nb > 0) && (nb < 5))
        channels[nb - 1].Flush();
    else {
        for (int i = 0; i < 4; i++)
            channels[i].Flush();
    }
}


///////////////////////////////////////////////////////////////////////////
// Access to DataChannels internal variables:
///////////////////////////////////////////////////////////////////////////

/** \brief
 *  Get the read index of the stream
 *
 *  \param
 *  nb: channel number
 *  \return
 *  Read index of the stream
 */
unsigned int DStream::ReadIndex(uint8_t nb) {
    return channels[nb - 1].readindex;
}

/** \brief
 *  Get the write index of the stream
 *
 *  \param
 *  nb: channel number
 *  \return
 *  Write index of the stream
 */
unsigned int DStream::WriteIndex(uint8_t nb) {
    return channels[nb - 1].writeindex;
}

/** \brief
 *  Get the state of the stream
 *
 *  \param
 *  nb: channel number
 *  \return
 *  State of the stream
 */
int DStream::State(uint8_t nb) {
    return channels[nb - 1].state;
}

/** \brief
 *  Get the number of datas of the stream
 *
 *  \param
 *  nb: channel number
 *  \return
 *  Number of datas of the stream
 */
unsigned long DStream::Ndata(uint8_t nb) {
    return channels[nb - 1].ndata;
}

/** \brief
 *  Get the channel of the stream
 *
 *  \param
 *  nb: channel number
 *  \return
 *  Channel of the stream
 */
int DStream::Pchan(uint8_t nb) {
    return channels[nb - 1].pch;
}

/** \brief
 *  Get the channel number of the stream
 *
 *  \param
 *  nb: channel number
 *  \return
 *  Channel number of the stream
 */
int DStream::Nchan(uint8_t nb) {
    return channels[nb - 1].nch;
}

/** \brief
 *  Get the gain of the stream
 *
 *  \param
 *  nb: channel number
 *  \return
 *  Gain of the stream
 */
int DStream::Gain(uint8_t nb) {
    return channels[nb - 1].g;
}

/** \brief
 *  Get the stream
 *
 *  \param
 *  nb: channel number
 *  \return
 *  Stream
 */
signed int DStream::Get(uint8_t nb) {
    return channels[nb - 1].Get();
}

/** \brief
 *  Get if the stream has reached the end
 *
 *  \param
 *  nb: channel number
 *  \return
 *  Return if the stream has reached the end
 */
int DStream::endReached(uint8_t nb) {
    return channels[nb - 1].endReached();
}

/** \brief
 *  Put a value into the stream
 *
 *  \param
 *  nb: channel number
 *  index: index to put the value
 *  value: data to put in the stream
 */
void DStream::Put(uint8_t nb, unsigned int index, signed int value) {
    channels[nb - 1].Put(index, value);
}

/** \brief
 *  Reset the stream
 *
 *  \param
 *  nb: channel number
 */
void DStream::Reset(uint8_t nb) {
    channels[nb - 1].reset();
}

// External functions //////////////////////////////////////////////////////////

/** \brief
 *  Execute an action using a normal stream
 *
 */
void stream_sm() {
    static int led_status = 0;
    static unsigned long ntemp = 0;
    // counts slots of 500us

    for (int i = 0; i < 4; i++) {
        if ((channels[i].state == CH_RUN) && (channels[i].dctype == STREAM_TYPE) && !(ntemp % channels[i].period)) {

            ledSet(LEDRED, 1);
            channels[i].Activate();
            channels[i].waitStabilization();
            channels[i].Action();
            ledSet(LEDRED, 0);
        }
    }

    ntemp++;
}

/** \brief
 *  Execute an action using a burst stream
 *
 */
void burst_sm() {
    if (channels[0].state == CH_RUN) {
        channels[0].Activate();
        channels[0].Action();
    }
}

/** \brief
 *  Execute an action using an external stream
 *
 *  \params
 *  bit_changes: value to detect the bits changes
 *  value: value to detect the stream edge
 */
void ext_sm(int bit_changes, int value) {
    for (int i = 0; i < 4; i++) {
        if ((channels[i].state == CH_RUN) && (channels[i].dctype == EXTERNAL_TYPE) && //channel external & running
                ((bit_changes & DC_PTNB[i]) != 0) && (((value & DC_PTNB[i]) != 0) == channels[i].edge)) { //edge detected & correct polarity

            ledSet(LEDRED, 1);
            channels[i].Activate();
            channels[i].waitStabilization();
            channels[i].Action();
            ledSet(LEDRED, 0);
        }
    }
}


/////////////////////////////////////////////////////////////////////////////
// INTERRUPT GENERAL ROUTINES:
/////////////////////////////////////////////////////////////////////////////

/** \brief
 *  Timer 2 interrupt: Stream And Burst experiments
 *
 */
ISR(TIMER2_COMPA_vect) {
    ODStream.tstreamCallback();
}

/** \brief
 *  External interrupt: External experiments on D1-D4 edge inputs
 *
 */
ISR(PCINT0_vect) {
    static unsigned interrupt;
    unsigned int i, j;
    static int lastValue = 0xF0;
    int refreshValue;
    int dif;

    if (PCIFR != 0)
        return;

    if (interrupt) {
        interrupt = 0;
        return;
    }
    //This is a bucle for waiting and avoid fake edges
    for (i = 0; i < 200; i++) {
        refreshValue = PINA;
        for (j = 0; j < 200; j++) {
            refreshValue &= PINA;
        }
    }

    dif = refreshValue^lastValue;
    ext_sm(dif, refreshValue);

    lastValue = refreshValue;

    //if a interrupt was detected, next re-entry will return inmediatly
    if (PCIFR != 0)
        interrupt = 1;
    else
        interrupt = 0;
}
