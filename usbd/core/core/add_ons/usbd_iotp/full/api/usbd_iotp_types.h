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

#ifndef USBD_IOTP_TYPES_H_
#define USBD_IOTP_TYPES_H_

#define USBD_IOTP_PRESENT      1


#ifndef STD_LIBS_H_
#include "std_libs.h"
#endif

#ifndef USBD_CFG_H_
#include "usbd_cfg.h"
#endif



#ifndef USBD_IOTP_SUPPORT_VECTOR_BUFFERS
#define USBD_IOTP_SUPPORT_VECTOR_BUFFERS           USBD_FEATURE_PRESENT
#endif

#ifndef USBD_IOTP_SUPPORT_TREE_BUFFERS
#define USBD_IOTP_SUPPORT_TREE_BUFFERS             USBD_FEATURE_PRESENT
#endif

#ifndef USBD_IOTP_SUPPORT_RING_BUFFERS
#define USBD_IOTP_SUPPORT_RING_BUFFERS             USBD_FEATURE_PRESENT
#endif

#ifndef USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS
#define USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS  USBD_FEATURE_PRESENT
#endif

#ifndef USBD_IOTP_SUPPORT_COMPLEX_BUFFERS
#define USBD_IOTP_SUPPORT_COMPLEX_BUFFERS    (USBD_IOTP_SUPPORT_VECTOR_BUFFERS || USBD_IOTP_SUPPORT_TREE_BUFFERS || USBD_IOTP_SUPPORT_RING_BUFFERS || USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS)
#endif



#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_COMPLEX_BUFFERS)
#ifndef BUFF_H_
#include "buff.h"
#endif
#endif



#define USBD_IOTP_GET_USBD_FROM_TP(tp)       USBD_GET_PTR(USBD_Params_XT, (tp)->core.pipe_params.data.usbd)
#define USBD_IOTP_GET_USBDC_FROM_TP(tp)      USBD_GET_PTR(USBDC_Params_XT, (tp)->core.pipe_params.data.usbdc)
#define USBD_IOTP_GET_EP_NUM_FROM_TP(tp)     ((tp)->core.pipe_params.data.ep_num)
#define USBD_IOTP_GET_EP_DIR_FROM_TP(tp)     ((tp)->core.pipe_params.data.dir)
#define USBD_IOTP_GET_SIZE_TRANSFERRED(tp)   ((tp)->up_link.data.size_transferred)
#define USBD_IOTP_GET_RING_FROM_TP(tp)       ((tp)->core.transfer_params.data.data.ring)


#define USBD_IOTP_DATA_LINEAR             0
#define USBD_IOTP_DATA_VECTOR             1
#define USBD_IOTP_DATA_TREE               2
#define USBD_IOTP_DATA_RING               3
#define USBD_IOTP_DATA_RING_INFINITIVE    4

typedef enum USBD_IOTP_Ring_Invoke_Requests_Enum_Tag
{
   USBD_IOTP_INVOKE_REQ_NONE = 0,
   USBD_IOTP_INVOKE_REQ_ABORT_NO_FLUSH_HW,
   USBD_IOTP_INVOKE_REQ_ABORT_FLUSH_HW
}USBD_IOTP_Ring_Invoke_Requests_ET;

#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_COMPLEX_BUFFERS)
typedef Buff_Readable_Vector_XT USBD_IOTP_Next_Contineous_In_Part_XT;
#else
typedef struct
{
   const void                *data;
   USBD_IO_Inout_Data_Size_DT size;
}USBD_IOTP_Next_Contineous_In_Part_XT;
#endif

struct USBD_IOTP_Params_eXtended_Tag;

typedef uint8_t USBD_IOTP_Data_DT;

typedef struct USBD_IOTP_Pipe_Params_eXtended_Tag
{
   struct
   {
      USBD_DECLARE_INVOKE_PARAMS(invoke)
      USBD_Atomic_Uint8_DT          req;
   }invoke;
   USBD_IO_UP_EP_Handlers_XT handlers;
   struct
   {
      struct USBD_params_eXtended_Tag *usbd;
      struct USBDC_params_eXtended_Tag *usbdc;
      uint16_t mps;
      uint8_t ep_num;
      uint8_t ep_type;
      USB_EP_Direction_ET dir;
   }data;
}USBD_IOTP_Pipe_Params_XT;

typedef struct USBD_IOTP_Transfer_Params_Ring_Infinite_Only_eXtended_Tag
{
   uint8_t             data_type;
   /* when 0 then transfer is not present, when 1 then transfer is in progress */
   USBD_Atomic_Bool_DT transfer_active;
   struct
   {
      union
      {
         Buff_Ring_XT *ring;
      }data;
   }data;
}USBD_IOTP_Transfer_Params_Ring_Infinite_Only_XT;

typedef struct USBD_IOTP_Transfer_Params_eXtended_Tag
{
   uint8_t             data_type;
   /* when 0 then transfer is not present, when 1 then transfer is in progress */
   USBD_Atomic_Bool_DT transfer_active;
   struct
   {
      union
      {
         const USBD_IOTP_Data_DT            *linear;
#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_COMPLEX_BUFFERS)
         struct
         {
            const Buff_Readable_Vector_XT   *data;
            Buff_Num_Elems_DT                num_elems;
         }vector;
         struct
         {
            const Buff_Readable_Tree_XT     *data;
            Buff_Num_Elems_DT                num_elems;
         }tree;
         Buff_Ring_XT                       *ring;
#endif
      }data;
      USBD_IO_Inout_Data_Size_DT             offset;
      USBD_IO_Inout_Data_Size_DT             size;
   }data;
   union
   {
      struct
      {
         USBD_IOTP_Next_Contineous_In_Part_XT   next_contineous_part;
         USBD_IO_Inout_Data_Size_DT             last_packet_size;
         USBD_Bool_DT                           req_in_progress;
      }in;
      struct
      {
         USBD_IO_Inout_Data_Size_DT             next_contineous_part;
         USBD_IO_OUT_Data_Method_Port_HT        mem_cpy;
         USBD_IO_Inout_Data_Size_DT             size_in_progress;
         USBD_Bool_DT                           dont_wait_for_ready;
      }out;
   }dir;
}USBD_IOTP_Transfer_Params_XT;

typedef struct USBD_IOTP_Up_Link_Params_eXtended_Tag
{
   struct
   {
      void (*ready)     (struct USBD_IOTP_Params_eXtended_Tag *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size);
      void (*buf_empty) (struct USBD_IOTP_Params_eXtended_Tag *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size);
      void (*error)     (struct USBD_IOTP_Params_eXtended_Tag *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size);
      void (*abort)     (struct USBD_IOTP_Params_eXtended_Tag *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size);
   }handlers;
   struct
   {
      USBD_IO_Inout_Data_Size_DT size_transferred;
      USBD_Vendor_Data_XT        vendor_data[2];
   }data;
}USBD_IOTP_Up_Link_Params_XT;

typedef struct USBD_IOTP_Params_eXtended_Tag
{
   struct
   {
      USBD_IOTP_Pipe_Params_XT      pipe_params;
      USBD_IOTP_Transfer_Params_XT  transfer_params;
   }core;
   USBD_IOTP_Up_Link_Params_XT      up_link;
}USBD_IOTP_Params_XT;

/* this structure can be used instead USBD_IOTP_Params_XT when ONLY USBD_IOTP_DATA_RING_INFINITIVE data type is used */
typedef struct USBD_IOTP_Params_Ring_Infinite_Only_eXtended_Tag
{
   struct
   {
      USBD_IOTP_Pipe_Params_XT                        pipe_params;
      USBD_IOTP_Transfer_Params_Ring_Infinite_Only_XT transfer_params;
   }core;
}USBD_IOTP_Params_Ring_Infinite_Only_XT;

typedef void (*USBD_IOTP_Callback_HT) (USBD_IOTP_Params_XT *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size);

#endif
