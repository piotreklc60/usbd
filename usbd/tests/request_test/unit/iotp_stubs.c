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

#include "iotp_stubs.h"

#include "usbd_iotp_internal.h"


static iotp_stubs_t *iotp_stubs = NULL;

static void *stub_usbd = NULL;


void set_stubs(iotp_stubs_t *stubs)
{
    USBD_ENTER_FUNC(USBD_DBG_PORT_REQ);

    iotp_stubs = stubs;

    USBD_EXIT_FUNC(USBD_DBG_PORT_REQ);
}





#define USBD_IOTP_Init                            USBD_IOTP_Init_stub
#define USBD_IOTP_Install                         USBD_IOTP_Install_stub
#define USBD_IOTP_Install_Default_Control_Pipe    USBD_IOTP_Install_Default_Control_Pipe_stub
#define USBD_IOTP_Send                            USBD_IOTP_Send_stub
#define USBD_IOTP_Send_Status                     USBD_IOTP_Send_Status_stub
#define USBD_IOTP_Send_Stall                      USBD_IOTP_Send_Stall_stub
#define USBD_IOTP_Abort                           USBD_IOTP_Abort_stub
#define USBD_IOTP_Set_Buf_Empty_Handler           USBD_IOTP_Set_Buf_Empty_Handler_stub

#include "usbd_iotp.c"


#undef USBD_IOTP_Init
#undef USBD_IOTP_Install
#undef USBD_IOTP_Install_Default_Control_Pipe
#undef USBD_IOTP_Send
#undef USBD_IOTP_Send_Status
#undef USBD_IOTP_Send_Stall
#undef USBD_IOTP_Abort
#undef USBD_IOTP_Set_Buf_Empty_Handler

#define USE_NATIVE_IOTP_METHODS     1





USBD_Bool_DT USBD_IOTP_Install(
        USBD_IOTP_Params_XT *tp_params)
{
    USBD_Bool_DT result;

    USBD_ENTER_FUNC(USBD_DBG_PORT_REQ);

    result = USBD_IOTP_Install_stub(tp_params);

    USBD_EXIT_FUNC(USBD_DBG_PORT_REQ);

    return result;
}

void USBD_IOTP_Init(
      USBD_Params_XT *usbd,
      USBDC_Params_XT *usbdc,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_IOTP_Params_XT *tp)
{
    USBD_ENTER_FUNC(USBD_DBG_PORT_REQ);

    USBD_IOTP_Init_stub(usbd, usbdc, ep_num, dir, tp);
    stub_usbd = usbd;

    USBD_EXIT_FUNC(USBD_DBG_PORT_REQ);
}

USBD_Bool_DT USBD_IOTP_Install_Default_Control_Pipe(
      USBD_Params_XT *usbd,
      USBD_IOTP_Params_XT *tp_in,
      USBD_IOTP_Params_XT *tp_out)
{
    USBD_Bool_DT result;

    USBD_ENTER_FUNC(USBD_DBG_PORT_REQ);

    result = USBD_IOTP_Install_Default_Control_Pipe_stub(usbd, tp_in, tp_out);

    USBD_EXIT_FUNC(USBD_DBG_PORT_REQ);

    return result;
}

USBD_Bool_DT USBD_IOTP_Send(
      USBD_IOTP_Params_XT *tp,
      const void *data,
      USBD_IO_Inout_Data_Size_DT size,
      USBD_IO_Inout_Data_Size_DT *size_left)
{
    bool call_state = true;
    USBD_Bool_DT result = USBD_TRUE;

    USBD_ENTER_FUNC(USBD_DBG_PORT_REQ);

    if(NULL != iotp_stubs)
    {
        call_state = !(iotp_stubs->lock_native_send);
    }

    if(call_state)
    {
        result = USBD_IOTP_Send_stub(tp, data, size, size_left);
    }

    if((NULL != iotp_stubs) && (NULL != stub_usbd))
    {
        if(NULL != iotp_stubs->tsend)
        {
            iotp_stubs->tsend(stub_usbd, 0, data, size);
        }
    }

    USBD_EXIT_FUNC(USBD_DBG_PORT_REQ);

    return result;
}

USBD_Bool_DT USBD_IOTP_Send_Status(
      USBD_IOTP_Params_XT *tp,
      USBD_IO_Inout_Data_Size_DT *size_left)
{
    bool call_state = true;
    USBD_Bool_DT result = USBD_TRUE;

    USBD_ENTER_FUNC(USBD_DBG_PORT_REQ);

    if(NULL != iotp_stubs)
    {
        call_state = !(iotp_stubs->lock_native_status);
    }

    if(call_state)
    {
        result = USBD_IOTP_Send_Status_stub(tp, size_left);
    }

    if((NULL != iotp_stubs) && (NULL != stub_usbd))
    {
        if(NULL != iotp_stubs->tstatus)
        {
            iotp_stubs->tstatus(stub_usbd, 0);
        }
    }

    USBD_EXIT_FUNC(USBD_DBG_PORT_REQ);

    return result;
}

USBD_Bool_DT USBD_IOTP_Send_Stall(
      USBD_IOTP_Params_XT *tp)
{
    bool call_state = true;
    USBD_Bool_DT result = USBD_TRUE;

    USBD_ENTER_FUNC(USBD_DBG_PORT_REQ);

    if(NULL != iotp_stubs)
    {
        call_state = !(iotp_stubs->lock_native_stall);
    }

    if(call_state)
    {
        result = USBD_IOTP_Send_Stall_stub(tp);
    }

    if((NULL != iotp_stubs) && (NULL != stub_usbd))
    {
        if(NULL != iotp_stubs->tstall)
        {
            iotp_stubs->tstall(stub_usbd, 0);
        }
    }

    USBD_EXIT_FUNC(USBD_DBG_PORT_REQ);

    return result;
}

USBD_Bool_DT USBD_IOTP_Abort(
      USBD_IOTP_Params_XT *tp,
      USBD_Bool_DT flush_hw_bufs)
{
    bool call_state = true;
    USBD_Bool_DT result = USBD_TRUE;

    USBD_ENTER_FUNC(USBD_DBG_PORT_REQ);

    if(NULL != iotp_stubs)
    {
        call_state = !(iotp_stubs->lock_native_abort);
    }

    if(call_state)
    {
        result = USBD_IOTP_Abort_stub(tp, flush_hw_bufs);
    }

    if((NULL != iotp_stubs) && (NULL != stub_usbd))
    {
        if(NULL != iotp_stubs->tabort)
        {
            iotp_stubs->tabort(stub_usbd, 0);
        }
    }

    USBD_EXIT_FUNC(USBD_DBG_PORT_REQ);

    return result;
}


void USBD_IOTP_Set_Buf_Empty_Handler(
      USBD_IOTP_Params_XT *tp,
      USBD_IOTP_Callback_HT buf_empty)
{
    bool call_state = true;

    USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

    if(NULL != iotp_stubs)
    {
        call_state = !(iotp_stubs->lock_native_set_ready);
    }

    if(call_state)
    {
        USBD_IOTP_Set_Buf_Empty_Handler_stub(tp, buf_empty);
    }

    if(NULL != iotp_stubs)
    {
        if(NULL != iotp_stubs->tset_ready)
        {
            iotp_stubs->tset_ready(tp, buf_empty);
        }
    }

    USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);
} /* USBD_IOTP_Set_Buf_Empty_Handler */

