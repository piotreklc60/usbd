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
#include "main.h"
#include "osal.h"

#define PIN_HIGH(pin, port)   (port)->BSRR = (pin)
#define PIN_LOW(pin, port)    (port)->BRR = (pin)



#define LED_OFF()             PIN_HIGH(LED1_Pin, LED1_GPIO_Port)
#define LED_ON()              PIN_LOW(LED1_Pin, LED1_GPIO_Port)



#define Num_Elems(tab)    (sizeof(tab) / sizeof(tab[0]))
#define MAX(a,b)          (((a) > (b)) ? (a) : (b))
#define MIN(a,b)          (((a) < (b)) ? (a) : (b))



#define USBD_DECLARE_INVOKE_PARAMS(_invoke_params)       OSAL_Invoke_XT _invoke_params;

extern OSAL_Bool_DT USBD_Port_STM32_CAT_A_OS_Is_Usbd_Context(OSAL_Invoke_XT *invoke_params, OSAL_Context_Id_XT *current_context_id);
extern OSAL_Bool_DT USBD_Port_STM32_CAT_A_OS_Irq_Lock (OSAL_Invoke_XT *invoke_params);
extern void USBD_Port_STM32_CAT_A_OS_Irq_Unlock (OSAL_Invoke_XT *invoke_params);

#define USBD_INIT_INVOKE_PARAMS(_invoke_params)          OSAL_Invoke_Init(&(_invoke_params)); \
                                                         OSAL_Set_Destination_Id_Vendor_Checker(&(_invoke_params), USBD_Port_STM32_CAT_A_OS_Is_Usbd_Context); \
                                                         OSAL_Set_Destination_Irq_Lock_Unlock(&(_invoke_params), USBD_Port_STM32_CAT_A_OS_Irq_Lock, USBD_Port_STM32_CAT_A_OS_Irq_Unlock)


#define USBD_MARK_INVOKE_DESTINATION(_invoke_params)     OSAL_Mark_Invoke_Params(&(_invoke_params))


#define USBD_IS_INVOKE_NEEDED(_invoke_params)            OSAL_Is_Invoke_Needed(&(_invoke_params))


#define USBD_INVOKE(_invoke_params, _action)             OSAL_INVOKE(&(_invoke_params), _action)



#endif
