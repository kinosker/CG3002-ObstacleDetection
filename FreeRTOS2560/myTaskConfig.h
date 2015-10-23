/*
* myTask.h
*
* Created: 9/24/2015 6:42:00 AM
*  Author: tienlong
*/


#ifndef MYTASK_H_
#define MYTASK_H_

#define QUEUE_SIZE							5
#define SONAR_NUM							5


//#define USART_0_PRIORITY					( tskIDLE_PRIORITY + 2 )
//#define USART_1_PRIORITY					( tskIDLE_PRIORITY + 3 )
//#define BLINK_2_PRIORITY					( tskIDLE_PRIORITY + 1 )
//#define BLINK_1_PRIORITY					( tskIDLE_PRIORITY + 1 )

#define MAXSONAR_PRIORITY					( tskIDLE_PRIORITY + 3 )
#define RPI_SEND_PRIORITY					( tskIDLE_PRIORITY + 2 )
#define RPI_RECEIVE_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define MY_TIMER_PRIORITY					( tskIDLE_PRIORITY + 4 ) // Highest priority... will be suspended most of the time...



#define MAXSONAR_STACK					(configMINIMAL_STACK_SIZE + 280)
#define RPI_SEND_STACK					(configMINIMAL_STACK_SIZE + 50)
#define RPI_RECEIVE_STACK				(configMINIMAL_STACK_SIZE + 50)
#define MY_TIMER_STACK					(configMINIMAL_STACK_SIZE + 20)

//#define USART_0_STACK					(configMINIMAL_STACK_SIZE + 50)
//#define USART_1_STACK					(configMINIMAL_STACK_SIZE + 50)
//#define BLINK_2_STACK					(configMINIMAL_STACK_SIZE + 20)
//#define BLINK_1_STACK					(configMINIMAL_STACK_SIZE)



#endif /* MYTASK_H_ */