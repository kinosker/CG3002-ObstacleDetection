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


#define MOTOR_LEFT_START()			PORTE |= ( 1 << 4 )	// start motor Left (Pin 2)
#define MOTOR_LEFT_STOP()			PORTE &= ~( 1 << 4 ) // stop motor Left

#define MOTOR_RIGHT_START()			PORTH |= ( 1 << 4 )	// start motor Right (Pin 7)
#define MOTOR_RIGHT_STOP()			PORTH &= ~( 1 << 4 ) // stop motor Right


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
			//myUSART_transmitUSART1(", ");
			
		}	
		myUSART_transmitUSART0_c('\n');
		
	}
}

char checkWithinRange(int reading, int * checkReading, const char range)
{
	return reading > (*checkReading + range) || reading < (*checkReading - range);
}

// Calibrate if initial reading, firstCheck reading and final reading is within range. ( Stable reading after ....)
// final reading = btmIR reading (i = CALIBRATE_COUNT)
// firstCheck reading = btmIR reading ( i = CALIBRATE_COUNT/2)
// initial reading  = btmIR reading (i = 0)
// Write new calibrate value if all matches..
void activeCalibration(int* calibratedReading, int reading)
{
	static const char range = 5; // put at header file later...
	static const char CALIBRATE_COUNT = 20; // put at header file later...
	static int i = 0;
	static int checkReading[2] = {0};
	
	if(checkWithinRange(reading, calibratedReading, range) && i == 0)
	{
		// if current reading and calibratedReading is within range and no checking in progess
		// skip the calibration process... not needed
		return;
	}
	
	
	if (i == 0)
	{
		checkReading[0] = reading;
	}
	else if (i == CALIBRATE_COUNT/2)
	{
		if(checkWithinRange(reading, checkReading, range))
		{
			// Out of range.. restart to find new calibration point..
			checkReading[0] = reading;
			i = 0; // reset to count...
		}
		else
		{	// within range.. need more confirmation
			checkReading[1] = reading;
		}
	}
	else if (i == CALIBRATE_COUNT)
	{
		i = 0; // reset to count..

		if(checkWithinRange(reading, checkReading, range))
		{
			// Out of range.. restart to find new calibration point..
			checkReading[0] = reading;
		}
		else
		{
			// all 3 readings within range... can calibrate as new stable.
			*calibratedReading = checkReading[0]; // btmIR is calibrated..
		}
	}
	
	i = (i+1) % CALIBRATE_COUNT;
	return -1; // failed to calibrate.
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


char detectStairs(int calibratedBtmIR, int btmIR)
{
	if(btmIR > calibratedBtmIR + 15 || btmIR < calibratedBtmIR - 15)
	{
		return 1; // stairs found
	}
	else
	{
		return 0; // no stairs
		
	}
}

void obstacleDetection(int frontSonar, int leftSonar, int rightSonar, int btmIR, int calibratedBtmIR)
{
	if(frontSonar < 70)
		{
			if(leftSonar < 45 && rightSonar < 45)
			{
				MOTOR_LEFT_START();
				MOTOR_RIGHT_START();
			}
			else if( (leftSonar+10) > rightSonar)
			{
				MOTOR_RIGHT_STOP();
				MOTOR_LEFT_START();
			}
			else if (rightSonar > (leftSonar+10))
			{
				MOTOR_LEFT_STOP();
				MOTOR_RIGHT_START();
			}
			
		}
		else if (rightSonar < 30 && leftSonar > 30)
		{
			// narrow path
			// too close to right
			MOTOR_RIGHT_STOP();
			MOTOR_LEFT_START();
		}
		else if (leftSonar < 30 && rightSonar > 30)
		{
			// narrow path
			// too close to left
			MOTOR_RIGHT_START();
			MOTOR_LEFT_STOP();
		}
		else if (detectStairs(calibratedBtmIR, btmIR))
		{
			// stairs detection
			MOTOR_LEFT_START();
			MOTOR_RIGHT_START();
		}
		else
		{
			// narrow path or no obstacle infront.
			MOTOR_RIGHT_STOP();
			MOTOR_LEFT_STOP();
		}
}

void Sonar_Task(void *p)
{
	TickType_t xLastWakeTime;
	char obstacleDetected = 0;
	int frontSonar, leftSonar, rightSonar, btmIR;
	char deviceToSend[4] = {0};
	char end= '\n';

	int calibratedBtmIR = mySharpIR_Read(AN12);
	
	xLastWakeTime = xTaskGetTickCount(); // get tick count
		
	while(1)
	{	
		myMaxSonar_Start();
		frontSonar	= myMaxSonar_Read(AN15);
		leftSonar	= myMaxSonar_Read(AN14);
		rightSonar	= myMaxSonar_Read(AN13); // for offset...
		btmIR		= mySharpIR_Read(AN12);	
	
		activeCalibration(&calibratedBtmIR, btmIR); // attempt to re-calibrate btm ir sensor if stable enough..
		obstacleDetection(frontSonar, leftSonar, rightSonar, btmIR, calibratedBtmIR);
		
		
		
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
			

			//myUSART_transmitUSART0(obstacleDetected);
			//myUSART_transmitUSART0_c(end);
			
			obstacleSend(deviceToSend[FRONT_DEVICE], frontSonar);

			obstacleSend(deviceToSend[LEFT_DEVICE], leftSonar);

			obstacleSend(deviceToSend[RIGHT_DEVICE], rightSonar);

			obstacleSend(deviceToSend[BTM_DEVICE], btmIR);

			obstacleDetected = 0; // reset back...
			
			deviceToSend[0] = deviceToSend[1] = deviceToSend[2] = deviceToSend[3] = 0;
			
		}
		
		//prevIR = btmIR;
		
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
		
		DDRE |= (1 << DDE4) ; // E4 => digitial pin 2 (LEFT)
		DDRH |= (1 << DDH4) ; // H4 => digital pin 7 (RIGHT)
		
		
	}
	
	sei(); // enable interrupts..
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