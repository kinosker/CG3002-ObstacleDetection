/*
 * maxSonar.c
 *
 * Created: 9/25/2015 4:47:37 PM
 *  Author: tienlong
 */ 

#include <myMaxSonar.h>
#include <myADC.h>
#include <myTimer.h>

void myMaxSonar_BtmStart()
{
	MaxSonar_BtmTriggerStart();
	
	myTimer_DelayMicro(25); // delay 25 micro seconds...
	//vTaskDelay(5000);	// delay 1ms using vTaskDelay
	
	MaxSonar_BtmTriggerStop();
}

void myMaxSonar_TopStart()
{
	MaxSonar_TopTriggerStart();
	
	myTimer_DelayMicro(25); // delay 25 micro seconds...
	//vTaskDelay(5000);	// delay 1ms using vTaskDelay
	
	MaxSonar_TopTriggerStop();
}


int myMaxSonar_getMedian(int currentReading, int* prevReading, int *sample, int sampleSize)
{
	currentReading = myMaxSonar_Stabilizer(currentReading, prevReading);
	myMaxSonar_AddSample(currentReading, sample, sampleSize);
	return sample[sampleSize/2];
}



void myMaxSonar_AddSample(int currentReading ,int *sample, int sampleSize)
{
	int i;
	int temp;
	
	for( i = 0 ; i < sampleSize ; i++)
	{
		if(currentReading < sample[i])
		{
			temp = sample[i];
			sample[i] = currentReading;
			currentReading = temp;
		}
	}
}

int myMaxSonar_Stabilizer(const int currentReading, int* prevReading)
{
	if(currentReading > (*prevReading + NOISE_RANGE) || currentReading < (*prevReading - NOISE_RANGE))
	{
		*prevReading = currentReading;
		return currentReading; // when current reading exceed noise range.... return current reading
	}
	else
		return *prevReading; // return prevReading if in noise range..
}

// Return distance in cm
int myMaxSonar_Read(char analogChannel)
{
	int reading = 0;
	
	int adcReading = myADC_analogRead(analogChannel);
	reading = adcReading /FLOAT_TO_CM; // get the decimal point of ADC
	reading += adcReading; // add the exact adcReading
	
	return reading;
}

// Return distance in bits 8:2
int myMaxSonar_ReadRaw(char analogChannel)
{
	return myADC_analogRead(analogChannel);
}
