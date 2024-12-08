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

#include "dummy.h"

void dummy_set_up_ep_handlers(USBD_IO_UP_EP_Handlers_XT *handlers)
{
    handlers->data_event.out= USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_OUT_Data_Event_HT);
    handlers->error         = USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT);
    handlers->sfin          = USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Stall_Fin_HT);
    handlers->reinit        = USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Reinit_HT);
    handlers->abort         = USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Abort_HT);
}

void dummy_set_down_ep_handlers(USBD_IO_UP_DOWN_Transaction_Params_XT *handlers)
{
    handlers->handlers.out.provide  = USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT);
    handlers->handlers.out.mem_cpy  = USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT);
    handlers->handlers.in.provide   = USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT);
    handlers->handlers.in.mem_cpy   = USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT);
    handlers->data.out.transferred_size                  = 0;
    handlers->data.out.provide_recursive_call_protection = USBD_FALSE;
    handlers->data.out.mem_cpy_recursive_call_protection = USBD_FALSE;
    handlers->data.in.provide_recursive_call_protection  = USBD_FALSE;
    handlers->data.in.mem_cpy_recursive_call_protection  = USBD_FALSE;
}

void dummy_set_down_common_handlers(USBD_IO_DOWN_Common_Handlers_XT *handlers)
{
    handlers->trigger                  = USBD_MAKE_INVALID_HANDLER(USBD_IO_DOWN_Trigger_HT);
    handlers->get_ep_out_waiting_size  = USBD_MAKE_INVALID_HANDLER(USBD_IO_DOWN_Get_EP_OUT_Waiting_Size_HT);
    handlers->get_ep_in_buffered_size  = USBD_MAKE_INVALID_HANDLER(USBD_IO_DOWN_Get_EP_IN_Buffered_Size_HT);
    handlers->abort                    = USBD_MAKE_INVALID_HANDLER(USBD_IO_DOWN_Abort_HT);
    handlers->stall                    = USBD_MAKE_INVALID_HANDLER(USBD_IO_DOWN_Stall_HT);
    handlers->halt                     = USBD_MAKE_INVALID_HANDLER(USBD_IO_DOWN_Halt_Configure_HT);
    handlers->configure                = USBD_MAKE_INVALID_HANDLER(USBD_IO_DOWN_Halt_Configure_HT);
}
