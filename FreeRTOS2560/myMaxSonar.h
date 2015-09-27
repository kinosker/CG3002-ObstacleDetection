/*
 * maxSonar.h
 *
 * Created: 9/25/2015 4:31:27 PM
 *  Author: tienlong
 */ 


#ifndef MAXSONAR_H_
#define MAXSONAR_H_

#define MaxSonar_Init()				DDRG |= (1 << DDG1) // Set pin G1 as output (Pin 40)
#define MaxSonar_TriggerStart()		PORTG |= ( 1 << 1 )	// start trigger pin (Pin 40)		
#define MaxSonar_TriggerStop()		PORTG &= ~( 1 << 1 ) // stop trigger pin (Pin 40)

#define valueToCM 5

void myMaxSonar_Start();

// Return distance in cm
int myMaxSonar_Read(char analogChannel);

// Return distance in bits 8:2
int myMaxSonar_ReadRaw(char analogChannel);


#endif /* MAXSONAR_H_ */