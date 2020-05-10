#include "docker_main.h"
#include "led.h"
#include "sensors.h"
#include "../userthread.hpp"
#include "../global.hpp"
#include <Types.h>
using namespace amiro;
extern Global global;

/* How much percentage of battery's charge left */
uint8_t GetBatteryPercentage()
{
    return global.robot.getPowerStatus().state_of_charge;
}

/* How long it will take to completely charge battery in minutes */
uint16_t GetTimeToFullyChargeBattery()
{
    return global.robot.getPowerStatus().minutes_remaining;
}

/*Check battery charging adapter is physically plugged or not [ true: It doesn't mean robot is charging, It is just plugged ]*/
bool IsChargingAdapterPlugged()
{
   if(global.robot.getPowerStatus().charging_flags.content.powermanagement_plugged_in){
        return true;
   }else{
       return false;
   }
}

/* Check wheather robot's battery is getting charged from charging adapter */
bool IsRobotChargingFromAdapter()
{
   if(global.robot.getPowerStatus().charging_flags.content.powermanagement_charging){
        return true;
   }else{
       return false;
   }
}

/* Check wheather robot's battery is getting charged from Docking Station */
bool IsRobotChargingFromDocker()
{
   if(global.robot.getPowerStatus().charging_flags.content.diwheeldrive_charging){
        return true;
   }else{
       return false;
   }
}

/* Enable battery charging from Docking Station */
void EnableDockerCharging()
{
    global.robot.getPowerStatus().charging_flags.content.diwheeldrive_enable_power_path = 1;
}

/* Disable battery charging from Docking Station */
void DisableDockerCharging()
{
    global.robot.getPowerStatus().charging_flags.content.diwheeldrive_enable_power_path = 0;
}
