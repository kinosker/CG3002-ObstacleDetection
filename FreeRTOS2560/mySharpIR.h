/*
 * mySharpIR.h
 *
 * Created: 10/9/2015 2:04:55 PM
 *  Author: tienlong
 */ 
#include <stdint.h>


#ifndef MYSHARPIR_H_
#define MYSHARPIR_H_

#define IR_MAX_VALUE 999
#define IR_MIN_VALUE 0


#define CALIBRATE_TOP_HIGH_THRESHOLD 45
#define CALIBRATE_TOP_LOW_THRESHOLD 25

#define CALIBRATE_BTM_HIGH_THRESHOLD 45
#define CALIBRATE_BTM_LOW_THRESHOLD 25
#define CALIBRATE_RANGE 5 
#define CALIBRATE_SAMPLE_RATE 5
#define CALIBRATE_SAMPLE_REQUIRE 34 // calibrate approx every 5 sec of stable reading 

int mySharpIR_Read(char analogChannel);

void mySharpIR_ReCalibrate(int* calibratedReading, int * referenceReading, uint8_t * sampleCount, int reading, const int highThreshold, const int lowThreshold);

char checkOutOfRange(int reading, int * checkReading, const char range);

#endif /* MYSHARPIR_H_ */