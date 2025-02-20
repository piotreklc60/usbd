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

#include "main_usbd_init.h"
#include "stm32_helper.h"
#include "usbd_port_stm32_cat_a.h"
#include "boot_config.h"
#include "flash.h"

#define FLASH_SIZE_REG     ((uint16_t*)0x1FFFF7E0)

uint32_t force_dfu = 0;

static USBD_Params_XT  usbd;
static USBDC_Params_XT usbdc;
/* DFU */
static DFU_Params_XT dfu;
static uint32_t dfu_next_page_addr;
static uint32_t dfu_prog_addr;
static uint16_t dfu_page_size;
static uint8_t dfu_buf[DFU_W_TRANSFER_SIZE];

static uint8_t config_desc_dfu[USB_DESC_TYPE_CONFIGURATION_SIZE + DFU_CONFIG_DESC_PART_SIZE] =
{
   USB_CONFIGURATION_DESC_TABLE_FILL(
      /* _wTotalLength */        USB_DESC_TYPE_CONFIGURATION_SIZE + DFU_CONFIG_DESC_PART_SIZE,
      /* _bNumInterfaces */      1,
      /* _bConfigurationValue */ 1,
      /* _iConfiguration */      0,
      /* _self_powered */        USB_CFG_DESC_SELF_POWERED,
      /* _remote_wakeup */       USB_CFG_DESC_NO_REMOTE_WAKEUP,
      /* _max_current_mA */      25
   ),

   DFU_DFU_MODE_DESC_PART(0, 0)

}; /* config_desc */

static DFU_bStatus_ET main_usbd_download(DFU_Params_XT *dfu, uint8_t **buffer, uint16_t length, uint16_t packet_number)
{
   USBD_LOG_DATA_NOTICE_2(MAIN_APP, *buffer, length, "packet_nr: %d with length %d", packet_number, length);

   USBD_UNUSED_PARAM(dfu);
   USBD_UNUSED_PARAM(packet_number);

#if(BOOT_LED_NOTOFICATION_SUPPORT)
   LED_ON();
#endif

   // get into critical section - disable interrupts
   __asm volatile ("   CPSID   I");

   Program_Flash_Unlock();

   while((dfu_prog_addr + length) > dfu_next_page_addr)
   {
      if (Program_Flash_Erase_Page(dfu_next_page_addr))
      // flash erase failed
      {
         Program_Flash_Lock();
         // leave critical section - enable interrupts
         __asm volatile ("   CPSIE   I");
#if(BOOT_LED_NOTOFICATION_SUPPORT)
         LED_OFF();
#endif
         return(DFU_BSTATUS_ERR_ERASE);
      }

      dfu_next_page_addr += dfu_page_size;
   }

   if (Program_Flash_Write_Page(dfu_prog_addr, dfu_buf, length))
   // flash write failed
   {
      Program_Flash_Lock();
      // leave critical section - enable interrupts
      __asm volatile ("   CPSIE   I");
#if(BOOT_LED_NOTOFICATION_SUPPORT)
      LED_OFF();
#endif
      return(DFU_BSTATUS_ERR_WRITE);
   }

   dfu_prog_addr += length;

   Program_Flash_Lock();

   // leave critical section - enable interrupts
   __asm volatile ("   CPSIE   I");

   *buffer = dfu_buf;

   return DFU_BSTATUS_OK;
}

static void dfu_user_event(DFU_Params_XT *dfu, DFU_Event_ET event)
{
   USBD_UNUSED_PARAM(dfu);

   USBD_ENTER_FUNC(MAIN_APP);

   USBD_NOTICE_1(MAIN_APP, "dfu event: %d", event);

   switch(event)
   {
#if(USBD_FEATURE_PRESENT == DFU_WILL_DETACH_SUPPORT)
      case DFU_USER_EVENT_DETACH_GO_TO_APP:
      case DFU_USER_EVENT_DETACH_GO_TO_DFU:
#else
      case DFU_USER_EVENT_RESET_GO_TO_APP:
      case DFU_USER_EVENT_RESET_GO_TO_DFU:
#endif

//         USBD_DEV_Deactivate(&usbd);

         NVIC_SystemReset();
         break;

      case DFU_USER_EVENT_MANIFEST_IMAGE:
#if(BOOT_LED_NOTOFICATION_SUPPORT)
         LED_OFF();
#endif
         break;

      default:
         break;
   }

   USBD_EXIT_FUNC(MAIN_APP);
} /* dfu_user_event */

void main_usbd_init(void)
{
   USBD_DEV_Installation_Result_XT install_result;
#if(USBD_FEATURE_PRESENT == USBD_DEV_SUPPORT_CONFIG_VALIDATION)
   USBD_DEV_Config_Desc_Check_Result_XT check_result;
#endif
   USB_Device_Desc_DT dev;

   USBD_ENTER_FUNC(MAIN_APP);

#if(USBD_FEATURE_PRESENT == USBD_MULTI_SESSION_SUPPORTED)
   USBD_Init(&usbd);
   USBDC_Init(&usbdc);
#endif

#if(USBD_FEATURE_PRESENT == USBD_DEV_SUPPORT_CONFIG_VALIDATION)
   check_result = USBD_DEV_Check_Config_Desc(&usbd, USBD_PORT_STM32_CAT_A, (uint8_t*)(&config_desc_dfu), sizeof(config_desc_dfu));

   if(USBD_DEV_CONFIG_DESC_OK != check_result.result)
   {
      USBD_ERROR_6(MAIN_APP,"configuration check error:: result: %d, index: %d, if_num: %d, if_alt: %d, ep_num: %d, ep_dir: %s",
         check_result.result, check_result.index,
         check_result.if_num, check_result.if_alt,
         check_result.ep_num, USB_EP_DIRECTION_OUT == check_result.ep_dir ? "OUT" : "IN");
   }
#endif

   USBDC_Bind_Config_Desc(&usbdc, config_desc_dfu);

   USBD_NOTICE_1(MAIN_APP, "init DFU as %s", "DFU");

   /* DFU */
#if(USBD_FEATURE_PRESENT == USBD_MULTI_SESSION_SUPPORTED)
   DFU_Init(&dfu);
#endif
   DFU_SET_USER_EVENT_HANDLER(&dfu, dfu_user_event);
   DFU_SET_PART_DOWNLOAD_READY_HANDLER(&dfu, main_usbd_download);
   DFU_DFU_Install_In_Config(&usbdc, &dfu, dfu_buf);

   install_result = USBD_DEV_Install_Config(&usbd, USBD_PORT_STM32_CAT_A, &usbdc);

   if(USBD_DEV_INSTALLATION_OK != install_result.result)
   {
      USBD_ERROR_1(MAIN_APP, "configuration install error - result: %d", install_result.result);
   }
   else
   {
      USBD_DEBUG_LO_1(MAIN_APP, "configuration install ok - index: %d", install_result.index);
   }

   dev.bcdUSB           = 0x0110;
   dev.bDeviceClass     = USB_CLASS_APPLICATION_SPECIFIC;
   dev.bDeviceSubclass  = USB_SUBCLASS_APP_SPEC_DEVICE_FIRMWARE_UPGRADE;
   dev.bDeviceProtocol  = USB_PROTOCOL_APP_SPEC_DFU_DFU_MODE;
   dev.idVendor         = USBD_VENDOR_ID;
   dev.idProduct        = USBD_PRODUCT_ID;
   dev.bcdDevice        = 0x1212;

   if(USBD_BOOL_IS_FALSE(USBD_DEV_Set_Dev_Desc(&usbd, &dev)))
   {
      USBD_ERROR(MAIN_APP, "set device desc error!");
   }

   if(*FLASH_SIZE_REG >= 256)
   {
      dfu_page_size = 2048;
   }
   else
   {
      dfu_page_size = 1024;
   }
   dfu_next_page_addr = DEF_APP_ADDRESS;
   dfu_prog_addr = DEF_APP_ADDRESS;

   USBD_DEV_Activate(&usbd, USBD_PORT_STM32_CAT_A);

   USBD_EXIT_FUNC(MAIN_APP);
}

