/*
 * myTimer.h
 *
 * Created: 9/26/2015 9:44:37 PM
 *  Author: tienlong
 */ 


#ifndef MYTIMER_H_
#define MYTIMER_H_


#define MAX_TICKS 250
#define MICROSECONDS_PER_TICK 4
#define IMPOSSIBLE_RANGE 5000
#define MAX_SEMA_WAIT 2 // wait only 2ms

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>



// Initialize my timer 
void myTimer_Init(TaskHandle_t *task);

// Return timer 0 value
unsigned char myTimer_Read();

// Dont use for > 1ms...
// Max error of around + 3 microseconds...
// careful when usin in more than 1 task.. need handle (future implementation?)
void myTimer_DelayMicro(int delay);
void myTimer_DelayMicro2(int delay);

// check if the delayMicro have lapsed..
void myTimer_DelayChecker();


#endif /* MYTIMER_H_ */