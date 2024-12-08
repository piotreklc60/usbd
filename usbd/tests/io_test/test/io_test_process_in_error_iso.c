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

#include "io_test_process_in_error_iso.h"

#ifndef USBD_IO_INTERNAL_H_
#include "usbd_io_internal.h"
#endif
#ifndef USBD_DEV_INTERNAL_H_
#include "usbd_dev_internal.h"
#endif

#include "dummy.h"

#include "cfg.h"
#include "main_get_exe.h"


static uint8_t working_buf[IOCMD_WORKING_BUF_RECOMMENDED_SIZE];

#define REPORT_ERROR()\
   IOCMD_Proc_Buffered_Logs(true, main_get_exe(), working_buf, sizeof(working_buf));\
   printf("file: %s\nline: %d\n", __FILE__, __LINE__);\
   report_error()

static void report_error(void)
{
   char ec;

   printf("some error occured!\n\rpress any key to continue... ");
   ec = getc(stdin);
}

static void test_activate_deactivate(USBD_Params_XT* usbd, USBD_Bool_DT state);
static USBD_Bool_DT test_parse_ep(USBD_Params_XT* usbd, uint8_t ep_num, const USB_Interface_Desc_DT *if_desc, const USB_Endpoint_Desc_DT *ep_desc);
static USBD_Bool_DT test_parse_cfg(USBD_Params_XT* usbd, const uint8_t *desc, uint16_t desc_size, USBD_DEV_Config_Desc_Check_Result_XT *details);
static USBD_DEV_Speed_ET test_get_supported_speed(USBD_Params_XT *usbd);
static USBD_DEV_Speed_ET test_get_current_speed(USBD_Params_XT *usbd);
static uint16_t test_get_dev_status(USBD_Params_XT *usbd);
static uint16_t test_get_frame_nr(USBD_Params_XT *usbd);
static USBD_DEV_PORT_Get_EP_Desc_Result_DT test_get_ep0_low_full_speed(USBD_Params_XT* usbd);
static USBD_DEV_PORT_Get_EP_Desc_Result_DT test_get_ep0_high_speed(USBD_Params_XT* usbd);

static USBD_IO_UP_EP_Handlers_XT        up_ep_handlers;
static USBD_IO_DOWN_Common_Handlers_XT  down_common_handlers;
static uint32_t sequence_cntr;
static uint8_t called_up_out;
static uint8_t called_up_in;
static uint8_t called_up_error;
static uint8_t called_up_sfin;
static uint8_t called_up_reinit;
static uint8_t called_up_abort;
static uint8_t called_down_ep_out_provide;
static uint8_t called_down_ep_out_mem_cpy;
static uint8_t called_down_ep_in_provide;
static uint8_t called_down_ep_in_mem_cpy;
static uint8_t called_down_common_trigger;
static uint8_t called_down_common_get_ep_out_waiting_size;
static uint8_t called_down_common_get_ep_in_buffered_size;
static uint8_t called_down_common_abort;
static uint8_t called_down_common_stall;
static uint8_t called_down_common_halt;
static uint8_t called_down_common_configure;
static uint32_t test_param;
static uint8_t test_ep_num;
static USB_EP_Direction_ET test_dir;
static USBD_IO_Inout_Data_Size_DT test_size;
static uint8_t data;
static USBD_Bool_DT test_setting_up = USBD_FALSE;

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

static void print_states(USBD_Params_XT *usbd)
{
   printf("%s result:\n\r", __FILE__);
   printf("called_up_out                                = %d\n\r", called_up_out);
   printf("called_up_in                                 = %d\n\r", called_up_in);
   printf("called_up_error                              = %d\n\r", called_up_error);
   printf("called_up_sfin                               = %d\n\r", called_up_sfin);
   printf("called_up_reinit                             = %d\n\r", called_up_reinit);
   printf("called_up_abort                              = %d\n\r", called_up_abort);
   printf("called_down_ep_out_provide                   = %d\n\r", called_down_ep_out_provide);
   printf("called_down_ep_out_mem_cpy                   = %d\n\r", called_down_ep_out_mem_cpy);
   printf("called_down_ep_in_provide                    = %d\n\r", called_down_ep_in_provide);
   printf("called_down_ep_in_mem_cpy                    = %d\n\r", called_down_ep_in_mem_cpy);
   printf("called_down_common_trigger                   = %d\n\r", called_down_common_trigger);
   printf("called_down_common_get_ep_out_waiting_size   = %d\n\r", called_down_common_get_ep_out_waiting_size);
   printf("called_down_common_get_ep_in_buffered_size   = %d\n\r", called_down_common_get_ep_in_buffered_size);
   printf("called_down_common_abort                     = %d\n\r", called_down_common_abort);
   printf("called_down_common_stall                     = %d\n\r", called_down_common_stall);
   printf("called_down_common_halt                      = %d\n\r", called_down_common_halt);
   printf("called_down_common_configure                 = %d\n\r", called_down_common_configure);
   printf("sequence_cntr = %d\n\r", sequence_cntr);
}

static void print_debug(USBD_Params_XT *usbd)
{
   printf("test_param = %d(0x%X)\n\rtest ep number = %d(0x%X), test dir = %s\n\r",
         test_param, test_param,
         test_ep_num, test_ep_num,
         (USB_EP_DIRECTION_OUT == test_dir) ? "out" : "in");
   print_states(usbd);
}

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

static void test_up_out(void *tp_params, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction_params, USBD_IO_Inout_Data_Size_DT size, USBD_IO_OUT_Data_Method_Port_HT mem_cpy)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   called_up_out = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

static void test_up_in(void *tp_params, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction_params, USBD_IO_Inout_Data_Size_DT size, USBD_IO_IN_Data_Method_Port_HT mem_cpy)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   called_up_in = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

static void test_up_error(void *tp_params, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction_params, USBD_IO_Inout_Data_Size_DT size)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   called_up_error = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

static void test_up_sfin(void *tp_params, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction_params)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   called_up_sfin = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

static void test_up_reinit(void *tp_params, void *tp_owner, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction_params, USBD_Bool_DT active)

{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   called_up_reinit = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

static void test_up_abort(void *tp_params, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction_params)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   called_up_abort = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}
/*
static void test_down_ep_out_provide (
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction_params,
   USBD_IO_Inout_Data_Size_DT packet_size,
   USBD_IO_Inout_Data_Size_DT left_size,
   USBD_IO_OUT_Data_Method_Port_HT data_method)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   called_down_ep_out_provide = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

static void test_down_ep_out_mem_cpy (
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction_params,
   USBD_IO_Inout_Data_Size_DT packet_size,
   USBD_IO_Inout_Data_Size_DT left_size,
   USBD_IO_OUT_Data_Method_Port_HT data_method)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   called_down_ep_out_mem_cpy = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

static void test_down_ep_in_provide (
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction_params,
   USBD_IO_IN_Data_Method_Port_HT data_method)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   called_down_ep_in_provide = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

static void test_down_ep_in_mem_cpy (
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction_params,
   USBD_IO_IN_Data_Method_Port_HT data_method)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   called_down_ep_in_mem_cpy = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}*/

static USBD_Bool_DT test_down_common_trigger(USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, USBD_Bool_DT dont_wait)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   called_down_common_trigger = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

static USBD_IO_Inout_Data_Size_DT test_down_common_get_ep_out_waiting_size(USBD_Params_XT *usbd, uint8_t ep_num, USBD_Bool_DT left_size)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   called_down_common_get_ep_out_waiting_size = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

static USBD_IO_Inout_Data_Size_DT test_down_common_get_ep_in_buffered_size(USBD_Params_XT *usbd, uint8_t ep_num)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   called_down_common_get_ep_in_buffered_size = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

static void test_down_common_abort(USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, USBD_Bool_DT flush_hw_bufs)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   called_down_common_abort = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

static void test_down_common_stall(USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   called_down_common_stall = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

static void test_down_common_halt(USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const USB_Endpoint_Desc_DT *ep_desc, USBD_Bool_DT state)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   called_down_common_halt = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

static void test_down_common_configure(USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const USB_Endpoint_Desc_DT *ep_desc, USBD_Bool_DT state)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   called_down_common_configure = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

static void prepare_test(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   called_up_out                 = 0;
   called_up_in                  = 0;
   called_up_error               = 0;
   called_up_sfin                = 0;
   called_up_reinit              = 0;
   called_up_abort               = 0;
   called_down_ep_out_provide    = 0;
   called_down_ep_out_mem_cpy    = 0;
   called_down_ep_in_provide     = 0;
   called_down_ep_in_mem_cpy     = 0;
   called_down_common_trigger    = 0;
   called_down_common_get_ep_out_waiting_size = 0;
   called_down_common_get_ep_in_buffered_size = 0;
   called_down_common_abort      = 0;
   called_down_common_stall      = 0;
   called_down_common_halt       = 0;
   called_down_common_configure  = 0;
   sequence_cntr               = 1;

   dummy_set_up_ep_handlers(&up_ep_handlers);
   dummy_set_down_common_handlers(&down_common_handlers);

   if(USB_EP_DIRECTION_OUT == test_dir)
   {
      up_ep_handlers.data_event.out = test_up_out;
   }
   else
   {
      up_ep_handlers.data_event.in  = test_up_in;
   }
   up_ep_handlers.error                         = test_up_error;
   up_ep_handlers.sfin                          = test_up_sfin;
   up_ep_handlers.reinit                        = test_up_reinit;
   up_ep_handlers.abort                         = test_up_abort;
   down_common_handlers.trigger                 = test_down_common_trigger;
   down_common_handlers.get_ep_out_waiting_size = test_down_common_get_ep_out_waiting_size;
   down_common_handlers.get_ep_in_buffered_size = test_down_common_get_ep_in_buffered_size;
   down_common_handlers.abort                   = test_down_common_abort;
   down_common_handlers.stall                   = test_down_common_stall;
   down_common_handlers.halt                    = test_down_common_halt;
   down_common_handlers.configure               = test_down_common_configure;

   USBD_IO_DOWN_Set_Common_Handlers(usbd, &down_common_handlers);

   test_setting_up = USBD_TRUE;
   if(test_param & (1 << 0))
   {
      USBD_IO_UP_Set_TP(usbd, test_ep_num, test_dir, &up_ep_handlers, USBD_MAKE_INVALID_PTR(void), USBD_MAKE_INVALID_PTR(void));
   }
   else
   {
      USBD_IO_UP_Set_TP(usbd, test_ep_num, test_dir, USBD_MAKE_INVALID_PTR(USBD_IO_UP_EP_Handlers_XT), USBD_MAKE_INVALID_PTR(void), USBD_MAKE_INVALID_PTR(void));
   }
   test_setting_up = USBD_FALSE;

   if(test_param & (1 << 1))
   {
      up_ep_handlers.error         = test_up_error;
   }
   else
   {
      up_ep_handlers.error         = USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Error_HT);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

static void perform_test(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_IO_DOWN_Process_IN_Error_Iso(usbd, test_ep_num, test_size);

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

static void check_result(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   if((test_param & (1 << 0)) && (test_param & (1 << 1)))
   {
      if(  (0 == called_up_out)
        && (0 == called_up_in)
        && (1 == called_up_error)
        && (0 == called_up_sfin)
        && (0 == called_up_reinit)
        && (0 == called_up_abort)
        && (0 == called_down_ep_out_provide)
        && (0 == called_down_ep_out_mem_cpy)
        && (0 == called_down_ep_in_provide)
        && (0 == called_down_ep_in_mem_cpy)
        && (0 == called_down_common_trigger)
        && (0 == called_down_common_get_ep_out_waiting_size)
        && (0 == called_down_common_get_ep_in_buffered_size)
        && (0 == called_down_common_abort)
        && (0 == called_down_common_stall)
        && (0 == called_down_common_halt)
        && (0 == called_down_common_configure)
        && (2 == sequence_cntr))
      {
         /* OK */
      }
      else
      {
         printf("Invalid \"called\" condition!\n\r");
         print_debug(usbd);
         REPORT_ERROR();
      }
   }
   else
   {
      if(  (0 == called_up_out)
        && (0 == called_up_in)
        && (0 == called_up_error)
        && (0 == called_up_sfin)
        && (0 == called_up_reinit)
        && (0 == called_up_abort)
        && (0 == called_down_ep_out_provide)
        && (0 == called_down_ep_out_mem_cpy)
        && (0 == called_down_ep_in_provide)
        && (0 == called_down_ep_in_mem_cpy)
        && (0 == called_down_common_trigger)
        && (0 == called_down_common_get_ep_out_waiting_size)
        && (0 == called_down_common_get_ep_in_buffered_size)
        && (0 == called_down_common_abort)
        && (0 == called_down_common_stall)
        && (0 == called_down_common_halt)
        && (0 == called_down_common_configure)
        && (1 == sequence_cntr))
      {
         /* OK */
      }
      else
      {
         printf("Invalid \"called\" condition!\n\r");
         print_debug(usbd);
         REPORT_ERROR();
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

void test_process_in_error_iso(USBD_Params_XT *usbd, uint16_t param, uint8_t ep_num, uint8_t size)
{
   test_param  = param;
   test_ep_num = ep_num;
   test_dir   = USB_EP_DIRECTION_IN;
   test_size   = size;

   prepare_test(usbd);
   perform_test(usbd);
   check_result(usbd);

   USBD_IO_UP_Set_TP(usbd, test_ep_num, test_dir, USBD_MAKE_INVALID_PTR(USBD_IO_UP_EP_Handlers_XT), USBD_MAKE_INVALID_PTR(void), USBD_MAKE_INVALID_PTR(void));
   USBD_IO_DOWN_Set_Common_Handlers(usbd, USBD_MAKE_INVALID_PTR(USBD_IO_DOWN_Common_Handlers_XT));

   IOCMD_Clear_All_Logs(true);
}
