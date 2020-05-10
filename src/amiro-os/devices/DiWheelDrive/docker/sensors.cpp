#include <stdint.h>
#include "../global.hpp"
#include "sensors.h"
#include "led.h"
using namespace amiro;
extern Global global;

FloorSensorStatus_t FloorSensorValue(FloorSensorLocation_t location){
    uint16_t sensor_value = global.vcnl4020[location].getProximityScaledWoOffset();  

    if (sensor_value < BlackFalling ) {
        return  FloorBlack;
	}else{
        return FloorWhite;
    }
}

uint16_t ProximitySensorValue(ProxSensorLocation_t location)
{
    return global.robot.getProximityRingValue(location);
}

int32_t MagnetometerReading(mag_axis_t axis)
{
    return global.hmc5883l.getMagnetization(axis);
}

int16_t AccelReading(accel_axis_t axis)
{
   return global.lis331dlh.getAcceleration(axis);
}

types::position OdometerReading()
{
    return global.odometry.getPosition();
}

void OdometerReset()
{
   global.odometry.resetPosition();
}

uint16_t ProxyDistance2uint16_t(float distance)
{
    uint16_t value = 0;
   
    if(distance == 0.25){
        value = 37000;
    }else if(distance == 0.5){
        value = 1690;
    }else if(distance == 1){
        value = 7600;
    }else if(distance == 1.5){
        value = 4200;
    }else if(distance == 2){
        value = 2720;
    }else if(distance == 2.5){
        value = 1840;
    }else if(distance == 3){
        value = 1320;
    }else if(distance == 3.5){
        value = 950;
    }else if(distance == 4){
        value = 740;
    }else if(distance == 4.5){
        value = 590;
    }else if(distance == 5){
        value = 450;
    }else if(distance == 5.5){
        value = 360;
    }else if(distance == 6){
        value = 317;
    }else if(distance == 6.5){
        value = 210;
    }else if(distance == 7){
        value = 197;
    }else if(distance == 7.5){
        value = 153;
    }else if(distance == 8){
        value = 111;
    }else if(distance == 8.5){
        value = 93;
    }else if(distance == 9){
        value = 67;
    }else if(distance == 9.5){
        value = 50;
    }else if(distance == 10){
        value = 22;
    }else{
        value = 0;
    }

    return value;
}
