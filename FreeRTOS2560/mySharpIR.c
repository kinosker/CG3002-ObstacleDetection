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
#include <myUSART.h>

// Return distance in cm
int mySharpIR_Read(char analogChannel)
{
	int adcReading = myADC_analogRead(analogChannel);
	adcReading = 10650.08 * pow(adcReading,-0.935) - 10;
	if(adcReading > IR_MAX_VALUE || adcReading < IR_MIN_VALUE)
		adcReading = IR_MAX_VALUE;
	
	return adcReading;
}

// return 1 if out of range..
char checkOutOfRange(int reading, int * checkReading, const char range)
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
	static uint8_t sampleCount = 0;
	static int referenceReading = 0;
	//static int testPrint = 0;
	
	if(!checkOutOfRange(reading, calibratedReading, CALIBRATE_RANGE) && sampleCount == 0)
	{
		// if current reading and calibratedReading is within range and no checking in progess
		// skip the calibration process... not needed
		return;
	}
	
	if(reading < CALIBRATE_LOW_THRESHOLD || reading > CALIBRATE_HIGH_THRESHOLD)
	{
		// Too low or Too high for it to be ground..
		referenceReading = 0;
		return;		
	}
	
	
	if (sampleCount == 0)
	{
		// Possible calibration... take reference from reading
		//testPrint = 0;
		referenceReading = reading;
		//myUSART_transmitUSART0("\n-----RS----\n");

	}
	else if ((sampleCount % CALIBRATE_SAMPLE_RATE) == 0) // when count reach 
	{
		//testPrint++;
		//myUSART_transmitUSART0_c(testPrint + '0');
		//myUSART_transmitUSART0("\n");
		if(checkOutOfRange(reading, &referenceReading, CALIBRATE_RANGE))
		{
			// Out of range.. restart to find new calibration point..
			referenceReading = 0;
			sampleCount = 0; // reset to count...
			return;
		}
		else
		{	// within range.. need more confirmation, update reference point..
			referenceReading = reading;
		}
	}
	else if (sampleCount >= CALIBRATE_SAMPLE_REQUIRE)
	{
		// Sample long enough...
		sampleCount = 0; // reset to count..

		if(checkOutOfRange(reading, &referenceReading, CALIBRATE_RANGE))
		{
			// Out of range.. restart to find new calibration point..
			referenceReading = 0;
			return;
		}
		else
		{
			// all readings within range... can calibrate as new stable.
			*calibratedReading = referenceReading; // btmIR is calibrated..
			//myUSART_transmitUSART0("\nACK\n");
			return;
		}
	}
	
	sampleCount++;
}
