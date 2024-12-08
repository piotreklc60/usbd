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

#include "test_get_desc_other_endpoint.h"
#include "iotp_stubs.h"

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


#define DEV_DESC_SIZE         (sizeof(USB_Device_Desc_DT) / sizeof(uint8_t))


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

static uint8_t test_if_num;
static uint8_t test_ep_num;
static USB_EP_Direction_ET test_dir;
static uint8_t test_param;
static uint16_t test_length;
static USBD_IO_Inout_Data_Size_DT send_size;

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

   request  = ((uint16_t)(req->bRequest)) & 0x00FF;
   request *= 256;
   request += req->bmRequestType;

   if(USBD_STD_REQ_GET_DESCRIPTOR_ENDPOINT != request)
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

   request  = ((uint16_t)(req->bRequest)) & 0x00FF;
   request *= 256;
   request += req->bmRequestType;

   called_perform_interface_irq = sequence_cntr;
   sequence_cntr++;

   if(USBD_STD_REQ_GET_DESCRIPTOR_ENDPOINT != request)
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

   if(USBD_DEV_STATE_ATTACHED == (USBD_DEV_STATE_ATTACHED & state))
   {
      printf("USBD_DEV_STATE_ATTACHED");
   }
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
   called_perform_interface_irq = 0;
   send_size         = 0xFFFF;

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

   USBDC_REQUEST_Interface_Irq_Install(usbdc, test_perform_interface_irq, test_if_num);

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}


static void perform_test(USBD_Params_XT *usbd)
{
   USBD_REQUEST_Req_DT req;
   uint16_t request = USBD_STD_REQ_GET_DESCRIPTOR_ENDPOINT;

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   req.bmRequestType = (uint8_t)(request & 0xFF);
   req.bRequest      = (uint8_t)((request >> 8) & 0xFF);
   req.wValue        = USB_DESC_TYPE_INTERFACE_POWER * 256;
   req.wIndex        = test_ep_num | test_dir;
   req.wLength       = test_length;

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
   printf("test_param = %d(0x%X)\n\rtest test_length = %d(0x%X), send_size = %d(0x%X), device descrptor size = %d(0x%X)\n\r",
         test_param, test_param,
         test_length, test_length,
         send_size, send_size,
         DEV_DESC_SIZE, DEV_DESC_SIZE);
   print_states(usbd);
}


static void check_result(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   state_after = USBD_DEV_Get_State(usbd);

   if(USBD_DEV_STATE_CONFIGURED == (USBD_DEV_STATE_CONFIGURED & state_before))
   {
      /* case: port handler installed and returns true */
      if(0 == (test_param % 4))
      {
         if(test_if_num == USBD_DEV_Get_EP_Interface_Num(usbd, test_ep_num, test_dir))
         {
            if(   (0 == called_send)
               && (0 == called_status)
               && (0 == called_stall)
               && (2 == called_abort)
               && (0 == called_set_ready)
               && (3 == called_set_port)
               && (4 == called_perform_interface_irq)
               && (0xFFFF == send_size))
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
         }
         else
         {
            if(   (0 == called_send)
               && (0 == called_status)
               && (0 == called_stall)
               && (2 == called_abort)
               && (0 == called_set_ready)
               && (3 == called_set_port)
               && (0 == called_perform_interface_irq)
               && (0xFFFF == send_size))
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
         }
      } /* if(0 == (test_param % 4) */ /* if(USBD_DEV_STATE_CONFIGURED == (USBD_DEV_STATE_CONFIGURED & state_before)) */
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
            && (0xFFFF == send_size))
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
      } /* else if(1 == (test_param % 4)) */ /* if(USBD_DEV_STATE_CONFIGURED == (USBD_DEV_STATE_CONFIGURED & state_before)) */
      /* case: port handler/callbacks structure not installed */
      else
      {
         if(test_if_num == USBD_DEV_Get_EP_Interface_Num(usbd, test_ep_num, test_dir))
         {
            if(   (0 == called_send)
               && (0 == called_status)
               && (0 == called_stall)
               && (2 == called_abort)
               && (0 == called_set_ready)
               && (0 == called_set_port)
               && (3 == called_perform_interface_irq)
               && (0xFFFF == send_size))
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
         }
         else
         {
            if(   (0 == called_send)
               && (0 == called_status)
               && (0 == called_stall)
               && (2 == called_abort)
               && (0 == called_set_ready)
               && (0 == called_set_port)
               && (0 == called_perform_interface_irq)
               && (0xFFFF == send_size))
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
         }
      } /* if(1 < (test_param % 4)) */ /* if(USBD_DEV_STATE_CONFIGURED == (USBD_DEV_STATE_CONFIGURED & state_before)) */
   } /* if(USBD_DEV_STATE_CONFIGURED == (USBD_DEV_STATE_CONFIGURED & state_before)) */
   else
   {
      if(   (0 == called_send)
         && (0 == called_status)
         && (0 == called_stall)
         && (2 == called_abort)
         && (0 == called_set_ready)
         && (0 == called_set_port)
         && (0 == called_perform_interface_irq)
         && (0xFFFF == send_size))
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
   } /* if(USBD_DEV_STATE_CONFIGURED != (USBD_DEV_STATE_CONFIGURED & state_before)) */

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}



void test_get_desc_other_endpoint(
   USBD_Params_XT *usbd, USBDC_Params_XT *usbdc,
   uint8_t port_param, uint16_t length, uint8_t if_num, uint8_t ep_num, uint8_t dir)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   ep_num &= 0x7F;

   if(0xFF == if_num)
   {
      USBD_WARN(MAIN_APP_TEST, "if num 0xFF decreased because 255 is not allowed!!!");
      if_num--;
   }

   USBD_DEBUG_HI_6(MAIN_APP_TEST, "%s, port_param = %d, length = %d, if_num = %d, ep_num = %d, dir = %s",
      __FUNCTION__, port_param, length, if_num, ep_num, (0 == (dir & 1)) ? "OUT" : "IN");

/*
#if(FUNC_ENTER_NOTIF)
   printf("usbdc1::config_desc::interface 0, alternate 0: ep1, ep2\n\r");
   printf("usbdc1::config_desc::interface 0, alternate 1: ep2\n\r");
   printf("usbdc1::config_desc::interface 0, alternate 2: ep1, ep2\n\r");
   printf("usbdc1::config_desc::interface 1, alternate 0: ep none\n\r");
   printf("usbdc1::config_desc::interface 2, alternate 0: ep3, ep4 ep5\n\r");
#endif
*/
   test_length = length;
   test_param  = port_param;
   test_if_num = if_num;
   test_ep_num = ep_num;
   test_dir   = (0 == (dir & 1)) ? USB_EP_DIRECTION_OUT : USB_EP_DIRECTION_IN;

   USBDC_REQUEST_Interface_Irq_Install(
      usbdc,
      USBD_MAKE_INVALID_HANDLER(USBD_REQUEST_Interface_HT),
      USBD_DEV_Get_EP_Interface_Num(usbd, test_ep_num, test_dir));

   prepare_test(usbd, usbdc);
   perform_test(usbd);
   check_result(usbd);

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
