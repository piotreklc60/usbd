/** ---------------------------------------- LICENSE SECTION -------------------------------------------------------------------
 *
 * Copyright (c) 2025 Piotr Wojtowicz
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

#include "std_libs.h"

#define USBD_USE_INVALIDATION_ON_INIT                       USBD_FEATURE_NOT_PRESENT

#define USBD_MAX_NUM_CONFIGURATIONS                         1
#define USBD_MAX_NUM_INTERFACES                             1
#define USBD_MAX_NUM_ALTERNATE_INTERFACE_SETTINGS           0
#define USBD_MAX_NUM_ENDPOINTS                              1
#define USBD_MAX_NUM_STRINGS                                0
#define USBD_MAX_NUM_EVENTS                                 0
#define USBDC_MAX_NUM_EVENTS                                1

/**
 * WARNING!!!
 * Be careful when changing this definition. This is only for agressively reducing image size,
 * for example for DFU bootloader.
 * EP HALT function is mandatory when some error happen, to block endpoint.
 * However, if there is used only endpoint 0, blocking it causes device is no more usable.
 * Only when USBD_MAX_NUM_ENDPOINTS = 1 there is possible to remove HALT function, however it is not recommended!
 */
#define USBD_EP_HALT_SUPPORTED                              USBD_FEATURE_NOT_PRESENT

#define USBD_PORT_STM32_CAT_A_EP0_MPS                       64


#define USBD_PORT_STM32_CAT_A_DETECT_VBUS_CHANGE            USBD_FEATURE_NOT_PRESENT
#define USBD_PORT_STM32_CAT_A_USE_PULL_UP                   USBD_FEATURE_PRESENT

#define USBD_EVENT_REASON_UNPOWERED_SUPPORTED               USBD_FEATURE_NOT_PRESENT
#define USBD_EVENT_REASON_POWERED_SUPPORTED                 USBD_FEATURE_NOT_PRESENT
#define USBD_EVENT_REASON_RESET_SUPPORTED                   USBD_FEATURE_NOT_PRESENT
#define USBD_EVENT_REASON_ADDRESSED_SUPPORTED               USBD_FEATURE_NOT_PRESENT
#define USBD_EVENT_REASON_UNCONFIGURED_SUPPORTED            USBD_FEATURE_PRESENT
#define USBD_EVENT_REASON_CONFIGURED_SUPPORTED              USBD_FEATURE_PRESENT
#define USBD_EVENT_REASON_INTERFACE_SET_SUPPORTED           USBD_FEATURE_NOT_PRESENT
#define USBD_EVENT_REASON_SOF_RECEIVED_SUPPORTED            USBD_FEATURE_PRESENT
#define USBD_EVENT_REASON_SUSPENDED_SUPPORTED               USBD_FEATURE_NOT_PRESENT
#define USBD_EVENT_REASON_RESUMED_SUPPORTED                 USBD_FEATURE_NOT_PRESENT

#define USBD_IO_PROVIDE_METHODS_SUPPORTED                   USBD_FEATURE_NOT_PRESENT

#define USBD_DEV_SUPPORT_CONFIG_VALIDATION                  USBD_FEATURE_NOT_PRESENT

#define USBD_REQ_PORT_GUARD_SET_CONFIGURATION_SUPPORTED     USBD_DEV_SUPPORT_CONFIG_VALIDATION
#define USBD_REQ_VENDOR_SUPPORTED                           USBD_FEATURE_NOT_PRESENT
/**
 * WARNING!!!
 * Be careful when changing this definition. This is only for agressively reducing image size,
 * for example for DFU bootloader.
 * SET / CLEAR FEATURE requests are mandatory, however HOST doesn't
 * send it on standard enumeration, only when some specific action is executed, like EP HALT/UNHALT.
 * Be sure HOST will not use these requests in your application before turning OFF these requests.
 */
#define USBD_REQ_SET_CLEAR_FEATURE_SUPPORTED                USBD_FEATURE_NOT_PRESENT


#define DFU_MULTIPLE_MEMORIES_SUPPORT                       USBD_FEATURE_NOT_PRESENT
#define DFU_UPLOAD_SUPPORT                                  USBD_FEATURE_NOT_PRESENT
#define DFU_APP_MODE_SUPPORT                                USBD_FEATURE_NOT_PRESENT


#endif
