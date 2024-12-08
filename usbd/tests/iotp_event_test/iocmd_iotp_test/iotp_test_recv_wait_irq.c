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

#include "iotp_test_recv_wait_irq.h"

#ifndef USBD_H_
#include "usbd.h"
#endif

#ifndef USBD_IO_INTERNAL_H_
#include "usbd_io_internal.h"
#endif
#ifndef USBD_IOTP_EVENT_INTERNAL_H_
#include "usbd_iotp_event_internal.h"
#endif

#include "cfg.h"
#include "port_test_control.h"
#include "main_get_exe.h"

typedef struct
{
    USBD_IOTP_EVENT_Params_XT              *tp;
    USB_EP_Direction_ET                    dir;
    USBD_Bool_DT                            in_progress;
    USBD_Bool_DT                            is_tp_in;
    uint8_t                                 ep_num_bufs;
    uint8_t                                *data;
    USBD_IO_Inout_Data_Size_DT              size;
    USBD_IO_Inout_Data_Size_DT              size_result;
    uint8_t                                 data_result[TEST_MAX_DATA_SIZE];
    USBD_Bool_DT                            read_in_progress;
    USBD_Bool_DT                            test_failed;
}test_params_T;

/*
static USBD_IOTP_EVENT_Params_XT *test_tp;
static USB_EP_Direction_ET test_dir;
static USBD_Bool_DT test_in_progress;
static USBD_Bool_DT test_is_tp_in;
//static uint8_t test_ep_num;
static uint8_t test_ep_num_bufs;
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


static void test_ready(USBD_IOTP_EVENT_Params_XT *tp_params, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
    uint8_t ep_num = USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP((USBD_IOTP_EVENT_Params_XT*)tp_params);
    uint8_t ep_index = ep_num * ((USB_EP_DIRECTION_IN == dir) ? 1 : 2);
    test_params_T *test = &test_params[ep_index];

    USBD_ENTER_FUNC(MAIN_APP_TEST);

    USBD_DEBUG_HI_2(MAIN_APP_TEST, "%s; size = %d", __FUNCTION__, size);

    if(USBD_BOOL_IS_TRUE(test->read_in_progress))
    {
        USBD_WARN_2(MAIN_APP_TEST_ERROR, "%s called from inside \"USBD_IOTP_EVENT_Recv_And_Ready\" with size = %d", __FUNCTION__, size);
        REPORT_ERROR();
    }

    if((size >= 0) && ((-1) == USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(USBD_IOTP_EVENT_Get_USBD(tp_params), ep_num, USBD_FALSE)))
    {
        USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_wait irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
               test->size,
               ep_num,
               (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
               test->ep_num_bufs);
        USBD_WARN_2(MAIN_APP_TEST_ERROR, "endpoint is not waiting on %s callback when size = %d!", __FUNCTION__, size);
        REPORT_ERROR();
    }

    if((size < 0) && ((-1) == USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(USBD_IOTP_EVENT_Get_USBD(tp_params), ep_num, USBD_FALSE)))
    {
        USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_wait irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
               test->size,
               ep_num,
               (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
               test->ep_num_bufs);
        USBD_WARN_2(MAIN_APP_TEST_ERROR, "endpoint is not waiting on %s callback when size = %d!", __FUNCTION__, size);
        REPORT_ERROR();
    }

    if((-1) == size)
    {
        test->in_progress = USBD_IOTP_EVENT_Is_Transfer_Active(tp_params);
    }

    USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void test_buf_empty(USBD_IOTP_EVENT_Params_XT *tp_params, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
    uint8_t ep_num = USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP((USBD_IOTP_EVENT_Params_XT*)tp_params);
    uint8_t ep_index = ep_num * ((USB_EP_DIRECTION_IN == dir) ? 1 : 2);
    test_params_T *test = &test_params[ep_index];

    USBD_ENTER_FUNC(MAIN_APP_TEST);

    USBD_WARN_2(MAIN_APP_TEST_ERROR, "%s called! size = %d", __FUNCTION__, size);
    REPORT_ERROR();

    USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void test_error(USBD_IOTP_EVENT_Params_XT *tp_params, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
    USBD_ENTER_FUNC(MAIN_APP_TEST);

    USBD_WARN_2(MAIN_APP_TEST_ERROR, "%s; size = %d", __FUNCTION__, size);

    USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void test_abort(USBD_IOTP_EVENT_Params_XT *tp_params, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
    USBD_ENTER_FUNC(MAIN_APP_TEST);

    USBD_WARN_2(MAIN_APP_TEST_ERROR, "%s; size = %d", __FUNCTION__, size);

    USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void test_print_data_context(uint8_t *data_1, uint8_t *data_2, uint32_t size)
{
    uint32_t counter;
    char buf[200];

    for(counter = 0; counter < size; counter++)
    {
        if((counter + 8) <= size)
        {
            snprintf(buf, sizeof(buf), "data:: %8d - %8d / %8d: %02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X -> %02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X",
                counter, counter + 7, size,
                data_1[counter],data_1[counter+1],data_1[counter+2],data_1[counter+3],data_1[counter+4],data_1[counter+5],data_1[counter+6],data_1[counter+7],
                data_2[counter],data_2[counter+1],data_2[counter+2],data_2[counter+3],data_2[counter+4],data_2[counter+5],data_2[counter+6],data_2[counter+7]);
            counter += 7;
            USBD_WARN_1(MAIN_APP_TEST_ERROR, "%s", buf);
        }
        else if((counter + 7) <= size)
        {
            snprintf(buf, sizeof(buf), "data:: %8d - %8d / %8d: %02X,%02X,%02X,%02X,%02X,%02X,%02X,   -> %02X,%02X,%02X,%02X,%02X,%02X,%02X",
                counter, counter + 6, size,
                data_1[counter],data_1[counter+1],data_1[counter+2],data_1[counter+3],data_1[counter+4],data_1[counter+5],data_1[counter+6],
                data_2[counter],data_2[counter+1],data_2[counter+2],data_2[counter+3],data_2[counter+4],data_2[counter+5],data_2[counter+6]);
            counter += 6;
            USBD_WARN_1(MAIN_APP_TEST_ERROR, "%s", buf);
        }
        else if((counter + 6) <= size)
        {
            snprintf(buf, sizeof(buf), "data:: %8d - %8d / %8d: %02X,%02X,%02X,%02X,%02X,%02X       -> %02X,%02X,%02X,%02X,%02X,%02X",
                counter, counter + 5, size,
                data_1[counter],data_1[counter+1],data_1[counter+2],data_1[counter+3],data_1[counter+4],data_1[counter+5],
                data_2[counter],data_2[counter+1],data_2[counter+2],data_2[counter+3],data_2[counter+4],data_2[counter+5]);
            counter += 5;
            USBD_WARN_1(MAIN_APP_TEST_ERROR, "%s", buf);
        }
        else if((counter + 5) <= size)
        {
            snprintf(buf, sizeof(buf), "data:: %8d - %8d / %8d: %02X,%02X,%02X,%02X,%02X          -> %02X,%02X,%02X,%02X,%02X",
                counter, counter + 4, size,
                data_1[counter],data_1[counter+1],data_1[counter+2],data_1[counter+3],data_1[counter+4],
                data_2[counter],data_2[counter+1],data_2[counter+2],data_2[counter+3],data_2[counter+4]);
            counter += 4;
            USBD_WARN_1(MAIN_APP_TEST_ERROR, "%s", buf);
        }
        else if((counter + 4) <= size)
        {
            snprintf(buf, sizeof(buf), "data:: %8d - %8d / %8d: %02X,%02X,%02X,%02X             -> %02X,%02X,%02X,%02X",
                counter, counter + 3, size,
                data_1[counter],data_1[counter+1],data_1[counter+2],data_1[counter+3],
                data_2[counter],data_2[counter+1],data_2[counter+2],data_2[counter+3]);
            counter += 3;
            USBD_WARN_1(MAIN_APP_TEST_ERROR, "%s", buf);
        }
        else if((counter + 3) <= size)
        {
            snprintf(buf, sizeof(buf), "data:: %8d - %8d / %8d: %02X,%02X,%02X                -> %02X,%02X,%02X",
                counter, counter + 2, size,
                data_1[counter],data_1[counter+1],data_1[counter+2],
                data_2[counter],data_2[counter+1],data_2[counter+2]);
            counter += 2;
            USBD_WARN_1(MAIN_APP_TEST_ERROR, "%s", buf);
        }
        else if((counter + 2) <= size)
        {
            snprintf(buf, sizeof(buf), "data:: %8d - %8d / %8d: %02X,%02X                   -> %02X,%02X",
                counter, counter + 1, size,
                data_1[counter],data_1[counter+1],
                data_2[counter],data_2[counter+1]);
            counter += 1;
            USBD_WARN_1(MAIN_APP_TEST_ERROR, "%s", buf);
        }
        else if((counter + 1) <= size)
        {
            snprintf(buf, sizeof(buf), "data:: %8d - %8d / %8d: %02X                      -> %02X",
                counter, counter, size,
                data_1[counter],
                data_2[counter]);
            USBD_WARN_1(MAIN_APP_TEST_ERROR, "%s", buf);
        }
    }
}

static void prepare_test(USBD_Params_XT *usbd, uint8_t ep_index, uint8_t ep_num)
{
    test_params_T *test = &test_params[ep_index];

    USBD_ENTER_FUNC(MAIN_APP_TEST);

    USBD_IOTP_EVENT_Set_Handlers(
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
    test_params_T *test = &test_params[ep_index];
    USBD_IO_Inout_Data_Size_DT size_res;
    USBD_Bool_DT result;
#ifdef TEST_LIMITED_NUM_LOOPS
   int i = 0;
#endif

    USBD_ENTER_FUNC(MAIN_APP_TEST);

    USBD_MARK_INVOKE_DESTINATION(USBD_IOTP_EVENT_GET_INVOKE_PARAMS(test->tp));

    test->read_in_progress = USBD_TRUE;

    result = USBD_IOTP_EVENT_Recv_And_Wait(test->tp, test->data_result, test->size, &size_res);

    test->read_in_progress = USBD_FALSE;

    if(USBD_BOOL_IS_TRUE(result))
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
            if(USBD_BOOL_IS_TRUE(test->is_tp_in))
            {
                break;
            }
        }while(USBD_BOOL_IS_TRUE(result/*port_test_is_transfer_active(ep_num, test->dir)*/));
    }
    else if(USB_EP_DIRECTION_OUT == test->dir)
    {
        USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_wait irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
               test->size,
               ep_num,
               (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
               test->ep_num_bufs);
        USBD_WARN(MAIN_APP_TEST_ERROR, "USBD_IOTP_EVENT_recv_and_ready failed!");
        REPORT_ERROR();
    }

    USBD_UNMARK_INVOKE_DESTINATION(USBD_IOTP_EVENT_GET_INVOKE_PARAMS(test->tp));

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
        if(USBD_BOOL_IS_TRUE(test->in_progress))
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
            USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_wait irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                  test->size,
                  ep_num,
                  (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                  test->ep_num_bufs);
            USBD_WARN_2(MAIN_APP_TEST_ERROR, "data transferred size incorrect! test->size_result = %d; test->size = %d", test->size_result, test->size);
            REPORT_ERROR();
        }
        else if(USBD_IOTP_EVENT_Get_Transferred_Size(test->tp) != test->size)
        {
            USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_wait irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                  test->size,
                  ep_num,
                  (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                  test->ep_num_bufs);
            USBD_WARN_2(MAIN_APP_TEST_ERROR, "data transferred size incorrect! transferred_size = %d; test->size = %d",
                  USBD_IOTP_EVENT_Get_Transferred_Size(test->tp), test->size);
            REPORT_ERROR();
        }
        else if(0 != memcmp(test->data, test->data_result, test->size))
        {
            USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_wait irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                  test->size,
                  ep_num,
                  (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                  test->ep_num_bufs);
            USBD_WARN(MAIN_APP_TEST_ERROR, "data transferred context incorrect!");
            test_print_data_context(test->data, test->data_result, test->size);
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
                        USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_wait irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
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
                    num_expected_transactions_passed = test->size / mps + 1;
                    if(num_expected_transactions_passed != port_test_get_num_transactions_passed(ep_num, test->dir))
                    {
                        USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_wait irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
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
                        USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_wait irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
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
                USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_wait irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
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
            USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_wait irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
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
                    USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_wait irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
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
                USBD_WARN_4(MAIN_APP_TEST_ERROR, "recv_wait irq: size = %d, ep_num: %d, dir: %s, num buffers: %d",
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

void iotp_test_recv_wait_irq(USBD_Params_XT *usbd, USBD_IOTP_EVENT_Params_XT *tp, uint8_t ep_num, USB_EP_Direction_ET dir, uint8_t num_bufs, uint8_t *data, USBD_IO_Inout_Data_Size_DT size)
{
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

    test->tp     = tp;
    test->dir    = dir;
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

    USBD_DEBUG_HI_5(MAIN_APP_TEST_INTRO, "%s: size = %d, ep_num: %d, dir: %s, num buffers: %d",
        __FUNCTION__, test->size, ep_num, (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT", test->ep_num_bufs);

    prepare_test(usbd, ep_index, ep_num);
    perform_test(usbd, ep_index, ep_num);
    check_result(usbd, ep_index, ep_num);

    if((-1) != USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(USBD_IOTP_EVENT_Get_USBD(test->tp), ep_num, USBD_FALSE))
    {
        USBD_MARK_INVOKE_DESTINATION(USBD_IOTP_EVENT_GET_INVOKE_PARAMS(test->tp));

        USBD_IOTP_EVENT_Recv_Ready(test->tp);

        USBD_UNMARK_INVOKE_DESTINATION(USBD_IOTP_EVENT_GET_INVOKE_PARAMS(test->tp));
    }

    test->read_in_progress = USBD_FALSE;

    USBD_EXIT_FUNC(MAIN_APP_TEST);

    IOCMD_Clear_All_Logs(true);
} /* test_init */
