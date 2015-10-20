/*
 * mySharpIR.c
 *
 * Created: 10/9/2015 2:04:31 PM
 *  Author: tienlong
 */ 
#include <mySharpIR.h>
#include <myADC.h>
#include <math.h>
#include <stdint.h>

// Return distance in cm
int mySharpIR_Read(char analogChannel)
{
	int adcReading = myADC_analogRead(analogChannel);
	adcReading = 10650.08 * pow(adcReading,-0.935) - 10;
	if(adcReading > IR_MAX_VALUE)
		adcReading = IR_MAX_VALUE;
	
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
	static uint8_t i = 0;
	static int checkReading[2] = {0};
	
	if(checkWithinRange(reading, calibratedReading, CALIBRATE_RANGE) && i == 0)
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
		if(checkWithinRange(reading, checkReading, CALIBRATE_RANGE))
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

		if(checkWithinRange(reading, checkReading, CALIBRATE_RANGE))
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
	
	i++;
}
