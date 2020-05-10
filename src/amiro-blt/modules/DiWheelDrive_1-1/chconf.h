/*
 * AMiRo-OS is an operating system designed for the Autonomous Mini Robot (AMiRo) platform.
 * Copyright (C) 2016..2019  Thomas Schöpping et al.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file    
 * @brief   ChibiOS Configuration file for the DiWheelDrive v1.1 module.
 * @details Contains the application specific kernel settings.
 *
 * @addtogroup diwheeldrive_ch_config
 * @details Kernel related settings and hooks.
 * @{
 */

#ifndef CHCONF_H
#define CHCONF_H

#define _CHIBIOS_RT_CONF_
#define _CHIBIOS_RT_CONF_VER_5_1_

#include <aosconf.h>

/*===========================================================================*/
/**
 * @name System timers settings
 * @{
 */
/*===========================================================================*/

/**
 * @brief   System time counter resolution.
 * @note    Allowed values are 16 or 32 bits.
 */
#if !defined(CH_CFG_ST_RESOLUTION)
#define CH_CFG_ST_RESOLUTION                16
#endif

// more common definition in aos_chconf.h

/** @} */

/*===========================================================================*/
/**
 * @name Kernel parameters and options
 * @{
 */
/*===========================================================================*/

// common definitions in aos_chconf.h

/** @} */

/*===========================================================================*/
/**
 * @name Performance options
 * @{
 */
/*===========================================================================*/

// common definitions in aos_chconf.h

/** @} */

/*===========================================================================*/
/**
 * @name Subsystem options
 * @{
 */
/*===========================================================================*/

// common definitions in aos_chconf.h

/** @} */

/*===========================================================================*/
/**
 * @name Objects factory options
 * @{
 */
/*===========================================================================*/

// common definitions in aos_chconf.h

/** @} */

/*===========================================================================*/
/**
 * @name Debug options
 * @{
 */
/*===========================================================================*/

// common definitions in aos_chconf.h

/** @} */

/*===========================================================================*/
/**
 * @name Kernel hooks
 * @{
 */
/*===========================================================================*/

// common definitions in aos_chconf.h

/** @} */

/*===========================================================================*/
/**
 * @name Port-specific settings (override port settings defaulted in chcore.h).
 * @{
 */
/*===========================================================================*/

/**
 * @brief   NVIC VTOR initialization offset.
 * @details On initialization, the code at this address in the flash memory will be executed.
 */
#define CORTEX_VTOR_INIT 0x00006000U

/** @} */

/*===========================================================================*/
/**
 * @name other
 * @{
 */
/*===========================================================================*/

// common definitions in aos_chconf.h

/** @} */

#include <aos_chconf.h>

#endif  /* CHCONF_H */

/** @} */
