#include <amiro/Color.h>
#include "../userthread.hpp"
#include <stdint.h>
#include "wall_follow.h"
//#include "../global.hpp"
#include "led.h"
#include "sensors.h"
#include "motors.h"
#include "search_wall.h"

using namespace amiro;

search_wall_code_t  sw_wall[8];

sm_search_wall_node_t sm_sw_transition_table[] = {
    { SW_Fn_Start,                  SW_CODE_NONE,       SW_Fn_CheckFrontLeft        },
    /*set 1*/
    { SW_Fn_CheckFrontLeft,         SW_CODE_WALL_AWAY,  SW_Fn_CheckFrontRight_1     },
    { SW_Fn_CheckFrontLeft,         SW_CODE_WALL_FAR,   SW_Fn_CheckFrontRight_2     },
    { SW_Fn_CheckFrontLeft,         SW_CODE_WALL_SAFE,  SW_Fn_CheckFrontRight_3     },
    { SW_Fn_CheckFrontLeft,         SW_CODE_WALL_NEAR,  SW_Fn_CheckFrontRight_3     },
    /*set 2*/
    { SW_Fn_CheckFrontRight_1,      SW_CODE_WALL_AWAY,  SW_Fn_DriveForward_Fast     },
    { SW_Fn_CheckFrontRight_1,      SW_CODE_WALL_SAFE,  SW_Fn_RotateClockwise_Slow  },
    { SW_Fn_CheckFrontRight_1,      SW_CODE_WALL_NEAR,  SW_Fn_RotateClockwise_Slow  },
    { SW_Fn_CheckFrontRight_1,      SW_CODE_WALL_FAR,   SW_Fn_DriveForward_Slow     },
    /*set 3*/
    { SW_Fn_CheckFrontRight_2,      SW_CODE_WALL_AWAY,  SW_Fn_DriveForward_Slow     },
    { SW_Fn_CheckFrontRight_2,      SW_CODE_WALL_FAR,   SW_Fn_DriveForward_Slow     },
    { SW_Fn_CheckFrontRight_2,      SW_CODE_WALL_SAFE,  SW_Fn_RotateClockwise_Slow  },
    { SW_Fn_CheckFrontRight_2,      SW_CODE_WALL_NEAR,  SW_Fn_RotateClockwise_Slow  },
    /*set 4*/
    { SW_Fn_CheckFrontRight_3,      SW_CODE_WALL_AWAY,  SW_Fn_RotateClockwise_Slow  },
    { SW_Fn_CheckFrontRight_3,      SW_CODE_WALL_FAR,   SW_Fn_RotateClockwise_Slow  },
    { SW_Fn_CheckFrontRight_3,      SW_CODE_WALL_SAFE,  SW_Fn_RotateClockwise_Slow  },
    { SW_Fn_CheckFrontRight_3,      SW_CODE_WALL_NEAR,  SW_Fn_RotateClockwise_Fast  },
    /*set 5*/
    { SW_Fn_DriveForward_Fast,      SW_CODE_NONE,       SW_Fn_CheckLeftTop          },
    { SW_Fn_DriveForward_Slow,      SW_CODE_NONE,       SW_Fn_CheckLeftTop          },
    /*set 6*/
    { SW_Fn_RotateClockwise_Fast,   SW_CODE_NONE,       SW_Fn_CheckLeftTop          },
    { SW_Fn_RotateClockwise_Slow,   SW_CODE_NONE,       SW_Fn_CheckLeftTop          },
    /*set 7*/
    { SW_Fn_CheckLeftTop,           SW_CODE_WALL_AWAY,  SW_Fn_CheckLeftBottom       }, 
    { SW_Fn_CheckLeftTop,           SW_CODE_WALL_FAR,   SW_Fn_CheckLeftBottom       }, 
    { SW_Fn_CheckLeftTop,           SW_CODE_WALL_SAFE,  SW_Fn_FollowWall            }, 
    { SW_Fn_CheckLeftTop,           SW_CODE_WALL_NEAR,  SW_Fn_FollowWall            }, 
    /*set 8*/
    { SW_Fn_CheckLeftBottom,        SW_CODE_WALL_NEAR,  SW_Fn_FollowWall            }, 
    { SW_Fn_CheckLeftBottom,        SW_CODE_WALL_SAFE,  SW_Fn_FollowWall            }, 
    { SW_Fn_CheckLeftBottom,        SW_CODE_WALL_AWAY,  SW_Fn_CheckFrontLeft        }, 
    { SW_Fn_CheckLeftBottom,        SW_CODE_WALL_FAR,   SW_Fn_CheckFrontLeft        } 
};


search_wall_code_t SW_GetWallState(ProxSensorLocation_t prox_location)
{
   uint16_t value  = ProximitySensorValue(prox_location);
   Color led_color;
   search_wall_code_t wall_status;

   if(value == 0){
        wall_status =  SW_CODE_WALL_AWAY;
        led_color = LED_COLOR_BLACK;
   }else if(value > SEARCH_WALL_PROX_HIGH){
        wall_status =  SW_CODE_WALL_NEAR;
        led_color = LED_COLOR_PINK;
   }else if(value < SEARCH_WALL_PROX_LOW){
        wall_status = SW_CODE_WALL_FAR;
        led_color = LED_COLOR_BLACK;
        //led_color = LED_COLOR_GREY;
   }else{
        wall_status = SW_CODE_WALL_SAFE;
        led_color = LED_COLOR_YELLOW;
   }

  
   LedOn(GetLedLocation(prox_location), led_color);

   return wall_status;
}



void SW_GetWallStateAll(search_wall_code_t *wall_status)
{
   for(int i=0; i<8; i++){
         wall_status[i] = SW_GetWallState((ProxSensorLocation_t)i);
   }
}

fp_search_wall_t sw_get_next_state(fp_search_wall_t current_state, search_wall_code_t state_code)
{
    int state_entries = sizeof(sm_sw_transition_table) / sizeof(sm_search_wall_node_t);
    for(int i = 0; i<state_entries; i++){
        if((sm_sw_transition_table[i].current_state == current_state) && 
           (sm_sw_transition_table[i].state_code == state_code)){
                return sm_sw_transition_table[i].next_state;
        }
    }
    return NULL;
}



docker_main_codes_t SearchWall(UserThread *thread)
{
    search_wall_code_t state_code = SW_CODE_NONE;
    fp_search_wall_t current_state = SW_Fn_Start, next_state = NULL;
    LedOnAllHold(thread, LED_ALERT_STATE_CHANGE);
    while(1){
        SW_GetWallStateAll(sw_wall);
        
        /*Execute State*/
        state_code = current_state();

        if((current_state == SW_Fn_FollowWall)){
            //chprintf((BaseSequentialStream*) &SD1, "Search Wall State Completed\n");
            LedOnAllHold(thread, LED_ALERT_STATE_SUCCESS);
            return DOCK_MAIN_CODE_SEARCH_WALL_SUCCESS;
        }

        /*Find Next State*/
        next_state = sw_get_next_state(current_state, state_code);

        if(next_state == NULL){
            LedOnAllHold(thread, LED_ALERT_ERROR);
            return DOCK_MAIN_CODE_SEARCH_WALL_FAILURE;
        }

        current_state = next_state;

        
        thread->sleep(MS2ST(500));
    }
    
    return DOCK_MAIN_CODE_SEARCH_WALL_SUCCESS;
}

search_wall_code_t SW_Fn_Start()
{
    return SW_CODE_NONE;
}

search_wall_code_t SW_Fn_CheckFrontLeft()
{
    return sw_wall[ProxFrontLeft];
}

search_wall_code_t SW_Fn_CheckFrontRight_1()
{
    return sw_wall[ProxFrontRight];
}


search_wall_code_t SW_Fn_CheckFrontRight_2()
{

    return sw_wall[ProxFrontRight];
}

search_wall_code_t SW_Fn_CheckFrontRight_3()
{
    return sw_wall[ProxFrontRight];
}

search_wall_code_t SW_Fn_DriveForward_Slow()
{
    RobotDriveForward(SEARCH_WALL_SPEED_LOW, 0);
    return SW_CODE_NONE;
}

search_wall_code_t SW_Fn_DriveForward_Fast()
{
    RobotDriveForward(SEARCH_WALL_SPEED_HIGH, 0);
    return SW_CODE_NONE;
}

search_wall_code_t SW_Fn_RotateClockwise_Fast()
{
    RobotRotateClockwise(SEARCH_WALL_ROTATE_HIGH, 0);
    return SW_CODE_NONE;
}

search_wall_code_t SW_Fn_RotateClockwise_Slow()
{
    RobotRotateClockwise(SEARCH_WALL_ROTATE_LOW, 0);
    return SW_CODE_NONE;
}

search_wall_code_t SW_Fn_CheckLeftTop()
{
    return sw_wall[ProxLeftTop];
}

search_wall_code_t SW_Fn_CheckLeftBottom()
{
    return sw_wall[ProxLeftBottom];
}

search_wall_code_t SW_Fn_FollowWall()
{
    RobotStop();
    return SW_CODE_NONE;
}



