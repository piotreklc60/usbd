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
#include "main.h"

#include "gpio.h"


#include "usbd.h"
#include "main_usbd_init.h"
#include "boot_config.h"
#include "stm32_helper.h"
#include "user_app_handle.h"

#define __RCC_APB1Periph_ALL               ((uint32_t)0xFFFFFFFF)
#define __RCC_APB2Periph_ALL               ((uint32_t)0xFFFFFFFF)

static void main_rcc_deinit_all(void);

void SystemClock_Config(void)
{
   /*-------------------------------- PLL Configuration -----------------------*/

   /* Configure the main PLL clock source and multiplication factors. */
   __HAL_RCC_PLL_CONFIG(RCC_PLLSOURCE_HSI_DIV2, RCC_PLL_MUL12);
   /* Enable the main PLL. */
   __HAL_RCC_PLL_ENABLE();

   /* Wait till PLL is ready */
   while (RCC_CR_PLLRDY != (RCC->CR & RCC_CR_PLLRDY))
   {
   }

   /* To correctly read data from FLASH memory, the number of wait states (LATENCY)
   must be correctly programmed according to the frequency of the CPU clock
   (HCLK) of the device. */

#if defined(FLASH_ACR_LATENCY)
   /* Increasing the number of wait states because of higher CPU frequency */
   if (FLASH_LATENCY_1 > __HAL_FLASH_GET_LATENCY())
   {
      /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
      __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_1);
   }

#endif /* FLASH_ACR_LATENCY */
   /*-------------------------- HCLK Configuration --------------------------*/
   /* Set the highest APBx dividers in order to ensure that we do not go through
   a non-spec phase whatever we decrease or increase HCLK. */
   /* Set the new HCLK clock divider */
   MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2 | RCC_CFGR_HPRE, RCC_HCLK_DIV16 | (RCC_HCLK_DIV16 << 3) | RCC_SYSCLK_DIV1);

   /*------------------------- SYSCLK Configuration ---------------------------*/
   __HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_PLLCLK);

   while (__HAL_RCC_GET_SYSCLK_SOURCE() != (RCC_SYSCLKSOURCE_PLLCLK << RCC_CFGR_SWS_Pos))
   {
   }

#if defined(FLASH_ACR_LATENCY)
   /* Decreasing the number of wait states because of lower CPU frequency */
   if (FLASH_LATENCY_1 < __HAL_FLASH_GET_LATENCY())
   {
      /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
      __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_1);
   }
#endif /* FLASH_ACR_LATENCY */

   /*-------------------------- PCLK1 & PCLK2 Configuration ---------------------------*/
   MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2, RCC_HCLK_DIV2 | ((RCC_HCLK_DIV1) << 3));
}



void SystemInit(void)
{
}

static void main_rcc_deinit_all(void)
{
   /* Full peripheral reset */
   RCC->APB1RSTR |= __RCC_APB1Periph_ALL;
   RCC->APB2RSTR |= __RCC_APB2Periph_ALL;
   /* Disable APB2 Peripheral Reset */
   RCC->APB2RSTR = 0x00000000;

   /* Disable APB1 Peripheral Reset */
   RCC->APB1RSTR = 0x00000000;

   /* FLITF and SRAM Clock ON */
   RCC->AHBENR = 0x00000014;

   /* Disable APB2 Peripheral Clock */
   RCC->APB2ENR = 0x00000000;

   /* Disable APB1 Peripheral Clock */
   RCC->APB1ENR = 0x00000000;

   /* Set HSI as system clock */
   RCC->CFGR &= ~((uint32_t)RCC_CFGR_SW_Msk);

   while(RCC_CFGR_SWS_HSI != (RCC->CFGR & RCC_CFGR_SWS))
   {
   }

   /* Disable the main PLL. */
   __HAL_RCC_PLL_DISABLE();

   while(RCC_CR_PLLRDY == (RCC->CR & RCC_CR_PLLRDY))
   {
   }

   /* Reset all PLL and other parameters */
   RCC->CFGR = 0;

   /* Reset all RCC parameters */
   RCC->CR = 0x83;

   /* Disable all interrupts */
   RCC->CIR = 0x00000000;

   while(0x83 != (RCC->CR & 0xFFFF00FF))
   {
   }

   while(0 != RCC->CFGR)
   {
   }
}

// -----------------------------------------------------------------------------
//  main
// -----------------------------------------------------------------------------
int main(void)
{
   main_rcc_deinit_all();
#if (PREFETCH_ENABLE != 0)
#if defined(STM32F101x6) || defined(STM32F101xB) || defined(STM32F101xE) || defined(STM32F101xG) || \
    defined(STM32F102x6) || defined(STM32F102xB) || \
    defined(STM32F103x6) || defined(STM32F103xB) || defined(STM32F103xE) || defined(STM32F103xG) || \
    defined(STM32F105xC) || defined(STM32F107xC)

   /* Prefetch buffer is not available on value line devices */
   __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
#endif
#endif /* PREFETCH_ENABLE */

   NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

   RCC->APB1ENR |= RCC_APB1ENR_PWREN;
   RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;

   /* Configure the system clock */
   SystemClock_Config();

   /* Initialize LED and FORCE pins */
   GPIO_Init_Input_With_EXTI(FORCE_PORT, FORCE_PIN, FORCE_PIN_PULL, FORCE_PIN_MODE);

#if(BOOT_LED_NOTOFICATION_SUPPORT)
   GPIO_Init_Output_Push_Pull(LED1_PORT, LED1_PIN);

   LED_OFF();
#endif

   if(IS_FORCE_ACTIVE() || USBD_BOOL_IS_FALSE(Is_App_Correct()))
   {
      main_usbd_init();

      /* Infinite loop */
      while (1)
      {
      }
   }
   else
   {
      main_rcc_deinit_all();

      Call_User_App_Reset_Handler();
   }
} /* main */

/*! \file main.c
   \brief Main application's entry point

*/

