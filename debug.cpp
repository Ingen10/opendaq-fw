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
 *  Version:    150717
 *  Author:     JRB
 *  Revised by: AV (17/07/15)
 */

#include <avr/pgmspace.h>
#include <Arduino.h>

// program memory version of printf - copy of format string and result share a
// buffer to reduce memory use


/**
 * @file debug.cpp
 * Program memory version of printf - copy of format string and result share a 
 * buffer to reduce memory use
 */


/** \brief
 *  program memory version of printf - copy of format string and result share a
 *  buffer to reduce memory use
 *
 *  \param  out: outflow
 *  \param format: format of the output string
 */
void StreamPrint_progmem(Print &out, PGM_P format, ...) {
    char formatString[128], *ptr;

    // copy in from program mem
    strncpy_P(formatString, format, sizeof (formatString));

    // null terminate
    formatString[sizeof (formatString) - 2] = '\0';
    ptr = &formatString[strlen(formatString) + 1]; // our result buffer...

    va_list args;
    va_start(args, format);
    vsnprintf(ptr, sizeof (formatString) - 1 - strlen(formatString), formatString, args);
    va_end(args);

    formatString[ sizeof (formatString) - 1 ] = '\0';
    out.print(ptr);
}
