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

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include "usbd.h"
#include "port_test.h"
#include "port_test_control.h"

#ifndef USBD_DEV_INTERNAL_H_
#include "usbd_dev_internal.h"
#endif

#include "cfg.h"

#include "main_get_exe.h"


#define TEST_LOOP_MAX_NUM_REPEATS      0x7FFF
#define TEST_LOOP_MIN_NUM_REPEATS      0x3FF

#define FUNC_ENTER_NOTIF      (false)

#define USE_INVALID_CONFIGS    (false)


#define DEV_DESC_SIZE         (sizeof(USB_device_desc_DT) / sizeof(uint8_t))



typedef struct main_test_iotp_collection_eXtended_Tag
{
   USBD_IOTP_Params_XT  *iotp;
   USB_EP_Direction_ET      dir;
   uint8_t                ep_num;
}main_test_iotp_collection_XT;



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

void test_cmd_tp_list            (IOCMD_Arg_DT *arg);
void test_cmd_loop               (IOCMD_Arg_DT *arg);
void test_cmd_exit               (IOCMD_Arg_DT *arg);
void test_cmd_logs_enable        (IOCMD_Arg_DT *arg);
void test_cmd_logs_disable       (IOCMD_Arg_DT *arg);
void test_cmd_entr_enable        (IOCMD_Arg_DT *arg);
void test_cmd_entr_disable       (IOCMD_Arg_DT *arg);
void test_cmd_print              (IOCMD_Arg_DT *arg);

static USBD_Params_XT *usbd_param_ptr;

static bool_t loop_active = false;

static uint8_t random_data_buf[2*TEST_MAX_DATA_SIZE];

//static USB_Configuration_Desc_DT desc1;
static USB_Configuration_Desc_DT desc2;
static USB_Configuration_Desc_DT desc3;
static USB_Configuration_Desc_DT desc4;
static USBDC_Params_XT usbdcp1;
static USBDC_Params_XT usbdcp2;
static USBDC_Params_XT usbdcp3;
static USBDC_Params_XT usbdcp4;
static USBDC_Params_XT *usbdc1 = &usbdcp1;
static USBDC_Params_XT *usbdc2 = &usbdcp2;
static USBDC_Params_XT *usbdc3 = &usbdcp3;
static USBDC_Params_XT *usbdc4 = &usbdcp4;

USBD_IOTP_Params_XT tp_ep0in;
USBD_IOTP_Params_XT tp_ep0out;
USBD_IOTP_Params_XT tp_ep1in;
USBD_IOTP_Params_XT tp_ep2out;

USBD_IOTP_Params_XT tp_ep3in;
USBD_IOTP_Params_XT tp_ep3out;
USBD_IOTP_Params_XT tp_ep4in;
USBD_IOTP_Params_XT tp_ep5out;

USBD_IOTP_Params_XT tp_ep6in;
USBD_IOTP_Params_XT tp_ep6out;
USBD_IOTP_Params_XT tp_ep7in;
USBD_IOTP_Params_XT tp_ep8out;

USBD_IOTP_Params_XT tp_ep9in;
USBD_IOTP_Params_XT tp_ep9out;
USBD_IOTP_Params_XT tp_ep10in;
USBD_IOTP_Params_XT tp_ep11out;

#ifdef X_ADD_IOTP
#undef X_ADD_IOTP
#endif
#define X_ADD_IOTP(a,b,c)   {c,b,a},
static main_test_iotp_collection_XT iotp_collection[TEST_MAX_NUM_IOTPS] = {MAIN_TEST_IOTP_COLLECTION};

static uint8_t desc1[] =
{
      /* bLength             */ sizeof(USB_Configuration_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_CONFIGURATION,
      /* wTotalLength.L      */ (uint8_t)(0 & 0xFF),
      /* wTotalLength.H      */ (uint8_t)((0 >> 8) & 0xFF),
      /* bNumInterfaces      */ 1,
      /* bConfigurationValue */ 0,
      /* iConfiguration      */ 4,
      /* bmAttributes        */ 0x40,
      /* bMaxPower           */ 50,

      /* bLength             */ sizeof(USB_Interface_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_INTERFACE,
      /* bInterfaceNumber    */ 0,
      /* bAlternateSetting   */ 0,
      /* bNumEndpoints       */ 14,
      /* bInterfaceClass     */ 1,
      /* bInterfaceSubClass  */ 2,
      /* bInterfaceProtocol  */ 3,
      /* iInterface          */ 0,

      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_IN | 1,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_CONTROL,
      /* wMaxPacketSize.L    */ 8,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF,

      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_OUT | 2,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_CONTROL,
      /* wMaxPacketSize.L    */ 8,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF,



      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_IN | 3,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* wMaxPacketSize.L    */ 8,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF,

      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_OUT | 3,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* wMaxPacketSize.L    */ 8,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF,

      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_IN | 4,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* wMaxPacketSize.L    */ 8,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF,

      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_OUT | 5,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* wMaxPacketSize.L    */ 8,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF,



      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_IN | 6,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* wMaxPacketSize.L    */ 8,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF,

      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_OUT | 6,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* wMaxPacketSize.L    */ 8,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF,

      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_IN | 7,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* wMaxPacketSize.L    */ 8,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF,

      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_OUT | 8,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* wMaxPacketSize.L    */ 8,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF,



      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_IN | 9,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS,
      /* wMaxPacketSize.L    */ 8,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF,

      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_OUT | 9,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS,
      /* wMaxPacketSize.L    */ 8,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF,

      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_IN | 10,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS,
      /* wMaxPacketSize.L    */ 8,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF,

      /* bLength             */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType     */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress    */ USB_EP_DESC_DIR_OUT | 11,
      /* bmAttributes        */ USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS,
      /* wMaxPacketSize.L    */ 8,
      /* wMaxPacketSize.H    */ 0,
      /* bInterval           */ 0xFF
};

static uint16_t string1[] =
{
   USB_STRING_COMPOSE_LENGTH_AND_TYPE(sizeof("Mstring1")),
   'M', 's', 't', 'r', 'i', 'n', 'g', '1'
};

static uint16_t string2[] =
{
   USB_STRING_COMPOSE_LENGTH_AND_TYPE(sizeof("Pstring2")),
   'P', 's', 't', 'r', 'i', 'n', 'g', '2'
};

static uint16_t string3[] =
{
   USB_STRING_COMPOSE_LENGTH_AND_TYPE(sizeof("Sstring3")),
   'S', 's', 't', 'r', 'i', 'n', 'g', '3'
};

static uint16_t string4[] =
{
   USB_STRING_COMPOSE_LENGTH_AND_TYPE(sizeof("Ostring4")),
   'S', 's', 't', 'r', 'i', 'n', 'g', '4'
};



static void test_compose_configs(USBD_Params_XT *usbd)
{
   USBD_DEV_Installation_Result_XT install_result;
   USBD_DEV_Config_Desc_Check_Result_XT check_result;

   if(0 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs\n\r");
      REPORT_ERROR();
   }

   desc1[2] = sizeof(desc1) & 0xFF;
   desc1[3] = (sizeof(desc1) >> 8) & 0xFF;

   check_result = USBD_DEV_Check_Config_Desc(usbd, USBD_PORT_TEST, (uint8_t*)(&desc1), sizeof(desc1));

   if(USBD_DEV_CONFIG_DESC_OK != check_result.result)
   {
      printf("incorrect configuration descriptor!\n\r");
      printf("config check result: result: %d, index: %d, if_num: %d, if_alt: %d, ep_num: %d, ep_dir: %s\n\r",
            check_result.result, check_result.index,
            check_result.if_num, check_result.if_alt,
            check_result.ep_num, USB_EP_DIRECTION_OUT == check_result.ep_dir ? "OUT" : "IN");
      REPORT_ERROR();
   }

   USBDC_Bind_Config_Desc(usbdc1, desc1);

   install_result = USBD_DEV_Install_Config(
      usbd, USBD_PORT_TEST, usbdc1);

#if(FUNC_ENTER_NOTIF)
   printf("usbdc1::config_desc::interface 0, alternate 0: ep1, ep2\n\r");
   printf("usbdc1::config_desc::interface 0, alternate 1: ep2\n\r");
   printf("usbdc1::config_desc::interface 0, alternate 2: ep1, ep2\n\r");
   printf("usbdc1::config_desc::interface 1, alternate 0: ep none\n\r");
   printf("usbdc1::config_desc::interface 2, alternate 0: ep3, ep4 ep5\n\r");
#endif

   if(1 != USBD_DEV_Get_Num_Installed_Configs(usbd))
   {
      printf("incorrect number of installed configs: %d\n\r", USBD_DEV_Get_Num_Installed_Configs(usbd));
      printf("config install result: result: %d, index: %d\n\r", install_result.result, install_result.index);
      REPORT_ERROR();
   }

   if((USBD_DEV_Get_Config(usbd, 0) != usbdc1)
      || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 0) != (uint8_t*)(&desc1))
      || (0 != install_result.index)
      || (USBD_DEV_INSTALLATION_OK != install_result.result))
   {
      printf("config is not same which was installed\n\r");
      REPORT_ERROR();
   }

   if(1 != USBD_DEV_Get_Num_Installed_Strings(usbd))
   {
      printf("incorrect number of installed strings\n\r");
      REPORT_ERROR();
   }

   install_result = USBD_DEV_Install_Manufacturer_String(usbd, (uint8_t*)string1, sizeof(string1));

   if(2 != USBD_DEV_Get_Num_Installed_Strings(usbd))
   {
      printf("incorrect number of installed strings: %d\n\r", USBD_DEV_Get_Num_Installed_Strings(usbd));
      printf("string install result: result: %d, index: %d\n\r", install_result.result, install_result.index);
      REPORT_ERROR();
   }

   if((0 != memcmp(USBD_DEV_Get_String(usbd, 1), (uint8_t*)string1, sizeof(string1))) || (1 != install_result.index)
      || (USBD_DEV_INSTALLATION_OK != install_result.result))
   {
      printf("string_table[0] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   install_result = USBD_DEV_Install_Product_String(usbd, (uint8_t*)string2, sizeof(string2));

   if(3 != USBD_DEV_Get_Num_Installed_Strings(usbd))
   {
      printf("incorrect number of installed strings: %d\n\r", USBD_DEV_Get_Num_Installed_Strings(usbd));
      printf("string install result: result: %d, index: %d\n\r", install_result.result, install_result.index);
      REPORT_ERROR();
   }

   if((0 != memcmp(USBD_DEV_Get_String(usbd, 2), (uint8_t*)string2, sizeof(string2))) || (2 != install_result.index)
      || (USBD_DEV_INSTALLATION_OK != install_result.result))
   {
      printf("string_table[1] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   install_result = USBD_DEV_Install_Serial_Number_String(usbd, (uint8_t*)string3, sizeof(string3));

   if(4 != USBD_DEV_Get_Num_Installed_Strings(usbd))
   {
      printf("incorrect number of installed strings: %d\n\r", USBD_DEV_Get_Num_Installed_Strings(usbd));
      printf("string install result: result: %d, index: %d\n\r", install_result.result, install_result.index);
      REPORT_ERROR();
   }

   if((0 != memcmp(USBD_DEV_Get_String(usbd, 3), (uint8_t*)string3, sizeof(string3))) || (3 != install_result.index)
      || (USBD_DEV_INSTALLATION_OK != install_result.result))
   {
      printf("string_table[2] is not same which was installed\n\r");
      REPORT_ERROR();
   }

   install_result = USBD_DEV_Install_String(usbd, (uint8_t*)string4, sizeof(string4), (desc1[6]));

   if(5 != USBD_DEV_Get_Num_Installed_Strings(usbd))
   {
      printf("incorrect number of installed strings: %d\n\r", USBD_DEV_Get_Num_Installed_Strings(usbd));
      printf("string install result: result: %d, index: %d\n\r", install_result.result, install_result.index);
      REPORT_ERROR();
   }

   if((0 != memcmp(USBD_DEV_Get_String(usbd, 4), (uint8_t*)string4, sizeof(string4)))
      || (4 != install_result.index)
      || (USBD_DEV_INSTALLATION_OK != install_result.result))
   {
      printf("string_table[3] is not same which was installed\n\r");
      REPORT_ERROR();
   }
}

void prepare_device(USBD_Params_XT *usbd, uint8_t step)
{
   int i;
   USBD_Init(usbd);

#if(FUNC_ENTER_NOTIF)
   printf("%s, state = 0x%X, step = %d\n\r", __FUNCTION__, USBD_DEV_Get_State(usbd), step);
#endif

   USBD_DEV_Activate(usbd, USBD_PORT_TEST);

   if(USBD_DEV_STATE_OFF != USBD_DEV_Get_State(usbd))
   {
      printf("incorrect state after USBD_DEV_Activate(usbd)!\n\r");
      REPORT_ERROR();
   }

   if(step > 1)
   {
      port_test_powered();
   }
   if(step > 2)
   {
      test_compose_configs(usbd);
      port_test_reset();
   }
   if(step > 3)
   {
      port_test_addressed(2);
   }
   if(step > 4)
   {
      if(USBD_BOOL_IS_FALSE(port_test_configured(1)))
      {
         printf("configuration set failed!\n\r");
         REPORT_ERROR();
      }

      for(i = 2; i < NUM_ELEMS(iotp_collection); i++)
      {
         USBD_IOTP_Init(usbd, usbdc1, iotp_collection[i].ep_num, iotp_collection[i].dir, iotp_collection[i].iotp);
      }
      /*
      USBD_IOTP_Init(usbd, usbdc1, 1,  USB_EP_DIRECTION_IN,  &tp_ep1in);
      USBD_IOTP_Init(usbd, usbdc1, 2,  USB_EP_DIRECTION_OUT, &tp_ep2out);
      USBD_IOTP_Init(usbd, usbdc1, 3,  USB_EP_DIRECTION_IN,  &tp_ep3in);
      USBD_IOTP_Init(usbd, usbdc1, 3,  USB_EP_DIRECTION_OUT, &tp_ep3out);
      USBD_IOTP_Init(usbd, usbdc1, 4,  USB_EP_DIRECTION_IN,  &tp_ep4in);
      USBD_IOTP_Init(usbd, usbdc1, 5,  USB_EP_DIRECTION_OUT, &tp_ep5out);
      USBD_IOTP_Init(usbd, usbdc1, 6,  USB_EP_DIRECTION_IN,  &tp_ep6in);
      USBD_IOTP_Init(usbd, usbdc1, 6,  USB_EP_DIRECTION_OUT, &tp_ep6out);
      USBD_IOTP_Init(usbd, usbdc1, 7,  USB_EP_DIRECTION_IN,  &tp_ep7in);
      USBD_IOTP_Init(usbd, usbdc1, 8,  USB_EP_DIRECTION_OUT, &tp_ep8out);
      USBD_IOTP_Init(usbd, usbdc1, 9,  USB_EP_DIRECTION_IN,  &tp_ep9in);
      USBD_IOTP_Init(usbd, usbdc1, 9,  USB_EP_DIRECTION_OUT, &tp_ep9out);
      USBD_IOTP_Init(usbd, usbdc1, 10, USB_EP_DIRECTION_IN,  &tp_ep10in);
      USBD_IOTP_Init(usbd, usbdc1, 11, USB_EP_DIRECTION_OUT, &tp_ep11out);*/

   //    USBD_IOTP_Install_Default_Control_Pipe(usbd, &tp_ep0in, &tp_ep0out);
      for(i = 2; i < NUM_ELEMS(iotp_collection); i++)
      {
         USBD_IOTP_Install(iotp_collection[i].iotp);
      }
      /*
      USBD_IOTP_Install(&tp_ep1in);
      USBD_IOTP_Install(&tp_ep2out);
      USBD_IOTP_Install(&tp_ep3in);
      USBD_IOTP_Install(&tp_ep3out);
      USBD_IOTP_Install(&tp_ep4in);
      USBD_IOTP_Install(&tp_ep5out);
      USBD_IOTP_Install(&tp_ep6in);
      USBD_IOTP_Install(&tp_ep6out);
      USBD_IOTP_Install(&tp_ep7in);
      USBD_IOTP_Install(&tp_ep8out);
      USBD_IOTP_Install(&tp_ep9in);
      USBD_IOTP_Install(&tp_ep9out);
      USBD_IOTP_Install(&tp_ep10in);
      USBD_IOTP_Install(&tp_ep11out);*/
   }
#if(FUNC_ENTER_NOTIF)
   printf("%s, state = 0x%X\n\r", __FUNCTION__, USBD_DEV_Get_State(usbd));
#endif
} /* prepare_device */

uint8_t x_if(void)
{
   return(rand() % USBD_MAX_NUM_INTERFACES);
}

uint8_t x_u8(void)
{
   return(rand() % 256);
}

void set_dummy_cfg(USB_Configuration_Desc_DT *desc)
{
   desc->bLength = USB_DESC_TYPE_CONFIGURATION_SIZE;
   desc->bDescriptorType = USB_DESC_TYPE_CONFIGURATION;
   desc->wTotalLength.L = USB_DESC_TYPE_CONFIGURATION_SIZE;
   desc->wTotalLength.H = 0;
   desc->bNumInterfaces = 0;
   desc->bConfigurationValue = 0;
   desc->iConfiguration = 0;
   desc->bmAttributes = 0x40;
   desc->bMaxPower = 50;
}



void test_cmd_tp_list(IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   int cntr;
   uint8_t ep_num;

   printf("%s: TPs:\n\r", __FUNCTION__);
   printf(" TP, dir, EP:\n\r");

   for(cntr = 0; cntr < NUM_ELEMS(iotp_collection); cntr++)
   {
      printf("%3d, %s, %2d\n\r", cntr, (USB_EP_DIRECTION_OUT == iotp_collection[cntr].dir) ? "out" : " in", iotp_collection[cntr].ep_num);
   }

   printf("\n\r");
} /* test_cmd_tp_list */



void test_cmd_loop(IOCMD_Arg_DT *arg)
{
   loop_active = true;

   printf("commands loop activated!\n\n\r");
} /* test_cmd_loop */

void test_cmd_exit(IOCMD_Arg_DT *arg)
{
   loop_active = false;

   printf("commands loop exited!\n\n\r");
} /* test_cmd_exit */

void test_cmd_logs_enable         (IOCMD_Arg_DT *arg)
{
   IOCMD_Set_All_Logs(IOCMD_LOG_LEVEL_DEBUG_LO, IOCMD_LOG_LEVEL_DEBUG_LO);
}

void test_cmd_logs_disable         (IOCMD_Arg_DT *arg)
{
   IOCMD_Set_All_Logs(IOCMD_LOG_LEVEL_EMERG, IOCMD_LOG_LEVEL_EMERG);
}

void test_cmd_entr_enable         (IOCMD_Arg_DT *arg)
{
   IOCMD_Set_All_Entrances(IOCMD_ENTRANCE_ENABLED);
}

void test_cmd_entr_disable         (IOCMD_Arg_DT *arg)
{
   IOCMD_Set_All_Entrances(IOCMD_ENTRANCE_DISABLED);
}

void test_cmd_print              (IOCMD_Arg_DT *arg)
{
   IOCMD_Proc_Buffered_Logs(true, main_get_exe(), working_buf, sizeof(working_buf));
}


static IOCMD_Command_Tree_XT cmd_tree[] =
{
   IOCMD_ELEM(       "tplist"           , test_cmd_tp_list            , "lists all TPs that can be used in test"),
   IOCMD_ELEM(       "loop"             , test_cmd_loop               , "allows executing commands in loop"),
   IOCMD_ELEM(       "exit"             , test_cmd_exit               , "exits loop commands executing"),
   IOCMD_GROUP_START(  "logs"           ,                               "group of commands logs manipulation"),
      IOCMD_ELEM(       "enable"        , test_cmd_logs_enable        , "enables all logs"),
      IOCMD_ELEM(       "disable"       , test_cmd_logs_disable       , "disables all logs"),
   IOCMD_GROUP_END(),
   IOCMD_GROUP_START(  "entr"           ,                               "group of commands entrances logging manipulation"),
      IOCMD_ELEM(       "enable"        , test_cmd_entr_enable        , "enables all entrances"),
      IOCMD_ELEM(       "disable"       , test_cmd_entr_disable       , "disables all entrances"),
   IOCMD_GROUP_END(),
   IOCMD_ELEM(       "print"            , test_cmd_print              , "allows executing commands in loop")
};

int main(int argc, char* argv[])
{
   char line[20000];
   char args[200][1000];
   char *arg_tab[200];
   int num_chars;
   int num_args;
   USBD_Params_XT usbd;
   int j;
   uint8_t ep_num;
   int number_of_repeats;
   uint32_t cntr;
   int result;

   printf("line pointer: %p; test:%016dtest:%-+16.8dtest\n\r", line, 91, 88);
   printf("TEST STRING: '%32s' end_string", "test\n\r");

   IOCMD_Logs_Init();

   /*

   set_dummy_cfg(&desc2);
   set_dummy_cfg(&desc3);
   set_dummy_cfg(&desc4);

   desc1[2] = sizeof(desc1) & 0xFF;
   desc1[3] = (sizeof(desc1) >> 8) & 0xFF;*/

   srand(time(NULL));
   /*
   number_of_repeats = rand() % TEST_LOOP_MAX_NUM_REPEATS;
   if(number_of_repeats < TEST_LOOP_MIN_NUM_REPEATS)
   {
      number_of_repeats += TEST_LOOP_MIN_NUM_REPEATS;
   }

   printf("number_of_repeats: %d\n\r",number_of_repeats);

   for(j = 0; j < number_of_repeats; j++)
   {
      x_u8();
   }*/

   for(cntr = 0; cntr < sizeof(random_data_buf); cntr++)
   {
      random_data_buf[cntr] = rand() % 256;
   }

   prepare_device(&usbd, 5);

   num_args = argc;
   for(cntr = 0; cntr < argc; cntr++)
   {
      arg_tab[cntr] = argv[cntr];
   }

   do
   {
      usbd_param_ptr = &usbd;
      IOCMD_Parse_Command(num_args - 1, (const char**)(&arg_tab[1]), main_get_exe(), cmd_tree, Num_Elems(cmd_tree), IOCMD_TRUE);
      printf("\n\rplease put next command:\n\n\r");
      fgets(line, sizeof(line) - 1, stdin);
      printf("command: \"%s\"\n\r", line);
      num_chars = strlen(line) - 1;

      num_args = 0;
      cntr = 0;
      do
      {
         j = sscanf(&line[cntr], "%s", args[num_args]);
         if(j <= 0)
         {
            break;
         }
         cntr += strlen(args[num_args]) + 1;
         num_args++;
      }while(cntr < num_chars);

      for(cntr = 0; cntr < num_args; cntr++)
      {
         arg_tab[cntr + 1] = args[cntr];
      }
      num_args++;

      IOCMD_Proc_Buffered_Logs(true, main_get_exe(), working_buf, sizeof(working_buf));
   }while(loop_active);

   printf("\n\n\rsize of USBD_Params_XT: %d\n\r", (sizeof(USBD_Params_XT) / sizeof(uint8_t)));
   printf("size of USBDC_Params_XT: %d\n\r", (sizeof(USBDC_Params_XT) / sizeof(uint8_t)));
   printf("size of USBD_IO_Params_XT: %d\n\r", (sizeof(USBD_IO_Params_XT) / sizeof(uint8_t)));
   printf("size of USBD_IOTP_Params_XT: %d\n\r", (sizeof(USBD_IOTP_Params_XT) / sizeof(uint8_t)));
   printf("size of USBD_IO_UP_IN_Data_Event_HT: %d\n\r", (sizeof(USBD_IO_UP_IN_Data_Event_HT) / sizeof(uint8_t)));
   printf("size of string1: %d, from descriptor: %d\n\r", sizeof(string1), string1[0] & 0xFF);
   printf("size of string2: %d, from descriptor: %d\n\r", sizeof(string2), string2[0] & 0xFF);
   printf("size of string3: %d, from descriptor: %d\n\r", sizeof(string3), string3[0] & 0xFF);
   printf("size of string4: %d, from descriptor: %d\n\n\r", sizeof(string4), string4[0] & 0xFF);

   printf("\ntests done\n\n");

   return result;
}

void dummy_printf(const char *arg, ...)
{
}

void IOTERM_CMD_XMACRO_printf(const char *format, ...)
{
   va_list args;
   va_start(args, format);
   vprintf(format, args);
   va_end(args);
}
