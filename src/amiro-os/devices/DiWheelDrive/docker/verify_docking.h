#ifndef __VERIFY_DOCKING_H
#define __VERIFY_DOCKING_H
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
#include "sensors.h"
#include <Types.h>
using namespace amiro;
#define VERIFY_DOCK_WALL_CLOSE 50000

typedef enum{
    VERIFY_DOCK_WALL_NEAR,
    VERIFY_DOCK_WALL_FAR
}verify_dock_wall_status_t;

docker_main_codes_t verify_docking(UserThread *thread);
extern verify_dock_wall_status_t VerifyDock_GetRearWallStatus();
#endif
