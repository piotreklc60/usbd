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

#ifndef USBD_IOTP_RING_INFINITE_C_
#define USBD_IOTP_RING_INFINITE_C_
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


typedef USBD_Bool_DT (*USBD_IOTP_ring_start_stop_transfer_infinitely_invoked_HT)(
   USBD_IOTP_Params_XT *tp, Buff_Ring_XT *ring, USBD_Bool_DT *result);



#if(USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS == USBD_FEATURE_PRESENT)
static USBD_Bool_DT USBD_IOTP_ring_start_transfer_infinitely_invoked(
   USBD_IOTP_Params_XT *tp, Buff_Ring_XT *ring, USBD_Bool_DT *result)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   tp->core.transfer_params.data.data.ring = ring;
   tp->core.transfer_params.data_type = USBD_IOTP_DATA_RING_INFINITIVE;

   if(USBD_COMPARE_PTRS(void, tp, void, USBD_IO_UP_Get_TP_Params(
      USBD_IOTP_GET_USBD_FROM_TP(tp), USBD_IOTP_GET_EP_NUM_FROM_TP(tp), USBD_IOTP_GET_EP_DIR_FROM_TP(tp))))
   {
      USBD_IO_UP_Set_TP(
         USBD_IOTP_GET_USBD_FROM_TP(tp),
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IOTP_GET_EP_DIR_FROM_TP(tp),
         &(tp->core.pipe_params.handlers),
         tp,
         (void*)USBD_IOTP_ring_infinite_owner);

      *result = USBD_TRUE;
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return USBD_TRUE;
}



static USBD_Bool_DT USBD_IOTP_ring_stop_transfer_infinitely_invoked(
   USBD_IOTP_Params_XT *tp, Buff_Ring_XT *ring, USBD_Bool_DT *result)
{
   USBD_UNUSED_PARAM(ring);

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   tp->core.transfer_params.data_type = USBD_IOTP_DATA_LINEAR;

   if(USBD_COMPARE_PTRS(void, tp, void, USBD_IO_UP_Get_TP_Params(
      USBD_IOTP_GET_USBD_FROM_TP(tp), USBD_IOTP_GET_EP_NUM_FROM_TP(tp), USBD_IOTP_GET_EP_DIR_FROM_TP(tp))))
   {
      USBD_IO_UP_Set_TP(
         USBD_IOTP_GET_USBD_FROM_TP(tp),
         USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
         USBD_IOTP_GET_EP_DIR_FROM_TP(tp),
         &(tp->core.pipe_params.handlers),
         tp,
         (void*)USBD_IOTP_not_ring_infinite_owner);

      *result = USBD_TRUE;
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   return USBD_TRUE;
} /* USBD_IOTP_ring_stop_transfer_infinitely_invoked */



static USBD_Bool_DT USBD_IOTP_Ring_start_stop_transfer_infinitely(
   USBD_IOTP_Params_XT          *tp,
   void                         *tp_owner_expected,
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

      if(USBD_COMPARE_PTRS(void, tp_owner, void, tp_owner_expected)
         && (USB_EP_DIRECTION_IN == USBD_IOTP_GET_EP_DIR_FROM_TP(tp)))
      {
         USBD_DEBUG_HI_5(
            USBD_DBG_IOTPEV_PROCESSING,
            "%s%s::ep: %d::%s; tr_active = %s", desc, "infinitely before",
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_IN) ? "active" : "passive",
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false");

         if(USBD_ATOMIC_BOOL_IS_FALSE(tp->core.transfer_params.transfer_active)
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

         USBD_DEBUG_HI_5(
            USBD_DBG_IOTPEV_PROCESSING,
            "%s%s::ep: %d::%s; tr_active = %s", desc, "infinitely after ",
            USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
            USBD_IO_UP_Is_EP_Active(
               USBD_IOTP_GET_USBD_FROM_TP(tp),
               USBD_IOTP_GET_EP_NUM_FROM_TP(tp),
               USB_EP_DIRECTION_IN) ? "active" : "passive",
            USBD_ATOMIC_BOOL_IS_TRUE(tp->core.transfer_params.transfer_active) ? "true" : "false");
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
                  USBD_IOTP_GET_INVOKE_PARAMS(tp), USBD_IOTP_Trigger_Out_Invoked(usbd, ep_num)))
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
               USBD_IOTP_GET_INVOKE_PARAMS(tp), USBD_IOTP_Trigger_In_Invoked(usbd, ep_num)))
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
   USBD_IOTP_Vendor_Memcpy_XT vendor_data;

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
            USBD_IOTP_Ring_Vendor_Memcpy_In,
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
   USBD_IOTP_Vendor_Memcpy_XT vendor_data;
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
            USBD_IOTP_Ring_Vendor_Memcpy_Out,
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



USBD_Bool_DT USBD_IOTP_Trigger_In_Invoked(USBD_Params_XT *usbd, uint8_t ep_num)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_EVENT);

   (void)USBD_IO_UP_Trigger_INOUT(usbd, ep_num, USB_EP_DIRECTION_IN, USBD_FALSE);

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_EVENT);

   return USBD_TRUE;
} /* USBD_IOTP_Trigger_In_Invoked */



USBD_Bool_DT USBD_IOTP_Trigger_Out_Invoked(USBD_Params_XT *usbd, uint8_t ep_num)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_EVENT);

   USBD_IO_UP_Trigger_INOUT(usbd, ep_num, USB_EP_DIRECTION_OUT, USBD_TRUE);

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_EVENT);

   return USBD_TRUE;
} /* USBD_IOTP_Trigger_Out_Invoked */



void USBD_IOTP_Ring_Io_Evdata_Out(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_OUT_Data_Method_Port_HT mem_cpy)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   USBD_IOTP_ring_io_memcpy_out(tp_params, transaction, size, size, mem_cpy);

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_Ring_Io_Evdata_Out */



void USBD_IOTP_Ring_Io_Evdata_In(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_IN_Data_Method_Port_HT mem_cpy)
{
   USBD_UNUSED_PARAM(size);

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   USBD_IOTP_ring_io_data_memcpy_in(tp_params, transaction, mem_cpy);

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_Ring_Io_Evdata_In */



void USBD_IOTP_Ring_Clear_Buff(
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
} /* USBD_IOTP_Ring_Clear_Buff */



USBD_Bool_DT USBD_IOTP_Ring_Start_Transfer_Infinitely(
   USBD_IOTP_Params_XT          *tp,
   Buff_Ring_XT                 *ring)
{
   return USBD_IOTP_Ring_start_stop_transfer_infinitely(
      tp, (void*)USBD_IOTP_not_ring_infinite_owner, ring, USBD_IOTP_ring_start_transfer_infinitely_invoked, "start");
} /* USBD_IOTP_Ring_Start_Transfer_Infinitely */



USBD_Bool_DT USBD_IOTP_Ring_Stop_Transfer_Infinitely(
   USBD_IOTP_Params_XT          *tp,
   Buff_Ring_XT                 *ring)
{
   return USBD_IOTP_Ring_start_stop_transfer_infinitely(
      tp, (void*)USBD_IOTP_ring_infinite_owner, ring, USBD_IOTP_ring_stop_transfer_infinitely_invoked, "stop");
} /* USBD_IOTP_Ring_Stop_Transfer_Infinitely */
#endif



#ifdef USBD_IOTP_RING_INFINITE_POST_IMP_INCLUDE
#include "usbd_iotp_ring_infinite_post_imp.h"
#endif
