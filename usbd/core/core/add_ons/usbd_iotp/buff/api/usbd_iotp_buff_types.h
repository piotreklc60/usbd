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



#define USBD_IOTP_BUFF_GET_USBD_FROM_TP(tp)       USBD_GET_PTR(USBD_Params_XT, (tp)->core.pipe_params.data.usbd)
#define USBD_IOTP_BUFF_GET_USBDC_FROM_TP(tp)      USBD_GET_PTR(USBDC_Params_XT, (tp)->core.pipe_params.data.usbdc)
#define USBD_IOTP_BUFF_GET_EP_NUM_FROM_TP(tp)     ((tp)->core.pipe_params.data.ep_num)
#define USBD_IOTP_BUFF_GET_EP_DIR_FROM_TP(tp)     ((USB_EP_Direction_ET)((tp)->core.pipe_params.data.dir))
#define USBD_IOTP_BUFF_GET_BUFF_FROM_TP(tp)       USBD_GET_PTR(Buff_Ring_XT, (tp)->core.buff)



#define USBD_IOTP_BUFF_SLOW_DOWN_OUT_PIPE_WHEN_BUFF_IS_FULL          BUFF_FALSE
#define USBD_IOTP_BUFF_ALLOW_OUT_BUFF_OVERWRITE_WHEN_BUFF_IS_FULL    BUFF_TRUE



typedef struct USBD_IOTP_BUFF_Params_eXtended_Tag
{
   /* this field must be present for security reasons to identify properly tp as extension params in Buff_Ring_XT */
   const void *owner;
   struct
   {
      union
      {
         USBD_Vendor_Data_XT        vendor_data;
         Buff_Bool_DT               allow_out_buff_overwrite_when_full;
      }data;
   }up_link;
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


#endif

