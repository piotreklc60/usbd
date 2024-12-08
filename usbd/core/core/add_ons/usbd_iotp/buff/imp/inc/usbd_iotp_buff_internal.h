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

/*!
 *  \brief    USB Device IO Transport Protocol RingBuffer-based module
 *  \details   This module defines IN-OUT API which is part of USBD software layer
 *  \author   Piotr Wojtowicz
 *  \version   1.3.0.0
 *  \date     2012
 *  \pre      First initialize the system.
 *  \warning   Improper use can crash your application
 *  \copyright GNU Public License.
 */

#ifndef USBD_IOTP_BUFF_INTERNAL_H_
#define USBD_IOTP_BUFF_INTERNAL_H_



#define USBD_IOTP_BUFF_CHECK_READY_HANDLER(tp)                USBD_CHECK_HANDLER(USBD_IOTP_BUFF_Callback_HT, (tp)->up_link.handlers.ready)
#define USBD_IOTP_BUFF_CHECK_BUF_EMPTY_HANDLER(tp)            USBD_CHECK_HANDLER(USBD_IOTP_BUFF_Callback_HT, (tp)->up_link.handlers.buf_empty)
#define USBD_IOTP_BUFF_CHECK_ERROR_HANDLER(tp)                USBD_CHECK_HANDLER(USBD_IOTP_BUFF_Callback_HT, (tp)->up_link.handlers.error)
#define USBD_IOTP_BUFF_CHECK_ABORT_HANDLER(tp)                USBD_CHECK_HANDLER(USBD_IOTP_BUFF_Callback_HT, (tp)->up_link.handlers.abort)


#define USBD_IOTP_BUFF_GET_READY_HANDLER(tp)                  USBD_GET_HANDLER(USBD_IOTP_BUFF_Callback_HT,   (tp)->up_link.handlers.ready)
#define USBD_IOTP_BUFF_GET_BUF_EMPTY_HANDLER(tp)              USBD_GET_HANDLER(USBD_IOTP_BUFF_Callback_HT,   (tp)->up_link.handlers.buf_empty)
#define USBD_IOTP_BUFF_GET_ERROR_HANDLER(tp)                  USBD_GET_HANDLER(USBD_IOTP_BUFF_Callback_HT,   (tp)->up_link.handlers.error)
#define USBD_IOTP_BUFF_GET_ABORT_HANDLER(tp)                  USBD_GET_HANDLER(USBD_IOTP_BUFF_Callback_HT,   (tp)->up_link.handlers.abort)
#define USBD_IOTP_BUFF_GET_VENDOR_DATA_PTR(tp)                (&((tp)->up_link.data.vendor_data))




#define USBD_IOTP_BUFF_SET_USBD_INSIDE_TP(tp, new_value)      USBD_SET_PTR(void, (tp)->core.pipe_params.data.usbd, (new_value))
#define USBD_IOTP_BUFF_SET_USBDC_INSIDE_TP(tp, new_value)     USBD_SET_PTR(void, (tp)->core.pipe_params.data.usbdc, (new_value))
#define USBD_IOTP_BUFF_SET_EP_NUM_INSIDE_TP(tp, new_value)    (tp)->core.pipe_params.data.ep_num = (new_value)
#define USBD_IOTP_BUFF_SET_EP_DIR_INSIDE_TP(tp, new_value)    (tp)->core.pipe_params.data.dir = (uint8_t)(new_value)
#define USBD_IOTP_BUFF_SET_BUFF_INSIDE_TP(tp, new_value)      (tp)->core.buff = (new_value)


#define USBD_IOTP_BUFF_GET_INVOKE_PARAMS(tp)                  (tp)->invoke.invoke


#define USBD_IOTP_BUFF_SET_READY_HANDLER(tp, new_value) \
            USBD_SET_HANDLER( \
               USBD_IOTP_BUFF_Callback_HT, \
               (tp)->up_link.handlers.ready, \
               (new_value))

#define USBD_IOTP_BUFF_SET_BUF_EMPTY_HANDLER(tp, new_value) \
            USBD_SET_HANDLER( \
               USBD_IOTP_BUFF_Callback_HT, \
               (tp)->up_link.handlers.buf_empty, \
               (new_value))

#define USBD_IOTP_BUFF_SET_ERROR_HANDLER(tp, new_value) \
            USBD_SET_HANDLER( \
               USBD_IOTP_BUFF_Callback_HT, \
               (tp)->up_link.handlers.error, \
               (new_value))

#define USBD_IOTP_BUFF_SET_ABORT_HANDLER(tp, new_value) \
            USBD_SET_HANDLER( \
               USBD_IOTP_BUFF_Callback_HT, \
               (tp)->up_link.handlers.abort, \
               (new_value))


#define USBD_IOTP_BUFF_CALL_READY(usbd, ep_num, dir, tp, size) \
            USBD_CALL_HANDLER(USBD_IOTP_BUFF_Callback_HT, (tp)->up_link.handlers.ready)( \
               ((USBD_IOTP_BUFF_Params_XT*)(tp)), \
               (dir), \
               (size))

#define USBD_IOTP_BUFF_CALL_BUF_EMPTY(usbd, ep_num, dir, tp, size) \
            USBD_CALL_HANDLER(USBD_IOTP_BUFF_Callback_HT, (tp)->up_link.handlers.buf_empty)( \
               ((USBD_IOTP_BUFF_Params_XT*)(tp)), \
               (dir), \
               (size))

#define USBD_IOTP_BUFF_CALL_ERROR(usbd, ep_num, dir, tp, size) \
            USBD_CALL_HANDLER(USBD_IOTP_BUFF_Callback_HT, (tp)->up_link.handlers.error)( \
               ((USBD_IOTP_BUFF_Params_XT*)(tp)), \
               (dir), \
               (size))

#define USBD_IOTP_BUFF_CALL_ABORT(usbd, ep_num, dir, tp, size) \
            USBD_CALL_HANDLER(USBD_IOTP_BUFF_Callback_HT, (tp)->up_link.handlers.abort)( \
               ((USBD_IOTP_BUFF_Params_XT*)(tp)), \
               (dir), \
               (size))


#endif
