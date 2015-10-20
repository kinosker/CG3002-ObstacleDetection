/*
 * mySharpIR.h
 *
 * Created: 10/9/2015 2:04:55 PM
 *  Author: tienlong
 */ 


#ifndef MYSHARPIR_H_
#define MYSHARPIR_H_

#define IR_MAX_VALUE 999
#define CALIBRATE_RANGE 5 
#define CALIBRATE_COUNT 200 

int mySharpIR_Read(char analogChannel);
void mySharpIR_ReCalibrate(int* calibratedReading, int reading);
char checkWithinRange(int reading, int * checkReading, const char range);

#endif /* MYSHARPIR_H_ */