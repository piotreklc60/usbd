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
} /* USB_CDP_Find_Interface */

uint16_t USB_CDP_Find_Endpoint (
      const uint8_t *desc,
      uint16_t desc_size,
      uint16_t start_address,
      uint8_t ep_num,
      USB_EP_Direction_ET dir)
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
            if((ep_num == (desc[pos + 2] & 0x0F)) && ((uint8_t)dir == (desc[pos + 2] & USB_EP_DESC_DIR_MASK)))
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
#ifdef USB_CDP_POST_IMP_INCLUDE
#include "usb_cdp_post_imp.h"
#endif
