/*
 * myHcSonar.h
 *
 * Created: 9/27/2015 2:47:42 AM
 *  Author: tienlong
 */ 



#ifndef MYHCSONAR_H_
#define MYHCSONAR_H_

#define MAX_TICKS 250


#define HC_Echo_Init()				DDRB &= ~(1 << DDB3) // set echo pin as input (Pin 50)
#define HC_Trigger_Init()			DDRB |= (1 << DDB2) // set trigger pin as output  (Pin 51)

#define HcSonar_TriggerStart()		PORTB |= ( 1 << 2 )	// start trigger pin 
#define HcSonar_TriggerStop()		PORTB &= ~( 1 << 2 ) // stop trigger pin 

#define HC_Echo_Read				(PINB & (1 << PINB3)) // read echo...


void myHcSonar_Init();

void myHcSonar_Start();

int myHcSonar_Read();



#endif /* MYHCSONAR_H_ */