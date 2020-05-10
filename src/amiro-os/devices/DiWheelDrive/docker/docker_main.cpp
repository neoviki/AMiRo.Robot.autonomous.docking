#include "docker_main.h"
#include "wall_follow.h"
#include "motors.h"
#include "docker.h"
#include "led.h"
#include "sensors.h"
#include "../userthread.hpp"
#include "../global.hpp"
#include <stdint.h>
#include "search_wall.h"
#include "verify_docking.h"
#include "sensors.h"
#include "battery.h"
#include <Types.h>
using namespace amiro;
extern Global global;

bool dock_success = false;
int32_t mag_max = 0, mag_min = -2;

sm_docker_main_nodes_t sm_docker_main_table [] = {
    { DockerMain_Start, DOCK_MAIN_CODE_NONE,                 SearchWall          },
    { SearchWall,       DOCK_MAIN_CODE_SEARCH_WALL_SUCCESS,  WallFollow          },
    { SearchWall,       DOCK_MAIN_CODE_SEARCH_WALL_FAILURE,  DockerMain_Failure  },
    { WallFollow,       DOCK_MAIN_CODE_FOLLOW_WALL_SUCCESS,  DockRobot           },
    { WallFollow,       DOCK_MAIN_CODE_FOLLOW_WALL_FAILURE,  SearchWall          },
    { DockRobot,        DOCK_MAIN_CODE_DOCK_ROBOT_SUCCESS,   DockerMain_Success  },
    { DockRobot,        DOCK_MAIN_CODE_DOCK_ROBOT_FAILURE,   WallFollow          }
};

fp_docker_main_t dm_get_next_state(fp_docker_main_t current_state, docker_main_codes_t state_code)
{
    int state_entries = sizeof(sm_docker_main_table) / sizeof(sm_docker_main_nodes_t);
    for(int i = 0; i<state_entries; i++){
        if((sm_docker_main_table[i].current_state == current_state) && (sm_docker_main_table[i].state_code == state_code)){
                return sm_docker_main_table[i].next_state;
        }
    }
    return NULL;
}


void docker_main(UserThread *thread)
{

    LedOffAll();

	if(GetBatteryPercentage() > 20){
		thread->sleep(MS2ST(1000*60*BATTERY_STATUS_CHECK_INTERVAL));
		return;
	}

	/*return if previous dock was successful*/
    if(dock_success){
        return;
    }
    
   
    LedOnAllHold(thread, LED_ALERT_ENTER_MAIN); 
    LedOnAllHold(thread, LED_COLOR_BLACK); 


    //RobotRearMagMaxMin(thread, &mag_max, &mag_min);
    
    fp_docker_main_t current_state = DockerMain_Start, next_state = NULL;
    docker_main_codes_t state_code = DOCK_MAIN_CODE_NONE;

    while(1){
           /* Execute Current State */
           state_code =  current_state(thread);

           if((current_state == DockerMain_Success)){
                break;
           }
           if((current_state == DockerMain_Failure)){
                dock_success = false;
                return;
           }
           
           next_state = dm_get_next_state(current_state, state_code);
           if(next_state == NULL){
                dock_success = false;
                LedOnAllHold(thread, LED_ALERT_ERROR);
                return;
           }

           current_state = next_state;
           thread->sleep(MS2ST(500));
    }

    while(1){
        if(GetBatteryPercentage() < 100){
            thread->sleep(MS2ST(1000*60*BATTERY_STATUS_CHECK_INTERVAL));
        }else{
             /*Charging Done*/
             LedOnAllHold(thread, LED_ALERT_SUCCESS);
             MoveForwardAwayFromDock(thread);
             dock_success = true;
             return;
        }
    }

}

docker_main_codes_t DockerMain_Start(UserThread *thread)
{
    return DOCK_MAIN_CODE_NONE; 
}

docker_main_codes_t DockerMain_Success(UserThread *thread)
{
    RobotStop();
    dock_success = true;
    LedOnAllHold(thread, LED_ALERT_SUCCESS);
    LedOnAllHold(thread, LED_COLOR_WHITE);
    LedOnAllHold(thread, LED_ALERT_SUCCESS);
    LedOnAllHold(thread, LED_COLOR_WHITE);
    LedOnAllHold(thread, LED_ALERT_SUCCESS);
    LedOnAllHold(thread, LED_COLOR_WHITE);
    return DOCK_MAIN_CODE_NONE; 
}

docker_main_codes_t DockerMain_Failure(UserThread *thread)
{
    RobotStop();
    LedOnAllHold(thread, LED_ALERT_FAILURE);
    LedOnAllHold(thread, LED_ALERT_STATE_FAILURE);
    LedOnAllHold(thread, LED_ALERT_FAILURE);
    LedOnAllHold(thread, LED_ALERT_STATE_FAILURE);
    LedOnAllHold(thread, LED_ALERT_FAILURE);
    LedOnAllHold(thread, LED_ALERT_STATE_FAILURE);
    LedOnAllHold(thread, LED_ALERT_FAILURE);
    LedOnAllHold(thread, LED_ALERT_STATE_FAILURE);
    LedOnAllHold(thread, LED_ALERT_FAILURE);
    return DOCK_MAIN_CODE_NONE; 
}

docker_main_codes_t RobotRearMagMaxMin(UserThread *thread, int32_t *pmax, int32_t *pmin)
{
    int32_t max =  0;
    int32_t min = -1;
    int32_t reading;
    LedOnAllHold(thread, LED_ALERT_STATE_CHANGE); 
    for (int i = 0; i<36; i++){
        RobotRotateClockwise(1, 0);
        reading = MagnetometerReading(MAG_X);
        if(reading > max){
            max = reading;
        }

        if(reading < min){
            min = reading;
        }
        thread->sleep(MS2ST(1000));
    }
    RobotDriveForward(0, 0);

    LedOnAllHold(thread, LED_ALERT_STATE_SUCCESS); 

    *pmax = max;
    *pmin = min;
    
    chprintf((BaseSequentialStream*) &SD1, "[ MAX %d ] [ MIN : %d]\n", max, min);
    return DOCK_MAIN_CODE_NONE;
}
