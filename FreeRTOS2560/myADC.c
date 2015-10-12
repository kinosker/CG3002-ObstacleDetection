/*
 * myADC.c
 *
 * Created: 9/25/2015 1:07:25 AM
 *  Author: tienlong
 */ 

#include <myADC.h>

SemaphoreHandle_t semaGuardStartADC;
SemaphoreHandle_t semaReadADC;

volatile unsigned char adcReading_H;
volatile unsigned char adcReading_L;


/*ADC Conversion Complete Interrupt Service Routine (ISR)*/
ISR(ADC_vect)
{	
	adcReading_L = ADCL; // dont change the ORDER L first then H!!! Reading L will hold H value.
	adcReading_H = ADCH; // read value of H (not used, 640cm and 1280cm for the resolution)
	// give semaphore, dont run block task immediately..
	xSemaphoreGiveFromISR(semaReadADC, pdFALSE);
}

void myADC_Init()
{
	
	ADCSRA = ( 1<<ADEN ) | ( 1<<ADIE ) | ADC_PRESCALER; // enable ADC, enable interrupt, set prescaler
	ADCSRB = 0; // No auto trigger (0 for ADATE), No analog compare... 
	ADMUX = ( 1 << REFS0 ) ; // use AVCC as reference, right adjust (take LSB but not 2 MSB)
	
	// initialize semaphore..
	semaReadADC = xSemaphoreCreateBinary();
	semaGuardStartADC = xSemaphoreCreateMutex();
	xSemaphoreGive(semaGuardStartADC); // free to read at start.
}


int myADC_analogRead(char channel)
{
	myADC_startADC(channel);
	return myADC_readADC(channel);
}

// channel = AN0 to AN15, use the macro...
void myADC_startADC(char channel)
{
	
	 //char buffer [10];	
	xSemaphoreTake(semaGuardStartADC, portMAX_DELAY); // guard against concurrent task starting adc at same time
    ADMUX = ( ADMUX & 11100000 ) | ( channel & 0b00011111 ); // keep bit 5:7, set bit 0:4 as Mux
	
	//transmitUSART0("ADMUX = ");
	//transmitUSART0(itoa(ADMUX, buffer, 2)); // debugging...
	//transmitUSART0(" \n");
	
	ADCSRB = ( ADCSRB & 11100000 ) | (( channel & 0b00100000 ) >> 2); // keep all bits except bit 3, set Mux @ bit 3
	
	
	//transmitUSART0("ADCSRB = ");
	//transmitUSART0(itoa(ADCSRB, buffer, 2)); // debugging...
	//transmitUSART0(" \n");
	//
	
	//vTaskDelay(1);
	ADCSRA |= ( 1 << ADSC );	// READY TO SEND!!
}

int myADC_readADC(char channel)
{
	int adcReading=0;
	xSemaphoreTake(semaReadADC, portMAX_DELAY); // wait for reading...

	adcReading = ((adcReading_H & 0b11) << 8);
	adcReading += adcReading_L;

	xSemaphoreGive(semaGuardStartADC); // reading done, nxt task can start ADC
	
	return adcReading;
}