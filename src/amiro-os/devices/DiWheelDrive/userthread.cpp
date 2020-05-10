#include "userthread.hpp"

#include "global.hpp"
//#include "docker/corner_detector.h"
//#include "docker/led.h"
//#include "docker/wall_follow.h"
//#include "docker/sensors.h"
//#include "docker/motors.h"
#include "docker/docker_main.h"

using namespace amiro;

extern Global global;

UserThread::UserThread() :
  chibios_rt::BaseStaticThread<USER_THREAD_STACK_SIZE>()
{
}

UserThread::~UserThread()
{
}

msg_t
UserThread::main()
{
     chprintf((BaseSequentialStream*) &SD1, "UserThread::main()\n");

    /*
     * thread loop
     */
    while (!this->shouldTerminate()) {
       docker_main(this);
    }

	return RDY_OK;
}

