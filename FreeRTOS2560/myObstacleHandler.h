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
#define HIGH_DEVICE							5


// ASCII to Identify the sensors...
#define TOP_SONAR_ID						84
#define FRONT_SONAR_ID						70
#define LEFT_SONAR_ID						76
#define RIGHT_SONAR_ID						82
#define BTM_SONAR_ID						66
#define HIGH_SONAR_ID						72


// Obstacle detection range...
#define FRONT_OBSTACLE_DISTANCE				50
#define SIDE_OBSTACLE_DISTANCE				30
#define LEFT_OBSTACLE_DISTANCE				30 
#define RIGHT_OBSTACLE_DISTANCE				30
#define TOP_OBSTACLE_DISTANCE				55

#define HIGH_OBSTACLE_DISTANCE				50

#define LEFT_TOO_NEAR						30
#define RIGHT_TOO_NEAR						24
#define STAIRS_OFFSET						8


#define LEFT_POSITIONAL_OFFSET				20
#define INDISTINGUISHABLE_RANGE				7

#define STAIRS_TOP_LIMIT_H					110 // 80 to 110
#define STAIRS_TOP_LIMIT_L					80	


/* Optimal Values for demo #1 (w/0 stairs)
#define FRONT_OBSTACLE_DISTANCE				50
#define SIDE_OBSTACLE_DISTANCE				30
#define LEFT_OBSTACLE_DISTANCE				30
#define RIGHT_OBSTACLE_DISTANCE				30
#define TOP_OBSTACLE_DISTANCE				55
#define LEFT_TOO_NEAR						30
#define RIGHT_TOO_NEAR						24
#define STAIRS_OFFSET						8

#define LEFT_POSITIONAL_OFFSET				20
#define INDISTINGUISHABLE_RANGE				7
*/
typedef struct
{
	unsigned char deviceID;
	char data[4];
	
} obstacleStruct;

void cheatPrintAll(char* deviceBlocked, char *obstacleDetected);
char detectStairs(int calibratedBtmIR, int btmIR);
void obstacleAvoidance(int frontSonar, int topSonar, int leftSonar, int rightSonar, int btmIR,  int topIR ,char const * const deviceBlocked);
char obstacleDetection(int frontSonar, char obstacleDetected, char * deviceBlocked, int leftSonar, int rightSonar, int topSonar, int calibratedBtmIR, int btmIR, int topIR);

char possibleStairs(int frontSonar, int topSonar);




#endif /* MYOBSTACLEALGO_H_ */