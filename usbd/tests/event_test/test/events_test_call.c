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

#include "events_test_call.h"
#ifndef USBDC_INTERNAL_H_
#include "usbdc_internal.h"
#endif
#ifndef USBD_DEV_INTERNAL_H_
#include "usbd_dev_internal.h"
#endif
#ifndef USBD_EVENT_INTERNAL_H_
#include "usbd_event_internal.h"
#endif

#include "cfg.h"
#include "main_get_exe.h"

//#define PRINT_ON_EVENT_CALL    1

static void test_activate_deactivate(USBD_Params_XT* usbd, USBD_Bool_DT state);
static USBD_Bool_DT test_parse_ep(USBD_Params_XT* usbd, uint8_t ep_num, const USB_Interface_Desc_DT *if_desc, const USB_Endpoint_Desc_DT *ep_desc);
static USBD_Bool_DT test_parse_cfg(USBD_Params_XT* usbd, const uint8_t *desc, uint16_t desc_size, USBD_DEV_Config_Desc_Check_Result_XT *details);
static USBD_DEV_Speed_ET test_get_supported_speed(USBD_Params_XT *usbd);
static USBD_DEV_Speed_ET test_get_current_speed(USBD_Params_XT *usbd);
static uint16_t test_get_dev_status(USBD_Params_XT *usbd);
static uint16_t test_get_frame_nr(USBD_Params_XT *usbd);
static const USB_Endpoint_Desc_DT* test_get_ep0_low_full_speed(USBD_Params_XT* usbd);
static const USB_Endpoint_Desc_DT* test_get_ep0_high_speed(USBD_Params_XT* usbd);

static USBD_DEV_Installation_Result_XT install_result_1;
static USBD_DEV_Installation_Result_XT install_result_2;
static USBD_DEV_Installation_Result_XT install_result_3;
static USBD_DEV_Installation_Result_XT install_result_4;
static USB_Configuration_Desc_DT desc1;
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



static USBD_EVENT_Event_Header_XT *usbd_event_params[4];
static USBD_EVENT_Event_Header_XT *usbdc1_event_params[3];
static USBD_EVENT_Event_Header_XT *usbdc2_event_params[2];
static USBD_EVENT_Event_Header_XT *usbdc3_event_params[4];
static USBD_EVENT_Event_Header_XT *usbdc4_event_params[5];

static uint8_t event_data[5][5];

static uint8_t activity = 0;
static USB_Endpoint_Desc_DT ep0_full =
{
   .bLength          = USB_DESC_TYPE_ENDPOINT_SIZE,
   .bDescriptorType    = USB_DESC_TYPE_ENDPOINT,
   .bEndpointAddress   = 0,
   .bmAttributes      = USB_EP_DESC_TRANSFER_TYPE_CONTROL,
   .wMaxPacketSize.L   = TEST_EP0_MPS,
   .wMaxPacketSize.H   = 0,
   .bInterval         = 0xFF,
};
static USB_Endpoint_Desc_DT ep0_high =
{
   .bLength          = USB_DESC_TYPE_ENDPOINT_SIZE,
   .bDescriptorType    = USB_DESC_TYPE_ENDPOINT,
   .bEndpointAddress   = 0,
   .bmAttributes      = USB_EP_DESC_TRANSFER_TYPE_CONTROL,
   .wMaxPacketSize.L   = TEST_EP0_MPS,
   .wMaxPacketSize.H   = 0,
   .bInterval         = 0xFF,
};
static USBD_DEV_Speed_ET speed = USBD_DEV_FULL_AND_HIGH_SPEED;
static uint16_t device_status = 0;
static uint16_t frame_nr = 0;
static USBD_DEV_Port_Handler_XT port =
{
   .handlers.activate            = test_activate_deactivate,
   .handlers.ep_parse            = test_parse_ep,
   .handlers.cfg_parse           = test_parse_cfg,
   .handlers.get_supported_speed   = test_get_supported_speed,
   .handlers.get_current_speed     = test_get_current_speed,
   .handlers.get_device_status     = test_get_dev_status,
   .handlers.get_frame_num        = test_get_frame_nr,
   .handlers.get_ep0_low_full_speed = test_get_ep0_low_full_speed,
   .handlers.get_ep0_high_speed    = test_get_ep0_high_speed
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

static const USB_Endpoint_Desc_DT* test_get_ep0_low_full_speed(USBD_Params_XT* usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return (&ep0_full);
}

static const USB_Endpoint_Desc_DT* test_get_ep0_high_speed(USBD_Params_XT* usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return (&ep0_high);
}


static void event_usbd_0(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[0][0] = 1;
#ifdef PRINT_ON_EVENT_CALL
   printf("%s: %d\n\r", __FUNCTION__, event_data[0][0]);
#endif

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
static void event_usbd_1(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[0][1] = 2;
#ifdef PRINT_ON_EVENT_CALL
   printf("%s: %d\n\r", __FUNCTION__, event_data[0][1]);
#endif

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
static void event_usbd_2(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[0][2] = 3;
#ifdef PRINT_ON_EVENT_CALL
   printf("%s: %d\n\r", __FUNCTION__, event_data[0][2]);
#endif

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
static void event_usbd_3(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[0][3] = 4;
#ifdef PRINT_ON_EVENT_CALL
   printf("%s: %d\n\r", __FUNCTION__, event_data[0][3]);
#endif

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
static void event_usbdc1_0(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[1][0] = 16;
#ifdef PRINT_ON_EVENT_CALL
   printf("%s: %d\n\r", __FUNCTION__, event_data[1][0]);
#endif

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
static void event_usbdc1_1(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[1][1] = 17;
#ifdef PRINT_ON_EVENT_CALL
   printf("%s: %d\n\r", __FUNCTION__, event_data[1][1]);
#endif

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
static void event_usbdc1_2(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[1][2] = 18;
#ifdef PRINT_ON_EVENT_CALL
   printf("%s: %d\n\r", __FUNCTION__, event_data[1][2]);
#endif

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
static void event_usbdc2_0(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[2][0] = 32;
#ifdef PRINT_ON_EVENT_CALL
   printf("%s: %d\n\r", __FUNCTION__, event_data[2][0]);
#endif

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
static void event_usbdc2_1(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[2][1] = 33;
#ifdef PRINT_ON_EVENT_CALL
   printf("%s: %d\n\r", __FUNCTION__, event_data[2][1]);
#endif

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
static void event_usbdc4_0(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[4][0] = 64;
#ifdef PRINT_ON_EVENT_CALL
   printf("%s: %d\n\r", __FUNCTION__, event_data[4][0]);
#endif

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
static void event_usbdc4_1(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[4][1] = 65;
#ifdef PRINT_ON_EVENT_CALL
   printf("%s: %d\n\r", __FUNCTION__, event_data[4][1]);
#endif

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
static void event_usbdc4_2(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[4][2] = 66;
#ifdef PRINT_ON_EVENT_CALL
   printf("%s: %d\n\r", __FUNCTION__, event_data[4][2]);
#endif

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
static void event_usbdc4_3(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[4][3] = 67;
#ifdef PRINT_ON_EVENT_CALL
   printf("%s: %d\n\r", __FUNCTION__, event_data[4][3]);
#endif

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
static void event_usbdc4_4(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[4][4] = 68;
#ifdef PRINT_ON_EVENT_CALL
   printf("%s: %d\n\r", __FUNCTION__, event_data[4][4]);
#endif

   USBD_EXIT_FUNC(MAIN_APP_TEST);
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





static void set_dummy_cfg(USB_Configuration_Desc_DT *desc, uint8_t bConfigurationValue)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   desc->bLength = USB_DESC_TYPE_CONFIGURATION_SIZE;
   desc->bDescriptorType = USB_DESC_TYPE_CONFIGURATION;
   desc->wTotalLength.L = USB_DESC_TYPE_CONFIGURATION_SIZE;
   desc->wTotalLength.H = 0;
   desc->bNumInterfaces = 0;
   desc->bConfigurationValue = bConfigurationValue;
   desc->iConfiguration = 0;
   desc->bmAttributes = 0x40;
   desc->bMaxPower = 50;

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void test_compose_configs(USBD_Params_XT *usbd)
{
   USBD_DEV_Config_Header_XT *config_table = USBD_DEV_GET_CONFIG_TAB_PTR(usbd);

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   if(0 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   set_dummy_cfg(&desc1, 1);
   set_dummy_cfg(&desc2, 2);
   set_dummy_cfg(&desc3, 3);
   set_dummy_cfg(&desc4, 4);

   USBDC_Bind_Config_Desc(usbdc1, (const uint8_t *)&desc1);
   USBDC_Bind_Config_Desc(usbdc2, (const uint8_t *)&desc2);
   USBDC_Bind_Config_Desc(usbdc3, (const uint8_t *)&desc3);
   USBDC_Bind_Config_Desc(usbdc4, (const uint8_t *)&desc4);

   install_result_1 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), usbdc1);
   install_result_2 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), usbdc2);
   install_result_3 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), usbdc3);
   install_result_4 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), usbdc4);

   if(4 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 0) != usbdc1)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 0) != (uint8_t*)(&desc1))
     || (0 != install_result_1.index)
     || (USBD_DEV_INSTALLATION_OK != install_result_1.result))
   {
      printf("config_table[0] is not same which was installed\n\r");
      REPORT_ERROR();
   }

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

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void test_compose_events(USBD_Params_XT *usbd)
{
   USBD_EVENT_Event_Header_XT *usbd_event_table = USBD_EVENT_GET_EVENT_TAB_PTR(usbd);
   USBD_EVENT_Event_Header_XT *usbdc1_event_table = USBDC_EVENT_GET_EVENT_TAB_PTR(usbdc1);
   USBD_EVENT_Event_Header_XT *usbdc2_event_table = USBDC_EVENT_GET_EVENT_TAB_PTR(usbdc2);
   USBD_EVENT_Event_Header_XT *usbdc3_event_table = USBDC_EVENT_GET_EVENT_TAB_PTR(usbdc3);
   USBD_EVENT_Event_Header_XT *usbdc4_event_table = USBDC_EVENT_GET_EVENT_TAB_PTR(usbdc4);

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   usbd_event_params[0] = USBD_EVENT_Install(usbd, event_usbd_0, USBD_EVENT_REASON_ALL);
   usbd_event_params[1] = USBD_EVENT_Install(usbd, event_usbd_1, USBD_EVENT_REASON_ALL);
   usbd_event_params[2] = USBD_EVENT_Install(usbd, event_usbd_2, USBD_EVENT_REASON_ALL);
   usbd_event_params[3] = USBD_EVENT_Install(usbd, event_usbd_3, USBD_EVENT_REASON_ALL);

   usbdc1_event_params[0] = USBDC_EVENT_Install(usbdc1, event_usbdc1_0, USBD_EVENT_REASON_ALL);
   usbdc1_event_params[1] = USBDC_EVENT_Install(usbdc1, event_usbdc1_1, USBD_EVENT_REASON_ALL);
   usbdc1_event_params[2] = USBDC_EVENT_Install(usbdc1, event_usbdc1_2, USBD_EVENT_REASON_ALL);

   usbdc2_event_params[0] = USBDC_EVENT_Install(usbdc2, event_usbdc2_0, USBD_EVENT_REASON_ALL);

   /* usbdc3 has no events */

   usbdc4_event_params[0] = USBDC_EVENT_Install(usbdc4, event_usbdc4_0, USBD_EVENT_REASON_ALL);
   usbdc4_event_params[1] = USBDC_EVENT_Install(usbdc4, event_usbdc4_1, USBD_EVENT_REASON_ALL);
   usbdc4_event_params[2] = USBDC_EVENT_Install(usbdc4, event_usbdc4_2, USBD_EVENT_REASON_ALL);
   usbdc4_event_params[3] = USBDC_EVENT_Install(usbdc4, event_usbdc4_3, USBD_EVENT_REASON_ALL);

   if(4 != usbd->event.core.data.num_installed_events)
   {
      printf("incorrect number of installed events\n\r");
      REPORT_ERROR();
   }
   if(4 != USBD_EVENT_Get_Num_Installed_Events(usbd))
   {
      printf("incorrect number of installed events\n\r");
      REPORT_ERROR();
   }

   if((usbd_event_params[0] != (&(usbd_event_table[0])))
     || (usbd_event_table[0].event != event_usbd_0))
   {
      printf("event_1 not installed correctly\n\r");
      REPORT_ERROR();
   }

   if((usbd_event_params[1] != (&(usbd_event_table[1])))
     || (usbd_event_table[1].event != event_usbd_1))
   {
      printf("event_2 not installed correctly\n\r");
      REPORT_ERROR();
   }

   if((usbd_event_params[2] != (&(usbd_event_table[2])))
     || (usbd_event_table[2].event != event_usbd_2))
   {
      printf("event_3 not installed correctly\n\r");
      REPORT_ERROR();
   }

   if((usbd_event_params[3] != (&(usbd_event_table[3])))
     || (usbd_event_table[3].event != event_usbd_3))
   {
      printf("event_4 not installed correctly\n\r");
      REPORT_ERROR();
   }



   if(3 != usbdc1->event.core.data.num_installed_events)
   {
      printf("incorrect number of installed events\n\r");
      REPORT_ERROR();
   }
   if(3 != USBDC_EVENT_Get_Num_Installed_Events(usbdc1))
   {
      printf("incorrect number of installed events\n\r");
      REPORT_ERROR();
   }

   if((usbdc1_event_params[0] != (&(usbdc1_event_table[0])))
     || (usbdc1_event_table[0].event != event_usbdc1_0))
   {
      printf("event_1 not installed correctly\n\r");
      REPORT_ERROR();
   }

   if((usbdc1_event_params[1] != (&(usbdc1_event_table[1])))
     || (usbdc1_event_table[1].event != event_usbdc1_1))
   {
      printf("event_2 not installed correctly\n\r");
      REPORT_ERROR();
   }

   if((usbdc1_event_params[2] != (&(usbdc1_event_table[2])))
     || (usbdc1_event_table[2].event != event_usbdc1_2))
   {
      printf("event_3 not installed correctly\n\r");
      REPORT_ERROR();
   }



   if(1 != usbdc2->event.core.data.num_installed_events)
   {
      printf("incorrect number of installed events\n\r");
      REPORT_ERROR();
   }
   if(1 != USBDC_EVENT_Get_Num_Installed_Events(usbdc2))
   {
      printf("incorrect number of installed events\n\r");
      REPORT_ERROR();
   }

   if((usbdc2_event_params[0] != (&(usbdc2_event_table[0])))
     || (usbdc2_event_table[0].event != event_usbdc2_0))
   {
      printf("event_1 not installed correctly\n\r");
      REPORT_ERROR();
   }



   if(4 != usbdc4->event.core.data.num_installed_events)
   {
      printf("incorrect number of installed events\n\r");
      REPORT_ERROR();
   }
   if(4 != USBDC_EVENT_Get_Num_Installed_Events(usbdc4))
   {
      printf("incorrect number of installed events\n\r");
      REPORT_ERROR();
   }

   if((usbdc4_event_params[0] != (&(usbdc4_event_table[0])))
     || (usbdc4_event_table[0].event != event_usbdc4_0))
   {
      printf("event_1 not installed correctly\n\r");
      REPORT_ERROR();
   }

   if((usbdc4_event_params[1] != (&(usbdc4_event_table[1])))
     || (usbdc4_event_table[1].event != event_usbdc4_1))
   {
      printf("event_2 not installed correctly\n\r");
      REPORT_ERROR();
   }

   if((usbdc4_event_params[2] != (&(usbdc4_event_table[2])))
     || (usbdc4_event_table[2].event != event_usbdc4_2))
   {
      printf("event_3 not installed correctly\n\r");
      REPORT_ERROR();
   }

   if((usbdc4_event_params[3] != (&(usbdc4_event_table[3])))
     || (usbdc4_event_table[3].event != event_usbdc4_3))
   {
      printf("event_3 not installed correctly\n\r");
      REPORT_ERROR();
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void activate_and_test(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   activity = 0;
   USBD_DEV_Activate(usbd, &port);

   if(USBD_DEV_STATE_OFF != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_activate(usbd)!\n\r");
      REPORT_ERROR();
   }

   if(activity != 5)
   {
      printf("port method \"activate\" didn't called! %d\n\r", activity);
      REPORT_ERROR();
   }

   activity = 0;

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void clear_event_markers(void)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   memset(event_data, 0, sizeof(event_data));

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void check_event_markers_cold(void)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   if(event_data[0][0] != 1)
   {
      printf("event_usbd_0 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[0][1] != 2)
   {
      printf("event_usbd_1 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[0][2] != 3)
   {
      printf("event_usbd_2 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[0][3] != 4)
   {
      printf("event_usbd_3 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[0][4] != 0)
   {
      printf("event_usbd_4 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[1][0] != 16)
   {
      printf("event_usbdc1_0 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[1][1] != 17)
   {
      printf("event_usbdc1_1 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[1][2] != 18)
   {
      printf("event_usbdc1_2 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[1][3] != 0)
   {
      printf("event_usbdc1_3 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[1][4] != 0)
   {
      printf("event_usbdc1_4 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[2][0] != 32)
   {
      printf("event_usbdc2_0 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[2][1] != 0)
   {
      printf("event_usbdc2_1 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[2][2] != 0)
   {
      printf("event_usbdc2_2 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[2][3] != 0)
   {
      printf("event_usbdc2_3 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[2][4] != 0)
   {
      printf("event_usbdc2_4 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[3][0] != 0)
   {
      printf("event_usbdc3_0 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[3][1] != 0)
   {
      printf("event_usbdc3_1 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[3][2] != 0)
   {
      printf("event_usbdc3_2 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[3][3] != 0)
   {
      printf("event_usbdc3_3 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[3][4] != 0)
   {
      printf("event_usbdc3_4 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[4][0] != 64)
   {
      printf("event_usbdc4_0 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[4][1] != 65)
   {
      printf("event_usbdc4_1 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[4][2] != 66)
   {
      printf("event_usbdc4_2 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[4][3] != 67)
   {
      printf("event_usbdc4_3 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[4][4] != 0)
   {
      printf("event_usbdc4_4 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void check_event_markers_warm(void)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   if(event_data[0][0] != 1)
   {
      printf("event_usbd_0 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[0][1] != 2)
   {
      printf("event_usbd_1 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[0][2] != 3)
   {
      printf("event_usbd_2 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[0][3] != 4)
   {
      printf("event_usbd_3 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[0][4] != 0)
   {
      printf("event_usbd_4 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[1][0] != 0)
   {
      printf("event_usbdc1_0 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[1][1] != 0)
   {
      printf("event_usbdc1_1 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[1][2] != 0)
   {
      printf("event_usbdc1_2 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[1][3] != 0)
   {
      printf("event_usbdc1_3 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[1][4] != 0)
   {
      printf("event_usbdc1_4 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[2][0] != 0)
   {
      printf("event_usbdc3_0 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[2][1] != 0)
   {
      printf("event_usbdc3_1 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[2][2] != 0)
   {
      printf("event_usbdc3_2 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[2][3] != 0)
   {
      printf("event_usbdc3_3 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[2][4] != 0)
   {
      printf("event_usbdc2_4 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[3][0] != 0)
   {
      printf("event_usbdc3_0 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[3][1] != 0)
   {
      printf("event_usbdc3_1 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[3][2] != 0)
   {
      printf("event_usbdc3_2 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[3][3] != 0)
   {
      printf("event_usbdc3_3 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[3][4] != 0)
   {
      printf("event_usbdc3_4 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   if(event_data[4][0] != 64)
   {
      printf("event_usbdc4_0 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[4][1] != 65)
   {
      printf("event_usbdc4_1 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[4][2] != 66)
   {
      printf("event_usbdc4_2 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[4][3] != 67)
   {
      printf("event_usbdc4_3 not called correctly!\n\t");
      REPORT_ERROR();
   }

   if(event_data[4][4] != 0)
   {
      printf("event_usbdc4_4 cannnot be called!\n\t");
      REPORT_ERROR();
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

void test_events_call(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_Init(usbd);
   USBDC_Init(usbdc1);
   USBDC_Init(usbdc2);
   USBDC_Init(usbdc3);
   USBDC_Init(usbdc4);

   test_compose_configs(usbd);
   test_compose_events(usbd);

   activate_and_test(usbd);

   clear_event_markers();
   USBD_DEV_Attached(usbd, USBD_TRUE);
   check_event_markers_cold();

   clear_event_markers();
   USBD_DEV_Configured(usbd, 4, USBD_MAKE_INVALID_HANDLER(USBD_DEV_Set_Configuration_Respond_HT));
   check_event_markers_warm();

   USBD_EXIT_FUNC(MAIN_APP_TEST);

   IOCMD_Clear_All_Logs(true);

} /* test_configs */
