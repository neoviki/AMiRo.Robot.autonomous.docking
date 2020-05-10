/*  AMiRo LED Wrapper 
 *
 *              Author: Viki (a) Vignesh Natarajan (vikiworks.io)
 */
#include <amiro/Color.h>
#include "../global.hpp"
#include "sensors.h"
#ifndef __LED_H
#define __LED_H


#define LED_ALERT_ERROR         Color(Color::RED)
#define LED_ALERT_SUCCESS       Color(Color::GREEN)
#define LED_ALERT_FAILURE       Color(Color::BLUE)
#define LED_ALERT_WALL_CLOSE    Color(Color::HOTPINK)
#define LED_ALERT_WALL_SAFE     Color(Color::YELLOW)
#define LED_ALERT_ENTER_MAIN    Color(Color::HOTPINK)
#define LED_ALERT_STATE_CHANGE  Color(Color::YELLOW)
#define LED_ALERT_STATE_FAILURE Color(Color::CHOCOLATE)
#define LED_ALERT_STATE_SUCCESS Color(Color::BLUE)

#define LED_COLOR_YELLOW Color(Color::YELLOW)
#define LED_COLOR_BLACK Color(Color::BLACK)
#define LED_COLOR_PINK Color(Color::HOTPINK)
#define LED_COLOR_WHITE Color(Color::WHITE)
#if 0
#define LED_COLOR_RED Color(Color::RED)
#define LED_COLOR_GREEN Color(Color::GREEN)
#define LED_COLOR_OLIVE Color(Color::OLIVE)
#define LED_COLOR_BLUE Color(Color::BLUE)
#define LED_COLOR_SKY Color(Color::SKYBLUE)
#define LED_COLOR_VIOLET Color(Color::VIOLET)
#define LED_COLOR_BROWN Color(Color::BROWN)
#define LED_COLOR_WHITE Color(Color::WHITE)
#define LED_COLOR_GREY Color(Color::LIGHTGRAY)
#endif 

using namespace amiro;

typedef enum {
    LedFrontRight = 0, 
    LedFrontLeft = 7, 
    LedBackLeft = 4, 
    LedBackRight = 3,
    LedLeftTop = 6, 
    LedLeftBottom = 5, 
    LedRightTop = 1, 
    LedRightBottom = 2
}LedLocation_t;

extern void LedOnAll(Color color);
extern void LedOn(LedLocation_t location, Color color);
extern void LedOnAllHold(UserThread *thread, Color color);

extern void LedOffAll();
extern void LedOff(LedLocation_t location);
extern LedLocation_t GetLedLocation(ProxSensorLocation_t prox_location);
extern void LedTurnLeft();
extern void LedTurnRight();
#endif
