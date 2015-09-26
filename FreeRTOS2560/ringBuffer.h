/*
 * ringBuffer.h
 *
 * Created: 9/24/2015 10:23:40 AM
 *  Author: tienlong
 */ 


#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <FreeRTOS.h>
#include <task.h>

typedef struct
{
	unsigned char *rBuffer;
	unsigned char rSize; // how big
	unsigned char rLength; // how many stored..
	unsigned char rIndex; // current..
} ringBuffer;



// Initialize ring buffer, with the buffer provided
void ringBufferInit(ringBuffer *ring, unsigned char* buffer, unsigned char size);

// Pre Condition => check buffer is not empty before poping
// Return current index element
unsigned char ringBufferPop(ringBuffer *ring);

// Push to the buffer => ignore if buffer is full
// Good to check if buffer is full.
void ringBufferPush( ringBuffer *ring, unsigned char element );

// clear ring buffer
void ringBufferClear(ringBuffer *ring);

// return 1 if not empty.
// return 0 if empty.
char ringBufferNotEmpty(ringBuffer *ring);

// return 1 if full.
// return 0 if not full.
char ringBufferFull(ringBuffer *ring);









#endif /* RINGBUFFER_H_ */