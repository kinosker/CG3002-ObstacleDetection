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
	
	myTimer_DelayMicro2(25); // delay 25 micro seconds...
	//vTaskDelay(5000);	// delay 1ms using vTaskDelay
	
	MaxSonar_TopTriggerStop();
}

// extract the median of 3 values in the sample array
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


// Return filtered reading of maxSonar (Median of 3 values, with noise eliminated)
int myMaxSonar_getFilteredReading(int currentReading, int *sample)
{
	//currentReading = myMaxSonar_Stabilizer(currentReading, prevReading);
	myMaxSonar_AddSample(currentReading, sample);
	return myMaxSonar_extractMedian(sample);

}

// Add sample circular buffer way...
void myMaxSonar_AddSample(int currentReading ,int *sample)
{
	static uint8_t i = SONAR_SAMPLE_SIZE; // start with prev reading index first.. 
	 
	currentReading = myMaxSonar_Stabilizer(currentReading, sample[i]);
	
	i = (i+1) % SONAR_SAMPLE_SIZE; // at correct index already..
	
	sample[i] = currentReading; //store..
}

// return currentReading if it is more than the noise threshold..
int myMaxSonar_Stabilizer(const int currentReading, int prevReading)
{
	if(currentReading > (prevReading + NOISE_THRESHOLD) || currentReading < (prevReading - NOISE_THRESHOLD))
	{
		return currentReading; // when current reading exceed noise range.... return current reading
	}
	else
		return prevReading; // return prevReading if in noise range..
}

// Return distance in cm
int myMaxSonar_Read(char analogChannel)
{
	int reading = 0;
	
	int adcReading = myADC_analogRead(analogChannel);
	reading = adcReading /FLOAT_TO_CM; // get the decimal point of ADC
	reading += adcReading; // add the exact adcReading
	
	if(reading > 1000)
		return 999;
	
	return reading;
}

// Return distance in bits 
int myMaxSonar_ReadRaw(char analogChannel)
{
	return myADC_analogRead(analogChannel);
}
