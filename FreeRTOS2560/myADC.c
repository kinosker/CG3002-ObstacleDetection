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


unsigned char myADC_analogRead(char channel)
{
	myADC_startADC(channel);
	return myADC_readADC(channel);
}

// channel = AN0 to AN15, use the macro...
void myADC_startADC(char channel)
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

unsigned char myADC_readADC(char channel)
{
	xSemaphoreTake(semaReadADC, portMAX_DELAY); // wait for reading...
	xSemaphoreGive(semaGuardStartADC); // reading done, nxt task can start ADC

	if(adcReading_H)
	{
		return 0xFF; // return max reading if got high reading..
	}
	else
	{
			return adcReading_L; // return L bits reading dont need far distance...		
	}
}