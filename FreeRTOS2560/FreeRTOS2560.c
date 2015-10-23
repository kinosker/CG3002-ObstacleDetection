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
//#include <myHcSonar.h>
#include <mySharpIR.h>
#include <myTimer.h>
#include <mySharpIR.h>
#include <myObstacleHandler.h>
#include <myMotor.h>
#include <queue.h>


void task1(void *p);
void task2(void *p);
void task3(void *p);
void task4(void *p);
void init();

void obstacleSend(char deviceBlocked, int reading);
void sendObstacleDetected(char obstacleDetected, char * deviceBlocked, int frontSonar, int leftSonar, int rightSonar, int btmIR, int topSonar, int topIR);

void myItoa(int input, char * buffer);

xQueueHandle queueObstacleData;
xQueueHandle queueObstacleNumber;


// receive task should peek........
// if is handshake ACK give semaphore...
//
void RPI_receiveTask(void *p)
{
	static char receiveState = 0;
	
	unsigned char data;
	
	while(1)
	{
		data = myUSART_receiveUSART1();
		
		if (myUSART_receiveHandShakeAck(data))
		{
			myUSART_completeHandShake();
		}
		else if (myUSART_receiveMessageACK(data))
		{
			// ack received..
		}
		else if (receiveState == RX_IDLE_STATE)
		{
			if(myUSART_receiveHandShakeStart(data))
			{
				// receive SYNC
				myUSART_transmitUSART1_c(HANDSHAKE_ACK);
				receiveState = RX_HS_START_STATE;	
			}
				
		}
		else if(receiveState == RX_HS_START_STATE)
		{
			if(myUSART_receiveHandShakeFin(data))
			{
				// Handshake completed, state change.
				receiveState = RX_HS_FIN_STATE;
			}
			else
			{
				// Invalid received..
				receiveState = RX_IDLE_STATE;
			}
		}

		else if(receiveState == RX_HS_FIN_STATE)
		{
			// accepting data...
			if(data == MESSAGE_RST)
			{
				receiveState = RX_IDLE_STATE;
			}
			else
			{
				// data = ????
			}	
			
		}
	}

}

	
void RPI_sendTask(void *p)
{
	
	
	obstacleStruct dataToSend;
	char obstacleDetected;
	char handShakeState = 0;

	
	while(1)
	{
	//	myUSART_transmitUSART0("Init\n");
		handShakeState = myUSART_startHandShake(); // 1 = success, 0 = timeout
	//	myUSART_transmitUSART0("Hs Done\n");
			
		xQueueReceive(queueObstacleNumber, &(obstacleDetected), portMAX_DELAY);
		
		
			while(obstacleDetected--)
			{

				xQueueReceive(queueObstacleData, &(dataToSend), portMAX_DELAY);
			
				myUSART_transmitUSART0_c(dataToSend.deviceID);
				myUSART_transmitUSART0(":");
				myUSART_transmitUSART0(dataToSend.data);
				myUSART_transmitUSART0(",");
			
			
				if(handShakeState)
				{
					myUSART_transmitUSART1_c(obstacleDetected+'0');
					myUSART_transmitUSART1_c('\n');
					myUSART_transmitUSART1_c(dataToSend.deviceID);
					
					myUSART_transmitUSART1(dataToSend.data);
					myUSART_transmitUSART1_c('\n');
				}
			
			}	
			myUSART_transmitUSART0_c('\n');
		
		
	}
}


void Sonar_Task(void *p)
{
	TickType_t xLastWakeTime;
	char obstacleDetected = 0;
	int btmReference = 0;
	uint8_t btmSampleCount = 0;
	int topSonar, frontSonar, leftSonar, rightSonar, btmIR, topIR;
	int topSonarSample[SONAR_SAMPLE_SIZE] = {0}, frontSonarSample[SONAR_SAMPLE_SIZE] = {0}, leftSonarSample[SONAR_SAMPLE_SIZE] = {0}, rightSonarSample[SONAR_SAMPLE_SIZE] = {0};
	char deviceBlocked[SENSOR_NUM] = {0}; // flag to indicate if we should send the reading to RPI
	
	
	int calibratedBtmIR = mySharpIR_Read(AN12); // get first value...
	
	xLastWakeTime = xTaskGetTickCount(); // get tick count
		
	while(1)
	{
					
		myMaxSonar_BtmStart();
		frontSonar	= myMaxSonar_getFilteredReading(myMaxSonar_Read(AN15), frontSonarSample);
		leftSonar	= myMaxSonar_getFilteredReading(myMaxSonar_Read(AN14), leftSonarSample);
		rightSonar	= myMaxSonar_getFilteredReading(myMaxSonar_Read(AN13), rightSonarSample); 	
		
		myMaxSonar_TopStart();
		topSonar = myMaxSonar_getFilteredReading(myMaxSonar_Read(AN11), topSonarSample);
			
			
		btmIR	= mySharpIR_Read(AN12);
		mySharpIR_ReCalibrate(&calibratedBtmIR, &btmReference, &btmSampleCount, btmIR, CALIBRATE_BTM_HIGH_THRESHOLD, CALIBRATE_BTM_LOW_THRESHOLD); // attempt to re-calibrate btm ir sensor if stable enough..
	
		topIR = mySharpIR_Read(AN7);
		
		
		obstacleDetected = obstacleDetection(frontSonar, obstacleDetected, deviceBlocked, leftSonar, rightSonar, topSonar, calibratedBtmIR, btmIR, topIR);
		obstacleAvoidance(frontSonar, topSonar, leftSonar, rightSonar, btmIR, topIR, deviceBlocked);

		
		// for debuggin to print all...
		//cheatPrintAll(deviceBlocked, &obstacleDetected); 
		// remove top statement... when not debuggin..
		
		
		
		sendObstacleDetected(obstacleDetected, deviceBlocked, frontSonar, leftSonar, rightSonar, btmIR, topSonar, topIR);
	
		
		vTaskDelayUntil( &xLastWakeTime, MAXSONAR_CHAIN_WAIT);  // delay 150 ms for 3 sonar chain...
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
		TaskHandle_t t_maxSonar, t_rx, t_tx, t_delay;
	
		init();
	
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
		
		//myHcSonar_Init();
		
		
		queueObstacleNumber = xQueueCreate(QUEUE_SIZE, sizeof (char)); // create queue
		queueObstacleData = xQueueCreate( (QUEUE_SIZE*SENSOR_NUM), sizeof (obstacleStruct)); // create queue
		
		MOTOR_LEFT_INIT();
		MOTOR_RIGHT_INIT();
		
		
	}
	
	sei(); // enable interrupts..
}


void obstacleSend(char deviceBlocked, int reading)
{	
	obstacleStruct obstacleInfo;
		
	if(deviceBlocked)
	{
		myItoa(reading, obstacleInfo.data); // convert to ascii
		
		obstacleInfo.deviceID = deviceBlocked;
		
		
		xQueueSendToBack(queueObstacleData, (void*) &obstacleInfo, portMAX_DELAY); // send data to queueData
	}
}



// Queue the obstacle to send..
void sendObstacleDetected(char obstacleDetected, char * deviceBlocked, int frontSonar, int leftSonar, int rightSonar, int btmIR, int topSonar, int topIR)
{	
	
	if(obstacleDetected > 0)
	{
		
		obstacleSend(deviceBlocked[FRONT_DEVICE], frontSonar);

		obstacleSend(deviceBlocked[LEFT_DEVICE], leftSonar);

		obstacleSend(deviceBlocked[RIGHT_DEVICE], rightSonar);

		obstacleSend(deviceBlocked[BTM_DEVICE], btmIR);
		
		obstacleSend(deviceBlocked[TOP_DEVICE], topSonar);
		
		obstacleSend(deviceBlocked[HIGH_DEVICE], topIR);
		
			//for(i =0; i < 5; i++)
			//totalObs += (deviceBlocked[i] > 0);
			//
			//if(totalObs == obstacleDetected)
			//{
				//myUSART_transmitUSART0("p\n");
				//myUSART_transmitUSART0_c(totalObs + '0');
				//myUSART_transmitUSART0("M\n");
			//}
			
		xQueueSendToBack(queueObstacleNumber,  &obstacleDetected, portMAX_DELAY); // send obstacle...
		
	}
}


// either 2 or 3 digit +ve int
void myItoa(int input, char * buffer)
{
	int temp = input;
	input /= 100; // get 100s digit
	
	if(input)
		*buffer++ = input + '0';
		
	temp = temp - input * 100; // get remainder
	
	
	input = temp / 10; // get 10s digit
	*buffer++ = input + '0';
	 
	 input = temp - input * 10; // get 1s digit
	 *buffer++ = input + '0';
	 
	 *buffer = '\0';
	 
}

//// Tasks flash LEDs at Pins 12 and 13 at 1Hz and 2Hz respectively.
//void task1(void *p)
//{
		//DDRB |= (1 <<DDB6 ); // led
		//
//while(1)
//{
//PORTB |= ( 1 << 6 );
//vTaskDelay(1000); // Delay for 500 ticks. Since each tick is 1ms,
////this delays for 500ms.
//PORTB &= ( 0 << 6 );
//vTaskDelay(1000);
//}
//}


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