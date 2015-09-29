/*
 * maxSonar.c
 *
 * Created: 9/25/2015 4:47:37 PM
 *  Author: tienlong
 */ 

#include <myMaxSonar.h>
#include <myADC.h>
#include <myTimer.h>

void myMaxSonar_Start()
{
	MaxSonar_TriggerStart();
	
	myTimer_DelayMicro(25); // delay 25 micro seconds...
	//vTaskDelay(1);	// delay 1ms using vTaskDelay
	
	MaxSonar_TriggerStop();
}

// Return distance in cm
int myMaxSonar_Read(char analogChannel)
{
	int reading = 0;
	
	unsigned char adcReading = myADC_analogRead(analogChannel);
	reading = adcReading * valueToCM;
	reading = reading + (adcReading/12); // divided by 12 for float compensation
	
	return reading;
}

// Return distance in bits 8:2
int myMaxSonar_ReadRaw(char analogChannel)
{
	return myADC_analogRead(analogChannel);
}
