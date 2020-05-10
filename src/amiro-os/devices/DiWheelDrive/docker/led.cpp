#include "led.h"
#include "../global.hpp"
#include "sensors.h"
using namespace amiro;
extern Global global;

void LedOnAll(Color color)
{
    for (uint8_t led = 0; led < 8; ++led) {
        global.robot.setLightColor(led, color);
    }
}

void LedOnAllHold(UserThread *thread, Color color)
{
    for(uint8_t i=0; i<10; i++){
        LedOnAll(color);
        thread->sleep(MS2ST(500));
    }
    LedOffAll();
}

void LedOn(LedLocation_t location, Color color)
{
    global.robot.setLightColor(location, color);
}

void LedTurnLeft()
{
    global.robot.setLightColor(LedFrontLeft, Color(Color::ORANGE));
}

void LedTurnRight()
{
    global.robot.setLightColor(LedFrontRight, Color(Color::ORANGE));
}

void LedOffAll()
{
    for (uint8_t led = 0; led < 8; ++led) {
        global.robot.setLightColor(led, Color(Color::BLACK));
    }
}

void LedOff(LedLocation_t location)
{
    global.robot.setLightColor(location, Color(Color::BLACK));
}

LedLocation_t GetLedLocation(ProxSensorLocation_t prox_location)
{
   LedLocation_t led_location = LedFrontLeft;
   switch(prox_location){
       case ProxFrontLeft:
            led_location = LedFrontLeft;
            break;
       case ProxFrontRight:
            led_location = LedFrontRight;
            break;
       case ProxBackLeft:
            led_location = LedBackLeft;
            break;
       case ProxBackRight:
            led_location = LedBackRight;
            break;
       case ProxLeftTop:
            led_location = LedLeftTop;
            break;
       case ProxLeftBottom:
            led_location = LedLeftBottom;
            break;
       case ProxRightTop:
            led_location = LedRightTop;
            break;
       case ProxRightBottom:
            led_location = LedRightBottom;
            break;
       default:
            break;
   }

   return led_location;
}


