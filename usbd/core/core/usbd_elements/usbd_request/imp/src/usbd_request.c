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

#ifndef USBD_REQUEST_C_
#define USBD_REQUEST_C_
#endif

#ifndef USBD_H_
#include "usbd.h"
#endif

#ifndef USBD_REQUEST_INTERNAL_H_
#include "usbd_request_internal.h"
#endif

#ifdef USBD_DEV_PRESENT
#ifndef USBD_DEV_INTERNAL_H_
#include "usbd_dev_internal.h"
#endif
#endif

#ifdef USBD_EVENT_PRESENT
#ifndef USBD_EVENT_INTERNAL_H_
#include "usbd_event_internal.h"
#endif
#endif

#ifdef USBD_IO_PRESENT
#ifndef USBD_IO_INTERNAL_H_
#include "usbd_io_internal.h"
#endif
#endif

#ifdef USBDC_PRESENT
#ifndef USBDC_INTERNAL_H_
#include "usbdc_internal.h"
#endif
#endif

#define USBD_REQUEST_INTERNAL_MAX_POSSIBLE_NUM_ENDPONTS  16
#define USBD_REQUEST_INTERNAL_EP_TYPE_MASK               0x80
#define USBD_REQUEST_INTERNAL_EP_TYPE_OUT                0
#define USBD_REQUEST_INTERNAL_EP_TYPE_IN                 0x80

#ifdef USBD_USE_IOCMD
static const char * const USBD_REQUEST_standard_req_desc[] =
{
   "GET_STATUS",
   "CLEAR_FEATURE",
   "RESERVED",
   "SET_FEATURE",
   "RESERVED",
   "SET_ADDRESS",
   "GET_DESCRIPTOR",
   "SET_DESCRIPTOR",
   "GET_CONFIGURATION",
   "SET_CONFIGURATION",
   "GET_INTERFACE",
   "SET_INTERFACE",
   "SYNCH_FRAME"
};
#endif

static USBD_Bool_DT USBD_REQUEST_process_other_standard_requests(
   USBD_Params_XT *usbd,
   USBDC_Params_XT *usbdc,
   uint8_t ep_num,
   USBD_REQUEST_Req_DT *req);
static void USBD_REQUEST_set_addr_finish(USBD_IOTP_EVENT_Params_XT *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size);

static void USBD_REQUEST_fill_request(USBD_REQUEST_Req_DT *req)
{
   req->request = (((uint16_t)(req->bRequest) & 0xFF) * 256) | ((uint16_t)(req->bmRequestType) & 0xFF);
} /* USBD_REQUEST_fill_request */

void USBD_REQUEST_Install_Port_Callbacks(
      USBD_Params_XT *usbd,
      const USBD_REQUEST_Port_Callbacks_XT *callbacks)
{
   USBD_ENTER_FUNC(USBD_DBG_REQ_INIT);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      usbd->request.core.data.port_cbk = callbacks;
   }

   USBD_EXIT_FUNC(USBD_DBG_REQ_INIT);
} /* USBD_REQUEST_Install_Port_Callbacks */

static USBD_Bool_DT USBD_REQUEST_process_other_standard_requests(
      USBD_Params_XT *usbd,
      USBDC_Params_XT *usbdc,
      uint8_t ep_num,
      USBD_REQUEST_Req_DT *req)
{
   USB_EP_Direction_ET dir;
   uint8_t req_ep_num;
   uint8_t number;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_REQ_PROCESSING);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBDC_Params_XT, usbdc))
   {
      if(USBD_BMREQUESTTYPE_INTERFACE == (req->bmRequestType & USBD_BMREQUESTTYPE_DESTINATION_MASK))
      {
         number = req->wIndex & 0x00FF;

         if(number < USBD_MAX_NUM_INTERFACES)
         {
            if(USBDC_REQUEST_CHECK_INTERFACE_HANDLER(usbdc, number))
            {
               USBD_DEBUG_HI_1(USBD_DBG_REQ_PROCESSING, "calling if_irq for if: %d", number);

               result = USBDC_REQUEST_CALL_INTERFACE_IRQ(usbd, usbdc, ep_num, number, req);
            }
         }
      }
      else if(USBD_BMREQUESTTYPE_ENDPOINT == (req->bmRequestType & USBD_BMREQUESTTYPE_DESTINATION_MASK))
      {
         req_ep_num = req->wIndex & 0x007F;
         dir = (USB_EP_Direction_ET)(req->wIndex & USB_EP_DESC_DIR_MASK);

         USBD_DEBUG_HI_3(USBD_DBG_REQ_PROCESSING, "req for EP:%d.%s(%d)",
            req_ep_num, (USB_EP_DIRECTION_OUT == dir) ? "OUT" : "IN", dir);

         if(req_ep_num < USBD_MAX_NUM_ENDPOINTS)
         {
            number = USBD_DEV_Get_EP_Interface_Num(usbd, req_ep_num, dir);

            if(number < USBD_MAX_NUM_INTERFACES)
            {
               if(USBDC_REQUEST_CHECK_INTERFACE_HANDLER(usbdc, number))
               {
                  USBD_DEBUG_HI_1(USBD_DBG_REQ_PROCESSING, "calling if_irq for if: %d", number);

                  result = USBDC_REQUEST_CALL_INTERFACE_IRQ(usbd, usbdc, ep_num, number, req);
               }
            }
         }
      }
      /* All other request types are processed by vendor irq */
      else
      {
         if(USBDC_REQUEST_CHECK_VENDOR_HANDLER(usbdc))
         {
            result = USBDC_REQUEST_CALL_VENDOR_IRQ(usbd, usbdc, ep_num, req);
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_REQ_PROCESSING);

   return result;
} /* USBD_REQUEST_process_other_standard_requests */

static void USBD_REQUEST_set_addr_finish(USBD_IOTP_EVENT_Params_XT *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   USBD_Params_XT *usbd;
   USBD_REQUEST_Req_DT req;
   uint8_t addr;

   USBD_UNUSED_PARAM(dir);
   USBD_UNUSED_PARAM(size);

   USBD_ENTER_FUNC(USBD_DBG_REQ_PROCESSING);

   if(USBD_CHECK_PTR(void, tp))
   {
      usbd = USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp);

      if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
      {
         if(USBD_REQUEST_CHECK_PORT_CBK_TAB_PTR(usbd))
         {
            if(USBD_REQUEST_CHECK_PORT_SET_ADDRESS_HANDLER(usbd))
            {
               req.bRequest = USBD_BREQUEST_SET_ADDRESS;
               req.bmRequestType = USBD_BMREQUESTTYPE_SET_CLEAR_DEVICE;
               req.wValue = ((uint16_t)(usbd->request.core.data.req_data.u8)) & 0x00FF;
               req.wIndex = 0;
               req.wLength = 0;
               USBD_REQUEST_fill_request(&req);
               addr = usbd->request.core.data.req_data.u8;

               (void)USBD_REQUEST_CALL_PORT_SET_ADDRESS_HANDLER(usbd, USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp), &req);

               if(0 != addr)
               {
                  USBD_DEV_Addressed(usbd, addr);
               }
               else
               {
                  USBD_DEV_Reset(usbd);
               }
            }
         }
      }
      else
      {
         USBD_WARN_1(USBD_DBG_REQ_INVALID_PARAMS, "function invalid parameters! usbd: %p", usbd);
      }
   }
   else
   {
      USBD_WARN_1(USBD_DBG_REQ_INVALID_PARAMS, "function invalid parameters! tp: %p", tp);
   }

   USBD_EXIT_FUNC(USBD_DBG_REQ_PROCESSING);
} /* USBD_REQUEST_set_addr_finish */

static USBD_Bool_DT USBD_REQUEST_port_guard(USBD_Params_XT *usbd, USBD_REQUEST_Port_Callback_HT callback, USBD_REQUEST_Req_DT *req)
{
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_REQ_PROCESSING);

   if(USBD_CHECK_HANDLER(USBD_REQUEST_Port_Callback_HT, callback))
   {
      result = USBD_CALL_HANDLER(USBD_REQUEST_Port_Callback_HT, callback)(usbd, 0, req);
   }
   else
   {
      result = USBD_TRUE;
   }

   USBD_DEBUG_HI_1(USBD_DBG_REQ_PROCESSING, "port guard result is '%s'",
      USBD_BOOL_IS_TRUE(result) ? "true" : "false");

   USBD_EXIT_FUNC(USBD_DBG_REQ_PROCESSING);

   return result;
} /* USBD_REQUEST_port_guard */

static void USBD_REQUEST_get_status(
      USBD_Params_XT *usbd,
      USBDC_Params_XT *usbdc,
      USBD_REQUEST_Req_DT *req,
      USB_EP_Direction_ET dir,
      uint8_t destination)
{
   uint8_t ep_num;
   USBD_Bool_DT result   = USBD_TRUE;
   USBD_Bool_DT send_ack = USBD_FALSE;

   USBD_ENTER_FUNC(USBD_DBG_REQ_PROCESSING);

   switch(destination)
   {
      case USBD_BMREQUESTTYPE_DEVICE:
         if(USBD_DEV_STATE_ADDRESSED == (USBD_DEV_STATE_ADDRESSED & USBD_DEV_Get_State(usbd)))
         {
            usbd->request.core.data.req_data.u16 = 0;

            if(USBD_REQUEST_CHECK_PORT_CBK_TAB_PTR(usbd))
            {
               result = USBD_REQUEST_port_guard(usbd, USBD_REQUEST_GET_PORT_GET_STATUS_HANDLER(usbd), req);
            }

            if(USBD_BOOL_IS_TRUE(result))
            {
               if(USBD_CHECK_PORT_PTR(usbd))
               {
                  if(USBD_DEV_CHECK_PORT_DEVICE_STATUS_HANDLER(USBD_DEV_GET_PORT_PTR(usbd)))
                  {
                     usbd->request.core.data.req_data.u16 = USBD_DEV_GET_PORT_DEVICE_STATUS(usbd, USBD_DEV_GET_PORT_PTR(usbd));
                  }
               }

               send_ack = USBD_TRUE;
            }
         }
         break;

      case USBD_BMREQUESTTYPE_INTERFACE:
         if(USBD_DEV_STATE_CONFIGURED == (USBD_DEV_STATE_CONFIGURED & USBD_DEV_Get_State(usbd)))
         {
            if(USBD_REQUEST_CHECK_PORT_CBK_TAB_PTR(usbd))
            {
               result = USBD_REQUEST_port_guard(usbd, USBD_REQUEST_GET_PORT_GET_STATUS_HANDLER(usbd), req);
            }

            if(USBD_BOOL_IS_TRUE(result))
            {
               if(req->wIndex < USBD_DEV_Get_Num_Interfaces_In_Active_Config(usbd))
               {
                  usbd->request.core.data.req_data.u16 = 0;
                  send_ack = USBD_TRUE;
               }
            }
         }
         break;

      case USBD_BMREQUESTTYPE_ENDPOINT:
         ep_num = req->wIndex & 0x007F;

         if((USBD_DEV_STATE_CONFIGURED == (USBD_DEV_STATE_CONFIGURED & USBD_DEV_Get_State(usbd)))
            || (
               (USBD_DEV_STATE_ADDRESSED == (USBD_DEV_STATE_ADDRESSED & USBD_DEV_Get_State(usbd)))
               && (0 == ep_num) ))
         {
            if(USBD_REQUEST_CHECK_PORT_CBK_TAB_PTR(usbd))
            {
               result = USBD_REQUEST_port_guard(usbd, USBD_REQUEST_GET_PORT_GET_STATUS_HANDLER(usbd), req);
            }

            if(USBD_BOOL_IS_TRUE(result))
            {
               if(USBD_DEV_EP_OFF != USBD_DEV_Get_EP_State(usbd, ep_num, dir))
               {
                  if(USBD_DEV_Get_EP_Halt(usbd, ep_num, dir))
                  {
                     usbd->request.core.data.req_data.u16 = 1;
                  }
                  else
                  {
                     usbd->request.core.data.req_data.u16 = 0;
                  }
                  send_ack = USBD_TRUE;
               }
            }
         }
         break;

      default:
         (void)USBD_REQUEST_process_other_standard_requests(usbd, usbdc, 0, req);
         break;
   }

   if(USBD_BOOL_IS_TRUE(send_ack))
   {
      USBD_IOTP_EVENT_Send(
         USBD_REQUEST_GET_EP0_IN_IOTP(usbd),
         (uint8_t*)(&(usbd->request.core.data.req_data.u16)),
         2,
         USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
   }

   USBD_EXIT_FUNC(USBD_DBG_REQ_PROCESSING);
} /* USBD_REQUEST_get_status */

static void USBD_REQUEST_set_clear_feature(
      USBD_Params_XT *usbd,
      USBDC_Params_XT *usbdc,
      USBD_REQUEST_Req_DT *req,
      USB_EP_Direction_ET dir,
      USBD_Bool_DT is_set,
      uint8_t destination)
{
   USBD_REQUEST_Port_Callback_HT port_cbk;
   uint8_t ep_num;
   USBD_Bool_DT result = USBD_TRUE;
   USBD_Bool_DT is_allowed = USBD_FALSE;

   USBD_ENTER_FUNC(USBD_DBG_REQ_PROCESSING);

   switch(destination)
   {
      case USBD_BMREQUESTTYPE_DEVICE:
         if(USBD_BOOL_IS_TRUE(is_set))
         {
            result = USBD_FALSE;

            if(USBD_DEV_STATE_DEFAULT == (USBD_DEV_STATE_DEFAULT & USBD_DEV_Get_State(usbd)))
            {
               is_allowed = USBD_TRUE;
            }
         }
         else
         {
            if(USBD_DEV_STATE_ADDRESSED == (USBD_DEV_STATE_ADDRESSED & USBD_DEV_Get_State(usbd)))
            {
               is_allowed = USBD_TRUE;
            }
         }
         if(USBD_BOOL_IS_TRUE(is_allowed))
         {
            if(USBD_REQUEST_CHECK_PORT_CBK_TAB_PTR(usbd))
            {
               if(USBD_BOOL_IS_TRUE(is_set))
               {
                  port_cbk = USBD_REQUEST_GET_PORT_SET_FEATURE_HANDLER(usbd);
               }
               else
               {
                  port_cbk = USBD_REQUEST_GET_PORT_CLEAR_FEATURE_HANDLER(usbd);
               }

               if(USBD_CHECK_HANDLER(USBD_REQUEST_Port_Callback_HT, port_cbk))
               {
                  if(USBD_BOOL_IS_TRUE(USBD_CALL_HANDLER(USBD_REQUEST_Port_Callback_HT, port_cbk)(usbd, 0, req)))
                  {
                     result = USBD_TRUE;

                     USBD_IOTP_EVENT_Send_Status(
                        USBD_REQUEST_GET_EP0_IN_IOTP(usbd),
                        USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
                  }
               }
            }

            if(USBD_BOOL_IS_FALSE(result))
            {
               USBD_IOTP_EVENT_Send_Stall(USBD_REQUEST_GET_EP0_IN_IOTP(usbd));
            }
         }
         break;

      case USBD_BMREQUESTTYPE_INTERFACE:
         if(USBD_DEV_STATE_CONFIGURED == (USBD_DEV_STATE_CONFIGURED & USBD_DEV_Get_State(usbd)))
         {
            if(USBD_REQUEST_CHECK_PORT_CBK_TAB_PTR(usbd))
            {
               if(USBD_BOOL_IS_TRUE(is_set))
               {
                  port_cbk = USBD_REQUEST_GET_PORT_SET_FEATURE_HANDLER(usbd);
               }
               else
               {
                  port_cbk = USBD_REQUEST_GET_PORT_CLEAR_FEATURE_HANDLER(usbd);
               }
               result = USBD_REQUEST_port_guard(usbd, port_cbk, req);
            }

            if(USBD_BOOL_IS_TRUE(result))
            {
               (void)USBD_REQUEST_process_other_standard_requests(usbd, usbdc, 0, req);
            }
         }
         break;

      case USBD_BMREQUESTTYPE_ENDPOINT:
         ep_num = req->wIndex & 0x007F;

         if((USBD_DEV_STATE_CONFIGURED == (USBD_DEV_STATE_CONFIGURED & USBD_DEV_Get_State(usbd)))
            || (
               (USBD_DEV_STATE_ADDRESSED == (USBD_DEV_STATE_ADDRESSED & USBD_DEV_Get_State(usbd)))
               && (0 == ep_num) ))
         {
            if(USBD_REQUEST_CHECK_PORT_CBK_TAB_PTR(usbd))
            {
               if(USBD_BOOL_IS_TRUE(is_set))
               {
                  port_cbk = USBD_REQUEST_GET_PORT_SET_FEATURE_HANDLER(usbd);
               }
               else
               {
                  port_cbk = USBD_REQUEST_GET_PORT_CLEAR_FEATURE_HANDLER(usbd);
               }
               result = USBD_REQUEST_port_guard(usbd, port_cbk, req);
            }

            if(USBD_BOOL_IS_TRUE(result))
            {
               if(USBD_DEV_EP_OFF != USBD_DEV_Get_EP_State(usbd, ep_num, dir))
               {
                  ep_num &= USB_EP_DESC_ADDR_MASK;
                  result  = USBD_FALSE;
                  if(USBD_CHECK_PTR(USBDC_Params_XT, usbdc) && (0 != ep_num))
                  {
                     if(USBD_REQUEST_INTERNAL_EP_TYPE_OUT == (req->wIndex & USBD_REQUEST_INTERNAL_EP_TYPE_MASK))
                     {
                        if(USBDC_REQUEST_CHECK_EP_OUT_HALT_MASK(usbdc, ep_num))
                        {
                           result = USBD_REQUEST_process_other_standard_requests(usbd, usbdc, 0, req);
                        }
                     }
                     else
                     {
                        if(USBDC_REQUEST_CHECK_EP_IN_HALT_MASK(usbdc, ep_num))
                        {
                           result = USBD_REQUEST_process_other_standard_requests(usbd, usbdc, 0, req);
                        }
                     }
                  }

                  if(USBD_BOOL_IS_FALSE(result))
                  {
                     USBD_DEV_Set_EP_Halt(usbd, (uint8_t)(ep_num), dir, is_set);
                     USBD_IOTP_EVENT_Send_Status(
                        USBD_REQUEST_GET_EP0_IN_IOTP(usbd),
                        USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
                  }
               }
            }
         }
         break;

      default:
         (void)USBD_REQUEST_process_other_standard_requests(usbd, usbdc, 0, req);
         break;
   }

   USBD_EXIT_FUNC(USBD_DBG_REQ_PROCESSING);
} /* USBD_REQUEST_set_clear_feature */

static void USBD_REQUEST_set_address(
      USBD_Params_XT *usbd,
      USBDC_Params_XT *usbdc,
      USBD_REQUEST_Req_DT *req,
      uint8_t destination)
{
   USBD_ENTER_FUNC(USBD_DBG_REQ_PROCESSING);

   if((USBD_BMREQUESTTYPE_DEVICE == destination)
      && (USBD_DEV_STATE_DEFAULT == (USBD_DEV_STATE_DEFAULT & USBD_DEV_Get_State(usbd))))
   {
      if(req->wValue < 128)
      {
         USBD_IOTP_EVENT_Set_Buf_Empty_Handler(USBD_REQUEST_GET_EP0_IN_IOTP(usbd), USBD_REQUEST_set_addr_finish);

         usbd->request.core.data.req_data.u8 = (uint8_t)(req->wValue);

         USBD_IOTP_EVENT_Send_Status(USBD_REQUEST_GET_EP0_IN_IOTP(usbd), USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
      }
   }
   else
   {
      (void)USBD_REQUEST_process_other_standard_requests(usbd, usbdc, 0, req);
   }

   USBD_EXIT_FUNC(USBD_DBG_REQ_PROCESSING);
} /* USBD_REQUEST_set_address */

static void USBD_REQUEST_get_descrptor(
      USBD_Params_XT *usbd,
      USBDC_Params_XT *usbdc,
      USBD_REQUEST_Req_DT *req,
      uint8_t destination)
{
#ifdef USBD_USE_IOCMD
   const char *name = "";
#endif
   const USB_Endpoint_Desc_DT *ep0_desc;
   const USBD_DEV_Port_Handler_XT *port;
   const uint8_t *desc;
   USBD_IO_Inout_Data_Size_DT size;
   uint16_t number;
   USBD_Bool_DT result = USBD_TRUE;

   USBD_ENTER_FUNC(USBD_DBG_REQ_PROCESSING);

   if((USBD_BMREQUESTTYPE_DEVICE == destination)
      && (USBD_DEV_STATE_DEFAULT == (USBD_DEV_STATE_DEFAULT & USBD_DEV_Get_State(usbd))))
   {
      if(USBD_REQUEST_CHECK_PORT_CBK_TAB_PTR(usbd))
      {
         result = USBD_REQUEST_port_guard(usbd, USBD_REQUEST_GET_PORT_GET_DESCRIPTOR_HANDLER(usbd), req);
      }

      if(USBD_BOOL_IS_TRUE(result))
      {
         switch (((req->wValue >> 8) & 0x00FF))
         {
            case USB_DESC_TYPE_DEVICE:
               USBD_DEBUG_HI_1(USBD_DBG_REQ_PROCESSING, "value_H => desc type: %s", "DEVICE");
               if(req->wLength >= USB_DESC_TYPE_DEVICE_SIZE)
               {
                  size = USB_DESC_TYPE_DEVICE_SIZE;
               }
               else
               {
                  size = req->wLength;
               }

               USBD_LOG_DATA_DEBUG_LO_3(USBD_DBG_REQ_PROCESSING, (uint8_t*)(&(usbd->dev.core.data.dev_desc)), size,
                  "send %d bytes of %s descriptor with size %d",
                  size, "DEVICE", USB_DESC_TYPE_DEVICE_SIZE);

               USBD_IOTP_EVENT_Send(
                  USBD_REQUEST_GET_EP0_IN_IOTP(usbd),
                  (uint8_t*)(&(usbd->dev.core.data.dev_desc)),
                  size,
                  USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));

               break;

            case USB_DESC_TYPE_DEVICE_QUALIFIER:
               USBD_DEBUG_HI_1(USBD_DBG_REQ_PROCESSING, "value_H => desc type: %s", "DEVICE_QUALIFIER");
               if(USBD_CHECK_PORT_PTR(usbd))
               {
                  port = USBD_DEV_GET_PORT_PTR(usbd);

                  if(USBD_DEV_CHECK_PORT_SUPPORTED_SPEED_HANDLER(port))
                  {
                     if(USBD_DEV_HIGH_SPEED == (USBD_DEV_HIGH_SPEED & USBD_DEV_GET_PORT_SUPPORTED_SPEED(usbd, port)))
                     {
                        usbd->request.core.data.req_data.qual.bLength          = USB_DESC_TYPE_DEVICE_QUALIFIER_SIZE;
                        usbd->request.core.data.req_data.qual.bDescriptorType  = USB_DESC_TYPE_DEVICE_QUALIFIER;
                        usbd->request.core.data.req_data.qual.bcdUSB           = usbd->dev.core.data.dev_desc.bcdUSB;
                        usbd->request.core.data.req_data.qual.bDeviceClass     = usbd->dev.core.data.dev_desc.bDeviceClass;
                        usbd->request.core.data.req_data.qual.bDeviceSubclass  = usbd->dev.core.data.dev_desc.bDeviceSubclass;
                        usbd->request.core.data.req_data.qual.bDeviceProtocol  = usbd->dev.core.data.dev_desc.bDeviceProtocol;
                        if(USBD_DEV_CHECK_PORT_CURRENT_SPEED_HANDLER(port))
                        {
                           if(USBD_DEV_HIGH_SPEED
                              == (USBD_DEV_HIGH_SPEED & USBD_DEV_GET_PORT_CURRENT_SPEED(usbd, port)))
                           {
                              if(USBD_DEV_CHECK_PORT_EP0_LOW_FULL_SPEED_HANDLER(port))
                              {
                                 ep0_desc = USBD_DEV_GET_PORT_EP0_LOW_FULL_SPEED_DESC(usbd, port);

                                 if(USBD_CHECK_PTR(const USB_Endpoint_Desc_DT, ep0_desc))
                                 {
                                    usbd->request.core.data.req_data.qual.bMaxPacketSize0 = ep0_desc->wMaxPacketSize.L;
                                 }
                                 else
                                 {
                                    usbd->request.core.data.req_data.qual.bMaxPacketSize0 = usbd->dev.core.data.dev_desc.bMaxPacketSize0;
                                 }
                              }
                              else
                              {
                                 usbd->request.core.data.req_data.qual.bMaxPacketSize0 = usbd->dev.core.data.dev_desc.bMaxPacketSize0;
                              }
                           }
                           else
                           {
                              if(USBD_DEV_CHECK_PORT_EP0_HIGH_SPEED_HANDLER(port))
                              {
                                 ep0_desc = USBD_DEV_GET_PORT_EP0_HIGH_SPEED_DESC(usbd, port);

                                 if(USBD_CHECK_PTR(const USB_Endpoint_Desc_DT, ep0_desc))
                                 {
                                    usbd->request.core.data.req_data.qual.bMaxPacketSize0 = ep0_desc->wMaxPacketSize.L;
                                 }
                                 else
                                 {
                                    usbd->request.core.data.req_data.qual.bMaxPacketSize0 = usbd->dev.core.data.dev_desc.bMaxPacketSize0;
                                 }
                              }
                              else
                              {
                                 usbd->request.core.data.req_data.qual.bMaxPacketSize0 = usbd->dev.core.data.dev_desc.bMaxPacketSize0;
                              }
                           }
                        }
                        else
                        {
                           usbd->request.core.data.req_data.qual.bMaxPacketSize0 = usbd->dev.core.data.dev_desc.bMaxPacketSize0;
                        }
                        usbd->request.core.data.req_data.qual.bNumConfigurations = usbd->dev.core.data.dev_desc.bNumConfigurations;
                        usbd->request.core.data.req_data.qual.bReserved          = 0;

                        if(req->wLength >= USB_DESC_TYPE_DEVICE_QUALIFIER_SIZE)
                        {
                           size = USB_DESC_TYPE_DEVICE_QUALIFIER_SIZE;
                        }
                        else
                        {
                           size = req->wLength;
                        }

                        USBD_LOG_DATA_DEBUG_LO_3(USBD_DBG_REQ_PROCESSING, (uint8_t*)(&(usbd->request.core.data.req_data.qual)), size,
                           "send %d bytes of %s descriptor with size %d",
                           size, "DEVICE_QUALIFIER", USB_DESC_TYPE_DEVICE_QUALIFIER_SIZE);

                        USBD_IOTP_EVENT_Send(
                           USBD_REQUEST_GET_EP0_IN_IOTP(usbd),
                           (uint8_t*)(&(usbd->request.core.data.req_data.qual)),
                           size,
                           USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
                     }
                  }
               }
               break;

            case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
            case USB_DESC_TYPE_CONFIGURATION:
               if(USBD_COMPILATION_SWITCH_LOG(USBD_DBG_REQ_PROCESSING, IOCMD_LOG_LEVEL_DEBUG_LO))
               {
#ifdef USBD_USE_IOCMD
                  if(USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION == ((req->wValue >> 8) & 0x00FF))
                  {
                     name = "OTHER_SPEED_CONFIGURATION";
                  }
                  else
                  {
                     name = "CONFIGURATION";
                  }
#endif

                  USBD_DEBUG_HI_1(USBD_DBG_REQ_PROCESSING, "value_H => desc type: %s", name);
               }
               /* set "number" to requested configuration number */
               number = req->wValue & 0x00FF;
               USBD_DEBUG_HI_1(USBD_DBG_REQ_PROCESSING, "value_L => config num: %d", number);
               desc   = (const uint8_t*)USBD_DEV_Get_Config_Desc(usbd, number);

               if(USBD_CHECK_PTR(const uint8_t, desc))
               {
                  /* set "number" to descriptor size */
                  number = USB_CDP_Get_Config_Desc_Total_Length(desc);

                  if(req->wLength <= number)
                  {
                     size = req->wLength;
                  }
                  else
                  {
                     size = number;
                  }

                  USBD_LOG_DATA_DEBUG_LO_3(USBD_DBG_REQ_PROCESSING, desc, size,
                     "send %d bytes of %s descriptor with size %d",
                     size, name, number);

                  USBD_IOTP_EVENT_Send(
                     USBD_REQUEST_GET_EP0_IN_IOTP(usbd),
                     desc,
                     size,
                     USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
               }
               break;

            case USB_DESC_TYPE_STRING:
               USBD_DEBUG_HI_1(USBD_DBG_REQ_PROCESSING, "value_H => desc type: %s", "STRING");
               USBD_DEBUG_HI_1(USBD_DBG_REQ_PROCESSING, "value_L => string num: %d", (req->wValue & 0x00FF));
               desc = USBD_DEV_Get_String(usbd, (uint8_t)(req->wValue & 0x00FF));

               if(USBD_CHECK_PTR(const uint8_t, desc))
               {
                  if(req->wLength <= desc[0])
                  {
                     size = req->wLength;
                  }
                  else
                  {
                     size = desc[0];
                  }

                  USBD_LOG_DATA_DEBUG_LO_3(USBD_DBG_REQ_PROCESSING, desc, size,
                     "send %d bytes of %s descriptor with size %d",
                     size, "STRING", desc[0]);

                  USBD_IOTP_EVENT_Send(
                     USBD_REQUEST_GET_EP0_IN_IOTP(usbd),
                     desc,
                     size,
                     USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
               }
               break;

            default:
               if(USBD_DEV_CHECK_ACTIVE_CONFIG_PTR(usbd))
               {
                  (void)USBD_REQUEST_process_other_standard_requests(usbd, usbdc, 0, req);
               }
               break;
         }
      }
   }
   else if( ( (USBD_BMREQUESTTYPE_INTERFACE == destination) || (USBD_BMREQUESTTYPE_ENDPOINT == destination) )
      && (USBD_DEV_STATE_CONFIGURED == (USBD_DEV_STATE_CONFIGURED & USBD_DEV_Get_State(usbd))))
   {
      if(USBD_REQUEST_CHECK_PORT_CBK_TAB_PTR(usbd))
      {
         result = USBD_REQUEST_port_guard(usbd, USBD_REQUEST_GET_PORT_GET_DESCRIPTOR_HANDLER(usbd), req);
      }

      if(USBD_BOOL_IS_TRUE(result) && (USBD_DEV_CHECK_ACTIVE_CONFIG_PTR(usbd)))
      {
         (void)USBD_REQUEST_process_other_standard_requests(usbd, usbdc, 0, req);
      }
   }
   else
   {
      (void)USBD_REQUEST_process_other_standard_requests(usbd, usbdc, 0, req);
   }

   USBD_EXIT_FUNC(USBD_DBG_REQ_PROCESSING);
} /* USBD_REQUEST_get_descrptor */

static void USBD_REQUEST_get_configuration(
      USBD_Params_XT *usbd,
      USBDC_Params_XT *usbdc,
      USBD_REQUEST_Req_DT *req,
      uint8_t destination)
{
   USBD_Bool_DT result = USBD_TRUE;

   USBD_ENTER_FUNC(USBD_DBG_REQ_PROCESSING);

   if((USBD_BMREQUESTTYPE_DEVICE == destination)
      && (USBD_DEV_STATE_ADDRESSED == (USBD_DEV_STATE_ADDRESSED & USBD_DEV_Get_State(usbd))))
   {
      if(USBD_REQUEST_CHECK_PORT_CBK_TAB_PTR(usbd))
      {
         result = USBD_REQUEST_port_guard(usbd, USBD_REQUEST_GET_PORT_GET_CONFIGURATION_HANDLER(usbd), req);
      }

      if(USBD_BOOL_IS_TRUE(result))
      {
         if(USBD_DEV_CHECK_ACTIVE_CONFIG_PTR(usbd))
         {
            usbd->request.core.data.req_data.u8 = USBD_DEV_GET_ACTIVE_CONFIG_NUM(usbd) + 1;
         }
         else
         {
            usbd->request.core.data.req_data.u8 = 0;
         }

         USBD_IOTP_EVENT_Send(
            USBD_REQUEST_GET_EP0_IN_IOTP(usbd),
            &(usbd->request.core.data.req_data.u8),
            1,
            USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
      }
   }
   else
   {
      (void)USBD_REQUEST_process_other_standard_requests(usbd, usbdc, 0, req);
   }

   USBD_EXIT_FUNC(USBD_DBG_REQ_PROCESSING);
} /* USBD_REQUEST_get_configuration */

static void USBD_REQUEST_set_configuration_respond(USBD_Params_XT *usbd)
{
   USBD_IOTP_EVENT_Send_Status(
      USBD_REQUEST_GET_EP0_IN_IOTP(usbd),
      USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
}

static void USBD_REQUEST_set_configuration(
      USBD_Params_XT *usbd,
      USBDC_Params_XT *usbdc,
      USBD_REQUEST_Req_DT *req,
      uint8_t destination)
{
   USBD_Bool_DT result = USBD_TRUE;

   USBD_ENTER_FUNC(USBD_DBG_REQ_PROCESSING);

   if((USBD_BMREQUESTTYPE_DEVICE == destination)
      && (USBD_DEV_STATE_ADDRESSED == (USBD_DEV_STATE_ADDRESSED & USBD_DEV_Get_State(usbd))))
   {
      if(USBD_REQUEST_CHECK_PORT_CBK_TAB_PTR(usbd))
      {
         result = USBD_REQUEST_port_guard(usbd, USBD_REQUEST_GET_PORT_SET_CONFIGURATION_HANDLER(usbd), req);
      }

      if(USBD_BOOL_IS_TRUE(result))
      {
         if(USBD_BOOL_IS_FALSE(USBD_DEV_Configured(usbd, (uint8_t)(req->wValue), USBD_REQUEST_set_configuration_respond)))
         {
            USBD_WARN_1(USBD_DBG_REQ_PROCESSING, "configuration %u setting failed!", req->wValue);
         }
      }
   }
   else
   {
      (void)USBD_REQUEST_process_other_standard_requests(usbd, usbdc, 0, req);
   }

   USBD_EXIT_FUNC(USBD_DBG_REQ_PROCESSING);
} /* USBD_REQUEST_set_configuration */

static void USBD_REQUEST_get_interface(
      USBD_Params_XT *usbd,
      USBDC_Params_XT *usbdc,
      USBD_REQUEST_Req_DT *req,
      uint8_t destination)
{
   const USB_Interface_Desc_DT *desc;
   USBD_Bool_DT result = USBD_TRUE;

   USBD_ENTER_FUNC(USBD_DBG_REQ_PROCESSING);

   if((USBD_BMREQUESTTYPE_INTERFACE == destination)
      && (USBD_DEV_STATE_CONFIGURED == (USBD_DEV_STATE_CONFIGURED & USBD_DEV_Get_State(usbd))))
   {
      if(USBD_REQUEST_CHECK_PORT_CBK_TAB_PTR(usbd))
      {
         result = USBD_REQUEST_port_guard(usbd, USBD_REQUEST_GET_PORT_GET_INTERFACE_HANDLER(usbd), req);
      }

      if(USBD_BOOL_IS_TRUE(result))
      {
         desc = USBD_DEV_Get_Interface_Desc(usbd, (uint8_t)(req->wIndex & 0x00FF));

         if(USBD_CHECK_PTR(USB_Interface_Desc_DT, desc))
         {
            usbd->request.core.data.req_data.u8 = desc->bAlternateSetting;

            USBD_IOTP_EVENT_Send(
               USBD_REQUEST_GET_EP0_IN_IOTP(usbd),
               &(usbd->request.core.data.req_data.u8),
               1,
               USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
         }
      }
   }
   else
   {
      (void)USBD_REQUEST_process_other_standard_requests(usbd, usbdc, 0, req);
   }

   USBD_EXIT_FUNC(USBD_DBG_REQ_PROCESSING);
} /* USBD_REQUEST_get_interface */

static void USBD_REQUEST_set_interface(
      USBD_Params_XT *usbd,
      USBDC_Params_XT *usbdc,
      USBD_REQUEST_Req_DT *req,
      uint8_t destination)
{
   USBD_DEV_Set_Interface_Result_ET set_result;
   USBD_Bool_DT result = USBD_TRUE;

   USBD_ENTER_FUNC(USBD_DBG_REQ_PROCESSING);

   if((USBD_BMREQUESTTYPE_INTERFACE == destination)
      && (USBD_DEV_STATE_CONFIGURED == (USBD_DEV_STATE_CONFIGURED & USBD_DEV_Get_State(usbd))))
   {
      if(USBD_REQUEST_CHECK_PORT_CBK_TAB_PTR(usbd))
      {
         result = USBD_REQUEST_port_guard(usbd, USBD_REQUEST_GET_PORT_SET_INTERFACE_HANDLER(usbd), req);
      }

      if(USBD_BOOL_IS_TRUE(result))
      {
         set_result = USBD_DEV_Set_Interface(usbd, (uint8_t)(req->wIndex), (uint8_t)(req->wValue));

         if(USBD_DEV_SET_INTERFACE_RESULT_OK == set_result)
         {
            USBD_IOTP_EVENT_Send_Status(
               USBD_REQUEST_GET_EP0_IN_IOTP(usbd),
               USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
         }
         else if(USBD_DEV_SET_INTERFACE_RESULT_ALT_SETTING_NOT_SUPPORTED == set_result)
         {
            USBD_IOTP_EVENT_Send_Stall(USBD_REQUEST_GET_EP0_IN_IOTP(usbd));
         }
      }
   }
   else
   {
      (void)USBD_REQUEST_process_other_standard_requests(usbd, usbdc, 0, req);
   }

   USBD_EXIT_FUNC(USBD_DBG_REQ_PROCESSING);
} /* USBD_REQUEST_set_interface */

static void USBD_REQUEST_synch_frame(
      USBD_Params_XT *usbd,
      USBDC_Params_XT *usbdc,
      USBD_REQUEST_Req_DT *req,
      uint8_t destination)
{
   USBD_Bool_DT result = USBD_TRUE;

   USBD_ENTER_FUNC(USBD_DBG_REQ_PROCESSING);


   if((USBD_BMREQUESTTYPE_ENDPOINT == destination)
      && (USBD_DEV_STATE_CONFIGURED == (USBD_DEV_STATE_CONFIGURED & USBD_DEV_Get_State(usbd))))
   {
      if(USBD_REQUEST_CHECK_PORT_CBK_TAB_PTR(usbd))
      {
         result = USBD_REQUEST_port_guard(usbd, USBD_REQUEST_GET_PORT_SYNCH_FRAME_HANDLER(usbd), req);
      }

      if(USBD_BOOL_IS_TRUE(result))
      {
         /** TODO: implementation of sync frame */
      }
   }
   else
   {
      (void)USBD_REQUEST_process_other_standard_requests(usbd, usbdc, 0, req);
   }

   USBD_EXIT_FUNC(USBD_DBG_REQ_PROCESSING);
} /* USBD_REQUEST_synch_frame */

void USBD_REQUEST_Process_Req(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USBD_REQUEST_Req_DT *req)
{
#ifdef USBD_USE_IOCMD
   const char *name;
#endif
   USBDC_Params_XT *usbdc;
   USB_EP_Direction_ET dir;
   uint16_t number;
   uint8_t destination;

   USBD_ENTER_FUNC(USBD_DBG_REQ_PROCESSING);

   if((USBD_CHECK_PTR(USBD_Params_XT, usbd)) && (USBD_CHECK_PTR(USBD_REQUEST_Req_DT, req)))
   {
      USBD_REQUEST_fill_request(req);

      USBD_DEBUG_HI_1(USBD_DBG_REQ_PROCESSING, "new SETUP on EP: %d:", ep_num);
      USBD_DEBUG_HI_5(USBD_DBG_REQ_PROCESSING,
         "bmRequestType: 0x%02X(%d) = 7_dir(%s) | 65_type(%s) | 40_recipient(%s)",
         req->bmRequestType, req->bmRequestType,
         (USBD_BMREQUESTTYPE_OUT == (req->bmRequestType & USBD_BMREQUESTTYPE_DIRECTION_MASK)) ? "OUT" : "IN",
         (USBD_BMREQUESTTYPE_STANDARD == (req->bmRequestType & USBD_BMREQUESTTYPE_TYPE_MASK)) ? "STANDARD" :
         (
            (USBD_BMREQUESTTYPE_CLASS == (req->bmRequestType & USBD_BMREQUESTTYPE_TYPE_MASK)) ? "CLASS" :
            (
               (USBD_BMREQUESTTYPE_VENDOR == (req->bmRequestType & USBD_BMREQUESTTYPE_TYPE_MASK)) ? "VENDOR" : "RESERVED"
            )
         ),
         (USBD_BMREQUESTTYPE_DEVICE == (req->bmRequestType & USBD_BMREQUESTTYPE_RECIPIENT_MASK)) ? "DEVICE" :
         (
            (USBD_BMREQUESTTYPE_INTERFACE == (req->bmRequestType & USBD_BMREQUESTTYPE_RECIPIENT_MASK)) ? "INTERFACE" :
            (
               (USBD_BMREQUESTTYPE_ENDPOINT == (req->bmRequestType & USBD_BMREQUESTTYPE_RECIPIENT_MASK)) ? "ENDPOINT" :
               (
                  (USBD_BMREQUESTTYPE_OTHER == (req->bmRequestType & USBD_BMREQUESTTYPE_RECIPIENT_MASK)) ?
                     "OTHER" : "RESERVED"
               )
            )
         ));

      if(USBD_COMPILATION_SWITCH_LOG(USBD_DBG_REQ_PROCESSING, IOCMD_LOG_LEVEL_DEBUG_HI))
      {
         if(req->bRequest <= USBD_BREQUEST_SYNCH_FRAME)
         {
#ifdef USBD_USE_IOCMD
            name = (req->bRequest < Num_Elems(USBD_REQUEST_standard_req_desc)) ?
               USBD_REQUEST_standard_req_desc[req->bRequest] : USBD_REQUEST_standard_req_desc[2];
#endif
            USBD_DEBUG_HI_3(USBD_DBG_REQ_PROCESSING, "bRequest: 0x%02X(%d) => %s", req->bRequest, req->bRequest, name);
         }
         else
         {
            USBD_DEBUG_HI_2(USBD_DBG_REQ_PROCESSING, "bRequest: 0x%02X(%d)", req->bRequest, req->bRequest);
         }
      }

      USBD_DEBUG_HI_6(USBD_DBG_REQ_PROCESSING,
         "wValue: 0x%04X(%d), wIndex: 0x%04X(%d), wLength: 0x%04X(%d)",
         req->wValue, req->wValue,
         req->wIndex, req->wIndex,
         req->wLength, req->wLength);

      usbdc = USBD_DEV_GET_ACTIVE_CONFIG_PTR(usbd);

      if(0 == ep_num)
      {
         USBD_IOTP_EVENT_Abort(USBD_REQUEST_GET_EP0_IN_IOTP(usbd), USBD_TRUE);
         USBD_IOTP_EVENT_Abort(USBD_REQUEST_GET_EP0_OUT_IOTP(usbd), USBD_TRUE);

         USBD_IOTP_EVENT_Set_Handlers(
            USBD_REQUEST_GET_EP0_IN_IOTP(usbd),
            USBD_MAKE_INVALID_HANDLER(USBD_IOTP_EVENT_Callback_HT),
            USBD_MAKE_INVALID_HANDLER(USBD_IOTP_EVENT_Callback_HT),
            USBD_MAKE_INVALID_HANDLER(USBD_IOTP_EVENT_Callback_HT),
            USBD_MAKE_INVALID_HANDLER(USBD_IOTP_EVENT_Callback_HT));
         USBD_IOTP_EVENT_Set_Handlers(
            USBD_REQUEST_GET_EP0_OUT_IOTP(usbd),
            USBD_MAKE_INVALID_HANDLER(USBD_IOTP_EVENT_Callback_HT),
            USBD_MAKE_INVALID_HANDLER(USBD_IOTP_EVENT_Callback_HT),
            USBD_MAKE_INVALID_HANDLER(USBD_IOTP_EVENT_Callback_HT),
            USBD_MAKE_INVALID_HANDLER(USBD_IOTP_EVENT_Callback_HT));
      }

      if((USBD_BMREQUESTTYPE_STANDARD == (req->bmRequestType & USBD_BMREQUESTTYPE_TYPE_MASK))
         && (0 == ep_num))
      {
         destination = req->bmRequestType & USBD_BMREQUESTTYPE_DESTINATION_MASK;
         if(USB_EP_DESC_DIR_OUT == (req->wIndex & USB_EP_DESC_DIR_MASK))
         {
            dir = USB_EP_DIRECTION_OUT;
         }
         else
         {
            dir = USB_EP_DIRECTION_IN;
         }

         switch(req->bRequest)
         {
            case USBD_BREQUEST_GET_STATUS:
               USBD_REQUEST_get_status(usbd, usbdc, req, dir, destination);
               break;

            case USBD_BREQUEST_CLEAR_FEATURE:
               USBD_REQUEST_set_clear_feature(usbd, usbdc, req, dir, USBD_FALSE, destination);
               break;

            case USBD_BREQUEST_SET_FEATURE:
               USBD_REQUEST_set_clear_feature(usbd, usbdc, req, dir, USBD_TRUE, destination);
               break;

            case USBD_BREQUEST_SET_ADDRESS:
               USBD_REQUEST_set_address(usbd, usbdc, req, destination);
               break;

            case USBD_BREQUEST_GET_DESCRIPTOR:
               USBD_REQUEST_get_descrptor(usbd, usbdc, req, destination);
               break;

            case USBD_BREQUEST_SET_DESCRIPTOR:
               if(USBD_DEV_STATE_ADDRESSED == (USBD_DEV_STATE_ADDRESSED & USBD_DEV_Get_State(usbd)))
               {
                  (void)USBD_REQUEST_process_other_standard_requests(usbd, usbdc, 0, req);
               }
               break;

            case USBD_BREQUEST_GET_CONFIGURATION:
               USBD_REQUEST_get_configuration(usbd, usbdc, req, destination);
               break;

            case USBD_BREQUEST_SET_CONFIGURATION:
               USBD_REQUEST_set_configuration(usbd, usbdc, req, destination);
               break;

            case USBD_BREQUEST_GET_INTERFACE:
               USBD_REQUEST_get_interface(usbd, usbdc, req, destination);
               break;

            case USBD_BREQUEST_SET_INTERFACE:
               USBD_REQUEST_set_interface(usbd, usbdc, req, destination);
               break;

            case USBD_BREQUEST_SYNCH_FRAME:
               USBD_REQUEST_synch_frame(usbd, usbdc, req, destination);
               break;

            default:
               (void)USBD_REQUEST_process_other_standard_requests(usbd, usbdc, 0, req);
               break;
         }
      } /* if((USBD_BMREQUESTTYPE_STANDARD == (req->bmRequestType & USBD_BMREQUESTTYPE_TYPE_MASK)) */
      else
      {
         if(USBD_DEV_CHECK_ACTIVE_CONFIG_PTR(usbd))
         {
            /* Check if received request is Standard request */
            if(USBD_BMREQUESTTYPE_STANDARD == (req->bmRequestType & USBD_BMREQUESTTYPE_TYPE_MASK))
            {
               (void)USBD_REQUEST_process_other_standard_requests(usbd, usbdc, ep_num, req);
            }
            /* Check if received request is Class request */
            if(USBD_BMREQUESTTYPE_CLASS == (req->bmRequestType & USBD_BMREQUESTTYPE_TYPE_MASK))
            {
               number = req->wIndex & 0xFF;

               if(number < USBD_MAX_NUM_INTERFACES)
               {
                  if(USBDC_REQUEST_CHECK_INTERFACE_HANDLER(usbdc, number))
                  {
                     (void)USBDC_REQUEST_CALL_INTERFACE_IRQ(usbd, usbdc, ep_num, number, req);
                  }
               }
            }
            /* All other request types are processed by vendor irq */
            else
            {
               if(USBDC_REQUEST_CHECK_VENDOR_HANDLER(usbdc))
               {
                  (void)USBDC_REQUEST_CALL_VENDOR_IRQ(usbd, usbdc, ep_num, req);
               }
            }
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_REQ_PROCESSING);
} /* USBD_REQUEST_Process_Req */

void USBDC_REQUEST_Vendor_Irq_Install(
      USBDC_Params_XT *usbdc,
      USBD_REQUEST_Vendor_HT irq)
{
   USBD_ENTER_FUNC(USBD_DBG_REQ_INIT);

   if(USBD_CHECK_PTR(USBDC_Params_XT, usbdc))
   {
      USBDC_REQUEST_SET_VENDOR_HANDLER(usbdc, irq);
   }

   USBD_EXIT_FUNC(USBD_DBG_REQ_INIT);
} /* USBDC_REQUEST_Vendor_Irq_Install */

void USBDC_REQUEST_Interface_Irq_Install(
      USBDC_Params_XT *usbdc,
      USBD_REQUEST_Interface_HT irq,
      uint8_t interface_num)
{
   USBD_ENTER_FUNC(USBD_DBG_REQ_INIT);

   if(USBD_CHECK_PTR(USBDC_Params_XT, usbdc))
   {
      if(interface_num < USBD_MAX_NUM_INTERFACES)
      {
         USBD_DEBUG_HI_1(USBD_DBG_REQ_INIT, "install irq for if: %d", interface_num);

         USBDC_REQUEST_SET_INTERFACE_HANDLER(usbdc, interface_num, irq);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_REQ_INIT);
} /* USBDC_REQUEST_Interface_Irq_Install */

void USBDC_REQUEST_Set_EP_Halt_Mask(
      USBDC_Params_XT *usbdc,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_Bool_DT state)
{
   USBD_ENTER_FUNC(USBD_DBG_REQ_INIT);

   if((USBD_CHECK_PTR(USBDC_Params_XT, usbdc)) && (ep_num < USBD_MAX_NUM_ENDPOINTS))
   {
      if(USB_EP_DIRECTION_OUT == dir)
      {
         if(USBD_BOOL_IS_TRUE(state))
         {
            USBDC_REQUEST_SET_EP_OUT_HALT_MASK(usbdc, ep_num);
         }
         else
         {
            USBDC_REQUEST_CLEAR_EP_OUT_HALT_MASK(usbdc, ep_num);
         }
      }
      else
      {
         if(USBD_BOOL_IS_TRUE(state))
         {
            USBDC_REQUEST_SET_EP_IN_HALT_MASK(usbdc, ep_num);
         }
         else
         {
            USBDC_REQUEST_CLEAR_EP_IN_HALT_MASK(usbdc, ep_num);
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_REQ_INIT);
} /* USBDC_REQUEST_Set_EP_Halt_Mask */

#ifdef USBD_REQUEST_POST_IMP_INCLUDE
#include "usbd_request_post_imp.h"
#endif
