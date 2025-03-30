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
#include "call_bootloader.h"

#define VCOM_0_NOTIF_IF_NUM        0
#define VCOM_0_DATA_IF_NUM         1
#define HID_IF_NUM                 2
#define DFU_APP_IF_NUM             3

static USBD_Params_XT  usbd;
static USBDC_Params_XT usbdc;
/* VCOM 0 */
CDC_VCOM_Params_XT vcom_0;
Buff_Ring_XT buf_0_in;
static Buff_Ring_XT buf_0_out;
static Buff_Ring_Extensions_XT buf_0_in_ext;
static Buff_Ring_Extensions_XT buf_0_out_ext;
static uint8_t mem_0_in[256];
static uint8_t mem_0_out[256];
/* HID - keyboard */
HID_Params_XT hid;
const uint8_t hid_report_descriptor_keyboard[63] =
{
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0x95, 0x05,                    //   REPORT_COUNT (5)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x05, 0x08,                    //   USAGE_PAGE (LEDs)
    0x19, 0x01,                    //   USAGE_MINIMUM (Num Lock)
    0x29, 0x05,                    //   USAGE_MAXIMUM (Kana)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x03,                    //   REPORT_SIZE (3)
    0x91, 0x03,                    //   OUTPUT (Cnst,Var,Abs)
    0x95, 0x06,                    //   REPORT_COUNT (6)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0,                          // END_COLLECTION
};

static HID_Report_XT reports[1];
Keyboard_Report_In_DT keyboard_report_in;
static Keyboard_Report_In_DT keyboard_report_in_second_buf;
Keyboard_Report_Out_DT keyboard_report_out;
static Keyboard_Report_Out_DT keyboard_report_out_second_buf;
/* DFU */
static DFU_Params_XT dfu;

static uint8_t config_desc[] =
{
   USB_CONFIGURATION_DESC_TABLE_FILL(
      /* _wTotalLength */        0,
      /* _bNumInterfaces */      4,
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
      /* _ep_out_mps */          64
   ),

   HID_DESC_PART(
      /* _hid_if_num */          HID_IF_NUM,
      /* _hid_iInterface */      0,
      /* _hid_protocol */        USB_PROTOCOL_HID_BOOTABLE_KEYBOARD,
      /* _ep_in */               3,
      /* _ep_in_mps */           16,
      /* _country_code */        HID_COUNTRY_CODE_NOT_SUPPORTED,
      /* _report_desc_length */  sizeof(hid_report_descriptor_keyboard)
   ),

   DFU_APP_MODE_DESC_PART(
      /* _dfu_if_num */          DFU_APP_IF_NUM,
      /* _dfu_iInterface */      0
   )
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
   0x0312,
   'k',
   'e',
   'y',
   'b',
   'o',
   'a',
   'r',
   'd'
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

static void dfu_user_event(DFU_Params_XT *dfu, DFU_Event_ET event)
{
   USBD_ENTER_FUNC(MAIN_APP);

   USBD_UNUSED_PARAM(dfu);

   USBD_NOTICE_1(MAIN_APP, "dfu event: %d", event);

   switch(event)
   {
#if(USBD_FEATURE_PRESENT == DFU_WILL_DETACH_SUPPORT)
      case DFU_USER_EVENT_DETACH_GO_TO_APP:
      case DFU_USER_EVENT_DETACH_GO_TO_DFU:

         USBD_DEV_Deactivate(&usbd);
#else
      case DFU_USER_EVENT_RESET_GO_TO_APP:
      case DFU_USER_EVENT_RESET_GO_TO_DFU:
#endif
         CALL_BOOTLOADER();
         break;

      default:
         break;
   }

   USBD_EXIT_FUNC(MAIN_APP);
} /* dfu_user_event */

void main_usbd_init(void)
{
   USBD_DEV_Installation_Result_XT install_result;
   USBD_DEV_Config_Desc_Check_Result_XT check_result;
   USB_Device_Desc_DT dev;

   USBD_ENTER_FUNC(MAIN_APP);

   USBD_Init(&usbd);
   USBDC_Init(&usbdc);

   USB_SET_CONFIGURATION_DESC_W_TOTAL_LENGTH(config_desc, sizeof(config_desc));

   check_result = USBD_DEV_Check_Config_Desc(&usbd, USBD_PORT_STM32_CAT_A, (uint8_t*)(&config_desc), sizeof(config_desc));

   if(USBD_DEV_CONFIG_DESC_OK != check_result.result)
   {
      USBD_ERROR_6(MAIN_APP,"configuration check error:: result: %d, index: %d, if_num: %d, if_alt: %d, ep_num: %d, ep_dir: %s",
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

   CDC_VCOM_Init(&vcom_0, &buf_0_in, &buf_0_out, "keyboard");
   CDC_VCOM_Install_In_Config(&usbdc, &vcom_0, VCOM_0_NOTIF_IF_NUM);

   /* HID */
   HID_Reports_Tab_Init(reports, Num_Elems(reports));
   HID_Report_Set_Name(reports, 0, "keyboard");
   memset(&keyboard_report_in,  0, sizeof(keyboard_report_in));
   memset(&keyboard_report_out, 0, sizeof(keyboard_report_out));
   HID_Report_Set_In( reports, 0, &keyboard_report_in,  &keyboard_report_in_second_buf,  sizeof(keyboard_report_in));
   HID_Report_Set_Out(reports, 0, &keyboard_report_out, &keyboard_report_out_second_buf, sizeof(keyboard_report_out));
   HID_Init(&hid, hid_report_descriptor_keyboard, sizeof(hid_report_descriptor_keyboard), reports, Num_Elems(reports));
   HID_Report_Set_In_Event( &hid, 0, Keyboard_In_Report_Done);
   HID_Report_Set_Out_Event(&hid, 0, Keyboard_Out_Report_Done);
   HID_Install_In_Config(&usbdc, &hid, HID_IF_NUM);

   /* DFU */
   DFU_Init(&dfu);
   DFU_SET_USER_EVENT_HANDLER(&dfu, dfu_user_event);
   DFU_APP_Install_In_Config(&usbdc, &dfu, DFU_APP_IF_NUM);

   install_result = USBD_DEV_Install_Config(&usbd, USBD_PORT_STM32_CAT_A, &usbdc);

   if(USBD_DEV_INSTALLATION_OK != install_result.result)
   {
      USBD_ERROR_1(MAIN_APP, "configuration install error - result: %d", install_result.result);
   }
   else
   {
      USBD_DEBUG_LO_1(MAIN_APP, "configuration install ok - index: %d", install_result.index);
   }

   install_result = USBD_DEV_Install_Manufacturer_String(&usbd, (const uint8_t*)manuf_string, sizeof(manuf_string));

   if(USBD_DEV_INSTALLATION_OK != install_result.result)
   {
      USBD_ERROR_1(MAIN_APP, "string install error - result: %d", install_result.result);
   }
   else
   {
      USBD_DEBUG_LO_1(MAIN_APP, "string install ok - index: %d", install_result.index);
   }

   install_result = USBD_DEV_Install_Product_String(&usbd, (const uint8_t*)product_string, sizeof(product_string));

   if(USBD_DEV_INSTALLATION_OK != install_result.result)
   {
      USBD_ERROR_1(MAIN_APP, "string install error - result: %d", install_result.result);
   }
   else
   {
      USBD_DEBUG_LO_1(MAIN_APP, "string install ok - index: %d", install_result.index);
   }

   install_result = USBD_DEV_Install_Serial_Number_String(&usbd, (const uint8_t*)serial_string, sizeof(serial_string));

   if(USBD_DEV_INSTALLATION_OK != install_result.result)
   {
      USBD_ERROR_1(MAIN_APP, "string install error - result: %d", install_result.result);
   }
   else
   {
      USBD_DEBUG_LO_1(MAIN_APP, "string install ok - index: %d", install_result.index);
   }

   if(USBD_BOOL_IS_FALSE(USBD_DEV_Get_Dev_Desc(&usbd, &dev)))
   {
      USBD_ERROR(MAIN_APP, "get device desc error!");
   }

   dev.bcdUSB           = 0x0110;
   dev.bDeviceClass     = USB_CLASS_MISCELLANEOUS;
   dev.bDeviceSubclass  = USB_SUBCLASS_MISC_INTERFACE_ASSOCIACION_DESC;
   dev.bDeviceProtocol  = USB_PROTOCOL_MISC_INTERFACE_ASSOCIACION_DESC;
   dev.idVendor         = USBD_VENDOR_ID;
   dev.idProduct        = USBD_PRODUCT_ID;
   dev.bcdDevice        = 0x1212;

   if(USBD_BOOL_IS_FALSE(USBD_DEV_Set_Dev_Desc(&usbd, &dev)))
   {
      USBD_ERROR(MAIN_APP, "set device desc error!");
   }

   USBD_DEV_Activate(&usbd, USBD_PORT_STM32_CAT_A);

   USBD_EXIT_FUNC(MAIN_APP);
}

