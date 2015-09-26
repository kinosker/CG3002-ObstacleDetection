/*
 * myTimer.h
 *
 * Created: 9/26/2015 9:44:37 PM
 *  Author: tienlong
 */ 


#ifndef MYTIMER_H_
#define MYTIMER_H_


#define MICROSECONDS_PER_TICK 4
#define IMPOSSIBLE_RANGE 254
#define MAX_SEMA_WAIT 2 // wait only 2ms


// Initialize my timer 
void MyTimer_Init(TaskHandle_t *task);

// Return timer 0 value
inline unsigned char readTimer();

// Dont use for > 1ms...
// Max error of around + 3 microseconds...
// careful when usin in more than 1 task.. need handle (future implementation?)
void delayMicro(int delay);

// check if the delayMicro have lapsed..
void delayMicroCheck();


#endif /* MYTIMER_H_ */