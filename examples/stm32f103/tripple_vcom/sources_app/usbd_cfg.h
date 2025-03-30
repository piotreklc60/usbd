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

#ifndef USBD_CFG_H_
#define USBD_CFG_H_

#include "cfg.h"

#include "usbd_port_stm32_cat_a_board_settings_cfg.h"

#define USBD_USE_INVALIDATION_ON_INIT              USBD_FEATURE_NOT_PRESENT

#define USBD_MAX_NUM_CONFIGURATIONS                5
#define USBD_MAX_NUM_INTERFACES                    7
#define USBD_MAX_NUM_ALTERNATE_INTERFACE_SETTINGS  0
#define USBD_MAX_NUM_ENDPOINTS                     8
#define USBD_MAX_NUM_STRINGS                       8
#define USBD_MAX_NUM_EVENTS                        4
#define USBDC_MAX_NUM_EVENTS                       5


#define USBD_IOTP_SUPPORT_VECTOR_BUFFERS           USBD_FEATURE_NOT_PRESENT
#define USBD_IOTP_SUPPORT_TREE_BUFFERS             USBD_FEATURE_NOT_PRESENT
#define USBD_IOTP_SUPPORT_RING_BUFFERS             USBD_FEATURE_NOT_PRESENT
#define USBD_IOTP_SUPPORT_RING_INFINITE_BUFFERS    USBD_FEATURE_PRESENT

#define USBD_IO_ISOCHRONOUS_TRANSFER_SUPPORTED     USBD_FEATURE_NOT_PRESENT

#define DFU_WILL_DETACH_SUPPORT                    USBD_PORT_STM32_CAT_A_USE_PULL_UP

#endif
