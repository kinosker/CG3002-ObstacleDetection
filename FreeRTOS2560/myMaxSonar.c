/*
 * maxSonar.c
 *
 * Created: 9/25/2015 4:47:37 PM
 *  Author: tienlong
 */ 

#include <myMaxSonar.h>
#include <myADC.h>

void MaxSonar_Start()
{
	MaxSonar_TriggerStart();
	
	vTaskDelay(1);	
	
	MaxSonar_TriggerStop();
}

// Return distance in cm
int MaxSonar_Read(char analogChannel)
{
	return analogRead(analogChannel) * valueToCM;
}

// Return distance in bits 8:2
int MaxSonar_ReadRaw(char analogChannel)
{
	return analogRead(analogChannel);
}
