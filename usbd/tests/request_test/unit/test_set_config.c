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

#include "test_set_config.h"
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


static void tsend(USBD_Params_XT *usbd, uint8_t ep_num, const void *data, USBD_IO_Inout_Data_Size_DT size);
static void tstatus(USBD_Params_XT *usbd, uint8_t ep_num);
static void tstall(USBD_Params_XT *usbd, uint8_t ep_num);
static void tabort(USBD_Params_XT *usbd, uint8_t ep_num);
static void tset_ready(void *tp_params, USBD_IOTP_EVENT_Callback_HT ready);
static USBD_Bool_DT tset_port(USBD_Params_XT *usbd, uint8_t ep_num, USBD_REQUEST_Req_DT *req);

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

static uint8_t test_config;
static uint8_t config;
static uint8_t called_set_port;
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
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   called_set_port = sequence_cntr;
   sequence_cntr++;
   config = (uint8_t)(req->wValue & 0xFF);

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
   printf("%s result:\n\rcalled_send = %d\n\rcalled_status = %d\n\rcalled_stall = %d\n\rcalled_abort = %d\n\rcalled_set_ready = %d\n\rcalled_set_port = %d\n\r",
         __FILE__, called_send, called_status, called_stall, called_abort, called_set_ready, called_set_port);
}



static void prepare_test(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   called_send       = 0;
   called_status     = 0;
   called_stall      = 0;
   called_abort      = 0;
   called_set_ready  = 0;
   called_set_port   = 0;
   config = 128;

   sequence_cntr = 1;

   state_before = USBD_DEV_Get_State(usbd);
   state_after  = USBD_DEV_STATE_OFF;

   set_stubs(&stubs);

   memset(&port_cbks, 0, sizeof(port_cbks));
   port_cbks.set_configuration = tset_port;

   USBD_REQUEST_Install_Port_Callbacks(usbd, &port_cbks);

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}


static void perform_test(USBD_Params_XT *usbd)
{
   USBD_REQUEST_Req_DT req;
   uint16_t request = USBD_STD_REQ_SET_CONFIGURATION_DEVICE;

   USBD_ENTER_FUNC(MAIN_APP_TEST);

   req.bmRequestType = (uint8_t)(request & 0xFF);
   req.bRequest      = (uint8_t)((request >> 8) & 0xFF);
   req.wValue        = test_config;
   req.wIndex        = 0;
   req.wLength       = 0;

   USBD_REQUEST_Process_Req(usbd, 0, &req);

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}


static void check_result(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   IOCMD_Logs_Postpone();

   state_after = USBD_DEV_Get_State(usbd);

   if(USBD_DEV_STATE_ADDRESSED == (USBD_DEV_STATE_ADDRESSED & state_before))
   {
      if(config <= USBD_DEV_Get_Num_Installed_Configs(usbd))
      {
         if((0 == called_send) && (((0 != test_config) ? 4 : 0) == called_status) && (0 == called_stall) && (2 == called_abort) && (0 == called_set_ready) && (3 == called_set_port))
         {
            /* OK */
            if(config == test_config)
            {
               /* OK */
               if(((0 == config) && (USBD_DEV_STATE_ADDRESSED == (USBD_DEV_STATE_ADDRESSED & state_after)))
                 || ((0 != config) && (USBD_DEV_STATE_CONFIGURED == (USBD_DEV_STATE_CONFIGURED & state_after))))
               {
                  /* OK */
               }
               else
               {
                  printf("Invalid state after \"set config device\"!\n\r");
                  printf("state_before = 0x%X; state_after = 0x%X\n\r", state_before, state_after);
                  print_states(usbd);
                  REPORT_ERROR();
               }
            }
            else
            {
               printf("Invalid adddress was set! expected config: %d; set config: %d\n\r", test_config, config);
               printf("state_before = 0x%X; state_after = 0x%X\n\r", state_before, state_after);
               print_states(usbd);
               REPORT_ERROR();
            }
         }
         else
         {
            print_states(usbd);
            REPORT_ERROR();
         }
      }
      else
      {
         if((0 == called_send) && (0 == called_status) && (0 == called_stall) && (2 == called_abort) && (0 == called_set_ready) && (3 == called_set_port))
         {
            /* OK */
            if(config == test_config)
            {
               /* OK */
               if(USBD_DEV_STATE_ADDRESSED == (USBD_DEV_STATE_ADDRESSED & state_after))
               {
                  /* OK */
               }
               else
               {
                  printf("Invalid state after \"set config device\"!\n\r");
                  printf("state_before = 0x%X; state_after = 0x%X\n\r", state_before, state_after);
                  print_states(usbd);
                  REPORT_ERROR();
               }
            }
            else
            {
               printf("Invalid adddress was set! expected config: %d; set config: %d\n\r", test_config, config);
               printf("state_before = 0x%X; state_after = 0x%X\n\r", state_before, state_after);
               print_states(usbd);
               REPORT_ERROR();
            }
         }
         else
         {
            print_states(usbd);
            REPORT_ERROR();
         }
      }
   }
   else
   {
      if((0 == called_send) && (0 == called_status) && (0 == called_stall) && (2 == called_abort) && (0 == called_set_ready) && (0 == called_set_port))
      {
         /* OK */
         if(state_before == state_after)
         {
            /* OK */
         }
         else
         {
            printf("Invalid state after \"set config device\"!\n\r");
            printf("state_before = 0x%X; state_after = 0x%X\n\r", state_before, state_after);
            print_states(usbd);
            REPORT_ERROR();
         }
      }
      else
      {
         print_states(usbd);
         REPORT_ERROR();
      }
   }

   IOCMD_Logs_Resume();

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}



void test_set_config(USBD_Params_XT *usbd, uint8_t cfg)
{
   USBD_ENTER_FUNC(MAIN_APP_TEST);

   USBD_DEBUG_HI_2(MAIN_APP_TEST, "%s, config = %d\n\r", __FUNCTION__, cfg);

   test_config = cfg;
   prepare_test(usbd);
   perform_test(usbd);
   check_result(usbd);

   USBD_EXIT_FUNC(MAIN_APP_TEST);
}
