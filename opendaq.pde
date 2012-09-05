
#include "odstream.h"
#include "datachannel.h"
#include "commdata.h"
#include "calibration.h"

extern DataChannel Channel1, Channel2, Channel3, Channel4;
extern DStream ODStream;


void setup() {
	int i;

	daqInit();
	
	i = Cal.RecallCalibration();
	
	Comm.begin();
	SetDigitalDir(0X0F);
	
	#ifdef SERIAL_DEBUG
  Serial.print("Calibracion: ");
  Serial.println(i,HEX);	
	for(i=0;i<6;i++){
		Serial.print(i,HEX);	
		Serial.print(":  m= ");
		Serial.print(Cal.gain_m[i],DEC);	
		Serial.print("  b= ");
		Serial.println(Cal.gain_b[i],DEC);			
	}
	#endif
	
	/*
	pioMode(0, OUTPUT);
	pioWrite(	0, LOW);
*/
		
	delay(100);
	SetAnalogVoltage(0);
	delay(100);
	SetAnalogVoltage(900);


	ODStream.Initialize();
	ODStream.CreateStreamChannel(1,100);
	ODStream.ConfigChan(1, ANALOG_INPUT, 7, 0, 0);
/*	ODStream.Start();	//FAILS! if no channel activated

	ODStream.CreateStreamChannel(2,2);
	ODStream.CreateStreamChannel(3,40);
	ODStream.ConfigChan(2, ANALOG_INPUT, 7, 0, 8);
	ODStream.ConfigChan(3, ANALOG_INPUT, 7, 0, 1);
  
  
	ODStream.CreateBurstChannel(400);
	ODStream.CreateExternalChannel(2,0);
	ODStream.ConfigChan(1, ANALOG_OUTPUT);
	ODStream.SetupChan(1,2,0);
	i=0;
	while(i<=2){
		Channel1.databuffer[i] = 600 * (1+i);
		i++;
	}
	
	ODStream.CreateStreamChannel(2,20);
	ODStream.ConfigChan(2, ANALOG_INPUT, 7, 0, 8);
	*/
/*

	ODStream.CreateStreamChannel(3,60);
	ODStream.CreateStreamChannel(4,50);
	ODStream.TriggerMode(3, DIN5_TRG, 1);
	*/
	

	#ifdef SERIAL_DEBUG
  Serial.print("memoria");
  Serial.print("< ");
  Serial.println(availableMemory(),DEC);	
  #endif
	
	ledSet(LEDGREEN,1);
	ledSet(LEDRED,0);
	
}
  

void loop() 
{ 	
  static int j=0,i=0;
	
	#ifdef SERIAL_DEBUG
		delay(300);
  #endif
	
	ODStream.CheckTriggers();
	Comm.Process_Stream();
	while(Comm.available()) {
		Comm.parseInput();
	}
	

}


