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
#define TOP_DEVICE							4

// ASCII to Identify the sensors...
#define TOP_SONAR_ID						84
#define FRONT_SONAR_ID						70
#define LEFT_SONAR_ID						76
#define RIGHT_SONAR_ID						82
#define BTM_SONAR_ID						66


// Obstacle detection range...
#define FRONT_OBSTACLE_DISTANCE				70
#define SIDE_OBSTACLE_DISTANCE				30
#define LEFT_OBSTACLE_DISTANCE				30 // same as right although position difference because... detecting infront of user.
#define RIGHT_OBSTACLE_DISTANCE				30
#define LEFT_TOO_NEAR						38
#define RIGHT_TOO_NEAR						20
#define STAIRS_OFFSET						12
#define INDISTINGUISHABLE_RANGE				8 


void cheatPrintAll(char* deviceBlocked, char *obstacleDetected);
char detectStairs(int calibratedBtmIR, int btmIR);
void obstacleAvoidance(int frontSonar, int leftSonar, int rightSonar, int btmIR, char * deviceBlocked);
char obstacleDetection(int frontSonar, char obstacleDetected, char * deviceBlocked, int leftSonar, int rightSonar, int topSonar, int calibratedBtmIR, int btmIR);





#endif /* MYOBSTACLEALGO_H_ */