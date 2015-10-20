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
#define FRONT_OBSTACLE_DISTANCE				50
#define SIDE_OBSTACLE_DISTANCE				30
#define LEFT_OBSTACLE_DISTANCE				35 
#define RIGHT_OBSTACLE_DISTANCE				30
#define TOP_OBSTACLE_DISTANCE				55
#define LEFT_TOO_NEAR						35
#define RIGHT_TOO_NEAR						20
#define STAIRS_OFFSET						7
#define INDISTINGUISHABLE_RANGE				4 
#define STAIRS_TOP_LIMIT_H					110 // 80 to 110(150)
#define STAIRS_TOP_LIMIT_L					80	

void cheatPrintAll(char* deviceBlocked, char *obstacleDetected);
char detectStairs(int calibratedBtmIR, int btmIR);
void obstacleAvoidance(int frontSonar, int topSonar, int leftSonar, int rightSonar, int btmIR, char * deviceBlocked);
char obstacleDetection(int frontSonar, char obstacleDetected, char * deviceBlocked, int leftSonar, int rightSonar, int topSonar, int calibratedBtmIR, int btmIR);
char possibleStairs(int frontSonar, int topSonar);




#endif /* MYOBSTACLEALGO_H_ */