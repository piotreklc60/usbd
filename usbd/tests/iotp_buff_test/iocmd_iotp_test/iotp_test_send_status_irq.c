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

#include "iotp_test_send_status_irq.h"

#ifndef USBD_H_
#include "usbd.h"
#endif

#ifndef USBD_IO_INTERNAL_H_
#include "usbd_io_internal.h"
#endif
#ifndef USBD_IOTP_BUFF_INTERNAL_H_
#include "usbd_iotp_buff_internal.h"
#endif

#include "cfg.h"
#include "port_test_control.h"
#include "main_get_exe.h"

typedef struct
{
   USBD_IOTP_BUFF_Params_XT            *tp;
   USB_EP_Direction_ET               dir;
   USBD_Bool_DT                     in_progress;
   USBD_Bool_DT                     is_tp_in;
   uint8_t                         ep_num_bufs;
   USBD_IO_Inout_Data_Size_DT           size;
   USBD_IO_Inout_Data_Size_DT           size_result;
   uint8_t                         data_result[TEST_MAX_DATA_SIZE];
}test_params_T;

/*
static USBD_IOTP_BUFF_Params_XT *test_tp;
static USB_EP_Direction_ET test_dir;
static USBD_Bool_DT test_in_progress;
static USBD_Bool_DT test_is_tp_in;
// static uint8_t ep_num;
static uint8_t test_ep_num_bufs;
static USBD_IO_Inout_Data_Size_DT test_size;
static USBD_IO_Inout_Data_Size_DT test_size_result;
static uint8_t test_data_result[TEST_EP0_MPS];
*/

static test_params_T test_params[2 * (USBD_MAX_NUM_ENDPOINTS + 1)];

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


static void test_ready(USBD_IOTP_BUFF_Params_XT *tp_params, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_DEBUG_HI_2(MAIN_APP_TEST, "%s; size = %d", __FUNCTION__, size);

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void test_buf_empty(USBD_IOTP_BUFF_Params_XT *tp_params, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   uint8_t ep_num = USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP((USBD_IOTP_BUFF_Params_XT*)tp_params);
   uint8_t ep_index = ep_num * ((USB_EP_DIRECTION_IN == dir) ? 1 : 2);
   test_params_T *test = &test_params[ep_index];

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_DEBUG_HI_2(MAIN_APP_TEST, "%s; size = %d", __FUNCTION__, size);
   test->in_progress = USBD_FALSE;

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void test_error(USBD_IOTP_BUFF_Params_XT *tp_params, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

//   test_printf("%s; size = %d\n\r", __FUNCTION__, size);

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void test_abort(USBD_IOTP_BUFF_Params_XT *tp_params, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

//   test_printf("%s; size = %d\n\r", __FUNCTION__, size);

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void prepare_test(USBD_Params_XT *usbd, uint8_t ep_index, uint8_t ep_num)
{
   test_params_T *test = &test_params[ep_index];

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_IOTP_BUFF_Set_Handlers(
      test->tp,
      test_ready,
      test_buf_empty,
      test_error,
      test_abort);

   port_test_set_data(ep_num, test->dir, test->data_result, test->size, &test->size_result);
   port_test_set_num_buffers(ep_num, test->dir, test->ep_num_bufs);
   test->size_result = 0;
   test->in_progress = USBD_TRUE;

   USBD_EXIT_FUNC(MAIN_APP_TEST);
} /* prepare_test */

static void perform_test(USBD_Params_XT *usbd, uint8_t ep_index, uint8_t ep_num)
{
   test_params_T *test = &test_params[ep_index];
   USBD_Bool_DT result = USBD_FALSE;
#ifdef TEST_LIMITED_NUM_LOOPS
   int i = 0;
#endif

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_MARK_INVOKE_DESTINATION(USBD_IOTP_BUFF_GET_INVOKE_PARAMS(test->tp));

   result = USBD_IOTP_BUFF_Send_Status(test->tp, USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));

   if(USBD_BOOL_IS_TRUE(result))
   {
      do
      {
         if(USBD_BOOL_IS_FALSE(test->is_tp_in))
         {
            break;
         }

         port_test_process_in(ep_num);

         USBD_UNMARK_INVOKE_DESTINATION(USBD_IOTP_BUFF_GET_INVOKE_PARAMS(test->tp));

#ifdef TEST_LIMITED_NUM_LOOPS
         i++;
         if(i > TEST_LIMITED_NUM_LOOPS)
         {
            break;
         }
#endif
      }while(USBD_BOOL_IS_TRUE(test->in_progress/*port_test_is_transfer_active(ep_num, test->dir)*/));
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);
} /* perform_test */

static void check_result(USBD_Params_XT *usbd, uint8_t ep_index, uint8_t ep_num)
{
   test_params_T *test = &test_params[ep_index];
   USB_Endpoint_Desc_DT *ep_desc;
   uint16_t mps;
   USBD_Bool_DT result = USBD_TRUE;

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   if(USBD_BOOL_IS_TRUE(test->is_tp_in))
   {
      if(0 != port_test_get_num_used_bufs(ep_num, USB_EP_DIRECTION_IN))
      {
         USBD_WARN_1(MAIN_APP_TEST_ERROR, "transfer not finished yet! %d bufs still in use!!!", port_test_get_num_used_bufs(ep_num, USB_EP_DIRECTION_IN));
         REPORT_ERROR();
         result = USBD_FALSE;
      }
      else if(0 != test->size_result)
      {
         USBD_WARN_2(MAIN_APP_TEST_ERROR, "data transferred size incorrect! test->size_result = %d; test->size = %d", test->size_result, 0);
         REPORT_ERROR();
         result = USBD_FALSE;
      }
      else if(1 != port_test_get_num_transactions_passed(ep_num, test->dir))
      {
         USBD_WARN_1(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: 1, current num: %d",
              port_test_get_num_transactions_passed(ep_num, test->dir));
         REPORT_ERROR();
         result = USBD_FALSE;
      }
   }
   else
   {
      if(0 != test->size_result)
      {
         USBD_WARN_2(MAIN_APP_TEST_ERROR, "data transferred size incorrect! test->size_result = %d; test->size = %d", test->size_result, test->size);
         REPORT_ERROR();
         result = USBD_FALSE;
      }
      else if(0 != port_test_get_num_transactions_passed(ep_num, test->dir))
      {
         USBD_WARN_1(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: 0, current num: %d",
              port_test_get_num_transactions_passed(ep_num, test->dir));
         REPORT_ERROR();
         result = USBD_FALSE;
      }
   }

   if(USBD_BOOL_IS_TRUE(result))
   {
      USBD_DEBUG_LO(MAIN_APP_TEST, "Test finished correctly!");
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);
} /* check_result */

void iotp_test_send_status_irq(
      USBD_Params_XT *usbd,
      USBD_IOTP_BUFF_Params_XT *tp,
      uint8_t ep_num,
      USB_EP_Direction_ET dir)
{
   uint8_t ep_index = ep_num * ((USB_EP_DIRECTION_IN == dir) ? 1 : 2);
   test_params_T *test = &test_params[ep_index];

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == tp->core.pipe_params.data.ep_type)
   {
      test->tp    = tp;
      test->dir   = dir;
      if(USB_EP_DIRECTION_IN == dir)
      {
         test->is_tp_in   = USBD_TRUE;
      }
      else
      {
         test->is_tp_in   = USBD_FALSE;
      }
      test->size   = 0;
      test->ep_num_bufs = 1;

      USBD_DEBUG_HI_4(MAIN_APP_TEST_INTRO, "%s: ep_num: %d, dir: %s, num buffers: %d",
         __FUNCTION__, ep_num, (USB_EP_DIRECTION_IN == dir) ? "IN" : "OUT", test->ep_num_bufs);

      prepare_test(usbd, ep_index, ep_num);
      perform_test(usbd, ep_index, ep_num);
      check_result(usbd, ep_index, ep_num);
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);

   IOCMD_Clear_All_Logs(true);
} /* test_init */