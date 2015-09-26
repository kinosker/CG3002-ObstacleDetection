/*
* FreeRTOS2560.c
*
* Created: 15/09/2015 15:41:32
*  Author: tienlong
*/


#include <avr/io.h>
#include <util/atomic.h>
#include <FreeRTOS.h>
#include <task.h>
//#include <Arduino.h>
#include <myUSART.h>
#include <myTaskConfig.h>
#include <myADC.h>
#include <myMaxSonar.h>

#include <stdlib.h>


void task1(void *p);
void task2(void *p);
void task3(void *p);
void task4(void *p);
void init();


// Tasks flash LEDs at Pins 12 and 13 at 1Hz and 2Hz respectively.
void task1(void *p)
{
	while(1)
	{
		PORTB |= ( 1 << 6 );
		vTaskDelay(1000); // Delay for 500 ticks. Since each tick is 1ms,
		//this delays for 500ms.
		PORTB &= ( 0 << 6 );
		vTaskDelay(1000);
	}
}


void task2(void *p)
{
	while(1)
	{
		PORTB |= ( 1 << 7 );
		vTaskDelay(500);
		PORTB &= ( 0 << 7 );
		vTaskDelay(500);
		
	}
}


void RPI_receiveTask(void *p)
{
	waitForHandshake();	
	transmitUSART0("\n\nRPI Handshake Done\n\n");

}

void RPI_sendTask(void *p)
{
	startHandShake();
	transmitUSART0("\n\nMy Handshake Done\n\n");
}


void maxSonarTask(void *p)
{
	int frontSonar, leftSonar, rightSonar;
	char asciiReading[4];
	
	unsigned char front[] = "Front :";
	unsigned char left[] = "Left :";
	unsigned char right[] = "Right :";
	
	unsigned char next[] = " cm, ";
	unsigned char end[] = " cm\n";
	
	
	while(1)
	{
		MaxSonar_Start();
		
		frontSonar = MaxSonar_Read(AN15);

		itoa(frontSonar, asciiReading, 10);
		transmitUSART0(front);
		transmitUSART0(asciiReading);
		transmitUSART0(next);

		leftSonar = MaxSonar_Read(AN14);		
		
		itoa(leftSonar, asciiReading, 10);
		transmitUSART0(left);
		transmitUSART0(asciiReading);
		transmitUSART0(next);
		
		
		rightSonar = MaxSonar_Read(AN13);
		rightSonar -= 10; // Hardware Problem : offset
		
		itoa(rightSonar, asciiReading, 10);
		transmitUSART0(right);
		transmitUSART0(asciiReading);
		transmitUSART0(end);
		
		
		vTaskDelay(150); // delay 150 ms for sonar chain...
		
	}
}


void vApplicationIdleHook()
{
	
	// do nth
}

int main(void)
{
	while(1)
	{
		init();
		TaskHandle_t t1,t2,t3,t4,t5;
		
		xTaskCreate(maxSonarTask, "maxSonar", MAXSONAR_STACK, NULL, MAXSONAR_PRIORITY, &t3);
		xTaskCreate(task1, "Task 1", BLINK_1_STACK, NULL, BLINK_1_PRIORITY, &t1);
		xTaskCreate(task2, "Task 2", BLINK_2_STACK, NULL, BLINK_2_PRIORITY, &t2);
		
		xTaskCreate(RPI_receiveTask, "RPI_Receive", RPI_RECEIVE_STACK, NULL, RPI_RECEIVE_PRIORITY, &t4);
		xTaskCreate(RPI_sendTask, "RPI_Send", RPI_SEND_STACK, NULL, RPI_SEND_PRIORITY, &t5);
		

		vTaskStartScheduler();
		
	}
}



void setDigitalInputPowerReduction()
{
	// Set when confirm which ADC pin not used..
	//DIDR0 =s
	//DIDR1 =
	//DIDR2 =
}

// Shut down unused...
void setPowerReduction()
{
	PRR0 = (1<<PRTWI) | (1<<PRTIM2) | (1<<PRTIM1) | (1<<PRSPI); // Shut down I2C, timer2, timer 1, SPI
	// Include USART0 after finish debugging using usb.... (1<<PRUSART0)
	
	PRR1 = (1<<PRTIM5) | (1<<PRTIM4) | (1<<PRTIM3) | (1<<PRUSART3) | (1<<PRUSART2); // Shut down timer5, timer4, timer3, USART3, USART2
	
	setDigitalInputPowerReduction();
}

void clearTimer()
{
	// Clear Timer1
	TCCR1A = 0;     // Clear all settings
	TCCR1B = 0;     // Clear all settings
	TIMSK1 = 0;     // Disable all interrupts

	// Clear Timer2
	TCCR2A = 0;     // Clear all settings
	TCCR2B = 0;     // Clear all settings
	TIMSK2 = 0;     // Disable all interrupts
	
	// Clear Timer3
	TCCR3A = 0;     // Clear all settings
	TCCR3B = 0;     // Clear all settings
	TIMSK3 = 0;     // Disable all interrupts

	// Clear Timer4
	TCCR4A = 0;     // Clear all settings
	TCCR4B = 0;     // Clear all settings
	TIMSK4 = 0;     // Disable all interrupts

	// Clear Timer5
	TCCR5A = 0;     // Clear all settings
	TCCR5B = 0;     // Clear all settings
	TIMSK5 = 0;     // Disable all interrupts
}

void init()
{
	// Ensure atomic...
	ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
	{
		clearTimer();
		setPowerReduction();
		USART0_Init();
		USART1_Init();
		ADC_Init();
		MaxSonar_Init();
		
		DDRB |= (1 << DDB6) | (1 << DDB7); // set direction...
		
		
	}
	
	sei(); // enable interrupts..
}


/* itoa:  convert n to characters in s */
//void itoa(int n, unsigned char s[])
//{
	//int i = 0;
	//
	//do {       /* generate digits in reverse order */
		//s[i++] = n % 10 + '0';   /* get next digit */	
	//}	while ((n /= 10) > 0);     /* delete it */
	//
	//reverse(i,s);
//}
//
//void reverse(int size, unsigned char s[])
//{
	//int i, j;
	//char c;
	//
	//for (i = 0, j = size-1; i<j; i++, j--) 
	//{
		//c = s[i];
		//s[i] = s[j];
		//s[j] = c;
	//}
//}