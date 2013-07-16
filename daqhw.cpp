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
 *  Version:  120522
 *  Author:   JRB
 */

#include "daqhw.h"

////////////////////////////////////////////
//fake functions for time counting verification

void ActivateAnalogInput(int number)
{
    if(number > 0)
        PORTA = 1 << (8 - number);
    else
        PORTA = 0;
}


signed int ReadAnalog()
{
    return -100;
}


// DIGITAL PIOS:    ///////////////////////////////////////////////

// !! for all functions: pio indicates Digital input number - 1 (from 0 to 5)

void SetpioMode(uint8_t pio, uint8_t mode)
{
    uint8_t bit = pioToBitMask(pio);
    uint8_t port = pioToPort(pio);
    volatile uint8_t *reg;

    reg = portModeRegister(port);

    if (mode == 0) *reg &= ~bit;
    else *reg |= bit;
}


int GetpioMode(uint8_t pio)
{
    uint8_t bit = pioToBitMask(pio);
    uint8_t port = pioToPort(pio);
    volatile uint8_t *reg;

    reg = portModeRegister(port);

    return (*reg & bit) != 0;

}


void pioWrite(uint8_t pio, uint8_t val)
{
    uint8_t bit = pioToBitMask(pio);
    uint8_t port = pioToPort(pio);
    volatile uint8_t *out;

    out = portOutputRegister(port);

    if (val == 0) *out &= ~bit;
    else *out |= bit;
}


int pioRead(uint8_t pio)
{
    uint8_t bit = pioToBitMask(pio);
    uint8_t port = pioToPort(pio);

    if (*portInputRegister(port) & bit) return 1;

    return 0;
}


void OutputDigital(int value)
{
    PORTA &= 0x0F;
    PORTD &= 0XD7;
    PORTA |= ((value & 0x01) > 0) ? (1 << 7) : 0;
    PORTA |= ((value & 0x02) > 0) ? (1 << 6) : 0;
    PORTA |= ((value & 0x04) > 0) ? (1 << 5) : 0;
    PORTA |= ((value & 0x08) > 0) ? (1 << 4) : 0;
    PORTD |= ((value & 0x10) > 0) ? (1 << 5) : 0;
    PORTD |= ((value & 0x20) > 0) ? (1 << 3) : 0;

}


void SetDigitalDir(int value)
{
    DDRA &= 0x0F;
    DDRD &= 0XD7;
    DDRA |= ((value & 0x01) > 0) ? (1 << 7) : 0;
    DDRA |= ((value & 0x02) > 0) ? (1 << 6) : 0;
    DDRA |= ((value & 0x04) > 0) ? (1 << 5) : 0;
    DDRA |= ((value & 0x08) > 0) ? (1 << 4) : 0;
    DDRD |= ((value & 0x10) > 0) ? (1 << 5) : 0;
    DDRD |= ((value & 0x20) > 0) ? (1 << 3) : 0;
}


//TODO
int ReadDigital()
{
    int k = 0;
    k |= (PINA & (1 << 7)) > 0;
    k |= ((PINA & (1 << 6)) > 0) << 1;
    k |= ((PINA & (1 << 5)) > 0) << 2;
    k |= ((PINA & (1 << 4)) > 0) << 3;
    k |= ((PIND & (1 << 5)) > 0) << 4;
    k |= ((PIND & (1 << 3)) > 0) << 5;
    return k;
}


//TODO
int GetDigitalDir()
{
    int k = 0;
    k |= (DDRA & (1 << 7)) > 0;
    k |= ((DDRA & (1 << 6)) > 0) << 1;
    k |= ((DDRA & (1 << 5)) > 0) << 2;
    k |= ((DDRA & (1 << 4)) > 0) << 3;
    k |= ((DDRD & (1 << 5)) > 0) << 4;
    k |= ((DDRD & (1 << 3)) > 0) << 5;
    return k;
}


void ledSet(uint8_t nb, uint8_t val)
{
    if( val == 0) {
        PORTC |= (1 << (2 + nb)); //Inverted output logic for the leds (ON=0V)
    } else
        PORTC &= ~(1 << (2 + nb));
}


// ANALOG CHANNELS & SPI: /////////////////////////////////////////////
signed int ReadADC()
{
    unsigned long result;

    PORTB &= ~(0X01 << 2);

    SPDR = 0;
    while(!(SPSR & (1 << SPIF)));
    result = SPDR;

    SPDR = 0;
    while(!(SPSR & (1 << SPIF)));
    result = (result << 8) + SPDR;

    SPDR = 0;
    while(!(SPSR & (1 << SPIF)));
    result = (result << 8) + SPDR;

    PORTB |= 0X01 << 2;

    result >>= 2;

    return (signed int) result & 0xFFFF;

}


signed int ReadNADC(int nsamples)
{
    uint8_t i = 0;
    int32_t sum = 0;
    do {
        sum += ReadADC();
        i++;
    } while(i <= nsamples);

    sum /= nsamples + 1;

    return (signed int) sum;
}


signed int ReadAnalogIn(int n)
{
    signed long r;
    r = ReadNADC(n);
    r *= -153;
    r /= 1000;

    return (signed int) r;
}


void ConfigAnalogGain(uint8_t gain)
{
    PORTC &= ~0X03;
    PORTC &= ~(0X01 << 4);

    if(gain == 0) {
        PORTC |= (0X01 << 4);
    } else {
        PORTC |= ((gain - 1) & 0X03);
    }

}

void ConfigAnalogChannels(uint8_t chp, uint8_t chm)
{
    uint8_t ain_pos, ain_neg;

    ain_pos = ainToPort(chp);
    ain_neg = ainToPort(chm);

    PORTA &= ~0X07;
    PORTC &= ~0XE0;

    PORTA |= (ain_neg & 0X07);
    PORTC |= (ain_pos & 0X07) << 5;
}


void ConfigAnalog(uint8_t chp, uint8_t chm, uint8_t gain)
{
    ConfigAnalogChannels(chp, chm);
    ConfigAnalogGain(gain);
}


void SetDacOutput(int value)
{
    PORTB &= ~(0X01 << 3);
    SPDR = (value & 0xFF00) >> 8;
    while(!(SPSR & (1 << SPIF)));
    SPDR = value & 0xFF;
    while(!(SPSR & (1 << SPIF)));
    PORTB |= 0X01 << 3;
}

/*
int SetAnalogVoltage(signed int mv)
{
    unsigned long aux;

    if(mv<-5000)
        aux = 0;
    else if(mv>5000)
        aux = 10000;
    else
        aux = mv + 5000;

    aux *= 16383;
    aux /= 10000;
    SetDacOutput(aux);

    return (int) aux;
}
*/

int SetAnalogVoltage(signed int mv)
{
    unsigned long aux;

    if(mv < -4096)
        aux = 0;
    else if(mv > 4096)
        aux = 8192;
    else
        aux = mv + 4096;

//  aux *= 2;
    aux *= 16384;
    aux /= 8192;
    SetDacOutput(aux);

    return (int) aux;
}

// TIMER1 based functions ///////////////////////////////////////////////

void pwmInit(int duty, long microseconds)
{
    //SetpioMode(4, OUTPUT);        //SI ponemos el pin del PIO5/OC1A/PD5 como salida, no funciona el PWM??!!
    Timer1.pwm(5, duty, microseconds * 2);
}


void PwmStop()
{
    Timer1.disablePwm(5);
}


void setPwmDuty(int duty)
{
    Timer1.setPwmDuty(5, duty);
}

void captureInit(unsigned long microseconds)
{
    SetpioMode(4, INPUT);
    Timer1.startCapture(microseconds);
}


void captureStop()
{
    Timer1.stopCapture();
}

int  getCapture(int state)
{
    return (int) Timer1.getCapture(state);
}

void counterInit(int edge)
{
    SetpioMode(4, INPUT);
    Timer1.startCounter(edge);
}

int getCounter(int reset)
{
    return (int) Timer1.getCounter(reset);
}


// GENERAL FUNCTIONS //////////////////////////////////////////////////////////

//Hardware init:
void daqInit()
{
    //PORTA: MMA,MMB,MMC as outputs
    DDRA = 0X07;

    //PORTB: SCK,SDO, DAC_CS, ADC_CS, SS as outputs
    DDRB = 0XBC;

    //PORTC: MPA,MPB,MPC,CGND_EN,LEDR,LEDG,GA1,GA0 as outputs (all)
    DDRC = 0XFF;

    //PORTD: no outputs (U1TX alternate function)
    DDRD = 0X00;

    //SPI begin
    SPCR = (1 << SPE) | (1 << MSTR) | (0x04); //fclk/4
    //SPSR |= 1;
    /*
      SPCR = (1<<SPE)|(1<<MSTR)|(0x04)|1;           //fclk/4
      SPSR |= 1;
    */

    //LEDG ON
    PORTC |= 1 << 3;
}


int availableMemory()
{
    int size = 4096; // ATMEGA644P. Use 2048 with ATmega328
    byte *buf;

    while ((buf = (byte *) malloc(--size)) == NULL);

    free(buf);

    return size;
}
