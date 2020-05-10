/*  AMiRo sensors wrapper 
 *
 *              Author: Viki (a) Vignesh Natarajan (vikiworks.io)
 */

#ifndef __MOTORS_H
#define __MOTORS_H
#define SPEED_OFFSET 2000000 
//#define SPEED_OFFSET 1000000 


typedef enum {
    WheelLeft = 0, 
    WheelRight = 1
}WheelLocation_t;


extern void RobotDriveForward(int speed, int distance);
extern void RobotDriveReverse(int speed, int distance);
extern void RobotRotateClockwise(int speed, int degrees);
extern void RobotRotateCClockwise(int speed, int degrees);
extern void RobotStop();
/*
void RobotTurnLeft(int degrees);
void RobotTurnRight(int degrees);
void RobotRotateCClockwise(int degrees);

void RobotMotorSpin(int left_motor_speed, int right_motor_speed);
*/
#endif
