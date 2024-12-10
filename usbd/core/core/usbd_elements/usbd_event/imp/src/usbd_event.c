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
 *  \brief    USB Device DEV module
 *  \details   This module implements device dependant API which is part of USBD software layer
 *  \author   Piotr Wojtowicz
 *  \version   1.3.0.0
 *  \date     2012
 *  \pre      First initialize the system.
 *  \warning   Improper use can crash your application
 *  \copyright GNU Public License.
 */

#ifndef USBD_EVENT_C_
#define USBD_EVENT_C_
#endif

#ifndef USBD_H_
#include "usbd.h"
#endif

#ifndef USBD_EVENT_INTERNAL_H_
#include "usbd_event_internal.h"
#endif

#ifdef USBD_DEV_PRESENT
#ifndef USBD_DEV_INTERNAL_H_
#include "usbd_dev_internal.h"
#endif
#endif

#ifdef USBDC_PRESENT
#ifndef USBDC_INTERNAL_H_
#include "usbdc_internal.h"
#endif
#endif

static USBD_EVENT_Event_Header_XT *USBD_EVENT_only_once_check_and_install(
   USBD_EVENT_Proc_Params_XT *params,
   USBD_EVENT_Event_HT event,
   size_t max_num_elems,
   USBD_EVENT_Reason_ET mask);

static USBD_Bool_DT USBD_EVENT_find_and_remove(USBD_EVENT_Proc_Params_XT *params, USBD_EVENT_Event_HT event);

static void USBD_EVENT_process_events(
   USBD_Params_XT *usbd,
   USBDC_Params_XT *usbdc,
   USBD_EVENT_Proc_Params_XT *params,
   USBD_EVENT_Reason_ET reason);

static USBD_EVENT_Event_Header_XT *USBD_EVENT_only_once_check_and_install(
   USBD_EVENT_Proc_Params_XT *params,
   USBD_EVENT_Event_HT event,
   size_t max_num_elems,
   USBD_EVENT_Reason_ET mask)
{
   USBD_EVENT_Event_Header_XT *result = USBD_MAKE_INVALID_PTR(USBD_EVENT_Event_Header_XT);
   size_t         pos;
   USBD_Bool_DT   only_once;

   USBD_ENTER_FUNC(USBD_DBG_EVENT_INIT);

   only_once = 0 != (mask & USBD_EVENT_INSTALL_ONLY_ONCE);

   if(USBD_BOOL_IS_TRUE(only_once))
   {
      for(pos = 0; pos < params->num_installed_events; pos++)
      {
         if(USBD_COMPARE_HANDLERS(USBD_EVENT_Event_HT, event, USBD_EVENT_Event_HT, params->event_tab[pos].event))
         {
            result = &(params->event_tab[pos]);
            pos = max_num_elems;
            break;
         }
      }
   }
   else
   {
      pos = params->num_installed_events;
   }

   if(pos < max_num_elems)
   {
      result                        = &(params->event_tab[pos]);
      params->num_installed_events  = pos + 1;
      result->event                 = event;
      result->mask                  = mask & USBD_EVENT_REASON_ALL;
      result->proc_marker           = params->marker - 1;
   }
   else if(!USBD_CHECK_PTR(USBD_EVENT_Event_Header_XT, result))
   {
      USBD_WARN_1(USBD_DBG_EVENT_INVALID_PARAMS, "maximum num of events %d reached!", max_num_elems);
   }

   USBD_EXIT_FUNC(USBD_DBG_EVENT_INIT);

   return result;
} /* USBD_EVENT_only_once_check_and_install */



static USBD_Bool_DT USBD_EVENT_find_and_remove(USBD_EVENT_Proc_Params_XT *params, USBD_EVENT_Event_HT event)
{
   USBD_EVENT_Event_Header_XT *tab = params->event_tab;
   size_t counter;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_EVENT_INIT);

   result = USBD_FALSE;

   for(counter = 0; counter < params->num_installed_events; counter++)
   {
      if(USBD_COMPARE_HANDLERS(USBD_EVENT_Event_HT, event, USBD_EVENT_Event_HT, tab[counter].event))
      {
         params->num_installed_events--;
         tab[counter].event = tab[params->num_installed_events].event;
         tab[counter].mask  = tab[params->num_installed_events].mask;
         memcpy(&(tab[counter].vendor),
            &(tab[params->num_installed_events].vendor),
            sizeof(USBD_Vendor_Data_XT));
         memcpy(&(tab[counter].vendor_data),
            &(tab[params->num_installed_events].vendor_data),
            sizeof(USBD_Vendor_Data_XT));

         if(USBD_BOOL_IS_TRUE(params->proc_ongoing))
         {
            params->proc_dirty = USBD_TRUE;
         }

         result = USBD_TRUE;
         break;
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_EVENT_INIT);

   return result;
} /* USBD_EVENT_find_and_remove */



USBD_EVENT_Event_Header_XT *USBD_EVENT_Install(
      USBD_Params_XT *usbd,
      USBD_EVENT_Event_HT event,
      USBD_EVENT_Reason_ET mask)
{
   USBD_EVENT_Event_Header_XT *result;

   USBD_ENTER_FUNC(USBD_DBG_EVENT_INIT);

   result = USBD_MAKE_INVALID_PTR(USBD_EVENT_Event_Header_XT);

   if((USBD_CHECK_PTR(USBD_Params_XT, usbd)) && (USBD_CHECK_HANDLER(USBD_EVENT_Event_HT, event)))
   {
      result = USBD_EVENT_only_once_check_and_install(
         (USBD_EVENT_Proc_Params_XT*)(usbd->event.core.data), event, USBD_MAX_NUM_EVENTS, mask);
   }
   else
   {
      USBD_WARN_3(USBD_DBG_EVENT_INVALID_PARAMS, "function invalid parameters! %s: %p; event: %p", "usbd", usbd, event);
   }

   USBD_EXIT_FUNC(USBD_DBG_EVENT_INIT);

   return result;
} /* USBD_EVENT_Install */

USBD_Bool_DT USBD_EVENT_Remove_Event(
      USBD_Params_XT *usbd,
      USBD_EVENT_Event_HT event)
{
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_EVENT_INIT);

   result = USBD_FALSE;

   if((USBD_CHECK_PTR(USBD_Params_XT, usbd)) && (USBD_CHECK_HANDLER(USBD_EVENT_Event_HT, event)))
   {
      result = USBD_EVENT_find_and_remove((USBD_EVENT_Proc_Params_XT*)(usbd->event.core.data), event);
   }
   else
   {
      USBD_WARN_3(USBD_DBG_EVENT_INVALID_PARAMS, "function invalid parameters! %s: %p; event: %p", "usbd", usbd, event);
   }

   USBD_EXIT_FUNC(USBD_DBG_EVENT_INIT);

   return result;
} /* USBD_EVENT_Remove_Event */

USBD_Bool_DT USBD_EVENT_Remove_All_Events(
      USBD_Params_XT *usbd)
{
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_EVENT_INIT);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      ((USBD_EVENT_Proc_Params_XT*)(usbd->event.core.data))->num_installed_events = 0;
      result = USBD_TRUE;
   }
   else
   {
      USBD_WARN_2(USBD_DBG_EVENT_INVALID_PARAMS, "function invalid parameters! %s: %p", "usbd", usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_EVENT_INIT);

   return result;
} /* USBD_EVENT_Remove_All_Events */

size_t USBD_EVENT_Get_Num_Installed_Events(
      USBD_Params_XT *usbd)
{
   size_t result = 0;

   USBD_ENTER_FUNC(USBD_DBG_EVENT_STATE);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      result = ((USBD_EVENT_Proc_Params_XT*)(usbd->event.core.data))->num_installed_events;
   }

   USBD_EXIT_FUNC(USBD_DBG_EVENT_STATE);

   return result;
} /* USBD_EVENT_Get_Num_Installed_Events */



USBD_EVENT_Event_Header_XT *USBDC_EVENT_Install(
      USBDC_Params_XT *usbdc,
      USBD_EVENT_Event_HT event,
      USBD_EVENT_Reason_ET mask)
{
   USBD_EVENT_Event_Header_XT *result;

   USBD_ENTER_FUNC(USBD_DBG_EVENT_INIT);

   result = USBD_MAKE_INVALID_PTR(USBD_EVENT_Event_Header_XT);

   if((USBD_CHECK_PTR(USBDC_Params_XT, usbdc)) && (USBD_CHECK_HANDLER(USBD_EVENT_Event_HT, event)))
   {
      result = USBD_EVENT_only_once_check_and_install(
         (USBD_EVENT_Proc_Params_XT*)(usbdc->event.core.data), event, USBDC_MAX_NUM_EVENTS, mask);
   }
   else
   {
      USBD_WARN_3(USBD_DBG_EVENT_INVALID_PARAMS, "function invalid parameters! %s: %p; event: %p", "usbdc", usbdc, event);
   }

   USBD_EXIT_FUNC(USBD_DBG_EVENT_INIT);

   return result;
} /* USBDC_EVENT_Install */

USBD_Bool_DT USBDC_EVENT_Remove_Event(
      USBDC_Params_XT *usbdc,
      USBD_EVENT_Event_HT event)
{
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_EVENT_INIT);

   result = USBD_FALSE;

   if((USBD_CHECK_PTR(USBDC_Params_XT, usbdc)) && (USBD_CHECK_HANDLER(USBD_EVENT_Event_HT, event)))
   {
      result = USBD_EVENT_find_and_remove((USBD_EVENT_Proc_Params_XT*)(usbdc->event.core.data), event);
   }
   else
   {
      USBD_WARN_3(USBD_DBG_EVENT_INVALID_PARAMS, "function invalid parameters! %s: %p; event: %p", "usbdc", usbdc, event);
   }

   USBD_EXIT_FUNC(USBD_DBG_EVENT_INIT);

   return result;
} /* USBDC_EVENT_Remove_Event */

USBD_Bool_DT USBDC_EVENT_Remove_All_Events(
      USBDC_Params_XT *usbdc)
{
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_EVENT_INIT);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBDC_Params_XT, usbdc))
   {
      ((USBD_EVENT_Proc_Params_XT*)(usbdc->event.core.data))->num_installed_events = 0;
      result = USBD_TRUE;
   }
   else
   {
      USBD_WARN_2(USBD_DBG_EVENT_INVALID_PARAMS, "function invalid parameters! %s: %p", "usbdc", usbdc);
   }

   USBD_EXIT_FUNC(USBD_DBG_EVENT_INIT);

   return result;
} /* USBDC_EVENT_Remove_All_Events */

size_t USBDC_EVENT_Get_Num_Installed_Events(
      USBDC_Params_XT *usbdc)
{
   size_t result = 0;

   USBD_ENTER_FUNC(USBD_DBG_EVENT_STATE);

   if(USBD_CHECK_PTR(USBDC_Params_XT, usbdc))
   {
      result = ((USBD_EVENT_Proc_Params_XT*)(usbdc->event.core.data))->num_installed_events;
   }

   USBD_EXIT_FUNC(USBD_DBG_EVENT_STATE);

   return result;
} /* USBDC_EVENT_Get_Num_Installed_Events */



static void USBD_EVENT_process_events(
   USBD_Params_XT *usbd,
   USBDC_Params_XT *usbdc,
   USBD_EVENT_Proc_Params_XT *params,
   USBD_EVENT_Reason_ET reason)
{
   USBD_EVENT_Event_Header_XT *tab = params->event_tab;
   size_t counter;

   USBD_ENTER_FUNC(USBD_DBG_EVENT_PROCESSING);

   params->marker++;
   params->proc_ongoing = USBD_TRUE;
   params->proc_dirty   = USBD_TRUE;

   while(USBD_BOOL_IS_TRUE(params->proc_dirty))
   {
      params->proc_dirty = USBD_FALSE;
      for(counter = 0; counter < params->num_installed_events; counter++)
      {
         if(params->marker != tab[counter].proc_marker)
         {
            tab[counter].proc_marker = params->marker;
            if(0 != (tab[counter].mask & reason))
            {
               if(USBD_CHECK_HANDLER(USBD_EVENT_Event_HT, tab[counter].event))
               {
                  USBD_EVENT_CALL_EVENT(usbd, usbdc, tab, counter, reason);
               }
            }
         }
      }
   }

   params->proc_ongoing = USBD_FALSE;

   USBD_EXIT_FUNC(USBD_DBG_EVENT_PROCESSING);
} /* USBD_EVENT_process_events */

void USBD_EVENT_Process_Cold_Event(
      USBD_Params_XT *usbd,
      USBD_EVENT_Reason_ET reason)
{
   USBDC_Params_XT *usbdc;
   USBD_DEV_Config_Header_XT *config_tab;
   uint8_t counter;
   uint8_t num_of_configs;

   USBD_ENTER_FUNC(USBD_DBG_EVENT_PROCESSING);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      /* processes device events */
      if(((USBD_EVENT_Proc_Params_XT*)(usbd->event.core.data))->num_installed_events > 0)
      {
         USBD_EVENT_process_events(
            usbd,
            USBD_MAKE_INVALID_PTR(USBDC_Params_XT),
            (USBD_EVENT_Proc_Params_XT*)(usbd->event.core.data),
            reason);
      }

      num_of_configs = usbd->dev.core.data.dev_desc.bNumConfigurations;

      config_tab = USBD_DEV_GET_CONFIG_TAB_PTR(usbd);

      /* processes events for all configurations */
      for(counter = 0; counter < num_of_configs; counter++)
      {
         usbdc = config_tab[counter].usbdc;

         if(USBD_CHECK_PTR(USBDC_Params_XT, usbdc))
         {
            if(((USBD_EVENT_Proc_Params_XT*)(usbdc->event.core.data))->num_installed_events > 0)
            {
               USBD_EVENT_process_events(
                  usbd,
                  usbdc,
                  (USBD_EVENT_Proc_Params_XT*)(usbdc->event.core.data),
                  reason);
            }
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_EVENT_PROCESSING);
} /* USBD_EVENT_Process_Cold_Event */

void USBD_EVENT_Process_Warm_Event(
      USBD_Params_XT *usbd,
      USBD_EVENT_Reason_ET reason)
{
   USBDC_Params_XT *usbdc;

   USBD_ENTER_FUNC(USBD_DBG_EVENT_PROCESSING);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      /* processes device events */
      if(((USBD_EVENT_Proc_Params_XT*)(usbd->event.core.data))->num_installed_events > 0)
      {
         USBD_EVENT_process_events(
            usbd,
            USBD_MAKE_INVALID_PTR(USBDC_Params_XT),
            (USBD_EVENT_Proc_Params_XT*)(usbd->event.core.data),
            reason);
      }

      if(USBD_DEV_GET_ACTIVE_CONFIG_NUM(usbd) < usbd->dev.core.data.dev_desc.bNumConfigurations)
      {
         /* processes events for active configuration */
         usbdc = USBD_DEV_GET_ACTIVE_CONFIG_PTR(usbd);

         if(USBD_CHECK_PTR(USBDC_Params_XT, usbdc))
         {
            if(((USBD_EVENT_Proc_Params_XT*)(usbdc->event.core.data))->num_installed_events > 0)
            {
               USBD_EVENT_process_events(
                  usbd,
                  usbdc,
                  (USBD_EVENT_Proc_Params_XT*)(usbdc->event.core.data),
                  reason);
            }
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_EVENT_PROCESSING);
} /* USBD_EVENT_Process_Warm_Event */

#ifdef USBD_EVENT_POST_IMP_INCLUDE
#include "usbd_event_post_imp.h"
#endif
