/*
 * myUSART.h
 *
 * Created: 9/23/2015 12:47:09 AM
 *  Author: tienlong
 */ 

#ifndef MYUSART_H_
#define MYUSART_H_



#define F_CPU 16000000UL  // freq of clk
#define USART_BAUDRATE 9600 // baud rate..
#define BAUD_PRESCALE (((F_CPU / (16UL * USART_BAUDRATE))) - 1) // macro to get prescale value (8UL if using 2x, 16 UL if using 1x)

#define RX_IDLE_STATE 0
#define RX_HS_START_STATE 1
#define	RX_HS_FIN_STATE 2

#define RX_SIZE 32
#define TX_SIZE 32

#define HANDSHAKE_START 22 // (SYN)
#define HANDSHAKE_ACK 5 // (ENQ)
#define HANDSHAKE_FIN 2 // (Start of text)
#define MESSAGE_ACK 6
#define MESSAGE_RST 24 // (RST)

#define TRANSMIT_TIMEOUT 500  // 500 ms
#define HANDSHAKE_TIMEOUT 500 // 500 ms

 void myUSART_USART0_Init(void);
 
 void myUSART_USART1_Init(void);

 void myUSART_transmitUSART0_c(unsigned char data);

 void myUSART_transmitUSART1_c(unsigned char data);

 void myUSART_transmitUSART1(const unsigned char* data);

 void myUSART_transmitUSART0(const unsigned char* data);

 unsigned char myUSART_receiveUSART1();

 unsigned char myUSART_receiveUSART0();
 
 
 // 0 if fail, 1 = success
 char myUSART_startHandShake();
 
 char myUSART_receiveHandShakeAck(unsigned char message);
 
 char myUSART_receiveHandShakeStart(unsigned char message);
 
 char myUSART_receiveHandShakeFin(unsigned char message);
 
void myUSART_completeHandShake();

char myUSART_receiveMessageACK(unsigned char message);

unsigned char myUSART_peekReceiveUSART1();


#endif /* MYUSART_H_ */