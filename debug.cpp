#include <avr/pgmspace.h>
#include <Arduino.h>

// program memory version of printf - copy of format string and result share a
// buffer to reduce memory use
void StreamPrint_progmem(Print &out,PGM_P format,...)
{
    char formatString[128], *ptr;
 
    // copy in from program mem
    strncpy_P(formatString, format, sizeof(formatString));

    // null terminate
    formatString[sizeof(formatString)-2] = '\0'; 
    ptr = &formatString[strlen(formatString) + 1]; // our result buffer...

    va_list args;
    va_start (args,format);
    vsnprintf(ptr, sizeof(formatString) - 1 - strlen(formatString), formatString, args);
    va_end (args);

    formatString[ sizeof(formatString)-1 ]='\0'; 
    out.print(ptr);
}
