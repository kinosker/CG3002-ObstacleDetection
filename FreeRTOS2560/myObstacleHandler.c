/*
 * myObstacleAlgo.c
 *
 * Created: 10/13/2015 3:27:56 PM
 *  Author: tienlong
 */ 
#include <myObstacleHandler.h>
#include <FreeRTOS.h>
#include <myMotor.h>



char detectStairs(int calibratedBtmIR, int btmIR)
{
	if(btmIR > calibratedBtmIR + STAIRS_OFFSET || btmIR < calibratedBtmIR - STAIRS_OFFSET)
	{
		return 1; // stairs found
	}
	else
	{
		return 0; // no stairs
		
	}
}

void obstacleAvoidance(int frontSonar, int leftSonar, int rightSonar, int btmIR, int calibratedBtmIR)
{
	if(frontSonar < FRONT_OBSTACLE_DISTANCE)
	{
		if(leftSonar < SIDE_OBSTACLE_DISTANCE && rightSonar < SIDE_OBSTACLE_DISTANCE)
		{
			// dead end... block on 3 side...
			MOTOR_LEFT_START();
			MOTOR_RIGHT_START();
		}
		else if( (leftSonar+10) > rightSonar)
		{
			MOTOR_RIGHT_STOP();
			MOTOR_LEFT_START();
		}
		else if (rightSonar > (leftSonar+10))
		{
			MOTOR_LEFT_STOP();
			MOTOR_RIGHT_START();
		}
		
	}
	else if (rightSonar < SIDE_OBSTACLE_DISTANCE && leftSonar > SIDE_OBSTACLE_DISTANCE)
	{
		// too close to right
		MOTOR_RIGHT_STOP();
		MOTOR_LEFT_START();
	}
	else if (leftSonar < SIDE_OBSTACLE_DISTANCE && rightSonar > SIDE_OBSTACLE_DISTANCE)
	{
		// too close to left
		MOTOR_RIGHT_START();
		MOTOR_LEFT_STOP();
	}
	else if (detectStairs(calibratedBtmIR, btmIR))
	{
		// stairs detection
		MOTOR_LEFT_START();
		MOTOR_RIGHT_START();
	}
	else
	{
		// narrow path or no obstacle infront.
		MOTOR_RIGHT_STOP();
		MOTOR_LEFT_STOP();
	}
}

// return number of obstacle detected...
// implicitly return the device to send.
char obstacleDetection(int frontSonar, char obstacleDetected, char * deviceBlocked, int leftSonar, int rightSonar, int topSonar)
{
	// Commented out when debuggin.

	//	if(frontSonar < FRONT_OBSTACLE_DISTANCE)
	{
		obstacleDetected ++;
		deviceBlocked[FRONT_DEVICE] = FRONT_SONAR_ID;
	}
	//	if (leftSonar < SIDE_OBSTACLE_DISTANCE)
	{
		obstacleDetected ++;
		deviceBlocked[LEFT_DEVICE] = LEFT_SONAR_ID;
	}
	//	if (rightSonar < SIDE_OBSTACLE_DISTANCE)
	{
		obstacleDetected++;
		deviceBlocked[RIGHT_DEVICE] = RIGHT_SONAR_ID;
	}
	//	if (detectStairs(calibratedBtmIR, btmIR))
	{
		obstacleDetected++;
		deviceBlocked[BTM_DEVICE] = BTM_SONAR_ID;
	}
	// if (...)
	{
		
		obstacleDetected++;
		deviceBlocked[TOP_DEVICE] = TOP_SONAR_ID;
	}
	
	return obstacleDetected;
}

