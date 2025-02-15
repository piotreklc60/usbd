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

#include "iotp_test_send_irq.h"

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
   USBD_IOTP_BUFF_Params_XT     *tp;
   USB_EP_Direction_ET           dir;
   USBD_Bool_DT                  is_tp_in;
   uint8_t                       ep_num_bufs;
   uint8_t                      *data;
   USBD_IO_Inout_Data_Size_DT    size;
   USBD_IO_Inout_Data_Size_DT    size_result;
   uint8_t                       data_result[TEST_MAX_DATA_SIZE];
   USBD_Bool_DT                  test_failed;
}test_params_T;

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






static void prepare_test(USBD_Params_XT *usbd, uint8_t ep_index, uint8_t ep_num)
{
   test_params_T *test = &test_params[ep_index];

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   port_test_set_data(ep_num, test->dir, test->data_result, test->size, &test->size_result);
   port_test_set_num_buffers(ep_num, test->dir, test->ep_num_bufs);
   test->size_result = 0;

   USBD_EXIT_FUNC(MAIN_APP_TEST);
} /* prepare_test */

static void perform_test(USBD_Params_XT *usbd, uint8_t ep_index, uint8_t ep_num)
{
   test_params_T *test = &test_params[ep_index];
   USBD_IO_Inout_Data_Size_DT size_res;
   Buff_Size_DT result;
#ifdef TEST_LIMITED_NUM_LOOPS
   int i = 0;
#endif

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_MARK_INVOKE_DESTINATION(USBD_IOTP_BUFF_GET_INVOKE_PARAMS(test->tp));

   USBD_NOTICE_3(MAIN_APP_TEST, "Buff_Ring_Write with size: %d to buf with size: %d, free_size: %d", test->size, Buff_Ring_Get_Size(test->tp->core.buff, true), Buff_Ring_Get_Free_Size(test->tp->core.buff, true));

   result = Buff_Ring_Write(test->tp->core.buff, test->data, test->size, false, true);

   USBD_NOTICE_3(MAIN_APP_TEST, "Buff_Ring_Write result: %d to buf with size: %d, free_size: %d", result, Buff_Ring_Get_Size(test->tp->core.buff, true), Buff_Ring_Get_Free_Size(test->tp->core.buff, true));

   if(result > 0)
   {
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
      }while(BUFF_BOOL_IS_FALSE(Buff_Ring_Is_Empty(test->tp->core.buff, BUFF_FALSE)) || (0 != port_test_get_num_used_bufs(ep_num, USB_EP_DIRECTION_IN)));
   }

   USBD_UNMARK_INVOKE_DESTINATION(USBD_IOTP_BUFF_GET_INVOKE_PARAMS(test->tp));

   USBD_EXIT_FUNC(MAIN_APP_TEST);
} /* perform_test */

static void check_result(USBD_Params_XT *usbd, uint8_t ep_index, uint8_t ep_num)
{
   test_params_T *test = &test_params[ep_index];
   const USB_Endpoint_Desc_DT *ep_desc;
   uint16_t mps;

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   if(USBD_BOOL_IS_TRUE(test->is_tp_in))
   {
      if(0 != port_test_get_num_used_bufs(ep_num, USB_EP_DIRECTION_IN))
      {
         USBD_WARN_1(MAIN_APP_TEST_ERROR, "transfer not finished yet! %d bufs still in use!!!", port_test_get_num_used_bufs(ep_num, USB_EP_DIRECTION_IN));
         REPORT_ERROR();
      }
      else if((-1) != USBD_IO_UP_EP_IN_Get_Buffered_Data_Size(usbd, ep_num))
      {
         USBD_WARN_1(MAIN_APP_TEST_ERROR, "data buffered size = %d incorrect!", USBD_IO_UP_EP_IN_Get_Buffered_Data_Size(usbd, ep_num));
         REPORT_ERROR();
      }
      else if(test->size_result != test->size)
      {
         USBD_WARN_2(MAIN_APP_TEST_ERROR, "data transferred size incorrect! test->size_result = %d; test->size = %d", test->size_result, test->size);
         REPORT_ERROR();
      }
      else if(0 != memcmp(test->data, test->data_result, test->size))
      {
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

            if(PORT_TEST_TRUE_DMA == port_test_get_dma_type(ep_num, test->dir))
            {
               if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == (ep_desc->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK))
               {
                  if(test->size < mps)
                  {
                     if(1 != port_test_get_num_transactions_passed(ep_num, test->dir))
                     {
                        USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                           1, port_test_get_num_transactions_passed(ep_num, test->dir));
                        REPORT_ERROR();
                     }
                  }
                  else if((test->size > mps) && (0 == (test->size % mps)))
                  {
                     if(3 != port_test_get_num_transactions_passed(ep_num, test->dir))
                     {
                        USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                           3, port_test_get_num_transactions_passed(ep_num, test->dir));
                        REPORT_ERROR();
                     }
                  }
                  else
                  {
                     if(2 != port_test_get_num_transactions_passed(ep_num, test->dir))
                     {
                        USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                           2, port_test_get_num_transactions_passed(ep_num, test->dir));
                        REPORT_ERROR();
                     }
                  }
               }
               else
               {
                  if(test->size <= mps)
                  {
                     if(1 != port_test_get_num_transactions_passed(ep_num, test->dir))
                     {
                        USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                           1, port_test_get_num_transactions_passed(ep_num, test->dir));
                        REPORT_ERROR();
                     }
                  }
                  else
                  {
                     if(2 != port_test_get_num_transactions_passed(ep_num, test->dir))
                     {
                        USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                           2, port_test_get_num_transactions_passed(ep_num, test->dir));
                        REPORT_ERROR();
                     }
                  }
               }
            }
            else
            {
               if(0 == test->size)
               {
                  if(1 != port_test_get_num_transactions_passed(ep_num, test->dir))
                  {
                     USBD_WARN_1(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: 1, current num: %d",
                          port_test_get_num_transactions_passed(ep_num, test->dir));
                     REPORT_ERROR();
                  }
               }
               else
               {
                  if(((test->size / mps) + ((0 == (test->size % mps)) ? 0 : 1)) != port_test_get_num_transactions_passed(ep_num, test->dir))
                  {
                     USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                          ((test->size / mps) + ((0 == (test->size % mps)) ? 0 : 1)), port_test_get_num_transactions_passed(ep_num, test->dir));
                     REPORT_ERROR();
                  }
               }
            }
         }
         else
         {
            USBD_WARN(MAIN_APP_TEST_ERROR, "ep descriptor get failed!");
            REPORT_ERROR();
         }
      }
   }
   else
   {
      if(0 != test->size_result)
      {
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
               USBD_WARN_2(MAIN_APP_TEST_ERROR, "num transactions passed invalid! expected num: %d, current num: %d",
                    ((test->size / mps) + 1), port_test_get_num_transactions_passed(ep_num, test->dir));
               REPORT_ERROR();
            }
         }
         else
         {
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

void iotp_test_send_irq(USBD_Params_XT *usbd, USBD_IOTP_BUFF_Params_XT *tp, uint8_t ep_num, USB_EP_Direction_ET dir, uint8_t num_bufs, uint8_t *data, USBD_IO_Inout_Data_Size_DT size)
{
   uint8_t ep_index = ep_num * ((USB_EP_DIRECTION_IN == dir) ? 1 : 2);
   test_params_T *test = &test_params[ep_index];

   USBD_ENTER_FUNC(MAIN_APP_TEST);

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
   if(0 == size)
   {
      USBD_WARN_1(MAIN_APP_TEST_INTRO, "test size = %d has no sense for buff iotp! changed to 1.", size);

      size = 1;
   }
   test->size   = size;
   test->ep_num_bufs = 1 + ((num_bufs - 1) & 1);

   USBD_DEBUG_HI_5(MAIN_APP_TEST_INTRO, "%s: size = %d, ep_num: %d, dir: %s, num buffers: %d",
      __FUNCTION__, test->size, ep_num, (USB_EP_DIRECTION_IN == dir) ? "IN" : "OUT", test->ep_num_bufs);

   prepare_test(usbd, ep_index, ep_num);
   perform_test(usbd, ep_index, ep_num);
   check_result(usbd, ep_index, ep_num);

   if(USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(usbd, ep_num, USBD_TRUE) >= 0)
   {
      USBD_WARN_1(MAIN_APP_TEST_INTRO, "test size = %d; EP IS STILL WAITING!", size);
      REPORT_ERROR();
   }

   if(port_test_get_out_ep_buf_size(ep_num) > 0)
   {
      USBD_WARN_1(MAIN_APP_TEST_INTRO, "test size = %d; EP IS STILL WAITING!", size);
      REPORT_ERROR();
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);

   IOCMD_Clear_All_Logs(true);
} /* test_init */
