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

#ifndef USBD_IOTP_FINITE_IN_C_
#define USBD_IOTP_FINITE_IN_C_
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



typedef USBD_Bool_DT (*USBD_IOTP_send_invoked_connect_data_HT)(
   USBD_IOTP_Params_XT       *tp,
   const void                *data,
   USBD_IO_Inout_Data_Size_DT size);



typedef USBD_IO_Inout_Data_Size_DT (*USBD_IOTP_io_data_memcpy_in_HT)(
   USBD_IOTP_Params_XT *tp, USBD_IO_IN_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size_left);



static USBD_Bool_DT USBD_IOTP_send_invoked_connect_data_linear(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size);
#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_VECTOR_BUFFERS)
static USBD_Bool_DT USBD_IOTP_send_invoked_connect_data_vector(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size);
#endif
#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_TREE_BUFFERS)
static USBD_Bool_DT USBD_IOTP_send_invoked_connect_data_tree(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size);
#endif
#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_BUFFERS)
static USBD_Bool_DT USBD_IOTP_send_invoked_connect_data_ring(
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
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_in_linear(
   USBD_IOTP_Params_XT *tp, USBD_IO_IN_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size_left);
#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_VECTOR_BUFFERS)
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_in_vector(
   USBD_IOTP_Params_XT *tp, USBD_IO_IN_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size_left);
#endif
#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_TREE_BUFFERS)
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_in_tree(
   USBD_IOTP_Params_XT *tp, USBD_IO_IN_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size_left);
#endif
#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_BUFFERS)
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_in_ring(
   USBD_IOTP_Params_XT *tp, USBD_IO_IN_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size_left);
#endif
static void USBD_IOTP_io_data_memcpy_in(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_IN_Data_Method_Port_HT data_method);



static const USBD_IOTP_io_data_memcpy_in_HT USBD_IOTP_memcpy_in_tab[4] =
{
   [USBD_IOTP_DATA_LINEAR] = USBD_IOTP_io_data_memcpy_in_linear,
#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_VECTOR_BUFFERS)
   [USBD_IOTP_DATA_VECTOR] = USBD_IOTP_io_data_memcpy_in_vector,
#else
   [USBD_IOTP_DATA_VECTOR] = USBD_MAKE_INVALID_HANDLER(USBD_IOTP_io_data_memcpy_in_HT),
#endif
#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_TREE_BUFFERS)
   [USBD_IOTP_DATA_TREE]   = USBD_IOTP_io_data_memcpy_in_tree,
#else
   [USBD_IOTP_DATA_TREE]   = USBD_MAKE_INVALID_HANDLER(USBD_IOTP_io_data_memcpy_in_HT),
#endif
#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_BUFFERS)
   [USBD_IOTP_DATA_RING]   = USBD_IOTP_io_data_memcpy_in_ring
#else
   [USBD_IOTP_DATA_RING]   = USBD_MAKE_INVALID_HANDLER(USBD_IOTP_io_data_memcpy_in_HT)
#endif
};



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

      if(USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_not_ring_infinite_owner))
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



static USBD_Bool_DT USBD_IOTP_send_invoked_connect_data_linear(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size)
{
   tp->core.transfer_params.data.data.linear  = data;
   tp->core.transfer_params.data.offset       = 0;
   tp->core.transfer_params.data.size         = size;
   tp->core.transfer_params.dir.in.next_contineous_part.data = data;
   tp->core.transfer_params.dir.in.next_contineous_part.size = size;
   tp->core.transfer_params.data_type                        = USBD_IOTP_DATA_LINEAR;

   return USBD_TRUE;
} /* USBD_IOTP_send_invoked_connect_data_linear */



#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_VECTOR_BUFFERS)
static USBD_Bool_DT USBD_IOTP_send_invoked_connect_data_vector(
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
   tp->core.transfer_params.data_type                  = USBD_IOTP_DATA_VECTOR;

   return USBD_TRUE;
} /* USBD_IOTP_send_invoked_connect_data_vector */
#endif



#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_TREE_BUFFERS)
static USBD_Bool_DT USBD_IOTP_send_invoked_connect_data_tree(
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
   tp->core.transfer_params.data_type                  = USBD_IOTP_DATA_TREE;

   return USBD_TRUE;
} /* USBD_IOTP_send_invoked_connect_data_tree */
#endif



#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_BUFFERS)
static USBD_Bool_DT USBD_IOTP_send_invoked_connect_data_ring(
   USBD_IOTP_Params_XT *tp,
   const void *data,
   USBD_IO_Inout_Data_Size_DT size)
{
   Buff_Ring_XT *ring = (Buff_Ring_XT*)data;
   USBD_Bool_DT result = USBD_TRUE;

   tp->core.transfer_params.data.data.ring                  = ring;
   tp->core.transfer_params.data.offset                     = 0;
   tp->core.transfer_params.dir.in.next_contineous_part.data= USBD_MAKE_INVALID_PTR(const void);
   tp->core.transfer_params.dir.in.next_contineous_part.size= 0;
   tp->core.transfer_params.data.size                       = size;
   tp->core.transfer_params.data_type                       = USBD_IOTP_DATA_RING;

   if(Buff_Ring_Is_Empty(ring, BUFF_TRUE))
   {
      // TODO: connect buffer extensions
      result = USBD_FALSE;
   }

   return result;
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
   USBD_Bool_DT do_trigger;
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
      tp->up_link.data.size_transferred                 = 0;
      do_trigger = USBD_CALL_HANDLER(USBD_IOTP_send_invoked_connect_data_HT, connect_data)(tp, data, size);
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

      transfer_active = USBD_ATOMIC_BOOL_GET(tp->core.transfer_params.transfer_active);
      USBD_ATOMIC_BOOL_SET(tp->core.transfer_params.transfer_active, USBD_TRUE);

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
         USBD_ATOMIC_BOOL_SET(tp->core.transfer_params.transfer_active, transfer_active);
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

         USBD_ATOMIC_BOOL_SET(tp->core.transfer_params.transfer_active, USBD_FALSE);
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

      if(USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_not_ring_infinite_owner)
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
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            tp->core.transfer_params.dir.in.last_packet_size);

         if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_params.transfer_active)
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
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            tp->core.transfer_params.dir.in.last_packet_size);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_Send */



#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_VECTOR_BUFFERS)
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

      if(USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_not_ring_infinite_owner)
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
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            tp->core.transfer_params.dir.in.last_packet_size);

         if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_params.transfer_active)
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
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            tp->core.transfer_params.dir.in.last_packet_size);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_Send_From_Vector */
#endif



#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_TREE_BUFFERS)
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

      if(USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_not_ring_infinite_owner)
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
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            tp->core.transfer_params.dir.in.last_packet_size);

         if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_params.transfer_active)
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
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            tp->core.transfer_params.dir.in.last_packet_size);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return result;
} /* USBD_IOTP_Send_From_Tree */
#endif



#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_BUFFERS)
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

      if(USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_not_ring_infinite_owner)
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
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
            tp->core.transfer_params.data.size,
            tp->core.transfer_params.data.offset,
            tp->core.transfer_params.dir.in.last_packet_size);

         if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_params.transfer_active)
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
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
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
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->core.transfer_params.dir.in.last_packet_size);

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active)
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
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->core.transfer_params.dir.in.last_packet_size);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_io_data_provide_in */



Buff_Size_DT USBD_IOTP_Ring_Vendor_Memcpy_In(const Buff_Memcpy_Params_XT *params)
{
   USBD_IOTP_Vendor_Memcpy_XT *vendor_data;
   USBD_IOTP_Params_XT        *tp;
   Buff_Size_DT                result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   vendor_data = (USBD_IOTP_Vendor_Memcpy_XT*)(params->dest);
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
} /* USBD_IOTP_Ring_Vendor_Memcpy_In */



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

#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_VECTOR_BUFFERS)
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_in_vector(
   USBD_IOTP_Params_XT *tp, USBD_IO_IN_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size_left)
{
   USBD_IOTP_Vendor_Memcpy_XT vendor_data;
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
      USBD_IOTP_Ring_Vendor_Memcpy_In);

   tp->core.transfer_params.dir.in.next_contineous_part = Buff_Readable_Vector_Get_Contineous_Part(
      tp->core.transfer_params.data.data.vector.data,
      tp->core.transfer_params.data.data.vector.num_elems,
      tp->core.transfer_params.data.offset + result);

   return (USBD_IO_Inout_Data_Size_DT)result;
} /* USBD_IOTP_io_data_memcpy_in_vector */
#endif

#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_TREE_BUFFERS)
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_in_tree(
   USBD_IOTP_Params_XT *tp, USBD_IO_IN_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size_left)
{
   USBD_IOTP_Vendor_Memcpy_XT vendor_data;
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
      USBD_IOTP_Ring_Vendor_Memcpy_In);

   tp->core.transfer_params.dir.in.next_contineous_part = Buff_Readable_Tree_Get_Contineous_Part(
      tp->core.transfer_params.data.data.tree.data,
      tp->core.transfer_params.data.data.tree.num_elems,
      tp->core.transfer_params.data.offset + result);

   return (USBD_IO_Inout_Data_Size_DT)result;
} /* USBD_IOTP_io_data_memcpy_in_tree */
#endif

#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_BUFFERS)
static USBD_IO_Inout_Data_Size_DT USBD_IOTP_io_data_memcpy_in_ring(
   USBD_IOTP_Params_XT *tp, USBD_IO_IN_Data_Method_Port_HT data_method, USBD_IO_Inout_Data_Size_DT size_left)
{
   USBD_IOTP_Vendor_Memcpy_XT vendor_data;
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
         USBD_IOTP_Ring_Vendor_Memcpy_In,
         BUFF_FALSE);
   }

   BUFF_PROTECTION_UNLOCK(tp->core.transfer_params.data.data.ring);

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
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->core.transfer_params.dir.in.last_packet_size);

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active))
      {
         size_left = tp->core.transfer_params.data.size - tp->core.transfer_params.data.offset;

         if((size_left > 0) || (0 == tp->core.transfer_params.dir.in.last_packet_size))
         {
            size = USBD_CALL_HANDLER(USBD_IOTP_io_data_memcpy_in_HT, USBD_IOTP_memcpy_in_tab[tp->core.transfer_params.data_type])(
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

            USBD_ATOMIC_BOOL_SET(tp->core.transfer_params.transfer_active, USBD_FALSE);
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
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->core.transfer_params.dir.in.last_packet_size);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_io_data_memcpy_in */

void USBD_IOTP_Io_Evdata_In(
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
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->core.transfer_params.dir.in.last_packet_size);

      if(USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active))
      {
         USBD_IOTP_io_data_memcpy_in(tp_params, transaction, mem_cpy);
      }

      if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_params.transfer_active))
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
         USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false",
         tp->core.transfer_params.data.size,
         tp->core.transfer_params.data.offset,
         tp->core.transfer_params.dir.in.last_packet_size);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_Io_Evdata_In */



#ifdef USBD_IOTP_FINITE_IN_POST_IMP_INCLUDE
#include "usbd_iotp_finite_in_post_imp.h"
#endif
