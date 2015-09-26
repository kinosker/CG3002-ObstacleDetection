/*
 * myHcSonar.h
 *
 * Created: 9/27/2015 2:47:42 AM
 *  Author: tienlong
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <myTimer.h>

#ifndef MYHCSONAR_H_
#define MYHCSONAR_H_

#define MAX_TICKS 250

#define HC_Trigger_Init()			DDRB |= (1 << DDB2); // set trigger pin as output  (Pin 51)
#define HC_Echo_Init()				DDRB &= (0 << DDB3); // set echo pin as input (Pin 50)

#define HcSonar_TriggerStart()		PORTB |= ( 1 << PINB2 )	// start trigger pin 
#define HcSonar_TriggerStop()		PORTB &= ( 0 << PINB2 ) // stop trigger pin 

#define HC_Echo_Read			PINB &= (1 << PINB3) // read echo...


/*
 * myHcSonar.c
 *
 * Created: 9/27/2015 2:47:27 AM
 *  Author: tienlong
 */ 

volatile int ms_tickStart;
volatile int ms_tickLapsed;
volatile int us_tickEnd;
volatile int us_tickStart;
volatile int us_tickLapsed;

SemaphoreHandle_t semaReadReady;

void myHcSonar_Init();

void myHcSonar_Start();

int myHcSonar_Read();



#endif /* MYHCSONAR_H_ */