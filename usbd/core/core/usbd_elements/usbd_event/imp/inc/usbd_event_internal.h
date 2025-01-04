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

#ifndef USBD_EVENT_INTERNAL_H_
#define USBD_EVENT_INTERNAL_H_

#define USBD_EVENT_GET_EVENT_TAB_PTR(usbd)      USBD_GET_PTR(USBD_EVENT_Event_Header_XT,     (usbd)->event.core.event_tab)

#define USBDC_EVENT_GET_EVENT_TAB_PTR(usbdc)    USBD_GET_PTR(USBD_EVENT_Event_Header_XT,     (usbdc)->event.core.event_tab)

#define USBD_EVENT_CALL_EVENT(usbd, usbdc, event_tab, event_num, reason) \
            USBD_CALL_HANDLER(USBD_EVENT_Event_HT, ((event_tab)[event_num]).event)( \
               (usbd), \
               (usbdc), \
               &((event_tab)[event_num]), \
               (reason))

void USBD_EVENT_Process_Cold_Event(
      USBD_Params_XT *usbd,
      USBD_EVENT_Reason_ET reason);

#if(USBD_MAX_NUM_CONFIGURATIONS > 1)
void USBD_EVENT_Process_Warm_Event(
      USBD_Params_XT *usbd,
      USBD_EVENT_Reason_ET reason);
#else
#define USBD_EVENT_Process_Warm_Event  USBD_EVENT_Process_Cold_Event
#endif

#endif
