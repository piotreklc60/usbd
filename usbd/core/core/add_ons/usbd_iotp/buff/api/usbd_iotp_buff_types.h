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

#ifndef USBD_IOTP_BUFF_TYPES_H_
#define USBD_IOTP_BUFF_TYPES_H_

#define USBD_IOTP_BUFF_PRESENT      1


#ifndef STD_LIBS_H_
#include "std_libs.h"
#endif

#ifndef USBD_CFG_H_
#include "usbd_cfg.h"
#endif

#ifndef BUFF_H_
#include "buff.h"
#endif

#if(!BUFF_RING_USE_EXTENSIONS)
#error "usbd_iotp_buff requires buff library with extensions enabled!"
#endif

#ifndef USBD_IOTP_BUFF_USE_UP_LINK
/**
 * UP LINK (callbacks) is not necessary for this TP-type because Buff_Ring_XT provides already callbacks functionality.
 * For this reason USBD_IOTP_BUFF_USE_UP_LINK is declared by default as USBD_FEATURE_NOT_PRESENT.
 * However, if TP callbacks are still needed, then USBD_IOTP_BUFF_USE_UP_LINK must be defined in usbd_cfg.h as USBD_FEATURE_PRESENT
 */
#define USBD_IOTP_BUFF_USE_UP_LINK                 USBD_FEATURE_NOT_PRESENT
#endif

#ifndef USBD_IOTP_BUFF_SUPPORT_CONTROL_TRANSFER
/**
 * if defined as USBD_FEATURE_PRESENT then library will support CONTROL endpoint features like
 * sending 0-length packet if last control IN packet == MPS; function USBD_IOTP_BUFF_Send_Status
 * (used to force 0-length packet) is present, etc
 */
#define USBD_IOTP_BUFF_SUPPORT_CONTROL_TRANSFER    USBD_FEATURE_NOT_PRESENT
#endif


#define USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp)       USBD_GET_PTR(USBD_Params_XT, (tp)->core.pipe_params.data.usbd)
#define USBD_IOTP_BUFF_GET_USBDC_FROM_TP(tp)      USBD_GET_PTR(USBDC_Params_XT, (tp)->core.pipe_params.data.usbdc)
#define USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp)     ((tp)->core.pipe_params.data.ep_num)
#define USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp)     ((USB_EP_Direction_ET)((tp)->core.pipe_params.data.dir))
#define USBD_IOTP_BUFF_GET_BUFF_FROM_TP(tp)       USBD_GET_PTR(Buff_Ring_XT, (tp)->core.buff)

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)

#define USBD_IOTP_BUFF_GET_SIZE_TRANSFERRED(tp)   ((tp)->up_link.data.in_size_transferred)

typedef uint8_t USBD_IOTP_BUFF_Data_DT;

#endif



typedef struct USBD_IOTP_BUFF_Params_eXtended_Tag
{
   /* this field must be present for security reasons to identify properly tp as extension params in Buff_Ring_XT */
   const void *owner;
   struct
   {
#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
      struct
      {
         void (*ready)     (struct USBD_IOTP_BUFF_Params_eXtended_Tag *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size);
         void (*buf_empty) (struct USBD_IOTP_BUFF_Params_eXtended_Tag *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size);
         void (*error)     (struct USBD_IOTP_BUFF_Params_eXtended_Tag *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size);
         void (*abort)     (struct USBD_IOTP_BUFF_Params_eXtended_Tag *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size);
      }handlers;
#endif
      union
      {
#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
         USBD_IO_Inout_Data_Size_DT in_size_transferred;
         USBD_Bool_DT               out_transfer_active;
#endif
         USBD_Vendor_Data_XT        vendor_data;
      }data;
   }up_link;
#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)
   USBD_Bool_DT                     req_in_progress;
   USBD_IO_OUT_Data_Method_Port_HT  out_memcpy;
#endif
   struct
   {
      USBD_DECLARE_INVOKE_PARAMS(invoke)
      USBD_Atomic_Uint8_DT          req;
   }invoke;
   struct
   {
      Buff_Ring_XT                 *buff;
      struct
      {
         USBD_IO_UP_EP_Handlers_XT  handlers;
         struct
         {
            struct USBD_params_eXtended_Tag  *usbd;
            struct USBDC_params_eXtended_Tag *usbdc;
            uint16_t                mps;
            uint8_t                 ep_num;
            uint8_t                 ep_type;
            /* USB_EP_Direction_ET */
            uint8_t                 dir;
         }data;
      }pipe_params;
   }core;
}USBD_IOTP_BUFF_Params_XT;

#if(USBD_FEATURE_PRESENT == USBD_IOTP_BUFF_USE_UP_LINK)

typedef void (*USBD_IOTP_BUFF_Callback_HT) (USBD_IOTP_BUFF_Params_XT *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size);

#endif

#endif

