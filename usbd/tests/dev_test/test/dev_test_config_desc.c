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

#include "dev_test_configs.h"
#ifndef USBD_DEV_INTERNAL_H_
#include "usbd_dev_internal.h"
#endif

#include "cfg.h"
#include "main_get_exe.h"

static void test_activate_deactivate(USBD_Params_XT* usbd, USBD_Bool_DT state);
static USBD_Bool_DT test_parse_ep(USBD_Params_XT* usbd, uint8_t ep_num, const USB_Interface_Desc_DT *if_desc, const USB_Endpoint_Desc_DT *ep_desc);
static USBD_Bool_DT test_parse_cfg(USBD_Params_XT* usbd, const uint8_t *desc, uint16_t desc_size, USBD_DEV_Config_Desc_Check_Result_XT *details);
static USBD_DEV_Speed_ET test_get_supported_speed(USBD_Params_XT *usbd);
static USBD_DEV_Speed_ET test_get_current_speed(USBD_Params_XT *usbd);
static uint16_t test_get_dev_status(USBD_Params_XT *usbd);
static uint16_t test_get_frame_nr(USBD_Params_XT *usbd);
static USBD_DEV_PORT_Get_EP_Desc_Result_DT test_get_ep0_low_full_speed(USBD_Params_XT* usbd);
static USBD_DEV_PORT_Get_EP_Desc_Result_DT test_get_ep0_high_speed(USBD_Params_XT* usbd);

USBDC_Params_XT usbdc1;

USBD_DEV_Interface_Header_XT if_header[USBD_MAX_NUM_INTERFACES];

USBD_DEV_Installation_Result_XT install_result_1;

bool remove_result;

//USB_Configuration_Desc_DT desc1;




static uint8_t desc1[] =
{
      /* bLength             */ sizeof(USB_Configuration_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_CONFIGURATION,
      /* wTotalLength.L      */ (uint8_t)(0 & 0xFF),
      /* wTotalLength.H      */ (uint8_t)((0 >> 8) & 0xFF),
      /* bNumInterfaces      */ 3,
      /* bConfigurationValue */ 0,
      /* iConfiguration      */ 0,
      /* bmAttributes        */ 0x40,
      /* bMaxPower           */ 50,

      /* bLength             */ sizeof(USB_Interface_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_INTERFACE,
      /* bInterfaceNumber    */ 0,
      /* bAlternateSetting   */ 0,
      /* bNumEndpoints       */ 2,
      /* bInterfaceClass     */ 1,
      /* bInterfaceSubClass  */ 2,
      /* bInterfaceProtocol  */ 3,
      /* iInterface          */ 0,

      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_IN | 1,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* wMaxPacketSize.L    */ 8,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF,

      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_OUT | 2,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* wMaxPacketSize.L    */ 8,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF,

      /* bLength             */ sizeof(USB_Interface_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_INTERFACE,
      /* bInterfaceNumber    */ 0,
      /* bAlternateSetting   */ 1,
#if 0
      /* bNumEndpoints       */ 2,
#else
      /* bNumEndpoints       */ 1,
#endif
      /* bInterfaceClass     */ 1,
      /* bInterfaceSubClass  */ 2,
      /* bInterfaceProtocol  */ 3,
      /* iInterface          */ 0,
#if 0
      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_IN | 1,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* wMaxPacketSize.L    */ 64,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF,
#endif
      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_OUT | 2,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* wMaxPacketSize.L    */ 8,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF,

      /* bLength             */ sizeof(USB_Interface_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_INTERFACE,
      /* bInterfaceNumber    */ 0,
      /* bAlternateSetting   */ 2,
      /* bNumEndpoints       */ 2,
      /* bInterfaceClass     */ 1,
      /* bInterfaceSubClass  */ 2,
      /* bInterfaceProtocol  */ 3,
      /* iInterface          */ 0,
#if 1
      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_IN | 1,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* wMaxPacketSize.L    */ 0,
      /* wMaxPacketSize.H    */ 2,
      /* bInterval           */ 0xFF,
#endif
      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_OUT | 2,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* wMaxPacketSize.L    */ 0,
      /* wMaxPacketSize.H    */ 1,
      /* bInterval           */ 0xFF,

      /* bLength             */ sizeof(USB_Interface_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_INTERFACE,
      /* bInterfaceNumber    */ 1,
      /* bAlternateSetting   */ 0,
      /* bNumEndpoints       */ 0,
      /* bInterfaceClass     */ 6,
      /* bInterfaceSubClass  */ 3,
      /* bInterfaceProtocol  */ 11,
      /* iInterface          */ 0,

      /* bLength             */ sizeof(USB_Interface_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_INTERFACE,
      /* bInterfaceNumber    */ 2,
      /* bAlternateSetting   */ 0,
      /* bNumEndpoints       */ 3,
      /* bInterfaceClass     */ 1,
      /* bInterfaceSubClass  */ 2,
      /* bInterfaceProtocol  */ 3,
      /* iInterface          */ 0,

      /* other type descriptor */
      4,
      12,
      2,
      3,

      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_IN | 3,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* wMaxPacketSize.L    */ 64,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF,

      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_OUT | 4,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* wMaxPacketSize.L    */ 8,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF,

      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_OUT | 5,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS,
      /* wMaxPacketSize.L    */ 0,
      /* wMaxPacketSize.H    */ 4,
      /* bInterval           */ 0xFF
};

static uint8_t activity = 0;
static USB_Endpoint_Desc_DT ep0_full =
{
   .bLength             = USB_DESC_TYPE_ENDPOINT_SIZE,
   .bDescriptorType     = USB_DESC_TYPE_ENDPOINT,
   .bEndpointAddress    = 0,
   .bmAttributes        = USB_EP_DESC_TRANSFER_TYPE_CONTROL,
   .wMaxPacketSize.L    = TEST_EP0_MPS,
   .wMaxPacketSize.H    = 0,
   .bInterval           = 0xFF,
};
static USB_Endpoint_Desc_DT ep0_high =
{
   .bLength             = USB_DESC_TYPE_ENDPOINT_SIZE,
   .bDescriptorType     = USB_DESC_TYPE_ENDPOINT,
   .bEndpointAddress    = 0,
   .bmAttributes        = USB_EP_DESC_TRANSFER_TYPE_CONTROL,
   .wMaxPacketSize.L    = TEST_EP0_MPS,
   .wMaxPacketSize.H    = 0,
   .bInterval           = 0xFF,
};
static USBD_DEV_Speed_ET speed = USBD_DEV_FULL_AND_HIGH_SPEED;
static uint16_t device_status = 0;
static uint16_t frame_nr = 0;
static USBD_DEV_Port_Handler_XT port =
{
   .handlers.activate               = test_activate_deactivate,
   .handlers.ep_parse               = test_parse_ep,
   .handlers.cfg_parse              = test_parse_cfg,
   .handlers.get_supported_speed    = test_get_supported_speed,
   .handlers.get_current_speed      = test_get_current_speed,
   .handlers.get_device_status      = test_get_dev_status,
   .handlers.get_frame_num          = test_get_frame_nr,
   .handlers.get_ep0_low_full_speed = test_get_ep0_low_full_speed,
   .handlers.get_ep0_high_speed     = test_get_ep0_high_speed
};

static bool test_active = false;
static uint8_t test_state = 0;



static void report_error(void)
{
   char ec;

   printf("some error occured!\n\rpress any key to continue... ");
   ec = getc(stdin);
} /* report_error */

static uint8_t working_buf[IOCMD_WORKING_BUF_RECOMMENDED_SIZE];

#define REPORT_ERROR()\
   IOCMD_Proc_Buffered_Logs(true, main_get_exe(), working_buf, sizeof(working_buf));\
   printf("file: %s\nline: %d\n", __FILE__, __LINE__);\
   report_error()



static void test_activate_deactivate(USBD_Params_XT* usbd, USBD_Bool_DT state)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   if(state)
   {
      activity = 5;
   }
   else
   {
      activity = 11;
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

static USBD_Bool_DT test_parse_ep(USBD_Params_XT* usbd, uint8_t ep_num, const USB_Interface_Desc_DT *if_desc, const USB_Endpoint_Desc_DT *ep_desc)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   if(USBD_CHECK_PTR(USB_Endpoint_Desc_DT, ep_desc))
   {
      printf("EP: %d; dir: %s; MPS: %d; Attr: 0x%X; Interval: 0x%X",
            ep_desc->bEndpointAddress & 0x0F,
            (USB_EP_DESC_DIR_IN == (ep_desc->bEndpointAddress & USB_EP_DESC_DIR_MASK)) ? "IN" : "OUT",
            (ep_desc->wMaxPacketSize.H * 256) + ep_desc->wMaxPacketSize.L,
            ep_desc->bmAttributes,
            ep_desc->bInterval);
   }

   if(USBD_CHECK_PTR(USB_Interface_Desc_DT, if_desc))
   {
      printf(" Interface: %d; Alt: %d\n\r", if_desc->bInterfaceNumber, if_desc->bAlternateSetting);
   }
   else
   {
      printf(" NO INTERFACE\n\r");
   }

   if(test_active)
   {
      if(USBD_CHECK_PTR(USB_Endpoint_Desc_DT, ep_desc) && USBD_CHECK_PTR(USB_Interface_Desc_DT, if_desc))
      {
         if(0 == test_state)
         {
            test_state = 1;
            if(!((1 == (ep_desc->bEndpointAddress & 0x0F)) && (0 == if_desc->bInterfaceNumber) && (0 == if_desc->bAlternateSetting)))
            {
               printf("incorrect conditions: EP_addr: %d; if_num: %d; if_alt: %d\n\r", (ep_desc->bEndpointAddress & 0x0F), if_desc->bInterfaceNumber, if_desc->bAlternateSetting);
               REPORT_ERROR();
            }
         }
         else if(1 == test_state)
         {
            test_state = 2;
            if(!((2 == (ep_desc->bEndpointAddress & 0x0F)) && (0 == if_desc->bInterfaceNumber) && (0 == if_desc->bAlternateSetting)))
            {
               printf("incorrect conditions: EP_addr: %d; if_num: %d; if_alt: %d\n\r", (ep_desc->bEndpointAddress & 0x0F), if_desc->bInterfaceNumber, if_desc->bAlternateSetting);
               REPORT_ERROR();
            }
         }
         else if(2 == test_state)
         {
            test_state = 3;
            if(!((2 == (ep_desc->bEndpointAddress & 0x0F)) && (0 == if_desc->bInterfaceNumber) && (1 == if_desc->bAlternateSetting)))
            {
               printf("incorrect conditions: EP_addr: %d; if_num: %d; if_alt: %d\n\r", (ep_desc->bEndpointAddress & 0x0F), if_desc->bInterfaceNumber, if_desc->bAlternateSetting);
               REPORT_ERROR();
            }
         }
         else if(3 == test_state)
         {
            test_state = 4;
            if(!((1 == (ep_desc->bEndpointAddress & 0x0F)) && (0 == if_desc->bInterfaceNumber) && (2 == if_desc->bAlternateSetting)))
            {
               printf("incorrect conditions: EP_addr: %d; if_num: %d; if_alt: %d\n\r", (ep_desc->bEndpointAddress & 0x0F), if_desc->bInterfaceNumber, if_desc->bAlternateSetting);
               REPORT_ERROR();
            }
         }
         else if(4 == test_state)
         {
            test_state = 5;
            if(!((2 == (ep_desc->bEndpointAddress & 0x0F)) && (0 == if_desc->bInterfaceNumber) && (2 == if_desc->bAlternateSetting)))
            {
               printf("incorrect conditions: EP_addr: %d; if_num: %d; if_alt: %d\n\r", (ep_desc->bEndpointAddress & 0x0F), if_desc->bInterfaceNumber, if_desc->bAlternateSetting);
               REPORT_ERROR();
            }
         }
         else if(5 == test_state)
         {
            test_state = 6;
            if(!((3 == (ep_desc->bEndpointAddress & 0x0F)) && (2 == if_desc->bInterfaceNumber) && (0 == if_desc->bAlternateSetting)))
            {
               printf("incorrect conditions: EP_addr: %d; if_num: %d; if_alt: %d\n\r", (ep_desc->bEndpointAddress & 0x0F), if_desc->bInterfaceNumber, if_desc->bAlternateSetting);
               REPORT_ERROR();
            }
         }
         else if(6 == test_state)
         {
            test_state = 7;
            if(!((4 == (ep_desc->bEndpointAddress & 0x0F)) && (2 == if_desc->bInterfaceNumber) && (0 == if_desc->bAlternateSetting)))
            {
               printf("incorrect conditions: EP_addr: %d; if_num: %d; if_alt: %d\n\r", (ep_desc->bEndpointAddress & 0x0F), if_desc->bInterfaceNumber, if_desc->bAlternateSetting);
               REPORT_ERROR();
            }
         }
         else if(7 == test_state)
         {
            test_state = 8;
            if(!((5 == (ep_desc->bEndpointAddress & 0x0F)) && (2 == if_desc->bInterfaceNumber) && (0 == if_desc->bAlternateSetting)))
            {
               printf("incorrect conditions: EP_addr: %d; if_num: %d; if_alt: %d\n\r", (ep_desc->bEndpointAddress & 0x0F), if_desc->bInterfaceNumber, if_desc->bAlternateSetting);
               REPORT_ERROR();
            }
         }
         else
         {
            printf("parse ep port handler called in forbidden state! %d\n\r", test_state);
            REPORT_ERROR();
         }
      }
      else
      {
         if(!USBD_CHECK_PTR(USB_Endpoint_Desc_DT, ep_desc))
         {
            printf("invalid ep descriptor\n\r");
         }
         else
         {
            printf("invalid interface descriptor\n\r");
         }
         REPORT_ERROR();
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return USBD_TRUE;
}

static USBD_Bool_DT test_parse_cfg(USBD_Params_XT* usbd, const uint8_t *desc, uint16_t desc_size, USBD_DEV_Config_Desc_Check_Result_XT *details)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return USBD_TRUE;
}

static USBD_DEV_Speed_ET test_get_supported_speed(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return USBD_DEV_FULL_AND_HIGH_SPEED;
}

static USBD_DEV_Speed_ET test_get_current_speed(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return speed;
}

static uint16_t test_get_dev_status(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return device_status;
}

static uint16_t test_get_frame_nr(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   frame_nr++;
   frame_nr &= 0x7FF;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return frame_nr;
}

static USBD_DEV_PORT_Get_EP_Desc_Result_DT test_get_ep0_low_full_speed(USBD_Params_XT* usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return (&ep0_full);
}

static USBD_DEV_PORT_Get_EP_Desc_Result_DT test_get_ep0_high_speed(USBD_Params_XT* usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return (&ep0_high);
}

void USBD_IO_Perform_Halt(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      bool state)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   printf("USBD_IO_perform_halt: %d, state: %d\n\r", ep_num, state);

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

void USBD_IO_EP_Enable_And_Configure(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir)
{
   const USB_Endpoint_Desc_DT *ep_desc;

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   ep_desc = USBD_DEV_Get_EP_Desc(usbd, ep_num, dir);

   printf("USBD_IO_ep_enable_and_configure: ep_num = %d, dir = %s\n\r", ep_num, (USB_EP_DIRECTION_OUT == dir) ? "USB_EP_DIRECTION_OUT" : "USB_EP_DIRECTION_IN");
   if(USBD_MAKE_INVALID_PTR(USB_Endpoint_Desc_DT) == ep_desc)
   {
      printf("Invalid EP descriptor\n\r");
      REPORT_ERROR();
   }
   else if(test_active)
   {
      if(USBD_CHECK_PTR(USB_Endpoint_Desc_DT, ep_desc))
      {
         if(8 == test_state)
         {
            test_state = 9;
            if(1 != (ep_desc->bEndpointAddress & 0x0F))
            {
               printf("incorrect conditions: EP_addr: %d\n\r", (ep_desc->bEndpointAddress & 0x0F));
               REPORT_ERROR();
            }
            if(0 != USBD_DEV_Get_EP_Interface_Num(usbd, ep_num, dir))
            {
               printf("incorrect conditions: If Owner: %d\n\r", USBD_DEV_Get_EP_Interface_Num(usbd, ep_num, dir));
               REPORT_ERROR();
            }
         }
         else if(9 == test_state)
         {
            test_state = 10;
            if(2 != (ep_desc->bEndpointAddress & 0x0F))
            {
               printf("incorrect conditions: EP_addr: %d\n\r", (ep_desc->bEndpointAddress & 0x0F));
               REPORT_ERROR();
            }
            if(0 != USBD_DEV_Get_EP_Interface_Num(usbd, ep_num, dir))
            {
               printf("incorrect conditions: If Owner: %d\n\r", USBD_DEV_Get_EP_Interface_Num(usbd, ep_num, dir));
               REPORT_ERROR();
            }
         }
         else if(10 == test_state)
         {
            test_state = 11;
            if(3 != (ep_desc->bEndpointAddress & 0x0F))
            {
               printf("incorrect conditions: EP_addr: %d\n\r", (ep_desc->bEndpointAddress & 0x0F));
               REPORT_ERROR();
            }
            if(2 != USBD_DEV_Get_EP_Interface_Num(usbd, ep_num, dir))
            {
               printf("incorrect conditions: If Owner: %d\n\r", USBD_DEV_Get_EP_Interface_Num(usbd, ep_num, dir));
               REPORT_ERROR();
            }
         }
         else if(11 == test_state)
         {
            test_state = 12;
            if(4 != (ep_desc->bEndpointAddress & 0x0F))
            {
               printf("incorrect conditions: EP_addr: %d\n\r", (ep_desc->bEndpointAddress & 0x0F));
               REPORT_ERROR();
            }
            if(2 != USBD_DEV_Get_EP_Interface_Num(usbd, ep_num, dir))
            {
               printf("incorrect conditions: If Owner: %d\n\r", USBD_DEV_Get_EP_Interface_Num(usbd, ep_num, dir));
               REPORT_ERROR();
            }
         }
         else if(12 == test_state)
         {
            test_state = 13;
            if(5 != (ep_desc->bEndpointAddress & 0x0F))
            {
               printf("incorrect conditions: EP_addr: %d\n\r", (ep_desc->bEndpointAddress & 0x0F));
               REPORT_ERROR();
            }
            if(2 != USBD_DEV_Get_EP_Interface_Num(usbd, ep_num, dir))
            {
               printf("incorrect conditions: If Owner: %d\n\r", USBD_DEV_Get_EP_Interface_Num(usbd, ep_num, dir));
               REPORT_ERROR();
            }
         }
         else if(15 == test_state)
         {
            test_state = 16;
            if(2 != (ep_desc->bEndpointAddress & 0x0F))
            {
               printf("incorrect conditions: EP_addr: %d\n\r", (ep_desc->bEndpointAddress & 0x0F));
               REPORT_ERROR();
            }
         }
         else
         {
            printf("\"USBD_IO_ep_enable_and_configure\" called in forbidden state! %d\n\r", test_state);
            REPORT_ERROR();
         }
      }
      else
      {
         printf("invalid ep descriptor\n\r");
         REPORT_ERROR();
      }
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

void USBD_IO_EP_Disable(
      USBD_Params_XT *usbd,
      uint8_t ep_num)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   printf("USBD_IO_ep_disable: %d\n\r", ep_num);
   if(test_active)
   {
      if(13 == test_state)
      {
         test_state = 14;
         if(1 != ep_num)
         {
            printf("incorrect EP number: %d\n\r", ep_num);
            REPORT_ERROR();
         }
      }
      else if(14 == test_state)
      {
         test_state = 15;
         if(2 != ep_num)
         {
            printf("incorrect EP number: %d\n\r", ep_num);
            REPORT_ERROR();
         }
      }
      else
      {
         printf("\"USBD_IO_ep_disable\" called in forbidden state! %d\n\r", test_state);
         REPORT_ERROR();
      }
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}



static void test_prepare(USBD_Params_XT *usbd)
{
   USBD_DEV_Config_Header_XT *config_table = USBD_DEV_GET_CONFIG_TAB_PTR(usbd);
   USBD_DEV_Interface_Header_XT *if_header = USBD_DEV_GET_INTERFACE_TAB_PTR(usbd);

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_Init(usbd);
   USBDC_Init(&usbdc1);

   desc1[2] = sizeof(desc1) & 0xFF;
   desc1[3] = (sizeof(desc1) >> 8) & 0xFF;

   USBDC_Bind_Config_Desc(&usbdc1, desc1);

   printf("size of desc1: %d\n\r", sizeof(desc1));

   if(0 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   install_result_1 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc1);

   if(1 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   if((config_table[0].usbdc != &usbdc1)
     || (config_table[0].config_desc != (uint8_t*)(&desc1))
     || (0 != install_result_1.index)
     || (USBD_DEV_INSTALLATION_OK != install_result_1.result))
   {
      printf("config_table[0] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 0) != &usbdc1)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 0) != (uint8_t*)(&desc1))
     || (0 != install_result_1.index)
     || (USBD_DEV_INSTALLATION_OK != install_result_1.result))
   {
      printf("config_table[0] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   activity = 0;
   USBD_DEV_Activate(usbd, &port);

   if(USBD_DEV_STATE_OFF != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Activate(usbd)!\n\r");
      REPORT_ERROR();
   }

   if(activity != 5)
   {
      printf("port method \"activate\" didn't called!\n\r");
      REPORT_ERROR();
   }

   activity = 0;

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}


void test_config_desc(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   test_prepare(usbd);

   USBD_DEV_Reset(usbd);

   test_active = true;
   USBD_DEV_Configured(usbd, 1, USBD_MAKE_INVALID_HANDLER(USBD_DEV_Set_Configuration_Respond_HT));

   USBD_DEV_Set_Interface(usbd, 0, 1);

   test_active = false;

   USBD_DEV_Deactivate(usbd);

   if(USBD_DEV_STATE_OFF != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Deactivate(usbd)!\n\r");
      REPORT_ERROR();
   }

   if(activity != 11)
   {
      printf("port method \"activate\" didn't called!\n\r");
      REPORT_ERROR();
   }
   activity = 0;

   remove_result = USBD_DEV_Remove_All_Configs(usbd);

   if((0 != USBD_DEV_Get_Num_Installed_Configs(usbd)) || (!remove_result))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);
} /* test_configs */
