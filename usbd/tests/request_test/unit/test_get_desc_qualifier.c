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

#include "test_get_desc_qualifier.h"
#include "iotp_stubs.h"
#include "cfg.h"

#ifndef USBD_REQUEST_INTERNAL_H_
#include "usbd_request_internal.h"
#endif

#ifdef USBD_DEV_PRESENT
#ifndef USBD_DEV_INTERNAL_H_
#include "usbd_dev_internal.h"
#endif
#endif

#ifdef USBD_EVENT_PRESENT
#ifndef USBD_EVENT_INTERNAL_H_
#include "usbd_event_internal.h"
#endif
#endif

#ifdef USBD_IO_PRESENT
#ifndef USBD_IO_INTERNAL_H_
#include "usbd_io_internal.h"
#endif
#endif

#ifdef USBDC_PRESENT
#ifndef USBDC_INTERNAL_H_
#include "usbdc_internal.h"
#endif
#endif

#ifndef USBD_IOTP_EVENT_INTERNAL_H_
#include "usbd_iotp_event_internal.h"
#endif

#include "main_get_exe.h"


#define FUNC_ENTER_NOTIF      (false)


#define TEST_FULL_SPEED_MPS    16
#define TEST_HIGH_SPEED_MPS    64


#define QUAL_DESC_SIZE         (sizeof(USB_Device_Qualifier_Desc_DT) / sizeof(uint8_t))


static void tsend(USBD_Params_XT *usbd, uint8_t ep_num, const void *data, USBD_IO_Inout_Data_Size_DT size);
static void tstatus(USBD_Params_XT *usbd, uint8_t ep_num);
static void tstall(USBD_Params_XT *usbd, uint8_t ep_num);
static void tabort(USBD_Params_XT *usbd, uint8_t ep_num);
static void tset_ready(void *tp_params, USBD_IOTP_EVENT_Callback_HT ready);
static USBD_Bool_DT tset_port(USBD_Params_XT *usbd, uint8_t ep_num, USBD_REQUEST_Req_DT *req);
static USBD_Bool_DT test_perform_interface_irq(
   USBDC_Params_XT *usbdc, uint8_t ep_num, uint8_t if_num, USBD_REQUEST_Req_DT *req, USBD_IOTP_EVENT_Params_XT *tp_in, USBD_IOTP_EVENT_Params_XT *tp_out);

static iotp_stubs_t stubs =
{
   tsend,
   tstatus,
   tstall,
   tabort,
   tset_ready,
   USBD_FALSE,
   USBD_FALSE,
   USBD_FALSE,
   USBD_FALSE,
   USBD_FALSE
};

static USBD_REQUEST_Port_Callbacks_XT port_cbks;

static uint8_t sequence_cntr;
static uint8_t called_send;
static uint8_t called_status;
static uint8_t called_stall;
static uint8_t called_abort;
static uint8_t called_set_ready;
static uint8_t called_perform_interface_irq;

static uint8_t test_param;
static uint8_t test_present_mask;
static uint16_t test_length;
static USBD_DEV_Speed_ET test_supported_speed;
static USBD_DEV_Speed_ET test_current_speed;
static USBD_IO_Inout_Data_Size_DT send_size;

static uint8_t mps;
static uint8_t called_set_port;
static USBD_Bool_DT test_port_result;
static USBD_DEV_States_ET state_before;
static USBD_DEV_States_ET state_after;

static uint8_t working_buf[IOCMD_WORKING_BUF_RECOMMENDED_SIZE];

#define REPORT_ERROR()\
   IOCMD_Proc_Buffered_Logs(true, main_get_exe(), working_buf, sizeof(working_buf));\
   printf("file: %s\nline: %d\n", __FILE__, __LINE__);\
   report_error()

static void report_error(void)
{
   char ec;

   printf("some error occured!\n\rpress any key to continue... ");
   ec = getc(stdin);
}


static void tsend(USBD_Params_XT *usbd, uint8_t ep_num, const void *data, USBD_IO_Inout_Data_Size_DT size)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   called_send = sequence_cntr;
   sequence_cntr++;

   send_size = size;

   if(!USBD_COMPARE_PTRS(USB_Device_Qualifier_Desc_DT, data, USB_Device_Qualifier_Desc_DT, &(usbd->request.core.data.req_data.qual)))
   {
      printf("invalid data sent!!!\n\r");
      REPORT_ERROR();
   }
   else
   {
      mps = ((USB_Device_Qualifier_Desc_DT*)(data))->bMaxPacketSize0;
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void tstatus(USBD_Params_XT *usbd, uint8_t ep_num)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   called_status = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void tstall(USBD_Params_XT *usbd, uint8_t ep_num)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   called_stall = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void tabort(USBD_Params_XT *usbd, uint8_t ep_num)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   called_abort = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static void tset_ready(void *tp_params, USBD_IOTP_EVENT_Callback_HT ready)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   called_set_ready = sequence_cntr;
   sequence_cntr++;

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}

static USBD_Bool_DT tset_port(USBD_Params_XT *usbd, uint8_t ep_num, USBD_REQUEST_Req_DT *req)
{
   uint16_t request;

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   called_set_port = sequence_cntr;
   sequence_cntr++;

   request = ((uint16_t)(req->bRequest)) & 0x00FF;
   request *= 256;
   request += req->bmRequestType;

   if(!(USBD_STD_REQ_GET_DESCRIPTOR_DEVICE == request))
   {
      printf("iinvalid request called: 0x%X !!!\n\r", request);
      REPORT_ERROR();
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);

   return test_port_result;
}

static USBD_Bool_DT test_perform_interface_irq(
   USBDC_Params_XT *usbdc, uint8_t ep_num, uint8_t if_num, USBD_REQUEST_Req_DT *req, USBD_IOTP_EVENT_Params_XT *tp_in, USBD_IOTP_EVENT_Params_XT *tp_out)
{
   uint16_t request;

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   request = ((uint16_t)(req->bRequest)) & 0x00FF;
   request *= 256;
   request += req->bmRequestType;

   called_perform_interface_irq = sequence_cntr;
   sequence_cntr++;

   if(!(USBD_STD_REQ_GET_DESCRIPTOR_DEVICE == request))
   {
      printf("invalid request called: 0x%X !!!\n\r", request);
      REPORT_ERROR();
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);

   return true;
}


static void print_usbd_state(USBD_Params_XT *usbd)
{
   USBD_DEV_States_ET state = USBD_DEV_Get_State(usbd);

   printf("usbd state: ");

   if(USBD_DEV_STATE_POWERED == (USBD_DEV_STATE_POWERED & state))
   {
      printf(" | USBD_DEV_STATE_POWERED");
   }
   if(USBD_DEV_STATE_DEFAULT == (USBD_DEV_STATE_DEFAULT & state))
   {
      printf(" | USBD_DEV_STATE_DEFAULT");
   }
   if(USBD_DEV_STATE_ADDRESSED == (USBD_DEV_STATE_ADDRESSED & state))
   {
      printf(" | USBD_DEV_STATE_ADDRESSED");
   }
   if(USBD_DEV_STATE_CONFIGURED == (USBD_DEV_STATE_CONFIGURED & state))
   {
      printf(" | USBD_DEV_STATE_CONFIGURED");
   }
   if(USBD_DEV_STATE_SUSPENDED == (USBD_DEV_STATE_SUSPENDED & state))
   {
      printf(" | USBD_DEV_STATE_SUSPENDED");
   }
      printf("\n\r");
}



static void print_states(USBD_Params_XT *usbd)
{
   int i;
   for(i = 0; i < USBD_MAX_NUM_ENDPOINTS; i++)
   {
      printf("ep%d->if:: IN: %d(0x%X), OUT: %d(0x%X)\n\r",
         i,
         USBD_DEV_Get_EP_Interface_Num(usbd, i, USB_EP_DIRECTION_IN),
         USBD_DEV_Get_EP_Interface_Num(usbd, i, USB_EP_DIRECTION_IN),
         USBD_DEV_Get_EP_Interface_Num(usbd, i, USB_EP_DIRECTION_OUT),
         USBD_DEV_Get_EP_Interface_Num(usbd, i, USB_EP_DIRECTION_OUT));
   }
   print_usbd_state(usbd);
   printf("%s result:\n\rcalled_send = %d\n\rcalled_status = %d\n\rcalled_stall = %d\n\rcalled_abort = %d\n\rcalled_set_ready = %d\n\rcalled_set_port = %d\n\rcalled_perform_interface_irq = %d\n\r",
         __FILE__, called_send, called_status, called_stall, called_abort, called_set_ready, called_set_port, called_perform_interface_irq);
}



static void prepare_test(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   called_send       = 0;
   called_status     = 0;
   called_stall      = 0;
   called_abort      = 0;
   called_set_ready  = 0;
   called_set_port   = 0;
   send_size         = 0xFFFF;
   mps              = 0xFF;

   if((test_param % 4) < 1)
   {
      test_port_result = USBD_TRUE;
   }
   else
   {
      test_port_result = USBD_FALSE;
   }

   sequence_cntr = 1;

   state_before = USBD_DEV_Get_State(usbd);
   state_after  = USBD_DEV_STATE_OFF;

   set_stubs(&stubs);

   memset(&port_cbks, 0, sizeof(port_cbks));
   if((test_param % 4) < 2)
   {
      port_cbks.get_descriptor = tset_port;
   }
   else
   {
      port_cbks.get_descriptor = USBD_MAKE_INVALID_HANDLER(USBD_REQUEST_Port_Callback_HT);
   }

   if((test_param % 4) < 3)
   {
      USBD_REQUEST_Install_Port_Callbacks(usbd, &port_cbks);
   }
   else
   {
      USBD_REQUEST_Install_Port_Callbacks(usbd, USBD_MAKE_INVALID_PTR(USBD_REQUEST_Port_Callbacks_XT));
   }

   /*
   if(!(test_interface_irq_present % 2))
   {
      USBDC_REQUEST_Interface_Irq_Install(usbdc, test_perform_interface_irq, test_interface_num);
   }
   else
   {
      USBDC_REQUEST_Interface_Irq_Install(usbdc, USBD_MAKE_INVALID_HANDLER(USBD_REQUEST_Interface_HT), test_interface_num);
   }
   if((test_mask_cond % 2))
   {
      USBDC_REQUEST_Set_EP_Halt_Mask(usbdc, test_endpoint_num, USBD_TRUE);
   }
   else
   {
      USBDC_REQUEST_Set_EP_Halt_Mask(usbdc, test_endpoint_num, USBD_FALSE);
   }*/

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}


static void perform_test(USBD_Params_XT *usbd)
{
   USBD_REQUEST_Req_DT req;
   uint16_t request = USBD_STD_REQ_GET_DESCRIPTOR_DEVICE;

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   req.bmRequestType = (uint8_t)(request & 0xFF);
   req.bRequest     = (uint8_t)((request >> 8) & 0xFF);
   req.wValue      = USB_DESC_TYPE_DEVICE_QUALIFIER * 256;
   req.wIndex      = 0;
   req.wLength      = test_length;

#if(FUNC_ENTER_NOTIF)
   printf("req.bmRequestType = 0x%X\n\r", req.bmRequestType);
   printf("req.bRequest      = %d\n\r", req.bRequest);
   printf("req.wValue        = 0x%X\n\r", req.wValue);
   printf("req.wIndex        = 0x%X\n\r", req.wIndex);
   printf("req.wLength       = %d\n\r", req.wLength);
#endif

   USBD_REQUEST_Process_Req(usbd, 0, &req);

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}


static void print_debug(USBD_Params_XT *usbd)
{
   printf("state_before = 0x%X; state_after = 0x%X\n\r", state_before, state_after);
   printf("test_param = %d(0x%X), test_present_mask = %d(0x%X)\n\rtest test_length = %d(0x%X), send_size = %d(0x%X), qualifier descrptor size = %d(0x%X)\n\r",
         test_param, test_param,
         test_present_mask, test_present_mask,
         test_length, test_length,
         send_size, send_size,
         QUAL_DESC_SIZE, QUAL_DESC_SIZE);
   print_states(usbd);
}


USBD_IO_Inout_Data_Size_DT ret_expected_size(void)
{
   USBD_IO_Inout_Data_Size_DT result = test_length;

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   if(test_length > QUAL_DESC_SIZE)
   {
      result = QUAL_DESC_SIZE;
   }

   USBD_EXIT_FUNC(MAIN_APP_TEST);

   return result;
}


#define QUALIFIER_SUPPORTED           ((test_present_mask & (1 << 0)) && (test_present_mask & (1 << 4)))

#define HIGH_SPEED_MPS_TO_BE_RET      ((test_present_mask & (1 << 1)) && (!(test_present_mask & (1 << 5))) && (test_present_mask & (1 << 3)))

#define FULL_SPEED_MPS_TO_BE_RET      ((test_present_mask & (1 << 1)) && (test_present_mask & (1 << 5)) && (test_present_mask & (1 << 2)))

static void check_result(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   state_after = USBD_DEV_Get_State(usbd);



   if(USBD_DEV_STATE_DEFAULT == (USBD_DEV_STATE_DEFAULT & state_before))
   {
      /* case: port handler installed and returns true */
      if(0 == (test_param % 4))
      {
         if(QUALIFIER_SUPPORTED)
         {
            if(HIGH_SPEED_MPS_TO_BE_RET)
            {
               if(   (4 == called_send)
                  && (0 == called_status)
                  && (0 == called_stall)
                  && (2 == called_abort)
                  && (0 == called_set_ready)
                  && (3 == called_set_port)
                  && (0 == called_perform_interface_irq)
                  && (ret_expected_size() == send_size)
                  && (TEST_HIGH_SPEED_MPS == mps))
               {
                  /* OK */
                  if(state_before == state_after)
                  {
                     /* OK */
                  }
                  else
                  {
                     printf("Invalid state after \"get configuration\"!\n\r");
                     print_debug(usbd);
                     REPORT_ERROR();
                  }
               }
               else
               {
                  printf("Invalid \"called\" condition!\n\r");
                  print_debug(usbd);
                  REPORT_ERROR();
               }
            } /* if(HIGH_SPEED_MPS_TO_BE_RET) */
            else if(FULL_SPEED_MPS_TO_BE_RET)
            {
               if(   (4 == called_send)
                  && (0 == called_status)
                  && (0 == called_stall)
                  && (2 == called_abort)
                  && (0 == called_set_ready)
                  && (3 == called_set_port)
                  && (0 == called_perform_interface_irq)
                  && (ret_expected_size() == send_size)
                  && (TEST_FULL_SPEED_MPS == mps))
               {
                  /* OK */
                  if(state_before == state_after)
                  {
                     /* OK */
                  }
                  else
                  {
                     printf("Invalid state after \"get configuration\"!\n\r");
                     print_debug(usbd);
                     REPORT_ERROR();
                  }
               }
               else
               {
                  printf("Invalid \"called\" condition!\n\r");
                  print_debug(usbd);
                  REPORT_ERROR();
               }
            } /* if(FULL_SPEED_MPS_TO_BE_RET) */
            else
            {
               if(   (4 == called_send)
                  && (0 == called_status)
                  && (0 == called_stall)
                  && (2 == called_abort)
                  && (0 == called_set_ready)
                  && (3 == called_set_port)
                  && (0 == called_perform_interface_irq)
                  && (ret_expected_size() == send_size)
                  && (TEST_EP0_MPS == mps))
               {
                  /* OK */
                  if(state_before == state_after)
                  {
                     /* OK */
                  }
                  else
                  {
                     printf("Invalid state after \"get configuration\"!\n\r");
                     print_debug(usbd);
                     REPORT_ERROR();
                  }
               }
               else
               {
                  printf("Invalid \"called\" condition!\n\r");
                  print_debug(usbd);
                  REPORT_ERROR();
               }
            } /* device desc MPS to be ret */
         } /* if(QUALIFIER_SUPPORTED) */
         else
         {
            if(   (0 == called_send)
               && (0 == called_status)
               && (0 == called_stall)
               && (2 == called_abort)
               && (0 == called_set_ready)
               && (3 == called_set_port)
               && (0 == called_perform_interface_irq)
               && (0xFFFF == send_size)
               && (0xFF == mps))
            {
               /* OK */
               if(state_before == state_after)
               {
                  /* OK */
               }
               else
               {
                  printf("Invalid state after \"get configuration\"!\n\r");
                  print_debug(usbd);
                  REPORT_ERROR();
               }
            }
            else
            {
               printf("Invalid \"called\" condition!\n\r");
               print_debug(usbd);
               REPORT_ERROR();
            }
         } /* if(!QUALIFIER_SUPPORTED) */
      } /* if(0 == (test_param % 4) */ /* if(USBD_DEV_STATE_DEFAULT == (USBD_DEV_STATE_DEFAULT & state_before)) */
      /* case: port handler installed but returns false */
      else if(1 == (test_param % 4))
      {
         if(   (0 == called_send)
            && (0 == called_status)
            && (0 == called_stall)
            && (2 == called_abort)
            && (0 == called_set_ready)
            && (3 == called_set_port)
            && (0 == called_perform_interface_irq)
            && (0xFFFF == send_size)
            && (0xFF == mps))
         {
            /* OK */
            if(state_before == state_after)
            {
               /* OK */
            }
            else
            {
               printf("Invalid state after \"get configuration\"!\n\r");
               print_debug(usbd);
               REPORT_ERROR();
            }
         }
         else
         {
            printf("Invalid \"called\" condition!\n\r");
            print_debug(usbd);
            REPORT_ERROR();
         }
      } /* else if(1 == (test_param % 4)) */ /* if(USBD_DEV_STATE_DEFAULT == (USBD_DEV_STATE_DEFAULT & state_before)) */
      /* case: port handler/callbacks structure not installed */
      else
      {
         if(QUALIFIER_SUPPORTED)
         {
            if(HIGH_SPEED_MPS_TO_BE_RET)
            {
               if(   (3 == called_send)
                  && (0 == called_status)
                  && (0 == called_stall)
                  && (2 == called_abort)
                  && (0 == called_set_ready)
                  && (0 == called_set_port)
                  && (0 == called_perform_interface_irq)
                  && (ret_expected_size() == send_size)
                  && (TEST_HIGH_SPEED_MPS == mps))
               {
                  /* OK */
                  if(state_before == state_after)
                  {
                     /* OK */
                  }
                  else
                  {
                     printf("Invalid state after \"get configuration\"!\n\r");
                     print_debug(usbd);
                     REPORT_ERROR();
                  }
               }
               else
               {
                  printf("Invalid \"called\" condition!\n\r");
                  print_debug(usbd);
                  REPORT_ERROR();
               }
            } /* if(HIGH_SPEED_MPS_TO_BE_RET) */
            else if(FULL_SPEED_MPS_TO_BE_RET)
            {
               if(   (3 == called_send)
                  && (0 == called_status)
                  && (0 == called_stall)
                  && (2 == called_abort)
                  && (0 == called_set_ready)
                  && (0 == called_set_port)
                  && (0 == called_perform_interface_irq)
                  && (ret_expected_size() == send_size)
                  && (TEST_FULL_SPEED_MPS == mps))
               {
                  /* OK */
                  if(state_before == state_after)
                  {
                     /* OK */
                  }
                  else
                  {
                     printf("Invalid state after \"get configuration\"!\n\r");
                     print_debug(usbd);
                     REPORT_ERROR();
                  }
               }
               else
               {
                  printf("Invalid \"called\" condition!\n\r");
                  print_debug(usbd);
                  REPORT_ERROR();
               }
            } /* if(FULL_SPEED_MPS_TO_BE_RET) */
            else
            {
               if(   (3 == called_send)
                  && (0 == called_status)
                  && (0 == called_stall)
                  && (2 == called_abort)
                  && (0 == called_set_ready)
                  && (0 == called_set_port)
                  && (0 == called_perform_interface_irq)
                  && (ret_expected_size() == send_size)
                  && (TEST_EP0_MPS == mps))
               {
                  /* OK */
                  if(state_before == state_after)
                  {
                     /* OK */
                  }
                  else
                  {
                     printf("Invalid state after \"get configuration\"!\n\r");
                     print_debug(usbd);
                     REPORT_ERROR();
                  }
               }
               else
               {
                  printf("Invalid \"called\" condition!\n\r");
                  print_debug(usbd);
                  REPORT_ERROR();
               }
            } /* device desc MPS to be ret */
         } /* if(QUALIFIER_SUPPORTED) */
         else
         {
            if(   (0 == called_send)
               && (0 == called_status)
               && (0 == called_stall)
               && (2 == called_abort)
               && (0 == called_set_ready)
               && (0 == called_set_port)
               && (0 == called_perform_interface_irq)
               && (0xFFFF == send_size)
               && (0xFF == mps))
            {
               /* OK */
               if(state_before == state_after)
               {
                  /* OK */
               }
               else
               {
                  printf("Invalid state after \"get configuration\"!\n\r");
                  print_debug(usbd);
                  REPORT_ERROR();
               }
            }
            else
            {
               printf("Invalid \"called\" condition!\n\r");
               print_debug(usbd);
               REPORT_ERROR();
            }
         } /* if(!QUALIFIER_SUPPORTED) */
      } /* if(1 < (test_param % 4)) */ /* if(USBD_DEV_STATE_DEFAULT == (USBD_DEV_STATE_DEFAULT & state_before)) */
   } /* if(USBD_DEV_STATE_DEFAULT == (USBD_DEV_STATE_DEFAULT & state_before)) */
   else
   {
      if(   (0 == called_send)
         && (0 == called_status)
         && (0 == called_stall)
         && (2 == called_abort)
         && (0 == called_set_ready)
         && (0 == called_set_port)
         && (0 == called_perform_interface_irq)
         && (0xFFFF == send_size)
         && (0xFF == mps))
      {
         /* OK */
         if(state_before == state_after)
         {
            /* OK */
         }
         else
         {
            printf("Invalid state after \"get configuration\"!\n\r");
            print_debug(usbd);
            REPORT_ERROR();
         }
      }
      else
      {
         printf("Invalid \"called\" condition!\n\r");
         print_debug(usbd);
         REPORT_ERROR();
      }
   } /* if(USBD_DEV_STATE_DEFAULT != (USBD_DEV_STATE_DEFAULT & state_before)) */

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}



static USBD_DEV_Speed_ET test_get_supported_speed(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_EXIT_FUNC(MAIN_APP_TEST);

   return test_supported_speed;
}

static USBD_DEV_Speed_ET test_get_current_speed(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_EXIT_FUNC(MAIN_APP_TEST);

   return test_current_speed;
}

void test_get_desc_qualifier(
      USBD_Params_XT *usbd,
      uint8_t port_param,
      /** bits:
       * 0 -supported speed handler present
       * 1 -current speed handler present
       * 2 -full and low speed ep0 desc present
       * 3 -high speed ep0 desc present
       * 4 -high speed supported
       * 5 -high speed present
       */
      uint8_t present_mask,
      uint16_t length,
      USBD_DEV_Port_Handler_XT *port)
{
   USBD_DEV_PORT_Get_Speed_HT    supported_speed_buf;
   USBD_DEV_PORT_Get_Speed_HT    current_speed_buf;
   USBD_DEV_PORT_Get_EP_Desc_HT  ep0_low_full_speed;
   USBD_DEV_PORT_Get_EP_Desc_HT  ep0_high_speed;
   USB_Endpoint_Desc_DT        *ep0_low_full_speed_ptr;
   USB_Endpoint_Desc_DT        *ep0_high_speed_ptr;
   uint8_t mps_full;
   uint8_t mps_high;

   USBD_ENTER_FUNC(MAIN_APP_TEST);

#if(FUNC_ENTER_NOTIF)
   printf("\n\r%s, port_param = %d, present mask = %d, length = %d\n\r", __FUNCTION__, port_param, present_mask, length);
#endif
/*
#if(FUNC_ENTER_NOTIF)
   printf("usbdc1::config_desc::interface 0, alternate 0: ep1, ep2\n\r");
   printf("usbdc1::config_desc::interface 0, alternate 1: ep2\n\r");
   printf("usbdc1::config_desc::interface 0, alternate 2: ep1, ep2\n\r");
   printf("usbdc1::config_desc::interface 1, alternate 0: ep none\n\r");
   printf("usbdc1::config_desc::interface 2, alternate 0: ep3, ep4 ep5\n\r");
#endif
*/

   supported_speed_buf     = port->handlers.get_supported_speed;
   current_speed_buf       = port->handlers.get_current_speed;
   ep0_low_full_speed      = port->handlers.get_ep0_low_full_speed;
   ep0_high_speed          = port->handlers.get_ep0_high_speed;
   ep0_low_full_speed_ptr  = (USB_Endpoint_Desc_DT*)(port->handlers.get_ep0_low_full_speed(usbd));
   ep0_high_speed_ptr      = (USB_Endpoint_Desc_DT*)(port->handlers.get_ep0_high_speed(usbd));
   mps_full                = ep0_low_full_speed_ptr->wMaxPacketSize.L;
   mps_high                = ep0_high_speed_ptr->wMaxPacketSize.L;

   test_param        = port_param;
   test_present_mask = present_mask;
   test_length       = length;

   ep0_low_full_speed_ptr->wMaxPacketSize.L  = TEST_FULL_SPEED_MPS;
   ep0_high_speed_ptr->wMaxPacketSize.L      = TEST_HIGH_SPEED_MPS;

   if(present_mask & (1 << 0))
   {
      port->handlers.get_supported_speed     = test_get_supported_speed;
   }
   else
   {
      port->handlers.get_supported_speed     = USBD_MAKE_INVALID_HANDLER(USBD_DEV_PORT_Get_Speed_HT);
   }

   if(present_mask & (1 << 1))
   {
      port->handlers.get_current_speed       = test_get_current_speed;
   }
   else
   {
      port->handlers.get_current_speed       = USBD_MAKE_INVALID_HANDLER(USBD_DEV_PORT_Get_Speed_HT);
   }

   if(!(present_mask & (1 << 2)))
   {
      port->handlers.get_ep0_low_full_speed  = USBD_MAKE_INVALID_HANDLER(USBD_DEV_PORT_Get_EP_Desc_HT);
   }

   if(!(present_mask & (1 << 3)))
   {
      port->handlers.get_ep0_high_speed      = USBD_MAKE_INVALID_HANDLER(USBD_DEV_PORT_Get_EP_Desc_HT);
   }

   if(present_mask & (1 << 4))
   {
      test_supported_speed         = USBD_DEV_FULL_AND_HIGH_SPEED;
   }
   else
   {
      test_supported_speed         = USBD_DEV_FULL_SPEED;
   }

   if(present_mask & (1 << 5))
   {
      test_current_speed           = USBD_DEV_HIGH_SPEED;
   }
   else
   {
      test_current_speed           = USBD_DEV_FULL_SPEED;
   }

   prepare_test(usbd, NULL);
   perform_test(usbd);
   check_result(usbd);

   port->handlers.get_supported_speed        = supported_speed_buf;
   port->handlers.get_current_speed          = current_speed_buf;
   port->handlers.get_ep0_low_full_speed     = ep0_low_full_speed;
   port->handlers.get_ep0_high_speed         = ep0_high_speed;
   ep0_low_full_speed_ptr->wMaxPacketSize.L  = mps_full;
   ep0_high_speed_ptr->wMaxPacketSize.L      = mps_high;

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
