#include "docker.h"

#include <amiro/Color.h>
#include "../userthread.hpp"
#include <stdint.h>
//#include "../global.hpp"
#include "led.h"
#include "sensors.h"
#include "motors.h"

using namespace amiro;

docker_codes_t docker_wall[8];

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

void DockRobot(UserThread *thread)
{
    int rows = 0;

    /*Move sensor out of dark line*/
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


    fp_docker_t current_state = Docker1_Start, next_state = NULL;
    docker_codes_t state_code = DOCKER_CODE_NONE; 
    rows = sizeof(docker1_table) / sizeof(sm_docker_nodes_t);
    while(1){
        //LedOffAll();
        DockerWallStateAll(docker_wall);

        /*Execute State*/
        state_code = current_state();

        if((current_state == Docker1_FollowWall)){
            //chprintf((BaseSequentialStream*) &SD1, "Wall Follow State Completed\n");
            LedOnAll(LED_COLOR_BLUE);
            thread->sleep(MS2ST(1000));
            return;
        }
        
        if((current_state == Docker1_Phase2)){
            /*Proceed to next substate*/
            LedOnAll(LED_COLOR_GREEN);
            thread->sleep(MS2ST(4000));
            break;
        }

        /*Find Next State*/
        next_state = docker_next_state(docker1_table, rows, current_state, state_code);

        if(next_state == NULL){
            LedOnAll(LED_COLOR_RED);
            break;
        }

        current_state = next_state;
        thread->sleep(MS2ST(500));
    }
}

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
    return DOCKER_CODE_NONE;
}

docker_codes_t Docker1_Phase2()
{
    RobotDriveForward(0, 0);
    return DOCKER_CODE_NONE;
}
