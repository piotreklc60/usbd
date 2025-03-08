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

#ifndef IOTP_STUBS_H_
#define IOTP_STUBS_H_

#include "usbd.h"

typedef void (*stubs_send_from_irq) (USBD_Params_XT *usbd, uint8_t ep_num, const void *data, USBD_IO_Inout_Data_Size_DT size);
typedef void (*stubs_send_status_stall_abort_from_irq)(USBD_Params_XT *usbd, uint8_t ep_num);
typedef void (*stubs_set_ready_handler)(void *tp_params, USBD_IOTP_Callback_HT ready);

typedef struct iotp_stubs_tag
{
    stubs_send_from_irq                     tsend;
    stubs_send_status_stall_abort_from_irq  tstatus;
    stubs_send_status_stall_abort_from_irq  tstall;
    stubs_send_status_stall_abort_from_irq  tabort;
    stubs_set_ready_handler                 tset_ready;
    USBD_Bool_DT lock_native_send;
    USBD_Bool_DT lock_native_status;
    USBD_Bool_DT lock_native_stall;
    USBD_Bool_DT lock_native_abort;
    USBD_Bool_DT lock_native_set_ready;
}iotp_stubs_t;


void set_stubs(iotp_stubs_t *stubs);

#endif
