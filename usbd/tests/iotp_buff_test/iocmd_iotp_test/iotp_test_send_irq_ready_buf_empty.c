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

#include "iotp_test_send_irq_ready_buf_empty.h"

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

static void print_transactions_context(uint8_t ep_index, uint8_t ep_num);

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
   uint8_t                         data_result[TEST_MAX_DATA_SIZE * (1 + TEST_MAX_NUM_CONNECTED_TRANSACTIONS)];
   iotp_test_send_irq_transaction_req_XT  *transactions;
   size_t                          data_result_pos;
   size_t                          num_transactions;
   size_t                          num_executed_transactions;
   uint32_t                        first_data_num_transactions_passed;
   uint32_t                        num_executed_connected_transactions[TEST_MAX_NUM_CONNECTED_TRANSACTIONS];
   USBD_Bool_DT                     test_failed;
}test_params_T;

/*
static USBD_IOTP_BUFF_Params_XT *test_tp;
static USB_EP_Direction_ET test_dir;
static USBD_Bool_DT test_in_progress;
static USBD_Bool_DT test_is_tp_in;
// static uint8_t ep_num;
static uint8_t test_ep_num_bufs;
static uint8_t *test_data;
static USBD_IO_Inout_Data_Size_DT test_size;
static USBD_IO_Inout_Data_Size_DT test_size_result;
static uint8_t test_data_result[TEST_MAX_DATA_SIZE * (1 + TEST_MAX_NUM_CONNECTED_TRANSACTIONS)];
static iotp_test_send_irq_transaction_req_XT *test_transactions;
static size_t test_data_result_pos;
static size_t test_num_transactions;
static size_t test_num_executed_transactions;
static uint32_t test_first_data_num_transactions_passed;
static uint32_t test_num_executed_connected_transactions[TEST_MAX_NUM_CONNECTED_TRANSACTIONS];
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
   Buff_Size_DT result;

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_DEBUG_HI_4(MAIN_APP_TEST, "%s; size = %d; num_executed_transactions = %d; num_transactions = %d", __FUNCTION__, size, test->num_executed_transactions, test->num_transactions);

   if(0 == test->num_executed_transactions)
   {
      test->first_data_num_transactions_passed = port_test_get_num_transactions_passed(ep_num, test->dir);
      USBD_DEBUG_HI_1(MAIN_APP_TEST, "get num executed transactions for basic transaction = %d",
         test->first_data_num_transactions_passed);
   }

   if(USBD_CHECK_PTR(iotp_test_send_irq_transaction_req_XT, test->transactions))
   {
      if(test->num_executed_transactions > 0)
      {
         if(test->num_executed_transactions <= test->num_transactions)
         {
            test->num_executed_connected_transactions[test->num_executed_transactions - 1] = port_test_get_num_transactions_passed(ep_num, test->dir);
            USBD_DEBUG_HI_2(MAIN_APP_TEST, "get num executed transactions for transaction[%d] = %d",
               test->num_executed_transactions - 1, test->num_executed_connected_transactions[test->num_executed_transactions - 1]);
         }
      }
      if(test->num_executed_transactions < test->num_transactions)
      {
         if(IOTP_TEST_RESEND_ON_READY == test->transactions[test->num_executed_transactions].resend_time)
         {
            port_test_set_data(ep_num, test->dir, &test->data_result[TEST_MAX_DATA_SIZE * (1 + test->num_executed_transactions)], test->transactions[test->num_executed_transactions].size, &test->size_result);

            USBD_DEBUG_HI_4(MAIN_APP_TEST, "re-send on buf_ready: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                     test->transactions[test->num_executed_transactions].size,
                     ep_num,
                     (USB_EP_DIRECTION_IN == dir) ? "IN" : "OUT",
                     test->ep_num_bufs);

            result = Buff_Ring_Write(
                  test->tp->core.buff,
                  test->transactions[test->num_executed_transactions].data,
                  test->transactions[test->num_executed_transactions].size,
                  false,
                  true);

            test->data_result_pos += test->transactions[test->num_executed_transactions].size;
            test->num_executed_transactions++;

            if(0 == result)
            {
               USBD_COMPARE_DATA_WARN(MAIN_APP_TEST_ERROR,
                  test->transactions[test->num_executed_transactions].data,
                  test->transactions[test->num_executed_transactions].size,
                  &test->data_result[TEST_MAX_DATA_SIZE * (1 + test->num_executed_transactions)],
                  test->transactions[test->num_executed_transactions].size,
                  "data re-send failed!");

               REPORT_ERROR();
            }
            else if(BUFF_RING_IS_EMPTY(test->tp->core.buff))
            {
               test_ready(tp_params, dir, size);
            }

         }
      }
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void test_buf_empty(USBD_IOTP_BUFF_Params_XT *tp_params, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   uint8_t ep_num = USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP((USBD_IOTP_BUFF_Params_XT*)tp_params);
   uint8_t ep_index = ep_num * ((USB_EP_DIRECTION_IN == dir) ? 1 : 2);
   test_params_T *test = &test_params[ep_index];
   const USB_Endpoint_Desc_DT *ep_desc;
   uint16_t mps;
   Buff_Size_DT result = 0;

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_DEBUG_HI_4(MAIN_APP_TEST, "%s; size = %d; num_executed_transactions = %d; num_transactions = %d", __FUNCTION__, size, test->num_executed_transactions, test->num_transactions);

   ep_desc = USBD_DEV_Get_EP_Desc(USBD_IOTP_BUFF_Get_USBD(tp_params), ep_num, test->dir);

   if(USBD_CHECK_PTR(const USB_Endpoint_Desc_DT, ep_desc))
   {
      mps = 0;
      mps += ep_desc->wMaxPacketSize.H;
      mps *= 256;
      mps += ep_desc->wMaxPacketSize.L;
   }
   else
   {
      USBD_WARN(MAIN_APP_TEST_ERROR, "ep descriptor get failed!");
      REPORT_ERROR();
   }

   if(0 == test->num_executed_transactions)
   {
      test->first_data_num_transactions_passed = port_test_get_num_transactions_passed(ep_num, test->dir);
      USBD_DEBUG_HI_1(MAIN_APP_TEST, "get num executed transactions for basic transaction = %d",
         test->first_data_num_transactions_passed);
   }

   if(USBD_CHECK_PTR(iotp_test_send_irq_transaction_req_XT, test->transactions))
   {
      if(test->num_executed_transactions > 0)
      {
         if(test->num_executed_transactions <= test->num_transactions)
         {
            test->num_executed_connected_transactions[test->num_executed_transactions - 1] = port_test_get_num_transactions_passed(ep_num, test->dir);
            USBD_DEBUG_HI_2(MAIN_APP_TEST, "get num executed transactions for transaction[%d] = %d",
               test->num_executed_transactions - 1, test->num_executed_connected_transactions[test->num_executed_transactions - 1]);
         }
      }
      if(test->num_executed_transactions < test->num_transactions)
      {
         if(IOTP_TEST_RESEND_ON_BUF_EMPTY == test->transactions[test->num_executed_transactions].resend_time)
         {
            port_test_set_data(ep_num, test->dir, &test->data_result[TEST_MAX_DATA_SIZE * (1 + test->num_executed_transactions)], test->transactions[test->num_executed_transactions].size, &test->size_result);

            USBD_DEBUG_HI_4(MAIN_APP_TEST, "re-send on buf_empty: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                     test->transactions[test->num_executed_transactions].size,
                     ep_num,
                     (USB_EP_DIRECTION_IN == dir) ? "IN" : "OUT",
                     test->ep_num_bufs);

            result = Buff_Ring_Write(
                  test->tp->core.buff,
                  test->transactions[test->num_executed_transactions].data,
                  test->transactions[test->num_executed_transactions].size,
                  false,
                  true);

            test->data_result_pos += test->transactions[test->num_executed_transactions].size;
            test->num_executed_transactions++;

            if(0 == result)
            {
               USBD_COMPARE_DATA_WARN(MAIN_APP_TEST_ERROR,
                  test->transactions[test->num_executed_transactions].data,
                  test->transactions[test->num_executed_transactions].size,
                  &test->data_result[TEST_MAX_DATA_SIZE * (1 + test->num_executed_transactions)],
                  test->transactions[test->num_executed_transactions].size,
                  "data re-send failed!");

               REPORT_ERROR();
            }
            else if(BUFF_RING_IS_EMPTY(test->tp->core.buff))
            {
               test_ready(tp_params, dir, size);
            }
         }
      }
   }

   test->in_progress = result;

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

static void print_transactions_context(uint8_t ep_index, uint8_t ep_num)
{
   test_params_T *test = &test_params[ep_index];
   size_t cntr;

   USBD_WARN_3(MAIN_APP_TEST_ERROR, "ep_num: %d, dir: %s, num buffers: %d",
      ep_num,
      (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
      test->ep_num_bufs);
   USBD_WARN(MAIN_APP_TEST_ERROR, "   tr:      size:         tr_type:   num_exec_trans:");
   USBD_WARN_3(MAIN_APP_TEST_ERROR, " basic   %8d     %s           %d",
         test->size,
         "             ",
         test->first_data_num_transactions_passed);
   for(cntr = 0; cntr < test->num_transactions; cntr++)
   {
      USBD_WARN_4(MAIN_APP_TEST_ERROR, "  %4d   %8d     %s           %d",
            cntr,
            test->transactions[cntr].size,
            (IOTP_TEST_RESEND_ON_READY == test->transactions[cntr].resend_time) ? "send_on_ready   " : "send_on_buf_empty",
            test->num_executed_connected_transactions[cntr]);
   }
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
   Buff_Size_DT result;
#ifdef TEST_LIMITED_NUM_LOOPS
   int i = 0;
#endif

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_MARK_INVOKE_DESTINATION(USBD_IOTP_BUFF_GET_INVOKE_PARAMS(test->tp));

   result = Buff_Ring_Write(test->tp->core.buff, test->data, test->size, false, true);

   if(result > 0)
   {
      if(BUFF_RING_IS_EMPTY(test->tp->core.buff)
         && !((USB_EP_DESC_TRANSFER_TYPE_CONTROL == test->tp->core.pipe_params.data.ep_type) && (0 == (test->size % test->tp->core.pipe_params.data.mps))))
      {
         USBD_DEBUG_HI_1(MAIN_APP_TEST, "send with size %d finished immedaitely", test->size);
         test_ready(test->tp, test->dir, 8);
      }

      do
      {
         if(USBD_BOOL_IS_FALSE(test->is_tp_in))
         {
            break;
         }

         port_test_process_in(ep_num);

#ifdef TEST_LIMITED_NUM_LOOPS
         i++;
         if(i > TEST_LIMITED_NUM_LOOPS)
         {
            break;
         }
#endif
      }while(USBD_BOOL_IS_TRUE(test->in_progress/*port_test_is_transfer_active(ep_num, test->dir)*/));
   }

   USBD_UNMARK_INVOKE_DESTINATION(USBD_IOTP_BUFF_GET_INVOKE_PARAMS(test->tp));

   USBD_EXIT_FUNC(MAIN_APP_TEST);
} /* perform_test */

static void check_result(USBD_Params_XT *usbd, uint8_t ep_index, uint8_t ep_num)
{
   test_params_T *test = &test_params[ep_index];
   const USB_Endpoint_Desc_DT *ep_desc;
   size_t i;
   uint32_t num_expected_transactions;
   uint16_t mps;

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   if(USBD_BOOL_IS_TRUE(test->is_tp_in))
   {
      if(0 != port_test_get_num_used_bufs(ep_num, USB_EP_DIRECTION_IN))
      {
         USBD_WARN_1(MAIN_APP_TEST_ERROR, "transfer not finished yet! %d bufs still in use!!!", port_test_get_num_used_bufs(ep_num, USB_EP_DIRECTION_IN));
         REPORT_ERROR();
      }
      else if(test->size_result != test->data_result_pos)
      {
         USBD_WARN_2(MAIN_APP_TEST_ERROR, "data transferred size incorrect! test->size_result = %d; test->size = %d", test->size_result, test->data_result_pos);
         print_transactions_context(ep_num, ep_index);
         REPORT_ERROR();
      }
      else if(0 != memcmp(test->data, test->data_result, test->size))
      {
         USBD_COMPARE_DATA_WARN(MAIN_APP_TEST_ERROR, test->data, test->size, test->data_result, test->size, "data transferred context incorrect!");
         print_transactions_context(ep_num, ep_index);
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

            if(PORT_TEST_TRUE_DMA == port_test_get_dma_type(ep_num, test->dir))
            {
               if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == (ep_desc->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK))
               {
                  if(test->size < mps)
                  {
                     if(1 != test->first_data_num_transactions_passed)
                     {
                        USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                           1, test->first_data_num_transactions_passed);
                        REPORT_ERROR();
                     }
                  }
                  else if((test->size > mps) && (0 == (test->size % mps)))
                  {
                     if(3 != test->first_data_num_transactions_passed)
                     {
                        USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                           3, test->first_data_num_transactions_passed);
                        REPORT_ERROR();
                     }
                  }
                  else
                  {
                     if(2 != test->first_data_num_transactions_passed)
                     {
                        USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                           2, test->first_data_num_transactions_passed);
                        REPORT_ERROR();
                     }
                  }
               }
               else
               {
                  if(test->size <= mps)
                  {
                     if(1 != test->first_data_num_transactions_passed)
                     {
                        USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                           1, test->first_data_num_transactions_passed);
                        REPORT_ERROR();
                     }
                  }
                  else
                  {
                     if(2 != test->first_data_num_transactions_passed)
                     {
                        USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                           2, test->first_data_num_transactions_passed);
                        REPORT_ERROR();
                     }
                  }
               }
            }
            else
            {
               if(0 == test->size)
               {
                  if(1 != test->first_data_num_transactions_passed)
                  {
                     USBD_WARN_1(MAIN_APP_TEST_ERROR, "num basic transactions passed invalid! expected num: 1, current num: %d",
                        test->first_data_num_transactions_passed);
                     print_transactions_context(ep_num, ep_index);
                     REPORT_ERROR();
                  }
               }
               else if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == (ep_desc->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK))
               {
                  num_expected_transactions = (test->size / mps) + 1;

                  if(num_expected_transactions != test->first_data_num_transactions_passed)
                  {
                     USBD_WARN_2(MAIN_APP_TEST_ERROR, "num basic transactions passed invalid! expected num: %d, current num: %d",
                        num_expected_transactions, test->first_data_num_transactions_passed);
                     print_transactions_context(ep_num, ep_index);
                     REPORT_ERROR();
                  }
               }
               else
               {
                  num_expected_transactions = (test->size / mps) + ((0 == (test->size % mps)) ? 0 : 1);

                  if(num_expected_transactions != test->first_data_num_transactions_passed)
                  {
                     USBD_WARN_2(MAIN_APP_TEST_ERROR, "num basic transactions passed invalid! expected num: %d, current num: %d",
                        num_expected_transactions, test->first_data_num_transactions_passed);
                     print_transactions_context(ep_num, ep_index);
                     REPORT_ERROR();
                  }
               }
            }

            if(USBD_CHECK_PTR(iotp_test_send_irq_transaction_req_XT, test->transactions))
            {
               for(i = 0; i < test->num_transactions; i++)
               {
                  if(0 != memcmp(test->transactions[i].data, &test->data_result[TEST_MAX_DATA_SIZE * (1 + i)], test->transactions[i].size))
                  {
                     USBD_COMPARE_DATA_WARN_1(MAIN_APP_TEST_ERROR,
                        test->transactions[i].data,
                        test->transactions[i].size,
                        &test->data_result[TEST_MAX_DATA_SIZE * (1 + i)],
                        test->transactions[i].size,
                        "connected data transferred context incorrect for transaction %d!", i);
                     print_transactions_context(ep_num, ep_index);
                     REPORT_ERROR();
                  }
                  else if(PORT_TEST_TRUE_DMA != port_test_get_dma_type(ep_num, test->dir))
                  {
                     if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == (ep_desc->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK))
                     {
                        num_expected_transactions = (test->transactions[i].size / mps) + 1;

                        if(num_expected_transactions != test->num_executed_connected_transactions[i])
                        {
                           USBD_WARN_3(MAIN_APP_TEST_ERROR, "num connected transactions passed invalid for transaction %d! expected num: %d, current num: %d",
                                i, num_expected_transactions, test->num_executed_connected_transactions[i]);
                           print_transactions_context(ep_num, ep_index);
                           REPORT_ERROR();
                        }
                     }
                     else
                     {
                        num_expected_transactions =
                           (test->transactions[i].size / mps)
                           + ((0 == (test->transactions[i].size % mps)) ? 0 : 1)
                           + ((0 == test->transactions[i].size) ? 1 : 0);

                        if(num_expected_transactions != test->num_executed_connected_transactions[i])
                        {
                           USBD_WARN_3(MAIN_APP_TEST_ERROR, "num connected transactions passed invalid for transaction %d! expected num: %d, current num: %d",
                                i, num_expected_transactions, test->num_executed_connected_transactions[i]);
                           print_transactions_context(ep_num, ep_index);
                           REPORT_ERROR();
                        }
                     }
                  }
                  else
                  {
                     if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == (ep_desc->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK))
                     {
                        if(test->transactions[i].size < mps)
                        {
                           if(1 != test->num_executed_connected_transactions[i])
                           {
                              USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                                 1, test->num_executed_connected_transactions[i]);
                              REPORT_ERROR();
                           }
                        }
                        else if((test->transactions[i].size > mps) && (0 == (test->transactions[i].size % mps)))
                        {
                           if(3 != test->num_executed_connected_transactions[i])
                           {
                              USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                                 3, test->num_executed_connected_transactions[i]);
                              REPORT_ERROR();
                           }
                        }
                        else
                        {
                           if(2 != test->num_executed_connected_transactions[i])
                           {
                              USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                                 2, test->num_executed_connected_transactions[i]);
                              REPORT_ERROR();
                           }
                        }
                     }
                     else
                     {
                        if(test->transactions[i].size <= mps)
                        {
                           if(1 != test->num_executed_connected_transactions[i])
                           {
                              USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                                 1, test->num_executed_connected_transactions[i]);
                              REPORT_ERROR();
                           }
                        }
                        else
                        {
                           if(2 != test->num_executed_connected_transactions[i])
                           {
                              USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                                 2, test->num_executed_connected_transactions[i]);
                              REPORT_ERROR();
                           }
                        }
                     }
#if 0
                     if(0 == test->transactions[i].size)
                     {
                        if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == (ep_desc->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK))
                        {
                           //  if previous transfer size % MPS was equal to 0 then num_expected_transactions = 0, otherwise num_expected_transactions = 1
                           if(0 == i)
                           {
                              if((0 == (test->size % mps)) && (IOTP_TEST_RESEND_ON_READY == test->transactions[i].resend_time))
                              {
                                 num_expected_transactions = 0;
                              }
                              else
                              {
                                 num_expected_transactions = 1;
                              }
                           }
                           else
                           {
                              if((0 == (test->transactions[i-1].size % mps)) && (IOTP_TEST_RESEND_ON_READY == test->transactions[i].resend_time))
                              {
                                 num_expected_transactions = 0;
                              }
                              else
                              {
                                 num_expected_transactions = 1;
                              }
                           }
                        }
                        else
                        {
                           //  if previous transfer size % MPS was equal to 0 then num_expected_transactions = 0, otherwise num_expected_transactions = 1
                           if(0 == i)
                           {
                              if((0 == test->size) && (IOTP_TEST_RESEND_ON_READY == test->transactions[i].resend_time))
                              {
                                 num_expected_transactions = 0;
                              }
                              else
                              {
                                 num_expected_transactions = 1;
                              }
                           }
                           else
                           {
                              if((0 == test->transactions[i-1].size) && (IOTP_TEST_RESEND_ON_READY == test->transactions[i].resend_time))
                              {
                                 num_expected_transactions = 0;
                              }
                              else
                              {
                                 num_expected_transactions = 1;
                              }
                           }
                        }

                        if(num_expected_transactions != test->num_executed_connected_transactions[i])
                        {
                           USBD_WARN_3(MAIN_APP_TEST_ERROR, "num connected transactions passed invalid for transaction %d! expected num: %d, current num: %d",
                                i,
                                num_expected_transactions,
                                test->num_executed_connected_transactions[i]);
                           print_transactions_context(ep_num, ep_index);
                           REPORT_ERROR();
                        }
                     }
                     else if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == (ep_desc->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK))
                     {
                        num_expected_transactions = (test->transactions[i].size / mps) + 1;

                        if(num_expected_transactions != test->num_executed_connected_transactions[i])
                        {
                           USBD_WARN_3(MAIN_APP_TEST_ERROR, "num connected transactions passed invalid for transaction %d! expected num: %d, current num: %d",
                                i, num_expected_transactions, test->num_executed_connected_transactions[i]);
                           print_transactions_context(ep_num, ep_index);
                           REPORT_ERROR();
                        }
                     }
                     else
                     {
                        num_expected_transactions = (test->transactions[i].size / mps) + ((0 == (test->transactions[i].size % mps)) ? 0 : 1);

                        if(num_expected_transactions != test->num_executed_connected_transactions[i])
                        {
                           USBD_WARN_3(MAIN_APP_TEST_ERROR, "num connected transactions passed invalid for transaction %d! expected num: %d, current num: %d",
                                i, num_expected_transactions, test->num_executed_connected_transactions[i]);
                           print_transactions_context(ep_num, ep_index);
                           REPORT_ERROR();
                        }
                     }
#endif
                  }
               }
            }
         }
         else
         {
            USBD_WARN(MAIN_APP_TEST_ERROR, "ep descriptor get failed!");
            print_transactions_context(ep_num, ep_index);
            REPORT_ERROR();
         }
      }
   }
   else
   {
      if(0 != test->size_result)
      {
         USBD_WARN_2(MAIN_APP_TEST_ERROR, "data transferred size incorrect! test->size_result = %d; test->size = %d", test->size_result, test->size);
         print_transactions_context(ep_num, ep_index);
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

            if(0 != test->first_data_num_transactions_passed)
            {
               USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                    ((test->size / mps) + 1), test->first_data_num_transactions_passed);
               print_transactions_context(ep_num, ep_index);
               REPORT_ERROR();
            }
         }
         else
         {
            USBD_WARN(MAIN_APP_TEST_ERROR, "ep descriptor get failed!");
            print_transactions_context(ep_num, ep_index);
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

void iotp_test_send_irq_ready_buf_empty(
   USBD_Params_XT *usbd,
   USBD_IOTP_BUFF_Params_XT *tp,
   uint8_t ep_num,
   USB_EP_Direction_ET dir,
   uint8_t num_bufs,
   uint8_t *data,
   USBD_IO_Inout_Data_Size_DT size,
   iotp_test_send_irq_transaction_req_XT *transactions,
   size_t num_transactions)
{
   test_params_T *test;
   size_t cntr;
   uint8_t ep_index = ep_num * ((USB_EP_DIRECTION_IN == dir) ? 1 : 2);

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   test = &test_params[ep_index];

   if(size > TEST_MAX_DATA_SIZE)
   {
      USBD_WARN_2(MAIN_APP_TEST_INTRO, "test size = %d too big! now is cut to %d!", size, TEST_MAX_DATA_SIZE);
      REPORT_ERROR();
      size = TEST_MAX_DATA_SIZE;
   }
   else if(0 == size)
   {
      USBD_WARN_1(MAIN_APP_TEST_INTRO, "test size = %d has no sense for buff iotp! changed to 1.", size);

      size = 1;
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
   test->ep_num_bufs = 1 + ((num_bufs - 1) & 1);
   for(cntr = 0; cntr < num_transactions; cntr++)
   {
      if(0 == transactions[cntr].size)
      {
         USBD_WARN_2(MAIN_APP_TEST_INTRO, "test transactions[%d].size = %d has no sense for buff iotp! changed to 1.",
            cntr, transactions[cntr].size);

         transactions[cntr].size = 1;
      }
   }
   test->transactions = transactions;
   if(USBD_CHECK_PTR(iotp_test_send_irq_transaction_req_XT, transactions))
   {
      if(num_transactions > TEST_MAX_NUM_CONNECTED_TRANSACTIONS)
      {
         test->num_transactions = TEST_MAX_NUM_CONNECTED_TRANSACTIONS;
      }
      else
      {
         test->num_transactions = num_transactions;
      }
   }
   else
   {
      test->num_transactions = 0;
   }
   test->num_executed_transactions = 0;
   test->data_result_pos = size;
   test->first_data_num_transactions_passed = 0;

   USBD_DEBUG_HI_6(MAIN_APP_TEST_INTRO, "%s: size = %d, ep_num: %d, dir: %s, num buffers: %d, num_transactions: %d",
         __FUNCTION__, test->size, ep_num, (USB_EP_DIRECTION_IN == dir) ? "IN" : "OUT", test->ep_num_bufs, num_transactions);
   for(cntr = 0; cntr < num_transactions; cntr++)
   {
      USBD_WARN_3(MAIN_APP_TEST_INTRO, "test transactions[%d].size = %d, on: %s",
         cntr, transactions[cntr].size, (IOTP_TEST_RESEND_ON_READY == transactions[cntr].resend_time) ? "on_ready" : "on_buf_empty");
   }

   prepare_test(usbd, ep_index, ep_num);
   perform_test(usbd, ep_index, ep_num);
   check_result(usbd, ep_index, ep_num);

   USBD_EXIT_FUNC(MAIN_APP_TEST);

   IOCMD_Clear_All_Logs(true);
} /* test_init */
