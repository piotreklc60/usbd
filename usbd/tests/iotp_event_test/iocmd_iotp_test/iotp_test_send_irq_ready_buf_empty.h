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

#ifndef IOTP_TEST_SEND_IRQ_READY_BUF_EMPTY_H_
#define IOTP_TEST_SEND_IRQ_READY_BUF_EMPTY_H_

#include "usbd.h"

typedef enum iotp_test_send_event_type_resend_Enum_Tag
{
    IOTP_TEST_RESEND_ON_READY,
    IOTP_TEST_RESEND_ON_BUF_EMPTY
}iotp_test_send_event_type_resend_ET;

typedef struct iotp_test_send_irq_transaction_req_eXtended_Tag
{
    uint8_t *data;
    USBD_IO_Inout_Data_Size_DT size;
    iotp_test_send_event_type_resend_ET resend_time;
}iotp_test_send_irq_transaction_req_XT;

void iotp_test_send_irq_ready_buf_empty(
    USBD_Params_XT *usbd,
    USBD_IOTP_EVENT_Params_XT *tp,
    uint8_t ep_num,
    USB_EP_Direction_ET dir,
    uint8_t num_bufs,
    uint8_t *data,
    USBD_IO_Inout_Data_Size_DT size,
    iotp_test_send_irq_transaction_req_XT *transactions,
    size_t num_transactions /* max TEST_MAX_NUM_CONNECTED_TRANSACTIONS */);

#endif
