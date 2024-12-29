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

#ifndef DFU_INTERNAL_H_
#define DFU_INTERNAL_H_

typedef enum
{
   USBD_DFU_BREQUEST_DETACH        = 0,
   USBD_DFU_BREQUEST_DNLOAD        = 1,
   USBD_DFU_BREQUEST_UPLOAD        = 2,
   USBD_DFU_BREQUEST_GETSTATUS     = 3,
   USBD_DFU_BREQUEST_CLRSTATUS     = 4,
   USBD_DFU_BREQUEST_GETSTATE      = 5,
   USBD_DFU_BREQUEST_ABORT         = 6,

   USBD_RESET                      = 7,
   USBD_DFU_BWPOLLTIMEOUT_ELAPSED  = 8
} DFU_bRequest_ET;

typedef enum
{
   DFU_DETACH_INTERFACE    = (USBD_BMREQUESTTYPE_SET_CLEAR_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (USBD_DFU_BREQUEST_DETACH      * 256)),
   DFU_DNLOAD_INTERFACE    = (USBD_BMREQUESTTYPE_SET_CLEAR_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (USBD_DFU_BREQUEST_DNLOAD      * 256)),
   DFU_UPLOAD_INTERFACE    = (USBD_BMREQUESTTYPE_GET_SYNCH_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (USBD_DFU_BREQUEST_UPLOAD      * 256)),
   DFU_GETSTATUS_INTERFACE = (USBD_BMREQUESTTYPE_GET_SYNCH_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (USBD_DFU_BREQUEST_GETSTATUS   * 256)),
   DFU_CLRSTATUS_INTERFACE = (USBD_BMREQUESTTYPE_SET_CLEAR_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (USBD_DFU_BREQUEST_CLRSTATUS   * 256)),
   DFU_GETSTATE_INTERFACE  = (USBD_BMREQUESTTYPE_GET_SYNCH_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (USBD_DFU_BREQUEST_GETSTATE    * 256)),
   DFU_ABORT_INTERFACE     = (USBD_BMREQUESTTYPE_SET_CLEAR_INTERFACE | USBD_BMREQUESTTYPE_CLASS | (USBD_DFU_BREQUEST_ABORT       * 256))
} DFU_Request_ET;



typedef union
{
   uint32_t timeout;
   struct {
      uint8_t L;
      uint8_t M;
      uint8_t H;
      uint8_t not_used;
   }elements;
}DFU_timeout;



#endif

