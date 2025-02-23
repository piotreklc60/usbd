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

#ifndef USBD_IOTP_EVENT_TYPES_H_
#define USBD_IOTP_EVENT_TYPES_H_

#define USBD_IOTP_EVENT_PRESENT      1


#ifndef STD_LIBS_H_
#include "std_libs.h"
#endif

#ifndef USBD_CFG_H_
#include "usbd_cfg.h"
#endif



#define USBD_IOTP_EVENT_GET_USBD_FROM_TP(tp)       USBD_GET_PTR(USBD_Params_XT, (tp)->core.pipe_params.data.usbd)
#define USBD_IOTP_EVENT_GET_USBDC_FROM_TP(tp)      USBD_GET_PTR(USBDC_Params_XT, (tp)->core.pipe_params.data.usbdc)
#define USBD_IOTP_EVENT_GET_EP_NUM_FROM_TP(tp)     ((tp)->core.pipe_params.data.ep_num)
#define USBD_IOTP_EVENT_GET_EP_DIR_FROM_TP(tp)     ((tp)->core.pipe_params.data.dir)
#define USBD_IOTP_EVENT_GET_SIZE_TRANSFERRED(tp)   ((tp)->up_link.data.size_transferred)



typedef uint8_t USBD_IOTP_EVENT_Data_DT;


typedef struct USBD_IOTP_EVENT_Params_eXtended_Tag
{
   struct
   {
      struct
      {
         void (*ready)     (struct USBD_IOTP_EVENT_Params_eXtended_Tag *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size);
         void (*buf_empty) (struct USBD_IOTP_EVENT_Params_eXtended_Tag *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size);
         void (*error)     (struct USBD_IOTP_EVENT_Params_eXtended_Tag *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size);
         void (*abort)     (struct USBD_IOTP_EVENT_Params_eXtended_Tag *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size);
      }handlers;
      struct
      {
         USBD_IO_Inout_Data_Size_DT size_transferred;
         USBD_Vendor_Data_XT        vendor_data[2];
      }data;
   }up_link;
   struct
   {
      union
      {
         struct
         {
            struct
            {
               const USBD_IOTP_EVENT_Data_DT   *data;
               USBD_IO_Inout_Data_Size_DT       size_left;
               USBD_IO_Inout_Data_Size_DT       last_packet_size;
               USBD_Bool_DT                     req_in_progress;
            }proc;
         }in;
         struct
         {
            struct
            {
               USBD_IO_OUT_Data_Method_Port_HT  mem_cpy;
               USBD_IOTP_EVENT_Data_DT         *data;
               USBD_IO_Inout_Data_Size_DT       size_left;
               USBD_IO_Inout_Data_Size_DT       size_in_progress;
               USBD_Bool_DT                     dont_wait_for_ready;
            }proc;
         }out;
      }transfer_params;
      struct
      {
         USBD_IO_UP_EP_Handlers_XT handlers;
         struct
         {
            struct USBD_params_eXtended_Tag *usbd;
            struct USBDC_params_eXtended_Tag *usbdc;
            USBD_DECLARE_INVOKE_PARAMS(invoke)
            uint16_t mps;
            uint8_t ep_num;
            uint8_t ep_type;
            USB_EP_Direction_ET dir;
         }data;
      }pipe_params;
      /* when 0 then transfer is not present, when 1 then transfer is in progress */
      USBD_Atomic_Bool_DT transfer_active;
   }core;
}USBD_IOTP_EVENT_Params_XT;

typedef void (*USBD_IOTP_EVENT_Callback_HT) (USBD_IOTP_EVENT_Params_XT *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size);

#endif
