//#define SERIAL_DEBUG

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
    SetAnalogVoltage(-1000);

    _DEBUG("memory < %d\n", availableMemory());

    ledSet(LEDGREEN, 1);
    ledSet(LEDRED, 0);

    wdt_enable(WDTO_2S);
    
    ///////////////////////////
    /*
    ODStream.Initialize();
    ODStream.CreateStreamChannel(1,20);
    ODStream.ConfigChan(1, ANALOG_INPUT, 8, 0, 1);
    ODStream.ConfigChan(2, ANALOG_INPUT, 7, 2, 2);
    ODStream.ConfigChan(3, ANALOG_INPUT, 6, 0, 3);
    ODStream.ConfigChan(4, ANALOG_INPUT, 5, 4, 0);
    ODStream.Start();
    */
    ////////////////////////////
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
