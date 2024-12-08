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

#include "iotp_test_recv_irq_next.h"

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
    uint8_t                                 data_result[TEST_MAX_DATA_SIZE * TEST_MAX_NUM_CONNECTED_TRANSACTIONS];
    iotp_test_recv_irq_transaction_req_XT  *transactions;
    iotp_test_recv_irq_transaction_req_XT   transactions_result[TEST_MAX_NUM_CONNECTED_TRANSACTIONS];
    size_t                                  data_result_pos;
    size_t                                  num_transactions;
    size_t                                  num_executed_transactions;
    uint32_t                                num_executed_connected_transactions[TEST_MAX_NUM_CONNECTED_TRANSACTIONS];
    USBD_Bool_DT                            recv_to_whole_buffer;
    USBD_Bool_DT                            test_failed;
}test_params_T;

/*
static USBD_IOTP_EVENT_Params_XT *test_tp[2 * (USBD_MAX_NUM_ENDPOINTS + 1)];
static USB_EP_Direction_ET test_dir[2 * (USBD_MAX_NUM_ENDPOINTS + 1)];
static USBD_Bool_DT test_in_progress[2 * (USBD_MAX_NUM_ENDPOINTS + 1)];
static USBD_Bool_DT is_tp_in[2 * (USBD_MAX_NUM_ENDPOINTS + 1)];
// static uint8_t test_ep_num;
static uint8_t test_ep_num_bufs[2 * (USBD_MAX_NUM_ENDPOINTS + 1)];
static uint8_t test_data_result[2 * (USBD_MAX_NUM_ENDPOINTS + 1)][TEST_MAX_DATA_SIZE * TEST_MAX_NUM_CONNECTED_TRANSACTIONS];
static iotp_test_recv_irq_transaction_req_XT *test_transactions[2 * (USBD_MAX_NUM_ENDPOINTS + 1)];
static iotp_test_recv_irq_transaction_req_XT test_transactions_result[2 * (USBD_MAX_NUM_ENDPOINTS + 1)][TEST_MAX_NUM_CONNECTED_TRANSACTIONS];
static size_t test_data_result_pos[2 * (USBD_MAX_NUM_ENDPOINTS + 1)];
static size_t test_num_transactions[2 * (USBD_MAX_NUM_ENDPOINTS + 1)];
static size_t test_num_executed_transactions[2 * (USBD_MAX_NUM_ENDPOINTS + 1)];
static uint32_t test_num_executed_connected_transactions[2 * (USBD_MAX_NUM_ENDPOINTS + 1)][TEST_MAX_NUM_CONNECTED_TRANSACTIONS];
*/

static test_params_T test_params[2 * (USBD_MAX_NUM_ENDPOINTS + 1)];

static void initialize_transaction(USBD_Params_XT *usbd, uint8_t ep_index, uint8_t ep_num);
static void print_transactions_context(uint8_t ep_index, uint8_t ep_num);
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
    USBD_Bool_DT result;
    const USB_Endpoint_Desc_DT *ep_desc;
    USBD_Bool_DT is_control = USBD_FALSE;

    USBD_ENTER_FUNC(MAIN_APP_TEST);

    test->in_progress = USBD_FALSE;

    USBD_DEBUG_HI_4(MAIN_APP_TEST, "%s; size = %d, transaction: %d/%d", __FUNCTION__, size, test->num_executed_transactions, test->num_transactions);

    ep_desc = USBD_DEV_Get_EP_Desc(USBD_IOTP_EVENT_Get_USBD(tp_params), ep_num, test->dir);

    if(USBD_CHECK_PTR(const USB_Endpoint_Desc_DT, ep_desc))
    {
        if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == (ep_desc->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK))
        {
            is_control = USBD_TRUE;
        }
    }
    else
    {
        is_control = USBD_TRUE;
    }

    if(test->num_executed_transactions < test->num_transactions)
    {
        if(IOTP_TEST_RECV_AND_READY == test->transactions_result[test->num_executed_transactions].recv_method)
        {
            if((size < 0) && ((-1) != USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(USBD_IOTP_EVENT_Get_USBD(tp_params), ep_num, USBD_FALSE)))
            {
                print_transactions_context(ep_index, ep_num);
                USBD_WARN_5(MAIN_APP_TEST_ERROR, "recv_next irq for transaction: %d: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                        test->num_executed_transactions,
                        test->transactions[test->num_executed_transactions].size,
                        ep_num,
                        (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                        test->ep_num_bufs);
                USBD_WARN_2(MAIN_APP_TEST_ERROR, "endpoint is waiting on %s callback when size = %d!", __FUNCTION__, size);
                REPORT_ERROR();
            }
        }
        else
        {
            if((size < 0) && ((-1) == USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(USBD_IOTP_EVENT_Get_USBD(tp_params), ep_num, USBD_FALSE)))
            {
                print_transactions_context(ep_index, ep_num);
                USBD_WARN_5(MAIN_APP_TEST_ERROR, "recv_next irq for transaction: %d: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                        test->num_executed_transactions,
                        test->transactions[test->num_executed_transactions].size,
                        ep_num,
                        (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                        test->ep_num_bufs);
                USBD_WARN_2(MAIN_APP_TEST_ERROR, "endpoint is not waiting on %s callback when size = %d!", __FUNCTION__, size);
                REPORT_ERROR();
            }
        }

        if((size >= 0) && ((-1) == USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(USBD_IOTP_EVENT_Get_USBD(tp_params), ep_num, USBD_FALSE)))
        {
            print_transactions_context(ep_index, ep_num);
            USBD_WARN_5(MAIN_APP_TEST_ERROR, "recv_next irq for transaction: %d: size = %d, ep_num: %d, dir: %s, num buffers: %d",
                    test->num_executed_transactions,
                    test->transactions[test->num_executed_transactions].size,
                    ep_num,
                    (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                    test->ep_num_bufs);
            USBD_WARN_2(MAIN_APP_TEST_ERROR, "endpoint is not waiting on %s callback when size = %d!", __FUNCTION__, size);
            REPORT_ERROR();
        }

        test->num_executed_connected_transactions[test->num_executed_transactions] = port_test_get_num_transactions_passed(ep_num, test->dir);

        if(USBD_IOTP_EVENT_Get_Transferred_Size(test->tp) != test->transactions[test->num_executed_transactions].size)
        {
            USBD_WARN_5(MAIN_APP_TEST_ERROR, "recv_next irq for transaction: %d, size = %d, ep_num: %d, dir: %s, num buffers: %d",
                  test->num_executed_transactions,
                  test->transactions[test->num_executed_transactions].size,
                  ep_num,
                  (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
                  test->ep_num_bufs);
            USBD_WARN_2(MAIN_APP_TEST_ERROR, "data transferred size incorrect! transferred_size = %d; test_size = %d",
                USBD_IOTP_EVENT_Get_Transferred_Size(test->tp),
                test->transactions[test->num_executed_transactions].size);
            REPORT_ERROR();
        }

        /*
         * if previous transaction was IOTP_TEST_RECV_AND_WAIT then this transaction finished previous one at the begining
         * so test->num_executed_connected_transactions must be decreased for correct comparision
         */
  /*      if((test->num_executed_transactions > 0)
            && (IOTP_TEST_RECV_AND_WAIT == test->transactions[test->num_executed_transactions - 1].recv_method))
        {
            test->num_executed_connected_transactions[test->num_executed_transactions] -= 1;
            USBD_DEBUG_HI_2(MAIN_APP_TEST, "num transactions = %d decreased by one in line: %d!",
                    test->num_executed_connected_transactions[test->num_executed_transactions],
                    __LINE__);
        }//*/

        test->num_executed_transactions++;

        if(test->num_executed_transactions < test->num_transactions)
        {
            initialize_transaction(USBD_IOTP_EVENT_Get_USBD(tp_params), ep_index, ep_num);
        }
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

static void initialize_transaction(USBD_Params_XT *usbd, uint8_t ep_index, uint8_t ep_num)
{
    test_params_T *test = &test_params[ep_index];
    USBD_IO_Inout_Data_Size_DT size_res = 0;
    const USB_Endpoint_Desc_DT *ep_desc;
    USBD_Bool_DT is_control = USBD_FALSE;
    USBD_IO_Inout_Data_Size_DT size;

    USBD_ENTER_FUNC(MAIN_APP_TEST);

    ep_desc = USBD_DEV_Get_EP_Desc(usbd, ep_num, test->dir);

    if(USBD_CHECK_PTR(const USB_Endpoint_Desc_DT, ep_desc))
    {
        if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == (ep_desc->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK))
        {
            is_control = USBD_TRUE;
        }
    }
    else
    {
        is_control = USBD_TRUE;
    }

    if(test->num_executed_transactions < test->num_transactions)
    {
        port_test_set_data(
                ep_num,
                test->dir,
                test->transactions[test->num_executed_transactions].data,
                test->transactions[test->num_executed_transactions].size,
                &(test->transactions_result[test->num_executed_transactions].size));

        if(USBD_BOOL_IS_FALSE(test->transactions[test->num_executed_transactions].recv_to_whole_buffer))
        {
            size = test->transactions[test->num_executed_transactions].size;
        }
        else
        {
            size = TEST_MAX_DATA_SIZE;
        }

        if(IOTP_TEST_RECV_AND_READY == test->transactions[test->num_executed_transactions].recv_method)
        {
            test->in_progress = USBD_IOTP_EVENT_Recv_And_Ready(
                    test->tp,
                    test->transactions_result[test->num_executed_transactions].data,
                    size,
                    &size_res);
        }
        else
        {
            test->in_progress = USBD_IOTP_EVENT_Recv_And_Wait(
                    test->tp,
                    test->transactions_result[test->num_executed_transactions].data,
                    size,
                    &size_res);
        }

        USBD_DEBUG_HI_1(MAIN_APP_TEST, "test->in_progress: %s after recv", USBD_BOOL_IS_TRUE(test->in_progress) ? "true" : "false");

        if(USBD_BOOL_IS_FALSE(test->in_progress) && USBD_BOOL_IS_FALSE(USBD_IOTP_EVENT_Is_Transfer_Active(test->tp)) && (USB_EP_DIRECTION_OUT == test->dir))
        {
            print_transactions_context(ep_index, ep_num);
            USBD_WARN_1(MAIN_APP_TEST_ERROR, "USBD_IOTP_EVENT_Recv_And_Ready/wait failed for transaction: %d!", test->num_executed_transactions);
            REPORT_ERROR();
        }

        if((0 == size_res) && (USB_EP_DIRECTION_OUT == test->dir) && USBD_BOOL_IS_FALSE(USBD_IOTP_EVENT_Is_Transfer_Active(test->tp))
          && ((0 != test->transactions[test->num_executed_transactions].size) || USBD_BOOL_IS_FALSE(is_control)))
        {
#ifdef USE_TEST_PRINTF
            print_transactions_context(ep_index, ep_num);
#endif
            USBD_WARN_1(MAIN_APP_TEST, "test_ready called immediately for transaction: %d!", test->num_executed_transactions);
            test_ready(test->tp, test->dir, 0);
        }
    }

    USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void prepare_test(USBD_Params_XT *usbd, uint8_t ep_index, uint8_t ep_num)
{
    test_params_T *test = &test_params[ep_index];
    uint8_t cntr;

    USBD_ENTER_FUNC(MAIN_APP_TEST);

    for(cntr = 0; cntr < test->num_transactions; cntr++)
    {
        test->transactions_result[cntr].data                    = &test->data_result[TEST_MAX_DATA_SIZE * cntr];
        test->transactions_result[cntr].size                    = 0;
        test->transactions_result[cntr].recv_method             = test->transactions[cntr].recv_method;
        test->transactions_result[cntr].recv_to_whole_buffer    = test->transactions[cntr].recv_to_whole_buffer;
        test->num_executed_connected_transactions[cntr]         = 0;
    }
    test->num_executed_transactions = 0;

    USBD_IOTP_EVENT_Set_Handlers(
        test->tp,
        test_ready,
        test_buf_empty,
        test_error,
        test_abort);

    port_test_set_num_buffers(ep_num, test->dir, test->ep_num_bufs);
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

    initialize_transaction(usbd, ep_index, ep_num);

    if(USBD_BOOL_IS_TRUE(test->in_progress))
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
        print_transactions_context(ep_index, ep_num);
        USBD_WARN_1(MAIN_APP_TEST_ERROR, "USBD_IOTP_EVENT_Recv_And_Ready failed for transaction: %d!", test->num_executed_transactions);
        REPORT_ERROR();
    }

    USBD_UNMARK_INVOKE_DESTINATION(USBD_IOTP_EVENT_GET_INVOKE_PARAMS(test->tp));

    USBD_EXIT_FUNC(MAIN_APP_TEST);
} /* perform_test */

static void print_transactions_context(uint8_t ep_index, uint8_t ep_num)
{
    test_params_T *test = &test_params[ep_index];
    size_t cntr;

    USBD_WARN_3(MAIN_APP_TEST_ERROR, "ep_num: %d, dir: %s, num buffers: %d",
        ep_num,
        (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT",
        test->ep_num_bufs);
    USBD_WARN(MAIN_APP_TEST_ERROR, "                   size:");
    USBD_WARN(MAIN_APP_TEST_ERROR, "    tr:        req:   result:           tr_type:   num_exec_trans:");
    for(cntr = 0; cntr < test->num_transactions; cntr++)
    {

        USBD_WARN_5(MAIN_APP_TEST_ERROR, "  %4d    %8d  %8d      %s                 %d",
               cntr,
               test->transactions[cntr].size,
               test->transactions_result[cntr].size,
               (IOTP_TEST_RECV_AND_READY == test->transactions[cntr].recv_method) ? "recv_and_ready" : "recv_and_wait ",
               test->num_executed_connected_transactions[cntr]);
    }
}

static void check_result(USBD_Params_XT *usbd, uint8_t ep_index, uint8_t ep_num)
{
    test_params_T *test = &test_params[ep_index];
    const USB_Endpoint_Desc_DT *ep_desc;
    size_t cntr;
    uint32_t num_expected_transactions_passed;
    uint16_t mps;

    USBD_ENTER_FUNC(MAIN_APP_TEST);

    for(cntr = 0; cntr < test->num_transactions; cntr++)
    {
        if(USBD_BOOL_IS_FALSE(test->is_tp_in))
        {
            if(USBD_BOOL_IS_TRUE(test->in_progress))
            {
                print_transactions_context(ep_index, ep_num);
                USBD_WARN_1(MAIN_APP_TEST_ERROR, "transfer still active! transaction: %d", cntr);
                REPORT_ERROR();
            }
            else if(test->transactions_result[cntr].size != test->transactions[cntr].size)
            {
                print_transactions_context(ep_index, ep_num);
                USBD_WARN_3(MAIN_APP_TEST_ERROR, "data transferred size incorrect for transaction: %d! test_size_result = %d; test_size = %d",
                        cntr,
                        test->transactions_result[cntr].size, test->transactions[cntr].size);
                REPORT_ERROR();
            }
            else if(0 != memcmp(test->transactions[cntr].data, test->transactions_result[cntr].data, test->transactions[cntr].size))
            {
                print_transactions_context(ep_index, ep_num);
                USBD_WARN_1(MAIN_APP_TEST_ERROR, "data transferred context incorrect for transaction: %d!", cntr);
                test_print_data_context(test->transactions[cntr].data, test->transactions_result[cntr].data, test->transactions[cntr].size);
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

                    if(USBD_BOOL_IS_TRUE(port_test_is_dma_active(ep_num, test->dir)))
                    {
                        /* not analyze it */
                    }
                    else
                    {
                        if(IOTP_TEST_RECV_AND_WAIT == test->transactions[cntr].recv_method)
                        {
                            if(0 == test->transactions[cntr].size)
                            {
                                num_expected_transactions_passed = 1;
                                if(num_expected_transactions_passed != test->num_executed_connected_transactions[cntr])
                                {
                                    print_transactions_context(ep_index, ep_num);
                                    USBD_WARN_3(MAIN_APP_TEST_ERROR, "num transactions passed invalid for transaction: %d! expected num: %d, current num: %d",
                                            cntr,
                                            num_expected_transactions_passed, test->num_executed_connected_transactions[cntr]);
                                    REPORT_ERROR();
                                }
                            }
                            else if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == (ep_desc->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK))
                            {
                                num_expected_transactions_passed = test->transactions[cntr].size / mps + 1;
                                if(num_expected_transactions_passed != test->num_executed_connected_transactions[cntr])
                                {
                                    print_transactions_context(ep_index, ep_num);
                                    USBD_WARN_3(MAIN_APP_TEST_ERROR, "num transactions passed invalid for transaction: %d! expected num: %d, current num: %d",
                                            cntr,
                                            num_expected_transactions_passed, test->num_executed_connected_transactions[cntr]);
                                    REPORT_ERROR();
                                }
                            }
                            else
                            {
                                num_expected_transactions_passed = (test->transactions[cntr].size / mps) + ((0 == (test->transactions[cntr].size % mps)) ? 0 : 1);
                                if(num_expected_transactions_passed != test->num_executed_connected_transactions[cntr])
                                {
                                    print_transactions_context(ep_index, ep_num);
                                    USBD_WARN_3(MAIN_APP_TEST_ERROR, "num transactions passed invalid for transaction: %d! expected num: %d, current num: %d",
                                            cntr,
                                            num_expected_transactions_passed, test->num_executed_connected_transactions[cntr]);
                                    REPORT_ERROR();
                                }
                            }
                        }
                        else
                        {
                            if(0 == test->transactions[cntr].size)
                            {
                                if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == (ep_desc->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK))
                                {
                                    num_expected_transactions_passed = 1;
                                }
                                else
                                {
                                    num_expected_transactions_passed = 1;
                                }
                                if(num_expected_transactions_passed != test->num_executed_connected_transactions[cntr])
                                {
                                    print_transactions_context(ep_index, ep_num);
                                    USBD_WARN_3(MAIN_APP_TEST_ERROR, "num transactions passed invalid for transaction: %d! expected num: %d, current num: %d",
                                            cntr,
                                            num_expected_transactions_passed, test->num_executed_connected_transactions[cntr]);
                                    REPORT_ERROR();
                                }
                            }
                            else if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == (ep_desc->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK))
                            {
                                num_expected_transactions_passed = (test->transactions[cntr].size / mps) + 1;
                                if(num_expected_transactions_passed != test->num_executed_connected_transactions[cntr])
                                {
                                    print_transactions_context(ep_index, ep_num);
                                    USBD_WARN_3(MAIN_APP_TEST_ERROR, "num transactions passed invalid for transaction: %d! expected num: %d, current num: %d",
                                            cntr,
                                            num_expected_transactions_passed, test->num_executed_connected_transactions[cntr]);
                                    REPORT_ERROR();
                                }
                            }
                            else
                            {
                                num_expected_transactions_passed = (test->transactions[cntr].size / mps) + ((0 == (test->transactions[cntr].size % mps)) ? 0 : 1);
                                if(num_expected_transactions_passed != test->num_executed_connected_transactions[cntr])
                                {
                                    print_transactions_context(ep_index, ep_num);
                                    USBD_WARN_3(MAIN_APP_TEST_ERROR, "num transactions passed invalid for transaction: %d! expected num: %d, current num: %d",
                                            cntr,
                                            num_expected_transactions_passed, test->num_executed_connected_transactions[cntr]);
                                    REPORT_ERROR();
                                }
                            }
                        }
                    }
                }
                else
                {
                    print_transactions_context(ep_index, ep_num);
                    USBD_WARN(MAIN_APP_TEST_ERROR, "ep descriptor get failed!");
                    REPORT_ERROR();
                }
            }
        }
        else
        {
            if(0 != test->transactions_result[cntr].size)
            {
                print_transactions_context(ep_index, ep_num);
                USBD_WARN_3(MAIN_APP_TEST_ERROR, "data transferred size incorrect for transaction: %d! test_size_result = %d; test_size = %d",
                        cntr,
                        test->transactions_result[cntr].size,
                        test->transactions[cntr].size);
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

                    if(0 != test->num_executed_connected_transactions[cntr])
                    {
                        print_transactions_context(ep_index, ep_num);
                        USBD_WARN_3(MAIN_APP_TEST_ERROR, "num transactions passed invalid for transaction: %d! expected num: %d, current num: %d",
                                cntr,
                                ((test->transactions[cntr].size / mps) + 1),
                                test->num_executed_connected_transactions[cntr]);
                        REPORT_ERROR();
                    }
                }
                else
                {
                    print_transactions_context(ep_index, ep_num);
                    USBD_WARN(MAIN_APP_TEST_ERROR, "ep descriptor get failed!");
                    REPORT_ERROR();
                }
            }
        }
    }

    if(USBD_BOOL_IS_FALSE(test->test_failed))
    {
        USBD_DEBUG_LO(MAIN_APP_TEST, "Test finished correctly!");
    }

    USBD_EXIT_FUNC(MAIN_APP_TEST);
} /* check_result */

void iotp_test_recv_irq_next(
    USBD_Params_XT *usbd,
    USBD_IOTP_EVENT_Params_XT *tp,
    uint8_t ep_num,
    USB_EP_Direction_ET dir,
    uint8_t num_bufs,
    iotp_test_recv_irq_transaction_req_XT *transactions,
    size_t num_transactions /* max TEST_MAX_NUM_CONNECTED_TRANSACTIONS */)
{
    uint8_t                     ep_index        = ep_num * ((USB_EP_DIRECTION_IN == dir) ? 1 : 2);
    test_params_T              *test            = &test_params[ep_index];
    size_t                      cntr;
    const USB_Endpoint_Desc_DT*ep_desc;
    uint16_t                    mps             = 1;
    USBD_Bool_DT                is_control      = USBD_FALSE;
    USBD_Bool_DT                is_test_allowed = USBD_TRUE;

    USBD_ENTER_FUNC(MAIN_APP_TEST);

    if(USBD_CHECK_PTR(iotp_test_recv_irq_transaction_req_XT, transactions))
    {
        ep_desc = USBD_DEV_Get_EP_Desc(USBD_IOTP_EVENT_Get_USBD(tp), ep_num, test->dir);

        if(USBD_CHECK_PTR(const USB_Endpoint_Desc_DT, ep_desc))
        {
            mps = 0;
            mps += ep_desc->wMaxPacketSize.H;
            mps *= 256;
            mps += ep_desc->wMaxPacketSize.L;

            if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == (ep_desc->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK))
            {
                is_control = USBD_TRUE;
            }
        }

        for(cntr = 0; cntr < num_transactions; cntr++)
        {
            if(transactions[cntr].size > TEST_MAX_DATA_SIZE)
            {
                USBD_WARN_3(MAIN_APP_TEST_INTRO, "test size = %d too big for transaction: %d! now is cut to %d!", transactions[cntr].size, cntr, TEST_MAX_DATA_SIZE);
                REPORT_ERROR();
                transactions[cntr].size = TEST_MAX_DATA_SIZE;
            }

            if(USBD_BOOL_IS_TRUE(transactions[cntr].recv_to_whole_buffer) && USBD_BOOL_IS_FALSE(is_control))
            {
                if(0 == (transactions[cntr].size % mps))
                {
                    is_test_allowed = USBD_FALSE;
                    USBD_DEBUG_HI_2(MAIN_APP_TEST_INTRO, "test not allowed because of size = %d for transaction %d", transactions[cntr].size, cntr);
                }
            }
        }

        if(USBD_BOOL_IS_TRUE(is_test_allowed))
        {
            test->test_failed = USBD_FALSE;

            test->tp                    = tp;
            test->dir                   = dir;
            if(USB_EP_DIRECTION_IN == dir)
            {
                test->is_tp_in          = USBD_TRUE;
            }
            else
            {
                test->is_tp_in          = USBD_FALSE;
            }
            test->ep_num_bufs           = 1 + ((num_bufs - 1) & 1);
            test->transactions          = transactions;
            if(num_transactions <= TEST_MAX_NUM_CONNECTED_TRANSACTIONS)
            {
                test->num_transactions  = num_transactions;
            }
            else
            {
                test->num_transactions  = TEST_MAX_NUM_CONNECTED_TRANSACTIONS;
            }

            USBD_DEBUG_HI_5(MAIN_APP_TEST_INTRO, "%s: ep_num: %d, dir: %s, num buffers: %d; num_transactions: %d",
                __FUNCTION__, ep_num, (USB_EP_DIRECTION_IN == test->dir) ? "IN" : "OUT", test->ep_num_bufs, test->num_transactions);

            if(IOCMD_COMPILATION_SWITCH_LOG(MAIN_APP_TEST_INTRO, IOCMD_LOG_LEVEL_DEBUG_HI))
            {
               print_transactions_context(ep_index, ep_num);
            }

            prepare_test(usbd, ep_index, ep_num);
            perform_test(usbd, ep_index, ep_num);
            check_result(usbd, ep_index, ep_num);

            if((-1) != USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(USBD_IOTP_EVENT_Get_USBD(test->tp), ep_num, USBD_FALSE))
            {
                USBD_MARK_INVOKE_DESTINATION(USBD_IOTP_EVENT_GET_INVOKE_PARAMS(test->tp));

                USBD_IOTP_EVENT_Recv_Ready(test->tp);

                USBD_UNMARK_INVOKE_DESTINATION(USBD_IOTP_EVENT_GET_INVOKE_PARAMS(test->tp));
            }
        }
    }

    USBD_EXIT_FUNC(MAIN_APP_TEST);

    IOCMD_Clear_All_Logs(true);
} /* test_init */
