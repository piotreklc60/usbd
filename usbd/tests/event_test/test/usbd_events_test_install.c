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

#include "usbd_events_test_install.h"
#ifndef USBD_EVENT_INTERNAL_H_
#include "usbd_event_internal.h"
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
static const USB_Endpoint_Desc_DT* test_get_ep0_low_full_speed(USBD_Params_XT* usbd);
static const USB_Endpoint_Desc_DT* test_get_ep0_high_speed(USBD_Params_XT* usbd);

static USBD_EVENT_Event_Header_XT *event_params[6];

static uint8_t event_data[6];

static USBD_Bool_DT remove_result;

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


static void event_1(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[0] = 1;

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
static void event_2(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[1] = 2;

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
static void event_3(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[2] = 3;

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
static void event_4(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[3] = 4;

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
static void event_5(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[4] = 5;

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
static void event_6(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   event_data[5] = 6;

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









void test_usbd_events_install(USBD_Params_XT *usbd)
{
   USBD_EVENT_Event_Header_XT *event_table;
   USBD_DEV_Config_Header_XT  *config_table;

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_Init(usbd);

   event_table = USBD_EVENT_GET_EVENT_TAB_PTR(usbd);

   if(0 != usbd->event.core.data.num_installed_events)
   {
      printf("incorrect number of installed configs: %d\n\r", usbd->event.core.data.num_installed_events);
      REPORT_ERROR();
   }

   event_params[0] = USBD_EVENT_Install(usbd, event_1, USBD_EVENT_REASON_DETACHED);
   event_params[1] = USBD_EVENT_Install(usbd, event_2, (USBD_EVENT_REASON_ATTACHED | USBD_EVENT_REASON_UNPOWERED | USBD_EVENT_REASON_POWERED));
   event_params[2] = USBD_EVENT_Install(usbd, event_3, (USBD_EVENT_REASON_RESET | USBD_EVENT_REASON_ADDRESSED));
   event_params[3] = USBD_EVENT_Install(usbd, event_4, (USBD_EVENT_REASON_UNCONFIGURED | USBD_EVENT_REASON_SUSPENDED));
   event_params[4] = USBD_EVENT_Install(usbd, event_5, (USBD_EVENT_REASON_CONFIGURED | USBD_EVENT_REASON_SOF_RECEIVED));
   event_params[5] = USBD_EVENT_Install(usbd, event_6, USBD_EVENT_REASON_ALL);

   if(5 != usbd->event.core.data.num_installed_events)
   {
      printf("incorrect number of installed events: %d\n\r", usbd->event.core.data.num_installed_events);
      REPORT_ERROR();
   }

   if((event_params[0] != (&(event_table[0])))
     || (event_table[0].event != event_1)
     || (USBD_EVENT_REASON_DETACHED != event_table[0].mask))
   {
      printf("event_1 not installed correctly\n\r");
      REPORT_ERROR();
   }

   if((event_params[1] != (&(event_table[1])))
     || (event_table[1].event != event_2)
     || ((USBD_EVENT_REASON_ATTACHED | USBD_EVENT_REASON_UNPOWERED | USBD_EVENT_REASON_POWERED) != event_table[1].mask))
   {
      printf("event_2 not installed correctly\n\r");
      REPORT_ERROR();
   }

   if((event_params[2] != (&(event_table[2])))
     || (event_table[2].event != event_3)
     || ((USBD_EVENT_REASON_RESET | USBD_EVENT_REASON_ADDRESSED) != event_table[2].mask))
   {
      printf("event_3 not installed correctly\n\r");
      REPORT_ERROR();
   }

   if((event_params[3] != (&(event_table[3])))
     || (event_table[3].event != event_4)
     || ((USBD_EVENT_REASON_UNCONFIGURED | USBD_EVENT_REASON_SUSPENDED) != event_table[3].mask))
   {
      printf("event_4 not installed correctly\n\r");
      REPORT_ERROR();
   }

   if((event_params[4] != (&(event_table[4])))
     || (event_table[4].event != event_5)
     || ((USBD_EVENT_REASON_CONFIGURED | USBD_EVENT_REASON_SOF_RECEIVED) != event_table[4].mask))
   {
      printf("event_5 not installed correctly\n\r");
      REPORT_ERROR();
   }

   if(USBD_MAKE_INVALID_PTR(USBD_EVENT_Event_Header_XT) != event_params[5])
   {
      printf("installation of event_6 returned unexpected result\n\r");
      REPORT_ERROR();
   }

   remove_result = USBD_EVENT_Remove_All_Events(usbd);

   if((0 != usbd->event.core.data.num_installed_events) || USBD_BOOL_IS_FALSE(remove_result))
   {
      printf("incorrect number of installed events\n\r");
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
      printf("port method \"activate\" didn't called! %d\n\r", activity);
      REPORT_ERROR();
   }

   activity = 0;

   event_params[0] = USBD_EVENT_Install(usbd, event_1, USBD_EVENT_REASON_ALL);

   event_params[0]->vendor.pvoid = (void*)(&event_params[0]);
   event_params[0]->vendor_data.pvoid = (void*)(&event_params[3]);

   if(1 != usbd->event.core.data.num_installed_events)
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   if((event_params[0] != (&(event_table[0])))
     || (event_table[0].event != event_1)
     || (USBD_EVENT_REASON_ALL != event_table[0].mask))
   {
      printf("event_1 not installed correctly\n\r");
      REPORT_ERROR();
   }

   event_params[1] = USBD_EVENT_Install(usbd, event_2, USBD_EVENT_REASON_ALL);

   event_params[1]->vendor.pvoid = (void*)(&event_params[1]);
   event_params[1]->vendor_data.pvoid = (void*)(&event_params[4]);

   if(2 != usbd->event.core.data.num_installed_events)
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   if((event_table[0].event != event_1)
     || (USBD_EVENT_REASON_ALL != event_table[0].mask))
   {
      printf("event_1 not installed correctly\n\r");
      REPORT_ERROR();
   }

   if((event_params[1] != (&(event_table[1])))
     || (event_table[1].event != event_2)
     || (USBD_EVENT_REASON_ALL != event_table[1].mask))
   {
      printf("event_2 not installed correctly\n\r");
      REPORT_ERROR();
   }

   event_params[2] = USBD_EVENT_Install(usbd, event_3, USBD_EVENT_REASON_ALL);

   event_params[2]->vendor.pvoid = (void*)(&event_params[2]);
   event_params[2]->vendor_data.pvoid = (void*)(&event_params[5]);

   if(3 != usbd->event.core.data.num_installed_events)
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   if((event_table[0].event != event_1)
     || (USBD_EVENT_REASON_ALL != event_table[0].mask))
   {
      printf("event_1 not installed correctly\n\r");
      REPORT_ERROR();
   }

   if((event_table[1].event != event_2)
     || (USBD_EVENT_REASON_ALL != event_table[1].mask))
   {
      printf("event_2 not installed correctly\n\r");
      REPORT_ERROR();
   }

   if((event_params[2] != (&(event_table[2])))
     || (event_table[2].event != event_3)
     || (USBD_EVENT_REASON_ALL != event_table[2].mask))
   {
      printf("event_3 not installed correctly\n\r");
      REPORT_ERROR();
   }

   remove_result = USBD_EVENT_Remove_Event(usbd, event_2);

   if((2 != usbd->event.core.data.num_installed_events) || USBD_BOOL_IS_FALSE(remove_result))
   {
      printf("incorrect number of installed events, %d\n\r", usbd->event.core.data.num_installed_events);
      REPORT_ERROR();
   }

   if((event_table[0].event != event_1)
     || (event_params[0]->vendor.pvoid != (void*)(&event_params[0]))
     || (event_params[0]->vendor_data.pvoid != (void*)(&event_params[3])))
   {
      printf("event_1 not still installed\n\r");
      REPORT_ERROR();
   }

   if((event_table[1].event != event_3)
     || (event_params[2]->vendor.pvoid != (void*)(&event_params[2]))
     || (event_params[2]->vendor_data.pvoid != (void*)(&event_params[5])))
   {
      printf("event_2 not still installed\n\r");
      REPORT_ERROR();
   }

   remove_result = USBD_EVENT_Remove_Event(usbd, event_2);

   if((2 != usbd->event.core.data.num_installed_events) || USBD_BOOL_IS_TRUE(remove_result))
   {
      printf("incorrect number of installed events, %d\n\r", usbd->event.core.data.num_installed_events);
      REPORT_ERROR();
   }

   if((event_table[0].event != event_1)
     || (event_params[0]->vendor.pvoid != (void*)(&event_params[0]))
     || (event_params[0]->vendor_data.pvoid != (void*)(&event_params[3])))
   {
      printf("event_1 not still installed\n\r");
      REPORT_ERROR();
   }

   if((event_table[1].event != event_3)
     || (event_params[1]->vendor.pvoid != (void*)(&event_params[2]))
     || (event_params[1]->vendor_data.pvoid != (void*)(&event_params[5])))
   {
      printf("event_2 not still installed\n\r");
      REPORT_ERROR();
   }

   remove_result = USBD_EVENT_Remove_Event(usbd, event_1);

   if((1 != usbd->event.core.data.num_installed_events) || USBD_BOOL_IS_FALSE(remove_result))
   {
      printf("incorrect number of installed events, %d\n\r", usbd->event.core.data.num_installed_events);
      REPORT_ERROR();
   }

   if((event_table[0].event != event_3)
     || (event_params[0]->vendor.pvoid != (void*)(&event_params[2]))
     || (event_params[0]->vendor_data.pvoid != (void*)(&event_params[5])))
   {
      printf("event_2 not still installed\n\r");
      REPORT_ERROR();
   }

   remove_result = USBD_EVENT_Remove_Event(usbd, event_3);

   if((0 != usbd->event.core.data.num_installed_events) || USBD_BOOL_IS_FALSE(remove_result))
   {
      printf("incorrect number of installed events\n\r");
      REPORT_ERROR();
   }

   remove_result = USBD_EVENT_Remove_Event(usbd, event_3);

   if((0 != usbd->event.core.data.num_installed_events) || USBD_BOOL_IS_TRUE(remove_result))
   {
      printf("incorrect number of installed events\n\r");
      REPORT_ERROR();
   }

   event_params[0] = USBD_EVENT_Install(usbd, event_1, USBD_EVENT_REASON_ALL);
   event_params[1] = USBD_EVENT_Install(usbd, event_2, USBD_EVENT_REASON_ALL);
   event_params[2] = USBD_EVENT_Install(usbd, event_3, USBD_EVENT_REASON_ALL);

   if(3 != usbd->event.core.data.num_installed_events)
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   if((event_params[0] != (&(event_table[0])))
     || (event_table[0].event != event_1)
     || (USBD_EVENT_REASON_ALL != event_table[0].mask))
   {
      printf("event_1 not installed correctly\n\r");
      REPORT_ERROR();
   }

   if((event_params[1] != (&(event_table[1])))
     || (event_table[1].event != event_2)
     || (USBD_EVENT_REASON_ALL != event_table[1].mask))
   {
      printf("event_2 not installed correctly\n\r");
      REPORT_ERROR();
   }

   if((event_params[2] != (&(event_table[2])))
     || (event_table[2].event != event_3)
     || (USBD_EVENT_REASON_ALL != event_table[2].mask))
   {
      printf("event_3 not installed correctly\n\r");
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
      printf("port method \"activate\" didn't called!\n\r");
      REPORT_ERROR();
   }
   activity = 0;

   remove_result = USBD_EVENT_Remove_All_Events(usbd);

   if((0 != usbd->event.core.data.num_installed_events) || USBD_BOOL_IS_FALSE(remove_result))
   {
      printf("incorrect number of installed events\n\r");
      REPORT_ERROR();
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);

   IOCMD_Clear_All_Logs(true);
} /* test_configs */
