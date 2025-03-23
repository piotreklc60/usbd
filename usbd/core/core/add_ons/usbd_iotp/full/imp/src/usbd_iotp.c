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

#ifndef USBD_IOTP_C_
#define USBD_IOTP_C_
#endif

#ifndef USBD_H_
#include "usbd.h"
#endif

#ifndef USBD_PUBLIC_MACRO_H_
#include "usbd_public_macro.h"
#endif

#ifndef USBD_IOTP_INTERNAL_H_
#include "usbd_iotp_internal.h"
#endif



static USBD_Bool_DT USBD_IOTP_proc_abort_invoked(USBD_IOTP_Params_XT *tp, USBD_Bool_DT flush_hw_bufs);
#ifdef USBD_EVENT_PRESENT
static void USBD_IOTP_proc_event(USBD_IOTP_Params_XT *tp, uint8_t req);
static void USBD_IOTP_event(
   USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason);
#endif
static void USBD_IOTP_io_abort(void *tp_params, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction);
#if(USBD_FEATURE_PRESENT == USBD_IO_ISOCHRONOUS_TRANSFER_SUPPORTED)
static void USBD_IOTP_io_error(
   void *tp_params, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction, USBD_IO_Inout_Data_Size_DT size);
#endif
static void USBD_IOTP_io_reinit(
   void *tp_params, void *tp_owner, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction, USBD_Bool_DT active);



static const USBD_IO_UP_Error_HT USBD_IOTP_error_table[4] =
{
   [USB_EP_DESC_TRANSFER_TYPE_CONTROL]      = USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT),
#if(USBD_FEATURE_PRESENT == USBD_IO_ISOCHRONOUS_TRANSFER_SUPPORTED)
   [USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS]  = USBD_IOTP_io_error,
#else
   [USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS]  = USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT),
#endif
   [USB_EP_DESC_TRANSFER_TYPE_BULK]         = USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT),
   [USB_EP_DESC_TRANSFER_TYPE_INTERRUPT]    = USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT)
};



USBD_Atomic_Bool_DT USBD_IOTP_refresh_trigger;
const uint8_t USBD_IOTP_not_ring_infinite_owner[1] = {0};
#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS)
const uint8_t USBD_IOTP_ring_infinite_owner[1] = {0};
#endif

#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS)
USBD_Bool_DT USBD_IOTP_check_both_tp_owners(void *tp_owner)
{
   return (USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_not_ring_infinite_owner)
         || USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_ring_infinite_owner));
} /* USBD_IOTP_check_both_tp_owners */
#endif

static USBD_Bool_DT USBD_IOTP_init_internal(
      USBD_Params_XT *usbd,
      USBDC_Params_XT *usbdc,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_IOTP_Params_XT *tp,
      size_t tp_size)
{
#ifdef USBD_EVENT_PRESENT
   USBD_EVENT_Event_Header_XT *event;
#endif
   USBD_Bool_DT result = USBD_FALSE;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_ONOFF);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      memset(tp, 0, tp_size);

      USBD_IOTP_SET_USBD_INSIDE_TP(tp, usbd);
      USBD_IOTP_SET_USBDC_INSIDE_TP(tp, usbdc);
      USBD_IOTP_SET_EP_NUM_INSIDE_TP(tp, ep_num);
      USBD_IOTP_SET_EP_DIR_INSIDE_TP(tp, dir);

      USBD_INIT_INVOKE_PARAMS(USBD_IOTP_GET_INVOKE_PARAMS(tp));

#ifdef USBD_EVENT_PRESENT
      event = USBDC_EVENT_Install(usbdc, USBD_IOTP_event, USBD_EVENT_REASON_SOF_RECEIVED | USBD_EVENT_INSTALL_ONLY_ONCE);

      if(!USBD_CHECK_PTR(USBD_EVENT_Event_Header_XT, event))
      {
         USBD_WARN(USBD_DBG_IOTPEV_INVALID_PARAMS, "IOTP event cannot be installed");
      }
      else
      {
         /* ok */
         result = USBD_TRUE;
      }
#endif
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_ONOFF);

   return result;
} /* USBD_IOTP_init_internal */

void USBD_IOTP_Init(
      USBD_Params_XT *usbd,
      USBDC_Params_XT *usbdc,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_IOTP_Params_XT *tp)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_ONOFF);

   if(USBD_BOOL_IS_TRUE(USBD_IOTP_init_internal(usbd, usbdc, ep_num, dir, tp, sizeof(USBD_IOTP_Params_XT))))
   {
#if (USBD_FEATURE_NOT_PRESENT != USBD_USE_INVALIDATION_ON_INIT)
      USBD_INVALIDATE_HANDLER_ON_INIT(USBD_IOTP_Callback_HT, tp->up_link.handlers.ready);
      USBD_INVALIDATE_HANDLER_ON_INIT(USBD_IOTP_Callback_HT, tp->up_link.handlers.buf_empty);
      USBD_INVALIDATE_HANDLER_ON_INIT(USBD_IOTP_Callback_HT, tp->up_link.handlers.error);
      USBD_INVALIDATE_HANDLER_ON_INIT(USBD_IOTP_Callback_HT, tp->up_link.handlers.abort);
#endif
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_ONOFF);
} /* USBD_IOTP_Init */

#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS)
void USBD_IOTP_Init_Infinitive_Only(
      USBD_Params_XT  *usbd,
      USBDC_Params_XT *usbdc,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_IOTP_Params_Ring_Infinite_Only_XT *tp,
      Buff_Ring_XT *buff)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_ONOFF);

   if(USBD_BOOL_IS_TRUE(USBD_IOTP_init_internal(
      usbd, usbdc, ep_num, dir, (USBD_IOTP_Params_XT*)tp, sizeof(USBD_IOTP_Params_Ring_Infinite_Only_XT))))
   {
      if(USBD_BOOL_IS_TRUE(USBD_IOTP_Ring_Start_Transfer_Infinitely((USBD_IOTP_Params_XT*)tp, buff)))
      {
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_ONOFF);
} /* USBD_IOTP_Init_Infinitive_Only */
#endif

USBD_Bool_DT USBD_IOTP_Install(
      USBD_IOTP_Params_XT *tp)
{
   void *tp_owner = (void*)(USBD_IOTP_not_ring_infinite_owner);
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_ONOFF);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      if(USBD_CHECK_PTR(void, USBD_IOTP_GET_USBD_FROM_TP(tp))
         && (USBD_IOTP_GET_EP_NUM_FROM_TP(tp) < USBD_MAX_NUM_ENDPOINTS)
         && ((USB_EP_DIRECTION_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp))
            || (USB_EP_DIRECTION_IN == USBD_IOTP_GET_EP_DIR_FROM_TP(tp))) )
      {
         if(USBD_BOOL_IS_FALSE(USBD_IO_UP_Is_TP_Locked(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_GET_EP_DIR_FROM_TP(tp))))
         {
            USBD_IO_UP_SET_ERROR_HANDLER(
               &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT));
            USBD_IO_UP_SET_SFIN_HANDLER (
               &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Stall_Fin_HT));
            USBD_IO_UP_SET_REINIT_HANDLER(
               &(tp->core.pipe_params.handlers),USBD_IOTP_io_reinit);
            USBD_IO_UP_SET_ABORT_HANDLER (
               &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Abort_HT));
            USBD_IO_UP_SET_OUT_DATA_EVENT_HANDLER(
               &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_OUT_Data_Event_HT));

            USBD_ATOMIC_BOOL_SET(tp->core.transfer_params.transfer_active, USBD_FALSE);

#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS)
            if(USBD_IOTP_DATA_RING_INFINITIVE == tp->core.transfer_params.data_type)
            {
               tp_owner = (void*)USBD_IOTP_ring_infinite_owner;
            }
            else
#endif
            {
               if(USB_EP_DIRECTION_IN == USBD_IOTP_GET_EP_DIR_FROM_TP(tp))
               {
                  tp->core.transfer_params.dir.in.last_packet_size= 0;
                  tp->core.transfer_params.dir.in.req_in_progress = USBD_FALSE;
               }
               else
               {
                  tp->core.transfer_params.dir.out.mem_cpy              = USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_Port_HT);
                  tp->core.transfer_params.dir.out.dont_wait_for_ready  = USBD_FALSE;
               }

               tp->core.transfer_params.data.data.linear       = USBD_MAKE_INVALID_PTR(USBD_IOTP_Data_DT);
               tp->core.transfer_params.data.offset            = 0;
               tp->core.transfer_params.data.size              = 0;
            }

            USBD_IO_UP_Set_TP(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_GET_EP_DIR_FROM_TP(tp),
               &(tp->core.pipe_params.handlers),
               tp,
               tp_owner);

            result = USBD_TRUE;
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_ONOFF);

   return result;
} /* USBD_IOTP_Install */

USBD_Bool_DT USBD_IOTP_Install_Default_Control_Pipe(
      USBD_Params_XT *usbd,
      USBD_IOTP_Params_XT *tp_in,
      USBD_IOTP_Params_XT *tp_out)
{
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_ONOFF);

   result = USBD_FALSE;

   if((USBD_CHECK_PTR(void, usbd))
      && (USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp_in)) && (USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp_out)))
   {
      /** initializes IN pipe */
      USBD_IOTP_Init(usbd, USBD_MAKE_INVALID_PTR(void), 0, USB_EP_DIRECTION_IN, tp_in);

      USBD_IO_UP_SET_IN_DATA_EVENT_HANDLER(
         &(tp_in->core.pipe_params.handlers) ,USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_IN_Data_Event_HT));
      USBD_IO_UP_SET_ERROR_HANDLER    (
         &(tp_in->core.pipe_params.handlers) ,USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT));
      USBD_IO_UP_SET_SFIN_HANDLER     (
         &(tp_in->core.pipe_params.handlers) ,USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Stall_Fin_HT));
      USBD_IO_UP_SET_REINIT_HANDLER   (
         &(tp_in->core.pipe_params.handlers) ,USBD_IOTP_io_reinit);
      USBD_IO_UP_SET_ABORT_HANDLER    (
         &(tp_in->core.pipe_params.handlers) ,USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Abort_HT));

      USBD_IO_UP_Set_TP(
         usbd,
         0,
         USB_EP_DIRECTION_IN,
         &(tp_in->core.pipe_params.handlers),
         tp_in,
         (void*)USBD_IOTP_not_ring_infinite_owner);

      /** initializes OUT pipe */
      USBD_IOTP_Init(usbd, USBD_MAKE_INVALID_PTR(void), 0, USB_EP_DIRECTION_OUT, tp_out);

      USBD_IO_UP_SET_OUT_DATA_EVENT_HANDLER(
         &(tp_out->core.pipe_params.handlers) ,USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_OUT_Data_Event_HT));
      USBD_IO_UP_SET_ERROR_HANDLER    (
         &(tp_out->core.pipe_params.handlers) ,USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT));
      USBD_IO_UP_SET_SFIN_HANDLER     (
         &(tp_out->core.pipe_params.handlers) ,USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Stall_Fin_HT));
      USBD_IO_UP_SET_REINIT_HANDLER   (
         &(tp_out->core.pipe_params.handlers) ,USBD_IOTP_io_reinit);
      USBD_IO_UP_SET_ABORT_HANDLER    (
         &(tp_out->core.pipe_params.handlers) ,USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Abort_HT));

      USBD_IO_UP_Set_TP(
         usbd,
         0,
         USB_EP_DIRECTION_OUT,
         &(tp_out->core.pipe_params.handlers),
         tp_out,
         (void*)USBD_IOTP_not_ring_infinite_owner);

      /** locks IN and OUT pipes */
      USBD_IO_UP_Lock_TP_Params(usbd, 0, USB_EP_DIRECTION_IN);
      USBD_IO_UP_Lock_TP_Params(usbd, 0, USB_EP_DIRECTION_OUT);

      result = USBD_TRUE;
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_ONOFF);

   return result;
} /* USBD_IOTP_Install_Default_Control_Pipe */



USBD_Params_XT *USBD_IOTP_Get_USBD(
      USBD_IOTP_Params_XT *tp)
{
   USBD_Params_XT *result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   result = USBD_MAKE_INVALID_PTR(USBD_Params_XT);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      result = USBD_IOTP_GET_USBD_FROM_TP(tp);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);

   return result;
} /* USBD_IOTP_Get_USBD */

USBDC_Params_XT *USBD_IOTP_Get_USBDC(
      USBD_IOTP_Params_XT *tp)
{
   USBDC_Params_XT *result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   result = USBD_MAKE_INVALID_PTR(USBDC_Params_XT);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      result = USBD_IOTP_GET_USBDC_FROM_TP(tp);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);

   return result;
} /* USBD_IOTP_Get_USBDC */

uint8_t USBD_IOTP_Get_EP_Num(
      USBD_IOTP_Params_XT *tp)
{
   uint8_t result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   result = USBD_MAX_NUM_ENDPOINTS;

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      result = USBD_IOTP_GET_EP_NUM_FROM_TP(tp);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);

   return result;
} /* USBD_IOTP_Get_EP_Num */

USB_EP_Direction_ET USBD_IOTP_Get_Dir(
      USBD_IOTP_Params_XT *tp)
{
   USB_EP_Direction_ET result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   result = USB_EP_DIRECTION_OUT;

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      result = USBD_IOTP_GET_EP_DIR_FROM_TP(tp);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);

   return result;
} /* USBD_IOTP_Get_Dir */

USBD_IO_Inout_Data_Size_DT USBD_IOTP_Get_Transferred_Size(
      USBD_IOTP_Params_XT *tp)
{
   USBD_IO_Inout_Data_Size_DT result = 0;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      result = USBD_IOTP_GET_SIZE_TRANSFERRED(tp);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);

   return result;
} /* USBD_IOTP_Get_Transferred_Size */



void USBD_IOTP_Set_Handlers(
      USBD_IOTP_Params_XT *tp,
      USBD_IOTP_Callback_HT ready,
      USBD_IOTP_Callback_HT buf_empty,
      USBD_IOTP_Callback_HT error,
      USBD_IOTP_Callback_HT abort)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      USBD_IOTP_SET_READY_HANDLER(tp, ready);
      USBD_IOTP_SET_BUF_EMPTY_HANDLER(tp, buf_empty);
      USBD_IOTP_SET_ERROR_HANDLER(tp, error);
      USBD_IOTP_SET_ABORT_HANDLER(tp, abort);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);
} /* USBD_IOTP_Set_Handlers */

void USBD_IOTP_Set_Ready_Handler(
      USBD_IOTP_Params_XT *tp,
      USBD_IOTP_Callback_HT ready)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      USBD_IOTP_SET_READY_HANDLER(tp, ready);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);
} /* USBD_IOTP_Set_Ready_Handler */

void USBD_IOTP_Set_Buf_Empty_Handler(
      USBD_IOTP_Params_XT *tp,
      USBD_IOTP_Callback_HT buf_empty)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      USBD_IOTP_SET_BUF_EMPTY_HANDLER(tp, buf_empty);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);
} /* USBD_IOTP_Set_Buf_Empty_Handler */

void USBD_IOTP_Set_Error_Handler(
      USBD_IOTP_Params_XT *tp,
      USBD_IOTP_Callback_HT error)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      USBD_IOTP_SET_ERROR_HANDLER(tp, error);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);
} /* USBD_IOTP_Set_Error_Handler */

void USBD_IOTP_Set_Abort_Handler(
      USBD_IOTP_Params_XT *tp,
      USBD_IOTP_Callback_HT abort)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      USBD_IOTP_SET_ABORT_HANDLER(tp, abort);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);
} /* USBD_IOTP_Set_Abort_Handler */

USBD_Vendor_Data_XT *USBD_IOTP_Get_Vendor_Data_Container(USBD_IOTP_Params_XT *tp)
{
   USBD_Vendor_Data_XT *result = USBD_MAKE_INVALID_PTR(USBD_Vendor_Data_XT);

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      result = USBD_IOTP_GET_VENDOR_DATA_PTR(tp);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);

   return result;
} /* USBD_IOTP_Get_Vendor_Data_Container */



USBD_Bool_DT USBD_IOTP_Is_Transfer_Active(
      USBD_IOTP_Params_XT *tp)
{
   void *tp_owner;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_STATE);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      tp_owner = USBD_IO_UP_Get_TP_Owner(
         USBD_IOTP_GET_USBD_FROM_TP(tp),
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IOTP_GET_EP_DIR_FROM_TP(tp));

      if(USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_not_ring_infinite_owner))
      {
         result = USBD_ATOMIC_BOOL_GET(tp->core.transfer_params.transfer_active);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);

   return result;
} /* USBD_IOTP_Is_Transfer_Active */



static USBD_Bool_DT USBD_IOTP_proc_abort_invoked(USBD_IOTP_Params_XT *tp, USBD_Bool_DT flush_hw_bufs)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   USBD_DEBUG_HI_5(
      USBD_DBG_IOTPEV_PROCESSING,
      "%s::ep: %d::%s; tr_active = %s; data_type: %d", "abort before",
      USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
      USBD_IO_UP_Is_EP_Active(
         USBD_IOTP_GET_USBD_FROM_TP(tp),
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IOTP_GET_EP_DIR_FROM_TP(tp)) ? "active" : "passive",
      USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
      tp->core.transfer_params.data_type);

   if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) || USBD_BOOL_IS_TRUE(flush_hw_bufs))
   {
      USBD_IO_Abort(
         USBD_IOTP_GET_USBD_FROM_TP(tp),
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IOTP_GET_EP_DIR_FROM_TP(tp),
         flush_hw_bufs);
   }

   USBD_DEBUG_HI_5(
      USBD_DBG_IOTPEV_PROCESSING,
      "%s:: ep: %d::%s; tr_active = %s; data_type: %d", "abort after ",
      USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
      USBD_IO_UP_Is_EP_Active(
         USBD_IOTP_GET_USBD_FROM_TP(tp),
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IOTP_GET_EP_DIR_FROM_TP(tp)) ? "active" : "passive",
      USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
      tp->core.transfer_params.data_type);

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   /* this returned value MUST be always true, it is needed for Invoke mechanism */
   return USBD_TRUE;
} /* USBD_IOTP_proc_abort_invoked */



USBD_Bool_DT USBD_IOTP_Abort(
      USBD_IOTP_Params_XT *tp,
      USBD_Bool_DT flush_hw_bufs)
{
   void *tp_owner;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      tp_owner = USBD_IO_UP_Get_TP_Owner(
         USBD_IOTP_GET_USBD_FROM_TP(tp),
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IOTP_GET_EP_DIR_FROM_TP(tp));

      if(USBD_BOOL_IS_TRUE(USBD_IOTP_check_both_tp_owners(tp_owner)))
      {
         USBD_DEBUG_HI_5(
            USBD_DBG_IOTPEV_PROCESSING,
            "%s::ep: %d::%s; tr_active = %s; data_type: %d", "abort before",
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_GET_EP_DIR_FROM_TP(tp)) ? "active" : "passive",
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data_type);

         if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) || USBD_BOOL_IS_TRUE(flush_hw_bufs))
         {
            /* try to synchronize transaction to USBD task/irq */
            if(!USBD_INVOKE(
               USBD_IOTP_GET_INVOKE_PARAMS(tp), USBD_IOTP_proc_abort_invoked(tp, flush_hw_bufs)))
            {
               USBD_DEBUG_HI_5(
                  USBD_DBG_IOTPEV_PROCESSING,
                  "%s::ep: %d::%s; tr_active = %s; data_type: %d", "abort invoke",
                  USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
                  USBD_IO_UP_Is_EP_Active(
                     USBD_IOTP_GET_USBD_FROM_TP(tp),
                     USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
                     USB_EP_DIRECTION_IN) ? "active" : "passive",
                  USB_EP_DESC_DIR_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
                  tp->core.transfer_params.data_type);

                  USBD_ATOMIC_UINT8_SET(
                     USBD_IOTP_GET_INVOKE_REQ(tp),
                     USBD_BOOL_IS_TRUE(flush_hw_bufs) ? USBD_IOTP_INVOKE_REQ_ABORT_FLUSH_HW : USBD_IOTP_INVOKE_REQ_ABORT_NO_FLUSH_HW);

                  USBD_ATOMIC_BOOL_SET(USBD_IOTP_refresh_trigger, USBD_TRUE);
            }
         }

         USBD_DEBUG_HI_5(
            USBD_DBG_IOTPEV_PROCESSING,
            "%s::ep: %d::%s; tr_active = %s; data_type: %d", "abort after ",
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_GET_EP_DIR_FROM_TP(tp)) ? "active" : "passive",
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data_type);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_Abort */



#ifdef USBD_EVENT_PRESENT
static void USBD_IOTP_proc_event(USBD_IOTP_Params_XT *tp, uint8_t req)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_EVENT);

   USBD_ATOMIC_UINT8_SET(USBD_IOTP_GET_INVOKE_REQ(tp), USBD_IOTP_INVOKE_REQ_NONE);

   if(USBD_IOTP_INVOKE_REQ_ABORT_NO_FLUSH_HW == req)
   {
      (void)USBD_IOTP_proc_abort_invoked(tp, USBD_FALSE);
   }
   else if(USBD_IOTP_INVOKE_REQ_ABORT_FLUSH_HW == req)
   {
      (void)USBD_IOTP_proc_abort_invoked(tp, USBD_TRUE);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_EVENT);
} /* USBD_IOTP_proc_event */

static void USBD_IOTP_event(
   USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_IOTP_Params_XT  *tp;
#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS)
   USBD_IO_Inout_Data_Size_DT waiting_size;
#endif
   uint8_t ep_num;
   uint8_t req;

   USBD_UNUSED_PARAM(usbdc);
   USBD_UNUSED_PARAM(event_params);
   USBD_UNUSED_PARAM(reason);

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_EVENT);

   if(USBD_ATOMIC_BOOL_IS_TRUE(USBD_IOTP_refresh_trigger))
   {
      for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
      {
         /* OUT TP */
         if(USBD_BOOL_IS_TRUE(USBD_IOTP_check_both_tp_owners(USBD_IO_UP_Get_OUT_TP_Owner(usbd, ep_num))))
         {
            tp = USBD_IO_UP_Get_OUT_TP_Params(usbd, ep_num);
            req = USBD_ATOMIC_UINT8_GET(USBD_IOTP_GET_INVOKE_REQ(tp));

            if(USBD_IOTP_INVOKE_REQ_NONE != req)
            {
               USBD_IOTP_proc_event(tp, req);
            }

#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS)
            if(USBD_IOTP_DATA_RING_INFINITIVE == tp->core.transfer_params.data_type)
            {
               waiting_size = USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(usbd, ep_num, USBD_TRUE);

               if((waiting_size > 0)
                  && ((USBD_IO_Inout_Data_Size_DT)BUFF_RING_GET_FREE_SIZE(tp->core.transfer_params.data.data.ring) >= waiting_size))
               {
                  USBD_IOTP_Trigger_Out_Invoked(usbd, ep_num);
               }
            }
#endif
         }
         /* IN TP */
         if(USBD_BOOL_IS_TRUE(USBD_IOTP_check_both_tp_owners(USBD_IO_UP_Get_IN_TP_Owner(usbd, ep_num))))
         {
            tp = USBD_IO_UP_Get_IN_TP_Params(usbd, ep_num);
            req = USBD_ATOMIC_UINT8_GET(USBD_IOTP_GET_INVOKE_REQ(tp));

            if(USBD_IOTP_INVOKE_REQ_NONE != req)
            {
               USBD_IOTP_proc_event(tp, req);
            }

#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS)
            if(USBD_IOTP_DATA_RING_INFINITIVE == tp->core.transfer_params.data_type)
            {
               if((!BUFF_RING_IS_EMPTY(tp->core.transfer_params.data.data.ring))
                  && (USBD_IO_UP_EP_IN_Get_Buffered_Data_Size(usbd, ep_num) < 0))
               {
                  USBD_IOTP_Trigger_In_Invoked(usbd, ep_num);
               }
            }
#endif
         }
      }

      USBD_ATOMIC_BOOL_SET(USBD_IOTP_refresh_trigger, USBD_FALSE);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_EVENT);
} /* USBD_IOTP_event */
#endif



/**
 * Handler called by USBD when CONTROL, BULK or INTERRUPT transaction error is detected or transfer is stopped.
 * In that situation transfer is aborted automatically by USBD and upper layer is informed.
 */
static void USBD_IOTP_io_abort(void *tp_params, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction)
{
   USBD_IOTP_Params_XT *tp = (USBD_IOTP_Params_XT*)tp_params;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPEV_PROCESSING,
         "%s for ep: %d::%s, dir: %s",
         "abort",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_GET_EP_DIR_FROM_TP(tp)) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp) ? "out" : "in");

      USBD_MARK_INVOKE_DESTINATION(USBD_IOTP_GET_INVOKE_PARAMS(tp));

#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS)
      if(USBD_IOTP_DATA_RING_INFINITIVE == tp->core.transfer_params.data_type)
      {
         USBD_IOTP_Ring_Clear_Buff(tp, transaction, USBD_FALSE);
      }
      else
#endif
         if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active))
      {
         USBD_ATOMIC_BOOL_SET(tp->core.transfer_params.transfer_active, USBD_FALSE);
         tp->core.transfer_params.data_type        = USBD_IOTP_DATA_LINEAR;
         tp->core.transfer_params.data.data.linear = USBD_MAKE_INVALID_PTR(USBD_IOTP_Data_DT);
         tp->core.transfer_params.data.offset      = 0;
         tp->core.transfer_params.data.size        = 0;
         if(USB_EP_DESC_DIR_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp))
         {
            tp->core.transfer_params.dir.out.dont_wait_for_ready = USBD_FALSE;
         }
         else
         {
            tp->core.transfer_params.dir.in.last_packet_size     = 0;
            tp->core.transfer_params.dir.in.req_in_progress      = USBD_FALSE;

            if(USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction))
            {
               USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
               USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
            }
         }

         USBD_IOTP_CHECK_AND_CALL_ABORT(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_GET_EP_DIR_FROM_TP(tp),
            tp,
            tp->up_link.data.size_transferred);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_io_abort */



/**
 * Handler called by USBD when ISOCHRONOUS transaction error is detected.
 * In that situation transfer is not aborted like for CONTROL/BULK/INTERRUPT
 * but only upper layer is informed about this situation to decide there what shall be done.
 */
#if(USBD_FEATURE_PRESENT == USBD_IO_ISOCHRONOUS_TRANSFER_SUPPORTED)
static void USBD_IOTP_io_error(
   void *tp_params, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction, USBD_IO_Inout_Data_Size_DT size)
{
   USBD_IOTP_Params_XT *tp = (USBD_IOTP_Params_XT*)tp_params;

   USBD_UNUSED_PARAM(transaction);

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPEV_PROCESSING,
         "%s for ep: %d::%s, dir: %s",
         "error",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_GET_EP_DIR_FROM_TP(tp)) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp) ? "out" : "in");

      USBD_MARK_INVOKE_DESTINATION(USBD_IOTP_GET_INVOKE_PARAMS(tp));

#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS)
      if(USBD_IOTP_DATA_RING_INFINITIVE == tp->core.transfer_params.data_type)
      {
         USBD_IOTP_Ring_Clear_Buff(tp, transaction, USBD_FALSE);
      }
#endif

      USBD_IOTP_CHECK_AND_CALL_ERROR(
         USBD_IOTP_GET_USBD_FROM_TP(tp),
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IOTP_GET_EP_DIR_FROM_TP(tp),
         tp,
         size);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_io_error */
#endif



static void USBD_IOTP_io_reinit(
   void *tp_params, void *tp_owner, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction, USBD_Bool_DT active)
{
   USBD_IOTP_Params_XT         *tp;
   const USB_Endpoint_Desc_DT  *ep_desc;
   uint16_t mps = 1;
   uint8_t  ep_type;

   USBD_UNUSED_PARAM(tp_owner);

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_ONOFF);

   tp = (USBD_IOTP_Params_XT*)tp_params;

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPEV_PROCESSING,
         "reinit before:: ep: %d::%s; dir: %s; tr_active = %s",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_GET_EP_DIR_FROM_TP(tp)) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false");

      if(USBD_BOOL_IS_FALSE(active))
      {
         USBD_CALL_HANDLER(USBD_IO_UP_Abort_HT, tp->core.pipe_params.handlers.abort)(
            tp_params,
            transaction);
      }

      USBD_IO_UP_SET_ERROR_HANDLER(&(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT));
      USBD_IO_UP_SET_SFIN_HANDLER (&(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Stall_Fin_HT));
      USBD_IO_UP_SET_ABORT_HANDLER(&(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Abort_HT));
      USBD_IO_UP_SET_OUT_DATA_EVENT_HANDLER(
         &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_OUT_Data_Event_HT));

      USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
      USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
      USBD_IO_SET_OUT_TRANSFERRED_SIZE(transaction, 0);

      USBD_ATOMIC_BOOL_SET(tp->core.transfer_params.transfer_active, USBD_FALSE);

#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS)
      if(USBD_IOTP_DATA_RING_INFINITIVE != tp->core.transfer_params.data_type)
#endif
      {
         tp->core.transfer_params.data.data.linear = USBD_MAKE_INVALID_PTR(USBD_IOTP_Data_DT);
         tp->core.transfer_params.data.offset      = 0;
         tp->core.transfer_params.data.size        = 0;

         if(USB_EP_DESC_DIR_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp))
         {
            tp->core.transfer_params.dir.out.mem_cpy             = USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_Port_HT);
            tp->core.transfer_params.dir.out.dont_wait_for_ready = USBD_FALSE;
         }
         else
         {
            tp->core.transfer_params.dir.in.last_packet_size     = 0;
            tp->core.transfer_params.dir.in.req_in_progress      = USBD_FALSE;
         }
      }

      if(USBD_BOOL_IS_TRUE(active))
      {
         ep_desc = USBD_DEV_Get_EP_Desc(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_GET_EP_DIR_FROM_TP(tp));

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

         tp->core.pipe_params.data.mps       = mps;
         tp->core.pipe_params.data.ep_type   = ep_type;

#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS)
         if(USBD_IOTP_DATA_RING_INFINITIVE == tp->core.transfer_params.data_type)
         {
            if(USB_EP_DESC_DIR_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp))
            {
               USBD_IO_UP_SET_OUT_DATA_EVENT_HANDLER(&(tp->core.pipe_params.handlers), USBD_IOTP_Ring_Io_Evdata_Out);
            }
            else
            {
               USBD_IO_UP_SET_IN_DATA_EVENT_HANDLER(&(tp->core.pipe_params.handlers), USBD_IOTP_Ring_Io_Evdata_In);
            }

            USBD_IOTP_Ring_Clear_Buff(tp, transaction, USBD_TRUE);
         }
         else
#endif
         {
            if(USB_EP_DESC_DIR_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp))
            {
               USBD_IO_UP_SET_OUT_DATA_EVENT_HANDLER(&(tp->core.pipe_params.handlers), USBD_IOTP_Io_Evdata_Out);
            }
            else
            {
               USBD_IO_UP_SET_IN_DATA_EVENT_HANDLER(&(tp->core.pipe_params.handlers), USBD_IOTP_Io_Evdata_In);
            }
         }
         USBD_IO_UP_SET_ERROR_HANDLER(&(tp->core.pipe_params.handlers),USBD_IOTP_error_table[ep_type]);
         USBD_IO_UP_SET_SFIN_HANDLER (&(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Stall_Fin_HT));
         USBD_IO_UP_SET_ABORT_HANDLER(&(tp->core.pipe_params.handlers),USBD_IOTP_io_abort);

         USBD_MARK_INVOKE_DESTINATION(USBD_IOTP_GET_INVOKE_PARAMS(tp));
      }

      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPEV_PROCESSING,
         "reinit after :: ep: %d::%s; dir: %s; tr_active = %s",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_GET_EP_DIR_FROM_TP(tp)) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false");
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_ONOFF);
} /* USBD_IOTP_io_reinit */



#ifdef USBD_IOTP_POST_IMP_INCLUDE
#include "usbd_iotp_post_imp.h"
#endif
