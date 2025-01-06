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

#ifndef USBD_IOTP_BUFF_C_
#define USBD_IOTP_BUFF_C_
#endif

#ifndef USBD_H_
#include "usbd.h"
#endif

#ifndef USBD_PUBLIC_MACRO_H_
#include "usbd_public_macro.h"
#endif

#ifndef USBD_IOTP_BUFF_INTERNAL_H_
#include "usbd_iotp_buff_internal.h"
#endif



typedef enum USBD_IOTP_BUFF_Invoke_Requests_Enum_Tag
{
   USBD_IOTP_BUFF_INVOKE_REQ_NONE = 0,
   USBD_IOTP_BUFF_INVOKE_REQ_ABORT_NO_FLUSH_HW,
   USBD_IOTP_BUFF_INVOKE_REQ_ABORT_FLUSH_HW
}USBD_IOTP_BUFF_Invoke_Requests_ET;



typedef struct usbd_iotp_buff_vendor_memcpy_eXtended_Tag
{
   USBD_IOTP_BUFF_Params_XT           *tp;
   union
   {
      USBD_IO_IN_Data_Method_Port_HT   in;
      USBD_IO_OUT_Data_Method_Port_HT  out;
   }data_method;
   USBD_IO_Inout_Data_Size_DT          out_left_data_size;
   USBD_Bool_DT                        is_last_part;
}usbd_iotp_buff_vendor_memcpy_XT;


static void USBD_IOTP_BUFF_process_extension_on_write(
   Buff_Ring_XT *buf,
   Buff_Ring_Extensions_XT *extension,
   USBD_Bool_DT call_trigger);
static void USBD_IOTP_BUFF_extension_on_write(
   Buff_Ring_XT *buf,
   Buff_Ring_Extensions_XT *extension,
   Buff_Size_DT size,
   bool_t rewind_occured);
static void USBD_IOTP_BUFF_extension_on_write_connect_memcpy_only(
   Buff_Ring_XT *buf,
   Buff_Ring_Extensions_XT *extension,
   Buff_Size_DT size,
   bool_t rewind_occured);
static Buff_Size_DT USBD_IOTP_BUFF_vendor_memcpy_in(
   const Buff_Memcpy_Params_XT *params);
static void USBD_IOTP_BUFF_io_data_memcpy_in(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_IN_Data_Method_Port_HT data_method);
static void USBD_IOTP_BUFF_io_evdata_in(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_IN_Data_Method_Port_HT mem_cpy);

static void USBD_IOTP_BUFF_extension_on_read_remove(Buff_Ring_XT *buf, Buff_Ring_Extensions_XT *extension);
static void USBD_IOTP_BUFF_extension_on_read(
   Buff_Ring_XT *buf,
   Buff_Ring_Extensions_XT *extension,
   Buff_Size_DT size,
   bool_t rewind_occured);
static void USBD_IOTP_BUFF_extension_on_remove(
   Buff_Ring_XT *buf,
   Buff_Ring_Extensions_XT *extension,
   bool_t rewind_occured);
static Buff_Size_DT USBD_IOTP_BUFF_vendor_memcpy_out(
   const Buff_Memcpy_Params_XT *params);
static void USBD_IOTP_BUFF_io_memcpy_out(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT packet_size,
   USBD_IO_Inout_Data_Size_DT left_size,
   USBD_IO_OUT_Data_Method_Port_HT mem_cpy);
static void USBD_IOTP_BUFF_io_evdata_out(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_OUT_Data_Method_Port_HT mem_cpy);

static void USBD_IOTP_BUFF_clear_buff(
   USBD_IOTP_BUFF_Params_XT *tp, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction, USBD_Bool_DT set_valid_handlers);
static void USBD_IOTP_BUFF_io_abort(
   void *tp_params, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction);
#if(USBD_FEATURE_PRESENT == USBD_IO_ISOCHRONOUS_TRANSFER_SUPPORTED)
static void USBD_IOTP_BUFF_io_error(
   void *tp_params, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction, USBD_IO_Inout_Data_Size_DT size);
#endif
static void USBD_IOTP_BUFF_io_reinit(
   void *tp_params, void *tp_owner, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction, USBD_Bool_DT active);
static USBD_Bool_DT USBD_IOTP_BUFF_trigger_in(
   USBD_IOTP_BUFF_Params_XT *tp, Buff_Ring_Extensions_XT *extension, USBD_Params_XT *usbd, uint8_t ep_num, USBD_Bool_DT call_trigger);
static USBD_Bool_DT USBD_IOTP_BUFF_trigger_out(
   USBD_IOTP_BUFF_Params_XT *tp, Buff_Ring_Extensions_XT *extension, USBD_Params_XT *usbd, uint8_t ep_num);
#ifdef USBD_EVENT_PRESENT
static void USBD_IOTP_BUFF_proc_event(USBD_IOTP_BUFF_Params_XT *tp, uint8_t req);
static void USBD_IOTP_BUFF_event(
   USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason);
#endif



static const USBD_IO_UP_Error_HT USBD_IOTP_BUFF_error_table[4] =
{
   USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT),
#if(USBD_FEATURE_PRESENT == USBD_IO_ISOCHRONOUS_TRANSFER_SUPPORTED)
   USBD_IOTP_BUFF_io_error,
#else
   USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT),
#endif
   USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT),
   USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT)
};

static const uint8_t USBD_IOTP_BUFF_dummy_data[2] = {0, 1};

void USBD_IOTP_BUFF_Init(
      USBD_Params_XT *usbd,
      USBDC_Params_XT *usbdc,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_IOTP_BUFF_Params_XT *tp,
      Buff_Ring_XT *buff)
{
#ifdef USBD_EVENT_PRESENT
   USBD_EVENT_Event_Header_XT *event;
#endif

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_ONOFF);

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp))
   {
      USBD_DEBUG_MID_6(USBD_DBG_IOTPBF_ONOFF, "init tp: %p with usbd: %p, usbdc: %p, ep_num: %d, dir: %s, buff: %p",
         tp,
         usbd,
         usbdc,
         ep_num,
         (USB_EP_DIRECTION_OUT == dir) ? "out" : " in",
         buff);

      memset(tp, 0, sizeof(USBD_IOTP_BUFF_Params_XT));

      USBD_IOTP_BUFF_SET_USBD_INSIDE_TP(tp, usbd);
      USBD_IOTP_BUFF_SET_USBDC_INSIDE_TP(tp, usbdc);
      USBD_IOTP_BUFF_SET_EP_NUM_INSIDE_TP(tp, ep_num);
      USBD_IOTP_BUFF_SET_EP_DIR_INSIDE_TP(tp, dir);
      USBD_IOTP_BUFF_SET_BUFF_INSIDE_TP(tp, buff);

      USBD_INIT_INVOKE_PARAMS(USBD_IOTP_BUFF_GET_INVOKE_PARAMS(tp));

#ifdef USBD_EVENT_PRESENT
      event = USBDC_EVENT_Install(usbdc, USBD_IOTP_BUFF_event, USBD_EVENT_REASON_SOF_RECEIVED | USBD_EVENT_INSTALL_ONLY_ONCE);

      if(!USBD_CHECK_PTR(USBD_EVENT_Event_Header_XT, event))
      {
         USBD_WARN(USBD_DBG_IOTPBF_INVALID_PARAMS, "IOTP_BUFF event cannot be installed");
      }
      else
      {
         /* ok */
      }
#endif
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_ONOFF);
} /* USBD_IOTP_BUFF_Init */

USBD_Bool_DT USBD_IOTP_BUFF_Install(
      USBD_IOTP_BUFF_Params_XT *tp)
{
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_ONOFF);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp))
   {
      if(USBD_CHECK_PTR(void, USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp))
         && BUFF_CHECK_PTR(Buff_Ring_XT, USBD_IOTP_BUFF_GET_BUFF_FROM_TP(tp))
         && BUFF_CHECK_PTR(Buff_Ring_Extensions_XT, USBD_IOTP_BUFF_GET_BUFF_FROM_TP(tp)->extension)
         && (USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp) < USBD_MAX_NUM_ENDPOINTS)
         && ((USB_EP_DIRECTION_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp))
            || (USB_EP_DIRECTION_IN == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp))) )
      {
         if(USBD_BOOL_IS_FALSE(USBD_IO_UP_Is_TP_Locked(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp))))
         {
            USBD_IO_UP_SET_ERROR_HANDLER(
               &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT));
            USBD_IO_UP_SET_SFIN_HANDLER (
               &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Stall_Fin_HT));
            USBD_IO_UP_SET_REINIT_HANDLER(
               &(tp->core.pipe_params.handlers),USBD_IOTP_BUFF_io_reinit);
            USBD_IO_UP_SET_ABORT_HANDLER (
               &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Abort_HT));

            if(USB_EP_DIRECTION_IN == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp))
            {
               USBD_IO_UP_SET_IN_DATA_EVENT_HANDLER(
                  &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_IN_Data_Event_HT));
            }
            else
            {
               USBD_IO_UP_SET_OUT_DATA_EVENT_HANDLER(
                  &(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_OUT_Data_Event_HT));
            }

            USBD_IO_UP_Set_TP(
               USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
               USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp),
               &(tp->core.pipe_params.handlers),
               tp,
               (void*)USBD_IOTP_BUFF_dummy_data);

            result = USBD_TRUE;
         }
         else
         {
            USBD_WARN(USBD_DBG_IOTPBF_ONOFF, "TP locked!");
         }
      }
      else
      {
         USBD_WARN_6(USBD_DBG_IOTPBF_INVALID_PARAMS, "wrong params - usbd: %p, buff: %p, buff_ext: %p, ep_num: %d, did: %d(%s)",
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_BUFF_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_BUFF_FROM_TP(tp)->extension,
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp),
            (USB_EP_DIRECTION_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp)) ? "OUT" : "IN");
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_ONOFF);

   return result;
} /* USBD_IOTP_BUFF_Install */



USBD_Params_XT *USBD_IOTP_BUFF_Get_USBD(
      USBD_IOTP_BUFF_Params_XT *tp)
{
   USBD_Params_XT *result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_STATE);

   result = USBD_MAKE_INVALID_PTR(USBD_Params_XT);

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp))
   {
      result = USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_STATE);

   return result;
} /* USBD_IOTP_BUFF_Get_USBD */

USBDC_Params_XT *USBD_IOTP_BUFF_Get_USBDC(
      USBD_IOTP_BUFF_Params_XT *tp)
{
   USBDC_Params_XT *result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_STATE);

   result = USBD_MAKE_INVALID_PTR(USBDC_Params_XT);

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp))
   {
      result = USBD_IOTP_BUFF_GET_USBDC_FROM_TP(tp);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_STATE);

   return result;
} /* USBD_IOTP_BUFF_Get_USBDC */

uint8_t USBD_IOTP_BUFF_Get_EP_Num(
      USBD_IOTP_BUFF_Params_XT *tp)
{
   uint8_t result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_STATE);

   result = USBD_MAX_NUM_ENDPOINTS;

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp))
   {
      result = USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_STATE);

   return result;
} /* USBD_IOTP_BUFF_Get_EP_Num */

USB_EP_Direction_ET USBD_IOTP_BUFF_Get_Dir(
      USBD_IOTP_BUFF_Params_XT *tp)
{
   USB_EP_Direction_ET result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_STATE);

   result = USB_EP_DIRECTION_OUT;

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp))
   {
      result = USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_STATE);

   return result;
} /* USBD_IOTP_BUFF_Get_Dir */

Buff_Ring_XT *USBD_IOTP_BUFF_Get_Buff(
      USBD_IOTP_BUFF_Params_XT *tp)
{
   Buff_Ring_XT *result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_STATE);

   result = BUFF_MAKE_INVALID_PTR(Buff_Ring_XT);

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp))
   {
      result = USBD_IOTP_BUFF_GET_BUFF_FROM_TP(tp);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_STATE);

   return result;
} /* USBD_IOTP_BUFF_Get_Buff */

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)

USBD_IO_Inout_Data_Size_DT USBD_IOTP_BUFF_Get_Transferred_Size(
      USBD_IOTP_BUFF_Params_XT *tp)
{
   USBD_IO_Inout_Data_Size_DT result = 0;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp))
   {
      if(USB_EP_DIRECTION_IN == tp->core.pipe_params.data.dir)
      {
         result = USBD_IOTP_BUFF_GET_SIZE_TRANSFERRED(tp);
      }
      else if(BUFF_CHECK_PTR(Buff_Ring_XT, tp->core.buff))
      {
         result = (USBD_IO_Inout_Data_Size_DT)BUFF_RING_GET_BUSY_SIZE(tp->core.buff);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_STATE);

   return result;
} /* USBD_IOTP_BUFF_Get_Transferred_Size */



void USBD_IOTP_BUFF_Set_Handlers(
      USBD_IOTP_BUFF_Params_XT *tp,
      USBD_IOTP_BUFF_Callback_HT ready,
      USBD_IOTP_BUFF_Callback_HT buf_empty,
      USBD_IOTP_BUFF_Callback_HT error,
      USBD_IOTP_BUFF_Callback_HT abort)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp))
   {
      USBD_IOTP_BUFF_SET_READY_HANDLER(tp, ready);
      USBD_IOTP_BUFF_SET_BUF_EMPTY_HANDLER(tp, buf_empty);
      USBD_IOTP_BUFF_SET_ERROR_HANDLER(tp, error);
      USBD_IOTP_BUFF_SET_ABORT_HANDLER(tp, abort);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_STATE);
} /* USBD_IOTP_BUFF_Set_Handlers */

void USBD_IOTP_BUFF_Set_Ready_Handler(
      USBD_IOTP_BUFF_Params_XT *tp,
      USBD_IOTP_BUFF_Callback_HT ready)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp))
   {
      USBD_IOTP_BUFF_SET_READY_HANDLER(tp, ready);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_STATE);
} /* USBD_IOTP_BUFF_Set_Ready_Handler */

void USBD_IOTP_BUFF_Set_Buf_Empty_Handler(
      USBD_IOTP_BUFF_Params_XT *tp,
      USBD_IOTP_BUFF_Callback_HT buf_empty)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp))
   {
      USBD_IOTP_BUFF_SET_BUF_EMPTY_HANDLER(tp, buf_empty);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_STATE);
} /* USBD_IOTP_BUFF_Set_Buf_Empty_Handler */

void USBD_IOTP_BUFF_Set_Error_Handler(
      USBD_IOTP_BUFF_Params_XT *tp,
      USBD_IOTP_BUFF_Callback_HT error)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp))
   {
      USBD_IOTP_BUFF_SET_ERROR_HANDLER(tp, error);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_STATE);
} /* USBD_IOTP_BUFF_Set_Error_Handler */

void USBD_IOTP_BUFF_Set_Abort_Handler(
      USBD_IOTP_BUFF_Params_XT *tp,
      USBD_IOTP_BUFF_Callback_HT abort)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp))
   {
      USBD_IOTP_BUFF_SET_ABORT_HANDLER(tp, abort);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_STATE);
} /* USBD_IOTP_BUFF_Set_Abort_Handler */

#endif

USBD_Vendor_Data_XT *USBD_IOTP_BUFF_Get_Vendor_Data_Container(USBD_IOTP_BUFF_Params_XT *tp)
{
   USBD_Vendor_Data_XT *result = USBD_MAKE_INVALID_PTR(USBD_Vendor_Data_XT);

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_STATE);

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp))
   {
      result = USBD_IOTP_BUFF_GET_VENDOR_DATA_PTR(tp);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_STATE);

   return result;
} /* USBD_IOTP_BUFF_Get_Vendor_Data_Container */



USBD_Bool_DT USBD_IOTP_BUFF_Is_Transfer_Active(
      USBD_IOTP_BUFF_Params_XT *tp)
{
   Buff_Ring_Extensions_XT *extension;
   void *tp_owner;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_STATE);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp))
   {
      tp_owner = USBD_IO_UP_Get_TP_Owner(
         USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp));

      if(USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_BUFF_dummy_data) && BUFF_CHECK_PTR(Buff_Ring_XT, tp->core.buff))
      {
         extension = tp->core.buff->extension;

         if(BUFF_CHECK_PTR(Buff_Ring_Extensions_XT, extension))
         {
            if(USB_EP_DIRECTION_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp))
            {
#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
               result = tp->up_link.data.out_transfer_active;
#else
               if(!BUFF_RING_IS_FULL(tp->core.buff))
               {
                  result = USBD_TRUE;
               }
#endif
            }
            else
            {
               if(!BUFF_COMPARE_HANDLERS(
                  Buff_Ring_Extension_On_Write, extension->on_write,
                  Buff_Ring_Extension_On_Write, USBD_IOTP_BUFF_extension_on_write))
               {
                  result = USBD_TRUE;
               }
            }
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_STATE);

   return result;
} /* USBD_IOTP_BUFF_Is_Transfer_Active */

static USBD_Bool_DT USBD_IOTP_BUFF_proc_abort(USBD_IOTP_BUFF_Params_XT *tp, USBD_Bool_DT flush_hw_bufs)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   USBD_DEBUG_HI_4(
      USBD_DBG_IOTPBF_PROCESSING,
      "abort      before::ep: %d::%s, dir: %s; busy_size = %d",
      USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
      USBD_IO_UP_Is_EP_Active(
         USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USB_EP_DIRECTION_IN) ? "active" : "passive",
      USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
      BUFF_RING_GET_BUSY_SIZE(tp->core.buff));

   if(USBD_BOOL_IS_TRUE(USBD_IOTP_BUFF_Is_Transfer_Active(tp)) || USBD_BOOL_IS_TRUE(flush_hw_bufs))
   {
      USBD_IO_Abort(
         USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp),
         flush_hw_bufs);
   }

   USBD_DEBUG_HI_4(
      USBD_DBG_IOTPBF_PROCESSING,
      "abort      after ::ep: %d::%s, dir: %s; busy_size = %d",
      USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
      USBD_IO_UP_Is_EP_Active(
         USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USB_EP_DIRECTION_IN) ? "active" : "passive",
      USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
      BUFF_RING_GET_BUSY_SIZE(tp->core.buff));

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   return USBD_TRUE;
}

USBD_Bool_DT USBD_IOTP_BUFF_Abort(
      USBD_IOTP_BUFF_Params_XT *tp,
      USBD_Bool_DT flush_hw_bufs)
{
   void *tp_owner;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp))
   {
      tp_owner = USBD_IO_UP_Get_TP_Owner(
         USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp));

      if(USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_BUFF_dummy_data))
      {
         if(USBD_BOOL_IS_TRUE(USBD_IOTP_BUFF_Is_Transfer_Active(tp)) || USBD_BOOL_IS_TRUE(flush_hw_bufs))
         {
            /* try to synchronize transaction to USBD task/irq */
            if(!USBD_INVOKE(
               USBD_IOTP_BUFF_GET_INVOKE_PARAMS(tp), USBD_IOTP_BUFF_proc_abort(tp, flush_hw_bufs)))
            {
               USBD_DEBUG_HI_4(
                  USBD_DBG_IOTPBF_PROCESSING,
                  "abort   invoke::ep: %d::%s, dir: %s; busy_size = %d",
                  USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
                  USBD_IO_UP_Is_EP_Active(
                     USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
                     USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
                     USB_EP_DIRECTION_IN) ? "active" : "passive",
                  USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
                  BUFF_RING_GET_BUSY_SIZE(tp->core.buff));

                  USBD_ATOMIC_UINT8_SET(
                     tp->invoke.req,
                     USBD_BOOL_IS_TRUE(flush_hw_bufs) ? USBD_IOTP_BUFF_INVOKE_REQ_ABORT_FLUSH_HW : USBD_IOTP_BUFF_INVOKE_REQ_ABORT_NO_FLUSH_HW);
            }
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   return result;
} /* USBD_IOTP_BUFF_Abort */



#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_SUPPORT_CONTROL_TRANSFER)

USBD_Bool_DT USBD_IOTP_BUFF_Send_Status(
      USBD_IOTP_BUFF_Params_XT *tp,
      USBD_IO_Inout_Data_Size_DT *size_left)
{
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction;
   void                                  *usbd;
   uint8_t                                ep_num;
   USBD_Bool_DT                           result = USBD_FALSE;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp)
      && USBD_COMPARE_PTRS(void, tp->owner, void, USBD_IOTP_BUFF_dummy_data)
      && BUFF_CHECK_PTR(Buff_Ring_XT, tp->core.buff)
      && (USB_EP_DIRECTION_IN == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp)) )
   {
      usbd   = USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp);
      ep_num = USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp);

      if(USBD_COMPARE_PTRS(USBD_IOTP_BUFF_Params_XT, USBD_IO_UP_Get_IN_TP_Params(usbd, ep_num), USBD_IOTP_BUFF_Params_XT, tp))
      {
         Buff_Ring_Clear(tp->core.buff, BUFF_TRUE);

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
         tp->up_link.data.in_size_transferred   = 0;
         tp->req_in_progress                    = USBD_TRUE;
#endif

         transaction = USBD_IO_Get_IN_Transaction_Params(usbd, ep_num);

         if(USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction))
         {
            USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_IOTP_BUFF_io_data_memcpy_in);
         }

         result = USBD_IO_UP_Trigger_IN(usbd, ep_num);

         if(USBD_CHECK_PTR(USBD_IO_Inout_Data_Size_DT, size_left))
         {
            *size_left = USBD_IO_UP_EP_IN_Get_Buffered_Data_Size(
               USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
               USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp));
         }

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
         tp->req_in_progress                 = USBD_FALSE;
#endif
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   return result;
} /* USBD_IOTP_BUFF_Send_Status */

#endif



static void USBD_IOTP_BUFF_process_extension_on_write(
   Buff_Ring_XT *buf, Buff_Ring_Extensions_XT *extension, USBD_Bool_DT call_trigger)
{
   USBD_IOTP_BUFF_Params_XT  *tp;
   void                      *usbd;
   uint8_t                    ep_num;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   tp = (USBD_IOTP_BUFF_Params_XT *)(extension->on_write_params);

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp)
      && USBD_COMPARE_PTRS(void, tp->owner, void, USBD_IOTP_BUFF_dummy_data)
      && BUFF_COMPARE_PTRS(Buff_Ring_XT, buf, Buff_Ring_XT, tp->core.buff) )
   {
      usbd   = USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp);
      ep_num = USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp);

      if(USBD_COMPARE_PTRS(USBD_IOTP_BUFF_Params_XT, USBD_IO_UP_Get_IN_TP_Params(usbd, ep_num), USBD_IOTP_BUFF_Params_XT, tp))
      {
         if(!USBD_INVOKE(
            USBD_IOTP_BUFF_GET_INVOKE_PARAMS(tp), USBD_IOTP_BUFF_trigger_in(tp, extension, usbd, ep_num, call_trigger)))
         {
            /* action not yet executed */
         }
      }
      else
      {
         USBD_ERROR(USBD_DBG_IOTPBF_ONOFF, "wrong tp!");
      }
   }
   else
   {
      USBD_ERROR(USBD_DBG_IOTPBF_ONOFF, "wrong tp owner!");
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_PROCESSING);
} /* USBD_IOTP_BUFF_process_extension_on_write */

static void USBD_IOTP_BUFF_extension_on_write(
   Buff_Ring_XT *buf, Buff_Ring_Extensions_XT *extension, Buff_Size_DT size, bool_t rewind_occured)
{
   USBD_UNUSED_PARAM(size);
   USBD_UNUSED_PARAM(rewind_occured);

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   USBD_IOTP_BUFF_process_extension_on_write(buf, extension, USBD_TRUE);

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_PROCESSING);
} /* USBD_IOTP_BUFF_extension_on_write */

static void USBD_IOTP_BUFF_extension_on_write_connect_memcpy_only(
   Buff_Ring_XT *buf, Buff_Ring_Extensions_XT *extension, Buff_Size_DT size, bool_t rewind_occured)
{
   USBD_UNUSED_PARAM(size);
   USBD_UNUSED_PARAM(rewind_occured);

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   USBD_IOTP_BUFF_process_extension_on_write(buf, extension, USBD_FALSE);

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_PROCESSING);
} /* USBD_IOTP_BUFF_extension_on_write_connect_memcpy_only */

static Buff_Size_DT USBD_IOTP_BUFF_vendor_memcpy_in(const Buff_Memcpy_Params_XT *params)
{
   usbd_iotp_buff_vendor_memcpy_XT *vendor_data;
   USBD_IOTP_BUFF_Params_XT        *tp;
   Buff_Size_DT                     result;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   vendor_data = (usbd_iotp_buff_vendor_memcpy_XT*)(params->dest);
   tp          = vendor_data->tp;

   USBD_DEBUG_HI_4(
      USBD_DBG_IOTPBF_PROCESSING,
      "vendor_memcpy_in called with params: src: %p; src_offset: %d; size: %d; is_last_part: %s",
      params->src,
      params->src_offset,
      params->size,
      params->is_last_part ? "true" : "false");

   if(params->is_last_part)
   {
      vendor_data->is_last_part = USBD_TRUE;
   }
   else
   {
      vendor_data->is_last_part = USBD_FALSE;
   }

   result = (Buff_Size_DT)USBD_CALL_HANDLER(USBD_IO_IN_Data_Method_Port_HT, vendor_data->data_method.in)(
      USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
      USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
      USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp),
      &(((uint8_t*)(params->src))[params->src_offset]),
      params->size,
      vendor_data->is_last_part);

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   return result;
} /* USBD_IOTP_BUFF_vendor_memcpy_in */

static void USBD_IOTP_BUFF_io_data_memcpy_in(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_IN_Data_Method_Port_HT data_method)
{
   Buff_Ring_Extensions_XT        *extension;
   USBD_IOTP_BUFF_Params_XT       *tp          = (USBD_IOTP_BUFF_Params_XT*)tp_params;
   usbd_iotp_buff_vendor_memcpy_XT vendor_data;
#if((USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK) || (USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_SUPPORT_CONTROL_TRANSFER))
   Buff_Size_DT                    size        = 0;
#endif
   USBD_Bool_DT                    port_memcpy_called = USBD_FALSE;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction)
      && USBD_CHECK_HANDLER(USBD_IO_IN_Data_Method_Port_HT, data_method) && BUFF_CHECK_PTR(Buff_Ring_XT, tp->core.buff))
   {
      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPBF_PROCESSING,
         "memcpy_in  before::ep: %d::%s, dir: %s; busy_size = %d",
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_IN) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         BUFF_RING_GET_BUSY_SIZE(tp->core.buff));

      if(!BUFF_RING_IS_EMPTY(tp->core.buff))
      {
         port_memcpy_called         = USBD_TRUE;
         vendor_data.tp             = tp;
         vendor_data.data_method.in = data_method;
         vendor_data.is_last_part   = USBD_TRUE;

#if((USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK) || (USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_SUPPORT_CONTROL_TRANSFER))
         size = Buff_Ring_Read_Vendor(
            tp->core.buff,
            &vendor_data,
            (Buff_Size_DT)(tp->core.pipe_params.data.mps + tp->core.pipe_params.data.mps),
            USBD_IOTP_BUFF_vendor_memcpy_in,
            BUFF_TRUE);
#else
         (void)Buff_Ring_Read_Vendor(
            tp->core.buff,
            &vendor_data,
            (Buff_Size_DT)(tp->core.pipe_params.data.mps + tp->core.pipe_params.data.mps),
            USBD_IOTP_BUFF_vendor_memcpy_in,
            BUFF_TRUE);
#endif
      }
#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_SUPPORT_CONTROL_TRANSFER)
      else if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == tp->core.pipe_params.data.ep_type)
      {
         port_memcpy_called         = USBD_TRUE;
         (void)USBD_CALL_HANDLER(USBD_IO_IN_Data_Method_Port_HT, data_method)(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp),
            &vendor_data,
            0,
            USBD_TRUE);
      }
#endif

      USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));

      extension = tp->core.buff->extension;

      if(BUFF_CHECK_PTR(Buff_Ring_Extensions_XT, extension))
      {
         tp->owner                  = USBD_IOTP_BUFF_dummy_data;
         extension->on_write_params = tp;
         extension->on_write        = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Write);
      }

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)

      tp->up_link.data.in_size_transferred += size;

      if( (0 == BUFF_RING_GET_BUSY_SIZE(tp->core.buff))
#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_SUPPORT_CONTROL_TRANSFER)
         &&
         (
            (USB_EP_DESC_TRANSFER_TYPE_CONTROL != tp->core.pipe_params.data.ep_type)
            || (tp->core.pipe_params.data.mps != size) && ((tp->core.pipe_params.data.mps + tp->core.pipe_params.data.mps) != size)
         )
#endif
      )
      {
         /** call ready handler */
         if(USBD_IOTP_BUFF_CHECK_READY_HANDLER(tp) && USBD_BOOL_IS_FALSE(tp->req_in_progress))
         {
            USBD_IOTP_BUFF_CALL_READY(
               USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
               USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp),
               tp,
               tp->core.pipe_params.data.mps);
         }
      }

#endif

      if(BUFF_RING_GET_BUSY_SIZE(tp->core.buff) > 0)
      {
         USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_IOTP_BUFF_io_data_memcpy_in);
      }
#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_SUPPORT_CONTROL_TRANSFER)
      else if((USB_EP_DESC_TRANSFER_TYPE_CONTROL == tp->core.pipe_params.data.ep_type)
         &&
         (
            (tp->core.pipe_params.data.mps == size) || ((tp->core.pipe_params.data.mps + tp->core.pipe_params.data.mps) == size)
         )
      )
      {
         USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_IOTP_BUFF_io_data_memcpy_in);
      }
#endif
      else
      {
         USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));

         extension = tp->core.buff->extension;

         if(BUFF_CHECK_PTR(Buff_Ring_Extensions_XT, extension))
         {
            tp->owner                  = USBD_IOTP_BUFF_dummy_data;
            extension->on_write_params = tp;
            if(USBD_BOOL_IS_TRUE(port_memcpy_called))
            {
               extension->on_write        = USBD_IOTP_BUFF_extension_on_write_connect_memcpy_only;
            }
            else
            {
               extension->on_write        = USBD_IOTP_BUFF_extension_on_write;
            }
         }
      }

      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPBF_PROCESSING,
         "memcpy_in  after ::ep: %d::%s, dir: %s; busy_size = %d",
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_IN) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         BUFF_RING_GET_BUSY_SIZE(tp->core.buff));
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_PROCESSING);
} /* USBD_IOTP_BUFF_io_data_memcpy_in */

static void USBD_IOTP_BUFF_io_evdata_in(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_IN_Data_Method_Port_HT mem_cpy)
{
   Buff_Ring_Extensions_XT        *extension;
   USBD_IOTP_BUFF_Params_XT       *tp          = (USBD_IOTP_BUFF_Params_XT*)tp_params;
   usbd_iotp_buff_vendor_memcpy_XT vendor_data;
#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
   Buff_Size_DT                    ret_size    = 0;
#else
   USBD_UNUSED_PARAM(size);
#endif

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction)
      && USBD_CHECK_HANDLER(USBD_IO_IN_Data_Method_Port_HT, mem_cpy) && BUFF_CHECK_PTR(Buff_Ring_XT, tp->core.buff))
   {
      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPBF_PROCESSING,
         "evdata_in  before::ep: %d::%s, dir: %s; busy_size = %d",
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_IN) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         BUFF_RING_GET_BUSY_SIZE(tp->core.buff));

      if(!BUFF_RING_IS_EMPTY(tp->core.buff))
      {
         vendor_data.tp             = tp;
         vendor_data.data_method.in = mem_cpy;
         vendor_data.is_last_part   = USBD_TRUE;

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
         ret_size = Buff_Ring_Read_Vendor(
            tp->core.buff,
            &vendor_data,
            (Buff_Size_DT)(tp->core.pipe_params.data.mps + tp->core.pipe_params.data.mps),
            USBD_IOTP_BUFF_vendor_memcpy_in,
            BUFF_TRUE);
#else
         (void)Buff_Ring_Read_Vendor(
            tp->core.buff,
            &vendor_data,
            (Buff_Size_DT)(tp->core.pipe_params.data.mps + tp->core.pipe_params.data.mps),
            USBD_IOTP_BUFF_vendor_memcpy_in,
            BUFF_TRUE);
#endif
      }

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)

      tp->up_link.data.in_size_transferred += ret_size;

      if((size < 0) && (0 == ret_size))
      {
         /** informs upper layer that IN buffer is empty */
         if(USBD_IOTP_BUFF_CHECK_BUF_EMPTY_HANDLER(tp) && USBD_BOOL_IS_FALSE(tp->req_in_progress))
         {
            USBD_IOTP_BUFF_CALL_BUF_EMPTY(
               USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
               USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp),
               tp,
               tp->core.pipe_params.data.mps);
         }
      }
#endif

      USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));

      if(BUFF_RING_GET_BUSY_SIZE(tp->core.buff) > 0)
      {
         USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_IOTP_BUFF_io_data_memcpy_in);

         extension = tp->core.buff->extension;

         if(BUFF_CHECK_PTR(Buff_Ring_Extensions_XT, extension))
         {
            tp->owner                  = USBD_IOTP_BUFF_dummy_data;
            extension->on_write_params = tp;
            extension->on_write        = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Write);
         }
      }
      else
      {
         USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));

         extension = tp->core.buff->extension;

         if(BUFF_CHECK_PTR(Buff_Ring_Extensions_XT, extension))
         {
            tp->owner                  = USBD_IOTP_BUFF_dummy_data;
            extension->on_write_params = tp;

            if(USBD_IO_UP_EP_IN_Get_Buffered_Data_Size(
               USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
               USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp)) < 0)
            {
               extension->on_write     = USBD_IOTP_BUFF_extension_on_write;
            }
            else
            {
               extension->on_write     = USBD_IOTP_BUFF_extension_on_write_connect_memcpy_only;
            }
         }
      }

      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPBF_PROCESSING,
         "evdata_in  after ::ep: %d::%s, dir: %s; busy_size = %d",
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_IN) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         BUFF_RING_GET_BUSY_SIZE(tp->core.buff));
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_PROCESSING);
} /* USBD_IOTP_BUFF_io_evdata_in */



static void USBD_IOTP_BUFF_extension_on_read_remove(Buff_Ring_XT *buf, Buff_Ring_Extensions_XT *extension)
{
   USBD_IOTP_BUFF_Params_XT *tp;
   USBD_Params_XT *usbd;
   uint8_t ep_num;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   tp = (USBD_IOTP_BUFF_Params_XT *)(extension->on_read_params);

   if(!BUFF_RING_IS_FULL(buf))
   {
      if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp)
         && USBD_COMPARE_PTRS(void, tp->owner, void, USBD_IOTP_BUFF_dummy_data)
         && BUFF_COMPARE_PTRS(Buff_Ring_XT, buf, Buff_Ring_XT, tp->core.buff) )
      {
         usbd   = USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp);
         ep_num = USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp);

         if(USBD_COMPARE_PTRS(USBD_IOTP_BUFF_Params_XT, USBD_IO_UP_Get_OUT_TP_Params(usbd, ep_num), USBD_IOTP_BUFF_Params_XT, tp))
         {
            if(!USBD_INVOKE(
               USBD_IOTP_BUFF_GET_INVOKE_PARAMS(tp), USBD_IOTP_BUFF_trigger_out(tp, extension, usbd, ep_num)))
            {
               /* action not yet executed */
            }
         }
         else
         {
            USBD_ERROR(USBD_DBG_IOTPBF_ONOFF, "wrong tp!");
         }
      }
      else
      {
         USBD_ERROR(USBD_DBG_IOTPBF_ONOFF, "wrong tp owner!");
      }
   }
   else
   {
      USBD_ERROR(USBD_DBG_IOTPBF_ONOFF, "buff ring is full but extension has been called - ignore it!");
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_PROCESSING);
} /* USBD_IOTP_BUFF_extension_on_read_remove */

static void USBD_IOTP_BUFF_extension_on_read(
   Buff_Ring_XT *buf, Buff_Ring_Extensions_XT *extension, Buff_Size_DT size, bool_t rewind_occured)
{
   USBD_UNUSED_PARAM(size);
   USBD_UNUSED_PARAM(rewind_occured);

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   USBD_IOTP_BUFF_extension_on_read_remove(buf, extension);

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_PROCESSING);
} /* USBD_IOTP_BUFF_extension_on_read */

static void USBD_IOTP_BUFF_extension_on_remove(Buff_Ring_XT *buf, Buff_Ring_Extensions_XT *extension, bool_t rewind_occured)
{
   USBD_UNUSED_PARAM(rewind_occured);

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   USBD_IOTP_BUFF_extension_on_read_remove(buf, extension);

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_PROCESSING);
} /* USBD_IOTP_BUFF_extension_on_remove */

static Buff_Size_DT USBD_IOTP_BUFF_vendor_memcpy_out(const Buff_Memcpy_Params_XT *params)
{
   usbd_iotp_buff_vendor_memcpy_XT *vendor_data;
   Buff_Size_DT result = 0;
   USBD_Bool_DT is_last_part;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   vendor_data = (usbd_iotp_buff_vendor_memcpy_XT*)(params->src);

   USBD_DEBUG_HI_4(
      USBD_DBG_IOTPBF_PROCESSING,
      "vendor_memcpy_out called with params: dest: %p; dest_offset: %d; size: %d; is_last_part: %s",
      params->dest,
      params->dest_offset,
      params->size,
      params->is_last_part ? "true" : "false");

   if(params->is_last_part)
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
         USBD_IOTP_BUFF_GET_USBD_FROM_TP(vendor_data->tp),
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(vendor_data->tp),
         USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(vendor_data->tp),
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
      USBD_ERROR(USBD_DBG_IOTPBF_ONOFF, "vendor_memcpy_out called when no data is waiting - ignore it!");
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   return result;
} /* USBD_IOTP_BUFF_vendor_memcpy_out */

static void USBD_IOTP_BUFF_io_memcpy_out(
   void* tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT packet_size,
   USBD_IO_Inout_Data_Size_DT left_size,
   USBD_IO_OUT_Data_Method_Port_HT data_method)
{
   Buff_Ring_Extensions_XT        *extension;
   USBD_IOTP_BUFF_Params_XT       *tp          = (USBD_IOTP_BUFF_Params_XT*)tp_params;
   usbd_iotp_buff_vendor_memcpy_XT vendor_data;
   Buff_Size_DT                    ret_size    = 0;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction)
      && USBD_CHECK_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, data_method) && BUFF_CHECK_PTR(Buff_Ring_XT, tp->core.buff))
   {
      USBD_DEBUG_HI_7(
         USBD_DBG_IOTPBF_PROCESSING,
         "memcpy_out before::ep: %d::%s, dir: %s; buff:: size: %d; busy_size: %d; first_free: %d; first_busy: %d",
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         tp->core.buff->size,
         tp->core.buff->busy_size,
         tp->core.buff->first_free,
         tp->core.buff->first_busy);

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
      tp->up_link.data.out_transfer_active = USBD_TRUE;
#endif

      vendor_data.tp                   = tp;
      vendor_data.data_method.out      = data_method;
      vendor_data.out_left_data_size   = left_size;

      if(0 != packet_size)
      {
         ret_size = Buff_Ring_Write_Vendor(
            tp->core.buff, &vendor_data, left_size, USBD_IOTP_BUFF_vendor_memcpy_out, BUFF_FALSE, BUFF_TRUE);

         if(0 == ret_size)
         {
            /**
             * nothing has been written -
             * perform data reading with size 0 only to block endpoint on reading and wait for more space in the buffer
             */
               (void)USBD_CALL_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, data_method)(
                  USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
                  USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
                  USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp),
                  &left_size,
                  0,
                  USBD_FALSE);
         }
      }
      else
      {
         /* zero length packet - release EP buffer */
         (void)USBD_CALL_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, data_method)(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp),
            &left_size,
            0,
            USBD_TRUE);

         vendor_data.out_left_data_size = (-1);
      }

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)

      if(BUFF_RING_IS_FULL(tp->core.buff) || (packet_size < tp->core.pipe_params.data.mps))
      {
         tp->up_link.data.out_transfer_active = USBD_FALSE;
         extension = tp->core.buff->extension;

         if(USBD_IOTP_BUFF_CHECK_READY_HANDLER(tp) && BUFF_CHECK_PTR(Buff_Ring_Extensions_XT, extension))
         {
            tp->out_memcpy                = data_method;
            tp->owner                     = USBD_IOTP_BUFF_dummy_data;
            extension->on_read_params     = tp;
            extension->on_remove_params   = tp;

            if(vendor_data.out_left_data_size >= 0)
            {
               USBD_DEBUG_MID(USBD_DBG_IOTPBF_PROCESSING, "add OUT extensions");

               extension->on_read         = USBD_IOTP_BUFF_extension_on_read;
               extension->on_remove       = USBD_IOTP_BUFF_extension_on_remove;
            }
            else
            {
               USBD_DEBUG_MID(USBD_DBG_IOTPBF_PROCESSING, "remove OUT extensions");

               extension->on_read         = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Read);
               extension->on_remove       = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Remove);
            }

            USBD_IOTP_BUFF_CALL_READY(
               USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
               USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp),
               tp,
               vendor_data.out_left_data_size);

            if((!BUFF_RING_IS_FULL(tp->core.buff)) && (packet_size >= tp->core.pipe_params.data.mps))
            {
               tp->up_link.data.out_transfer_active = USBD_TRUE;
            }

            tp->out_memcpy                = USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_Port_HT);
         }
      }

#endif

      extension = tp->core.buff->extension;

      if(BUFF_CHECK_PTR(Buff_Ring_Extensions_XT, extension))
      {
         tp->owner                     = USBD_IOTP_BUFF_dummy_data;
         extension->on_read_params     = tp;
         extension->on_remove_params   = tp;

         if(BUFF_RING_IS_FULL(tp->core.buff))
         {
            USBD_DEBUG_MID(USBD_DBG_IOTPBF_PROCESSING, "add OUT extensions");

            extension->on_read            = USBD_IOTP_BUFF_extension_on_read;
            extension->on_remove          = USBD_IOTP_BUFF_extension_on_remove;
         }
         else
         {
            USBD_DEBUG_MID(USBD_DBG_IOTPBF_PROCESSING, "remove OUT extensions");

            extension->on_read            = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Read);
            extension->on_remove          = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Remove);
         }
      }

      USBD_DEBUG_HI_7(
         USBD_DBG_IOTPBF_PROCESSING,
         "memcpy_out after ::ep: %d::%s, dir: %s; buff:: size: %d; busy_size: %d; first_free: %d; first_busy: %d",
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         tp->core.buff->size,
         tp->core.buff->busy_size,
         tp->core.buff->first_free,
         tp->core.buff->first_busy);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_PROCESSING);
} /* USBD_IOTP_BUFF_io_memcpy_out */

static void USBD_IOTP_BUFF_io_evdata_out(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_OUT_Data_Method_Port_HT mem_cpy)
{
   Buff_Ring_Extensions_XT        *extension;
   USBD_IOTP_BUFF_Params_XT       *tp          = (USBD_IOTP_BUFF_Params_XT*)tp_params;
   usbd_iotp_buff_vendor_memcpy_XT vendor_data;
   Buff_Size_DT                    ret_size    = 0;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction)
      && USBD_CHECK_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, mem_cpy) && BUFF_CHECK_PTR(Buff_Ring_XT, tp->core.buff))
   {
      USBD_DEBUG_HI_7(
         USBD_DBG_IOTPBF_PROCESSING,
         "evdata_out before::ep: %d::%s, dir: %s; buff:: size: %d; busy_size: %d; first_free: %d; first_busy: %d",
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         tp->core.buff->size,
         tp->core.buff->busy_size,
         tp->core.buff->first_free,
         tp->core.buff->first_busy);

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
      tp->up_link.data.out_transfer_active = USBD_TRUE;
#endif

      vendor_data.tp                   = tp;
      vendor_data.data_method.out      = mem_cpy;
      vendor_data.out_left_data_size   = size;

      if(0 != size)
      {
         ret_size = Buff_Ring_Write_Vendor(
            tp->core.buff, &vendor_data, size, USBD_IOTP_BUFF_vendor_memcpy_out, BUFF_FALSE, BUFF_TRUE);

         if(0 == ret_size)
         {
            /**
             * nothing has been written -
             * perform data reading with size 0 only to stuck endpoint on reading and wait for more space in the buffer
             */
            (void)USBD_CALL_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, mem_cpy)(
               USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
               USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp),
               &size,
               0,
               USBD_FALSE);
         }
      }
      else
      {
         /* zero length packet - release EP buffer */
         (void)USBD_CALL_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, mem_cpy)(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp),
            &size,
            0,
            USBD_TRUE);

         vendor_data.out_left_data_size = (-1);
      }

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)

      if(BUFF_RING_IS_FULL(tp->core.buff) || (size < tp->core.pipe_params.data.mps))
      {
         tp->up_link.data.out_transfer_active = USBD_FALSE;
         extension = tp->core.buff->extension;

         if(USBD_IOTP_BUFF_CHECK_READY_HANDLER(tp) && BUFF_CHECK_PTR(Buff_Ring_Extensions_XT, extension))
         {
            tp->out_memcpy                = mem_cpy;
            tp->owner                     = USBD_IOTP_BUFF_dummy_data;
            extension->on_read_params     = tp;
            extension->on_remove_params   = tp;

            if(vendor_data.out_left_data_size >= 0)
            {
               USBD_DEBUG_MID(USBD_DBG_IOTPBF_PROCESSING, "add OUT extensions");

               extension->on_read         = USBD_IOTP_BUFF_extension_on_read;
               extension->on_remove       = USBD_IOTP_BUFF_extension_on_remove;
            }
            else
            {
               USBD_DEBUG_MID(USBD_DBG_IOTPBF_PROCESSING, "remove OUT extensions");

               extension->on_read         = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Read);
               extension->on_remove       = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Remove);
            }

            USBD_IOTP_BUFF_CALL_READY(
               USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
               USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp),
               tp,
               vendor_data.out_left_data_size);

            if((!BUFF_RING_IS_FULL(tp->core.buff)) && (size >= tp->core.pipe_params.data.mps))
            {
               tp->up_link.data.out_transfer_active = USBD_TRUE;
            }

            tp->out_memcpy                = USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_Port_HT);
         }
      }

#endif

      USBD_IO_SET_OUT_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
      USBD_IO_SET_OUT_MEMCPY_HANDLER(transaction,  USBD_IOTP_BUFF_io_memcpy_out);

      extension = tp->core.buff->extension;

      if(BUFF_CHECK_PTR(Buff_Ring_Extensions_XT, extension))
      {
         tp->owner                     = USBD_IOTP_BUFF_dummy_data;
         extension->on_read_params     = tp;
         extension->on_remove_params   = tp;

         if(!BUFF_RING_IS_FULL(tp->core.buff))
         {
            USBD_DEBUG_MID(USBD_DBG_IOTPBF_PROCESSING, "remove OUT extensions");

            extension->on_read            = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Read);
            extension->on_remove          = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Remove);
         }
         else
         {
            USBD_DEBUG_MID(USBD_DBG_IOTPBF_PROCESSING, "add OUT extensions");

            extension->on_read            = USBD_IOTP_BUFF_extension_on_read;
            extension->on_remove          = USBD_IOTP_BUFF_extension_on_remove;
         }
      }

      USBD_DEBUG_HI_7(
         USBD_DBG_IOTPBF_PROCESSING,
         "evdata_out after ::ep: %d::%s, dir: %s; buff:: size: %d; busy_size: %d; first_free: %d; first_busy: %d",
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         tp->core.buff->size,
         tp->core.buff->busy_size,
         tp->core.buff->first_free,
         tp->core.buff->first_busy);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_PROCESSING);
} /* USBD_IOTP_BUFF_io_evdata_out */



static void USBD_IOTP_BUFF_clear_buff(
   USBD_IOTP_BUFF_Params_XT *tp, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction, USBD_Bool_DT set_valid_handlers)
{
   Buff_Ring_Extensions_XT *extension;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   extension = tp->core.buff->extension;

   if(BUFF_CHECK_PTR(Buff_Ring_Extensions_XT, extension))
   {
      if(USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp))
      {
         USBD_IO_SET_OUT_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
         if(USBD_BOOL_IS_TRUE(set_valid_handlers))
         {
            USBD_IO_SET_OUT_MEMCPY_HANDLER(transaction,  USBD_IOTP_BUFF_io_memcpy_out);
         }
         else
         {
            USBD_IO_SET_OUT_MEMCPY_HANDLER(transaction,  USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_TP_HT));
         }

         extension->on_read         = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Read);
         extension->on_remove       = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Remove);
#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
         tp->up_link.data.out_transfer_active = USBD_FALSE;
#endif
      }
      else
      {
         USBD_IO_SET_IN_PROVIDE_HANDLER(transaction, USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));
         USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_MAKE_INVALID_HANDLER(USBD_IO_IN_Data_Method_TP_HT));

         tp->owner                  = USBD_IOTP_BUFF_dummy_data;
         extension->on_write_params = tp;

         if(USBD_BOOL_IS_FALSE(set_valid_handlers))
         {
            extension->on_write     = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Write);
         }
         else
         {
            extension->on_write     = USBD_IOTP_BUFF_extension_on_write;
         }
      }
   }

   Buff_Ring_Clear(tp->core.buff, BUFF_TRUE);

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_PROCESSING);
}

/**
 * Handler called by USBD when CONTROL, BULK or INTERRUPT transaction error is detected or transfer is stopped.
 * In that situation transfer is aborted automatically by USBD and upper layer is informed.
 */
static void USBD_IOTP_BUFF_io_abort(void *tp_params, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction)
{
   USBD_IOTP_BUFF_Params_XT *tp = (USBD_IOTP_BUFF_Params_XT*)tp_params;
#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
   USBD_Bool_DT              transfer_active;
#endif

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction)
      && BUFF_CHECK_PTR(Buff_Ring_XT, tp->core.buff))
   {
      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPBF_PROCESSING,
         "abort      before::ep: %d::%s, dir: %s; busy_size = %d",
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         BUFF_RING_GET_BUSY_SIZE(tp->core.buff));

      USBD_MARK_INVOKE_DESTINATION(USBD_IOTP_BUFF_GET_INVOKE_PARAMS(tp));

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
      transfer_active = USBD_IOTP_BUFF_Is_Transfer_Active(tp);
#endif

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)

      if( USBD_IOTP_BUFF_CHECK_ABORT_HANDLER(tp) && USBD_BOOL_IS_TRUE(transfer_active) )
      {
         USBD_IOTP_BUFF_CALL_ABORT(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp),
            tp,
            USBD_IOTP_BUFF_Get_Transferred_Size(tp));
      }

#endif

      USBD_IOTP_BUFF_clear_buff(tp, transaction, USBD_TRUE);

      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPBF_PROCESSING,
         "abort      after ::ep: %d::%s, dir: %s; busy_size = %d",
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         BUFF_RING_GET_BUSY_SIZE(tp->core.buff));
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_PROCESSING);
} /* USBD_IOTP_BUFF_io_abort */



/**
 * Handler called by USBD when ISOCHRONOUS transaction error is detected.
 * In that situation transfer is not aborted like for CONTROL/BULK/INTERRUPT
 * but only upper layer is informed about this situation to decide there what shall be done.
 */
#if(USBD_FEATURE_PRESENT == USBD_IO_ISOCHRONOUS_TRANSFER_SUPPORTED)
static void USBD_IOTP_BUFF_io_error(void *tp_params, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction, USBD_IO_Inout_Data_Size_DT size)
{
#if(USBD_FEATURE_PRESENT != USBD_IOTP_BUFF_USE_UP_LINK)
   USBD_UNUSED_PARAM(size);
#endif

   USBD_IOTP_BUFF_Params_XT *tp = (USBD_IOTP_BUFF_Params_XT*)tp_params;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_PROCESSING);

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction)
      && BUFF_CHECK_PTR(Buff_Ring_XT, tp->core.buff))
   {
      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPBF_PROCESSING,
         "error      before::ep: %d::%s, dir: %s; busy_size = %d",
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         BUFF_RING_GET_BUSY_SIZE(tp->core.buff));

      USBD_MARK_INVOKE_DESTINATION(USBD_IOTP_BUFF_GET_INVOKE_PARAMS(tp));

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)

      if(USBD_IOTP_BUFF_CHECK_ERROR_HANDLER(tp))
      {
         USBD_IOTP_BUFF_CALL_ERROR(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp),
            tp,
            size);
      }

#endif

      USBD_IOTP_BUFF_clear_buff(tp, transaction, USBD_TRUE);

      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPBF_PROCESSING,
         "error      after ::ep: %d::%s, dir: %s; busy_size = %d",
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         BUFF_RING_GET_BUSY_SIZE(tp->core.buff));
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_PROCESSING);
} /* USBD_IOTP_BUFF_io_error */
#endif



static void USBD_IOTP_BUFF_io_reinit(
   void *tp_params, void *tp_owner, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction, USBD_Bool_DT active)
{
   USBD_IOTP_BUFF_Params_XT     *tp;
   const USB_Endpoint_Desc_DT   *ep_desc;
   uint16_t                      mps = 1;
   uint8_t                       ep_type;
#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
   USBD_Bool_DT                  transfer_active;
#endif

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_ONOFF);

   tp = (USBD_IOTP_BUFF_Params_XT*)tp_params;

   if(USBD_CHECK_PTR(USBD_IOTP_BUFF_Params_XT, tp) && USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction)
      && BUFF_CHECK_PTR(Buff_Ring_XT, tp->core.buff) && USBD_COMPARE_PTRS(void, tp_owner, void, USBD_IOTP_BUFF_dummy_data))
   {
      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPBF_PROCESSING,
         "reinit     before::ep: %d::%s, dir: %s; busy_size = %d",
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         BUFF_RING_GET_BUSY_SIZE(tp->core.buff));

      if(USBD_BOOL_IS_FALSE(active))
      {
#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
         transfer_active = USBD_IOTP_BUFF_Is_Transfer_Active(tp);
#endif

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
         if( USBD_IOTP_BUFF_CHECK_ABORT_HANDLER(tp) && USBD_BOOL_IS_TRUE(transfer_active) )
         {
            USBD_IOTP_BUFF_CALL_ABORT(
               USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
               USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
               USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp),
               tp,
               USBD_IOTP_BUFF_Get_Transferred_Size(tp));
         }
#endif

         USBD_IOTP_BUFF_clear_buff(tp, transaction, USBD_FALSE);

         USBD_IO_UP_SET_ERROR_HANDLER(&(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT));
         USBD_IO_UP_SET_SFIN_HANDLER (&(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Stall_Fin_HT));
         USBD_IO_UP_SET_ABORT_HANDLER(&(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Abort_HT));

         if(USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp))
         {
            USBD_IO_UP_SET_OUT_DATA_EVENT_HANDLER(
               &(tp->core.pipe_params.handlers), USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_OUT_Data_Event_HT));
         }
         else
         {
            USBD_IO_UP_SET_IN_DATA_EVENT_HANDLER(
               &(tp->core.pipe_params.handlers), USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_IN_Data_Event_HT));
         }
      }
      else
      {
         ep_desc = USBD_DEV_Get_EP_Desc(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp));

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

         if(USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp))
         {
            USBD_IO_UP_SET_OUT_DATA_EVENT_HANDLER(&(tp->core.pipe_params.handlers), USBD_IOTP_BUFF_io_evdata_out);
         }
         else
         {
            USBD_IO_UP_SET_IN_DATA_EVENT_HANDLER(&(tp->core.pipe_params.handlers), USBD_IOTP_BUFF_io_evdata_in);
         }

         USBD_IO_UP_SET_ERROR_HANDLER(&(tp->core.pipe_params.handlers),USBD_IOTP_BUFF_error_table[ep_type]);
         USBD_IO_UP_SET_SFIN_HANDLER (&(tp->core.pipe_params.handlers),USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Stall_Fin_HT));
         USBD_IO_UP_SET_ABORT_HANDLER(&(tp->core.pipe_params.handlers),USBD_IOTP_BUFF_io_abort);

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
         tp->up_link.data.in_size_transferred   = 0;
         tp->req_in_progress                    = USBD_FALSE;
#endif
         USBD_IOTP_BUFF_clear_buff(tp, transaction, USBD_TRUE);

         USBD_DEBUG_HI_2(
            USBD_DBG_IOTPBF_PROCESSING,
            "invoke for ep: %d, dir: %s",
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp) ? "out" : "in");

         USBD_MARK_INVOKE_DESTINATION(USBD_IOTP_BUFF_GET_INVOKE_PARAMS(tp));
      }

      USBD_DEBUG_HI_4(
         USBD_DBG_IOTPBF_PROCESSING,
         "reinit     after ::ep: %d::%s, dir: %s; busy_size = %d",
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USBD_IO_UP_Is_EP_Active(
            USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
            USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
            USB_EP_DIRECTION_OUT) ? "active" : "passive",
         USB_EP_DESC_DIR_OUT == USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp) ? "out" : "in",
         BUFF_RING_GET_BUSY_SIZE(tp->core.buff));
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_ONOFF);
} /* USBD_IOTP_BUFF_io_reinit */



static USBD_Bool_DT USBD_IOTP_BUFF_trigger_in(
   USBD_IOTP_BUFF_Params_XT *tp, Buff_Ring_Extensions_XT *extension, USBD_Params_XT *usbd, uint8_t ep_num, USBD_Bool_DT call_trigger)
{
#if(USBD_FEATURE_PRESENT != USBD_IOTP_BUFF_USE_UP_LINK)
   USBD_UNUSED_PARAM(tp);
#endif

   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction;

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_EVENT);

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
   tp->up_link.data.in_size_transferred   = 0;
   tp->req_in_progress                    = USBD_TRUE;
#endif

   if(USBD_CHECK_PTR(Buff_Ring_Extensions_XT, extension))
   {
      extension->on_write = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Write);
   }

   transaction = USBD_IO_Get_IN_Transaction_Params(usbd, ep_num);

   if(USBD_CHECK_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT, transaction))
   {
      USBD_IO_SET_IN_MEMCPY_HANDLER(transaction,  USBD_IOTP_BUFF_io_data_memcpy_in);
   }
   if(USBD_BOOL_IS_TRUE(call_trigger))
   {
      (void)USBD_IO_UP_Trigger_IN(usbd, ep_num);
   }

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
   tp->req_in_progress                 = USBD_FALSE;
#endif

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_EVENT);

   return USBD_TRUE;
} /* USBD_IOTP_BUFF_trigger_in */

static USBD_Bool_DT USBD_IOTP_BUFF_trigger_out(
   USBD_IOTP_BUFF_Params_XT *tp, Buff_Ring_Extensions_XT *extension, USBD_Params_XT *usbd, uint8_t ep_num)
{
#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
   usbd_iotp_buff_vendor_memcpy_XT vendor_data;
#endif

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_EVENT);

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
   if(BUFF_CHECK_HANDLER(USBD_IO_OUT_Data_Method_Port_HT, tp->out_memcpy))
   {
      vendor_data.tp                      = tp;
      vendor_data.data_method.out         = tp->out_memcpy;
      vendor_data.out_left_data_size      = USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(
         USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp),
         USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp),
         USBD_TRUE);

      (void)Buff_Ring_Write_Vendor(
         tp->core.buff,
         &vendor_data,
         vendor_data.out_left_data_size,
         USBD_IOTP_BUFF_vendor_memcpy_out,
         BUFF_FALSE,
         BUFF_TRUE);

      if(vendor_data.out_left_data_size < 0)
      {
         extension->on_read   = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Read);
         extension->on_remove = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Remove);
      }
   }
   else
#endif
   {
      USBD_IO_UP_Trigger_OUT(usbd, ep_num, USBD_TRUE);

      if((!BUFF_RING_IS_FULL(tp->core.buff)) && ((-1) == USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(usbd, ep_num, USBD_FALSE)))
      {
         extension->on_read   = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Read);
         extension->on_remove = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Remove);
      }
   }


   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_EVENT);

   return USBD_TRUE;
} /* USBD_IOTP_BUFF_trigger_out */



#ifdef USBD_EVENT_PRESENT
static void USBD_IOTP_BUFF_proc_event(USBD_IOTP_BUFF_Params_XT *tp, uint8_t req)
{
   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_EVENT);

   USBD_ATOMIC_UINT8_SET(tp->invoke.req, USBD_IOTP_BUFF_INVOKE_REQ_NONE);

   if(USBD_IOTP_BUFF_INVOKE_REQ_ABORT_NO_FLUSH_HW == req)
   {
      (void)USBD_IOTP_BUFF_proc_abort(tp, USBD_FALSE);
   }
   else if(USBD_IOTP_BUFF_INVOKE_REQ_ABORT_FLUSH_HW == req)
   {
      (void)USBD_IOTP_BUFF_proc_abort(tp, USBD_TRUE);
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_EVENT);
} /* USBD_IOTP_proc_event */

static void USBD_IOTP_BUFF_event(
   USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_IOTP_BUFF_Params_XT *tp;
   uint8_t ep_num;
   uint8_t req;

   USBD_UNUSED_PARAM(usbdc);
   USBD_UNUSED_PARAM(event_params);
   USBD_UNUSED_PARAM(reason);

   USBD_ENTER_FUNC(USBD_DBG_IOTPBF_EVENT);

   for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
   {
      /* OUT TP */
      if(USBD_COMPARE_PTRS(void, USBD_IO_UP_Get_OUT_TP_Owner(usbd, ep_num), void, USBD_IOTP_BUFF_dummy_data))
      {
         tp = USBD_IO_UP_Get_OUT_TP_Params(usbd, ep_num);
         req = USBD_ATOMIC_UINT8_GET(tp->invoke.req);

         if(USBD_IOTP_BUFF_INVOKE_REQ_NONE != req)
         {
            USBD_IOTP_BUFF_proc_event(tp, req);
         }

         if(!BUFF_RING_IS_FULL(tp->core.buff) && (USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(usbd, ep_num, USBD_FALSE) > 0))
         {
            USBD_IOTP_BUFF_trigger_out(tp, tp->core.buff->extension, usbd, ep_num);
         }
      }
      /* IN TP */
      if(USBD_COMPARE_PTRS(void, USBD_IO_UP_Get_IN_TP_Owner(usbd, ep_num), void, USBD_IOTP_BUFF_dummy_data))
      {
         tp = USBD_IO_UP_Get_IN_TP_Params(usbd, ep_num);
         req = USBD_ATOMIC_UINT8_GET(tp->invoke.req);

         if(USBD_IOTP_BUFF_INVOKE_REQ_NONE != req)
         {
            USBD_IOTP_BUFF_proc_event(tp, req);
         }

         if(!BUFF_RING_IS_EMPTY(tp->core.buff) && (USBD_IO_UP_EP_IN_Get_Buffered_Data_Size(usbd, ep_num) < 0))
         {
            USBD_IOTP_BUFF_trigger_in(tp, tp->core.buff->extension, usbd, ep_num, USBD_TRUE);
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_IOTPBF_EVENT);
} /* USBD_IOTP_BUFF_event */
#endif



#ifdef USBD_IOTP_BUFF_POST_IMP_INCLUDE
#include "usbd_iotp_event_post_imp.h"
#endif

