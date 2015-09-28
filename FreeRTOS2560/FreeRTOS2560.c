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
#include <myHcSonar.h>
#include <myTimer.h>

#include <stdlib.h>
#include <queue.h>


void task1(void *p);
void task2(void *p);
void task3(void *p);
void task4(void *p);
void init();


xQueueHandle queueObstacleData;
xQueueHandle queueObstacleNumber;


typedef struct 
{
	unsigned char deviceID;
	char data[4];
	
} obstacleData;


//void Sonar_Task(void *p)
//{
//TickType_t xLastWakeTime;
//int frontSonar, leftSonar, rightSonar;
//char asciiReading[4];
//
//
//unsigned char front[] = "Front :";
//unsigned char left[] = "Left :";
//unsigned char right[] = "Right :";
//unsigned char btm[] = "Btm :";
//
//unsigned char next[] = " cm, ";
//unsigned char end[] = " cm\n";
//
//xLastWakeTime = xTaskGetTickCount(); // get tick count
//
//while(1)
//{
//myMaxSonar_Start();
//
//
//
//frontSonar = myMaxSonar_Read(AN15);
//
//itoa(frontSonar, asciiReading, 10);
//myUSART_transmitUSART0(front);
//myUSART_transmitUSART0(asciiReading);
//myUSART_transmitUSART0(next);
//
//leftSonar = myMaxSonar_Read(AN14);
//
//itoa(leftSonar, asciiReading, 10);
//myUSART_transmitUSART0(left);
//myUSART_transmitUSART0(asciiReading);
//myUSART_transmitUSART0(next);
//
//
//rightSonar = myMaxSonar_Read(AN13);
//rightSonar -= 10; // Hardware Problem : offset
//
//itoa(rightSonar, asciiReading, 10);
//myUSART_transmitUSART0(right);
//myUSART_transmitUSART0(asciiReading);
//myUSART_transmitUSART0(next);
//
//// test btm sonar
//int btmSonar = myHcSonar_Read();
//itoa(btmSonar, asciiReading, 10);
//myUSART_transmitUSART0(btm);
//myUSART_transmitUSART0(asciiReading);
//myUSART_transmitUSART0(end);
//
//vTaskDelayUntil( &xLastWakeTime, 150);  // delay 150 ms for 3 sonar chain...
//
//}
//}



void RPI_receiveTask(void *p)
{
	while(1)
	{
		myUSART_waitForHandshake();
		myUSART_transmitUSART0("\n\nRPI Handshake Done\n\n");
		
	}

}

void RPI_sendTask(void *p)
{
	//myUSART_startHandShake();
	//myUSART_transmitUSART0("\n\nMy Handshake Done\n\n");
	
	obstacleData dataToSend;
	char obstacleDetected;
	

	
	while(1)
	{
		xQueueReceive(queueObstacleNumber, &(obstacleDetected), portMAX_DELAY);
		
		
		while(obstacleDetected--)
		{
			//myUSART_transmitUSART0_c((obstacleDetected + '0'));
			//myUSART_transmitUSART0_c(('\n'));
			
			
			xQueueReceive(queueObstacleData, &(dataToSend), portMAX_DELAY);
			myUSART_transmitUSART0_c(dataToSend.deviceID);
			myUSART_transmitUSART0(": ");
			myUSART_transmitUSART0(dataToSend.data);
			myUSART_transmitUSART0(", ");
			

		}
		myUSART_transmitUSART0_c('\n');
		
		vTaskDelay(500);
	}

	

	
	

}



void obstacleSend(char deviceToSend, int reading)
{
	obstacleData queueData;

	
	if(deviceToSend)
	{
		itoa(reading, queueData.data, 10); // convert to ascii
		
		queueData.deviceID = deviceToSend;
			
		xQueueSendToBack(queueObstacleData, &queueData, portMAX_DELAY); // send data to queueData
	}
}

void Sonar_Task(void *p)
{
	TickType_t xLastWakeTime;
	char obstacleDetected = 0;
	int frontSonar, leftSonar, rightSonar, btmSonar;
	char deviceToSend[4] = {0};
	char end= '\n';
	
	xLastWakeTime = xTaskGetTickCount(); // get tick count
		
	while(1)
	{
		myMaxSonar_Start();
		frontSonar = myMaxSonar_Read(AN15);
		leftSonar = myMaxSonar_Read(AN14);
		rightSonar = myMaxSonar_Read(AN13) - 10;
		btmSonar = myHcSonar_Read();
		
		if (btmSonar > 999)
			btmSonar = 0;
		
	//	if(frontSonar < OBSTACLE_DISTANCE)
		{
			obstacleDetected ++;
			deviceToSend[FRONT_DEVICE] = FRONT_SONAR_ID;
		}
	//	if (leftSonar < OBSTACLE_DISTANCE)
		{
			obstacleDetected ++;
			deviceToSend[LEFT_DEVICE] = LEFT_SONAR_ID;		
		}
	//	if (rightSonar < OBSTACLE_DISTANCE)
		{	
			obstacleDetected++;
			deviceToSend[RIGHT_DEVICE] = RIGHT_SONAR_ID;
		}
	//	if (btmSonar < OBSTACLE_DISTANCE)
		{
			obstacleDetected++;
			deviceToSend[BTM_DEVICE] = BTM_SONAR_ID;
		}
		
	//	if(obstacleDetected > 0)
		{	
			//obstacleDetected + '0'; // simple conversion to ascii..
			
			xQueueSendToBack(queueObstacleNumber,  &obstacleDetected, portMAX_DELAY); // send obstacle...
			
			// start handshake...
			//myUSART_startHandShake();
			//myUSART_transmitUSART0("\n\nMy Handshake Done\n\n");
			// end handshake
			
			//myUSART_transmitUSART0(obstacleDetected);
			//myUSART_transmitUSART0_c(end);
			
			obstacleSend(deviceToSend[FRONT_DEVICE], frontSonar);
//			myUSART_transmitUSART0_c(end);
			obstacleSend(deviceToSend[LEFT_DEVICE], leftSonar);
	//		myUSART_transmitUSART0_c(end);
			obstacleSend(deviceToSend[RIGHT_DEVICE], rightSonar);
		//	myUSART_transmitUSART0_c(end);
			obstacleSend(deviceToSend[BTM_DEVICE], btmSonar);
			//myUSART_transmitUSART0_c(end);

			obstacleDetected = 0; // reset back...
			
			deviceToSend[0] = deviceToSend[1] = deviceToSend[2] = deviceToSend[3] = 0;
			
		}
		
		vTaskDelayUntil( &xLastWakeTime, 150);  // delay 150 ms for 3 sonar chain...
	}
}



void myTimerTask(void *p)
{
	TaskHandle_t *t_delay = (TaskHandle_t*)p; // recast back.

	
	myTimer_Init(t_delay);	
	
	while(1)
	{
		myTimer_DelayChecker(); // task should be suspended and resumed only when delayMicro is used..
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
		TaskHandle_t t_maxSonar, t_rx, t_tx, t_delay, t1;
		
		init();

		xTaskCreate(task1, "Task 1", BLINK_1_STACK, NULL, BLINK_1_PRIORITY, &t1);
		//xTaskCreate(task2, "Task 2", BLINK_2_STACK, NULL, BLINK_2_PRIORITY, &t2);
		
		xTaskCreate(myTimerTask, "myTimer", MY_TIMER_STACK, (&t_delay) , MY_TIMER_PRIORITY, &t_delay); // danger?!?		
		xTaskCreate(Sonar_Task, "maxSonar", MAXSONAR_STACK, NULL, MAXSONAR_PRIORITY, &t_maxSonar);

		
		xTaskCreate(RPI_receiveTask, "RPI_Receive", RPI_RECEIVE_STACK, NULL, RPI_RECEIVE_PRIORITY, &t_rx);
		xTaskCreate(RPI_sendTask, "RPI_Send", RPI_SEND_STACK, NULL, RPI_SEND_PRIORITY, &t_tx);
	

		
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
		myUSART_USART0_Init();
		myUSART_USART1_Init();
		myADC_Init();
		MaxSonar_Init();
		myHcSonar_Init();
		
		queueObstacleNumber = xQueueCreate(QUEUE_SIZE, sizeof (char)); // create queue
		queueObstacleData = xQueueCreate( (QUEUE_SIZE*SONAR_NUM), sizeof (obstacleData)); // create queue
		
		DDRB |= (1 << DDB6) | (1 << DDB7); // set direction for led
		
		
	}
	
	sei(); // enable interrupts..
}








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


//void task2(void *p)
//{
//while(1)
//{
//PORTB |= ( 1 << 7 );
//vTaskDelay(500);
//PORTB &= ( 0 << 7 );
//vTaskDelay(500);
//
//}
//}

