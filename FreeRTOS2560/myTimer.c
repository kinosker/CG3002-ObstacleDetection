/*
 * myTimer.c
 *
 * Created: 9/26/2015 9:44:11 PM
 *  Author: tienlong
 */ 

#include <myTimer.h>

SemaphoreHandle_t semaDelayMicro, semaDelayMicro2;
volatile int expectedTick = IMPOSSIBLE_RANGE; // max tick is 250... refer to RTOSConfig (timer compare...)
volatile int expectedTick2 = IMPOSSIBLE_RANGE; // wont reach this at delayMicroCheck, wont give sema at init()...									   
											   
TaskHandle_t * timerTask;					   // hold the task for suspension and resume..



// timer task should be the highest priority....
// set taskHandle...
void myTimer_Init(TaskHandle_t *task)
{
	timerTask = task;
	vTaskSuspend(*timerTask); // suspend first not using delay micro
	semaDelayMicro = xSemaphoreCreateBinary();
	semaDelayMicro2 = xSemaphoreCreateBinary();
}

// Return timer 0 value
unsigned char myTimer_Read()
{
	return TCNT0;
}

// Dont use for > 1ms...
// Max error of around + 3 microseconds...
// careful when usin in more than 1 task.. need handle (future implementation?)
void myTimer_DelayMicro(int delay)
{
	// NOTE : Tick here refer to timer0 tick not FreeRtos tick (1ms)
	unsigned char currentTick = myTimer_Read();


	delay /= MICROSECONDS_PER_TICK; // convert delay into ticks..
	expectedTick = currentTick + delay; 	
	
	vTaskResume( *timerTask ); // resume delayMicroCheck..
	xSemaphoreTake(semaDelayMicro, MAX_SEMA_WAIT);	// delay for the micro here... safety mech: max wait for 2 ms... cannot be more than tht..
}

// duplicate of delayMicro....
void myTimer_DelayMicro2(int delay)
{
	unsigned char currentTick = myTimer_Read();

	delay /= MICROSECONDS_PER_TICK; // convert delay into ticks..
	expectedTick2 = currentTick + delay; 
	vTaskResume( *timerTask ); // resume delayMicroCheck..
	xSemaphoreTake(semaDelayMicro2, MAX_SEMA_WAIT);	// delay for the micro here... safety mech: max wait for 2 ms... cannot be more than tht..
}

// Semaphore on sale if u match lucky number ...
void myTimer_DelayChecker()
{
	unsigned char currentTick = myTimer_Read();
	
	if(currentTick == 0) // overflow happened
	{
		if(expectedTick != IMPOSSIBLE_RANGE)
			expectedTick -= MAX_TICKS;

		if(expectedTick2 != IMPOSSIBLE_RANGE)
			expectedTick2 -= MAX_TICKS;
	}
	
	if( currentTick > expectedTick) // more than => 1 more tick => positive error..
	{
		expectedTick = IMPOSSIBLE_RANGE; // set back to impossible range.
		xSemaphoreGive(semaDelayMicro); // give the semaphore to resume...	
	}
	
	
	if( currentTick > expectedTick2) // more than => 1 more tick => positive error..
	{
		expectedTick2 = IMPOSSIBLE_RANGE; // set back to impossible range.
		xSemaphoreGive(semaDelayMicro2); // give the semaphore to resume...
	}
	
	if(expectedTick == IMPOSSIBLE_RANGE && expectedTick2 == IMPOSSIBLE_RANGE)
	{
		vTaskSuspend(*timerTask); // suspend when both is done..
	}
}