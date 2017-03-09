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
 *  Version:    161207
 *  Author:     JRB
 *  Revised by: 
 */

/**
 * @file daqhw.c
 * Low level hardware control functions
*/


//#define SERIAL_DEBUG
#include "daqhw.h"
#include "debug.h"


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


/** **************************************************************************************
 * \section DIOs Digital IO pins control funtions
 *
 * \note For all functions: pio indicates Digital input number - 1 (from 0 to 5)
 */

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


/** **************************************************************************************
 * \section AIN Analog input channels and SPI communications with the ADC
 *
 */


/** \brief
 *  Read ADC
 *
 *  \return
 *  Raw ADC value 16 bits (-32768..+32767 on input range)
 */
signed int ReadADC() {

#if HW_VERSION==1 //[M] 
    unsigned long result;

    PORTB &= ~ADC_CS;

    SPDR = 0;
    while (!(SPSR & (1 << SPIF)));
    result = SPDR;

    SPDR = 0;
    while (!(SPSR & (1 << SPIF)));
    result = (result << 8) + SPDR;

    SPDR = 0;
    while (!(SPSR & (1 << SPIF)));
    result = (result << 8) + SPDR;

    PORTB |= ADC_CS;
    
    result >>= 2;

    return (signed int) -(result & 0xFFFF);
    
   
#elif HW_VERSION==2 //[S]
    unsigned long result = 0;
    signed int a;

    PORTB &= ~ADC_CS;

    //SPDR = 0x04;        //Gain-Mux register
    //while (!(SPSR & (1 << SPIF)));

    SPDR = 0x80 | myGains | myChannels;
    while (!(SPSR & (1 << SPIF)));

    SPDR = 0x61;    //??
    while (!(SPSR & (1 << SPIF)));

    SPDR = 0;
    while (!(SPSR & (1 << SPIF)));
    result = SPDR;

    SPDR = 0;
    while (!(SPSR & (1 << SPIF)));
    result = (result << 8) + SPDR;

    PORTB |= ADC_CS;
    
    a = (signed int) result & 0xFFFC;

    if(de_mode == 0)
        a -= 0x4000; //16384  - Substract 0V code

    a *= 2;

    return a;

#elif HW_VERSION==3 //[N] 
    unsigned long result;

    PORTB &= ~ADC_CS;

    SPDR = 0;
    while (!(SPSR & (1 << SPIF)));
    result = SPDR;

    SPDR = 0;
    while (!(SPSR & (1 << SPIF)));
    result = (result << 8) + SPDR;

    SPDR = 0;
    while (!(SPSR & (1 << SPIF)));
    result = (result << 8) + SPDR;

    PORTB |= ADC_CS;
    
    result >>= 2;
    
    if(invert)
        return (signed int) -(result & 0xFFFF);
    else
        return (signed int) (result & 0xFFFF);   
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
 *  Read ADC n times, and returns the teorical voltage value in mV (not calibrated)
 *
 *  \param
 *  n: number of times the ADC will be read
 *  \return
 *  ADC value average in mV
 */
signed int ReadAnalogIn(int n) {
    signed long r;

    r = ReadNADC(n);

    r *= 12000;
    r /= 32768;

    return (signed int) r;
}

/** \brief
 *  Setup the analog gain and channels to be readed
 *
 *  \param  chp: positive channel
 *  \param chm: second channel to take a differential reading
 *  \param gain: gain value
 */
void ConfigAnalog(uint8_t chp, uint8_t chm, uint8_t gain) {
#if HW_VERSION==1   //[M]
    uint8_t ain_pos, ain_neg;

    ain_pos = ainToPort(chp);
    ain_neg = ainToPort(chm);

    PORTA &= ~0X07;
    PORTC &= ~0XE0;

    PORTA |= (ain_neg & 0X07);
    PORTC |= (ain_pos & 0X07) << 5;

    PORTC &= ~0X03;
    PORTC &= ~(0X01 << 4);

    if (gain == 0) {
        PORTC |= (0X01 << 4);
    } else {
        PORTC |= ((gain - 1) & 0X03);
    }

#elif HW_VERSION==2       //[S]
    Config7871();
    //If chm channel is different from ground (0), a differential reading will be taken
    //The firmware automatically selects the complementary channel to chp
    if (chp < 1)
        chp = 1;
    if (chp > 8)
        chp = 8;

    if (chm == 0) {
        de_mode = 0;
        myChannels = ainToSEChan(chp - 1);
    } else {
        de_mode = 1;
        myChannels = ainToDEChan(chp - 1);
    }
    myGains = gainToAdcset(gain);

#elif HW_VERSION==3       //[N]
    int chm_gain =0;
    if (chm >0)
        chm_gain = gain;
    else
        chm_gain = 0;
    //set special SPI settings for MCP6S26: CPOl=1 CPHA=1
    SPCR = (1 << SPE) | (1 << MSTR) | (0x0C) | 1; //fclk/16, CLK HIGH inactive, trailing edge
    
    if(chp>4){
        invert = 1;
        SetupMcp6s26_Channel(PGA2_CS, ainToMcpChan(chp)); 
        SetupMcp6s26_Gain(PGA2_CS, gain);

        SetupMcp6s26_Channel(PGA1_CS, ainToMcpChan(chm)); 
        SetupMcp6s26_Gain(PGA1_CS, chm_gain);
    }
    else{
        invert = 0;
        SetupMcp6s26_Channel(PGA1_CS, ainToMcpChan(chp)); 
        SetupMcp6s26_Gain(PGA1_CS, gain);
        
        SetupMcp6s26_Channel(PGA2_CS, ainToMcpChan(chm)); 
        SetupMcp6s26_Gain(PGA2_CS, chm_gain);
    }
    //Restore SPI settings to default: CPOl=0 CPHA=1
    SPCR = (1 << SPE) | (1 << MSTR) | 0x04; //fclk/4, CLK low inactive, trailing edge

#endif

}

/** **************************************************************************************
 * \section MCP6S26 control functions for configuration
 *
 */

#if HW_VERSION==3   //[N]

/** \brief
 *  Setup MCP6S26 gain control register
 *  CS lines are not controlled inside this function
 *
 *  \param
 *  chip: bit adress of the MCP6S26 chip on Port A
 *  gain: gain selection [+1, +2, +4, +5, +8, +10, +16 or +32 V/V]
 */
void SetupMcp6s26_Gain(uint8_t chip, uint8_t gain) {
    PORTA &= ~chip;
    SPDR = 0x40; //Write on gain register
    while (!(SPSR & (1 << SPIF)));
    SPDR = 0x07 & gain; //setup gain
    while (!(SPSR & (1 << SPIF)));
    PORTA |= chip;
}

/** \brief
 *  Setup MCP6S26 channel control register
 *  CS lines are not controlled inside this function
 *
 *  \param
 *  chip: bit adress of the MCP6S26 chip on Port A
 *  ch: analog input channel [0 .. 5]
 */
void SetupMcp6s26_Channel(uint8_t chip, uint8_t ch) {
    PORTA &= ~chip;
    SPDR = 0x41; //Write on channel register
    while (!(SPSR & (1 << SPIF)));
    SPDR = 0x07 & ch; //setup channel
    while (!(SPSR & (1 << SPIF)));
    PORTA |= chip;
}

#endif

/** **************************************************************************************
 * \subsection ADS7871 ADS7871 Config functions
 *
 */

#if HW_VERSION==2

/** \brief
 *  Setup internal parameters of hw version 2
 *
 */
void Config7871() {
    PORTB &= ~ADC_CS;

    SPDR = 0x07; //REF/OSC REGISTER
    while (!(SPSR & (1 << SPIF)));

    SPDR = 0x3C; //INTERNAL OSC, CCLK OUTPUT ON, REF ON, BUF ON, 2.5V REF
    while (!(SPSR & (1 << SPIF)));

    PORTB |= ADC_CS;
}

/** \brief
 *  Setup PIO control register
 *
 *  \param
 *  x: value to set PIO
 */
void config7871PIO(int x) {
    int result;

    PORTB &= ~ADC_CS;

    SPDR = 0x06; //PIO control register
    while (!(SPSR & (1 << SPIF)));

    SPDR = 0x0F & x; //All outputs
    while (!(SPSR & (1 << SPIF)));

    PORTB |= ADC_CS;

}

/** \brief
 *  Setup PIO state register
 *
 *  \param
 *  x: value to set PIO
 */
void set7871PIO(int x) {
    int result;

    PORTB &= ~ADC_CS;
    SPDR = 0x05; //PIO state register
    while (!(SPSR & (1 << SPIF)));
    SPDR = 0x0F & x; //Set outputs
    PORTB |= ADC_CS;
}
#endif



/** **************************************************************************************
 * \section DAC DAC control & config functions
 *
 */

#if HW_VERSION==2

/** \brief
 *  Setup LTC2630
 *
 */
void setupLTC2630() {
    PORTB &= ~DAC_CS;

    SPDR = 0x60; //Select Internal Reference (Power-on Reset Default)
    while (!(SPSR & (1 << SPIF)));
    SPDR = 0;
    while (!(SPSR & (1 << SPIF)));
    SPDR = 0;
    while (!(SPSR & (1 << SPIF)));

    PORTB |= DAC_CS;
}
#endif

/** \brief
 *  Set DAC output
 *
 *  \param
 *  value: value to set DAC output (-32767..32768)
 *  \return
 *  0
 */
int SetDacOutput(int16_t value) {
    int32_t rawcode;
    
    PORTB &= ~DAC_CS;
#if HW_VERSION==2   //[S] -> 0..65536 0V..4.096V 12bits
    rawcode = (value>0)? value*2:0;
    SPDR = 0x30; //Write to and Update (Power up) DAC Register
    while (!(SPSR & (1 << SPIF)));
    SPDR = (rawcode & 0xFF00) >> 8;
    while (!(SPSR & (1 << SPIF)));
    SPDR = rawcode & 0xF0;
    while (!(SPSR & (1 << SPIF)));
#else       //[M]/[N] -> 0..16384 -4.096V..+4.096V 14bits
    rawcode = value/4 + 8192;   
    SPDR = (rawcode & 0xFF00) >> 8;
    while (!(SPSR & (1 << SPIF)));
    SPDR = rawcode & 0xFF;
    while (!(SPSR & (1 << SPIF)));
#endif
    PORTB |= DAC_CS;
    return 0;
}

/** \brief
 *  Set analog voltage
 *
 *  \param
 *  mv: voltage to setup in millivolts (-4096..4096mV [M]&[N], 0..4096mV [S])
 *  \return
 *  raw binary code applied to the DAC function
 */
int SetAnalogVoltage(signed int mv) {
    int16_t aux;

    if (mv < -4096)
        mv = -4096;
    else if (mv > 4096)
        mv = 4096;

    aux = 8*mv;
    SetDacOutput(aux);
    return (int) aux;
}

/** **************************************************************************************
 * \section TIMER1 TIMER1 control & config functions
 *
 */

/** \brief
 *  Initialize pwm
 *
 *  \param
 *  microseconds: value in microseconds to initialize the timer
 */
void pwmInit(int duty, long microseconds) {
    //SetpioMode(4, OUTPUT);        
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
uint32_t getCapture(int state) {
    return (uint32_t) Timer1.getCapture(state);
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
uint32_t getCounter(int reset) {
    return (uint32_t) Timer1.getCounter(reset);
}




/** **************************************************************************************
 * \section HWGeneral General hardware configuration and initialitation
 *
 */


/** \brief
 *  Initialize DAC
 *
 */
void daqInit() {
    //PORTA: MMA,MMB,MMC as outputs
    DDRA = 0X0F;

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
    SPCR = (1 << SPE) | (1 << MSTR) | (0x04); //fclk/4, CLK low, trailing edge
    PORTA |= 0X03;    
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
