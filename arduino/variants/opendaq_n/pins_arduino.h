/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Modified by Jose R. Blanco <joserbm@ingen10.com>

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
*/

// ATMEL ATMEGA644p TQFP


#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <Arduino.h>
#include <avr/pgmspace.h>

//1: OPENDAQ-M	2: OPENDAQ-S 3: OPENDAQ-N
#define HW_VERSION 3

#define PIO1    0
#define PIO2    1
#define PIO3    2
#define PIO4    3
#define PIO5    4
#define PIO6    5

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

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
    _BV(7),
    _BV(6),
    _BV(5),
    _BV(4),
    _BV(5),
    _BV(3)
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
    PA,
    PA,
    PA,
    PA,
    PD,
    PD
};

const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    TIMER1B,
    NOT_ON_TIMER
};

#endif
#endif
