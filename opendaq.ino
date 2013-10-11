#define SERIAL_DEBUG

#include <avr/wdt.h>
#include "calibration.h"
#include "datachannel.h"
#include "odstream.h"
#include "commdata.h"
#include "debug.h"

extern DStream ODStream;

void setup()
{
    int i;

    daqInit();
    i = Cal.RecallCalibration();
    Comm.begin();

    delay(100);
    SetAnalogVoltage(0);

    _DEBUG("memory < %d\r\n", availableMemory());
    _DEBUG("VERSION %d\r\n", HW_VERSION);

    ledSet(LEDGREEN, 1);
    ledSet(LEDRED, 0);

    wdt_enable(WDTO_2S);
}


void loop()
{

#ifdef SERIAL_DEBUG
    delay(300);
#endif
    ODStream.CheckTriggers();
    Comm.processStream();   
    
    while(Comm.available())
        Comm.parseInput(0);

    wdt_reset();
}
