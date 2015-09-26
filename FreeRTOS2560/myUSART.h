/*
 * myUSART.h
 *
 * Created: 9/23/2015 12:47:09 AM
 *  Author: tienlong
 */ 

#ifndef MYUSART_H_
#define MYUSART_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <ringBuffer.h>
#include <FreeRTOS.h>
#include <semphr.h>


#define F_CPU 16000000UL  // freq of clk
#define USART_BAUDRATE 9600 // baud rate..
#define BAUD_PRESCALE (((F_CPU / (16UL * USART_BAUDRATE))) - 1) // macro to get prescale value (8UL if using 2x, 16 UL if using 1x)

#define RX_SIZE 32
#define TX_SIZE 32

#define HANDSHAKE_START 0x00
#define HANDSHAKE_ACK 0x55
#define HANDSHAKE_FIN 0xFF

 void USART0_Init(void);
 
 void USART1_Init(void);

 void transmitUSART0_c(unsigned char data);

 void transmitUSART1_c(unsigned char data);

 void transmitUSART1(const unsigned char* data);

 void transmitUSART0(const unsigned char* data);

 unsigned char receiveUSART1();

 unsigned char receiveUSART0();
 
 
 // -1 if fail, 0 success
 char startHandShake();
 
 char waitForHandshake();

 char receiveHandShakeAck(unsigned char message);
 
 char receiveHandShakeStart(unsigned char message);
 
 char receiveHandShakeFin(unsigned char message);
 

#endif /* MYUSART_H_ */