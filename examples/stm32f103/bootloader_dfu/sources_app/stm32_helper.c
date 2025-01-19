/** ---------------------------------------- LICENSE SECTION -------------------------------------------------------------------
 *
 * Copyright (c) 2025 Piotr Wojtowicz
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * -----------------------------------------------------------------------------------------------------------------------------
 */

#include "std_libs.h"
#include "stm32_helper.h"

#define STM32_HELPER_USE_EXTI          0
#define STM32_HELPER_USE_EXTI_ON_GPIO  0

#define GPIO_CR_MODE_INPUT          0x00
#define GPIO_CR_MODE_OUTPUT_10MHZ   0x01
#define GPIO_CR_MODE_OUTPUT_2MHZ    0x02
#define GPIO_CR_MODE_OUTPUT_50MHZ   0x03

#define GPIO_CR_CNF_IN_ANALOG       0x00
#define GPIO_CR_CNF_IN_FLOATING     0x01
#define GPIO_CR_CNF_IN_PULL         0x02
#define GPIO_CR_CNF_IN_RESERVED     0x03

#define GPIO_CR_CNF_OUT_GENERAL_PP  0x00
#define GPIO_CR_CNF_OUT_GENERAL_OD  0x04
#define GPIO_CR_CNF_OUT_ALT_PP      0x08
#define GPIO_CR_CNF_OUT_ALT_OD      0x0C

#define GPIO_ODR_INPUT_PULL_DOWN    0x0
#define GPIO_ODR_INPUT_PULL_UP      0x1

#define GPIO_EXTI_RISING_EDGE       0x00100000u
#define GPIO_EXTI_FALLING_EDGE      0x00200000u

static void gpio_set_cr_regs(GPIO_TypeDef *gpio, uint32_t cr, uint8_t pin_id)
{
   volatile uint32_t *reg;
   uint32_t mask;

   if(pin_id < 8)
   {
      reg = &(gpio->CRL);
   }
   else
   {
      reg = &(gpio->CRH);
      pin_id -= 8;
   }

   pin_id <<= 2;

   cr <<= pin_id;

   mask = 0x0000000F << pin_id;
   mask = ~mask;
   *reg = (*reg & mask) | cr;
}

void GPIO_Deinit(GPIO_TypeDef *gpio, uint8_t pin_id)
{
   uint32_t mask = 1 << pin_id;
   mask = ~mask;
   EXTI->IMR  &= mask;
   EXTI->RTSR &= mask;
   EXTI->FTSR &= mask;
   gpio_set_cr_regs(gpio, GPIO_CR_MODE_INPUT | GPIO_CR_CNF_IN_FLOATING, pin_id);
} /* GPIO_Deinit */

void GPIO_Init_Output_Push_Pull(GPIO_TypeDef *gpio, uint8_t pin_id)
{
   gpio_set_cr_regs(gpio, GPIO_CR_MODE_OUTPUT_10MHZ | GPIO_CR_CNF_OUT_GENERAL_PP, pin_id);
} /* GPIO_Init_Output_Push_Pull */

#if(STM32_HELPER_USE_EXTI)
void GPIO_Init_EXTI(uint8_t pin_id, uint32_t exti_type)
{
   uint32_t mask = 1 << pin_id;

   EXTI->RTSR &= ~mask;
   EXTI->FTSR &= ~mask;

   if(0 != exti_type)
   {
      /* Enable or disable the rising trigger */
      if (0 != (exti_type & GPIO_EXTI_RISING_EDGE))
      {
         EXTI->RTSR |= mask;
      }

      /* Enable or disable the falling trigger */
      if (0 != (exti_type & GPIO_EXTI_FALLING_EDGE))
      {
         EXTI->FTSR |= mask;
      }

      EXTI->IMR |= mask;
   }
} /* GPIO_Init_EXTI */
#endif

void GPIO_Init_Input_With_EXTI(GPIO_TypeDef  *gpio, uint8_t pin_id, uint32_t pull_type, uint32_t exti_type)
{
   uint32_t cr;
#if(STM32_HELPER_USE_EXTI_ON_GPIO)
   uint32_t port_id;
   uint32_t mask;
   uint8_t  reg_id;
   uint8_t  pos;
#else
   (void)(exti_type);
#endif

   if(GPIO_NOPULL != pull_type)
   {
      cr = GPIO_CR_MODE_INPUT | (GPIO_CR_CNF_IN_PULL << 2);
   }
   else
   {
      cr = GPIO_CR_MODE_INPUT | (GPIO_CR_CNF_IN_FLOATING << 2);
   }
   gpio_set_cr_regs(gpio, cr, pin_id);
   pull_type &= 0x1;
   gpio->ODR = pull_type << pin_id;

#if(STM32_HELPER_USE_EXTI_ON_GPIO)
   __HAL_RCC_AFIO_CLK_ENABLE();

   port_id  = (uint32_t)gpio - (uint32_t)GPIOA_BASE;
   port_id /= GPIOB_BASE - GPIOA_BASE;
   reg_id   = pin_id >> 2;
   pos   = pin_id & 0x3;
   pos <<= 2;
   mask  = 0xF << pos;
   mask  = ~mask;
   AFIO->EXTICR[reg_id] = (AFIO->EXTICR[reg_id] & mask) | (port_id << pos);

   GPIO_Init_EXTI(pin_id, exti_type);
#endif
} /* GPIO_Init_Input_With_EXTI */
