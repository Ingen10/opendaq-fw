
#include <avr/wdt.h>
#include "commdata.h"
#include "calibration.h"
#include "datachannel.h"
#include "odstream.h"

extern DStream ODStream;


void setup()
{
    int i;

    daqInit();
    i = Cal.RecallCalibration();
    Comm.begin();

#ifdef SERIAL_DEBUG
    Serial.print("*Calibration: ");
    Serial.println(i, HEX);

    for(i = 0; i < 6; i++) {
        Serial.print(i, HEX);
        Serial.print(":  m= ");
        Serial.print(Cal.gain_m[i], DEC);
        Serial.print("  b= ");
        Serial.println(Cal.gain_b[i], DEC);
    }
#endif

    delay(100);
    SetAnalogVoltage(0);
    delay(100);
    SetAnalogVoltage(900);

#ifdef SERIAL_DEBUG
    Serial.print("memory");
    Serial.print("< ");
    Serial.println(availableMemory(), DEC);
#endif

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
