/*
 * myUSART.c
 *
 * Created: 9/23/2015 12:08:08 AM
 *  Author: tienlong
 */ 

#include <myUSART.h>
#include <ringBuffer.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <ringBuffer.h>
#include <FreeRTOS.h>
#include <semphr.h>


SemaphoreHandle_t semaUsart1Receive = NULL;
SemaphoreHandle_t semaUsart0Receive = NULL;
SemaphoreHandle_t semaUsart1HandShake = NULL;
SemaphoreHandle_t mutexUsart0Transmit = NULL;
SemaphoreHandle_t mutexUsart1Transmit = NULL;


 ringBuffer uart0_rxRingBuffer;
 ringBuffer uart0_txRingBuffer;
 ringBuffer uart1_rxRingBuffer;
 ringBuffer uart1_txRingBuffer;

unsigned char uart0_rxBuffer[RX_SIZE];
unsigned char uart0_txBuffer[TX_SIZE];
unsigned char uart1_rxBuffer[RX_SIZE];
unsigned char uart1_txBuffer[TX_SIZE];


ISR( USART1_RX_vect )
{
	// if using parity bit, check error....
	unsigned char receive = UDR1;
	ringBufferPush(&uart1_rxRingBuffer, receive);
	
	// give semaphore, dont run block task immediately..
	xSemaphoreGiveFromISR(semaUsart1Receive, pdFALSE);
	
	if(ringBufferFull(&uart1_rxRingBuffer))
	{
		// send data overwritten error...
	}
}


ISR( USART0_RX_vect )
{
	// if using parity bit, check error....
	unsigned char receive = UDR0;
	ringBufferPush(&uart0_rxRingBuffer, receive);
	
	
	// give semaphore, dont run block task immediately..
	xSemaphoreGiveFromISR(semaUsart1Receive, pdFALSE);
	
	if(ringBufferFull(&uart0_rxRingBuffer))
	{
		// send data overwritten error...
	}
}

ISR( USART1_UDRE_vect )
{
	if(ringBufferNotEmpty(&uart1_txRingBuffer))
	{
		UDR1 = ringBufferPop(&uart1_txRingBuffer);
	}
	else
	{
		// Nothing more to transmit so disable UDRE interrupts
		UCSR1B &= ~( 1 << UDRIE1 );
		xSemaphoreGiveFromISR(mutexUsart1Transmit, pdFALSE);
	}
}

ISR( USART0_UDRE_vect )
{
	if(ringBufferNotEmpty(&uart0_txRingBuffer))
	{
		UDR0 = ringBufferPop(&uart0_txRingBuffer);
	}
	else
	{
		// Nothing more to transmit so disable UDRE interrupts
		UCSR0B &= ~( 1 << UDRIE0 );
		xSemaphoreGiveFromISR(mutexUsart0Transmit, pdFALSE);

	}
}

 
 void myUSART_USART0_Init(void)
 {
		 
	// Set baud rate
	UBRR0H = (BAUD_PRESCALE >> 8); // Set upper 8-bit of prescale to register...
	UBRR0L = BAUD_PRESCALE;// Set lower 8-bit of prescale to register...
	
	//UCSR0A = (1<< U2X0); // Set double async usart transmission speed 
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0) | ( 1 << UDRIE0 ); // Enable receiver and transmitter and receive complete interrupt
	UCSR0C = ( 1 << UCSZ00 ) | ( 1 << UCSZ01 );    // 8 bit data format

	
	// set buffer as ring...	
	ringBufferInit(&uart0_rxRingBuffer, uart0_rxBuffer, RX_SIZE);
	ringBufferInit(&uart0_txRingBuffer, uart0_txBuffer, TX_SIZE);
	
	// Semaphore
	semaUsart0Receive = xSemaphoreCreateBinary();
	mutexUsart0Transmit = xSemaphoreCreateMutex();
	
}

void myUSART_USART1_Init(void)
{
	// Set baud rate
	UBRR1H = (BAUD_PRESCALE >> 8); // Set upper 8-bit of prescale to register...
	UBRR1L = BAUD_PRESCALE;// Set lower 8-bit of prescale to register...
	
	//UCSR1A = (1<< U2X1); // Set double async usart transmission speed
	UCSR1B = ( 1 << TXEN1 ) | ( 1 <<RXEN1 ) | ( 1 << RXCIE1 ) | ( 1 << UDRIE1 ); // Enable receiver and transmitter and receive complete, transmit interrupt
	UCSR1C = ( 1 << UCSZ10 ) | ( 1 << UCSZ11 );    // 8 bit data format
	
	// set buffer as ring...
	ringBufferInit(&uart1_rxRingBuffer, uart1_rxBuffer, RX_SIZE);
	ringBufferInit(&uart1_txRingBuffer, uart1_txBuffer, TX_SIZE);
	
	
	// Semaphore
	semaUsart1Receive = xSemaphoreCreateBinary();
	semaUsart1HandShake = xSemaphoreCreateBinary();
	mutexUsart1Transmit = xSemaphoreCreateMutex();
	
}


void myUSART_transmitUSART0_c(unsigned char data )
{
	xSemaphoreTake(mutexUsart0Transmit, portMAX_DELAY);

	while ( ringBufferFull(&uart0_txRingBuffer)); // wait till there is space..
	
	ringBufferPush(&uart0_txRingBuffer, data);

	UCSR0B |= ( 1 << UDRIE0 ); // Set data register empty interrupt..
	UCSR0A |= ( 1 << TXC0 ); // Ready to send!!
	
}

void myUSART_transmitUSART1_c(unsigned char data )
{
	xSemaphoreTake(mutexUsart1Transmit, portMAX_DELAY);
	
	
	while ( ringBufferFull(&uart1_txRingBuffer)); // wait till there is space..
	
	ringBufferPush(&uart1_txRingBuffer, data);

	UCSR1B |= ( 1 << UDRIE1 ); // Set data register empty interrupt..
	UCSR1A |= ( 1 << TXC1 ); // Ready to send!!
	
}

void myUSART_transmitUSART1(const unsigned char* data)
{	
	while (*data)
	{
			myUSART_transmitUSART1_c(*data++);
	}

}

void myUSART_transmitUSART0(const unsigned char* data)
{	
	while (*data)
	{
		myUSART_transmitUSART0_c(*data++);
	}

}

unsigned char myUSART_receiveUSART1()
{
	unsigned char data;
	
	xSemaphoreTake(semaUsart1Receive, portMAX_DELAY);
	//( TickType_t ) 10 ) == pdTRUE ...
	
	data = ringBufferPop(&uart1_rxRingBuffer);
	
	//need?
	if(ringBufferNotEmpty(&uart1_rxRingBuffer))
	{
		xSemaphoreGive(semaUsart1Receive);
	}
	
	return data;
}

unsigned char myUSART_receiveUSART0()
{
	unsigned char data;
	
	xSemaphoreTake(semaUsart0Receive, portMAX_DELAY);
	
	data = ringBufferPop(&uart0_rxRingBuffer);
	
	// need?
	if(ringBufferNotEmpty(&uart0_rxRingBuffer))
	{
		xSemaphoreGive(semaUsart0Receive);
	}
	
	return data;
}

// 0 if fail, 1 success
char myUSART_startHandShake()
{
	
	myUSART_transmitUSART1_c(HANDSHAKE_START);

	if( xSemaphoreTake( semaUsart1HandShake, ( TickType_t ) HANDSHAKE_TIMEOUT ) == pdTRUE )
	{	
		// got the semaphore
		myUSART_transmitUSART1_c(HANDSHAKE_FIN);
		return 1;
	}
	else
	{
		return 0;
	}

}


void myUSART_completeHandShake()
{
	xSemaphoreGive(semaUsart1HandShake);	
}

char myUSART_receiveHandShakeAck(unsigned char message)
{
	return (message == HANDSHAKE_ACK);
}

char myUSART_receiveHandShakeStart(unsigned char message)
{
	return (message == HANDSHAKE_START);
}


char myUSART_receiveHandShakeFin(unsigned char message)
{
	return (message == HANDSHAKE_FIN);
}


char myUSART_receiveMessageACK(unsigned char message)
{
	return (message == MESSAGE_ACK);
}


unsigned char myUSART_peekReceiveUSART1()
{
	unsigned char data;
	
	xSemaphoreTake(semaUsart1Receive, portMAX_DELAY);
	data = ringBufferPeek(&uart1_rxRingBuffer);
	
	//need?
	if(ringBufferNotEmpty(&uart1_rxRingBuffer))
	{
		xSemaphoreGive(semaUsart1Receive);
	}
	
	return data;
}