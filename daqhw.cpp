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
    uint8_t bit = digitalPinToBitMask(pio);
    volatile uint8_t *reg;

    reg = portModeRegister(digitalPinToPort(pio));

    if (mode == 0) *reg &= ~bit;
    else *reg |= bit;
}


int GetpioMode(uint8_t pio)
{
    uint8_t bit = digitalPinToBitMask(pio);
    volatile uint8_t *reg;

    reg = portModeRegister(digitalPinToPort(pio));

    return (*reg & bit) != 0;
}


void pioWrite(uint8_t pio, uint8_t val)
{
    uint8_t bit = digitalPinToBitMask(pio);
    volatile uint8_t *out;

    out = portOutputRegister(digitalPinToPort(pio));

    if (val == 0) *out &= ~bit;
    else *out |= bit;
}


int pioRead(uint8_t pio)
{
    uint8_t bit = digitalPinToBitMask(pio);

    if (*portInputRegister(digitalPinToPort(pio)) & bit)
        return 1;
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
    if( val == 0)
        PORTC |= (1 << (2 + nb)); //Inverted output logic for the leds (ON=0V)
    else
        PORTC &= ~(1 << (2 + nb));
}


// ANALOG CHANNELS & SPI: /////////////////////////////////////////////
signed int ReadADC()
{
#if HW_VERSION==2
    unsigned long result;
    signed int a;

    PORTB &= ~(0X01<<2); 

    SPDR = 0x80|myGains|myChannels;
    while(!(SPSR & (1<<SPIF))); 

    SPDR = 0x61;
    while(!(SPSR & (1<<SPIF))); 

    SPDR = 0;
    while(!(SPSR & (1<<SPIF))); 
    result = SPDR;

    SPDR = 0;
    while(!(SPSR & (1<<SPIF))); 
    result = (result<<8)+SPDR;

    PORTB |= 0X01<<2; 

    a = (signed int) result& 0xFFFC;
    a /= 4;

    return a;
    
#else

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
#endif
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
    
#if HW_VERSION==2
	r*=2500;
	r/=8192;
#else
    r *= -153;
    r /= 1000;
#endif
    return (signed int) r;
}


void ConfigAnalogGain(uint8_t gain)
{
#if HW_VERSION==2
	myGains = gainToAdcset(gain);
#else
    PORTC &= ~0X03;
    PORTC &= ~(0X01 << 4);

    if(gain == 0) {
        PORTC |= (0X01 << 4);
    } else {
        PORTC |= ((gain - 1) & 0X03);
    }
#endif
}

void ConfigAnalogChannels(uint8_t chp, uint8_t chm)
{
#if HW_VERSION==2
    //If chm channel is different from ground (0), a differential reading will be taken
    //The firmware automatically selects the complementary channel to chp
    uint8_t c;

    c = chp;
    if(c<1)
        c=1;
    if(c>8)
        c=8;

    if(chm == 0){
        myChannels = ainToSEChan(chp-1);
    }
    else{
        myChannels = ainToDEChan(chp-1);
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


void ConfigAnalog(uint8_t chp, uint8_t chm, uint8_t gain)
{
#if HW_VERSION==2
    Config7871();
#endif
    ConfigAnalogChannels(chp, chm);
    ConfigAnalogGain(gain);
}


////////////////////////////////////////////////

#if HW_VERSION==2

void Config7871()
{
	PORTB &= ~(0X01<<2); 
  
	SPDR = 0x07;			//REF/OSC REGISTER
  while(!(SPSR & (1<<SPIF))); 
	
  SPDR = 0x3C;			//INTERNAL OSC, CCLK OUTPUT ON, REF ON, BUF ON, 2.5V REF
  while(!(SPSR & (1<<SPIF))); 
	
  PORTB |= 0X01<<2; 
}

void config7871PIO(int x)
{
	int result;
  
	PORTB &= ~(0X01<<2); 
  
	SPDR = 0x06;			//PIO control register
  while(!(SPSR & (1<<SPIF))); 
	
	SPDR = 0x0F&x;			//All outputs
  while(!(SPSR & (1<<SPIF))); 
	
  PORTB |= 0X01<<2; 
			
}


void set7871PIO(int x)
{
	int result;
  
	PORTB &= ~(0X01<<2); 
  
	SPDR = 0x05;			//PIO state register
  while(!(SPSR & (1<<SPIF))); 
	
	SPDR = 0x0F&x;		//Set outputs
  while(!(SPSR & (1<<SPIF))); 
	
  PORTB |= 0X01<<2; 	
}
#endif



// DAC functions ///////////////////////////////////////////////

#if HW_VERSION==2
    void setupLTC2630()
    {
        PORTB &= ~(0X01<<3); 
      
        SPDR = 0x60;			//Select Internal Reference (Power-on Reset Default)
      while(!(SPSR & (1<<SPIF))); 	

        SPDR = 0;
      while(!(SPSR & (1<<SPIF))); 

        SPDR = 0;
      while(!(SPSR & (1<<SPIF))); 
        
      PORTB |= 0X01<<3; 	
    }
#endif

int SetDacOutput(int value)
{
    PORTB &= ~(0X01<<3); 
#if HW_VERSION==2
    SPDR = 0x30;			//Write to and Update (Power up) DAC Register
    while(!(SPSR & (1<<SPIF))); 
    SPDR = (value & 0xFF00) >> 8;
    while(!(SPSR & (1<<SPIF))); 
    SPDR = value & 0xF0;
    while(!(SPSR & (1<<SPIF))); 
#else
    SPDR = (value & 0xFF00) >> 8;
    while(!(SPSR & (1 << SPIF)));
    SPDR = value & 0xFF;
    while(!(SPSR & (1 << SPIF)));
#endif
    PORTB |= 0X01 << 3; 
    return 0;
}


int SetAnalogVoltage(signed int mv)
{
#if HW_VERSION==2
	unsigned int aux;
	
	if(mv<0)
		aux = 0;
	else if(mv>4096)
		aux = 65536;
	else
		aux = 16*mv;

	SetDacOutput(aux);
	
	return (int)aux;
#else
    unsigned long aux;

    if (mv < -4096)
        mv = -4096;
    else if (mv > 4096)
        mv = 4096;
    
    aux = 2*(mv + 4096);
    SetDacOutput(aux);

    return (int)aux;
#endif
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

    //LEDG ON
    PORTC |= 1 << 3;
    
#if HW_VERSION==2
        SPCR = (1<<SPE)|(1<<MSTR)|(0x0C)|1;             //fclk/16, CLK HIGH inactive, trailing edge
        setupLTC2630();
        Config7871();
#else
        SPCR = (1<<SPE)|(1<<MSTR)|(0x04);               //fclk/4
#endif
    
    
}


int availableMemory()
{
    int size = 4096; // ATMEGA644P
    byte *buf;

    while ((buf = (byte *) malloc(--size)) == NULL);

    free(buf);

    return size;
}
