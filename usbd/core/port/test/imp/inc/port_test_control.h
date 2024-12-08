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

#ifndef PORT_TEST_CONTROL_H_
#define PORT_TEST_CONTROL_H_

#ifndef USBD_H_
#include "usbd.h"
#endif

/** ******************************************************************************************************************************
 * USBD DEV part
 ********************************************************************************************************************************/

void port_test_attached(void);
void port_test_detached(void);
void port_test_powered(void);
void port_test_unpowered(void);
void port_test_reset(void);
void port_test_addressed(uint8_t addr);
USBD_Bool_DT port_test_configured(uint8_t config_num);
void port_test_suspended(void);
void port_test_sof_received(void);

/** ******************************************************************************************************************************
 * USBD IO part
 ********************************************************************************************************************************/

typedef enum Port_Test_DMA_Type_Tag
{
    PORT_TEST_NO_DMA   = 0,
    PORT_TEST_HALF_DMA = 1,
    PORT_TEST_TRUE_DMA = 2
}Port_Test_DMA_Type_T;

void port_test_set_num_buffers(uint8_t ep_num, USB_EP_Direction_ET dir, uint8_t num_buffers);
void port_test_set_dma_type(uint8_t ep_num, USB_EP_Direction_ET dir, Port_Test_DMA_Type_T dma_type);
Port_Test_DMA_Type_T port_test_get_dma_type(uint8_t ep_num, USB_EP_Direction_ET dir);
USBD_Bool_DT port_test_is_dma_active(uint8_t ep_num, USB_EP_Direction_ET dir);
void port_test_set_data(uint8_t ep_num, USB_EP_Direction_ET dir, uint8_t *data, USBD_IO_Inout_Data_Size_DT size, USBD_IO_Inout_Data_Size_DT *size_done);
USBD_IO_Inout_Data_Size_DT port_test_get_waiting_user_data_size(uint8_t ep_num, USB_EP_Direction_ET dir);
uint32_t port_test_get_num_transactions_passed(uint8_t ep_num, USB_EP_Direction_ET dir);
uint8_t port_test_get_num_used_bufs(uint8_t ep_num, USB_EP_Direction_ET dir);
USBD_IO_Inout_Data_Size_DT port_test_get_out_ep_buf_size(uint8_t ep_num);

void port_test_process_in(uint8_t ep_num);
USBD_Bool_DT port_test_process_out(uint8_t ep_num);

#endif
