/*
AMiRo-OS is an operating system designed for the Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2016..2019  Thomas Schöpping et al.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    aos_thread.c
 * @brief   Thread code.
 *
 * @addtogroup aos_threads
 * @{
 */

#include <aos_thread.h>

/**
 * @brief   Lets the calling thread sleep until the specifide system uptime.
 *
 * @param[in] t     Deadline until the thread will sleep.
 */
void aosThdSleepUntilS(const aos_timestamp_t* t)
{
  aosDbgCheck(t != NULL);

  aos_timestamp_t uptime;

  // get the current system uptime
  aosSysGetUptimeX(&uptime);

  // while the remaining time is too long, it must be split into multiple sleeps
  while ( (*t > uptime) && ((*t - uptime) > AOS_THD_MAX_SLEEP_US) ) {
    chThdSleepS(chTimeUS2I(AOS_THD_MAX_SLEEP_US));
    aosSysGetUptimeX(&uptime);
  }

  // sleep the remaining time
  if (*t > uptime) {
    sysinterval_t rest = chTimeUS2I(*t - uptime);
    if (rest > TIME_IMMEDIATE) {
      chThdSleepS(rest);
    }
  }

  return;
}

#if (CH_DBG_FILL_THREADS == TRUE) || defined(__DOXYGEN__)
/**
 * @brief   Calculate the peak stack utilization for a specific thread so far in bytes.
 *
 * @param[in] thread    Thread to calculate the stack utilization for.
 *
 * @return  Absolute peak stack utilization in bytes.
 */
size_t aosThdGetStackPeakUtilization(thread_t* thread)
{
  aosDbgCheck(thread != NULL);

  size_t util;
  uint8_t* ptr = (uint8_t*)thread->wabase;

  chSysLock();
  while (*ptr == CH_DBG_STACK_FILL_VALUE && ptr < (uint8_t*)thread->wabase + aosThdGetStacksize(thread)) {
    ++ptr;
  }
  util = aosThdGetStacksize(thread) - (ptr - (uint8_t*)thread->wabase);
  chSysUnlock();

  return util;
}
#endif /* CH_DBG_FILL_THREADS == TRUE */

/** @} */
