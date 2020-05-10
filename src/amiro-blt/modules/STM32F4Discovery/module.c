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
 * @brief   Structures and constant for the PowerManagement module.
 *
 * @addtogroup powermanagement_module
 * @{
 */

#include "module.h"

#include <amiroos.h>

/*===========================================================================*/
/**
 * @name Module specific functions
 * @{
 */
/*===========================================================================*/

/** @} */

/*===========================================================================*/
/**
 * @name ChibiOS/HAL configuration
 * @{
 */
/*===========================================================================*/

CANConfig moduleHalCanConfig = {
  /* mcr  */ CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
  /* btr  */ CAN_BTR_SJW(1) | CAN_BTR_TS2(3) | CAN_BTR_TS1(15) | CAN_BTR_BRP(1),
};

SerialConfig moduleHalProgIfConfig = {
  /* bit rate */ 115200,
  /* CR1      */ 0,
  /* CR1      */ 0,
  /* CR1      */ 0,
};

/** @} */

/*===========================================================================*/
/**
 * @name GPIO definitions
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Red LED output signal GPIO.
 */
static apalGpio_t _gpioLedRed = {
  /* port */ GPIOD,
  /* pad  */ GPIOD_LED5,
};
ROMCONST apalControlGpio_t moduleGpioLedRed = {
  /* GPIO */ &_gpioLedRed,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   Green LED output signal GPIO.
 */
static apalGpio_t _gpioLedGreen = {
  /* port */ GPIOD,
  /* pad  */ GPIOD_LED4,
};
ROMCONST apalControlGpio_t moduleGpioLedGreen = {
  /* GPIO */ &_gpioLedGreen,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   Blue LED output signal GPIO.
 */
static apalGpio_t _gpioLedBlue = {
  /* port */ GPIOD,
  /* pad  */ GPIOD_LED6,
};
ROMCONST apalControlGpio_t moduleGpioLedBlue = {
  /* GPIO */ &_gpioLedBlue,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   Orange LED output signal GPIO.
 */
static apalGpio_t _gpioLedOrange = {
  /* port */ GPIOD,
  /* pad  */ GPIOD_LED3,
};
ROMCONST apalControlGpio_t moduleGpioLedOrange = {
  /* GPIO */ &_gpioLedOrange,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   User button input signal GPIO.
 */
static apalGpio_t _gpioUserButton = {
  /* port */ GPIOA,
  /* pad  */ GPIOA_BUTTON,
};
ROMCONST apalControlGpio_t moduleGpioUserButton = {
  /* GPIO */ &_gpioUserButton,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/** @} */

/*===========================================================================*/
/**
 * @name AMiRo-OS core configurations
 * @{
 */
/*===========================================================================*/

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
ROMCONST char* moduleShellPrompt = "STM32F407Discovery";
#endif

/** @} */

/*===========================================================================*/
/**
 * @name Startup Shutdown Synchronization Protocol (SSSP)
 * @{
 */
/*===========================================================================*/

/** @} */

/*===========================================================================*/
/**
 * @name Low-level drivers
 * @{
 */
/*===========================================================================*/

/** @} */

/*===========================================================================*/
/**
 * @name Unit tests (UT)
 * @{
 */
/*===========================================================================*/
#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)

#endif /* AMIROOS_CFG_TESTS_ENABLE == true */

/** @} */
/** @} */
