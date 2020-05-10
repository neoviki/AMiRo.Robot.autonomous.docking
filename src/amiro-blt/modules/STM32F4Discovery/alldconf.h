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
 * @file
 * @brief   AMiRo-LLD configuration file for the PowerManagement v1.1 module.
 * @details Contains the application specific AMiRo-LLD settings.
 *
 * @addtogroup powermanagement_lld_config
 * @{
 */

#ifndef _ALLDCONF_H_
#define _ALLDCONF_H_

/*
 * compatibility guards
 */
#define _AMIRO_LLD_CFG_
#define AMIRO_LLD_CFG_VERSION_MAJOR         1
#define AMIRO_LLD_CFG_VERSION_MINOR         0

/**
 * @brief   Width of the apalTime_t data type.
 *
 * @details Possible values are 8, 16, 32, and 64 bits.
 *          By definition time is represented at microsecond precision.
 */
#define AMIROLLD_CFG_TIME_SIZE          32

#endif /* _ALLDCONF_H_ */

/** @} */
