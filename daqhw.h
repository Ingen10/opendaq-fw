#ifndef DAQHW_H
#define DAQHW_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "pins_opendaq.h"
#include "Timer1.h"

extern "C" {
	#include <stdlib.h>
}


typedef uint8_t byte;


#define LEDG  	13
#define LEDR  	14

#define ADC_CS  29
#define DAC_CS  30
#define SS 			31
#define MOSI 		 0
#define MISO 		 1
#define SCK 		 2

#define MMA 		24
#define MMB 		25
#define MMC 		26
#define MPA 		18
#define MPB 		17
#define MPC 		16

#define GA1 		12
#define GA0 		11

#define CGND_EN	15

#define PIO1		19
#define PIO2		20
#define PIO3		21
#define PIO4		22
#define PIO5		 8
#define PIO6		 6

#define ALL_INPUTS	0
#define ALL_OUTPUTS	0xFF


//Analog input channel definitions (MMA-MMB-MMC-MPA-MPB-MPC bits select)
#define AIN1	0X0
#define AIN2	0X4
#define AIN3	0X2
#define AIN4	0X6
#define AIN5	0X1
#define AIN6	0X5
#define AIN7	0X3
#define AIN8	0X7
#define AGND	0X0
#define AREF	0X4

//Analog gain definitions
#define GAINX0.5	0
#define GAINX001	1
#define GAINX002	2
#define GAINX010	3
#define GAINX100	4

//PIO pin definitions
#define PA 1
#define PB 2
#define PC 3
#define PD 4

#ifndef INPUT
	#define INPUT 0
#endif
#ifndef OUTPUT
	#define OUTPUT 0
#endif
#ifndef LOW
	#define LOW 0
#endif
#ifndef HIGH
	#define HIGH 1
#endif

#define LEDGREEN 	0
#define LEDRED 		1


const uint8_t PROGMEM pio_to_bit_mask_PGM[] =
{
	_BV(7), 
	_BV(6),
	_BV(5),
	_BV(4),
	_BV(5),
	_BV(3)
};

const uint8_t PROGMEM pio_to_port_PGM[] =
{
 	PA, 
	PA,
	PA,
	PA,
	PD,
	PD
};

const uint8_t PROGMEM ain_to_port_PGM[] =
{
	AGND,
	AIN1,
	AIN2,
	AIN3,
	AIN4,
	AIN5,
	AIN6,
	AIN7,
	AIN8,
	AREF
};

#define pioToPort(P) ( pgm_read_byte( pio_to_port_PGM + (P) ) )
#define pioToBitMask(P) ( pgm_read_byte( pio_to_bit_mask_PGM + (P) ) )
#define ainToPort(P) ( pgm_read_byte( ain_to_port_PGM + (P) ) )


void SetpioMode(uint8_t pio, uint8_t mode);
int GetpioMode(uint8_t pio);
void pioWrite(uint8_t pio, uint8_t val);
int  pioRead(uint8_t pio);

int ReadDigital();
void OutputDigital(int value);

void SetDigitalDir(int value);
int GetDigitalDir();

void ledSet(uint8_t nb, uint8_t val);

void ActivateAnalogInput(int number);

signed int ReadAnalog();
signed int ReadADC();
signed int ReadNADC(int nsamples);
signed int ReadAnalogIn(int n);


void ConfigAnalogChannels(uint8_t chp, uint8_t chm);
void ConfigAnalogGain(uint8_t gain);
void ConfigAnalog(uint8_t chp, uint8_t chm, uint8_t gain);

void SetDacOutput(int value);
int  SetAnalogVoltage(signed int mv);

void pwmInit(int duty, long microseconds);
void PwmStop();
void setPwmDuty(int duty);

void captureInit(unsigned long microseconds);
void captureStop();
int  getCapture(int state);

void counterInit(int edge);
int getCounter(int reset);

void daqInit();
int availableMemory();


#endif

