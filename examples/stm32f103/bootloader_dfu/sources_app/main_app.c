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



void SystemClock_Config(void)
{
   RCC_OscInitTypeDef RCC_OscInitStruct = {0};
   RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

   /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
   RCC_OscInitStruct.HSIState = RCC_HSI_ON;
   RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
   RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
   (void)HAL_RCC_OscConfig(&RCC_OscInitStruct);

   /** Initializes the CPU, AHB and APB buses clocks
   */
   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

   (void)HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
}


void SystemInit(void)
{
}

void main_rcc_deinit_all(void)
{
   // Full peripheral reset
   RCC->APB1RSTR |= __RCC_APB1Periph_ALL;
   RCC->APB2RSTR |= __RCC_APB2Periph_ALL;
   // Disable APB2 Peripheral Reset
   RCC->APB2RSTR = 0x00000000;

   // Disable APB1 Peripheral Reset
   RCC->APB1RSTR = 0x00000000;

   // FLITF and SRAM Clock ON
   RCC->AHBENR = 0x00000014;

   // Disable APB2 Peripheral Clock
   RCC->APB2ENR = 0x00000000;

   // Disable APB1 Peripheral Clock
   RCC->APB1ENR = 0x00000000;

   // Set HSI as system clock
    RCC->CFGR &= ~((uint32_t)0x3);

    // Disable PLL
    *(volatile uint32_t *) CR_PLLON_BB = (uint32_t)0;

    // Reset all PLL and other parameters
    RCC->CFGR = 0;

    // Reset all RCC parameters
    RCC->CR = 0x83;

    // Disable all interrupts
    RCC->CIR = 0x00000000;
}

// -----------------------------------------------------------------------------
//  main
// -----------------------------------------------------------------------------
int main(void)
{
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

   __HAL_RCC_GPIOA_CLK_ENABLE();
   __HAL_RCC_GPIOB_CLK_ENABLE();
   __HAL_RCC_GPIOC_CLK_ENABLE();
   __HAL_RCC_AFIO_CLK_ENABLE();
   __HAL_RCC_PWR_CLK_ENABLE();
   __HAL_AFIO_REMAP_SWJ_ENABLE();

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

