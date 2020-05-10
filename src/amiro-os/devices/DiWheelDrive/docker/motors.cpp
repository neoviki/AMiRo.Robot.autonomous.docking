#include "../global.hpp"
#include "motors.h"
#include "led.h"
using namespace amiro;
extern Global global;

void RobotDriveForward(int speed, int distance)
{
    (void) distance;
    /*(LEFT, RIGHT)*/
	global.motorcontrol.setTargetRPM(speed * SPEED_OFFSET, speed * SPEED_OFFSET);
}

void RobotDriveReverse(int speed, int distance)
{
    (void) distance;
    /*(LEFT, RIGHT)*/
	global.motorcontrol.setTargetRPM(-(speed * SPEED_OFFSET), -(speed * SPEED_OFFSET));
}

void RobotRotateClockwise(int speed, int degrees)
{
    //LedTurnRight();
    (void) degrees;
	global.motorcontrol.setTargetRPM((speed * SPEED_OFFSET), -(speed * SPEED_OFFSET));
}

void RobotRotateCClockwise(int speed, int degrees)
{
    //LedTurnLeft();
    (void) degrees;
	global.motorcontrol.setTargetRPM(-(speed * SPEED_OFFSET), (speed * SPEED_OFFSET));

}

void RobotStop()
{
	global.motorcontrol.setTargetRPM(0, 0);

}

