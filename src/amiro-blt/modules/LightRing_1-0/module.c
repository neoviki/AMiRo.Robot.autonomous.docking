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
  /* btr  */ CAN_BTR_SJW(1) | CAN_BTR_TS2(2) | CAN_BTR_TS1(13) | CAN_BTR_BRP(1),
};

I2CConfig moduleHalI2cEepromConfig = {
  /* I²C mode   */ OPMODE_I2C,
  /* frequency  */ 400000, // TODO: replace with some macro (-> ChibiOS/HAL)
  /* duty cycle */ FAST_DUTY_CYCLE_2,
};

SerialConfig moduleHalProgIfConfig = {
  /* bit rate */ 115200,
  /* CR1      */ 0,
  /* CR1      */ 0,
  /* CR1      */ 0,
};

SPIConfig moduleHalSpiLightConfig = {
  /* circular buffer mode        */ false,
  /* callback function pointer   */ NULL,
  /* chip select line port       */ GPIOC,
  /* chip select line pad number */ GPIOC_LIGHT_XLAT,
  /* CR1                         */ SPI_CR1_BR_0 | SPI_CR1_BR_1,
  /* CR2                         */ SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN,
};

SPIConfig moduleHalSpiWlConfig = {
  /* circular buffer mode        */ false,
  /* callback function pointer   */ NULL,
  /* chip select line port       */ GPIOB,
  /* chip select line pad number */ GPIOB_WL_SS_N,
  /* CR1                         */ SPI_CR1_BR_0,
  /* CR2                         */ SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN,
};

/*===========================================================================*/
/**
 * @name GPIO definitions
 * @{
 */
/*===========================================================================*/

/**
 * @brief   LIGHT_BANK output signal GPIO.
 */
static apalGpio_t _gpioLightBlank = {
  /* port */ GPIOA,
  /* pad  */ GPIOA_LIGHT_BLANK,
};
ROMCONST apalControlGpio_t moduleGpioLightBlank = {
  /* GPIO */ &_gpioLightBlank,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ TLC5947_LLD_BLANK_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   LASER_EN output signal GPIO.
 */
static apalGpio_t _gpioLaserEn = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_LASER_EN,
};
ROMCONST apalControlGpio_t moduleGpioLaserEn = {
  /* GPIO */ &_gpioLaserEn,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ TPS2051B_LLD_ENABLE_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   LASER_OC input signal GPIO.
 */
static apalGpio_t _gpioLaserOc = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_LASER_OC_N,
};
ROMCONST apalControlGpio_t moduleGpioLaserOc = {
  /* GPIO */ &_gpioLaserOc,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ TPS2051B_LLD_OVERCURRENT_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   SYS_UART_DN bidirectional signal GPIO.
 */
static apalGpio_t _gpioSysUartDn = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_SYS_UART_DN,
};
ROMCONST apalControlGpio_t moduleGpioSysUartDn = {
  /* GPIO */ &_gpioSysUartDn,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   WL_GDO2 input signal GPIO.
 */
static apalGpio_t _gpioWlGdo2 = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_WL_GDO2,
};
ROMCONST apalControlGpio_t moduleGpioWlGdo2 = {
  /* GPIO */ &_gpioWlGdo2,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   WL_GDO0 input signal GPIO.
 */
static apalGpio_t _gpioWlGdo0= {
  /* port */ GPIOB,
  /* pad  */ GPIOB_WL_GDO0,
};
ROMCONST apalControlGpio_t moduleGpioWlGdo0 = {
  /* GPIO */ &_gpioWlGdo0,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   LIGHT_XLAT output signal GPIO.
 */
static apalGpio_t _gpioLightXlat = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_LIGHT_XLAT,
};
ROMCONST apalControlGpio_t moduleGpioLightXlat = {
  /* GPIO */ &_gpioLightXlat,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ TLC5947_LLD_XLAT_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   SYS_PD bidirectional signal GPIO.
 */
static apalGpio_t _gpioSysPd = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_SYS_PD_N,
};
ROMCONST apalControlGpio_t moduleGpioSysPd = {
  /* GPIO */ &_gpioSysPd,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   SYS_SYNC bidirectional signal GPIO.
 */
static apalGpio_t _gpioSysSync = {
  /* port */ GPIOD,
  /* pad  */ GPIOD_SYS_INT_N,
};
ROMCONST apalControlGpio_t moduleGpioSysSync = {
  /* GPIO */ &_gpioSysSync,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
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
ROMCONST char* moduleShellPrompt = "LightRing";
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

AT24C01BNDriver moduleLldEeprom = {
  /* I2C driver   */ &MODULE_HAL_I2C_EEPROM,
  /* I2C address  */ 0x00u,
};

TLC5947Driver moduleLldLedPwm = {
  /* SPI driver         */ &MODULE_HAL_SPI_LIGHT,
  /* BLANK signal GPIO  */ &moduleGpioLightBlank,
  /* XLAT signal GPIO   */ &moduleGpioLightXlat,
};

TPS2051BDriver moduleLldPowerSwitchLaser = {
  /* laser enable GPIO      */ &moduleGpioLaserEn,
  /* laser overcurrent GPIO */ &moduleGpioLaserOc,
};

/** @} */

/*===========================================================================*/
/**
 * @name Unit tests (UT)
 * @{
 */
/*===========================================================================*/
#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)

/* EEPROM (AT24C01BN) */
static int _utShellCmdCb_AlldAt24c01bn(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  aosUtRun(stream, &moduleUtAlldAt24c01bn, NULL);
  return AOS_OK;
}
static ut_at24c01bndata_t _utAt24c01bnData = {
  /* driver   */ &moduleLldEeprom,
  /* timeout  */ MICROSECONDS_PER_SECOND,
};
aos_unittest_t moduleUtAlldAt24c01bn = {
  /* name           */ "AT24C01BN-SH-B",
  /* info           */ "1kbit EEPROM",
  /* test function  */ utAlldAt24c01bnFunc,
  /* shell command  */ {
    /* name     */ "unittest:EEPROM",
    /* callback */ _utShellCmdCb_AlldAt24c01bn,
    /* next     */ NULL,
  },
  /* data           */ &_utAt24c01bnData,
};

/* LED PWM driver (TLD5947) */
static int _utShellCmdCb_Tlc5947(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  aosUtRun(stream, &moduleUtAlldTlc5947, NULL);
  return AOS_OK;
}
aos_unittest_t moduleUtAlldTlc5947 = {
  /* info           */ "TLC5947",
  /* name           */ "LED PWM driver",
  /* test function  */ utAlldTlc5947Func,
  /* shell command  */ {
    /* name     */ "unittest:Lights",
    /* callback */ _utShellCmdCb_Tlc5947,
    /* next     */ NULL,
  },
  /* data           */ &moduleLldLedPwm,
};

/* power switch (Laser) */
static int _utShellCmdCb_Tps2051bdbv(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  aosUtRun(stream,&moduleUtAlldTps2051bdbv, NULL);
  return AOS_OK;
}
aos_unittest_t moduleUtAlldTps2051bdbv = {
  /* info           */ "TPS2051BDBV",
  /* name           */ "current-limited power switch",
  /* test function  */ utAlldTps2051bdbvFunc,
  /* shell command  */ {
    /* name     */ "unittest:PowerSwitch",
    /* callback */ _utShellCmdCb_Tps2051bdbv,
    /* next     */ NULL,
  },
  /* data           */ &moduleLldPowerSwitchLaser,
};

#endif /* AMIROOS_CFG_TESTS_ENABLE == true */

/** @} */
