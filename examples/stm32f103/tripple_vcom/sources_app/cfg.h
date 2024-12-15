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

#ifndef CFG_H_
#define CFG_H_

#include "std_libs.h"
#include "gpio.h"
#include "os.h"

#define Num_Elems(tab)    (sizeof(tab) / sizeof(tab[0]))
#define MAX(a,b)          (((a) > (b)) ? (a) : (b))
#define MIN(a,b)          (((a) < (b)) ? (a) : (b))

extern int critical_global_cntr;

#define ENTER_CRITICAL() \
do { \
   __asm volatile ("   CPSID   I"); \
   critical_global_cntr++; \
} while(0)

#define EXIT_CRITICAL() \
do { \
   if(critical_global_cntr > 0) \
   { \
     critical_global_cntr--; \
   } \
   if(0 == critical_global_cntr) \
   { \
     __asm volatile ("   CPSIE   I"); \
   } \
} while(0)


#if 1


#define USBD_DECLARE_INVOKE_PARAMS(_invoke_params)       OS_Invoke_XT _invoke_params;

extern bool_t USBD_Port_STM32F103_OS_Is_Usbd_Context(OS_Invoke_XT *invoke_params, OS_Context_Id current_context_unique_id);
extern bool_t USBD_Port_STM32F103_OS_Irq_Lock (struct OS_Invoke_eXtendedTag *invoke_params);
extern void USBD_Port_STM32F103_OS_Irq_Unlock (struct OS_Invoke_eXtendedTag *invoke_params);

#define USBD_INIT_INVOKE_PARAMS(_invoke_params)          OS_Invoke_Init(&(_invoke_params)); \
                                                         OS_Set_Destination_Id_Vendor_Checker(&(_invoke_params), USBD_Port_STM32F103_OS_Is_Usbd_Context); \
                                                         OS_Set_Destination_Irq_Lock_Unlock(&(_invoke_params), USBD_Port_STM32F103_OS_Irq_Lock, USBD_Port_STM32F103_OS_Irq_Unlock)


#define USBD_MARK_INVOKE_DESTINATION(_invoke_params)     OS_Mark_Invoke_Params(&(_invoke_params))


#define USBD_IS_INVOKE_NEEDED(_invoke_params)            OS_Is_Invoke_Needed(&(_invoke_params))


#define USBD_INVOKE(_invoke_params, _action)             OS_INVOKE(&(_invoke_params), _action)


#else


#define USBD_DECLARE_INVOKE_PARAMS(_invoke_params)       struct { OS_Context_Id invoke_context_id; }_invoke_params;


#define USBD_INIT_INVOKE_PARAMS(_invoke_params)          (_invoke_params).invoke_context_id = OS_MAX_NUM_CONTEXTS


#define USBD_MARK_INVOKE_DESTINATION(_invoke_params)     (_invoke_params).invoke_context_id = OS_Get_Current_Context_Unique_Id();


#define USBD_IS_INVOKE_NEEDED(_invoke_params)            ((_invoke_params).invoke_context_id != OS_Get_Current_Context_Unique_Id())


#define USBD_TRY_INVOKE(_invoke_params, _action)         ((!USBD_IS_INVOKE_NEEDED(_invoke_params)) && _action)


#endif


#endif
