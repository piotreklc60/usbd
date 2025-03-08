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
#include "usbd.h"
#include "port_test.h"
#include "port_test_control.h"

#ifndef USBD_DEV_INTERNAL_H_
#include "usbd_dev_internal.h"
#endif

#include "cfg.h"

#include "main_get_exe.h"

#include "iotp_stubs.h"

#include "test_set_addr.h"
#include "test_set_config.h"
#include "test_set_interface.h"
#include "test_set_feature_device.h"
#include "test_set_feature_interface.h"
#include "test_set_feature_endpoint.h"
#include "test_clear_feature_device.h"
#include "test_clear_feature_interface.h"
#include "test_clear_feature_endpoint.h"
#include "test_get_status_device.h"
#include "test_get_status_interface.h"
#include "test_get_status_endpoint.h"
#include "test_get_interface.h"
#include "test_get_configuration.h"
#include "test_get_desc_device.h"
#include "test_get_desc_qualifier.h"
#include "test_get_desc_config.h"
#include "test_get_desc_string.h"
#include "test_get_desc_other_endpoint.h"
#include "test_get_desc_other_interface.h"
#include "test_get_desc_other_device.h"

#include <stdlib.h>

#define TEST_LOOP_MAX_NUM_REPEATS      0x7FFF
#define TEST_LOOP_MIN_NUM_REPEATS      0x3FF

#define FUNC_ENTER_NOTIF      (false)

#define USE_INVALID_CONFIGS    (false)


#define DEV_DESC_SIZE         (sizeof(USB_Device_Desc_DT) / sizeof(uint8_t))



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

static void test_activate_deactivate(USBD_Params_XT* usbd, USBD_Bool_DT state);
static USBD_Bool_DT test_parse_ep(USBD_Params_XT* usbd, uint8_t ep_num, const USB_Interface_Desc_DT *if_desc, const USB_Endpoint_Desc_DT *ep_desc);
static USBD_Bool_DT test_parse_cfg(USBD_Params_XT* usbd, const uint8_t *desc, uint16_t desc_size, USBD_DEV_Config_Desc_Check_Result_XT *details);
static USBD_DEV_Speed_ET test_get_supported_speed(USBD_Params_XT *usbd);
static USBD_DEV_Speed_ET test_get_current_speed(USBD_Params_XT *usbd);
static uint16_t test_get_dev_status(USBD_Params_XT *usbd);
static uint16_t test_get_frame_nr(USBD_Params_XT *usbd);
static const USB_Endpoint_Desc_DT* test_get_ep0_low_full_speed(USBD_Params_XT* usbd);
static const USB_Endpoint_Desc_DT* test_get_ep0_high_speed(USBD_Params_XT* usbd);

void test_cmd_all                      (IOCMD_Arg_DT *arg);
void test_cmd_tp_list                  (IOCMD_Arg_DT *arg);
void test_cmd_loop                     (IOCMD_Arg_DT *arg);
void test_cmd_exit                     (IOCMD_Arg_DT *arg);
void test_cmd_logs_enable              (IOCMD_Arg_DT *arg);
void test_cmd_logs_disable             (IOCMD_Arg_DT *arg);
void test_cmd_entr_enable              (IOCMD_Arg_DT *arg);
void test_cmd_entr_disable             (IOCMD_Arg_DT *arg);
void test_cmd_print                    (IOCMD_Arg_DT *arg);
void test_cmd_set_addr                 (IOCMD_Arg_DT *arg);
void test_cmd_set_config               (IOCMD_Arg_DT *arg);
void test_cmd_set_interface            (IOCMD_Arg_DT *arg);
void test_cmd_set_feature_device       (IOCMD_Arg_DT *arg);
void test_cmd_set_feature_interface    (IOCMD_Arg_DT *arg);
void test_cmd_set_feature_endpoint     (IOCMD_Arg_DT *arg);
void test_cmd_clear_feature_device     (IOCMD_Arg_DT *arg);
void test_cmd_clear_feature_interface  (IOCMD_Arg_DT *arg);
void test_cmd_clear_feature_endpoint   (IOCMD_Arg_DT *arg);
void test_cmd_get_status_device        (IOCMD_Arg_DT *arg);
void test_cmd_get_status_interface     (IOCMD_Arg_DT *arg);
void test_cmd_get_status_endpoint      (IOCMD_Arg_DT *arg);
void test_cmd_get_interface            (IOCMD_Arg_DT *arg);
void test_cmd_get_configuration        (IOCMD_Arg_DT *arg);
void test_cmd_get_desc_device          (IOCMD_Arg_DT *arg);
void test_cmd_get_desc_qualifier       (IOCMD_Arg_DT *arg);
void test_cmd_get_desc_config          (IOCMD_Arg_DT *arg);
void test_cmd_get_desc_string          (IOCMD_Arg_DT *arg);
void test_cmd_get_desc_other_device    (IOCMD_Arg_DT *arg);
void test_cmd_get_desc_other_interface (IOCMD_Arg_DT *arg);
void test_cmd_get_desc_other_endpoint  (IOCMD_Arg_DT *arg);

static USBD_Params_XT *usbd_param_ptr;

static USBD_Bool_DT loop_active = false;

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


static int number_of_repeats;


static uint8_t desc1[] =
#if 1
{
      /* bLength              */ sizeof(USB_Configuration_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_CONFIGURATION,
      /* wTotalLength.L       */ (uint8_t)(0 & 0xFF),
      /* wTotalLength.H       */ (uint8_t)((0 >> 8) & 0xFF),
      /* bNumInterfaces       */ TEST_NUM_INTERFACES,
      /* bConfigurationValue  */ 1,
      /* iConfiguration       */ 4,
      /* bmAttributes         */ 0x40,
      /* bMaxPower            */ 50,

      /* bLength              */ sizeof(USB_Interface_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_INTERFACE,
      /* bInterfaceNumber     */ 0,
      /* bAlternateSetting    */ 0,
      /* bNumEndpoints        */ 2,
      /* bInterfaceClass      */ 1,
      /* bInterfaceSubClass   */ 2,
      /* bInterfaceProtocol   */ 3,
      /* iInterface           */ 0,

      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_IN | 1,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_OUT | 2,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Interface_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_INTERFACE,
      /* bInterfaceNumber     */ 0,
      /* bAlternateSetting    */ 1,
      /* bNumEndpoints        */ 2,
      /* bInterfaceClass      */ 1,
      /* bInterfaceSubClass   */ 2,
      /* bInterfaceProtocol   */ 3,
      /* iInterface           */ 0,
#if 1
      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_IN | 1,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* wMaxPacketSize.L     */ 64,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,
#endif
      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_OUT | 2,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Interface_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_INTERFACE,
      /* bInterfaceNumber     */ 0,
      /* bAlternateSetting    */ 2,
      /* bNumEndpoints        */ 2,
      /* bInterfaceClass      */ 1,
      /* bInterfaceSubClass   */ 2,
      /* bInterfaceProtocol   */ 3,
      /* iInterface           */ 0,
#if 1
      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_IN | 1,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* wMaxPacketSize.L     */ 0,
      /* wMaxPacketSize.H     */ 2,
      /* bInterval            */ 0xFF,
#endif
      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_OUT | 2,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* wMaxPacketSize.L     */ 0,
      /* wMaxPacketSize.H     */ 1,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Interface_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_INTERFACE,
      /* bInterfaceNumber     */ 1,
      /* bAlternateSetting    */ 0,
      /* bNumEndpoints        */ 0,
      /* bInterfaceClass      */ 6,
      /* bInterfaceSubClass   */ 3,
      /* bInterfaceProtocol   */ 11,
      /* iInterface           */ 0,

      /* bLength              */ sizeof(USB_Interface_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_INTERFACE,
      /* bInterfaceNumber     */ 2,
      /* bAlternateSetting    */ 0,
      /* bNumEndpoints        */ 3,
      /* bInterfaceClass      */ 1,
      /* bInterfaceSubClass   */ 2,
      /* bInterfaceProtocol   */ 3,
      /* iInterface           */ 0,

      /* other type descriptor */
      4,
      12,
      2,
      3,

      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_IN | 3,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* wMaxPacketSize.L     */ 64,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_OUT | 4,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_OUT | 5,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS,
      /* wMaxPacketSize.L     */ 0,
      /* wMaxPacketSize.H     */ 4,
      /* bInterval            */ 0xFF
};
#else
{
      /* bLength              */ sizeof(USB_Configuration_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_CONFIGURATION,
      /* wTotalLength.L       */ (uint8_t)(0 & 0xFF),
      /* wTotalLength.H       */ (uint8_t)((0 >> 8) & 0xFF),
      /* bNumInterfaces       */ 1,
      /* bConfigurationValue  */ 0,
      /* iConfiguration       */ 0,
      /* bmAttributes         */ 0x40,
      /* bMaxPower            */ 50,

      /* bLength              */ sizeof(USB_Interface_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_INTERFACE,
      /* bInterfaceNumber     */ 0,
      /* bAlternateSetting    */ 0,
      /* bNumEndpoints        */ 14,
      /* bInterfaceClass      */ 1,
      /* bInterfaceSubClass   */ 2,
      /* bInterfaceProtocol   */ 3,
      /* iInterface           */ 0,

      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_IN | 1,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_CONTROL,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_OUT | 2,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_CONTROL,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,



      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_IN | 3,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_OUT | 3,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_IN | 4,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_OUT | 5,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,



      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_IN | 6,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_OUT | 6,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_IN | 7,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_OUT | 8,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,



      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_IN | 9,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_OUT | 9,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_IN | 10,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF,

      /* bLength              */ sizeof(USB_Endpoint_Desc_DT),
      /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
      /* bEndpointAddress     */ USB_EP_DESC_DIR_OUT | 11,
      /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS,
      /* wMaxPacketSize.L     */ 8,
      /* wMaxPacketSize.H     */ 0,
      /* bInterval            */ 0xFF
};
#endif

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

static USBD_DEV_Port_Handler_XT main_port =
{
   .handlers.activate            = test_activate_deactivate,
   .handlers.ep_parse            = test_parse_ep,
   .handlers.cfg_parse           = test_parse_cfg,
   .handlers.get_supported_speed   = test_get_supported_speed,
   .handlers.get_current_speed     = test_get_current_speed,
   .handlers.get_device_status     = test_get_dev_status,
   .handlers.get_frame_num        = test_get_frame_nr,
   .handlers.get_ep0_low_full_speed = test_get_ep0_low_full_speed,
   .handlers.get_ep0_high_speed    = test_get_ep0_high_speed
};

#if 0
static uint8_t activity = 0;
static USB_Endpoint_Desc_DT ep0_full =
{
   /* bLength              */ USB_DESC_ENDPOINT_SIZE,
   /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
   /* bEndpointAddress     */ 0,
   /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_CONTROL,
   {
   /* wMaxPacketSize.L     */ TEST_EP0_MPS,
   /* wMaxPacketSize.H     */ 0
   },
   /* bInterval            */ 0xFF,
};
static USB_Endpoint_Desc_DT ep0_high =
{
   /* bLength              */ USB_DESC_ENDPOINT_SIZE,
   /* bDescriptorType      */ USB_DESC_TYPE_ENDPOINT,
   /* bEndpointAddress     */ 0,
   /* bmAttributes         */ USB_EP_DESC_TRANSFER_TYPE_CONTROL,
   {
   /* wMaxPacketSize.L     */ TEST_EP0_MPS,
   /* wMaxPacketSize.H     */ 0
   },
   /* bInterval            */ 0xFF,
};
static USBD_DEV_Speed_ET speed = USBD_DEV_FULL_AND_HIGH_SPEED;
static uint16_t device_status = 0;
static uint16_t frame_nr = 0;
#endif

static void test_activate_deactivate(USBD_Params_XT* usbd, USBD_Bool_DT state)
{
   const USBD_DEV_Port_Handler_XT *port = USBD_PORT_TEST;

   port->handlers.activate(usbd, state);
}
static USBD_Bool_DT test_parse_ep(USBD_Params_XT* usbd, uint8_t ep_num, const USB_Interface_Desc_DT *if_desc, const USB_Endpoint_Desc_DT *ep_desc)
{
   const USBD_DEV_Port_Handler_XT *port = USBD_PORT_TEST;

   return port->handlers.ep_parse(usbd, ep_num, if_desc, ep_desc);
}

static USBD_Bool_DT test_parse_cfg(USBD_Params_XT* usbd, const uint8_t *desc, uint16_t desc_size, USBD_DEV_Config_Desc_Check_Result_XT *details)
{
   const USBD_DEV_Port_Handler_XT *port = USBD_PORT_TEST;

   return port->handlers.cfg_parse(usbd, desc, desc_size, details);
}

static USBD_DEV_Speed_ET test_get_supported_speed(USBD_Params_XT *usbd)
{
   const USBD_DEV_Port_Handler_XT *port = USBD_PORT_TEST;

   return port->handlers.get_supported_speed(usbd);
}

static USBD_DEV_Speed_ET test_get_current_speed(USBD_Params_XT *usbd)
{
   const USBD_DEV_Port_Handler_XT *port = USBD_PORT_TEST;

   return port->handlers.get_current_speed(usbd);
}

static uint16_t test_get_dev_status(USBD_Params_XT *usbd)
{
   const USBD_DEV_Port_Handler_XT *port = USBD_PORT_TEST;

   return port->handlers.get_device_status(usbd);
}

static uint16_t test_get_frame_nr(USBD_Params_XT *usbd)
{
   const USBD_DEV_Port_Handler_XT *port = USBD_PORT_TEST;

   return port->handlers.get_frame_num(usbd);
}

static const USB_Endpoint_Desc_DT* test_get_ep0_low_full_speed(USBD_Params_XT* usbd)
{
   const USBD_DEV_Port_Handler_XT *port = USBD_PORT_TEST;

   return port->handlers.get_ep0_low_full_speed(usbd);
}

static const USB_Endpoint_Desc_DT* test_get_ep0_high_speed(USBD_Params_XT* usbd)
{
   const USBD_DEV_Port_Handler_XT *port = USBD_PORT_TEST;

   return port->handlers.get_ep0_high_speed(usbd);
}


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

   check_result = USBD_DEV_Check_Config_Desc(usbd, &main_port, (uint8_t*)(&desc1), sizeof(desc1));

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

   install_result = USBD_DEV_Install_Config(usbd, &main_port, usbdc1);

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

   install_result = USBD_DEV_Install_String(usbd, (uint8_t*)string4, sizeof(string4), desc1[6]);

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

   IOCMD_Clear_All_Logs(true);

   USBD_Init(usbd);

#if(FUNC_ENTER_NOTIF)
   printf("%s, state = 0x%X, step = %d\n\r", __FUNCTION__, USBD_DEV_Get_State(usbd), step);
#endif

   USBD_DEV_Activate(usbd, &main_port);

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

   //   USBD_IOTP_Install_Default_Control_Pipe(usbd, &tp_ep0in, &tp_ep0out);
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



void test_cmd_all(IOCMD_Arg_DT *arg)
{
   test_cmd_set_addr                (arg);
   test_cmd_set_config              (arg);
   test_cmd_set_interface           (arg);
   test_cmd_set_feature_device      (arg);
   test_cmd_set_feature_interface   (arg);
   test_cmd_set_feature_endpoint    (arg);
   test_cmd_clear_feature_device    (arg);
   test_cmd_clear_feature_interface (arg);
   test_cmd_clear_feature_endpoint  (arg);
   test_cmd_get_status_device       (arg);
   test_cmd_get_status_interface    (arg);
   test_cmd_get_status_endpoint     (arg);
   test_cmd_get_interface           (arg);
   test_cmd_get_configuration       (arg);
   test_cmd_get_desc_device         (arg);
   test_cmd_get_desc_qualifier      (arg);
   test_cmd_get_desc_config         (arg);
   test_cmd_get_desc_string         (arg);
   test_cmd_get_desc_other_device   (arg);
   test_cmd_get_desc_other_interface(arg);
   test_cmd_get_desc_other_endpoint (arg);
} /* test_cmd_loop */

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

void test_cmd_set_addr             (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_set_addr test\n\r");

   for(i = 0; i < 6; i++)
   {
      printf("loop: %d\n\r", i);
      prepare_device(usbd, i);
      test_set_addr(usbd, 0);
      prepare_device(usbd, i);
      test_set_addr(usbd, (rand() % 127) + 1);
      prepare_device(usbd, i);
      test_set_addr(usbd, 0);
      prepare_device(usbd, i);
      test_set_addr(usbd, (rand() % 127) + 1);

      printf("iteration 0; %d repeats\n\r", number_of_repeats);
      for(j = 0; j < number_of_repeats; j++)
      {
         test_set_addr(usbd, rand() % 256);
      }

      printf("iteration 1; %d repeats\n\r", number_of_repeats);
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_set_addr(usbd, rand() % 256);
      }

      printf("iteration 2; %d repeats\n\r", number_of_repeats);
      for(a = 0; a < 256; a++)
      {
         test_set_addr(usbd, (uint8_t)a);
      }

      printf("iteration 3; %d repeats\n\r", number_of_repeats);
      for(a = 0; a < 256; a++)
      {
         prepare_device(usbd, i);
         test_set_addr(usbd, (uint8_t)a);
      }
   }
}

void test_cmd_set_config            (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_set_config test\n\r");

   for(i = 4; i < 6; i++)
   {
      prepare_device(usbd, i);
      test_set_config(usbd, 0);
      prepare_device(usbd, i);
      test_set_config(usbd, /*(rand() % 3) + 1*/2);
      prepare_device(usbd, i);
      test_set_config(usbd, 0);
      prepare_device(usbd, i);
      test_set_config(usbd, (rand() % 255) + 1);
      prepare_device(usbd, i);
      test_set_config(usbd, 0);
      prepare_device(usbd, i);
      test_set_config(usbd, (rand() % 250) + 5);
      prepare_device(usbd, i);
      test_set_config(usbd, 0);
      prepare_device(usbd, i);
      test_set_config(usbd, (rand() % 3) + 1);
      for(j = 0; j < number_of_repeats; j++)
      {
         test_set_config(usbd, rand() % 256);
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_set_config(usbd, rand() % 256);
      }
      for(a = 0; a < 256; a++)
      {
         test_set_config(usbd, (uint8_t)a);
      }
      for(a = 0; a < 256; a++)
      {
         prepare_device(usbd, i);
         test_set_config(usbd, (uint8_t)a);
      }
   }
}

void test_cmd_set_interface         (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;
   uint8_t if_table_size = USBD_MAX_NUM_INTERFACES;
   USBD_Bool_DT bind_if_table = true;

   printf("test_set_interface test\n\r");

   for(i = 0; i < 6; i++)
   {
      bind_if_table = true;
      if(bind_if_table)
      {
         if_table_size = USBD_MAX_NUM_INTERFACES;
      }
      else
      {
         if_table_size = 0;
      }
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 0, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 0, 1);
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 0, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 0, 3);
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 1, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 1, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 2, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 2, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 3, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 3, 4);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 0, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 0, 1);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 0, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 0, 3);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 1, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 1, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 2, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 2, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 3, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 3, 4);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 0, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 0, 1);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 0, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 0, 3);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 1, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 1, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 2, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 2, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 3, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 3, 4);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 0, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 0, 1);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 0, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 0, 3);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 1, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 1, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 2, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 2, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 3, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 3, 4);

      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 0, 0);
      test_set_interface(usbd, 0, 0, 1);
      test_set_interface(usbd, 0, 0, 2);
      test_set_interface(usbd, 0, 0, 3);
      test_set_interface(usbd, 0, 1, 0);
      test_set_interface(usbd, 0, 1, 2);
      test_set_interface(usbd, 0, 2, 0);
      test_set_interface(usbd, 0, 2, 2);
      test_set_interface(usbd, 0, 3, 0);
      test_set_interface(usbd, 0, 3, 4);
      bind_if_table = true;
      if(bind_if_table)
      {
         if_table_size = 1;
      }
      else
      {
         if_table_size = 0;
      }
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 0, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 0, 1);
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 0, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 0, 3);
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 1, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 1, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 2, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 2, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 3, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 0, 3, 4);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 0, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 0, 1);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 0, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 0, 3);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 1, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 1, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 2, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 2, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 3, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 1, 3, 4);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 0, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 0, 1);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 0, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 0, 3);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 1, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 1, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 2, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 2, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 3, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 2, 3, 4);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 0, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 0, 1);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 0, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 0, 3);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 1, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 1, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 2, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 2, 2);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 3, 0);
      prepare_device(usbd, i);
      test_set_interface(usbd, 3, 3, 4);
      bind_if_table = true;
      if(bind_if_table)
      {
         if_table_size = USBD_MAX_NUM_INTERFACES;
      }
      else
      {
         if_table_size = 0;
      }
      prepare_device(usbd, i);
      for(j = 0; j < number_of_repeats; j++)
      {
         test_set_interface(usbd, x_u8(), x_u8(), x_u8());
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         bind_if_table = true;
         if(bind_if_table)
         {
            if_table_size = (x_u8() % (USBD_MAX_NUM_INTERFACES - 1)) + 1;
         }
         else
         {
            if_table_size = 0;
         }
         prepare_device(usbd, i);
         test_set_interface(usbd, x_u8(), x_u8(), x_u8());
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         bind_if_table = false;
         if(bind_if_table)
         {
            if_table_size = (x_u8() % (USBD_MAX_NUM_INTERFACES - 1)) + 1;
         }
         else
         {
            if_table_size = 0;
         }
         prepare_device(usbd, i);
         test_set_interface(usbd, x_u8(), x_u8(), x_u8());
      }

      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 8; b++)
         {
            for(c = 0; c < 8; c++)
            {
               for(d = 0; d < 4; d++)
               {
                  if(0 == d)
                  {
                     bind_if_table = false;
                  }
                  else
                  {
                     bind_if_table = true;
                  }
                  if(bind_if_table)
                  {
                     if_table_size = d;
                  }
                  else
                  {
                     if_table_size = 0;
                  }
                  prepare_device(usbd, i);
                  test_set_interface(usbd, a, b, c);
               }
            }
         }
      }
   }
}

void test_cmd_set_feature_device      (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_set_feature_device test\n\r");

   for(i = 0; i < 6; i++)
   {
      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % 64), 2);
      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % (128 - 64)) + 64, 2);
      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % (196 - 128)) + 128, 2);
      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % (256 - 196)) + 196, 2);
      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % 64), 2);
      test_set_feature_device(usbd, (rand() % 64), 2);
      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % (128 - 64)) + 64, 2);
      test_set_feature_device(usbd, (rand() % (128 - 64)) + 64, 2);
      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % (196 - 128)) + 128, 2);
      test_set_feature_device(usbd, (rand() % (196 - 128)) + 128, 2);
      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % (256 - 196)) + 196, 2);
      test_set_feature_device(usbd, (rand() % (256 - 196)) + 196, 2);
      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % 64), 2);
      test_set_feature_device(usbd, (rand() % (128 - 64)) + 64, 2);
      test_set_feature_device(usbd, (rand() % (196 - 128)) + 128, 2);
      test_set_feature_device(usbd, (rand() % (256 - 196)) + 196, 2);
      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % 64), 2);
      test_set_feature_device(usbd, (rand() % 64), 2);
      test_set_feature_device(usbd, (rand() % (128 - 64)) + 64, 2);
      test_set_feature_device(usbd, (rand() % (128 - 64)) + 64, 2);
      test_set_feature_device(usbd, (rand() % (196 - 128)) + 128, 2);
      test_set_feature_device(usbd, (rand() % (196 - 128)) + 128, 2);
      test_set_feature_device(usbd, (rand() % (256 - 196)) + 196, 2);
      test_set_feature_device(usbd, (rand() % (256 - 196)) + 196, 2);
      test_set_feature_device(usbd, (rand() % 64), 2);
      test_set_feature_device(usbd, (rand() % (128 - 64)) + 64, 2);
      test_set_feature_device(usbd, (rand() % (196 - 128)) + 128, 2);
      test_set_feature_device(usbd, (rand() % (256 - 196)) + 196, 2);
      test_set_feature_device(usbd, (rand() % 64), 2);
      test_set_feature_device(usbd, (rand() % 64), 2);
      test_set_feature_device(usbd, (rand() % (128 - 64)) + 64, 2);
      test_set_feature_device(usbd, (rand() % (128 - 64)) + 64, 2);
      test_set_feature_device(usbd, (rand() % (196 - 128)) + 128, 2);
      test_set_feature_device(usbd, (rand() % (196 - 128)) + 128, 2);
      test_set_feature_device(usbd, (rand() % (256 - 196)) + 196, 2);
      test_set_feature_device(usbd, (rand() % (256 - 196)) + 196, 2);


      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % 64), 1);
      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % (128 - 64)) + 64, 1);
      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % (196 - 128)) + 128, 1);
      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % (256 - 196)) + 196, 1);
      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % 64), 1);
      test_set_feature_device(usbd, (rand() % 64), 1);
      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % (128 - 64)) + 64, 1);
      test_set_feature_device(usbd, (rand() % (128 - 64)) + 64, 1);
      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % (196 - 128)) + 128, 1);
      test_set_feature_device(usbd, (rand() % (196 - 128)) + 128, 1);
      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % (256 - 196)) + 196, 1);
      test_set_feature_device(usbd, (rand() % (256 - 196)) + 196, 1);
      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % 64), 1);
      test_set_feature_device(usbd, (rand() % (128 - 64)) + 64, 1);
      test_set_feature_device(usbd, (rand() % (196 - 128)) + 128, 1);
      test_set_feature_device(usbd, (rand() % (256 - 196)) + 196, 1);
      prepare_device(usbd, i);
      test_set_feature_device(usbd, (rand() % 64), 1);
      test_set_feature_device(usbd, (rand() % 64), 1);
      test_set_feature_device(usbd, (rand() % (128 - 64)) + 64, 1);
      test_set_feature_device(usbd, (rand() % (128 - 64)) + 64, 1);
      test_set_feature_device(usbd, (rand() % (196 - 128)) + 128, 1);
      test_set_feature_device(usbd, (rand() % (196 - 128)) + 128, 1);
      test_set_feature_device(usbd, (rand() % (256 - 196)) + 196, 1);
      test_set_feature_device(usbd, (rand() % (256 - 196)) + 196, 1);
      test_set_feature_device(usbd, (rand() % 64), 1);
      test_set_feature_device(usbd, (rand() % (128 - 64)) + 64, 1);
      test_set_feature_device(usbd, (rand() % (196 - 128)) + 128, 1);
      test_set_feature_device(usbd, (rand() % (256 - 196)) + 196, 1);
      test_set_feature_device(usbd, (rand() % 64), 1);
      test_set_feature_device(usbd, (rand() % 64), 1);
      test_set_feature_device(usbd, (rand() % (128 - 64)) + 64, 1);
      test_set_feature_device(usbd, (rand() % (128 - 64)) + 64, 1);
      test_set_feature_device(usbd, (rand() % (196 - 128)) + 128, 1);
      test_set_feature_device(usbd, (rand() % (196 - 128)) + 128, 1);
      test_set_feature_device(usbd, (rand() % (256 - 196)) + 196, 1);
      test_set_feature_device(usbd, (rand() % (256 - 196)) + 196, 1);
      for(j = 0; j < number_of_repeats; j++)
      {
         test_set_feature_device(usbd, rand() % 256, rand() % 256);
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_set_feature_device(usbd, rand() % 256, rand() % 256);
      }

      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            test_set_feature_device(usbd, a*64, b);
         }
      }

      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            prepare_device(usbd, i);
            test_set_feature_device(usbd, a*64, b);
         }
      }
   }
}

void test_cmd_set_feature_interface   (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_set_feature_interface test\n\r");

   for(i = 0; i < 6; i++)
   {
      prepare_device(usbd, i);
      test_set_feature_interface(usbd, usbdc1, 0 | 0x0 | 0x0, 0);
      prepare_device(usbd, i);
      test_set_feature_interface(usbd, usbdc1, 0 | 0x0 | 0x1, 0);
      prepare_device(usbd, i);
      test_set_feature_interface(usbd, usbdc1, 0 | 0x8 | 0x0, 0);
      prepare_device(usbd, i);
      test_set_feature_interface(usbd, usbdc1, 0 | 0x8 | 0x1, 0);
      prepare_device(usbd, i);
      test_set_feature_interface(usbd, usbdc1, 64 | 0x0 | 0x0, 0);
      prepare_device(usbd, i);
      test_set_feature_interface(usbd, usbdc1, 64 | 0x0 | 0x1, 0);
      prepare_device(usbd, i);
      test_set_feature_interface(usbd, usbdc1, 64 | 0x8 | 0x0, 0);
      prepare_device(usbd, i);
      test_set_feature_interface(usbd, usbdc1, 64 | 0x8 | 0x1, 0);
      prepare_device(usbd, i);
      test_set_feature_interface(usbd, usbdc1, 128 | 0x0 | 0x0, 0);
      prepare_device(usbd, i);
      test_set_feature_interface(usbd, usbdc1, 128 | 0x0 | 0x1, 0);
      prepare_device(usbd, i);
      test_set_feature_interface(usbd, usbdc1, 128 | 0x8 | 0x0, 0);
      prepare_device(usbd, i);
      test_set_feature_interface(usbd, usbdc1, 128 | 0x8 | 0x1, 0);
      prepare_device(usbd, i);
      test_set_feature_interface(usbd, usbdc1, 196 | 0x0 | 0x0, 0);
      prepare_device(usbd, i);
      test_set_feature_interface(usbd, usbdc1, 196 | 0x0 | 0x1, 0);
      prepare_device(usbd, i);
      test_set_feature_interface(usbd, usbdc1, 196 | 0x8 | 0x0, 0);
      prepare_device(usbd, i);
      test_set_feature_interface(usbd, usbdc1, 196 | 0x8 | 0x1, 0);
      test_set_feature_interface(usbd, usbdc1, 0 | 0x0 | 0x0, 0);
      test_set_feature_interface(usbd, usbdc1, 0 | 0x0 | 0x1, 0);
      test_set_feature_interface(usbd, usbdc1, 0 | 0x8 | 0x0, 0);
      test_set_feature_interface(usbd, usbdc1, 0 | 0x8 | 0x1, 0);
      test_set_feature_interface(usbd, usbdc1, 64 | 0x0 | 0x0, 0);
      test_set_feature_interface(usbd, usbdc1, 64 | 0x0 | 0x1, 0);
      test_set_feature_interface(usbd, usbdc1, 64 | 0x8 | 0x0, 0);
      test_set_feature_interface(usbd, usbdc1, 64 | 0x8 | 0x1, 0);
      test_set_feature_interface(usbd, usbdc1, 128 | 0x0 | 0x0, 0);
      test_set_feature_interface(usbd, usbdc1, 128 | 0x0 | 0x1, 0);
      test_set_feature_interface(usbd, usbdc1, 128 | 0x8 | 0x0, 0);
      test_set_feature_interface(usbd, usbdc1, 128 | 0x8 | 0x1, 0);
      test_set_feature_interface(usbd, usbdc1, 196 | 0x0 | 0x0, 0);
      test_set_feature_interface(usbd, usbdc1, 196 | 0x0 | 0x1, 0);
      test_set_feature_interface(usbd, usbdc1, 196 | 0x8 | 0x0, 0);
      test_set_feature_interface(usbd, usbdc1, 196 | 0x8 | 0x1, 0);
      for(j = 0; j < number_of_repeats; j++)
      {
         test_set_feature_interface(usbd, usbdc1, rand() % 256, rand() % 256);
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_set_feature_interface(usbd, usbdc1, rand() % 256, rand() % 256);
      }

      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            test_set_feature_interface(usbd, usbdc1, a*64, b);
         }
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            prepare_device(usbd, i);
            test_set_feature_interface(usbd, usbdc1, a*64, b);
         }
      }
   }
}

void test_cmd_set_feature_endpoint    (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_set_feature_endpoint test\n\r");

   for(i = 0; i < 6; i++)
   {
      prepare_device(usbd, i);
      for(j = 0; j < 512; j++)
      {
         test_set_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/0, /*dir*/0, /*mask*/x_u8());
         test_set_feature_endpoint(usbd, usbdc1, /*port*/1, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/0, /*dir*/0, /*mask*/x_u8());
         test_set_feature_endpoint(usbd, usbdc1, /*port*/2, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/0, /*dir*/0, /*mask*/x_u8());
         test_set_feature_endpoint(usbd, usbdc1, /*port*/3, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/0, /*dir*/0, /*mask*/x_u8());
         test_set_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/0, /*dir*/1, /*mask*/x_u8());
         test_set_feature_endpoint(usbd, usbdc1, /*port*/1, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/0, /*dir*/1, /*mask*/x_u8());
         test_set_feature_endpoint(usbd, usbdc1, /*port*/2, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/0, /*dir*/1, /*mask*/x_u8());
         test_set_feature_endpoint(usbd, usbdc1, /*port*/3, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/0, /*dir*/1, /*mask*/x_u8());
         /* correct port parsing */
         /* mask off */
         test_set_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/1, /*dir*/0, /*mask*/0);
         test_set_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/1, /*dir*/1, /*mask*/0);
         /* mask on */
         /* ep out of interface or interface irq not connected */
         test_set_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/2, /*if_irq*/0, /*ep*/1, /*dir*/0, /*mask*/1);
         test_set_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/0, /*if_irq*/1, /*ep*/1, /*dir*/0, /*mask*/1);
         test_set_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/2, /*if_irq*/1, /*ep*/1, /*dir*/0, /*mask*/1);
         test_set_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/2, /*if_irq*/0, /*ep*/1, /*dir*/1, /*mask*/1);
         test_set_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/0, /*if_irq*/1, /*ep*/1, /*dir*/1, /*mask*/1);
         test_set_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/2, /*if_irq*/1, /*ep*/1, /*dir*/1, /*mask*/1);
         /* correct condition */
         test_set_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/0, /*if_irq*/0, /*ep*/1, /*dir*/0, /*mask*/1);
         test_set_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/0, /*if_irq*/0, /*ep*/1, /*dir*/1, /*mask*/1);

         /* port blocks request */
         test_set_feature_endpoint(usbd, usbdc1, /*port*/1, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/1, /*dir*/0, /*mask*/x_u8());
         test_set_feature_endpoint(usbd, usbdc1, /*port*/1, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/1, /*dir*/1, /*mask*/x_u8());
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         test_set_feature_endpoint(usbd, usbdc1, /*port*/x_u8(), /*if*/x_u8(), /*if_irq*/x_u8(), /*ep*/x_u8(), /*dir*/x_u8(), /*mask*/x_u8());
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_set_feature_endpoint(usbd, usbdc1, /*port*/x_u8(), /*if*/x_u8(), /*if_irq*/x_u8(), /*ep*/x_u8(), /*dir*/x_u8(), /*mask*/x_u8());
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 8; b++)
         {
            for(c = 0; c < 8; c++)
            {
               for(d = 0; d < 32; d++)
               {
                  for(e = 0; e < 2; e++)
                  {
                     for(f = 0; f < 2; f++)
                     {
                        test_set_feature_endpoint(usbd, usbdc1, /*port*/a, /*if*/b, /*if_irq*/c, /*ep*/d, /*dir*/e, /*mask*/f);
                     }
                  }
               }
            }
         }
      }
   }
}

void test_cmd_clear_feature_device    (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_clear_feature_device test\n\r");

   for(i = 0; i < 6; i++)
   {
      printf("iteration %d\n\r", i);
      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % 64), 2);
      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % (128 - 64)) + 64, 2);
      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % (196 - 128)) + 128, 2);
      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % (256 - 196)) + 196, 2);
      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % 64), 2);
      test_clear_feature_device(usbd, (rand() % 64), 2);
      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % (128 - 64)) + 64, 2);
      test_clear_feature_device(usbd, (rand() % (128 - 64)) + 64, 2);
      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % (196 - 128)) + 128, 2);
      test_clear_feature_device(usbd, (rand() % (196 - 128)) + 128, 2);
      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % (256 - 196)) + 196, 2);
      test_clear_feature_device(usbd, (rand() % (256 - 196)) + 196, 2);
      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % 64), 2);
      test_clear_feature_device(usbd, (rand() % (128 - 64)) + 64, 2);
      test_clear_feature_device(usbd, (rand() % (196 - 128)) + 128, 2);
      test_clear_feature_device(usbd, (rand() % (256 - 196)) + 196, 2);
      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % 64), 2);
      test_clear_feature_device(usbd, (rand() % 64), 2);
      test_clear_feature_device(usbd, (rand() % (128 - 64)) + 64, 2);
      test_clear_feature_device(usbd, (rand() % (128 - 64)) + 64, 2);
      test_clear_feature_device(usbd, (rand() % (196 - 128)) + 128, 2);
      test_clear_feature_device(usbd, (rand() % (196 - 128)) + 128, 2);
      test_clear_feature_device(usbd, (rand() % (256 - 196)) + 196, 2);
      test_clear_feature_device(usbd, (rand() % (256 - 196)) + 196, 2);
      test_clear_feature_device(usbd, (rand() % 64), 2);
      test_clear_feature_device(usbd, (rand() % (128 - 64)) + 64, 2);
      test_clear_feature_device(usbd, (rand() % (196 - 128)) + 128, 2);
      test_clear_feature_device(usbd, (rand() % (256 - 196)) + 196, 2);
      test_clear_feature_device(usbd, (rand() % 64), 2);
      test_clear_feature_device(usbd, (rand() % 64), 2);
      test_clear_feature_device(usbd, (rand() % (128 - 64)) + 64, 2);
      test_clear_feature_device(usbd, (rand() % (128 - 64)) + 64, 2);
      test_clear_feature_device(usbd, (rand() % (196 - 128)) + 128, 2);
      test_clear_feature_device(usbd, (rand() % (196 - 128)) + 128, 2);
      test_clear_feature_device(usbd, (rand() % (256 - 196)) + 196, 2);
      test_clear_feature_device(usbd, (rand() % (256 - 196)) + 196, 2);


      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % 64), 1);
      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % (128 - 64)) + 64, 1);
      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % (196 - 128)) + 128, 1);
      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % (256 - 196)) + 196, 1);
      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % 64), 1);
      test_clear_feature_device(usbd, (rand() % 64), 1);
      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % (128 - 64)) + 64, 1);
      test_clear_feature_device(usbd, (rand() % (128 - 64)) + 64, 1);
      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % (196 - 128)) + 128, 1);
      test_clear_feature_device(usbd, (rand() % (196 - 128)) + 128, 1);
      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % (256 - 196)) + 196, 1);
      test_clear_feature_device(usbd, (rand() % (256 - 196)) + 196, 1);
      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % 64), 1);
      test_clear_feature_device(usbd, (rand() % (128 - 64)) + 64, 1);
      test_clear_feature_device(usbd, (rand() % (196 - 128)) + 128, 1);
      test_clear_feature_device(usbd, (rand() % (256 - 196)) + 196, 1);
      prepare_device(usbd, i);
      test_clear_feature_device(usbd, (rand() % 64), 1);
      test_clear_feature_device(usbd, (rand() % 64), 1);
      test_clear_feature_device(usbd, (rand() % (128 - 64)) + 64, 1);
      test_clear_feature_device(usbd, (rand() % (128 - 64)) + 64, 1);
      test_clear_feature_device(usbd, (rand() % (196 - 128)) + 128, 1);
      test_clear_feature_device(usbd, (rand() % (196 - 128)) + 128, 1);
      test_clear_feature_device(usbd, (rand() % (256 - 196)) + 196, 1);
      test_clear_feature_device(usbd, (rand() % (256 - 196)) + 196, 1);
      test_clear_feature_device(usbd, (rand() % 64), 1);
      test_clear_feature_device(usbd, (rand() % (128 - 64)) + 64, 1);
      test_clear_feature_device(usbd, (rand() % (196 - 128)) + 128, 1);
      test_clear_feature_device(usbd, (rand() % (256 - 196)) + 196, 1);
      test_clear_feature_device(usbd, (rand() % 64), 1);
      test_clear_feature_device(usbd, (rand() % 64), 1);
      test_clear_feature_device(usbd, (rand() % (128 - 64)) + 64, 1);
      test_clear_feature_device(usbd, (rand() % (128 - 64)) + 64, 1);
      test_clear_feature_device(usbd, (rand() % (196 - 128)) + 128, 1);
      test_clear_feature_device(usbd, (rand() % (196 - 128)) + 128, 1);
      test_clear_feature_device(usbd, (rand() % (256 - 196)) + 196, 1);
      test_clear_feature_device(usbd, (rand() % (256 - 196)) + 196, 1);
      printf("%d repeats - 1\n\r", number_of_repeats);
      for(j = 0; j < number_of_repeats; j++)
      {
         test_clear_feature_device(usbd, rand() % 256, rand() % 256);
      }
      printf("%d repeats - 2\n\r", number_of_repeats);
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_clear_feature_device(usbd, rand() % 256, rand() % 256);
      }
      printf("%d x %d repeats - 3\n\r", a, b);
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            test_clear_feature_device(usbd, a*64, b);
         }
      }
      printf("%d x %d repeats - 4\n\r", a, b);
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            prepare_device(usbd, i);
            test_clear_feature_device(usbd, a*64, b);
         }
      }
   }
}

void test_cmd_clear_feature_interface  (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_clear_feature_interface test\n\r");

   for(i = 0; i < 6; i++)
   {
      prepare_device(usbd, i);
      test_clear_feature_interface(usbd, usbdc1, 0 | 0x0 | 0x0, 0);
      prepare_device(usbd, i);
      test_clear_feature_interface(usbd, usbdc1, 0 | 0x0 | 0x1, 0);
      prepare_device(usbd, i);
      test_clear_feature_interface(usbd, usbdc1, 0 | 0x8 | 0x0, 0);
      prepare_device(usbd, i);
      test_clear_feature_interface(usbd, usbdc1, 0 | 0x8 | 0x1, 0);
      prepare_device(usbd, i);
      test_clear_feature_interface(usbd, usbdc1, 64 | 0x0 | 0x0, 0);
      prepare_device(usbd, i);
      test_clear_feature_interface(usbd, usbdc1, 64 | 0x0 | 0x1, 0);
      prepare_device(usbd, i);
      test_clear_feature_interface(usbd, usbdc1, 64 | 0x8 | 0x0, 0);
      prepare_device(usbd, i);
      test_clear_feature_interface(usbd, usbdc1, 64 | 0x8 | 0x1, 0);
      prepare_device(usbd, i);
      test_clear_feature_interface(usbd, usbdc1, 128 | 0x0 | 0x0, 0);
      prepare_device(usbd, i);
      test_clear_feature_interface(usbd, usbdc1, 128 | 0x0 | 0x1, 0);
      prepare_device(usbd, i);
      test_clear_feature_interface(usbd, usbdc1, 128 | 0x8 | 0x0, 0);
      prepare_device(usbd, i);
      test_clear_feature_interface(usbd, usbdc1, 128 | 0x8 | 0x1, 0);
      prepare_device(usbd, i);
      test_clear_feature_interface(usbd, usbdc1, 196 | 0x0 | 0x0, 0);
      prepare_device(usbd, i);
      test_clear_feature_interface(usbd, usbdc1, 196 | 0x0 | 0x1, 0);
      prepare_device(usbd, i);
      test_clear_feature_interface(usbd, usbdc1, 196 | 0x8 | 0x0, 0);
      prepare_device(usbd, i);
      test_clear_feature_interface(usbd, usbdc1, 196 | 0x8 | 0x1, 0);
      test_clear_feature_interface(usbd, usbdc1, 0 | 0x0 | 0x0, 0);
      test_clear_feature_interface(usbd, usbdc1, 0 | 0x0 | 0x1, 0);
      test_clear_feature_interface(usbd, usbdc1, 0 | 0x8 | 0x0, 0);
      test_clear_feature_interface(usbd, usbdc1, 0 | 0x8 | 0x1, 0);
      test_clear_feature_interface(usbd, usbdc1, 64 | 0x0 | 0x0, 0);
      test_clear_feature_interface(usbd, usbdc1, 64 | 0x0 | 0x1, 0);
      test_clear_feature_interface(usbd, usbdc1, 64 | 0x8 | 0x0, 0);
      test_clear_feature_interface(usbd, usbdc1, 64 | 0x8 | 0x1, 0);
      test_clear_feature_interface(usbd, usbdc1, 128 | 0x0 | 0x0, 0);
      test_clear_feature_interface(usbd, usbdc1, 128 | 0x0 | 0x1, 0);
      test_clear_feature_interface(usbd, usbdc1, 128 | 0x8 | 0x0, 0);
      test_clear_feature_interface(usbd, usbdc1, 128 | 0x8 | 0x1, 0);
      test_clear_feature_interface(usbd, usbdc1, 196 | 0x0 | 0x0, 0);
      test_clear_feature_interface(usbd, usbdc1, 196 | 0x0 | 0x1, 0);
      test_clear_feature_interface(usbd, usbdc1, 196 | 0x8 | 0x0, 0);
      test_clear_feature_interface(usbd, usbdc1, 196 | 0x8 | 0x1, 0);
      for(j = 0; j < number_of_repeats; j++)
      {
         test_clear_feature_interface(usbd, usbdc1, rand() % 256, rand() % 256);
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_clear_feature_interface(usbd, usbdc1, rand() % 256, rand() % 256);
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            test_clear_feature_interface(usbd, usbdc1, a*64, b);
         }
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            prepare_device(usbd, i);
            test_clear_feature_interface(usbd, usbdc1, a*64, b);
         }
      }
   }
}

void test_cmd_clear_feature_endpoint   (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_clear_feature_endpoint test\n\r");

   for(i = 0; i < 6; i++)
   {
      prepare_device(usbd, i);
      for(j = 0; j < 512; j++)
      {
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/0, /*dir*/0, /*mask*/x_u8());
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/1, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/0, /*dir*/0, /*mask*/x_u8());
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/2, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/0, /*dir*/0, /*mask*/x_u8());
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/3, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/0, /*dir*/0, /*mask*/x_u8());
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/0, /*dir*/1, /*mask*/x_u8());
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/1, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/0, /*dir*/1, /*mask*/x_u8());
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/2, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/0, /*dir*/1, /*mask*/x_u8());
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/3, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/0, /*dir*/1, /*mask*/x_u8());
         /* correct port parsing */
         /* mask off */
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/1, /*dir*/0, /*mask*/0);
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/1, /*dir*/1, /*mask*/0);
         /* mask on */
         /* ep out of interface or interface irq not connected */
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/2, /*if_irq*/0, /*ep*/1, /*dir*/0, /*mask*/1);
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/0, /*if_irq*/1, /*ep*/1, /*dir*/0, /*mask*/1);
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/2, /*if_irq*/1, /*ep*/1, /*dir*/0, /*mask*/1);
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/2, /*if_irq*/0, /*ep*/1, /*dir*/1, /*mask*/1);
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/0, /*if_irq*/1, /*ep*/1, /*dir*/1, /*mask*/1);
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/2, /*if_irq*/1, /*ep*/1, /*dir*/1, /*mask*/1);
         /* correct condition */
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/0, /*if_irq*/0, /*ep*/1, /*dir*/0, /*mask*/1);
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/0, /*if*/0, /*if_irq*/0, /*ep*/1, /*dir*/1, /*mask*/1);

         /* port blocks request */
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/1, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/1, /*dir*/0, /*mask*/x_u8());
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/1, /*if*/x_if(), /*if_irq*/x_u8(), /*ep*/1, /*dir*/1, /*mask*/x_u8());
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/x_u8(), /*if*/x_u8(), /*if_irq*/x_u8(), /*ep*/x_u8(), /*dir*/x_u8(), /*mask*/x_u8());
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/x_u8(), /*if*/x_u8(), /*if_irq*/x_u8(), /*ep*/x_u8(), /*dir*/x_u8(), /*mask*/x_u8());
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/x_u8(), /*if*/x_u8(), /*if_irq*/x_u8(), /*ep*/x_u8(), /*dir*/x_u8(), /*mask*/x_u8());
         test_clear_feature_endpoint(usbd, usbdc1, /*port*/x_u8(), /*if*/x_u8(), /*if_irq*/x_u8(), /*ep*/x_u8(), /*dir*/x_u8(), /*mask*/x_u8());
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 8; b++)
         {
            for(c = 0; c < 8; c++)
            {
               for(d = 0; d < 32; d++)
               {
                  for(e = 0; e < 2; e++)
                  {
                     for(f = 0; f < 2; f++)
                     {
                        test_clear_feature_endpoint(usbd, usbdc1, /*port*/a, /*if*/b, /*if_irq*/c, /*ep*/d, /*dir*/e, /*mask*/f);
                     }
                  }
               }
            }
         }
      }
   }
}

void test_cmd_get_status_device      (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_get_status_device test\n\r");

   for(i = 0; i < 6; i++)
   {
      prepare_device(usbd, i);
      /* case: port handler installed and returns true */
      test_get_status_device(usbd, 0, 1, 1, 1, &(main_port.handlers.get_device_status));
      test_get_status_device(usbd, 0, 0, 1, 1, &(main_port.handlers.get_device_status));
      /* case: port handler installed but returns false */
      test_get_status_device(usbd, 1, 1, 1, 1, &(main_port.handlers.get_device_status));
      test_get_status_device(usbd, 1, 0, 1, 1, &(main_port.handlers.get_device_status));
      /* case: port handler/callbacks structure not installed */
      test_get_status_device(usbd, 2, 1, 1, 1, &(main_port.handlers.get_device_status));
      test_get_status_device(usbd, 2, 0, 1, 1, &(main_port.handlers.get_device_status));
      test_get_status_device(usbd, 3, 1, 1, 1, &(main_port.handlers.get_device_status));
      test_get_status_device(usbd, 3, 0, 1, 1, &(main_port.handlers.get_device_status));
      for(j = 0; j < number_of_repeats; j++)
      {
         test_get_status_device(usbd, x_u8(), x_u8(), x_u8(), x_u8(), &(main_port.handlers.get_device_status));
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_get_status_device(usbd, x_u8(), x_u8(), x_u8(), x_u8(), &(main_port.handlers.get_device_status));
      }
      USBD_DEV_SET_PORT_PTR(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT));
      for(j = 0; j < number_of_repeats; j++)
      {
         test_get_status_device(usbd, x_u8(), x_u8(), x_u8(), x_u8(), &(main_port.handlers.get_device_status));
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 2; b++)
         {
            for(c = 0; c < 2; c++)
            {
               for(d = 0; d < 2; d++)
               {
                  prepare_device(usbd, i);
                  test_get_status_device(usbd, a, b, c, d, &(main_port.handlers.get_device_status));
               }
            }
         }
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 2; b++)
         {
            for(c = 0; c < 2; c++)
            {
               for(d = 0; d < 2; d++)
               {
                  test_get_status_device(usbd, a, b, c, d, &(main_port.handlers.get_device_status));
               }
            }
         }
      }
   }
}

void test_cmd_get_status_interface    (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_get_status_interface test" "\n\r");

   for(i = 0; i < 6; i++)
   {
      prepare_device(usbd, i);
      /* case: port handler installed and returns true */
      test_get_status_interface(usbd, 0, x_u8() % TEST_NUM_INTERFACES);
      test_get_status_interface(usbd, 0, (x_u8() % TEST_NUM_INTERFACES) + TEST_NUM_INTERFACES);
      /* case: port handler installed but returns false */
      test_get_status_interface(usbd, 1, x_u8() % TEST_NUM_INTERFACES);
      test_get_status_interface(usbd, 1, (x_u8() % TEST_NUM_INTERFACES) + TEST_NUM_INTERFACES);
      /* case: port handler/callbacks structure not installed */
      test_get_status_interface(usbd, 2, x_u8() % TEST_NUM_INTERFACES);
      test_get_status_interface(usbd, 2, (x_u8() % TEST_NUM_INTERFACES) + TEST_NUM_INTERFACES);
      test_get_status_interface(usbd, 3, x_u8() % TEST_NUM_INTERFACES);
      test_get_status_interface(usbd, 3, (x_u8() % TEST_NUM_INTERFACES) + TEST_NUM_INTERFACES);
      for(j = 0; j < number_of_repeats; j++)
      {
         test_get_status_interface(usbd, x_u8(), x_u8());
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_get_status_interface(usbd, x_u8(), x_u8());
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            prepare_device(usbd, i);
            test_get_status_interface(usbd, a, b);
         }
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            test_get_status_interface(usbd, a, b);
         }
      }
   }
}

void test_cmd_get_status_endpoint     (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_get_status_endpoint test" "\n\r");

   for(i = 0; i < 6; i++)
   {
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 0 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 0 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 0 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 0 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 1 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 1 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 1 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 1 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 111 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 111 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 111 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 111 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 0 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 0 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 0 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 0 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 1 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 1 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 1 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 1 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 111 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 111 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 111 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 111 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 0 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 0 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 0 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 0 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 1 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 1 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 1 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 1 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 111 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 111 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 111 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 111 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 0 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 0 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 0 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 0 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 1 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 1 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 1 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 1 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 111 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 111 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 111 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 111 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 0 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 0 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 0 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 0 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 1 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 1 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 1 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 1 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 111 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 111 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 111 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 111 /*ep_num*/, 0/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 0 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 0 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 0 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 0 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 1 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 1 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 1 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 1 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 111 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 111 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 111 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 111 /*ep_num*/, 0/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 0 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 0 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 0 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 0 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 1 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 1 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 1 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 1 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 111 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 111 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 111 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 111 /*ep_num*/, 1/*dir*/, 0 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 0 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 0 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 0 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 0 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 1 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 1 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 1 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 1 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 0 /*port_param*/, 111 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 1 /*port_param*/, 111 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 2 /*port_param*/, 111 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      prepare_device(usbd, i);
      test_get_status_endpoint(usbd, 3 /*port_param*/, 111 /*ep_num*/, 1/*dir*/, 1 /*halt_state*/);
      for(j = 0; j < number_of_repeats; j++)
      {
         test_get_status_endpoint(usbd, x_u8() /*port_param*/, x_u8() /*ep_num*/, x_u8()/*dir*/, x_u8() /*halt_state*/);
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_get_status_endpoint(usbd, x_u8() /*port_param*/, x_u8() /*ep_num*/, x_u8()/*dir*/, x_u8() /*halt_state*/);
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 128; b++)
         {
            for(c = 0; c < 2; c++)
            {
               for(d = 0; d < 2; d++)
               {
                  prepare_device(usbd, i);
                  test_get_status_endpoint(usbd, a /*port_param*/, b /*ep_num*/, c/*dir*/, d /*halt_state*/);
               }
            }
         }
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 128; b++)
         {
            for(c = 0; c < 2; c++)
            {
               for(d = 0; d < 2; d++)
               {
                  test_get_status_endpoint(usbd, a /*port_param*/, b /*ep_num*/, c/*dir*/, d /*halt_state*/);
               }
            }
         }
      }
   }
}

void test_cmd_get_interface         (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_get_interface test" "\n\r");

   for(i = 0; i < 6; i++)
   {
      prepare_device(usbd, i);
      test_get_interface(usbd, 0 /*port_param*/, 0 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 1 /*port_param*/, 0 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 2 /*port_param*/, 0 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 3 /*port_param*/, 0 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 0 /*port_param*/, 0 /*if_num*/, 5 /*alt*/);
      test_get_interface(usbd, 1 /*port_param*/, 0 /*if_num*/, 5 /*alt*/);
      test_get_interface(usbd, 2 /*port_param*/, 0 /*if_num*/, 5 /*alt*/);
      test_get_interface(usbd, 3 /*port_param*/, 0 /*if_num*/, 5 /*alt*/);
      test_get_interface(usbd, 0 /*port_param*/, 7 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 1 /*port_param*/, 7 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 2 /*port_param*/, 7 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 3 /*port_param*/, 7 /*if_num*/, 0 /*alt*/);

      prepare_device(usbd, i);
      test_get_interface(usbd, 0 /*port_param*/, 2 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 1 /*port_param*/, 2 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 2 /*port_param*/, 2 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 3 /*port_param*/, 2 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 0 /*port_param*/, 0 /*if_num*/, 5 /*alt*/);
      test_get_interface(usbd, 1 /*port_param*/, 0 /*if_num*/, 5 /*alt*/);
      test_get_interface(usbd, 2 /*port_param*/, 0 /*if_num*/, 5 /*alt*/);
      test_get_interface(usbd, 3 /*port_param*/, 0 /*if_num*/, 5 /*alt*/);
      test_get_interface(usbd, 0 /*port_param*/, 7 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 1 /*port_param*/, 7 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 2 /*port_param*/, 7 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 3 /*port_param*/, 7 /*if_num*/, 0 /*alt*/);

      prepare_device(usbd, i);
      test_get_interface(usbd, 0 /*port_param*/, 2 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 1 /*port_param*/, 2 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 2 /*port_param*/, 2 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 3 /*port_param*/, 2 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 0 /*port_param*/, 0 /*if_num*/, 5 /*alt*/);
      test_get_interface(usbd, 1 /*port_param*/, 0 /*if_num*/, 5 /*alt*/);
      test_get_interface(usbd, 2 /*port_param*/, 0 /*if_num*/, 5 /*alt*/);
      test_get_interface(usbd, 3 /*port_param*/, 0 /*if_num*/, 5 /*alt*/);
      test_get_interface(usbd, 0 /*port_param*/, 7 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 1 /*port_param*/, 7 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 2 /*port_param*/, 7 /*if_num*/, 0 /*alt*/);
      test_get_interface(usbd, 3 /*port_param*/, 7 /*if_num*/, 0 /*alt*/);

      prepare_device(usbd, i);
      for(j = 0; j < number_of_repeats; j++)
      {
         test_get_interface(usbd, x_u8() /*port_param*/, x_u8() /*if_num*/, x_u8() /*alt*/);
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_get_interface(usbd, x_u8() /*port_param*/, x_u8() /*if_num*/, x_u8() /*alt*/);
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_get_interface(usbd, x_u8() /*port_param*/, x_u8() /*if_num*/, x_u8() /*alt*/);
      }

      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 8; b++)
         {
            for(c = 0; c < 8; c++)
            {
               prepare_device(usbd, i);
               test_get_interface(usbd, a, b, c);
            }
         }
      }
   }
}

void test_cmd_get_configuration      (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_get_configuration test" "\n\r");

   for(i = 0; i < 6; i++)
   {
      prepare_device(usbd, i);
      /* case: port handler installed and returns true */
      test_get_configuration(usbd, 0, x_u8());
      /* case: port handler installed but returns false */
      test_get_configuration(usbd, 1, x_u8());
      /* case: port handler/callbacks structure not installed */
      test_get_configuration(usbd, 2, x_u8());
      test_get_configuration(usbd, 3, x_u8());
      for(j = 0; j < number_of_repeats; j++)
      {
         test_get_configuration(usbd, x_u8(), x_u8());
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_get_configuration(usbd, x_u8(), x_u8());
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            prepare_device(usbd, i);
            test_get_configuration(usbd, a, b);
         }
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            test_get_configuration(usbd, a, b);
         }
      }
   }
}

void test_cmd_get_desc_device        (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_get_descriptor device test" "\n\r");

   for(i = 0; i < 6; i++)
   {
      prepare_device(usbd, i);
      /* case: port handler installed and returns true */
      test_get_desc_device(usbd, 0, x_u8() % (DEV_DESC_SIZE - 2));
      test_get_desc_device(usbd, 0, (x_u8() % DEV_DESC_SIZE) + DEV_DESC_SIZE + 1);
      /* case: port handler installed but returns false */
      test_get_desc_device(usbd, 1, x_u8() % (DEV_DESC_SIZE - 2));
      test_get_desc_device(usbd, 1, (x_u8() % DEV_DESC_SIZE) + DEV_DESC_SIZE + 1);
      /* case: port handler/callbacks structure not installed */
      test_get_desc_device(usbd, 2, x_u8() % (DEV_DESC_SIZE - 2));
      test_get_desc_device(usbd, 2, (x_u8() % DEV_DESC_SIZE) + DEV_DESC_SIZE + 1);
      test_get_desc_device(usbd, 3, x_u8() % (DEV_DESC_SIZE - 2));
      test_get_desc_device(usbd, 3, (x_u8() % DEV_DESC_SIZE) + DEV_DESC_SIZE + 1);
      for(j = 0; j < number_of_repeats; j++)
      {
         test_get_desc_device(usbd, x_u8(), x_u8());
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_get_desc_device(usbd, x_u8(), x_u8());
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            prepare_device(usbd, i);
            test_get_desc_device(usbd, a, b);
         }
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            test_get_desc_device(usbd, a, b);
         }
      }
   }
}

void test_cmd_get_desc_qualifier      (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_get_descriptor qualifier test" "\n\r");

   for(i = 0; i < 6; i++)
   {
      prepare_device(usbd, i);
      for(j = 0; j < number_of_repeats; j++)
      {
         test_get_desc_qualifier(usbd, x_u8(), x_u8(), x_u8(), &main_port);
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_get_desc_qualifier(usbd, x_u8(), x_u8(), x_u8(), &main_port);
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 64; b++)
         {
            for(c = 0; c < 256; c++)
            {
               prepare_device(usbd, i);
               test_get_desc_qualifier(usbd, a, b, c, &main_port);
            }
         }
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 64; b++)
         {
            for(c = 0; c < 256; c++)
            {
               test_get_desc_qualifier(usbd, a, b, c, &main_port);
            }
         }
      }
   }
}

void test_cmd_get_desc_config        (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_get_descriptor configuration test" "\n\r");

   for(i = 0; i < 6; i++)
   {
      prepare_device(usbd, i);
      for(j = 0; j < number_of_repeats; j++)
      {
         test_get_desc_config(usbd, x_u8(), x_u8(), x_u8());
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_get_desc_config(usbd, x_u8(), x_u8(), x_u8());
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            for(c = 0; c < 256; c++)
            {
               prepare_device(usbd, i);
               test_get_desc_config(usbd, a, b, c);
            }
         }
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 64; b++)
         {
            for(c = 0; c < 256; c++)
            {
               test_get_desc_config(usbd, a, b, c);
            }
         }
      }
   }
}

void test_cmd_get_desc_string        (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_get_descriptor string test" "\n\r");

   for(i = 0; i < 6; i++)
   {
      prepare_device(usbd, i);
      for(j = 0; j < number_of_repeats; j++)
      {
         test_get_desc_string(usbd, x_u8(), x_u8(), x_u8());
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_get_desc_string(usbd, x_u8(), x_u8(), x_u8());
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            for(c = 0; c < 256; c++)
            {
               prepare_device(usbd, i);
               test_get_desc_string(usbd, a, b, c);
            }
         }
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 64; b++)
         {
            for(c = 0; c < 256; c++)
            {
               test_get_desc_string(usbd, a, b, c);
            }
         }
      }
   }
}

void test_cmd_get_desc_other_device   (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_get_descriptor other type device test" "\n\r");

   for(i = 0; i < 6; i++)
   {
      prepare_device(usbd, i);
      for(j = 0; j < number_of_repeats; j++)
      {
         test_get_desc_other_device(usbd, usbdc1, x_u8(), x_u8());
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_get_desc_other_device(usbd, usbdc1, x_u8(), x_u8());
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            prepare_device(usbd, i);
            test_get_desc_other_device(usbd, usbdc1, a, b);
         }
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            test_get_desc_other_device(usbd, usbdc1, a, b);
         }
      }
   }
}

void test_cmd_get_desc_other_interface (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_get_descriptor other type interface test" "\n\r");

   for(i = 0; i < 6; i++)
   {
      prepare_device(usbd, i);
      for(j = 0; j < number_of_repeats; j++)
      {
         test_get_desc_other_interface(usbd, usbdc1, x_u8(), x_u8());
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_get_desc_other_interface(usbd, usbdc1, x_u8(), x_u8());
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            prepare_device(usbd, i);
            test_get_desc_other_interface(usbd, usbdc1, a, b);
         }
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            test_get_desc_other_interface(usbd, usbdc1, a, b);
         }
      }
   }
}

void test_cmd_get_desc_other_endpoint  (IOCMD_Arg_DT *arg)
{
   USBD_Params_XT *usbd = usbd_param_ptr;
   uint8_t i;
   int j;
   uint16_t a,b,c,d,e,f;

   printf("test_get_descriptor other type endpoint test" "\n\r");

   for(i = 0; i < 6; i++)
   {
      prepare_device(usbd, i);
      for(j = 0; j < number_of_repeats; j++)
      {
         test_get_desc_other_endpoint(usbd, usbdc1, x_u8(), x_u8(), x_u8(), x_u8(), x_u8());
      }
      for(j = 0; j < number_of_repeats; j++)
      {
         prepare_device(usbd, i);
         test_get_desc_other_endpoint(usbd, usbdc1, x_u8(), x_u8(), x_u8(), x_u8(), x_u8());
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            for(c = 0; c < (USBD_MAX_NUM_INTERFACES + 1); c++)
            {
               for(d = 0; d < (USBD_MAX_NUM_ENDPOINTS + 1); d++)
               {
                  for(e = 0; e < 2; e++)
                  {
                     prepare_device(usbd, i);
                     test_get_desc_other_endpoint(usbd, usbdc1, a, b, c/* if_num */, d/* ep_num */, e/* dir */);
                  }
               }
            }
         }
      }
      for(a = 0; a < 4; a++)
      {
         for(b = 0; b < 256; b++)
         {
            for(c = 0; c < 8; c++)
            {
               for(d = 0; d < 32; d++)
               {
                  for(e = 0; e < 4; e++)
                  {
                     test_get_desc_other_endpoint(usbd, usbdc1, a, b, c, d, e);
                  }
               }
            }
         }
      }
   }
}



static IOCMD_Command_Tree_XT cmd_tree[] =
{
   IOCMD_ELEM(          ""                   , test_cmd_all                      , "executes all tests"),
   IOCMD_GROUP_START(   "set"                ,                                     "group of commands for testing 'set' usb requests"),
      IOCMD_ELEM(          "addr"            , test_cmd_set_addr                 , "executes set_addr tests"),
      IOCMD_ELEM(          "config"          , test_cmd_set_config               , "executes set_config tests"),
      IOCMD_ELEM(          "interface"       , test_cmd_set_interface            , "executes set_interface tests"),
      IOCMD_GROUP_START(  "feature"          ,                                     "group of commands for testing 'set feature' usb requests"),
         IOCMD_ELEM(          "device"       , test_cmd_set_feature_device       , "executes set_feature_device   tests"),
         IOCMD_ELEM(          "interface"    , test_cmd_set_feature_interface    , "executes set_feature_interface tests"),
         IOCMD_ELEM(          "endpoint"     , test_cmd_set_feature_endpoint     , "executes set_feature_endpoint  tests"),
      IOCMD_GROUP_END(),
   IOCMD_GROUP_END(),
   IOCMD_GROUP_START(  "clear"               ,                                     "group of commands for testing 'clear' usb requests"),
      IOCMD_GROUP_START(  "feature"          ,                                     "group of commands for testing 'set feature' usb requests"),
         IOCMD_ELEM(          "device"       , test_cmd_clear_feature_device     , "executes clear_feature_device   tests"),
         IOCMD_ELEM(          "interface"    , test_cmd_clear_feature_interface  , "executes clear_feature_interface tests"),
         IOCMD_ELEM(          "endpoint"     , test_cmd_clear_feature_endpoint   , "executes clear_feature_endpoint  tests"),
      IOCMD_GROUP_END(),
   IOCMD_GROUP_END(),
   IOCMD_GROUP_START(  "get"                 ,                                     "group of commands for testing 'get' usb requests"),
      IOCMD_GROUP_START(  "status"           ,                                     "group of commands for testing 'get status' usb requests"),
         IOCMD_ELEM(          "device"       , test_cmd_get_status_device        , "executes get_status_device   tests"),
         IOCMD_ELEM(          "interface"    , test_cmd_get_status_interface     , "executes get_status_interface tests"),
         IOCMD_ELEM(          "endpoint"     , test_cmd_get_status_endpoint      , "executes get_status_endpoint  tests"),
      IOCMD_GROUP_END(),
      IOCMD_ELEM(          "interface"       , test_cmd_get_interface            , "executes get_interface tests"),
      IOCMD_ELEM(          "configuration"   , test_cmd_get_configuration        , "executes get_configuration tests"),
      IOCMD_GROUP_START(   "desc"            ,                                     "group of commands for testing 'get desc' usb requests"),
         IOCMD_ELEM(          "device"       , test_cmd_get_desc_device          , "executes get_desc_device tests"),
         IOCMD_ELEM(          "qualifier"    , test_cmd_get_desc_qualifier       , "executes get_desc_qualifier tests"),
         IOCMD_ELEM(          "config"       , test_cmd_get_desc_config          , "executes get_desc_config tests"),
         IOCMD_ELEM(          "string"       , test_cmd_get_desc_string          , "executes get_desc_string tests"),
         IOCMD_GROUP_START(   "other"        ,                                     "group of commands for testing 'get desc other' usb requests"),
            IOCMD_ELEM(          "device"    , test_cmd_get_desc_other_device    , "executes get_desc_other_device tests"),
            IOCMD_ELEM(          "interface" ,test_cmd_get_desc_other_interface  , "executes get_desc_other_interface tests"),
            IOCMD_ELEM(          "endpoint"  , test_cmd_get_desc_other_endpoint  , "executes get_desc_other_endpoint tests"),
         IOCMD_GROUP_END(),
      IOCMD_GROUP_END(),
   IOCMD_GROUP_END(),
   IOCMD_ELEM(          "tplist"             , test_cmd_tp_list                  , "lists all TPs that can be used in test"),
   IOCMD_ELEM(          "loop"               , test_cmd_loop                     , "allows executing commands in loop"),
   IOCMD_ELEM(          "exit"               , test_cmd_exit                     , "exits loop commands executing"),
   IOCMD_GROUP_START(   "logs"               ,                                     "group of commands logs manipulation"),
      IOCMD_ELEM(          "enable"          , test_cmd_logs_enable              , "enables all logs"),
      IOCMD_ELEM(          "disable"         , test_cmd_logs_disable             , "disables all logs"),
   IOCMD_GROUP_END(),
   IOCMD_GROUP_START(   "entr"               ,                                     "group of commands entrances logging manipulation"),
      IOCMD_ELEM(          "enable"          , test_cmd_entr_enable              , "enables all entrances"),
      IOCMD_ELEM(          "disable"         , test_cmd_entr_disable             , "disables all entrances"),
   IOCMD_GROUP_END(),
   IOCMD_ELEM(          "print"             , test_cmd_print                     , "allows executing commands in loop"),
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
   uint16_t a,b,c,d,e,f;
   uint8_t i, ep_num;
   uint32_t cntr;

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

   number_of_repeats = rand() % TEST_LOOP_MAX_NUM_REPEATS;

   do
   {
      usbd_param_ptr = &usbd;
      if(1 == num_args)
      {
         arg_tab[1] = "all";
         num_args = 2;
      }
      IOCMD_Parse_Command(num_args - 1, (const char**)(&arg_tab[1]), main_get_exe(), cmd_tree, Num_Elems(cmd_tree), IOCMD_TRUE);
      printf("\n\rplease put next command:\n\n\r");
      fgets(line, sizeof(line) - 1, stdin);
      printf("command:\"%s\"\n\r", line);
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

      IOCMD_Proc_Buffered_Logs(true, main_get_exe(), working_buf, sizeof(working_buf));\
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

   return 0;
}
