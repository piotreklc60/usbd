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

/*!
 *  \brief    USB Device IO Transport Protocol Event-based module
 *  \details   This module defines IN-OUT API which is part of USBD software layer
 *  \author   Piotr Wojtowicz
 *  \version   1.3.0.0
 *  \date     2012
 *  \pre      First initialize the system.
 *  \warning   Improper use can crash your application
 *  \copyright GNU Public License.
 */

#ifndef USBD_IOTP_EVENT_C_
#define USBD_IOTP_EVENT_C_
#endif

#ifndef USBD_H_
#include "usbd.h"
#endif

#ifndef USBD_PUBLIC_MACRO_H_
#include "usbd_public_macro.h"
#endif

#ifndef USBD_IOTP_EVENT_INTERNAL_H_
#include "usbd_iotp_event_internal.h"
#endif



static void USBD_IOTP_EVENT_io_data_provide_in(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_IN_Data_Method_Port_HT data_method);
static void USBD_IOTP_EVENT_io_data_memcpy_in(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_IN_Data_Method_Port_HT data_method);
static void USBD_IOTP_EVENT_io_evdata_in(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_IN_Data_Method_Port_HT mem_cpy);
static void USBD_IOTP_EVENT_io_provide_out(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT packet_size,
   USBD_IO_Inout_Data_Size_DT left_size,
   USBD_IO_OUT_Data_Method_Port_HT provide);
static void USBD_IOTP_EVENT_io_memcpy_out(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT packet_size,
   USBD_IO_Inout_Data_Size_DT left_size,
   USBD_IO_OUT_Data_Method_Port_HT mem_cpy);
static void USBD_IOTP_EVENT_io_memcpy_out_control(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT packet_size,
   USBD_IO_Inout_Data_Size_DT left_size,
   USBD_IO_OUT_Data_Method_Port_HT mem_cpy);
static void USBD_IOTP_EVENT_io_evdata_out(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_OUT_Data_Method_Port_HT mem_cpy);
static void USBD_IOTP_EVENT_io_abort(void *tp_params, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction);
static void USBD_IOTP_EVENT_io_error(
   void *tp_params, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction, USBD_IO_Inout_Data_Size_DT size);
static void USBD_IOTP_EVENT_io_reinit(
   void *tp_params, void *tp_owner, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction, USBD_Bool_DT active);


static const USBD_IO_OUT_Data_Method_TP_HT USBD_IOTP_EVENT_memcpy_out_table[4] =
{
   USBD_IOTP_EVENT_io_memcpy_out_control,
   USBD_IOTP_EVENT_io_memcpy_out,
   USBD_IOTP_EVENT_io_memcpy_out,
   USBD_IOTP_EVENT_io_memcpy_out
};

static const USBD_IO_UP_Error_HT USBD_IOTP_EVENT_error_table[4] =
{
   USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT),
   USBD_IOTP_EVENT_io_error,
   USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT),
   USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT)
};

static const uint8_t USBD_IOTP_EVENT_dummy_data[2] = {0, 1};

void USBD_IOTP_EVENT_Init(
      USBD_Params_XT *usbd,
      USBDC_Params_XT *usbdc,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_IOTP_EVENT_Params_XT *tp)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_ONOFF);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      memset(tp, 0, sizeof(USBD_IOTP_EVENT_Params_XT));

#if (USBD_FEATURE_NOT_PRESENT != USBD_USE_INVALIDATION_ON_INIT)
      USBD_INVALIDATE_HANDLER_ON_INIT(USBD_IOTP_EVENT_Callback_HT, tp->up_link.handlers.ready);
      USBD_INVALIDATE_HANDLER_ON_INIT(USBD_IOTP_EVENT_Callback_HT, tp->up_link.handlers.buf_empty);
      USBD_INVALIDATE_HANDLER_ON_INIT(USBD_IOTP_EVENT_Callback_HT, tp->up_link.handlers.error);
      USBD_INVALIDATE_HANDLER_ON_INIT(USBD_IOTP_EVENT_Callback_HT, tp->up_link.handlers.abort);
#endif

      USBD_IOTP_EVENT_SET_USBD_INSIDE_TP(tp, usbd);
      USBD_IOTP_EVENT_SET_USBDC_INSIDE_TP(tp, usbdc);
      USBD_IOTP_EVENT_SET_EP_NUM_INSIDE_TP(tp, ep_num);
      USBD_IOTP_EVENT_SET_EP_DIR_INSIDE_TP(tp, dir);

      USBD_INIT_INVOKE_PARAMS(USBD_IOTP_EVENT_GET_INVOKE_PARAMS(tp));
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_ONOFF);
} /* USBD_IOTP_EVENT_Init */

USBD_Bool_DT USBD_IOTP_EVENT_Install(
      USBD_IOTP_EVENT_Params_XT *tp)
{
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_ONOFF);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      if(USBD_CHECK_PTR(void, USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp))
         && (USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp) < USBD_MAX_NUM_ENDPOINTS)
         && ((USB_EP_DIRECTION_OUT == USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp))
            || (USB_EP_DIRECTION_IN == USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp))) )
      {
         if(USBD_BOOL_IS_FALSE(USBD_IO_UP_Is_TP_Locked(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp))))
         {
            USBD_IO_UP_SET_ERROR_HANDLER(
               &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT));
            USBD_IO_UP_SET_SFIN_HANDLER (
               &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Stall_Fin_HT));
            USBD_IO_UP_SET_REINIT_HANDLER(
               &(tp->core.pipe_params.handlers),USBD_IOTP_EVENT_io_reinit);
            USBD_IO_UP_SET_ABORT_HANDLER (
               &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Abort_HT));

            USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);

            if(USB_EP_DIRECTION_IN == USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp))
            {
               USBD_IO_UP_SET_IN_DATA_EVENT_HANDLER(
                  &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_IN_Data_Event_HT));

               tp->core.transfer_params.in.proc.data                 = USBD_MAKE_INVALID_PTR(USBD_IOTP_EVENT_Data_DT);
               tp->core.transfer_params.in.proc.size_left            = 0;
               tp->core.transfer_params.in.proc.last_packet_size     = 0;
               tp->core.transfer_params.in.proc.req_in_progress      = USBD_FALSE;
            }
            else
            {
               USBD_IO_UP_SET_OUT_DATA_EVENT_HANDLER(
                  &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_OUT_Data_Event_HT));

               tp->core.transfer_params.out.proc.mem_cpy             =
                  USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_Port_HT);
               tp->core.transfer_params.out.proc.data                = USBD_MAKE_INVALID_PTR(USBD_IOTP_EVENT_Data_DT);
               tp->core.transfer_params.out.proc.size_left           = 0;
               tp->core.transfer_params.out.proc.dont_wait_for_ready = USBD_FALSE;
            }

            USBD_IO_UP_Set_TP(
               USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp),
               &(tp->core.pipe_params.handlers),
               tp,
               (void*)(&USBD_IOTP_EVENT_dummy_data));

            result = USBD_TRUE;
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_ONOFF);

   return result;
} /* USBD_IOTP_EVENT_Install */

USBD_Bool_DT USBD_IOTP_EVENT_Install_Default_Control_Pipe(
      USBD_Params_XT *usbd,
      USBD_IOTP_EVENT_Params_XT *tp_in,
      USBD_IOTP_EVENT_Params_XT *tp_out)
{
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_ONOFF);

   result = USBD_FALSE;

   if((USBD_CHECK_PTR(void, usbd))
      && (USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp_in)) && (USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp_out)))
   {
      /** initializes IN pipe */
      USBD_IOTP_EVENT_Init(usbd, USBD_MAKE_INVALID_PTR(void), 0, USB_EP_DIRECTION_IN, tp_in);

      USBD_IO_UP_SET_IN_DATA_EVENT_HANDLER(
         &(tp_in->core.pipe_params.handlers) ,USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_IN_Data_Event_HT));
      USBD_IO_UP_SET_ERROR_HANDLER    (
         &(tp_in->core.pipe_params.handlers) ,USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT));
      USBD_IO_UP_SET_SFIN_HANDLER     (
         &(tp_in->core.pipe_params.handlers) ,USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Stall_Fin_HT));
      USBD_IO_UP_SET_REINIT_HANDLER   (
         &(tp_in->core.pipe_params.handlers) ,USBD_IOTP_EVENT_io_reinit);
      USBD_IO_UP_SET_ABORT_HANDLER    (
         &(tp_in->core.pipe_params.handlers) ,USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Abort_HT));

      USBD_IO_UP_Set_TP(
         usbd,
         0,
         USB_EP_DIRECTION_IN,
         &(tp_in->core.pipe_params.handlers),
         tp_in,
         (void*)(&USBD_IOTP_EVENT_dummy_data));

      /** initializes OUT pipe */
      USBD_IOTP_EVENT_Init(usbd, USBD_MAKE_INVALID_PTR(void), 0, USB_EP_DIRECTION_OUT, tp_out);

      USBD_IO_UP_SET_OUT_DATA_EVENT_HANDLER(
         &(tp_out->core.pipe_params.handlers) ,USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_OUT_Data_Event_HT));
      USBD_IO_UP_SET_ERROR_HANDLER    (
         &(tp_out->core.pipe_params.handlers) ,USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT));
      USBD_IO_UP_SET_SFIN_HANDLER     (
         &(tp_out->core.pipe_params.handlers) ,USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Stall_Fin_HT));
      USBD_IO_UP_SET_REINIT_HANDLER   (
         &(tp_out->core.pipe_params.handlers) ,USBD_IOTP_EVENT_io_reinit);
      USBD_IO_UP_SET_ABORT_HANDLER    (
         &(tp_out->core.pipe_params.handlers) ,USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Abort_HT));

      USBD_IO_UP_Set_TP(
         usbd,
         0,
         USB_EP_DIRECTION_OUT,
         &(tp_out->core.pipe_params.handlers),
         tp_out,
         (void*)(&USBD_IOTP_EVENT_dummy_data));

      /** locks IN and OUT pipes */
      USBD_IO_UP_Lock_TP_Params(usbd, 0, USB_EP_DIRECTION_IN);
      USBD_IO_UP_Lock_TP_Params(usbd, 0, USB_EP_DIRECTION_OUT);

      result = USBD_TRUE;
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_ONOFF);

   return result;
} /* USBD_IOTP_EVENT_Install_Default_Control_Pipe */



USBD_Params_XT *USBD_IOTP_EVENT_Get_USBD(
      USBD_IOTP_EVENT_Params_XT *tp)
{
   USBD_Params_XT *result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   result = USBD_MAKE_INVALID_PTR(USBD_Params_XT);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      result = USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);

   return result;
} /* USBD_IOTP_EVENT_Get_USBD */

USBDC_Params_XT *USBD_IOTP_EVENT_Get_USBDC(
      USBD_IOTP_EVENT_Params_XT *tp)
{
   USBDC_Params_XT *result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   result = USBD_MAKE_INVALID_PTR(USBDC_Params_XT);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      result = USBD_IOTP_EVENT_GET_USBDC_FROM_TP(tp);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);

   return result;
} /* USBD_IOTP_EVENT_Get_USBDC */

uint8_t USBD_IOTP_EVENT_Get_EP_Num(
      USBD_IOTP_EVENT_Params_XT *tp)
{
   uint8_t result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   result = USBD_MAX_NUM_ENDPOINTS;

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      result = USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);

   return result;
} /* USBD_IOTP_EVENT_Get_EP_Num */

USB_EP_Direction_ET USBD_IOTP_EVENT_Get_Dir(
      USBD_IOTP_EVENT_Params_XT *tp)
{
   USB_EP_Direction_ET result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   result = USB_EP_DIRECTION_OUT;

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      result = USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);

   return result;
} /* USBD_IOTP_EVENT_Get_Dir */

USBD_IO_Inout_Data_Size_DT USBD_IOTP_EVENT_Get_Transferred_Size(
      USBD_IOTP_EVENT_Params_XT *tp)
{
   USBD_IO_Inout_Data_Size_DT result = 0;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      result = USBD_IOTP_EVENT_GET_SIZE_TRANSFERRED(tp);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);

   return result;
} /* USBD_IOTP_EVENT_Get_Transferred_Size */



void USBD_IOTP_EVENT_Set_Handlers(
      USBD_IOTP_EVENT_Params_XT *tp,
      USBD_IOTP_EVENT_Callback_HT ready,
      USBD_IOTP_EVENT_Callback_HT buf_empty,
      USBD_IOTP_EVENT_Callback_HT error,
      USBD_IOTP_EVENT_Callback_HT abort)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      USBD_IOTP_EVENT_SET_READY_HANDLER(tp, ready);
      USBD_IOTP_EVENT_SET_BUF_EMPTY_HANDLER(tp, buf_empty);
      USBD_IOTP_EVENT_SET_ERROR_HANDLER(tp, error);
      USBD_IOTP_EVENT_SET_ABORT_HANDLER(tp, abort);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);
} /* USBD_IOTP_EVENT_Set_Handlers */

void USBD_IOTP_EVENT_Set_Ready_Handler(
      USBD_IOTP_EVENT_Params_XT *tp,
      USBD_IOTP_EVENT_Callback_HT ready)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      USBD_IOTP_EVENT_SET_READY_HANDLER(tp, ready);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);
} /* USBD_IOTP_EVENT_Set_Ready_Handler */

void USBD_IOTP_EVENT_Set_Buf_Empty_Handler(
      USBD_IOTP_EVENT_Params_XT *tp,
      USBD_IOTP_EVENT_Callback_HT buf_empty)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      USBD_IOTP_EVENT_SET_BUF_EMPTY_HANDLER(tp, buf_empty);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);
} /* USBD_IOTP_EVENT_Set_Buf_Empty_Handler */

void USBD_IOTP_EVENT_Set_Error_Handler(
      USBD_IOTP_EVENT_Params_XT *tp,
      USBD_IOTP_EVENT_Callback_HT error)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      USBD_IOTP_EVENT_SET_ERROR_HANDLER(tp, error);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);
} /* USBD_IOTP_EVENT_Set_Error_Handler */

void USBD_IOTP_EVENT_Set_Abort_Handler(
      USBD_IOTP_EVENT_Params_XT *tp,
      USBD_IOTP_EVENT_Callback_HT abort)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      USBD_IOTP_EVENT_SET_ABORT_HANDLER(tp, abort);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);
} /* USBD_IOTP_EVENT_Set_Abort_Handler */

USBD_Vendor_Data_XT *USBD_IOTP_EVENT_Get_Vendor_Data_Container(USBD_IOTP_EVENT_Params_XT *tp)
{
   USBD_Vendor_Data_XT *result = USBD_MAKE_INVALID_PTR(USBD_Vendor_Data_XT);

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      result = USBD_IOTP_EVENT_GET_VENDOR_DATA_PTR(tp);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);

   return result;
} /* USBD_IOTP_EVENT_Get_Vendor_Data_Container */



USBD_Bool_DT USBD_IOTP_EVENT_Is_Transfer_Active(
      USBD_IOTP_EVENT_Params_XT *tp)
{
   void *tp_owner;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      tp_owner = USBD_IO_UP_Get_TP_Owner(
         USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp));

      if(USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_EVENT_dummy_data))
      {
         result = USBD_ATOMIC_BOOL_GET(tp->core.transfer_active);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);

   return result;
} /* USBD_IOTP_EVENT_Is_Transfer_Active */

USBD_Bool_DT USBD_IOTP_EVENT_Abort(
      USBD_IOTP_EVENT_Params_XT *tp,
      USBD_Bool_DT flush_hw_bufs)
{
   void *tp_owner;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      tp_owner = USBD_IO_UP_Get_TP_Owner(
         USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp));

      if(USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_EVENT_dummy_data))
      {
         USBD_DEBUG_HI_3(
            USBD_DBG_IOTPEV_PROCESSING,
            "abort before::ep: %d::%s; tr_active = %s",
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp)) ? "active" : "passive",
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false");

         /* check if transaction can be processed or if request must be synchronizes to USBD task/irq */
         if(USBD_BOOL_IS_FALSE(USBD_IS_INVOKE_NEEDED(USBD_IOTP_EVENT_GET_INVOKE_PARAMS(tp))))
         {
            if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) || USBD_BOOL_IS_TRUE(flush_hw_bufs))
            {
               if(USB_EP_DIRECTION_OUT == USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp))
               {
                  USBD_IO_OUT_Abort(
                     USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                     USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
                     flush_hw_bufs);
               }
               else
               {
                  USBD_IO_IN_Abort(
                     USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                     USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
                     flush_hw_bufs);
               }
            }
         }
         else
         {
            USBD_EMERG(USBD_DBG_IOTPEV_PROCESSING, "Invoke not implemented!");
         }

         USBD_DEBUG_HI_3(
            USBD_DBG_IOTPEV_PROCESSING,
            "abort after:: ep: %d::%s; tr_active = %s",
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp)) ? "active" : "passive",
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false");
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_EVENT_Abort */



USBD_Bool_DT USBD_IOTP_EVENT_Send(
      USBD_IOTP_EVENT_Params_XT *tp,
      const void *data,
      USBD_IO_Inout_Data_Size_DT size,
      USBD_IO_Inout_Data_Size_DT *size_left)
{
   void                                  *tp_owner;
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction;
   USBD_Bool_DT                           result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   result = USBD_FALSE;

   if((USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
      && (USBD_CHECK_PTR(void, data)))
   {
      tp_owner = USBD_IO_UP_Get_IN_TP_Owner(
         USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp));

      if(USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_EVENT_dummy_data)
         && (USB_EP_DIRECTION_IN == USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp)))
      {
         USBD_DEBUG_HI_6(
            USBD_DBG_IOTPEV_PROCESSING,
            "send before::ep: %d::%s; size = %d; tr_active = %s; size_left = %d; last_packet_size = %d",
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_IN) ? "active" : "passive",
            size,
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.in.proc.size_left,
            tp->core.transfer_params.in.proc.last_packet_size);

         if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_active) || (0 == tp->core.transfer_params.in.proc.size_left))
         {
            /* check if transaction can be processed or if request must be synchronizes to USBD task/irq */
            if(USBD_BOOL_IS_FALSE(USBD_IS_INVOKE_NEEDED(USBD_IOTP_EVENT_GET_INVOKE_PARAMS(tp))))
            {
               tp->core.transfer_params.in.proc.req_in_progress = USBD_TRUE;

               transaction = USBD_IO_Get_IN_Transaction_Params(
                  USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                  USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp));

               if(USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction))
               {
                  result = USBD_TRUE;
                  tp->up_link.data.size_transferred                  = 0;
                  tp->core.transfer_params.in.proc.data              = data;
                  tp->core.transfer_params.in.proc.size_left         = size;
                  tp->core.transfer_params.in.proc.last_packet_size  = size % tp->core.pipe_params.data.mps;
                  if(USB_EP_DESC_TRANSFER_TYPE_CONTROL != tp->core.pipe_params.data.ep_type)
                  {
                     if((0 == tp->core.transfer_params.in.proc.last_packet_size) && (size > 0))
                     {
                        tp->core.transfer_params.in.proc.last_packet_size = tp->core.pipe_params.data.mps;
                     }
                  }

                  USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_TRUE);

                  if(size > tp->core.pipe_params.data.mps)
                  {
                     USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_IOTP_EVENT_io_data_provide_in);
                     USBD_IO_SET_IN_MEMCPY_HANDLER(transaction, USBD_IOTP_EVENT_io_data_memcpy_in);

                     result = USBD_IO_UP_Trigger_IN(
                        USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                        USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp));
                  }
                  else if(size > 0)
                  {
                     USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
                     USBD_IO_SET_IN_MEMCPY_HANDLER(transaction, USBD_IOTP_EVENT_io_data_memcpy_in);

                     result = USBD_IO_UP_Trigger_IN(
                        USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                        USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp));
                  }
                  else if(tp->core.transfer_params.in.proc.last_packet_size != tp->core.pipe_params.data.mps)
                  {
                     USBD_DEBUG_HI_1(
                        USBD_DBG_IOTPEV_PROCESSING,
                        "status on send with size 0 - %s", "immediately or at next packet - port decides");

                     USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
                     USBD_IO_SET_IN_MEMCPY_HANDLER(transaction, USBD_IOTP_EVENT_io_data_memcpy_in);

                     result = USBD_IO_UP_Trigger_IN(
                        USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                        USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp));
                  }
                  else
                  {
                     USBD_DEBUG_HI_1(
                        USBD_DBG_IOTPEV_PROCESSING,
                        "status on send with size 0 - %s", "on status executing - ommit");
                  }

                  if(USBD_BOOL_IS_FALSE(result))
                  {
                     USBD_DEBUG_HI(USBD_DBG_IOTPEV_PROCESSING, "trigger failed");

                     USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
                     USBD_IO_SET_IN_MEMCPY_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));

                     USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);
                  }
               }

               tp->core.transfer_params.in.proc.req_in_progress = USBD_FALSE;

               if(USBD_CHECK_PTR(USBD_IO_Inout_Data_Size_DT, size_left))
               {
                  *size_left = tp->core.transfer_params.in.proc.size_left;

                  if((0 == tp->core.transfer_params.in.proc.size_left)
                     && (0 != tp->core.transfer_params.in.proc.last_packet_size))
                  {
                     *size_left = (-1);
                  }

                  USBD_DEBUG_HI_1(USBD_DBG_IOTPEV_PROCESSING, "send with *size_left = %d", *size_left);
               }
            }
            else
            {
               USBD_EMERG(USBD_DBG_IOTPEV_PROCESSING, "Invoke not implemented!");
            }
         }
         else
         {
            USBD_WARN(USBD_DBG_IOTPEV_PROCESSING, "transfer is active! pls stop previous transfer at frst!");
         }

         USBD_DEBUG_HI_6(
            USBD_DBG_IOTPEV_PROCESSING,
            "send after ::ep: %d::%s; size = %d; tr_active = %s; size_left = %d; last_packet_size = %d",
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_IN) ? "active" : "passive",
            size,
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.in.proc.size_left,
            tp->core.transfer_params.in.proc.last_packet_size);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_EVENT_Send */

USBD_Bool_DT USBD_IOTP_EVENT_Send_Status(
      USBD_IOTP_EVENT_Params_XT *tp,
      USBD_IO_Inout_Data_Size_DT *size_left)
{
   return USBD_IOTP_EVENT_Send(tp, &tp, 0, size_left);
} /* USBD_IOTP_EVENT_Send_Status */



static void USBD_IOTP_EVENT_io_data_provide_in(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_IN_Data_Method_Port_HT data_method)
{
   USBD_IOTP_EVENT_Params_XT *tp   = (USBD_IOTP_EVENT_Params_XT*)tp_params;
   USBD_IO_Inout_Data_Size_DT size = 0;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction)
      && USBD_CHECK_HANDLER(USBD_IO_IN_Data_Method_Port_HT, data_method))
   {
      USBD_DEBUG_HI_5(
         USBD_DBG_IOTPEV_PROCESSING,
         "provide_in before::ep: %d::%s; tr_active = %s; size_left = %d; last_packet_size = %d",
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_IN) ? "active" : "passive",
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.in.proc.size_left,
         tp->core.transfer_params.in.proc.last_packet_size);

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active))
      {
         if(tp->core.transfer_params.in.proc.size_left > 1)
         {
            size = USBD_CALL_HANDLER(USBD_IO_IN_Data_Method_Port_HT, data_method)(
               USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp),
               tp->core.transfer_params.in.proc.data,
               tp->core.transfer_params.in.proc.size_left - 1,
               USBD_TRUE);
         }

         tp->up_link.data.size_transferred           += size;
         tp->core.transfer_params.in.proc.size_left  -= size;
         tp->core.transfer_params.in.proc.data       += size;

         if(tp->core.transfer_params.in.proc.size_left <= tp->core.pipe_params.data.mps)
         {
            USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
            USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_IOTP_EVENT_io_data_memcpy_in);
         }
         else
         {
            USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_IOTP_EVENT_io_data_provide_in);
            USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_IOTP_EVENT_io_data_memcpy_in);
         }
      }

      USBD_DEBUG_HI_5(
         USBD_DBG_IOTPEV_PROCESSING,
         "provide_in after ::ep: %d::%s; tr_active = %s; size_left = %d; last_packet_size = %d",
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_IN) ? "active" : "passive",
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.in.proc.size_left,
         tp->core.transfer_params.in.proc.last_packet_size);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_EVENT_io_data_provide_in */

static void USBD_IOTP_EVENT_io_data_memcpy_in(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_IN_Data_Method_Port_HT data_method)
{
   USBD_IOTP_EVENT_Params_XT *tp   = (USBD_IOTP_EVENT_Params_XT*)tp_params;
   USBD_IO_Inout_Data_Size_DT size = 0;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction)
      && USBD_CHECK_HANDLER(USBD_IO_IN_Data_Method_Port_HT, data_method))
   {
      USBD_DEBUG_HI_5(
         USBD_DBG_IOTPEV_PROCESSING,
         "memcpy_in  before::ep: %d::%s; tr_active = %s; size_left = %d; last_packet_size = %d",
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_IN) ? "active" : "passive",
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.in.proc.size_left,
         tp->core.transfer_params.in.proc.last_packet_size);

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active))
      {
         if((tp->core.transfer_params.in.proc.size_left > 0) || (0 == tp->core.transfer_params.in.proc.last_packet_size))
         {
            size = USBD_CALL_HANDLER(USBD_IO_IN_Data_Method_Port_HT, data_method)(
               USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp),
               tp->core.transfer_params.in.proc.data,
               tp->core.transfer_params.in.proc.size_left,
               USBD_TRUE);
         }

         if((0 == tp->core.transfer_params.in.proc.size_left) && (0 == tp->core.transfer_params.in.proc.last_packet_size))
         {
            tp->core.transfer_params.in.proc.last_packet_size = tp->core.pipe_params.data.mps;
         }

         tp->up_link.data.size_transferred += size;

         if(size <= tp->core.transfer_params.in.proc.size_left)
         {
            tp->core.transfer_params.in.proc.size_left  -= size;
            tp->core.transfer_params.in.proc.data       += size;
         }
         else
         {
            tp->core.transfer_params.in.proc.size_left   = 0;
         }

         if((0 == tp->core.transfer_params.in.proc.size_left) && (0 != tp->core.transfer_params.in.proc.last_packet_size))
         {
            /** call ready handler */
            if(USBD_IOTP_EVENT_CHECK_READY_HANDLER(tp)
               && USBD_BOOL_IS_FALSE(tp->core.transfer_params.in.proc.req_in_progress))
            {
               USBD_IOTP_EVENT_CALL_READY(
                  USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                  USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
                  USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp),
                  tp,
                  tp->core.pipe_params.data.mps);
            }
         }

         if((0 == tp->core.transfer_params.in.proc.size_left) && (0 != tp->core.transfer_params.in.proc.last_packet_size))
         {
            USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
            USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));

            USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);
         }
         else if(tp->core.transfer_params.in.proc.size_left <= tp->core.pipe_params.data.mps)
         {
            USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
            USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_IOTP_EVENT_io_data_memcpy_in);
         }
         else
         {
            USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_IOTP_EVENT_io_data_provide_in);
            USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_IOTP_EVENT_io_data_memcpy_in);
         }
      }

      USBD_DEBUG_HI_5(
         USBD_DBG_IOTPEV_PROCESSING,
         "memcpy_in  after ::ep: %d::%s; tr_active = %s; size_left = %d; last_packet_size = %d",
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_IN) ? "active" : "passive",
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.in.proc.size_left,
         tp->core.transfer_params.in.proc.last_packet_size);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_EVENT_io_data_memcpy_in */

static void USBD_IOTP_EVENT_io_evdata_in(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_IN_Data_Method_Port_HT mem_cpy)
{
   USBD_IOTP_EVENT_Params_XT *tp = (USBD_IOTP_EVENT_Params_XT*)tp_params;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction))
   {
      USBD_DEBUG_HI_6(
         USBD_DBG_IOTPEV_PROCESSING,
         "evdata_in before::ep: %d::%s; size = %d; tr_active = %s; size_left = %d; last_packet_size = %d",
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_IN) ? "active" : "passive",
         size,
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.in.proc.size_left,
         tp->core.transfer_params.in.proc.last_packet_size);

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active))
      {
         USBD_IOTP_EVENT_io_data_memcpy_in(tp_params, transaction, mem_cpy);
      }

      if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_active))
      {
         if(size < 0)
         {
            /** informs upper layer that IN buffer is empty */
            if(USBD_IOTP_EVENT_CHECK_BUF_EMPTY_HANDLER(tp)
               && USBD_BOOL_IS_FALSE(tp->core.transfer_params.in.proc.req_in_progress))
            {
               USBD_IOTP_EVENT_CALL_BUF_EMPTY(
                  USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                  USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
                  USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp),
                  tp,
                  tp->core.pipe_params.data.mps);
            }
         }
         else
         {
            /** informs upper layer that IN data can be now provided to port */
            if(USBD_IOTP_EVENT_CHECK_READY_HANDLER(tp)
               && USBD_BOOL_IS_FALSE(tp->core.transfer_params.in.proc.req_in_progress))
            {
               USBD_IOTP_EVENT_CALL_READY(
                  USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                  USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
                  USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp),
                  tp,
                  tp->core.pipe_params.data.mps);
            }
         }
      }

      USBD_DEBUG_HI_6(
         USBD_DBG_IOTPEV_PROCESSING,
         "evdata_in after ::ep: %d::%s; size = %d; tr_active = %s; size_left = %d; last_packet_size = %d",
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_IN) ? "active" : "passive",
         size,
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.in.proc.size_left,
         tp->core.transfer_params.in.proc.last_packet_size);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_EVENT_io_evdata_in */



USBD_Bool_DT USBD_IOTP_EVENT_Recv_And_Ready(
      USBD_IOTP_EVENT_Params_XT *tp,
      void *data,
      USBD_IO_Inout_Data_Size_DT size,
      USBD_IO_Inout_Data_Size_DT *size_left)
{
   void                                  *tp_owner;
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction;
   USBD_IO_Inout_Data_Size_DT             packet_size;
   USBD_Bool_DT                           is_last_part = USBD_TRUE;
   USBD_Bool_DT                           result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   result = USBD_FALSE;

   if((USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
      && (USBD_CHECK_PTR(void, data)))
   {
      tp_owner = USBD_IO_UP_Get_OUT_TP_Owner(
         USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp));

      if(USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_EVENT_dummy_data)
         && (USB_EP_DIRECTION_OUT == USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp)))
      {
         USBD_DEBUG_HI_7(
            USBD_DBG_IOTPEV_PROCESSING,
            "recv_and_ready before::"
            "ep: %d::%s; size = %d; tr_active = %s; size_left = %d; dont_wait_for_ready = %s; data = 0x%08X",
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_OUT) ? "active" : "passive",
            size,
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.out.proc.size_left,
            USBD_BOOL_IS_TRUE(tp->core.transfer_params.out.proc.dont_wait_for_ready) ? "true" : "false",
            tp->core.transfer_params.out.proc.data);

         if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_active))
         {
            /* check if transaction can be processed or if request must be synchronizes to USBD task/irq */
            if(USBD_BOOL_IS_FALSE(USBD_IS_INVOKE_NEEDED(USBD_IOTP_EVENT_GET_INVOKE_PARAMS(tp))))
            {
               transaction = USBD_IO_Get_OUT_Transaction_Params(
                  USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                  USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp));

               if(USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction))
               {
                  tp->up_link.data.size_transferred                     = 0;
                  tp->core.transfer_params.out.proc.data                = data;
                  tp->core.transfer_params.out.proc.size_left           = size;
                  tp->core.transfer_params.out.proc.dont_wait_for_ready = USBD_TRUE;
                  result                                                = USBD_TRUE;

                  if(size > tp->core.pipe_params.data.mps)
                  {
                     USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_TRUE);
                     USBD_IO_SET_OUT_PROVIDE_HANDLER(
                        transaction, USBD_IOTP_EVENT_io_provide_out);
                     USBD_IO_SET_OUT_MEMCPY_HANDLER(
                        transaction, USBD_IOTP_EVENT_memcpy_out_table[tp->core.pipe_params.data.ep_type]);
                  }
                  else
                  {
                     USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_TRUE);
                     USBD_IO_SET_OUT_PROVIDE_HANDLER(
                        transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
                     USBD_IO_SET_OUT_MEMCPY_HANDLER(
                        transaction, USBD_IOTP_EVENT_memcpy_out_table[tp->core.pipe_params.data.ep_type]);
                  }

                  if(!USBD_CHECK_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, tp->core.transfer_params.out.proc.mem_cpy))
                  {
                     result = USBD_IO_UP_Trigger_OUT(
                        USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                        USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
                        USBD_TRUE);
                  }
                  else
                  {
                     size = USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(
                        USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                        USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
                        USBD_TRUE);

                     if(size >= 0)
                     {
                        if(size > tp->core.transfer_params.out.proc.size_left)
                        {
                           size = tp->core.transfer_params.out.proc.size_left;
                           is_last_part = USBD_FALSE;
                        }

                        packet_size = USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(
                           USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                           USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
                           USBD_FALSE);

                        size = USBD_CALL_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, tp->core.transfer_params.out.proc.mem_cpy)(
                           USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                           USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
                           USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp),
                           tp->core.transfer_params.out.proc.data,
                           size,
                           is_last_part);

                        tp->up_link.data.size_transferred            = size;
                        tp->core.transfer_params.out.proc.size_left -= size;
                        tp->core.transfer_params.out.proc.data      += size;

                        if(0 == tp->core.transfer_params.out.proc.size_left)
                        {
                           if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == tp->core.pipe_params.data.ep_type)
                           {
                              if(tp->core.pipe_params.data.mps != packet_size)
                              {
                                 USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);

                                 USBD_IO_SET_OUT_PROVIDE_HANDLER(
                                    transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
                                 USBD_IO_SET_OUT_MEMCPY_HANDLER(
                                    transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
                              }
                           }
                           else
                           {
                              USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);

                              USBD_IO_SET_OUT_PROVIDE_HANDLER(
                                 transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
                              USBD_IO_SET_OUT_MEMCPY_HANDLER(
                                 transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
                           }
                        }

                        if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active))
                        {
                           if(tp->core.transfer_params.out.proc.size_left > tp->core.pipe_params.data.mps)
                           {
                              USBD_IO_SET_OUT_PROVIDE_HANDLER(
                                 transaction, USBD_IOTP_EVENT_io_provide_out);
                              USBD_IO_SET_OUT_MEMCPY_HANDLER(
                                 transaction, USBD_IOTP_EVENT_memcpy_out_table[tp->core.pipe_params.data.ep_type]);
                           }
                           else
                           {
                              USBD_IO_SET_OUT_PROVIDE_HANDLER(
                                 transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
                              USBD_IO_SET_OUT_MEMCPY_HANDLER(
                                 transaction, USBD_IOTP_EVENT_memcpy_out_table[tp->core.pipe_params.data.ep_type]);
                           }
                        }
                     }
                     else
                     {
                        result = USBD_IO_UP_Trigger_OUT(
                           USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                           USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
                           USBD_TRUE);
                     }
                  }

                  if(USBD_BOOL_IS_FALSE(result))
                  {
                     USBD_DEBUG_HI(USBD_DBG_IOTPEV_PROCESSING, "trigger failed");

                     USBD_IO_SET_OUT_PROVIDE_HANDLER(
                        transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
                     USBD_IO_SET_OUT_MEMCPY_HANDLER(
                        transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));

                     USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);
                  }

                  if(USBD_CHECK_PTR(USBD_IO_Inout_Data_Size_DT, size_left))
                  {
                     if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active))
                     {
                        *size_left = size;
                     }
                     else
                     {
                        *size_left = (-1);
                     }
                  }
               }
            }
            else
            {
               USBD_EMERG(USBD_DBG_IOTPEV_PROCESSING, "Invoke not implemented!");
            }
         }

         USBD_DEBUG_HI_8(
            USBD_DBG_IOTPEV_PROCESSING,
            "recv_and_ready after ::"
            "ep: %d::%s; size = %d; tr_active = %s; size_left = %d; dont_wait_for_ready = %s; data = 0x%08X, result: %s",
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_OUT) ? "active" : "passive",
            size,
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.out.proc.size_left,
            USBD_BOOL_IS_TRUE(tp->core.transfer_params.out.proc.dont_wait_for_ready) ? "true" : "false",
            tp->core.transfer_params.out.proc.data,
            USBD_BOOL_IS_TRUE(result) ? "true" : "false");
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_EVENT_Recv_And_Ready */



USBD_Bool_DT USBD_IOTP_EVENT_Recv_And_Wait(
      USBD_IOTP_EVENT_Params_XT *tp,
      void *data,
      USBD_IO_Inout_Data_Size_DT size,
      USBD_IO_Inout_Data_Size_DT *size_left)
{
   void                                  *tp_owner;
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction;
   USBD_IO_Inout_Data_Size_DT             packet_size;
   USBD_Bool_DT                           is_last_part = USBD_TRUE;
   USBD_Bool_DT                           result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   result = USBD_FALSE;

   if((USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
      && (USBD_CHECK_PTR(void, data)))
   {
      tp_owner = USBD_IO_UP_Get_OUT_TP_Owner(
         USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp));

      if(USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_EVENT_dummy_data)
         && (USB_EP_DIRECTION_OUT == USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp)))
      {
         USBD_DEBUG_HI_7(
            USBD_DBG_IOTPEV_PROCESSING,
            "recv_and_wait  before::"
            "ep: %d::%s; size = %d; tr_active = %s; size_left = %d; dont_wait_for_ready = %s; data = 0x%08X",
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_OUT) ? "active" : "passive",
            size,
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.out.proc.size_left,
            USBD_BOOL_IS_TRUE(tp->core.transfer_params.out.proc.dont_wait_for_ready) ? "true" : "false",
            tp->core.transfer_params.out.proc.data);

         if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_active))
         {
            /* check if transaction can be processed or if request must be synchronizes to USBD task/irq */
            if(USBD_BOOL_IS_FALSE(USBD_IS_INVOKE_NEEDED(USBD_IOTP_EVENT_GET_INVOKE_PARAMS(tp))))
            {
               transaction = USBD_IO_Get_OUT_Transaction_Params(
                  USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                  USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp));

               if(USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction))
               {
                  tp->up_link.data.size_transferred                     = 0;
                  tp->core.transfer_params.out.proc.data                = data;
                  tp->core.transfer_params.out.proc.size_left           = size;
                  tp->core.transfer_params.out.proc.dont_wait_for_ready = USBD_FALSE;
                  result                                                = USBD_TRUE;

                  if(size > tp->core.pipe_params.data.mps)
                  {
                     USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_TRUE);
                     USBD_IO_SET_OUT_PROVIDE_HANDLER(
                        transaction, USBD_IOTP_EVENT_io_provide_out);
                     USBD_IO_SET_OUT_MEMCPY_HANDLER(
                        transaction, USBD_IOTP_EVENT_memcpy_out_table[tp->core.pipe_params.data.ep_type]);
                  }
                  else
                  {
                     USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_TRUE);
                     USBD_IO_SET_OUT_PROVIDE_HANDLER(
                        transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
                     USBD_IO_SET_OUT_MEMCPY_HANDLER(
                        transaction, USBD_IOTP_EVENT_memcpy_out_table[tp->core.pipe_params.data.ep_type]);
                  }

                  if(!USBD_CHECK_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, tp->core.transfer_params.out.proc.mem_cpy))
                  {
                     result = USBD_IO_UP_Trigger_OUT(
                        USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                        USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
                        USBD_FALSE);
                  }
                  else
                  {
                     size = USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(
                        USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                        USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
                        USBD_TRUE);

                     if(size >= 0)
                     {
                        packet_size = USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(
                           USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                           USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
                           USBD_FALSE);

                        if(size > tp->core.transfer_params.out.proc.size_left)
                        {
                           size = tp->core.transfer_params.out.proc.size_left;
                           is_last_part = USBD_FALSE;
                        }
                        else if(size == tp->core.transfer_params.out.proc.size_left)
                        {
                           if(USB_EP_DESC_TRANSFER_TYPE_CONTROL != tp->core.pipe_params.data.ep_type)
                           {
                              is_last_part = USBD_FALSE;
                           }
                           else if(tp->core.pipe_params.data.mps != packet_size)
                           {
                              is_last_part = USBD_FALSE;
                           }
                        }

                        size = USBD_CALL_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, tp->core.transfer_params.out.proc.mem_cpy)(
                           USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                           USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
                           USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp),
                           tp->core.transfer_params.out.proc.data,
                           size,
                           is_last_part);

                        tp->up_link.data.size_transferred            = size;
                        tp->core.transfer_params.out.proc.size_left -= size;
                        tp->core.transfer_params.out.proc.data      += size;

                        if(0 == tp->core.transfer_params.out.proc.size_left)
                        {
                           if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == tp->core.pipe_params.data.ep_type)
                           {
                              if(tp->core.pipe_params.data.mps != packet_size)
                              {
                                 USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);

                                 USBD_IO_SET_OUT_PROVIDE_HANDLER(
                                    transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
                                 USBD_IO_SET_OUT_MEMCPY_HANDLER(
                                    transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
                              }
                           }
                           else
                           {
                              USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);

                              USBD_IO_SET_OUT_PROVIDE_HANDLER(
                                 transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
                              USBD_IO_SET_OUT_MEMCPY_HANDLER(
                                 transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
                           }
                        }

                        if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active))
                        {
                           if(tp->core.transfer_params.out.proc.size_left > tp->core.pipe_params.data.mps)
                           {
                              USBD_IO_SET_OUT_PROVIDE_HANDLER(
                                 transaction, USBD_IOTP_EVENT_io_provide_out);
                              USBD_IO_SET_OUT_MEMCPY_HANDLER(
                                 transaction, USBD_IOTP_EVENT_memcpy_out_table[tp->core.pipe_params.data.ep_type]);
                           }
                           else
                           {
                              USBD_IO_SET_OUT_PROVIDE_HANDLER(
                                 transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
                              USBD_IO_SET_OUT_MEMCPY_HANDLER(
                                 transaction, USBD_IOTP_EVENT_memcpy_out_table[tp->core.pipe_params.data.ep_type]);
                           }
                        }
                     }
                     else
                     {
                        result = USBD_IO_UP_Trigger_OUT(
                           USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                           USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
                           USBD_FALSE);
                     }
                  }

                  if(USBD_BOOL_IS_FALSE(result))
                  {
                     USBD_DEBUG_HI(USBD_DBG_IOTPEV_PROCESSING, "trigger failed");

                     USBD_IO_SET_OUT_PROVIDE_HANDLER(
                        transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
                     USBD_IO_SET_OUT_MEMCPY_HANDLER(
                        transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));

                     USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);
                  }

                  if(USBD_CHECK_PTR(USBD_IO_Inout_Data_Size_DT, size_left))
                  {
                     if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active))
                     {
                        *size_left = size;
                     }
                     else
                     {
                        *size_left = (-1);
                     }
                  }
               }
            }
            else
            {
               USBD_EMERG(USBD_DBG_IOTPEV_PROCESSING, "Invoke not implemented!");
            }
         }

         USBD_DEBUG_HI_8(
            USBD_DBG_IOTPEV_PROCESSING,
            "recv_and_wait  after ::"
            "ep: %d::%s; size = %d; tr_active = %s; size_left = %d; dont_wait_for_ready = %s; data = 0x%08X, result: %s",
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_OUT) ? "active" : "passive",
            size,
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.out.proc.size_left,
            USBD_BOOL_IS_TRUE(tp->core.transfer_params.out.proc.dont_wait_for_ready) ? "true" : "false",
            tp->core.transfer_params.out.proc.data,
            USBD_BOOL_IS_TRUE(result) ? "true" : "false");
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_EVENT_Recv_And_Wait */



USBD_Bool_DT USBD_IOTP_EVENT_Recv_Ready(
      USBD_IOTP_EVENT_Params_XT *tp)
{
   void                                  *tp_owner;
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction;
   USBD_IO_OUT_Data_Method_TP_HT          provide;
   USBD_IO_OUT_Data_Method_TP_HT          mem_cpy;
   USBD_Bool_DT                           result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      tp_owner = USBD_IO_UP_Get_OUT_TP_Owner(
         USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp));

      if(USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_EVENT_dummy_data)
         && (USB_EP_DIRECTION_OUT == USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp)))
      {
         USBD_DEBUG_HI_6(
            USBD_DBG_IOTPEV_PROCESSING,
            "recv_ready    before::ep: %d::%s; tr_active = %s; size_left = %d; dont_wait_for_ready = %s; data = 0x%08X",
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_OUT) ? "active" : "passive",
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.out.proc.size_left,
            USBD_BOOL_IS_TRUE(tp->core.transfer_params.out.proc.dont_wait_for_ready) ? "true" : "false",
            tp->core.transfer_params.out.proc.data);

         /* check if transaction can be processed or if request must be synchronizes to USBD task/irq */
         if(USBD_BOOL_IS_FALSE(USBD_IS_INVOKE_NEEDED(USBD_IOTP_EVENT_GET_INVOKE_PARAMS(tp))))
         {
            transaction = USBD_IO_Get_OUT_Transaction_Params(
               USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp));

            provide = USBD_IO_GET_OUT_PROVIDE_HANDLER(transaction);
            mem_cpy = USBD_IO_GET_OUT_MEMCPY_HANDLER(transaction);

            USBD_IO_SET_OUT_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
            USBD_IO_SET_OUT_MEMCPY_HANDLER(transaction,  USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));

            result = USBD_IO_UP_Trigger_OUT(
               USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
               USBD_FALSE);

            USBD_IO_SET_OUT_PROVIDE_HANDLER(transaction, provide);
            USBD_IO_SET_OUT_MEMCPY_HANDLER(transaction,  mem_cpy);
         }
         else
         {
            USBD_EMERG(USBD_DBG_IOTPEV_PROCESSING, "Invoke not implemented!");
         }

         USBD_DEBUG_HI_6(
            USBD_DBG_IOTPEV_PROCESSING,
            "recv_ready    after ::ep: %d::%s; tr_active = %s; size_left = %d; dont_wait_for_ready = %s; data = 0x%08X",
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_OUT) ? "active" : "passive",
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.out.proc.size_left,
            USBD_BOOL_IS_TRUE(tp->core.transfer_params.out.proc.dont_wait_for_ready) ? "true" : "false",
            tp->core.transfer_params.out.proc.data);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_EVENT_Recv_Ready__irq */



USBD_Bool_DT USBD_IOTP_EVENT_Send_Stall(
      USBD_IOTP_EVENT_Params_XT *tp)
{
   void *tp_owner;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      tp_owner = USBD_IO_UP_Get_TP_Owner(
         USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp));

      if(USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_EVENT_dummy_data))
      {
         /* check if transaction can be processed or if request must be synchronizes to USBD task/irq */
         if(USBD_BOOL_IS_FALSE(USBD_IS_INVOKE_NEEDED(USBD_IOTP_EVENT_GET_INVOKE_PARAMS(tp))))
         {
            result = USBD_TRUE;

            USBD_IO_UP_Respond_Stall(
               USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp));
         }
         else
         {
            USBD_EMERG(USBD_DBG_IOTPEV_PROCESSING, "Invoke not implemented!");
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_EVENT_Send_Stall */



static void USBD_IOTP_EVENT_io_provide_out(
   void* tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT packet_size,
   USBD_IO_Inout_Data_Size_DT left_size,
   USBD_IO_OUT_Data_Method_Port_HT provide)
{
   USBD_IOTP_EVENT_Params_XT *tp          = (USBD_IOTP_EVENT_Params_XT*)tp_params;
   USBD_IO_Inout_Data_Size_DT part_size   = 0;

#ifndef USBD_USE_IOCMD
   USBD_UNUSED_PARAM(packet_size);
   USBD_UNUSED_PARAM(left_size);
#endif

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction)
      && USBD_CHECK_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, provide))
   {
      USBD_DEBUG_HI_9(
         USBD_DBG_IOTPEV_PROCESSING,
         "provide_out   before::"
         "ep: %d::%s; packet_size = %d; left_size = %d;"
         " tr_active = %s; size_left = %d; size_transferred: %d; dont_wait_for_ready = %s; data = 0x%08X",
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         packet_size,
         left_size,
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.out.proc.size_left,
         tp->up_link.data.size_transferred,
         USBD_BOOL_IS_TRUE(tp->core.transfer_params.out.proc.dont_wait_for_ready) ? "true" : "false",
         tp->core.transfer_params.out.proc.data);

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active))
      {
         if(tp->core.transfer_params.out.proc.size_left > 1)
         {
            part_size = USBD_CALL_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, provide)(
               USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp),
               tp->core.transfer_params.out.proc.data,
               tp->core.transfer_params.out.proc.size_left - 1,
               USBD_TRUE);
         }

         tp->core.transfer_params.out.proc.size_left       -= part_size;
         tp->core.transfer_params.out.proc.data            += part_size;
         tp->core.transfer_params.out.proc.size_in_progress = part_size;

         if(tp->core.transfer_params.out.proc.size_left < tp->core.pipe_params.data.mps)
         {
            USBD_IO_SET_OUT_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
            USBD_IO_SET_OUT_MEMCPY_HANDLER(transaction, USBD_IOTP_EVENT_memcpy_out_table[tp->core.pipe_params.data.ep_type]);
         }
         else
         {
            USBD_IO_SET_OUT_PROVIDE_HANDLER(transaction, USBD_IOTP_EVENT_io_provide_out);
            USBD_IO_SET_OUT_MEMCPY_HANDLER(transaction, USBD_IOTP_EVENT_memcpy_out_table[tp->core.pipe_params.data.ep_type]);
         }
      }

      USBD_DEBUG_HI_9(
         USBD_DBG_IOTPEV_PROCESSING,
         "provide_out   after ::"
         "ep: %d::%s; packet_size = %d; left_size = %d;"
         " tr_active = %s; size_left = %d; size_transferred: %d; dont_wait_for_ready = %s; data = 0x%08X",
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         packet_size,
         left_size,
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.out.proc.size_left,
         tp->up_link.data.size_transferred,
         USBD_BOOL_IS_TRUE(tp->core.transfer_params.out.proc.dont_wait_for_ready) ? "true" : "false",
         tp->core.transfer_params.out.proc.data);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_EVENT_io_provide_out */



static void USBD_IOTP_EVENT_io_memcpy_out(
   void* tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT packet_size,
   USBD_IO_Inout_Data_Size_DT left_size,
   USBD_IO_OUT_Data_Method_Port_HT data_method)
{
   USBD_IOTP_EVENT_Params_XT *tp          = (USBD_IOTP_EVENT_Params_XT*)tp_params;
   USBD_IO_Inout_Data_Size_DT part_size;
   USBD_Bool_DT               is_last_part= USBD_FALSE;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction)
      && USBD_CHECK_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, data_method))
   {
      USBD_DEBUG_HI_9(
         USBD_DBG_IOTPEV_PROCESSING,
         "memcpy_out    before::"
         "ep: %d::%s; packet_size = %d; left_size = %d;"
         " tr_active = %s; size_left = %d; size_transferred: %d; dont_wait_for_ready = %s; data = 0x%08X",
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         packet_size,
         left_size,
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.out.proc.size_left,
         tp->up_link.data.size_transferred,
         USBD_BOOL_IS_TRUE(tp->core.transfer_params.out.proc.dont_wait_for_ready) ? "true" : "false",
         tp->core.transfer_params.out.proc.data);

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active))
      {
         part_size = tp->core.transfer_params.out.proc.size_left;

         if(part_size > left_size)
         {
            part_size         = left_size;

            if(packet_size < ((USBD_IO_Inout_Data_Size_DT)(tp->core.pipe_params.data.mps)))
            {
               is_last_part   = tp->core.transfer_params.out.proc.dont_wait_for_ready;
            }
            else
            {
               is_last_part   = USBD_TRUE;
            }
         }
         else if(part_size == left_size)
         {
            is_last_part      = tp->core.transfer_params.out.proc.dont_wait_for_ready;
         }

         part_size = USBD_CALL_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, data_method)(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp),
            tp->core.transfer_params.out.proc.data,
            part_size,
            is_last_part);

         tp->up_link.data.size_transferred += part_size;

         if(part_size <= tp->core.transfer_params.out.proc.size_left)
         {
            tp->core.transfer_params.out.proc.size_left    -= part_size;
            tp->core.transfer_params.out.proc.data         += part_size;
         }
         else
         {
            tp->core.transfer_params.out.proc.size_left     = 0;
         }
         tp->core.transfer_params.out.proc.size_in_progress = 0;

         /* no more space or packet smaller than MPS */
         if((0 == tp->core.transfer_params.out.proc.size_left)
            || (packet_size < ((USBD_IO_Inout_Data_Size_DT)(tp->core.pipe_params.data.mps))))
         {
            USBD_IO_SET_OUT_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
            USBD_IO_SET_OUT_MEMCPY_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));

            USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);

            if(USBD_IOTP_EVENT_CHECK_READY_HANDLER(tp))
            {
               left_size = USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(
                  USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                  USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
                  USBD_TRUE);

               if(left_size >= 0)
               {
                  tp->core.transfer_params.out.proc.mem_cpy = data_method;
               }

               USBD_IOTP_EVENT_CALL_READY(
                  USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                  USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
                  USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp),
                  tp,
                  left_size);

               tp->core.transfer_params.out.proc.mem_cpy = USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_Port_HT);
            }
         }
         else
         {
            if(tp->core.transfer_params.out.proc.size_left >= tp->core.pipe_params.data.mps)
            {
               USBD_IO_SET_OUT_PROVIDE_HANDLER(
                  transaction, USBD_IOTP_EVENT_io_provide_out);
               USBD_IO_SET_OUT_MEMCPY_HANDLER(
                  transaction, USBD_IOTP_EVENT_memcpy_out_table[tp->core.pipe_params.data.ep_type]);
            }
            else
            {
               USBD_IO_SET_OUT_PROVIDE_HANDLER(
                  transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
               USBD_IO_SET_OUT_MEMCPY_HANDLER(
                  transaction, USBD_IOTP_EVENT_memcpy_out_table[tp->core.pipe_params.data.ep_type]);
            }
         }
      }

      USBD_DEBUG_HI_9(
         USBD_DBG_IOTPEV_PROCESSING,
         "memcpy_out    after ::"
         "ep: %d::%s; packet_size = %d; left_size = %d;"
         " tr_active = %s; size_left = %d; size_transferred: %d; dont_wait_for_ready = %s; data = 0x%08X",
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         packet_size,
         left_size,
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.out.proc.size_left,
         tp->up_link.data.size_transferred,
         USBD_BOOL_IS_TRUE(tp->core.transfer_params.out.proc.dont_wait_for_ready) ? "true" : "false",
         tp->core.transfer_params.out.proc.data);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_EVENT_io_memcpy_out */



static void USBD_IOTP_EVENT_io_memcpy_out_control(
   void* tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT packet_size,
   USBD_IO_Inout_Data_Size_DT left_size,
   USBD_IO_OUT_Data_Method_Port_HT data_method)
{
   USBD_IOTP_EVENT_Params_XT *tp          = (USBD_IOTP_EVENT_Params_XT*)tp_params;
   USBD_IO_Inout_Data_Size_DT part_size;
   USBD_Bool_DT               is_last_part= USBD_FALSE;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction)
      && USBD_CHECK_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, data_method))
   {
      USBD_DEBUG_HI_9(
         USBD_DBG_IOTPEV_PROCESSING,
         "memcpy_out    before::"
         "ep: %d::%s; packet_size = %d; left_size = %d;"
         " tr_active = %s; size_left = %d; size_transferred: %d; dont_wait_for_ready = %s; data = 0x%08X",
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         packet_size,
         left_size,
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.out.proc.size_left,
         tp->up_link.data.size_transferred,
         USBD_BOOL_IS_TRUE(tp->core.transfer_params.out.proc.dont_wait_for_ready) ? "true" : "false",
         tp->core.transfer_params.out.proc.data);

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active))
      {
         part_size = tp->core.transfer_params.out.proc.size_left;

         if(part_size > left_size)
         {
            part_size         = left_size;

            if(packet_size < ((USBD_IO_Inout_Data_Size_DT)(tp->core.pipe_params.data.mps)))
            {
               is_last_part   = tp->core.transfer_params.out.proc.dont_wait_for_ready;
            }
            else
            {
               is_last_part   = USBD_TRUE;
            }
         }
         else if(part_size == left_size)
         {
            if(((USBD_IO_Inout_Data_Size_DT)(tp->core.pipe_params.data.mps)) == packet_size)
            {
               is_last_part   = USBD_TRUE;
            }
            else
            {
               is_last_part   = tp->core.transfer_params.out.proc.dont_wait_for_ready;
            }
         }

         part_size = USBD_CALL_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, data_method)(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp),
            tp->core.transfer_params.out.proc.data,
            part_size,
            is_last_part);

         tp->up_link.data.size_transferred += part_size;

         if(part_size <= tp->core.transfer_params.out.proc.size_left)
         {
            tp->core.transfer_params.out.proc.size_left    -= part_size;
            tp->core.transfer_params.out.proc.data         += part_size;
         }
         else
         {
            tp->core.transfer_params.out.proc.size_left     = 0;
         }
         tp->core.transfer_params.out.proc.size_in_progress = 0;

         if((0 == tp->core.transfer_params.out.proc.size_left)
            /*&& (USB_EP_DESC_TRANSFER_TYPE_CONTROL == tp->core.pipe_params.data.ep_type)*/
            && (packet_size == tp->core.pipe_params.data.mps)
            && (part_size >= left_size))
         {
            USBD_IO_SET_OUT_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
            USBD_IO_SET_OUT_MEMCPY_HANDLER(transaction, USBD_IOTP_EVENT_memcpy_out_table[tp->core.pipe_params.data.ep_type]);
         }
         else if((0 == tp->core.transfer_params.out.proc.size_left)
            || (packet_size < ((USBD_IO_Inout_Data_Size_DT)(tp->core.pipe_params.data.mps))))
         {
            USBD_IO_SET_OUT_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
            USBD_IO_SET_OUT_MEMCPY_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));

            USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);

            if(USBD_IOTP_EVENT_CHECK_READY_HANDLER(tp))
            {
               left_size = USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(
                  USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                  USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
                  USBD_TRUE);

               if(left_size >= 0)
               {
                  tp->core.transfer_params.out.proc.mem_cpy = data_method;
               }

               USBD_IOTP_EVENT_CALL_READY(
                  USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
                  USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
                  USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp),
                  tp,
                  left_size);

               tp->core.transfer_params.out.proc.mem_cpy = USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_Port_HT);
            }
         }
         else
         {
            if(tp->core.transfer_params.out.proc.size_left >= tp->core.pipe_params.data.mps)
            {
               USBD_IO_SET_OUT_PROVIDE_HANDLER(
                  transaction, USBD_IOTP_EVENT_io_provide_out);
               USBD_IO_SET_OUT_MEMCPY_HANDLER(
                  transaction, USBD_IOTP_EVENT_memcpy_out_table[tp->core.pipe_params.data.ep_type]);
            }
            else
            {
               USBD_IO_SET_OUT_PROVIDE_HANDLER(
                  transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
               USBD_IO_SET_OUT_MEMCPY_HANDLER(
                  transaction, USBD_IOTP_EVENT_memcpy_out_table[tp->core.pipe_params.data.ep_type]);
            }
         }
      }

      USBD_DEBUG_HI_9(
         USBD_DBG_IOTPEV_PROCESSING,
         "memcpy_out    after ::"
         "ep: %d::%s; packet_size = %d; left_size = %d;"
         " tr_active = %s; size_left = %d; size_transferred: %d; dont_wait_for_ready = %s; data = 0x%08X",
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         packet_size,
         left_size,
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.out.proc.size_left,
         tp->up_link.data.size_transferred,
         USBD_BOOL_IS_TRUE(tp->core.transfer_params.out.proc.dont_wait_for_ready) ? "true" : "false",
         tp->core.transfer_params.out.proc.data);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_EVENT_io_memcpy_out_control */

static void USBD_IOTP_EVENT_io_evdata_out(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_OUT_Data_Method_Port_HT mem_cpy)
{
   USBD_IOTP_EVENT_Params_XT *tp = (USBD_IOTP_EVENT_Params_XT*)tp_params;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction))
   {
      USBD_DEBUG_HI_8(
         USBD_DBG_IOTPEV_PROCESSING,
         "evdata_out    before::"
         "ep: %d::%s; size = %d; tr_active = %s; size_left = %d; size_transferred: %d; dont_wait_for_ready = %s; data = 0x%08X",
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         size,
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.out.proc.size_left,
         tp->up_link.data.size_transferred,
         USBD_BOOL_IS_TRUE(tp->core.transfer_params.out.proc.dont_wait_for_ready) ? "true" : "false",
         tp->core.transfer_params.out.proc.data);

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active))
      {
         if((tp->core.transfer_params.out.proc.size_in_progress != USBD_IO_GET_OUT_TRANSFERRED_SIZE(transaction))
            || ((-1) != size))
         {
            USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);
         }
      }

      tp->up_link.data.size_transferred += USBD_IO_GET_OUT_TRANSFERRED_SIZE(transaction);

      if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_active))
      {
         USBD_IO_SET_OUT_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
         USBD_IO_SET_OUT_MEMCPY_HANDLER(transaction,  USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));

         /** informs upper layer that OUT buffer is empty */
         if(USBD_IOTP_EVENT_CHECK_READY_HANDLER(tp))
         {
            tp->core.transfer_params.out.proc.mem_cpy = mem_cpy;

            USBD_IOTP_EVENT_CALL_READY(
               USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp),
               tp,
               size);

            tp->core.transfer_params.out.proc.mem_cpy = USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_Port_HT);
         }
      }

      USBD_DEBUG_HI_8(
         USBD_DBG_IOTPEV_PROCESSING,
         "evdata_out    after ::"
         "ep: %d::%s; size = %d; tr_active = %s; size_left = %d; size_transferred: %d; dont_wait_for_ready = %s; data = 0x%08X",
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         size,
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.out.proc.size_left,
         tp->up_link.data.size_transferred,
         USBD_BOOL_IS_TRUE(tp->core.transfer_params.out.proc.dont_wait_for_ready) ? "true" : "false",
         tp->core.transfer_params.out.proc.data);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_EVENT_io_evdata_out */



/**
 * Handler called by USBD when CONTROL, BULK or INTERRUPT transaction error is detected or transfer is stopped.
 * In that situation transfer is aborted automatically by USBD and upper layer is informed.
 */
static void USBD_IOTP_EVENT_io_abort(void *tp_params, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction)
{
   USBD_IOTP_EVENT_Params_XT *tp = (USBD_IOTP_EVENT_Params_XT*)tp_params;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPEV_PROCESSING,
         "%s for ep: %d::%s, dir: %s",
         "abort",
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp)) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp) ? "out" : "in");

      USBD_MARK_INVOKE_DESTINATION(USBD_IOTP_EVENT_GET_INVOKE_PARAMS(tp));

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active))
      {
         USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);
         if(USB_EP_DESC_DIR_OUT == USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp))
         {
            tp->core.transfer_params.out.proc.data                = USBD_MAKE_INVALID_PTR(USBD_IOTP_EVENT_Data_DT);
            tp->core.transfer_params.out.proc.size_left           = 0;
            tp->core.transfer_params.out.proc.dont_wait_for_ready = USBD_FALSE;
         }
         else
         {
            tp->core.transfer_params.in.proc.data                 = USBD_MAKE_INVALID_PTR(USBD_IOTP_EVENT_Data_DT);
            tp->core.transfer_params.in.proc.size_left            = 0;
            tp->core.transfer_params.in.proc.last_packet_size     = 0;
            tp->core.transfer_params.in.proc.req_in_progress      = USBD_FALSE;

            if(USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction))
            {
               USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
               USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
            }
         }

         if(USBD_IOTP_EVENT_CHECK_ABORT_HANDLER(tp))
         {
            USBD_IOTP_EVENT_CALL_ABORT(
               USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp),
               tp,
               tp->up_link.data.size_transferred);
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_EVENT_io_abort */



/**
 * Handler called by USBD when ISOCHRONOUS transaction error is detected.
 * In that situation transfer is not aborted like for CONTROL/BULK/INTERRUPT
 * but only upper layer is informed about this situation to decide there what shall be done.
 */
static void USBD_IOTP_EVENT_io_error(
   void *tp_params, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction, USBD_IO_Inout_Data_Size_DT size)
{
   USBD_IOTP_EVENT_Params_XT *tp = (USBD_IOTP_EVENT_Params_XT*)tp_params;

   USBD_UNUSED_PARAM(transaction);

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPEV_PROCESSING,
         "%s for ep: %d::%s, dir: %s",
         "error",
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp)) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp) ? "out" : "in");

      USBD_MARK_INVOKE_DESTINATION(USBD_IOTP_EVENT_GET_INVOKE_PARAMS(tp));

      if(USBD_IOTP_EVENT_CHECK_ERROR_HANDLER(tp))
      {
         USBD_IOTP_EVENT_CALL_ERROR(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp),
            tp,
            size);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_EVENT_io_error */



static void USBD_IOTP_EVENT_io_reinit(
   void *tp_params, void *tp_owner, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction, USBD_Bool_DT active)
{
   USBD_IOTP_EVENT_Params_XT    *tp;
   const USB_Endpoint_Desc_DT  *ep_desc;
   uint16_t mps = 1;
   uint8_t  ep_type;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_ONOFF);

   tp = (USBD_IOTP_EVENT_Params_XT*)tp_params;

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp) && USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_EVENT_dummy_data))
   {
      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPEV_PROCESSING,
         "reinit before:: ep: %d::%s; dir: %s; tr_active = %s",
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp)) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false");

      if(USBD_BOOL_IS_FALSE(active))
      {
         USBD_IOTP_EVENT_io_abort(tp_params, transaction);

         USBD_IO_UP_SET_ERROR_HANDLER(&(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT));
         USBD_IO_UP_SET_SFIN_HANDLER (&(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Stall_Fin_HT));
         USBD_IO_UP_SET_ABORT_HANDLER(&(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Abort_HT));
      }

      USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);

      if(USB_EP_DESC_DIR_OUT == USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp))
      {
         USBD_IO_UP_SET_OUT_DATA_EVENT_HANDLER(
            &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_OUT_Data_Event_HT));

         tp->core.transfer_params.out.proc.mem_cpy             = USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_Port_HT);
         tp->core.transfer_params.out.proc.data                = USBD_MAKE_INVALID_PTR(USBD_IOTP_EVENT_Data_DT);
         tp->core.transfer_params.out.proc.size_left           = 0;
         tp->core.transfer_params.out.proc.dont_wait_for_ready = USBD_FALSE;
      }
      else
      {
         USBD_IO_UP_SET_IN_DATA_EVENT_HANDLER(
            &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_IN_Data_Event_HT));

         tp->core.transfer_params.in.proc.data                 = USBD_MAKE_INVALID_PTR(USBD_IOTP_EVENT_Data_DT);
         tp->core.transfer_params.in.proc.size_left            = 0;
         tp->core.transfer_params.in.proc.last_packet_size     = 0;
         tp->core.transfer_params.in.proc.req_in_progress      = USBD_FALSE;
      }

      if(USBD_BOOL_IS_TRUE(active))
      {
         ep_desc = USBD_DEV_Get_EP_Desc(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp));

         if(USBD_CHECK_PTR(const USB_Endpoint_Desc_DT, ep_desc))
         {
            ep_type = ep_desc->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK;

            mps   = 0;
            mps  += ep_desc->wMaxPacketSize.H;
            mps  *= 256;
            mps  += ep_desc->wMaxPacketSize.L;
         }
         else
         {
            ep_type    = USB_EP_DESC_TRANSFER_TYPE_CONTROL;
         }

         tp->core.pipe_params.data.mps    = mps;
         tp->core.pipe_params.data.ep_type = ep_type;

         if(USB_EP_DESC_DIR_OUT == USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp))
         {
            USBD_IO_UP_SET_OUT_DATA_EVENT_HANDLER(&(tp->core.pipe_params.handlers), USBD_IOTP_EVENT_io_evdata_out);
         }
         else
         {
            USBD_IO_UP_SET_IN_DATA_EVENT_HANDLER(&(tp->core.pipe_params.handlers), USBD_IOTP_EVENT_io_evdata_in);
         }

         USBD_IO_UP_SET_ERROR_HANDLER(&(tp->core.pipe_params.handlers),USBD_IOTP_EVENT_error_table[ep_type]);
         USBD_IO_UP_SET_SFIN_HANDLER (&(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Stall_Fin_HT));
         USBD_IO_UP_SET_ABORT_HANDLER(&(tp->core.pipe_params.handlers),USBD_IOTP_EVENT_io_abort);

         USBD_MARK_INVOKE_DESTINATION(USBD_IOTP_EVENT_GET_INVOKE_PARAMS(tp));
      }

      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPEV_PROCESSING,
         "reinit after :: ep: %d::%s; dir: %s; tr_active = %s",
         USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp)) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false");
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_ONOFF);
} /* USBD_IOTP_EVENT_io_reinit */



#ifdef USBD_IOTP_EVENT_POST_IMP_INCLUDE
#include "usbd_iotp_event_post_imp.h"
#endif
