#ifndef __DOCKER_H
#define __DOCKER_H

#include "sensors.h"
#include "../userthread.hpp"
#include "../global.hpp"
#include "docker_main.h"

#define DOCKER_PROX_HIGH ProxyDistance2uint16_t(1.5)   //2200
#define DOCKER_PROX_LOW  ProxyDistance2uint16_t(2.5)   //1000
using namespace amiro;

#define DOCKER_SPEED_LOW       1 //

#define DOCKER_ROTATE_LOW      1 //
#define DOCKER_ROTATION_LIMIT  5

#define DOCKER_FINAL_WALL_CLOSE         62000  //50000 
#define DOCKER_ROBOT_AWAY_FROM_DOCK     ProxyDistance2uint16_t(1.5)  //1

#define DOCK_RETRY_LIMIT 10
extern int dock_retry_ctr;
typedef enum{
    DOCKER_CODE_NONE,
    DOCKER_CODE_WALL_FAR,
    DOCKER_CODE_WALL_NEAR,
    DOCKER_CODE_WALL_AWAY,
    DOCKER_CODE_WALL_SAFE,
    DOCKER_CODE_LINE_ON,   /* ON when floor sensor detects dark line */
    DOCKER_CODE_LINE_OFF,
    DOCKER_CODE_SENSOR_ON,
    DOCKER_CODE_SENSOR_OFF,
    DOCKER_CODE_TRUE,
    DOCKER_CODE_FALSE, 
    DOCKER_CODE_THRESHOLD_REACHED, 
    DOCKER_CODE_THRESHOLD_NOT_REACHED, 
    DOCKER_CODE_DOCK_PROPER,
    DOCKER_CODE_DOCK_IMPROPER,
    DOCKER_CODE_RETRIES_EXCEEDED
}docker_codes_t;

typedef docker_codes_t (*fp_docker_t) ();

typedef struct {
    fp_docker_t current_state;
    docker_codes_t state_code;
    fp_docker_t next_state;
}sm_docker_nodes_t;

/* Phase1: Move the front of robot to the charging point */
docker_codes_t Docker1_Start();   
docker_codes_t Docker1_CheckFloorSensor_1();  /* Either one of Front Floor Sensor */   
docker_codes_t Docker1_CheckFloorSensor_2();  /* Either one of Front Floor Sensor */   
docker_codes_t Docker1_CheckFloorSensor_3();  /* Either one of Front Floor Sensor */   
docker_codes_t Docker1_CheckFrontLeftSensor();
docker_codes_t Docker1_CheckFrontRightSensor();
docker_codes_t Docker1_RotateCClock();
docker_codes_t Docker1_DriveForward();  
docker_codes_t Docker1_FollowWall(); /* Failure State */ 
docker_codes_t Docker1_Phase2(); /* Failure State */ 


/* Phase2: Move Back of the robot towards charging point */
docker_codes_t Docker2_Start();
docker_codes_t Docker2_CheckFrontFloorSensorOn_Either();
docker_codes_t Docker2_CheckFrontFloorSensorOff_Both();
docker_codes_t Docker2_RotateCClockwise_1();
docker_codes_t Docker2_RotateCClockwise_2();
docker_codes_t Docker2_Phase3();

/* Phase3: Dock Robot to Charging Point */
docker_codes_t Docker3_Start();
docker_codes_t Docker3_DockRobot();
docker_codes_t Docker3_VerifyDock();
docker_codes_t Docker3_MoveAwayFromDock();
docker_codes_t Docker3_DockSuccess();
docker_codes_t Docker3_DockFailed();

extern bool check_front_floor_sensor();
extern void MoveForwardAwayFromDock(UserThread *thread);
extern void MoveBackwardTowardsDock();
extern void MoveAlternateZigZag();
extern void MoveAlternateZigZag_1();
extern void MoveAlternateZigZag_2();

extern docker_main_codes_t DockRobot(UserThread *thread);
#endif
