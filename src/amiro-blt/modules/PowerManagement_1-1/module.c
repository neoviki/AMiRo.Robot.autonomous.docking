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

ADCConversionGroup moduleHalAdcVsysConversionGroup = {
  /* buffer type        */ true,
  /* number of channels */ 1,
  /* callback function  */ NULL,
  /* error callback     */ NULL,
  /* CR1                */ ADC_CR1_AWDEN | ADC_CR1_AWDIE,
  /* CR2                */ ADC_CR2_SWSTART | ADC_CR2_CONT,
  /* SMPR1              */ 0,
  /* SMPR2              */ ADC_SMPR2_SMP_AN9(ADC_SAMPLE_480),
  /* HTR                */ ADC_HTR_HT,
  /* LTR                */ 0,
  /* SQR1               */ ADC_SQR1_NUM_CH(1),
  /* SQR2               */ 0,
  /* SQR3               */ ADC_SQR3_SQ1_N(ADC_CHANNEL_IN9),
};

CANConfig moduleHalCanConfig = {
  /* mcr  */ CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
  /* btr  */ CAN_BTR_SJW(1) | CAN_BTR_TS2(3) | CAN_BTR_TS1(15) | CAN_BTR_BRP(1),
};

I2CConfig moduleHalI2cProxPm18Pm33GaugeRearConfig = {
  /* I²C mode   */ OPMODE_I2C,
  /* frequency  */ 400000, // TODO: replace with some macro (-> ChibiOS/HAL)
  /* duty cycle */ FAST_DUTY_CYCLE_2,
};

I2CConfig moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig = {
  /* I²C mode   */ OPMODE_I2C,
  /* frequency  */ 400000, // TODO: replace with some macro (-> ChibiOS/HAL)
  /* duty cycle */ FAST_DUTY_CYCLE_2,
};

PWMConfig moduleHalPwmBuzzerConfig = {
  /* frequency              */ 1000000,
  /* period                 */ 0,
  /* callback               */ NULL,
  /* channel configurations */ {
    /* channel 0              */ {
      /* mode                   */ PWM_OUTPUT_DISABLED,
      /* callback               */ NULL
    },
    /* channel 1              */ {
      /* mode                   */ PWM_OUTPUT_ACTIVE_HIGH,
      /* callback               */ NULL
    },
    /* channel 2              */ {
      /* mode                   */ PWM_OUTPUT_DISABLED,
      /* callback               */ NULL
    },
    /* channel 3              */ {
      /* mode                   */ PWM_OUTPUT_DISABLED,
      /* callback               */ NULL
    },
  },
  /* TIM CR2 register       */ 0,
#if STM32_PWM_USE_ADVANCED
  /* TIM BDTR register      */ 0,
#endif
  /* TIM DIER register      */ 0,
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
 * @brief   SYS_REG_EN output signal GPIO.
 */
static apalGpio_t _gpioSysRegEn = {
  /* port */ GPIOA,
  /* pad  */ GPIOA_SYS_REG_EN,
};
ROMCONST apalControlGpio_t moduleSysRegEn = {
  /* GPIO */ &_gpioSysRegEn,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   IR_INT1 input signal GPIO.
 */
static apalGpio_t _gpioIrInt1 = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_IR_INT1_N,
};
ROMCONST apalControlGpio_t moduleGpioIrInt1 = {
  /* GPIO */ &_gpioIrInt1,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ (VCNL4020_LLD_INT_EDGE == APAL_GPIO_EDGE_RISING) ? APAL_GPIO_ACTIVE_HIGH : APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ VCNL4020_LLD_INT_EDGE,
  },
};

/**
 * @brief   POWER_EN output signal GPIO.
 */
static apalGpio_t _gpioPowerEn = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_POWER_EN,
};
ROMCONST apalControlGpio_t moduleGpioPowerEn = {
  /* GPIO */ &_gpioPowerEn,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
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
 * @brief   CHARGE_STAT2A input signal GPIO.
 */
static apalGpio_t _gpioChargeStat2A = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_CHARGE_STAT2A,
};
ROMCONST apalControlGpio_t moduleGpioChargeStat2A = {
  /* GPIO */ &_gpioChargeStat2A,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ BQ24103A_LLD_CHARGE_STATUS_GPIO_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   GAUGE_BATLOW2 input signal GPIO.
 */
static apalGpio_t _gpioGaugeBatLow2 = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_GAUGE_BATLOW2,
};
ROMCONST apalControlGpio_t moduleGpioGaugeBatLow2 = {
  /* GPIO */ &_gpioGaugeBatLow2,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ BQ27500_LLD_BATLOW_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   GAUGE_BATGD2 input signal GPIO.
 */
static apalGpio_t _gpioGaugeBatGd2 = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_GAUGE_BATGD2_N,
};
ROMCONST apalControlGpio_t moduleGpioGaugeBatGd2 = {
  /* GPIO */ &_gpioGaugeBatGd2,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ BQ27500_LLD_BATGOOD_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   LED output signal GPIO.
 */
static apalGpio_t _gpioLed = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_LED,
};
ROMCONST apalControlGpio_t moduleGpioLed = {
  /* GPIO */ &_gpioLed,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ LED_LLD_GPIO_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   SYS_UART_UP bidirectional signal GPIO.
 */
static apalGpio_t _gpioSysUartUp = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_SYS_UART_UP,
};
ROMCONST apalControlGpio_t moduleGpioSysUartUp = {
  /* GPIO */ &_gpioSysUartUp,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   CHARGE_STAT1A input signal GPIO.
 */
static apalGpio_t _gpioChargeStat1A = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_CHARGE_STAT1A,
};
ROMCONST apalControlGpio_t moduleGpioChargeStat1A = {
  /* GPIO */ &_gpioChargeStat1A,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ BQ24103A_LLD_CHARGE_STATUS_GPIO_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   GAUGE_BATLOW1 input signal GPIO.
 */
static apalGpio_t _gpioGaugeBatLow1 = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_GAUGE_BATLOW1,
};
ROMCONST apalControlGpio_t moduleGpioGaugeBatLow1 = {
  /* GPIO */ &_gpioGaugeBatLow1,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ BQ27500_LLD_BATLOW_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   GAUGE_BATGD1 input signal GPIO.
 */
static apalGpio_t _gpioGaugeBatGd1 = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_GAUGE_BATGD1_N,
};
ROMCONST apalControlGpio_t moduleGpioGaugeBatGd1 = {
  /* GPIO */ &_gpioGaugeBatGd1,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ BQ27500_LLD_BATGOOD_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   CHARG_EN1 output signal GPIO.
 */
static apalGpio_t _gpioChargeEn1 = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_CHARGE_EN1_N,
};
ROMCONST apalControlGpio_t moduleGpioChargeEn1 = {
  /* GPIO */ &_gpioChargeEn1,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ BQ24103A_LLD_ENABLED_GPIO_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   IR_INT2 input signal GPIO.
 */
static apalGpio_t _gpioIrInt2 = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_IR_INT2_N,
};
ROMCONST apalControlGpio_t moduleGpioIrInt2 = {
  /* GPIO */ &_gpioIrInt2,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ (VCNL4020_LLD_INT_EDGE == APAL_GPIO_EDGE_RISING) ? APAL_GPIO_ACTIVE_HIGH : APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ VCNL4020_LLD_INT_EDGE,
  },
};

/**
 * @brief   TOUCH_INT input signal GPIO.
 */
static apalGpio_t _gpioTouchInt = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_TOUCH_INT_N,
};
ROMCONST apalControlGpio_t moduleGpioTouchInt = {
  /* GPIO */ &_gpioTouchInt,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ (MPR121_LLD_INT_EDGE == APAL_GPIO_EDGE_RISING) ? APAL_GPIO_ACTIVE_HIGH : APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ MPR121_LLD_INT_EDGE,
  },
};

/**
 * @brief   SYS_DONE input signal GPIO.
 */
static apalGpio_t _gpioSysDone = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_SYS_DONE,
};
ROMCONST apalControlGpio_t moduleGpioSysDone = {
  /* GPIO */ &_gpioSysDone,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   SYS_PROG output signal GPIO.
 */
static apalGpio_t _gpioSysProg = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_SYS_PROG_N,
};
ROMCONST apalControlGpio_t moduleGpioSysProg = {
  /* GPIO */ &_gpioSysProg,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   PATH_DC input signal GPIO.
 */
static apalGpio_t _gpioPathDc = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_PATH_DC,
};
ROMCONST apalControlGpio_t moduleGpioPathDc = {
  /* GPIO */ &_gpioPathDc,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   SYS_SPI_DIR bidirectional signal GPIO.
 */
static apalGpio_t _gpioSysSpiDir = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_SYS_SPI_DIR,
};
ROMCONST apalControlGpio_t moduleGpioSysSpiDir = {
  /* GPIO */ &_gpioSysSpiDir,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_FALLING,
  },
};

/**
 * @brief   SYS_SYNC bidirectional signal GPIO.
 */
static apalGpio_t _gpioSysSync = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_SYS_INT_N,
};
ROMCONST apalControlGpio_t moduleGpioSysSync = {
  /* GPIO */ &_gpioSysSync,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
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
 * @brief   SYS_WARMRST bidirectional signal GPIO.
 */
static apalGpio_t _gpioSysWarmrst = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_SYS_WARMRST_N,
};
ROMCONST apalControlGpio_t moduleGpioSysWarmrst = {
  /* GPIO */ &_gpioSysWarmrst,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   BT_RST output signal GPIO.
 */
static apalGpio_t _gpioBtRst = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_BT_RST,
};
ROMCONST apalControlGpio_t moduleGpioBtRst = {
  /* GPIO */ &_gpioBtRst,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   CHARGE_EN2 output signal GPIO.
 */
static apalGpio_t _gpioChargeEn2 = {
  /* port */ GPIOD,
  /* pad  */ GPIOD_CHARGE_EN2_N,
};
ROMCONST apalControlGpio_t moduleGpioChargeEn2 = {
  /* GPIO */ &_gpioChargeEn2,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ BQ24103A_LLD_ENABLED_GPIO_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
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
ROMCONST char* moduleShellPrompt = "PowerManagement";
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
  /* I2C driver   */ &MODULE_HAL_I2C_PROX_PM42_PM50_PMVDD_EEPROM_TOUCH_GAUGEFRONT,
  /* I2C address  */ AT24C01BN_LLD_I2C_ADDR_FIXED,
};

BQ24103ADriver moduleLldBatteryChargerFront = {
  /* charge enable GPIO */ &moduleGpioChargeEn1,
  /* charge status GPIO */ &moduleGpioChargeStat1A,
};

BQ24103ADriver moduleLldBatteryChargerRear = {
  /* charge enable GPIO */ &moduleGpioChargeEn2,
  /* charge status GPIO */ &moduleGpioChargeStat2A,
};

BQ27500Driver moduleLldFuelGaugeFront = {
  /* I2C driver         */ &MODULE_HAL_I2C_PROX_PM42_PM50_PMVDD_EEPROM_TOUCH_GAUGEFRONT,
  /* battery low GPIO   */ &moduleGpioGaugeBatLow1,
  /* battery good GPIO  */ &moduleGpioGaugeBatGd1,
};

BQ27500Driver moduleLldFuelGaugeRear = {
  /* I2C driver         */ &MODULE_HAL_I2C_PROX_PM18_PM33_GAUGEREAR,
  /* battery low GPIO   */ &moduleGpioGaugeBatLow2,
  /* battery good GPIO  */ &moduleGpioGaugeBatGd2,
};

INA219Driver moduleLldPowerMonitorVdd = {
  /* I2C Driver       */ &MODULE_HAL_I2C_PROX_PM42_PM50_PMVDD_EEPROM_TOUCH_GAUGEFRONT,
  /* I²C address      */ INA219_LLD_I2C_ADDR_A0 | INA219_LLD_I2C_ADDR_A1,
  /* current LSB (uA) */ 0x00u,
  /* configuration    */ NULL,
};

INA219Driver moduleLldPowerMonitorVio18 = {
  /* I2C Driver       */ &MODULE_HAL_I2C_PROX_PM18_PM33_GAUGEREAR,
  /* I²C address      */ INA219_LLD_I2C_ADDR_A1,
  /* current LSB (uA) */ 0x00u,
  /* configuration    */ NULL,
};

INA219Driver moduleLldPowerMonitorVio33 = {
  /* I2C Driver       */ &MODULE_HAL_I2C_PROX_PM18_PM33_GAUGEREAR,
  /* I²C address      */ INA219_LLD_I2C_ADDR_FIXED,
  /* current LSB (uA) */ 0x00u,
  /* configuration    */ NULL,
};

INA219Driver moduleLldPowerMonitorVsys42 = {
  /* I2C Driver       */ &MODULE_HAL_I2C_PROX_PM42_PM50_PMVDD_EEPROM_TOUCH_GAUGEFRONT,
  /* I²C address      */ INA219_LLD_I2C_ADDR_FIXED,
  /* current LSB (uA) */ 0x00u,
  /* configuration    */ NULL,
};

INA219Driver moduleLldPowerMonitorVio50 = {
  /* I2C Driver       */ &MODULE_HAL_I2C_PROX_PM42_PM50_PMVDD_EEPROM_TOUCH_GAUGEFRONT,
  /* I²C address      */ INA219_LLD_I2C_ADDR_A1,
  /* current LSB (uA) */ 0x00u,
  /* configuration    */ NULL,
};

LEDDriver moduleLldStatusLed = {
  /* LED GPIO */ &moduleGpioLed,
};

MPR121Driver moduleLldTouch = {
  /* I²C Driver */ &MODULE_HAL_I2C_PROX_PM42_PM50_PMVDD_EEPROM_TOUCH_GAUGEFRONT,
};

PCA9544ADriver moduleLldI2cMultiplexer1 = {
  /* I²C driver   */ &MODULE_HAL_I2C_PROX_PM18_PM33_GAUGEREAR,
  /* I²C address  */ PCA9544A_LLD_I2C_ADDR_A0 | PCA9544A_LLD_I2C_ADDR_A1 | PCA9544A_LLD_I2C_ADDR_A2,
};

PCA9544ADriver moduleLldI2cMultiplexer2 = {
  /* I²C driver   */ &MODULE_HAL_I2C_PROX_PM42_PM50_PMVDD_EEPROM_TOUCH_GAUGEFRONT,
  /* I²C address  */ PCA9544A_LLD_I2C_ADDR_A0 | PCA9544A_LLD_I2C_ADDR_A1 | PCA9544A_LLD_I2C_ADDR_A2,
};

TPS62113Driver moduleLldStepDownConverter = {
  /* Power enable GPIO */ &moduleGpioPowerEn,
};

VCNL4020Driver moduleLldProximity1 = {
  /* I²C Driver */ &MODULE_HAL_I2C_PROX_PM18_PM33_GAUGEREAR,
};

VCNL4020Driver moduleLldProximity2 = {
  /* I²C Driver */ &MODULE_HAL_I2C_PROX_PM42_PM50_PMVDD_EEPROM_TOUCH_GAUGEFRONT,
};

/** @} */

/*===========================================================================*/
/**
 * @name Unit tests (UT)
 * @{
 */
/*===========================================================================*/
#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)
#include <string.h>

/* ADC  */
static int _utShellCmdCb_Adc(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  aosUtRun(stream, &moduleUtAdcVsys, NULL);
  return AOS_OK;
}
static ut_adcdata_t _utAdcVsysData = {
  /* driver               */ &MODULE_HAL_ADC_VSYS,
  /* ADC conversion group */ &moduleHalAdcVsysConversionGroup,
};
aos_unittest_t moduleUtAdcVsys = {
  /* name           */ "ADC",
  /* info           */ "VSYS",
  /* test function  */ utAdcFunc,
  /* shell command  */ {
    /* name     */ "unittest:ADC",
    /* callback */ _utShellCmdCb_Adc,
    /* next     */ NULL,
  },
  /* data           */ &_utAdcVsysData,
};

/* AT24C01BN-SH-B (EEPROM) */
static int _utShellCmdCb_AlldAt24c01bn(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  aosUtRun(stream, &moduleUtAlldAt24c01bn, NULL);
  return AOS_OK;
}
static ut_at24c01bndata_t _utAlldAt24c01bnData = {
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
  /* data           */ &_utAlldAt24c01bnData,
};

/* BQ24103A (battery charger) */
static int _utShellCmdCb_AlldBq24103a(BaseSequentialStream* stream, int argc, char* argv[])
{
  // local variables
  bool print_help = false;

  // evaluate argument
  if (argc == 2) {
    if (strcmp(argv[1], "-f") == 0 || strcmp(argv[1], "--front") == 0) {
      moduleUtAlldBq24103a.data = &moduleLldBatteryChargerFront;
      aosUtRun(stream, &moduleUtAlldBq24103a, "front battery");
      moduleUtAlldBq24103a.data = NULL;
    }
    else if (strcmp(argv[1], "-r") == 0 || strcmp(argv[1], "--rear") == 0) {
      moduleUtAlldBq24103a.data = &moduleLldBatteryChargerRear;
      aosUtRun(stream, &moduleUtAlldBq24103a, "rear battery");
      moduleUtAlldBq24103a.data = NULL;
    }
    else {
      print_help = true;
    }
  } else {
    print_help = true;
  }

  // print help or just return
  if (print_help) {
    chprintf(stream, "Usage: %s OPTION\n", argv[0]);
    chprintf(stream, "Options:\n");
    chprintf(stream, "  --front, -f\n");
    chprintf(stream, "    Test the front battery charger.\n");
    chprintf(stream, "  --rear, -r\n");
    chprintf(stream, "    Test the rear battery charger.\n");
    return AOS_INVALID_ARGUMENTS;
  } else {
    return AOS_OK;
  }
}
aos_unittest_t moduleUtAlldBq24103a = {
  /* name           */ "BQ24103A",
  /* info           */ "battery charger",
  /* test function  */ utAlldBq24103aFunc,
  /* shell command  */ {
    /* name     */ "unittest:BatteryCharger",
    /* callback */ _utShellCmdCb_AlldBq24103a,
    /* next     */ NULL,
  },
  /* data           */ NULL,
};

/* BQ27500 (fuel gauge) */
static int _utShellCmdCb_AlldBq27500(BaseSequentialStream* stream, int argc, char* argv[])
{
  // evaluate arguments
  if (argc == 2) {
    if (strcmp(argv[1], "-f") == 0 || strcmp(argv[1], "--front") == 0) {
      ((ut_bq27500data_t*)moduleUtAlldBq27500.data)->driver = &moduleLldFuelGaugeFront;
      aosUtRun(stream, &moduleUtAlldBq27500, "front battery");
      ((ut_bq27500data_t*)moduleUtAlldBq27500.data)->driver = NULL;
      return AOS_OK;
    }
    else if (strcmp(argv[1], "-r") == 0 || strcmp(argv[1], "--rear") == 0) {
      ((ut_bq27500data_t*)moduleUtAlldBq27500.data)->driver = &moduleLldFuelGaugeRear;
      aosUtRun(stream, &moduleUtAlldBq27500, "rear battery");
      ((ut_bq27500data_t*)moduleUtAlldBq27500.data)->driver = NULL;
      return AOS_OK;
    }
  }
  // print help
  chprintf(stream, "Usage: %s OPTION\n", argv[0]);
  chprintf(stream, "Options:\n");
  chprintf(stream, "  --front, -f\n");
  chprintf(stream, "    Test the front battery fuel gauge.\n");
  chprintf(stream, "  --rear, -r\n");
  chprintf(stream, "    Test the rear battery fuel gauge.\n");
  return AOS_INVALID_ARGUMENTS;
}
static ut_bq27500data_t _utAlldBq27500Data = {
  /* driver   */ NULL,
  /* timeout  */ MICROSECONDS_PER_SECOND,
};
aos_unittest_t moduleUtAlldBq27500 = {
  /* name           */ "BQ27500",
  /* info           */ "fuel gauge",
  /* test function  */ utAlldBq27500Func,
  /* shell command  */ {
    /* name     */ "unittest:FuelGauge",
    /* callback */ _utShellCmdCb_AlldBq27500,
    /* next     */ NULL,
  },
  /* data           */ &_utAlldBq27500Data,
};

/* BQ27500 (fuel gauge) in combination with BQ24103A (battery charger) */
static int _utShellCmdCb_AlldBq27500Bq24103a(BaseSequentialStream* stream, int argc, char* argv[])
{
  // evaluate arguments
  if (argc == 2) {
    if (strcmp(argv[1], "-f") == 0 || strcmp(argv[1], "--front") == 0) {
      ((ut_bq27500bq24103adata_t*)moduleUtAlldBq27500Bq24103a.data)->bq27500 = &moduleLldFuelGaugeFront;
      ((ut_bq27500bq24103adata_t*)moduleUtAlldBq27500Bq24103a.data)->bq24103a = &moduleLldBatteryChargerFront;
      aosUtRun(stream, &moduleUtAlldBq27500Bq24103a, "front battery");
      ((ut_bq27500bq24103adata_t*)moduleUtAlldBq27500Bq24103a.data)->bq27500 = NULL;
      ((ut_bq27500bq24103adata_t*)moduleUtAlldBq27500Bq24103a.data)->bq24103a = NULL;
      return AOS_OK;
    }
    else if (strcmp(argv[1], "-r") == 0 || strcmp(argv[1], "--rear") == 0) {
      ((ut_bq27500bq24103adata_t*)moduleUtAlldBq27500Bq24103a.data)->bq27500 = &moduleLldFuelGaugeRear;
      ((ut_bq27500bq24103adata_t*)moduleUtAlldBq27500Bq24103a.data)->bq24103a = &moduleLldBatteryChargerRear;
      aosUtRun(stream, &moduleUtAlldBq27500Bq24103a, "rear battery");
      ((ut_bq27500bq24103adata_t*)moduleUtAlldBq27500Bq24103a.data)->bq27500 = NULL;
      ((ut_bq27500bq24103adata_t*)moduleUtAlldBq27500Bq24103a.data)->bq24103a = NULL;
      return AOS_OK;
    }
  }
  // print help
  chprintf(stream, "Usage: %s OPTION\n", argv[0]);
  chprintf(stream, "Options:\n");
  chprintf(stream, "  --front, -f\n");
  chprintf(stream, "    Test the front battery fuel gauge and charger.\n");
  chprintf(stream, "  --rear, -r\n");
  chprintf(stream, "    Test the rear battery fuel gauge and charger.\n");
  return AOS_INVALID_ARGUMENTS;
}
static ut_bq27500bq24103adata_t _utAlldBq27500Bq24103aData= {
  /* BQ27500 driver   */ NULL,
  /* BQ23203A driver  */ NULL,
  /* timeout          */ MICROSECONDS_PER_SECOND,
};
aos_unittest_t moduleUtAlldBq27500Bq24103a = {
  /* name           */ "BQ27500 & BQ24103A",
  /* info           */ "fuel gauge & battery charger",
  /* test function  */ utAlldBq27500Bq24103aFunc,
  /* shell command  */ {
    /* name     */ "unittest:FuelGauge&BatteryCharger",
    /* callback */ _utShellCmdCb_AlldBq27500Bq24103a,
    /* next     */ NULL,
  },
  /* data           */ &_utAlldBq27500Bq24103aData,
};

/* INA219 (power monitor) */
static int _utShellCmdCb_AlldIna219(BaseSequentialStream* stream, int argc, char* argv[])
{
  // evaluate arguments
  if (argc == 2) {
    if (strcmp(argv[1], "VDD") == 0) {
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->inad = &moduleLldPowerMonitorVdd;
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->v_expected = 3.3f;
      aosUtRun(stream, &moduleUtAlldIna219, "VDD (3.3V)");
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->inad = NULL;
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->v_expected = 0.0f;
      return AOS_OK;
    }
    else if (strcmp(argv[1], "VIO1.8") == 0) {
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->inad = &moduleLldPowerMonitorVio18;
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->v_expected = 1.8f;
      aosUtRun(stream, &moduleUtAlldIna219, "VIO (1.8V)");
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->inad = NULL;
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->v_expected = 0.0f;
      return AOS_OK;
    }
    else if (strcmp(argv[1], "VIO3.3") == 0) {
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->inad = &moduleLldPowerMonitorVio33;
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->v_expected = 3.3f;
      aosUtRun(stream, &moduleUtAlldIna219, "VIO (3.3V)");
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->inad = NULL;
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->v_expected = 0.0f;
      return AOS_OK;
    }
    else if (strcmp(argv[1], "VSYS4.2") == 0) {
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->inad = &moduleLldPowerMonitorVsys42;
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->v_expected = 4.2f;
      aosUtRun(stream, &moduleUtAlldIna219, "VSYS (4.2V)");
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->inad = NULL;
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->v_expected = 0.0f;
      return AOS_OK;
    }
    else if (strcmp(argv[1], "VIO5.0") == 0) {
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->inad = &moduleLldPowerMonitorVio50;
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->v_expected = 5.0f;
      aosUtRun(stream, &moduleUtAlldIna219, "VIO (5.0V)");
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->inad = NULL;
      ((ut_ina219data_t*)moduleUtAlldIna219.data)->v_expected = 0.0f;
      return AOS_OK;
    }
  }
  // print help
  chprintf(stream, "Usage: %s OPTION\n", argv[0]);
  chprintf(stream, "Options:\n");
  chprintf(stream, "  VDD\n");
  chprintf(stream, "    Test VDD (3.3V) power monitor.\n");
  chprintf(stream, "  VIO1.8\n");
  chprintf(stream, "    Test VIO 1.8V power monitor.\n");
  chprintf(stream, "  VIO3.3\n");
  chprintf(stream, "    Test VIO 3.3V power monitor.\n");
  chprintf(stream, "  VSYS4.2\n");
  chprintf(stream, "    Test VSYS 4.2V power monitor.\n");
  chprintf(stream, "  VIO5.0\n");
  chprintf(stream, "    Test VIO 5.0V power monitor.\n");
  return AOS_INVALID_ARGUMENTS;
}
static ut_ina219data_t _utAlldIna219Data = {
  /* driver           */ NULL,
  /* expected voltage */ 0.0f,
  /* tolerance        */ 0.05f,
  /* timeout          */ MICROSECONDS_PER_SECOND,
};
aos_unittest_t moduleUtAlldIna219 = {
  /* name           */ "INA219",
  /* info           */ "power monitor",
  /* test function  */ utAlldIna219Func,
  /* shell command  */ {
    /* name     */ "unittest:PowerMonitor",
    /* callback */ _utShellCmdCb_AlldIna219,
    /* next     */ NULL,
  },
  /* data           */ &_utAlldIna219Data,
};

/* Status LED */
static int _utShellCmdCb_AlldLed(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  aosUtRun(stream, &moduleUtAlldLed, NULL);
  return AOS_OK;
}
aos_unittest_t moduleUtAlldLed = {
  /* name           */ "LED",
  /* info           */ NULL,
  /* test function  */ utAlldLedFunc,
  /* shell command  */ {
    /* name     */ "unittest:StatusLED",
    /* callback */ _utShellCmdCb_AlldLed,
    /* next     */ NULL,
  },
  /* data           */ &moduleLldStatusLed,
};

/* MPR121 (touch sensor) */
static int _utShellCmdCb_AlldMpr121(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  aosUtRun(stream, &moduleUtAlldMpr121, NULL);
  return AOS_OK;
}
static ut_mpr121data_t _utAlldMpr121Data= {
  /* MPR121 driver  */ &moduleLldTouch,
  /* timeout        */ MICROSECONDS_PER_SECOND,
  /* event source   */ &aos.events.io,
  /* event flags    */ MODULE_OS_IOEVENTFLAGS_TOUCHINT,
};
aos_unittest_t moduleUtAlldMpr121 = {
  /* name           */ "MPR121",
  /* info           */ "touch sensor",
  /* test function  */ utAlldMpr121Func,
  /* shell command  */ {
    /* name     */ "unittest:Touch",
    /* callback */ _utShellCmdCb_AlldMpr121,
    /* next     */ NULL,
  },
  /* data           */ &_utAlldMpr121Data,
};

/* PCA9544A (I2C multiplexer) */
static int _utShellCmdCb_AlldPca5944a(BaseSequentialStream* stream, int argc, char* argv[])
{
  // evaluate arguments
  if (argc == 2) {
    if (strcmp(argv[1], "#1") == 0) {
      ((ut_pca9544adata_t*)moduleUtAlldPca9544a.data)->driver = &moduleLldI2cMultiplexer1;
      aosUtRun(stream, &moduleUtAlldPca9544a, "I2C bus #1");
      ((ut_pca9544adata_t*)moduleUtAlldPca9544a.data)->driver = NULL;
      return AOS_OK;
    }
    else if (strcmp(argv[1], "#2") == 0) {
      ((ut_pca9544adata_t*)moduleUtAlldPca9544a.data)->driver = &moduleLldI2cMultiplexer2;
      aosUtRun(stream, &moduleUtAlldPca9544a, "I2C bus #2");
      ((ut_pca9544adata_t*)moduleUtAlldPca9544a.data)->driver = NULL;
      return AOS_OK;
    }
  }
  // print help
  chprintf(stream, "Usage: %s OPTION\n", argv[0]);
  chprintf(stream, "Options:\n");
  chprintf(stream, "  #1\n");
  chprintf(stream, "    Test the multiplexer on the I2C bus #1.\n");
  chprintf(stream, "  #2\n");
  chprintf(stream, "    Test the multiplexer on the I2C bus #2.\n");
  return AOS_INVALID_ARGUMENTS;
}
static ut_pca9544adata_t _utAlldPca9544aData = {
  /* driver   */ NULL,
  /* timeout  */ MICROSECONDS_PER_SECOND,
};
aos_unittest_t moduleUtAlldPca9544a = {
  /* name           */ "PCA9544A",
  /* info           */ "I2C multiplexer",
  /* test function  */ utAlldPca9544aFunc,
  /* shell command  */ {
    /* name     */ "unittest:I2CMultiplexer",
    /* callback */ _utShellCmdCb_AlldPca5944a,
    /* next     */ NULL,
  },
  /* data           */ &_utAlldPca9544aData,
};

/* PKLCS1212E4001 (buzzer) */
static int _utShellCmdCb_AlldPklcs1212e4001(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  aosUtRun(stream, &moduleUtAlldPklcs1212e4001, NULL);
  return AOS_OK;
}
static ut_pklcs1212e4001_t _utAlldPklcs1212e4001Data = {
  /* PWM driver   */ &MODULE_HAL_PWM_BUZZER,
  /* PWM channel  */ MODULE_HAL_PWM_BUZZER_CHANNEL
};
aos_unittest_t moduleUtAlldPklcs1212e4001 = {
  /* name           */ "PKLCS1212E4001",
  /* info           */ "buzzer",
  /* test function  */ utAlldPklcs1212e4001Func,
  /* shell command  */ {
    /* name     */ "unittest:Buzzer",
    /* callback */ _utShellCmdCb_AlldPklcs1212e4001,
    /* next     */ NULL,
  },
  /* data           */ &_utAlldPklcs1212e4001Data,
};

/* TPS62113 (stop-down converter) */
static int _utShellCmdCb_AlldTps62113(BaseSequentialStream* stream, int argc, char* argv[])
{
  // Although there are four TPS62113 on the PCB, they all share the same input signal.
  // A sa result, no additional shell arguments need to be evaluated.
  (void)argc;
  (void)argv;
  aosUtRun(stream, &moduleUtAlldTps62113, NULL);
  return AOS_OK;
}
aos_unittest_t moduleUtAlldTps62113 = {
  /* name           */ "TPS62113",
  /* info           */ "step-down converter",
  /* test function  */ utAlldTps62113Func,
  /* shell command  */ {
    /* name     */ "unittest:StepDownConverter",
    /* callback */ _utShellCmdCb_AlldTps62113,
    /* next     */ NULL,
  },
  /* data           */ &moduleLldStepDownConverter,
};

/* TPS62113 (step-donw converter) in combination with INA219 (power monitor) */
static int _utShellCmdCb_AlldTps62113Ina219(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  aosUtRun(stream, &moduleUtAlldTps62113Ina219, "VSYS (4.2V)");
  return AOS_OK;
}
static ut_tps62113ina219data_t _utAlldTps62113Ina219Data = {
  /* TPS62113 */ &moduleLldStepDownConverter,
  /* INA219   */ &moduleLldPowerMonitorVsys42,
  /* timeout  */ MICROSECONDS_PER_SECOND,
};
aos_unittest_t moduleUtAlldTps62113Ina219 = {
  /* name           */ "TPS62113 & INA219",
  /* info           */ "step-down converter & power monitor",
  /* test function  */ utAlldTps62113Ina219Func,
  /* shell command  */ {
    /* name     */ "unittest:StepDownConverter&PowerMonitor",
    /* callback */ _utShellCmdCb_AlldTps62113Ina219,
    /* next     */ NULL,
  },
  /* data           */ &_utAlldTps62113Ina219Data,
};

/* VCNL4020 (proximity sensor) */
static void _utAlldVcnl4020_disableInterrupt(VCNL4020Driver* vcnl)
{
  uint8_t intstatus;
  vcnl4020_lld_writereg(vcnl, VCNL4020_LLD_REGADDR_INTCTRL, 0, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
  vcnl4020_lld_readreg(vcnl, VCNL4020_LLD_REGADDR_INTSTATUS, &intstatus, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
  if (intstatus) {
    vcnl4020_lld_writereg(vcnl, VCNL4020_LLD_REGADDR_INTSTATUS, intstatus, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
  }
  return;
}
static int _utShellCmdCb_AlldVcnl4020(BaseSequentialStream* stream, int argc, char* argv[])
{
  enum {
    UNKNOWN,
    NNE, ENE, ESE, SSE, SSW,WSW,WNW, NNW,
  } sensor = UNKNOWN;
  // evaluate arguments
  if (argc == 2) {
    if (strcmp(argv[1], "-nne") == 0) {
      sensor = NNE;
    } else if (strcmp(argv[1], "-ene") == 0) {
      sensor = ENE;
    } else if (strcmp(argv[1], "-ese") == 0) {
      sensor = ESE;
    } else if (strcmp(argv[1], "-sse") == 0) {
      sensor = SSE;
    } else if (strcmp(argv[1], "-ssw") == 0) {
      sensor = SSW;
    } else if (strcmp(argv[1], "-wsw") == 0) {
      sensor = WSW;
    } else if (strcmp(argv[1], "-wnw") == 0) {
      sensor = WNW;
    } else if (strcmp(argv[1], "-nnw") == 0) {
      sensor = NNW;
    }
  }
  if (sensor != UNKNOWN) {
    PCA9544ADriver* mux = NULL;
    switch (sensor) {
      case SSE:
      case SSW:
      case WSW:
      case WNW:
        mux = &moduleLldI2cMultiplexer1;
        ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->vcnld = &moduleLldProximity1;
        ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->evtflags = MODULE_OS_IOEVENTFLAGS_IRINT2;
        break;
      case NNW:
      case NNE:
      case ENE:
      case ESE:
        mux = &moduleLldI2cMultiplexer2;
        ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->vcnld = &moduleLldProximity2;
        ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->evtflags = MODULE_OS_IOEVENTFLAGS_IRINT1;
        break;
      default:
        break;
    }
    pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH0, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
    _utAlldVcnl4020_disableInterrupt(((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->vcnld);
    pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH1, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
    _utAlldVcnl4020_disableInterrupt(((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->vcnld);
    pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH2, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
    _utAlldVcnl4020_disableInterrupt(((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->vcnld);
    pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH3, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
    _utAlldVcnl4020_disableInterrupt(((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->vcnld);
    switch (sensor) {
      case NNE:
        pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH1, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
        aosUtRun(stream, &moduleUtAlldVcnl4020, "north-northeast sensor");
        break;
      case ENE:
        pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH3, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
        aosUtRun(stream, &moduleUtAlldVcnl4020, "east-northeast sensor");
        break;
      case ESE:
        pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH2, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
        aosUtRun(stream, &moduleUtAlldVcnl4020, "north-southeast sensor");
        break;
      case SSE:
        pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH0, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
        aosUtRun(stream, &moduleUtAlldVcnl4020, "south-southeast sensor");
        break;
      case SSW:
        pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH1, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
        aosUtRun(stream, &moduleUtAlldVcnl4020, "south-southwest sensor");
        break;
      case WSW:
        pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH3, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
        aosUtRun(stream, &moduleUtAlldVcnl4020, "west-southwest sensor");
        break;
      case WNW:
        pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH2, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
        aosUtRun(stream, &moduleUtAlldVcnl4020, "west-northwest sensor");
        break;
      case NNW:
        pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH0, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
        aosUtRun(stream, &moduleUtAlldVcnl4020, "north-northwest sensor");
        break;
      default:
        break;
    }
    ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->vcnld = NULL;
    ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->evtflags = 0;
    return AOS_OK;
  }
  // print help
  chprintf(stream, "Usage: %s OPTION\n", argv[0]);
  chprintf(stream, "Options:\n");
  chprintf(stream, "  -nne\n");
  chprintf(stream, "    Test north-northeast sensor.\n");
  chprintf(stream, "  -ene\n");
  chprintf(stream, "    Test east-northeast sensor.\n");
  chprintf(stream, "  -ese\n");
  chprintf(stream, "    Test east-southeast sensor.\n");
  chprintf(stream, "  -sse\n");
  chprintf(stream, "    Test south-southeast sensor.\n");
  chprintf(stream, "  -ssw\n");
  chprintf(stream, "    Test south-southwest sensor.\n");
  chprintf(stream, "  -wsw\n");
  chprintf(stream, "    Test west-southwest sensor.\n");
  chprintf(stream, "  -wnw\n");
  chprintf(stream, "    Test west-northwest sensor.\n");
  chprintf(stream, "  -nnw\n");
  chprintf(stream, "    Test north-northwest sensor.\n");
  return AOS_INVALID_ARGUMENTS;
}
static ut_vcnl4020data_t _utAlldVcnl4020Data = {
  /* driver       */ NULL,
  /* timeout      */ MICROSECONDS_PER_SECOND,
  /* event source */ &aos.events.io,
  /* event flags  */ 0,
};
aos_unittest_t moduleUtAlldVcnl4020 = {
  /* name           */ "VCNL4020",
  /* info           */ "proximity sensor",
  /* test function  */ utAlldVcnl4020Func,
  /* shell command  */ {
    /* name     */ "unittest:Proximity",
    /* callback */ _utShellCmdCb_AlldVcnl4020,
    /* next     */ NULL,
  },
  /* data           */ &_utAlldVcnl4020Data,
};

#endif /* AMIROOS_CFG_TESTS_ENABLE == true */

/** @} */
/** @} */
