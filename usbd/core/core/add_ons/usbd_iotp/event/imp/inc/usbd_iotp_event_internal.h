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
 *  \brief    USB Device IO Transport Protocol Event-based module
 *  \details   This module defines IN-OUT API which is part of USBD software layer
 *  \author   Piotr Wojtowicz
 *  \version   1.3.0.0
 *  \date     2012
 *  \pre      First initialize the system.
 *  \warning   Improper use can crash your application
 *  \copyright GNU Public License.
 */

#ifndef USBD_IOTP_EVENT_INTERNAL_H_
#define USBD_IOTP_EVENT_INTERNAL_H_



#define USBD_IOTP_EVENT_CHECK_READY_HANDLER(tp)                USBD_CHECK_HANDLER(USBD_IOTP_EVENT_Callback_HT, (tp)->up_link.handlers.ready)
#define USBD_IOTP_EVENT_CHECK_BUF_EMPTY_HANDLER(tp)            USBD_CHECK_HANDLER(USBD_IOTP_EVENT_Callback_HT, (tp)->up_link.handlers.buf_empty)
#define USBD_IOTP_EVENT_CHECK_ERROR_HANDLER(tp)                USBD_CHECK_HANDLER(USBD_IOTP_EVENT_Callback_HT, (tp)->up_link.handlers.error)
#define USBD_IOTP_EVENT_CHECK_ABORT_HANDLER(tp)                USBD_CHECK_HANDLER(USBD_IOTP_EVENT_Callback_HT, (tp)->up_link.handlers.abort)


#define USBD_IOTP_EVENT_GET_READY_HANDLER(tp)                  USBD_GET_HANDLER(USBD_IOTP_EVENT_Callback_HT,   (tp)->up_link.handlers.ready)
#define USBD_IOTP_EVENT_GET_BUF_EMPTY_HANDLER(tp)              USBD_GET_HANDLER(USBD_IOTP_EVENT_Callback_HT,   (tp)->up_link.handlers.buf_empty)
#define USBD_IOTP_EVENT_GET_ERROR_HANDLER(tp)                  USBD_GET_HANDLER(USBD_IOTP_EVENT_Callback_HT,   (tp)->up_link.handlers.error)
#define USBD_IOTP_EVENT_GET_ABORT_HANDLER(tp)                  USBD_GET_HANDLER(USBD_IOTP_EVENT_Callback_HT,   (tp)->up_link.handlers.abort)
#define USBD_IOTP_EVENT_GET_VENDOR_DATA_PTR(tp)                (&((tp)->up_link.data.vendor_data))



#define USBD_IOTP_EVENT_SET_USBD_INSIDE_TP(tp, new_value)      USBD_SET_PTR(void, (tp)->core.pipe_params.data.usbd, (new_value))
#define USBD_IOTP_EVENT_SET_USBDC_INSIDE_TP(tp, new_value)     USBD_SET_PTR(void, (tp)->core.pipe_params.data.usbdc, (new_value))
#define USBD_IOTP_EVENT_SET_EP_NUM_INSIDE_TP(tp, new_value)    (tp)->core.pipe_params.data.ep_num = (new_value)
#define USBD_IOTP_EVENT_SET_EP_DIR_INSIDE_TP(tp, new_value)    (tp)->core.pipe_params.data.dir = (new_value)


#define USBD_IOTP_EVENT_GET_INVOKE_PARAMS(tp)                  (tp)->core.pipe_params.data.invoke


#define USBD_IOTP_EVENT_SET_READY_HANDLER(tp, new_value) \
            USBD_SET_HANDLER( \
               USBD_IOTP_EVENT_Callback_HT, \
               (tp)->up_link.handlers.ready, \
               (new_value))

#define USBD_IOTP_EVENT_SET_BUF_EMPTY_HANDLER(tp, new_value) \
            USBD_SET_HANDLER( \
               USBD_IOTP_EVENT_Callback_HT, \
               (tp)->up_link.handlers.buf_empty, \
               (new_value))

#define USBD_IOTP_EVENT_SET_ERROR_HANDLER(tp, new_value) \
            USBD_SET_HANDLER( \
               USBD_IOTP_EVENT_Callback_HT, \
               (tp)->up_link.handlers.error, \
               (new_value))

#define USBD_IOTP_EVENT_SET_ABORT_HANDLER(tp, new_value) \
            USBD_SET_HANDLER( \
               USBD_IOTP_EVENT_Callback_HT, \
               (tp)->up_link.handlers.abort, \
               (new_value))


#define USBD_IOTP_EVENT_CALL_READY(usbd, ep_num, dir, tp, size) \
            USBD_CALL_HANDLER(USBD_IOTP_EVENT_Callback_HT, (tp)->up_link.handlers.ready)( \
               ((USBD_IOTP_EVENT_Params_XT*)(tp)), \
               (dir), \
               (size))

#define USBD_IOTP_EVENT_CALL_BUF_EMPTY(usbd, ep_num, dir, tp, size) \
            USBD_CALL_HANDLER(USBD_IOTP_EVENT_Callback_HT, (tp)->up_link.handlers.buf_empty)( \
               ((USBD_IOTP_EVENT_Params_XT*)(tp)), \
               (dir), \
               (size))

#define USBD_IOTP_EVENT_CALL_ERROR(usbd, ep_num, dir, tp, size) \
            USBD_CALL_HANDLER(USBD_IOTP_EVENT_Callback_HT, (tp)->up_link.handlers.error)( \
               ((USBD_IOTP_EVENT_Params_XT*)(tp)), \
               (dir), \
               (size))

#define USBD_IOTP_EVENT_CALL_ABORT(usbd, ep_num, dir, tp, size) \
            USBD_CALL_HANDLER(USBD_IOTP_EVENT_Callback_HT, (tp)->up_link.handlers.abort)( \
               ((USBD_IOTP_EVENT_Params_XT*)(tp)), \
               (dir), \
               (size))




/**
 * Instals IN and OUT TP structures and handlers in USB device for 0 endpont (default control pipe)
 * Pipe must be unlocked before calling it!!!
 *
 * \param usbd pointer to USBD_Params_XT structure which is a pointer to USB device
 * \param tp_params_in pointer to IN USBD_IOTP_EVENT_Params_XT structure which will be installed
 * \param tp_params_out pointer to OUT USBD_IOTP_EVENT_Params_XT structure which will be installed
 * \return USBD_TRUE when TP installed successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_IOTP_EVENT_Install_Default_Control_Pipe(
      USBD_Params_XT *usbd,
      USBD_IOTP_EVENT_Params_XT *tp_in,
      USBD_IOTP_EVENT_Params_XT *tp_out);


#endif