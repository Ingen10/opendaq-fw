
#include <avr/wdt.h>
#include "commdata.h"
#include "calibration.h"
#include "datachannel.h"
#include "odstream.h"
#include "debug.h"

extern DStream ODStream;


void setup()
{
    int i;

    daqInit();
    i = Cal.RecallCalibration();
    Comm.begin();

#ifdef SERIAL_DEBUG
    _DEBUG("Calibration: %X\n", i);
    for(i = 0; i < 6; i++)
        _DEBUG("%X: m=%d b=%d\n", i, Cal.gain_m[i], Cal.gain_b[i]);
#endif

    delay(100);
    SetAnalogVoltage(0);
    delay(100);
    SetAnalogVoltage(900);

    _DEBUG("memory < %d\n", availableMemory());

    ledSet(LEDGREEN, 1);
    ledSet(LEDRED, 0);

    wdt_enable(WDTO_250MS);
}


void loop()
{
#ifdef SERIAL_DEBUG
    delay(300);
#endif
    ODStream.CheckTriggers();
    PORTA = PORTA | 0x20;
    Comm.processStream();
    PORTA = PORTA & ~0x20;

    while(Comm.available())
        Comm.parseInput(0);

    wdt_reset();
}
