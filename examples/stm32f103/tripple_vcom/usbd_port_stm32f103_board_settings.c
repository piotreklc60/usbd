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
#include "stm32f10x_conf.h"


#define RCC_OFFSET            (RCC_BASE - PERIPH_BASE)
#define CFGR_OFFSET            (RCC_OFFSET + 0x04)
#define USBPRE_BitNumber        0x16
#define CFGR_USBPRE_BB         (PERIPH_BB_BASE + (CFGR_OFFSET * 32) + (USBPRE_BitNumber * 4))

#define STM32F103_USB_HP_CAN1_TX_IRQ_ID      35
#define STM32F103_USB_LP_CAN1_RX0_IRQ_ID     36
#define STM32F103_USBWAKEUP_IRQ_ID           58

extern USBD_Atomic_Bool_DT port_stm32_cat_a_irq_active;
USBD_Atomic_Bool_DT port_stm32f103_irq_enabled;

#if (USBD_FEATURE_PRESENT == USBD_PORT_STM32F103_DETECT_VBUS_CHANGE)

static USBD_Bool_DT usbd_port_stm32f103_vbus_irq_state = USBD_FALSE;

void USBD_Port_STM32_CAT_A_Configure_Vbus_Detection_Irq(USBD_Bool_DT configure)
{
   GPIO_InitTypeDef gpio_data;
   NVIC_InitTypeDef USBD_NVIC_InitStructure;
   EXTI_InitTypeDef USBD_EXTInit;

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_DEBUG_HI_1(USBD_DBG_PORT_DEV, "configure Vbus to %s", USBD_BOOL_IS_TRUE(configure) ? "ON" : "OFF");

   if(USBD_BOOL_IS_TRUE(configure))
   {
      /* choose port for IRQ */
      GPIO_EXTILineConfig(USBD_VBUS_GPIO_PortSource, USBD_VBUS_GPIO_PinSource);

      /* initialize GPIO line */
      gpio_data.GPIO_Pin   = USBD_VBUS_PIN;
      gpio_data.GPIO_Speed = GPIO_Speed_50MHz;
      gpio_data.GPIO_Mode  = USBD_VBUS_MODE;
      GPIO_Init(USBD_VBUS_PORT, &gpio_data);

      /* configure IRQ in NVIC controller */
      USBD_NVIC_InitStructure.NVIC_IRQChannel = USBD_VBUS_EXTI_IRQChannel;
      USBD_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
      USBD_NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      USBD_NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&USBD_NVIC_InitStructure);

      /* turn ON IRQ */
      USBD_EXTInit.EXTI_Line     = USBD_VBUS_EXTI_Line;
      USBD_EXTInit.EXTI_LineCmd  = ENABLE;
      USBD_EXTInit.EXTI_Mode     = EXTI_Mode_Interrupt;
      USBD_EXTInit.EXTI_Trigger  = EXTI_Trigger_Rising_Falling;
      EXTI_Init(&USBD_EXTInit);

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
      /* turn OFF IRQ */
      USBD_EXTInit.EXTI_Line     = USBD_VBUS_EXTI_Line;
      USBD_EXTInit.EXTI_LineCmd  = DISABLE;
      USBD_EXTInit.EXTI_Mode     = EXTI_Mode_Interrupt;
      USBD_EXTInit.EXTI_Trigger  = EXTI_Trigger_Rising_Falling;
      EXTI_Init(&USBD_EXTInit);

      /* configure IRQ in NVIC controller */
      USBD_NVIC_InitStructure.NVIC_IRQChannel = USBD_VBUS_EXTI_IRQChannel;
      USBD_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
      USBD_NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      USBD_NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
      NVIC_Init(&USBD_NVIC_InitStructure);

      /* initialize GPIO line */
      gpio_data.GPIO_Pin   = USBD_VBUS_PIN;
      gpio_data.GPIO_Speed = GPIO_Speed_2MHz;
      gpio_data.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
      GPIO_Init(USBD_VBUS_PORT, &gpio_data);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}



void USBD_Port_STM32F103_Vbus_IrqHandler(void)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   /* clear IRQ flag */
   EXTI_ClearITPendingBit(USBD_VBUS_EXTI_Line);

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



#if (USBD_FEATURE_PRESENT == USBD_PORT_STM32F103_USE_PULL_UP)

void USBD_Port_STM32_CAT_A_Configure_Pull_Up(USBD_Bool_DT configure)
{
   GPIO_InitTypeDef gpio_data;

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_DEBUG_HI_1(USBD_DBG_PORT_DEV, "%sconfigure Pull-UP", USBD_BOOL_IS_TRUE(configure) ? "" : "de");

   if(USBD_BOOL_IS_TRUE(configure))
   {
      gpio_data.GPIO_Speed = GPIO_Speed_50MHz;
      gpio_data.GPIO_Pin   = USBD_DP_PULL_UP_PIN;
      gpio_data.GPIO_Mode  = USBD_DP_PULL_UP_MODE;
      GPIO_Init(USBD_DP_PULL_UP_PORT, &gpio_data);

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
      gpio_data.GPIO_Speed = GPIO_Speed_2MHz;
      gpio_data.GPIO_Pin   = USBD_DP_PULL_UP_PIN;
      gpio_data.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
      GPIO_Init(USBD_DP_PULL_UP_PORT, &gpio_data);
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
   NVIC_InitTypeDef USBD_NVIC_InitStructure;
   EXTI_InitTypeDef USBD_EXTI_InitStructure;

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_DEBUG_HI_1(USBD_DBG_PORT_DEV, "configure IRQs to %s", USBD_BOOL_IS_TRUE(configure) ? "ON" : "OFF");

   USBD_ATOMIC_BOOL_SET(port_stm32f103_irq_enabled, configure);

   if(USBD_BOOL_IS_TRUE(configure))
   {
      USBD_ATOMIC_BOOL_SET(port_stm32_cat_a_irq_active, USBD_FALSE);
      /* configure USB WakeUp IRQ */
      USBD_NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQChannel;
      USBD_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
      USBD_NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      USBD_NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&USBD_NVIC_InitStructure);

      USBD_EXTI_InitStructure.EXTI_Line = EXTI_Line18;
      USBD_EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
      USBD_EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
      USBD_EXTI_InitStructure.EXTI_LineCmd = ENABLE;
      EXTI_Init(&USBD_EXTI_InitStructure);

      /* configure lower priority USB Dev IRQ */
      USBD_NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN_RX0_IRQChannel;
      USBD_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
      USBD_NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      USBD_NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&USBD_NVIC_InitStructure);

      /* configure higher priority (data transfer) USB Dev IRQ */
      USBD_NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN_TX_IRQChannel;
      USBD_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
      USBD_NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      USBD_NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&USBD_NVIC_InitStructure);
   }
   else
   {
      /* deconfigure USB WakeUp IRQ */
      USBD_NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQChannel;
      USBD_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
      USBD_NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      USBD_NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
      NVIC_Init(&USBD_NVIC_InitStructure);

      /* deconfigure lower priority USB Dev IRQ */
      USBD_NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN_RX0_IRQChannel;
      USBD_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
      USBD_NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      USBD_NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
      NVIC_Init(&USBD_NVIC_InitStructure);

      /* deconfigure higher priority (data transfer) USB Dev IRQ */
      USBD_NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN_TX_IRQChannel;
      USBD_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
      USBD_NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      USBD_NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
      NVIC_Init(&USBD_NVIC_InitStructure);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

void USBD_Port_STM32_CAT_A_Clear_Exti_Line_18_Irq_Status(void)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_IRQ);

   if(RESET != EXTI_GetITStatus(EXTI_Line18))
   {
      USBD_DEBUG_MID(USBD_DBG_PORT_IRQ, "EXTI_Line18 cleared");

      EXTI_ClearITPendingBit(EXTI_Line18);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IRQ);
}

bool_t USBD_Port_STM32F103_OS_Is_Usbd_Context(OS_Invoke_XT *invoke_params, OS_Context_Id current_context_unique_id)
{
   bool_t result = true;

   if((STM32F103_USB_HP_CAN1_TX_IRQ_ID == current_context_unique_id)
      || (STM32F103_USB_LP_CAN1_RX0_IRQ_ID == current_context_unique_id)
      || (STM32F103_USBWAKEUP_IRQ_ID == current_context_unique_id))
   {
      result = false;
   }

   return result;
}


/**
  \brief   Enable Interrupt
  \details Enables a device specific interrupt in the NVIC interrupt controller.
  \param [in]      IRQn  Device specific interrupt number.
  \note    IRQn must not be negative.
 */
static inline void nvic_enable_irq(uint32_t irq_id)
{
   NVIC->ISER[(irq_id >> 5UL)] = (uint32_t)(1UL << (irq_id & 0x1FUL));
}



/**
  \brief   Disable Interrupt
  \details Disables a device specific interrupt in the NVIC interrupt controller.
  \param [in]      IRQn  Device specific interrupt number.
  \note    IRQn must not be negative.
 */
static inline void nvic_disable_irq(uint32_t irq_id)
{
   NVIC->ICER[(irq_id >> 5UL)] = (uint32_t)(1UL << (irq_id & 0x1FUL));
   __DSB();
   __ISB();
}

bool_t USBD_Port_STM32F103_OS_Irq_Lock (struct OS_Invoke_eXtendedTag *invoke_params)
{
   bool_t result = false;

   if(USBD_ATOMIC_BOOL_IS_FALSE(port_stm32_cat_a_irq_active))
   {
      if(USBD_ATOMIC_BOOL_IS_TRUE(port_stm32f103_irq_enabled))
      {
         nvic_disable_irq(USB_HP_CAN_TX_IRQChannel);
         nvic_disable_irq(USB_LP_CAN_RX0_IRQChannel);
         nvic_disable_irq(USBWakeUp_IRQChannel);
      }
      result = true;
   }

   return result;
}

void USBD_Port_STM32F103_OS_Irq_Unlock (struct OS_Invoke_eXtendedTag *invoke_params)
{
   if(USBD_ATOMIC_BOOL_IS_TRUE(port_stm32f103_irq_enabled))
   {
      nvic_enable_irq(USBWakeUp_IRQChannel);
      nvic_enable_irq(USB_LP_CAN_RX0_IRQChannel);
      nvic_enable_irq(USB_HP_CAN_TX_IRQChannel);
   }
}


