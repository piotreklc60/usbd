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

/*!
 *  \brief    USB Device IO module
 *  \details   This module defines IN-OUT API which is part of USBD software layer
 *  \author   Piotr Wojtowicz
 *  \version   1.3.0.0
 *  \date     2012
 *  \pre      First initialize the system.
 *  \warning   Improper use can crash your application
 *  \copyright GNU Public License.
 */

#ifndef USBDC_C_
#define USBDC_C_
#endif

#ifndef USBD_H_
#include "usbd.h"
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



void USBDC_Init(
      USBDC_Params_XT *usbdc)
{
#if (USBD_FEATURE_NOT_PRESENT != USBD_USE_INVALIDATION_ON_INIT)
   int cntr;
#endif

   USBD_ENTER_FUNC(USBD_DBG_USBDC);

   if(USBD_CHECK_PTR(void, usbdc))
   {
      memset(usbdc, 0, sizeof(USBDC_Params_XT));

#if (USBD_FEATURE_NOT_PRESENT != USBD_USE_INVALIDATION_ON_INIT)
#if (defined(USBD_REQUEST_PRESENT))
      USBD_INVALIDATE_HANDLER_ON_INIT(USBD_REQUEST_Vendor_HT, usbdc->request.core.handlers.vendor_irq);
#endif

      for(cntr = 0; cntr < USBD_MAX_NUM_INTERFACES; cntr++)
      {
#if (defined(USBD_REQUEST_PRESENT))
         USBD_INVALIDATE_HANDLER_ON_INIT(USBD_REQUEST_Interface_HT, usbdc->request.core.handlers.interface_irq_tab[cntr]);
#endif

         USBD_INVALIDATE_HANDLER_ON_INIT(USBD_Hvoid_HT,  USBDC_GET_INTERFACE_PARAMS_PTR(usbdc, cntr)->vendor.hvoid);
         USBD_INVALIDATE_HANDLER_ON_INIT(USBD_Hvoid_HT,  USBDC_GET_INTERFACE_PARAMS_PTR(usbdc, cntr)->vendor_data.hvoid);
      }
#endif
   }

   USBD_EXIT_FUNC(USBD_DBG_USBDC);
} /* USBDC_Init */

void USBDC_Bind_Usbd(
      USBDC_Params_XT *usbdc,
      USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_USBDC);

   if(USBD_CHECK_PTR(USBDC_Params_XT, usbdc))
   {
      USBDC_SET_USBD(usbdc, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_USBDC);
} /* USBDC_Bind_Usbd */

USBD_Params_XT *USBDC_Get_Usbd(
      USBDC_Params_XT *usbdc)
{
   USBD_Params_XT *result = USBD_MAKE_INVALID_PTR(USBD_Params_XT);

   USBD_ENTER_FUNC(USBD_DBG_USBDC);

   if(USBD_CHECK_PTR(USBDC_Params_XT, usbdc))
   {
      result = USBDC_GET_USBD(usbdc);
   }

   USBD_EXIT_FUNC(USBD_DBG_USBDC);

   return result;
} /* USBDC_Get_Usbd */

void USBDC_Bind_Config_Desc(
      USBDC_Params_XT *usbdc,
      const uint8_t *config_desc)
{
   USBD_ENTER_FUNC(USBD_DBG_USBDC);

   if(USBD_CHECK_PTR(USBDC_Params_XT, usbdc))
   {
      USBDC_SET_CONFIG_DESC(usbdc, config_desc);
   }

   USBD_EXIT_FUNC(USBD_DBG_USBDC);
} /* USBDC_Bind_Config_Desc */

const uint8_t *USBDC_Get_Config_Desc(
      USBDC_Params_XT *usbdc)
{
   const uint8_t *result = USBD_MAKE_INVALID_PTR(const uint8_t);

   USBD_ENTER_FUNC(USBD_DBG_USBDC);

   if(USBD_CHECK_PTR(USBDC_Params_XT, usbdc))
   {
      result = USBDC_GET_CONFIG_DESC(usbdc);
   }

   USBD_EXIT_FUNC(USBD_DBG_USBDC);

   return result;
} /* USBDC_Get_Config_Desc */

USBDC_Interface_Header_XT *USBDC_Get_Interface_Params(
      USBDC_Params_XT *usbdc,
      uint8_t interface_num)
{
   USBDC_Interface_Header_XT *result;

   USBD_ENTER_FUNC(USBD_DBG_USBDC);

   result = USBD_MAKE_INVALID_PTR(USBDC_Interface_Header_XT);

   if(USBD_CHECK_PTR(void, usbdc))
   {
      if(interface_num < USBD_MAX_NUM_INTERFACES)
      {
         result = USBDC_GET_INTERFACE_PARAMS_PTR(usbdc, interface_num);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_USBDC);

   return result;
} /* USBDC_get_Interface_Params */

#ifdef USBDC_POST_IMP_INCLUDE
#include "usbdc_post_imp.h"
#endif
