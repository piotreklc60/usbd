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

#ifndef USBD_DEV_C_
#define USBD_DEV_C_
#endif

#ifndef USBD_H_
#include "usbd.h"
#endif

#ifndef USBD_DEV_INTERNAL_H_
#include "usbd_dev_internal.h"
#endif

#ifdef USBD_IO_PRESENT
#ifndef USBD_IO_INTERNAL_H_
#include "usbd_io_internal.h"
#endif
#endif

#ifdef USBD_EVENT_PRESENT
#ifndef USBD_EVENT_INTERNAL_H_
#include "usbd_event_internal.h"
#endif
#endif

#ifdef USBDC_PRESENT
#ifndef USBDC_INTERNAL_H_
#include "usbdc_internal.h"
#endif
#endif

#ifdef USBD_REQUEST_PRESENT
#ifndef USBD_REQUEST_INTERNAL_H_
#include "usbd_request_internal.h"
#endif

#ifdef USBD_IOTP_EVENT_PRESENT
#ifndef USBD_IOTP_EVENT_INTERNAL_H_
#include "usbd_iotp_event_internal.h"
#endif
#endif

#endif

#define USBD_DEV_LANGUAGE_STRING_MANUAL_POSITION            0
#define USBD_DEV_MANUFACTURING_STRING_MANUAL_POSITION       1
#define USBD_DEV_PRODUCT_STRING_MANUAL_POSITION             2
#define USBD_DEV_SERIAL_NUMBER_STRING_MANUAL_POSITION       3

static USBD_Bool_DT USBD_DEV_configure(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_DEV_Set_Configuration_Respond_HT respond);
static void USBD_DEV_unconfigure(USBD_Params_XT *usbd);
static void USBD_DEV_calculate_num_endpoints_in_active_config(USBD_Params_XT *usbd);

static const uint8_t USBD_DEV_language_string_descriptor[] = {
   0x04,               /* bLength */
   USB_DESC_TYPE_STRING,  /* bDescriptorType: string descriptor */
   0x09,               /* Supported language: English - United States */
   0x04
};


void USBD_DEV_Init_Structure(
      USBD_Params_XT *usbd)
{
   USBD_DEV_String_Header_XT* tab;

   USBD_ENTER_FUNC(USBD_DBG_DEV_INIT);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      /* table can be installed only if device is not reset yet */
      if(0 == (usbd->dev.core.data.state & USBD_DEV_STATE_DEFAULT))
      {
         tab = USBD_DEV_GET_STRING_TAB_PTR(usbd);
         usbd->dev.core.data.num_installed_strings = 1;
         tab[0].string = USBD_DEV_language_string_descriptor;
      }
   }
   else
   {
      USBD_WARN_1(USBD_DBG_DEV_INVALID_PARAMS, "function invalid parameters! usbd: %p", usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_INIT);
} /* USBD_DEV_Init_Structure */

#ifdef USBD_USE_IOCMD
static void USBD_DEV_state_change(USBD_Params_XT *usbd, uint8_t new_state, uint16_t line)
{
   uint8_t old_state = usbd->dev.core.data.state;

#ifndef USBD_USE_IOCMD
   USBD_UNUSED_PARAM(line);
#endif

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   usbd->dev.core.data.state = new_state;

   if(old_state != new_state)
   {
#ifdef USBD_USE_IOCMD
#ifdef IOCMD__LINE__LOCAL
#undef IOCMD__LINE__LOCAL
#endif
#define IOCMD__LINE__LOCAL      line
#endif
      USBD_INFO_HI_6(USBD_DBG_DEV_STATE, "DEV state changed before: %s%s%s%s%s%s",
         (0 == old_state) ? "OFF        " : ((0 != (old_state & USBD_DEV_STATE_ATTACHED))   ? "ATTACHED   " : "           "),
         (0 != (old_state & USBD_DEV_STATE_POWERED))    ? "POWERED    " : "           ",
         (0 != (old_state & USBD_DEV_STATE_DEFAULT))    ? "DEFAULT    " : "           ",
         (0 != (old_state & USBD_DEV_STATE_ADDRESSED))  ? "ADDRESSED  " : "           ",
         (0 != (old_state & USBD_DEV_STATE_CONFIGURED)) ? "CONFIGURED " : "           ",
         (0 != (old_state & USBD_DEV_STATE_SUSPENDED))  ? "SUSPENDED  " : "           "
         );
      USBD_INFO_HI_6(USBD_DBG_DEV_STATE, "DEV state changed after : %s%s%s%s%s%s",
         (0 == new_state) ? "OFF        " : ((0 != (new_state & USBD_DEV_STATE_ATTACHED))   ? "ATTACHED   " : "           "),
         (0 != (new_state & USBD_DEV_STATE_POWERED))    ? "POWERED    " : "           ",
         (0 != (new_state & USBD_DEV_STATE_DEFAULT))    ? "DEFAULT    " : "           ",
         (0 != (new_state & USBD_DEV_STATE_ADDRESSED))  ? "ADDRESSED  " : "           ",
         (0 != (new_state & USBD_DEV_STATE_CONFIGURED)) ? "CONFIGURED " : "           ",
         (0 != (new_state & USBD_DEV_STATE_SUSPENDED))  ? "SUSPENDED  " : "           "
         );

#ifdef USBD_USE_IOCMD
#undef IOCMD__LINE__LOCAL
#define IOCMD__LINE__LOCAL      IOCMD__LINE__
#endif
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);
} /* USBD_DEV_state_change */
#else
#define USBD_DEV_state_change(usbd, new_state, line)        usbd->dev.core.data.state = new_state
#endif

static USBD_DEV_Installation_Result_XT USBD_DEV_install_internal_string(
      USBD_Params_XT *usbd,
      const uint8_t *string,
      uint8_t string_size,
      uint8_t iString,
      uint8_t iString_min_value)
{
   USBD_DEV_String_Header_XT* string_tab;
   USBD_DEV_Installation_Result_XT result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_INIT);

   result.result     = USBD_DEV_INSTALLATION_INVALID_PTR;
   result.index      = 0;

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd) && USBD_CHECK_PTR(const uint8_t, string))
   {
      /* any string can be installed only if device is not reset yet */
      if(0 == (usbd->dev.core.data.state & USBD_DEV_STATE_DEFAULT))
      {
         if((string_size == string[0]) && (USB_DESC_TYPE_STRING == string[1]))
         {
            if((iString < USBD_MAX_NUM_STRINGS) && (iString >= iString_min_value))
            {
               string_tab = USBD_DEV_GET_STRING_TAB_PTR(usbd);
               if( (iString > 0) && (!USBD_CHECK_PTR(const uint8_t, string_tab[iString].string)) )
               {
                  (usbd->dev.core.data.num_installed_strings)++;
               }
               string_tab[iString].string = string;

               USBD_DEBUG_HI_1(USBD_DBG_DEV_INIT, "Successfully installed string under index %u", iString);

               result.result  = USBD_DEV_INSTALLATION_OK;
               result.index   = iString;
            }
            else
            {
               result.result  = USBD_DEV_INSTALLATION_INDEX_OUT_OF_RANGE;
            }
         }
         else
         {
            result.result  = USBD_DEV_INSTALLATION_DATA_INVALID_STRUCTURE;
         }
      }
      else
      {
         result.result     = USBD_DEV_INSTALLATION_FORBIDDEN_STATE;
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_DEV_INVALID_PARAMS, "function invalid parameters! usbd: %p, string: %p", usbd, string);
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_INIT);

   return result;
} /* USBD_DEV_install_internal_string */


USBD_DEV_Installation_Result_XT USBD_DEV_Install_String(
      USBD_Params_XT *usbd,
      const uint8_t *string,
      uint8_t string_size,
      uint8_t iString)
{
   USBD_DEV_Installation_Result_XT result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_INIT);

   result = USBD_DEV_install_internal_string(
      usbd, string, string_size, iString, USBD_DEV_SERIAL_NUMBER_STRING_MANUAL_POSITION + 1);

   USBD_EXIT_FUNC(USBD_DBG_DEV_INIT);

   return result;
} /* USBD_DEV_Install_String */

USBD_DEV_Installation_Result_XT USBD_DEV_Install_Language_String(
      USBD_Params_XT *usbd,
      const uint8_t *string,
      uint8_t string_size)
{
   USBD_DEV_Installation_Result_XT result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_INIT);

   result = USBD_DEV_install_internal_string(usbd, string, string_size, 0, 0);

   USBD_EXIT_FUNC(USBD_DBG_DEV_INIT);

   return result;
} /* USBD_DEV_Install_Language_String */

USBD_DEV_Installation_Result_XT USBD_DEV_Install_Manufacturer_String(
      USBD_Params_XT *usbd,
      const uint8_t *string,
      uint8_t string_size)
{
   USBD_DEV_Installation_Result_XT result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_INIT);

   result = USBD_DEV_install_internal_string(usbd, string, string_size, USBD_DEV_MANUFACTURING_STRING_MANUAL_POSITION, 0);

   USBD_EXIT_FUNC(USBD_DBG_DEV_INIT);

   return result;
} /* USBD_DEV_Install_Manufacturer_String */

USBD_DEV_Installation_Result_XT USBD_DEV_Install_Product_String(
      USBD_Params_XT *usbd,
      const uint8_t *string,
      uint8_t string_size)
{
   USBD_DEV_Installation_Result_XT result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_INIT);

   result = USBD_DEV_install_internal_string(usbd, string, string_size, USBD_DEV_PRODUCT_STRING_MANUAL_POSITION, 0);

   USBD_EXIT_FUNC(USBD_DBG_DEV_INIT);

   return result;
} /* USBD_DEV_Install_Product_String */

USBD_DEV_Installation_Result_XT USBD_DEV_Install_Serial_Number_String(
      USBD_Params_XT *usbd,
      const uint8_t *string,
      uint8_t string_size)
{
   USBD_DEV_Installation_Result_XT result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_INIT);

   result = USBD_DEV_install_internal_string(usbd, string, string_size, USBD_DEV_SERIAL_NUMBER_STRING_MANUAL_POSITION, 0);

   USBD_EXIT_FUNC(USBD_DBG_DEV_INIT);

   return result;
} /* USBD_DEV_Install_Serial_Number_String */

USBD_Bool_DT USBD_DEV_Remove_String(
      USBD_Params_XT *usbd,
      const uint8_t *string)
{
   USBD_DEV_String_Header_XT* string_tab;
   uint8_t counter;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_INIT);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd) && USBD_CHECK_PTR(const uint8_t, string))
   {
      /* any string can be removed only if device is not reset yet */
      if(0 == (usbd->dev.core.data.state & USBD_DEV_STATE_DEFAULT))
      {
         if(usbd->dev.core.data.num_installed_strings > 0)
         {
            string_tab = USBD_DEV_GET_STRING_TAB_PTR(usbd);

            for(counter = 1; counter < USBD_MAX_NUM_STRINGS; counter++)
            {
               if(string == string_tab[counter].string)
               {
                  if(usbd->dev.core.data.num_installed_strings > 1)
                  {
                     (usbd->dev.core.data.num_installed_strings)--;
                  }
                  string_tab[counter].string  = USBD_MAKE_INVALID_PTR(const uint8_t);

                  result = USBD_TRUE;
                  break;
               }
            }
         }
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_DEV_INVALID_PARAMS, "function invalid parameters! usbd: %p, string: %p", usbd, string);
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_INIT);

   return result;
} /* USBD_DEV_Remove_String */

USBD_Bool_DT USBD_DEV_Remove_All_Strings(
      USBD_Params_XT *usbd)
{
   USBD_DEV_String_Header_XT* string_tab;
   uint8_t counter;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_INIT);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      /* any string can be removed only if device is not reset yet */
      if(0 == (usbd->dev.core.data.state & USBD_DEV_STATE_DEFAULT))
      {
         usbd->dev.core.data.num_installed_strings = 1;
         string_tab = USBD_DEV_GET_STRING_TAB_PTR(usbd);

         for(counter = 1; counter < USBD_MAX_NUM_STRINGS; counter++)
         {
            string_tab[counter].string  = USBD_MAKE_INVALID_PTR(const uint8_t);
         }

         result = USBD_TRUE;
      }
   }
   else
   {
      USBD_WARN_1(USBD_DBG_DEV_INVALID_PARAMS, "function invalid parameters! usbd: %p", usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_INIT);

   return result;
} /* USBD_DEV_Remove_All_Strings */

uint8_t USBD_DEV_Get_Num_Installed_Strings(
      USBD_Params_XT *usbd)
{
   uint8_t result = 0;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      result = usbd->dev.core.data.num_installed_strings;
   }
   else
   {
      USBD_WARN_1(USBD_DBG_DEV_INVALID_PARAMS, "function invalid parameters! usbd: %p", usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return result;
} /* USBD_DEV_Get_Num_Installed_Strings */

const uint8_t *USBD_DEV_Get_String(
      USBD_Params_XT *usbd,
      uint8_t index)
{
   const uint8_t *result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   result = USBD_MAKE_INVALID_PTR(const uint8_t);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      if(index < usbd->dev.core.data.num_installed_strings)
      {
         result = (USBD_DEV_GET_STRING_TAB_PTR(usbd))[index].string;
      }
   }
   else
   {
      USBD_WARN_1(USBD_DBG_DEV_INVALID_PARAMS, "function invalid parameters! usbd: %p", usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return result;
} /* USBD_DEV_Get_String */

uint8_t USBD_DEV_Find_String_Index_By_String_Ptr(
      USBD_Params_XT *usbd,
      const uint8_t *string)
{
   USBD_DEV_String_Header_XT* string_tab;
   uint8_t result = 0;
   uint8_t counter;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      result = usbd->dev.core.data.num_installed_strings;

      if((USBD_CHECK_PTR(const uint8_t, string)) && (usbd->dev.core.data.num_installed_strings > 0))
      {
         string_tab = USBD_DEV_GET_STRING_TAB_PTR(usbd);

         for(counter = 0; counter < usbd->dev.core.data.num_installed_strings; counter++)
         {
            if(USBD_CHECK_PTR(const uint8_t, string_tab[counter].string))
            {
               if(USBD_COMPARE_PTRS(const uint8_t, string_tab[counter].string, const uint8_t, string))
               {
                  result = counter;
               }
            }
         }
      }
   }
   else
   {
      USBD_WARN_1(USBD_DBG_DEV_INVALID_PARAMS, "function invalid parameters! usbd: %p", usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return result;
} /* USBD_DEV_Find_String_Index_By_String_Ptr */



USBD_Bool_DT USBD_DEV_Get_Dev_Desc(
      USBD_Params_XT *usbd,
      USB_Device_Desc_DT *desc)
{
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd) && USBD_CHECK_PTR(USB_Device_Desc_DT, desc))
   {
      desc->bLength              = USB_DESC_TYPE_DEVICE_SIZE;
      desc->bDescriptorType      = USB_DESC_TYPE_DEVICE;
      desc->bcdUSB               = usbd->dev.core.data.dev_desc.bcdUSB;
      desc->bDeviceClass         = usbd->dev.core.data.dev_desc.bDeviceClass;
      desc->bDeviceSubclass      = usbd->dev.core.data.dev_desc.bDeviceSubclass;
      desc->bDeviceProtocol      = usbd->dev.core.data.dev_desc.bDeviceProtocol;
      desc->bMaxPacketSize0      = usbd->dev.core.data.dev_desc.bMaxPacketSize0;
      desc->idVendor             = usbd->dev.core.data.dev_desc.idVendor;
      desc->idProduct            = usbd->dev.core.data.dev_desc.idProduct;
      desc->bcdDevice            = usbd->dev.core.data.dev_desc.bcdDevice;
      desc->iManufacturer        = usbd->dev.core.data.dev_desc.iManufacturer;
      desc->iProduct             = usbd->dev.core.data.dev_desc.iProduct;
      desc->iSerialNumber        = usbd->dev.core.data.dev_desc.iSerialNumber;
      desc->bNumConfigurations   = usbd->dev.core.data.dev_desc.bNumConfigurations;

      result = USBD_TRUE;
   }
   else
   {
      USBD_WARN_2(USBD_DBG_DEV_INVALID_PARAMS, "function invalid parameters! usbd: %p, desc: %p", usbd, desc);
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return result;
} /* USBD_DEV_Get_Dev_Desc */

USBD_Bool_DT USBD_DEV_Set_Dev_Desc(
      USBD_Params_XT *usbd,
      USB_Device_Desc_DT *desc)
{
   const USBD_DEV_Port_Handler_XT  *port;
   const USB_Endpoint_Desc_DT     *ep0_desc;
   USBD_Bool_DT                     result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_INIT);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd) && USBD_CHECK_PTR(USB_Device_Desc_DT, desc))
   {
      /* dev descriptor can be modified only if device is not reset yet */
      if(0 == (usbd->dev.core.data.state & USBD_DEV_STATE_DEFAULT))
      {

         usbd->dev.core.data.dev_desc.bLength           = USB_DESC_TYPE_DEVICE_SIZE;
         usbd->dev.core.data.dev_desc.bDescriptorType   = USB_DESC_TYPE_DEVICE;
         usbd->dev.core.data.dev_desc.bcdUSB            = desc->bcdUSB;
         usbd->dev.core.data.dev_desc.bDeviceClass      = desc->bDeviceClass;
         usbd->dev.core.data.dev_desc.bDeviceSubclass   = desc->bDeviceSubclass;
         usbd->dev.core.data.dev_desc.bDeviceProtocol   = desc->bDeviceProtocol;
         usbd->dev.core.data.dev_desc.bMaxPacketSize0   = desc->bMaxPacketSize0;
         usbd->dev.core.data.dev_desc.idVendor          = desc->idVendor;
         usbd->dev.core.data.dev_desc.idProduct         = desc->idProduct;
         usbd->dev.core.data.dev_desc.bcdDevice         = desc->bcdDevice;

         if(USBD_CHECK_PORT_PTR(usbd))
         {
            port = USBD_DEV_GET_PORT_PTR(usbd);

            if(USBD_DEV_CHECK_PORT_EP0_LOW_FULL_SPEED_HANDLER(port))
            {
               ep0_desc = USBD_DEV_GET_PORT_EP0_LOW_FULL_SPEED_DESC(usbd, port);

               if(USBD_CHECK_PTR(USB_Endpoint_Desc_DT, ep0_desc))
               {
                  usbd->dev.core.data.dev_desc.bMaxPacketSize0 = ep0_desc->wMaxPacketSize.L;
               }
            }
         }

         result = USBD_TRUE;
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_DEV_INVALID_PARAMS, "function invalid parameters! usbd: %p, desc: %p", usbd, desc);
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_INIT);

   return result;
} /* USBD_DEV_Set_Dev_Desc */



USBD_DEV_Config_Desc_Check_Result_XT USBD_DEV_Check_Config_Desc(
      USBD_Params_XT *usbd,
      const USBD_DEV_Port_Handler_XT *port,
      const uint8_t *desc,
      uint16_t desc_size)
{
   const USB_Configuration_Desc_DT    *config  = (const USB_Configuration_Desc_DT*)desc;
   const USB_Interface_Desc_DT        *if_desc;
   const USB_Endpoint_Desc_DT         *ep_desc;
   uint8_t                             *parsed_ep_type;
   USBD_DEV_Config_Desc_Check_Result_XT result;
   struct
   {
      uint8_t ep_type[2];
   }parsed_eps[USBD_MAX_NUM_ENDPOINTS];
   uint16_t checked_size = 0;
   uint16_t pos = 0;
   uint16_t pos_next = 0;
   uint8_t  if_num;
   uint8_t  ep_num;
   uint8_t  ep_dir;
   uint8_t  ep_type;
   uint8_t  ep_interface[USBD_MAX_NUM_ENDPOINTS];
   USBD_Bool_DT failure;
   USBD_Bool_DT ep_both_directions_must_have_same_type = USBD_FALSE;

   USBD_ENTER_FUNC(USBD_DBG_DEV_INIT);

   result.result  = USBD_DEV_CONFIG_DESC_INVALID_PARAMS;
   result.index   = 0xFFFF;
   result.if_num  = 0xFF;
   result.if_alt  = 0xFF;
   result.ep_num  = 0xFF;
   result.ep_dir  = USB_EP_DIRECTION_OUT;
   if_desc        = USBD_MAKE_INVALID_PTR(const USB_Interface_Desc_DT);
   ep_desc        = USBD_MAKE_INVALID_PTR(const USB_Endpoint_Desc_DT);
   failure        = USBD_FALSE;

   if(/*USBD_CHECK_PTR(USBD_Params_XT, usbd) && USBD is not needed here so can be NULL */ USBD_CHECK_PTR(uint8_t, desc))
   {
      failure        = USBD_FALSE;
      result.result  = USBD_DEV_CONFIG_DESC_OK;

      USBD_DEBUG_MID_1(USBD_DBG_DEV_INIT, "analyzing configuration descriptor with declared total size: %d", desc_size);

      /* check if first two bytes of descriptor are incorrect */
      if((USB_DESC_TYPE_CONFIGURATION_SIZE != desc[0])
         && (USB_DESC_TYPE_CONFIGURATION == desc[1]))
      {
         failure        = USBD_TRUE;
         result.result  = USBD_DEV_CONFIG_DESC_INVALID_CONFIG_HEADER;

         USBD_WARN_2(USBD_DBG_DEV_INIT, "invalid desc header! size: %d; type: %d", desc[0], desc[1]);
      }
      else if(config->bNumInterfaces > USBD_MAX_NUM_INTERFACES)
      {
         failure        = USBD_TRUE;
         result.result  = USBD_DEV_CONFIG_DESC_INVALID_NUM_INTERFACES;
         result.index   = 4;

         USBD_WARN_1(USBD_DBG_DEV_INIT, "invalid number of interfaces: %d!", config->bNumInterfaces);
      }
      else
      {
         checked_size += desc[3];
         checked_size *= 256;
         checked_size += desc[2];

         USBD_DEBUG_MID_1(USBD_DBG_DEV_INIT, "total size read from desc: %d", checked_size);

         /* check if descriptor size was incorrectly set */
         if(desc_size != checked_size)
         {
            failure        = USBD_TRUE;
            result.result  = USBD_DEV_CONFIG_DESC_INVALID_TOTAL_LENGTH;
            result.index   = 2;

            USBD_WARN_2(USBD_DBG_DEV_INIT, "invalid desc total size!: declared: %d; read from desc: %d",
               desc_size, checked_size);
         }
         else
         {
            USBD_DEBUG_MID_5(USBD_DBG_DEV_INIT,
               "config:: bNumInterfaces: %d; bConfigurationValue: %d; iConfiguration: %d; bmAttributes: %d; bMaxPower: %d",
               config->bNumInterfaces,
               config->bConfigurationValue,
               config->iConfiguration,
               config->bmAttributes,
               config->bMaxPower);

            for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
            {
               ep_interface[ep_num] = config->bNumInterfaces;
            }

            USBD_DEBUG_MID_1(USBD_DBG_DEV_INIT,
               "checking availability of %d interface descriptors with bAlternateSetting = 0...", config->bNumInterfaces);
            /*
             * find all interfaces mentioned in config descriptor -
             * there must be at least config->bNumInterface interfaces with bAlternateSetting = 0 with valid headers
             */
            for(if_num = 0; if_num < config->bNumInterfaces; if_num++)
            {
               pos = USB_CDP_Find_Interface(desc, desc_size, 0, if_num, 0);

               /* "pos" must be in range between 0 and desc_size, otherwise descriptor was not found */
               if(!((0 != pos) && (pos < desc_size)))
               {
                  failure        = USBD_TRUE;
                  result.result  = USBD_DEV_CONFIG_DESC_INVALID_INTERFACE_NOT_FOUND;
                  result.if_num  = if_num;
                  result.if_alt  = 0;

                  USBD_WARN_1(USBD_DBG_DEV_INIT, "IF %d.0 not found!", if_num);

                  break;
               }
               /* check if first two bytes of descriptor are incorrect */
               else if((USB_DESC_TYPE_INTERFACE_SIZE != desc[pos])
                  || (USB_DESC_TYPE_INTERFACE != desc[pos+1]))
               {
                  failure        = USBD_TRUE;
                  result.result  = USBD_DEV_CONFIG_DESC_INVALID_INTERFACE_HEADER;
                  result.index   = pos;
                  result.if_num  = if_num;
                  result.if_alt  = 0;

                  USBD_WARN_4(USBD_DBG_DEV_INIT, "IF %d.0 on byte: %d invalid header! size: %d; type: %d",
                     if_num, pos, desc[pos], desc[pos+1]);

                  break;
               }
               else
               {
                  USBD_DEBUG_MID_2(USBD_DBG_DEV_INIT, "-> IF %d.0 on byte: %d", if_num, pos);
               }
            }

            if(USBD_CHECK_PTR(const USBD_DEV_Port_Handler_XT, port))
            {
               ep_both_directions_must_have_same_type = port->data.ep_both_directions_must_have_same_type;
            }

            /* check all endpoints - all of them must have valid headers and numbers */
            if(USBD_BOOL_IS_FALSE(failure))
            {
               USBD_DEBUG_MID(USBD_DBG_DEV_INIT, "parsing all available endpoint descriptors...");

               memset(parsed_eps, 0xFF, sizeof(parsed_eps));

               for(pos = 0; pos < desc_size;)
               {
                  pos = USB_CDP_Find_Next_Endpoint(desc, desc_size, pos);

                  /* check if endpoint was found */
                  if((0 != pos) && (pos < desc_size))
                  {
                     ep_num  = desc[pos+2] & USB_EP_DESC_ADDR_MASK;
                     ep_dir  = (desc[pos+2] & USB_EP_DESC_DIR_MASK);
                     ep_type = desc[pos+3] & USB_EP_DESC_TRANSFER_TYPE_MASK;

                     /* check if first two bytes of descriptor are incorrect */
                     if((USB_DESC_TYPE_ENDPOINT_SIZE != desc[pos])
                        || (USB_DESC_TYPE_ENDPOINT != desc[pos+1]))
                     {
                        failure        = USBD_TRUE;
                        result.result  = USBD_DEV_CONFIG_DESC_INVALID_ENDPOINT_HEADER;
                        result.index   = pos;

                        USBD_WARN_4(USBD_DBG_DEV_INIT, "EP %d on byte: %d invalid header! size: %d; type: %d",
                           ep_num, pos, desc[pos], desc[pos+1]);

                        break;
                     }
                     /* check if endpoint address is bigger than maximum number of endpoints or desc for EP0 was found */
                     else if((ep_num >= USBD_MAX_NUM_ENDPOINTS) || (0 == ep_num))
                     {
                        failure        = USBD_TRUE;
                        result.result  = USBD_DEV_CONFIG_DESC_INVALID_ENDPOINT_NUM;
                        result.index   = pos;
                        result.ep_num  = ep_num;
                        if(USB_EP_DESC_DIR_OUT != ep_dir)
                        {
                           result.ep_dir = USB_EP_DIRECTION_IN;
                        }
                        else
                        {
                           result.ep_dir = USB_EP_DIRECTION_OUT;
                        }

                        USBD_WARN_2(USBD_DBG_DEV_INIT, "EP %d on byte: %d invalid EP number!", ep_num, pos);

                        break;
                     }

                     /* change 'ep_dir' from 0x80 to 1 to use for indexing table */
                     if(USB_EP_DESC_DIR_OUT != ep_dir)
                     {
                        ep_dir = 1;
                     }

                     parsed_ep_type = parsed_eps[ep_num].ep_type;

                     /* if current ep_type is not control and cell of parsed EPs still contains FF then set there curent EP type */
                     if(USB_EP_DESC_TRANSFER_TYPE_CONTROL != ep_type)
                     {
                        if(0xFF == parsed_ep_type[ep_dir])
                        {
                           parsed_ep_type[ep_dir] = ep_type;
                        }
                        else if((parsed_ep_type[ep_dir] != ep_type) ||
                           /* if port is not able to handle different EP types for IN/OUT of same endpoint then check it */
                           (
                              USBD_BOOL_IS_TRUE(ep_both_directions_must_have_same_type)
                              && ((parsed_ep_type[0] != ep_type) || (parsed_ep_type[1] != ep_type))
                           )
                        )
                        {
                           failure        = USBD_TRUE;
                           result.result  = USBD_DEV_CONFIG_DESC_INVALID_ENDPOINT_TYPE_MISMATCH;
                           result.index   = pos;
                           result.ep_num  = ep_num;
                           if(USB_EP_DESC_DIR_OUT != ep_dir)
                           {
                              result.ep_dir = USB_EP_DIRECTION_IN;
                           }
                           else
                           {
                              result.ep_dir = USB_EP_DIRECTION_OUT;
                           }

                           USBD_WARN_2(USBD_DBG_DEV_INIT, "EP %d on byte: %d EP duplicated with different type!", ep_num, pos);

                           break;
                        }
                     }
                     /* for control EP, both - IN and OUT parsed locations should be reported as same type or FF */
                     else if((0xFF == parsed_ep_type[ep_dir]) && (0xFF == parsed_ep_type[ep_dir]))
                     {
                        parsed_ep_type[0] = ep_type;
                        parsed_ep_type[1] = ep_type;
                     }
                     else if((parsed_ep_type[0] != ep_type) || (parsed_ep_type[1] != ep_type))
                     {
                        failure        = USBD_TRUE;
                        result.result  = USBD_DEV_CONFIG_DESC_INVALID_ENDPOINT_TYPE_MISMATCH;
                        result.index   = pos;
                        result.ep_num  = ep_num;
                        if(USB_EP_DESC_DIR_OUT != ep_dir)
                        {
                           result.ep_dir = USB_EP_DIRECTION_IN;
                        }
                        else
                        {
                           result.ep_dir = USB_EP_DIRECTION_OUT;
                        }

                        USBD_WARN_2(USBD_DBG_DEV_INIT, "EP %d on byte: %d EP duplicated with different type!", ep_num, pos);

                        break;
                     }

                     USBD_DEBUG_MID_2(USBD_DBG_DEV_INIT, "-> EP %d on byte: %d", ep_num, pos);
                  }
               }
            }

            /* parse all interfaces & related endpoints */
            if(USBD_BOOL_IS_FALSE(failure))
            {
               USBD_DEBUG_MID(USBD_DBG_DEV_INIT, "parsing depedencies between interfaces and endpoints descriptors...");

               /* parse all interfaces - there must be at least config->bNumInterface interfaces */
               for(pos = 0; pos < desc_size;)
               {
                  pos = USB_CDP_Find_Next_Interface(desc, desc_size, pos);

                  /* "pos" must be in range between 0 and desc_size, otherwise descriptor was not found */
                  if((0 != pos) && (pos < desc_size))
                  {
                     /*
                      * find next interface descriptor to find area of config descriptor
                      * between two interfaces where we will search endpoints
                      */
                     pos_next = USB_CDP_Find_Next_Interface(desc, desc_size, pos);
                     /* if next interface doesn't exist then set pos_next to end of whole config descriptor */
                     if((0 == pos_next) || (pos_next >= desc_size))
                     {
                        pos_next = desc_size;
                     }

                     /* check if first two bytes of descriptor are incorrect */
                     if((USB_DESC_TYPE_INTERFACE_SIZE != desc[pos])
                        || (USB_DESC_TYPE_INTERFACE != desc[pos+1]))
                     {
                        failure      = USBD_TRUE;
                        result.result = USBD_DEV_CONFIG_DESC_INVALID_INTERFACE_HEADER;
                        result.index  = pos;

                        USBD_WARN_3(USBD_DBG_DEV_INIT, "IF on byte: %d invalid header! size: %d; type: %d",
                           pos, desc[pos], desc[pos+1]);

                        break;
                     }
                     else
                     {
                        if_desc = (USB_Interface_Desc_DT*)(&desc[pos]);

                        USBD_DEBUG_MID_11(USBD_DBG_DEV_INIT,
                           "-> IF %d.%d on byte: %d; bNumEndpoints: %d; "
                           "bInterfaceClass: 0x%02X(%d); bInterfaceSubClass: 0x%02X(%d); "
                           "bInterfaceProtocol: 0x%02X(%d); iInterface: %d",
                           if_desc->bInterfaceNumber,
                           if_desc->bAlternateSetting,
                           pos,
                           if_desc->bNumEndpoints,
                           if_desc->bInterfaceClass,
                           if_desc->bInterfaceClass,
                           if_desc->bInterfaceSubClass,
                           if_desc->bInterfaceSubClass,
                           if_desc->bInterfaceProtocol,
                           if_desc->bInterfaceProtocol,
                           if_desc->iInterface);

                        /* check if interface number is bigger than number of interfaces in configuration */
                        if(if_desc->bInterfaceNumber >= config->bNumInterfaces)
                        {
                           failure        = USBD_TRUE;
                           result.result  = USBD_DEV_CONFIG_DESC_INVALID_INTERFACE_NUM;
                           result.index   = pos;
                           result.if_num  = if_desc->bInterfaceNumber;
                           result.if_alt  = if_desc->bAlternateSetting;

                           USBD_WARN(USBD_DBG_DEV_INIT, "IF number out of range!");

                           break;
                        }

                        /* parse all endpoints - there must at least if_desc->bNumEndpoints endpoints */
                        for(ep_num = 0; ep_num < if_desc->bNumEndpoints; ep_num++)
                        {
                           pos = USB_CDP_Find_Next_Endpoint(desc, desc_size, pos);

                           /* "pos" must be in range between 0 and desc_size, otherwise descriptor was not found */
                           if((0 != pos) && (pos < pos_next))
                           {
                              ep_desc = (const USB_Endpoint_Desc_DT*)(&desc[pos]);
                              /* check if first two bytes of descriptor are incorrect */
                              if((USB_DESC_TYPE_ENDPOINT_SIZE != ep_desc->bLength)
                                 || (USB_DESC_TYPE_ENDPOINT != ep_desc->bDescriptorType))
                              {
                                 failure        = USBD_TRUE;
                                 result.result  = USBD_DEV_CONFIG_DESC_INVALID_ENDPOINT_HEADER;
                                 result.index   = pos;
                                 result.if_num  = if_desc->bInterfaceNumber;
                                 result.if_alt  = if_desc->bAlternateSetting;

                                 USBD_WARN_4(USBD_DBG_DEV_INIT, "EP %d on byte: %d invalid header! size: %d; type: %d",
                                    ep_num, pos, ep_desc->bLength, ep_desc->bDescriptorType);

                                 break;
                              }
                              else
                              {
                                 USBD_DEBUG_MID_9(USBD_DBG_DEV_INIT,
                                    "   -> EP %d on byte: %d; bEndpointAddress: %02X; bmAttributes: %02X; "
                                    "wMaxPacketSize: 0x%02X%02X(%d); bInterval: 0x%02X(%d)",
                                    ep_desc->bEndpointAddress & USB_EP_DESC_ADDR_MASK,
                                    pos,
                                    ep_desc->bEndpointAddress,
                                    ep_desc->bmAttributes,
                                    ep_desc->wMaxPacketSize.H,
                                    ep_desc->wMaxPacketSize.L,
                                    (ep_desc->wMaxPacketSize.H * 256) + ep_desc->wMaxPacketSize.L,
                                    ep_desc->bInterval,
                                    ep_desc->bInterval);

                                 /* check if endpoint belongs to more than one interface */
                                 if((ep_interface[ep_desc->bEndpointAddress & USB_EP_DESC_ADDR_MASK] >=
                                       config->bNumInterfaces)
                                    || (ep_interface[ep_desc->bEndpointAddress & USB_EP_DESC_ADDR_MASK] ==
                                       if_desc->bInterfaceNumber))
                                 {
                                    ep_interface[ep_desc->bEndpointAddress & USB_EP_DESC_ADDR_MASK] =
                                       if_desc->bInterfaceNumber;

                                    if(USBD_CHECK_PTR(const USBD_DEV_Port_Handler_XT, port))
                                    {
                                       if(USBD_DEV_CHECK_PORT_PARSE_EP_HANDLER(port))
                                       {
                                          /* if port wants to parse each endpoint separately then call "PARSE_EP" */
                                          if(USBD_BOOL_IS_FALSE(USBD_DEV_CALL_PORT_PARSE_EP_HANDLER(
                                             usbd,
                                             port,
                                             ep_desc->bEndpointAddress & USB_EP_DESC_ADDR_MASK,
                                             if_desc,
                                             ep_desc)))
                                          {
                                             failure        = USBD_TRUE;
                                             result.result  = USBD_DEV_CONFIG_DESC_INVALID_ENDPOINT_PARAMS_FOR_HW;
                                             result.index   = pos;
                                             result.if_num  = if_desc->bInterfaceNumber;
                                             result.if_alt  = if_desc->bAlternateSetting;
                                             result.ep_num  = ep_desc->bEndpointAddress & USB_EP_DESC_ADDR_MASK;
                                             if(USB_EP_DESC_DIR_OUT !=
                                                (ep_desc->bEndpointAddress & USB_EP_DESC_DIR_MASK))
                                             {
                                                result.ep_dir = USB_EP_DIRECTION_IN;
                                             }
                                             else
                                             {
                                                result.ep_dir = USB_EP_DIRECTION_OUT;
                                             }

                                             USBD_WARN_1(USBD_DBG_DEV_INIT, "EP: %d parsing failed by port!",
                                                result.ep_num);
                                          }
                                       }
                                    }
                                 }
                                 else
                                 {
                                    failure        = USBD_TRUE;
                                    result.result  = USBD_DEV_CONFIG_DESC_INVALID_ENDPOINT_OWNER;
                                    result.index   = pos;
                                    result.if_num  = if_desc->bInterfaceNumber;
                                    result.if_alt  = if_desc->bAlternateSetting;
                                    result.ep_num  = ep_desc->bEndpointAddress & USB_EP_DESC_ADDR_MASK;
                                    if(USB_EP_DESC_DIR_OUT !=
                                       (ep_desc->bEndpointAddress & USB_EP_DESC_DIR_MASK))
                                    {
                                       result.ep_dir = USB_EP_DIRECTION_IN;
                                    }
                                    else
                                    {
                                       result.ep_dir = USB_EP_DIRECTION_OUT;
                                    }

                                    USBD_WARN_2(USBD_DBG_DEV_INIT, "EP: %d belongs already to interface %d!",
                                       result.ep_num,
                                       ep_interface[ep_desc->bEndpointAddress & USB_EP_DESC_ADDR_MASK]);
                                 }
                              }
                           }
                           /* endpoint was not found */
                           else
                           {
                              failure        = USBD_TRUE;
                              result.result  = USBD_DEV_CONFIG_DESC_INVALID_ENDPOINT_NOT_FOUND;
                              result.if_num  = if_desc->bInterfaceNumber;
                              result.if_alt  = if_desc->bAlternateSetting;
                              result.ep_num  = ep_num;

                              USBD_WARN_4(USBD_DBG_DEV_INIT, "missing EP %d from required %d needed by IF %d.%d",
                                 ep_num,
                                 if_desc->bNumEndpoints,
                                 if_desc->bInterfaceNumber,
                                 if_desc->bAlternateSetting);

                              break;
                           }
                        }
                        if(USBD_BOOL_IS_TRUE(failure))
                        {
                           break;
                        }
                     }
                  }
                  else
                  {
                     break;
                  }
               }
            }

            /* if port wants to parse whole config then call "PARSE_CFG" */
            if(USBD_BOOL_IS_FALSE(failure) && USBD_CHECK_PTR(const USBD_DEV_Port_Handler_XT, port))
            {
               if(USBD_DEV_CHECK_PORT_PARSE_CFG_HANDLER(port))
               {
                  if(USBD_BOOL_IS_FALSE(USBD_DEV_CALL_PORT_PARSE_CFG_HANDLER(
                     usbd,
                     port,
                     desc,
                     desc_size,
                     &result)))
                  {
                     failure      = USBD_TRUE;

                     USBD_WARN(USBD_DBG_DEV_INIT, "CFG parsing failed by port!");
                  }
               }
            }
         }
      }

      if(USBD_DEV_CONFIG_DESC_OK == result.result)
      {
         USBD_DEBUG_MID(USBD_DBG_DEV_INIT, "parsing configuration descriptor succeeded");
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_DEV_INVALID_PARAMS, "function invalid parameters! usbd: %p, desc: %p", usbd, desc);
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_INIT);

   return result;
} /* USBD_DEV_Check_Config_Desc */

USBD_DEV_Installation_Result_XT USBD_DEV_Install_Config(
      USBD_Params_XT *usbd,
      const USBD_DEV_Port_Handler_XT *port,
      USBDC_Params_XT *usbdc)
{
   const USB_Configuration_Desc_DT *config;
   USBD_DEV_Config_Header_XT *config_tab;
   const uint8_t *config_desc;
   USBD_DEV_Installation_Result_XT result;
   uint16_t config_desc_size;
   uint8_t config_index;

   USBD_ENTER_FUNC(USBD_DBG_DEV_INIT);

   result.result  = USBD_DEV_INSTALLATION_INVALID_PTR;
   result.index   = 0;

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd) && USBD_CHECK_PTR(USBDC_Params_XT, usbdc))
   {
      config_desc = USBDC_Get_Config_Desc(usbdc);

      if(USBD_CHECK_PTR(const uint8_t, config_desc))
      {
         /* any configuration can be installed only if device is not reset yet */
         if(0 == (usbd->dev.core.data.state & USBD_DEV_STATE_DEFAULT))
         {
            result.result = USBD_DEV_INSTALLATION_DATA_INVALID_STRUCTURE;

            config_desc_size = USB_CDP_Get_Config_Desc_Total_Length(config_desc);

            if(config_desc_size >= USB_DESC_TYPE_CONFIGURATION_SIZE)
            {
               config  = (const USB_Configuration_Desc_DT*)config_desc;
               config_index = config->bConfigurationValue;
               if(config_index > 0)
               {
                  config_index--;
               }

               if(config_index < USBD_MAX_NUM_CONFIGURATIONS)
               {
                  result.result = USBD_DEV_INSTALLATION_DATA_INVALID_STRUCTURE;

                  if(USBD_DEV_CONFIG_DESC_OK == USBD_DEV_Check_Config_Desc(usbd, port, config_desc, config_desc_size).result)
                  {
                     result.result     = USBD_DEV_INSTALLATION_OK;
                     result.index      = config_index;

                     config_tab = USBD_DEV_GET_CONFIG_TAB_PTR(usbd);

                     if(!USBD_CHECK_PTR(USBDC_Params_XT, config_tab[config_index].usbdc))
                     {
                        (usbd->dev.core.data.dev_desc.bNumConfigurations)++;
                     }

                     config_tab[config_index].usbdc         = usbdc;
                     config_tab[config_index].config_desc   = config_desc;

                     USBD_DEBUG_HI_1(USBD_DBG_DEV_INIT, "Successfully installed configuration under index %u", config_index);

#ifdef USBDC_PRESENT
                     USBDC_Bind_Usbd(usbdc, usbd);
                     USBDC_Bind_Config_Desc(usbdc, config_desc);
#endif
                  }
               }
               else
               {
                  result.result = USBD_DEV_INSTALLATION_INDEX_OUT_OF_RANGE;
               }
            }
         }
         else
         {
            result.result = USBD_DEV_INSTALLATION_FORBIDDEN_STATE;
         }
      }
      else
      {
         USBD_WARN(USBD_DBG_DEV_INVALID_PARAMS, "configuration descriptor not installed in usbdc structure!");
         result.result = USBD_DEV_INSTALLATION_INVALID_PARAMS;
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_DEV_INVALID_PARAMS, "function invalid parameters! usbd: %p, usbdc: %p", usbd, usbdc);
      result.result = USBD_DEV_INSTALLATION_INVALID_PARAMS;
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_INIT);

   return result;
} /* USBD_DEV_Install_Config */

USBD_Bool_DT USBD_DEV_Remove_Config(
      USBD_Params_XT *usbd,
      USBDC_Params_XT *usbdc)
{
   USBD_DEV_Config_Header_XT *config_tab;
   uint8_t counter;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_INIT);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd) && USBD_CHECK_PTR(USBDC_Params_XT, usbdc))
   {
      /* any configuration can be removed only if device is not reset yet */
      if(0 == (usbd->dev.core.data.state & USBD_DEV_STATE_DEFAULT))
      {
         if(usbd->dev.core.data.dev_desc.bNumConfigurations > 0)
         {
            config_tab = USBD_DEV_GET_CONFIG_TAB_PTR(usbd);

            for(counter = 0; counter < USBD_MAX_NUM_CONFIGURATIONS; counter++)
            {
               if(USBD_COMPARE_PTRS(USBDC_Params_XT, usbdc, USBDC_Params_XT, config_tab[counter].usbdc))
               {
                  (usbd->dev.core.data.dev_desc.bNumConfigurations)--;
                  config_tab[counter].usbdc        = USBD_MAKE_INVALID_PTR(USBDC_Params_XT);
                  config_tab[counter].config_desc  = USBD_MAKE_INVALID_PTR(const uint8_t);

                  result = USBD_TRUE;
                  break;
               }
            }
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_INIT);

   return result;
} /* USBD_DEV_Remove_Config */

USBD_Bool_DT USBD_DEV_Remove_All_Configs(
      USBD_Params_XT *usbd)
{
   USBD_DEV_Config_Header_XT *config_tab;
   uint8_t counter;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_INIT);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      /* any configuration can be removed only if device is not reset yet */
      if(0 == (usbd->dev.core.data.state & USBD_DEV_STATE_DEFAULT))
      {
         usbd->dev.core.data.dev_desc.bNumConfigurations = 0;
         config_tab = USBD_DEV_GET_CONFIG_TAB_PTR(usbd);

         for(counter = 0; counter < USBD_MAX_NUM_CONFIGURATIONS; counter++)
         {
            config_tab[counter].usbdc        = USBD_MAKE_INVALID_PTR(USBDC_Params_XT);
            config_tab[counter].config_desc  = USBD_MAKE_INVALID_PTR(const uint8_t);
         }

         result = USBD_TRUE;
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_INIT);

   return result;
} /* USBD_DEV_Remove_All_Configs */

uint8_t USBD_DEV_Get_Num_Installed_Configs(
      USBD_Params_XT *usbd)
{
   uint8_t result = 0;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      result = usbd->dev.core.data.dev_desc.bNumConfigurations;
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return result;
} /* USBD_DEV_Get_Num_Installed_Configs */

const USB_Configuration_Desc_DT *USBD_DEV_Get_Config_Desc(
      USBD_Params_XT *usbd,
      uint8_t index)
{
   const USB_Configuration_Desc_DT *result;
   USBD_DEV_Config_Header_XT *config_tab;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   result = USBD_MAKE_INVALID_PTR(const USB_Configuration_Desc_DT);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      if(index < USBD_MAX_NUM_CONFIGURATIONS)
      {
         config_tab = USBD_DEV_GET_CONFIG_TAB_PTR(usbd);

         result = (USB_Configuration_Desc_DT*)(config_tab[index].config_desc);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return result;
} /* USBD_DEV_Get_Config_Desc */

USBDC_Params_XT *USBD_DEV_Get_Config(
      USBD_Params_XT *usbd,
      uint8_t index)
{
   USBDC_Params_XT *result;
   USBD_DEV_Config_Header_XT *config_tab;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   result = USBD_MAKE_INVALID_PTR(USBDC_Params_XT);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      if(index < USBD_MAX_NUM_CONFIGURATIONS)
      {
         config_tab = USBD_DEV_GET_CONFIG_TAB_PTR(usbd);

         result = config_tab[index].usbdc;
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return result;
} /* USBD_DEV_Get_Config */

uint8_t USBD_DEV_Get_Active_Config_Num(
      USBD_Params_XT *usbd)
{
   uint8_t result = 0;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      result = USBD_DEV_GET_ACTIVE_CONFIG_NUM(usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return result;
} /* USBD_DEV_Get_Active_Config_Num */

uint8_t USBD_DEV_Get_Max_Num_Interfaces_In_Installed_Configs(
      USBD_Params_XT *usbd)
{
   USBD_DEV_Config_Header_XT *config_tab;
   uint8_t result = 0;
   uint8_t counter;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      config_tab = USBD_DEV_GET_CONFIG_TAB_PTR(usbd);

      for(counter = 0; counter < usbd->dev.core.data.dev_desc.bNumConfigurations; counter++)
      {
         if(USBD_CHECK_PTR(uint8_t, config_tab[counter].config_desc))
         {
            if(((USB_Configuration_Desc_DT*)(config_tab[counter].config_desc))->bNumInterfaces > result)
            {
               result = ((USB_Configuration_Desc_DT*)(config_tab[counter].config_desc))->bNumInterfaces;
            }
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return result;
} /* USBD_DEV_Get_Max_Num_Interfaces_In_Installed_Configs */

uint8_t USBD_DEV_Get_Num_Interfaces_In_Active_Config(
      USBD_Params_XT *usbd)
{
   uint8_t result = 0;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      if(USBD_DEV_CHECK_ACTIVE_CONFIG_PTR(usbd) && USBD_DEV_CHECK_ACTIVE_CONFIG_DESC_PTR(usbd))
      {
         result =
            ((const USB_Configuration_Desc_DT*)(USBD_DEV_GET_ACTIVE_CONFIG_DESC_PTR(usbd)))->bNumInterfaces;
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return result;
} /* USBD_DEV_Get_Num_Interfaces_In_Active_Config */

static void USBD_DEV_calculate_num_endpoints_in_active_config(USBD_Params_XT *usbd)
{
   const uint8_t *desc;
   uint16_t pos;
   uint16_t desc_size = 0;
   uint8_t ep_tab[USBD_MAX_NUM_ENDPOINTS];
   uint8_t result = 1;
   uint8_t ep_num;

   USBD_ENTER_FUNC(USBD_DBG_DEV_ONOFF);

   if(USBD_DEV_CHECK_ACTIVE_CONFIG_PTR(usbd) && USBD_DEV_CHECK_ACTIVE_CONFIG_DESC_PTR(usbd))
   {
      desc = USBD_DEV_GET_ACTIVE_CONFIG_DESC_PTR(usbd);

      memset(ep_tab, 0, sizeof(ep_tab));

      desc_size += desc[3];
      desc_size *= 256;
      desc_size += desc[2];

      for(pos = 0; pos < desc_size;)
      {
         pos = USB_CDP_Find_Next_Endpoint(desc, desc_size, pos);

         /* if endpoint was found */
         if((0 != pos) && (pos < desc_size))
         {
            ep_num = desc[pos + 2] & 0x0F;
            if(ep_num < USBD_MAX_NUM_ENDPOINTS)
            {
               ep_tab[ep_num] = 1;
            }
         }
      }

      for(ep_num = 1; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
      {
         if(0 != ep_tab[ep_num])
         {
            result++;
         }
      }
   }

   usbd->dev.core.data.num_used_endpoints = result;

   USBD_EXIT_FUNC(USBD_DBG_DEV_ONOFF);
} /* USBD_DEV_calculate_num_endpoints_in_active_config */

uint8_t USBD_DEV_Get_Num_Endpoints_In_Active_Config(
      USBD_Params_XT *usbd)
{
   uint8_t result = 1;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      result = usbd->dev.core.data.num_used_endpoints;
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return result;
} /* USBD_DEV_Get_Num_Endpoints_In_Active_Config */



const USB_Interface_Desc_DT *USBD_DEV_Get_Interface_Desc(
      USBD_Params_XT *usbd,
      uint8_t interface_num)
{
   const USB_Interface_Desc_DT *result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   result = USBD_MAKE_INVALID_PTR(const USB_Interface_Desc_DT);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      if(interface_num < USBD_MAX_NUM_INTERFACES)
      {
         result = (USBD_DEV_GET_INTERFACE_TAB_PTR(usbd))[interface_num].desc;
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return result;
} /* USBD_DEV_Get_Interface_Desc */



USBD_DEV_EP_States_ET USBD_DEV_Get_EP_State(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir)
{
   USBD_DEV_EP_States_ET result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   result = USBD_DEV_EP_OFF;

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      result = (USB_EP_DIRECTION_OUT == dir) ?
         USBD_DEV_GET_EP_OUT_STATE(usbd, ep_num) : USBD_DEV_GET_EP_IN_STATE(usbd, ep_num);
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return result;
} /* USBD_DEV_Get_EP_State */

const USB_Endpoint_Desc_DT *USBD_DEV_Get_EP_Desc(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir)
{
   const USB_Endpoint_Desc_DT *result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   result = USBD_MAKE_INVALID_PTR(const USB_Endpoint_Desc_DT);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      result = (USB_EP_DIRECTION_OUT == dir) ?
         USBD_DEV_GET_EP_OUT_DESC_PTR(usbd, ep_num) : USBD_DEV_GET_EP_IN_DESC_PTR(usbd, ep_num);
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return result;
} /* USBD_DEV_Get_EP_Desc */

uint8_t USBD_DEV_Get_EP_Interface_Num(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir)
{
   uint8_t interface_num = 0xFF;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   if((USBD_CHECK_PTR(USBD_Params_XT, usbd)) && (ep_num < USBD_MAX_NUM_ENDPOINTS))
   {
      interface_num = (USB_EP_DIRECTION_OUT == dir) ?
         USBD_DEV_GET_EP_OUT_INTERFACE(usbd, ep_num) : USBD_DEV_GET_EP_IN_INTERFACE(usbd, ep_num);
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return interface_num;
} /* USBD_DEV_Get_EP_Interface_Num */

void USBD_DEV_Set_EP_Halt(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_Bool_DT state)
{
   USBD_ENTER_FUNC(USBD_DBG_DEV_ONOFF);

   if((USBD_CHECK_PTR(USBD_Params_XT, usbd)) && (ep_num < USBD_MAX_NUM_ENDPOINTS))
   {
      USBD_DEBUG_HI_4(USBD_DBG_DEV_ONOFF, "EP: %d.%s:%s; set new state: %s",
         ep_num,
         (USB_EP_DIRECTION_OUT == dir) ? "OUT" : "IN",
         (USB_EP_DIRECTION_OUT == dir) ?
         (
            (USBD_DEV_EP_OFF == USBD_DEV_GET_EP_OUT_STATE(usbd, ep_num)) ? "OFF" :
               ((USBD_DEV_EP_HALT == USBD_DEV_GET_EP_OUT_STATE(usbd, ep_num)) ? "HALT" : "ON")
         ) :
         (
            (USBD_DEV_EP_OFF == USBD_DEV_GET_EP_IN_STATE(usbd, ep_num)) ? "OFF" :
               ((USBD_DEV_EP_HALT == USBD_DEV_GET_EP_IN_STATE(usbd, ep_num)) ? "HALT" : "ON")
         ),
         USBD_BOOL_IS_TRUE(state) ? "HALT" : "ON");

      if(USBD_BOOL_IS_FALSE(state))
      {
         if(USB_EP_DIRECTION_OUT == dir)
         {
            if(USBD_DEV_EP_HALT == USBD_DEV_GET_EP_OUT_STATE(usbd, ep_num))
            {
#ifdef USBD_IO_PRESENT
               USBD_IO_Perform_Halt(usbd, ep_num, dir, state);
#endif
               USBD_DEV_SET_EP_OUT_STATE(usbd, ep_num, USBD_DEV_EP_ON);
            }
         }
         else
         {
            if(USBD_DEV_EP_HALT == USBD_DEV_GET_EP_IN_STATE(usbd, ep_num))
            {
#ifdef USBD_IO_PRESENT
               USBD_IO_Perform_Halt(usbd, ep_num, dir, state);
#endif
               USBD_DEV_SET_EP_IN_STATE(usbd, ep_num, USBD_DEV_EP_ON);
            }
         }
      }
      else
      {
         if(USB_EP_DIRECTION_OUT == dir)
         {
            if(USBD_DEV_EP_ON == USBD_DEV_GET_EP_OUT_STATE(usbd, ep_num))
            {
#ifdef USBD_IO_PRESENT
               USBD_IO_Perform_Halt(usbd, ep_num, dir, state);
#endif
               USBD_DEV_SET_EP_OUT_STATE(usbd, ep_num, USBD_DEV_EP_HALT);
            }
         }
         else
         {
            if(USBD_DEV_EP_ON == USBD_DEV_GET_EP_IN_STATE(usbd, ep_num))
            {
#ifdef USBD_IO_PRESENT
               USBD_IO_Perform_Halt(usbd, ep_num, dir, state);
#endif
               USBD_DEV_SET_EP_IN_STATE(usbd, ep_num, USBD_DEV_EP_HALT);
            }
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_ONOFF);
} /* USBD_DEV_Set_EP_Halt */

USBD_Bool_DT USBD_DEV_Get_EP_Halt(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir)
{
   USBD_DEV_EP_States_ET state;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   result = USBD_FALSE;

   if((USBD_CHECK_PTR(USBD_Params_XT, usbd)) && (ep_num < USBD_MAX_NUM_ENDPOINTS))
   {
      state = (USB_EP_DIRECTION_OUT == dir) ?
         USBD_DEV_GET_EP_OUT_STATE(usbd, ep_num) : USBD_DEV_GET_EP_IN_STATE(usbd, ep_num);

      result = (USBD_DEV_EP_HALT == state) ? USBD_TRUE : USBD_FALSE;
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return result;
} /* USBD_DEV_Get_EP_Halt */



static void USBD_DEV_reset_dev_and_disable_endpoints(
   USBD_Params_XT *usbd, uint8_t num_first_endpoints_protected, USBD_Bool_DT force_hw_disabling)
{
   uint8_t num;

   USBD_ENTER_FUNC(USBD_DBG_DEV_ONOFF);

   /** deconfigre all endpoints except endpoint 0 */
   for(num = num_first_endpoints_protected; num < USBD_MAX_NUM_ENDPOINTS; num++)
   {
      if((USBD_DEV_EP_OFF != USBD_DEV_GET_EP_OUT_STATE(usbd, num)) || USBD_BOOL_IS_TRUE(force_hw_disabling))
      {
         USBD_DEV_SET_EP_OUT_STATE(usbd, num, USBD_DEV_EP_OFF);
#ifdef USBD_IO_PRESENT
         USBD_IO_EP_Disable(usbd, num, USB_EP_DIRECTION_OUT, USBD_TRUE, force_hw_disabling);
#endif
      }
      USBD_DEV_SET_EP_OUT_DESC_PTR(usbd, num, USBD_MAKE_INVALID_PTR(const USB_Endpoint_Desc_DT));
      USBD_DEV_SET_EP_OUT_INTERFACE(usbd, num, 0xFF);

      if((USBD_DEV_EP_OFF != USBD_DEV_GET_EP_IN_STATE(usbd, num)) || USBD_BOOL_IS_TRUE(force_hw_disabling))
      {
         USBD_DEV_SET_EP_IN_STATE(usbd, num, USBD_DEV_EP_OFF);
#ifdef USBD_IO_PRESENT
         USBD_IO_EP_Disable(usbd, num, USB_EP_DIRECTION_IN, USBD_TRUE, force_hw_disabling);
#endif
      }
      USBD_DEV_SET_EP_IN_DESC_PTR(usbd, num, USBD_MAKE_INVALID_PTR(const USB_Endpoint_Desc_DT));
      USBD_DEV_SET_EP_IN_INTERFACE(usbd, num, 0xFF);
   }

   /** remove pointers to all interfaces */
   for(num = 0; num < USBD_MAX_NUM_INTERFACES; num++)
   {
      (USBD_DEV_GET_INTERFACE_TAB_PTR(usbd))[num].desc = USBD_MAKE_INVALID_PTR(USB_Interface_Desc_DT);
   }

   USBD_DEV_SET_ACTIVE_CONFIG_NUM(usbd, 0);
   USBD_DEV_SET_ACTIVE_CONFIG_PTR(usbd, USBD_MAKE_INVALID_PTR(USBDC_Params_XT));
   USBD_DEV_SET_ACTIVE_CONFIG_DESC_PTR(usbd, USBD_MAKE_INVALID_PTR(const uint8_t));

   usbd->dev.core.data.num_used_endpoints = num_first_endpoints_protected;

   USBD_EXIT_FUNC(USBD_DBG_DEV_ONOFF);
} /* USBD_DEV_reset_dev_and_disable_endpoints */

USBD_Bool_DT USBD_DEV_Activate(
      USBD_Params_XT *usbd,
      const USBD_DEV_Port_Handler_XT *port)
{
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_ONOFF);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd) && USBD_CHECK_PTR(const USBD_DEV_Port_Handler_XT, port))
   {
      if(USBD_BOOL_IS_FALSE(usbd->dev.core.data.active))
      {
         USBD_DEV_SET_PORT_PTR(usbd, port);

         if(USBD_DEV_CHECK_PORT_ACTIVATE_HANDLER(port))
         {
            USBD_DEV_state_change(usbd, USBD_DEV_STATE_OFF, __LINE__);

            USBD_DEV_reset_dev_and_disable_endpoints(usbd, 0, USBD_FALSE);

            usbd->dev.core.data.active = USBD_TRUE;
            USBD_DEV_CALL_PORT_ACTIVATE_HANDLER(usbd, port, USBD_TRUE);
            result = USBD_TRUE;
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_ONOFF);

   return result;
} /* USBD_DEV_Activate */

USBD_Bool_DT USBD_DEV_Deactivate(
      USBD_Params_XT *usbd)
{
   const USBD_DEV_Port_Handler_XT *port;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_ONOFF);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
#ifdef USBD_EVENT_PRESENT
      /**
      * check if any configuration is currently active. In this situation
      * previously used configuration must be turned off at first
      */
      USBD_DEV_unconfigure(usbd);
#endif
      port = USBD_DEV_GET_PORT_PTR(usbd);
      if(USBD_CHECK_PTR(USBD_DEV_Port_Handler_XT, port))
      {
         if(USBD_DEV_CHECK_PORT_ACTIVATE_HANDLER(port))
         {
            USBD_DEV_CALL_PORT_ACTIVATE_HANDLER(usbd, port, USBD_FALSE);

            if(USBD_BOOL_IS_TRUE(usbd->dev.core.data.active))
            {
#ifdef USBD_EVENT_PRESENT
               USBD_EVENT_Process_Cold_Event(usbd, USBD_EVENT_REASON_DETACHED);
#endif
            }

            USBD_DEV_reset_dev_and_disable_endpoints(usbd, 0, USBD_FALSE);

            USBD_DEV_state_change(usbd, USBD_DEV_STATE_OFF, __LINE__);
            usbd->dev.core.data.active = USBD_FALSE;

            result = USBD_TRUE;
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_ONOFF);

   return result;
} /* USBD_DEV_Deactivate */



USBD_DEV_States_ET USBD_DEV_Get_State(
      USBD_Params_XT *usbd)
{
   USBD_DEV_States_ET result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   result = USBD_DEV_STATE_OFF;

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      result = usbd->dev.core.data.state;
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return result;
} /* USBD_DEV_Get_State */



uint16_t USBD_DEV_Get_Frame_Num(
      USBD_Params_XT *usbd)
{
   uint16_t result = 0;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      if(USBD_CHECK_PORT_PTR(usbd))
      {
         if(USBD_DEV_CHECK_PORT_FRAME_NUM_HANDLER(USBD_DEV_GET_PORT_PTR(usbd)))
         {
            result = USBD_DEV_GET_PORT_FRAME_NUM(usbd, USBD_DEV_GET_PORT_PTR(usbd));
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return result;
} /* USBD_DEV_Get_Frame_Num */



USBD_DEV_Speed_ET USBD_DEV_Get_Supported_Speed(
      USBD_Params_XT *usbd)
{
   const USBD_DEV_Port_Handler_XT *port;
   USBD_DEV_Speed_ET result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   result = USBD_DEV_SPEED_INVALID;

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      if(USBD_CHECK_PORT_PTR(usbd))
      {
         port = USBD_DEV_GET_PORT_PTR(usbd);

         if(USBD_DEV_CHECK_PORT_SUPPORTED_SPEED_HANDLER(port))
         {
            result = USBD_DEV_GET_PORT_SUPPORTED_SPEED(usbd, port) & USBD_DEV_SPEED_MASK;
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return result;
} /* USBD_DEV_Get_Supported_Speed */

USBD_DEV_Speed_ET USBD_DEV_Get_Current_Speed(
      USBD_Params_XT *usbd)
{
   const USBD_DEV_Port_Handler_XT *port;
   USBD_DEV_Speed_ET result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_STATE);

   result = USBD_DEV_SPEED_INVALID;

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      if(USBD_CHECK_PORT_PTR(usbd))
      {
         port = USBD_DEV_GET_PORT_PTR(usbd);

         if(USBD_DEV_CHECK_PORT_CURRENT_SPEED_HANDLER(port))
         {
            result = USBD_DEV_GET_PORT_CURRENT_SPEED(usbd, port) & USBD_DEV_SPEED_MASK;
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_STATE);

   return result;
} /* USBD_DEV_Get_Current_Speed */



void USBD_DEV_Attached(
      USBD_Params_XT *usbd,
      USBD_Bool_DT state)
{
   USBD_ENTER_FUNC(USBD_DBG_DEV_ONOFF);

   USBD_NOTICE_1(USBD_DBG_DEV_ONOFF, "DEV %s", USBD_BOOL_IS_TRUE(state) ? "attached" : "detached");

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      usbd->dev.core.data.num_used_endpoints = 0;

      if(USBD_BOOL_IS_TRUE(state))
      {
         USBD_DEV_reset_dev_and_disable_endpoints(usbd, 0, USBD_FALSE);

#ifdef USBD_EVENT_PRESENT
         USBD_EVENT_Process_Cold_Event(usbd, USBD_EVENT_REASON_ATTACHED);
#endif
         USBD_DEV_state_change(usbd, USBD_DEV_STATE_ATTACHED, __LINE__);
      }
      else
      {
#ifdef USBD_EVENT_PRESENT
         /**
         * check if any configuration is currently active. In this situation
         * previously used configuration must be turned off at first
         */
         USBD_DEV_unconfigure(usbd);
         USBD_EVENT_Process_Cold_Event(usbd, USBD_EVENT_REASON_DETACHED);
#endif
         USBD_DEV_reset_dev_and_disable_endpoints(usbd, 0, USBD_FALSE);

         USBD_DEV_state_change(usbd, USBD_DEV_STATE_OFF, __LINE__);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_ONOFF);
} /* USBD_DEV_Attached */

void USBD_DEV_Powered(
      USBD_Params_XT *usbd,
      USBD_Bool_DT state)
{
   USBD_ENTER_FUNC(USBD_DBG_DEV_ONOFF);

   USBD_NOTICE_1(USBD_DBG_DEV_ONOFF, "DEV %s", USBD_BOOL_IS_TRUE(state) ? "powered" : "unpowered");

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      if(USBD_BOOL_IS_TRUE(state))
      {
         USBD_DEV_reset_dev_and_disable_endpoints(usbd, 0, USBD_TRUE);

#ifdef USBD_EVENT_PRESENT
         USBD_EVENT_Process_Cold_Event(usbd, USBD_EVENT_REASON_POWERED);
#endif
         USBD_DEV_state_change(usbd, USBD_DEV_STATE_POWERED | USBD_DEV_STATE_ATTACHED, __LINE__);
      }
      else
      {
#ifdef USBD_EVENT_PRESENT
         /**
         * check if any configuration is currently active. In this situation
         * previously used configuration must be turned off at first
         */
         USBD_DEV_unconfigure(usbd);
         USBD_EVENT_Process_Cold_Event(usbd, USBD_EVENT_REASON_UNPOWERED);
#endif
         USBD_DEV_reset_dev_and_disable_endpoints(usbd, 0, USBD_TRUE);

         USBD_DEV_state_change(usbd, USBD_DEV_STATE_ATTACHED, __LINE__);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_ONOFF);
} /* USBD_DEV_Powered */

void USBD_DEV_Reset(
      USBD_Params_XT *usbd)
{
   const USBD_DEV_Port_Handler_XT *port;
   const USB_Endpoint_Desc_DT *ep0 = USBD_MAKE_INVALID_PTR(const USB_Endpoint_Desc_DT);
   USBD_DEV_String_Header_XT *string_tab;

   USBD_ENTER_FUNC(USBD_DBG_DEV_ONOFF);

   USBD_NOTICE(USBD_DBG_DEV_ONOFF, "DEV reset");

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      USBD_DEV_reset_dev_and_disable_endpoints(usbd, 0, USBD_TRUE);

      usbd->dev.core.data.num_used_endpoints = 1;

      /// TODO: check if dev->core.data.dev_desc.bNumConfigurations > 0 - if not, then deactivate and maybe call USBD_panic
      if(usbd->dev.core.data.dev_desc.bNumConfigurations > 0)
      {
#ifdef USBD_EVENT_PRESENT
         /**
         * check if any configuration is currently active. In this situation
         * previously used configuration must be turned off at first
         */
         USBD_DEV_unconfigure(usbd);

         USBD_DEV_state_change(usbd, USBD_DEV_STATE_POWERED | USBD_DEV_STATE_ATTACHED, __LINE__);
         USBD_EVENT_Process_Cold_Event(usbd, USBD_EVENT_REASON_RESET);
#endif

         USBD_DEV_state_change(usbd, USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED | USBD_DEV_STATE_ATTACHED, __LINE__);
         string_tab = USBD_DEV_GET_STRING_TAB_PTR(usbd);

         usbd->dev.core.data.dev_desc.iManufacturer = 0;
         usbd->dev.core.data.dev_desc.iProduct      = 0;
         usbd->dev.core.data.dev_desc.iSerialNumber = 0;

#if(USBD_MAX_NUM_STRINGS > USBD_DEV_MANUFACTURING_STRING_MANUAL_POSITION)
         if(USBD_CHECK_PTR(const uint8_t, string_tab[USBD_DEV_MANUFACTURING_STRING_MANUAL_POSITION].string))
         {
            usbd->dev.core.data.dev_desc.iManufacturer  = USBD_DEV_MANUFACTURING_STRING_MANUAL_POSITION;
         }
#endif
#if(USBD_MAX_NUM_STRINGS > USBD_DEV_PRODUCT_STRING_MANUAL_POSITION)
         if(USBD_CHECK_PTR(const uint8_t, string_tab[USBD_DEV_PRODUCT_STRING_MANUAL_POSITION].string))
         {
            usbd->dev.core.data.dev_desc.iProduct       = USBD_DEV_PRODUCT_STRING_MANUAL_POSITION;
         }
#endif
#if(USBD_MAX_NUM_STRINGS > USBD_DEV_SERIAL_NUMBER_STRING_MANUAL_POSITION)
         if(USBD_CHECK_PTR(const uint8_t, string_tab[USBD_DEV_SERIAL_NUMBER_STRING_MANUAL_POSITION].string))
         {
            usbd->dev.core.data.dev_desc.iSerialNumber  = USBD_DEV_SERIAL_NUMBER_STRING_MANUAL_POSITION;
         }
#endif

#ifdef USBD_REQUEST_PRESENT
         USBD_IO_IN_UNLOCK_TP(usbd, 0);
         USBD_IO_OUT_UNLOCK_TP(usbd, 0);
         (void)USBD_IOTP_EVENT_Install_Default_Control_Pipe(
            usbd,
            USBD_REQUEST_GET_EP0_IN_IOTP(usbd),
            USBD_REQUEST_GET_EP0_OUT_IOTP(usbd));
#endif

         if(USBD_CHECK_PORT_PTR(usbd))
         {
            port = USBD_DEV_GET_PORT_PTR(usbd);

            if(USBD_DEV_CHECK_PORT_EP0_LOW_FULL_SPEED_HANDLER(port))
            {
               ep0 = USBD_DEV_GET_PORT_EP0_LOW_FULL_SPEED_DESC(usbd, port);
            }

            if(USBD_DEV_CHECK_PORT_CURRENT_SPEED_HANDLER(port))
            {
               if(USBD_DEV_HIGH_SPEED == (USBD_DEV_HIGH_SPEED & USBD_DEV_GET_PORT_CURRENT_SPEED(usbd, port)))
               {
                  /*
                    * if high speed EP0 descriptor pointer is invalid
                    * then full speed EP0 descriptor will be used
                    * (full speed EP0 parameters will be used for high speed transactions)
                    */
                  if(USBD_DEV_CHECK_PORT_EP0_HIGH_SPEED_HANDLER(port))
                  {
                     ep0 = USBD_DEV_GET_PORT_EP0_HIGH_SPEED_DESC(usbd, port);
                  }
               }
            }

            if(USBD_CHECK_PTR(const USB_Endpoint_Desc_DT, ep0))
            {
               USBD_DEV_SET_EP_OUT_DESC_PTR(usbd, 0, ep0);
               USBD_DEV_SET_EP_IN_DESC_PTR(usbd, 0, ep0);
               usbd->dev.core.data.dev_desc.bMaxPacketSize0 = ep0->wMaxPacketSize.L;
#ifdef USBD_IO_PRESENT
               USBD_IO_EP_Enable_And_Configure(usbd, 0, USB_EP_DIRECTION_OUT);
#endif
               USBD_DEV_SET_EP_OUT_STATE(usbd, 0, USBD_DEV_EP_ON);

#ifdef USBD_IO_PRESENT
               USBD_IO_EP_Enable_And_Configure(usbd, 0, USB_EP_DIRECTION_IN);
#endif
               USBD_DEV_SET_EP_IN_STATE(usbd, 0, USBD_DEV_EP_ON);
            }
            else
            {
               USBD_ERROR(USBD_DBG_DEV_ONOFF, "no EP 0 descriptor provided by port! deactivate!");

               (void)USBD_DEV_Deactivate(usbd);
               /// TODO: USBD_panic
            }
         }
         else
         {
            USBD_ERROR(USBD_DBG_DEV_ONOFF, "no port handler present! deactivate!");

            (void)USBD_DEV_Deactivate(usbd);
            /// TODO: USBD_panic
         }

         usbd->dev.core.data.dev_desc.bLength           = USB_DESC_TYPE_DEVICE_SIZE;
         usbd->dev.core.data.dev_desc.bDescriptorType   = USB_DESC_TYPE_DEVICE;
      }
      else
      {
         USBD_ERROR(USBD_DBG_DEV_ONOFF, "no configuration has been installed! deactivate!");

         (void)USBD_DEV_Deactivate(usbd);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_ONOFF);
} /* USBD_DEV_Reset */

void USBD_DEV_Addressed(
      USBD_Params_XT *usbd,
      uint8_t address)
{
#ifndef USBD_USE_IOCMD
   USBD_UNUSED_PARAM(address);
#endif

   USBD_ENTER_FUNC(USBD_DBG_DEV_ONOFF);

   USBD_NOTICE_1(USBD_DBG_DEV_ONOFF, "DEV addressed; addr: %d", address);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
#ifdef USBD_EVENT_PRESENT
      USBD_EVENT_Process_Cold_Event(usbd, USBD_EVENT_REASON_ADDRESSED);
#endif

      usbd->dev.core.data.num_used_endpoints = 1;

      USBD_DEV_state_change(
         usbd, USBD_DEV_STATE_ADDRESSED | USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED | USBD_DEV_STATE_ATTACHED, __LINE__);
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_ONOFF);
} /* USBD_DEV_Addressed */

static USBD_Bool_DT USBD_DEV_configure(USBD_Params_XT *usbd, USBDC_Params_XT *usbdc, USBD_DEV_Set_Configuration_Respond_HT respond)
{
   const USBD_DEV_Port_Handler_XT *port;
   USB_Interface_Desc_DT *if_desc;
   const uint8_t *cfg_desc;
   uint16_t desc_size = 0;
   uint16_t pos;
   uint16_t if_pos1;
   uint16_t if_pos2;
   uint8_t ep_cntr;
   uint8_t ep_num;
   uint8_t if_num;
   USBD_Bool_DT result;

   USBD_UNUSED_PARAM(usbdc);

   USBD_ENTER_FUNC(USBD_DBG_DEV_ONOFF);

   cfg_desc = USBD_DEV_GET_ACTIVE_CONFIG_DESC_PTR(usbd);
   result = USBD_TRUE;

   desc_size += cfg_desc[3];
   desc_size *= 256;
   desc_size += cfg_desc[2];
   /** 1. parse configuration descriptor by core and port */
   if(USBD_CHECK_PORT_PTR(usbd))
   {
      port = USBD_DEV_GET_PORT_PTR(usbd);

      if(USBD_DEV_CONFIG_DESC_OK != USBD_DEV_Check_Config_Desc(usbd, port, cfg_desc, desc_size).result)
      {
         result = USBD_FALSE;
      }
   }

   if(USBD_BOOL_IS_TRUE(result))
   {
      if(USBD_CHECK_HANDLER(USBD_DEV_Set_Configuration_Respond_HT, respond))
      {
         USBD_CALL_HANDLER(USBD_DEV_Set_Configuration_Respond_HT, respond)(usbd);
      }
#ifdef USBD_EVENT_PRESENT
      USBD_EVENT_Process_Warm_Event(usbd, USBD_EVENT_REASON_CONFIGURED);
#endif

      /** 2. enumerate all interfaces */
      for(if_num = 0; if_num < cfg_desc[4]; if_num++)
      {
         if_pos1 = USB_CDP_Find_Interface(cfg_desc, desc_size, 0, if_num, 0);

         if((0 != if_pos1) && (if_pos1 < desc_size))
         {
            if_pos2 = USB_CDP_Find_Next_Interface(cfg_desc, desc_size, if_pos1);
            if(0 == if_pos2)
            {
               if_pos2 = desc_size;
            }

            if_desc = (USB_Interface_Desc_DT*)(&cfg_desc[if_pos1]);

            if(if_num < USBD_MAX_NUM_INTERFACES)
            {
               (USBD_DEV_GET_INTERFACE_TAB_PTR((usbd)))[if_num].desc = if_desc;
            }

            pos = if_pos1;
            for(ep_cntr = 0; ep_cntr < cfg_desc[if_pos1 + 4]; ep_cntr++)
            {
               pos = USB_CDP_Find_Next_Endpoint(cfg_desc, desc_size, pos);

               if((0 != pos) && (pos < if_pos2))
               {
                  ep_num = cfg_desc[pos + 2] & USB_EP_DESC_ADDR_MASK;

                  if(USB_EP_DESC_DIR_OUT == (cfg_desc[pos + 2] & USB_EP_DESC_DIR_MASK))
                  {
                     USBD_DEV_SET_EP_OUT_INTERFACE(usbd, ep_num, if_num);
                     USBD_DEV_SET_EP_OUT_DESC_PTR(usbd, ep_num, (const USB_Endpoint_Desc_DT*)(&cfg_desc[pos]));
#ifdef USBD_IO_PRESENT
                     USBD_IO_EP_Enable_And_Configure(usbd, ep_num, USB_EP_DIRECTION_OUT);
#endif
                     USBD_DEV_SET_EP_OUT_STATE(usbd, ep_num, USBD_DEV_EP_ON);
                  }
                  else
                  {
                     USBD_DEV_SET_EP_IN_INTERFACE(usbd, ep_num, if_num);
                     USBD_DEV_SET_EP_IN_DESC_PTR(usbd, ep_num, (const USB_Endpoint_Desc_DT*)(&cfg_desc[pos]));
#ifdef USBD_IO_PRESENT
                     USBD_IO_EP_Enable_And_Configure(usbd, ep_num, USB_EP_DIRECTION_IN);
#endif
                     USBD_DEV_SET_EP_IN_STATE(usbd, ep_num, USBD_DEV_EP_ON);
                  }
               }
               else
               {
                  break;
               }
            }
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_ONOFF);

   return result;
} /* USBD_DEV_configure */

static void USBD_DEV_unconfigure(USBD_Params_XT *usbd)
{
#ifdef USBD_EVENT_PRESENT
   if(USBD_DEV_CHECK_ACTIVE_CONFIG_PTR(usbd) && (0 != (usbd->dev.core.data.state & USBD_DEV_STATE_CONFIGURED)))
   {
      USBD_EVENT_Process_Warm_Event(usbd, USBD_EVENT_REASON_UNCONFIGURED);
   }
#endif

   USBD_DEV_state_change(usbd, usbd->dev.core.data.state & ~USBD_DEV_STATE_CONFIGURED, __LINE__);

   USBD_DEV_SET_ACTIVE_CONFIG_NUM(usbd, 0);
   USBD_DEV_SET_ACTIVE_CONFIG_PTR(usbd, USBD_MAKE_INVALID_PTR(USBDC_Params_XT));
   USBD_DEV_SET_ACTIVE_CONFIG_DESC_PTR(usbd, USBD_MAKE_INVALID_PTR(const uint8_t));
} /* USBD_DEV_unconfigure */

USBD_Bool_DT USBD_DEV_Configured(
      USBD_Params_XT *usbd,
      uint8_t config_num,
      USBD_DEV_Set_Configuration_Respond_HT respond)
{
   USBD_DEV_Config_Header_XT *config_tab;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_DEV_ONOFF);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      if(config_num <= usbd->dev.core.data.dev_desc.bNumConfigurations)
      {
         /**
         * check if any configuration is currently active. In this situation
         * previously used configuration must be turned off at first
         */
         USBD_DEV_unconfigure(usbd);
         USBD_DEV_reset_dev_and_disable_endpoints(usbd, 1, USBD_FALSE);

         /** check if config value is correct */
         if(0 != config_num)
         {
            config_tab = USBD_DEV_GET_CONFIG_TAB_PTR(usbd);

            config_num -= 1;

            USBD_DEV_state_change(
               usbd,
               USBD_DEV_STATE_ADDRESSED | USBD_DEV_STATE_DEFAULT | USBD_DEV_STATE_POWERED | USBD_DEV_STATE_ATTACHED,
               __LINE__);

            /** set new configuration */
            USBD_DEV_SET_ACTIVE_CONFIG_NUM(usbd, config_num);
            USBD_DEV_SET_ACTIVE_CONFIG_PTR(usbd, config_tab[config_num].usbdc);
            USBD_DEV_SET_ACTIVE_CONFIG_DESC_PTR(usbd, (const uint8_t*)(config_tab[config_num].config_desc));

            USBD_DEV_calculate_num_endpoints_in_active_config(usbd);

            if(USBD_DEV_CHECK_ACTIVE_CONFIG_PTR(usbd) && USBD_DEV_CHECK_ACTIVE_CONFIG_DESC_PTR(usbd))
            {
               result = USBD_DEV_configure(usbd, USBD_DEV_GET_ACTIVE_CONFIG_PTR(usbd), respond);

               if(USBD_BOOL_IS_TRUE(result))
               {
                  USBD_NOTICE_1(USBD_DBG_DEV_ONOFF, "DEV %sconfigured", "");
                  USBD_DEV_state_change(
                     usbd,
                     USBD_DEV_STATE_CONFIGURED
                        | USBD_DEV_STATE_ADDRESSED
                        | USBD_DEV_STATE_DEFAULT
                        | USBD_DEV_STATE_POWERED
                        | USBD_DEV_STATE_ATTACHED,
                     __LINE__);
               }
               else
               {
                  USBD_DEV_SET_ACTIVE_CONFIG_NUM(usbd, 0);
                  USBD_DEV_SET_ACTIVE_CONFIG_PTR(usbd, USBD_MAKE_INVALID_PTR(USBDC_Params_XT));
                  USBD_DEV_SET_ACTIVE_CONFIG_DESC_PTR(usbd, USBD_MAKE_INVALID_PTR(const uint8_t));

                  usbd->dev.core.data.num_used_endpoints = 1;
               }
            }
         }
         else
         {
            if(USBD_CHECK_HANDLER(USBD_DEV_Set_Configuration_Respond_HT, respond))
            {
               USBD_CALL_HANDLER(USBD_DEV_Set_Configuration_Respond_HT, respond)(usbd);
            }
            USBD_NOTICE_1(USBD_DBG_DEV_ONOFF, "DEV %sconfigured", "un");
            result = USBD_TRUE;
         }
      }
      else
      {
         USBD_WARN_2(USBD_DBG_DEV_ONOFF, "Config %u out of range! bNumConfigurations = %u",
            config_num, usbd->dev.core.data.dev_desc.bNumConfigurations);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_ONOFF);

   return result;
} /* USBD_DEV_Configured */

USBD_DEV_Set_Interface_Result_ET USBD_DEV_Set_Interface(USBD_Params_XT *usbd, uint8_t interface_num, uint8_t alternative_setting)
{
   USB_Interface_Desc_DT *if_desc;
   const uint8_t *cfg_desc;
   USBD_DEV_Set_Interface_Result_ET result;
   uint16_t desc_size = 0;
   uint16_t pos;
   uint16_t new_if_pos;
   uint16_t old_if_pos;
   uint16_t if_pos2;
   uint8_t ep_cntr;
   uint8_t ep_num;

   USBD_ENTER_FUNC(USBD_DBG_DEV_ONOFF);

   USBD_INFO_HI_2(USBD_DBG_DEV_ONOFF, "if: %d; alt: %d", interface_num, alternative_setting);

   result = USBD_DEV_SET_INTERFACE_RESULT_ALT_SETTING_NOT_SUPPORTED;

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      if(USBD_DEV_CHECK_ACTIVE_CONFIG_DESC_PTR(usbd)
         && USBD_DEV_CHECK_ACTIVE_CONFIG_PTR(usbd))
      {
         result = USBD_DEV_SET_INTERFACE_RESULT_SETTING_NOT_FOUND;

         cfg_desc = USBD_DEV_GET_ACTIVE_CONFIG_DESC_PTR(usbd);

         desc_size += cfg_desc[3];
         desc_size *= 256;
         desc_size += cfg_desc[2];

         new_if_pos = USB_CDP_Find_Interface(cfg_desc, desc_size, 0, interface_num, alternative_setting);

         if((0 != new_if_pos) && (new_if_pos < desc_size))
         {
            if(interface_num < USBD_MAX_NUM_INTERFACES)
            {
               result = USBD_DEV_SET_INTERFACE_RESULT_OK;

               /** deconfigure old alternate setting if exist */
               if(USBD_CHECK_PTR(
                  USB_Interface_Desc_DT,
                  (USBD_DEV_GET_INTERFACE_TAB_PTR(usbd))[interface_num].desc))
               {
                  /** find old version od interface in configuration description */
                  old_if_pos = USB_CDP_Find_Interface(
                     cfg_desc,
                     desc_size,
                     0,
                     interface_num,
                     (USBD_DEV_GET_INTERFACE_TAB_PTR(usbd))[interface_num].desc->bAlternateSetting);

                  if((0 != old_if_pos) && (old_if_pos < desc_size))
                  {
                     if_pos2 = USB_CDP_Find_Next_Interface(cfg_desc, desc_size, old_if_pos);
                     if(0 == if_pos2)
                     {
                        if_pos2 = desc_size;
                     }

                     pos = old_if_pos;
                     /** disable all endpoints which were used by old alternate setting of interface */
                     for(ep_cntr = 0; ep_cntr < cfg_desc[old_if_pos + 4]; ep_cntr++)
                     {
                        pos = USB_CDP_Find_Next_Endpoint(cfg_desc, desc_size, pos);

                        if((0 != pos) && (pos < if_pos2))
                        {
                           ep_num = cfg_desc[pos + 2] & USB_EP_DESC_ADDR_MASK;

                           if(USB_EP_DESC_DIR_OUT == (cfg_desc[pos + 2] & USB_EP_DESC_DIR_MASK))
                           {
                              USBD_DEV_SET_EP_OUT_STATE(usbd, ep_num, USBD_DEV_EP_OFF);
#if (defined(USBD_IO_PRESENT))
                              USBD_IO_EP_Disable(usbd, ep_num, USB_EP_DIRECTION_OUT, USBD_FALSE, USBD_FALSE);
#endif
                              USBD_DEV_SET_EP_OUT_INTERFACE(usbd, ep_num, 0xFF);
                              USBD_DEV_SET_EP_OUT_DESC_PTR(usbd, ep_num, USBD_MAKE_INVALID_PTR(const USB_Endpoint_Desc_DT));
                           }
                           else
                           {
                              USBD_DEV_SET_EP_IN_STATE(usbd, ep_num, USBD_DEV_EP_OFF);
#if (defined(USBD_IO_PRESENT))
                              USBD_IO_EP_Disable(usbd, ep_num, USB_EP_DIRECTION_IN, USBD_FALSE, USBD_FALSE);
#endif
                              USBD_DEV_SET_EP_IN_INTERFACE(usbd, ep_num, 0xFF);
                              USBD_DEV_SET_EP_IN_DESC_PTR(usbd, ep_num, USBD_MAKE_INVALID_PTR(const USB_Endpoint_Desc_DT));
                           }
                        }
                        else
                        {
                           break;
                        }
                     }
                  }
               }

               /** configure new alternate setting */

               if_pos2 = USB_CDP_Find_Next_Interface(cfg_desc, desc_size, new_if_pos);
               if(0 == if_pos2)
               {
                  if_pos2 = desc_size;
               }

               if_desc = (USB_Interface_Desc_DT*)(&cfg_desc[new_if_pos]);

               if(interface_num < USBD_MAX_NUM_INTERFACES)
               {
                  (USBD_DEV_GET_INTERFACE_TAB_PTR(usbd))[interface_num].desc = if_desc;
               }

#ifdef USBD_EVENT_PRESENT
               USBD_EVENT_Process_Warm_Event(usbd, USBD_EVENT_REASON_INTERFACE_SET);
#endif

               pos = new_if_pos;
               for(ep_cntr = 0; ep_cntr < cfg_desc[new_if_pos + 4]; ep_cntr++)
               {
                  pos = USB_CDP_Find_Next_Endpoint(cfg_desc, desc_size, pos);

                  if((0 != pos) && (pos < if_pos2))
                  {
                     ep_num = cfg_desc[pos + 2] & USB_EP_DESC_ADDR_MASK;

                     if(USB_EP_DESC_DIR_OUT == (cfg_desc[pos + 2] & USB_EP_DESC_DIR_MASK))
                     {
                        USBD_DEV_SET_EP_OUT_INTERFACE(usbd, ep_num, interface_num);
                        USBD_DEV_SET_EP_OUT_DESC_PTR(usbd, ep_num, (const USB_Endpoint_Desc_DT*)(&cfg_desc[pos]));
#ifdef USBD_IO_PRESENT
                        USBD_IO_EP_Enable_And_Configure(usbd, ep_num, USB_EP_DIRECTION_OUT);
#endif
                        USBD_DEV_SET_EP_OUT_STATE(usbd, ep_num, USBD_DEV_EP_ON);
                     }
                     else
                     {
                        USBD_DEV_SET_EP_IN_INTERFACE(usbd, ep_num, interface_num);
                        USBD_DEV_SET_EP_IN_DESC_PTR(usbd, ep_num, (const USB_Endpoint_Desc_DT*)(&cfg_desc[pos]));
#ifdef USBD_IO_PRESENT
                        USBD_IO_EP_Enable_And_Configure(usbd, ep_num, USB_EP_DIRECTION_IN);
#endif
                        USBD_DEV_SET_EP_IN_STATE(usbd, ep_num, USBD_DEV_EP_ON);
                     }
                  }
                  else
                  {
                     break;
                  }
               }
            }
            else
            {
               result = USBD_DEV_SET_INTERFACE_RESULT_ALT_SETTING_NOT_SUPPORTED;
            }
         }
         else
         {
            USBD_WARN_2(
               USBD_DBG_DEV_INVALID_PARAMS, "invalid interface! if_num: %d, alt: %d", interface_num, alternative_setting);
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_ONOFF);

   return result;
} /* USBD_DEV_Set_Interface */

void USBD_DEV_Suspended(
      USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_DEV_ONOFF);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
#ifdef USBD_EVENT_PRESENT
      USBD_EVENT_Process_Warm_Event(usbd, USBD_EVENT_REASON_SUSPENDED);
#endif

      USBD_DEV_state_change(usbd, usbd->dev.core.data.state | USBD_DEV_STATE_SUSPENDED, __LINE__);
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_ONOFF);
} /* USBD_DEV_Suspended */

void USBD_DEV_Resumed(
      USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_DEV_ONOFF);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
#ifdef USBD_EVENT_PRESENT
      USBD_EVENT_Process_Warm_Event(usbd, USBD_EVENT_REASON_RESUMED);
#endif

      USBD_DEV_state_change(usbd, usbd->dev.core.data.state & ~USBD_DEV_STATE_SUSPENDED, __LINE__);
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_ONOFF);
} /* USBD_DEV_Resumed */

void USBD_DEV_SOF_Received(
      USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_DEV_ONOFF_SOF_RECEIVED);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
#ifdef USBD_EVENT_PRESENT
      USBD_EVENT_Process_Warm_Event(usbd, USBD_EVENT_REASON_SOF_RECEIVED);
#endif

      USBD_DEV_state_change(usbd, usbd->dev.core.data.state & ~USBD_DEV_STATE_SUSPENDED, __LINE__);
   }

   USBD_EXIT_FUNC(USBD_DBG_DEV_ONOFF_SOF_RECEIVED);
} /* USBD_DEV_SOF_Received */

#ifdef USBD_DEV_POST_IMP_INCLUDE
#include "usbd_dev_post_imp.h"
#endif
