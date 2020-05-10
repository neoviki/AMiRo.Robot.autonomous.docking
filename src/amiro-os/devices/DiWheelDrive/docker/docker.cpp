#include "docker.h"

#include <amiro/Color.h>
#include "../userthread.hpp"
#include <stdint.h>
//#include "../global.hpp"
#include "led.h"
#include "sensors.h"
#include "motors.h"
#include "verify_docking.h"

using namespace amiro;

UserThread *g_thread;
int dock_retry_ctr;
docker_codes_t docker_wall[8];

int32_t docker_mag_max = 0; /* Maximum Magnetometer Value in negative direction */ 

docker_codes_t DockerWallState(ProxSensorLocation_t prox_location)
{
   uint16_t value  = ProximitySensorValue(prox_location);
   Color led_color;
   docker_codes_t wall_status;

   if(value == 0){
        wall_status =  DOCKER_CODE_WALL_AWAY;
        led_color = LED_COLOR_BLACK;
   }else if(value > DOCKER_PROX_HIGH){
        wall_status =  DOCKER_CODE_WALL_NEAR;
        led_color = LED_COLOR_PINK;
   }else if(value < DOCKER_PROX_LOW){
        wall_status = DOCKER_CODE_WALL_FAR;
        led_color = LED_COLOR_BLACK;
        //led_color = LED_COLOR_GREY;
   }else{
        wall_status = DOCKER_CODE_WALL_SAFE;
        led_color = LED_COLOR_YELLOW;
   }

   LedOn(GetLedLocation(prox_location), led_color);

   return wall_status;
}

void DockerWallStateAll(docker_codes_t *wall_status)
{
   for(int i=0; i<8; i++){
         wall_status[i] = DockerWallState((ProxSensorLocation_t)i);
   }
}

fp_docker_t docker_next_state(sm_docker_nodes_t *table, int rows, fp_docker_t current_state, docker_codes_t state_code)
{
   for(int i=0; i<rows; i++){
        if((table[i].current_state == current_state) && (table[i].state_code == state_code)) {
            return table[i].next_state;    
        }
   }
   return NULL;
}

docker_main_codes_t DockRobot(UserThread *thread)
{
    int rows = 0;
    g_thread = thread;
    LedOnAllHold(thread, LED_ALERT_STATE_CHANGE);
    dock_retry_ctr = 0;

    /*Move robot's front towards chargint point*/
    sm_docker_nodes_t docker1_table[] = {
    { Docker1_Start,                    DOCKER_CODE_NONE,       Docker1_CheckFloorSensor_1     },
    { Docker1_CheckFloorSensor_1,       DOCKER_CODE_LINE_ON,    Docker1_CheckFloorSensor_2     },
    { Docker1_CheckFloorSensor_1,       DOCKER_CODE_LINE_OFF,   Docker1_FollowWall             },
    { Docker1_CheckFloorSensor_2,       DOCKER_CODE_LINE_OFF,   Docker1_RotateCClock           },
    { Docker1_CheckFloorSensor_2,       DOCKER_CODE_LINE_ON,    Docker1_CheckFrontLeftSensor   },
    { Docker1_CheckFrontLeftSensor,     DOCKER_CODE_SENSOR_ON,  Docker1_CheckFloorSensor_3     },
    { Docker1_CheckFrontLeftSensor,     DOCKER_CODE_SENSOR_OFF, Docker1_CheckFrontRightSensor  },
    { Docker1_CheckFrontRightSensor,    DOCKER_CODE_SENSOR_ON,  Docker1_CheckFloorSensor_3     },
    { Docker1_CheckFrontRightSensor,    DOCKER_CODE_SENSOR_OFF, Docker1_DriveForward           },
    { Docker1_DriveForward,             DOCKER_CODE_NONE,       Docker1_CheckFloorSensor_2     },
    { Docker1_RotateCClock,             DOCKER_CODE_NONE,       Docker1_CheckFloorSensor_2     },
    { Docker1_CheckFloorSensor_3,       DOCKER_CODE_LINE_ON,    Docker1_Phase2                 },
    { Docker1_CheckFloorSensor_3,       DOCKER_CODE_LINE_OFF,   Docker1_FollowWall             }
    };

    /*Move robot's rear towards chargint point*/
    sm_docker_nodes_t docker2_table[] = {
        {Docker2_Start,                             DOCKER_CODE_NONE,       Docker2_CheckFrontFloorSensorOff_Both   },
        {Docker2_CheckFrontFloorSensorOff_Both,     DOCKER_CODE_FALSE,      Docker2_RotateCClockwise_1              },
        {Docker2_CheckFrontFloorSensorOff_Both,     DOCKER_CODE_TRUE,       Docker2_CheckFrontFloorSensorOn_Either  },
        {Docker2_CheckFrontFloorSensorOn_Either,    DOCKER_CODE_FALSE,      Docker2_RotateCClockwise_2              },
        {Docker2_CheckFrontFloorSensorOn_Either,    DOCKER_CODE_TRUE,       Docker2_Phase3                          },
        {Docker2_RotateCClockwise_1,                DOCKER_CODE_NONE,       Docker2_CheckFrontFloorSensorOff_Both   },
        {Docker2_RotateCClockwise_2,                DOCKER_CODE_NONE,       Docker2_CheckFrontFloorSensorOn_Either  }
    };

    /*Docke robot */
    sm_docker_nodes_t docker3_table[] = {
    { Docker3_Start,                   DOCKER_CODE_NONE,                   Docker3_DockRobot                  },
    { Docker3_DockRobot,               DOCKER_CODE_RETRIES_EXCEEDED,       Docker3_DockFailed                 },
    { Docker3_DockRobot,               DOCKER_CODE_NONE,                   Docker3_VerifyDock                 },
    { Docker3_VerifyDock,              DOCKER_CODE_DOCK_IMPROPER,          Docker3_MoveAwayFromDock           },
    { Docker3_VerifyDock,              DOCKER_CODE_DOCK_PROPER,            Docker3_DockSuccess                }, 
    { Docker3_MoveAwayFromDock,        DOCKER_CODE_NONE,                   Docker3_DockRobot                  } 
    };
 

    fp_docker_t current_state = Docker1_Start, next_state = NULL;
    docker_codes_t state_code = DOCKER_CODE_NONE; 
    rows = sizeof(docker1_table) / sizeof(sm_docker_nodes_t);

#if 1 //Phase 1
    LedOffAll();
    while(1){
        DockerWallStateAll(docker_wall);

        /*Execute State*/
        state_code = current_state();

        if((current_state == Docker1_FollowWall)){
            //chprintf((BaseSequentialStream*) &SD1, "Wall Follow State Completed\n");
            LedOnAllHold(thread, LED_ALERT_STATE_FAILURE);
            return DOCK_MAIN_CODE_DOCK_ROBOT_FAILURE;
        }
        
        if((current_state == Docker1_Phase2)){
            /*Proceed to next substate*/
            LedOnAllHold(thread, LED_ALERT_STATE_SUCCESS);
            break;
        }

        /*Find Next State*/
        next_state = docker_next_state(docker1_table, rows, current_state, state_code);

        if(next_state == NULL){
            RobotStop();
            LedOnAllHold(thread, LED_ALERT_ERROR);
            return DOCK_MAIN_CODE_DOCK_ROBOT_FAILURE;
        }

        current_state = next_state;
        thread->sleep(MS2ST(500));
    }
#endif //Phase 1

#if 1  //Phase 2
    LedOnAllHold(thread, LED_ALERT_STATE_CHANGE);
 
    current_state = Docker2_Start;
    next_state = NULL;
    state_code = DOCKER_CODE_NONE; 
    rows = sizeof(docker2_table) / sizeof(sm_docker_nodes_t);

    while(1){
        //LedOffAll();
        DockerWallStateAll(docker_wall);

        /*Execute State*/
        state_code = current_state();

        if((current_state == Docker2_Phase3)){
            /*Proceed to next substate*/
            LedOnAllHold(thread, LED_ALERT_STATE_SUCCESS);
            break;
        }

        /*Find Next State*/
        next_state = docker_next_state(docker2_table, rows, current_state, state_code);

        if(next_state == NULL){
            RobotStop();
            LedOnAllHold(thread, LED_ALERT_ERROR);
            return DOCK_MAIN_CODE_DOCK_ROBOT_FAILURE;
        }

        current_state = next_state;
        thread->sleep(MS2ST(500));
    }
#endif //Phase 2

#if 1  //Phase 3
    LedOnAllHold(thread, LED_ALERT_STATE_CHANGE);
 
    current_state = Docker3_Start;
    next_state = NULL;
    state_code = DOCKER_CODE_NONE; 
    rows = sizeof(docker3_table) / sizeof(sm_docker_nodes_t);

    while(1){
        //LedOffAll();
        DockerWallStateAll(docker_wall);

        /*Execute State*/
        state_code = current_state();

        if((current_state == Docker3_DockSuccess)){
            /*Proceed to next state*/
            LedOnAllHold(thread, LED_ALERT_STATE_SUCCESS);
            LedOnAllHold(thread, LED_ALERT_SUCCESS);
            LedOnAllHold(thread, LED_ALERT_SUCCESS);
            LedOnAllHold(thread, LED_ALERT_SUCCESS);
            return DOCK_MAIN_CODE_DOCK_ROBOT_SUCCESS;
        }
        
        if((current_state == Docker3_DockFailed)){
            /*Proceed to next state*/
            //LedOnAllHold(thread, LED_ALERT_STATE_FAILURE);
            return DOCK_MAIN_CODE_DOCK_ROBOT_FAILURE;
        }

        /*Find Next State*/
        next_state = docker_next_state(docker3_table, rows, current_state, state_code);

        if(next_state == NULL){
            RobotStop();
            LedOnAllHold(thread, LED_ALERT_ERROR);
            return DOCK_MAIN_CODE_DOCK_ROBOT_FAILURE;
        }

        current_state = next_state;
        thread->sleep(MS2ST(500));
    }

#endif //Phase 3
           
    return DOCK_MAIN_CODE_DOCK_ROBOT_SUCCESS;
}

#if 1 //PHASE_1 START
/*Position Both Front Floor Sensors inside black line*/
docker_codes_t Docker1_Start()
{
    return DOCKER_CODE_NONE;
}

docker_codes_t Docker1_CheckFloorSensor_1()
{
    if((FloorSensorValue(FloorFrontLeft) == FloorBlack) || 
            (FloorSensorValue(FloorFrontRight) == FloorBlack)){
        return DOCKER_CODE_LINE_ON;
    }
    return DOCKER_CODE_LINE_OFF;
}

docker_codes_t Docker1_CheckFloorSensor_2()
{
    if((FloorSensorValue(FloorFrontLeft) == FloorBlack) || 
            (FloorSensorValue(FloorFrontRight) == FloorBlack)){
        return DOCKER_CODE_LINE_ON;
    }
    return DOCKER_CODE_LINE_OFF;
}

docker_codes_t Docker1_CheckFloorSensor_3()
{
    if((FloorSensorValue(FloorFrontLeft) == FloorBlack) || 
            (FloorSensorValue(FloorFrontRight) == FloorBlack)){
        return DOCKER_CODE_LINE_ON;
    }
    return DOCKER_CODE_LINE_OFF;
}



docker_codes_t Docker1_CheckFrontLeftSensor()
{
    docker_codes_t status = DOCKER_CODE_SENSOR_OFF;
    if(docker_wall[ProxFrontLeft] == DOCKER_CODE_WALL_NEAR){// || 
            //docker_wall[ProxFrontLeft] == DOCKER_CODE_WALL_SAFE){
        status = DOCKER_CODE_SENSOR_ON;
    }
    return status;
}

docker_codes_t Docker1_CheckFrontRightSensor()
{
    docker_codes_t status = DOCKER_CODE_SENSOR_OFF;
    if(docker_wall[ProxFrontRight] == DOCKER_CODE_WALL_NEAR){// || 
            //docker_wall[ProxFrontLeft] == DOCKER_CODE_WALL_SAFE){
        status = DOCKER_CODE_SENSOR_ON;
    }
    return status;
}


docker_codes_t Docker1_RotateCClock()
{
    RobotRotateCClockwise(DOCKER_ROTATE_LOW, 0);
    return DOCKER_CODE_NONE;
}

docker_codes_t Docker1_DriveForward()
{
    RobotDriveForward(DOCKER_SPEED_LOW, 0);
    return DOCKER_CODE_NONE;
}

docker_codes_t Docker1_FollowWall()    /* FAILURE STATE */
{
    RobotStop();
    return DOCKER_CODE_NONE;
}

docker_codes_t Docker1_Phase2()
{
    RobotStop();
    return DOCKER_CODE_NONE;
}
#endif //PHASE_1 END

/*Phase2 : Move robto's rear towards charging point */
#if 1 //PHASE_2 START

docker_codes_t Docker2_Start()
{
    return DOCKER_CODE_NONE;
}

docker_codes_t Docker2_CheckFrontFloorSensorOff_Both()
{
    /*Either One is On*/
    if((FloorSensorValue(FloorFrontLeft) == FloorBlack) || 
            (FloorSensorValue(FloorFrontRight) == FloorBlack)){
        return DOCKER_CODE_FALSE;
    }
    /*Both are off*/
    return DOCKER_CODE_TRUE;
}

docker_codes_t Docker2_CheckFrontFloorSensorOn_Either()
{
    if((FloorSensorValue(FloorFrontLeft) == FloorBlack) || 
            (FloorSensorValue(FloorFrontRight) == FloorBlack)){
        return DOCKER_CODE_TRUE;
    }
    return DOCKER_CODE_FALSE;
}

docker_codes_t Docker2_RotateCClockwise_1()
{
    RobotRotateCClockwise(DOCKER_ROTATE_LOW, 0);
    return DOCKER_CODE_NONE;
}

docker_codes_t Docker2_RotateCClockwise_2()
{
    RobotRotateCClockwise(DOCKER_ROTATE_LOW, 0);
    return DOCKER_CODE_NONE;
}

docker_codes_t Docker2_Phase3()
{
   RobotStop();
   return DOCKER_CODE_NONE;
}

#endif//PHASE_2 END

#if 1 //PHASE_3 START
docker_codes_t Docker3_Start()
{
   return DOCKER_CODE_NONE;
}

docker_codes_t Docker3_VerifyDock()
{
    RobotStop();
    docker_main_codes_t status = verify_docking(g_thread);
    if( status == DOCK_MAIN_CODE_DOCK_ROBOT_SUCCESS ){
        return DOCKER_CODE_DOCK_PROPER;
    }
    
    return DOCKER_CODE_DOCK_IMPROPER;
}

docker_codes_t Docker3_DockFailed()
{
    RobotStop();
    return DOCKER_CODE_NONE; 
}

docker_codes_t Docker3_DockSuccess()
{
    RobotStop();
    return DOCKER_CODE_NONE; 
}

docker_codes_t Docker3_DockRobot()
{
    if(dock_retry_ctr >= DOCK_RETRY_LIMIT){
        return DOCKER_CODE_RETRIES_EXCEEDED; 
    }
    //MoveAlternateZigZag();
    MoveBackwardTowardsDock();
    //MoveAlternateZigZag();
    dock_retry_ctr++;
    return DOCKER_CODE_NONE;
}

docker_codes_t Docker3_MoveAwayFromDock()
{
    MoveForwardAwayFromDock(g_thread);
    return DOCKER_CODE_NONE;
}

bool check_front_floor_sensor()
{
    if((FloorSensorValue(FloorFrontLeft) == FloorBlack) || 
            (FloorSensorValue(FloorFrontRight) == FloorBlack)){
        return true;
    }
    return false;
}

void MoveForwardAwayFromDock(UserThread *thread)
{
    uint16_t back_left   = ProximitySensorValue(ProxBackLeft);
    uint16_t back_right  = ProximitySensorValue(ProxBackRight);
    bool moved_away = false;
    int ctr = 0;
    while(moved_away != true){

        back_left   = ProximitySensorValue(ProxBackLeft);
        back_right  = ProximitySensorValue(ProxBackRight);
 
        if((back_right <= DOCKER_ROBOT_AWAY_FROM_DOCK) || (back_left <= DOCKER_ROBOT_AWAY_FROM_DOCK)){
            moved_away = true;
            break;
        }else{
            moved_away = false;
        }

        ctr = 0;
        while(!check_front_floor_sensor()){
            RobotRotateClockwise(1, 0);
            g_thread->sleep(MS2ST(500));
            ctr++;
            if(ctr >= 500){
                return;
            }

        }

        if((FloorSensorValue(FloorFrontRight) != FloorBlack) && (FloorSensorValue(FloorFrontLeft) == FloorBlack)){
                RobotRotateCClockwise(1, 0);
        }else if((FloorSensorValue(FloorFrontRight) == FloorBlack) && (FloorSensorValue(FloorFrontLeft) != FloorBlack)){
                RobotRotateClockwise(1, 0);
        }else{
            RobotDriveForward(1, 0);
        }
        g_thread->sleep(MS2ST(500));
    }
        RobotStop();
}

void MoveBackwardTowardsDock()
{
    uint16_t back_left   = ProximitySensorValue(ProxBackLeft);
    uint16_t back_right  = ProximitySensorValue(ProxBackRight);
    bool moved_towards = false;
    int ctr = 0; 

    while(moved_towards != true){
        
        back_left   = ProximitySensorValue(ProxBackLeft);
        back_right  = ProximitySensorValue(ProxBackRight);
 
        if((back_right >= DOCKER_FINAL_WALL_CLOSE) || (back_left >= DOCKER_FINAL_WALL_CLOSE)){
            moved_towards = true;
            break;
        }else{
            moved_towards = false;
        }

        ctr = 0;
       
        while(!check_front_floor_sensor()){
            RobotRotateCClockwise(1, 0);
            g_thread->sleep(MS2ST(500));
            ctr++;
            if(ctr >= 500){
                return;
            }
        }
        
        static bool prev_clock = false;

        if((FloorSensorValue(FloorFrontRight) != FloorBlack) && (FloorSensorValue(FloorFrontLeft) == FloorBlack)){
            RobotRotateCClockwise(1, 0);
        }else if((FloorSensorValue(FloorFrontRight) == FloorBlack) && (FloorSensorValue(FloorFrontLeft) != FloorBlack)){
            RobotRotateClockwise(1, 0);
        }else{
            if(prev_clock){
                RobotRotateCClockwise(1, 0);
                prev_clock = true;
            }else{
                prev_clock = false;
                RobotRotateClockwise(1, 0);
            }
            g_thread->sleep(MS2ST(100));
            RobotDriveReverse(1, 0);
        }
        g_thread->sleep(MS2ST(500));
    }
    
    RobotStop();

}

void MoveAlternateZigZag()
{
   for(int i = 0; i <= 6; i++){
        RobotRotateClockwise(1, 0);
        g_thread->sleep(MS2ST(200));
        RobotStop();
        g_thread->sleep(MS2ST(100));
        int32_t reading = MagnetometerReading(MAG_X);
        if(reading < mag_min){
            mag_min = reading;
        }
   }

   for(int i = 0; i <=6; i++){
        RobotRotateClockwise(1, 0);
        g_thread->sleep(MS2ST(200));
        RobotStop();
        g_thread->sleep(MS2ST(100));

        int32_t reading = MagnetometerReading(MAG_X);
        if(reading < mag_min){
            mag_min = reading;
        }

   }

   RobotStop();
}



void MoveAlternateZigZag_1()
{
   static bool zigzag_clock = false;
   if(zigzag_clock){
        RobotRotateClockwise(1, 0);
        zigzag_clock = false;
   }else{
        RobotRotateCClockwise(1, 0);
        zigzag_clock = true;
   }
   g_thread->sleep(MS2ST(300));
   RobotStop();
}

void MoveAlternateZigZag_2()
{
   static bool zigzag_clock = false;
   if(zigzag_clock){
        RobotRotateClockwise(1, 0);
        zigzag_clock = false;
   }else{
        RobotRotateCClockwise(1, 0);
        zigzag_clock = true;
   }
   g_thread->sleep(MS2ST(300));
   RobotStop();
}

#endif//PHASE_3 END


