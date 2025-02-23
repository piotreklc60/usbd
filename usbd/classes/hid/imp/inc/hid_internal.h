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

#ifndef HID_INTERNAL_H_
#define HID_INTERNAL_H_

typedef enum
{
   USBD_HID_BREQUEST_GET_REPORT    = 0x01,
   USBD_HID_BREQUEST_SET_REPORT    = 0x09,

   USBD_HID_BREQUEST_GET_IDLE      = 0x02,
   USBD_HID_BREQUEST_SET_IDLE      = 0x0A,

   USBD_HID_BREQUEST_GET_PROTOCOL  = 0x03,
   USBD_HID_BREQUEST_SET_PROTOCOL  = 0x0B
}HID_bRequest_ET;

typedef enum
{
   HID_GET_DESCRIPTOR_INTERFACE  = (USBD_BMREQUESTTYPE_GET_SYNCH_INTERFACE | (USBD_BREQUEST_GET_DESCRIPTOR * 256)),
   HID_SET_DESCRIPTOR_INTERFACE  = (USBD_BMREQUESTTYPE_SET_CLEAR_INTERFACE | (USBD_BREQUEST_SET_DESCRIPTOR * 256)),

   HID_GET_REPORT_INTERFACE      = (USBD_BMREQUESTTYPE_GET_SYNCH_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (USBD_HID_BREQUEST_GET_REPORT * 256)),
   HID_SET_REPORT_INTERFACE      = (USBD_BMREQUESTTYPE_SET_CLEAR_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (USBD_HID_BREQUEST_SET_REPORT * 256)),

   HID_GET_IDLE_INTERFACE        = (USBD_BMREQUESTTYPE_GET_SYNCH_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (USBD_HID_BREQUEST_GET_IDLE * 256)),
   HID_SET_IDLE_INTERFACE        = (USBD_BMREQUESTTYPE_SET_CLEAR_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (USBD_HID_BREQUEST_SET_IDLE * 256)),

   HID_GET_PROTOCOL_INTERFACE    = (USBD_BMREQUESTTYPE_GET_SYNCH_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (USBD_HID_BREQUEST_GET_PROTOCOL * 256)),
   HID_SET_PROTOCOL_INTERFACE    = (USBD_BMREQUESTTYPE_SET_CLEAR_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (USBD_HID_BREQUEST_SET_PROTOCOL * 256))
} HID_Request_ET;


#endif

