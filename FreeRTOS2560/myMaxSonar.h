/*
 * maxSonar.h
 *
 * Created: 9/25/2015 4:31:27 PM
 *  Author: tienlong
 */ 


#ifndef MAXSONAR_H_
#define MAXSONAR_H_

#define MaxSonar_Init()				DDRG |= (1 << DDG1) // Set pin G1 as input (Pin 40)
#define MaxSonar_TriggerStart()		PORTG |= ( 1 << 1 )	// start trigger pin (Pin 40)		
#define MaxSonar_TriggerStop()		PORTG &= ( 0 << 1 ) // stop trigger pin (Pin 40)

#define valueToCM 5

void MaxSonar_Start();

// Return distance in cm
int MaxSonar_Read(char analogChannel);

// Return distance in bits 8:2
int MaxSonar_ReadRaw(char analogChannel);


#endif /* MAXSONAR_H_ */