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

#ifndef USBD_IOTP_INTERNAL_H_
#include "usbd_iotp_internal.h"
#endif

#include "cfg.h"

#include "main_get_exe.h"

#include "iotp_test_init.h"
#include "iotp_test_send_irq.h"
#include "iotp_test_send_irq_ready_buf_empty.h"
#include "iotp_test_send_irq_abort_by_user.h"
#include "iotp_test_send_irq_abort_by_reconfigure.h"
#include "iotp_test_send_status_irq.h"
#include "iotp_test_recv_ready_irq.h"
#include "iotp_test_recv_wait_irq.h"
#include "iotp_test_recv_irq_next.h"
#include "iotp_test_recv_ready_on_irq.h"
#include "iotp_test_recv_wait_on_irq.h"
#include "iotp_test_recv_ready_irq_too_less_space.h"
#include "iotp_test_recv_wait_irq_too_less_space.h"
#include "iotp_test_recv_ready_irq_shorter_msg.h"
#include "iotp_test_recv_wait_irq_shorter_msg.h"
#include "iotp_test_recv_ready_irq_abort_by_user.h"
#include "iotp_test_recv_wait_irq_abort_by_user.h"
#include "iotp_test_recv_ready_irq_abort_by_reconfigure.h"
#include "iotp_test_recv_wait_irq_abort_by_reconfigure.h"

#include <stdlib.h>


#define TEST_LOOP_MAX_NUM_REPEATS       0x7FFF
#define TEST_LOOP_MIN_NUM_REPEATS       0x3FF

#define FUNC_ENTER_NOTIF        (false)

#define USE_INVALID_CONFIGS     (false)


#define DEV_DESC_SIZE           (sizeof(USB_device_desc_DT) / sizeof(uint8_t))



typedef struct main_test_iotp_collection_eXtended_Tag
{
    USBD_IOTP_Params_XT  *iotp;
    USB_EP_Direction_ET        dir;
    uint8_t                     ep_num;
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

void test_cmd_tp_list                (IOCMD_Arg_DT *arg);
void test_cmd_all                    (IOCMD_Arg_DT *arg);
void test_cmd_all_in                 (IOCMD_Arg_DT *arg);
void test_cmd_all_out                (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_all            (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_in_all         (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_out_all        (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_all           (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_in_all        (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_out_all       (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_all           (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_in_all        (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_out_all       (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_in_send        (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_in_send1c      (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_in_send2c      (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_in_abort_user  (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_in_abort_rec   (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_in_status      (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_in_send       (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_in_send1c     (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_in_send2c     (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_in_abort_user (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_in_abort_rec  (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_in_status     (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_in_send       (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_in_send1c     (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_in_send2c     (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_in_abort_user (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_in_abort_rec  (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_in_status     (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_out_ready      (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_out_wait       (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_out_1c         (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_out_2c         (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_out_3c         (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_out_onready    (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_out_onwait     (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_out_lsready    (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_out_lswait     (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_out_shready    (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_out_shwait     (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_out_rd_auser   (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_out_wt_auser   (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_out_rd_rec     (IOCMD_Arg_DT *arg);
void test_cmd_not_dma_out_wt_rec     (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_out_ready     (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_out_wait      (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_out_1c        (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_out_2c        (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_out_3c        (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_out_onready   (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_out_onwait    (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_out_lsready   (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_out_lswait    (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_out_shready   (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_out_shwait    (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_out_rd_auser  (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_out_wt_auser  (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_out_rd_rec    (IOCMD_Arg_DT *arg);
void test_cmd_half_dma_out_wt_rec    (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_out_ready     (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_out_wait      (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_out_1c        (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_out_2c        (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_out_3c        (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_out_onready   (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_out_onwait    (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_out_lsready   (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_out_lswait    (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_out_shready   (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_out_shwait    (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_out_rd_auser  (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_out_wt_auser  (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_out_rd_rec    (IOCMD_Arg_DT *arg);
void test_cmd_true_dma_out_wt_rec    (IOCMD_Arg_DT *arg);
void test_cmd_loop                   (IOCMD_Arg_DT *arg);
void test_cmd_exit                   (IOCMD_Arg_DT *arg);
void test_cmd_logs_enable            (IOCMD_Arg_DT *arg);
void test_cmd_logs_disable           (IOCMD_Arg_DT *arg);
void test_cmd_entr_enable            (IOCMD_Arg_DT *arg);
void test_cmd_entr_disable           (IOCMD_Arg_DT *arg);
void test_cmd_print                  (IOCMD_Arg_DT *arg);

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
static iotp_test_send_irq_transaction_req_XT iotp_in_transactions[TEST_MAX_NUM_CONNECTED_TRANSACTIONS];
static iotp_test_recv_irq_transaction_req_XT iotp_out_transactions[TEST_MAX_NUM_CONNECTED_TRANSACTIONS];

static USBD_Params_XT *usbd_param_ptr;

static uint8_t desc1[] =
{
   USB_CONFIGURATION_DESC_TABLE_FILL(
      /* _wTotalLength */        0,
      /* _bNumInterfaces */      1,
      /* _bConfigurationValue */ 1,
      /* _iConfiguration */      4,
      /* _self_powered */        USB_CFG_DESC_SELF_POWERED,
      /* _remote_wakeup */       USB_CFG_DESC_NO_REMOTE_WAKEUP,
      /* _max_current_mA */      25
   ),

   USB_INTERFACE_DESC_TABLE_FILL(
      /* _bInterfaceNumber */    0,
      /* _bAlternateSetting */   0,
      /* _bNumEndpoints */       14,
      /* _bInterfaceClass */     1,
      /* _bFunctionSubClass */   2,
      /* _bInterfaceProtocol */  3,
      /* _iInterface */          0
   ),

   USB_ENDPOINT_DESC_TABLE_FILL(
      /* _ep_num */              1,
      /* _ep_dir */              USB_EP_DESC_DIR_IN,
      /* _ep_type */             USB_EP_DESC_TRANSFER_TYPE_CONTROL,
      /* _ep_sync - iso */       USB_EP_DESC_SYNC_TYPE_NOT_USED,
      /* _ep_usage - iso/irq3 */ USB_EP_DESC_USAGE_TYPE_NOT_USED,
      /* _wMaxPacketSize */      8,
      /* _bInterval */           0xFF
   ),

   USB_ENDPOINT_DESC_TABLE_FILL(
      /* _ep_num */              2,
      /* _ep_dir */              USB_EP_DESC_DIR_OUT,
      /* _ep_type */             USB_EP_DESC_TRANSFER_TYPE_CONTROL,
      /* _ep_sync - iso */       USB_EP_DESC_SYNC_TYPE_NOT_USED,
      /* _ep_usage - iso/irq3 */ USB_EP_DESC_USAGE_TYPE_NOT_USED,
      /* _wMaxPacketSize */      8,
      /* _bInterval */           0xFF
   ),



   USB_ENDPOINT_DESC_TABLE_FILL(
      /* _ep_num */              3,
      /* _ep_dir */              USB_EP_DESC_DIR_IN,
      /* _ep_type */             USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* _ep_sync - iso */       USB_EP_DESC_SYNC_TYPE_NOT_USED,
      /* _ep_usage - iso/irq3 */ USB_EP_DESC_USAGE_TYPE_NOT_USED,
      /* _wMaxPacketSize */      8,
      /* _bInterval */           0xFF
   ),

   USB_ENDPOINT_DESC_TABLE_FILL(
      /* _ep_num */              3,
      /* _ep_dir */              USB_EP_DESC_DIR_OUT,
      /* _ep_type */             USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* _ep_sync - iso */       USB_EP_DESC_SYNC_TYPE_NOT_USED,
      /* _ep_usage - iso/irq3 */ USB_EP_DESC_USAGE_TYPE_NOT_USED,
      /* _wMaxPacketSize */      8,
      /* _bInterval */           0xFF
   ),

   USB_ENDPOINT_DESC_TABLE_FILL(
      /* _ep_num */              4,
      /* _ep_dir */              USB_EP_DESC_DIR_IN,
      /* _ep_type */             USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* _ep_sync - iso */       USB_EP_DESC_SYNC_TYPE_NOT_USED,
      /* _ep_usage - iso/irq3 */ USB_EP_DESC_USAGE_TYPE_NOT_USED,
      /* _wMaxPacketSize */      8,
      /* _bInterval */           0xFF
   ),

   USB_ENDPOINT_DESC_TABLE_FILL(
      /* _ep_num */              5,
      /* _ep_dir */              USB_EP_DESC_DIR_OUT,
      /* _ep_type */             USB_EP_DESC_TRANSFER_TYPE_BULK,
      /* _ep_sync - iso */       USB_EP_DESC_SYNC_TYPE_NOT_USED,
      /* _ep_usage - iso/irq3 */ USB_EP_DESC_USAGE_TYPE_NOT_USED,
      /* _wMaxPacketSize */      8,
      /* _bInterval */           0xFF
   ),



   USB_ENDPOINT_DESC_TABLE_FILL(
      /* _ep_num */              6,
      /* _ep_dir */              USB_EP_DESC_DIR_IN,
      /* _ep_type */             USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* _ep_sync - iso */       USB_EP_DESC_SYNC_TYPE_NOT_USED,
      /* _ep_usage - iso/irq3 */ USB_EP_DESC_USAGE_TYPE_NOT_USED,
      /* _wMaxPacketSize */      8,
      /* _bInterval */           0xFF
   ),

   USB_ENDPOINT_DESC_TABLE_FILL(
      /* _ep_num */              6,
      /* _ep_dir */              USB_EP_DESC_DIR_OUT,
      /* _ep_type */             USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* _ep_sync - iso */       USB_EP_DESC_SYNC_TYPE_NOT_USED,
      /* _ep_usage - iso/irq3 */ USB_EP_DESC_USAGE_TYPE_NOT_USED,
      /* _wMaxPacketSize */      8,
      /* _bInterval */           0xFF
   ),

   USB_ENDPOINT_DESC_TABLE_FILL(
      /* _ep_num */              7,
      /* _ep_dir */              USB_EP_DESC_DIR_IN,
      /* _ep_type */             USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* _ep_sync - iso */       USB_EP_DESC_SYNC_TYPE_NOT_USED,
      /* _ep_usage - iso/irq3 */ USB_EP_DESC_USAGE_TYPE_NOT_USED,
      /* _wMaxPacketSize */      8,
      /* _bInterval */           0xFF
   ),

   USB_ENDPOINT_DESC_TABLE_FILL(
      /* _ep_num */              8,
      /* _ep_dir */              USB_EP_DESC_DIR_OUT,
      /* _ep_type */             USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,
      /* _ep_sync - iso */       USB_EP_DESC_SYNC_TYPE_NOT_USED,
      /* _ep_usage - iso/irq3 */ USB_EP_DESC_USAGE_TYPE_NOT_USED,
      /* _wMaxPacketSize */      8,
      /* _bInterval */           0xFF
   ),



   USB_ENDPOINT_DESC_TABLE_FILL(
      /* _ep_num */              9,
      /* _ep_dir */              USB_EP_DESC_DIR_IN,
      /* _ep_type */             USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS,
      /* _ep_sync - iso */       USB_EP_DESC_SYNC_TYPE_NOT_USED,
      /* _ep_usage - iso/irq3 */ USB_EP_DESC_USAGE_TYPE_NOT_USED,
      /* _wMaxPacketSize */      8,
      /* _bInterval */           0xFF
   ),

   USB_ENDPOINT_DESC_TABLE_FILL(
      /* _ep_num */              9,
      /* _ep_dir */              USB_EP_DESC_DIR_OUT,
      /* _ep_type */             USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS,
      /* _ep_sync - iso */       USB_EP_DESC_SYNC_TYPE_NOT_USED,
      /* _ep_usage - iso/irq3 */ USB_EP_DESC_USAGE_TYPE_NOT_USED,
      /* _wMaxPacketSize */      8,
      /* _bInterval */           0xFF
   ),

   USB_ENDPOINT_DESC_TABLE_FILL(
      /* _ep_num */              10,
      /* _ep_dir */              USB_EP_DESC_DIR_IN,
      /* _ep_type */             USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS,
      /* _ep_sync - iso */       USB_EP_DESC_SYNC_TYPE_NOT_USED,
      /* _ep_usage - iso/irq3 */ USB_EP_DESC_USAGE_TYPE_NOT_USED,
      /* _wMaxPacketSize */      8,
      /* _bInterval */           0xFF
   ),

   USB_ENDPOINT_DESC_TABLE_FILL(
      /* _ep_num */              11,
      /* _ep_dir */              USB_EP_DESC_DIR_OUT,
      /* _ep_type */             USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS,
      /* _ep_sync - iso */       USB_EP_DESC_SYNC_TYPE_NOT_USED,
      /* _ep_usage - iso/irq3 */ USB_EP_DESC_USAGE_TYPE_NOT_USED,
      /* _wMaxPacketSize */      8,
      /* _bInterval */           0xFF
   )
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

    USBDC_Bind_Config_Desc(usbdc1, (const uint8_t*)(&desc1));

    install_result = USBD_DEV_Install_Config(usbd, USBD_PORT_TEST, usbdc1);

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

        USBD_IOTP_Install_Default_Control_Pipe(usbd, &tp_ep0in, &tp_ep0out);
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


void main_test_irq_send_specific(USBD_Params_XT *usbd, uint32_t iotp_num, uint8_t num_bufs, uint32_t size)
{
    iotp_test_send_irq(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        &random_data_buf[rand() % TEST_MAX_DATA_SIZE],
        size);
}

void main_test_irq_send(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;

    printf("\n\rtest send %s - incremental size in range 0 - %d:\n\r", test_type, TEST_NUM_INCREMENTAL_SIZE_TESTS - 1);
    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_INCREMENTAL_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_INCREMENTAL_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);
            main_test_irq_send_specific(usbd, j, 1, cntr);
            main_test_irq_send_specific(usbd, j, 2, cntr);
        }
    }

    printf("\n\rtest send %s - random size in range 0 - %d:\n\r", test_type, TEST_MAX_DATA_SIZE - 1);
    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_RANDOM_DATA_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_RANDOM_DATA_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);
            main_test_irq_send_specific(usbd, j, (uint8_t)(rand() & 0xFF), rand() % TEST_MAX_DATA_SIZE);
        }
    }
} /* main_test_irq_send */

void main_test_irq_send_1_connected_transaction_specific(
    USBD_Params_XT *usbd,
    uint32_t iotp_num,
    uint8_t num_bufs,
    uint32_t size,
    uint32_t size_1,
    iotp_test_send_event_type_resend_ET resend_time_1)
{
    iotp_in_transactions[0].data = &random_data_buf[rand() % TEST_MAX_DATA_SIZE];
    iotp_in_transactions[0].size = size_1;
    iotp_in_transactions[0].resend_time = resend_time_1;

    iotp_test_send_irq_ready_buf_empty(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        &random_data_buf[rand() % TEST_MAX_DATA_SIZE],
        size,
        iotp_in_transactions,
        1);
}

void main_test_irq_send_1_connected_transaction(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;

    printf("\n\rtest send 1 connected transaction %s - incremental size in range 1 - %d:\n\r", test_type, (8 * TEST_EP0_MPS) - 1);
#if 0
                    iotp_in_transactions[0].data = &random_data_buf[rand() % TEST_MAX_DATA_SIZE];
                    iotp_in_transactions[0].size = cntr2;
                    iotp_in_transactions[0].resend_time = IOTP_TEST_RESEND_ON_READY;
                    iotp_test_send_irq_ready_buf_empty(
                        usbd,
                        iotp_collection[0].iotp,
                        iotp_collection[0].ep_num,
                        iotp_collection[0].dir,
                        num_bufs,
                        &random_data_buf[rand() % TEST_MAX_DATA_SIZE],
                        0,
                        iotp_in_transactions,
                        1);
#endif
#if 1
    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s",
                j,
                NUM_ELEMS(iotp_collection),
                iotp_collection[j].ep_num,
                (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN");
        for(num_bufs = 1; num_bufs < 3; num_bufs++)
        {
            for(cntr = 0; cntr < (8 * TEST_EP0_MPS); cntr++)
            {
                for(cntr2 = 0; cntr2 < (8 * TEST_EP0_MPS); cntr2++)
                {
                    main_test_irq_send_1_connected_transaction_specific(
                        usbd,
                        j,
                        num_bufs,
                        cntr,
                        cntr2,
                        IOTP_TEST_RESEND_ON_READY);

                    main_test_irq_send_1_connected_transaction_specific(
                        usbd,
                        j,
                        num_bufs,
                        cntr,
                        cntr2,
                        IOTP_TEST_RESEND_ON_BUF_EMPTY);
                }
            }
        }
    }
#endif
} /* main_test_irq_send_1_connected_transaction */

void main_test_irq_send_2_connected_transactions_specific(
    USBD_Params_XT *usbd,
    uint32_t iotp_num,
    uint8_t num_bufs,
    uint32_t size,
    uint32_t size_1,
    iotp_test_send_event_type_resend_ET resend_time_1,
    uint32_t size_2,
    iotp_test_send_event_type_resend_ET resend_time_2)
{
    iotp_in_transactions[0].data = &random_data_buf[rand() % TEST_MAX_DATA_SIZE];
    iotp_in_transactions[0].size = size_1;
    iotp_in_transactions[0].resend_time = resend_time_1;

    iotp_in_transactions[1].data = &random_data_buf[rand() % TEST_MAX_DATA_SIZE];
    iotp_in_transactions[1].size = size_2;
    iotp_in_transactions[1].resend_time = resend_time_2;

    iotp_test_send_irq_ready_buf_empty(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        &random_data_buf[rand() % TEST_MAX_DATA_SIZE],
        size,
        iotp_in_transactions,
        2);
}

void main_test_irq_send_2_connected_transactions(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;

    printf("\n\rtest send 2 connected transactions %s - incremental size in range 1 - %d:\n\r", test_type, (3 * TEST_EP0_MPS) - 1);

    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s",
                j,
                NUM_ELEMS(iotp_collection),
                iotp_collection[j].ep_num,
                (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN");
        for(num_bufs = 1; num_bufs < 3; num_bufs++)
        {
            for(cntr = 0; cntr < (3 * TEST_EP0_MPS); cntr++)
            {
                for(cntr2 = 0; cntr2 < (3 * TEST_EP0_MPS); cntr2++)
                {
                    for(cntr3 = 0; cntr3 < (3 * TEST_EP0_MPS); cntr3++)
                    {
                        main_test_irq_send_2_connected_transactions_specific(
                            usbd,
                            j,
                            num_bufs,
                            cntr,
                            cntr2,
                            IOTP_TEST_RESEND_ON_READY,
                            cntr3,
                            IOTP_TEST_RESEND_ON_READY);

                        main_test_irq_send_2_connected_transactions_specific(
                            usbd,
                            j,
                            num_bufs,
                            cntr,
                            cntr2,
                            IOTP_TEST_RESEND_ON_READY,
                            cntr3,
                            IOTP_TEST_RESEND_ON_BUF_EMPTY);

                        main_test_irq_send_2_connected_transactions_specific(
                            usbd,
                            j,
                            num_bufs,
                            cntr,
                            cntr2,
                            IOTP_TEST_RESEND_ON_BUF_EMPTY,
                            cntr3,
                            IOTP_TEST_RESEND_ON_READY);

                        main_test_irq_send_2_connected_transactions_specific(
                            usbd,
                            j,
                            num_bufs,
                            cntr,
                            cntr2,
                            IOTP_TEST_RESEND_ON_BUF_EMPTY,
                            cntr3,
                            IOTP_TEST_RESEND_ON_BUF_EMPTY);
                    }
                }
            }
        }
    }
} /* main_test_irq_send_2_connected_transactions */

void main_test_irq_send_abort_by_user_specific(USBD_Params_XT *usbd, uint32_t iotp_num, uint8_t num_bufs, uint32_t size, USBD_IO_Inout_Data_Size_DT abort_after_packets, USBD_Bool_DT flush_hw_bufs)
{
    iotp_test_send_irq_abort_by_user(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        &random_data_buf[rand() % TEST_MAX_DATA_SIZE],
        size,
        abort_after_packets,
        flush_hw_bufs);

} /* main_test_irq_send_abort_by_user_specific */

void main_test_irq_send_abort_by_user(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;

    printf("\n\rtest send %s aborted by user - incremental size in range 0 - %d:\n\r", test_type, (3 * TEST_EP0_MPS) - 1);
    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               (4 * TEST_EP0_MPS));
        for(cntr = 0; cntr < (3 * TEST_EP0_MPS); cntr++)
        {
            printf("\r%10d", cntr);
            for(cntr2 = 0; cntr2 < (4 * TEST_EP0_MPS); cntr2++)
            {
                printf("\r%10d", cntr);
                main_test_irq_send_abort_by_user_specific(usbd, j, 1, cntr, cntr2, USBD_TRUE);
                main_test_irq_send_abort_by_user_specific(usbd, j, 2, cntr, cntr2, USBD_TRUE);
                main_test_irq_send_abort_by_user_specific(usbd, j, 1, cntr, cntr2, USBD_FALSE);
                main_test_irq_send_abort_by_user_specific(usbd, j, 2, cntr, cntr2, USBD_FALSE);
            }
        }
    }

    printf("\n\rtest send %s aborted by user - random size in range 0 - %d:\n\r", test_type, TEST_MAX_DATA_SIZE - 1);
    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_RANDOM_DATA_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_RANDOM_DATA_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);
            main_test_irq_send_abort_by_user_specific(usbd, j, (uint8_t)(rand() & 0xFF), rand() % TEST_MAX_DATA_SIZE, rand() % TEST_MAX_DATA_SIZE, USBD_TRUE);
            main_test_irq_send_abort_by_user_specific(usbd, j, (uint8_t)(rand() & 0xFF), rand() % TEST_MAX_DATA_SIZE, rand() % TEST_MAX_DATA_SIZE, USBD_FALSE);
        }
    }
} /* main_test_irq_send_abort_by_user */

void main_test_irq_send_abort_by_rec_specific(USBD_Params_XT *usbd, uint32_t iotp_num, uint8_t num_bufs, uint32_t size, USBD_IO_Inout_Data_Size_DT abort_after_packets)
{
    iotp_test_send_irq_abort_by_reconfigure(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        &random_data_buf[rand() % TEST_MAX_DATA_SIZE],
        size,
        abort_after_packets);

} /* main_test_irq_send_abort_by_rec_specific */

void main_test_irq_send_abort_by_rec(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;

    printf("\n\rtest send %s aborted by reconfigure - incremental size in range 0 - %d:\n\r", test_type, (3 * TEST_EP0_MPS) - 1);
    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               (4 * TEST_EP0_MPS));
        for(cntr = 0; cntr < (3 * TEST_EP0_MPS); cntr++)
        {
            printf("\r%10d", cntr);
            for(cntr2 = 0; cntr2 < (4 * TEST_EP0_MPS); cntr2++)
            {
                printf("\r%10d", cntr);
                main_test_irq_send_abort_by_rec_specific(usbd, j, 1, cntr, cntr2);
                main_test_irq_send_abort_by_rec_specific(usbd, j, 2, cntr, cntr2);
            }
        }
    }

    printf("\n\rtest send %s aborted by reconfigure - random size in range 0 - %d:\n\r", test_type, TEST_MAX_DATA_SIZE - 1);
    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_RANDOM_DATA_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_RANDOM_DATA_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);
            main_test_irq_send_abort_by_rec_specific(usbd, j, (uint8_t)(rand() & 0xFF), rand() % TEST_MAX_DATA_SIZE, rand() % TEST_MAX_DATA_SIZE);
        }
    }
} /* main_test_irq_send_abort_by_rec */

void main_test_irq_send_status_specific(
    USBD_Params_XT *usbd,
    uint32_t iotp_num)
{
    iotp_test_send_status_irq(
            usbd,
            iotp_collection[iotp_num].iotp,
            iotp_collection[iotp_num].ep_num,
            iotp_collection[iotp_num].dir);
} /* main_test_irq_send_status_specific */

void main_test_irq_send_status(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;

    printf("\n\n\rtest send status %s\n\r", test_type);

    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        main_test_irq_send_status_specific(
                usbd,
                j);
    }
} /* main_test_irq_send_status */

void main_test_irq_recv_and_ready_specific(
    USBD_Params_XT *usbd,
    uint32_t iotp_num,
    uint8_t num_bufs,
    uint32_t size)
{
    iotp_test_recv_ready_irq(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        &random_data_buf[rand() % TEST_MAX_DATA_SIZE],
        size);
} /* main_test_irq_recv_and_ready_specific */

void main_test_irq_recv_and_ready(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;

    if(0 == strcmp(test_type, "irq_true_dma"))
    {
  //      IOCMD_Set_All_Logs(IOCMD_LOG_LEVEL_DEBUG_LO, IOCMD_LOG_LEVEL_DEBUG_LO);

  //      IOCMD_NOTICE_2(MAIN_APP, "all logs set to: level: %d; quiet_level: %d",
  //          IOCMD_LOG_LEVEL_DEBUG_LO, IOCMD_LOG_LEVEL_DEBUG_LO);
    }

    printf("\n\rtest recv and ready %s - incremental size in range 0 - %d:\n\r", test_type, TEST_NUM_INCREMENTAL_SIZE_TESTS - 1);

    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_INCREMENTAL_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_INCREMENTAL_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);
            main_test_irq_recv_and_ready_specific(
                usbd,
                j,
                1,
                cntr);
            main_test_irq_recv_and_ready_specific(
                usbd,
                j,
                2,
                cntr);
        }
    }

    printf("\n\rtest recv and ready %s - random size in range 0 - %d:\n\r", test_type, TEST_MAX_DATA_SIZE - 1);
    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_RANDOM_DATA_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_RANDOM_DATA_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);
            main_test_irq_recv_and_ready_specific(
                usbd,
                j,
                (uint8_t)(rand() & 0xFF),
                rand() % TEST_MAX_DATA_SIZE);
        }
    }
} /* main_test_irq_recv_and_ready */

void main_test_irq_recv_and_wait_specific(
    USBD_Params_XT *usbd,
    uint32_t iotp_num,
    uint8_t num_bufs,
    uint32_t size)
{
    iotp_test_recv_wait_irq(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        &random_data_buf[rand() % TEST_MAX_DATA_SIZE],
        size);
} /* main_test_irq_recv_and_wait_specific */

void main_test_irq_recv_and_wait(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;

    printf("\n\rtest recv and wait %s - incremental size in range 0 - %d:\n\r", test_type, TEST_NUM_INCREMENTAL_SIZE_TESTS - 1);

    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_INCREMENTAL_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_INCREMENTAL_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);
            main_test_irq_recv_and_wait_specific(
                usbd,
                j,
                1,
                cntr);
            main_test_irq_recv_and_wait_specific(
                usbd,
                j,
                2,
                cntr);
        }
    }

    printf("\n\rtest recv and wait %s - random size in range 0 - %d:\n\r", test_type, TEST_MAX_DATA_SIZE - 1);
    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_RANDOM_DATA_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_RANDOM_DATA_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);
            main_test_irq_recv_and_wait_specific(
                usbd,
                j,
                (uint8_t)(rand() & 0xFF),
                rand() % TEST_MAX_DATA_SIZE);
        }
    }
} /* main_test_irq_recv_and_wait */

void main_test_irq_recv_1_specific(
    USBD_Params_XT *usbd,
    uint32_t iotp_num,
    uint8_t num_bufs,
    uint32_t size_1,
    iotp_test_type_recv_ET recv_method_1,
    USBD_Bool_DT           recv_to_whole_buffer_1)
{
    iotp_out_transactions[0].data                   = &random_data_buf[rand() % TEST_MAX_DATA_SIZE];
    iotp_out_transactions[0].size                   = size_1;
    iotp_out_transactions[0].recv_method            = recv_method_1;
    iotp_out_transactions[0].recv_to_whole_buffer   = recv_to_whole_buffer_1;

    iotp_test_recv_irq_next(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        iotp_out_transactions,
        1);
} /* main_test_irq_recv_1_specific */

void main_test_irq_recv_and_ready_1_connected_transaction(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;

    printf("\n\rtest recv and ready 1 connected transaction %s - incremental size in range 0 - %d:\n\r", test_type, TEST_NUM_INCREMENTAL_SIZE_TESTS - 1);

    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_INCREMENTAL_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_INCREMENTAL_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);

            main_test_irq_recv_1_specific(usbd, j, 1, cntr, IOTP_TEST_RECV_AND_READY, USBD_FALSE);
            main_test_irq_recv_1_specific(usbd, j, 2, cntr, IOTP_TEST_RECV_AND_READY, USBD_FALSE);
        }
    }

    printf("\n\rtest recv and ready 1 connected transaction %s - random size in range 0 - %d:\n\r", test_type, TEST_MAX_DATA_SIZE - 1);
    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_RANDOM_DATA_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_RANDOM_DATA_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);

            main_test_irq_recv_1_specific(usbd, j, (uint8_t)(rand() & 0xFF), rand() % TEST_MAX_DATA_SIZE, IOTP_TEST_RECV_AND_READY, USBD_FALSE);
        }
    }
} /* main_test_irq_recv_and_ready_1_connected_transaction */

void main_test_irq_recv_and_wait_1_connected_transaction(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;
    bool_t recv_to_whole_buf_1;

    printf("\n\rtest recv and wait 1 connected transaction %s - incremental size in range 0 - %d:\n\r", test_type, TEST_NUM_INCREMENTAL_SIZE_TESTS - 1);

    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_INCREMENTAL_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_INCREMENTAL_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);

            for(recv_to_whole_buf_1 = 0; recv_to_whole_buf_1 < 2; recv_to_whole_buf_1++)
            {
                main_test_irq_recv_1_specific(usbd, j, 1, cntr, IOTP_TEST_RECV_AND_WAIT, recv_to_whole_buf_1);
                main_test_irq_recv_1_specific(usbd, j, 2, cntr, IOTP_TEST_RECV_AND_WAIT, recv_to_whole_buf_1);
            }
        }
    }

    printf("\n\rtest recv and wait 1 connected transaction %s - random size in range 0 - %d:\n\r", test_type, TEST_MAX_DATA_SIZE - 1);
    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_RANDOM_DATA_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_RANDOM_DATA_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);

            main_test_irq_recv_1_specific(usbd, j, (uint8_t)(rand() & 0xFF), rand() % TEST_MAX_DATA_SIZE, IOTP_TEST_RECV_AND_WAIT, USBD_FALSE);
        }
    }
} /* main_test_irq_recv_and_wait_1_connected_transaction */

void main_test_irq_recv_2_specific(
    USBD_Params_XT *usbd,
    uint32_t iotp_num,
    uint8_t num_bufs,
    uint32_t size_1,
    iotp_test_type_recv_ET recv_method_1,
    USBD_Bool_DT           recv_to_whole_buffer_1,
    uint32_t size_2,
    iotp_test_type_recv_ET recv_method_2,
    USBD_Bool_DT           recv_to_whole_buffer_2)
{
    iotp_out_transactions[0].data                   = &random_data_buf[rand() % TEST_MAX_DATA_SIZE];
    iotp_out_transactions[0].size                   = size_1;
    iotp_out_transactions[0].recv_method            = recv_method_1;
    iotp_out_transactions[0].recv_to_whole_buffer   = recv_to_whole_buffer_1;

    iotp_out_transactions[1].data                   = &random_data_buf[rand() % TEST_MAX_DATA_SIZE];
    iotp_out_transactions[1].size                   = size_2;
    iotp_out_transactions[1].recv_method            = recv_method_2;
    iotp_out_transactions[1].recv_to_whole_buffer   = recv_to_whole_buffer_2;

    iotp_test_recv_irq_next(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        iotp_out_transactions,
        2);
} /* main_test_irq_recv_2_specific */

void main_test_irq_recv_2_connected_transactions(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;
    bool_t recv_to_whole_buf_1;
    bool_t recv_to_whole_buf_2;

    incremental_max_size = (TEST_EP0_MPS * 3) + 1;
    printf("\n\rtest recv 2 connected transactions %s - incremental size in range 1 - %d:\n\r", test_type, incremental_max_size);

    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s",
                j,
                NUM_ELEMS(iotp_collection),
                iotp_collection[j].ep_num,
                (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN");
        for(num_bufs = 1; num_bufs < 3; num_bufs++)
        {
            for(cntr = 1; cntr < TEST_EP0_MPS; cntr++)
            {
                for(cntr2 = 0; cntr2 < incremental_max_size; cntr2++)
                {
                    for(cntr3 = 0; cntr3 < incremental_max_size; cntr3++)
                    {
                        for(recv_to_whole_buf_1 = 0; recv_to_whole_buf_1 < 2; recv_to_whole_buf_1++)
                        {
                            for(recv_to_whole_buf_2 = 0; recv_to_whole_buf_2 < 2; recv_to_whole_buf_2++)
                            {
                                main_test_irq_recv_2_specific(usbd, j, num_bufs, cntr2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1, cntr3, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2);
                                main_test_irq_recv_2_specific(usbd, j, num_bufs, cntr2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1, cntr3, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2);
                                main_test_irq_recv_2_specific(usbd, j, num_bufs, cntr2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1, cntr3, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2);
                                main_test_irq_recv_2_specific(usbd, j, num_bufs, cntr2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1, cntr3, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2);
                            }
                        }
                    }
                }
            }
        }
    }
} /* main_test_irq_recv_2_connected_transactions */

void main_test_irq_recv_3_specific(
    USBD_Params_XT *usbd,
    uint32_t iotp_num,
    uint8_t num_bufs,
    uint32_t size_1,
    iotp_test_type_recv_ET recv_method_1,
    USBD_Bool_DT           recv_to_whole_buffer_1,
    uint32_t size_2,
    iotp_test_type_recv_ET recv_method_2,
    USBD_Bool_DT           recv_to_whole_buffer_2,
    uint32_t size_3,
    iotp_test_type_recv_ET recv_method_3,
    USBD_Bool_DT           recv_to_whole_buffer_3)
{
    iotp_out_transactions[0].data                   = &random_data_buf[rand() % TEST_MAX_DATA_SIZE];
    iotp_out_transactions[0].size                   = size_1;
    iotp_out_transactions[0].recv_method            = recv_method_1;
    iotp_out_transactions[0].recv_to_whole_buffer   = recv_to_whole_buffer_1;

    iotp_out_transactions[1].data                   = &random_data_buf[rand() % TEST_MAX_DATA_SIZE];
    iotp_out_transactions[1].size                   = size_2;
    iotp_out_transactions[1].recv_method            = recv_method_2;
    iotp_out_transactions[1].recv_to_whole_buffer   = recv_to_whole_buffer_2;

    iotp_out_transactions[2].data                   = &random_data_buf[rand() % TEST_MAX_DATA_SIZE];
    iotp_out_transactions[2].size                   = size_3;
    iotp_out_transactions[2].recv_method            = recv_method_3;
    iotp_out_transactions[2].recv_to_whole_buffer   = recv_to_whole_buffer_3;

    iotp_test_recv_irq_next(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        iotp_out_transactions,
        3);
} /* main_test_irq_recv_3_specific */

void main_test_irq_recv_3_connected_transactions(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;
    bool_t recv_to_whole_buf_1;
    bool_t recv_to_whole_buf_2;
    bool_t recv_to_whole_buf_3;

    incremental_max_size = (TEST_EP0_MPS * 3) + 1;
    printf("\n\rtest recv 3 connected transactions %s - incremental size in range 1 - %d:\n\r", test_type, incremental_max_size);

    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s",
                j,
                NUM_ELEMS(iotp_collection),
                iotp_collection[j].ep_num,
                (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN");
        for(num_bufs = 1; num_bufs < 3; num_bufs++)
        {
            for(cntr = 1; cntr < (TEST_EP0_MPS / 2); cntr++)
            {
                for(cntr2 = 0; cntr2 < incremental_max_size; cntr2++)
                {
                    for(cntr3 = 0; cntr3 < incremental_max_size; cntr3++)
                    {
                        for(cntr4 = 0; cntr4 < incremental_max_size; cntr4++)
                        {
                            for(recv_to_whole_buf_1 = 0; recv_to_whole_buf_1 < 2; recv_to_whole_buf_1++)
                            {
                                for(recv_to_whole_buf_2 = 0; recv_to_whole_buf_2 < 2; recv_to_whole_buf_2++)
                                {
                                    for(recv_to_whole_buf_3 = 0; recv_to_whole_buf_3 < 2; recv_to_whole_buf_3++)
                                    {
                                        main_test_irq_recv_3_specific(
                                            usbd, j, num_bufs,
                                            cntr2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1,
                                            cntr3, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2,
                                            cntr4, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_3);
                                        main_test_irq_recv_3_specific(
                                            usbd, j, num_bufs,
                                            cntr2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1,
                                            cntr3, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2,
                                            cntr4, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_3);
                                        main_test_irq_recv_3_specific(
                                            usbd, j, num_bufs,
                                            cntr2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1,
                                            cntr3, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2,
                                            cntr4, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_3);
                                        main_test_irq_recv_3_specific(
                                            usbd, j, num_bufs,
                                            cntr2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1,
                                            cntr3, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2,
                                            cntr4, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_3);
                                        main_test_irq_recv_3_specific(
                                            usbd, j, num_bufs,
                                            cntr2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1,
                                            cntr3, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2,
                                            cntr4, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_3);
                                        main_test_irq_recv_3_specific(
                                            usbd, j, num_bufs,
                                            cntr2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1,
                                            cntr3, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2,
                                            cntr4, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_3);
                                        main_test_irq_recv_3_specific(
                                            usbd, j, num_bufs,
                                            cntr2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1,
                                            cntr3, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2,
                                            cntr4, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_3);
                                        main_test_irq_recv_3_specific(
                                            usbd, j, num_bufs,
                                            cntr2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1,
                                            cntr3, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2,
                                            cntr4, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_3);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
} /* main_test_irq_recv_3_connected_transactions */

void main_test_irq_recv_and_ready_on_irq_specific(
    USBD_Params_XT *usbd,
    uint32_t iotp_num,
    uint8_t num_bufs,
    uint32_t size)
{
    iotp_test_recv_ready_on_irq(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        &random_data_buf[rand() % TEST_MAX_DATA_SIZE],
        size);
} /* main_test_irq_recv_and_ready_on_irq_specific */

void main_test_irq_recv_and_ready_on_irq(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;

    incremental_max_size = (TEST_NUM_INCREMENTAL_SIZE_TESTS / 16) > 512 ? (TEST_NUM_INCREMENTAL_SIZE_TESTS / 16) : 512;
    printf("\n\rtest recv and ready on %s - incremental size in range 0 - %d:\n\r", test_type, incremental_max_size - 1);

    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               incremental_max_size);
        for(cntr = 0; cntr < incremental_max_size; cntr++)
        {
            printf("\r%10d", cntr);

            main_test_irq_recv_and_ready_on_irq_specific(usbd, j, 1, cntr);
            main_test_irq_recv_and_ready_on_irq_specific(usbd, j, 2, cntr);
        }
    }
} /* main_test_irq_recv_and_ready_on_irq */

void main_test_irq_recv_and_wait_on_irq_specific(
    USBD_Params_XT *usbd,
    uint32_t iotp_num,
    uint8_t num_bufs,
    uint32_t size)
{
    iotp_test_recv_wait_on_irq(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        &random_data_buf[rand() % TEST_MAX_DATA_SIZE],
        size);
} /* main_test_irq_recv_and_wait_on_irq_specific */

void main_test_irq_recv_and_wait_on_irq(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;

    incremental_max_size = (TEST_NUM_INCREMENTAL_SIZE_TESTS / 16) > 512 ? (TEST_NUM_INCREMENTAL_SIZE_TESTS / 16) : 512;
    printf("\n\rtest recv and wait on %s - incremental size in range 0 - %d:\n\r", test_type, incremental_max_size - 1);

    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               incremental_max_size);
        for(cntr = 0; cntr < incremental_max_size; cntr++)
        {
            printf("\r%10d", cntr);

            main_test_irq_recv_and_wait_on_irq_specific(usbd, j, 1, cntr);
            main_test_irq_recv_and_wait_on_irq_specific(usbd, j, 2, cntr);
        }
    }
} /* main_test_irq_recv_and_wait_on_irq */

void main_test_irq_recv_and_ready_too_less_space_specific(
    USBD_Params_XT *usbd,
    uint32_t iotp_num,
    uint8_t num_bufs,
    uint32_t size)
{
    iotp_test_recv_ready_irq_too_less_space(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        &random_data_buf[rand() % TEST_MAX_DATA_SIZE],
        size);
} /* main_test_irq_recv_and_ready_too_less_space_specific */

void main_test_irq_recv_and_ready_too_less_space(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;

    printf("\n\rtest recv and ready %s too less space - incremental size in range 0 - %d:\n\r", test_type, TEST_NUM_INCREMENTAL_SIZE_TESTS - 1);

    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_INCREMENTAL_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_INCREMENTAL_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);

            main_test_irq_recv_and_ready_too_less_space_specific(usbd, j, 1, cntr);
            main_test_irq_recv_and_ready_too_less_space_specific(usbd, j, 2, cntr);
        }
    }

    printf("\n\rtest recv and ready %s too less space - random size in range 0 - %d:\n\r", test_type, TEST_MAX_DATA_SIZE - 1);
    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_RANDOM_DATA_SIZE_TESTS / 2);
        for(cntr = 0; cntr < (TEST_NUM_RANDOM_DATA_SIZE_TESTS / 2); cntr++)
        {
            printf("\r%10d", cntr);

            main_test_irq_recv_and_ready_too_less_space_specific(usbd, j, (uint8_t)(rand() & 0xFF), rand() % TEST_MAX_DATA_SIZE);
        }
    }
} /* main_test_irq_recv_and_ready_too_less_space */

void main_test_irq_recv_and_wait_too_less_space_specific(
    USBD_Params_XT *usbd,
    uint32_t iotp_num,
    uint8_t num_bufs,
    uint32_t size)
{
    iotp_test_recv_wait_irq_too_less_space(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        &random_data_buf[rand() % TEST_MAX_DATA_SIZE],
        size);
} /* main_test_irq_recv_and_wait_too_less_space_specific */

void main_test_irq_recv_and_wait_too_less_space(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;

    printf("\n\rtest recv and wait %s too less space - incremental size in range 0 - %d:\n\r", test_type, TEST_NUM_INCREMENTAL_SIZE_TESTS - 1);

    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_INCREMENTAL_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_INCREMENTAL_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);

            main_test_irq_recv_and_wait_too_less_space_specific(usbd, j, 1, cntr);
            main_test_irq_recv_and_wait_too_less_space_specific(usbd, j, 2, cntr);
        }
    }

    printf("\n\rtest recv and wait %s too less space - random size in range 0 - %d:\n\r", test_type, TEST_MAX_DATA_SIZE - 1);
    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_RANDOM_DATA_SIZE_TESTS / 2);
        for(cntr = 0; cntr < (TEST_NUM_RANDOM_DATA_SIZE_TESTS / 2); cntr++)
        {
            printf("\r%10d", cntr);

            main_test_irq_recv_and_wait_too_less_space_specific(usbd, j, (uint8_t)(rand() & 0xFF), rand() % TEST_MAX_DATA_SIZE);
        }
    }
} /* main_test_irq_recv_and_wait_too_less_space */

void main_test_irq_recv_and_ready_msg_shorten_than_declared_specific(
    USBD_Params_XT *usbd,
    uint32_t iotp_num,
    uint8_t num_bufs,
    uint32_t size)
{
    iotp_test_recv_ready_irq_shorter_msg(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        &random_data_buf[rand() % TEST_MAX_DATA_SIZE],
        size);
} /* main_test_irq_recv_and_ready_msg_shorten_than_declared_specific */

void main_test_irq_recv_and_ready_msg_shorten_than_declared(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;

    printf("\n\rtest recv and ready %s - msg shorten than declared - incremental size in range 0 - %d:\n\r", test_type, TEST_NUM_INCREMENTAL_SIZE_TESTS - 1);

    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_INCREMENTAL_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_INCREMENTAL_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);

            main_test_irq_recv_and_ready_msg_shorten_than_declared_specific(usbd, j, 1, cntr);
            main_test_irq_recv_and_ready_msg_shorten_than_declared_specific(usbd, j, 2, cntr);
        }
    }

    printf("\n\rtest recv and ready %s - msg shorten than declared - random size in range 0 - %d:\n\r", test_type, TEST_MAX_DATA_SIZE - 1);
    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_RANDOM_DATA_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_RANDOM_DATA_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);

            main_test_irq_recv_and_ready_msg_shorten_than_declared_specific(usbd, j, (uint8_t)(rand() & 0xFF), rand() % TEST_MAX_DATA_SIZE);
        }
    }
} /* main_test_irq_recv_and_ready_msg_shorten_than_declared */

void main_test_irq_recv_and_wait_msg_shorten_than_declared_specific(
    USBD_Params_XT *usbd,
    uint32_t iotp_num,
    uint8_t num_bufs,
    uint32_t size)
{
    iotp_test_recv_wait_irq_shorter_msg(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        &random_data_buf[rand() % TEST_MAX_DATA_SIZE],
        size);
} /* main_test_irq_recv_and_wait_msg_shorten_than_declared_specific */

void main_test_irq_recv_and_wait_msg_shorten_than_declared(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;

    printf("\n\rtest recv and wait %s - msg shorten than declared - incremental size in range 0 - %d:\n\r", test_type, TEST_NUM_INCREMENTAL_SIZE_TESTS - 1);

    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_INCREMENTAL_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_INCREMENTAL_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);

            main_test_irq_recv_and_wait_msg_shorten_than_declared_specific(usbd, j, 1, cntr);
            main_test_irq_recv_and_wait_msg_shorten_than_declared_specific(usbd, j, 2, cntr);
        }
    }

    printf("\n\rtest recv and wait %s - msg shorten than declared - random size in range 0 - %d:\n\r", test_type, TEST_MAX_DATA_SIZE - 1);
    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_RANDOM_DATA_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_RANDOM_DATA_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);

            main_test_irq_recv_and_wait_msg_shorten_than_declared_specific(usbd, j, (uint8_t)(rand() & 0xFF), rand() % TEST_MAX_DATA_SIZE);
        }
    }
} /* main_test_irq_recv_and_wait_msg_shorten_than_declared */

void main_test_irq_recv_and_ready_abort_by_user_specific(
    USBD_Params_XT *usbd,
    uint32_t iotp_num,
    uint8_t num_bufs,
    uint32_t size,
    USBD_IO_Inout_Data_Size_DT abort_after_packets,
    USBD_Bool_DT flush_hw_bufs)
{
    iotp_test_recv_ready_irq_abort_by_user(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        &random_data_buf[rand() % TEST_MAX_DATA_SIZE],
        size,
        abort_after_packets,
        flush_hw_bufs);
} /* main_test_irq_recv_and_ready_abort_by_user_specific */

void main_test_irq_recv_and_ready_abort_by_user(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;

    printf("\n\rtest recv and ready abort by user %s - incremental size in range 0 - %d:\n\r", test_type, TEST_NUM_INCREMENTAL_SIZE_TESTS - 1);

    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               (4 * TEST_EP0_MPS));
        for(cntr = 0; cntr < (3 * TEST_EP0_MPS); cntr++)
        {
            printf("\r%10d", cntr);
            for(cntr2 = 0; cntr2 < (4 * TEST_EP0_MPS); cntr2++)
            {
                printf("\r%10d", cntr);
                main_test_irq_recv_and_ready_abort_by_user_specific(usbd, j, 1, cntr, cntr2, USBD_TRUE);
                main_test_irq_recv_and_ready_abort_by_user_specific(usbd, j, 2, cntr, cntr2, USBD_TRUE);
                main_test_irq_recv_and_ready_abort_by_user_specific(usbd, j, 1, cntr, cntr2, USBD_FALSE);
                main_test_irq_recv_and_ready_abort_by_user_specific(usbd, j, 2, cntr, cntr2, USBD_FALSE);
            }
        }
    }

    printf("\n\rtest recv and ready abort by user %s - random size in range 0 - %d:\n\r", test_type, TEST_MAX_DATA_SIZE - 1);
    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_RANDOM_DATA_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_RANDOM_DATA_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);
            main_test_irq_recv_and_ready_abort_by_user_specific(usbd, j, (uint8_t)(rand() & 0xFF), rand() % TEST_MAX_DATA_SIZE, rand() % TEST_MAX_DATA_SIZE, USBD_TRUE);
            main_test_irq_recv_and_ready_abort_by_user_specific(usbd, j, (uint8_t)(rand() & 0xFF), rand() % TEST_MAX_DATA_SIZE, rand() % TEST_MAX_DATA_SIZE, USBD_FALSE);
        }
    }
} /* main_test_irq_recv_and_ready_abort_by_user */

void main_test_irq_recv_and_wait_abort_by_user_specific(
    USBD_Params_XT *usbd,
    uint32_t iotp_num,
    uint8_t num_bufs,
    uint32_t size,
    USBD_IO_Inout_Data_Size_DT abort_after_packets,
    USBD_Bool_DT flush_hw_bufs)
{
    iotp_test_recv_wait_irq_abort_by_user(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        &random_data_buf[rand() % TEST_MAX_DATA_SIZE],
        size,
        abort_after_packets,
        flush_hw_bufs);
} /* main_test_irq_recv_and_wait_abort_by_user_specific */

void main_test_irq_recv_and_wait_abort_by_user(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;

    printf("\n\rtest recv and wait abort by user %s - incremental size in range 0 - %d:\n\r", test_type, TEST_NUM_INCREMENTAL_SIZE_TESTS - 1);

    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               (4 * TEST_EP0_MPS));
        for(cntr = 0; cntr < (3 * TEST_EP0_MPS); cntr++)
        {
            printf("\r%10d", cntr);
            for(cntr2 = 0; cntr2 < (4 * TEST_EP0_MPS); cntr2++)
            {
                printf("\r%10d", cntr);
                main_test_irq_recv_and_wait_abort_by_user_specific(usbd, j, 1, cntr, cntr2, USBD_TRUE);
                main_test_irq_recv_and_wait_abort_by_user_specific(usbd, j, 2, cntr, cntr2, USBD_TRUE);
                main_test_irq_recv_and_wait_abort_by_user_specific(usbd, j, 1, cntr, cntr2, USBD_FALSE);
                main_test_irq_recv_and_wait_abort_by_user_specific(usbd, j, 2, cntr, cntr2, USBD_FALSE);
            }
        }
    }

    printf("\n\rtest recv and wait abort by user %s - random size in range 0 - %d:\n\r", test_type, TEST_MAX_DATA_SIZE - 1);
    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_RANDOM_DATA_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_RANDOM_DATA_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);
            main_test_irq_recv_and_wait_abort_by_user_specific(usbd, j, (uint8_t)(rand() & 0xFF), rand() % TEST_MAX_DATA_SIZE, rand() % TEST_MAX_DATA_SIZE, USBD_TRUE);
            main_test_irq_recv_and_wait_abort_by_user_specific(usbd, j, (uint8_t)(rand() & 0xFF), rand() % TEST_MAX_DATA_SIZE, rand() % TEST_MAX_DATA_SIZE, USBD_FALSE);
        }
    }
} /* main_test_irq_recv_and_wait_abort_by_user */

void main_test_irq_recv_and_ready_abort_by_rec_specific(
    USBD_Params_XT *usbd,
    uint32_t iotp_num,
    uint8_t num_bufs,
    uint32_t size,
    USBD_IO_Inout_Data_Size_DT abort_after_packets)
{
    iotp_test_recv_ready_irq_abort_by_reconfigure(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        &random_data_buf[rand() % TEST_MAX_DATA_SIZE],
        size,
        abort_after_packets);
} /* main_test_irq_recv_and_ready_abort_by_rec_specific */

void main_test_irq_recv_and_ready_abort_by_rec(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;

    printf("\n\rtest recv and ready abort by reconfigure %s - incremental size in range 0 - %d:\n\r", test_type, TEST_NUM_INCREMENTAL_SIZE_TESTS - 1);

    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               (4 * TEST_EP0_MPS));
        for(cntr = 0; cntr < (3 * TEST_EP0_MPS); cntr++)
        {
            printf("\r%10d", cntr);
            for(cntr2 = 0; cntr2 < (4 * TEST_EP0_MPS); cntr2++)
            {
                printf("\r%10d", cntr);
                main_test_irq_recv_and_ready_abort_by_rec_specific(usbd, j, 1, cntr, cntr2);
                main_test_irq_recv_and_ready_abort_by_rec_specific(usbd, j, 2, cntr, cntr2);
            }
        }
    }

    printf("\n\rtest recv and ready abort by reconfigure %s - random size in range 0 - %d:\n\r", test_type, TEST_MAX_DATA_SIZE - 1);
    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_RANDOM_DATA_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_RANDOM_DATA_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);
            main_test_irq_recv_and_ready_abort_by_rec_specific(usbd, j, (uint8_t)(rand() & 0xFF), rand() % TEST_MAX_DATA_SIZE, rand() % TEST_MAX_DATA_SIZE);
        }
    }
} /* main_test_irq_recv_and_ready_abort_by_rec */

void main_test_irq_recv_and_wait_abort_by_rec_specific(
    USBD_Params_XT *usbd,
    uint32_t iotp_num,
    uint8_t num_bufs,
    uint32_t size,
    USBD_IO_Inout_Data_Size_DT abort_after_packets)
{
    iotp_test_recv_wait_irq_abort_by_reconfigure(
        usbd,
        iotp_collection[iotp_num].iotp,
        iotp_collection[iotp_num].ep_num,
        iotp_collection[iotp_num].dir,
        num_bufs,
        &random_data_buf[rand() % TEST_MAX_DATA_SIZE],
        size,
        abort_after_packets);
} /* main_test_irq_recv_and_wait_abort_by_rec_specific */

void main_test_irq_recv_and_wait_abort_by_rec(USBD_Params_XT *usbd, const char *test_type)
{
    int j;
    uint8_t ep_num;
    uint16_t param;
    int number_of_repeats;
    uint32_t cntr;
    uint32_t cntr2;
    uint32_t cntr3;
    uint32_t cntr4;
    uint32_t incremental_max_size;
    uint8_t num_bufs;
    USBD_IO_Inout_Data_Size_DT size;

    printf("\n\rtest recv and wait abort by reconfigure %s - incremental size in range 0 - %d:\n\r", test_type, TEST_NUM_INCREMENTAL_SIZE_TESTS - 1);

    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               (4 * TEST_EP0_MPS));
        for(cntr = 0; cntr < (3 * TEST_EP0_MPS); cntr++)
        {
            printf("\r%10d", cntr);
            for(cntr2 = 0; cntr2 < (4 * TEST_EP0_MPS); cntr2++)
            {
                printf("\r%10d", cntr);
                main_test_irq_recv_and_wait_abort_by_rec_specific(usbd, j, 1, cntr, cntr2);
                main_test_irq_recv_and_wait_abort_by_rec_specific(usbd, j, 2, cntr, cntr2);
            }
        }
    }

    printf("\n\rtest recv and wait abort by reconfigure %s - random size in range 0 - %d:\n\r", test_type, TEST_MAX_DATA_SIZE - 1);
    for(j = 0; j < NUM_ELEMS(iotp_collection); j++)
    {
        printf("\n\riotp %d / %d:: ep_num = %d, dir = %s\n\rstep:\n\r%10d / %d",
               j,
               NUM_ELEMS(iotp_collection),
               iotp_collection[j].ep_num,
               (USB_EP_DIRECTION_OUT == iotp_collection[j].dir) ? "OUT" : "IN",
               0,
               TEST_NUM_RANDOM_DATA_SIZE_TESTS);
        for(cntr = 0; cntr < TEST_NUM_RANDOM_DATA_SIZE_TESTS; cntr++)
        {
            printf("\r%10d", cntr);
            main_test_irq_recv_and_wait_abort_by_rec_specific(usbd, j, (uint8_t)(rand() & 0xFF), rand() % TEST_MAX_DATA_SIZE, rand() % TEST_MAX_DATA_SIZE);
        }
    }
} /* main_test_irq_recv_and_wait_abort_by_rec */

void main_perform_in_tests(USBD_Params_XT *usbd, const char *test_type)
{
    main_test_irq_send(usbd, test_type);
    main_test_irq_send_1_connected_transaction(usbd, test_type);
    main_test_irq_send_2_connected_transactions(usbd, test_type);
    main_test_irq_send_abort_by_user(usbd, test_type);
    main_test_irq_send_abort_by_rec(usbd, test_type);
    main_test_irq_send_status(usbd, test_type);
} /* main_perform_not_dma_tests */

void main_perform_out_tests(USBD_Params_XT *usbd, const char *test_type)
{
    main_test_irq_recv_and_ready(usbd, test_type);
    main_test_irq_recv_and_wait(usbd, test_type);
    main_test_irq_recv_and_ready_1_connected_transaction(usbd, test_type);
    main_test_irq_recv_and_wait_1_connected_transaction(usbd, test_type);
    main_test_irq_recv_2_connected_transactions(usbd, test_type);
    main_test_irq_recv_3_connected_transactions(usbd, test_type);
    main_test_irq_recv_and_ready_on_irq(usbd, test_type);
    main_test_irq_recv_and_wait_on_irq(usbd, test_type);
    main_test_irq_recv_and_ready_too_less_space(usbd, test_type);
    main_test_irq_recv_and_wait_too_less_space(usbd, test_type);
    main_test_irq_recv_and_ready_msg_shorten_than_declared(usbd, test_type);
    main_test_irq_recv_and_wait_msg_shorten_than_declared(usbd, test_type);
    main_test_irq_recv_and_ready_abort_by_user(usbd, test_type);
    main_test_irq_recv_and_wait_abort_by_user(usbd, test_type);
    main_test_irq_recv_and_ready_abort_by_rec(usbd, test_type);
    main_test_irq_recv_and_wait_abort_by_rec(usbd, test_type);
} /* main_perform_not_dma_tests */


void main_perform_full_test(USBD_Params_XT *usbd)
{
    uint8_t ep_num;

    printf("\n\n\rnot-dma tests\n\n\r");
    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    main_perform_in_tests(usbd, "irq");
    main_perform_out_tests(usbd, "irq");


    printf("\n\n\rhalf-dma tests\n\n\r");
    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    main_perform_in_tests(usbd, "irq_half_dma");
    main_perform_out_tests(usbd, "irq_half_dma");


    printf("\n\n\rtrue-dma tests\n\n\r");
    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    main_perform_in_tests(usbd, "irq_true_dma");
    main_perform_out_tests(usbd, "irq_true_dma");
} /* main_perform_full_test */


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


void test_cmd_all(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint8_t ep_num;

    printf("%s: perform full test\n\n\r", __FUNCTION__);

    (void)test_cmd_not_dma_all(arg);
    (void)test_cmd_half_dma_all(arg);
    (void)test_cmd_true_dma_all(arg);
} /* test_cmd_all */

void test_cmd_all_in(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint8_t ep_num;

    printf("%s: perform full in test\n\n\r", __FUNCTION__);

    (void)test_cmd_not_dma_in_all(arg);
    (void)test_cmd_half_dma_in_all(arg);
    (void)test_cmd_true_dma_in_all(arg);
} /* test_cmd_all_in */

void test_cmd_all_out(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint8_t ep_num;

    printf("%s: perform full out test\n\n\r", __FUNCTION__);

    (void)test_cmd_not_dma_out_all(arg);
    (void)test_cmd_half_dma_out_all(arg);
    (void)test_cmd_true_dma_out_all(arg);
} /* test_cmd_all_out */

void test_cmd_not_dma_all(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint8_t ep_num;

    printf("\n\n\rnot-dma tests\n\n\r");
    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    main_perform_in_tests(usbd, "irq");
    main_perform_out_tests(usbd, "irq");
} /* test_cmd_not_dma_all */

void test_cmd_half_dma_all(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint8_t ep_num;


    printf("\n\n\rhalf-dma tests\n\n\r");
    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    main_perform_in_tests(usbd, "irq_half_dma");
    main_perform_out_tests(usbd, "irq_half_dma");
} /* test_cmd_half_dma_all */

void test_cmd_true_dma_all(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint8_t ep_num;


    printf("\n\n\rtrue-dma tests\n\n\r");
    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    main_perform_in_tests(usbd, "irq_true_dma");
    main_perform_out_tests(usbd, "irq_true_dma");
} /* test_cmd_true_dma_all */

void test_cmd_not_dma_in_all(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint8_t ep_num;

    printf("\n\n\rnot-dma tests\n\n\r");
    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    main_perform_in_tests(usbd, "irq");
} /* test_cmd_not_dma_in_all */

void test_cmd_not_dma_out_all(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint8_t ep_num;

    printf("\n\n\rnot-dma tests\n\n\r");
    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    main_perform_out_tests(usbd, "irq");
} /* test_cmd_not_dma_out_all */

void test_cmd_half_dma_in_all(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint8_t ep_num;


    printf("\n\n\rhalf-dma tests\n\n\r");
    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    main_perform_in_tests(usbd, "irq_half_dma");
} /* test_cmd_half_dma_in_all */

void test_cmd_half_dma_out_all(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint8_t ep_num;


    printf("\n\n\rhalf-dma tests\n\n\r");
    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    main_perform_out_tests(usbd, "irq_half_dma");
} /* test_cmd_half_dma_out_all */

void test_cmd_true_dma_in_all(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint8_t ep_num;


    printf("\n\n\rtrue-dma tests\n\n\r");
    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    main_perform_in_tests(usbd, "irq_true_dma");
} /* test_cmd_true_dma_in_all */

void test_cmd_true_dma_out_all(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint8_t ep_num;


    printf("\n\n\rtrue-dma tests\n\n\r");
    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    main_perform_out_tests(usbd, "irq_true_dma");
} /* test_cmd_true_dma_out_all */

void test_cmd_not_dma_in_send(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_send_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_send(usbd, "irq");
    }
} /* test_cmd_not_dma_in_send */

void test_cmd_not_dma_in_send1c(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint32_t size_1;
    uint32_t send_time_1;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &size_1, ""))
                    {
                        if(IOCMD_Arg_Get_Uint32(arg, &send_time_1, ""))
                        {
                            specific = true;
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        if(0 == send_time_1)
        {
            main_test_irq_send_1_connected_transaction_specific(
                usbd,
                iotp_num,
                (uint8_t)num_bufs,
                size,
                size_1,
                IOTP_TEST_RESEND_ON_READY);
        }
        else
        {
            main_test_irq_send_1_connected_transaction_specific(
                usbd,
                iotp_num,
                (uint8_t)num_bufs,
                size,
                size_1,
                IOTP_TEST_RESEND_ON_BUF_EMPTY);
        }
    }
    else
    {
        main_test_irq_send_1_connected_transaction(usbd, "irq");
    }
} /* test_cmd_not_dma_in_send1c */

void test_cmd_not_dma_in_send2c(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint32_t size_1;
    uint32_t send_time_1;
    uint32_t size_2;
    uint32_t send_time_2;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &size_1, ""))
                    {
                        if(IOCMD_Arg_Get_Uint32(arg, &send_time_1, ""))
                        {
                            if(IOCMD_Arg_Get_Uint32(arg, &size_2, ""))
                            {
                                if(IOCMD_Arg_Get_Uint32(arg, &send_time_2, ""))
                                {
                                    specific = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        if((0 == send_time_1) && (0 == send_time_2))
        {
            main_test_irq_send_2_connected_transactions_specific(
                usbd,
                iotp_num,
                (uint8_t)num_bufs,
                size,
                size_1,
                IOTP_TEST_RESEND_ON_READY,
                size_2,
                IOTP_TEST_RESEND_ON_READY);
        }
        else if((1 == send_time_1) && (0 == send_time_2))
        {
            main_test_irq_send_2_connected_transactions_specific(
                usbd,
                iotp_num,
                (uint8_t)num_bufs,
                size,
                size_1,
                IOTP_TEST_RESEND_ON_BUF_EMPTY,
                size_2,
                IOTP_TEST_RESEND_ON_READY);
        }
        else if((0 == send_time_1) && (1 == send_time_2))
        {
            main_test_irq_send_2_connected_transactions_specific(
                usbd,
                iotp_num,
                (uint8_t)num_bufs,
                size,
                size_1,
                IOTP_TEST_RESEND_ON_READY,
                size_2,
                IOTP_TEST_RESEND_ON_BUF_EMPTY);
        }
        else
        {
            main_test_irq_send_2_connected_transactions_specific(
                usbd,
                iotp_num,
                (uint8_t)num_bufs,
                size,
                size_1,
                IOTP_TEST_RESEND_ON_BUF_EMPTY,
                size_2,
                IOTP_TEST_RESEND_ON_BUF_EMPTY);
        }
    }
    else
    {
        main_test_irq_send_2_connected_transactions(usbd, "irq");
    }
} /* test_cmd_not_dma_in_send2c */

void test_cmd_not_dma_in_abort_user(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    USBD_IO_Inout_Data_Size_DT abort_after_packets;
    uint8_t ep_num;
    bool_t specific = false;
    bool_t flush_hw_bufs;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &abort_after_packets, ""))
                    {
                        if(IOCMD_Arg_Get_Bool(arg, &flush_hw_bufs, ""))
                        {
                            specific = true;
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_send_abort_by_user_specific(usbd, iotp_num, (uint8_t)num_bufs, size, abort_after_packets, flush_hw_bufs);
    }
    else
    {
        main_test_irq_send_abort_by_user(usbd, "irq");
    }
} /* test_cmd_not_dma_in_abort_user */

void test_cmd_not_dma_in_abort_rec(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    USBD_IO_Inout_Data_Size_DT abort_after_packets;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &abort_after_packets, ""))
                    {
                        specific = true;
                    }
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_send_abort_by_rec_specific(usbd, iotp_num, (uint8_t)num_bufs, size, abort_after_packets);
    }
    else
    {
        main_test_irq_send_abort_by_rec(usbd, "irq");
    }
} /* test_cmd_not_dma_in_abort_rec */

void test_cmd_not_dma_in_status(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            specific = true;
        }
    }

    if(specific)
    {
        main_test_irq_send_status_specific(usbd, iotp_num);
    }
    else
    {
        main_test_irq_send_status(usbd, "irq");
    }
} /* test_cmd_not_dma_in_status */



void test_cmd_half_dma_in_send(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_send_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_send(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_in_send */

void test_cmd_half_dma_in_send1c(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint32_t size_1;
    uint32_t send_time_1;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &size_1, ""))
                    {
                        if(IOCMD_Arg_Get_Uint32(arg, &send_time_1, ""))
                        {
                            specific = true;
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        if(0 == send_time_1)
        {
            main_test_irq_send_1_connected_transaction_specific(
                usbd,
                iotp_num,
                (uint8_t)num_bufs,
                size,
                size_1,
                IOTP_TEST_RESEND_ON_READY);
        }
        else
        {
            main_test_irq_send_1_connected_transaction_specific(
                usbd,
                iotp_num,
                (uint8_t)num_bufs,
                size,
                size_1,
                IOTP_TEST_RESEND_ON_BUF_EMPTY);
        }
    }
    else
    {
        main_test_irq_send_1_connected_transaction(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_in_send1c */

void test_cmd_half_dma_in_send2c(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint32_t size_1;
    uint32_t send_time_1;
    uint32_t size_2;
    uint32_t send_time_2;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &size_1, ""))
                    {
                        if(IOCMD_Arg_Get_Uint32(arg, &send_time_1, ""))
                        {
                            if(IOCMD_Arg_Get_Uint32(arg, &size_2, ""))
                            {
                                if(IOCMD_Arg_Get_Uint32(arg, &send_time_2, ""))
                                {
                                    specific = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        if((0 == send_time_1) && (0 == send_time_2))
        {
            main_test_irq_send_2_connected_transactions_specific(
                usbd,
                iotp_num,
                (uint8_t)num_bufs,
                size,
                size_1,
                IOTP_TEST_RESEND_ON_READY,
                size_2,
                IOTP_TEST_RESEND_ON_READY);
        }
        else if((1 == send_time_1) && (0 == send_time_2))
        {
            main_test_irq_send_2_connected_transactions_specific(
                usbd,
                iotp_num,
                (uint8_t)num_bufs,
                size,
                size_1,
                IOTP_TEST_RESEND_ON_BUF_EMPTY,
                size_2,
                IOTP_TEST_RESEND_ON_READY);
        }
        else if((0 == send_time_1) && (1 == send_time_2))
        {
            main_test_irq_send_2_connected_transactions_specific(
                usbd,
                iotp_num,
                (uint8_t)num_bufs,
                size,
                size_1,
                IOTP_TEST_RESEND_ON_READY,
                size_2,
                IOTP_TEST_RESEND_ON_BUF_EMPTY);
        }
        else
        {
            main_test_irq_send_2_connected_transactions_specific(
                usbd,
                iotp_num,
                (uint8_t)num_bufs,
                size,
                size_1,
                IOTP_TEST_RESEND_ON_BUF_EMPTY,
                size_2,
                IOTP_TEST_RESEND_ON_BUF_EMPTY);
        }
    }
    else
    {
        main_test_irq_send_2_connected_transactions(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_in_send2c */

void test_cmd_half_dma_in_abort_user(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    USBD_IO_Inout_Data_Size_DT abort_after_packets;
    uint8_t ep_num;
    bool_t specific = false;
    bool_t flush_hw_bufs;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &abort_after_packets, ""))
                    {
                        if(IOCMD_Arg_Get_Bool(arg, &flush_hw_bufs, ""))
                        {
                            specific = true;
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_send_abort_by_user_specific(usbd, iotp_num, (uint8_t)num_bufs, size, abort_after_packets, flush_hw_bufs);
    }
    else
    {
        main_test_irq_send_abort_by_user(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_in_abort_user */

void test_cmd_half_dma_in_abort_rec(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    USBD_IO_Inout_Data_Size_DT abort_after_packets;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &abort_after_packets, ""))
                    {
                        specific = true;
                    }
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_send_abort_by_rec_specific(usbd, iotp_num, (uint8_t)num_bufs, size, abort_after_packets);
    }
    else
    {
        main_test_irq_send_abort_by_rec(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_in_abort_rec */

void test_cmd_half_dma_in_status(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            specific = true;
        }
    }

    if(specific)
    {
        main_test_irq_send_status_specific(usbd, iotp_num);
    }
    else
    {
        main_test_irq_send_status(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_in_status */



void test_cmd_true_dma_in_send(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_send_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_send(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_in_send */

void test_cmd_true_dma_in_send1c(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint32_t size_1;
    uint32_t send_time_1;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &size_1, ""))
                    {
                        if(IOCMD_Arg_Get_Uint32(arg, &send_time_1, ""))
                        {
                            specific = true;
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        if(0 == send_time_1)
        {
            main_test_irq_send_1_connected_transaction_specific(
                usbd,
                iotp_num,
                (uint8_t)num_bufs,
                size,
                size_1,
                IOTP_TEST_RESEND_ON_READY);
        }
        else
        {
            main_test_irq_send_1_connected_transaction_specific(
                usbd,
                iotp_num,
                (uint8_t)num_bufs,
                size,
                size_1,
                IOTP_TEST_RESEND_ON_BUF_EMPTY);
        }
    }
    else
    {
        main_test_irq_send_1_connected_transaction(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_in_send1c */

void test_cmd_true_dma_in_send2c(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint32_t size_1;
    uint32_t send_time_1;
    uint32_t size_2;
    uint32_t send_time_2;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &size_1, ""))
                    {
                        if(IOCMD_Arg_Get_Uint32(arg, &send_time_1, ""))
                        {
                            if(IOCMD_Arg_Get_Uint32(arg, &size_2, ""))
                            {
                                if(IOCMD_Arg_Get_Uint32(arg, &send_time_2, ""))
                                {
                                    specific = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        if((0 == send_time_1) && (0 == send_time_2))
        {
            main_test_irq_send_2_connected_transactions_specific(
                usbd,
                iotp_num,
                (uint8_t)num_bufs,
                size,
                size_1,
                IOTP_TEST_RESEND_ON_READY,
                size_2,
                IOTP_TEST_RESEND_ON_READY);
        }
        else if((1 == send_time_1) && (0 == send_time_2))
        {
            main_test_irq_send_2_connected_transactions_specific(
                usbd,
                iotp_num,
                (uint8_t)num_bufs,
                size,
                size_1,
                IOTP_TEST_RESEND_ON_BUF_EMPTY,
                size_2,
                IOTP_TEST_RESEND_ON_READY);
        }
        else if((0 == send_time_1) && (1 == send_time_2))
        {
            main_test_irq_send_2_connected_transactions_specific(
                usbd,
                iotp_num,
                (uint8_t)num_bufs,
                size,
                size_1,
                IOTP_TEST_RESEND_ON_READY,
                size_2,
                IOTP_TEST_RESEND_ON_BUF_EMPTY);
        }
        else
        {
            main_test_irq_send_2_connected_transactions_specific(
                usbd,
                iotp_num,
                (uint8_t)num_bufs,
                size,
                size_1,
                IOTP_TEST_RESEND_ON_BUF_EMPTY,
                size_2,
                IOTP_TEST_RESEND_ON_BUF_EMPTY);
        }
    }
    else
    {
        main_test_irq_send_2_connected_transactions(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_in_send2c */

void test_cmd_true_dma_in_abort_user(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    USBD_IO_Inout_Data_Size_DT abort_after_packets;
    uint8_t ep_num;
    bool_t specific = false;
    bool_t flush_hw_bufs;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &abort_after_packets, ""))
                    {
                        if(IOCMD_Arg_Get_Bool(arg, &flush_hw_bufs, ""))
                        {
                            specific = true;
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_send_abort_by_user_specific(usbd, iotp_num, (uint8_t)num_bufs, size, abort_after_packets, flush_hw_bufs);
    }
    else
    {
        main_test_irq_send_abort_by_user(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_in_abort_user */

void test_cmd_true_dma_in_abort_rec(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    USBD_IO_Inout_Data_Size_DT abort_after_packets;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &abort_after_packets, ""))
                    {
                        specific = true;
                    }
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_send_abort_by_rec_specific(usbd, iotp_num, (uint8_t)num_bufs, size, abort_after_packets);
    }
    else
    {
        main_test_irq_send_abort_by_rec(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_in_abort_rec */

void test_cmd_true_dma_in_status(IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            specific = true;
        }
    }

    if(specific)
    {
        main_test_irq_send_status_specific(usbd, iotp_num);
    }
    else
    {
        main_test_irq_send_status(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_in_status */


void test_cmd_not_dma_out_ready      (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_ready_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_ready(usbd, "irq");
    }
} /* test_cmd_not_dma_out_ready */

void test_cmd_not_dma_out_wait       (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_wait_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_wait(usbd, "irq");
    }
} /* test_cmd_not_dma_out_wait */

void test_cmd_not_dma_out_1c         (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint8_t ep_num;
    uint32_t size_1;
    uint32_t recv_method_1;
    bool_t recv_to_whole_buf_1 = false;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size_1, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &recv_method_1, ""))
                    {
                        if(IOCMD_Arg_Get_Bool(arg, &recv_to_whole_buf_1, ""))
                        {
                            specific = true;
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        if(0 == recv_method_1)
        {
            main_test_irq_recv_1_specific(usbd, iotp_num, (uint8_t)num_bufs, size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1);
        }
        else
        {
            main_test_irq_recv_1_specific(usbd, iotp_num, (uint8_t)num_bufs, size_1, IOTP_TEST_RECV_AND_WAIT, recv_to_whole_buf_1);
        }
    }
    else
    {
        main_test_irq_recv_and_ready_1_connected_transaction(usbd, "irq");
    }
} /* test_cmd_not_dma_out_1c */

void test_cmd_not_dma_out_2c         (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint8_t ep_num;
    uint32_t size_1;
    uint32_t recv_method_1;
    bool_t recv_to_whole_buf_1 = false;
    uint32_t size_2;
    uint32_t recv_method_2;
    bool_t recv_to_whole_buf_2 = false;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size_1, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &recv_method_1, ""))
                    {
                        if(IOCMD_Arg_Get_Bool(arg, &recv_to_whole_buf_1, ""))
                        {
                            if(IOCMD_Arg_Get_Uint32(arg, &size_2, ""))
                            {
                                if(IOCMD_Arg_Get_Uint32(arg, &recv_method_2, ""))
                                {
                                    if(IOCMD_Arg_Get_Bool(arg, &recv_to_whole_buf_2, ""))
                                    {
                                        specific = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        if((0 == recv_method_1) && (0 == recv_method_2))
        {
            main_test_irq_recv_2_specific(usbd, iotp_num, (uint8_t)num_bufs, size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1, size_2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2);
        }
        else if((0 != recv_method_1) && (0 == recv_method_2))
        {
            main_test_irq_recv_2_specific(usbd, iotp_num, (uint8_t)num_bufs, size_1, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1, size_2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2);
        }
        else if((0 == recv_method_1) && (0 != recv_method_2))
        {
            main_test_irq_recv_2_specific(usbd, iotp_num, (uint8_t)num_bufs, size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1, size_2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2);
        }
        else
        {
            main_test_irq_recv_2_specific(usbd, iotp_num, (uint8_t)num_bufs, size_1, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1, size_2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2);
        }
    }
    else
    {
        main_test_irq_recv_2_connected_transactions(usbd, "irq");
    }
} /* test_cmd_not_dma_out_2c */

void test_cmd_not_dma_out_3c         (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint8_t ep_num;
    uint32_t size_1;
    uint32_t recv_method_1;
    bool_t recv_to_whole_buf_1 = false;
    uint32_t size_2;
    uint32_t recv_method_2;
    bool_t recv_to_whole_buf_2 = false;
    uint32_t size_3;
    uint32_t recv_method_3;
    bool_t recv_to_whole_buf_3 = false;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size_1, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &recv_method_1, ""))
                    {
                        if(IOCMD_Arg_Get_Bool(arg, &recv_to_whole_buf_1, ""))
                        {
                            if(IOCMD_Arg_Get_Uint32(arg, &size_2, ""))
                            {
                                if(IOCMD_Arg_Get_Uint32(arg, &recv_method_2, ""))
                                {
                                    if(IOCMD_Arg_Get_Bool(arg, &recv_to_whole_buf_2, ""))
                                    {
                                        if(IOCMD_Arg_Get_Uint32(arg, &size_3, ""))
                                        {
                                            if(IOCMD_Arg_Get_Uint32(arg, &recv_method_3, ""))
                                            {
                                                if(IOCMD_Arg_Get_Bool(arg, &recv_to_whole_buf_3, ""))
                                                {
                                                    specific = true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        if((0 == recv_method_1) && (0 == recv_method_2) && (0 == recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_3);
        }
        else if((0 != recv_method_1) && (0 == recv_method_2) && (0 == recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_3);
        }
        else if((0 == recv_method_1) && (0 != recv_method_2) && (0 == recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_3);
        }
        else if((0 != recv_method_1) && (0 != recv_method_2) && (0 == recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_3);
        }
        else if((0 == recv_method_1) && (0 == recv_method_2) && (0 != recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_3);
        }
        else if((0 != recv_method_1) && (0 == recv_method_2) && (0 != recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_3);
        }
        else if((0 == recv_method_1) && (0 != recv_method_2) && (0 != recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_3);
        }
        else
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_3);
        }
    }
    else
    {
        main_test_irq_recv_3_connected_transactions(usbd, "irq");
    }
} /* test_cmd_not_dma_out_3c */

void test_cmd_not_dma_out_onready    (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_ready_on_irq_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_ready_on_irq(usbd, "irq");
    }
} /* test_cmd_not_dma_out_onready */

void test_cmd_not_dma_out_onwait     (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_wait_on_irq_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_wait_on_irq(usbd, "irq");
    }
} /* test_cmd_not_dma_out_onwait */

void test_cmd_not_dma_out_lsready    (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_ready_too_less_space_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_ready_too_less_space(usbd, "irq");
    }
} /* test_cmd_not_dma_out_lsready */

void test_cmd_not_dma_out_lswait     (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_wait_too_less_space_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_wait_too_less_space(usbd, "irq");
    }
} /* test_cmd_not_dma_out_lswait */

void test_cmd_not_dma_out_shready    (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_ready_msg_shorten_than_declared_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_ready_msg_shorten_than_declared(usbd, "irq");
    }
} /* test_cmd_not_dma_out_shready */

void test_cmd_not_dma_out_shwait     (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_wait_msg_shorten_than_declared_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_wait_msg_shorten_than_declared(usbd, "irq");
    }
} /* test_cmd_not_dma_out_shwait */


void test_cmd_not_dma_out_rd_auser   (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    USBD_IO_Inout_Data_Size_DT abort_after_packets;
    uint8_t ep_num;
    bool_t specific = false;
    bool_t flush_hw_bufs;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &abort_after_packets, ""))
                    {
                        if(IOCMD_Arg_Get_Bool(arg, &flush_hw_bufs, ""))
                        {
                            specific = true;
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_ready_abort_by_user_specific(usbd, iotp_num, (uint8_t)num_bufs, size, abort_after_packets, flush_hw_bufs);
    }
    else
    {
        main_test_irq_recv_and_ready_abort_by_user(usbd, "irq");
    }
} /* test_cmd_not_dma_out_rd_auser */


void test_cmd_not_dma_out_wt_auser   (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    USBD_IO_Inout_Data_Size_DT abort_after_packets;
    uint8_t ep_num;
    bool_t specific = false;
    bool_t flush_hw_bufs;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &abort_after_packets, ""))
                    {
                        if(IOCMD_Arg_Get_Bool(arg, &flush_hw_bufs, ""))
                        {
                            specific = true;
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_wait_abort_by_user_specific(usbd, iotp_num, (uint8_t)num_bufs, size, abort_after_packets, flush_hw_bufs);
    }
    else
    {
        main_test_irq_recv_and_wait_abort_by_user(usbd, "irq");
    }
} /* test_cmd_not_dma_out_wt_auser */


void test_cmd_not_dma_out_rd_rec   (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    USBD_IO_Inout_Data_Size_DT abort_after_packets;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &abort_after_packets, ""))
                    {
                        specific = true;
                    }
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_ready_abort_by_rec_specific(usbd, iotp_num, (uint8_t)num_bufs, size, abort_after_packets);
    }
    else
    {
        main_test_irq_recv_and_ready_abort_by_rec(usbd, "irq");
    }
} /* test_cmd_not_dma_out_rd_rec */


void test_cmd_not_dma_out_wt_rec   (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    USBD_IO_Inout_Data_Size_DT abort_after_packets;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "not-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_NO_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_NO_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &abort_after_packets, ""))
                    {
                        specific = true;
                    }
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_wait_abort_by_rec_specific(usbd, iotp_num, (uint8_t)num_bufs, size, abort_after_packets);
    }
    else
    {
        main_test_irq_recv_and_wait_abort_by_rec(usbd, "irq");
    }
} /* test_cmd_not_dma_out_wt_rec */

void test_cmd_half_dma_out_ready     (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_ready_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_ready(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_out_ready */

void test_cmd_half_dma_out_wait      (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_wait_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_wait(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_out_wait */

void test_cmd_half_dma_out_1c        (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint8_t ep_num;
    uint32_t size_1;
    uint32_t recv_method_1;
    bool_t recv_to_whole_buf_1 = false;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size_1, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &recv_method_1, ""))
                    {
                        if(IOCMD_Arg_Get_Bool(arg, &recv_to_whole_buf_1, ""))
                        {
                            specific = true;
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        if(0 == recv_method_1)
        {
            main_test_irq_recv_1_specific(usbd, iotp_num, (uint8_t)num_bufs, size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1);
        }
        else
        {
            main_test_irq_recv_1_specific(usbd, iotp_num, (uint8_t)num_bufs, size_1, IOTP_TEST_RECV_AND_WAIT, recv_to_whole_buf_1);
        }
    }
    else
    {
        main_test_irq_recv_and_wait_1_connected_transaction(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_out_1c */

void test_cmd_half_dma_out_2c        (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint8_t ep_num;
    uint32_t size_1;
    uint32_t recv_method_1;
    bool_t recv_to_whole_buf_1 = false;
    uint32_t size_2;
    uint32_t recv_method_2;
    bool_t recv_to_whole_buf_2 = false;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size_1, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &recv_method_1, ""))
                    {
                        if(IOCMD_Arg_Get_Bool(arg, &recv_to_whole_buf_1, ""))
                        {
                            if(IOCMD_Arg_Get_Uint32(arg, &size_2, ""))
                            {
                                if(IOCMD_Arg_Get_Uint32(arg, &recv_method_2, ""))
                                {
                                    if(IOCMD_Arg_Get_Bool(arg, &recv_to_whole_buf_2, ""))
                                    {
                                        specific = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        if((0 == recv_method_1) && (0 == recv_method_2))
        {
            main_test_irq_recv_2_specific(usbd, iotp_num, (uint8_t)num_bufs, size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1, size_2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2);
        }
        else if((0 != recv_method_1) && (0 == recv_method_2))
        {
            main_test_irq_recv_2_specific(usbd, iotp_num, (uint8_t)num_bufs, size_1, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1, size_2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2);
        }
        else if((0 == recv_method_1) && (0 != recv_method_2))
        {
            main_test_irq_recv_2_specific(usbd, iotp_num, (uint8_t)num_bufs, size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1, size_2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2);
        }
        else
        {
            main_test_irq_recv_2_specific(usbd, iotp_num, (uint8_t)num_bufs, size_1, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1, size_2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2);
        }
    }
    else
    {
        main_test_irq_recv_2_connected_transactions(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_out_2c */

void test_cmd_half_dma_out_3c        (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint8_t ep_num;
    uint32_t size_1;
    uint32_t recv_method_1;
    bool_t recv_to_whole_buf_1 = false;
    uint32_t size_2;
    uint32_t recv_method_2;
    bool_t recv_to_whole_buf_2 = false;
    uint32_t size_3;
    uint32_t recv_method_3;
    bool_t recv_to_whole_buf_3 = false;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size_1, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &recv_method_1, ""))
                    {
                        if(IOCMD_Arg_Get_Bool(arg, &recv_to_whole_buf_1, ""))
                        {
                            if(IOCMD_Arg_Get_Uint32(arg, &size_2, ""))
                            {
                                if(IOCMD_Arg_Get_Uint32(arg, &recv_method_2, ""))
                                {
                                    if(IOCMD_Arg_Get_Bool(arg, &recv_to_whole_buf_2, ""))
                                    {
                                        if(IOCMD_Arg_Get_Uint32(arg, &size_3, ""))
                                        {
                                            if(IOCMD_Arg_Get_Uint32(arg, &recv_method_3, ""))
                                            {
                                                if(IOCMD_Arg_Get_Bool(arg, &recv_to_whole_buf_3, ""))
                                                {
                                                    specific = true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        if((0 == recv_method_1) && (0 == recv_method_2) && (0 == recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_3);
        }
        else if((0 != recv_method_1) && (0 == recv_method_2) && (0 == recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_3);
        }
        else if((0 == recv_method_1) && (0 != recv_method_2) && (0 == recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_3);
        }
        else if((0 != recv_method_1) && (0 != recv_method_2) && (0 == recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_3);
        }
        else if((0 == recv_method_1) && (0 == recv_method_2) && (0 != recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_3);
        }
        else if((0 != recv_method_1) && (0 == recv_method_2) && (0 != recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_3);
        }
        else if((0 == recv_method_1) && (0 != recv_method_2) && (0 != recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_3);
        }
        else
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_3);
        }
    }
    else
    {
        main_test_irq_recv_3_connected_transactions(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_out_3c */

void test_cmd_half_dma_out_onready   (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_ready_on_irq_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_ready_on_irq(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_out_onready */

void test_cmd_half_dma_out_onwait    (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_wait_on_irq_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_wait_on_irq(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_out_onwait */

void test_cmd_half_dma_out_lsready   (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_ready_too_less_space_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_ready_too_less_space(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_out_lsready */

void test_cmd_half_dma_out_lswait    (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_wait_too_less_space_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_wait_too_less_space(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_out_lswait */

void test_cmd_half_dma_out_shready   (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_ready_msg_shorten_than_declared_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_ready_msg_shorten_than_declared(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_out_shready */

void test_cmd_half_dma_out_shwait    (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_wait_msg_shorten_than_declared_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_wait_msg_shorten_than_declared(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_out_shwait */

void test_cmd_half_dma_out_rd_auser  (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    USBD_IO_Inout_Data_Size_DT abort_after_packets;
    uint8_t ep_num;
    bool_t specific = false;
    bool_t flush_hw_bufs;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &abort_after_packets, ""))
                    {
                        if(IOCMD_Arg_Get_Bool(arg, &flush_hw_bufs, ""))
                        {
                            specific = true;
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_ready_abort_by_user_specific(usbd, iotp_num, (uint8_t)num_bufs, size, abort_after_packets, flush_hw_bufs);
    }
    else
    {
        main_test_irq_recv_and_ready_abort_by_user(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_out_rd_auser */

void test_cmd_half_dma_out_wt_auser  (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    USBD_IO_Inout_Data_Size_DT abort_after_packets;
    uint8_t ep_num;
    bool_t specific = false;
    bool_t flush_hw_bufs;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &abort_after_packets, ""))
                    {
                        if(IOCMD_Arg_Get_Bool(arg, &flush_hw_bufs, ""))
                        {
                            specific = true;
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_wait_abort_by_user_specific(usbd, iotp_num, (uint8_t)num_bufs, size, abort_after_packets, flush_hw_bufs);
    }
    else
    {
        main_test_irq_recv_and_wait_abort_by_user(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_out_wt_auser */

void test_cmd_half_dma_out_rd_rec  (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    USBD_IO_Inout_Data_Size_DT abort_after_packets;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &abort_after_packets, ""))
                    {
                        specific = true;
                    }
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_ready_abort_by_rec_specific(usbd, iotp_num, (uint8_t)num_bufs, size, abort_after_packets);
    }
    else
    {
        main_test_irq_recv_and_ready_abort_by_rec(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_out_rd_rec */

void test_cmd_half_dma_out_wt_rec  (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    USBD_IO_Inout_Data_Size_DT abort_after_packets;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "half-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_HALF_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_HALF_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &abort_after_packets, ""))
                    {
                        specific = true;
                    }
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_wait_abort_by_rec_specific(usbd, iotp_num, (uint8_t)num_bufs, size, abort_after_packets);
    }
    else
    {
        main_test_irq_recv_and_wait_abort_by_rec(usbd, "irq_half_dma");
    }
} /* test_cmd_half_dma_out_wt_rec */

void test_cmd_true_dma_out_ready     (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_ready_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_ready(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_out_ready */

void test_cmd_true_dma_out_wait      (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_wait_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_wait(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_out_wait */

void test_cmd_true_dma_out_1c        (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint8_t ep_num;
    uint32_t size_1;
    uint32_t recv_method_1;
    bool_t recv_to_whole_buf_1 = false;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size_1, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &recv_method_1, ""))
                    {
                        if(IOCMD_Arg_Get_Bool(arg, &recv_to_whole_buf_1, ""))
                        {
                            specific = true;
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        if(0 == recv_method_1)
        {
            main_test_irq_recv_1_specific(usbd, iotp_num, (uint8_t)num_bufs, size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1);
        }
        else
        {
            main_test_irq_recv_1_specific(usbd, iotp_num, (uint8_t)num_bufs, size_1, IOTP_TEST_RECV_AND_WAIT, recv_to_whole_buf_1);
        }
    }
    else
    {
        main_test_irq_recv_and_wait_1_connected_transaction(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_out_1c */

void test_cmd_true_dma_out_2c        (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint8_t ep_num;
    uint32_t size_1;
    uint32_t recv_method_1;
    bool_t recv_to_whole_buf_1 = false;
    uint32_t size_2;
    uint32_t recv_method_2;
    bool_t recv_to_whole_buf_2 = false;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size_1, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &recv_method_1, ""))
                    {
                        if(IOCMD_Arg_Get_Bool(arg, &recv_to_whole_buf_1, ""))
                        {
                            if(IOCMD_Arg_Get_Uint32(arg, &size_2, ""))
                            {
                                if(IOCMD_Arg_Get_Uint32(arg, &recv_method_2, ""))
                                {
                                    if(IOCMD_Arg_Get_Bool(arg, &recv_to_whole_buf_2, ""))
                                    {
                                        specific = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        if((0 == recv_method_1) && (0 == recv_method_2))
        {
            main_test_irq_recv_2_specific(usbd, iotp_num, (uint8_t)num_bufs, size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1, size_2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2);
        }
        else if((0 != recv_method_1) && (0 == recv_method_2))
        {
            main_test_irq_recv_2_specific(usbd, iotp_num, (uint8_t)num_bufs, size_1, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1, size_2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2);
        }
        else if((0 == recv_method_1) && (0 != recv_method_2))
        {
            main_test_irq_recv_2_specific(usbd, iotp_num, (uint8_t)num_bufs, size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1, size_2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2);
        }
        else
        {
            main_test_irq_recv_2_specific(usbd, iotp_num, (uint8_t)num_bufs, size_1, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1, size_2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2);
        }
    }
    else
    {
        main_test_irq_recv_2_connected_transactions(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_out_2c */

void test_cmd_true_dma_out_3c        (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint8_t ep_num;
    uint32_t size_1;
    uint32_t recv_method_1;
    bool_t recv_to_whole_buf_1 = false;
    uint32_t size_2;
    uint32_t recv_method_2;
    bool_t recv_to_whole_buf_2 = false;
    uint32_t size_3;
    uint32_t recv_method_3;
    bool_t recv_to_whole_buf_3 = false;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size_1, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &recv_method_1, ""))
                    {
                        if(IOCMD_Arg_Get_Bool(arg, &recv_to_whole_buf_1, ""))
                        {
                            if(IOCMD_Arg_Get_Uint32(arg, &size_2, ""))
                            {
                                if(IOCMD_Arg_Get_Uint32(arg, &recv_method_2, ""))
                                {
                                    if(IOCMD_Arg_Get_Bool(arg, &recv_to_whole_buf_2, ""))
                                    {
                                        if(IOCMD_Arg_Get_Uint32(arg, &size_3, ""))
                                        {
                                            if(IOCMD_Arg_Get_Uint32(arg, &recv_method_3, ""))
                                            {
                                                if(IOCMD_Arg_Get_Bool(arg, &recv_to_whole_buf_3, ""))
                                                {
                                                    specific = true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        if((0 == recv_method_1) && (0 == recv_method_2) && (0 == recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_3);
        }
        else if((0 != recv_method_1) && (0 == recv_method_2) && (0 == recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_3);
        }
        else if((0 == recv_method_1) && (0 != recv_method_2) && (0 == recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_3);
        }
        else if((0 != recv_method_1) && (0 != recv_method_2) && (0 == recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_3);
        }
        else if((0 == recv_method_1) && (0 == recv_method_2) && (0 != recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_3);
        }
        else if((0 != recv_method_1) && (0 == recv_method_2) && (0 != recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_3);
        }
        else if((0 == recv_method_1) && (0 != recv_method_2) && (0 != recv_method_3))
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_READY, recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_3);
        }
        else
        {
            main_test_irq_recv_3_specific(
                usbd, iotp_num, (uint8_t)num_bufs,
                size_1, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_1,
                size_2, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_2,
                size_3, IOTP_TEST_RECV_AND_WAIT , recv_to_whole_buf_3);
        }
    }
    else
    {
        main_test_irq_recv_3_connected_transactions(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_out_3c */

void test_cmd_true_dma_out_onready   (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_ready_on_irq_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_ready_on_irq(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_out_onready */

void test_cmd_true_dma_out_onwait    (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_wait_on_irq_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_wait_on_irq(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_out_onwait */

void test_cmd_true_dma_out_lsready   (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_ready_too_less_space_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_ready_too_less_space(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_out_lsready */

void test_cmd_true_dma_out_lswait    (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_wait_too_less_space_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_wait_too_less_space(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_out_lswait */

void test_cmd_true_dma_out_shready   (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_ready_msg_shorten_than_declared_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_ready_msg_shorten_than_declared(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_out_shready */

void test_cmd_true_dma_out_shwait    (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    specific = true;
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_wait_msg_shorten_than_declared_specific(usbd, iotp_num, (uint8_t)num_bufs, size);
    }
    else
    {
        main_test_irq_recv_and_wait_msg_shorten_than_declared(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_out_shwait */

void test_cmd_true_dma_out_rd_auser  (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    USBD_IO_Inout_Data_Size_DT abort_after_packets;
    uint8_t ep_num;
    bool_t specific = false;
    bool_t flush_hw_bufs;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &abort_after_packets, ""))
                    {
                        if(IOCMD_Arg_Get_Bool(arg, &flush_hw_bufs, ""))
                        {
                            specific = true;
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_ready_abort_by_user_specific(usbd, iotp_num, (uint8_t)num_bufs, size, abort_after_packets, flush_hw_bufs);
    }
    else
    {
        main_test_irq_recv_and_ready_abort_by_user(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_out_rd_auser */

void test_cmd_true_dma_out_wt_auser  (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    USBD_IO_Inout_Data_Size_DT abort_after_packets;
    uint8_t ep_num;
    bool_t specific = false;
    bool_t flush_hw_bufs;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &abort_after_packets, ""))
                    {
                        if(IOCMD_Arg_Get_Bool(arg, &flush_hw_bufs, ""))
                        {
                            specific = true;
                        }
                    }
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_wait_abort_by_user_specific(usbd, iotp_num, (uint8_t)num_bufs, size, abort_after_packets, flush_hw_bufs);
    }
    else
    {
        main_test_irq_recv_and_wait_abort_by_user(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_out_wt_auser */

void test_cmd_true_dma_out_rd_rec  (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    USBD_IO_Inout_Data_Size_DT abort_after_packets;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &abort_after_packets, ""))
                    {
                        specific = true;
                    }
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_ready_abort_by_rec_specific(usbd, iotp_num, (uint8_t)num_bufs, size, abort_after_packets);
    }
    else
    {
        main_test_irq_recv_and_ready_abort_by_rec(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_out_rd_rec */

void test_cmd_true_dma_out_wt_rec  (IOCMD_Arg_DT *arg)
{
    USBD_Params_XT *usbd = usbd_param_ptr;
    uint32_t iotp_num;
    uint32_t num_bufs;
    uint32_t size;
    USBD_IO_Inout_Data_Size_DT abort_after_packets;
    uint8_t ep_num;
    bool_t specific = false;

    USBD_NOTICE(MAIN_APP_TEST_INTRO, "true-dma tests");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_OUT, PORT_TEST_TRUE_DMA);
        port_test_set_dma_type(ep_num, USB_EP_DIRECTION_IN, PORT_TEST_TRUE_DMA);
    }

    if(NULL != arg)
    {
        if(IOCMD_Arg_Get_Uint32(arg, &iotp_num, ""))
        {
            if(IOCMD_Arg_Get_Uint32(arg, &num_bufs, ""))
            {
                if(IOCMD_Arg_Get_Uint32(arg, &size, ""))
                {
                    if(IOCMD_Arg_Get_Uint32(arg, &abort_after_packets, ""))
                    {
                        specific = true;
                    }
                }
            }
        }
    }

    if(specific)
    {
        main_test_irq_recv_and_wait_abort_by_rec_specific(usbd, iotp_num, (uint8_t)num_bufs, size, abort_after_packets);
    }
    else
    {
        main_test_irq_recv_and_wait_abort_by_rec(usbd, "irq_true_dma");
    }
} /* test_cmd_true_dma_out_wt_rec */

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

void test_cmd_logs_enable            (IOCMD_Arg_DT *arg)
{
    IOCMD_Set_All_Logs(IOCMD_LOG_LEVEL_DEBUG_LO, IOCMD_LOG_LEVEL_DEBUG_LO);
}

void test_cmd_logs_disable           (IOCMD_Arg_DT *arg)
{
    IOCMD_Set_All_Logs(IOCMD_LOG_LEVEL_EMERG, IOCMD_LOG_LEVEL_EMERG);
}

void test_cmd_entr_enable            (IOCMD_Arg_DT *arg)
{
    IOCMD_Set_All_Entrances(IOCMD_ENTRANCE_ENABLED);
}

void test_cmd_entr_disable           (IOCMD_Arg_DT *arg)
{
    IOCMD_Set_All_Entrances(IOCMD_ENTRANCE_DISABLED);
}

void test_cmd_print                  (IOCMD_Arg_DT *arg)
{
    IOCMD_Proc_Buffered_Logs(true, main_get_exe(), working_buf, sizeof(working_buf));
}

static IOCMD_Command_Tree_XT cmd_tree[] =
{
IOCMD_ELEM(         "tplist"              , test_cmd_tp_list               , "lists all TPs that can be used in test"),
IOCMD_ELEM(         "all"                 , test_cmd_all                   , "performs all tests"),
IOCMD_ELEM(         "in"                  , test_cmd_all_in                , "performs all in tests"),
IOCMD_ELEM(         "out"                 , test_cmd_all_out               , "performs all out tests"),
IOCMD_GROUP_START(          "norm"                ,                                  "group of commands for not-DMA tests"),
    IOCMD_ELEM(         ""                , test_cmd_not_dma_all           , "performs all not-dma tests"),
    IOCMD_GROUP_START(          "in"              ,                                  "group of commands for not-DMA IN tests"),
        IOCMD_ELEM(         ""            , test_cmd_not_dma_in_all        , "performs all not-dma IN tests"),
        IOCMD_ELEM(         "send"        , test_cmd_not_dma_in_send       , "performs not-dma IN \"send\" test\n"
                                                                                          "params:\n" \
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "send1c"      , test_cmd_not_dma_in_send1c     , "performs not-dma IN \"send 1 connected transaction\" test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<size_1>>, <<send_time_1>>\n"
                                                                                          "for all cases of this test: none\n"
                                                                                          "where: send_time_x: 0 - on ready; 1 - on buf empty"),
        IOCMD_ELEM(         "send2c"      , test_cmd_not_dma_in_send2c     , "performs not-dma IN \"send 2 connected transactions\" test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<size_1>>, <<send_time_1>>, <<size_2>>, <<send_time_2>>\n"
                                                                                          "for all cases of this test: none\n"
                                                                                          "where: send_time_x: 0 - on ready; 1 - on buf empty"),
        IOCMD_ELEM(         "auser"       , test_cmd_not_dma_in_abort_user , "performs not-dma IN \"send aborted by user\" test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<abort_after_packets>>, <<flush_hw_bufs>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "arec"       , test_cmd_not_dma_in_abort_rec   , "performs not-dma IN \"send aborted by reconfigure\" test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<abort_after_packets>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "status"      , test_cmd_not_dma_in_status     , "performs not-dma IN \"send status\" test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>\n"
                                                                                          "for all cases of this test: none"),
    IOCMD_GROUP_END(),
    IOCMD_GROUP_START(          "out"             ,                                  "group of commands for not-DMA OUT tests"),
        IOCMD_ELEM(         ""            , test_cmd_not_dma_out_all       , "performs all not-dma OUT tests"),
        IOCMD_ELEM(         "ready"       , test_cmd_not_dma_out_ready     , "performs \"read and ready\" not-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "wait"        , test_cmd_not_dma_out_wait      , "performs \"read nad wait\" not-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "1c"          , test_cmd_not_dma_out_1c        , "performs \"receive 1 connected transaction\" not-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size_1>>, <<recv_method_1>>, <<recv_to_whole_buf_1>>\n"
                                                                                          "for all cases of this test: none\n"
                                                                                          "where: recv_method_x: 0 - read and ready; 1 - read and wait\n"
                                                                                          "recv_to_whole_buf_1 - if 1 then max size buf will be provided to IOTP, 0 otherwise"),
        IOCMD_ELEM(         "2c"          , test_cmd_not_dma_out_2c        , "performs \"receive 2 connected transactions\" not-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>,\n"
                                                                                          "                   <<size_1>>, <<recv_method_1>>, <<recv_to_whole_buf_1>>,\n"
                                                                                          "                   <<size_2>>, <<recv_method_2>>, <<recv_to_whole_buf_2>>\n"
                                                                                          "for all cases of this test: none\n"
                                                                                          "where: recv_method_x: 0 - read and ready; 1 - read and wait\n"
                                                                                          "recv_to_whole_buf_1 - if 1 then max size buf will be provided to IOTP, 0 otherwise"),
        IOCMD_ELEM(         "3c"          , test_cmd_not_dma_out_3c        , "performs \"receive 3 connected transactions\" not-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>,\n"
                                                                                          "                   <<size_1>>, <<recv_method_1>>, <<recv_to_whole_buf_1>>,\n"
                                                                                          "                   <<size_2>>, <<recv_method_2>>, <<recv_to_whole_buf_2>>,\n"
                                                                                          "                   <<size_3>>, <<recv_method_3>>, <<recv_to_whole_buf_3>>\n"
                                                                                          "for all cases of this test: none\n"
                                                                                          "where: recv_method_x: 0 - read and ready; 1 - read and wait\n"
                                                                                          "recv_to_whole_buf_1 - if 1 then max size buf will be provided to IOTP, 0 otherwise"),
        IOCMD_ELEM(         "onready"     , test_cmd_not_dma_out_onready   , "performs \"receive and ready on event\" not-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "onwait"      , test_cmd_not_dma_out_onwait    , "performs \"receive and wait on event\" not-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "lsready"     , test_cmd_not_dma_out_lsready   , "performs \"receive and ready - too less space\" not-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "lswait"      , test_cmd_not_dma_out_lswait    , "performs \"receive and wait - too less space\" not-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "shready"     , test_cmd_not_dma_out_shready   , "performs \"receive and ready - msg shorten than declared\" not-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "shwait"      , test_cmd_not_dma_out_shwait    , "performs \"receive and wait - msg shorten than declared\" not-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "rdauser"     , test_cmd_not_dma_out_rd_auser  , "performs \"read and ready - abort by user\" not-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<abort_after_packets>>, <<flush_hw_bufs>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "wtauser"     , test_cmd_not_dma_out_wt_auser  , "performs \"read and wait - abort by user\" not-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<abort_after_packets>>, <<flush_hw_bufs>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "rdrec"       , test_cmd_not_dma_out_rd_rec    , "performs \"read and ready - abort by reconfigure\" not-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<abort_after_packets>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "wtrec"       , test_cmd_not_dma_out_wt_rec    , "performs \"read and wait - abort by reconfigure\" not-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<abort_after_packets>>\n"
                                                                                          "for all cases of this test: none"),
    IOCMD_GROUP_END(),
IOCMD_GROUP_END(),
IOCMD_GROUP_START(          "half"                ,                                  "group of commands for half-DMA tests"),
    IOCMD_ELEM(         ""                , test_cmd_half_dma_all          , "performs all half-dma tests"),
    IOCMD_GROUP_START(          "in"              ,                                  "group of commands for half-DMA IN tests"),
        IOCMD_ELEM(         ""            , test_cmd_half_dma_in_all       , "performs all half-dma IN tests"),
        IOCMD_ELEM(         "send"        , test_cmd_half_dma_in_send      , "performs half-dma IN \"send\" test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "send1c"      , test_cmd_half_dma_in_send1c    , "performs half-dma IN \"send 1 connected transaction\" test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<size_1>>, <<send_time_1>>\n"
                                                                                          "for all cases of this test: none\n"
                                                                                          "where: send_time_x: 0 - on ready; 1 - on buf empty"),
        IOCMD_ELEM(         "send2c"      , test_cmd_half_dma_in_send2c    , "performs half-dma IN \"send 2 connected transactions\" test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<size_1>>, <<send_time_1>>, <<size_2>>, <<send_time_2>>\n"
                                                                                          "for all cases of this test: none\n"
                                                                                          "where: send_time_x: 0 - on ready; 1 - on buf empty"),
        IOCMD_ELEM(         "auser"       , test_cmd_half_dma_in_abort_user, "performs half-dma IN \"send aborted by user\" test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<abort_after_packets>>, <<flush_hw_bufs>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "arec"        , test_cmd_half_dma_in_abort_rec , "performs half-dma IN \"send aborted by reconfigure\" test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<abort_after_packets>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "status"      , test_cmd_half_dma_in_status    , "performs half-dma IN \"send status\" test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>\n"
                                                                                          "for all cases of this test: none"),
    IOCMD_GROUP_END(),
    IOCMD_GROUP_START(          "out"             ,                                  "group of commands for half-DMA OUT tests"),
        IOCMD_ELEM(         ""            , test_cmd_half_dma_out_all      , "performs all half-dma OUT tests"),
        IOCMD_ELEM(         "ready"       , test_cmd_half_dma_out_ready    , "performs \"read and ready\" half-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "wait"        , test_cmd_half_dma_out_wait     , "performs \"read nad wait\" half-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "1c"          , test_cmd_half_dma_out_1c       , "performs \"receive 1 connected transaction\" half-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size_1>>, <<recv_method_1>>, <<recv_to_whole_buf_1>>\n"
                                                                                          "for all cases of this test: none\n"
                                                                                          "where: recv_method_x: 0 - read and ready; 1 - read and wait\n"
                                                                                          "recv_to_whole_buf_1 - if 1 then max size buf will be provided to IOTP, 0 otherwise"),
        IOCMD_ELEM(         "2c"          , test_cmd_half_dma_out_2c       , "performs \"receive 2 connected transactions\" half-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>,\n"
                                                                                          "                   <<size_1>>, <<recv_method_1>>, <<recv_to_whole_buf_1>>,\n"
                                                                                          "                   <<size_2>>, <<recv_method_2>>, <<recv_to_whole_buf_2>>\n"
                                                                                          "for all cases of this test: none\n"
                                                                                          "where: recv_method_x: 0 - read and ready; 1 - read and wait\n"
                                                                                          "recv_to_whole_buf_1 - if 1 then max size buf will be provided to IOTP, 0 otherwise"),
        IOCMD_ELEM(         "3c"          , test_cmd_half_dma_out_3c       , "performs \"receive 3 connected transactions\" half-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>,\n"
                                                                                          "                   <<size_1>>, <<recv_method_1>>, <<recv_to_whole_buf_1>>,\n"
                                                                                          "                   <<size_2>>, <<recv_method_2>>, <<recv_to_whole_buf_2>>,\n"
                                                                                          "                   <<size_3>>, <<recv_method_3>>, <<recv_to_whole_buf_3>>\n"
                                                                                          "for all cases of this test: none\n"
                                                                                          "where: recv_method_x: 0 - read and ready; 1 - read and wait\n"
                                                                                          "recv_to_whole_buf_1 - if 1 then max size buf will be provided to IOTP, 0 otherwise"),
        IOCMD_ELEM(         "onready"     , test_cmd_half_dma_out_onready  , "performs \"receive and ready on event\" half-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "onwait"      , test_cmd_half_dma_out_onwait   , "performs \"receive and wait on event\" half-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "lsready"     , test_cmd_half_dma_out_lsready  , "performs \"receive and ready - too less space\" half-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "lswait"      , test_cmd_half_dma_out_lswait   , "performs \"receive and wait - too less space\" half-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "shready"     , test_cmd_half_dma_out_shready  , "performs \"receive and ready - msg shorten than declared\" half-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "shwait"      , test_cmd_half_dma_out_shwait   , "performs \"receive and wait - msg shorten than declared\" half-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "rdauser"     , test_cmd_half_dma_out_rd_auser , "performs \"read and ready - abort by user\" half-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<abort_after_packets>>, <<flush_hw_bufs>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "wtauser"     , test_cmd_half_dma_out_wt_auser , "performs \"read and wait - abort by user\" half-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<abort_after_packets>>, <<flush_hw_bufs>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "rdrec"       , test_cmd_half_dma_out_rd_rec   , "performs \"read and ready - abort by reconfigure\" half-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<abort_after_packets>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "wtrec"       , test_cmd_half_dma_out_wt_rec   , "performs \"read and wait - abort by reconfigure\" half-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<abort_after_packets>>\n"
                                                                                          "for all cases of this test: none"),
    IOCMD_GROUP_END(),
IOCMD_GROUP_END(),
IOCMD_GROUP_START(          "true"                ,                                  "group of commands for true-DMA tests"),
    IOCMD_ELEM(         ""                , test_cmd_true_dma_all          , "performs all true-dma tests"),
    IOCMD_GROUP_START(          "in"              ,                                  "group of commands for true-DMA IN tests"),
        IOCMD_ELEM(         ""            , test_cmd_true_dma_in_all       , "performs all true-dma IN tests"),
        IOCMD_ELEM(         "send"        , test_cmd_true_dma_in_send      , "performs true-dma IN \"send\" test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "send1c"      , test_cmd_true_dma_in_send1c    , "performs true-dma IN \"send 1 connected transaction\" test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<size_1>>, <<send_time_1>>\n"
                                                                                          "for all cases of this test: none\n"
                                                                                          "where: send_time_x: 0 - on ready; 1 - on buf empty"),
        IOCMD_ELEM(         "send2c"      , test_cmd_true_dma_in_send2c    , "performs true-dma IN \"send 2 connected transactions\" test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<size_1>>, <<send_time_1>>, <<size_2>>, <<send_time_2>>\n"
                                                                                          "for all cases of this test: none\n"
                                                                                          "where: send_time_x: 0 - on ready; 1 - on buf empty"),
        IOCMD_ELEM(         "auser"       , test_cmd_true_dma_in_abort_user, "performs true-dma IN \"send aborted by user\" test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<abort_after_packets>>, <<flush_hw_bufs>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "arec"        , test_cmd_true_dma_in_abort_rec,  "performs true-dma IN \"send aborted by reconfigure\" test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<abort_after_packets>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "status"      , test_cmd_true_dma_in_status    , "performs true-dma IN \"send status\" test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>\n"
                                                                                          "for all cases of this test: none"),
    IOCMD_GROUP_END(),
    IOCMD_GROUP_START(          "out"             ,                                  "group of commands for true-DMA OUT tests"),
        IOCMD_ELEM(         ""            , test_cmd_true_dma_out_all      , "performs all true-dma OUT tests"),
        IOCMD_ELEM(         "ready"       , test_cmd_true_dma_out_ready    , "performs \"read and ready\" true-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "wait"        , test_cmd_true_dma_out_wait     , "performs \"read nad wait\" true-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "1c"          , test_cmd_true_dma_out_1c       , "performs \"receive 1 connected transaction\" true-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size_1>>, <<recv_method_1>>, <<recv_to_whole_buf_1>>\n"
                                                                                          "for all cases of this test: none\n"
                                                                                          "where: recv_method_x: 0 - read and ready; 1 - read and wait\n"
                                                                                          "recv_to_whole_buf_1 - if 1 then max size buf will be provided to IOTP, 0 otherwise"),
        IOCMD_ELEM(         "2c"          , test_cmd_true_dma_out_2c       , "performs \"receive 2 connected transactions\" true-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>,\n"
                                                                                          "                   <<size_1>>, <<recv_method_1>>, <<recv_to_whole_buf_1>>,\n"
                                                                                          "                   <<size_2>>, <<recv_method_2>>, <<recv_to_whole_buf_2>>\n"
                                                                                          "for all cases of this test: none\n"
                                                                                          "where: recv_method_x: 0 - read and ready; 1 - read and wait\n"
                                                                                          "recv_to_whole_buf_1 - if 1 then max size buf will be provided to IOTP, 0 otherwise"),
        IOCMD_ELEM(         "3c"          , test_cmd_true_dma_out_3c       , "performs \"receive 3 connected transactions\" true-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>,\n"
                                                                                          "                   <<size_1>>, <<recv_method_1>>, <<recv_to_whole_buf_1>>,\n"
                                                                                          "                   <<size_2>>, <<recv_method_2>>, <<recv_to_whole_buf_2>>,\n"
                                                                                          "                   <<size_3>>, <<recv_method_3>>, <<recv_to_whole_buf_3>>\n"
                                                                                          "for all cases of this test: none\n"
                                                                                          "where: recv_method_x: 0 - read and ready; 1 - read and wait\n"
                                                                                          "recv_to_whole_buf_1 - if 1 then max size buf will be provided to IOTP, 0 otherwise"),
        IOCMD_ELEM(         "onready"     , test_cmd_true_dma_out_onready  , "performs \"receive and ready on event\" true-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "onwait"      , test_cmd_true_dma_out_onwait   , "performs \"receive and wait on event\" true-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "lsready"     , test_cmd_true_dma_out_lsready  , "performs \"receive and ready - too less space\" true-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "lswait"      , test_cmd_true_dma_out_lswait   , "performs \"receive and wait - too less space\" true-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "shready"     , test_cmd_true_dma_out_shready  , "performs \"receive and ready - msg shorten than declared\" true-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "shwait"      , test_cmd_true_dma_out_shwait   , "performs \"receive and wait - msg shorten than declared\" true-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "rdauser"     , test_cmd_true_dma_out_rd_auser , "performs \"read and ready - abort by user\" true-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<abort_after_packets>>, <<flush_hw_bufs>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "wtauser"     , test_cmd_true_dma_out_wt_auser , "performs \"read and wait - abort by user\" true-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<abort_after_packets>>, <<flush_hw_bufs>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "rdrec"       , test_cmd_true_dma_out_rd_rec   , "performs \"read and ready - abort by reconfigure\" true-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<abort_after_packets>>\n"
                                                                                          "for all cases of this test: none"),
        IOCMD_ELEM(         "wtrec"       , test_cmd_true_dma_out_wt_rec   , "performs \"read and wait - abort by reconfigure\" true-dma OUT test\n"
                                                                                          "params:\n"
                                                                                          "for specific case: <<iotp>>, <<num_bufs>>, <<size>>, <<abort_after_packets>>\n"
                                                                                          "for all cases of this test: none"),
    IOCMD_GROUP_END(),
IOCMD_GROUP_END(),
IOCMD_ELEM(         "loop"                , test_cmd_loop                  , "allows executing commands in loop"),
IOCMD_ELEM(         "exit"                , test_cmd_exit                  , "exits loop commands executing"),
IOCMD_GROUP_START(          "logs"                ,                                  "group of commands logs manipulation"),
    IOCMD_ELEM(         "enable"          , test_cmd_logs_enable           , "enables all logs"),
    IOCMD_ELEM(         "disable"         , test_cmd_logs_disable          , "disables all logs"),
IOCMD_GROUP_END(),
IOCMD_GROUP_START(          "entr"                ,                                  "group of commands entrances logging manipulation"),
    IOCMD_ELEM(         "enable"          , test_cmd_entr_enable           , "enables all entrances"),
    IOCMD_ELEM(         "disable"         , test_cmd_entr_disable          , "disables all entrances"),
IOCMD_GROUP_END(),
IOCMD_ELEM(         "print"               , test_cmd_print                 , "allows executing commands in loop"),
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

    printf("\n\rtest init:\n\r");
    for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
    {
        iotp_test_init(&usbd, USB_EP_DIRECTION_OUT, ep_num);
        iotp_test_init(&usbd, USB_EP_DIRECTION_IN, ep_num);
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
        if(1 == num_args)
        {
            arg_tab[1] = "all";
            num_args = 2;
        }
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

    return 0;
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
