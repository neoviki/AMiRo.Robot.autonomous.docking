#include "verify_docking.h"

verify_dock_wall_status_t VerifyDock_GetRearWallStatus()
{
    uint16_t back_left   = ProximitySensorValue(ProxBackLeft);
    uint16_t back_right  = ProximitySensorValue(ProxBackRight);
    
    if((back_right > VERIFY_DOCK_WALL_CLOSE) || (back_left > VERIFY_DOCK_WALL_CLOSE)){
        return VERIFY_DOCK_WALL_NEAR;
    }
    
    return VERIFY_DOCK_WALL_FAR;
}


docker_main_codes_t verify_docking(UserThread *thread)
{
    LedOnAllHold(thread, LED_ALERT_STATE_CHANGE);
    docker_main_codes_t status;
    bool robot_moved = false;
    bool magnet_found = false;
#if 0
    for(int i=0; i<10; i++){
        RobotDriveForward(1, 0);
        thread->sleep(MS2ST(250));
        RobotStop();
        thread->sleep(MS2ST(1000)); //old 1000
    }

    verify_dock_wall_status_t wall_state = VerifyDock_GetRearWallStatus();

    if(wall_state == VERIFY_DOCK_WALL_NEAR){
        robot_moved = false;
    }else{
        robot_moved = true;
    }

    if(robot_moved){
        /*Move back to previous state*/
        for(int i=0; i<10; i++){
            RobotDriveReverse(1, 0);
            thread->sleep(MS2ST(250));
            RobotStop();
            thread->sleep(MS2ST(1000)); //old 1000
        }
    }
#endif

    int32_t mag_reading = MagnetometerReading(MAG_X);

    /*greate on the negative side*/
    if(mag_reading <= mag_min){
        /*Strong Magnetic Field*/
        magnet_found = true;
    }

    robot_moved = false;

    if((magnet_found == true)){
        LedOnAllHold(thread, LED_ALERT_STATE_SUCCESS);
        return DOCK_MAIN_CODE_DOCK_ROBOT_SUCCESS;
    }else{
        LedOnAllHold(thread, LED_ALERT_STATE_FAILURE);
        return DOCK_MAIN_CODE_DOCK_ROBOT_FAILURE;
    }
}

