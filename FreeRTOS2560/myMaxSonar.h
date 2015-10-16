/*
 * maxSonar.h
 *
 * Created: 9/25/2015 4:31:27 PM
 *  Author: tienlong
 */ 


#ifndef MAXSONAR_H_
#define MAXSONAR_H_

#define NOISE_RANGE 8

#define MaxSonar_Init()					DDRG |= ((1 << DDG1) | (1 << DDG0)) // Set pin G1,G0 as output (Pin 40, 41)

#define MaxSonar_BtmTriggerStart()		PORTG |= ( 1 << DDG1 )		// start trigger pin (Pin 40)		
#define MaxSonar_BtmTriggerStop()		PORTG &= ~( 1 << DDG1 )		// stop trigger pin (Pin 40)

#define MaxSonar_TopTriggerStart()		PORTG |= ( 1 << DDG0 )		// start trigger pin (Pin 41)
#define MaxSonar_TopTriggerStop()		PORTG &= ~( 1 << DDG0 )		// stop trigger pin (Pin 41)


// Resolution for MaxSonar = 5v/512 per inch
// ADC resolution = 5/1024..
// CM = 1/2 * 2.54  = 1.27 (0.27 left over ~ 1/4 or 1/3) 
// FLOAT to CM should be /3
#define FLOAT_TO_CM 3


void myMaxSonar_BtmStart();
void myMaxSonar_TopStart();

int myMaxSonar_getMedian(int currentReading, int* prevReading, int *sample, int sampleSize);
void myMaxSonar_AddSample(int currentReading ,int *sample, int sampleSize);

// Get new value only if it exceed noise limit.
int myMaxSonar_Stabilizer(const int currentReading, int* prevReading);

// Return distance in cm
int myMaxSonar_Read(char analogChannel);

// Return distance in bits 8:2
int myMaxSonar_ReadRaw(char analogChannel);


#endif /* MAXSONAR_H_ */