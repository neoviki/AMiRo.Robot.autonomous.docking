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
 * @brief   PowerManagement v1.1 Board specific initializations.
 *
 * @addtogroup powermanagement_board
 * @{
 */

#include <hal.h>
#include <stm32_gpio.h>

/**
 * @brief   GPIO initialization.
 *
 * @param[in]   gpiop   GPIO register block.
 * @param[in]   config  GPIO configuration.
 */



/**
 * @brief   GPIO initialization.
 *
 * @param[in] gpiop     GPIO register block.
 * @param[in] moder     Mode register configuration.
 * @param[in] otyper    Otype register configuration.
 * @param[in] ospeedr   Ospeed register configuration.
 * @param[in] pupdr     Pupd register configuration.
 * @param[in] odr       OD register configuration.
 * @param[in] afrl      AF register (low) configuration.
 * @param[in] afrh      AF register (high ) configuration.
 * @param[in] ignmask   Mask to ignore individual pads.
 */
static void _gpio_init(stm32_gpio_t *gpiop,
                      const uint32_t moder,
                      const uint32_t otyper,
                      const uint32_t ospeedr,
                      const uint32_t pupdr,
                      const uint32_t odr,
                      const uint32_t afrl,
                      const uint32_t afrh,
                      const uint16_t ignmask) {

  const uint8_t lut[] = {0x00, 0x03, 0x0C, 0x0F,
                         0x30, 0x33, 0x3C, 0x3F,
                         0xC0, 0xC3, 0xCC, 0xCF,
                         0xF0, 0xF3, 0xFC, 0xFF};

  /* some bit-magic to fan out the mask */
  const uint32_t ignmask2 = (lut[(ignmask >> 12)       ] << 24) |
                            (lut[(ignmask >>  8) & 0x0F] << 16) |
                            (lut[(ignmask >>  4) & 0x0F] <<  8) |
                            (lut[(ignmask      ) & 0x0F]);
  const uint32_t ignmask4_low = (lut[lut[(ignmask >> 6) & 0x03]] << 24) |
                                (lut[lut[(ignmask >> 4) & 0x03]] << 16) |
                                (lut[lut[(ignmask >> 2) & 0x03]] <<  8) |
                                (lut[lut[(ignmask     ) & 0x03]]);
  const uint32_t ignmask4_high = (lut[lut[(ignmask >> 14)       ]] << 24) |
                                 (lut[lut[(ignmask >> 12) & 0x03]] << 16) |
                                 (lut[lut[(ignmask >> 10) & 0x03]] <<  8) |
                                 (lut[lut[(ignmask >>  8) & 0x03]]);

  gpiop->OTYPER  = (gpiop->OTYPER  & ignmask      ) | (otyper  & ~ignmask      );
  gpiop->OSPEEDR = (gpiop->OSPEEDR & ignmask2     ) | (ospeedr & ~ignmask2     );
  gpiop->PUPDR   = (gpiop->PUPDR   & ignmask2     ) | (pupdr   & ~ignmask2     );
  gpiop->ODR     = (gpiop->ODR     & ignmask      ) | (odr     & ~ignmask      );
  gpiop->AFRL    = (gpiop->AFRL    & ignmask4_low ) | (afrl    & ~ignmask4_low );
  gpiop->AFRH    = (gpiop->AFRH    & ignmask4_high) | (afrh    & ~ignmask4_high);
  gpiop->MODER   = (gpiop->MODER   & ignmask2     ) | (moder   & ~ignmask2     );
}

/**
 * @brief   GPIO initilization for all ports.
 */
static void _stm32_gpio_init(void) {

  /* Enabling GPIO-related clocks, the mask comes from the
     registry header file.*/
  rccResetAHB1(STM32_GPIO_EN_MASK);
  rccEnableAHB1(STM32_GPIO_EN_MASK, true);

  /* Initializing all the defined GPIO ports.*/
#if STM32_HAS_GPIOA
  _gpio_init(GPIOA, VAL_GPIOA_MODER, VAL_GPIOA_OTYPER, VAL_GPIOA_OSPEEDR, VAL_GPIOA_PUPDR, VAL_GPIOA_ODR, VAL_GPIOA_AFRL, VAL_GPIOA_AFRH, VAL_GPIOA_IGNORE);
#endif
#if STM32_HAS_GPIOB
  _gpio_init(GPIOB, VAL_GPIOB_MODER, VAL_GPIOB_OTYPER, VAL_GPIOB_OSPEEDR, VAL_GPIOB_PUPDR, VAL_GPIOB_ODR, VAL_GPIOB_AFRL, VAL_GPIOB_AFRH, VAL_GPIOB_IGNORE);
#endif
#if STM32_HAS_GPIOC
  _gpio_init(GPIOC, VAL_GPIOC_MODER, VAL_GPIOC_OTYPER, VAL_GPIOC_OSPEEDR, VAL_GPIOC_PUPDR, VAL_GPIOC_ODR, VAL_GPIOC_AFRL, VAL_GPIOC_AFRH, VAL_GPIOC_IGNORE);
#endif
#if STM32_HAS_GPIOD
  _gpio_init(GPIOD, VAL_GPIOD_MODER, VAL_GPIOD_OTYPER, VAL_GPIOD_OSPEEDR, VAL_GPIOD_PUPDR, VAL_GPIOD_ODR, VAL_GPIOD_AFRL, VAL_GPIOD_AFRH, VAL_GPIOD_IGNORE);
#endif
#if STM32_HAS_GPIOE
  _gpio_init(GPIOE, VAL_GPIOE_MODER, VAL_GPIOE_OTYPER, VAL_GPIOE_OSPEEDR, VAL_GPIOE_PUPDR, VAL_GPIOE_ODR, VAL_GPIOE_AFRL, VAL_GPIOE_AFRH, VAL_GPIOE_IGNORE);
#endif
#if STM32_HAS_GPIOF
  _gpio_init(GPIOF, VAL_GPIOF_MODER, VAL_GPIOF_OTYPER, VAL_GPIOF_OSPEEDR, VAL_GPIOF_PUPDR, VAL_GPIOF_ODR, VAL_GPIOF_AFRL, VAL_GPIOF_AFRH, VAL_GPIOF_IGNORE);
#endif
#if STM32_HAS_GPIOG
  _gpio_init(GPIOG, VAL_GPIOG_MODER, VAL_GPIOG_OTYPER, VAL_GPIOG_OSPEEDR, VAL_GPIOG_PUPDR, VAL_GPIOG_ODR, VAL_GPIOG_AFRL, VAL_GPIOG_AFRH, VAL_GPIOG_IGNORE);
#endif
#if STM32_HAS_GPIOH
  _gpio_init(GPIOH, VAL_GPIOH_MODER, VAL_GPIOH_OTYPER, VAL_GPIOH_OSPEEDR, VAL_GPIOH_PUPDR, VAL_GPIOH_ODR, VAL_GPIOH_AFRL, VAL_GPIOH_AFRH, VAL_GPIOH_IGNORE);
#endif
#if STM32_HAS_GPIOI
  _gpio_init(GPIOI, VAL_GPIOI_MODER, VAL_GPIOI_OTYPER, VAL_GPIOI_OSPEEDR, VAL_GPIOI_PUPDR, VAL_GPIOI_ODR, VAL_GPIOI_AFRL, VAL_GPIOI_AFRH, VAL_GPIOI_IGNORE);
#endif
#if STM32_HAS_GPIOJ
  _gpio_init(GPIOJ, VAL_GPIOJ_MODER, VAL_GPIOJ_OTYPER, VAL_GPIOJ_OSPEEDR, VAL_GPIOJ_PUPDR, VAL_GPIOJ_ODR, VAL_GPIOJ_AFRL, VAL_GPIOJ_AFRH, VAL_GPIOJ_IGNORE);
#endif
#if STM32_HAS_GPIOK
  _gpio_init(GPIOK, VAL_GPIOK_MODER, VAL_GPIOK_OTYPER, VAL_GPIOK_OSPEEDR, VAL_GPIOK_PUPDR, VAL_GPIOK_ODR, VAL_GPIOK_AFRL, VAL_GPIOK_AFRH, VAL_GPIOK_IGNORE);
#endif
}

/**
 * @brief   Early initialization code.
 * @details This initialization must be performed just after stack setup
 *          and before any other initialization.
 */
void __early_init(void) {

  _stm32_gpio_init();
  stm32_clock_init();
}

/**
 * @brief   Board-specific initialization code.
 * @todo    Add your board-specific code, if any.
 */
void boardInit(void) {
}

/** @} */
