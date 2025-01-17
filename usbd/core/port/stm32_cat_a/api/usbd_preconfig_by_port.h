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

#ifndef USBD_PRECONFIG_BY_PORT_H_
#define USBD_PRECONFIG_BY_PORT_H_

#define USBD_DEV_SUPPORTED_SPEED                               USBD_DEV_FULL_SPEED_SUPPORTED

#define USBD_REQ_PORT_GUARD_GET_STATUS_SUPPORTED               USBD_FEATURE_NOT_PRESENT
#define USBD_REQ_PORT_GUARD_SET_CLEAR_FEATURE_SUPPORTED        USBD_FEATURE_NOT_PRESENT
#define USBD_REQ_PORT_GUARD_GET_DESCRIPTOR_SUPPORTED           USBD_FEATURE_NOT_PRESENT
#define USBD_REQ_PORT_GUARD_SET_DESCRIPTOR_SUPPORTED           USBD_FEATURE_NOT_PRESENT
#define USBD_REQ_PORT_GUARD_GET_CONFIGURATION_SUPPORTED        USBD_FEATURE_NOT_PRESENT
#define USBD_REQ_PORT_GUARD_GET_INTERFACE_SUPPORTED            USBD_FEATURE_NOT_PRESENT
#define USBD_REQ_PORT_GUARD_SYNCH_FRAME_SUPPORTED              USBD_FEATURE_NOT_PRESENT

#endif
