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

#ifndef USER_APP_HANDLE_H_
#define USER_APP_HANDLE_H_

#include "std_libs.h"
#include "usbd.h"

typedef void (*Irq_Event_Handler_HT) (void);

typedef struct App_Vector_Table_Begining_eXtendedTag
{
   uint32_t             initial_stack_pointer;
   Irq_Event_Handler_HT reset;
   Irq_Event_Handler_HT nmi;
   Irq_Event_Handler_HT hard_fault;
   Irq_Event_Handler_HT mem_management_fault;
   Irq_Event_Handler_HT bus_fault;
   Irq_Event_Handler_HT usage_fault;
   Irq_Event_Handler_HT reserved1;
   Irq_Event_Handler_HT reserved2;
   Irq_Event_Handler_HT reserved3;
   Irq_Event_Handler_HT reserved4;
   Irq_Event_Handler_HT svcall;
   Irq_Event_Handler_HT debug_mon;
   Irq_Event_Handler_HT reserved5;
   Irq_Event_Handler_HT pend_sv;
   Irq_Event_Handler_HT sys_tick;
} App_Vector_Table_Begining_XT;

USBD_Bool_DT Is_App_Correct(void);

void Call_User_App_Reset_Handler(void);

#endif /*USER_APP_HANDLE_H_*/

