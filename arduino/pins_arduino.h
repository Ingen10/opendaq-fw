/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2007 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.h 249 2007-02-03 16:52:51Z mellis $
*/

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <Arduino.h>
#include <avr/pgmspace.h>

#define LEDG    13
#define LEDR    14

#define ADC_CS  29
#define DAC_CS  30
#define SS      31
#define MOSI     0
#define MISO     1
#define SCK      2

#define MMA     24
#define MMB     25
#define MMC     26
#define MPA     18
#define MPB     17
#define MPC     16

#define GA1     12
#define GA0     11

#define CGND_EN 15

#define PIO1    19
#define PIO2    20
#define PIO3    21
#define PIO4    22
#define PIO5     8
#define PIO6     6

#define ALL_INPUTS  0
#define ALL_OUTPUTS 0xFF

#define TIMER0A 1
#define TIMER0B 2
#define TIMER1A 3
#define TIMER1B 4
#define TIMER2  5
#define TIMER2A 6
#define TIMER2B 7

// ATMEL ATMEGA644p TQFP / openDAQ

#define PA 1
#define PB 2
#define PC 3
#define PD 4


#ifdef ARDUINO_MAIN

// these arrays map port names (e.g. port B) to the
// appropriate addresses for various functions (e.g. reading
// and writing)
const uint16_t PROGMEM port_to_mode_PGM[] = {
    NOT_A_PORT,
    (uint16_t) &DDRA,
    (uint16_t) &DDRB,
    (uint16_t) &DDRC,
    (uint16_t) &DDRD,
};

const uint16_t PROGMEM port_to_output_PGM[] = {
    NOT_A_PORT,
    (uint16_t) &PORTA,
    (uint16_t) &PORTB,
    (uint16_t) &PORTC,
    (uint16_t) &PORTD
};

const uint16_t PROGMEM port_to_input_PGM[] = {
    NOT_A_PORT,
    (uint16_t) &PINA,
    (uint16_t) &PINB,
    (uint16_t) &PINC,
    (uint16_t) &PIND
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
    PB, /* 0 */
    PB,
    PB,
    PD,
    PD,
    PD,
    PD,
    PD,
    PD, /* 8 */
    PD,
    PD,
    PC,
    PC,
    PC,
    PC,
    PC,
    PC, /* 16 */
    PC,
    PC,
    PA,
    PA,
    PA,
    PA,
    PA,
    PA, /* 24 */
    PA,
    PA,
    PB,
    PB,
    PB,
    PB,
    PB  /* 31 */
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
	_BV(5), /* 0, port B */
	_BV(6),
	_BV(7),
	_BV(0),
	_BV(1),
	_BV(2),
	_BV(3),
	_BV(4),
	_BV(5), /* 8, port D */
	_BV(6),
	_BV(7),
	_BV(0),
	_BV(1),
	_BV(2),
	_BV(3),
	_BV(4),
	_BV(5), /* 16, port C */
	_BV(6),
	_BV(7),
	_BV(7),
	_BV(6),
	_BV(5),
	_BV(4),
	_BV(3),
	_BV(2), /* 24, port A */
	_BV(1),
	_BV(0),
	_BV(0),
	_BV(1),
	_BV(2),
	_BV(3),
	_BV(4)
};

const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
    NOT_ON_TIMER,   /* 0  - PB0 */
    NOT_ON_TIMER,   /* 1  - PB1 */
    NOT_ON_TIMER,   /* 2  - PB2 */
    TIMER0A,        /* 3  - PB3 */
    TIMER0B,        /* 4  - PB4 */
    NOT_ON_TIMER,   /* 5  - PB5 */
    NOT_ON_TIMER,   /* 6  - PB6 */
    NOT_ON_TIMER,   /* 7  - PB7 */
    NOT_ON_TIMER,   /* 8  - PD0 */
    NOT_ON_TIMER,   /* 9  - PD1 */
    NOT_ON_TIMER,   /* 10 - PD2 */
    NOT_ON_TIMER,   /* 11 - PD3 */
    TIMER1B,        /* 12 - PD4 */
    TIMER1A,        /* 13 - PD5 */
    TIMER2B,        /* 14 - PD6 */
    TIMER2A,        /* 15 - PD7 */
    NOT_ON_TIMER,   /* 16 - PC0 */
    NOT_ON_TIMER,   /* 17 - PC1 */
    NOT_ON_TIMER,   /* 18 - PC2 */
    NOT_ON_TIMER,   /* 19 - PC3 */
    NOT_ON_TIMER,   /* 20 - PC4 */
    NOT_ON_TIMER,   /* 21 - PC5 */
    NOT_ON_TIMER,   /* 22 - PC6 */
    NOT_ON_TIMER,   /* 23 - PC7 */
    NOT_ON_TIMER,   /* 24 - PA0 */
    NOT_ON_TIMER,   /* 25 - PA1 */
    NOT_ON_TIMER,   /* 26 - PA2 */
    NOT_ON_TIMER,   /* 27 - PA3 */
    NOT_ON_TIMER,   /* 28 - PA4 */
    NOT_ON_TIMER,   /* 29 - PA5 */
    NOT_ON_TIMER,   /* 30 - PA6 */
    NOT_ON_TIMER    /* 31 - PA7 */
};

//#define portOutputRegister(P) ( (volatile uint8_t *)( pgm_read_byte( port_to_output_PGM + (P))) )
//#define portInputRegister(P) ( (volatile uint8_t *)( pgm_read_byte( port_to_input_PGM + (P))) )
//#define portModeRegister(P) ( (volatile uint8_t *)( pgm_read_byte( port_to_mode_PGM + (P))) )

#endif

#endif
