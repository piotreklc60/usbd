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

#include "dev_test_states.h"
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

static USBD_DEV_Installation_Result_XT install_result_1;
static USB_Configuration_Desc_DT desc1;
static USBDC_Params_XT usbdcp1;
static USBDC_Params_XT *usbdc1 = &usbdcp1;

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












static void set_dummy_cfg(USB_Configuration_Desc_DT *desc)
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

static void test_powered(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_Init(usbd);

   activity = 0;
   USBD_DEV_Activate(usbd, &port);

   if(USBD_DEV_STATE_OFF != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Activate(usbd)!\n\r");
      REPORT_ERROR();
   }

   if(activity != 5)
   {
      printf("port method \"activate\" wasn't called!\n\r");
      REPORT_ERROR();
   }
   activity = 0;

   USBD_DEV_Activate(usbd, &port);

   if(USBD_DEV_STATE_OFF != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Activate(usbd)!\n\r");
      REPORT_ERROR();
   }

   if(activity == 5)
   {
      printf("port method \"activate\" was called!\n\r");
      REPORT_ERROR();
   }
   activity = 0;

   USBD_DEV_Powered(usbd, false);

   if(USBD_DEV_STATE_OFF != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Powered(usbd, false)!\n\r");
      REPORT_ERROR();
   }

   USBD_DEV_Powered(usbd, true);

   if(USBD_DEV_STATE_POWERED != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Powered(usbd, true)!\n\r");
      REPORT_ERROR();
   }

   activity = 0;
   USBD_DEV_Activate(usbd, &port);

   if(USBD_DEV_STATE_POWERED != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Activate(usbd)!\n\r");
      REPORT_ERROR();
   }

   if(activity == 5)
   {
      printf("port method \"activate\" was called!\n\r");
      REPORT_ERROR();
   }
   activity = 0;

   USBD_DEV_Powered(usbd, false);

   if(USBD_DEV_STATE_OFF != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Powered(usbd, false)!\n\r");
      REPORT_ERROR();
   }

   USBD_DEV_Powered(usbd, true);

   if(USBD_DEV_STATE_POWERED != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Powered(usbd, true)!\n\r");
      REPORT_ERROR();
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void test_compose_configs(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   if(0 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   set_dummy_cfg(&desc1);

   USBDC_Bind_Config_Desc(usbdc1, (const uint8_t*)(&desc1));

   install_result_1 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), usbdc1);

   if(1 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 0) != usbdc1)
     || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 0) != (uint8_t*)(&desc1))
     || (0 != install_result_1.index)
     || (USBD_DEV_INSTALLATION_OK != install_result_1.result))
   {
      printf("configuration installation error!!! index: %d, result: %d\n\r", install_result_1.index, install_result_1.result);
      REPORT_ERROR();
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void test_reset(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   test_powered(usbd);
   test_compose_configs(usbd);
   USBD_DEV_Reset(usbd);

   if((USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED) != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Reset(usbd)!\n\r");
      REPORT_ERROR();
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void test_addressed(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   test_reset(usbd);

   USBD_DEV_Addressed(usbd, 2);

   if((USBD_DEV_STATE_ADDRESSED | USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED) != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Addressed(usbd)!\n\r");
      REPORT_ERROR();
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void test_configured(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   test_addressed(usbd);

   USBD_DEV_Configured(usbd, 0, USBD_MAKE_INVALID_HANDLER(USBD_DEV_Set_Configuration_Respond_HT));

   if((USBD_DEV_STATE_ADDRESSED | USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED) != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Configured(usbd, 0, NULL)!\n\r");
      REPORT_ERROR();
   }

   USBD_DEV_Configured(usbd, 1, USBD_MAKE_INVALID_HANDLER(USBD_DEV_Set_Configuration_Respond_HT));

   if((USBD_DEV_STATE_CONFIGURED | USBD_DEV_STATE_ADDRESSED | USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED) != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Configured(usbd, 1, NULL)!\n\r");
      REPORT_ERROR();
   }

   USBD_DEV_Configured(usbd, 0, USBD_MAKE_INVALID_HANDLER(USBD_DEV_Set_Configuration_Respond_HT));

   if((USBD_DEV_STATE_ADDRESSED | USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED) != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Configured(usbd, 0, NULL)!\n\r");
      REPORT_ERROR();
   }

   USBD_DEV_Configured(usbd, 1, USBD_MAKE_INVALID_HANDLER(USBD_DEV_Set_Configuration_Respond_HT));

   if((USBD_DEV_STATE_CONFIGURED | USBD_DEV_STATE_ADDRESSED | USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED) != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Configured(usbd, 1, NULL)!\n\r");
      REPORT_ERROR();
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void test_suspended(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   test_reset(usbd);

   USBD_DEV_Suspended(usbd);

   if((USBD_DEV_STATE_SUSPENDED | USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED) != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Suspended(usbd)!\n\r");
      REPORT_ERROR();
   }

   USBD_DEV_SOF_Received(usbd);

   if((USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED) != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_SOF_Received(usbd)!\n\r");
      REPORT_ERROR();
   }

   USBD_DEV_Suspended(usbd);

   if((USBD_DEV_STATE_SUSPENDED | USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED) != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Suspended(usbd)!\n\r");
      REPORT_ERROR();
   }

   USBD_DEV_SOF_Received(usbd);

   if((USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED) != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_SOF_Received(usbd)!\n\r");
      REPORT_ERROR();
   }



   test_addressed(usbd);

   USBD_DEV_Suspended(usbd);

   if((USBD_DEV_STATE_SUSPENDED | USBD_DEV_STATE_ADDRESSED | USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED) != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Suspended(usbd)!\n\r");
      REPORT_ERROR();
   }

   USBD_DEV_SOF_Received(usbd);

   if((USBD_DEV_STATE_ADDRESSED | USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED) != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_SOF_Received(usbd)!\n\r");
      REPORT_ERROR();
   }

   USBD_DEV_Suspended(usbd);

   if((USBD_DEV_STATE_SUSPENDED | USBD_DEV_STATE_ADDRESSED | USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED) != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Suspended(usbd)!\n\r");
      REPORT_ERROR();
   }

   USBD_DEV_SOF_Received(usbd);

   if((USBD_DEV_STATE_ADDRESSED | USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED) != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_SOF_Received(usbd)!\n\r");
      REPORT_ERROR();
   }



   test_configured(usbd);

   USBD_DEV_Suspended(usbd);

   if((USBD_DEV_STATE_SUSPENDED | USBD_DEV_STATE_CONFIGURED | USBD_DEV_STATE_ADDRESSED | USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED) != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Suspended(usbd)!\n\r");
      REPORT_ERROR();
   }

   USBD_DEV_SOF_Received(usbd);

   if((USBD_DEV_STATE_CONFIGURED | USBD_DEV_STATE_ADDRESSED | USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED) != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_SOF_Received(usbd)!\n\r");
      REPORT_ERROR();
   }

   USBD_DEV_Suspended(usbd);

   if((USBD_DEV_STATE_SUSPENDED | USBD_DEV_STATE_CONFIGURED | USBD_DEV_STATE_ADDRESSED | USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED) != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Suspended(usbd)!\n\r");
      REPORT_ERROR();
   }

   USBD_DEV_SOF_Received(usbd);

   if((USBD_DEV_STATE_CONFIGURED | USBD_DEV_STATE_ADDRESSED | USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED) != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_SOF_Received(usbd)!\n\r");
      REPORT_ERROR();
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}


void test_states(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   test_suspended(usbd);

   activity = 0;
   USBD_DEV_Deactivate(usbd);

   if(USBD_DEV_STATE_OFF != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Deactivate(usbd)!\n\r");
      REPORT_ERROR();
   }

   if(activity != 11)
   {
      printf("port method \"activate\" wasn't called!\n\r");
      REPORT_ERROR();
   }
   activity = 0;

   USBD_DEV_Deactivate(usbd);

   if(USBD_DEV_STATE_OFF != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Deactivate(usbd)!\n\r");
      REPORT_ERROR();
   }

   if(activity != 11)
   {
      printf("port method \"activate\" wasn't called!\n\r");
      REPORT_ERROR();
   }
   activity = 0;

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
