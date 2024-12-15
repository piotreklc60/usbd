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

#include "main_usbd_init.h"
#include "usbd_port_stm32_cat_a.h"
#include "iocmd.h"

#define VCOM_0_NOTIF_IF_NUM        0
#define VCOM_0_DATA_IF_NUM         1
#define VCOM_1_NOTIF_IF_NUM        2
#define VCOM_1_DATA_IF_NUM         3
#define VCOM_2_NOTIF_IF_NUM        4
#define VCOM_2_DATA_IF_NUM         5

static USBD_Params_XT  usbd;
static USBDC_Params_XT usbdc;
/* VCOM 0 */
CDC_VCOM_Params_XT vcom_0;
static Buff_Ring_XT buf_0_in;
static Buff_Ring_XT buf_0_out;
static Buff_Ring_Extensions_XT buf_0_in_ext;
static Buff_Ring_Extensions_XT buf_0_out_ext;
static uint8_t mem_0_in[256];
static uint8_t mem_0_out[256];
/* VCOM 1 */
CDC_VCOM_Params_XT vcom_1;
Buff_Ring_XT buf_1_in;
static Buff_Ring_XT buf_1_out;
static Buff_Ring_Extensions_XT buf_1_in_ext;
static Buff_Ring_Extensions_XT buf_1_out_ext;
static uint8_t mem_1_in[256];
static uint8_t mem_1_out[256];
/* VCOM 2 */
CDC_VCOM_Params_XT vcom_2;
static Buff_Ring_XT buf_2;
static Buff_Ring_Extensions_XT buf_2_ext;
static uint8_t mem_2[256];

static uint8_t config_desc[] =
{
   USB_CONFIGURATION_DESC_TABLE_FILL(
      /* _wTotalLength */        0,
      /* _bNumInterfaces */      6,
      /* _bConfigurationValue */ 1,
      /* _iConfiguration */      0,
      /* _self_powered */        USB_CFG_DESC_SELF_POWERED,
      /* _remote_wakeup */       USB_CFG_DESC_NO_REMOTE_WAKEUP,
      /* _max_current_mA */      25
   ),

   CDC_VCOM_CONFIG_DESC_PART(
      /* _iFunction */           0,
      /* _notif_if_num */        VCOM_0_NOTIF_IF_NUM,
      /* _notif_iInterface */    0,
      /* _ep_notif */            1,
      /* _ep_notif_mps */        16,
      /* _data_if_num */         VCOM_0_DATA_IF_NUM,
      /* _data_iInterface */     0,
      /* _ep_in */               2,
      /* _ep_in_mps */           64,
      /* _ep_out */              2,
      /* _ep_out_mps */          64),

   CDC_VCOM_CONFIG_DESC_PART(
      /* _iFunction */           0,
      /* _notif_if_num */        VCOM_1_NOTIF_IF_NUM,
      /* _notif_iInterface */    0,
      /* _ep_notif */            3,
      /* _ep_notif_mps */        16,
      /* _data_if_num */         VCOM_1_DATA_IF_NUM,
      /* _data_iInterface */     0,
      /* _ep_in */               4,
      /* _ep_in_mps */           64,
      /* _ep_out */              4,
      /* _ep_out_mps */          64),

   CDC_VCOM_CONFIG_DESC_PART(
      /* _iFunction */           0,
      /* _notif_if_num */        VCOM_2_NOTIF_IF_NUM,
      /* _notif_iInterface */    0,
      /* _ep_notif */            5,
      /* _ep_notif_mps */        16,
      /* _data_if_num */         VCOM_2_DATA_IF_NUM,
      /* _data_iInterface */     0,
      /* _ep_in */               6,
      /* _ep_in_mps */           64,
      /* _ep_out */              6,
      /* _ep_out_mps */          64)

}; /* config_desc */

const uint16_t manuf_string[] =
{
   0x0320,
   'P',
   'i',
   'o',
   't',
   'r',
   ' ',
   'W',
   'o',
   'j',
   't',
   'o',
   'w',
   'i',
   'c',
   'z'
};

const uint16_t product_string[] =
{
   0x032A,
   't',
   'r',
   'i',
   'p',
   'p',
   'l',
   'e',
   ' ',
   'v',
   'c',
   'o',
   'm',
   ' ',
   'e',
   'x',
   'a',
   'm',
   'p',
   'l',
   'e'
};

const uint16_t serial_string[] =
{
   0x030C,
   '1',
   '2',
   '3',
   '4',
   '5'
};

void main_usbd_init(void)
{
   USBD_DEV_Installation_Result_XT install_result;
   USBD_DEV_Config_Desc_Check_Result_XT check_result;
   USB_Device_Desc_DT dev;

   IOCMD_ENTER_FUNC(MAIN_APP);

   USBD_Init(&usbd);
   USBDC_Init(&usbdc);

   USB_SET_CONFIGURATION_DESC_W_TOTAL_LENGTH(config_desc, sizeof(config_desc));

   check_result = USBD_DEV_Check_Config_Desc(&usbd, USBD_PORT_STM32_CAT_A, (uint8_t*)(&config_desc), sizeof(config_desc));

   if(USBD_DEV_CONFIG_DESC_OK != check_result.result)
   {
      IOCMD_ERROR_6(MAIN_APP,"configuration check error:: result: %d, index: %d, if_num: %d, if_alt: %d, ep_num: %d, ep_dir: %s",
         check_result.result, check_result.index,
         check_result.if_num, check_result.if_alt,
         check_result.ep_num, USB_EP_DIRECTION_OUT == check_result.ep_dir ? "OUT" : "IN");
   }

   USBDC_Bind_Config_Desc(&usbdc, config_desc);

   /* Init VCOM 0 */
   Buff_Ring_Init(&buf_0_in,  mem_0_in,  Num_Elems(mem_0_in));
   Buff_Ring_Init(&buf_0_out, mem_0_out, Num_Elems(mem_0_out));

   Buff_Ring_Add_Extension(&buf_0_in,  &buf_0_in_ext);
   Buff_Ring_Add_Extension(&buf_0_out, &buf_0_out_ext);

   CDC_VCOM_Init(&vcom_0, &buf_0_in, &buf_0_out, "UART");
   CDC_VCOM_Install_In_Config(&usbdc, &vcom_0, VCOM_0_NOTIF_IF_NUM);

   /* Init VCOM 1 */
   Buff_Ring_Init(&buf_1_in,  mem_1_in,  Num_Elems(mem_1_in));
   Buff_Ring_Init(&buf_1_out, mem_1_out, Num_Elems(mem_1_out));

   Buff_Ring_Add_Extension(&buf_1_in,  &buf_1_in_ext);
   Buff_Ring_Add_Extension(&buf_1_out, &buf_1_out_ext);

   CDC_VCOM_Init(&vcom_1, &buf_1_in, &buf_1_out, "CMD");
   CDC_VCOM_Install_In_Config(&usbdc, &vcom_1, VCOM_1_NOTIF_IF_NUM);

   /* Init VCOM 2 */
   Buff_Ring_Init(&buf_2, mem_2, Num_Elems(mem_2));

   Buff_Ring_Add_Extension(&buf_2, &buf_2_ext);

   CDC_VCOM_Init(&vcom_2, &buf_2, &buf_2, "ECHO");
   CDC_VCOM_Install_In_Config(&usbdc, &vcom_2, VCOM_2_NOTIF_IF_NUM);

   install_result = USBD_DEV_Install_Config(&usbd, USBD_PORT_STM32_CAT_A, &usbdc);

   if(USBD_DEV_INSTALLATION_OK != install_result.result)
   {
      IOCMD_ERROR_1(MAIN_APP, "configuration install error - result: %d", install_result.result);
   }
   else
   {
      IOCMD_DEBUG_LO_1(MAIN_APP, "configuration install ok - index: %d", install_result.index);
   }

   install_result = USBD_DEV_Install_Manufacturer_String(&usbd, (const uint8_t*)manuf_string, sizeof(manuf_string));

   if(USBD_DEV_INSTALLATION_OK != install_result.result)
   {
      IOCMD_ERROR_1(MAIN_APP, "string install error - result: %d", install_result.result);
   }
   else
   {
      IOCMD_DEBUG_LO_1(MAIN_APP, "string install ok - index: %d", install_result.index);
   }

   install_result = USBD_DEV_Install_Product_String(&usbd, (const uint8_t*)product_string, sizeof(product_string));

   if(USBD_DEV_INSTALLATION_OK != install_result.result)
   {
      IOCMD_ERROR_1(MAIN_APP, "string install error - result: %d", install_result.result);
   }
   else
   {
      IOCMD_DEBUG_LO_1(MAIN_APP, "string install ok - index: %d", install_result.index);
   }

   install_result = USBD_DEV_Install_Serial_Number_String(&usbd, (const uint8_t*)serial_string, sizeof(serial_string));

   if(USBD_DEV_INSTALLATION_OK != install_result.result)
   {
      IOCMD_ERROR_1(MAIN_APP, "string install error - result: %d", install_result.result);
   }
   else
   {
      IOCMD_DEBUG_LO_1(MAIN_APP, "string install ok - index: %d", install_result.index);
   }

   if(USBD_BOOL_IS_FALSE(USBD_DEV_Get_Dev_Desc(&usbd, &dev)))
   {
      IOCMD_ERROR(MAIN_APP, "get device desc error!");
   }

   dev.bcdUSB           = 0x0110;
   dev.bDeviceClass     = 0x02;
   dev.bDeviceSubclass  = 0x02;
   dev.bDeviceProtocol  = 0x01;
   dev.idVendor         = USBD_VENDOR_ID;
   dev.idProduct        = USBD_PRODUCT_ID;
   dev.bcdDevice        = 0x1212;

   if(USBD_BOOL_IS_FALSE(USBD_DEV_Set_Dev_Desc(&usbd, &dev)))
   {
      IOCMD_ERROR(MAIN_APP, "set device desc error!");
   }

   USBD_DEV_Activate(&usbd, USBD_PORT_STM32_CAT_A);

   IOCMD_EXIT_FUNC(MAIN_APP);
}

