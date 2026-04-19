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

#ifndef USBD_IOTP_RING_EXTENSIONS_C_
#define USBD_IOTP_RING_EXTENSIONS_C_
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



#if((USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_BUFFERS) || (USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_INFINITE_BUFFERS))

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

void USBD_IOTP_Ring_Extension_On_Read(
   Buff_Ring_XT *buf, Buff_Ring_Extensions_XT *extension, Buff_Size_DT size, Buff_Bool_DT rewind_occured)
{
   USBD_UNUSED_PARAM(size);
   USBD_UNUSED_PARAM(rewind_occured);

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   USBD_IOTP_ring_extension_on_read_remove(buf, extension);

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_Ring_Extension_On_Read */

void USBD_IOTP_Ring_Extension_On_Remove(Buff_Ring_XT *buf, Buff_Ring_Extensions_XT *extension, Buff_Bool_DT rewind_occured)
{
   USBD_UNUSED_PARAM(rewind_occured);

   USBD_ENTER_FUNC(USBD_DBG_IOTPEV_PROCESSING);

   USBD_IOTP_ring_extension_on_read_remove(buf, extension);

   USBD_EXIT_FUNC(USBD_DBG_IOTPEV_PROCESSING);
} /* USBD_IOTP_Ring_Extension_On_Remove */



void USBD_IOTP_Ring_Extension_On_Write(
   Buff_Ring_XT *buf, Buff_Ring_Extensions_XT *extension, Buff_Size_DT size, Buff_Bool_DT rewind_occured)
{
   USBD_IOTP_Params_XT  *tp;
   void                 *usbd;
   uint8_t               ep_num;

   USBD_UNUSED_PARAM(size);
   USBD_UNUSED_PARAM(rewind_occured);

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
} /* USBD_IOTP_Ring_Extension_On_Write */



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

#endif



#ifdef USBD_IOTP_RING_EXTENSIONS_POST_IMP_INCLUDE
#include "usbd_iotp_ring_extensions_post_imp.h"
#endif
