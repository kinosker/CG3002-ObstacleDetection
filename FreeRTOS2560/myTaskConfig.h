/*
* myTask.h
*
* Created: 9/24/2015 6:42:00 AM
*  Author: tienlong
*/


#ifndef MYTASK_H_
#define MYTASK_H_

#define USART_0_PRIORITY					( tskIDLE_PRIORITY + 2 )
#define USART_1_PRIORITY					( tskIDLE_PRIORITY + 3 )
#define BLINK_2_PRIORITY					( tskIDLE_PRIORITY + 1 )
#define BLINK_1_PRIORITY					( tskIDLE_PRIORITY + 1 )
#define MAXSONAR_PRIORITY					( tskIDLE_PRIORITY + 2 )
#define RPI_SEND_PRIORITY					( tskIDLE_PRIORITY + 2 )
#define RPI_RECEIVE_PRIORITY				( tskIDLE_PRIORITY + 3 )

#define MAXSONAR_STACK					(configMINIMAL_STACK_SIZE + 200)
#define RPI_SEND_STACK					(configMINIMAL_STACK_SIZE + 50)
#define RPI_RECEIVE_STACK				(configMINIMAL_STACK_SIZE + 50)

#define USART_0_STACK					(configMINIMAL_STACK_SIZE + 50)
#define USART_1_STACK					(configMINIMAL_STACK_SIZE + 50)
#define BLINK_2_STACK					(configMINIMAL_STACK_SIZE + 20)
#define BLINK_1_STACK					(configMINIMAL_STACK_SIZE + 20)



#endif /* MYTASK_H_ */