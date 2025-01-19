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

#include "usbd_port_stm32_cat_a_board_settings_cfg.h"
#include "usbd_port_stm32_cat_a_board_settings.h"
#include "stm32f1xx_hal.h"
#include "stm32_helper.h"


#define RCC_OFFSET            (RCC_BASE - PERIPH_BASE)
#define CFGR_OFFSET            (RCC_OFFSET + 0x04)
#define USBPRE_BitNumber        0x16
#define CFGR_USBPRE_BB         (PERIPH_BB_BASE + (CFGR_OFFSET * 32) + (USBPRE_BitNumber * 4))

#define STM32_CAT_A_USB_HP_CAN1_TX_IRQ_ID      35
#define STM32_CAT_A_USB_LP_CAN1_RX0_IRQ_ID     36
#define STM32_CAT_A_USBWAKEUP_IRQ_ID           58

#define STM32_CAT_A_IS_EXTI_PENDING(exti_pin)      (0 != (EXTI->PR & (exti_pin)))
#define STM32_CAT_A_CLEAR_EXTI_PENDING(exti_pin)   EXTI->PR = (exti_pin)

extern USBD_Atomic_Bool_DT port_stm32_cat_a_irq_active;
USBD_Atomic_Bool_DT port_stm32f103_irq_enabled;



#if (USBD_FEATURE_PRESENT == USBD_PORT_STM32_CAT_A_USE_PULL_UP)

void USBD_Port_STM32_CAT_A_Configure_Pull_Up(USBD_Bool_DT configure)
{
#if(USBD_FEATURE_PRESENT != USBD_MULTI_SESSION_SUPPORTED)
   USBD_UNUSED_PARAM(configure);
#endif

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_DEBUG_HI_1(USBD_DBG_PORT_DEV, "%sconfigure Pull-UP", USBD_BOOL_IS_TRUE(configure) ? "" : "de");

#if(USBD_FEATURE_PRESENT == USBD_MULTI_SESSION_SUPPORTED)
   if(USBD_BOOL_IS_TRUE(configure))
#endif
   {
      GPIO_Init_Output_Push_Pull(USBD_DP_PULL_UP_PORT, USBD_DP_PULL_UP_PIN);

      /* disable pin */
#if(USBD_DP_PULL_UP_ACTIVE_STATE)
      USBD_DP_PULL_UP_PORT->BRR = 1 << USBD_DP_PULL_UP_PIN;
#else
      USBD_DP_PULL_UP_PORT->BSRR = 1 << USBD_DP_PULL_UP_PIN;
#endif
   }
#if(USBD_FEATURE_PRESENT == USBD_MULTI_SESSION_SUPPORTED)
   else
   {
      /* disable */
      GPIO_Deinit(USBD_DP_PULL_UP_PORT, USBD_DP_PULL_UP_PIN);
   }
#endif

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}



void USBD_Port_STM32_CAT_A_Pull_Up_Change(USBD_Bool_DT attach_pull_up_resistor)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_DEBUG_HI_1(USBD_DBG_PORT_DEV, "Pull-UP change to %s", USBD_BOOL_IS_TRUE(attach_pull_up_resistor) ? "ON" : "OFF");

   if(USBD_BOOL_IS_TRUE(attach_pull_up_resistor))
   {
#if(USBD_DP_PULL_UP_ACTIVE_STATE)
      USBD_DP_PULL_UP_PORT->BSRR = 1 << USBD_DP_PULL_UP_PIN;
#else
      USBD_DP_PULL_UP_PORT->BRR  = 1 << USBD_DP_PULL_UP_PIN;
#endif
   }
   else
   {
#if(USBD_DP_PULL_UP_ACTIVE_STATE)
      USBD_DP_PULL_UP_PORT->BRR = 1 << USBD_DP_PULL_UP_PIN;
#else
      USBD_DP_PULL_UP_PORT->BSRR = 1 << USBD_DP_PULL_UP_PIN;
#endif
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

#endif



void USBD_Port_STM32_CAT_A_Configure_USBCLKSource(void)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   *(volatile uint32_t *) CFGR_USBPRE_BB = USBD_RCC_USBCLKSource_PLLCLK_presc;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}



void USBD_Port_STM32_CAT_A_Configure_USB_Irqs(USBD_Bool_DT configure)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_DEBUG_HI_1(USBD_DBG_PORT_DEV, "configure IRQs to %s", USBD_BOOL_IS_TRUE(configure) ? "ON" : "OFF");

   USBD_ATOMIC_BOOL_SET(port_stm32f103_irq_enabled, configure);

#if(USBD_FEATURE_PRESENT == USBD_MULTI_SESSION_SUPPORTED)
   if(USBD_BOOL_IS_TRUE(configure))
#endif
   {
      USBD_ATOMIC_BOOL_SET(port_stm32_cat_a_irq_active, USBD_FALSE);
#if(USBD_FEATURE_PRESENT == USBD_SUSPEND_RESUME_SUPPORTED)
      /* configure USB WakeUp IRQ */
      HAL_NVIC_SetPriority(USBWakeUp_IRQn, 4, 0);
      HAL_NVIC_EnableIRQ(USBWakeUp_IRQn);

      GPIO_Init_EXTI(18, EXTI_TRIGGER_RISING);
#endif

      /* configure lower priority USB Dev IRQ */
      HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 3, 0);
      HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

#if(USBD_MAX_NUM_ENDPOINTS > 1)
      /* configure higher priority (data transfer) USB Dev IRQ */
      HAL_NVIC_SetPriority(USB_HP_CAN1_TX_IRQn, 2, 0);
      HAL_NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
#endif
   }
#if(USBD_FEATURE_PRESENT == USBD_MULTI_SESSION_SUPPORTED)
   else
   {
#if(USBD_FEATURE_PRESENT == USBD_SUSPEND_RESUME_SUPPORTED)
      /* deconfigure USB WakeUp IRQ */
      HAL_NVIC_DisableIRQ(USBWakeUp_IRQn);
#endif
      /* deconfigure lower priority USB Dev IRQ */
      HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);

#if(USBD_MAX_NUM_ENDPOINTS > 1)
      /* deconfigure higher priority (data transfer) USB Dev IRQ */
      HAL_NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
#endif
   }
#endif

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

void USBD_Port_STM32_CAT_A_Clear_Exti_Line_18_Irq_Status(void)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_IRQ);

   if(STM32_CAT_A_IS_EXTI_PENDING(1 << 18))
   {
      USBD_DEBUG_MID(USBD_DBG_PORT_IRQ, "EXTI_Line18 cleared");

      STM32_CAT_A_CLEAR_EXTI_PENDING(1 << 18);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IRQ);
}

