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

#ifndef _AMIROOS_MODULE_H_
#define _AMIROOS_MODULE_H_

/*===========================================================================*/
/**
 * @name Module specific functions
 * @{
 */
/*===========================================================================*/

/*
 * @brief Makro to store data in the core coupled memory (ccm).
 *        Example:
 *        int compute_buffer[128] CCM_RAM;
 *
 * @note The ccm is not connected to any bus system.
 */
#define CCM_RAM                                 __attribute__((section(".ram4"), aligned(4)))

/*
 * @brief Makro to store data in the ethernet memory (eth).
 *        Example:
 *        int dma_buffer[128] ETH_RAM;
 *
 * @note The eth is a dedicated memory block with its own DMA controller.
 */
#define ETH_RAM                                 __attribute__((section(".ram2"), aligned(4)))

/*
 * @brief Makro to store data in the backup memory (bckp).
 *        Example:
 *        int backup_buffer[128] BCKP_RAM;
 *
 * @note The eth is a dedicated memory block with its own DMA controller.
 */
#define BCKP_RAM                                __attribute__((section(".ram5"), aligned(4)))

/** @} */

/*===========================================================================*/
/**
 * @name ChibiOS/HAL configuration
 * @{
 */
/*===========================================================================*/
#include <hal.h>

/**
 * @brief   CAN driver to use.
 */
#define MODULE_HAL_CAN                          CAND1

/**
 * @brief   Configuration for the CAN driver.
 */
extern CANConfig moduleHalCanConfig;

/**
 * @brief   Serial driver of the programmer interface.
 */
#define MODULE_HAL_PROGIF                       SD2

/**
 * @brief   Configuration for the programmer serial interface driver.
 */
extern SerialConfig moduleHalProgIfConfig;

/**
 * @brief   Real-Time Clock driver.
 */
#define MODULE_HAL_RTC                          RTCD1

/** @} */

/*===========================================================================*/
/**
 * @name GPIO definitions
 * @{
 */
/*===========================================================================*/
#include <amiro-lld.h>

/**
 * @brief   Red LED output signal.
 */
extern ROMCONST apalControlGpio_t moduleGpioLedRed;

/**
 * @brief   Green LED output signal.
 */
extern ROMCONST apalControlGpio_t moduleGpioLedGreen;

/**
 * @brief   Blue LED output signal.
 */
extern ROMCONST apalControlGpio_t moduleGpioLedBlue;

/**
 * @brief   Orange LED output signal.
 */
extern ROMCONST apalControlGpio_t moduleGpioLedOrange;

/**
 * @brief   User button input signal.
 */
extern ROMCONST apalControlGpio_t moduleGpioUserButton;

/** @} */

/*===========================================================================*/
/**
 * @name AMiRo-OS core configurations
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Event flag to be set on a USER_BUTTON interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_USERBUTTON       ((eventflags_t)1 << GPIOA_BUTTON)

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @brief   Shell prompt text.
 */
extern ROMCONST char* moduleShellPrompt;
#endif

/**
 * @brief   Interrupt initialization macro.
 * @note    SSSP related interrupt signals are already initialized in 'aos_system.c'.
 */
#define MODULE_INIT_INTERRUPTS() {                                            \
  /* user button */                                                           \
  palSetPadCallback(moduleGpioUserButton.gpio->port, moduleGpioUserButton.gpio->pad, _intCallback, &moduleGpioUserButton.gpio->pad);  \
  palEnablePadEvent(moduleGpioUserButton.gpio->port, moduleGpioUserButton.gpio->pad, APAL2CH_EDGE(moduleGpioUserButton.meta.edge));   \
}

/**
 * @brief   Unit test initialization hook.
 */
#define MODULE_INIT_TESTS() {                                                 \
  /* add unit-test shell commands */                                          \
}

/**
 * @brief   Periphery communication interfaces initialization hook.
 */
#define MODULE_INIT_PERIPHERY_COMM() {                                        \
  /* serial driver */                                                         \
  sdStart(&MODULE_HAL_PROGIF, &moduleHalProgIfConfig);                        \
}

/**
 * @brief   Periphery communication interface deinitialization hook.
 */
#define MODULE_SHUTDOWN_PERIPHERY_COMM() {                                    \
}

/**
 * @brief   HOOK to toggle the LEDs when the user button is pressed.
 */
#define MODULE_MAIN_LOOP_IO_EVENT(eventmask, eventflags) {                    \
  if (eventflags & MODULE_OS_IOEVENTFLAGS_USERBUTTON) {                       \
    apalGpioToggle(moduleGpioLedRed.gpio);                                    \
    apalGpioToggle(moduleGpioLedGreen.gpio);                                  \
    apalGpioToggle(moduleGpioLedBlue.gpio);                                   \
    apalGpioToggle(moduleGpioLedOrange.gpio);                                 \
  }                                                                           \
}

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

#endif /* _AMIROOS_MODULE_H_ */

/** @} */
