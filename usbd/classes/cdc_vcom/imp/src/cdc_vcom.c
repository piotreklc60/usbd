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

#ifndef CDC_VCOM_C_
#define CDC_VCOM_C_
#endif

#ifndef CDC_VCOM_H_
#include "cdc_vcom.h"
#endif

#ifndef CDC_VCOM_INTERNAL_H_
#include "cdc_vcom_internal.h"
#endif

#define CDC_VCOM_DESC_SLAVE_IF_NUM_OFFSET          13

#define CDC_VCOM_COMM_FEATURE_ABSTRACT_STATE       0x01
#define CDC_VCOM_COMM_FEATURE_COUNTRY_SETTING      0x02

#define CDC_VCOM_LINE_STATE_DTR_MASK               0x01
#define CDC_VCOM_LINE_STATE_RTS_MASK               0x02

#define CDC_VCOM_SERIAL_STATE_NOTIFICATION_RING    0x08
#define CDC_VCOM_SERIAL_STATE_NOTIFICATION_DSR     0x02
#define CDC_VCOM_SERIAL_STATE_NOTIFICATION_DCD     0x01

#ifdef USBD_USE_IOCMD
static const char* const cdc_vcom_set_line_coding_stop_bits[] =
{
   "1 stop bit",
   "1.5 stop bits",
   "2 stop bits"
};

static const char* const cdc_vcom_set_line_coding_parity[] =
{
   "None",
   "Odd",
   "Even",
   "Mark",
   "Space"
};
#endif

typedef void (*CDC_VCOM_check_params_execute) (CDC_VCOM_Params_XT *cdc_vcom, USBD_IOTP_EVENT_Params_XT *tp_in);

static void CDC_VCOM_check_params_and_execute(
   USBD_IOTP_EVENT_Params_XT *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size, CDC_VCOM_check_params_execute execute)
{
   USBD_Vendor_Data_XT *tp_vendor_data;
   USBDC_Interface_Header_XT *if_params;
   CDC_VCOM_Params_XT *cdc_vcom;
   USBD_Params_XT *usbd;
   USBD_IOTP_EVENT_Params_XT *tp_in;

   USBD_UNUSED_PARAM(dir);
   USBD_UNUSED_PARAM(size);

   USBD_ENTER_FUNC(CDC_VCOM_REQ);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      tp_vendor_data = USBD_IOTP_EVENT_Get_Vendor_Data_Container(tp);
      usbd = USBD_IOTP_EVENT_Get_USBD(tp);

      if(USBD_CHECK_PTR(USBD_Vendor_Data_XT, tp_vendor_data) && USBD_CHECK_PTR(void, usbd))
      {
         if_params   = (USBDC_Interface_Header_XT*)(tp_vendor_data->pvoid);
         tp_in       = (USBD_IOTP_EVENT_Params_XT*)USBD_IO_UP_Get_IN_TP_Params(usbd, 0);

         if(USBD_CHECK_PTR(USBDC_Interface_Header_XT, if_params) && USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp_in))
         {
            cdc_vcom = (CDC_VCOM_Params_XT*)(if_params->vendor.pvoid);

            if(USBD_CHECK_PTR(CDC_VCOM_Params_XT, cdc_vcom))
            {
               execute(cdc_vcom, tp_in);
            }
            else
            {
               USBD_WARN_2(CDC_VCOM_INVALID_PARAMS, "wrong %s (%p)", "cdc_vcom", cdc_vcom);
            }
         }
         else
         {
            USBD_WARN_4(CDC_VCOM_INVALID_PARAMS, "wrong %s (%p) or %s (%p)", "if_params", if_params, "tp_in", tp_in);
         }
      }
      else
      {
         USBD_WARN_4(CDC_VCOM_INVALID_PARAMS, "wrong %s (%p) or %s (%p)", "tp_vendor_data", tp_vendor_data, "usbd", usbd);
      }
   }
   else
   {
      USBD_WARN_2(CDC_VCOM_INVALID_PARAMS, "wrong %s (%p)", "tp", tp);
   }
} /* CDC_VCOM_check_params_and_execute */

#if(CDC_VCOM_MODE_DATA_AND_SIGNALS == CDC_VCOM_MODE)

static void CDC_VCOM_send_notif_done_action(CDC_VCOM_Params_XT *cdc_vcom, USBD_IOTP_EVENT_Params_XT *tp_in)
{
   USBD_ENTER_FUNC(CDC_VCOM_REQ);

   USBD_UNUSED_PARAM(tp_in);

   cdc_vcom->data.serial_state_notification.send_ongoing = USBD_FALSE;

   USBD_EXIT_FUNC(CDC_VCOM_REQ);
} /* CDC_VCOM_send_notif_done_action */

static void CDC_VCOM_send_notif_done(USBD_IOTP_EVENT_Params_XT *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   CDC_VCOM_check_params_and_execute(tp, dir, size, CDC_VCOM_send_notif_done_action);
} /* CDC_VCOM_send_notif_done */

#endif

void CDC_VCOM_on_ready_params_change_action(CDC_VCOM_Params_XT *cdc_vcom, USBD_IOTP_EVENT_Params_XT *tp_in)
{
   USBD_ENTER_FUNC(CDC_VCOM_REQ);

   (void)USBD_IOTP_EVENT_Send_Status(tp_in, USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));

   USBD_DEBUG_MID_4(CDC_VCOM_ONOFF, "%s.LINE_CODING.%-9s:  %d - %s",
      cdc_vcom->data.name, "baudrate", cdc_vcom->data.comm_physical_params.baudrate, "");
   USBD_DEBUG_MID_4(CDC_VCOM_ONOFF, "%s.LINE_CODING.%-9s:  %d - %s",
      cdc_vcom->data.name,
      "stop_bits",
      cdc_vcom->data.comm_physical_params.stop_bits,
      cdc_vcom->data.comm_physical_params.stop_bits < Num_Elems(cdc_vcom_set_line_coding_stop_bits) ?
         cdc_vcom_set_line_coding_stop_bits[cdc_vcom->data.comm_physical_params.stop_bits] : "Unknown");
   USBD_DEBUG_MID_4(CDC_VCOM_ONOFF, "%s.LINE_CODING.%-9s:  %d - %s",
      cdc_vcom->data.name,
      "partity",
      cdc_vcom->data.comm_physical_params.partity,
      cdc_vcom->data.comm_physical_params.partity < Num_Elems(cdc_vcom_set_line_coding_parity) ?
         cdc_vcom_set_line_coding_parity[cdc_vcom->data.comm_physical_params.partity] : "Unknown");
   USBD_DEBUG_MID_4(CDC_VCOM_ONOFF, "%s.LINE_CODING.%-9s:  %d - %s",
      cdc_vcom->data.name, "data_bits", cdc_vcom->data.comm_physical_params.data_bits, "");

   if(USBD_CHECK_HANDLER(CDC_VCOM_On_Params_Change_HT, cdc_vcom->handlers.change_params))
   {
      USBD_DEBUG_HI(CDC_VCOM_ONOFF, "call LINE_CODING change_params");

      USBD_CALL_HANDLER(CDC_VCOM_On_Params_Change_HT, cdc_vcom->handlers.change_params) (cdc_vcom);
   }

   USBD_EXIT_FUNC(CDC_VCOM_REQ);
} /* CDC_VCOM_on_ready_params_change_action */

static void CDC_VCOM_on_ready_params_change(USBD_IOTP_EVENT_Params_XT *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   CDC_VCOM_check_params_and_execute(tp, dir, size, CDC_VCOM_on_ready_params_change_action);
} /* CDC_VCOM_on_ready_params_change */

static void CDC_VCOM_on_ready_send_status(USBD_IOTP_EVENT_Params_XT *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   USBD_Params_XT *usbd;
   USBD_IOTP_EVENT_Params_XT *tp_in;

   USBD_UNUSED_PARAM(dir);
   USBD_UNUSED_PARAM(size);

   USBD_ENTER_FUNC(CDC_VCOM_REQ);

   if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp))
   {
      usbd = USBD_IOTP_EVENT_Get_USBD(tp);

      USBD_DEBUG_HI(CDC_VCOM_REQ, "on_ready_send_status");

      if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
      {
         tp_in = (USBD_IOTP_EVENT_Params_XT*)USBD_IO_UP_Get_IN_TP_Params(usbd, 0);

         if(USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp_in))
         {
            (void)USBD_IOTP_EVENT_Send_Status(tp_in, USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
         }
         else
         {
            USBD_WARN_2(CDC_VCOM_INVALID_PARAMS, "wrong %s (%p)", "tp_in", tp_in);
         }
      }
      else
      {
         USBD_WARN_2(CDC_VCOM_INVALID_PARAMS, "wrong %s (%p)", "usbd", usbd);
      }
   }

   USBD_EXIT_FUNC(CDC_VCOM_REQ);
} /* cdc_vcom_on_ready_send_status */

static USBD_Bool_DT CDC_VCOM_on_request(
   USBDC_Params_XT *usbdc,
   uint8_t ep_num,
   uint8_t if_num,
   USBD_REQUEST_Req_DT *req,
   USBD_IOTP_EVENT_Params_XT *tp_in,
   USBD_IOTP_EVENT_Params_XT *tp_out)
{
   USBD_Vendor_Data_XT *tp_vendor_data;
   USBDC_Interface_Header_XT *if_params;
   CDC_VCOM_Params_XT *cdc_vcom;
   uint16_t *ptr16;
   USBD_IO_Inout_Data_Size_DT size;
   uint16_t request;
#if(CDC_VCOM_MODE_DATA_AND_SIGNALS == CDC_VCOM_MODE)
   USBD_Bool_DT previous_dtr;
   USBD_Bool_DT previous_rts;
   USBD_Bool_DT dtr;
   USBD_Bool_DT rts;
#endif
   USBD_Bool_DT result = USBD_FALSE;

   USBD_ENTER_FUNC(CDC_VCOM_REQ);

   do
   {
      if(!(
         USBD_CHECK_PTR(USBDC_Params_XT, usbdc)
         && USBD_CHECK_PTR(USBD_REQUEST_Req_DT, req)
         && USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp_in)
         && USBD_CHECK_PTR(USBD_IOTP_EVENT_Params_XT, tp_out)
         && (0 == ep_num)))
      {
         USBD_ERROR_5(CDC_VCOM_INVALID_PARAMS, "wrong call params! usbdc: %p; req: %p; tp_in: %p; tp_out: %p; ep_num: %d", usbdc, req, tp_in, tp_out, ep_num);
         break;
      }

      if_params = USBDC_Get_Interface_Params(usbdc, if_num);

      if(!USBD_CHECK_PTR(USBDC_Interface_Header_XT, if_params))
      {
         USBD_WARN_2(CDC_VCOM_INVALID_PARAMS, "wrong %s (%p)", "if_params", if_params);
         break;
      }
      cdc_vcom = (CDC_VCOM_Params_XT*)(if_params->vendor.pvoid);

      if(!USBD_CHECK_PTR(CDC_VCOM_Params_XT, cdc_vcom))
      {
         USBD_WARN_2(CDC_VCOM_INVALID_PARAMS, "wrong %s (%p)", "cdc_vcom", cdc_vcom);
         break;
      }

      if(USBD_BMREQUESTTYPE_CLASS != (req->bmRequestType & USBD_BMREQUESTTYPE_TYPE_MASK))
      {
         USBD_WARN(CDC_VCOM_REQ, "bmRequestType is not class type!");
         break;
      }

      result = USBD_TRUE;

      request = (((uint16_t)(req->bRequest) & 0xFF) * 256) | ((uint16_t)(req->bmRequestType) & 0xFF);

      switch(request)
      {
         /* these 2 requests are not supported because there are never used */
         case CDC_SEND_ENCAPSULATED_COMMAND_INTERFACE :
            USBD_DEBUG_HI_2(CDC_VCOM_REQ, "send %s %s", cdc_vcom->data.name, "ENCAPSULATED_COMMAND");
            break;
         case CDC_GET_ENCAPSULATED_RESPONSE_INTERFACE:
            USBD_DEBUG_HI_2(CDC_VCOM_REQ, "get %s %s", cdc_vcom->data.name, "ENCAPSULATED_COMMAND");
            break;

         case CDC_SET_COMM_FEATURE_INTERFACE:
            ptr16 = USBD_MAKE_INVALID_PTR(uint16_t);

            switch(req->wValue)
            {
               case CDC_VCOM_COMM_FEATURE_ABSTRACT_STATE:
                  /* receive abstract state but not use it */
                  USBD_DEBUG_HI_2(CDC_VCOM_REQ, "set %s %s", cdc_vcom->data.name, "ABSTRACT_STATE");
                  ptr16 = &(cdc_vcom->data.abstract_state);
                  break;

               case CDC_VCOM_COMM_FEATURE_COUNTRY_SETTING:
                  /* receive country setting but not use it */
                  USBD_DEBUG_HI_2(CDC_VCOM_REQ, "set %s %s", cdc_vcom->data.name, "COUNTRY_SETTING");
                  ptr16 = &(cdc_vcom->data.country_setting);
                  break;

               default:
                  result = USBD_FALSE;
                  break;
            }

            if(USBD_CHECK_PTR(uint16_t, ptr16))
            {
               USBD_IOTP_EVENT_Set_Ready_Handler(tp_out, CDC_VCOM_on_ready_send_status);

               if(USBD_BOOL_IS_TRUE(USBD_IOTP_EVENT_Recv_And_Ready(tp_out, ptr16, sizeof(uint16_t), &size)))
               {
                  if(size < 0)
                  {
                     (void)USBD_IOTP_EVENT_Send_Status(tp_in, USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
                  }
               }
            }
            break;

         case CDC_GET_COMM_FEATURE_INTERFACE:
            switch(req->wValue)
            {
               case CDC_VCOM_COMM_FEATURE_ABSTRACT_STATE:
                  /* send abstract state */
                  USBD_DEBUG_HI_2(CDC_VCOM_REQ, "get %s %s", cdc_vcom->data.name, "ABSTRACT_STATE");
                  (void)USBD_IOTP_EVENT_Send(tp_in, &(cdc_vcom->data.abstract_state), sizeof(uint16_t), USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
                  break;

               case CDC_VCOM_COMM_FEATURE_COUNTRY_SETTING:
                  /* send country setting */
                  USBD_DEBUG_HI_2(CDC_VCOM_REQ, "get %s %s", cdc_vcom->data.name, "COUNTRY_SETTING");
                  (void)USBD_IOTP_EVENT_Send(tp_in, &(cdc_vcom->data.country_setting), sizeof(uint16_t), USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
                  break;

               default:
                  result = USBD_FALSE;
                  break;
            }
            break;

         case CDC_CLEAR_COMM_FEATURE_INTERFACE:
            switch(req->wValue)
            {
               case CDC_VCOM_COMM_FEATURE_ABSTRACT_STATE:
                  /* clear abstract state */
                  USBD_DEBUG_HI_2(CDC_VCOM_REQ, "clear %s %s", cdc_vcom->data.name, "ABSTRACT_STATE");
                  cdc_vcom->data.abstract_state = 0;
                  (void)USBD_IOTP_EVENT_Send_Status(tp_in, USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
                  break;

               case CDC_VCOM_COMM_FEATURE_COUNTRY_SETTING:
                  /* clear country setting */
                  USBD_DEBUG_HI_2(CDC_VCOM_REQ, "clear %s %s", cdc_vcom->data.name, "COUNTRY_SETTING");
                  cdc_vcom->data.country_setting = 0;
                  (void)USBD_IOTP_EVENT_Send_Status(tp_in, USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
                  break;

               default:
                  result = USBD_FALSE;
                  break;
            }
            break;

         case CDC_SET_LINE_CODING_INTERFACE:
            USBD_DEBUG_HI_2(CDC_VCOM_REQ, "set %s %s", cdc_vcom->data.name, "LINE_CODING");
            tp_vendor_data = USBD_IOTP_EVENT_Get_Vendor_Data_Container(tp_out);

            if(USBD_CHECK_PTR(USBD_Vendor_Data_XT, tp_vendor_data) && (req->wLength >= CDC_VCOM_LINE_CODING_CONTAINER_SIZE))
            {
               tp_vendor_data->pvoid = if_params;
               if_params->vendor_data.pvoid = cdc_vcom;

               USBD_IOTP_EVENT_Set_Ready_Handler(tp_out, CDC_VCOM_on_ready_params_change);

               if(USBD_BOOL_IS_TRUE(USBD_IOTP_EVENT_Recv_And_Ready(tp_out, &(cdc_vcom->data.comm_physical_params), req->wLength, &size)))
               {
                  if(size < 0)
                  {
                     CDC_VCOM_on_ready_params_change(tp_out, USB_EP_DIRECTION_OUT, (USBD_IO_Inout_Data_Size_DT)(req->wLength));
                  }
               }
            }
            else
            {
               USBD_WARN_2(CDC_VCOM_REQ, "%d bytes delivered too small, expected: %d",
                  req->wLength, CDC_VCOM_LINE_CODING_CONTAINER_SIZE);
               (void)USBD_IOTP_EVENT_Send_Stall(tp_in);
            }
            break;

         case CDC_GET_LINE_CODING_INTERFACE:
            USBD_DEBUG_HI_2(CDC_VCOM_REQ, "get %s %s", cdc_vcom->data.name, "LINE_CODING");
            (void)USBD_IOTP_EVENT_Send(
               tp_in, &(cdc_vcom->data.comm_physical_params), req->wLength, USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
            break;

         case CDC_SET_CONTROL_LINE_STATE_INTERFACE:
            USBD_DEBUG_HI_1(CDC_VCOM_REQ, "set %s", "LINE_STATE");
#if(CDC_VCOM_MODE_DATA_AND_SIGNALS == CDC_VCOM_MODE)
            previous_dtr = cdc_vcom->data.comm_physical_params.dtr;
            previous_rts = cdc_vcom->data.comm_physical_params.rts;

            dtr = 0 != (req->wValue & CDC_VCOM_LINE_STATE_DTR_MASK);
            rts = 0 != (req->wValue & CDC_VCOM_LINE_STATE_RTS_MASK);

            cdc_vcom->data.comm_physical_params.dtr = dtr;
            cdc_vcom->data.comm_physical_params.rts = rts;

            USBD_DEBUG_MID_3(CDC_VCOM_ONOFF, "%s.LINE_STATE.%-3s:  %d",
               cdc_vcom->data.name, "DTR", dtr);
            USBD_DEBUG_MID_3(CDC_VCOM_ONOFF, "%s.LINE_STATE.%-3s:  %d",
               cdc_vcom->data.name, "RTS", rts);

            if(previous_dtr != dtr)
            {
               if(USBD_CHECK_HANDLER(CDC_VCOM_On_DTR_Change_HT, cdc_vcom->handlers.change_dtr))
               {
                  USBD_DEBUG_HI(CDC_VCOM_ONOFF, "call LINE_STATE change_dtr");

                  USBD_CALL_HANDLER(CDC_VCOM_On_DTR_Change_HT, cdc_vcom->handlers.change_dtr) (cdc_vcom);
               }
            }

            if(previous_rts != rts)
            {
               if(USBD_CHECK_HANDLER(CDC_VCOM_On_RTS_Change_HT, cdc_vcom->handlers.change_rts))
               {
                  USBD_DEBUG_HI(CDC_VCOM_ONOFF, "call LINE_STATE change_rts");

                  USBD_CALL_HANDLER(CDC_VCOM_On_RTS_Change_HT, cdc_vcom->handlers.change_rts) (cdc_vcom);
               }
            }
#endif

            (void)USBD_IOTP_EVENT_Send_Status(tp_in, USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
            break;

         case CDC_SEND_BREAK_INTERFACE:
            USBD_DEBUG_HI_2(CDC_VCOM_REQ, "set %s %s", cdc_vcom->data.name, "BREAK");
            cdc_vcom->data.duration_of_break = req->wValue;
            USBD_DEBUG_MID_2(CDC_VCOM_ONOFF, "%s BREAK duration: %d ms", cdc_vcom->data.name, cdc_vcom->data.duration_of_break);
            (void)USBD_IOTP_EVENT_Send_Status(tp_in, USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
            break;

         default:
            result = USBD_FALSE;
            USBD_IOTP_EVENT_Send_Stall(tp_in);
            break;
      }

   }while(0);

   USBD_EXIT_FUNC(CDC_VCOM_REQ);

   return result;
} /* CDC_VCOM_on_request */

static void CDC_VCOM_on_event(
   USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   CDC_VCOM_Params_XT *cdc_vcom;
   Buff_Ring_XT *buff;
#if(CDC_VCOM_MODE_DATA_AND_SIGNALS != CDC_VCOM_MODE)
   USBD_IO_Inout_Data_Size_DT size_left = 0;
#endif

   USBD_ENTER_FUNC(CDC_VCOM_EVENT);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd) && USBD_CHECK_PTR(USBDC_Params_XT, usbdc)
      && USBD_CHECK_PTR(USBD_EVENT_Event_Header_XT, event_params))
   {
      cdc_vcom = event_params->vendor.pvoid;
      if(USBD_EVENT_REASON_CONFIGURED == reason)
      {
         USBD_DEBUG_HI_1(CDC_VCOM_EVENT, "USBD_EVENT_REASON_%sCONFIGURED", "");

         if(USBD_CHECK_PTR(CDC_VCOM_Params_XT, cdc_vcom))
         {
            buff = USBD_IOTP_BUFF_Get_Buff(&(cdc_vcom->data.iotp.in));
            USBD_DEBUG_HI_2(CDC_VCOM_EVENT, "init %s BUFF IN TP on EP: %d", cdc_vcom->data.name, cdc_vcom->data.hw.ep_in);
            USBD_IOTP_BUFF_Init(usbd, usbdc, cdc_vcom->data.hw.ep_in, USB_EP_DIRECTION_IN, &(cdc_vcom->data.iotp.in), buff);
            USBD_IOTP_BUFF_Install(&(cdc_vcom->data.iotp.in));
            buff = USBD_IOTP_BUFF_Get_Buff(&(cdc_vcom->data.iotp.out));
            USBD_DEBUG_HI_2(CDC_VCOM_EVENT, "init %s BUFF OUT TP on EP: %d", cdc_vcom->data.name, cdc_vcom->data.hw.ep_out);
            USBD_IOTP_BUFF_Init(usbd, usbdc, cdc_vcom->data.hw.ep_out, USB_EP_DIRECTION_OUT, &(cdc_vcom->data.iotp.out), buff);
            USBD_IOTP_BUFF_Install(&(cdc_vcom->data.iotp.out));
            USBD_DEBUG_HI_2(CDC_VCOM_EVENT, "init %s EVENT IN TP on EP: %d", cdc_vcom->data.name, cdc_vcom->data.hw.ep_notif);
            USBD_IOTP_EVENT_Init(usbd, usbdc, cdc_vcom->data.hw.ep_notif, USB_EP_DIRECTION_IN, &(cdc_vcom->data.iotp.notif));
            USBD_IOTP_EVENT_Install(&(cdc_vcom->data.iotp.notif));

            USBDC_REQUEST_Interface_Irq_Install(usbdc, CDC_VCOM_on_request, cdc_vcom->data.hw.if_notif);
         }
      }
      else if(USBD_EVENT_REASON_UNCONFIGURED == reason)
      {
         USBD_DEBUG_HI_1(CDC_VCOM_EVENT, "USBD_EVENT_REASON_%sCONFIGURED", "UN");
      }
      else if(USBD_EVENT_REASON_SUSPENDED == reason)
      {
         USBD_DEBUG_HI(CDC_VCOM_EVENT, "USBD_EVENT_REASON_SUSPENDED");
      }
      else if(USBD_EVENT_REASON_RESUMED == reason)
      {
         USBD_DEBUG_HI(CDC_VCOM_EVENT, "USBD_EVENT_REASON_RESUMED");
      }
#if(CDC_VCOM_MODE_DATA_AND_SIGNALS == CDC_VCOM_MODE)
      else if(USBD_EVENT_REASON_SOF_RECEIVED == reason)
      {
         if(USBD_ATOMIC_BOOL_IS_TRUE(cdc_vcom->data.serial_state_notification.was_changed)
            && USBD_BOOL_IS_FALSE(cdc_vcom->data.serial_state_notification.send_ongoing))
         {
            USBD_ATOMIC_BOOL_SET(cdc_vcom->data.serial_state_notification.was_changed, USBD_FALSE);

            cdc_vcom->data.serial_state_notification.data = 0;
            if(USBD_ATOMIC_BOOL_IS_TRUE(cdc_vcom->data.serial_state_notification.ring))
            {
               cdc_vcom->data.serial_state_notification.data |= CDC_VCOM_SERIAL_STATE_NOTIFICATION_RING;
            }
            if(USBD_ATOMIC_BOOL_IS_TRUE(cdc_vcom->data.serial_state_notification.dsr))
            {
               cdc_vcom->data.serial_state_notification.data |= CDC_VCOM_SERIAL_STATE_NOTIFICATION_DSR;
            }
            if(USBD_ATOMIC_BOOL_IS_TRUE(cdc_vcom->data.serial_state_notification.dcd))
            {
               cdc_vcom->data.serial_state_notification.data |= CDC_VCOM_SERIAL_STATE_NOTIFICATION_DCD;
            }

            USBD_IOTP_EVENT_Set_Ready_Handler(&(cdc_vcom->data.iotp.notif), CDC_VCOM_send_notif_done);
            if(USBD_BOOL_IS_TRUE(USBD_IOTP_EVENT_Send(
               &(cdc_vcom->data.iotp.notif),
               &(cdc_vcom->data.serial_state_notification.req),
               sizeof(cdc_vcom->data.serial_state_notification.req) + sizeof(cdc_vcom->data.serial_state_notification.data),
               &size_left) && (size_left > 0)))
            {
               cdc_vcom->data.serial_state_notification.send_ongoing = USBD_TRUE;
            }
         }
      }
#endif
   }

   USBD_EXIT_FUNC(CDC_VCOM_EVENT);
} /* CDC_VCOM_on_event */



void CDC_VCOM_Init(CDC_VCOM_Params_XT *cdc_vcom, Buff_Ring_XT *buff_in, Buff_Ring_XT *buff_out, const char *name)
{
   USBD_ENTER_FUNC(CDC_VCOM_INIT);

   memset(cdc_vcom, 0, sizeof(CDC_VCOM_Params_XT));

   USBD_IOTP_BUFF_Init(USBD_MAKE_INVALID_PTR(void), USBD_MAKE_INVALID_PTR(void), 0, 0, &(cdc_vcom->data.iotp.in), buff_in);
   USBD_IOTP_BUFF_Init(USBD_MAKE_INVALID_PTR(void), USBD_MAKE_INVALID_PTR(void), 0, 0, &(cdc_vcom->data.iotp.out), buff_out);
   USBD_IOTP_EVENT_Init(USBD_MAKE_INVALID_PTR(void), USBD_MAKE_INVALID_PTR(void), 0, 0, &(cdc_vcom->data.iotp.notif));

   if(!USBD_CHECK_PTR(const char, name))
   {
      name = "";
   }

   cdc_vcom->data.name = name;

   USBD_EXIT_FUNC(CDC_VCOM_INIT);
} /* CDC_VCOM_Init */

void CDC_VCOM_Install_In_Config(USBDC_Params_XT *usbdc, CDC_VCOM_Params_XT *cdc_vcom, uint8_t notif_if_num)
{
   USBDC_Interface_Header_XT *notif_if_params;
   USBDC_Interface_Header_XT *data_if_params;
   USBD_EVENT_Event_Header_XT *event;
   const uint8_t *desc;
   uint16_t desc_size;
   uint16_t notif_if_pos;
   uint16_t data_if_pos;
   uint16_t pos;
   uint8_t data_if_num;

   USBD_ENTER_FUNC(CDC_VCOM_INIT);

   if(USBD_CHECK_PTR(USBDC_Params_XT, usbdc) && USBD_CHECK_PTR(CDC_VCOM_Params_XT, cdc_vcom))
   {
      desc = USBDC_Get_Config_Desc(usbdc);

      if(USBD_CHECK_PTR(const uint8_t, desc))
      {
         desc_size = USB_CDP_Get_Config_Desc_Total_Length(desc);

         /* find notification interface in configuration descriptor */
         notif_if_pos = USB_CDP_Find_Interface(desc, desc_size, 0, notif_if_num, 0);

         if((0 != notif_if_pos) && (notif_if_pos < desc_size))
         {
            data_if_num = desc[notif_if_pos + USB_DESC_TYPE_INTERFACE_SIZE + CDC_VCOM_DESC_SLAVE_IF_NUM_OFFSET];

            /* find data interface in configuration descriptor */
            data_if_pos = USB_CDP_Find_Interface(desc, desc_size, 0, data_if_num, 0);

            if((0 != data_if_pos) && (data_if_pos < desc_size) && (data_if_pos != notif_if_pos))
            {
               /* find notification endpoint - we need to save its number */
               pos = USB_CDP_Find_Next_Endpoint(desc, desc_size, notif_if_pos);

               if((0 != pos) && (pos < desc_size))
               {
                  cdc_vcom->data.hw.ep_notif = desc[pos + 2] & USB_EP_DESC_ADDR_MASK;

                  /* find data endpoint 1 - we need to save its number */
                  pos = USB_CDP_Find_Next_Endpoint(desc, desc_size, data_if_pos);

                  if((0 != pos) && (pos < desc_size))
                  {
                     if(USB_EP_DESC_DIR_OUT == (USB_EP_DESC_DIR_MASK & desc[pos + 2]))
                     {
                        cdc_vcom->data.hw.ep_out = desc[pos + 2] & USB_EP_DESC_ADDR_MASK;
                     }
                     else
                     {
                        cdc_vcom->data.hw.ep_in = desc[pos + 2] & USB_EP_DESC_ADDR_MASK;
                     }

                     /* find data endpoint 1 - we need to save its number */
                     pos = USB_CDP_Find_Next_Endpoint(desc, desc_size, pos);

                     if((0 != pos) && (pos < desc_size))
                     {
                        notif_if_params = USBDC_Get_Interface_Params(usbdc, notif_if_num);
                        data_if_params = USBDC_Get_Interface_Params(usbdc, data_if_num);

                        if(USBD_CHECK_PTR(USBDC_Interface_Header_XT, notif_if_params)
                           && USBD_CHECK_PTR(USBDC_Interface_Header_XT, data_if_params))
                        {
                           if(USB_EP_DESC_DIR_OUT == (USB_EP_DESC_DIR_MASK & desc[pos + 2]))
                           {
                              cdc_vcom->data.hw.ep_out = desc[pos + 2] & USB_EP_DESC_ADDR_MASK;
                           }
                           else
                           {
                              cdc_vcom->data.hw.ep_in = desc[pos + 2] & USB_EP_DESC_ADDR_MASK;
                           }

                           cdc_vcom->data.hw.if_notif = notif_if_num;
                           event = USBDC_EVENT_Install(
                              usbdc,
                              CDC_VCOM_on_event,
                              USBD_EVENT_REASON_UNCONFIGURED | USBD_EVENT_REASON_CONFIGURED | USBD_EVENT_REASON_SOF_RECEIVED
                              | USBD_EVENT_REASON_SUSPENDED | USBD_EVENT_REASON_RESUMED);
                           event->vendor.pvoid = cdc_vcom;

                           notif_if_params->vendor.pvoid = cdc_vcom;
                           data_if_params->vendor.pvoid  = cdc_vcom;
                           cdc_vcom->data.usbdc = usbdc;
#if(CDC_VCOM_MODE_DATA_AND_SIGNALS == CDC_VCOM_MODE)
                           cdc_vcom->data.serial_state_notification.req.bmRequestType = 0xA1;
                           cdc_vcom->data.serial_state_notification.req.bRequest = 0x20;
                           cdc_vcom->data.serial_state_notification.req.wIndex = notif_if_num;
                           cdc_vcom->data.serial_state_notification.req.wLength = 2;
#endif

                           USBD_DEBUG_HI_8(CDC_VCOM_INIT, "install %s cdc_vcom for NIF: %d/EP: %d; DIF: %d/ EP_IN: %d, EP_OUT: %d, BUFF_IN: %p, BUFF_OUT: %p",
                              cdc_vcom->data.name,
                              notif_if_num,
                              cdc_vcom->data.hw.ep_notif,
                              data_if_num,
                              cdc_vcom->data.hw.ep_in,
                              cdc_vcom->data.hw.ep_out,
                              USBD_IOTP_BUFF_Get_Buff(&(cdc_vcom->data.iotp.in)),
                              USBD_IOTP_BUFF_Get_Buff(&(cdc_vcom->data.iotp.out)));
                        }
                     }
                  }
               }
            }
         }
      }
   }

   USBD_EXIT_FUNC(CDC_VCOM_INIT);
} /* CDC_VCOM_Install_In_Config */

const CDC_VCOM_Physical_Params_DT *CDC_VCOM_Get_Com_Physical_Params(CDC_VCOM_Params_XT *cdc_vcom)
{
   const CDC_VCOM_Physical_Params_DT *result = USBD_MAKE_INVALID_PTR(const CDC_VCOM_Physical_Params_DT);

   USBD_ENTER_FUNC(CDC_VCOM_STATE);

   if(USBD_CHECK_PTR(CDC_VCOM_Params_XT, cdc_vcom))
   {
      result = &(cdc_vcom->data.comm_physical_params);
   }

   USBD_EXIT_FUNC(CDC_VCOM_STATE);

   return result;
} /* CDC_VCOM_Get_Com_Physical_Params */

void CDC_VCOM_Set_Change_Params_Event(CDC_VCOM_Params_XT *cdc_vcom, CDC_VCOM_On_Params_Change_HT on_params_change)
{
   USBD_ENTER_FUNC(CDC_VCOM_INIT);

   if(USBD_CHECK_PTR(CDC_VCOM_Params_XT, cdc_vcom))
   {
      USBD_SET_HANDLER(CDC_VCOM_On_Params_Change_HT, cdc_vcom->handlers.change_params, on_params_change);
   }

   USBD_EXIT_FUNC(CDC_VCOM_INIT);
} /* CDC_VCOM_Set_Change_Params_Event */

#if(CDC_VCOM_MODE_DATA_AND_SIGNALS == CDC_VCOM_MODE)

void CDC_VCOM_Set_Change_Dtr_Event(CDC_VCOM_Params_XT *cdc_vcom, CDC_VCOM_On_DTR_Change_HT on_dtr_change)
{
   USBD_ENTER_FUNC(CDC_VCOM_INIT);

   if(USBD_CHECK_PTR(CDC_VCOM_Params_XT, cdc_vcom))
   {
      USBD_SET_HANDLER(CDC_VCOM_On_DTR_Change_HT, cdc_vcom->handlers.change_dtr, on_dtr_change);
   }

   USBD_EXIT_FUNC(CDC_VCOM_INIT);
} /* CDC_VCOM_Set_Change_Dtr_Event */

void CDC_VCOM_Set_Change_Rts_Event(CDC_VCOM_Params_XT *cdc_vcom, CDC_VCOM_On_RTS_Change_HT on_rts_change)
{
   USBD_ENTER_FUNC(CDC_VCOM_INIT);

   if(USBD_CHECK_PTR(CDC_VCOM_Params_XT, cdc_vcom))
   {
      USBD_SET_HANDLER(CDC_VCOM_On_RTS_Change_HT, cdc_vcom->handlers.change_rts, on_rts_change);
   }

   USBD_EXIT_FUNC(CDC_VCOM_INIT);
} /* CDC_VCOM_Set_Change_Rts_Event */

USBD_Bool_DT CDC_VCOM_Get_Dtr(CDC_VCOM_Params_XT *cdc_vcom)
{
   USBD_Bool_DT result = USBD_FALSE;

   USBD_ENTER_FUNC(CDC_VCOM_INIT);

   if(USBD_CHECK_PTR(CDC_VCOM_Params_XT, cdc_vcom))
   {
      result = cdc_vcom->data.comm_physical_params.dtr;
   }

   USBD_EXIT_FUNC(CDC_VCOM_INIT);

   return result;
} /* CDC_VCOM_Get_Dtr */

USBD_Bool_DT CDC_VCOM_Get_Rts(CDC_VCOM_Params_XT *cdc_vcom)
{
   USBD_Bool_DT result = USBD_FALSE;

   USBD_ENTER_FUNC(CDC_VCOM_INIT);

   if(USBD_CHECK_PTR(CDC_VCOM_Params_XT, cdc_vcom))
   {
      result = cdc_vcom->data.comm_physical_params.rts;
   }

   USBD_EXIT_FUNC(CDC_VCOM_INIT);

   return result;
} /* CDC_VCOM_Get_Rts */

void CDC_VCOM_Set_Ring(CDC_VCOM_Params_XT *cdc_vcom, USBD_Bool_DT state)
{
   USBD_ENTER_FUNC(CDC_VCOM_ONOFF);

   if(USBD_CHECK_PTR(CDC_VCOM_Params_XT, cdc_vcom))
   {
      USBD_ATOMIC_BOOL_SET(cdc_vcom->data.serial_state_notification.ring, state);
      USBD_ATOMIC_BOOL_SET(cdc_vcom->data.serial_state_notification.was_changed, USBD_TRUE);
   }

   USBD_EXIT_FUNC(CDC_VCOM_ONOFF);
} /* CDC_VCOM_Set_Ring */

void CDC_VCOM_Set_Dsr(CDC_VCOM_Params_XT *cdc_vcom, USBD_Bool_DT state)
{
   USBD_ENTER_FUNC(CDC_VCOM_ONOFF);

   if(USBD_CHECK_PTR(CDC_VCOM_Params_XT, cdc_vcom))
   {
      USBD_ATOMIC_BOOL_SET(cdc_vcom->data.serial_state_notification.dsr, state);
      USBD_ATOMIC_BOOL_SET(cdc_vcom->data.serial_state_notification.was_changed, USBD_TRUE);
   }

   USBD_EXIT_FUNC(CDC_VCOM_ONOFF);
} /* CDC_VCOM_Set_Dsr */

void CDC_VCOM_Set_Dcd(CDC_VCOM_Params_XT *cdc_vcom, USBD_Bool_DT state)
{
   USBD_ENTER_FUNC(CDC_VCOM_ONOFF);

   if(USBD_CHECK_PTR(CDC_VCOM_Params_XT, cdc_vcom))
   {
      USBD_ATOMIC_BOOL_SET(cdc_vcom->data.serial_state_notification.dcd, state);
      USBD_ATOMIC_BOOL_SET(cdc_vcom->data.serial_state_notification.was_changed, USBD_TRUE);
   }

   USBD_EXIT_FUNC(CDC_VCOM_ONOFF);
} /* CDC_VCOM_Set_Dcd */

#endif

Buff_Ring_XT *CDC_Vcom_Get_In_Buf(CDC_VCOM_Params_XT *cdc_vcom)
{
   Buff_Ring_XT *result = USBD_MAKE_INVALID_PTR(Buff_Ring_XT);

   USBD_ENTER_FUNC(CDC_VCOM_ONOFF);

   if(USBD_CHECK_PTR(CDC_VCOM_Params_XT, cdc_vcom))
   {
      result = USBD_IOTP_BUFF_Get_Buff(&(cdc_vcom->data.iotp.in));
   }

   USBD_EXIT_FUNC(CDC_VCOM_ONOFF);

   return result;
}

Buff_Ring_XT *CDC_Vcom_Get_Out_Buf(CDC_VCOM_Params_XT *cdc_vcom)
{
   Buff_Ring_XT *result = USBD_MAKE_INVALID_PTR(Buff_Ring_XT);

   USBD_ENTER_FUNC(CDC_VCOM_ONOFF);

   if(USBD_CHECK_PTR(CDC_VCOM_Params_XT, cdc_vcom))
   {
      result = USBD_IOTP_BUFF_Get_Buff(&(cdc_vcom->data.iotp.out));
   }

   USBD_EXIT_FUNC(CDC_VCOM_ONOFF);

   return result;
}


#ifdef CDC_VCOM_POST_IMP_INCLUDE
#include "cdc_vcom_post_imp.h"
#endif
