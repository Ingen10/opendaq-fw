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

#include "daqhw.h"

////////////////////////////////////////////
//fake functions for time counting verification
/**
 * @file daqhw.cpp
 * Fake functions for time counting verification
 */
/** \brief
 *  fake functions for time counting verification
 *
 */
void ActivateAnalogInput(int number) {
    if (number > 0)
        PORTA = 1 << (8 - number);
    else
        PORTA = 0;
}

/** \brief
 *  fake functions for time counting verification
 *
 */
signed int ReadAnalog() {
    return -100;
}


// DIGITAL PIOS:    ///////////////////////////////////////////////

// !! for all functions: pio indicates Digital input number - 1 (from 0 to 5)

/** \brief
 *  Set pio mode
 *
 *  \param  pio: digital input number
 *  \param mode: mode to set pio
 */
void SetpioMode(uint8_t pio, uint8_t mode) {
    uint8_t bit = digitalPinToBitMask(pio);
    volatile uint8_t *reg;

    reg = portModeRegister(digitalPinToPort(pio));

    if (mode == 0) *reg &= ~bit;
    else *reg |= bit;
}

/** \brief
 *  Get pio mode
 *
 *  \param
 *  pio: digital input number
 *  \return
 *  pio mode
 */
int GetpioMode(uint8_t pio) {
    uint8_t bit = digitalPinToBitMask(pio);
    volatile uint8_t *reg;

    reg = portModeRegister(digitalPinToPort(pio));

    return (*reg & bit) != 0;
}

/** \brief
 *  Write a value into pio
 *
 *  \param  pio: digital input number
 *  \param val: value to write into pio
 */
void pioWrite(uint8_t pio, uint8_t val) {
    uint8_t bit = digitalPinToBitMask(pio);
    volatile uint8_t *out;

    out = portOutputRegister(digitalPinToPort(pio));

    if (val == 0) *out &= ~bit;
    else *out |= bit;
}

/** \brief
 *  Read a value from pio
 *
 *  \param
 *  pio: digital input number
 *  \return
 *  1 if right, 0 if wrong
 */
int pioRead(uint8_t pio) {
    uint8_t bit = digitalPinToBitMask(pio);

    if (*portInputRegister(digitalPinToPort(pio)) & bit)
        return 1;
    return 0;
}

/** \brief
 *  Set the digital output
 *
 *  \param
 *  value: value to set the digital output
 */
void OutputDigital(int value) {
    PORTA &= 0x0F;
    PORTD &= 0XD7;
    PORTA |= ((value & 0x01) > 0) ? (1 << 7) : 0;
    PORTA |= ((value & 0x02) > 0) ? (1 << 6) : 0;
    PORTA |= ((value & 0x04) > 0) ? (1 << 5) : 0;
    PORTA |= ((value & 0x08) > 0) ? (1 << 4) : 0;
    PORTD |= ((value & 0x10) > 0) ? (1 << 5) : 0;
    PORTD |= ((value & 0x20) > 0) ? (1 << 3) : 0;

}

/** \brief
 *  Set digital dir
 *
 *  \param
 *  value to set the digital dir
 */
void SetDigitalDir(int value) {
    DDRA &= 0x0F;
    DDRD &= 0XD7;
    DDRA |= ((value & 0x01) > 0) ? (1 << 7) : 0;
    DDRA |= ((value & 0x02) > 0) ? (1 << 6) : 0;
    DDRA |= ((value & 0x04) > 0) ? (1 << 5) : 0;
    DDRA |= ((value & 0x08) > 0) ? (1 << 4) : 0;
    DDRD |= ((value & 0x10) > 0) ? (1 << 5) : 0;
    DDRD |= ((value & 0x20) > 0) ? (1 << 3) : 0;
}

/** \brief
 *  Read the digital input
 *
 *  \return
 *  Digital input value
 */
int ReadDigital() {
    int k = 0;
    k |= (PINA & (1 << 7)) > 0;
    k |= ((PINA & (1 << 6)) > 0) << 1;
    k |= ((PINA & (1 << 5)) > 0) << 2;
    k |= ((PINA & (1 << 4)) > 0) << 3;
    k |= ((PIND & (1 << 5)) > 0) << 4;
    k |= ((PIND & (1 << 3)) > 0) << 5;
    return k;
}

/** \brief
 *  Get the digital dir
 *
 *  \return
 *  Digital dir value
 */
int GetDigitalDir() {
    int k = 0;
    k |= (DDRA & (1 << 7)) > 0;
    k |= ((DDRA & (1 << 6)) > 0) << 1;
    k |= ((DDRA & (1 << 5)) > 0) << 2;
    k |= ((DDRA & (1 << 4)) > 0) << 3;
    k |= ((DDRD & (1 << 5)) > 0) << 4;
    k |= ((DDRD & (1 << 3)) > 0) << 5;
    return k;
}

/** \brief
 *  Set the led value
 *
 *  \param  nb: led value
 *  \param val: value to set
 */
void ledSet(uint8_t nb, uint8_t val) {
    if (val == 0)
        PORTC |= (1 << (2 + nb)); //Inverted output logic for the leds (ON=0V)
    else
        PORTC &= ~(1 << (2 + nb));
}


// ANALOG CHANNELS & SPI: /////////////////////////////////////////////

/** \brief
 *  Read ADC
 *
 *  \return
 *  ADC value
 */
signed int ReadADC() {
#if HW_VERSION==2 //[S]
    unsigned long result;
    signed int a;

    PORTB &= ~(0X01 << 2);

    SPDR = 0x80 | myGains | myChannels;
    while (!(SPSR & (1 << SPIF)));

    SPDR = 0x61;
    while (!(SPSR & (1 << SPIF)));

    SPDR = 0;
    while (!(SPSR & (1 << SPIF)));
    result = SPDR;

    SPDR = 0;
    while (!(SPSR & (1 << SPIF)));
    result = (result << 8) + SPDR;

    PORTB |= 0X01 << 2;

    a = (signed int) result & 0xFFFC;

    //    a /= 4;
    a -= 16384;
    a *= 2;

    return a;

#else

    unsigned long result;

    PORTB &= ~(0X01 << 2);

    SPDR = 0;
    while (!(SPSR & (1 << SPIF)));
    result = SPDR;

    SPDR = 0;
    while (!(SPSR & (1 << SPIF)));
    result = (result << 8) + SPDR;

    SPDR = 0;
    while (!(SPSR & (1 << SPIF)));
    result = (result << 8) + SPDR;

    PORTB |= 0X01 << 2;

    result >>= 2;

    return (signed int) -(result & 0xFFFF);
#endif
}

/** \brief
 *  Read ADC nsamples times, and returns the average
 *
 *  \param
 *  nsamples: number of times the ADC will be read
 *  \return
 *  ADC value average
 */
signed int ReadNADC(int nsamples) {
    uint8_t i = 0;
    int32_t sum = 0;
    do {
        sum += ReadADC();
        i++;
    } while (i <= nsamples);

    sum /= nsamples + 1;

    return (signed int) sum;
}

/** \brief
 *  Read ADC n times, and returns the average calibrated
 *
 *  \param
 *  n: number of times the ADC will be read
 *  \return
 *  ADC value average calibrated
 */
signed int ReadAnalogIn(int n) {
    signed long r;

    r = ReadNADC(n);

#if HW_VERSION==2
    r *= 2500;
    r /= 8192;
#else
    r *= -153;
    r /= 1000;
#endif
    return (signed int) r;
}

/** \brief
 *  Set analog gain
 *
 *  \param
 *  gain: value to set the gain
 */
void ConfigAnalogGain(uint8_t gain) {
#if HW_VERSION==2
    myGains = gainToAdcset(gain);
#else
    PORTC &= ~0X03;
    PORTC &= ~(0X01 << 4);

    if (gain == 0) {
        PORTC |= (0X01 << 4);
    } else {
        PORTC |= ((gain - 1) & 0X03);
    }
#endif
}

/** \brief
 *  Setup the analog channels to be readed
 *
 *  \param  chp: positive channel
 *  \param chm: second channel to take a differential reading
 */
void ConfigAnalogChannels(uint8_t chp, uint8_t chm) {
#if HW_VERSION==2
    //If chm channel is different from ground (0), a differential reading will be taken
    //The firmware automatically selects the complementary channel to chp
    uint8_t c;

    c = chp;
    if (c < 1)
        c = 1;
    if (c > 8)
        c = 8;

    if (chm == 0) {
        myChannels = ainToSEChan(chp - 1);
    } else {
        myChannels = ainToDEChan(chp - 1);
    }
#else
    uint8_t ain_pos, ain_neg;

    ain_pos = ainToPort(chp);
    ain_neg = ainToPort(chm);

    PORTA &= ~0X07;
    PORTC &= ~0XE0;

    PORTA |= (ain_neg & 0X07);
    PORTC |= (ain_pos & 0X07) << 5;
#endif
}

/** \brief
 *  Setup the analog gain and channels to be readed
 *
 *  \param  chp: positive channel
 *  \param chm: second channel to take a differential reading
 *  \param gain: gain value
 */
void ConfigAnalog(uint8_t chp, uint8_t chm, uint8_t gain) {
#if HW_VERSION==2
    Config7871();
#endif
    ConfigAnalogChannels(chp, chm);
    ConfigAnalogGain(gain);
}


////////////////////////////////////////////////

#if HW_VERSION==2

/** \brief
 *  Setup internal parameters of hw version 2
 *
 */
void Config7871() {
    PORTB &= ~(0X01 << 2);

    SPDR = 0x07; //REF/OSC REGISTER
    while (!(SPSR & (1 << SPIF)));

    SPDR = 0x3C; //INTERNAL OSC, CCLK OUTPUT ON, REF ON, BUF ON, 2.5V REF
    while (!(SPSR & (1 << SPIF)));

    PORTB |= 0X01 << 2;
}

/** \brief
 *  Setup PIO control register
 *
 *  \param
 *  x: value to set PIO
 */
void config7871PIO(int x) {
    int result;

    PORTB &= ~(0X01 << 2);

    SPDR = 0x06; //PIO control register
    while (!(SPSR & (1 << SPIF)));

    SPDR = 0x0F & x; //All outputs
    while (!(SPSR & (1 << SPIF)));

    PORTB |= 0X01 << 2;

}

/** \brief
 *  Setup PIO state register
 *
 *  \param
 *  x: value to set PIO
 */
void set7871PIO(int x) {
    int result;

    PORTB &= ~(0X01 << 2);

    SPDR = 0x05; //PIO state register
    while (!(SPSR & (1 << SPIF)));

    SPDR = 0x0F & x; //Set outputs
    while (!(SPSR & (1 << SPIF)));

    PORTB |= 0X01 << 2;
}
#endif



// DAC functions ///////////////////////////////////////////////

#if HW_VERSION==2

/** \brief
 *  Setup LTC2630
 *
 */
void setupLTC2630() {
    PORTB &= ~(0X01 << 3);

    SPDR = 0x60; //Select Internal Reference (Power-on Reset Default)
    while (!(SPSR & (1 << SPIF)));

    SPDR = 0;
    while (!(SPSR & (1 << SPIF)));

    SPDR = 0;
    while (!(SPSR & (1 << SPIF)));

    PORTB |= 0X01 << 3;
}
#endif

/** \brief
 *  Set DAC output
 *
 *  \param
 *  value: value to set DAC output
 *  \return
 *  0
 */
int SetDacOutput(int16_t value) {
    int32_t rawcode;
    
    PORTB &= ~(0X01 << 3);
#if HW_VERSION==2   //[S] -> 0..65536 0V..4.096V
    rawcode = (value>0)? value*2:0;
    SPDR = 0x30; //Write to and Update (Power up) DAC Register
    while (!(SPSR & (1 << SPIF)));
    SPDR = (rawcode & 0xFF00) >> 8;
    while (!(SPSR & (1 << SPIF)));
    SPDR = rawcode & 0xF0;
    while (!(SPSR & (1 << SPIF)));
#else       //[M] -> 0..16384 -4.096V..+4.096V 
    rawcode = value/4 + 8192;
    SPDR = (rawcode & 0xFF00) >> 8;
    while (!(SPSR & (1 << SPIF)));
    SPDR = rawcode & 0xFF;
    while (!(SPSR & (1 << SPIF)));
#endif
    PORTB |= 0X01 << 3;
    return 0;
}

/** \brief
 *  Set analog voltage
 *
 *  \param
 *  mv: voltage to setup
 *  \return
 *  raw voltage value
 */
int SetAnalogVoltage(signed int mv) {
#if HW_VERSION==2       //[S]
    unsigned int aux;

    if (mv < 0)
        aux = 0;
    else if (mv > 4096)
        aux = 32768;
    else
        aux = 8 * mv;

    SetDacOutput(aux);

    return (int) aux;
#else
    unsigned long aux;

    if (mv < -4096)
        mv = -4096;
    else if (mv > 4096)
        mv = 4096;

    SetDacOutput(8*aux);

    return (int) 8*aux;
#endif
}

// TIMER1 based functions ///////////////////////////////////////////////

/** \brief
 *  Initialize pwm
 *
 *  \param
 *  microseconds: value in microseconds to initialize the timer
 */
void pwmInit(int duty, long microseconds) {
    //SetpioMode(4, OUTPUT);        //SI ponemos el pin del PIO5/OC1A/PD5 como salida, no funciona el PWM??!!
    Timer1.pwm(5, duty, microseconds * 2);
}

/** \brief
 *  Stop pwm
 *
 */
void PwmStop() {
    Timer1.disablePwm(5);
}

/** \brief
 *  Setup the duty of pwm
 *
 *  \param
 *  duty: duty to set pwm
 */
void setPwmDuty(int duty) {
    Timer1.setPwmDuty(5, duty);
}

/** \brief
 *  Initialize capture
 *
 *  \param
 *  microseconds: value in microseconds to initialize the capture
 */
void captureInit(unsigned long microseconds) {
    SetpioMode(4, INPUT);
    Timer1.startCapture(microseconds);
}

/** \brief
 *  Stop capture
 *
 */
void captureStop() {
    Timer1.stopCapture();
}

/** \brief
 *  Get a capture value
 *
 *  \param
 *  state: high (1) or low (0)
 */
int getCapture(int state) {
    return (int) Timer1.getCapture(state);
}

/** \brief
 *  Initialize the counter
 *
 *  \param
 *  edge: edge to start counter
 */
void counterInit(int edge) {
    SetpioMode(4, INPUT);
    Timer1.startCounter(edge);
}

/** \brief
 *  Get counter value
 *
 *  \param
 *  reset: flag to reset the counter
 */
int getCounter(int reset) {
    return (int) Timer1.getCounter(reset);
}


// GENERAL FUNCTIONS //////////////////////////////////////////////////////////

//Hardware init:

/** \brief
 *  Initialize DAC
 *
 */
void daqInit() {
    //PORTA: MMA,MMB,MMC as outputs
    DDRA = 0X07;

    //PORTB: SCK,SDO, DAC_CS, ADC_CS, SS as outputs
    DDRB = 0XBC;

    //PORTC: MPA,MPB,MPC,CGND_EN,LEDR,LEDG,GA1,GA0 as outputs (all)
    DDRC = 0XFF;

    //PORTD: no outputs (U1TX alternate function)
    DDRD = 0X00;

    //LEDG ON
    PORTC |= 1 << 3;

#if HW_VERSION==2
    SPCR = (1 << SPE) | (1 << MSTR) | (0x0C) | 1; //fclk/16, CLK HIGH inactive, trailing edge
    setupLTC2630();
    Config7871();
#else
    SPCR = (1 << SPE) | (1 << MSTR) | (0x04); //fclk/4
#endif


}

/** \brief
 *  Return the avaiable memory
 *
 *  \return
 *  size of the avaiable memory
 */
int availableMemory() {
    int size = 4096; // ATMEGA644P
    byte *buf;

    while ((buf = (byte *) malloc(--size)) == NULL);

    free(buf);

    return size;
}
