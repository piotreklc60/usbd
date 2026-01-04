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

#ifndef OSAL_H_
#include "osal.h"
#endif

static const char* const osal_stm32f103_irq_name[] =
{
   "---",
   "Reset",          // The reset handler
   "NMI",
   "Fault.Hard",
   "Fault.MemManag",
   "Fault.Bus",
   "Fault.Usage",
   "---", "---", "---", "---",         /* Reserved */
   "SVC",
   "DebugMonitor",
   "---",                 /* Reserved */
   "PendSV",
   "SysTick",
   "WWDG",
   "PVD",
   "TAMPER",
   "RTC",
   "FLASH",
   "RCC",
   "EXTI.0",
   "EXTI.1",
   "EXTI.2",
   "EXTI.3",
   "EXTI.4",
   "DMA.1",
   "DMA.2",
   "DMA.3",
   "DMA.4",
   "DMA.5",
   "DMA.6",
   "DMA.7",
   "ADC",
   "USB.HP_CAN1_TX",
   "USB.LP_CAN1_RX0",
   "CAN.RX1",
   "CAN.SCE",
   "EXTI.9_5",
   "TIM.1.BRK",
   "TIM.1.UP",
   "TIM.1.TRG_COM",
   "TIM.1.CC",
   "TIM.2",
   "TIM.3",
   "TIM.4",
   "I2C.1.EV",
   "I2C.1.ER",
   "I2C.2.EV",
   "I2C.2.ER",
   "SPI.1",
   "SPI.2",
   "USART.1",
   "USART.2",
   "USART.3",
   "EXTI.15_10",
   "RTC.Alarm",
   "USB.WakeUp"
};

const char *OSAL_Get_Interrupt_Name(OSAL_Context_Unique_Id_DT irq_id)
{
   const char *result = "---";

   if(irq_id < Num_Elems(osal_stm32f103_irq_name))
   {
      result = osal_stm32f103_irq_name[irq_id];
   }

   return result;
} /* OSAL_Get_Interrupt_Name */

