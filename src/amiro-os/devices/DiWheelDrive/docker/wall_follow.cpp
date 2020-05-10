#include <amiro/Color.h>
#include "../userthread.hpp"
#include <stdint.h>
#include "wall_follow.h"
//#include "../global.hpp"
#include "led.h"
#include "sensors.h"
#include "motors.h"

using namespace amiro;

wall_follow_codes_t wall[8];

int WF_SpeedHigh;
int WF_SpeedLow;
int WF_RotationRateHigh;
int WF_RotationRateLow;

UserThread *wf_g_thread = NULL;

sm_wall_follow_node_t sm_wf_transition_table[] = {
    { WF_Fn_Start,               WF_CODE_NONE,                  WF_Fn_CheckFloor            },
    { WF_Fn_CheckFloor,          WF_CODE_DOCK_LINE_FOUND,       WF_Fn_DockRobot             },
    { WF_Fn_CheckFloor,          WF_CODE_DOCK_LINE_NOT_FOUND,   WF_Fn_CheckFrontRight       },
    /*Set 1*/
    { WF_Fn_CheckFrontRight,     WF_CODE_SENSOR_ON,             WF_Fn_FrontWallRotateClock_1 },
    { WF_Fn_CheckFrontRight,     WF_CODE_SENSOR_OFF,            WF_Fn_CheckFrontLeft         },
    /*Set 2*/
    { WF_Fn_CheckFrontLeft,      WF_CODE_SENSOR_ON,             WF_Fn_FrontWallRotateClock_2 },
    { WF_Fn_CheckFrontLeft,      WF_CODE_SENSOR_OFF,            WF_Fn_CheckLeftTop           }, 
    /*Set 3*/
    { WF_Fn_FrontWallRotateClock_1,   WF_CODE_NONE,             WF_Fn_CheckFrontRight        }, 
    { WF_Fn_FrontWallRotateClock_2,   WF_CODE_NONE,             WF_Fn_CheckFrontLeft      }, 
    /*Set 3*/
    { WF_Fn_CheckLeftTop,        WF_CODE_WALL_AWAY,             WF_Fn_CheckLeftBottom_1   },
    { WF_Fn_CheckLeftTop,        WF_CODE_WALL_NEAR,             WF_Fn_RotateClock_Slow    },
    { WF_Fn_CheckLeftTop,        WF_CODE_WALL_FAR,              WF_Fn_Rotate_CClock_Slow  },
    { WF_Fn_CheckLeftTop,        WF_CODE_WALL_SAFE,             WF_Fn_CheckLeftBottom_2   },
    /*Set 4*/ 
    { WF_Fn_CheckLeftBottom_1,   WF_CODE_WALL_AWAY,             WF_Fn_SearchWall          },
    { WF_Fn_CheckLeftBottom_1,   WF_CODE_WALL_SAFE,             WF_Fn_Rotate_CClock_Fast  },
    { WF_Fn_CheckLeftBottom_1,   WF_CODE_WALL_FAR,              WF_Fn_Rotate_CClock_Fast  },
    { WF_Fn_CheckLeftBottom_1,   WF_CODE_WALL_NEAR,             WF_Fn_Rotate_CClock_Fast  },
    /*Set 5*/ 
    { WF_Fn_CheckLeftBottom_2,   WF_CODE_WALL_AWAY,             WF_Fn_RotateClock_Slow    },
    { WF_Fn_CheckLeftBottom_2,   WF_CODE_WALL_FAR,              WF_Fn_RotateClock_Slow    },
    { WF_Fn_CheckLeftBottom_2,   WF_CODE_WALL_NEAR,             WF_Fn_Rotate_CClock_Slow  },
    { WF_Fn_CheckLeftBottom_2,   WF_CODE_WALL_SAFE,             WF_Fn_DriveForward_Fast   },
    /*Set 6*/
    { WF_Fn_Rotate_CClock_Fast,  WF_CODE_NONE,                  WF_Fn_DriveForward_Fast   }, 
    { WF_Fn_Rotate_CClock_Slow,  WF_CODE_NONE,                  WF_Fn_DriveForward_Slow   }, 
    /*Set 7*/
    { WF_Fn_RotateClock_Fast,    WF_CODE_NONE,                  WF_Fn_DriveForward_Fast   }, 
    { WF_Fn_RotateClock_Slow,    WF_CODE_NONE,                  WF_Fn_DriveForward_Slow   }, 
    /*Set 8*/
    { WF_Fn_DriveForward_Slow,   WF_CODE_NONE,                  WF_Fn_CheckFloor          }, 
    { WF_Fn_DriveForward_Fast,   WF_CODE_NONE,                  WF_Fn_CheckFloor          } 
};
 
fp_wall_follow_t wf_next_state(fp_wall_follow_t current_state, wall_follow_codes_t state_code)
{
    int state_entries = sizeof(sm_wf_transition_table) / sizeof(sm_wall_follow_node_t);
    for(int i = 0; i<state_entries; i++){
        if((sm_wf_transition_table[i].state_current == current_state) && 
           (sm_wf_transition_table[i].state_code == state_code)){
                return sm_wf_transition_table[i].state_next;
        }
    }
    return NULL;
}


wall_follow_codes_t GetWallState(ProxSensorLocation_t prox_location)
{
   uint16_t value  = ProximitySensorValue(prox_location);
   Color led_color;
   wall_follow_codes_t wall_status;

   if(prox_location == ProxFrontRight || prox_location == ProxFrontLeft){
       if(value == 0){
           wall_status =  WF_CODE_WALL_AWAY;
           led_color = LED_COLOR_BLACK;
       }else if(value > WF_FRONT_PROX_HIGH){
           wall_status =  WF_CODE_WALL_NEAR;
           led_color = LED_COLOR_PINK;
       }else if(value < WF_FRONT_PROX_LOW){
           wall_status = WF_CODE_WALL_FAR;
           led_color = LED_COLOR_BLACK;
           //led_color = LED_COLOR_GREY;
       }else{
           wall_status = WF_CODE_WALL_SAFE;
           led_color = LED_COLOR_YELLOW;
       }
   }else{
       if(value == 0){
           wall_status =  WF_CODE_WALL_AWAY;
           led_color = LED_COLOR_BLACK;
       }else if(value > WF_LEFT_PROX_HIGH){
           wall_status =  WF_CODE_WALL_NEAR;
           led_color = LED_COLOR_PINK;
       }else if(value < WF_LEFT_PROX_LOW){
           wall_status = WF_CODE_WALL_FAR;
           led_color = LED_COLOR_BLACK;
           //led_color = LED_COLOR_GREY;
       }else{
           wall_status = WF_CODE_WALL_SAFE;
           led_color = LED_COLOR_YELLOW;
       }

   }

   LedOn(GetLedLocation(prox_location), led_color);

   return wall_status;
}



void GetWallStateAll(wall_follow_codes_t *wall_status)
{
   for(int i=0; i<8; i++){
         wall_status[i] = GetWallState((ProxSensorLocation_t)i);
   }
}

docker_main_codes_t WallFollow(UserThread *thread)
{
    wf_g_thread = thread;
    WF_SpeedLow  = WF_FRONT_SENSOR_OFF_SPEED_LOW;
    WF_SpeedHigh = WF_FRONT_SENSOR_OFF_SPEED_HIGH;
    WF_RotationRateLow = WF_FRONT_SENSOR_OFF_RATE_LOW; 
    WF_RotationRateHigh = WF_FRONT_SENSOR_OFF_RATE_HIGH;


    fp_wall_follow_t current_state = WF_Fn_Start, next_state = NULL;
    wall_follow_codes_t state_code = WF_CODE_NONE; 
    
    LedOnAllHold(thread, LED_ALERT_STATE_CHANGE);

    while(1){
        GetWallStateAll(wall);

        /*Execute State*/
        state_code = current_state();

        if((current_state == WF_Fn_SearchWall)){
            //chprintf((BaseSequentialStream*) &SD1, "Wall Follow State Completed\n");
            LedOnAllHold(thread, LED_ALERT_STATE_FAILURE);
            return DOCK_MAIN_CODE_FOLLOW_WALL_FAILURE;
        }else if((current_state == WF_Fn_DockRobot)){
            LedOnAllHold(thread, LED_ALERT_STATE_SUCCESS);
            return DOCK_MAIN_CODE_FOLLOW_WALL_SUCCESS;
        }

        /*Find Next State*/
        next_state = wf_next_state(current_state, state_code);

        if(next_state == NULL){
            RobotStop();
            LedOnAllHold(thread, LED_ALERT_ERROR);
            return DOCK_MAIN_CODE_FOLLOW_WALL_FAILURE;
        }

        current_state = next_state;
        thread->sleep(MS2ST(500));
    }
    
    return DOCK_MAIN_CODE_FOLLOW_WALL_SUCCESS;
}

wall_follow_codes_t WF_Fn_Start()
{
    return WF_CODE_NONE; 
}

wall_follow_codes_t WF_Fn_CheckFloor()
{
    if(FloorSensorValue(FloorFrontLeft) == FloorBlack){
        return WF_CODE_DOCK_LINE_FOUND;
    }else if(FloorSensorValue(FloorFrontRight) == FloorBlack){
        return WF_CODE_DOCK_LINE_FOUND;
    }

    return WF_CODE_DOCK_LINE_NOT_FOUND;
}

wall_follow_codes_t WF_Fn_CheckFrontRight()
{
    if(wall[ProxFrontRight] == WF_CODE_WALL_NEAR){
        return WF_CODE_SENSOR_ON;
    }
    return WF_CODE_SENSOR_OFF;
}

wall_follow_codes_t WF_Fn_CheckFrontLeft()
{
    if(wall[ProxFrontLeft] == WF_CODE_WALL_NEAR){
        return WF_CODE_SENSOR_ON;
    }
    return WF_CODE_SENSOR_OFF;
}

wall_follow_codes_t WF_Fn_CheckLeftTop()
{
    return  wall[ProxLeftTop];        
}

wall_follow_codes_t WF_Fn_CheckLeftBottom_1()
{
    return  wall[ProxLeftBottom];        
}

wall_follow_codes_t WF_Fn_CheckLeftBottom_2()
{
    return  wall[ProxLeftBottom];        
}

wall_follow_codes_t WF_Fn_RotateClock_Slow()
{
    RobotRotateClockwise(WF_RotationRateLow, 0);
    return WF_CODE_NONE; 
}

wall_follow_codes_t WF_Fn_RotateClock_Fast()
{
    RobotRotateClockwise(WF_RotationRateHigh, 0);
    return WF_CODE_NONE; 
}


wall_follow_codes_t WF_Fn_Rotate_CClock_Slow()
{
    RobotRotateCClockwise(WF_RotationRateLow, 0);
    return WF_CODE_NONE; 
}

wall_follow_codes_t WF_Fn_Rotate_CClock_Fast()
{
    RobotRotateCClockwise(WF_RotationRateHigh, 0);
    return WF_CODE_NONE; 
}


wall_follow_codes_t WF_Fn_DriveForward_Slow()
{
    RobotDriveForward(WF_SpeedLow, 0);
    return WF_CODE_NONE; 
}

wall_follow_codes_t WF_Fn_DriveForward_Fast()
{
    RobotDriveForward(WF_SpeedHigh, 0);
    return WF_CODE_NONE; 
}

wall_follow_codes_t WF_Fn_FrontWallRotateClock_1()
{
    RobotRotateClockwise(WF_FRONT_SENSOR_ON_ROTATE_RATE, 0);
    return WF_CODE_NONE;
}

wall_follow_codes_t WF_Fn_FrontWallRotateClock_2()
{
    RobotRotateClockwise(WF_FRONT_SENSOR_ON_ROTATE_RATE, 0);
    return WF_CODE_NONE;
}

wall_follow_codes_t WF_Fn_DockRobot()
{
    RobotStop();
    return WF_CODE_NONE; 
}

wall_follow_codes_t WF_Fn_SearchWall()
{
    RobotStop();
    return WF_CODE_NONE; 
}
