#ifndef __DOCKER_MAIN_H
#define __DOCKER_MAIN_H
#include "../userthread.hpp"
#include "../global.hpp"
#include <stdint.h>
using namespace amiro;

extern bool dock_success;

/*
#define MS2ST(msec)                                                         \
  ((systime_t)(((((uint32_t)(msec)) * ((uint32_t)CH_FREQUENCY) - 1UL) /     \
                1000UL) + 1UL))
*/
extern void docker_main(UserThread *thread);

#define BATTERY_STATUS_CHECK_INTERVAL 1//in minutes

typedef enum{
    DOCK_MAIN_CODE_NONE,
    DOCK_MAIN_CODE_SEARCH_WALL_SUCCESS,
    DOCK_MAIN_CODE_SEARCH_WALL_FAILURE,
    DOCK_MAIN_CODE_FOLLOW_WALL_SUCCESS,
    DOCK_MAIN_CODE_FOLLOW_WALL_FAILURE,
    DOCK_MAIN_CODE_DOCK_ROBOT_SUCCESS,
    DOCK_MAIN_CODE_DOCK_ROBOT_FAILURE,
}docker_main_codes_t;

typedef docker_main_codes_t (*fp_docker_main_t) (UserThread *thread);

typedef struct{
    fp_docker_main_t current_state;
    docker_main_codes_t state_code;
    fp_docker_main_t next_state;
}sm_docker_main_nodes_t;

/* Does a full rotation and take the max and min of rear magnetometer reading*/
docker_main_codes_t RobotRearMagMaxMin(UserThread *thread, int32_t *pmax, int32_t *pmin);

extern docker_main_codes_t DockerMain_Start(UserThread *thread);
extern int32_t mag_max, mag_min;
extern docker_main_codes_t DockerMain_Success(UserThread *thread);
extern docker_main_codes_t DockerMain_Failure(UserThread *thread);

#endif
