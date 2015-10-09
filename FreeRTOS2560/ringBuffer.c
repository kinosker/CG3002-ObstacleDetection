/*
 * ringBuffer.c
 *
 * Created: 9/24/2015 7:42:42 AM
 *  Author: tienlong
 */ 


#include <ringBuffer.h>


void ringBufferInit(ringBuffer *ring, unsigned char* buffer, unsigned char size)
{
	ring->rBuffer = buffer;
	ring->rSize = size;
	ring->rIndex = 0;
	ring->rLength = 0;	 
}

unsigned char ringBufferPeek(ringBuffer *ring)
{
	   return ring->rBuffer[ ring->rIndex ];	
}

// int?
unsigned char ringBufferPop(ringBuffer *ring)
{
    unsigned char element;
	
            element = ring->rBuffer[ ring->rIndex ];
            ring->rIndex ++;
            if ( ring->rIndex >= ring->rSize )
            {
                ring->rIndex -= ring->rSize;
            }
            ring->rLength --;
    
    return element;
}



void ringBufferPush( ringBuffer *ring, unsigned char element )
{
    
        if ( ring->rLength < ring->rSize )
        {
            ring->rBuffer[ ( ring->rIndex + ring->rLength ) % ring->rSize ] = element;
            ring->rLength++;
        }
    
    // If buffer is full, ignore the push()
}



// return 1 if full
// return 0 if not full
char ringBufferFull(ringBuffer *ring)
{
   return (ring->rSize == ring->rLength);
}

// return 1 if not empty
// return 0 if empty
char ringBufferNotEmpty(ringBuffer *ring)
{
	return (ring->rLength > 0);
}


void ringBufferClear(ringBuffer *ring)
{
    ring->rLength = 0;
}






