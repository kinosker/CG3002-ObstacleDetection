/*
 * myADC.c
 *
 * Created: 9/25/2015 1:07:25 AM
 *  Author: tienlong
 */ 

#include <myADC.h>

SemaphoreHandle_t semaGuardStartADC;
SemaphoreHandle_t semaReadADC;

volatile unsigned char adcReading;


/*ADC Conversion Complete Interrupt Service Routine (ISR)*/
ISR(ADC_vect)
{	
	adcReading = ADCH; // read value
	
	// give semaphore, dont run block task immediately..
	xSemaphoreGiveFromISR(semaReadADC, pdFALSE);
}

void ADC_Init()
{
	
	ADCSRA = ( 1<<ADEN ) | ( 1<<ADIE ) | ADC_PRESCALER; // enable ADC, enable interrupt, set prescaler
	ADCSRB = 0; // No auto trigger (0 for ADATE), No analog compare... 
	ADMUX = ( 1 << REFS0 ) | (1 << ADLAR ); // use AVCC as reference, Left adjust (dont need 2 LSB)
	
	// initialize semaphore..
	semaReadADC = xSemaphoreCreateBinary();
	semaGuardStartADC = xSemaphoreCreateMutex();
	xSemaphoreGive(semaGuardStartADC); // free to read at start.
}


unsigned char analogRead(char channel)
{
	startADC(channel);
	return readADC(channel);
}

// channel = AN0 to AN15, use the macro...
void startADC(char channel)
{
	
	 //char buffer [10];
	
	
	xSemaphoreTake(semaGuardStartADC, portMAX_DELAY);
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

unsigned char readADC(char channel)
{
	xSemaphoreTake(semaReadADC, portMAX_DELAY); // wait for reading...
	xSemaphoreGive(semaGuardStartADC); // reading done, nxt task can start ADC
	return adcReading;
}