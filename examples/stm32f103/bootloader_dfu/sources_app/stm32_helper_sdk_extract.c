/** ---------------------------------------- LICENSE SECTION -------------------------------------------------------------------
 *
 * This file is modified version of STMicroelectronics software.
 *
 * Copyright (c) 2025 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
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

HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef  *RCC_OscInitStruct)
{
   /*----------------------------- HSI Configuration --------------------------*/
   if (((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_HSI) == RCC_OSCILLATORTYPE_HSI)
   {
      /* Check if HSI is used as system clock or as PLL source when PLL is selected as system clock */
      if ((__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_HSI)
      || ((__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_PLLCLK) && (__HAL_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_HSI_DIV2)))
      {
         /* When HSI is used as system clock it will not disabled */
         if ((__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) != RESET) && (RCC_OscInitStruct->HSIState != RCC_HSI_ON))
         {
            return HAL_ERROR;
         }
         /* Otherwise, just the calibration is allowed */
         else
         {
            /* Adjusts the Internal High Speed oscillator (HSI) calibration value.*/
            __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(RCC_OscInitStruct->HSICalibrationValue);
         }
      }
      else
      {
         /* Check the HSI State */
         if (RCC_OscInitStruct->HSIState != RCC_HSI_OFF)
         {
            /* Enable the Internal High Speed oscillator (HSI). */
            __HAL_RCC_HSI_ENABLE();

            /* Wait till HSI is ready */
            while (__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET)
            {
            }

            /* Adjusts the Internal High Speed oscillator (HSI) calibration value.*/
            __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(RCC_OscInitStruct->HSICalibrationValue);
         }
         else
         {
            /* Disable the Internal High Speed oscillator (HSI). */
            __HAL_RCC_HSI_DISABLE();

            /* Wait till HSI is disabled */
            while (__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) != RESET)
            {
            }
         }
      }
   }

   /*-------------------------------- PLL Configuration -----------------------*/
   if ((RCC_OscInitStruct->PLL.PLLState) != RCC_PLL_NONE)
   {
      /* Check if the PLL is used as system clock or not */
      if (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_PLLCLK)
      {
         if ((RCC_OscInitStruct->PLL.PLLState) == RCC_PLL_ON)
         {
            /* Disable the main PLL. */
            __HAL_RCC_PLL_DISABLE();

            /* Wait till PLL is disabled */
            while (__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY)  != RESET)
            {
            }

            /* Configure the main PLL clock source and multiplication factors. */
            __HAL_RCC_PLL_CONFIG(RCC_OscInitStruct->PLL.PLLSource,
            RCC_OscInitStruct->PLL.PLLMUL);
            /* Enable the main PLL. */
            __HAL_RCC_PLL_ENABLE();

            /* Wait till PLL is ready */
            while (__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY)  == RESET)
            {
            }
         }
         else
         {
            /* REMOVED TO REDUCE FUNCTION SIZE */
         }
      }
      else
      {
         /* REMOVED TO REDUCE FUNCTION SIZE */
      }
   }

   return HAL_OK;
}

HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef  *RCC_ClkInitStruct, uint32_t FLatency)
{
   /* To correctly read data from FLASH memory, the number of wait states (LATENCY)
   must be correctly programmed according to the frequency of the CPU clock
   (HCLK) of the device. */

#if defined(FLASH_ACR_LATENCY)
   /* Increasing the number of wait states because of higher CPU frequency */
   if (FLatency > __HAL_FLASH_GET_LATENCY())
   {
      /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
      __HAL_FLASH_SET_LATENCY(FLatency);

      /* Check that the new number of wait states is taken into account to access the Flash
      memory by reading the FLASH_ACR register */
      if (__HAL_FLASH_GET_LATENCY() != FLatency)
      {
         return HAL_ERROR;
      }
   }

#endif /* FLASH_ACR_LATENCY */
   /*-------------------------- HCLK Configuration --------------------------*/
   if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_HCLK) == RCC_CLOCKTYPE_HCLK)
   {
      /* Set the highest APBx dividers in order to ensure that we do not go through
      a non-spec phase whatever we decrease or increase HCLK. */
      if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK1) == RCC_CLOCKTYPE_PCLK1)
      {
         MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_HCLK_DIV16);
      }

      if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK2) == RCC_CLOCKTYPE_PCLK2)
      {
         MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, (RCC_HCLK_DIV16 << 3));
      }

      /* Set the new HCLK clock divider */
      assert_param(IS_RCC_HCLK(RCC_ClkInitStruct->AHBCLKDivider));
      MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_ClkInitStruct->AHBCLKDivider);
   }

   /*------------------------- SYSCLK Configuration ---------------------------*/
   if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_SYSCLK) == RCC_CLOCKTYPE_SYSCLK)
   {
      /* PLL is selected as System Clock Source */
      if (RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_PLLCLK)
      {
         /* Check the PLL ready flag */
         if (__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET)
         {
            return HAL_ERROR;
         }
      }
      __HAL_RCC_SYSCLK_CONFIG(RCC_ClkInitStruct->SYSCLKSource);

      while (__HAL_RCC_GET_SYSCLK_SOURCE() != (RCC_ClkInitStruct->SYSCLKSource << RCC_CFGR_SWS_Pos))
      {
      }
   }

#if defined(FLASH_ACR_LATENCY)
   /* Decreasing the number of wait states because of lower CPU frequency */
   if (FLatency < __HAL_FLASH_GET_LATENCY())
   {
      /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
      __HAL_FLASH_SET_LATENCY(FLatency);

      /* Check that the new number of wait states is taken into account to access the Flash
      memory by reading the FLASH_ACR register */
      if (__HAL_FLASH_GET_LATENCY() != FLatency)
      {
         return HAL_ERROR;
      }
   }
#endif /* FLASH_ACR_LATENCY */

   /*-------------------------- PCLK1 Configuration ---------------------------*/
   if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK1) == RCC_CLOCKTYPE_PCLK1)
   {
      assert_param(IS_RCC_PCLK(RCC_ClkInitStruct->APB1CLKDivider));
      MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_ClkInitStruct->APB1CLKDivider);
   }

   /*-------------------------- PCLK2 Configuration ---------------------------*/
   if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK2) == RCC_CLOCKTYPE_PCLK2)
   {
      assert_param(IS_RCC_PCLK(RCC_ClkInitStruct->APB2CLKDivider));
      MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, ((RCC_ClkInitStruct->APB2CLKDivider) << 3));
   }

   return HAL_OK;
}

void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
{
  uint32_t prioritygroup = 0x00U;

  prioritygroup = NVIC_GetPriorityGrouping();

  NVIC_SetPriority(IRQn, NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority));
}

void HAL_NVIC_EnableIRQ(IRQn_Type IRQn)
{
  /* Enable interrupt */
  NVIC_EnableIRQ(IRQn);
}

void HAL_NVIC_DisableIRQ(IRQn_Type IRQn)
{
  /* Disable interrupt */
  NVIC_DisableIRQ(IRQn);
}
