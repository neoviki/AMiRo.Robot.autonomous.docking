#ifndef __BATTERY_H
#define __BATTERY_H
#include <stdint.h>

/* How much percentage of battery's charge left */
extern uint8_t GetBatteryPercentage();

/* How long it will take to completely charge battery in minutes */
extern uint16_t GetTimeToFullyChargeBattery();

/*Check battery charging adapter is physically plugged or not [ true: It doesn't mean robot is charging, It is just plugged ]*/
extern bool IsChargingAdapterPlugged();

/* Check wheather robot's battery is getting charged from charging adapter */
extern bool IsRobotChargingFromAdapter();

/* Check wheather robot's battery is getting charged from Docking Station */
extern bool IsRobotChargingFromDocker();

/* Enable battery charging from Docking Station */
extern void EnableDockerCharging();

/* Disable battery charging from Docking Station */
extern void DisableDockerCharging();
#endif
