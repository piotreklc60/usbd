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

#ifndef CDC_VCOM_INTERNAL_H_
#define CDC_VCOM_INTERNAL_H_

typedef enum
{
   CDC_VCOM_SEND_ENCAPSULATED_COMMAND  = 0x00,
   CDC_VCOM_GET_ENCAPSULATED_RESPONSE  = 0x01,

   CDC_VCOM_SET_COMM_FEATURE           = 0x02,
   CDC_VCOM_GET_COMM_FEATURE           = 0x03,
   CDC_VCOM_CLEAR_COMM_FEATURE         = 0x04,

   CDC_VCOM_SET_LINE_CODING            = 0x20,
   CDC_VCOM_GET_LINE_CODING            = 0x21,

   CDC_VCOM_SET_CONTROL_LINE_STATE     = 0x22,

   CDC_VCOM_SEND_BREAK                 = 0x23
}CDC_VCOM_bRequest_ET;

typedef enum
{
   CDC_SEND_ENCAPSULATED_COMMAND_INTERFACE      = (USBD_BMREQUESTTYPE_SET_CLEAR_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (CDC_VCOM_SEND_ENCAPSULATED_COMMAND * 256)),
   CDC_GET_ENCAPSULATED_RESPONSE_INTERFACE      = (USBD_BMREQUESTTYPE_GET_SYNCH_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (CDC_VCOM_GET_ENCAPSULATED_RESPONSE * 256)),

   CDC_SET_COMM_FEATURE_INTERFACE               = (USBD_BMREQUESTTYPE_SET_CLEAR_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (CDC_VCOM_SET_COMM_FEATURE * 256)),
   CDC_GET_COMM_FEATURE_INTERFACE               = (USBD_BMREQUESTTYPE_GET_SYNCH_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (CDC_VCOM_GET_COMM_FEATURE * 256)),
   CDC_CLEAR_COMM_FEATURE_INTERFACE             = (USBD_BMREQUESTTYPE_SET_CLEAR_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (CDC_VCOM_CLEAR_COMM_FEATURE * 256)),

   CDC_SET_LINE_CODING_INTERFACE                = (USBD_BMREQUESTTYPE_SET_CLEAR_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (CDC_VCOM_SET_LINE_CODING * 256)),
   CDC_GET_LINE_CODING_INTERFACE                = (USBD_BMREQUESTTYPE_GET_SYNCH_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (CDC_VCOM_GET_LINE_CODING * 256)),

   CDC_SET_CONTROL_LINE_STATE_INTERFACE         = (USBD_BMREQUESTTYPE_SET_CLEAR_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (CDC_VCOM_SET_CONTROL_LINE_STATE * 256)),

   CDC_SEND_BREAK_INTERFACE                     = (USBD_BMREQUESTTYPE_SET_CLEAR_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (CDC_VCOM_SEND_BREAK * 256))
} CDC_VCOM_Request_ET;


#endif

