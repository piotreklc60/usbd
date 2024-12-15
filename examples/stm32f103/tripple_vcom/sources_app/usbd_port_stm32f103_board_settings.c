/** ---------------------------------------- LICENSE SECTION -------------------------------------------------------------------
 *
 * Copyright (c) 2024 Piotr Wojtowicz
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

#include "usbd_port_stm32f103_board_settings_cfg.h"
#include "usbd_port_stm32_cat_a_board_settings.h"
#include "stm32f1xx_hal.h"


#define RCC_OFFSET            (RCC_BASE - PERIPH_BASE)
#define CFGR_OFFSET            (RCC_OFFSET + 0x04)
#define USBPRE_BitNumber        0x16
#define CFGR_USBPRE_BB         (PERIPH_BB_BASE + (CFGR_OFFSET * 32) + (USBPRE_BitNumber * 4))

#define STM32_CAT_A_USB_HP_CAN1_TX_IRQ_ID      35
#define STM32_CAT_A_USB_LP_CAN1_RX0_IRQ_ID     36
#define STM32_CAT_A_USBWAKEUP_IRQ_ID           58

#define STM32_CAT_A_IS_EXTI_PENDING(bit_num)    (0 != (EXTI->PR & (1 << (bit_num))))
#define STM32_CAT_A_CLEAR_EXTI_PENDING(bit_num) EXTI->PR = (1 << (bit_num))

extern USBD_Atomic_Bool_DT port_stm32_cat_a_irq_active;
USBD_Atomic_Bool_DT port_stm32f103_irq_enabled;
static EXTI_HandleTypeDef port_stm32f103_exti_wkup_handle;

#if (USBD_FEATURE_PRESENT == USBD_PORT_STM32_CAT_A_DETECT_VBUS_CHANGE)

static USBD_Bool_DT usbd_port_stm32f103_vbus_irq_state = USBD_FALSE;

void USBD_Port_STM32_CAT_A_Configure_Vbus_Detection_Irq(USBD_Bool_DT configure)
{
   GPIO_InitTypeDef gpio_data;

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_DEBUG_HI_1(USBD_DBG_PORT_DEV, "configure Vbus to %s", USBD_BOOL_IS_TRUE(configure) ? "ON" : "OFF");

   if(USBD_BOOL_IS_TRUE(configure))
   {
      /* initialize GPIO line */
      gpio_data.Pin   = USBD_VBUS_PIN;
      gpio_data.Mode  = USBD_VBUS_MODE;
      gpio_data.Pull  = USBD_VBUS_PULL;
      HAL_GPIO_Init(USBD_VBUS_PORT, &gpio_data);

      /* configure IRQ in NVIC controller */
      HAL_NVIC_SetPriority(USBD_VBUS_EXTI_IRQChannel, 4, 0);
      HAL_NVIC_EnableIRQ(USBD_VBUS_EXTI_IRQChannel);

      /* check if VBUS is already active */
#if(USBD_VBUS_ACTIVE_STATE)
      if(0 != (USBD_VBUS_PORT->IDR & USBD_VBUS_PIN))
#else
      if(0 == (USBD_VBUS_PORT->IDR & USBD_VBUS_PIN))
#endif
      {
         usbd_port_stm32f103_vbus_irq_state = USBD_TRUE;

         USBD_Port_STM32_CAT_A_Vbus_Detection(USBD_TRUE);
      }
   }
   else
   {
      /* initialize GPIO line */
      HAL_GPIO_DeInit(USBD_VBUS_PORT, USBD_VBUS_PIN);

      /* configure IRQ in NVIC controller */
      HAL_NVIC_DisableIRQ(USBD_VBUS_EXTI_IRQChannel);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}



void USBD_Port_STM32_CAT_A_Vbus_IrqHandler(void)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   /* clear IRQ flag */
   STM32_CAT_A_CLEAR_EXTI_PENDING(USBD_VBUS_PIN);

#if(USBD_VBUS_ACTIVE_STATE)
   if(0 != (USBD_VBUS_PORT->IDR & USBD_VBUS_PIN))
#else
   if(0 == (USBD_VBUS_PORT->IDR & USBD_VBUS_PIN))
#endif
   {
      if(USBD_BOOL_IS_FALSE(usbd_port_stm32f103_vbus_irq_state))
      {
         usbd_port_stm32f103_vbus_irq_state = USBD_TRUE;
         USBD_Port_STM32_CAT_A_Vbus_Detection(USBD_TRUE);
      }
   }
   else
   {
      if(USBD_BOOL_IS_TRUE(usbd_port_stm32f103_vbus_irq_state))
      {
         usbd_port_stm32f103_vbus_irq_state = USBD_FALSE;
         USBD_Port_STM32_CAT_A_Vbus_Detection(USBD_FALSE);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

#endif



#if (USBD_FEATURE_PRESENT == USBD_PORT_STM32_CAT_A_USE_PULL_UP)

void USBD_Port_STM32_CAT_A_Configure_Pull_Up(USBD_Bool_DT configure)
{
   GPIO_InitTypeDef gpio_data;

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_DEBUG_HI_1(USBD_DBG_PORT_DEV, "%sconfigure Pull-UP", USBD_BOOL_IS_TRUE(configure) ? "" : "de");

   if(USBD_BOOL_IS_TRUE(configure))
   {
      gpio_data.Pin   = USBD_DP_PULL_UP_PIN;
      gpio_data.Mode  = USBD_DP_PULL_UP_MODE;
      gpio_data.Pull  = USBD_DP_PULL_UP_PULL;
      gpio_data.Speed = GPIO_SPEED_FREQ_HIGH;
      HAL_GPIO_Init(USBD_DP_PULL_UP_PORT, &gpio_data);

      /* disable pin */
#if(USBD_DP_PULL_UP_ACTIVE_STATE)
      USBD_DP_PULL_UP_PORT->BRR = USBD_DP_PULL_UP_PIN;
#else
      USBD_DP_PULL_UP_PORT->BSRR = USBD_DP_PULL_UP_PIN;
#endif
   }
   else
   {
      /* disable */
      HAL_GPIO_DeInit(USBD_DP_PULL_UP_PORT, USBD_DP_PULL_UP_PIN);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}



void USBD_Port_STM32_CAT_A_Pull_Up_Change(USBD_Bool_DT attach_pull_up_resistor)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_DEBUG_HI_1(USBD_DBG_PORT_DEV, "Pull-UP change to %s", USBD_BOOL_IS_TRUE(attach_pull_up_resistor) ? "ON" : "OFF");

   if(USBD_BOOL_IS_TRUE(attach_pull_up_resistor))
   {
#if(USBD_DP_PULL_UP_ACTIVE_STATE)
      USBD_DP_PULL_UP_PORT->BSRR = USBD_DP_PULL_UP_PIN;
#else
      USBD_DP_PULL_UP_PORT->BRR  = USBD_DP_PULL_UP_PIN;
#endif
   }
   else
   {
#if(USBD_DP_PULL_UP_ACTIVE_STATE)
      USBD_DP_PULL_UP_PORT->BRR = USBD_DP_PULL_UP_PIN;
#else
      USBD_DP_PULL_UP_PORT->BSRR = USBD_DP_PULL_UP_PIN;
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
   EXTI_ConfigTypeDef exti_init_structure;

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_DEBUG_HI_1(USBD_DBG_PORT_DEV, "configure IRQs to %s", USBD_BOOL_IS_TRUE(configure) ? "ON" : "OFF");

   USBD_ATOMIC_BOOL_SET(port_stm32f103_irq_enabled, configure);

   if(USBD_BOOL_IS_TRUE(configure))
   {
      USBD_ATOMIC_BOOL_SET(port_stm32_cat_a_irq_active, USBD_FALSE);
      /* configure USB WakeUp IRQ */
      HAL_NVIC_SetPriority(USBWakeUp_IRQn, 4, 0);
      HAL_NVIC_EnableIRQ(USBWakeUp_IRQn);

      exti_init_structure.Line = EXTI_LINE_18;
      exti_init_structure.Mode = EXTI_MODE_INTERRUPT;
      exti_init_structure.Trigger = EXTI_TRIGGER_RISING;
      exti_init_structure.GPIOSel = 0;
      HAL_EXTI_SetConfigLine(&port_stm32f103_exti_wkup_handle, &exti_init_structure);

      /* configure lower priority USB Dev IRQ */
      HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 3, 0);
      HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

      /* configure higher priority (data transfer) USB Dev IRQ */
      HAL_NVIC_SetPriority(USB_HP_CAN1_TX_IRQn, 2, 0);
      HAL_NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
   }
   else
   {
      /* deconfigure USB WakeUp IRQ */
      HAL_NVIC_DisableIRQ(USBWakeUp_IRQn);

      /* deconfigure lower priority USB Dev IRQ */
      HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);

      /* deconfigure higher priority (data transfer) USB Dev IRQ */
      HAL_NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

void USBD_Port_STM32_CAT_A_Clear_Exti_Line_18_Irq_Status(void)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_IRQ);

   if(STM32_CAT_A_IS_EXTI_PENDING(18))
   {
      USBD_DEBUG_MID(USBD_DBG_PORT_IRQ, "EXTI_Line18 cleared");

      STM32_CAT_A_CLEAR_EXTI_PENDING(18);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IRQ);
}

bool_t USBD_Port_STM32_CAT_A_OS_Is_Usbd_Context(OS_Invoke_XT *invoke_params, OS_Context_Id current_context_unique_id)
{
   bool_t result = true;

   USBD_UNUSED_PARAM(invoke_params);

   if((STM32_CAT_A_USB_HP_CAN1_TX_IRQ_ID == current_context_unique_id)
      || (STM32_CAT_A_USB_LP_CAN1_RX0_IRQ_ID == current_context_unique_id)
      || (STM32_CAT_A_USBWAKEUP_IRQ_ID == current_context_unique_id))
   {
      result = false;
   }

   return result;
}

bool_t USBD_Port_STM32_CAT_A_OS_Irq_Lock (struct OS_Invoke_eXtendedTag *invoke_params)
{
   bool_t result = false;

   USBD_UNUSED_PARAM(invoke_params);

   if(USBD_ATOMIC_BOOL_IS_FALSE(port_stm32_cat_a_irq_active))
   {
      if(USBD_ATOMIC_BOOL_IS_TRUE(port_stm32f103_irq_enabled))
      {
         /* deconfigure higher priority (data transfer) USB Dev IRQ */
         HAL_NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);

         /* deconfigure lower priority USB Dev IRQ */
         HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);

         /* deconfigure USB WakeUp IRQ */
         HAL_NVIC_DisableIRQ(USBWakeUp_IRQn);
      }
      result = true;
   }

   return result;
}

void USBD_Port_STM32_CAT_A_OS_Irq_Unlock (struct OS_Invoke_eXtendedTag *invoke_params)
{
   USBD_UNUSED_PARAM(invoke_params);

   if(USBD_ATOMIC_BOOL_IS_TRUE(port_stm32f103_irq_enabled))
   {
      HAL_NVIC_EnableIRQ(USBWakeUp_IRQn);
      HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
      HAL_NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
   }
}


