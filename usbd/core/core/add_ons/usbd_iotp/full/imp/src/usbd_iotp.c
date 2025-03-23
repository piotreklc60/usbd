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



#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_COMPLEX_BUFFERS)
typedef struct
{
   union
   {
      const Buff_Readable_Vector_XT   *vector;
      const Buff_Readable_Tree_XT     *tree;
   }data;
   Buff_Num_Elems_DT                   data_num_elems;
}USBD_IOTP_complex_data_container_XT;
#endif



typedef void (*USBD_IOTP_send_invoked_connect_data_HT)(
   USBD_IOTP_Params_XT       *tp,
   const void                *data,
   USBD_IO_Inout_Data_Size_DT size);



typedef USBD_Bool_DT (*USBD_IOTP_ring_start_stop_transfer_infinitely_invoked_HT)(
   USBD_IOTP_Params_XT *tp, Buff_Ring_XT *ring, USBD_Bool_DT *result);



typedef USBD_IO_Inout_Data_Size_DT (*USBD_IOTP_io_data_memcpy_in_HT)(
   USBD_IOTP_Params_XT *tp, USBD_IO_IN_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size_left);

typedef USBD_IO_Inout_Data_Size_DT (*USBD_IOTP_io_data_memcpy_out_HT)(
   USBD_IOTP_Params_XT *tp, USBD_IO_OUT_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);

typedef struct usbd_iotp_vendor_memcpy_eXtended_Tag
{
   USBD_IOTP_Params_XT                *tp;
   union
   {
      USBD_IO_IN_Data_Method_Port_HT   in;
      USBD_IO_OUT_Data_Method_Port_HT  out;
   }data_method;
   USBD_IO_Inout_Data_Size_DT          out_left_data_size;
   USBD_Bool_DT                        is_last_part;
}usbd_iotp_vendor_memcpy_XT;



static USBD_Bool_DT USBD_IOTP_proc_abort_invoked(USBD_IOTP_Params_XT *tp, USBD_Bool_DT flush_hw_bufs);
static USBD_Bool_DT USBD_IOTP_trigger_in_invoked(USBD_Params_XT *usbd, uint8_t ep_num);
static USBD_Bool_DT USBD_IOTP_trigger_out_invoked(USBD_Params_XT *usbd, uint8_t ep_num);
#ifdef USBD_EVENT_PRESENT
static void USBD_IOTP_proc_event(USBD_IOTP_Params_XT *tp, uint8_t req);
static void USBD_IOTP_event(
   USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason);
#endif
static void USBD_IOTP_send_invoked_connect_data_linear(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size);
#if(USBD_IOTP_SUPPORT_VECTOR_BUFFERS == USBD_FEATURE_PRESENT)
static void USBD_IOTP_send_invoked_connect_data_vector(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size);
#endif
#if(USBD_IOTP_SUPPORT_TREE_BUFFERS == USBD_FEATURE_PRESENT)
static void USBD_IOTP_send_invoked_connect_data_tree(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size);
#endif
#if(USBD_IOTP_SUPPORT_RING_BUFFERS == USBD_FEATURE_PRESENT)
static void USBD_IOTP_send_invoked_connect_data_ring(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size);
#endif
static USBD_Bool_DT USBD_IOTP_send_invoked(
   USBD_IOTP_Params_XT                    *tp,
   USBD_IOTP_send_invoked_connect_data_HT  connect_data,
   const void                             *data,
   USBD_IO_Inout_Data_Size_DT              size,
   USBD_IO_Inout_Data_Size_DT             *size_left,
   USBD_Bool_DT                           *result_ret);
static void USBD_IOTP_io_data_provide_in(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_IN_Data_Method_Port_HT data_method);
static Buff_Size_DT USBD_IOTP_ring_vendor_memcpy_in(const Buff_Memcpy_Params_XT *params);
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_in_linear(
   USBD_IOTP_Params_XT *tp, USBD_IO_IN_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size_left);
#if(USBD_IOTP_SUPPORT_VECTOR_BUFFERS == USBD_FEATURE_PRESENT)
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_in_vector(
   USBD_IOTP_Params_XT *tp, USBD_IO_IN_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size_left);
#endif
#if(USBD_IOTP_SUPPORT_TREE_BUFFERS == USBD_FEATURE_PRESENT)
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_in_tree(
   USBD_IOTP_Params_XT *tp, USBD_IO_IN_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size_left);
#endif
#if(USBD_IOTP_SUPPORT_RING_BUFFERS == USBD_FEATURE_PRESENT)
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_in_ring(
   USBD_IOTP_Params_XT *tp, USBD_IO_IN_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size_left);
#endif
static void USBD_IOTP_io_data_memcpy_in(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_IN_Data_Method_Port_HT data_method);
static void USBD_IOTP_io_evdata_in(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_IN_Data_Method_Port_HT mem_cpy);
static void USBD_IOTP_recv_invoked_connect_data_linear(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size);
static void USBD_IOTP_recv_invoked_connect_data_ring(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size);
static Buff_Size_DT USBD_IOTP_ring_vendor_memcpy_out(const Buff_Memcpy_Params_XT *params);
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_out_linear(
   USBD_IOTP_Params_XT *tp, USBD_IO_OUT_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);
#if(USBD_IOTP_SUPPORT_RING_BUFFERS == USBD_FEATURE_PRESENT)
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_out_ring(
   USBD_IOTP_Params_XT *tp, USBD_IO_OUT_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);
#endif
static USBD_Bool_DT USBD_IOTP_recv_invoked(
   USBD_IOTP_Params_XT *tp,
   USBD_IOTP_send_invoked_connect_data_HT  connect_data,
   void *data,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_Inout_Data_Size_DT *size_left_ret,
   USBD_Bool_DT dont_wait_for_ready,
   USBD_Bool_DT *result_ret);
static USBD_Bool_DT USBD_IOTP_recv_ready_invoked(USBD_IOTP_Params_XT *tp, USBD_Bool_DT *result);
static USBD_Bool_DT USBD_IOTP_recv(
      USBD_IOTP_Params_XT                    *tp,
      USBD_IOTP_send_invoked_connect_data_HT  connect_data,
      void                                   *data,
      USBD_IO_Inout_Data_Size_DT              size,
      USBD_IO_Inout_Data_Size_DT             *size_left_ret,
      USBD_Bool_DT                            dont_wait_for_ready);
static void USBD_IOTP_io_provide_out(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT packet_size,
   USBD_IO_Inout_Data_Size_DT left_size,
   USBD_IO_OUT_Data_Method_Port_HT provide);
static void USBD_IOTP_io_memcpy_out(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT packet_size,
   USBD_IO_Inout_Data_Size_DT left_size,
   USBD_IO_OUT_Data_Method_Port_HT mem_cpy);
static void USBD_IOTP_io_evdata_out(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_OUT_Data_Method_Port_HT mem_cpy);
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
static const USBD_IOTP_io_data_memcpy_in_HT USBD_IOTP_memcpy_in_tab[4] =
{
   [USBD_IOTP_DATA_LINEAR] = USBD_IOTP_io_data_memcpy_in_linear,
#if(USBD_IOTP_SUPPORT_VECTOR_BUFFERS == USBD_FEATURE_PRESENT)
   [USBD_IOTP_DATA_VECTOR] = USBD_IOTP_io_data_memcpy_in_vector,
#else
   [USBD_IOTP_DATA_RING]   = USBD_MAKE_INVALID_HANDLER(USBD_IOTP_io_data_memcpy_in_HT)
#endif
#if(USBD_IOTP_SUPPORT_TREE_BUFFERS == USBD_FEATURE_PRESENT)
   [USBD_IOTP_DATA_TREE]   = USBD_IOTP_io_data_memcpy_in_tree,
#else
   [USBD_IOTP_DATA_RING]   = USBD_MAKE_INVALID_HANDLER(USBD_IOTP_io_data_memcpy_in_HT)
#endif
#if(USBD_IOTP_SUPPORT_RING_BUFFERS == USBD_FEATURE_PRESENT)
   [USBD_IOTP_DATA_RING]   = USBD_IOTP_io_data_memcpy_in_ring
#else
   [USBD_IOTP_DATA_RING]   = USBD_MAKE_INVALID_HANDLER(USBD_IOTP_io_data_memcpy_in_HT)
#endif
};
static const USBD_IOTP_io_data_memcpy_out_HT USBD_IOTP_memcpy_out_tab[4] =
{
   [USBD_IOTP_DATA_LINEAR] = USBD_IOTP_io_data_memcpy_out_linear,
   [USBD_IOTP_DATA_VECTOR] = USBD_MAKE_INVALID_HANDLER(USBD_IOTP_io_data_memcpy_out_HT),
   [USBD_IOTP_DATA_TREE]   = USBD_MAKE_INVALID_HANDLER(USBD_IOTP_io_data_memcpy_out_HT),
#if(USBD_IOTP_SUPPORT_RING_BUFFERS == USBD_FEATURE_PRESENT)
   [USBD_IOTP_DATA_RING]   = USBD_IOTP_io_data_memcpy_out_ring
#else
   [USBD_IOTP_DATA_RING]   = USBD_MAKE_INVALID_HANDLER(USBD_IOTP_io_data_memcpy_out_HT)
#endif
};

static USBD_Atomic_Bool_DT USBD_IOTP_refresh_trigger;

void USBD_IOTP_Init(
      USBD_Params_XT *usbd,
      USBDC_Params_XT *usbdc,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_IOTP_Params_XT *tp)
{
#ifdef USBD_EVENT_PRESENT
   USBD_EVENT_Event_Header_XT *event;
#endif

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_ONOFF);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      memset(tp, 0, sizeof(USBD_IOTP_Params_XT));

#if (USBD_FEATURE_NOT_PRESENT != USBD_USE_INVALIDATION_ON_INIT)
      USBD_INVALIDATE_HANDLER_ON_INIT(USBD_IOTP_Callback_HT, tp->up_link.handlers.ready);
      USBD_INVALIDATE_HANDLER_ON_INIT(USBD_IOTP_Callback_HT, tp->up_link.handlers.buf_empty);
      USBD_INVALIDATE_HANDLER_ON_INIT(USBD_IOTP_Callback_HT, tp->up_link.handlers.error);
      USBD_INVALIDATE_HANDLER_ON_INIT(USBD_IOTP_Callback_HT, tp->up_link.handlers.abort);
#endif

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
      }
#endif
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_ONOFF);
} /* USBD_IOTP_Init */

USBD_Bool_DT USBD_IOTP_Install(
      USBD_IOTP_Params_XT *tp)
{
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

            USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);

            if(USB_EP_DIRECTION_IN == USBD_IOTP_GET_EP_DIR_FROM_TP(tp))
            {
               USBD_IO_UP_SET_IN_DATA_EVENT_HANDLER(
                  &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_IN_Data_Event_HT));

               tp->core.transfer_params.dir.in.last_packet_size= 0;
               tp->core.transfer_params.dir.in.req_in_progress = USBD_FALSE;
            }
            else
            {
               USBD_IO_UP_SET_OUT_DATA_EVENT_HANDLER(
                  &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_OUT_Data_Event_HT));

               tp->core.transfer_params.dir.out.mem_cpy              = USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_Port_HT);
               tp->core.transfer_params.dir.out.dont_wait_for_ready  = USBD_FALSE;
            }

            tp->core.transfer_params.data.data.linear       = USBD_MAKE_INVALID_PTR(USBD_IOTP_Data_DT);
            tp->core.transfer_params.data.offset            = 0;
            tp->core.transfer_params.data.size              = 0;

            USBD_IO_UP_Set_TP(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_GET_EP_DIR_FROM_TP(tp),
               &(tp->core.pipe_params.handlers),
               tp,
               (void*)(&USBD_IOTP_refresh_trigger));

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
         (void*)(&USBD_IOTP_refresh_trigger));

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
         (void*)(&USBD_IOTP_refresh_trigger));

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

      if(USBD_COMPARE_PTRS(void, tp_owner, void, &USBD_IOTP_refresh_trigger))
      {
         result = USBD_ATOMIC_BOOL_GET(tp->core.transfer_active);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_STATE);

   return result;
} /* USBD_IOTP_Is_Transfer_Active */



static USBD_Bool_DT USBD_IOTP_proc_abort_invoked(USBD_IOTP_Params_XT *tp, USBD_Bool_DT flush_hw_bufs)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   USBD_DEBUG_HI_3(
      USBD_DBG_IOTPEV_PROCESSING,
      "abort before::ep: %d::%s; tr_active = %s",
      USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
      USBD_IO_UP_Is_EP_Active(
         USBD_IOTP_GET_USBD_FROM_TP(tp),
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IOTP_GET_EP_DIR_FROM_TP(tp)) ? "active" : "passive",
      USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false");

   if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) || USBD_BOOL_IS_TRUE(flush_hw_bufs))
   {
      USBD_IO_Abort(
         USBD_IOTP_GET_USBD_FROM_TP(tp),
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IOTP_GET_EP_DIR_FROM_TP(tp),
         flush_hw_bufs);
   }

   USBD_DEBUG_HI_3(
      USBD_DBG_IOTPEV_PROCESSING,
      "abort after:: ep: %d::%s; tr_active = %s",
      USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
      USBD_IO_UP_Is_EP_Active(
         USBD_IOTP_GET_USBD_FROM_TP(tp),
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IOTP_GET_EP_DIR_FROM_TP(tp)) ? "active" : "passive",
      USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false");

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

      if(USBD_COMPARE_PTRS(void, tp_owner, void, &USBD_IOTP_refresh_trigger))
      {
         USBD_DEBUG_HI_3(
            USBD_DBG_IOTPEV_PROCESSING,
            "abort before::ep: %d::%s; tr_active = %s",
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_GET_EP_DIR_FROM_TP(tp)) ? "active" : "passive",
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false");

         if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) || USBD_BOOL_IS_TRUE(flush_hw_bufs))
         {
            /* try to synchronize transaction to USBD task/irq */
            if(!USBD_INVOKE(
               USBD_IOTP_GET_INVOKE_PARAMS(tp), USBD_IOTP_proc_abort_invoked(tp, flush_hw_bufs)))
            {
               USBD_DEBUG_HI_3(
                  USBD_DBG_IOTPEV_PROCESSING,
                  "abort   invoke::ep: %d::%s, dir: %s",
                  USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
                  USBD_IO_UP_Is_EP_Active(
                     USBD_IOTP_GET_USBD_FROM_TP(tp),
                     USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
                     USB_EP_DIRECTION_IN) ? "active" : "passive",
                  USB_EP_DESC_DIR_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp) ? "out" : "in");

                  USBD_ATOMIC_UINT8_SET(
                     USBD_IOTP_GET_INVOKE_REQ(tp),
                     USBD_BOOL_IS_TRUE(flush_hw_bufs) ? USBD_IOTP_INVOKE_REQ_ABORT_FLUSH_HW : USBD_IOTP_INVOKE_REQ_ABORT_NO_FLUSH_HW);

                  USBD_ATOMIC_BOOL_SET(USBD_IOTP_refresh_trigger, USBD_TRUE);
            }
         }

         USBD_DEBUG_HI_3(
            USBD_DBG_IOTPEV_PROCESSING,
            "abort after:: ep: %d::%s; tr_active = %s",
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_GET_EP_DIR_FROM_TP(tp)) ? "active" : "passive",
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false");
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_Abort */



USBD_Bool_DT USBD_IOTP_Send_Status(
   USBD_IOTP_Params_XT  *tp,
   USBD_IO_Inout_Data_Size_DT *size_left)
{
   return USBD_IOTP_Send(tp, tp, 0, size_left);
} /* USBD_IOTP_Send_Status */



USBD_Bool_DT USBD_IOTP_Send_Status_For_Out_Tp(
   USBD_IOTP_Params_XT  *tp_out,
   USBD_IO_Inout_Data_Size_DT *size_left,
   USBD_IOTP_Callback_HT buf_empty,
   USBD_Vendor_Data_XT        *vendor_data)
{
   USBD_Params_XT            *usbd;
   USBD_IOTP_Params_XT *tp_in;
   USBD_Vendor_Data_XT       *tp_vendor_data;
   USBD_Bool_DT               result = USBD_FALSE;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp_out))
   {
      usbd = USBD_IOTP_Get_USBD(tp_out);

      if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
      {
         tp_in = (USBD_IOTP_Params_XT*)USBD_IO_UP_Get_IN_TP_Params(usbd, 0);

         if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp_in))
         {
            USBD_IOTP_Set_Buf_Empty_Handler(tp_in, buf_empty);
            if(USBD_CHECK_PTR(USBD_Vendor_Data_XT, vendor_data))
            {
               tp_vendor_data = USBD_IOTP_Get_Vendor_Data_Container(tp_in);
               memcpy(tp_vendor_data, vendor_data, sizeof(tp_vendor_data[0]));
            }
            result = USBD_IOTP_Send_Status(tp_in, size_left);
         }
         else
         {
            USBD_WARN_2(USBD_DBG_IOTPEV_INVALID_PARAMS, "wrong %s (%p)", "tp_in", tp_in);
         }
      }
      else
      {
         USBD_WARN_2(USBD_DBG_IOTPEV_INVALID_PARAMS, "wrong %s (%p)", "usbd", usbd);
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IOTPEV_INVALID_PARAMS, "wrong %s (%p)", "tp_out", tp_out);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_Send_Status_For_Out_Tp */



USBD_Bool_DT USBD_IOTP_Send_Stall(
      USBD_IOTP_Params_XT *tp)
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

      if(USBD_COMPARE_PTRS(void, tp_owner, void, &USBD_IOTP_refresh_trigger))
      {
         /* check if transaction can be processed or if request must be synchronizes to USBD task/irq */
         if(USBD_BOOL_IS_FALSE(USBD_IS_INVOKE_NEEDED(USBD_IOTP_GET_INVOKE_PARAMS(tp))))
         {
            result = USBD_TRUE;

            USBD_IO_UP_Respond_Stall(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_GET_EP_DIR_FROM_TP(tp));
         }
         else
         {
            USBD_EMERG(USBD_DBG_IOTPEV_PROCESSING, "Invoke not implemented!");
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_Send_Stall */



static USBD_Bool_DT USBD_IOTP_trigger_in_invoked(USBD_Params_XT *usbd, uint8_t ep_num)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_EVENT);

   (void)USBD_IO_UP_Trigger_INOUT(usbd, ep_num, USB_EP_DIRECTION_IN, USBD_FALSE);

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_EVENT);

   return USBD_TRUE;
} /* USBD_IOTP_trigger_in_invoked */



static USBD_Bool_DT USBD_IOTP_trigger_out_invoked(USBD_Params_XT *usbd, uint8_t ep_num)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_EVENT);

   USBD_IO_UP_Trigger_INOUT(usbd, ep_num, USB_EP_DIRECTION_OUT, USBD_TRUE);

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_EVENT);

   return USBD_TRUE;
} /* USBD_IOTP_trigger_out_invoked */



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
#if(USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS == USBD_FEATURE_PRESENT)
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
         if(USBD_COMPARE_PTRS(void, USBD_IO_UP_Get_OUT_TP_Owner(usbd, ep_num), void, (&USBD_IOTP_refresh_trigger)))
         {
            tp = USBD_IO_UP_Get_OUT_TP_Params(usbd, ep_num);
            req = USBD_ATOMIC_UINT8_GET(USBD_IOTP_GET_INVOKE_REQ(tp));

            if(USBD_IOTP_INVOKE_REQ_NONE != req)
            {
               USBD_IOTP_proc_event(tp, req);
            }

#if(USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS == USBD_FEATURE_PRESENT)
            if(USBD_IOTP_DATA_RING_INFINITIVE == tp->core.data_type)
            {
               waiting_size = USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(usbd, ep_num, USBD_TRUE);

               if((waiting_size > 0)
                  && ((USBD_IO_Inout_Data_Size_DT)BUFF_RING_GET_FREE_SIZE(tp->core.transfer_params.data.data.ring) >= waiting_size))
               {
                  USBD_IOTP_trigger_out_invoked(usbd, ep_num);
               }
            }
#endif
         }
         /* IN TP */
         if(USBD_COMPARE_PTRS(void, USBD_IO_UP_Get_IN_TP_Owner(usbd, ep_num), void, (&USBD_IOTP_refresh_trigger)))
         {
            tp = USBD_IO_UP_Get_IN_TP_Params(usbd, ep_num);
            req = USBD_ATOMIC_UINT8_GET(USBD_IOTP_GET_INVOKE_REQ(tp));

            if(USBD_IOTP_INVOKE_REQ_NONE != req)
            {
               USBD_IOTP_proc_event(tp, req);
            }

#if(USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS == USBD_FEATURE_PRESENT)
            if(USBD_IOTP_DATA_RING_INFINITIVE == tp->core.data_type)
            {
               if((!BUFF_RING_IS_EMPTY(tp->core.transfer_params.data.data.ring))
                  && (USBD_IO_UP_EP_IN_Get_Buffered_Data_Size(usbd, ep_num) < 0))
               {
                  USBD_IOTP_trigger_in_invoked(usbd, ep_num);
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



static void USBD_IOTP_send_invoked_connect_data_linear(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size)
{
   tp->core.transfer_params.data.data.linear  = data;
   tp->core.transfer_params.data.offset       = 0;
   tp->core.transfer_params.data.size         = size;
   tp->core.transfer_params.dir.in.next_contineous_part.data = data;
   tp->core.transfer_params.dir.in.next_contineous_part.size = size;
   tp->core.data_type                                 = USBD_IOTP_DATA_LINEAR;
} /* USBD_IOTP_send_invoked_connect_data_linear */



#if(USBD_IOTP_SUPPORT_VECTOR_BUFFERS == USBD_FEATURE_PRESENT)
static void USBD_IOTP_send_invoked_connect_data_vector(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size)
{
   USBD_IOTP_complex_data_container_XT *vector = (USBD_IOTP_complex_data_container_XT*)data;

   tp->core.transfer_params.data.data.vector.data      = vector->data.vector;
   tp->core.transfer_params.data.data.vector.num_elems = vector->data_num_elems;
   tp->core.transfer_params.data.offset                = 0;
   tp->core.transfer_params.dir.in.next_contineous_part= Buff_Readable_Vector_Get_Contineous_Part(
      vector->data.vector, vector->data_num_elems, 0);
   tp->core.transfer_params.data.size                  = size;
   tp->core.data_type                                  = USBD_IOTP_DATA_VECTOR;
} /* USBD_IOTP_send_invoked_connect_data_vector */
#endif



#if(USBD_IOTP_SUPPORT_TREE_BUFFERS == USBD_FEATURE_PRESENT)
static void USBD_IOTP_send_invoked_connect_data_tree(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size)
{
   USBD_IOTP_complex_data_container_XT *vector = (USBD_IOTP_complex_data_container_XT*)data;

   tp->core.transfer_params.data.data.tree.data        = vector->data.tree;
   tp->core.transfer_params.data.data.tree.num_elems   = vector->data_num_elems;
   tp->core.transfer_params.data.offset                = 0;
   tp->core.transfer_params.dir.in.next_contineous_part= Buff_Readable_Tree_Get_Contineous_Part(
      vector->data.tree, vector->data_num_elems, 0);
   tp->core.transfer_params.data.size                  = size;
   tp->core.data_type                                  = USBD_IOTP_DATA_TREE;
} /* USBD_IOTP_send_invoked_connect_data_tree */
#endif



#if(USBD_IOTP_SUPPORT_RING_BUFFERS == USBD_FEATURE_PRESENT)
static void USBD_IOTP_send_invoked_connect_data_ring(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size)
{
   Buff_Ring_XT *ring = (Buff_Ring_XT*)data;

   tp->core.transfer_params.data.data.ring                  = ring;
   tp->core.transfer_params.data.offset                     = 0;
   tp->core.transfer_params.dir.in.next_contineous_part.data= USBD_MAKE_INVALID_PTR(const void);
   tp->core.transfer_params.dir.in.next_contineous_part.size= 0;
   tp->core.transfer_params.data.size                       = size;
   tp->core.data_type                                       = USBD_IOTP_DATA_RING;
} /* USBD_IOTP_send_invoked_connect_data_ring */
#endif



static USBD_Bool_DT USBD_IOTP_send_invoked(
   USBD_IOTP_Params_XT                    *tp,
   USBD_IOTP_send_invoked_connect_data_HT  connect_data,
   const void                             *data,
   USBD_IO_Inout_Data_Size_DT              size,
   USBD_IO_Inout_Data_Size_DT             *size_left,
   USBD_Bool_DT                           *result_ret)
{
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction;
   USBD_Bool_DT do_trigger = USBD_TRUE;
   USBD_Bool_DT result = USBD_FALSE;
   USBD_Bool_DT transfer_active;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   tp->core.transfer_params.dir.in.req_in_progress = USBD_TRUE;

   transaction = USBD_IO_Get_IN_Transaction_Params(
      USBD_IOTP_GET_USBD_FROM_TP(tp),
      USBD_IOTP_GET_EP_NUM_FROM_TP(tp));

   if(USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction))
   {
      result = USBD_TRUE;
      tp->up_link.data.size_transferred                  = 0;
      USBD_CALL_HANDLER(USBD_IOTP_send_invoked_connect_data_HT, connect_data)(tp, data, size);
      tp->core.transfer_params.dir.in.last_packet_size  = size % tp->core.pipe_params.data.mps;
#if(USBD_MAX_NUM_ENDPOINTS > 1)
      if(USB_EP_DESC_TRANSFER_TYPE_CONTROL != tp->core.pipe_params.data.ep_type)
      {
         if((0 == tp->core.transfer_params.dir.in.last_packet_size) && (size > 0))
         {
            tp->core.transfer_params.dir.in.last_packet_size = tp->core.pipe_params.data.mps;
         }
      }
#endif

      transfer_active = USBD_ATOMIC_BOOL_GET(tp->core.transfer_active);
      USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_TRUE);

      if(tp->core.transfer_params.dir.in.next_contineous_part.size > tp->core.pipe_params.data.mps)
      {
         USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_IOTP_io_data_provide_in);
      }
      else if(size > 0)
      {
         USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
      }
      else if(tp->core.transfer_params.dir.in.last_packet_size != tp->core.pipe_params.data.mps)
      {
         USBD_DEBUG_HI_1(
            USBD_DBG_IOTPEV_PROCESSING,
            "status on send with size 0 - %s", "immediately or at next packet - port decides");

         USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
      }
      else
      {
         USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, transfer_active);
         do_trigger = USBD_FALSE;
         USBD_DEBUG_HI_1(
            USBD_DBG_IOTPEV_PROCESSING,
            "status on send with size 0 - %s", "on status executing - ommit");
      }

      if(USBD_BOOL_IS_TRUE(do_trigger))
      {
         USBD_IO_SET_IN_MEMCPY_HANDLER(transaction, USBD_IOTP_io_data_memcpy_in);

         result = USBD_IO_UP_Trigger_INOUT(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_IN,
            USBD_FALSE);
      }

      if(USBD_BOOL_IS_FALSE(result))
      {
         USBD_DEBUG_HI(USBD_DBG_IOTPEV_PROCESSING, "trigger failed");

         USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
         USBD_IO_SET_IN_MEMCPY_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));

         USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);
      }

      if(USBD_CHECK_PTR(USBD_IO_Inout_Data_Size_DT, size_left))
      {
         *size_left = tp->core.transfer_params.data.size - tp->core.transfer_params.data.offset;

         if((0 == *size_left) && (0 != tp->core.transfer_params.dir.in.last_packet_size))
         {
            *size_left = (-1);
         }

         USBD_DEBUG_HI_1(USBD_DBG_IOTPEV_PROCESSING, "send with *size_left = %d", *size_left);
      }
   }

   tp->core.transfer_params.dir.in.req_in_progress = USBD_FALSE;
   *result_ret = result;

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   /* this returned value MUST be always true, it is needed for Invoke mechanism */
   return USBD_TRUE;
} /* USBD_IOTP_send_invoked */

USBD_Bool_DT USBD_IOTP_Send(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_Inout_Data_Size_DT *size_left)
{
   void          *tp_owner;
   USBD_Bool_DT   result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   result = USBD_FALSE;

   if((USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
      && (USBD_CHECK_PTR(void, data)))
   {
      tp_owner = USBD_IO_UP_Get_IN_TP_Owner(
         USBD_IOTP_GET_USBD_FROM_TP(tp),
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp));

      if(USBD_COMPARE_PTRS(void, tp_owner, void, &USBD_IOTP_refresh_trigger)
         && (USB_EP_DIRECTION_IN == USBD_IOTP_GET_EP_DIR_FROM_TP(tp)))
      {
         USBD_DEBUG_HI_8(
            USBD_DBG_IOTPEV_PROCESSING,
            "%s::ep: %d::%s; req size = %d; tr_active = %s; size = %d; offset = %d; last_packet_size = %d", "send before",
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_IN) ? "active" : "passive",
            size,
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            tp->core.transfer_params.dir.in.last_packet_size);

         if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_active)
            || (tp->core.transfer_params.data.size <= tp->core.transfer_params.data.offset))
         {
            /* check if transaction can be processed or if request must be synchronizes to USBD task/irq */
            if(USBD_INVOKE(
               USBD_IOTP_GET_INVOKE_PARAMS(tp), USBD_IOTP_send_invoked(
                  tp, USBD_IOTP_send_invoked_connect_data_linear, data, size, size_left, &result)))
            {
               /* OK */
            }
            else
            {
               USBD_EMERG(USBD_DBG_IOTPEV_PROCESSING, "Invoke not possible!");
            }
         }
         else
         {
            USBD_WARN(USBD_DBG_IOTPEV_PROCESSING, "transfer is active! pls stop previous transfer at frst!");
         }

         USBD_DEBUG_HI_8(
            USBD_DBG_IOTPEV_PROCESSING,
            "%s::ep: %d::%s; req size = %d; tr_active = %s; size = %d; offset = %d; last_packet_size = %d", "send after ",
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_IN) ? "active" : "passive",
            size,
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            tp->core.transfer_params.dir.in.last_packet_size);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_Send */



#if(USBD_IOTP_SUPPORT_VECTOR_BUFFERS == USBD_FEATURE_PRESENT)
USBD_Bool_DT USBD_IOTP_Send_From_Vector(
   USBD_IOTP_Params_XT          *tp,
   const Buff_Readable_Vector_XT*data,
   Buff_Num_Elems_DT             data_num_elems,
   USBD_IO_Inout_Data_Size_DT    size,
   USBD_IO_Inout_Data_Size_DT   *size_left)
{
   USBD_IOTP_complex_data_container_XT vector;
   void          *tp_owner;
   USBD_Bool_DT   result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   result = USBD_FALSE;

   if((USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
      && (USBD_CHECK_PTR(const Buff_Readable_Vector_XT, data))
      && (data_num_elems > 0))
   {
      tp_owner = USBD_IO_UP_Get_IN_TP_Owner(
         USBD_IOTP_GET_USBD_FROM_TP(tp),
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp));

      if(USBD_COMPARE_PTRS(void, tp_owner, void, &USBD_IOTP_refresh_trigger)
         && (USB_EP_DIRECTION_IN == USBD_IOTP_GET_EP_DIR_FROM_TP(tp)))
      {
         USBD_DEBUG_HI_8(
            USBD_DBG_IOTPEV_PROCESSING,
            "%s::ep: %d::%s; req size = %d; tr_active = %s; size = %d; offset = %d; last_packet_size = %d", "send before",
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_IN) ? "active" : "passive",
            size,
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            tp->core.transfer_params.dir.in.last_packet_size);

         if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_active)
            || (tp->core.transfer_params.data.size <= tp->core.transfer_params.data.offset))
         {
            vector.data.vector      = data;
            vector.data_num_elems   = data_num_elems;
            /* check if transaction can be processed or if request must be synchronizes to USBD task/irq */
            if(USBD_INVOKE(
               USBD_IOTP_GET_INVOKE_PARAMS(tp), USBD_IOTP_send_invoked(
                  tp, USBD_IOTP_send_invoked_connect_data_vector, &vector, size, size_left, &result)))
            {
               /* OK */
            }
            else
            {
               USBD_EMERG(USBD_DBG_IOTPEV_PROCESSING, "Invoke not possible!");
            }
         }
         else
         {
            USBD_WARN(USBD_DBG_IOTPEV_PROCESSING, "transfer is active! pls stop previous transfer at frst!");
         }

         USBD_DEBUG_HI_8(
            USBD_DBG_IOTPEV_PROCESSING,
            "%s::ep: %d::%s; req size = %d; tr_active = %s; size = %d; offset = %d; last_packet_size = %d", "send after ",
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_IN) ? "active" : "passive",
            size,
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            tp->core.transfer_params.dir.in.last_packet_size);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_Send_From_Vector */
#endif



#if(USBD_IOTP_SUPPORT_TREE_BUFFERS == USBD_FEATURE_PRESENT)
USBD_Bool_DT USBD_IOTP_Send_From_Tree(
   USBD_IOTP_Params_XT          *tp,
   const Buff_Readable_Tree_XT  *data,
   Buff_Num_Elems_DT             data_num_elems,
   USBD_IO_Inout_Data_Size_DT    size,
   USBD_IO_Inout_Data_Size_DT   *size_left)
{
   USBD_IOTP_complex_data_container_XT tree;
   void          *tp_owner;
   USBD_Bool_DT   result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   result = USBD_FALSE;

   if((USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
      && (USBD_CHECK_PTR(const Buff_Readable_Tree_XT, data))
      && (data_num_elems > 0))
   {
      tp_owner = USBD_IO_UP_Get_IN_TP_Owner(
         USBD_IOTP_GET_USBD_FROM_TP(tp),
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp));

      if(USBD_COMPARE_PTRS(void, tp_owner, void, &USBD_IOTP_refresh_trigger)
         && (USB_EP_DIRECTION_IN == USBD_IOTP_GET_EP_DIR_FROM_TP(tp)))
      {
         USBD_DEBUG_HI_8(
            USBD_DBG_IOTPEV_PROCESSING,
            "%s::ep: %d::%s; req size = %d; tr_active = %s; size = %d; offset = %d; last_packet_size = %d", "send before",
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_IN) ? "active" : "passive",
            size,
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            tp->core.transfer_params.dir.in.last_packet_size);

         if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_active)
            || (tp->core.transfer_params.data.size <= tp->core.transfer_params.data.offset))
         {
            tree.data.tree       = data;
            tree.data_num_elems  = data_num_elems;
            /* check if transaction can be processed or if request must be synchronizes to USBD task/irq */
            if(USBD_INVOKE(
               USBD_IOTP_GET_INVOKE_PARAMS(tp), USBD_IOTP_send_invoked(
                  tp, USBD_IOTP_send_invoked_connect_data_tree, &tree, size, size_left, &result)))
            {
               /* OK */
            }
            else
            {
               USBD_EMERG(USBD_DBG_IOTPEV_PROCESSING, "Invoke not possible!");
            }
         }
         else
         {
            USBD_WARN(USBD_DBG_IOTPEV_PROCESSING, "transfer is active! pls stop previous transfer at frst!");
         }

         USBD_DEBUG_HI_8(
            USBD_DBG_IOTPEV_PROCESSING,
            "%s::ep: %d::%s; req size = %d; tr_active = %s; size = %d; offset = %d; last_packet_size = %d", "send after ",
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_IN) ? "active" : "passive",
            size,
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            tp->core.transfer_params.dir.in.last_packet_size);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_Send_From_Tree */
#endif



#if(USBD_IOTP_SUPPORT_RING_BUFFERS == USBD_FEATURE_PRESENT)
USBD_Bool_DT USBD_IOTP_Send_From_Ring(
   USBD_IOTP_Params_XT          *tp,
   Buff_Ring_XT                 *ring,
   USBD_IO_Inout_Data_Size_DT    size,
   USBD_IO_Inout_Data_Size_DT   *size_left)
{
   void          *tp_owner;
   USBD_Bool_DT   result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   result = USBD_FALSE;

   if((USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp)) && (USBD_CHECK_PTR(Buff_Ring_XT, ring)))
   {
      tp_owner = USBD_IO_UP_Get_IN_TP_Owner(
         USBD_IOTP_GET_USBD_FROM_TP(tp),
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp));

      if(USBD_COMPARE_PTRS(void, tp_owner, void, &USBD_IOTP_refresh_trigger)
         && (USB_EP_DIRECTION_IN == USBD_IOTP_GET_EP_DIR_FROM_TP(tp)))
      {
         USBD_DEBUG_HI_8(
            USBD_DBG_IOTPEV_PROCESSING,
            "%s::ep: %d::%s; req size = %d; tr_active = %s; size = %d; offset = %d; last_packet_size = %d", "send before",
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_IN) ? "active" : "passive",
            size,
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            tp->core.transfer_params.dir.in.last_packet_size);

         if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_active)
            || (tp->core.transfer_params.data.size <= tp->core.transfer_params.data.offset))
         {
            /* check if transaction can be processed or if request must be synchronizes to USBD task/irq */
            if(USBD_INVOKE(
               USBD_IOTP_GET_INVOKE_PARAMS(tp), USBD_IOTP_send_invoked(
                  tp, USBD_IOTP_send_invoked_connect_data_ring, ring, size, size_left, &result)))
            {
               /* OK */
            }
            else
            {
               USBD_EMERG(USBD_DBG_IOTPEV_PROCESSING, "Invoke not possible!");
            }
         }
         else
         {
            USBD_WARN(USBD_DBG_IOTPEV_PROCESSING, "transfer is active! pls stop previous transfer at frst!");
         }

         USBD_DEBUG_HI_8(
            USBD_DBG_IOTPEV_PROCESSING,
            "%s::ep: %d::%s; req size = %d; tr_active = %s; size = %d; offset = %d; last_packet_size = %d", "send after ",
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_IN) ? "active" : "passive",
            size,
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            tp->core.transfer_params.dir.in.last_packet_size);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_Send_From_Ring */
#endif



static void USBD_IOTP_io_data_provide_in(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_IN_Data_Method_Port_HT data_method)
{
   USBD_IOTP_Params_XT *tp   = (USBD_IOTP_Params_XT*)tp_params;
   USBD_IO_Inout_Data_Size_DT size = 0;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction)
      && USBD_CHECK_HANDLER(USBD_IO_IN_Data_Method_Port_HT, data_method))
   {
      USBD_DEBUG_HI_7(
         USBD_DBG_IOTPEV_PROCESSING,
         "%s::ep: %d::%s; tr_active = %s; size = %d; offset = %d; last_packet_size = %d", "provide_in before",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_IN) ? "active" : "passive",
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->core.transfer_params.dir.in.last_packet_size);

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active)
         && (tp->core.transfer_params.dir.in.next_contineous_part.size > tp->core.pipe_params.data.mps))
      {
         size = USBD_CALL_HANDLER(USBD_IO_IN_Data_Method_Port_HT, data_method)(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_GET_EP_DIR_FROM_TP(tp),
            tp->core.transfer_params.dir.in.next_contineous_part.data,
            tp->core.transfer_params.dir.in.next_contineous_part.size - 1,
            USBD_TRUE);

         tp->up_link.data.size_transferred                          += size;
         tp->core.transfer_params.data.offset                       += size;
         tp->core.transfer_params.dir.in.next_contineous_part.size  -= size;
         tp->core.transfer_params.dir.in.next_contineous_part.data   =
            &((const uint8_t*)(tp->core.transfer_params.dir.in.next_contineous_part.data))[size];

         if(tp->core.transfer_params.dir.in.next_contineous_part.size <= tp->core.pipe_params.data.mps)
         {
            USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
            USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_IOTP_io_data_memcpy_in);
         }
         else
         {
            USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_IOTP_io_data_provide_in);
            USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_IOTP_io_data_memcpy_in);
         }
      }

      USBD_DEBUG_HI_7(
         USBD_DBG_IOTPEV_PROCESSING,
         "%s::ep: %d::%s; tr_active = %s; size = %d; offset = %d; last_packet_size = %d", "provide_in after ",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_IN) ? "active" : "passive",
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->core.transfer_params.dir.in.last_packet_size);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_io_data_provide_in */



static Buff_Size_DT USBD_IOTP_ring_vendor_memcpy_in(const Buff_Memcpy_Params_XT *params)
{
   usbd_iotp_vendor_memcpy_XT *vendor_data;
   USBD_IOTP_Params_XT        *tp;
   Buff_Size_DT                result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   vendor_data = (usbd_iotp_vendor_memcpy_XT*)(params->dest);
   tp          = vendor_data->tp;

   USBD_DEBUG_HI_4(
      USBD_DBG_IOTPEV_PROCESSING,
      "vendor_memcpy_in called with params: src: %p; src_offset: %d; size: %d; is_last_part: %s",
      params->src,
      params->src_offset,
      params->size,
      params->is_last_part ? "true" : "false");

   if(BUFF_BOOL_IS_TRUE(params->is_last_part))
   {
      vendor_data->is_last_part = USBD_TRUE;
   }
   else
   {
      vendor_data->is_last_part = USBD_FALSE;
   }

   result = (Buff_Size_DT)USBD_CALL_HANDLER(USBD_IO_IN_Data_Method_Port_HT, vendor_data->data_method.in)(
      USBD_IOTP_GET_USBD_FROM_TP(tp),
      USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
      USBD_IOTP_GET_EP_DIR_FROM_TP(tp),
      &(((uint8_t*)(params->src))[params->src_offset]),
      params->size,
      vendor_data->is_last_part);

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_ring_vendor_memcpy_in */



static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_in_linear(
   USBD_IOTP_Params_XT *tp, USBD_IO_IN_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size_left)
{
   USBD_IO_Inout_Data_Size_DT result = USBD_CALL_HANDLER(USBD_IO_IN_Data_Method_Port_HT, data_method)(
      USBD_IOTP_GET_USBD_FROM_TP(tp),
      USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
      USBD_IOTP_GET_EP_DIR_FROM_TP(tp),
      &tp->core.transfer_params.data.data.linear[tp->core.transfer_params.data.offset],
      size_left,
      USBD_TRUE);

   tp->core.transfer_params.dir.in.next_contineous_part.size = size_left - result;

   return result;
} /* USBD_IOTP_io_data_memcpy_in_linear */

#if(USBD_IOTP_SUPPORT_VECTOR_BUFFERS == USBD_FEATURE_PRESENT)
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_in_vector(
   USBD_IOTP_Params_XT *tp, USBD_IO_IN_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size_left)
{
   usbd_iotp_vendor_memcpy_XT vendor_data;
   Buff_Size_DT               result;

   vendor_data.tp             = tp;
   vendor_data.data_method.in = data_method;
   vendor_data.is_last_part   = USBD_TRUE;

   if(size_left > (tp->core.pipe_params.data.mps + tp->core.pipe_params.data.mps))
   {
      size_left = (USBD_IO_Inout_Data_Size_DT)(tp->core.pipe_params.data.mps + tp->core.pipe_params.data.mps);
   }

   result = Buff_Copy_From_Vector_Vendor(
      &vendor_data,
      tp->core.transfer_params.data.data.vector.data,
      tp->core.transfer_params.data.data.vector.num_elems,
      (Buff_Size_DT)size_left,
      0,
      tp->core.transfer_params.data.offset,
      USBD_IOTP_ring_vendor_memcpy_in);

   tp->core.transfer_params.dir.in.next_contineous_part = Buff_Readable_Vector_Get_Contineous_Part(
      tp->core.transfer_params.data.data.vector.data,
      tp->core.transfer_params.data.data.vector.num_elems,
      tp->core.transfer_params.data.offset + result);

   return (USBD_IO_Inout_Data_Size_DT)result;
} /* USBD_IOTP_io_data_memcpy_in_vector */
#endif

#if(USBD_IOTP_SUPPORT_TREE_BUFFERS == USBD_FEATURE_PRESENT)
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_in_tree(
   USBD_IOTP_Params_XT *tp, USBD_IO_IN_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size_left)
{
   usbd_iotp_vendor_memcpy_XT vendor_data;
   Buff_Size_DT               result;

   vendor_data.tp             = tp;
   vendor_data.data_method.in = data_method;
   vendor_data.is_last_part   = USBD_TRUE;

   if(size_left > (tp->core.pipe_params.data.mps + tp->core.pipe_params.data.mps))
   {
      size_left = (USBD_IO_Inout_Data_Size_DT)(tp->core.pipe_params.data.mps + tp->core.pipe_params.data.mps);
   }

   result = Buff_Copy_From_Tree_Vendor(
      &vendor_data,
      tp->core.transfer_params.data.data.tree.data,
      tp->core.transfer_params.data.data.tree.num_elems,
      (Buff_Size_DT)size_left,
      0,
      tp->core.transfer_params.data.offset,
      BUFF_MAKE_INVALID_PTR(Buff_Size_DT),
      USBD_IOTP_ring_vendor_memcpy_in);

   tp->core.transfer_params.dir.in.next_contineous_part = Buff_Readable_Tree_Get_Contineous_Part(
      tp->core.transfer_params.data.data.tree.data,
      tp->core.transfer_params.data.data.tree.num_elems,
      tp->core.transfer_params.data.offset + result);

   return (USBD_IO_Inout_Data_Size_DT)result;
} /* USBD_IOTP_io_data_memcpy_in_tree */
#endif

#if(USBD_IOTP_SUPPORT_RING_BUFFERS == USBD_FEATURE_PRESENT)
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_in_ring(
   USBD_IOTP_Params_XT *tp, USBD_IO_IN_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size_left)
{
   usbd_iotp_vendor_memcpy_XT vendor_data;
   Buff_Size_DT               result = 0;

   USBD_UNUSED_PARAM(size_left);

   BUFF_PROTECTION_LOCK(tp->core.transfer_params.data.data.ring);

   if(!BUFF_RING_IS_EMPTY(tp->core.transfer_params.data.data.ring))
   {
      vendor_data.tp             = tp;
      vendor_data.data_method.in = data_method;
      vendor_data.is_last_part   = USBD_TRUE;

      result = Buff_Ring_Read_Vendor(
         tp->core.transfer_params.data.data.ring,
         &vendor_data,
         (Buff_Size_DT)(tp->core.pipe_params.data.mps + tp->core.pipe_params.data.mps),
         USBD_IOTP_ring_vendor_memcpy_in,
         BUFF_FALSE);
   }

   BUFF_PROTECTION_UNLOCK(tp->core.buff);

   return (USBD_IO_Inout_Data_Size_DT)result;
} /* USBD_IOTP_io_data_memcpy_in_ring */
#endif

static void USBD_IOTP_io_data_memcpy_in(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_IN_Data_Method_Port_HT data_method)
{
   USBD_IOTP_Params_XT *tp   = (USBD_IOTP_Params_XT*)tp_params;
   USBD_IO_Inout_Data_Size_DT size = 0;
   USBD_IO_Inout_Data_Size_DT size_left;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction)
      && USBD_CHECK_HANDLER(USBD_IO_IN_Data_Method_Port_HT, data_method))
   {
      USBD_DEBUG_HI_7(
         USBD_DBG_IOTPEV_PROCESSING,
         "%s::ep: %d::%s; tr_active = %s; size = %d; offset = %d; last_packet_size = %d", "memcpy_in  before",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_IN) ? "active" : "passive",
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->core.transfer_params.dir.in.last_packet_size);

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active))
      {
         size_left = tp->core.transfer_params.data.size - tp->core.transfer_params.data.offset;

         if((size_left > 0) || (0 == tp->core.transfer_params.dir.in.last_packet_size))
         {
            size = USBD_CALL_HANDLER(USBD_IOTP_io_data_memcpy_in_HT, USBD_IOTP_memcpy_in_tab[tp->core.data_type])(
               tp, data_method, size_left);
         }

         if((0 == size_left) && (0 == tp->core.transfer_params.dir.in.last_packet_size))
         {
            tp->core.transfer_params.dir.in.last_packet_size = tp->core.pipe_params.data.mps;
         }

         tp->up_link.data.size_transferred += size;

         if(size <= size_left)
         {
            tp->core.transfer_params.data.offset += size;
            size_left                            -= size;
         }
         else
         {
            tp->core.transfer_params.data.offset  = tp->core.transfer_params.data.size;
            size_left                             = 0;
         }

         if((0 == size_left) && (0 != tp->core.transfer_params.dir.in.last_packet_size))
         {
            /** call ready handler */
            if(USBD_IOTP_CHECK_READY_HANDLER(tp)
               && USBD_BOOL_IS_FALSE(tp->core.transfer_params.dir.in.req_in_progress))
            {
               USBD_IOTP_CALL_READY(
                  USBD_IOTP_GET_USBD_FROM_TP(tp),
                  USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
                  USBD_IOTP_GET_EP_DIR_FROM_TP(tp),
                  tp,
                  tp->core.pipe_params.data.mps);
            }
         }

         if((tp->core.transfer_params.data.size == tp->core.transfer_params.data.offset)
            && (0 != tp->core.transfer_params.dir.in.last_packet_size))
         {
            USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
            USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));

            USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);
         }
         else if(tp->core.transfer_params.dir.in.next_contineous_part.size <= tp->core.pipe_params.data.mps)
         {
            USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
            USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_IOTP_io_data_memcpy_in);
         }
         else
         {
            USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_IOTP_io_data_provide_in);
            USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_IOTP_io_data_memcpy_in);
         }
      }

      USBD_DEBUG_HI_7(
         USBD_DBG_IOTPEV_PROCESSING,
         "%s::ep: %d::%s; tr_active = %s; size = %d; offset = %d; last_packet_size = %d", "memcpy_in  after ",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_IN) ? "active" : "passive",
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->core.transfer_params.dir.in.last_packet_size);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_io_data_memcpy_in */

static void USBD_IOTP_io_evdata_in(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_IN_Data_Method_Port_HT mem_cpy)
{
   USBD_IOTP_Params_XT *tp = (USBD_IOTP_Params_XT*)tp_params;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction))
   {
      USBD_DEBUG_HI_8(
         USBD_DBG_IOTPEV_PROCESSING,
         "%s::ep: %d::%s; req size = %d; tr_active = %s; size = %d; offset = %d; last_packet_size = %d", "evdata_in before",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_IN) ? "active" : "passive",
         size,
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->core.transfer_params.dir.in.last_packet_size);

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active))
      {
         USBD_IOTP_io_data_memcpy_in(tp_params, transaction, mem_cpy);
      }

      if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_active))
      {
         if(size < 0)
         {
            /** informs upper layer that IN buffer is empty */
            if(USBD_IOTP_CHECK_BUF_EMPTY_HANDLER(tp)
               && USBD_BOOL_IS_FALSE(tp->core.transfer_params.dir.in.req_in_progress))
            {
               USBD_IOTP_CALL_BUF_EMPTY(
                  USBD_IOTP_GET_USBD_FROM_TP(tp),
                  USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
                  USBD_IOTP_GET_EP_DIR_FROM_TP(tp),
                  tp,
                  tp->core.pipe_params.data.mps);
            }
         }
         else
         {
            /** informs upper layer that IN data can be now provided to port */
            if(USBD_IOTP_CHECK_READY_HANDLER(tp)
               && USBD_BOOL_IS_FALSE(tp->core.transfer_params.dir.in.req_in_progress))
            {
               USBD_IOTP_CALL_READY(
                  USBD_IOTP_GET_USBD_FROM_TP(tp),
                  USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
                  USBD_IOTP_GET_EP_DIR_FROM_TP(tp),
                  tp,
                  tp->core.pipe_params.data.mps);
            }
         }
      }

      USBD_DEBUG_HI_8(
         USBD_DBG_IOTPEV_PROCESSING,
         "%s::ep: %d::%s; req size = %d; tr_active = %s; size = %d; offset = %d; last_packet_size = %d", "evdata_in after ",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_IN) ? "active" : "passive",
         size,
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->core.transfer_params.dir.in.last_packet_size);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_io_evdata_in */



static void USBD_IOTP_recv_invoked_connect_data_linear(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size)
{
   tp->core.transfer_params.data.data.linear             = (void*)data;
   tp->core.transfer_params.data.offset                  = 0;
   tp->core.transfer_params.data.size                    = size;
   tp->core.transfer_params.dir.out.next_contineous_part = size;
   tp->core.data_type                                    = USBD_IOTP_DATA_LINEAR;
} /* USBD_IOTP_recv_invoked_connect_data_linear */



static void USBD_IOTP_recv_invoked_connect_data_ring(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size)
{
   Buff_Ring_XT *ring = (Buff_Ring_XT*)data;
   tp->core.transfer_params.data.data.ring               = ring;
   tp->core.transfer_params.data.offset                  = 0;
   tp->core.transfer_params.data.size                    = size;
   tp->core.transfer_params.dir.out.next_contineous_part = 0;
   tp->core.data_type                                    = USBD_IOTP_DATA_RING;
} /* USBD_IOTP_recv_invoked_connect_data_ring */

static Buff_Size_DT USBD_IOTP_ring_vendor_memcpy_out(const Buff_Memcpy_Params_XT *params)
{
   usbd_iotp_vendor_memcpy_XT *vendor_data;
   Buff_Size_DT result = 0;
   USBD_Bool_DT is_last_part;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   vendor_data = (usbd_iotp_vendor_memcpy_XT*)(params->src);

   USBD_DEBUG_HI_4(
      USBD_DBG_IOTPEV_PROCESSING,
      "vendor_memcpy_out called with params: dest: %p; dest_offset: %d; size: %d; is_last_part: %s",
      params->dest,
      params->dest_offset,
      params->size,
      params->is_last_part ? "true" : "false");

   if(BUFF_BOOL_IS_TRUE(params->is_last_part))
   {
      if(params->size >= (Buff_Size_DT)(vendor_data->out_left_data_size))
      {
         is_last_part = USBD_TRUE;
      }
      else
      {
         is_last_part = USBD_FALSE;
      }
   }
   else
   {
      is_last_part = USBD_FALSE;
   }

   if(vendor_data->out_left_data_size >= 0)
   {
      result = (Buff_Size_DT)USBD_CALL_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, vendor_data->data_method.out)(
         USBD_IOTP_GET_USBD_FROM_TP(vendor_data->tp),
         USBD_IOTP_GET_EP_NUM_FROM_TP(vendor_data->tp),
         USBD_IOTP_GET_EP_DIR_FROM_TP(vendor_data->tp),
         &(((uint8_t*)(params->dest))[params->dest_offset]),
         params->size,
         is_last_part);

      vendor_data->out_left_data_size -= result;

      if(USBD_BOOL_IS_TRUE(is_last_part))
      {
         vendor_data->out_left_data_size = (-1);
      }
      else if(vendor_data->out_left_data_size <= 0)
      {
         vendor_data->out_left_data_size = 0;
      }
   }
   else
   {
      USBD_ERROR(USBD_DBG_IOTPEV_ONOFF, "vendor_memcpy_out called when no data is waiting - ignore it!");
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_ring_vendor_memcpy_out */



static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_out_linear(
   USBD_IOTP_Params_XT *tp, USBD_IO_OUT_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part)
{
   USBD_IO_Inout_Data_Size_DT result = USBD_CALL_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, data_method)(
      USBD_IOTP_GET_USBD_FROM_TP(tp),
      USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
      USBD_IOTP_GET_EP_DIR_FROM_TP(tp),
      (uint8_t*)(&tp->core.transfer_params.data.data.linear[tp->core.transfer_params.data.offset]),
      size,
      is_last_part);

   tp->core.transfer_params.dir.out.next_contineous_part -= result;

   return result;
} /* USBD_IOTP_io_data_memcpy_out_linear */



#if(USBD_IOTP_SUPPORT_RING_BUFFERS == USBD_FEATURE_PRESENT)
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_out_ring(
   USBD_IOTP_Params_XT *tp, USBD_IO_OUT_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part)
{
   usbd_iotp_vendor_memcpy_XT vendor_data;
   Buff_Size_DT               result = 0;

   vendor_data.tp                   = tp;
   vendor_data.data_method.out      = data_method;
   vendor_data.out_left_data_size   = size;

   BUFF_PROTECTION_LOCK(tp->core.transfer_params.data.data.ring);

   if(size <= (USBD_IO_Inout_Data_Size_DT)BUFF_RING_GET_FREE_SIZE(tp->core.transfer_params.data.data.ring))
   {
      result = Buff_Ring_Write_Vendor(
         tp->core.transfer_params.data.data.ring,
         &vendor_data,
         size,
         USBD_IOTP_ring_vendor_memcpy_out,
         BUFF_FALSE,
         BUFF_FALSE);
   }

   BUFF_PROTECTION_UNLOCK(tp->core.transfer_params.data.data.ring);

   return (USBD_IO_Inout_Data_Size_DT)result;
} /* USBD_IOTP_io_data_memcpy_out_ring */
#endif



static USBD_Bool_DT USBD_IOTP_recv_invoked(
   USBD_IOTP_Params_XT *tp,
   USBD_IOTP_send_invoked_connect_data_HT  connect_data,
   void *data,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_Inout_Data_Size_DT *size_left_ret,
   USBD_Bool_DT dont_wait_for_ready,
   USBD_Bool_DT *result_ret)
{
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction;
   USBD_IO_Inout_Data_Size_DT             size_left;
   USBD_Bool_DT                           is_last_part = USBD_TRUE;
   USBD_Bool_DT                           result       = USBD_FALSE;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   transaction = USBD_IO_Get_OUT_Transaction_Params(
      USBD_IOTP_GET_USBD_FROM_TP(tp),
      USBD_IOTP_GET_EP_NUM_FROM_TP(tp));

   if(USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction))
   {
      tp->up_link.data.size_transferred                     = 0;
      USBD_CALL_HANDLER(USBD_IOTP_send_invoked_connect_data_HT, connect_data)(tp, data, size);
      tp->core.transfer_params.dir.out.dont_wait_for_ready  = dont_wait_for_ready;
      result                                                = USBD_TRUE;

      if(tp->core.transfer_params.dir.out.next_contineous_part > tp->core.pipe_params.data.mps)
      {
         USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_TRUE);
         USBD_IO_SET_OUT_PROVIDE_HANDLER(
            transaction, USBD_IOTP_io_provide_out);
         USBD_IO_SET_OUT_MEMCPY_HANDLER(
            transaction, USBD_IOTP_io_memcpy_out);
      }
      else
      {
         USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_TRUE);
         USBD_IO_SET_OUT_PROVIDE_HANDLER(
            transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
         USBD_IO_SET_OUT_MEMCPY_HANDLER(
            transaction, USBD_IOTP_io_memcpy_out);
      }

      if(!USBD_CHECK_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, tp->core.transfer_params.dir.out.mem_cpy))
      {
         result = USBD_IO_UP_Trigger_INOUT(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT,
            dont_wait_for_ready);
      }
      else
      {
         size = USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_TRUE);

         if(size >= 0)
         {
            size_left = tp->core.transfer_params.data.size - tp->core.transfer_params.data.offset;
            if(size > size_left)
            {
               size = size_left;
               is_last_part = USBD_FALSE;
            }
            else if((size == size_left) && USBD_BOOL_IS_FALSE(dont_wait_for_ready))
            {
               is_last_part = USBD_FALSE;
            }

            size = USBD_CALL_HANDLER(USBD_IOTP_io_data_memcpy_out_HT, USBD_IOTP_memcpy_out_tab[tp->core.data_type])(
               tp, tp->core.transfer_params.dir.out.mem_cpy, size, is_last_part);

            tp->up_link.data.size_transferred      = size;
            tp->core.transfer_params.data.offset  += size;

            if(tp->core.transfer_params.data.size == tp->core.transfer_params.data.offset)
            {
               USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);

               USBD_IO_SET_OUT_PROVIDE_HANDLER(
                  transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
               USBD_IO_SET_OUT_MEMCPY_HANDLER(
                  transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
            }

            if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active))
            {
               if(tp->core.transfer_params.dir.out.next_contineous_part > tp->core.pipe_params.data.mps)
               {
                  USBD_IO_SET_OUT_PROVIDE_HANDLER(
                     transaction, USBD_IOTP_io_provide_out);
                  USBD_IO_SET_OUT_MEMCPY_HANDLER(
                     transaction, USBD_IOTP_io_memcpy_out);
               }
               else
               {
                  USBD_IO_SET_OUT_PROVIDE_HANDLER(
                     transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
                  USBD_IO_SET_OUT_MEMCPY_HANDLER(
                     transaction, USBD_IOTP_io_memcpy_out);
               }
            }
         }
         else
         {
            result = USBD_IO_UP_Trigger_INOUT(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_OUT,
               dont_wait_for_ready);
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

      if(USBD_CHECK_PTR(USBD_IO_Inout_Data_Size_DT, size_left_ret))
      {
         if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active))
         {
            *size_left_ret = size;
         }
         else
         {
            *size_left_ret = (-1);
         }
      }
   }

   *result_ret = result;

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   /* this returned value MUST be always true, it is needed for Invoke mechanism */
   return USBD_TRUE;
} /* USBD_IOTP_recv_invoked */



static USBD_Bool_DT USBD_IOTP_recv_ready_invoked(USBD_IOTP_Params_XT *tp, USBD_Bool_DT *result)
{
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction;
#if(USBD_FEATURE_PRESENT == USBD_IO_PROVIDE_METHODS_SUPPORTED)
   USBD_IO_OUT_Data_Method_TP_HT          provide;
#endif
   USBD_IO_OUT_Data_Method_TP_HT          mem_cpy;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   transaction = USBD_IO_Get_OUT_Transaction_Params(
      USBD_IOTP_GET_USBD_FROM_TP(tp),
      USBD_IOTP_GET_EP_NUM_FROM_TP(tp));

#if(USBD_FEATURE_PRESENT == USBD_IO_PROVIDE_METHODS_SUPPORTED)
   provide = USBD_IO_GET_OUT_PROVIDE_HANDLER(transaction);
#endif
   mem_cpy = USBD_IO_GET_OUT_MEMCPY_HANDLER(transaction);

   USBD_IO_SET_OUT_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
   USBD_IO_SET_OUT_MEMCPY_HANDLER(transaction,  USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));

   *result = USBD_IO_UP_Trigger_INOUT(
      USBD_IOTP_GET_USBD_FROM_TP(tp),
      USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
      USB_EP_DIRECTION_OUT,
      USBD_FALSE);

   USBD_IO_SET_OUT_PROVIDE_HANDLER(transaction, provide);
   USBD_IO_SET_OUT_MEMCPY_HANDLER(transaction,  mem_cpy);

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   /* this returned value MUST be always true, it is needed for Invoke mechanism */
   return USBD_TRUE;
} /* USBD_IOTP_recv_ready_invoked */



static USBD_Bool_DT USBD_IOTP_recv(
      USBD_IOTP_Params_XT                    *tp,
      USBD_IOTP_send_invoked_connect_data_HT  connect_data,
      void                                   *data,
      USBD_IO_Inout_Data_Size_DT              size,
      USBD_IO_Inout_Data_Size_DT             *size_left_ret,
      USBD_Bool_DT                            dont_wait_for_ready)
{
   void          *tp_owner;
#ifdef USBD_USE_IOCMD
   const char    *recv_option_desc = USBD_BOOL_IS_TRUE(dont_wait_for_ready) ? "ready" : "wait";
#endif
   USBD_Bool_DT   result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   result = USBD_FALSE;

   if((USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
      && (USBD_CHECK_PTR(void, data)))
   {
      tp_owner = USBD_IO_UP_Get_OUT_TP_Owner(
         USBD_IOTP_GET_USBD_FROM_TP(tp),
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp));

      if(USBD_COMPARE_PTRS(void, tp_owner, void, &USBD_IOTP_refresh_trigger)
         && (USB_EP_DIRECTION_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp)))
      {
         USBD_DEBUG_HI_9(
            USBD_DBG_IOTPEV_PROCESSING,
            "recv_and_%s before::"
            "ep: %d::%s; req size = %d; tr_active = %s; size = %d; offset = %d; dont_wait_for_ready = %s; data = 0x%08X",
            recv_option_desc,
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_OUT) ? "active" : "passive",
            size,
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            USBD_BOOL_IS_TRUE(tp->core.transfer_params.dir.out.dont_wait_for_ready) ? "true" : "false",
            tp->core.transfer_params.data.data.linear);

         if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_active))
         {
            /* check if transaction can be processed or if request must be synchronizes to USBD task/irq */
            if(USBD_INVOKE(
               USBD_IOTP_GET_INVOKE_PARAMS(tp), USBD_IOTP_recv_invoked(
                  tp, connect_data, data, size, size_left_ret, dont_wait_for_ready, &result)))
            {
               /* OK */
            }
            else
            {
               USBD_EMERG(USBD_DBG_IOTPEV_PROCESSING, "Invoke not possible!");
            }
         }

         USBD_DEBUG_HI_10(
            USBD_DBG_IOTPEV_PROCESSING,
            "recv_and_%s after ::"
            "ep: %d::%s; req size = %d; tr_active = %s; size = %d; offset = %d; dont_wait_for_ready = %s; data = 0x%08X, result: %s",
            recv_option_desc,
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_OUT) ? "active" : "passive",
            size,
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            USBD_BOOL_IS_TRUE(tp->core.transfer_params.dir.out.dont_wait_for_ready) ? "true" : "false",
            tp->core.transfer_params.data.data.linear,
            USBD_BOOL_IS_TRUE(result) ? "true" : "false");
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_recv */

USBD_Bool_DT USBD_IOTP_Recv_And_Ready(
      USBD_IOTP_Params_XT  *tp,
      void                       *data,
      USBD_IO_Inout_Data_Size_DT  size,
      USBD_IO_Inout_Data_Size_DT *size_left)
{
   return USBD_IOTP_recv(tp, USBD_IOTP_recv_invoked_connect_data_linear, data, size, size_left, USBD_TRUE);
}/* USBD_IOTP_Recv_And_Ready */



USBD_Bool_DT USBD_IOTP_Recv_And_Wait(
      USBD_IOTP_Params_XT *tp,
      void *data,
      USBD_IO_Inout_Data_Size_DT size,
      USBD_IO_Inout_Data_Size_DT *size_left)
{
   return USBD_IOTP_recv(tp, USBD_IOTP_recv_invoked_connect_data_linear, data, size, size_left, USBD_FALSE);
}/* USBD_IOTP_Recv_And_Wait */



#if(USBD_IOTP_SUPPORT_RING_BUFFERS == USBD_FEATURE_PRESENT)
USBD_Bool_DT USBD_IOTP_Recv_And_Ready_To_Ring(
      USBD_IOTP_Params_XT        *tp,
      Buff_Ring_XT               *ring,
      USBD_IO_Inout_Data_Size_DT  size,
      USBD_IO_Inout_Data_Size_DT *size_left)
{
   return USBD_IOTP_recv(tp, USBD_IOTP_recv_invoked_connect_data_ring, ring, size, size_left, USBD_TRUE);
}/* USBD_IOTP_Recv_And_Ready_To_Ring */



USBD_Bool_DT USBD_IOTP_Recv_And_Wait_To_Ring(
      USBD_IOTP_Params_XT        *tp,
      Buff_Ring_XT               *ring,
      USBD_IO_Inout_Data_Size_DT  size,
      USBD_IO_Inout_Data_Size_DT *size_left)
{
   return USBD_IOTP_recv(tp, USBD_IOTP_recv_invoked_connect_data_ring, ring, size, size_left, USBD_FALSE);
}/* USBD_IOTP_Recv_And_Wait_To_Ring */
#endif



USBD_Bool_DT USBD_IOTP_Recv_Ready(
      USBD_IOTP_Params_XT *tp)
{
   void        *tp_owner;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      tp_owner = USBD_IO_UP_Get_OUT_TP_Owner(
         USBD_IOTP_GET_USBD_FROM_TP(tp),
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp));

      if(USBD_COMPARE_PTRS(void, tp_owner, void, &USBD_IOTP_refresh_trigger)
         && (USB_EP_DIRECTION_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp)))
      {
         USBD_DEBUG_HI_7(
            USBD_DBG_IOTPEV_PROCESSING,
            "recv_ready    before::ep: %d::%s; tr_active = %s; size = %d; offset = %d; dont_wait_for_ready = %s; data = 0x%08X",
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_OUT) ? "active" : "passive",
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            USBD_BOOL_IS_TRUE(tp->core.transfer_params.dir.out.dont_wait_for_ready) ? "true" : "false",
            tp->core.transfer_params.data.data.linear);

         /* check if transaction can be processed or if request must be synchronizes to USBD task/irq */
         if(USBD_INVOKE(
            USBD_IOTP_GET_INVOKE_PARAMS(tp), USBD_IOTP_recv_ready_invoked(tp, &result)))
         {
            /* OK */
         }
         else
         {
            USBD_EMERG(USBD_DBG_IOTPEV_PROCESSING, "Invoke not implemented!");
         }

         USBD_DEBUG_HI_7(
            USBD_DBG_IOTPEV_PROCESSING,
            "recv_ready    after ::ep: %d::%s; tr_active = %s; size = %d; offset = %d; dont_wait_for_ready = %s; data = 0x%08X",
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_OUT) ? "active" : "passive",
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            USBD_BOOL_IS_TRUE(tp->core.transfer_params.dir.out.dont_wait_for_ready) ? "true" : "false",
            tp->core.transfer_params.data.data.linear);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_Recv_Ready */



static void USBD_IOTP_io_provide_out(
   void* tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT packet_size,
   USBD_IO_Inout_Data_Size_DT left_size,
   USBD_IO_OUT_Data_Method_Port_HT provide)
{
   USBD_IOTP_Params_XT       *tp       = (USBD_IOTP_Params_XT*)tp_params;
   USBD_IO_Inout_Data_Size_DT part_size= 0;

#ifndef USBD_USE_IOCMD
   USBD_UNUSED_PARAM(packet_size);
   USBD_UNUSED_PARAM(left_size);
#endif

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction)
      && USBD_CHECK_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, provide))
   {
      USBD_DEBUG_HI_10(
         USBD_DBG_IOTPEV_PROCESSING,
         "provide_out   before::"
         "ep: %d::%s; packet_size = %d; left_size = %d;"
         " tr_active = %s; size = %d; offset = %d; size_transferred: %d; dont_wait_for_ready = %s; data = 0x%08X",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         packet_size,
         left_size,
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->up_link.data.size_transferred,
         USBD_BOOL_IS_TRUE(tp->core.transfer_params.dir.out.dont_wait_for_ready) ? "true" : "false",
         tp->core.transfer_params.data.data.linear);

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active)
         && (tp->core.transfer_params.dir.out.next_contineous_part > tp->core.pipe_params.data.mps))
      {
         part_size = USBD_CALL_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, provide)(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_GET_EP_DIR_FROM_TP(tp),
            (uint8_t*)(&tp->core.transfer_params.data.data.linear[tp->core.transfer_params.data.offset]),
            tp->core.transfer_params.dir.out.next_contineous_part - 1,
            USBD_TRUE);

         tp->core.transfer_params.data.offset                 += part_size;
         tp->core.transfer_params.dir.out.next_contineous_part-= part_size;
         tp->core.transfer_params.dir.out.size_in_progress     = part_size;

         if(tp->core.transfer_params.dir.out.next_contineous_part < tp->core.pipe_params.data.mps)
         {
            USBD_IO_SET_OUT_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
            USBD_IO_SET_OUT_MEMCPY_HANDLER(transaction, USBD_IOTP_io_memcpy_out);
         }
         else
         {
            USBD_IO_SET_OUT_PROVIDE_HANDLER(transaction, USBD_IOTP_io_provide_out);
            USBD_IO_SET_OUT_MEMCPY_HANDLER(transaction, USBD_IOTP_io_memcpy_out);
         }
      }

      USBD_DEBUG_HI_10(
         USBD_DBG_IOTPEV_PROCESSING,
         "provide_out   after ::"
         "ep: %d::%s; packet_size = %d; left_size = %d;"
         " tr_active = %s; size = %d; offset = %d; size_transferred: %d; dont_wait_for_ready = %s; data = 0x%08X",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         packet_size,
         left_size,
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->up_link.data.size_transferred,
         USBD_BOOL_IS_TRUE(tp->core.transfer_params.dir.out.dont_wait_for_ready) ? "true" : "false",
         tp->core.transfer_params.data.data.linear);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_io_provide_out */



static void USBD_IOTP_io_memcpy_out(
   void* tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT packet_size,
   USBD_IO_Inout_Data_Size_DT left_size,
   USBD_IO_OUT_Data_Method_Port_HT data_method)
{
   USBD_IOTP_Params_XT       *tp          = (USBD_IOTP_Params_XT*)tp_params;
   USBD_IO_Inout_Data_Size_DT part_size;
   USBD_Bool_DT               is_last_part= USBD_FALSE;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction)
      && USBD_CHECK_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, data_method))
   {
      USBD_DEBUG_HI_10(
         USBD_DBG_IOTPEV_PROCESSING,
         "memcpy_out    before::"
         "ep: %d::%s; packet_size = %d; left_size = %d;"
         " tr_active = %s; size = %d; offset = %d; size_transferred: %d; dont_wait_for_ready = %s; data = 0x%08X",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         packet_size,
         left_size,
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->up_link.data.size_transferred,
         USBD_BOOL_IS_TRUE(tp->core.transfer_params.dir.out.dont_wait_for_ready) ? "true" : "false",
         tp->core.transfer_params.data.data.linear);

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active))
      {
         part_size = tp->core.transfer_params.data.size - tp->core.transfer_params.data.offset;

         if(part_size > left_size)
         {
            part_size         = left_size;

            if(packet_size < ((USBD_IO_Inout_Data_Size_DT)(tp->core.pipe_params.data.mps)))
            {
               is_last_part   = tp->core.transfer_params.dir.out.dont_wait_for_ready;
            }
            else
            {
               is_last_part   = USBD_TRUE;
            }
         }
         else if(part_size == left_size)
         {
            is_last_part      = tp->core.transfer_params.dir.out.dont_wait_for_ready;
         }

         part_size = USBD_CALL_HANDLER(USBD_IOTP_io_data_memcpy_out_HT, USBD_IOTP_memcpy_out_tab[tp->core.data_type])(
            tp, data_method, part_size, is_last_part);

         tp->up_link.data.size_transferred += part_size;

         if(part_size <= (tp->core.transfer_params.data.size - tp->core.transfer_params.data.offset))
         {
            tp->core.transfer_params.data.offset  += part_size;
         }
         else
         {
            tp->core.transfer_params.data.offset   = tp->core.transfer_params.data.size;
         }
         tp->core.transfer_params.dir.out.size_in_progress = 0;

         /* no more space or packet smaller than MPS */
         if((tp->core.transfer_params.data.size == tp->core.transfer_params.data.offset)
            || (packet_size < ((USBD_IO_Inout_Data_Size_DT)(tp->core.pipe_params.data.mps))))
         {
            USBD_IO_SET_OUT_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
            USBD_IO_SET_OUT_MEMCPY_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));

            USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);

            if(USBD_IOTP_CHECK_READY_HANDLER(tp))
            {
               left_size = USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(
                  USBD_IOTP_GET_USBD_FROM_TP(tp),
                  USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
                  USBD_TRUE);

               if(left_size >= 0)
               {
                  tp->core.transfer_params.dir.out.mem_cpy = data_method;
               }

               USBD_IOTP_CALL_READY(
                  USBD_IOTP_GET_USBD_FROM_TP(tp),
                  USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
                  USBD_IOTP_GET_EP_DIR_FROM_TP(tp),
                  tp,
                  left_size);

               tp->core.transfer_params.dir.out.mem_cpy = USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_Port_HT);
            }
         }
         else
         {
            if(tp->core.transfer_params.dir.out.next_contineous_part > tp->core.pipe_params.data.mps)
            {
               USBD_IO_SET_OUT_PROVIDE_HANDLER(
                  transaction, USBD_IOTP_io_provide_out);
               USBD_IO_SET_OUT_MEMCPY_HANDLER(
                  transaction, USBD_IOTP_io_memcpy_out);
            }
            else
            {
               USBD_IO_SET_OUT_PROVIDE_HANDLER(
                  transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
               USBD_IO_SET_OUT_MEMCPY_HANDLER(
                  transaction, USBD_IOTP_io_memcpy_out);
            }
         }
      }

      USBD_DEBUG_HI_10(
         USBD_DBG_IOTPEV_PROCESSING,
         "memcpy_out    after ::"
         "ep: %d::%s; packet_size = %d; left_size = %d;"
         " tr_active = %s; size = %d; offset = %d; size_transferred: %d; dont_wait_for_ready = %s; data = 0x%08X",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         packet_size,
         left_size,
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->up_link.data.size_transferred,
         USBD_BOOL_IS_TRUE(tp->core.transfer_params.dir.out.dont_wait_for_ready) ? "true" : "false",
         tp->core.transfer_params.data.data.linear);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_io_memcpy_out */



static void USBD_IOTP_io_evdata_out(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_OUT_Data_Method_Port_HT mem_cpy)
{
   USBD_IOTP_Params_XT *tp = (USBD_IOTP_Params_XT*)tp_params;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction))
   {
      USBD_DEBUG_HI_9(
         USBD_DBG_IOTPEV_PROCESSING,
         "evdata_out    before::"
         "ep: %d::%s; size = %d; tr_active = %s; size = %d; offset = %d; size_transferred: %d; dont_wait_for_ready = %s; data = 0x%08X",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         size,
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->up_link.data.size_transferred,
         USBD_BOOL_IS_TRUE(tp->core.transfer_params.dir.out.dont_wait_for_ready) ? "true" : "false",
         tp->core.transfer_params.data.data.linear);

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active))
      {
         if((tp->core.transfer_params.dir.out.size_in_progress != USBD_IO_GET_OUT_TRANSFERRED_SIZE(transaction))
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
         if(USBD_IOTP_CHECK_READY_HANDLER(tp))
         {
            tp->core.transfer_params.dir.out.mem_cpy = mem_cpy;

            USBD_IOTP_CALL_READY(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_GET_EP_DIR_FROM_TP(tp),
               tp,
               size);

            tp->core.transfer_params.dir.out.mem_cpy = USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_Port_HT);
         }
      }

      USBD_DEBUG_HI_9(
         USBD_DBG_IOTPEV_PROCESSING,
         "evdata_out    after ::"
         "ep: %d::%s; size = %d; tr_active = %s; size = %d; offset = %d; size_transferred: %d; dont_wait_for_ready = %s; data = 0x%08X",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         size,
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->up_link.data.size_transferred,
         USBD_BOOL_IS_TRUE(tp->core.transfer_params.dir.out.dont_wait_for_ready) ? "true" : "false",
         tp->core.transfer_params.data.data.linear);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_io_evdata_out */



#if(USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS == USBD_FEATURE_PRESENT)
static USBD_Bool_DT USBD_IOTP_ring_start_transfer_infinitely_invoked(
   USBD_IOTP_Params_XT *tp, Buff_Ring_XT *ring, USBD_Bool_DT *result)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   tp->core.transfer_params.data.data.ring = ring;
   tp->core.data_type = USBD_IOTP_DATA_RING_INFINITIVE;

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return USBD_TRUE;
}



static USBD_Bool_DT USBD_IOTP_ring_stop_transfer_infinitely_invoked(
   USBD_IOTP_Params_XT *tp, Buff_Ring_XT *ring, USBD_Bool_DT *result)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   tp->core.data_type = USBD_IOTP_DATA_LINEAR;

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return USBD_TRUE;
} /* USBD_IOTP_ring_stop_transfer_infinitely_invoked */



static USBD_Bool_DT USBD_IOTP_Ring_start_stop_transfer_infinitely(
   USBD_IOTP_Params_XT          *tp,
   Buff_Ring_XT                 *ring,
   USBD_IOTP_ring_start_stop_transfer_infinitely_invoked_HT action,
   const char                   *desc)
{
   void          *tp_owner;
   USBD_Bool_DT   result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp) && USBD_CHECK_PTR(Buff_Ring_XT, ring))
   {
      tp_owner = USBD_IO_UP_Get_IN_TP_Owner(
         USBD_IOTP_GET_USBD_FROM_TP(tp),
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp));

      if(USBD_COMPARE_PTRS(void, tp_owner, void, &USBD_IOTP_refresh_trigger)
         && (USB_EP_DIRECTION_IN == USBD_IOTP_GET_EP_DIR_FROM_TP(tp)))
      {
         USBD_DEBUG_HI_7(
            USBD_DBG_IOTPEV_PROCESSING,
            "%s%s::ep: %d::%s; tr_active = %s; size = %d; offset = %d", desc, "infinitely before",
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_IN) ? "active" : "passive",
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset);

         if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_active)
            || (tp->core.transfer_params.data.size <= tp->core.transfer_params.data.offset))
         {
            /* check if transaction can be processed or if request must be synchronizes to USBD task/irq */
            if(USBD_INVOKE(USBD_IOTP_GET_INVOKE_PARAMS(tp), action(tp, ring, &result)))
            {
               /* OK */
            }
            else
            {
               USBD_EMERG(USBD_DBG_IOTPEV_PROCESSING, "Invoke not possible!");
            }
         }
         else
         {
            USBD_WARN(USBD_DBG_IOTPEV_PROCESSING, "transfer is active! pls stop previous transfer at frst!");
         }

         USBD_DEBUG_HI_7(
            USBD_DBG_IOTPEV_PROCESSING,
            "%s%s::ep: %d::%s; tr_active = %s; size = %d; offset = %d", desc, "infinitely after ",
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_IN) ? "active" : "passive",
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_Ring_start_stop_transfer_infinitely */



static void USBD_IOTP_ring_extension_on_read_remove(Buff_Ring_XT *buf, Buff_Ring_Extensions_XT *extension)
{
   USBD_IOTP_Params_XT *tp;
   USBD_Params_XT *usbd;
   USBD_IO_Inout_Data_Size_DT waiting_size;
   uint8_t ep_num;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   tp = (USBD_IOTP_Params_XT *)(extension->on_read_params);

   if(!BUFF_RING_IS_FULL(buf))
   {
      if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp)
         && BUFF_COMPARE_PTRS(Buff_Ring_XT, buf, Buff_Ring_XT, tp->core.transfer_params.data.data.ring) )
      {
         usbd   = USBD_IOTP_GET_USBD_FROM_TP(tp);
         ep_num = USBD_IOTP_GET_EP_NUM_FROM_TP(tp);

         if(USBD_COMPARE_PTRS(USBD_IOTP_Params_XT, USBD_IO_UP_Get_OUT_TP_Params(usbd, ep_num), USBD_IOTP_Params_XT, tp))
         {
            waiting_size = USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(usbd, ep_num, USBD_TRUE);

            if((waiting_size > 0)
               && ((USBD_IO_Inout_Data_Size_DT)BUFF_RING_GET_FREE_SIZE(tp->core.transfer_params.data.data.ring) >= waiting_size))
            {
               if(!USBD_INVOKE(
                  USBD_IOTP_GET_INVOKE_PARAMS(tp), USBD_IOTP_trigger_out_invoked(usbd, ep_num)))
               {
                  /* action not yet executed, will be done by SOF event */
                  USBD_ATOMIC_BOOL_SET(USBD_IOTP_refresh_trigger, USBD_TRUE);
               }
            }
         }
         else
         {
            USBD_ERROR(USBD_DBG_IOTPEV_ONOFF, "wrong tp!");
         }
      }
      else
      {
         USBD_ERROR(USBD_DBG_IOTPEV_ONOFF, "wrong tp owner!");
      }
   }
   else
   {
      USBD_ERROR(USBD_DBG_IOTPEV_ONOFF, "buff ring is full but extension has been called - ignore it!");
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_ring_extension_on_read_remove */

static void USBD_IOTP_ring_extension_on_read(
   Buff_Ring_XT *buf, Buff_Ring_Extensions_XT *extension, Buff_Size_DT size, Buff_Bool_DT rewind_occured)
{
   USBD_UNUSED_PARAM(size);
   USBD_UNUSED_PARAM(rewind_occured);

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   USBD_IOTP_ring_extension_on_read_remove(buf, extension);

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_ring_extension_on_read */

static void USBD_IOTP_ring_extension_on_remove(Buff_Ring_XT *buf, Buff_Ring_Extensions_XT *extension, Buff_Bool_DT rewind_occured)
{
   USBD_UNUSED_PARAM(rewind_occured);

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   USBD_IOTP_ring_extension_on_read_remove(buf, extension);

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_ring_extension_on_remove */



static void USBD_IOTP_ring_process_extension_on_write(Buff_Ring_XT *buf, Buff_Ring_Extensions_XT *extension)
{
   USBD_IOTP_Params_XT  *tp;
   void                 *usbd;
   uint8_t               ep_num;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   tp = (USBD_IOTP_Params_XT *)(extension->on_write_params);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp)
      && BUFF_COMPARE_PTRS(Buff_Ring_XT, buf, Buff_Ring_XT, tp->core.transfer_params.data.data.ring) )
   {
      usbd   = USBD_IOTP_GET_USBD_FROM_TP(tp);
      ep_num = USBD_IOTP_GET_EP_NUM_FROM_TP(tp);

      if(USBD_COMPARE_PTRS(USBD_IOTP_Params_XT, USBD_IO_UP_Get_IN_TP_Params(usbd, ep_num), USBD_IOTP_Params_XT, tp))
      {
         if(USBD_IO_UP_EP_IN_Get_Buffered_Data_Size(usbd, ep_num) < 0)
         {
            if(!USBD_INVOKE(
               USBD_IOTP_GET_INVOKE_PARAMS(tp), USBD_IOTP_trigger_in_invoked(usbd, ep_num)))
            {
               /* action not yet executed, will be done by SOF event */
               USBD_ATOMIC_BOOL_SET(USBD_IOTP_refresh_trigger, USBD_TRUE);
            }
         }
      }
      else
      {
         USBD_ERROR(USBD_DBG_IOTPEV_ONOFF, "wrong tp!");
      }
   }
   else
   {
      USBD_ERROR(USBD_DBG_IOTPEV_ONOFF, "wrong tp owner!");
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_ring_process_extension_on_write */



static void USBD_IOTP_ring_extension_on_write(
   Buff_Ring_XT *buf, Buff_Ring_Extensions_XT *extension, Buff_Size_DT size, Buff_Bool_DT rewind_occured)
{
   USBD_UNUSED_PARAM(size);
   USBD_UNUSED_PARAM(rewind_occured);

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   USBD_IOTP_ring_process_extension_on_write(buf, extension);

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_ring_extension_on_write */



static void USBD_IOTP_ring_io_data_memcpy_in(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_IN_Data_Method_Port_HT data_method)
{
   USBD_IOTP_Params_XT       *tp          = (USBD_IOTP_Params_XT*)tp_params;
   usbd_iotp_vendor_memcpy_XT vendor_data;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction)
      && USBD_CHECK_HANDLER(USBD_IO_IN_Data_Method_Port_HT, data_method)
      && BUFF_CHECK_PTR(Buff_Ring_XT, tp->core.transfer_params.data.data.ring))
   {
      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPEV_PROCESSING,
         "memcpy_in  before::ep: %d::%s, dir: %s; busy_size = %d",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_IN) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         BUFF_RING_GET_BUSY_SIZE(tp->core.transfer_params.data.data.ring));

      BUFF_PROTECTION_LOCK(tp->core.transfer_params.data.data.ring);

      if(!BUFF_RING_IS_EMPTY(tp->core.transfer_params.data.data.ring))
      {
         vendor_data.tp             = tp;
         vendor_data.data_method.in = data_method;
         vendor_data.is_last_part   = USBD_TRUE;

         (void)Buff_Ring_Read_Vendor(
            tp->core.transfer_params.data.data.ring,
            &vendor_data,
            (Buff_Size_DT)(tp->core.pipe_params.data.mps + tp->core.pipe_params.data.mps),
            USBD_IOTP_ring_vendor_memcpy_in,
            BUFF_FALSE);
      }

      BUFF_PROTECTION_UNLOCK(tp->core.transfer_params.data.data.ring);

      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPEV_PROCESSING,
         "memcpy_in  after ::ep: %d::%s, dir: %s; busy_size = %d",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_IN) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         BUFF_RING_GET_BUSY_SIZE(tp->core.transfer_params.data.data.ring));
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_ring_io_data_memcpy_in */



static void USBD_IOTP_ring_io_memcpy_out(
   void* tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT packet_size,
   USBD_IO_Inout_Data_Size_DT left_size,
   USBD_IO_OUT_Data_Method_Port_HT data_method)
{
   USBD_IOTP_Params_XT       *tp          = (USBD_IOTP_Params_XT*)tp_params;
   usbd_iotp_vendor_memcpy_XT vendor_data;
   Buff_Size_DT               ret_size    = 0;

   USBD_UNUSED_PARAM(packet_size);

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction)
      && USBD_CHECK_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, data_method)
      && BUFF_CHECK_PTR(Buff_Ring_XT, tp->core.transfer_params.data.data.ring))
   {
      USBD_DEBUG_HI_9(
         USBD_DBG_IOTPEV_PROCESSING,
         "memcpy_out before::ep: %d::%s, dir: %s; buff:: size: %d; busy_size: %d; first_free: %d; first_busy: %d; left_size: %d; packet_size: %d",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         tp->core.transfer_params.data.data.ring->size,
         tp->core.transfer_params.data.data.ring->busy_size,
         tp->core.transfer_params.data.data.ring->first_free,
         tp->core.transfer_params.data.data.ring->first_busy,
         left_size,
         packet_size);

      vendor_data.tp                   = tp;
      vendor_data.data_method.out      = data_method;
      vendor_data.out_left_data_size   = left_size;

      BUFF_PROTECTION_LOCK(tp->core.transfer_params.data.data.ring);

      if(left_size <= (USBD_IO_Inout_Data_Size_DT)BUFF_RING_GET_FREE_SIZE(tp->core.transfer_params.data.data.ring))
      {
         ret_size = Buff_Ring_Write_Vendor(
            tp->core.transfer_params.data.data.ring,
            &vendor_data,
            left_size,
            USBD_IOTP_ring_vendor_memcpy_out,
            BUFF_FALSE,
            BUFF_FALSE);
      }

      BUFF_PROTECTION_UNLOCK(tp->core.transfer_params.data.data.ring);

      if(0 == ret_size)
      {
         /**
          * nothing has been written -
          * perform data reading with size 0 only to block endpoint on reading and wait for more space in the buffer
          */
            (void)USBD_CALL_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, data_method)(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_GET_EP_DIR_FROM_TP(tp),
               &left_size,
               0,
               USBD_FALSE);
      }

      USBD_DEBUG_HI_7(
         USBD_DBG_IOTPEV_PROCESSING,
         "memcpy_out after ::ep: %d::%s, dir: %s; buff:: size: %d; busy_size: %d; first_free: %d; first_busy: %d",
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_GET_USBD_FROM_TP(tp),
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         tp->core.transfer_params.data.data.ring->size,
         tp->core.transfer_params.data.data.ring->busy_size,
         tp->core.transfer_params.data.data.ring->first_free,
         tp->core.transfer_params.data.data.ring->first_busy);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_ring_io_memcpy_out */



static void USBD_IOTP_ring_io_evdata_out(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_OUT_Data_Method_Port_HT mem_cpy)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   USBD_IOTP_ring_io_memcpy_out(tp_params, transaction, size, size, mem_cpy);

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_ring_io_evdata_out */



static void USBD_IOTP_ring_io_evdata_in(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_IN_Data_Method_Port_HT mem_cpy)
{
   USBD_UNUSED_PARAM(size);

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   USBD_IOTP_ring_io_data_memcpy_in(tp_params, transaction, mem_cpy);

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_ring_io_evdata_in */



static void USBD_IOTP_ring_clear_buff(
   USBD_IOTP_Params_XT *tp, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction, USBD_Bool_DT set_valid_handlers)
{
   Buff_Ring_Extensions_XT *extension;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   extension = tp->core.transfer_params.data.data.ring->extension;

   BUFF_PROTECTION_LOCK(tp->core.transfer_params.data.data.ring);

   if(BUFF_CHECK_PTR(Buff_Ring_Extensions_XT, extension))
   {
      if(USB_EP_DESC_DIR_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp))
      {
         USBD_IO_SET_OUT_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));

         extension->on_read_params  = tp;
         extension->on_remove_params= tp;

         if(USBD_BOOL_IS_TRUE(set_valid_handlers))
         {
            USBD_IO_SET_OUT_MEMCPY_HANDLER(transaction,  USBD_IOTP_ring_io_memcpy_out);
            extension->on_read      = USBD_IOTP_ring_extension_on_read;
            extension->on_remove    = USBD_IOTP_ring_extension_on_remove;
         }
         else
         {
            USBD_IO_SET_OUT_MEMCPY_HANDLER(transaction,  USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
            extension->on_read      = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Read);
            extension->on_remove    = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Remove);
         }
      }
      else
      {
         USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));

         extension->on_write_params = tp;

         if(USBD_BOOL_IS_FALSE(set_valid_handlers))
         {
            USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
            extension->on_write     = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Write);
         }
         else
         {
            USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_IOTP_ring_io_data_memcpy_in);
            extension->on_write     = USBD_IOTP_ring_extension_on_write;
         }
      }
   }

   Buff_Ring_Clear(tp->core.transfer_params.data.data.ring, BUFF_FALSE);

   BUFF_PROTECTION_UNLOCK(tp->core.transfer_params.data.data.ring);

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_ring_clear_buff */



USBD_Bool_DT USBD_IOTP_Ring_Start_Transfer_Infinitely(
   USBD_IOTP_Params_XT          *tp,
   Buff_Ring_XT                 *ring)
{
   return USBD_IOTP_Ring_start_stop_transfer_infinitely(tp, ring, USBD_IOTP_ring_start_transfer_infinitely_invoked, "start");
} /* USBD_IOTP_Ring_Start_Transfer_Infinitely */



USBD_Bool_DT USBD_IOTP_Ring_Stop_Transfer_Infinitely(
   USBD_IOTP_Params_XT          *tp,
   Buff_Ring_XT                 *ring)
{
   return USBD_IOTP_Ring_start_stop_transfer_infinitely(tp, ring, USBD_IOTP_ring_stop_transfer_infinitely_invoked, "stop");
} /* USBD_IOTP_Ring_Stop_Transfer_Infinitely */
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

#if(USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS == USBD_FEATURE_PRESENT)
      if(USBD_IOTP_DATA_RING_INFINITIVE == tp->core.data_type)
      {
         USBD_IOTP_ring_clear_buff(tp, transaction, USBD_FALSE);
      }
      else
#endif
         if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active))
      {
         USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);
         tp->core.data_type                        = USBD_IOTP_DATA_LINEAR;
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

#if(USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS == USBD_FEATURE_PRESENT)
      if(USBD_IOTP_DATA_RING_INFINITIVE == tp->core.data_type)
      {
         USBD_IOTP_ring_clear_buff(tp, transaction, USBD_FALSE);
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
   USBD_IOTP_Params_XT    *tp;
   const USB_Endpoint_Desc_DT  *ep_desc;
   uint16_t mps = 1;
   uint8_t  ep_type;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_ONOFF);

   tp = (USBD_IOTP_Params_XT*)tp_params;

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp) && USBD_COMPARE_PTRS(void, tp_owner, void, &USBD_IOTP_refresh_trigger))
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
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false");

      if(USBD_BOOL_IS_FALSE(active))
      {
         USBD_CALL_HANDLER(USBD_IO_UP_Abort_HT, tp->core.pipe_params.handlers.abort)(
            tp_params,
            transaction);

         USBD_IO_UP_SET_ERROR_HANDLER(&(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT));
         USBD_IO_UP_SET_SFIN_HANDLER (&(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Stall_Fin_HT));
         USBD_IO_UP_SET_ABORT_HANDLER(&(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Abort_HT));
      }

      USBD_ATOMIC_BOOL_SET(tp->core.transfer_active, USBD_FALSE);

#if(USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS == USBD_FEATURE_PRESENT)
      if(USBD_IOTP_DATA_RING_INFINITIVE != tp->core.data_type)
#endif
      {
         tp->core.transfer_params.data.data.linear = USBD_MAKE_INVALID_PTR(USBD_IOTP_Data_DT);
         tp->core.transfer_params.data.offset      = 0;
         tp->core.transfer_params.data.size        = 0;
      }

      if(USB_EP_DESC_DIR_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp))
      {
         USBD_IO_UP_SET_OUT_DATA_EVENT_HANDLER(
            &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_OUT_Data_Event_HT));

         tp->core.transfer_params.dir.out.mem_cpy             = USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_Port_HT);
         tp->core.transfer_params.dir.out.dont_wait_for_ready = USBD_FALSE;
      }
      else
      {
         USBD_IO_UP_SET_IN_DATA_EVENT_HANDLER(
            &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_IN_Data_Event_HT));

         tp->core.transfer_params.dir.in.last_packet_size     = 0;
         tp->core.transfer_params.dir.in.req_in_progress      = USBD_FALSE;
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

#if(USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS == USBD_FEATURE_PRESENT)
         if(USBD_IOTP_DATA_RING_INFINITIVE == tp->core.data_type)
         {
            if(USB_EP_DESC_DIR_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp))
            {
               USBD_IO_UP_SET_OUT_DATA_EVENT_HANDLER(&(tp->core.pipe_params.handlers), USBD_IOTP_ring_io_evdata_out);
            }
            else
            {
               USBD_IO_UP_SET_IN_DATA_EVENT_HANDLER(&(tp->core.pipe_params.handlers), USBD_IOTP_ring_io_evdata_in);
            }

            USBD_IOTP_ring_clear_buff(tp, transaction, USBD_TRUE);
         }
         else
#endif
         {
            if(USB_EP_DESC_DIR_OUT == USBD_IOTP_GET_EP_DIR_FROM_TP(tp))
            {
               USBD_IO_UP_SET_OUT_DATA_EVENT_HANDLER(&(tp->core.pipe_params.handlers), USBD_IOTP_io_evdata_out);
            }
            else
            {
               USBD_IO_UP_SET_IN_DATA_EVENT_HANDLER(&(tp->core.pipe_params.handlers), USBD_IOTP_io_evdata_in);
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
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_active) ? "true" : "false");
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_ONOFF);
} /* USBD_IOTP_io_reinit */



#ifdef USBD_IOTP_POST_IMP_INCLUDE
#include "usbd_iotp_post_imp.h"
#endif
