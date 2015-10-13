/*
 * myObstacleAlgo.h
 *
 * Created: 10/13/2015 3:28:28 PM
 *  Author: tienlong
 */ 


#ifndef MYOBSTACLEALGO_H_
#define MYOBSTACLEALGO_H_

#define FRONT_DEVICE						0
#define LEFT_DEVICE							1
#define RIGHT_DEVICE						2
#define BTM_DEVICE							3

#define FRONT_SONAR_ID						70
#define LEFT_SONAR_ID						76
#define RIGHT_SONAR_ID						82
#define BTM_SONAR_ID						66

#define OBSTACLE_DISTANCE					55


char detectStairs(int calibratedBtmIR, int btmIR);
void obstacleAvoidance(int frontSonar, int leftSonar, int rightSonar, int btmIR, int calibratedBtmIR);
char obstacleDetection(int frontSonar, char obstacleDetected, char * deviceToSend, int leftSonar, int rightSonar);
void sendObstacleDetected(char obstacleDetected, char * deviceToSend, int frontSonar, int leftSonar, int rightSonar, int btmIR);




#endif /* MYOBSTACLEALGO_H_ */