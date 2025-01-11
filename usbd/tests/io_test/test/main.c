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

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "usbd.h"

#ifndef USBD_DEV_INTERNAL_H_
#include "usbd_dev_internal.h"
#endif

#include "cfg.h"


#include "io_test_configure.h"
#include "io_test_deconfigure.h"
#include "io_test_halt.h"
#include "io_test_tp_attach.h"
#include "io_test_stall_in.h"
#include "io_test_stall_out.h"
#include "io_test_in_abort.h"
#include "io_test_out_abort.h"
#include "io_test_process_in_error_cbi.h"
#include "io_test_process_out_error_cbi.h"
#include "io_test_process_in_error_iso.h"
#include "io_test_process_out_error_iso.h"

#include "main_get_exe.h"


#define TEST_LOOP_MAX_NUM_REPEATS      0x7FFF
#define TEST_LOOP_MIN_NUM_REPEATS      0x3FF

#define FUNC_ENTER_NOTIF      (false)

//#define USE_INVALID_CONFIGS    (true)


#define DEV_DESC_SIZE         (sizeof(USB_device_desc_DT) / sizeof(uint8_t))

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


static void test_activate_deactivate(USBD_Params_XT* usbd, USBD_Bool_DT state);
static USBD_Bool_DT test_parse_ep(USBD_Params_XT* usbd, uint8_t ep_num, const USB_Interface_Desc_DT *if_desc, const USB_Endpoint_Desc_DT *ep_desc);
static USBD_Bool_DT test_parse_cfg(USBD_Params_XT* usbd, const uint8_t *desc, uint16_t desc_size, USBD_DEV_Config_Desc_Check_Result_XT *details);
static USBD_DEV_Speed_ET test_get_supported_speed(USBD_Params_XT *usbd);
static USBD_DEV_Speed_ET test_get_current_speed(USBD_Params_XT *usbd);
static uint16_t test_get_dev_status(USBD_Params_XT *usbd);
static uint16_t test_get_frame_nr(USBD_Params_XT *usbd);
static USBD_DEV_PORT_Get_EP_Desc_Result_DT test_get_ep0_low_full_speed(USBD_Params_XT* usbd);
static USBD_DEV_PORT_Get_EP_Desc_Result_DT test_get_ep0_high_speed(USBD_Params_XT* usbd);

static USBD_DEV_Installation_Result_XT install_result_1;
static USBD_DEV_Installation_Result_XT install_result_2;
static USBD_DEV_Installation_Result_XT install_result_3;
static USBD_DEV_Installation_Result_XT install_result_4;

static USBD_DEV_Installation_Result_XT string_install_result_1;
static USBD_DEV_Installation_Result_XT string_install_result_2;
static USBD_DEV_Installation_Result_XT string_install_result_3;
static USBD_DEV_Installation_Result_XT string_install_result_4;

//static USB_Configuration_Desc_DT desc1;
static USB_Configuration_Desc_DT desc2;
static USB_Configuration_Desc_DT desc3;
static USB_Configuration_Desc_DT desc4;
static USBDC_Params_XT usbdcp1;
static USBDC_Params_XT usbdcp2;
static USBDC_Params_XT usbdcp3;
static USBDC_Params_XT usbdcp4;
static USBDC_Params_XT *usbdc1 = &usbdcp1;
static USBDC_Params_XT *usbdc2 = &usbdcp2;
static USBDC_Params_XT *usbdc3 = &usbdcp3;
static USBDC_Params_XT *usbdc4 = &usbdcp4;

static int number_of_repeats;



static uint8_t desc1[] =
{
      /* bLength              */ sizeof(USB_Configuration_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_CONFIGURATION,
      /* wTotalLength.L       */ (uint8_t)(0 & 0xFF),
      /* wTotalLength.H       */ (uint8_t)((0 >> 8) & 0xFF),
      /* bNumInterfaces       */ TEST_NUM_INTERFACES,
      /* bConfigurationValue  */ 0,
      /* iConfiguration       */ 4,
      /* bmAttributes         */ 0x40,
      /* bMaxPower            */ 50,

      /* bLength              */ sizeof(USB_Interface_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_INTERFACE,
      /* bInterfaceNumber     */ 0,
      /* bAlternateSetting    */ 0,
      /* bNumEndpoints        */ 2,
      /* bInterfaceClass      */ 1,
      /* bInterfaceSubClass   */ 2,
      /* bInterfaceProtocol   */ 3,
      /* iInterface           */ 0,

      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_IN | 1,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_OUT | 2,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Interface_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_INTERFACE,
      /* bInterfaceNumber     */ 0,
      /* bAlternateSetting    */ 1,
      /* bNumEndpoints        */ 1,
      /* bInterfaceClass      */ 1,
      /* bInterfaceSubClass   */ 2,
      /* bInterfaceProtocol   */ 3,
      /* iInterface           */ 0,
#if 0
      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_IN | 1,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* wMaxPacketSize.L     */ 64,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,
#endif
      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_OUT | 2,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Interface_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_INTERFACE,
      /* bInterfaceNumber     */ 0,
      /* bAlternateSetting    */ 2,
      /* bNumEndpoints        */ 2,
      /* bInterfaceClass      */ 1,
      /* bInterfaceSubClass   */ 2,
      /* bInterfaceProtocol   */ 3,
      /* iInterface           */ 0,
#if 1
      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_IN | 1,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* wMaxPacketSize.L     */ 0,
      /* wMaxPacketSize.H     */ 2,
      /* bInterval            */ 0xFF,
#endif
      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_OUT | 2,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* wMaxPacketSize.L     */ 0,
      /* wMaxPacketSize.H     */ 1,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Interface_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_INTERFACE,
      /* bInterfaceNumber     */ 1,
      /* bAlternateSetting    */ 0,
      /* bNumEndpoints        */ 0,
      /* bInterfaceClass      */ 6,
      /* bInterfaceSubClass   */ 3,
      /* bInterfaceProtocol   */ 11,
      /* iInterface           */ 0,

      /* bLength              */ sizeof(USB_Interface_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_INTERFACE,
      /* bInterfaceNumber     */ 2,
      /* bAlternateSetting    */ 0,
      /* bNumEndpoints        */ 3,
      /* bInterfaceClass      */ 1,
      /* bInterfaceSubClass   */ 2,
      /* bInterfaceProtocol   */ 3,
      /* iInterface           */ 0,

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

static uint16_t string1[] =
{
   USB_STRING_COMPOSE_LENGTH_AND_TYPE(sizeof("Mstring1")),
   'M', 's', 't', 'r', 'i', 'n', 'g', '1'
};

static uint16_t string2[] =
{
   USB_STRING_COMPOSE_LENGTH_AND_TYPE(sizeof("Pstring2")),
   'P', 's', 't', 'r', 'i', 'n', 'g', '2'
};

static uint16_t string3[] =
{
   USB_STRING_COMPOSE_LENGTH_AND_TYPE(sizeof("Sstring3")),
   'S', 's', 't', 'r', 'i', 'n', 'g', '3'
};

static uint16_t string4[] =
{
   USB_STRING_COMPOSE_LENGTH_AND_TYPE(sizeof("Ostring4")),
   'S', 's', 't', 'r', 'i', 'n', 'g', '4'
};

static uint8_t activity = 0;
static USB_Endpoint_Desc_DT ep0_full =
{
   .bLength            = USB_DESC_TYPE_ENDPOINT_SIZE,
   .bDescriptorType    = USB_DESC_TYPE_ENDPOINT,
   .bEndpointAddress   = 0,
   .bmAttributes       = USB_EP_DESC_TRANSFER_TYPE_CONTROL,
   .wMaxPacketSize.L   = TEST_EP0_MPS,
   .wMaxPacketSize.H   = 0,
   .bInterval          = 0xFF
};
static USB_Endpoint_Desc_DT ep0_high =
{
   .bLength            = USB_DESC_TYPE_ENDPOINT_SIZE,
   .bDescriptorType    = USB_DESC_TYPE_ENDPOINT,
   .bEndpointAddress   = 0,
   .bmAttributes       = USB_EP_DESC_TRANSFER_TYPE_CONTROL,
   .wMaxPacketSize.L   = TEST_EP0_MPS,
   .wMaxPacketSize.H   = 0,
   .bInterval          = 0xFF
};
static USBD_DEV_Speed_ET speed = USBD_DEV_FULL_AND_HIGH_SPEED;
static uint16_t device_status = 0;
static uint16_t frame_nr = 0;
static USBD_DEV_Port_Handler_XT port =
{
   .handlers.activate              = test_activate_deactivate,
   .handlers.ep_parse              = test_parse_ep,
   .handlers.cfg_parse             = test_parse_cfg,
   .handlers.get_supported_speed   = test_get_supported_speed,
   .handlers.get_current_speed     = test_get_current_speed,
   .handlers.get_device_status     = test_get_dev_status,
   .handlers.get_frame_num         = test_get_frame_nr,
   .handlers.get_ep0_low_full_speed= test_get_ep0_low_full_speed,
   .handlers.get_ep0_high_speed    = test_get_ep0_high_speed
};

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

   frame_nr ++;
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


static void test_compose_configs(USBD_Params_XT *usbd)
{
   USBD_DEV_Config_Desc_Check_Result_XT check_result;

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   if(0 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   desc1[2] = sizeof(desc1) & 0xFF;
   desc1[3] = (sizeof(desc1) >> 8) & 0xFF;

   check_result = USBD_DEV_Check_Config_Desc(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), (uint8_t*)(&desc1), sizeof(desc1));

   if(USBD_DEV_CONFIG_DESC_OK != check_result.result)
   {
      printf("incorrect configuration descriptor!\n\r");
      printf("config check result: result: %d, index: %d, if_num: %d, if_alt: %d, ep_num: %d, ep_dir: %s\n\r",
            check_result.result, check_result.index,
            check_result.if_num, check_result.if_alt,
            check_result.ep_num, USB_EP_DIRECTION_OUT == check_result.ep_dir ? "OUT" : "IN");
      REPORT_ERROR();
   }

#if(USE_INVALID_CONFIGS)
   memset(&desc2, 0, sizeof(desc2));
   memset(&desc3, 0, sizeof(desc3));
   memset(&desc4, 0, sizeof(desc4));
#endif

   USBDC_Bind_Config_Desc(usbdc1, (const uint8_t *)(&desc1));
#if(USE_INVALID_CONFIGS)
   USBDC_Bind_Config_Desc(usbdc2, (const uint8_t *)(&desc2));
   USBDC_Bind_Config_Desc(usbdc3, (const uint8_t *)(&desc3));
   USBDC_Bind_Config_Desc(usbdc4, (const uint8_t *)(&desc4));
#endif

   install_result_1 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(const USBD_DEV_Port_Handler_XT), usbdc1);
#if(USE_INVALID_CONFIGS)
   install_result_2 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(const USBD_DEV_Port_Handler_XT), usbdc2);
   install_result_3 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(const USBD_DEV_Port_Handler_XT), usbdc3);
   install_result_4 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(const USBD_DEV_Port_Handler_XT), usbdc4);
#endif

#if(USE_INVALID_CONFIGS)
   if(4 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs: %d\n\r", usbd->dev.core.data.dev_desc.bNumConfigurations);
      REPORT_ERROR();
   }
#else
   if(1 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs: %d\n\r", usbd->dev.core.data.dev_desc.bNumConfigurations);
      REPORT_ERROR();
   }
#endif

   if((USBD_DEV_Get_Config(usbd, 0) != usbdc1)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 0) != (uint8_t*)(&desc1))
     || (0 != install_result_1.index)
     || (USBD_DEV_INSTALLATION_OK != install_result_1.result))
   {
      printf("config_table[0] is not same which was installed\n\r");
      REPORT_ERROR();
   }

#if(USE_INVALID_CONFIGS)
   if((USBD_DEV_Get_Config(usbd, 1) != usbdc2)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 1) != (uint8_t*)(&desc2))
     || (1 != install_result_2.index)
     || (USBD_DEV_INSTALLATION_OK != install_result_2.result))
   {
      printf("config_table[1] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 2) != usbdc3)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 2) != (uint8_t*)(&desc3))
     || (2 != install_result_3.index)
     || (USBD_DEV_INSTALLATION_OK != install_result_3.result))
   {
      printf("config_table[2] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 3) != usbdc4)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 3) != (uint8_t*)(&desc4))
     || (3 != install_result_4.index)
     || (USBD_DEV_INSTALLATION_OK != install_result_4.result))
   {
      printf("config_table[3] is not same which was installed\n\r");
      REPORT_ERROR();
   }
#endif

   if(1 != USBD_DEV_Get_Num_Installed_Strings(usbd))
   {
      printf("incorrect number of installed strings\n\r");
      REPORT_ERROR();
   }

   string_install_result_1 = USBD_DEV_Install_Manufacturer_String(usbd, (uint8_t*)string1, sizeof(string1));

   if(2 != USBD_DEV_Get_Num_Installed_Strings(usbd))
   {
      printf("incorrect number of installed strings\n\r");
      REPORT_ERROR();
   }

   if((0 != memcmp(USBD_DEV_Get_String(usbd, 1), (uint8_t*)string1, sizeof(string1))) || (1 != string_install_result_1.index)
      || (USBD_DEV_INSTALLATION_OK != string_install_result_1.result))
   {
      printf("string_table[0] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   string_install_result_2 = USBD_DEV_Install_Product_String(usbd, (uint8_t*)string2, sizeof(string2));

   if(3 != USBD_DEV_Get_Num_Installed_Strings(usbd))
   {
      printf("incorrect number of installed strings\n\r");
      REPORT_ERROR();
   }

   if((0 != memcmp(USBD_DEV_Get_String(usbd, 2), (uint8_t*)string2, sizeof(string2))) || (2 != string_install_result_2.index)
      || (USBD_DEV_INSTALLATION_OK != string_install_result_2.result))
   {
      printf("string_table[1] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   string_install_result_3 = USBD_DEV_Install_Serial_Number_String(usbd, (uint8_t*)string3, sizeof(string3));

   if(4 != USBD_DEV_Get_Num_Installed_Strings(usbd))
   {
      printf("incorrect number of installed strings\n\r");
      REPORT_ERROR();
   }

   if((0 != memcmp(USBD_DEV_Get_String(usbd, 3), (uint8_t*)string3, sizeof(string3))) || (3 != string_install_result_3.index)
      || (USBD_DEV_INSTALLATION_OK != string_install_result_3.result))
   {
      printf("string_table[2] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   string_install_result_4 = USBD_DEV_Install_String(usbd, (uint8_t*)string4, sizeof(string4), desc1[6]);

   if(5 != USBD_DEV_Get_Num_Installed_Strings(usbd))
   {
      printf("incorrect number of installed strings\n\r");
      REPORT_ERROR();
   }

   if((0 != memcmp(USBD_DEV_Get_String(usbd, 4), (uint8_t*)string4, sizeof(string4)))
      || (4 != string_install_result_4.index)
      || (USBD_DEV_INSTALLATION_OK != string_install_result_4.result))
   {
      printf("string_table[3] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

void prepare_device(USBD_Params_XT *usbd, uint8_t step)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_Init(usbd);

#if(FUNC_ENTER_NOTIF)
   printf("%s, state = 0x%X, step = %d\n\r", __FUNCTION__, USBD_DEV_Get_State(usbd), step);
#endif

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

   if(step > 1)
   {
      USBD_DEV_Powered(usbd, true);
   }
   if(step > 2)
   {
      test_compose_configs(usbd);
      USBD_DEV_Reset(usbd);
   }
   if(step > 3)
   {
      USBD_DEV_Addressed(usbd, 2);
   }
   if(step > 4)
   {
      USBD_DEV_Configured(usbd, 1, USBD_MAKE_INVALID_HANDLER(USBD_DEV_Set_Configuration_Respond_HT));
   }
#if(FUNC_ENTER_NOTIF)
   printf("%s, state = 0x%X\n\r", __FUNCTION__, USBD_DEV_Get_State(usbd));
#endif

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

uint8_t x_if(void)
{
   return(rand() % USBD_MAX_NUM_INTERFACES);
}

uint8_t x_u8(void)
{
   return(rand() % 256);
}

void set_dummy_cfg(USB_Configuration_Desc_DT *desc)
{
   desc->bLength = USB_DESC_TYPE_CONFIGURATION_SIZE;
   desc->bDescriptorType = USB_DESC_TYPE_CONFIGURATION;
   desc->wTotalLength.L = USB_DESC_TYPE_CONFIGURATION_SIZE;
   desc->wTotalLength.H = 0;
   desc->bNumInterfaces = 0;
   desc->bConfigurationValue = 0;
   desc->iConfiguration = 0;
   desc->bmAttributes = 0x40;
   desc->bMaxPower = 50;
}

int main(void)
{
   USBD_Params_XT usbd;
   int j;
   uint8_t ep_num;
   uint16_t param;

    IOCMD_Logs_Init();

   printf("testing: configure & deconfigure & halt with USBD_Init\n\r");
   for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
   {
      for(param = 0; param < 8; param++)
      {
         USBD_Init(&usbd);
         test_configure(&usbd, param, ep_num, USB_EP_DIRECTION_OUT);
         USBD_Init(&usbd);
         test_configure(&usbd, param, ep_num, USB_EP_DIRECTION_IN);
         USBD_Init(&usbd);
         test_deconfigure(&usbd, param, ep_num, USB_EP_DIRECTION_OUT);
         USBD_Init(&usbd);
         test_deconfigure(&usbd, param, ep_num, USB_EP_DIRECTION_IN);
         USBD_Init(&usbd);
         test_halt(&usbd, param, ep_num, USB_EP_DIRECTION_OUT, USBD_TRUE);
         USBD_Init(&usbd);
         test_halt(&usbd, param, ep_num, USB_EP_DIRECTION_IN, USBD_TRUE);
         USBD_Init(&usbd);
         test_halt(&usbd, param, ep_num, USB_EP_DIRECTION_OUT, USBD_FALSE);
         USBD_Init(&usbd);
         test_halt(&usbd, param, ep_num, USB_EP_DIRECTION_IN, USBD_FALSE);
      }
   }

   printf("testing: configure & deconfigure & halt without USBD_Init\n\r");
   for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
   {
      for(param = 0; param < 32; param++)
      {
         USBD_Init(&usbd);
         test_configure(&usbd, param, ep_num, USB_EP_DIRECTION_OUT);
         test_configure(&usbd, param, ep_num, USB_EP_DIRECTION_IN);
         test_deconfigure(&usbd, param, ep_num, USB_EP_DIRECTION_OUT);
         test_deconfigure(&usbd, param, ep_num, USB_EP_DIRECTION_IN);
         test_halt(&usbd, param, ep_num, USB_EP_DIRECTION_OUT, USBD_TRUE);
         test_halt(&usbd, param, ep_num, USB_EP_DIRECTION_IN, USBD_TRUE);
         test_halt(&usbd, param, ep_num, USB_EP_DIRECTION_OUT, USBD_FALSE);
         test_halt(&usbd, param, ep_num, USB_EP_DIRECTION_IN, USBD_FALSE);
      }
   }

   printf("testing: attach\n\r");
   USBD_Init(&usbd);
   for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
   {
      test_tp_attach(&usbd, ep_num, USB_EP_DIRECTION_OUT);
      test_tp_attach(&usbd, ep_num, USB_EP_DIRECTION_IN);
   }
   USBD_Init(&usbd);
   for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
   {
      test_configure(&usbd, param, ep_num, USB_EP_DIRECTION_OUT);
      test_configure(&usbd, param, ep_num, USB_EP_DIRECTION_IN);
      test_tp_attach(&usbd, ep_num, USB_EP_DIRECTION_OUT);
      test_tp_attach(&usbd, ep_num, USB_EP_DIRECTION_IN);
   }

   printf("testing: stall_in\n\r");
   for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
   {
      for(param = 0; param < 4; param++)
      {
         test_stall_in(&usbd, param, ep_num);
      }
   }

   printf("testing: stall_out\n\r");
   for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
   {
      for(param = 0; param < 4; param++)
      {
         test_stall_out(&usbd, param, ep_num);
      }
   }

   USBD_Init(&usbd);

   printf("testing: in_abort\n\r");
   for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
   {
      for(param = 0; param < 256; param++)
      {
         test_in_abort(&usbd, param, ep_num);
      }
   }

   printf("testing: out_abort\n\r");
   for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
   {
      for(param = 0; param < 256; param++)
      {
         test_out_abort(&usbd, param, ep_num);
      }
   }

   printf("testing: process_in_error_cbi\n\r");
   for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
   {
      for(param = 0; param < 256; param++)
      {
         test_process_in_error_cbi(&usbd, param, ep_num);
      }
   }

   printf("testing: process_out_error_cbi\n\r");
   for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
   {
      for(param = 0; param < 256; param++)
      {
         test_process_out_error_cbi(&usbd, param, ep_num);
      }
   }

   printf("testing: process_in_error_iso\n\r");
   for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
   {
      for(param = 0; param < 256; param++)
      {
         for(j = 0; j < 32; j++)
         {
            test_process_in_error_iso(&usbd, param, ep_num, (uint8_t)j);
         }
      }
   }

   printf("testing: process_out_error_iso\n\r");
   for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
   {
      for(param = 0; param < 256; param++)
      {
         for(j = 0; j < 32; j++)
         {
            test_process_out_error_iso(&usbd, param, ep_num, (uint8_t)j);
         }
      }
   }

   printf("size of USBD_Params_XT: %d\n\r", (sizeof(USBD_Params_XT) / sizeof(uint8_t)));
   printf("size of USBDC_Params_XT: %d\n\r", (sizeof(USBDC_Params_XT) / sizeof(uint8_t)));
   printf("size of USBD_IO_params_XT: %d\n\r", (sizeof(USBD_IO_Params_XT) / sizeof(uint8_t)));
   printf("size of string1: %d, from descriptor: %d\n\r", sizeof(string1), string1[0] & 0xFF);
   printf("size of string2: %d, from descriptor: %d\n\r", sizeof(string2), string2[0] & 0xFF);
   printf("size of string3: %d, from descriptor: %d\n\r", sizeof(string3), string3[0] & 0xFF);
   printf("size of string4: %d, from descriptor: %d\n\n\r", sizeof(string4), string4[0] & 0xFF);

   printf("\ntests done\n\n");

   return 0;
}

