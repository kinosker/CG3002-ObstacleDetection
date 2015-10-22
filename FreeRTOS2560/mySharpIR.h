/*
 * mySharpIR.h
 *
 * Created: 10/9/2015 2:04:55 PM
 *  Author: tienlong
 */ 


#ifndef MYSHARPIR_H_
#define MYSHARPIR_H_

#define IR_MAX_VALUE 999
#define IR_MIN_VALUE 0
#define CALIBRATE_HIGH_THRESHOLD 45
#define CALIBRATE_LOW_THRESHOLD 25
#define CALIBRATE_RANGE 5 
#define CALIBRATE_SAMPLE_RATE 5
#define CALIBRATE_SAMPLE_REQUIRE 34 // calibrate approx every 5 sec of stable reading 

int mySharpIR_Read(char analogChannel);
void mySharpIR_ReCalibrate(int* calibratedReading, int reading);
char checkOutOfRange(int reading, int * checkReading, const char range);

#endif /* MYSHARPIR_H_ */