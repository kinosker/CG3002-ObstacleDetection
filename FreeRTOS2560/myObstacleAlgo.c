/*
 * myObstacleAlgo.c
 *
 * Created: 10/13/2015 3:27:56 PM
 *  Author: tienlong
 */ 
#include <myObstacleAlgo.h>
#include <FreeRTOS.h>
#include <myMotor.h>



char detectStairs(int calibratedBtmIR, int btmIR)
{
	if(btmIR > calibratedBtmIR + 15 || btmIR < calibratedBtmIR - 15)
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
	if(frontSonar < 70)
	{
		if(leftSonar < 45 && rightSonar < 45)
		{
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
	else if (rightSonar < 30 && leftSonar > 30)
	{
		// narrow path
		// too close to right
		MOTOR_RIGHT_STOP();
		MOTOR_LEFT_START();
	}
	else if (leftSonar < 30 && rightSonar > 30)
	{
		// narrow path
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
char obstacleDetection(int frontSonar, char obstacleDetected, char * deviceToSend, int leftSonar, int rightSonar)
{
	// Commented out when debuggin.

	//	if(frontSonar < OBSTACLE_DISTANCE)
	{
		obstacleDetected ++;
		deviceToSend[FRONT_DEVICE] = FRONT_SONAR_ID;
	}
	//	if (leftSonar < OBSTACLE_DISTANCE)
	{
		obstacleDetected ++;
		deviceToSend[LEFT_DEVICE] = LEFT_SONAR_ID;
	}
	//	if (rightSonar < OBSTACLE_DISTANCE)
	{
		obstacleDetected++;
		deviceToSend[RIGHT_DEVICE] = RIGHT_SONAR_ID;
	}
	//	if (btmSonar < OBSTACLE_DISTANCE)
	{
		obstacleDetected++;
		deviceToSend[BTM_DEVICE] = BTM_SONAR_ID;
	}
	
	return obstacleDetected;
}

