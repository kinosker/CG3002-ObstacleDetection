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

char checkWithinRange(int reading, int * checkReading, const char range)
{
	return reading > (*checkReading + range) || reading < (*checkReading - range);
}

// Calibrate if initial reading, firstCheck reading and final reading is within range. ( Stable reading after ....)
// final reading = btmIR reading (i = CALIBRATE_COUNT)
// firstCheck reading = btmIR reading ( i = CALIBRATE_COUNT/2)
// initial reading  = btmIR reading (i = 0)
// Write new calibrate value if all matches..
void mySharpIR_ReCalibrate(int* calibratedReading, int reading)
{
	static const char range = 5; // put at header file later...
	static const char CALIBRATE_COUNT = 20; // put at header file later...
	static int i = 0;
	static int checkReading[2] = {0};
	
	if(checkWithinRange(reading, calibratedReading, range) && i == 0)
	{
		// if current reading and calibratedReading is within range and no checking in progess
		// skip the calibration process... not needed
		return;
	}
	
	
	if (i == 0)
	{
		checkReading[0] = reading;
	}
	else if (i == CALIBRATE_COUNT/2)
	{
		if(checkWithinRange(reading, checkReading, range))
		{
			// Out of range.. restart to find new calibration point..
			checkReading[0] = reading;
			i = 0; // reset to count...
		}
		else
		{	// within range.. need more confirmation
			checkReading[1] = reading;
		}
	}
	else if (i == CALIBRATE_COUNT)
	{
		i = 0; // reset to count..

		if(checkWithinRange(reading, checkReading, range))
		{
			// Out of range.. restart to find new calibration point..
			checkReading[0] = reading;
		}
		else
		{
			// all 3 readings within range... can calibrate as new stable.
			*calibratedReading = checkReading[0]; // btmIR is calibrated..
		}
	}
	
	i = (i+1) % CALIBRATE_COUNT;
}