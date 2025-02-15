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

#ifndef USBD_IOTP_BUFF_INTERNAL_H_
#define USBD_IOTP_BUFF_INTERNAL_H_



#define USBD_IOTP_BUFF_SET_USBD_INSIDE_TP(tp, new_value)      USBD_SET_PTR(void, (tp)->core.pipe_params.data.usbd, (new_value))
#define USBD_IOTP_BUFF_SET_USBDC_INSIDE_TP(tp, new_value)     USBD_SET_PTR(void, (tp)->core.pipe_params.data.usbdc, (new_value))
#define USBD_IOTP_BUFF_SET_EP_NUM_INSIDE_TP(tp, new_value)    (tp)->core.pipe_params.data.ep_num = (new_value)
#define USBD_IOTP_BUFF_SET_EP_DIR_INSIDE_TP(tp, new_value)    (tp)->core.pipe_params.data.dir = (uint8_t)(new_value)
#define USBD_IOTP_BUFF_SET_BUFF_INSIDE_TP(tp, new_value)      (tp)->core.buff = (new_value)


#define USBD_IOTP_BUFF_GET_INVOKE_PARAMS(tp)                  (tp)->invoke.invoke



#endif
