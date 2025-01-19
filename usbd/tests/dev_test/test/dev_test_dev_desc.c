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

#include "dev_test_dev_desc.h"
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

static USBD_DEV_Installation_Result_XT config_install_result_1;
static USBD_DEV_Installation_Result_XT config_install_result_2;
static USBD_DEV_Installation_Result_XT config_install_result_3;

static USB_Configuration_Desc_DT desc1;
static USB_Configuration_Desc_DT desc2;
static USB_Configuration_Desc_DT desc3;

uint8_t iString1;
uint8_t iString2;

static USBD_DEV_Installation_Result_XT string_install_result_1;
static USBD_DEV_Installation_Result_XT string_install_result_2;
static USBD_DEV_Installation_Result_XT string_install_result_manuf;
static USBD_DEV_Installation_Result_XT string_install_result_prod;
static USBD_DEV_Installation_Result_XT string_install_result_ser;

static bool remove_result;

static uint16_t string1[] =
{
   USB_STRING_COMPOSE_LENGTH_AND_TYPE(sizeof("string1")),
   's','t','r','i','n','g','1'
};
static uint16_t string2[] =
{
   USB_STRING_COMPOSE_LENGTH_AND_TYPE(sizeof("string2")),
   's','t','r','i','n','g','2'
};
static uint16_t manuf[] =
{
   USB_STRING_COMPOSE_LENGTH_AND_TYPE(sizeof("USBD Group")),
   'U','S','B','D',' ','G','r','o','u','p'
};
static uint16_t prod[] =
{
   USB_STRING_COMPOSE_LENGTH_AND_TYPE(sizeof("USBD DEV")),
   'U','S','B','D',' ','D','E','V'
};
static uint16_t ser[] =
{
   USB_STRING_COMPOSE_LENGTH_AND_TYPE(sizeof("1234567890")),
   '1','2','3','4','5','6','7','8','9','0'
};

static USB_Device_Desc_DT dev_desc;

static bool desc_result;

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

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   frame_nr++;
   frame_nr &= 0x7FF;
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

void test_dev_desc(USBD_Params_XT *usbd)
{
   USBD_DEV_String_Header_XT *string_table = USBD_DEV_GET_STRING_TAB_PTR(usbd);

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_Init(usbd);

   USBDC_Init(&usbdc1);
   USBDC_Init(&usbdc2);
   USBDC_Init(&usbdc3);

   memset(&desc1, 0, sizeof(desc1));
   memset(&desc2, 0, sizeof(desc2));
   memset(&desc3, 0, sizeof(desc3));

   USBDC_Bind_Config_Desc(&usbdc1, (const uint8_t *)(&desc1));
   USBDC_Bind_Config_Desc(&usbdc2, (const uint8_t *)(&desc2));
   USBDC_Bind_Config_Desc(&usbdc3, (const uint8_t *)(&desc3));

   if(0 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   if(1 != USBD_DEV_Get_Num_Installed_Strings(usbd))
   {
      printf("incorrect number of installed strings\n\r");
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
      printf("port methid \"activate\" didn't called!\n\r");
      REPORT_ERROR();
   }

   activity = 0;

   set_dummy_cfg(&desc1, 1);
   set_dummy_cfg(&desc2, 2);
   set_dummy_cfg(&desc3, 3);

   config_install_result_1 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc1);
   config_install_result_2 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc2);
   config_install_result_3 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), &usbdc3);

   if(3 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 0) != &usbdc1)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 0) != (uint8_t*)(&desc1))
     || (0 != config_install_result_1.index)
     || (USBD_DEV_INSTALLATION_OK != config_install_result_1.result))
   {
      printf("config_table[0] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 1) != &usbdc2)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 1) != (uint8_t*)(&desc2))
     || (1 != config_install_result_2.index)
     || (USBD_DEV_INSTALLATION_OK != config_install_result_2.result))
   {
      printf("config_table[1] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 2) != &usbdc3)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 2) != (uint8_t*)(&desc3))
     || (2 != config_install_result_3.index)
     || (USBD_DEV_INSTALLATION_OK != config_install_result_3.result))
   {
      printf("config_table[2] is not same which was installed\n\r");
      REPORT_ERROR();
   }


   string_install_result_1 = USBD_DEV_Install_String(usbd, (uint8_t*)string1, sizeof(string1), 4);

   if(2 != USBD_DEV_Get_Num_Installed_Strings(usbd))
   {
      printf("incorrect number of installed strings\n\r");
      REPORT_ERROR();
   }
   else
   {
      if((0 != memcmp(string_table[4].string, string1, sizeof(string1)))
      || (4 != string_install_result_1.index)
      || (USBD_DEV_INSTALLATION_OK != string_install_result_1.result))
      {
         printf("string_table[1] is not same which was installed\n\r");
         REPORT_ERROR();
      }
   }

   string_install_result_manuf = USBD_DEV_Install_Manufacturer_String(usbd, (uint8_t*)manuf, sizeof(manuf));

   if(3 != USBD_DEV_Get_Num_Installed_Strings(usbd))
   {
      printf("incorrect number of installed strings\n\r");
      REPORT_ERROR();
   }
   else
   {
      if(0 != memcmp(string_table[4].string, string1, sizeof(string1)))
      {
         printf("string_table[1] is not same which was installed\n\r");
         REPORT_ERROR();
      }

      if((0 != memcmp(string_table[1].string, manuf, sizeof(manuf)))
      || (1 != string_install_result_manuf.index)
      || (USBD_DEV_INSTALLATION_OK != string_install_result_manuf.result))
      {
         printf("string_table[2] is not same which was installed\n\r");
         REPORT_ERROR();
      }
   }

   string_install_result_prod = USBD_DEV_Install_Product_String(usbd, (uint8_t*)prod, sizeof(prod));

   if(4 != USBD_DEV_Get_Num_Installed_Strings(usbd))
   {
      printf("incorrect number of installed strings\n\r");
      REPORT_ERROR();
   }
   else
   {
      if(0 != memcmp(string_table[4].string, string1, sizeof(string1)))
      {
         printf("string_table[1] is not same which was installed\n\r");
         REPORT_ERROR();
      }

      if(0 != memcmp(string_table[1].string, manuf, sizeof(manuf)))
      {
         printf("string_table[2] is not same which was installed\n\r");
         REPORT_ERROR();
      }

      if((0 != memcmp(string_table[2].string, prod, sizeof(prod)))
      || (2 != string_install_result_prod.index)
      || (USBD_DEV_INSTALLATION_OK != string_install_result_prod.result))
      {
         printf("string_table[3] is not same which was installed\n\r");
         REPORT_ERROR();
      }
   }

   string_install_result_ser = USBD_DEV_Install_Serial_Number_String(usbd, (uint8_t*)ser, sizeof(ser));

   if(5 != USBD_DEV_Get_Num_Installed_Strings(usbd))
   {
      printf("incorrect number of installed strings\n\r");
      REPORT_ERROR();
   }
   else
   {
      if(0 != memcmp(string_table[4].string, string1, sizeof(string1)))
      {
         printf("string_table[1] is not same which was installed\n\r");
         REPORT_ERROR();
      }

      if(0 != memcmp(string_table[1].string, manuf, sizeof(manuf)))
      {
         printf("string_table[2] is not same which was installed\n\r");
         REPORT_ERROR();
      }

      if(0 != memcmp(string_table[2].string, prod, sizeof(prod)))
      {
         printf("string_table[3] is not same which was installed\n\r");
         REPORT_ERROR();
      }

      if(0 != memcmp(string_table[3].string, ser, sizeof(ser)))
      {
         printf("string_table[4] is not same which was installed\n\r");
         REPORT_ERROR();
      }

      if((0 != memcmp(string_table[3].string, ser, sizeof(ser)))
      || (3 != string_install_result_ser.index)
      || (USBD_DEV_INSTALLATION_OK != string_install_result_ser.result))
      {
         printf("string_table[4] is not same which was installed\n\r");
         REPORT_ERROR();
      }
   }

   string_install_result_2 = USBD_DEV_Install_String(usbd, (uint8_t*)string2, sizeof(string2), 6);

   if(5 != USBD_DEV_Get_Num_Installed_Strings(usbd))
   {
      printf("incorrect number of installed strings\n\r");
      REPORT_ERROR();
   }
   else
   {
      if(0 != memcmp(string_table[4].string, string1, sizeof(string1)))
      {
         printf("string_table[1] is not same which was installed\n\r");
         REPORT_ERROR();
      }

      if(0 != memcmp(string_table[1].string, manuf, sizeof(manuf)))
      {
         printf("string_table[2] is not same which was installed\n\r");
         REPORT_ERROR();
      }

      if(0 != memcmp(string_table[2].string, prod, sizeof(prod)))
      {
         printf("string_table[3] is not same which was installed\n\r");
         REPORT_ERROR();
      }

      if(0 != memcmp(string_table[3].string, ser, sizeof(ser)))
      {
         printf("string_table[4] is not same which was installed\n\r");
         REPORT_ERROR();
      }

      if((0 != string_install_result_2.index)
      || (USBD_DEV_INSTALLATION_INDEX_OUT_OF_RANGE != string_install_result_2.result))
      {
         printf("string_table[5] should not be installed\n\r");
         REPORT_ERROR();
      }
   }

   desc_result = USBD_DEV_Get_Dev_Desc(usbd, &dev_desc);

   if(!desc_result)
   {
      printf("Device Descriptor read failed!");
      REPORT_ERROR();
   }

   dev_desc.bLength            = 0xA5;
   dev_desc.bDescriptorType      = 0xA5;
   dev_desc.bcdUSB            = 0x0110;
   dev_desc.bDeviceClass        = 0x01;
   dev_desc.bDeviceSubclass      = 0x02;
   dev_desc.bDeviceProtocol      = 0x03;
   dev_desc.bMaxPacketSize0      = 8;
   dev_desc.idVendor           = 0x1234;
   dev_desc.idProduct          = 0x5678;
   dev_desc.bcdDevice          = 0x0124;
   dev_desc.iManufacturer       = 0xA5;
   dev_desc.iProduct           = 0xA5;
   dev_desc.iSerialNumber       = 0xA5;
   dev_desc.bNumConfigurations   = 0xA5;

   desc_result = USBD_DEV_Set_Dev_Desc(usbd, &dev_desc);

   if(!desc_result)
   {
      printf("Device Descriptor set failed!");
      REPORT_ERROR();
   }

   desc_result = USBD_DEV_Get_Dev_Desc(usbd, &dev_desc);

   if(!desc_result)
   {
      printf("Device Descriptor read failed!");
      REPORT_ERROR();
   }

   if((sizeof(USB_Device_Desc_DT) / sizeof(uint8_t)) != dev_desc.bLength)
   {
      printf("Device Descriptor param bLength invalid: %d!", dev_desc.bLength);
      REPORT_ERROR();
   }

   if((USB_DESC_TYPE_DEVICE) != dev_desc.bDescriptorType)
   {
      printf("Device Descriptor param bDescriptorType invalid: %d!", dev_desc.bDescriptorType);
      REPORT_ERROR();
   }

   if((0x0110) != dev_desc.bcdUSB)
   {
      printf("Device Descriptor param bcdUSB invalid: %d!", dev_desc.bcdUSB);
      REPORT_ERROR();
   }

   if((0x01) != dev_desc.bDeviceClass)
   {
      printf("Device Descriptor param bDeviceClass invalid: %d!", dev_desc.bDeviceClass);
      REPORT_ERROR();
   }

   if((0x02) != dev_desc.bDeviceSubclass)
   {
      printf("Device Descriptor param bDeviceSubclass invalid: %d!", dev_desc.bDeviceSubclass);
      REPORT_ERROR();
   }

   if((0x03) != dev_desc.bDeviceProtocol)
   {
      printf("Device Descriptor param bDeviceProtocol invalid: %d!", dev_desc.bDeviceProtocol);
      REPORT_ERROR();
   }

   if((8) != dev_desc.bMaxPacketSize0)
   {
      printf("Device Descriptor param bMaxPacketSize0 invalid: %d!", dev_desc.bMaxPacketSize0);
      REPORT_ERROR();
   }

   if((0x1234) != dev_desc.idVendor)
   {
      printf("Device Descriptor param idVendor invalid: %d!", dev_desc.idVendor);
      REPORT_ERROR();
   }

   if((0x5678) != dev_desc.idProduct)
   {
      printf("Device Descriptor param idProduct invalid: %d!", dev_desc.idProduct);
      REPORT_ERROR();
   }

   if((0x0124) != dev_desc.bcdDevice)
   {
      printf("Device Descriptor param bcdDevice invalid: %d!", dev_desc.bcdDevice);
      REPORT_ERROR();
   }

   if((0) != dev_desc.iManufacturer)
   {
      printf("Device Descriptor param iManufacturer invalid: %d!", dev_desc.iManufacturer);
      REPORT_ERROR();
   }

   if((0) != dev_desc.iProduct)
   {
      printf("Device Descriptor param iProduct invalid: %d!", dev_desc.iProduct);
      REPORT_ERROR();
   }

   if((0) != dev_desc.iSerialNumber)
   {
      printf("Device Descriptor param iSerialNumber invalid: %d!", dev_desc.iSerialNumber);
      REPORT_ERROR();
   }

   if((3) != dev_desc.bNumConfigurations)
   {
      printf("Device Descriptor param bNumConfigurations invalid: %d!", dev_desc.bNumConfigurations);
      REPORT_ERROR();
   }

   /* next dev_desc set but with other parameters */

   dev_desc.bLength            = 0x47;
   dev_desc.bDescriptorType      = 0x47;
   dev_desc.bcdUSB            = 0x0200;
   dev_desc.bDeviceClass        = 0x12;
   dev_desc.bDeviceSubclass      = 0x23;
   dev_desc.bDeviceProtocol      = 0x45;
   dev_desc.bMaxPacketSize0      = 64;
   dev_desc.idVendor           = 0x9abc;
   dev_desc.idProduct          = 0xdef0;
   dev_desc.bcdDevice          = 0x2020;
   dev_desc.iManufacturer       = 0x47;
   dev_desc.iProduct           = 0x47;
   dev_desc.iSerialNumber       = 0x47;
   dev_desc.bNumConfigurations   = 0x47;

   desc_result = USBD_DEV_Set_Dev_Desc(usbd, &dev_desc);

   if(!desc_result)
   {
      printf("Device Descriptor set failed!");
      REPORT_ERROR();
   }

   desc_result = USBD_DEV_Get_Dev_Desc(usbd, &dev_desc);

   if(!desc_result)
   {
      printf("Device Descriptor read failed!");
      REPORT_ERROR();
   }

   if((sizeof(USB_Device_Desc_DT) / sizeof(uint8_t)) != dev_desc.bLength)
   {
      printf("Device Descriptor param bLength invalid: %d!", dev_desc.bLength);
      REPORT_ERROR();
   }

   if((USB_DESC_TYPE_DEVICE) != dev_desc.bDescriptorType)
   {
      printf("Device Descriptor param bDescriptorType invalid: %d!", dev_desc.bDescriptorType);
      REPORT_ERROR();
   }

   if((0x0200) != dev_desc.bcdUSB)
   {
      printf("Device Descriptor param bcdUSB invalid: %d!", dev_desc.bcdUSB);
      REPORT_ERROR();
   }

   if((0x12) != dev_desc.bDeviceClass)
   {
      printf("Device Descriptor param bDeviceClass invalid: %d!", dev_desc.bDeviceClass);
      REPORT_ERROR();
   }

   if((0x23) != dev_desc.bDeviceSubclass)
   {
      printf("Device Descriptor param bDeviceSubclass invalid: %d!", dev_desc.bDeviceSubclass);
      REPORT_ERROR();
   }

   if((0x45) != dev_desc.bDeviceProtocol)
   {
      printf("Device Descriptor param bDeviceProtocol invalid: %d!", dev_desc.bDeviceProtocol);
      REPORT_ERROR();
   }

   if((64) != dev_desc.bMaxPacketSize0)
   {
      printf("Device Descriptor param bMaxPacketSize0 invalid: %d!", dev_desc.bMaxPacketSize0);
      REPORT_ERROR();
   }

   if((0x9abc) != dev_desc.idVendor)
   {
      printf("Device Descriptor param idVendor invalid: %d!", dev_desc.idVendor);
      REPORT_ERROR();
   }

   if((0xdef0) != dev_desc.idProduct)
   {
      printf("Device Descriptor param idProduct invalid: %d!", dev_desc.idProduct);
      REPORT_ERROR();
   }

   if((0x2020) != dev_desc.bcdDevice)
   {
      printf("Device Descriptor param bcdDevice invalid: %d!", dev_desc.bcdDevice);
      REPORT_ERROR();
   }

   if((0) != dev_desc.iManufacturer)
   {
      printf("Device Descriptor param iManufacturer invalid: %d!", dev_desc.iManufacturer);
      REPORT_ERROR();
   }

   if((0) != dev_desc.iProduct)
   {
      printf("Device Descriptor param iProduct invalid: %d!", dev_desc.iProduct);
      REPORT_ERROR();
   }

   if((0) != dev_desc.iSerialNumber)
   {
      printf("Device Descriptor param iSerialNumber invalid: %d!", dev_desc.iSerialNumber);
      REPORT_ERROR();
   }

   if((3) != dev_desc.bNumConfigurations)
   {
      printf("Device Descriptor param bNumConfigurations invalid: %d!", dev_desc.bNumConfigurations);
      REPORT_ERROR();
   }

   USBD_DEV_Reset(usbd);

   if((usbd->dev.core.data.dev_desc.iManufacturer != 1)
     || (usbd->dev.core.data.dev_desc.iProduct != 2)
     || (usbd->dev.core.data.dev_desc.iSerialNumber != 3))
   {
      printf("iStrings enumerated incorrectly\n\r");
      REPORT_ERROR();
   }

   dev_desc.bLength              = 0xA5;
   dev_desc.bDescriptorType      = 0xA5;
   dev_desc.bcdUSB               = 0x0110;
   dev_desc.bDeviceClass         = 0x01;
   dev_desc.bDeviceSubclass      = 0x02;
   dev_desc.bDeviceProtocol      = 0x03;
   dev_desc.bMaxPacketSize0      = 32;
   dev_desc.idVendor             = 0x1234;
   dev_desc.idProduct            = 0x5678;
   dev_desc.bcdDevice            = 0x0124;
   dev_desc.iManufacturer        = 0xA5;
   dev_desc.iProduct             = 0xA5;
   dev_desc.iSerialNumber        = 0xA5;
   dev_desc.bNumConfigurations   = 0xA5;

   desc_result = USBD_DEV_Set_Dev_Desc(usbd, &dev_desc);

   if(desc_result)
   {
      printf("Device Descriptor could be set!");
      REPORT_ERROR();
   }

   desc_result = USBD_DEV_Get_Dev_Desc(usbd, &dev_desc);

   if(!desc_result)
   {
      printf("Device Descriptor read failed!");
      REPORT_ERROR();
   }

   if((sizeof(USB_Device_Desc_DT) / sizeof(uint8_t)) != dev_desc.bLength)
   {
      printf("Device Descriptor param bLength invalid: %d!", dev_desc.bLength);
      REPORT_ERROR();
   }

   if((USB_DESC_TYPE_DEVICE) != dev_desc.bDescriptorType)
   {
      printf("Device Descriptor param bDescriptorType invalid: %d!", dev_desc.bDescriptorType);
      REPORT_ERROR();
   }

   if((0x0200) != dev_desc.bcdUSB)
   {
      printf("Device Descriptor param bcdUSB invalid: %d!", dev_desc.bcdUSB);
      REPORT_ERROR();
   }

   if((0x12) != dev_desc.bDeviceClass)
   {
      printf("Device Descriptor param bDeviceClass invalid: %d!", dev_desc.bDeviceClass);
      REPORT_ERROR();
   }

   if((0x23) != dev_desc.bDeviceSubclass)
   {
      printf("Device Descriptor param bDeviceSubclass invalid: %d!", dev_desc.bDeviceSubclass);
      REPORT_ERROR();
   }

   if((0x45) != dev_desc.bDeviceProtocol)
   {
      printf("Device Descriptor param bDeviceProtocol invalid: %d!", dev_desc.bDeviceProtocol);
      REPORT_ERROR();
   }

   if(TEST_EP0_MPS != dev_desc.bMaxPacketSize0)
   {
      printf("Device Descriptor param bMaxPacketSize0 invalid: %d!", dev_desc.bMaxPacketSize0);
      REPORT_ERROR();
   }

   if((0x9abc) != dev_desc.idVendor)
   {
      printf("Device Descriptor param idVendor invalid: %d!", dev_desc.idVendor);
      REPORT_ERROR();
   }

   if((0xdef0) != dev_desc.idProduct)
   {
      printf("Device Descriptor param idProduct invalid: %d!", dev_desc.idProduct);
      REPORT_ERROR();
   }

   if((0x2020) != dev_desc.bcdDevice)
   {
      printf("Device Descriptor param bcdDevice invalid: %d!", dev_desc.bcdDevice);
      REPORT_ERROR();
   }

   if((1) != dev_desc.iManufacturer)
   {
      printf("Device Descriptor param iManufacturer invalid: %d!", dev_desc.iManufacturer);
      REPORT_ERROR();
   }

   if((2) != dev_desc.iProduct)
   {
      printf("Device Descriptor param iProduct invalid: %d!", dev_desc.iProduct);
      REPORT_ERROR();
   }

   if((3) != dev_desc.iSerialNumber)
   {
      printf("Device Descriptor param iSerialNumber invalid: %d!", dev_desc.iSerialNumber);
      REPORT_ERROR();
   }

   if((3) != dev_desc.bNumConfigurations)
   {
      printf("Device Descriptor param bNumConfigurations invalid: %d!", dev_desc.bNumConfigurations);
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

   USBD_EXIT_FUNC(MAIN_APP_TEST);
} /* test_dev_desc */
