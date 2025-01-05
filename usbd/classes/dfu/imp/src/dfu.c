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

#ifndef DFU_H_
#include "dfu.h"
#endif

#ifndef DFU_INTERNAL_H_
#include "dfu_internal.h"
#endif



#define DFU_STATE_CHANGE(_dfu, _new_state) \
   USBD_DEBUG_HI_2(DFU_STATE, "DFU state change: %d->%d", (_dfu)->core.status.bState, (_new_state)); \
   (_dfu)->core.status.bState = (_new_state)




static void DFU_on_timeout(DFU_Params_XT *dfu)
{
   USBD_ENTER_FUNC(DFU_EVENT);

   switch(dfu->core.status.bState)
   {
#if(USBD_FEATURE_PRESENT == DFU_APP_MODE_SUPPORT)
      case DFU_BSTATE_APP_DETACH:
         DFU_STATE_CHANGE(dfu, DFU_BSTATE_APP_IDLE);
         break;
#endif

#if(USBD_FEATURE_PRESENT == DFU_DFU_MODE_SUPPORT)
      case DFU_BSTATE_DFU_MANIFEST_WAIT_RESET:
         DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_IDLE);
         break;

      case DFU_BSTATE_DFU_DNBUSY:
         DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_DNLOAD_IDLE);
         break;

      case DFU_BSTATE_DFU_MANIFEST:
         DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_MANIFEST_SYNC);
         dfu->core.manifestation_done = USBD_TRUE;
         break;
#endif

      default:
         break;
   }

   USBD_EXIT_FUNC(DFU_EVENT);
} /* DFU_on_timeout */

static void DFU_get_status(DFU_Params_XT *dfu, USBD_IOTP_EVENT_Params_XT *tp_in)
{
   uint32_t timeout;
   USBD_Bool_DT send_status = USBD_TRUE;

   USBD_ENTER_FUNC(DFU_REQ);

#if(USBD_FEATURE_PRESENT == DFU_DFU_MODE_SUPPORT)
   switch(dfu->core.status.bState)
   {
      case DFU_BSTATE_DFU_DNLOAD_SYNC:
         if(0 != dfu->core.bwPollTimeout)
         {
            DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_DNBUSY);
         }
         else
         {
            DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_DNLOAD_IDLE);
         }
         break;

      case DFU_BSTATE_DFU_MANIFEST_SYNC:
         if(USBD_BOOL_IS_FALSE(dfu->core.manifestation_done))
         {
            DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_MANIFEST);

            /* Call MANIFEST_IMAGE event before sending status so that upper layer can set proper bwPollTimeout it needs for the manifestation */
            if(USBD_CHECK_HANDLER(DFU_Event_HT, dfu->user_data.handlers.user_event))
            {
               USBD_CALL_HANDLER(DFU_Event_HT, dfu->user_data.handlers.user_event)(dfu, DFU_USER_EVENT_MANIFEST_IMAGE);
            }

            if(0 == dfu->core.bwPollTimeout)
            {
               DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_MANIFEST_SYNC);
               dfu->core.manifestation_done = USBD_TRUE;
            }
         }
         else
         {
            DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_IDLE);
            dfu->core.manifestation_done = USBD_FALSE;
         }
         break;

      case DFU_BSTATE_DFU_DNBUSY:
      case DFU_BSTATE_DFU_MANIFEST:
         DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_ERROR);
         dfu->core.status.bStatus = DFU_BSTATUS_ERR_STALLEDPKT;
         send_status = USBD_FALSE;
         break;

      default:
         break;
   }
#endif

   if(USBD_BOOL_IS_TRUE(send_status))
   {
      timeout = dfu->core.bwPollTimeout;
      dfu->core.status.bwPollTimeout_L = (uint8_t)timeout;
      timeout /= 256;
      dfu->core.status.bwPollTimeout_M = (uint8_t)timeout;
      timeout /= 256;
      dfu->core.status.bwPollTimeout_H = (uint8_t)timeout;
      USBD_IOTP_EVENT_Send(
         tp_in, (uint8_t*)(&(dfu->core.status)), sizeof(dfu->core.status), USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
   }
   else
   {
      USBD_IOTP_EVENT_Send_Stall(tp_in);
   }

   USBD_EXIT_FUNC(DFU_REQ);
} /* DFU_get_status */

static void DFU_get_state(DFU_Params_XT *dfu, USBD_IOTP_EVENT_Params_XT *tp_in)
{
   USBD_Bool_DT send_state = USBD_TRUE;

   USBD_ENTER_FUNC(DFU_REQ);

#if(USBD_FEATURE_PRESENT == DFU_DFU_MODE_SUPPORT)
   switch(dfu->core.status.bState)
   {
      case DFU_BSTATE_DFU_DNBUSY:
      case DFU_BSTATE_DFU_MANIFEST:
         DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_ERROR);
         dfu->core.status.bStatus = DFU_BSTATUS_ERR_STALLEDPKT;
         send_state = USBD_FALSE;
         break;

      default:
         break;
   }
#endif

   if(USBD_BOOL_IS_TRUE(send_state))
   {
      USBD_IOTP_EVENT_Send(
         tp_in,
         (uint8_t*)(&(dfu->core.status.bState)),
         sizeof(dfu->core.status.bState),
         USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
   }
   else
   {
      USBD_IOTP_EVENT_Send_Stall(tp_in);
   }

   USBD_EXIT_FUNC(DFU_REQ);
} /* DFU_get_state */

#if(USBD_FEATURE_PRESENT == DFU_WILL_DETACH_SUPPORT)
static void DFU_detach_status_ready(USBD_IOTP_EVENT_Params_XT *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   DFU_Params_XT *dfu;
   USBD_Vendor_Data_XT *tp_vendor_data;

   USBD_UNUSED_PARAM(dir);
   USBD_UNUSED_PARAM(size);

   USBD_ENTER_FUNC(DFU_REQ);

   tp_vendor_data = USBD_IOTP_EVENT_Get_Vendor_Data_Container(tp);
   dfu = tp_vendor_data->pvoid;

   if(USBD_CHECK_HANDLER(DFU_Event_HT, dfu->user_data.handlers.user_event))
   {
      USBD_CALL_HANDLER(DFU_Event_HT, dfu->user_data.handlers.user_event)(
         dfu, (dfu->core.status.bState < DFU_BSTATE_DFU_IDLE) ? DFU_USER_EVENT_DETACH_GO_TO_DFU : DFU_USER_EVENT_DETACH_GO_TO_APP);
   }

   USBD_EXIT_FUNC(DFU_REQ);
} /* DFU_detach_status_ready */
#endif

static void DFU_detach(DFU_Params_XT *dfu, USBD_IOTP_EVENT_Params_XT *tp_in, uint32_t timeout)
{
   USBD_Vendor_Data_XT *tp_vendor_data;
   USBD_Bool_DT send_status = USBD_FALSE;

   USBD_ENTER_FUNC(DFU_REQ);

   switch(dfu->core.status.bState)
   {
#if(USBD_FEATURE_PRESENT == DFU_APP_MODE_SUPPORT)
      case DFU_BSTATE_APP_IDLE:
         DFU_STATE_CHANGE(dfu, DFU_BSTATE_APP_DETACH);
         send_status = USBD_TRUE;
         break;
#endif

#if(USBD_FEATURE_PRESENT == DFU_DFU_MODE_SUPPORT)
      case DFU_BSTATE_DFU_IDLE:
         DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_MANIFEST_WAIT_RESET);
         send_status = USBD_TRUE;
         break;
#endif

      default:
         break;
   }

   if(USBD_BOOL_IS_TRUE(send_status))
   {
#if(USBD_FEATURE_PRESENT == DFU_WILL_DETACH_SUPPORT)
      USBD_IOTP_EVENT_Set_Buf_Empty_Handler(tp_in, DFU_detach_status_ready);
      tp_vendor_data = USBD_IOTP_EVENT_Get_Vendor_Data_Container(tp_in);
      tp_vendor_data->pvoid =  dfu;
#endif
      dfu->core.bwPollTimeout = timeout;

      USBD_IOTP_EVENT_Send_Status(tp_in, USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
   }
   else
   {
      DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_ERROR);
      dfu->core.status.bStatus = DFU_BSTATUS_ERR_STALLEDPKT;
      USBD_IOTP_EVENT_Send_Stall(tp_in);
   }

   USBD_EXIT_FUNC(DFU_REQ);
} /* DFU_detach */

#if(USBD_FEATURE_PRESENT == DFU_DFU_MODE_SUPPORT)
#if(USBD_FEATURE_PRESENT == DFU_DNLOAD_SUPPORT)
static void DFU_download_recv_ready(USBD_IOTP_EVENT_Params_XT *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   DFU_Params_XT       *dfu;
   USBD_Vendor_Data_XT *tp_vendor_data;

   USBD_UNUSED_PARAM(dir);
   USBD_UNUSED_PARAM(size);

   USBD_ENTER_FUNC(DFU_REQ);

   USBD_DEBUG_HI_2(DFU_REQ, "%s transfer with size %d DONE", "OUT", size);

   tp_vendor_data = USBD_IOTP_EVENT_Get_Vendor_Data_Container(tp);
   dfu = tp_vendor_data->pvoid;

   if(USBD_CHECK_HANDLER(DFU_Download_HT, dfu->user_data.handlers.part_download_ready))
   {
      dfu->core.status.bStatus = USBD_CALL_HANDLER(DFU_Download_HT, dfu->user_data.handlers.part_download_ready)(
         dfu, &(dfu->core.transfer_buffer), dfu->core.transfer_data_size, dfu->core.packet_number);

      if(DFU_BSTATUS_OK != dfu->core.status.bStatus)
      {
         DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_ERROR);
      }

      (void)USBD_IOTP_EVENT_Send_Status_For_Out_Tp(
         tp,
         USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT),
         USBD_MAKE_INVALID_HANDLER(USBD_IOTP_EVENT_Callback_HT),
         USBD_MAKE_INVALID_PTR(USBD_Vendor_Data_XT));
   }

   USBD_EXIT_FUNC(DFU_REQ);
} /* DFU_download_recv_ready */

static void DFU_download(
   DFU_Params_XT             *dfu,
   USBD_IOTP_EVENT_Params_XT *tp_in,
   USBD_IOTP_EVENT_Params_XT *tp_out,
   uint16_t                   length,
   uint16_t                   block_num)
{
   USBD_Vendor_Data_XT *tp_vendor_data;

   USBD_ENTER_FUNC(DFU_REQ);

   dfu->core.status.bStatus = DFU_BSTATUS_OK;

   switch(dfu->core.status.bState)
   {
      case DFU_BSTATE_DFU_IDLE:
      case DFU_BSTATE_DFU_DNLOAD_IDLE:
         if((length > 0) && (length <= DFU_W_TRANSFER_SIZE))
         {
            DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_DNLOAD_SYNC);
            dfu->core.packet_number = block_num;
            USBD_IOTP_EVENT_Set_Ready_Handler(tp_out, DFU_download_recv_ready);
            tp_vendor_data = USBD_IOTP_EVENT_Get_Vendor_Data_Container(tp_out);
            tp_vendor_data->pvoid =  dfu;
            if(USBD_CHECK_PTR(uint8_t, dfu->core.transfer_buffer))
            {
               dfu->core.transfer_data_size = length;
               if(USBD_BOOL_IS_FALSE(USBD_IOTP_EVENT_Recv_And_Ready(
                  tp_out, dfu->core.transfer_buffer, length, USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT))))
               {
                  USBD_ERROR_2(DFU_REQ, "unable to initiate %s transfer with size %d", "OUT", length);
               }
            }
            else
            {
               dfu->core.status.bStatus = DFU_BSTATUS_ERR_STALLEDPKT;
            }
         }
         else if((DFU_BSTATE_DFU_DNLOAD_IDLE == dfu->core.status.bState) && (0 == length))
         {
            DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_MANIFEST_SYNC);
            USBD_IOTP_EVENT_Send_Status(tp_in, USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
         }
         else
         {
            dfu->core.status.bStatus = DFU_BSTATUS_ERR_STALLEDPKT;
         }
         break;

      default:
         dfu->core.status.bStatus = DFU_BSTATUS_ERR_STALLEDPKT;
         break;
   }

   if(DFU_BSTATUS_OK != dfu->core.status.bStatus)
   {
      DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_ERROR);
      USBD_IOTP_EVENT_Send_Stall(tp_in);
   }

   USBD_EXIT_FUNC(DFU_REQ);
} /* DFU_download */
#endif

#if(USBD_FEATURE_PRESENT == DFU_UPLOAD_SUPPORT)
static void DFU_upload(
   DFU_Params_XT *dfu, USBD_IOTP_EVENT_Params_XT *tp_in, uint16_t *length, uint16_t block_num)
{
   USBD_ENTER_FUNC(DFU_REQ);

   dfu->core.status.bStatus = DFU_BSTATUS_OK;

   switch(dfu->core.status.bState)
   {
      case DFU_BSTATE_DFU_IDLE:
      case DFU_BSTATE_DFU_UPLOAD_IDLE:
         if(USBD_CHECK_HANDLER(DFU_Upload_HT, dfu->user_data.handlers.part_upload_prepare)
            && (*length > 0) && (*length <= DFU_W_TRANSFER_SIZE))
         {
            dfu->core.status.bStatus = USBD_CALL_HANDLER(DFU_Upload_HT, dfu->user_data.handlers.part_upload_prepare)(
               dfu, &(dfu->core.transfer_buffer), length, dfu->core.packet_number);

            if((DFU_BSTATUS_OK == dfu->core.status.bStatus) && (*length > 0) && (*length <= DFU_W_TRANSFER_SIZE))
            {
               DFU_STATE_CHANGE(dfu, (DFU_W_TRANSFER_SIZE == *length) ? DFU_BSTATE_DFU_UPLOAD_IDLE : DFU_BSTATE_DFU_IDLE);
               dfu->core.packet_number = block_num;
               if(USBD_CHECK_PTR(uint8_t, dfu->core.transfer_buffer))
               {
                  if(USBD_BOOL_IS_FALSE(USBD_IOTP_EVENT_Send(
                     tp_in,
                     dfu->core.transfer_buffer,
                     (USBD_IO_Inout_Data_Size_DT)(*length),
                     USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT))))
                  {
                     USBD_ERROR_2(DFU_REQ, "unable to initiate %s transfer with size %d", "IN", *length);
                  }
               }
               else
               {
                  dfu->core.status.bStatus = DFU_BSTATUS_ERR_STALLEDPKT;
               }
            }
         }
         else
         {
            dfu->core.status.bStatus = DFU_BSTATUS_ERR_STALLEDPKT;
         }
         break;

      default:
         dfu->core.status.bStatus = DFU_BSTATUS_ERR_STALLEDPKT;
         break;
   }

   if(DFU_BSTATUS_OK != dfu->core.status.bStatus)
   {
      DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_ERROR);
      USBD_IOTP_EVENT_Send_Stall(tp_in);
   }

   USBD_EXIT_FUNC(DFU_REQ);
} /* DFU_upload */
#endif
#endif

#if(USBD_FEATURE_PRESENT == DFU_APP_MODE_SUPPORT)
static USBD_Bool_DT DFU_APP_on_request(
   USBDC_Params_XT *usbdc,
   uint8_t ep_num,
   uint8_t if_num,
   USBD_REQUEST_Req_DT *req,
   USBD_IOTP_EVENT_Params_XT *tp_in,
   USBD_IOTP_EVENT_Params_XT *tp_out)
{
   USBDC_Interface_Header_XT *if_params;
   DFU_Params_XT *dfu;
   USBD_Bool_DT result = USBD_FALSE;

   USBD_UNUSED_PARAM(ep_num);
   USBD_UNUSED_PARAM(tp_out);

   USBD_ENTER_FUNC(DFU_REQ);

   if_params = USBDC_Get_Interface_Params(usbdc, if_num);

   if(USBD_CHECK_PTR(USBDC_Interface_Header_XT, if_params))
   {
      dfu = if_params->vendor.pvoid;
      result = USBD_TRUE;

      if(USBD_CHECK_PTR(DFU_Params_XT, dfu))
      {
         switch(req->request)
         {
            case DFU_GETSTATUS_INTERFACE:
               USBD_DEBUG_HI_2(DFU_REQ, "DFU state: %2d; USBD_REQ_%s", dfu->core.status.bState, "GETSTATUS");
               DFU_get_status(dfu, tp_in);
               break;

            case DFU_GETSTATE_INTERFACE:
               USBD_DEBUG_HI_2(DFU_REQ, "DFU state: %2d; USBD_REQ_%s", dfu->core.status.bState, "GETSTATE");
               DFU_get_state(dfu, tp_in);
               break;

            case DFU_DETACH_INTERFACE:
               USBD_DEBUG_HI_2(DFU_REQ, "DFU state: %2d; USBD_REQ_%s", dfu->core.status.bState, "DETACH");
               DFU_detach(dfu, tp_in, (uint32_t)(req->wValue));
               break;

            default:
               result = USBD_FALSE;
               DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_ERROR);
               dfu->core.status.bStatus = DFU_BSTATUS_ERR_STALLEDPKT;
               USBD_IOTP_EVENT_Send_Stall(tp_in);
               break;
         }
      }
   }

   USBD_EXIT_FUNC(DFU_REQ);

   return result;
} /* DFU_APP_on_request */
#endif

#if(USBD_FEATURE_PRESENT == DFU_DFU_MODE_SUPPORT)
static USBD_Bool_DT DFU_DFU_on_request(
   USBDC_Params_XT *usbdc,
   uint8_t ep_num,
   uint8_t if_num,
   USBD_REQUEST_Req_DT *req,
   USBD_IOTP_EVENT_Params_XT *tp_in,
   USBD_IOTP_EVENT_Params_XT *tp_out)
{
   USBDC_Interface_Header_XT *if_params;
   DFU_Params_XT *dfu;
   USBD_Bool_DT result = USBD_FALSE;

   USBD_UNUSED_PARAM(ep_num);

   USBD_ENTER_FUNC(DFU_REQ);

   if_params = USBDC_Get_Interface_Params(usbdc, if_num);

   if(USBD_CHECK_PTR(USBDC_Interface_Header_XT, if_params))
   {
      dfu = if_params->vendor.pvoid;
      result = USBD_TRUE;

      if(USBD_CHECK_PTR(DFU_Params_XT, dfu))
      {
         switch(req->request)
         {
            case DFU_GETSTATUS_INTERFACE:
               USBD_DEBUG_HI_2(DFU_REQ, "DFU state: %2d; USBD_REQ_%s", dfu->core.status.bState, "GETSTATUS");
               DFU_get_status(dfu, tp_in);
               break;

            case DFU_GETSTATE_INTERFACE:
               USBD_DEBUG_HI_2(DFU_REQ, "DFU state: %2d; USBD_REQ_%s", dfu->core.status.bState, "GETSTATE");
               DFU_get_state(dfu, tp_in);
               break;

            case DFU_DETACH_INTERFACE:
               USBD_DEBUG_HI_2(DFU_REQ, "DFU state: %2d; USBD_REQ_%s", dfu->core.status.bState, "DETACH");
               DFU_detach(dfu, tp_in, (uint32_t)(req->wValue));
               break;

            case DFU_CLRSTATUS_INTERFACE:
            case DFU_ABORT_INTERFACE:
               USBD_DEBUG_HI_2(DFU_REQ, "DFU state: %2d; USBD_REQ_%s", dfu->core.status.bState, "CLR/ABORT");
               USBD_IOTP_EVENT_Send_Status(tp_in, USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
               if((DFU_BSTATE_DFU_ERROR == dfu->core.status.bState)
                  || (DFU_ABORT_INTERFACE == req->request))
               {
                  DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_IDLE);
                  dfu->core.status.bStatus = DFU_BSTATUS_OK;
                  dfu->core.bwPollTimeout  = 0;
               }
               break;

#if(USBD_FEATURE_PRESENT == DFU_DNLOAD_SUPPORT)
            case DFU_DNLOAD_INTERFACE:
               USBD_DEBUG_HI_2(DFU_REQ, "DFU state: %2d; USBD_REQ_%s", dfu->core.status.bState, "DNLOAD");
               USBD_DEBUG_HI_2(DFU_REQ, "length: %d, block number: %d", req->wLength, req->wIndex);
               DFU_download(dfu, tp_in, tp_out, req->wLength, req->wIndex);
               break;
#endif

#if(USBD_FEATURE_PRESENT == DFU_UPLOAD_SUPPORT)
            case DFU_UPLOAD_INTERFACE:
               USBD_DEBUG_HI_2(DFU_REQ, "DFU state: %2d; USBD_REQ_%s", dfu->core.status.bState, "UPLOAD");
               USBD_DEBUG_HI_2(DFU_REQ, "length: %d, block number: %d", req->wLength, req->wIndex);
               dfu->core.transfer_data_size = req->wLength;
               DFU_upload(dfu, tp_in, &(dfu->core.transfer_data_size), req->wIndex);
               break;
#endif

            default:
               result = USBD_FALSE;
               USBD_IOTP_EVENT_Send_Stall(tp_in);
               break;
         }
      }
   }

   USBD_EXIT_FUNC(DFU_REQ);

   return result;
} /* DFU_DFU_on_request */
#endif



static void DFU_on_event(
   USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
#if(USBD_FEATURE_PRESENT == DFU_MULTIPLE_MEMORIES_SUPPORT)
   const USB_Interface_Desc_DT *if_desc;
#endif
   DFU_Params_XT *dfu;

#if(USBD_FEATURE_PRESENT != DFU_MULTIPLE_MEMORIES_SUPPORT)
   USBD_UNUSED_PARAM(usbd);
#endif

   USBD_ENTER_FUNC(DFU_EVENT);

   dfu = event_params->vendor.pvoid;
   if(USBD_CHECK_PTR(DFU_Params_XT, dfu))
   {
      switch(reason)
      {
         case USBD_EVENT_REASON_CONFIGURED:
            USBD_DEBUG_HI_2(DFU_EVENT, "DFU state: %2d; USBD_EVENT_REASON_%s", dfu->core.status.bState, &"UNCONFIGURED"[2]);
#if((USBD_FEATURE_PRESENT == DFU_APP_MODE_SUPPORT) && (USBD_FEATURE_PRESENT == DFU_DFU_MODE_SUPPORT))
            if(dfu->core.status.bState < DFU_BSTATE_DFU_IDLE)
            {
               USBDC_REQUEST_Interface_Irq_Install(usbdc, DFU_APP_on_request, dfu->core.interface_num);
            }
            else
            {
               USBDC_REQUEST_Interface_Irq_Install(usbdc, DFU_DFU_on_request, dfu->core.interface_num);
            }
#elif(USBD_FEATURE_PRESENT == DFU_APP_MODE_SUPPORT)
            USBDC_REQUEST_Interface_Irq_Install(usbdc, DFU_APP_on_request, dfu->core.interface_num);
#elif(USBD_FEATURE_PRESENT == DFU_DFU_MODE_SUPPORT)
            USBDC_REQUEST_Interface_Irq_Install(usbdc, DFU_DFU_on_request, dfu->core.interface_num);
#endif
            break;

         case USBD_EVENT_REASON_UNCONFIGURED:
            USBD_DEBUG_HI_2(DFU_EVENT, "DFU state: %2d; USBD_EVENT_REASON_%s", dfu->core.status.bState, "UNCONFIGURED");
            dfu->core.bwPollTimeout = 0;
            DFU_STATE_CHANGE(dfu, (dfu->core.status.bState >= DFU_BSTATE_DFU_IDLE) ? DFU_BSTATE_DFU_IDLE : DFU_BSTATE_APP_IDLE);
            dfu->core.status.bStatus= DFU_BSTATUS_OK;
            break;

         case USBD_EVENT_REASON_SOF_RECEIVED:
            if(dfu->core.bwPollTimeout > 0)
            {
               dfu->core.bwPollTimeout--;

               USBD_DEBUG_LO_1(DFU_EVENT, "bwPollTimeout decrease to %d", dfu->core.bwPollTimeout);

               if(0 == dfu->core.bwPollTimeout)
               {
                  DFU_on_timeout(dfu);
               }
            }
            break;

#if(USBD_FEATURE_PRESENT == DFU_MULTIPLE_MEMORIES_SUPPORT)
         case USBD_EVENT_REASON_INTERFACE_SET:
            USBD_DEBUG_HI_2(DFU_EVENT, "DFU state: %2d; USBD_EVENT_REASON_%s", dfu->core.status.bState, "INTERFACE_SET");
            if_desc = USBD_DEV_Get_Interface_Desc(usbd, dfu->core.interface_num);

            if(USBD_CHECK_PTR(const USB_Interface_Desc_DT, if_desc)
               && USBD_CHECK_HANDLER(DFU_Set_Memory_Id_HT, dfu->user_data.handlers.set_memory_id))
            {
               if(if_desc->bAlternateSetting != dfu->core.memory_id)
               {
                  USBD_DEBUG_HI_1(DFU_EVENT, "IF alt (memory ID) set to %d", dfu->core.memory_id);
                  dfu->core.memory_id = if_desc->bAlternateSetting;

                  dfu->core.status.bStatus = USBD_CALL_HANDLER(DFU_Set_Memory_Id_HT, dfu->user_data.handlers.set_memory_id)(
                     dfu, dfu->core.memory_id);
                  if(DFU_BSTATUS_OK != dfu->core.status.bStatus)
                  {
                     DFU_STATE_CHANGE(dfu, DFU_BSTATE_DFU_ERROR);
                  }
               }
            }
            break;
#endif

#if(USBD_FEATURE_PRESENT != DFU_WILL_DETACH_SUPPORT)
         case USBD_EVENT_REASON_RESET:
            USBD_DEBUG_HI_2(DFU_EVENT, "DFU state: %2d; USBD_EVENT_REASON_%s", dfu->core.status.bState, "RESET");
            if(USBD_CHECK_HANDLER(DFU_Event_HT, dfu->user_data.handlers.user_event))
            {
               USBD_CALL_HANDLER(DFU_Event_HT, dfu->user_data.handlers.user_event)(
                  dfu, (dfu->core.status.bState < DFU_BSTATE_DFU_IDLE) ? DFU_USER_EVENT_RESET_GO_TO_DFU : DFU_USER_EVENT_RESET_GO_TO_APP);
            }
            break;
#endif

         default:
         break;
      }
   }

   USBD_EXIT_FUNC(DFU_EVENT);
} /* DFU_on_event */



static void DFU_init(USBDC_Params_XT *usbdc, DFU_Params_XT *dfu, uint8_t interface_num, DFU_bState_ET init_state)
{
   USBDC_Interface_Header_XT *if_params;
   USBD_EVENT_Event_Header_XT *event;

   USBD_ENTER_FUNC(DFU_INIT);

   dfu->core.usbdc         = usbdc;
   dfu->core.status.bState = init_state;
   dfu->core.interface_num = interface_num;

   if_params = USBDC_Get_Interface_Params(usbdc, interface_num);

   event = USBDC_EVENT_Install(
      usbdc,
      DFU_on_event,
      USBD_EVENT_REASON_UNCONFIGURED | USBD_EVENT_REASON_CONFIGURED
#if(USBD_FEATURE_PRESENT != DFU_WILL_DETACH_SUPPORT)
      | USBD_EVENT_REASON_RESET
#endif
      | USBD_EVENT_REASON_SOF_RECEIVED
#if(USBD_FEATURE_PRESENT == DFU_MULTIPLE_MEMORIES_SUPPORT)
      | USBD_EVENT_REASON_INTERFACE_SET
#endif
   );

   if(USBD_CHECK_PTR(USBD_EVENT_Event_Header_XT, event) && USBD_CHECK_PTR(USBDC_Interface_Header_XT, if_params))
   {
      event->vendor.pvoid     = dfu;
      if_params->vendor.pvoid = dfu;
   }

   USBD_EXIT_FUNC(DFU_INIT);
}



void DFU_Init(DFU_Params_XT *dfu)
{
   USBD_ENTER_FUNC(DFU_INIT);

   memset((void*)dfu, 0, sizeof(DFU_Params_XT));

   USBD_EXIT_FUNC(DFU_INIT);
} /* DFU_Init */



#if(USBD_FEATURE_PRESENT == DFU_APP_MODE_SUPPORT)
void DFU_APP_Install_In_Config(USBDC_Params_XT *usbdc, DFU_Params_XT *dfu, uint8_t interface_num)
{
   USBD_ENTER_FUNC(DFU_INIT);

   DFU_init(usbdc, dfu, interface_num, DFU_BSTATE_APP_IDLE);

   USBD_EXIT_FUNC(DFU_INIT);
} /* DFU_APP_Install */
#endif



#if(USBD_FEATURE_PRESENT == DFU_DFU_MODE_SUPPORT)
void DFU_DFU_Install_In_Config(USBDC_Params_XT *usbdc, DFU_Params_XT *dfu, uint8_t *buffer)
{
   USBD_ENTER_FUNC(DFU_INIT);

   dfu->core.transfer_buffer = buffer;
   DFU_init(usbdc, dfu, 0, DFU_BSTATE_DFU_IDLE);

   USBD_EXIT_FUNC(DFU_INIT);
} /* DFU_DFU_Install */
#endif



void DFU_set_bwPollTimeout(DFU_Params_XT *dfu, uint32_t timeout)
{
   if(USBD_CHECK_PTR(DFU_Params_XT, dfu))
   {
      dfu->core.bwPollTimeout = timeout;
   }
}

