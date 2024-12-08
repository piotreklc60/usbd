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

static USBDC_Params_XT usbdc1;
static USBDC_Params_XT usbdc2;
static USBDC_Params_XT usbdc3;
static USBDC_Params_XT usbdc4;
static USBDC_Params_XT usbdc5;
static USBDC_Params_XT usbdc6;

static USBD_DEV_Installation_Result_XT install_result_1;
static USBD_DEV_Installation_Result_XT install_result_2;
static USBD_DEV_Installation_Result_XT install_result_3;
static USBD_DEV_Installation_Result_XT install_result_4;
static USBD_DEV_Installation_Result_XT install_result_5;
static USBD_DEV_Installation_Result_XT install_result_6;

static bool remove_result;

static USB_Configuration_Desc_DT desc1;
static USB_Configuration_Desc_DT desc2;
static USB_Configuration_Desc_DT desc3;
static USB_Configuration_Desc_DT desc4;
static USB_Configuration_Desc_DT desc5;
static USB_Configuration_Desc_DT desc6;


static uint8_t desc[] =
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
      /* iInterface           */ 0,

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
      /* bNumEndpoints       */ 2,
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

static void test_activate_deactivate(USBD_Params_XT* usbd, USBD_Bool_DT state)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   if(USBD_BOOL_IS_TRUE(state))
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









static void set_dummy_cfg(USB_Configuration_Desc_DT *desc, uint8_t config_id)
{
   desc->bLength = USB_DESC_TYPE_CONFIGURATION_SIZE;
   desc->bDescriptorType = USB_DESC_TYPE_CONFIGURATION;
   desc->wTotalLength.L = USB_DESC_TYPE_CONFIGURATION_SIZE;
   desc->wTotalLength.H = 0;
   desc->bNumInterfaces = 0;
   desc->bConfigurationValue = config_id;
   desc->iConfiguration = 0;
   desc->bmAttributes = 0x40;
   desc->bMaxPower = 50;
}

void test_configs(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_Init(usbd);

   USBDC_Init(&usbdc1);
   USBDC_Init(&usbdc2);
   USBDC_Init(&usbdc3);
   USBDC_Init(&usbdc4);
   USBDC_Init(&usbdc5);
   USBDC_Init(&usbdc6);

   memset(&desc1, 0, sizeof(desc1));
   memset(&desc2, 0, sizeof(desc2));
   memset(&desc3, 0, sizeof(desc3));
   memset(&desc4, 0, sizeof(desc4));
   memset(&desc5, 0, sizeof(desc5));
   memset(&desc6, 0, sizeof(desc6));

   USBDC_Bind_Config_Desc(&usbdc1, (const uint8_t *)(&desc1));
   USBDC_Bind_Config_Desc(&usbdc2, (const uint8_t *)(&desc2));
   USBDC_Bind_Config_Desc(&usbdc3, (const uint8_t *)(&desc3));
   USBDC_Bind_Config_Desc(&usbdc4, (const uint8_t *)(&desc4));
   USBDC_Bind_Config_Desc(&usbdc5, (const uint8_t *)(&desc5));
   USBDC_Bind_Config_Desc(&usbdc6, (const uint8_t *)(&desc6));

   if(0 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   install_result_1 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc1);
   install_result_2 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc2);
   install_result_3 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc3);
   install_result_4 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc4);
   install_result_5 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc5);
   install_result_6 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc6);

   if(0 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   set_dummy_cfg(&desc1, 1);
   set_dummy_cfg(&desc2, 2);
   set_dummy_cfg(&desc3, 3);
   set_dummy_cfg(&desc4, 4);
   set_dummy_cfg(&desc5, 5);
   set_dummy_cfg(&desc6, 6);

   install_result_1 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc1);
   install_result_2 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc2);
   install_result_3 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc3);
   install_result_4 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc4);
   install_result_5 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc5);
   install_result_6 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc6);

   if(5 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs\n\r");
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

   if((USBD_DEV_Get_Config(usbd, 1) != &usbdc2)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 1) != (uint8_t*)(&desc2))
     || (1 != install_result_2.index)
     || (USBD_DEV_INSTALLATION_OK != install_result_2.result))
   {
      printf("config_table[1] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 2) != &usbdc3)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 2) != (uint8_t*)(&desc3))
     || (2 != install_result_3.index)
     || (USBD_DEV_INSTALLATION_OK != install_result_3.result))
   {
      printf("config_table[2] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 3) != &usbdc4)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 3) != (uint8_t*)(&desc4))
     || (3 != install_result_4.index)
     || (USBD_DEV_INSTALLATION_OK != install_result_4.result))
   {
      printf("config_table[3] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 4) != &usbdc5)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 4) != (uint8_t*)(&desc5))
     || (4 != install_result_5.index)
     || (USBD_DEV_INSTALLATION_OK != install_result_5.result))
   {
      printf("config_table[4] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   if(5 == install_result_6.index)
   {
      printf("config_table[5] does not exist and cannot be installed\n\r");
      REPORT_ERROR();
   }

   remove_result = USBD_DEV_Remove_All_Configs(usbd);

   if((0 != USBD_DEV_Get_Num_Installed_Configs(usbd)) || (!remove_result))
   {
      printf("incorrect number of installed configs\n\r");
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

   install_result_1 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc1);

   if(1 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs\n\r");
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

   install_result_2 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc2);

   if(2 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 0) != &usbdc1)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 0) != (uint8_t*)(&desc1)))
   {
      printf("config_table[0] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 1) != &usbdc2)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 1) != (uint8_t*)(&desc2))
     || (1 != install_result_2.index)
     || (USBD_DEV_INSTALLATION_OK != install_result_2.result))
   {
      printf("config_table[1] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   install_result_3 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc3);

   if(3 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 0) != &usbdc1)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 0) != (uint8_t*)(&desc1)))
   {
      printf("config_table[0] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 1) != &usbdc2)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 1) != (uint8_t*)(&desc2)))
   {
      printf("config_table[1] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 2) != &usbdc3)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 2) != (uint8_t*)(&desc3))
     || (2 != install_result_3.index)
     || (USBD_DEV_INSTALLATION_OK != install_result_3.result))
   {
      printf("config_table[2] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   remove_result = USBD_DEV_Remove_Config(usbd, &usbdc2);

   if((2 != USBD_DEV_Get_Num_Installed_Configs(usbd)) || (!remove_result))
   {
      printf("incorrect number of installed configs, %d\n\r", USBD_DEV_Get_Num_Installed_Configs(usbd));
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 0) != &usbdc1)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 0) != (uint8_t*)(&desc1)))
   {
      printf("config_table[0] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 2) != &usbdc3)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 2) != (uint8_t*)(&desc3)))
   {
      printf("config_table[2] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   remove_result = USBD_DEV_Remove_Config(usbd, &usbdc2);

   if((2 != USBD_DEV_Get_Num_Installed_Configs(usbd)) || (remove_result))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 0) != &usbdc1)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 0) != (uint8_t*)(&desc1)))
   {
      printf("config_table[0] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 2) != &usbdc3)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 2) != (uint8_t*)(&desc3)))
   {
      printf("config_table[1] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   remove_result = USBD_DEV_Remove_Config(usbd, &usbdc1);

   if((1 != USBD_DEV_Get_Num_Installed_Configs(usbd)) || (!remove_result))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 2) != &usbdc3)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 2) != (uint8_t*)(&desc3)))
   {
      printf("config_table[0] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   remove_result = USBD_DEV_Remove_Config(usbd, &usbdc3);

   if((0 != USBD_DEV_Get_Num_Installed_Configs(usbd)) || (!remove_result))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   remove_result = USBD_DEV_Remove_Config(usbd, &usbdc3);

   if((0 != USBD_DEV_Get_Num_Installed_Configs(usbd)) || (remove_result))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   USBDC_Bind_Config_Desc(&usbdc1, (const uint8_t *)(&desc3));
   USBDC_Bind_Config_Desc(&usbdc2, (const uint8_t *)(&desc1));
   USBDC_Bind_Config_Desc(&usbdc3, (const uint8_t *)(&desc2));

   desc2.bConfigurationValue = 1;
   desc3.bConfigurationValue = 2;
   desc1.bConfigurationValue = 3;

   install_result_1 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc3);
   install_result_2 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc1);
   install_result_3 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc2);

   if(3 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 0) != &usbdc3)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 0) != (uint8_t*)(&desc2))
     || (0 != install_result_1.index)
     || (USBD_DEV_INSTALLATION_OK != install_result_1.result))
   {
      printf("config_table[0] is not same which was installed, installation result: %d\n\r", install_result_1.index);
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 1) != &usbdc1)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 1) != (uint8_t*)(&desc3))
     || (1 != install_result_2.index)
     || (USBD_DEV_INSTALLATION_OK != install_result_2.result))
   {
      printf("config_table[1] is not same which was installed, installation result: %d\n\r", install_result_2.index);
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 2) != &usbdc2)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 2) != (uint8_t*)(&desc1))
     || (2 != install_result_3.index)
     || (USBD_DEV_INSTALLATION_OK != install_result_3.result))
   {
      printf("config_table[2] is not same which was installed, installation result: %d\n\r", install_result_3.index);
      REPORT_ERROR();
   }

   remove_result = USBD_DEV_Remove_All_Configs(usbd);

   if((0 != USBD_DEV_Get_Num_Installed_Configs(usbd)) || (!remove_result))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   USBDC_Bind_Config_Desc(&usbdc1, (const uint8_t *)(&desc1));
   USBDC_Bind_Config_Desc(&usbdc2, (const uint8_t *)(&desc2));
   USBDC_Bind_Config_Desc(&usbdc3, (const uint8_t *)(&desc3));

   desc1.bConfigurationValue = 1;
   desc2.bConfigurationValue = 2;
   desc3.bConfigurationValue = 3;

   install_result_1 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc1);
   install_result_2 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc2);
   install_result_3 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc3);

   if(3 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs\n\r");
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

   if((USBD_DEV_Get_Config(usbd, 1) != &usbdc2)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 1) != (uint8_t*)(&desc2))
     || (1 != install_result_2.index)
     || (USBD_DEV_INSTALLATION_OK != install_result_2.result))
   {
      printf("config_table[1] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 2) != &usbdc3)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 2) != (uint8_t*)(&desc3))
     || (2 != install_result_3.index)
     || (USBD_DEV_INSTALLATION_OK != install_result_3.result))
   {
      printf("config_table[2] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   USBD_DEV_Reset(usbd);

   remove_result = USBD_DEV_Remove_All_Configs(usbd);

   if((3 != USBD_DEV_Get_Num_Installed_Configs(usbd)) || (remove_result))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 0) != &usbdc1)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 0) != (uint8_t*)(&desc1)))
   {
      printf("config_table[0] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 1) != &usbdc2)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 1) != (uint8_t*)(&desc2)))
   {
      printf("config_table[1] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 2) != &usbdc3)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 2) != (uint8_t*)(&desc3)))
   {
      printf("config_table[2] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   USBD_DEV_Deactivate(usbd);

   if(USBD_DEV_STATE_OFF != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Deactivate(usbd)!\n\r");
      REPORT_ERROR();
   }

   if(activity != 11)
   {
      printf("port methid \"activate\" didn't called!\n\r");
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
