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

#include "user_app_handle.h"
#include "stm32f1xx_hal.h"
#include "boot_config.h"
#include "cmsis_gcc.h"

#define SCB                   ((SCB_Type       *)     SCB_BASE      )   /*!< SCB configuration struct */

#define FLASH_MEM_BASE        0x8000000

#define APP_VEC_TAB_OFFSET    ((DEF_APP_ADDRESS - FLASH_MEM_BASE) & 0x1FFFFF80)

USBD_Bool_DT Is_App_Correct(void)
{
   App_Vector_Table_Begining_XT *app_vector_isr = (App_Vector_Table_Begining_XT *)DEF_APP_ADDRESS;

   // app seems to be incorrect
   if ((0xFFFFFFFF == app_vector_isr->initial_stack_pointer)
      || (0x00000000 == app_vector_isr->initial_stack_pointer)
      || ((Irq_Event_Handler_HT)0xFFFFFFFF == app_vector_isr->reset)
      || ((Irq_Event_Handler_HT)0x00000000 == app_vector_isr->reset))
   {
      return USBD_FALSE;
   }
   return USBD_TRUE;
}

void Call_User_App_Reset_Handler(void)
{
   App_Vector_Table_Begining_XT *app_vector_isr = (App_Vector_Table_Begining_XT *)DEF_APP_ADDRESS;

   /* Set the vector table address to the beginning of the application. */
   SCB->VTOR = APP_VEC_TAB_OFFSET;

   /* Load the initial LR as it should be after the reset. */
   asm("movs    lr, #0xffffffff");

   /* Set the main stack pointer from the application's vector table. */
   __set_MSP(app_vector_isr->initial_stack_pointer);

   /* Jump  to the reset handler from the application's vector table */
   app_vector_isr->reset();
}

