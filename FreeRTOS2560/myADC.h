/*
 * myADC.h
 *
 * Created: 9/25/2015 10:16:42 AM
 *  Author: tienlong
 */ 


#ifndef MYADC_H_
#define MYADC_H_


#include <avr/io.h>
#include <avr/interrupt.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

// Mux bits to input to read analog channel
#define AN0 0
#define AN1 1
#define AN2 2
#define AN3 3
#define AN4 4
#define AN5 5
#define AN6 6
#define AN7 7
#define AN8 32
#define AN9 33
#define AN10 34
#define AN11 35
#define AN12 36
#define AN13 37
#define AN14 38
#define AN15 39
// end mux bit.

#define ADC_PRESCALER 7 // Select div factor of 128, 16Mhz/128 = 125Khz... (ADPS0:2 = 1 => 0b111)
	
void myADC_Init();

unsigned char myADC_analogRead(char channel);

// channel = AN0 to AN15, use the macro...
void myADC_startADC(char channel);

unsigned char myADC_readADC(char channel);



#endif /* MYADC_H_ */