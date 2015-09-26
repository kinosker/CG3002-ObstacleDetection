/*
 * myTimer.c
 *
 * Created: 9/26/2015 9:44:11 PM
 *  Author: tienlong
 */ 

#include <myTimer.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

SemaphoreHandle_t semaDelayMicro;
unsigned char expectedTick = IMPOSSIBLE_RANGE; // max tick is 250... refer to RTOSConfig (timer compare...)
											   // wont reach this at delayMicroCheck, wont give sema at init()...
TaskHandle_t * timerTask;					   // hold the task for suspension and resume..

// timer task should be the highest priority....
// set taskHandle...
void MyTimer_Init(TaskHandle_t *task)
{
	timerTask = task;
	vTaskSuspend(*timerTask); // suspend first not using delay micro
	semaDelayMicro = xSemaphoreCreateBinary();
}

// Return timer 0 value
inline unsigned char readTimer()
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
	xSemaphoreTake(semaDelayMicro, MAX_SEMA_WAIT);	// delay for the micro here... max wait for 2 ms... cannot be more than tht..
}

void delayMicroCheck()
{
	if(expectedTick >= readTimer())
	{
		xSemaphoreGive(semaDelayMicro); // give the semaphore to resume...	
		vTaskSuspend(NULL); // suspend again...
	}
}