/** ---------------------------------------- LICENSE SECTION -------------------------------------------------------------------
 *
 * Copyright (c) 2025 Piotr Wojtowicz
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

#ifndef HID_H_
#include "hid.h"
#endif

#ifndef HID_INTERNAL_H_
#include "hid_internal.h"
#endif



#define HID_TP_VENDOR_CONTAINER_HID       0
#define HID_TP_VENDOR_CONTAINER_REPORT    1



static const uint8_t HID_zero_const = 0;
static const uint8_t HID_one_const = 1;



static void HID_get_report_in_ready (USBD_IOTP_Params_XT *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   USBD_Vendor_Data_XT *tp_vendor_data;
   HID_Report_XT *report;

   USBD_UNUSED_PARAM(dir);
   USBD_UNUSED_PARAM(size);

   USBD_ENTER_FUNC(HID_IO);

   tp_vendor_data = USBD_IOTP_Get_Vendor_Data_Container(tp);

   if(USBD_CHECK_PTR(USBD_Vendor_Data_XT, tp_vendor_data))
   {
      report = tp_vendor_data[HID_TP_VENDOR_CONTAINER_REPORT].pvoid;

      USBD_DEBUG_MID_3(HID_IO, "Report: %d (%s) %s successfully sent", report->report_id, report->name, "IN");

      USBD_CHECK_AND_CALL_HANDLER(HID_On_Report_HT, report->report_events.in)(report, USBD_FALSE);
   }

   USBD_EXIT_FUNC(HID_IO);
} /* HID_get_report_in_ready */



static void HID_get_report_feature_ready (USBD_IOTP_Params_XT *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   USBD_Vendor_Data_XT *tp_vendor_data;
   HID_Report_XT *report;

   USBD_UNUSED_PARAM(dir);
   USBD_UNUSED_PARAM(size);

   USBD_ENTER_FUNC(HID_IO);

   tp_vendor_data = USBD_IOTP_Get_Vendor_Data_Container(tp);

   if(USBD_CHECK_PTR(USBD_Vendor_Data_XT, tp_vendor_data))
   {
      report = tp_vendor_data[HID_TP_VENDOR_CONTAINER_REPORT].pvoid;

      USBD_CHECK_AND_CALL_HANDLER(HID_On_Report_HT, report->report_events.in)(report, USBD_TRUE);
   }

   USBD_EXIT_FUNC(HID_IO);
} /* HID_get_report_feature_ready */



static void HID_set_report_in_ready (USBD_IOTP_Params_XT *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   USBD_Vendor_Data_XT *tp_vendor_data;
   HID_Report_XT *report;

   USBD_UNUSED_PARAM(dir);
   USBD_UNUSED_PARAM(size);

   USBD_ENTER_FUNC(HID_IO);

   tp_vendor_data = USBD_IOTP_Get_Vendor_Data_Container(tp);

   if(USBD_CHECK_PTR(USBD_Vendor_Data_XT, tp_vendor_data))
   {
      report = tp_vendor_data[HID_TP_VENDOR_CONTAINER_REPORT].pvoid;

      if(USBD_CHECK_PTR(uint8_t, report->report_second_buffer.in))
      {
         memcpy(report->report.in, report->report_second_buffer.in, report->report_size.in);
      }

      USBD_DEBUG_MID_3(HID_IO, "Report: %d (%s) received %s", report->report_id, report->name, "IN");

      (void)USBD_IOTP_Send_Status_For_Out_Tp(
         tp,
         USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT),
         USBD_MAKE_INVALID_HANDLER(USBD_IOTP_Callback_HT),
         USBD_MAKE_INVALID_PTR(USBD_Vendor_Data_XT));
   }

   USBD_EXIT_FUNC(HID_IO);
} /* HID_set_report_in_ready */



static void HID_set_report_out_ready (USBD_IOTP_Params_XT *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   USBD_Vendor_Data_XT *tp_vendor_data;
   HID_Report_XT *report;

   USBD_UNUSED_PARAM(dir);
   USBD_UNUSED_PARAM(size);

   USBD_ENTER_FUNC(HID_IO);

   tp_vendor_data = USBD_IOTP_Get_Vendor_Data_Container(tp);

   if(USBD_CHECK_PTR(USBD_Vendor_Data_XT, tp_vendor_data))
   {
      report = tp_vendor_data[HID_TP_VENDOR_CONTAINER_REPORT].pvoid;

      if(USBD_CHECK_PTR(uint8_t, report->report_second_buffer.out))
      {
         memcpy(report->report.out, report->report_second_buffer.out, report->report_size.out);
      }

      USBD_DEBUG_MID_3(HID_IO, "Report: %d (%s) received %s", report->report_id, report->name, "OUT");

      USBD_ATOMIC_BOOL_SET(report->out_has_been_changed, USBD_TRUE);

      (void)USBD_IOTP_Send_Status_For_Out_Tp(
         tp,
         USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT),
         USBD_MAKE_INVALID_HANDLER(USBD_IOTP_Callback_HT),
         USBD_MAKE_INVALID_PTR(USBD_Vendor_Data_XT));

      USBD_CHECK_AND_CALL_HANDLER(HID_On_Report_HT, report->report_events.out)(report, USBD_FALSE);
   }

   USBD_EXIT_FUNC(HID_IO);
} /* HID_set_report_out_ready */



static void HID_set_report_feature_ready (USBD_IOTP_Params_XT *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   USBD_Vendor_Data_XT *tp_vendor_data;
   HID_Report_XT *report;

   USBD_UNUSED_PARAM(dir);
   USBD_UNUSED_PARAM(size);

   USBD_ENTER_FUNC(HID_IO);

   tp_vendor_data = USBD_IOTP_Get_Vendor_Data_Container(tp);

   if(USBD_CHECK_PTR(USBD_Vendor_Data_XT, tp_vendor_data))
   {
      report = tp_vendor_data[HID_TP_VENDOR_CONTAINER_REPORT].pvoid;

      if(USBD_CHECK_PTR(uint8_t, report->report_second_buffer.feature))
      {
         memcpy(report->report.feature, report->report_second_buffer.feature, report->report_size.feature);
      }

      USBD_DEBUG_MID_3(HID_IO, "Report: %d (%s) received %s", report->report_id, report->name, "FEATURE");

      (void)USBD_IOTP_Send_Status_For_Out_Tp(
         tp,
         USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT),
         USBD_MAKE_INVALID_HANDLER(USBD_IOTP_Callback_HT),
         USBD_MAKE_INVALID_PTR(USBD_Vendor_Data_XT));

      USBD_CHECK_AND_CALL_HANDLER(HID_On_Report_HT, report->report_events.out)(report, USBD_TRUE);
   }

   USBD_EXIT_FUNC(HID_IO);
} /* HID_set_report_feature_ready */



static void HID_on_in_report_done(HID_Report_XT *report, USBD_Bool_DT is_feature)
{
   if(USBD_CHECK_PTR(HID_Report_XT, report) && USBD_CHECK_PTR(uint8_t, report->report.in) && USBD_BOOL_IS_FALSE(is_feature))
   {
      USBD_DEBUG_LO_2(HID_IO, "Report: %d (%s) cleared", report->report_id, report->name);
      memset(report->report.in, 0, report->report_size.in);
   }
} /* HID_on_in_report_done */



static USBD_Bool_DT HID_send_report(
   USBD_IOTP_Params_XT  *tp_in,
   USBD_IOTP_Callback_HT ready,
   HID_Report_XT              *report,
   const char                 *report_type,
   uint8_t                    *buffer,
   uint8_t                    *second_buffer,
   USBD_IO_Inout_Data_Size_DT *size_left,
   USBD_IO_Inout_Data_Size_DT  size,
   USBD_Bool_DT                update_markers)
{
   Buff_Readable_Vector_XT vector[2];
   uint8_t *data;
   USBD_Bool_DT result = USBD_FALSE;

#ifndef USBD_USE_IOCMD
   USBD_UNUSED_PARAM(report_type);
#endif

   USBD_ENTER_FUNC(HID_IO);

   if(USBD_CHECK_PTR(uint8_t, buffer))
   {
      if(USBD_CHECK_PTR(uint8_t, second_buffer))
      {
         memcpy(second_buffer, buffer, size);
         data = second_buffer;
      }
      else
      {
         data = buffer;
      }

      if(USBD_CHECK_HANDLER(USBD_IOTP_Callback_HT, ready))
      {
         USBD_IOTP_Set_Ready_Handler(tp_in, ready);
      }

      USBD_LOG_DATA_DEBUG_HI_3(HID_IO, data, size, "Report: %d (%s) send %s", report->report_id, report->name, report_type);

      if(0 != report->report_id)
      {
         vector[0].data = &(report->report_id);
         vector[0].size = sizeof(report->report_id);
      }
      else
      {
         vector[0].data = USBD_MAKE_INVALID_PTR(uint8_t);
         vector[0].size = 0;
      }
      vector[1].data = data;
      vector[1].size = size;

      result = USBD_IOTP_Send_From_Vector(tp_in, vector, Num_Elems(vector), size + vector[0].size, size_left);

      if(USBD_BOOL_IS_TRUE(update_markers))
      {
         USBD_ATOMIC_BOOL_SET(report->is_sending, USBD_TRUE);
         USBD_ATOMIC_BOOL_SET(report->in_has_been_changed, USBD_FALSE);
      }

      if(((-1) == *size_left) && USBD_CHECK_HANDLER(USBD_IOTP_Callback_HT, ready))
      {
         ready(tp_in, USB_EP_DIRECTION_IN, size);
      }
   }
   else
   {
      USBD_IOTP_Send_Stall(tp_in);
   }

   USBD_EXIT_FUNC(HID_IO);

   return result;
} /* HID_send_report */



static void HID_receive_report(
   USBD_IOTP_Params_XT  *tp_in,
   USBD_IOTP_Params_XT  *tp_out,
   USBD_IOTP_Callback_HT ready,
   uint8_t                    *buffer,
   uint8_t                    *second_buffer,
   USBD_IO_Inout_Data_Size_DT  size)
{
   uint8_t *data;

   if(USBD_CHECK_PTR(uint8_t, buffer))
   {
      if(USBD_CHECK_PTR(uint8_t, second_buffer))
      {
         data = second_buffer;
      }
      else
      {
         data = buffer;
      }
      USBD_IOTP_Set_Ready_Handler(tp_out, ready);

      USBD_IOTP_Recv_And_Ready(tp_out, data, size, USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
   }
   else
   {
      USBD_IOTP_Send_Stall(tp_in);
   }
} /* HID_receive_report */



static USBD_Bool_DT HID_on_request (
   USBDC_Params_XT *usbdc,
   uint8_t ep_num,
   uint8_t if_num,
   USBD_REQUEST_Req_DT *req,
   USBD_IOTP_Params_XT *tp_in,
   USBD_IOTP_Params_XT *tp_out)
{
   USBDC_Interface_Header_XT *hid_if_params = USBDC_Get_Interface_Params(usbdc, if_num);
   HID_Params_XT             *hid = (HID_Params_XT *)(hid_if_params->vendor.pvoid);
   HID_Report_XT             *report;
   USBD_Vendor_Data_XT       *tp_vendor_data;
   USBD_IO_Inout_Data_Size_DT size;
   USBD_IO_Inout_Data_Size_DT size_left;
   uint8_t                    report_id;
   uint8_t                    idle_rate;
   USBD_Bool_DT               result = USBD_FALSE;

   USBD_UNUSED_PARAM(ep_num);

   USBD_ENTER_FUNC(HID_REQ);

   if(USBD_CHECK_PTR(HID_Params_XT, hid))
   {
      report_id = (uint8_t)(req->wValue & 0xFF);

      // Standard Request
      if(0 == (req->bmRequestType & USBD_BMREQUESTTYPE_CLASS))
      {
         result = USBD_TRUE;

         switch (req->request)
         {
            // -------------------------------
            case HID_GET_DESCRIPTOR_INTERFACE:
            // -------------------------------
               if(req->wValue == (USBD_DESC_TYPE_REPORT * 256))
               {
                  USBD_IOTP_Send(
                     tp_in,
                     hid->report_params.report_descriptor,
                     (req->wLength >= hid->report_params.report_descriptor_size) ?
                        (USBD_IO_Inout_Data_Size_DT)(hid->report_params.report_descriptor_size) : (USBD_IO_Inout_Data_Size_DT)(req->wLength),
                     USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
               }
               // TODO: PHYSICAL DESCRIPTOR
               break;

            // -------------------------------
            case HID_SET_DESCRIPTOR_INTERFACE:
            // -------------------------------
               USBD_IOTP_Send_Stall(tp_in);
               break;


            default:
               result = USBD_FALSE;
               break;
         }
      }
      // Class Request
      else if(report_id <= hid->report_params.num_reports)
      {
         if(report_id > 0)
         {
            report = &(hid->report_params.reports_table[report_id - 1]);
         }
         else
         {
            report = &(hid->report_params.reports_table[report_id]);
         }

         result = USBD_TRUE;

         switch (req->request)
         {
            // ---------------------------
            case HID_GET_REPORT_INTERFACE:
            // ---------------------------
               tp_vendor_data = USBD_IOTP_Get_Vendor_Data_Container(tp_in);

               tp_vendor_data[HID_TP_VENDOR_CONTAINER_HID].pvoid = hid;
               tp_vendor_data[HID_TP_VENDOR_CONTAINER_REPORT].pvoid = report;

               switch (req->wValue & 0xFF00)
               {
                  case USBD_REPORT_REQ_INPUT:
                     size = (req->wLength >= (report->report_size.in)) ?
                        (USBD_IO_Inout_Data_Size_DT)(report->report_size.in) : (USBD_IO_Inout_Data_Size_DT)(req->wLength);

                     (void)HID_send_report(
                        tp_in,
                        HID_get_report_in_ready,
                        report,
                        "IN",
                        report->report.in,
                        report->report_second_buffer.in,
                        &size_left,
                        size,
                        USBD_TRUE);
                     break;

                  case USBD_REPORT_REQ_OUTPUT:
                     size = (req->wLength >= (report->report_size.out)) ?
                        (USBD_IO_Inout_Data_Size_DT)(report->report_size.out) : (USBD_IO_Inout_Data_Size_DT)(req->wLength);

                     (void)HID_send_report(
                        tp_in,
                        USBD_MAKE_INVALID_HANDLER(USBD_IOTP_Callback_HT),
                        report,
                        "OUT",
                        report->report.out,
                        report->report_second_buffer.out,
                        &size_left,
                        size,
                        USBD_FALSE);
                     break;

                  case USBD_REPORT_REQ_FEATURE:
                     size = (req->wLength >= (report->report_size.feature)) ?
                        (USBD_IO_Inout_Data_Size_DT)(report->report_size.feature) : (USBD_IO_Inout_Data_Size_DT)(req->wLength);

                     (void)HID_send_report(
                        tp_in,
                        HID_get_report_feature_ready,
                        report,
                        "FEATURE",
                        report->report.feature,
                        report->report_second_buffer.feature,
                        &size_left,
                        size,
                        USBD_FALSE);
                     break;

                  default:
                     result = USBD_FALSE;
                     break;
               }
               break;

            // ---------------------------
            case HID_SET_REPORT_INTERFACE:
            // ---------------------------
               tp_vendor_data = USBD_IOTP_Get_Vendor_Data_Container(tp_out);

               tp_vendor_data[HID_TP_VENDOR_CONTAINER_HID].pvoid = hid;
               tp_vendor_data[HID_TP_VENDOR_CONTAINER_REPORT].pvoid = report;

               switch (req->wValue & 0xFF00)
               {
                  case USBD_REPORT_REQ_INPUT:
                     HID_receive_report(
                        tp_in,
                        tp_out,
                        HID_set_report_in_ready,
                        report->report.in,
                        report->report_second_buffer.in,
                        report->report_size.in);
                     break;

                  case USBD_REPORT_REQ_OUTPUT:
                     HID_receive_report(
                        tp_in,
                        tp_out,
                        HID_set_report_out_ready,
                        report->report.out,
                        report->report_second_buffer.out,
                        report->report_size.out);
                     break;

                  case USBD_REPORT_REQ_FEATURE:
                     HID_receive_report(
                        tp_in,
                        tp_out,
                        HID_set_report_feature_ready,
                        report->report.feature,
                        report->report_second_buffer.feature,
                        report->report_size.feature);
                     break;

                  default:
                     result = USBD_FALSE;
                     break;
               }
               break;

            // -------------------------
            case HID_GET_IDLE_INTERFACE:
            // -------------------------
               USBD_IOTP_Send(tp_in, &(report->idle_rate), 1, USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
               break;

            // -------------------------
            case HID_SET_IDLE_INTERFACE:
            // -------------------------
               idle_rate = (uint8_t)((req->wValue >> 8) & 0xFF);

               if(0 != report_id)
               {
                  report->idle_rate = idle_rate;
               }
               else
               {
                  for(; report_id < hid->report_params.num_reports; ++report_id)
                  {
                     hid->report_params.reports_table[report_id].idle_rate = idle_rate;
                  }
               }
               USBD_IOTP_Send_Status(tp_in, USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
               break;

            // -----------------------------
            case HID_GET_PROTOCOL_INTERFACE:
            // -----------------------------
               if(hid->report_params.boot_protocol_value)
               {
                  USBD_IOTP_Send(tp_in, (uint8_t*) &HID_one_const, 1, USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
               }
               else
               {
                  USBD_IOTP_Send(tp_in, (uint8_t*) &HID_zero_const, 1, USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
               }
               break;

            // -----------------------------
            case HID_SET_PROTOCOL_INTERFACE:
            // ----------------------------
               if(req->wValue)
               {
                  hid->report_params.boot_protocol_value = hid->hw.bInterfaceProtocol;
               }
               else
               {
                  hid->report_params.boot_protocol_value = 0;
               }
               USBD_IOTP_Send_Status(tp_in, USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT));
               break;

            default:
               USBD_IOTP_Send_Stall(tp_in);
               result = USBD_FALSE;
               break;
         }
      }
   }

   USBD_EXIT_FUNC(HID_REQ);

   return result;
} /* HID_on_request */



static void HID_process_in_reports(
   HID_Params_XT *hid, USBD_IOTP_Params_XT *tp_in, uint8_t ending_report, uint8_t first_report)
{
   USBD_Vendor_Data_XT       *tp_vendor_data;
   HID_Report_XT             *report;
   USBD_IO_Inout_Data_Size_DT size_left;
   uint8_t                    reports_counter = first_report;

   USBD_ENTER_FUNC(HID_IO);

   if(ending_report < hid->report_params.num_reports)
   {
      USBD_ATOMIC_BOOL_SET(hid->report_params.reports_table[ending_report].is_sending, USBD_FALSE);
      reports_counter++;
   }

   tp_vendor_data = USBD_IOTP_Get_Vendor_Data_Container(tp_in);

   if(USBD_CHECK_PTR(USBD_Vendor_Data_XT, tp_vendor_data))
   {
      tp_vendor_data[HID_TP_VENDOR_CONTAINER_HID].pvoid = hid;

      for(; reports_counter < hid->report_params.num_reports; ++reports_counter)
      {
         report = &(hid->report_params.reports_table[reports_counter]);

         if(USBD_CHECK_PTR(uint8_t, report->report.in))
         {
            /* when idle_rate is 0 then report will be sent only on change, otherwise send it periodically */
            if( ((0 != report->idle_rate)
               && ((hid->report_params.sof_tick - report->last_report_time) >= (((uint32_t)report->idle_rate) * 4)))
               ||
               ((0 == report->idle_rate)
               && USBD_ATOMIC_BOOL_IS_TRUE(report->in_has_been_changed)) )
            {
               if(USBD_BOOL_IS_FALSE(USBD_IOTP_Is_Transfer_Active(tp_in)))
               {
                  tp_vendor_data[HID_TP_VENDOR_CONTAINER_REPORT].pvoid = report;
                  if(USBD_BOOL_IS_TRUE(HID_send_report(
                     tp_in,
                     HID_get_report_in_ready,
                     report,
                     "IN",
                     report->report.in,
                     report->report_second_buffer.in,
                     &size_left,
                     (USBD_IO_Inout_Data_Size_DT)(report->report_size.in),
                     USBD_TRUE)))
                  {
                     report->last_report_time = hid->report_params.sof_tick;
                     hid->report_params.report_in_use = reports_counter;
                     if(size_left > 0)
                     {
                        return;
                     }
                  }
               }
            }
         }
      }
   }
   hid->report_params.report_in_use = 0;

   USBD_EXIT_FUNC(HID_IO);
} /* HID_process_in_reports */



static void HID_in_ready (USBD_IOTP_Params_XT *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   HID_Params_XT *hid;

   USBD_UNUSED_PARAM(dir);
   USBD_UNUSED_PARAM(size);

   USBD_ENTER_FUNC(HID_IO);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      hid = USBD_IOTP_Get_Vendor_Data_Container(tp)[HID_TP_VENDOR_CONTAINER_HID].pvoid;

      HID_process_in_reports(hid, tp, hid->report_params.num_reports, hid->report_params.report_in_use);
   }

   USBD_EXIT_FUNC(HID_IO);
} /* HID_in_ready */



static void HID_in_abort (USBD_IOTP_Params_XT *tp, USB_EP_Direction_ET dir, USBD_IO_Inout_Data_Size_DT size)
{
   HID_Params_XT *hid;

   USBD_UNUSED_PARAM(dir);
   USBD_UNUSED_PARAM(size);

   USBD_ENTER_FUNC(HID_IO);

   if(USBD_CHECK_PTR(USBD_IOTP_Params_XT, tp))
   {
      hid = USBD_IOTP_Get_Vendor_Data_Container(tp)[HID_TP_VENDOR_CONTAINER_HID].pvoid;

      USBD_ATOMIC_BOOL_SET(hid->report_params.reports_table[hid->report_params.report_in_use].is_sending, USBD_FALSE);
      hid->report_params.report_in_use = 0;
   }

   USBD_EXIT_FUNC(HID_IO);
} /* HID_in_abort */



static void HID_on_event(
   USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_EVENT_Event_Header_XT *event_params, USBD_EVENT_Reason_ET reason)
{
   HID_Params_XT       *hid   = event_params->vendor.pvoid;
   USBD_IOTP_Params_XT *tp_in = &(hid->hw.in_iotp);
   uint8_t              reports_counter;

   USBD_ENTER_FUNC(HID_EVENT);

   if(reason == USBD_EVENT_REASON_SOF_RECEIVED)
   {
      hid->report_params.sof_tick++;

      HID_process_in_reports(hid, tp_in, hid->report_params.num_reports, 0);
   }
   else if(reason == USBD_EVENT_REASON_CONFIGURED)
   {
      USBD_IOTP_Init(usbd, usbdc, hid->hw.ep_in, USB_EP_DIRECTION_IN, tp_in);
      (void)USBD_IOTP_Install(tp_in);
      USBD_IOTP_Set_Ready_Handler(tp_in, HID_in_ready);
      USBD_IOTP_Set_Abort_Handler(tp_in, HID_in_abort);
      USBD_IOTP_Get_Vendor_Data_Container(tp_in)[HID_TP_VENDOR_CONTAINER_HID].pvoid = hid;
      hid->report_params.sof_tick = 0;
      for(reports_counter = 0; reports_counter < hid->report_params.num_reports; reports_counter++)
      {
         hid->report_params.reports_table[reports_counter].last_report_time = 0;
      }
      USBDC_REQUEST_Interface_Irq_Install(usbdc, HID_on_request, hid->hw.if_num);
   }
   else if(reason == USBD_EVENT_REASON_UNCONFIGURED)
   {
      hid->report_params.report_in_use = 0;
   }

   USBD_EXIT_FUNC(HID_EVENT);
} /* HID_on_event */



void HID_Reports_Tab_Init(HID_Report_XT *reports_tab, uint8_t reports_tab_num_elems)
{
   uint8_t cntr;

   if(USBD_CHECK_PTR(HID_Report_XT, reports_tab))
   {
      memset(reports_tab, 0, sizeof(HID_Report_XT) * reports_tab_num_elems);

      for(cntr = 0; cntr < reports_tab_num_elems; ++cntr)
      {
         reports_tab[cntr].report_id = cntr;
         reports_tab[cntr].name = "";
      }
   }
} /* HID_Reports_Tab_Init */

void HID_Report_Set_Name(HID_Report_XT *reports_tab, uint8_t report_id, char *name)
{
   if(USBD_CHECK_PTR(HID_Report_XT, reports_tab) && USBD_CHECK_PTR(char, name))
   {
      reports_tab[report_id].name = name;
   }
} /* HID_Report_Set_Name */

void HID_Report_Set_In(HID_Report_XT *reports_tab, uint8_t report_id, void *report, void *report_second_buf, uint8_t size)
{
   if(USBD_CHECK_PTR(HID_Report_XT, reports_tab))
   {
      reports_tab[report_id].report.in                = report;
      reports_tab[report_id].report_second_buffer.in  = report_second_buf;
      reports_tab[report_id].report_size.in           = size;
   }
} /* HID_Report_Set_In */

void HID_Report_Set_Out(HID_Report_XT *reports_tab, uint8_t report_id, void *report, void *report_second_buf, uint8_t size)
{
   if(USBD_CHECK_PTR(HID_Report_XT, reports_tab))
   {
      reports_tab[report_id].report.out               = report;
      reports_tab[report_id].report_second_buffer.out = report_second_buf;
      reports_tab[report_id].report_size.out          = size;
   }
} /* HID_Report_Set_Out */

void HID_Report_Set_Feature(HID_Report_XT *reports_tab, uint8_t report_id, void *report, void *report_second_buf, uint8_t size)
{
   if(USBD_CHECK_PTR(HID_Report_XT, reports_tab))
   {
      reports_tab[report_id].report.feature              = report;
      reports_tab[report_id].report_second_buffer.feature= report_second_buf;
      reports_tab[report_id].report_size.feature         = size;
   }
} /* HID_Report_Set_Feature */

void HID_Report_Set_In_Event(HID_Params_XT *hid, uint8_t report_id, HID_On_Report_HT event)
{
   if(USBD_CHECK_PTR(HID_Params_XT, hid) && USBD_CHECK_PTR(HID_Report_XT, hid->report_params.reports_table)
      && (report_id < hid->report_params.num_reports))
   {
      hid->report_params.reports_table[report_id].report_events.in   = event;
   }
} /* HID_Report_Set_In_Event */

void HID_Report_Set_Out_Event(HID_Params_XT *hid, uint8_t report_id, HID_On_Report_HT event)
{
   if(USBD_CHECK_PTR(HID_Params_XT, hid) && USBD_CHECK_PTR(HID_Report_XT, hid->report_params.reports_table)
      && (report_id < hid->report_params.num_reports))
   {
      hid->report_params.reports_table[report_id].report_events.out  = event;
   }
} /* HID_Report_Set_Out_Event */

void HID_Report_Clear_In_After_Each_Update(HID_Params_XT *hid, uint8_t report_id)
{
   HID_Report_Set_In_Event(hid, report_id, HID_on_in_report_done);
} /* HID_Report_Clear_In_After_Each_Update */



void HID_Init(
   HID_Params_XT *hid,
   const uint8_t *report_descriptor,
   uint16_t       report_descriptor_size,
   HID_Report_XT *reports_tab,
   uint8_t        num_reports)
{
   USBD_ENTER_FUNC(HID_INIT);

   if(USBD_CHECK_PTR(HID_Params_XT, hid))
   {
      memset((void*)hid, 0, sizeof(HID_Params_XT));

      hid->report_params.report_descriptor       = report_descriptor;
      hid->report_params.report_descriptor_size  = report_descriptor_size;
      hid->report_params.reports_table           = reports_tab;
      hid->report_params.num_reports             = num_reports;
   }

   USBD_EXIT_FUNC(HID_INIT);
} /* HID_Init */



void HID_Install_In_Config(USBDC_Params_XT *usbdc, HID_Params_XT *hid, uint8_t hid_if_num)
{
   USB_Endpoint_Desc_DT       *ep;
   USBD_EVENT_Event_Header_XT *event;
   USBDC_Interface_Header_XT  *hid_if_params;
   const uint8_t              *desc;
   uint16_t desc_size;
   uint16_t hid_if_pos;
   uint16_t next_if_pos;
   uint16_t first_ep_pos;
   uint16_t second_ep_pos;
   uint8_t  ep_in  = 0;
   uint8_t  ep_out = 0;

   USBD_ENTER_FUNC(HID_INIT);

   if(USBD_CHECK_PTR(USBDC_Params_XT, usbdc) && USBD_CHECK_PTR(HID_Params_XT, hid)
      && USBD_CHECK_PTR(const uint8_t, hid->report_params.report_descriptor)
      && USBD_CHECK_PTR(HID_Report_XT, hid->report_params.reports_table))
   {
      desc = USBDC_Get_Config_Desc(usbdc);

      if(USBD_CHECK_PTR(const uint8_t, desc))
      {
         desc_size = USB_CDP_Get_Config_Desc_Total_Length(desc);

         /* find HID interface in configuration descriptor */
         hid_if_pos = USB_CDP_Find_Interface(desc, desc_size, 0, hid_if_num, 0);
         /* find next interface in configuration descriptor */
         next_if_pos = USB_CDP_Find_Next_Interface(desc, desc_size, hid_if_pos);

         if((0 != hid_if_pos) && (hid_if_pos < desc_size))
         {
            /* find IN and OUT endpoints - we need to save its numbers, we need at least IN endpoint */
            first_ep_pos  = USB_CDP_Find_Next_Endpoint(desc, desc_size, hid_if_pos);
            second_ep_pos = USB_CDP_Find_Next_Endpoint(desc, desc_size, first_ep_pos);

            if((0 != first_ep_pos) && (first_ep_pos < next_if_pos))
            {
               ep = (USB_Endpoint_Desc_DT*)(&desc[first_ep_pos]);

               if(USB_EP_DESC_DIR_IN == (ep->bEndpointAddress & USB_EP_DESC_DIR_MASK))
               {
                  ep_in = ep->bEndpointAddress & USB_EP_DESC_ADDR_MASK;
               }
               else
               {
                  ep_out = ep->bEndpointAddress & USB_EP_DESC_ADDR_MASK;
               }
            }

            if((0 != second_ep_pos) && (second_ep_pos < next_if_pos))
            {
               ep = (USB_Endpoint_Desc_DT*)(&desc[second_ep_pos]);

               if(USB_EP_DESC_DIR_IN == (ep->bEndpointAddress & USB_EP_DESC_DIR_MASK))
               {
                  ep_in = ep->bEndpointAddress & USB_EP_DESC_ADDR_MASK;
               }
               else
               {
                  ep_out = ep->bEndpointAddress & USB_EP_DESC_ADDR_MASK;
               }
            }

            hid_if_params = USBDC_Get_Interface_Params(usbdc, hid_if_num);

            if((0 != ep_in) && USBD_CHECK_PTR(USBDC_Interface_Header_XT, hid_if_params))
            {
               event = USBDC_EVENT_Install(
                  usbdc,
                  HID_on_event,
                  USBD_EVENT_REASON_UNCONFIGURED | USBD_EVENT_REASON_CONFIGURED | USBD_EVENT_REASON_SOF_RECEIVED);

               if(USBD_CHECK_PTR(USBD_EVENT_Event_Header_XT, event))
               {
                  event->vendor.pvoid           = hid;
                  hid_if_params->vendor.pvoid   = hid;
                  hid->hw.ep_in                 = ep_in;
                  hid->hw.ep_out                = ep_out;
                  hid->hw.if_num                = hid_if_num;

                  USBD_DEBUG_HI_3(HID_INIT, "install HID for IF: %d/ EP_IN: %d, EP_OUT: %d",
                     hid_if_num, hid->hw.ep_in, hid->hw.ep_out);
               }
            }
         }
      }
   }

   USBD_EXIT_FUNC(HID_INIT);
} /* HID_Install_In_Config */

void HID_Mark_Report_In_Changed(HID_Params_XT *hid, uint8_t report_id)
{
   if(USBD_CHECK_PTR(HID_Params_XT, hid) && USBD_CHECK_PTR(HID_Report_XT, hid->report_params.reports_table))
   {
      USBD_DEBUG_MID_3(HID_IO, "Report: %d (%s) %s changed",
         report_id, hid->report_params.reports_table[report_id].name, "IN");

      USBD_ATOMIC_BOOL_SET(hid->report_params.reports_table[report_id].in_has_been_changed, USBD_TRUE);
   }
} /* HID_Mark_Report_In_Changed */

void HID_Clear_Report_Out_Changed(HID_Params_XT *hid, uint8_t report_id)
{
   if(USBD_CHECK_PTR(HID_Params_XT, hid) && USBD_CHECK_PTR(HID_Report_XT, hid->report_params.reports_table))
   {
      USBD_ATOMIC_BOOL_SET(hid->report_params.reports_table[report_id].out_has_been_changed, USBD_FALSE);
   }
} /* HID_Clear_Report_Out_Changed */

