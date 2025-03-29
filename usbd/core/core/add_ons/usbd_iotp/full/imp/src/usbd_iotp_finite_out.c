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

#ifndef USBD_IOTP_FINITE_OUT_C_
#define USBD_IOTP_FINITE_OUT_C_
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



typedef void (*USBD_IOTP_recv_invoked_connect_data_HT)(
   USBD_IOTP_Params_XT       *tp,
   const void                *data,
   USBD_IO_Inout_Data_Size_DT size);



typedef USBD_IO_Inout_Data_Size_DT (*USBD_IOTP_io_data_memcpy_out_HT)(
   USBD_IOTP_Params_XT *tp, USBD_IO_OUT_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);



static void USBD_IOTP_recv_invoked_connect_data_linear(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size);
#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_BUFFERS)
static void USBD_IOTP_recv_invoked_connect_data_ring(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size);
#endif
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_out_linear(
   USBD_IOTP_Params_XT *tp, USBD_IO_OUT_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);
#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_BUFFERS)
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_out_ring(
   USBD_IOTP_Params_XT *tp, USBD_IO_OUT_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);
#endif
static USBD_Bool_DT USBD_IOTP_recv_invoked(
   USBD_IOTP_Params_XT *tp,
   USBD_IOTP_recv_invoked_connect_data_HT  connect_data,
   void *data,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_Inout_Data_Size_DT *size_left_ret,
   USBD_Bool_DT dont_wait_for_ready,
   USBD_Bool_DT *result_ret);
static USBD_Bool_DT USBD_IOTP_recv_ready_invoked(USBD_IOTP_Params_XT *tp, USBD_Bool_DT *result);
static USBD_Bool_DT USBD_IOTP_recv(
      USBD_IOTP_Params_XT                    *tp,
      USBD_IOTP_recv_invoked_connect_data_HT  connect_data,
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



static const USBD_IOTP_io_data_memcpy_out_HT USBD_IOTP_memcpy_out_tab[4] =
{
   [USBD_IOTP_DATA_LINEAR] = USBD_IOTP_io_data_memcpy_out_linear,
   [USBD_IOTP_DATA_VECTOR] = USBD_MAKE_INVALID_HANDLER(USBD_IOTP_io_data_memcpy_out_HT),
   [USBD_IOTP_DATA_TREE]   = USBD_MAKE_INVALID_HANDLER(USBD_IOTP_io_data_memcpy_out_HT),
#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_BUFFERS)
   [USBD_IOTP_DATA_RING]   = USBD_IOTP_io_data_memcpy_out_ring
#else
   [USBD_IOTP_DATA_RING]   = USBD_MAKE_INVALID_HANDLER(USBD_IOTP_io_data_memcpy_out_HT)
#endif
};



static void USBD_IOTP_recv_invoked_connect_data_linear(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size)
{
   tp->core.transfer_params.data.data.linear             = (void*)data;
   tp->core.transfer_params.data.offset                  = 0;
   tp->core.transfer_params.data.size                    = size;
   tp->core.transfer_params.dir.out.next_contineous_part = size;
   tp->core.transfer_params.data_type                    = USBD_IOTP_DATA_LINEAR;
} /* USBD_IOTP_recv_invoked_connect_data_linear */



#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_BUFFERS)
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
   tp->core.transfer_params.data_type                    = USBD_IOTP_DATA_RING;
} /* USBD_IOTP_recv_invoked_connect_data_ring */
#endif



Buff_Size_DT USBD_IOTP_Ring_Vendor_Memcpy_Out(const Buff_Memcpy_Params_XT *params)
{
   USBD_IOTP_Vendor_Memcpy_XT *vendor_data;
   Buff_Size_DT result = 0;
   USBD_Bool_DT is_last_part;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   vendor_data = (USBD_IOTP_Vendor_Memcpy_XT*)(params->src);

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
} /* USBD_IOTP_Ring_Vendor_Memcpy_Out */



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



#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_BUFFERS)
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_out_ring(
   USBD_IOTP_Params_XT *tp, USBD_IO_OUT_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part)
{
   USBD_IOTP_Vendor_Memcpy_XT vendor_data;
   Buff_Size_DT               result = 0;

   USBD_UNUSED_PARAM(is_last_part);

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
         USBD_IOTP_Ring_Vendor_Memcpy_Out,
         BUFF_FALSE,
         BUFF_FALSE);
   }

   BUFF_PROTECTION_UNLOCK(tp->core.transfer_params.data.data.ring);

   return (USBD_IO_Inout_Data_Size_DT)result;
} /* USBD_IOTP_io_data_memcpy_out_ring */
#endif



static USBD_Bool_DT USBD_IOTP_recv_invoked(
   USBD_IOTP_Params_XT *tp,
   USBD_IOTP_recv_invoked_connect_data_HT  connect_data,
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
      USBD_CALL_HANDLER(USBD_IOTP_recv_invoked_connect_data_HT, connect_data)(tp, data, size);
      tp->core.transfer_params.dir.out.dont_wait_for_ready  = dont_wait_for_ready;
      result                                                = USBD_TRUE;

      if(tp->core.transfer_params.dir.out.next_contineous_part > tp->core.pipe_params.data.mps)
      {
         USBD_ATOMIC_BOOL_SET(tp->core.transfer_params.transfer_active, USBD_TRUE);
         USBD_IO_SET_OUT_PROVIDE_HANDLER(
            transaction, USBD_IOTP_io_provide_out);
         USBD_IO_SET_OUT_MEMCPY_HANDLER(
            transaction, USBD_IOTP_io_memcpy_out);
      }
      else
      {
         USBD_ATOMIC_BOOL_SET(tp->core.transfer_params.transfer_active, USBD_TRUE);
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

            size = USBD_CALL_HANDLER(USBD_IOTP_io_data_memcpy_out_HT, USBD_IOTP_memcpy_out_tab[tp->core.transfer_params.data_type])(
               tp, tp->core.transfer_params.dir.out.mem_cpy, size, is_last_part);

            tp->up_link.data.size_transferred      = size;
            tp->core.transfer_params.data.offset  += size;

            if(tp->core.transfer_params.data.size == tp->core.transfer_params.data.offset)
            {
               USBD_ATOMIC_BOOL_SET(tp->core.transfer_params.transfer_active, USBD_FALSE);

               USBD_IO_SET_OUT_PROVIDE_HANDLER(
                  transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
               USBD_IO_SET_OUT_MEMCPY_HANDLER(
                  transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
            }

            if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active))
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

         USBD_ATOMIC_BOOL_SET(tp->core.transfer_params.transfer_active, USBD_FALSE);
      }

      if(USBD_CHECK_PTR(USBD_IO_Inout_Data_Size_DT, size_left_ret))
      {
         if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active))
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
      USBD_IOTP_recv_invoked_connect_data_HT  connect_data,
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

      if(USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_not_ring_infinite_owner)
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
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            USBD_BOOL_IS_TRUE(tp->core.transfer_params.dir.out.dont_wait_for_ready) ? "true" : "false",
            tp->core.transfer_params.data.data.linear);

         if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_params.transfer_active))
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
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
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



#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_BUFFERS)
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

      if(USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_not_ring_infinite_owner)
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
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
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
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
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
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->up_link.data.size_transferred,
         USBD_BOOL_IS_TRUE(tp->core.transfer_params.dir.out.dont_wait_for_ready) ? "true" : "false",
         tp->core.transfer_params.data.data.linear);

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active)
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
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
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
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->up_link.data.size_transferred,
         USBD_BOOL_IS_TRUE(tp->core.transfer_params.dir.out.dont_wait_for_ready) ? "true" : "false",
         tp->core.transfer_params.data.data.linear);

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active))
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

         part_size = USBD_CALL_HANDLER(USBD_IOTP_io_data_memcpy_out_HT, USBD_IOTP_memcpy_out_tab[tp->core.transfer_params.data_type])(
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

            USBD_ATOMIC_BOOL_SET(tp->core.transfer_params.transfer_active, USBD_FALSE);

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
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->up_link.data.size_transferred,
         USBD_BOOL_IS_TRUE(tp->core.transfer_params.dir.out.dont_wait_for_ready) ? "true" : "false",
         tp->core.transfer_params.data.data.linear);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_io_memcpy_out */



void USBD_IOTP_Io_Evdata_Out(
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
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->up_link.data.size_transferred,
         USBD_BOOL_IS_TRUE(tp->core.transfer_params.dir.out.dont_wait_for_ready) ? "true" : "false",
         tp->core.transfer_params.data.data.linear);

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active))
      {
         if((tp->core.transfer_params.dir.out.size_in_progress != USBD_IO_GET_OUT_TRANSFERRED_SIZE(transaction))
            || ((-1) != size))
         {
            USBD_ATOMIC_BOOL_SET(tp->core.transfer_params.transfer_active, USBD_FALSE);
         }
      }

      tp->up_link.data.size_transferred += USBD_IO_GET_OUT_TRANSFERRED_SIZE(transaction);

      if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_params.transfer_active))
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
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->up_link.data.size_transferred,
         USBD_BOOL_IS_TRUE(tp->core.transfer_params.dir.out.dont_wait_for_ready) ? "true" : "false",
         tp->core.transfer_params.data.data.linear);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_Io_Evdata_Out */



#ifdef USBD_IOTP_FINITE_OUT_POST_IMP_INCLUDE
#include "usbd_iotp_finite_out_post_imp.h"
#endif
