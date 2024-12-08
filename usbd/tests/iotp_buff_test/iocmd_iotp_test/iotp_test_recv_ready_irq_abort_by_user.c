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

#include "iotp_test_recv_ready_irq_abort_by_user.h"

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
   USBD_IOTP_BUFF_Params_XT           *tp;
   USB_EP_Direction_ET               dir;
   USBD_Bool_DT                     in_progress;
   USBD_Bool_DT                     is_tp_in;
   uint8_t                         ep_num_bufs;
   uint8_t                        *data;
   USBD_IO_Inout_Data_Size_DT           size;
   USBD_IO_Inout_Data_Size_DT           size_result;
   USBD_IO_Inout_Data_Size_DT           abort_after_packets;
   uint8_t                         data_result[TEST_MAX_DATA_SIZE];
   USBD_Bool_DT                     read_in_progress;
   USBD_Bool_DT                     test_aborted;
   USBD_Bool_DT                     test_failed;
   USBD_Bool_DT                     flush_hw_bufs;
   uint8_t                         num_used_bufs;
}test_params_T;

/*
static USBD_IOTP_BUFF_Params_XT *test_tp;
static USB_EP_Direction_ET test_dir;
static USBD_Bool_DT test_in_progress;
static USBD_Bool_DT is_tp_in;
//static uint8_t test_ep_num;
static uint8_t ep_num_bufs;
static uint8_t *test_data;
static USBD_IO_Inout_Data_Size_DT test_size;
static USBD_IO_Inout_Data_Size_DT test_size_result;
static uint8_t test_data_result[TEST_MAX_DATA_SIZE];
*/

static test_params_T test_params[2 * (USBD_MAX_NUM_ENDPOINTS + 1)];

static uint8_t working_buf[IOCMD_WORKING_BUF_RECOMMENDED_SIZE];

#define REPORT_ERROR()\
   test->test_failed = USBD_TRUE;\
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

   USBD_WARN_2(MAIN_APP_TEST_ERROR, "%s called! size = %d", __FUNCTION__, size);
   REPORT_ERROR();

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void test_error(USBD_IOTP_BUFF_Params_XT *tp_params, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_WARN_2(MAIN_APP_TEST_ERROR, "%s; size = %d", __FUNCTION__, size);

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void test_abort(USBD_IOTP_BUFF_Params_XT *tp_params, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   uint8_t ep_num = USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP((USBD_IOTP_BUFF_Params_XT*)tp_params);
   uint8_t ep_index = ep_num * ((USB_EP_DIRECTION_IN == dir) ? 1 : 2);
   test_params_T *test = &test_params[ep_index];

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_DEBUG_HI_2(MAIN_APP_TEST, "%s; size = %d", __FUNCTION__, size);

   test->test_aborted = USBD_TRUE;

   Buff_Ring_Peak(test->tp->core.buff, test->data_result, test->size, 0, true);

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

   port_test_set_data(ep_num, test->dir, test->data, test->size, &test->size_result);
   port_test_set_num_buffers(ep_num, test->dir, test->ep_num_bufs);
   test->size_result = 0;
   test->in_progress = USBD_TRUE;

   USBD_EXIT_FUNC(MAIN_APP_TEST);
} /* prepare_test */

static void perform_test(USBD_Params_XT *usbd, uint8_t ep_index, uint8_t ep_num)
{
   volatile Buff_Ring_Extension_On_Remove on_remove;
   test_params_T *test = &test_params[ep_index];
   USBD_IO_Inout_Data_Size_DT size_res;
   USBD_IO_Inout_Data_Size_DT num_packets = 0;
   USBD_Bool_DT result;
#ifdef TEST_LIMITED_NUM_LOOPS
   int i = 0;
#endif

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_MARK_INVOKE_DESTINATION(USBD_IOTP_BUFF_GET_INVOKE_PARAMS(test->tp));

   test->read_in_progress = USBD_TRUE;

//   result = USBD_IOTP_BUFF_Recv_And_Ready(test->tp, test->data_result, test->size, &size_res);
   on_remove = test->tp->core.buff->extension->on_remove;
   test->tp->core.buff->extension->on_remove = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Remove);
   Buff_Ring_Clear(test->tp->core.buff, true);
   test->tp->core.buff->extension->on_remove = on_remove;

   test->read_in_progress = USBD_FALSE;

   if(BUFF_RING_IS_EMPTY(test->tp->core.buff))
   {
      do
      {
         if(USBD_BOOL_IS_TRUE(test->is_tp_in))
         {
            break;
         }

         if(num_packets >= test->abort_after_packets)
         {
            if(USBD_BOOL_IS_FALSE(test->flush_hw_bufs))
            {
               test->num_used_bufs = port_test_get_num_used_bufs(ep_num, USB_EP_DIRECTION_OUT);
            }
            USBD_IOTP_BUFF_Abort(test->tp, test->flush_hw_bufs);
            break;
         }

         result = port_test_process_out(ep_num);

#ifdef TEST_LIMITED_NUM_LOOPS
         i++;
         if(i > TEST_LIMITED_NUM_LOOPS)
         {
            break;
         }
#endif

         num_packets++;
      }while(USBD_BOOL_IS_TRUE(result/*port_test_is_transfer_active(ep_num, test->dir)*/));

      if(USBD_BOOL_IS_FALSE(test->test_aborted))
      {
         Buff_Ring_Peak(test->tp->core.buff, test->data_result, test->size, 0, true);
      }
   }
   else if(USB_EP_DIRECTION_OUT == test->dir)
   {
      USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
            test->size,
            ep_num,
            (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
            test->ep_num_bufs);
      USBD_WARN(MAIN_APP_TEST_ERROR, "USBD_IOTP_BUFF_Recv_And_Ready failed!");
      REPORT_ERROR();
   }

   USBD_UNMARK_INVOKE_DESTINATION(USBD_IOTP_BUFF_GET_INVOKE_PARAMS(test->tp));

   USBD_EXIT_FUNC(MAIN_APP_TEST);
} /* perform_test */

static void check_result(USBD_Params_XT *usbd, uint8_t ep_index, uint8_t ep_num)
{
   test_params_T *test = &test_params[ep_index];
   const USB_Endpoint_Desc_DT *ep_desc;
   uint32_t num_expected_packets;
   uint16_t mps;
   USBD_Bool_DT result = USBD_TRUE;

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   if(USBD_BOOL_IS_FALSE(test->is_tp_in))
   {
      ep_desc = USBD_DEV_Get_EP_Desc(usbd, ep_num, test->dir);

      if(USBD_CHECK_PTR(const USB_Endpoint_Desc_DT, ep_desc))
      {
         mps = 0;
         mps += ep_desc->wMaxPacketSize.H;
         mps *= 256;
         mps += ep_desc->wMaxPacketSize.L;

         if(PORT_TEST_TRUE_DMA == port_test_get_dma_type(ep_num, test->dir))
         {
         }
         else
         {
            if(0 == test->size)
            {
               num_expected_packets = 1;
            }
            else if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == (ep_desc->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK))
            {
               num_expected_packets = (test->size / mps) + 1;
            }
            else
            {
               num_expected_packets = (test->size / mps) + ((0 == (test->size % mps)) ? 0 : 1);
            }

            if((num_expected_packets > (test->abort_after_packets + 2)) && (0 != test->abort_after_packets) && USBD_BOOL_IS_FALSE(test->test_aborted))
            {
               USBD_WARN_2(MAIN_APP_TEST_ERROR, "Transfer with expected packets %d aborted after %d packets but no \"abort\" handler called!", num_expected_packets, test->abort_after_packets);
               REPORT_ERROR();
            }
            else if(test->size_result > (num_expected_packets * mps))
            {
               USBD_WARN_2(MAIN_APP_TEST_ERROR, "data transferred size incorrect! test->size_result = %d > num_expected_packets * mps = %d", test->size_result, num_expected_packets * mps);
               REPORT_ERROR();
            }
            else if(USBD_IOTP_BUFF_Get_Transferred_Size(test->tp) > (num_expected_packets * mps))
            {
               USBD_WARN_4(MAIN_APP_TEST_ERROR, "send irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                  test->size,
                  ep_num,
                  (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                  test->ep_num_bufs);
               USBD_WARN_2(MAIN_APP_TEST_ERROR, "data transferred size incorrect! test->size_result = %d > num_expected_packets * mps = %d",
                  USBD_IOTP_BUFF_Get_Transferred_Size(test->tp), num_expected_packets * mps);
               REPORT_ERROR();
            }
            else if(0 != memcmp(test->data, test->data_result, test->size_result))
            {
               USBD_COMPARE_DATA_WARN(MAIN_APP_TEST_ERROR, test->data, test->size, test->data_result, test->size, "data transferred context incorrect!");
               REPORT_ERROR();
            }
         }
      }
      else
      {
         USBD_WARN(MAIN_APP_TEST_ERROR, "ep descriptor get failed!");
         REPORT_ERROR();
      }
   }
   else
   {
      if(0 != test->size_result)
      {
         USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
              test->size,
              ep_num,
              (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
              test->ep_num_bufs);
         USBD_WARN_2(MAIN_APP_TEST_ERROR, "data transferred size incorrect! test->size_result = %d; test->size = %d", test->size_result, test->size);
         REPORT_ERROR();
      }
      else
      {
         ep_desc = USBD_DEV_Get_EP_Desc(usbd, ep_num, test->dir);

         if(USBD_CHECK_PTR(const USB_Endpoint_Desc_DT, ep_desc))
         {
            mps = 0;
            mps += ep_desc->wMaxPacketSize.H;
            mps *= 256;
            mps += ep_desc->wMaxPacketSize.L;

            if(0 != port_test_get_num_transactions_passed(ep_num, test->dir))
            {
               USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                    test->size,
                    ep_num,
                    (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                    test->ep_num_bufs);
               USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                    ((test->size / mps) + 1), port_test_get_num_transactions_passed(ep_num, test->dir));
               REPORT_ERROR();
            }
         }
         else
         {
            USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                 test->size,
                 ep_num,
                 (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                 test->ep_num_bufs);
            USBD_WARN(MAIN_APP_TEST_ERROR, "ep descriptor get failed!");
            REPORT_ERROR();
         }
      }
   }

   if(USBD_BOOL_IS_FALSE(test->test_failed))
   {
      USBD_DEBUG_LO(MAIN_APP_TEST, "Test finished correctly!");
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);
} /* check_result */

void iotp_test_recv_ready_irq_abort_by_user(
   USBD_Params_XT *usbd,
   USBD_IOTP_BUFF_Params_XT *tp,
   uint8_t ep_num,
   USB_EP_Direction_ET dir,
   uint8_t num_bufs,
   uint8_t *data,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_Inout_Data_Size_DT abort_after_packets,
   USBD_Bool_DT flush_hw_bufs)
{
   volatile Buff_Ring_Extension_On_Remove on_remove;
   uint8_t ep_index = ep_num * ((USB_EP_DIRECTION_IN == dir) ? 1 : 2);
   test_params_T *test = &test_params[ep_index];

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   test->read_in_progress = USBD_FALSE;

   if(size > TEST_MAX_DATA_SIZE)
   {
      USBD_WARN_2(MAIN_APP_TEST_INTRO, "test size = %d too big! now is cut to %d!", size, TEST_MAX_DATA_SIZE);
      REPORT_ERROR();
      size = TEST_MAX_DATA_SIZE;
   }

   test->test_failed = USBD_FALSE;

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
   test->data   = data;
   test->size   = size;
   test->abort_after_packets = abort_after_packets;
   test->flush_hw_bufs = flush_hw_bufs;
   test->num_used_bufs = 0;
   test->ep_num_bufs = 1 + ((num_bufs - 1) & 1);
   test->test_aborted = USBD_FALSE;

   USBD_DEBUG_HI_7(MAIN_APP_TEST_INTRO, "%s: size = %d, ep_num: %d, dir: %s, num buffers: %d; abort_after_packets: %d, flush_hw_bufs: %s",
      __FUNCTION__, test->size, ep_num, (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT", test->ep_num_bufs, abort_after_packets, USBD_BOOL_IS_TRUE(flush_hw_bufs) ? "true" : "false");

   prepare_test(usbd, ep_index, ep_num);
   perform_test(usbd, ep_index, ep_num);
   check_result(usbd, ep_index, ep_num);

   on_remove = test->tp->core.buff->extension->on_remove;
   test->tp->core.buff->extension->on_remove = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Remove);
   Buff_Ring_Clear(test->tp->core.buff, true);
   test->tp->core.buff->extension->on_remove = on_remove;

   test->read_in_progress = USBD_FALSE;

   USBD_EXIT_FUNC(MAIN_APP_TEST);

   IOCMD_Clear_All_Logs(true);
} /* iotp_test_recv_ready_irq_abort_by_user */
