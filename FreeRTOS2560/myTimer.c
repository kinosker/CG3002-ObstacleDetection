/*
 * myTimer.c
 *
 * Created: 9/26/2015 9:44:11 PM
 *  Author: tienlong
 */ 

#include <myTimer.h>

SemaphoreHandle_t semaDelayMicro, semaDelayMicro2;
volatile unsigned char expectedTick = IMPOSSIBLE_RANGE; // max tick is 250... refer to RTOSConfig (timer compare...)
volatile unsigned char expectedTick2 = IMPOSSIBLE_RANGE; // wont reach this at delayMicroCheck, wont give sema at init()...									   
											   
TaskHandle_t * timerTask;					   // hold the task for suspension and resume..



// timer task should be the highest priority....
// set taskHandle...
void MyTimer_Init(TaskHandle_t *task)
{
	timerTask = task;
	vTaskSuspend(*timerTask); // suspend first not using delay micro
	semaDelayMicro = xSemaphoreCreateBinary();
	semaDelayMicro2 = xSemaphoreCreateBinary();
}

// Return timer 0 value
unsigned char readTimer()
{
	return TCNT0;
}

// Dont use for > 1ms...
// Max error of around + 3 microseconds...
// careful when usin in more than 1 task.. need handle (future implementation?)
void delayMicro(int delay)
{
	// NOTE : Tick here refer to timer0 tick not FreeRtos tick (1ms)
	unsigned char currentTick = readTimer();


	delay /= MICROSECONDS_PER_TICK; // convert delay into ticks..
	expectedTick = currentTick + delay + 1; // add 1 tick for positive error...	
	
	vTaskResume( *timerTask ); // resume delayMicroCheck..
	xSemaphoreTake(semaDelayMicro, MAX_SEMA_WAIT);	// delay for the micro here... safety mech: max wait for 2 ms... cannot be more than tht..
}

// duplicate of delayMicro....
void delayMicro2(int delay)
{
	unsigned char currentTick = readTimer();

	delay /= MICROSECONDS_PER_TICK; // convert delay into ticks..
	expectedTick2 = currentTick + delay + 1; // add 1 tick for positive error...
	vTaskResume( *timerTask ); // resume delayMicroCheck..
	xSemaphoreTake(semaDelayMicro2, MAX_SEMA_WAIT);	// delay for the micro here... safety mech: max wait for 2 ms... cannot be more than tht..
}

// Semaphore on sale if u match lucky number ...
void delayMicroCheck()
{
	unsigned char currentTick = readTimer();
	
	if( currentTick >= expectedTick)
	{
		expectedTick = IMPOSSIBLE_RANGE; // set back to impossible range.
		xSemaphoreGive(semaDelayMicro); // give the semaphore to resume...	
	}
	
	
	if( currentTick >= expectedTick2)
	{
		expectedTick2 = IMPOSSIBLE_RANGE; // set back to impossible range.
		xSemaphoreGive(semaDelayMicro2); // give the semaphore to resume...
	}
	
	if(expectedTick == IMPOSSIBLE_RANGE && expectedTick2 == IMPOSSIBLE_RANGE)
	{
		vTaskSuspend(*timerTask); // suspend when both is done..
	}
}