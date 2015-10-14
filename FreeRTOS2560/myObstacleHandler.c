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


// Priority => BTM, Front, Side
void obstacleAvoidance(int frontSonar, int leftSonar, int rightSonar, int btmIR, char * deviceBlocked)
{
	
	if(deviceBlocked[BTM_DEVICE])
	{
		// stairs detected
			MOTOR_LEFT_START();
			MOTOR_RIGHT_START();
	}
	else if(deviceBlocked[FRONT_DEVICE])
	{
		// front sensor detected
		if(!deviceBlocked[LEFT_DEVICE] && !deviceBlocked[RIGHT_DEVICE])
		{
			// both not blocked... so select any side... (left safer to turn - see product)
				MOTOR_LEFT_START();
				MOTOR_RIGHT_STOP();
		}
		if(deviceBlocked[LEFT_DEVICE] && !(deviceBlocked[RIGHT_DEVICE]))
		{
			// left is blocked but not right
			if((rightSonar - leftSonar) > INDISTINGUISHABLE_RANGE) // if there's enough difference, prompt the user to move..
			{
				MOTOR_LEFT_STOP();
				MOTOR_RIGHT_START();	
			}
		}
		else if (deviceBlocked[RIGHT_DEVICE] && !(deviceBlocked[LEFT_DEVICE]))
		{
			// right is blocked but not left..
			if((leftSonar - rightSonar) > INDISTINGUISHABLE_RANGE) // if there's enough different, prompt the user to move...
			{
				MOTOR_LEFT_START();
				MOTOR_RIGHT_STOP();
			}
		}
		else if(deviceBlocked[RIGHT_DEVICE] && deviceBlocked[LEFT_DEVICE])
		{
			
			// both blocked...dead end
			//MOTOR_LEFT_STOP(); 
			//MOTOR_RIGHT_STOP();	
		}
		
	}
	else if (leftSonar < LEFT_TOO_NEAR)
	{
		// front able to walk, too near to wall or obstacle...
			MOTOR_LEFT_STOP();
			MOTOR_RIGHT_START();		
	}
	else if (rightSonar < RIGHT_TOO_NEAR)
	{
		// front able to walk, too near to wall or obstacle...
			MOTOR_LEFT_START();
			MOTOR_RIGHT_STOP();
	}
	else
	{
			MOTOR_LEFT_STOP();
			MOTOR_RIGHT_STOP();
	}
}


void cheatPrintAll(char* deviceBlocked, char *obstacleDetected)
{
	*obstacleDetected = 5;
	deviceBlocked[FRONT_DEVICE] = FRONT_SONAR_ID;
	deviceBlocked[LEFT_DEVICE] = LEFT_SONAR_ID;
	deviceBlocked[RIGHT_DEVICE] = RIGHT_SONAR_ID;
	deviceBlocked[BTM_DEVICE] = BTM_SONAR_ID;
	deviceBlocked[TOP_DEVICE] = TOP_SONAR_ID;
}


// return number of obstacle detected...
// implicitly return the device to send.
char obstacleDetection(int frontSonar, char obstacleDetected, char * deviceBlocked, int leftSonar, int rightSonar, int topSonar, int calibratedBtmIR, int btmIR)
{
	// Commented out when debuggin..

	if(frontSonar < FRONT_OBSTACLE_DISTANCE)
	{
		obstacleDetected ++;
		deviceBlocked[FRONT_DEVICE] = FRONT_SONAR_ID;
	}
	if (leftSonar < LEFT_OBSTACLE_DISTANCE)
	{
		obstacleDetected ++;
		deviceBlocked[LEFT_DEVICE] = LEFT_SONAR_ID;
	}
	if (rightSonar < RIGHT_OBSTACLE_DISTANCE)
	{
		obstacleDetected++;
		deviceBlocked[RIGHT_DEVICE] = RIGHT_SONAR_ID;
	}
	if (detectStairs(calibratedBtmIR, btmIR))
	{
		obstacleDetected++;
		deviceBlocked[BTM_DEVICE] = BTM_SONAR_ID;
	}
	// if (??)
	//{
	//	obstacleDetected++;
	//	deviceBlocked[TOP_DEVICE] = TOP_SONAR_ID;
	//}
	
	return obstacleDetected;
}




//char obstacleDetectionBackup(int frontSonar, char obstacleDetected, char * deviceBlocked, int leftSonar, int rightSonar, int topSonar, int calibratedBtmIR, int btmIR)
//{
	//// Commented out when debuggin..
//
	////if(frontSonar < FRONT_OBSTACLE_DISTANCE)
	//{
		//obstacleDetected ++;
		//deviceBlocked[FRONT_DEVICE] = FRONT_SONAR_ID;
	//}
	////if (leftSonar < LEFT_OBSTACLE_DISTANCE)
	//{
		//obstacleDetected ++;
		//deviceBlocked[LEFT_DEVICE] = LEFT_SONAR_ID;
	//}
	////if (rightSonar < RIGHT_OBSTACLE_DISTANCE)
	//{
		//obstacleDetected++;
		//deviceBlocked[RIGHT_DEVICE] = RIGHT_SONAR_ID;
	//}
	////if (detectStairs(calibratedBtmIR, btmIR))
	//{
		//obstacleDetected++;
		//deviceBlocked[BTM_DEVICE] = BTM_SONAR_ID;
	//}
	//// if (??)
	//{
	//	obstacleDetected++;
	//	deviceBlocked[TOP_DEVICE] = TOP_SONAR_ID;
	//}
	//
	//return obstacleDetected;
//}

//void obstacleAvoidanceBackup(int frontSonar, int leftSonar, int rightSonar, int btmIR, int calibratedBtmIR)
//{
	//if(frontSonar < FRONT_OBSTACLE_DISTANCE)
	//{
		//if(leftSonar < SIDE_OBSTACLE_DISTANCE && rightSonar < SIDE_OBSTACLE_DISTANCE)
		//{
			//// dead end... block on 3 side...
			//MOTOR_LEFT_START();
			//MOTOR_RIGHT_START();
		//}
		//else if( (leftSonar+10) > rightSonar)
		//{
			//MOTOR_RIGHT_STOP();
			//MOTOR_LEFT_START();
		//}
		//else if (rightSonar > (leftSonar+10))
		//{
			//MOTOR_LEFT_STOP();
			//MOTOR_RIGHT_START();
		//}
		//
	//}
	//else if (rightSonar < RIGHT_OBSTACLE_DISTANCE && leftSonar > LEFT_OBSTACLE_DISTANCE)
	//{
		//if(leftSonar - rightSonar < 7)
		//{
			//return; // to close to determine.. narrow path.
		//}
		//
		//// too close to right
		//MOTOR_RIGHT_STOP();
		//MOTOR_LEFT_START();
	//}
	//else if (leftSonar < LEFT_OBSTACLE_DISTANCE && rightSonar > RIGHT_OBSTACLE_DISTANCE)
	//{
		//
		//if(rightSonar - leftSonar < 7)
		//{
			//return; // to close to determine.. narrow path.
		//}
		//// too close to left
		//MOTOR_RIGHT_START();
		//MOTOR_LEFT_STOP();
	//}
	//else if (detectStairs(calibratedBtmIR, btmIR))
	//{
		//// stairs detection
		//MOTOR_LEFT_START();
		//MOTOR_RIGHT_START();
	//}
	//else
	//{
		//// narrow path or no obstacle infront.
		//MOTOR_RIGHT_STOP();
		//MOTOR_LEFT_STOP();
	//}
	//
//}