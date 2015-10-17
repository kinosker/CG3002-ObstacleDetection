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

int myMaxSonar_extractMedian(int * sample)
{
	if (sample[0] > sample[1])
	{
		if (sample[1] > sample[2])
		{
			return sample[1];
		}
		else if (sample[0] > sample[2]) 
		{
			return sample[2];
		}
		else 
		{
			return sample[0];
		}
	}
	else 
	{
		if (sample[0] > sample[2])
		{
			return sample[0];
		} 
		else if (sample[1] > sample[2]) 
		{
			return sample[2];
		}	
		else 
		{
			return sample[1];
		}
	}
}


int myMaxSonar_getFilteredReading(int currentReading, int* prevReading, int *sample, int sampleSize)
{
	currentReading = myMaxSonar_Stabilizer(currentReading, prevReading);
	myMaxSonar_AddSample(currentReading, sample, sampleSize);
	return myMaxSonar_extractMedian(sample);

}



void myMaxSonar_AddSample(int currentReading ,int *sample, int sampleSize)
{
	static int i = 0; 
	
	sample[i] = currentReading;
	i = (i+1) % sampleSize;
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
