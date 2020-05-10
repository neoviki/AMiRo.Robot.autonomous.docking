#ifndef __DOCKER_H
#define __DOCKER_H

#include "sensors.h"
#include "../userthread.hpp"
#include "../global.hpp"

#define DOCKER_PROX_HIGH 3000
#define DOCKER_PROX_LOW  1000
using namespace amiro;

#define DOCKER_SPEED_LOW       1 //
#define DOCKER_SPEED_HIGH      4 //5 

#define DOCKER_ROTATE_LOW      1 //
#define DOCKER_ROTATE_HIGH     3 

typedef enum{
    DOCKER_CODE_NONE,
    DOCKER_CODE_WALL_FAR,
    DOCKER_CODE_WALL_NEAR,
    DOCKER_CODE_WALL_AWAY,
    DOCKER_CODE_WALL_SAFE,
    DOCKER_CODE_LINE_ON,   /* ON when floor sensor detects dark line */
    DOCKER_CODE_LINE_OFF,
    DOCKER_CODE_SENSOR_ON,
    DOCKER_CODE_SENSOR_OFF
}docker_codes_t;

typedef docker_codes_t (*fp_docker_t) ();

typedef struct {
    fp_docker_t current_state;
    docker_codes_t state_code;
    fp_docker_t next_state;
}sm_docker_nodes_t;

/* Move the front of robot to the charging point */
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


extern void DockRobot(UserThread *thread);
#endif
