/*
 * myMotor.h
 *
 * Created: 10/13/2015 3:44:06 PM
 *  Author: tienlong
 */ 


#ifndef MYMOTOR_H_
#define MYMOTOR_H_


#define MOTOR_LEFT_START()			PORTE |= ( 1 << PE4 )	// start motor Left (Digital Pin 2)
#define MOTOR_LEFT_STOP()			PORTE &= ~( 1 << PE4 ) // stop motor Left

#define MOTOR_FRONT_START()			PORTE |= ( 1 << PE3 )	// start motor center (Digital Pin 5)
#define MOTOR_FRONT_STOP()			PORTE &= ~( 1 << PE3 ) // stop motor center


#define MOTOR_RIGHT_START()			PORTH |= ( 1 << PH4 )	// start motor Right (Digital Pin 7)
#define MOTOR_RIGHT_STOP()			PORTH &= ~( 1 << PH4 ) // stop motor Right

#define MOTOR_LEFT_INIT()			DDRE |= (1 << DDE4) | (1<< DDE3); // E4 => digitial pin 2 (LEFT), E3 => digital pin 5(Center) 
#define MOTOR_RIGHT_INIT()			DDRH |= (1 << DDH4) ; // H4 => digital pin 7 (RIGHT)
	





#endif /* MYMOTOR_H_ */