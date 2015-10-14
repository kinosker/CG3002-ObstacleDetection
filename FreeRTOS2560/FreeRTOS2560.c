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
#include <mySharpIR.h>
#include <myTimer.h>
#include <mySharpIR.h>
#include <myObstacleHandler.h>
#include <myMotor.h>

#include <stdlib.h>
#include <queue.h>


void task1(void *p);
void task2(void *p);
void task3(void *p);
void task4(void *p);
void init();

void obstacleSend(char deviceBlocked, int reading);
void sendObstacleDetected(char obstacleDetected, char * deviceBlocked, int frontSonar, int leftSonar, int rightSonar, int btmIR, int topSonar);

xQueueHandle queueObstacleData;
xQueueHandle queueObstacleNumber;


typedef struct 
{
	unsigned char deviceID;
	char data[4];
	
} obstacleData;



// receive task should peek........
// if is handshake ACK give semaphore...
//
void RPI_receiveTask(void *p)
{
	unsigned char data;
	while(1)
	{
		data = myUSART_peekReceiveUSART1();
		//myUSART_transmitUSART0("R = ");	
		//myUSART_transmitUSART0(data);
		//myUSART_transmitUSART0("\n");
		
		if (myUSART_receiveHandShakeAck(data))
		{
			myUSART_receiveUSART1(); // pop the data
			myUSART_completeHandShake();
		}
		else if (myUSART_receiveMessageACK(data))
		{
			// clear buffer for RPI_SEND
			myUSART_receiveUSART1(); // pop the data
			//myUSART_transmitUSART0("MSG ACk!\n");
		}
		else if (myUSART_receiveHandShakeStart(data))
		{
			myUSART_waitForHandshake();
			//myUSART_transmitUSART0("\n\nRPI Handshake Done\n\n");
				
		}
		else
		{
			myUSART_receiveUSART1(); // pop the data		
			
		}
	}

}

	
void RPI_sendTask(void *p)
{
	
	
	obstacleData dataToSend;
	char obstacleDetected;

	
	while(1)
	{
	//	myUSART_transmitUSART0("Init\n");
	//	myUSART_startHandShake();
	//	myUSART_transmitUSART0("Hs Done\n");
			
			
		xQueueReceive(queueObstacleNumber, &(obstacleDetected), portMAX_DELAY);
	
		myUSART_transmitUSART1_c(obstacleDetected+'0');
		myUSART_transmitUSART1_c('\n');
	
		
		while(obstacleDetected--)
		{

			xQueueReceive(queueObstacleData, &(dataToSend), portMAX_DELAY);
			
			myUSART_transmitUSART0_c(dataToSend.deviceID);
			myUSART_transmitUSART1_c(dataToSend.deviceID);
				
			myUSART_transmitUSART0(": ");
			
			myUSART_transmitUSART0(dataToSend.data);
			myUSART_transmitUSART1(dataToSend.data);
			myUSART_transmitUSART1_c('\n');
				
			myUSART_transmitUSART0(", ");
			
		}	
		myUSART_transmitUSART0_c('\n');
		
	}
}


void Sonar_Task(void *p)
{
	TickType_t xLastWakeTime;
	char obstacleDetected = 0;
	int topSonar, frontSonar, leftSonar, rightSonar, btmIR;
	char deviceBlocked[5] = {0}; // flag to indicate if we should send the reading to RPI
	
	int calibratedBtmIR = mySharpIR_Read(AN12); // get first value...
	
	xLastWakeTime = xTaskGetTickCount(); // get tick count
		
	while(1)
	{
		myMaxSonar_TopStart();
		topSonar = myMaxSonar_Read(AN11);
	
		myMaxSonar_BtmStart();
		frontSonar	= myMaxSonar_Read(AN15);
		leftSonar	= myMaxSonar_Read(AN14);
		rightSonar	= myMaxSonar_Read(AN13); 
		
		btmIR		= mySharpIR_Read(AN12);	
		
		mySharpIR_ReCalibrate(&calibratedBtmIR, btmIR); // attempt to re-calibrate btm ir sensor if stable enough..
	
		obstacleDetected = obstacleDetection(frontSonar, obstacleDetected, deviceBlocked, leftSonar, rightSonar, topSonar, calibratedBtmIR, btmIR);
		obstacleAvoidance(frontSonar, leftSonar, rightSonar, btmIR, deviceBlocked);
		
		// for debuggin to print all...
		cheatPrintAll(deviceBlocked, &obstacleDetected); 
		// remove top statement... when not debuggin..
		
		sendObstacleDetected(obstacleDetected, deviceBlocked, frontSonar, leftSonar, rightSonar, btmIR, topSonar);
	
		// reset the variables back to 0
		obstacleDetected = 0; 
		deviceBlocked[0] = deviceBlocked[1] = deviceBlocked[2] = deviceBlocked[3] = deviceBlocked[4] = 0;
		
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

		
		// Need fix receive concurrency issues.
		xTaskCreate(RPI_receiveTask, "RPI_Receive", RPI_RECEIVE_STACK, NULL, RPI_RECEIVE_PRIORITY, &t_rx);
		xTaskCreate(RPI_sendTask, "RPI_Send", RPI_SEND_STACK, NULL, RPI_SEND_PRIORITY, &t_tx);
	

		
		vTaskStartScheduler();
		
	}
}



void setDigitalInputPowerReduction()
{
	// Set when confirm which ADC pin not used for digital..
	DIDR0 = 0b11111111; // all adc pin not used for digital
	DIDR1 |= (1<<AIN1D) | (1<<AIN0D);
	DIDR2 = 0b11111111; // add adc pin not used for digital
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
		
		MOTOR_LEFT_INIT();
		MOTOR_RIGHT_INIT();
		
		
	}
	
	sei(); // enable interrupts..
}


void obstacleSend(char deviceBlocked, int reading)
{
	obstacleData queueData;
	
	
	
	if(deviceBlocked)
	{
		itoa(reading, queueData.data, 10); // convert to ascii
		
		queueData.deviceID = deviceBlocked;
		
		xQueueSendToBack(queueObstacleData, &queueData, portMAX_DELAY); // send data to queueData
	}
}



// Queue the obstacle to send..
void sendObstacleDetected(char obstacleDetected, char * deviceBlocked, int frontSonar, int leftSonar, int rightSonar, int btmIR, int topSonar)
{
	//if(obstacleDetected > 0)
	{
		
		xQueueSendToBack(queueObstacleNumber,  &obstacleDetected, portMAX_DELAY); // send obstacle...
		
		obstacleSend(deviceBlocked[FRONT_DEVICE], frontSonar);

		obstacleSend(deviceBlocked[LEFT_DEVICE], leftSonar);

		obstacleSend(deviceBlocked[RIGHT_DEVICE], rightSonar);

		obstacleSend(deviceBlocked[BTM_DEVICE], btmIR);
		
		obstacleSend(deviceBlocked[TOP_DEVICE], topSonar);
		
	}
}





//// Tasks flash LEDs at Pins 12 and 13 at 1Hz and 2Hz respectively.
void task1(void *p)
{
		DDRB |= (1 <<DDB6 ); // led
		
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