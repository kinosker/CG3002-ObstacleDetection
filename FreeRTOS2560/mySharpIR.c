/*
 * mySharpIR.c
 *
 * Created: 10/9/2015 2:04:31 PM
 *  Author: tienlong
 */ 
#include <mySharpIR.h>
#include <myADC.h>
#include <math.h>

// Return distance in cm
int mySharpIR_Read(char analogChannel)
{
	int adcReading = myADC_analogRead(analogChannel);
	adcReading = 10650.08 * pow(adcReading,-0.935) - 10;
	if(adcReading > 999)
		adcReading = 999;
	
	return adcReading;
}