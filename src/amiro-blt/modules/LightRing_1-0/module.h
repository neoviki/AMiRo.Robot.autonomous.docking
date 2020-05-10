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

#ifndef _AMIROOS_MODULE_H_
#define _AMIROOS_MODULE_H_

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
 * @brief   I2C driver to access the EEPROM.
 */
#define MODULE_HAL_I2C_EEPROM                   I2CD2

/**
 * @brief   Configuration for the EEPROM I2C driver.
 */
extern I2CConfig moduleHalI2cEepromConfig;

/**
 * @brief   Serial driver of the programmer interface.
 */
#define MODULE_HAL_PROGIF                       SD1

/**
 * @brief   Configuration for the programmer serial interface driver.
 */
extern SerialConfig moduleHalProgIfConfig;

/**
 * @brief   SPI interface driver for the motion sensors (gyroscope and accelerometer).
 */
#define MODULE_HAL_SPI_LIGHT                    SPID1

/**
 * @brief   SPI interface driver for the wireless transceiver.
 */
#define MODULE_HAL_SPI_WL                       SPID2

/**
 * @brief   Configuration for the SPI interface driver to communicate with the LED driver.
 */
extern SPIConfig moduleHalSpiLightConfig;

/**
 * @brief   Configuration for the SPI interface driver to communicate with the wireless transceiver.
 */
extern SPIConfig moduleHalSpiWlConfig;

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
 * @brief   LIGHT_BANK output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioLightBlank;

/**
 * @brief   LASER_EN output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioLaserEn;

/**
 * @brief   LASER_OC input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioLaserOc;

/**
 * @brief   SYS_UART_DN bidirectional signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysUartDn;

/**
 * @brief   WL_GDO2 input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioWlGdo2;

/**
 * @brief   WL_GDO0 input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioWlGdo0;

/**
 * @brief   LIGHT_XLAT output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioLightXlat;

/**
 * @brief   SYS_PD bidirectional signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysPd;

/**
 * @brief   SYS_SYNC bidirectional signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysSync;

/** @} */

/*===========================================================================*/
/**
 * @name AMiRo-OS core configurations
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Event flag to be set on a LASER_OC interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_LASEROC          ((eventflags_t)1 << GPIOB_LASER_OC_N)

/**
 * @brief   Event flag to be set on a SYS_UART_DN interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_SYSUARTDN        ((eventflags_t)1 << GPIOB_SYS_UART_DN)

/**
 * @brief   Event flag to be set on a WL_GDO2 interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_WLGDO2           ((eventflags_t)1 << GPIOB_WL_GDO2)

/**
 * @brief   Event flag to be set on a WL_GDO0 interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_WLGDO0           ((eventflags_t)1 << GPIOB_WL_GDO0)

/**
 * @brief   Event flag to be set on a SYS_PD interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_SYSPD            ((eventflags_t)1 << GPIOC_SYS_PD_N)

/**
 * @brief   Event flag to be set on a SYS_SYNC interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_SYSSYNC          ((eventflags_t)1 << GPIOD_SYS_INT_N)

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
  /* LASER_OC */                                                              \
  palSetPadCallback(moduleGpioLaserOc.gpio->port, moduleGpioLaserOc.gpio->pad, _intCallback, &moduleGpioLaserOc.gpio->pad); \
  palEnablePadEvent(moduleGpioLaserOc.gpio->port, moduleGpioLaserOc.gpio->pad, APAL2CH_EDGE(moduleGpioLaserOc.meta.edge));  \
  /* WL_GDO2 */                                                               \
  palSetPadCallback(moduleGpioWlGdo2.gpio->port, moduleGpioWlGdo2.gpio->pad, _intCallback, &moduleGpioWlGdo2.gpio->pad);  \
  palEnablePadEvent(moduleGpioWlGdo2.gpio->port, moduleGpioWlGdo2.gpio->pad, APAL2CH_EDGE(moduleGpioWlGdo2.meta.edge));   \
  /* WL_GDO0 */                                                               \
  palSetPadCallback(moduleGpioWlGdo0.gpio->port, moduleGpioWlGdo0.gpio->pad, _intCallback, &moduleGpioWlGdo0.gpio->pad);  \
  /*palEnablePadEvent(moduleGpioWlGdo0.gpio->port, moduleGpioWlGdo0.gpio->pad, APAL2CH_EDGE(moduleGpioWlGdo0.meta.edge)); // this is broken for some reason*/   \
}

/**
 * @brief   Unit test initialization hook.
 */
#define MODULE_INIT_TESTS() {                                                 \
  /* add unit-test shell commands */                                          \
  aosShellAddCommand(&aos.shell, &moduleUtAlldAt24c01bn.shellcmd);            \
  aosShellAddCommand(&aos.shell, &moduleUtAlldTlc5947.shellcmd);              \
  aosShellAddCommand(&aos.shell, &moduleUtAlldTps2051bdbv.shellcmd);          \
}

/**
 * @brief   Periphery communication interfaces initialization hook.
 */
#define MODULE_INIT_PERIPHERY_COMM() {                                        \
  /* serial driver */                                                         \
  sdStart(&MODULE_HAL_PROGIF, &moduleHalProgIfConfig);                        \
  /* I2C */                                                                   \
  moduleHalI2cEepromConfig.clock_speed = (AT24C01BN_LLD_I2C_MAXFREQUENCY < moduleHalI2cEepromConfig.clock_speed) ? AT24C01BN_LLD_I2C_MAXFREQUENCY : moduleHalI2cEepromConfig.clock_speed; \
  moduleHalI2cEepromConfig.duty_cycle = (moduleHalI2cEepromConfig.clock_speed <= 100000) ? STD_DUTY_CYCLE : FAST_DUTY_CYCLE_2;  \
  i2cStart(&MODULE_HAL_I2C_EEPROM, &moduleHalI2cEepromConfig);                \
  /* SPI */                                                                   \
  spiStart(&MODULE_HAL_SPI_LIGHT, &moduleHalSpiLightConfig);                  \
  spiStart(&MODULE_HAL_SPI_WL, &moduleHalSpiWlConfig);                        \
}

/**
 * @brief   Periphery communication interface deinitialization hook.
 */
#define MODULE_SHUTDOWN_PERIPHERY_COMM() {                                    \
  /* SPI */                                                                   \
  spiStop(&MODULE_HAL_SPI_LIGHT);                                             \
  spiStop(&MODULE_HAL_SPI_WL);                                                \
  /* I2C */                                                                   \
  i2cStop(&MODULE_HAL_I2C_EEPROM);                                            \
  /* don't stop the serial driver so messages can still be printed */         \
}

/** @} */

/*===========================================================================*/
/**
 * @name Startup Shutdown Synchronization Protocol (SSSP)
 * @{
 */
/*===========================================================================*/

/**
 * @brief   PD signal GPIO.
 */
#define moduleSsspGpioPd                        moduleGpioSysPd

/**
 * @brief   SYNC signal GPIO.
 */
#define moduleSsspGpioSync                      moduleGpioSysSync

/**
 * @brief   DN signal GPIO.
 */
#define moduleSsspGpioDn                        moduleGpioSysUartDn

/**
 * @brief   Event flags for PD signal events.
 */
#define MODULE_SSSP_EVENTFLAGS_PD               MODULE_OS_IOEVENTFLAGS_SYSPD

/**
 * @brief   Event flags for SYNC signal events.
 */
#define MODULE_SSSP_EVENTFLAGS_SYNC             MODULE_OS_IOEVENTFLAGS_SYSSYNC

/**
 * @brief   Event flags for DN signal events.
 */
#define MODULE_SSSP_EVENTFLAGS_DN               MODULE_OS_IOEVENTFLAGS_SYSUARTDN

/** @} */

/*===========================================================================*/
/**
 * @name Low-level drivers
 * @{
 */
/*===========================================================================*/
#include <alld_at24c01bn-sh-b.h>
#include <alld_tlc5947.h>
#include <alld_tps2051bdbv.h>

/**
 * @brief   EEPROM driver.
 */
extern AT24C01BNDriver moduleLldEeprom;

/**
 * @brief   LED PWM driver.
 */
extern TLC5947Driver moduleLldLedPwm;

/**
 * @brief   Power switch driver for the laser supply power.
 */
extern TPS2051BDriver moduleLldPowerSwitchLaser;

/** @} */

/*===========================================================================*/
/**
 * @name Unit tests (UT)
 * @{
 */
/*===========================================================================*/
#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)
#include <ut_alld_at24c01bn-sh-b.h>
#include <ut_alld_tlc5947.h>
#include <ut_alld_tps2051bdbv.h>
#include <ut_alld_dw1000.h>

/**
 * @brief   EEPROM unit test object.
 */
extern aos_unittest_t moduleUtAlldAt24c01bn;

/**
 * @brief   LED PWM driver unit test object.
 */
extern aos_unittest_t moduleUtAlldTlc5947;

/**
 * @brief   Current-limited power switch (Laser output)
 */
extern aos_unittest_t moduleUtAlldTps2051bdbv;


#endif /* AMIROOS_CFG_TESTS_ENABLE == true */

/** @} */

#endif /* _AMIROOS_MODULE_H_ */
