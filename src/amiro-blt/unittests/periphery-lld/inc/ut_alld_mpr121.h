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

#ifndef _AMIROOS_UT_ALLD_MPR121_H_
#define _AMIROOS_UT_ALLD_MPR121_H_

#include <aos_unittest.h>
#include <amiro-lld.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_MPR121)) || defined(__DOXYGEN__)

#include <alld_mpr121.h>

/**
 * @brief   Custom data structure for the unit test.
 */
typedef struct {
  /**
   * @brief   Pointer to the MPR121 driver to use.
   */
  MPR121Driver* mprd;

  /**
   * @brief   Timeout value (in us).
   */
  apalTime_t timeout;

  /**
   * @brief   Event source to listen to.
   */
  event_source_t* evtsource;

  /**
   * @brief   Event flags to watch.
   */
  eventflags_t evtflags;
} ut_mpr121data_t;

#ifdef __cplusplus
extern "C" {
#endif
  aos_utresult_t utAlldMpr121Func(BaseSequentialStream* stream, aos_unittest_t* ut);
#ifdef __cplusplus
}
#endif

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_MPR121) */

#endif /* _AMIROOS_UT_ALLD_MPR121_H_ */
