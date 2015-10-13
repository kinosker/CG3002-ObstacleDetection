/*
 * mySharpIR.h
 *
 * Created: 10/9/2015 2:04:55 PM
 *  Author: tienlong
 */ 


#ifndef MYSHARPIR_H_
#define MYSHARPIR_H_



int mySharpIR_Read(char analogChannel);
void mySharpIR_ReCalibrate(int* calibratedReading, int reading);
char checkWithinRange(int reading, int * checkReading, const char range);

#endif /* MYSHARPIR_H_ */