#ifndef __WALL_FOLLOW
#define __WALL_FOLLOW
#include "sensors.h"
#include "../userthread.hpp"
#include "../global.hpp"
#include "docker_main.h"
#define WF_LEFT_PROX_HIGH ProxyDistance2uint16_t(4) //2500 //3000 close to wall
#define WF_LEFT_PROX_LOW  ProxyDistance2uint16_t(5) //1200 //1000 away from wall

#define WF_FRONT_PROX_HIGH ProxyDistance2uint16_t(3) //2500 //3000
#define WF_FRONT_PROX_LOW  ProxyDistance2uint16_t(3.5) //1200 //1000

using namespace amiro;

#define WF_FRONT_SENSOR_ON_ROTATE_RATE  1

#define WF_FRONT_SENSOR_OFF_SPEED_LOW       1 //
#define WF_FRONT_SENSOR_OFF_RATE_LOW        1 //
#define WF_FRONT_SENSOR_OFF_SPEED_HIGH      3 //5 
#define WF_FRONT_SENSOR_OFF_RATE_HIGH       3 //5 


#define WF_FRONT_SENSOR_ON_SPEED_LOW        1 //
#define WF_FRONT_SENSOR_ON_RATE_LOW         1 //
#define WF_FRONT_SENSOR_ON_SPEED_HIGH       2 //5 
#define WF_FRONT_SENSOR_ON_RATE_HIGH        2 //5 

extern int WF_SpeedHigh;
extern int WF_SpeedLow;
extern int WF_RotationRateHigh;
extern int WF_RotationRateLow;

typedef enum{
   WF_CODE_NONE = 0,
   WF_CODE_WALL_AWAY,  
   WF_CODE_WALL_FAR,    
   WF_CODE_WALL_SAFE,   
   WF_CODE_WALL_NEAR,   
   WF_CODE_DOCK_LINE_FOUND,     /* Sensor identified black dock indication line */
   WF_CODE_DOCK_LINE_NOT_FOUND, /* Sensor couldn't find black dock indication line*/
   WF_CODE_SENSOR_ON,
   WF_CODE_SENSOR_OFF
}wall_follow_codes_t;

typedef wall_follow_codes_t (*fp_wall_follow_t) (); 

typedef struct{
    fp_wall_follow_t state_current;
    wall_follow_codes_t state_code;
    fp_wall_follow_t state_next;
}sm_wall_follow_node_t;

typedef struct{
   wall_follow_codes_t left_top;      /*Left Top Sensor */
   wall_follow_codes_t left_bottom;   /*Left Bottom Sensor */
   
}sm_sub_left_wall_t;

wall_follow_codes_t get_state_code(wall_follow_codes_t wall_state);

wall_follow_codes_t WF_Fn_Start();
wall_follow_codes_t WF_Fn_CheckFloor();
wall_follow_codes_t WF_Fn_CheckFrontRight();
wall_follow_codes_t WF_Fn_CheckFrontLeft();

wall_follow_codes_t WF_Fn_CheckLeftTop();
wall_follow_codes_t WF_Fn_CheckLeftBottom_1();
wall_follow_codes_t WF_Fn_CheckLeftBottom_2();

wall_follow_codes_t WF_Fn_RotateClock_Slow();
wall_follow_codes_t WF_Fn_RotateClock_Fast();
wall_follow_codes_t WF_Fn_Rotate_CClock_Slow();
wall_follow_codes_t WF_Fn_Rotate_CClock_Fast();
wall_follow_codes_t WF_Fn_DriveForward_Slow();
wall_follow_codes_t WF_Fn_DriveForward_Fast();

wall_follow_codes_t WF_Fn_FrontWallRotateClock_1();
wall_follow_codes_t WF_Fn_FrontWallRotateClock_2();
wall_follow_codes_t WF_Fn_DockRobot();
wall_follow_codes_t WF_Fn_SearchWall();


fp_wall_follow_t wf_next_state(fp_wall_follow_t current_state, wall_follow_codes_t state_code);

extern wall_follow_codes_t GetWallState(ProxSensorLocation_t prox_location);
extern void GetWallStateAll(wall_follow_codes_t *wall_status);
extern docker_main_codes_t WallFollow(UserThread *thread);
#endif
