/*
 *  Author: Vignesh Natarajan (a) Viki (vikiworks.io)
 */

#ifndef __SEARCH_WALL_H
#define __SEARCH_WALL_H

#include "../userthread.hpp"
#include "../global.hpp"
#include "docker_main.h"
#define SEARCH_WALL_PROX_HIGH ProxyDistance2uint16_t(2)    /* 2 cm */    //3000
#define SEARCH_WALL_PROX_LOW  ProxyDistance2uint16_t(3.5)      /* 3.5 cm */  //1000

using namespace amiro;

#define SEARCH_WALL_SPEED_LOW       1 //
#define SEARCH_WALL_SPEED_HIGH      6 //4 

#define SEARCH_WALL_ROTATE_LOW      1 //
#define SEARCH_WALL_ROTATE_HIGH     7 //


typedef enum {
    SW_CODE_NONE = 0,
    SW_CODE_WALL_AWAY,
    SW_CODE_WALL_FAR,
    SW_CODE_WALL_SAFE,
    SW_CODE_WALL_NEAR,
}search_wall_code_t;

typedef search_wall_code_t (*fp_search_wall_t) ();

search_wall_code_t SW_Fn_Start();
search_wall_code_t SW_Fn_CheckFrontLeft();
search_wall_code_t SW_Fn_CheckFrontRight_1();
search_wall_code_t SW_Fn_CheckFrontRight_2();
search_wall_code_t SW_Fn_CheckFrontRight_3();
search_wall_code_t SW_Fn_DriveForward_Slow();
search_wall_code_t SW_Fn_DriveForward_Fast();
search_wall_code_t SW_Fn_RotateClockwise_Fast();
search_wall_code_t SW_Fn_RotateClockwise_Slow();
search_wall_code_t SW_Fn_CheckLeftTop();
search_wall_code_t SW_Fn_CheckLeftBottom();
search_wall_code_t SW_Fn_FollowWall();

typedef struct{
    fp_search_wall_t    current_state;
    search_wall_code_t  state_code; 
    fp_search_wall_t    next_state;
}sm_search_wall_node_t;

fp_search_wall_t sw_get_next_state(fp_search_wall_t current_state, search_wall_code_t state_code);

extern docker_main_codes_t SearchWall(UserThread *thread);

#endif
