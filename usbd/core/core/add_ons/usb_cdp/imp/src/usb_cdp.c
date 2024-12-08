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

#ifndef USB_CDP_C_
#define USB_CDP_C_
#endif

#ifndef USBD_H_
#include "usbd.h"
#endif


//#if((USBD_MAX_NUM_ENDPOINTS > 1) || (USBD_ALTERNATE_SETTING_SUPPORT == USBD_FEATURE_ON) || (USBD_UPPER_LAYER_SUPPORT == USBD_FEATURE_ON) || (!USBD_CDP_USED_JUST_BY_USBD_CORE))
uint16_t USB_CDP_Get_Config_Desc_Total_Length(const uint8_t *desc)
{
   uint16_t result = 0;

   USB_ENTER_FUNC(USB_DBG_CDP);

   if(USBD_CHECK_PTR(uint8_t, desc))
   {
      result += desc[3];
      result *= 256;
      result += desc[2];
   }

   USB_EXIT_FUNC(USB_DBG_CDP);

   return result;
} /* USB_CDP_Get_Config_Desc_Total_Length */

uint16_t USB_CDP_Find_Interface (
      const uint8_t *desc,
      uint16_t desc_size,
      uint16_t start_address,
      uint8_t interface_number,
      uint8_t alternate_setting)
{
   uint16_t result = desc_size;
   uint16_t pos;

   USB_ENTER_FUNC(USB_DBG_CDP);

   if((USBD_CHECK_PTR(uint8_t, desc)) && (start_address < desc_size))
   {
      for(pos = start_address + desc[start_address]; pos < desc_size; pos += desc[pos])
      {
         if(USB_DESC_TYPE_INTERFACE == desc[pos + 1])
         {
            if((desc[pos + 2] == interface_number) && (desc[pos + 3] == alternate_setting))
            {
               result = pos;
               break;
            }
         }
         if(0 == desc[pos])
         {
            break;
         }
      }
   }

   USB_EXIT_FUNC(USB_DBG_CDP);

   return result;
} /* USB_CDP_Find_Interface */

uint16_t USB_CDP_Find_Next_Interface (
      const uint8_t *desc,
      uint16_t desc_size,
      uint16_t start_address)
{
   uint16_t result = desc_size;
   uint16_t pos;

   USB_ENTER_FUNC(USB_DBG_CDP);

   if((USBD_CHECK_PTR(uint8_t, desc)) && (start_address < desc_size))
   {
      for(pos = start_address + desc[start_address]; pos < desc_size; pos += desc[pos])
      {
         if(USB_DESC_TYPE_INTERFACE == desc[pos + 1])
         {
            result = pos;
            break;
         }
         if(0 == desc[pos])
         {
            break;
         }
      }
   }

   USB_EXIT_FUNC(USB_DBG_CDP);

   return result;
} /* USB_CDP_Find_Next_Interface */

//#if((USBD_ALTERNATE_SETTING_SUPPORT == USBD_FEATURE_ON) || (!USBD_CDP_USED_JUST_BY_USBD_CORE))
uint8_t USB_CDP_Find_Interface_Max_Alternate_Setting (
      const uint8_t *desc,
      uint16_t desc_size,
      uint8_t interface_number)
{
   uint16_t pos;
   uint8_t max_alt_setting = desc_size;

   USB_ENTER_FUNC(USB_DBG_CDP);

   if(USBD_CHECK_PTR(uint8_t, desc))
   {
      for(pos = desc[0]; pos < desc_size; pos += desc[pos])
      {
         if(USB_DESC_TYPE_INTERFACE == desc[pos + 1])
         {
            if((desc[pos + 2] == interface_number) && (desc[pos + 3] > max_alt_setting))
            {
               max_alt_setting = desc[pos + 3];
            }
         }
         if(0 == desc[pos])
         {
            break;
         }
      }
   }

   USB_EXIT_FUNC(USB_DBG_CDP);

   return max_alt_setting;
} /* USB_CDP_Find_Interface_Max_Alternate_Setting */
//#endif

uint16_t USB_CDP_Find_Endpoint (
      const uint8_t *desc,
      uint16_t desc_size,
      uint16_t start_address,
      uint8_t ep_num,
      USB_EP_Direction_ET dir)
{
   uint16_t result = desc_size;
   uint16_t pos;
   uint8_t direction;

   USB_ENTER_FUNC(USB_DBG_CDP);

   direction = (USB_EP_DIRECTION_OUT == dir) ? USB_EP_DESC_DIR_OUT : USB_EP_DESC_DIR_IN;

   if((USBD_CHECK_PTR(uint8_t, desc)) && (ep_num < USBD_MAX_NUM_ENDPOINTS) && (start_address < desc_size))
   {
      for(pos = start_address + desc[start_address]; pos < desc_size; pos += desc[pos])
      {
         if(USB_DESC_TYPE_ENDPOINT == desc[pos + 1])
         {
            if((ep_num == (desc[pos + 2] & 0x0F)) && (direction == (desc[pos + 2] & USB_EP_DESC_DIR_MASK)))
            {
               result = pos;
               break;
            }
         }
         if(0 == desc[pos])
         {
            break;
         }
      }
   }

   USB_EXIT_FUNC(USB_DBG_CDP);

   return result;
} /* USB_CDP_Find_Endpoint */

//#if((USBD_MAX_NUM_ENDPOINTS > 1) || (!USBD_CDP_USED_JUST_BY_USBD_CORE))
uint16_t USB_CDP_Find_Next_Endpoint (
      const uint8_t *desc,
      uint16_t desc_size,
      uint16_t start_address)
{
   uint16_t result = desc_size;
   uint16_t pos;

   USB_ENTER_FUNC(USB_DBG_CDP);

   if((USBD_CHECK_PTR(uint8_t, desc)) && (start_address < desc_size))
   {
      for(pos = start_address + desc[start_address]; pos < desc_size; pos += desc[pos])
      {
         if(USB_DESC_TYPE_ENDPOINT == desc[pos + 1])
         {
            result = pos;
            break;
         }
         if(0 == desc[pos])
         {
            break;
         }
      }
   }

   USB_EXIT_FUNC(USB_DBG_CDP);

   return result;
} /* USB_CDP_Find_Next_Endpoint */

void USB_CDP_Create_EP_Max_Used_Packet_Size_Table (uint8_t *descriptor, uint16_t *table) {
   union {
      uint16_t uint16;
      struct {
         uint8_t L;
         uint8_t H;
      }elems;
   }total_length, max_packet_size;
   uint16_t position = 0;
   uint8_t ep_number;

   USB_ENTER_FUNC(USB_DBG_CDP);

   total_length.elems.L = descriptor[2];
   total_length.elems.H = descriptor[3];

   for(ep_number = 0; ep_number < USBD_MAX_NUM_ENDPOINTS; ep_number++) {
      table[ep_number] = 0;
   }

   do {
      if(descriptor[position + 1] == USB_DESC_TYPE_ENDPOINT) {
         ep_number = descriptor[position + 2] & 0x0F;
         if(ep_number < USBD_MAX_NUM_ENDPOINTS) {
            max_packet_size.elems.L = descriptor[position + 4];
            max_packet_size.elems.H = descriptor[position + 5] & 0x07;
            if(table[ep_number] < max_packet_size.uint16) {
               table[ep_number] = max_packet_size.uint16;
            }
         }
      }
      position += descriptor[position];
      if(0 == descriptor[position])
      {
         break;
      }
   } while (position < total_length.uint16);

   USB_EXIT_FUNC(USB_DBG_CDP);
} /* USB_CDP_Create_EP_Max_Used_Packet_Size_Table */
//#endif
//#endif

#ifdef USB_CDP_POST_IMP_INCLUDE
#include "usb_cdp_post_imp.h"
#endif
