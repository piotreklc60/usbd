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

#include "iotp_test_recv_ready_irq_too_less_space.h"

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
   USBD_IOTP_BUFF_Params_XT  *tp;
   Buff_Size_DT               buff_original_size;
   USB_EP_Direction_ET       dir;
   USBD_Bool_DT               in_progress;
   USBD_Bool_DT               is_tp_in;
   uint8_t                    ep_num_bufs;
   uint16_t                   mps;
   uint8_t                   *data;
   USBD_IO_Inout_Data_Size_DT size;
   USBD_IO_Inout_Data_Size_DT size_result;
   USBD_IO_Inout_Data_Size_DT size_read;
   uint8_t                    data_result[TEST_MAX_DATA_SIZE];
   USBD_Bool_DT               test_failed;
   uint8_t                    test_ready_called;
}test_params_T;

/*
static USBD_IOTP_BUFF_Params_XT *test_tp;
static USB_EP_Direction_ET test_dir;
static USBD_Bool_DT test_in_progress;
static USBD_Bool_DT test_is_tp_in;
//static uint8_t test_ep_num;
static uint8_t test_ep_num_bufs;
static uint16_t test_mps;
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
   uint8_t ep_num = USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP((USBD_IOTP_BUFF_Params_XT*)tp_params);
   uint8_t ep_index = ep_num * ((USB_EP_DIRECTION_IN == dir) ? 1 : 2);
   test_params_T *test = &test_params[ep_index];

   USBD_IO_Inout_Data_Size_DT size_res;
   Buff_Size_DT size_read;
   const USB_Endpoint_Desc_DT *ep_desc;
   USBD_Bool_DT is_control = USBD_FALSE;

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_DEBUG_HI_3(MAIN_APP_TEST, "%s; size = %d; test_ready_called: %u", __FUNCTION__, size, test->test_ready_called);

   if(0 == test->test_ready_called)
   {
      if(0 != (BUFF_RING_GET_SIZE(test->tp->core.buff) % test->tp->core.pipe_params.data.mps))
      {
         if(size < 0)
         {
            USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                  test->size,
                  ep_num,
                  (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                  test->ep_num_bufs);
            USBD_WARN_2(MAIN_APP_TEST_ERROR, "endpoint is not waiting on first %s callback when size = %d!", __FUNCTION__, size);
            REPORT_ERROR();
         }
      }
      else
      {
         if(size >= 0)
         {
            USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                  test->size,
                  ep_num,
                  (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                  test->ep_num_bufs);
            USBD_WARN_2(MAIN_APP_TEST_ERROR, "endpoint is not waiting on first %s callback when size = %d!", __FUNCTION__, size);
            REPORT_ERROR();
         }
      }

      size_read = BUFF_RING_GET_BUSY_SIZE(test->tp->core.buff);
      test->size_read += Buff_Ring_Read(test->tp->core.buff, &test->data_result[test->size_read], size_read, true);

      test->test_ready_called = 1;
   }
   else if(1 == test->test_ready_called)
   {
      if(0 != ((BUFF_RING_GET_SIZE(test->tp->core.buff) * 2) % test->tp->core.pipe_params.data.mps))
      {
         if(size < 0)
         {
            USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                  test->size,
                  ep_num,
                  (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                  test->ep_num_bufs);
            USBD_WARN_2(MAIN_APP_TEST_ERROR, "endpoint is not waiting on first %s callback when size = %d!", __FUNCTION__, size);
            REPORT_ERROR();
         }
      }
      else
      {
         if(size >= 0)
         {
            USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                  test->size,
                  ep_num,
                  (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                  test->ep_num_bufs);
            USBD_WARN_2(MAIN_APP_TEST_ERROR, "endpoint is not waiting on first %s callback when size = %d!", __FUNCTION__, size);
            REPORT_ERROR();
         }
      }

      test->test_ready_called = 2;
   }
   else
   {
      if(size >= 0)
      {
         USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
               test->size,
               ep_num,
               (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
               test->ep_num_bufs);
         USBD_WARN_2(MAIN_APP_TEST_ERROR, "endpoint is waiting on second %s callback when size = %d!", __FUNCTION__, size);
         REPORT_ERROR();
      }

      size_read = BUFF_RING_GET_BUSY_SIZE(test->tp->core.buff);
      test->size_read += Buff_Ring_Read(test->tp->core.buff, &test->data_result[test->size_read], size_read, true);

      if(0 == test->size_read)
      {
         USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
              size_read,
              ep_num,
              (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
              test->ep_num_bufs);
         USBD_WARN(MAIN_APP_TEST_ERROR, "Buff_Ring_Read failed!");
         REPORT_ERROR();
      }

      test->in_progress = /*USBD_TRUE*/USBD_IOTP_BUFF_Is_Transfer_Active(tp_params);
   }

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
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_WARN_2(MAIN_APP_TEST_ERROR, "%s; size = %d", __FUNCTION__, size);

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
   test->size_read = 0;
   test->in_progress = USBD_TRUE;

   USBD_EXIT_FUNC(MAIN_APP_TEST);
} /* prepare_test */

static void perform_test(USBD_Params_XT *usbd, uint8_t ep_index, uint8_t ep_num)
{
   volatile Buff_Ring_Extension_On_Remove on_remove;
   test_params_T *test = &test_params[ep_index];

   USBD_IO_Inout_Data_Size_DT size_res;
   Buff_Size_DT size_read;
   USBD_Bool_DT result;
#ifdef TEST_LIMITED_NUM_LOOPS
   int i = 0;
#endif

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_MARK_INVOKE_DESTINATION(USBD_IOTP_BUFF_GET_INVOKE_PARAMS(test->tp));

//   result = USBD_IOTP_BUFF_Recv_And_Ready(test->tp, test->data_result, test->size/4, &size_res);
   on_remove = test->tp->core.buff->extension->on_remove;
   test->tp->core.buff->extension->on_remove = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Remove);
   Buff_Ring_Clear(test->tp->core.buff, true);
   test->tp->core.buff->extension->on_remove = on_remove;
   test->tp->core.buff->size = test->size / 4;
   test->tp->core.buff->original_size = test->size / 4;

   if(BUFF_RING_IS_EMPTY(test->tp->core.buff))
   {
      if(USBD_BOOL_IS_FALSE(test->is_tp_in))
      {
         do
         {
            result = port_test_process_out(ep_num);

#ifdef TEST_LIMITED_NUM_LOOPS
            i++;
            if(i > TEST_LIMITED_NUM_LOOPS)
            {
               break;
            }
#endif
         }while((2 != test->test_ready_called) && USBD_BOOL_IS_TRUE(result));

         if(0 != ((BUFF_RING_GET_SIZE(test->tp->core.buff) * 2) % test->tp->core.pipe_params.data.mps))
         {
            if((-1) == USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(USBD_IOTP_BUFF_Get_USBD(test->tp), ep_num, USBD_FALSE))
            {
               USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                     test->size,
                     ep_num,
                     (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                     test->ep_num_bufs);
               USBD_WARN(MAIN_APP_TEST_ERROR, "endpoint is not waiting when buffer is full");
               REPORT_ERROR();
            }
         }
         else
         {
            if((-1) != USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(USBD_IOTP_BUFF_Get_USBD(test->tp), ep_num, USBD_FALSE))
            {
               USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                     test->size,
                     ep_num,
                     (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                     test->ep_num_bufs);
               USBD_WARN(MAIN_APP_TEST_ERROR, "endpoint is not waiting when buffer is full");
               REPORT_ERROR();
            }
         }

         size_read = BUFF_RING_GET_BUSY_SIZE(test->tp->core.buff);
         test->size_read += Buff_Ring_Read(test->tp->core.buff, &test->data_result[test->size_read], size_read, true);

         if(0 == test->size_read)
         {
            USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                 size_read,
                 ep_num,
                 (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                 test->ep_num_bufs);
            USBD_WARN(MAIN_APP_TEST_ERROR, "Buff_Ring_Read failed!");
            REPORT_ERROR();
         }

         USBD_NOTICE_1(MAIN_APP_TEST_ERROR, "restore buffer original size %u", test->buff_original_size);

         test->tp->core.buff->size = test->buff_original_size;
         test->tp->core.buff->original_size = test->buff_original_size;

         if((-1) != USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(USBD_IOTP_BUFF_Get_USBD(test->tp), ep_num, USBD_FALSE))
         {
            USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                  test->size,
                  ep_num,
                  (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                  test->ep_num_bufs);
            USBD_WARN(MAIN_APP_TEST_ERROR, "endpoint is not waiting after read re-triggered");
            REPORT_ERROR();
         }

         do
         {
            result = port_test_process_out(ep_num);

#ifdef TEST_LIMITED_NUM_LOOPS
            i++;
            if(i > TEST_LIMITED_NUM_LOOPS)
            {
               break;
            }
#endif
         }while(USBD_BOOL_IS_TRUE(result));
      }
   }
   else if(USB_EP_DIRECTION_OUT == test->dir)
   {
      USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
            test->size/4,
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
   uint32_t num_expected_transactions_passed;
   uint16_t mps;

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   if(USBD_BOOL_IS_FALSE(test->is_tp_in))
   {
      if(USBD_BOOL_IS_TRUE(test->in_progress) && (0 == (test->size % test->tp->core.pipe_params.data.mps)))
      {
         test->size_read += Buff_Ring_Read(test->tp->core.buff, &test->data_result[test->size_read], BUFF_RING_GET_BUSY_SIZE(test->tp->core.buff), true);
      }

      if(USBD_BOOL_IS_TRUE(test->in_progress) && (0 != (test->size % test->tp->core.pipe_params.data.mps)))
      {
         USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
              test->size,
              ep_num,
              (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
              test->ep_num_bufs);
         USBD_WARN_2(MAIN_APP_TEST_ERROR, "transfer still active! test->size_result = %d; test->size = %d", test->size_result, test->size);
         REPORT_ERROR();
      }
      else if(test->size_result != test->size)
      {
         USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
              test->size,
              ep_num,
              (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
              test->ep_num_bufs);
         USBD_WARN_2(MAIN_APP_TEST_ERROR, "data transferred size incorrect! test->size_result = %d; test->size = %d", test->size_result, test->size);
         REPORT_ERROR();
      }
      else if(test->size_result != test->size_read)
      {
         USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
              test->size,
              ep_num,
              (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
              test->ep_num_bufs);
         USBD_WARN_2(MAIN_APP_TEST_ERROR, "data transferred size incorrect! test->size_result = %d; test->size_read = %d", test->size_result, test->size_read);
         REPORT_ERROR();
      }
      else if(0 != memcmp(test->data, test->data_result, test->size))
      {
         USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
              test->size,
              ep_num,
              (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
              test->ep_num_bufs);
         USBD_COMPARE_DATA_WARN(MAIN_APP_TEST_ERROR, test->data, test->size, test->data_result, test->size, "data transferred context incorrect!");
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

            if(0 == test->size)
            {
               num_expected_transactions_passed = 1;
               if(num_expected_transactions_passed != port_test_get_num_transactions_passed(ep_num, test->dir))
               {
                  USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                       test->size,
                       ep_num,
                       (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                       test->ep_num_bufs);
                  USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                       num_expected_transactions_passed, port_test_get_num_transactions_passed(ep_num, test->dir));
                  REPORT_ERROR();
               }
            }
            else if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == (ep_desc->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK))
            {
               num_expected_transactions_passed = (test->size / mps) + 1;
               if(num_expected_transactions_passed != port_test_get_num_transactions_passed(ep_num, test->dir))
               {
                  USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                       test->size,
                       ep_num,
                       (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                       test->ep_num_bufs);
                  USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                       num_expected_transactions_passed, port_test_get_num_transactions_passed(ep_num, test->dir));
                  REPORT_ERROR();
               }
            }
            else
            {
               num_expected_transactions_passed = (test->size / mps) + ((0 == (test->size % mps)) ? 0 : 1);
               if(num_expected_transactions_passed != port_test_get_num_transactions_passed(ep_num, test->dir))
               {
                  USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_ready irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                       test->size,
                       ep_num,
                       (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                       test->ep_num_bufs);
                  USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                       num_expected_transactions_passed, port_test_get_num_transactions_passed(ep_num, test->dir));
                  REPORT_ERROR();
               }
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


void printf_ep_desc(const USB_Endpoint_Desc_DT *ep_desc)
{
   USBD_WARN_1(MAIN_APP_TEST, "ep_desc = 0x%X", ep_desc);
   USBD_WARN_1(MAIN_APP_TEST, "bLength = %d", ep_desc->bLength);
   USBD_WARN_1(MAIN_APP_TEST, "bDescriptorType = %d", ep_desc->bDescriptorType);
   USBD_WARN_1(MAIN_APP_TEST, "bEndpointAddress = %d", ep_desc->bEndpointAddress);
   USBD_WARN_1(MAIN_APP_TEST, "bmAttributes = %d", ep_desc->bmAttributes);
   USBD_WARN_1(MAIN_APP_TEST, "wMaxPacketSize = %d", (ep_desc->wMaxPacketSize.H * 256) + ep_desc->wMaxPacketSize.L);
   USBD_WARN_1(MAIN_APP_TEST, "bInterval = %d", ep_desc->bInterval);
}

void iotp_test_recv_ready_irq_too_less_space(USBD_Params_XT *usbd, USBD_IOTP_BUFF_Params_XT *tp, uint8_t ep_num, USB_EP_Direction_ET dir, uint8_t num_bufs, uint8_t *data, USBD_IO_Inout_Data_Size_DT size)
{
   volatile Buff_Ring_Extension_On_Remove on_remove;
   uint8_t ep_index = ep_num * ((USB_EP_DIRECTION_IN == dir) ? 1 : 2);
   test_params_T *test = &test_params[ep_index];
   const USB_Endpoint_Desc_DT *ep_desc;
   uint16_t mps = 0xFFFF;

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   ep_desc = USBD_DEV_Get_EP_Desc(USBD_IOTP_BUFF_Get_USBD(tp), ep_num, dir);

   if(USBD_CHECK_PTR(const USB_Endpoint_Desc_DT, ep_desc))
   {
      mps = 0;
      mps += ep_desc->wMaxPacketSize.H;
      mps *= 256;
      mps += ep_desc->wMaxPacketSize.L;
   }

   if(size > (3 * mps))
   {
      if(size > TEST_MAX_DATA_SIZE)
      {
         USBD_WARN_2(MAIN_APP_TEST_INTRO, "test size = %d too big! now is cut to %d!", size, TEST_MAX_DATA_SIZE);
         REPORT_ERROR();
         size = TEST_MAX_DATA_SIZE;
      }

      test->test_failed       = USBD_FALSE;
      test->test_ready_called = 0;

      test->tp    = tp;
      test->buff_original_size = test->tp->core.buff->size;
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
      test->ep_num_bufs = 1 + ((num_bufs - 1) & 1);
      test->mps = mps;

      USBD_DEBUG_HI_6(MAIN_APP_TEST_INTRO, "%s: size = %d, mps = %d, ep_num: %d, dir: %s, num buffers: %d",
         __FUNCTION__, test->size, mps, ep_num, (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT", test->ep_num_bufs);
//       printf_ep_desc(ep_desc);

      prepare_test(usbd, ep_index, ep_num);
      perform_test(usbd, ep_index, ep_num);
      check_result(usbd, ep_index, ep_num);

      on_remove = test->tp->core.buff->extension->on_remove;
      test->tp->core.buff->extension->on_remove = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Remove);
      Buff_Ring_Clear(test->tp->core.buff, true);
      test->tp->core.buff->extension->on_remove = on_remove;
      test->tp->core.buff->size           = test->buff_original_size;
      test->tp->core.buff->original_size  = test->buff_original_size;
   }
   else
   {
//       printf("\n\n\r%s: size = %d, mps = %d, ep_num: %d, dir: %s, num buffers: %d\n\r", __FUNCTION__, size, mps, ep_num, (USB_EP_DIRECTION_IN == dir) ? "IN" : "OUT", num_bufs);
//       printf("condition: size > (2 * mps) not met!\n\r");
//       REPORT_ERROR();
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);

   IOCMD_Clear_All_Logs(true);
} /* test_init */
