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

#ifndef USBD_C_
#define USBD_C_
#endif

#ifndef USBD_H_
#include "usbd.h"
#endif

#ifdef USBD_DEV_PRESENT
#ifndef USBD_DEV_INTERNAL_H_
#include "usbd_dev_internal.h"
#endif
#endif

#ifdef USBD_IO_PRESENT
#ifndef USBD_IO_INTERNAL_H_
#include "usbd_io_internal.h"
#endif
#endif

#ifdef USBD_IOTP_EVENT_PRESENT
#ifndef USBD_IOTP_EVENT_PRESENT
#include "usbd_iotp_event_internal.h"
#endif
#endif

#ifdef USBD_EVENT_PRESENT
#ifndef USBD_EVENT_INTERNAL_H_
#include "usbd_event_internal.h"
#endif
#endif

#ifdef USBD_REQUEST_PRESENT
#ifndef USBD_REQUEST_INTERNAL_H_
#include "usbd_request_internal.h"
#endif
#endif


void USBD_Init(
      USBD_Params_XT *usbd)
{
#if ((defined(USBD_IO_PRESENT) || defined(USBD_DEV_PRESENT)) && (USBD_FEATURE_NOT_PRESENT != USBD_USE_INVALIDATION_ON_INIT))
   uint8_t i;
#endif

   USBD_ENTER_FUNC(USBD_DBG);

   if(USBD_CHECK_PTR(void, usbd))
   {
      memset(usbd, 0, sizeof(USBD_Params_XT));

#if (defined(USBD_DEV_PRESENT))
#if (USBD_FEATURE_NOT_PRESENT != USBD_USE_INVALIDATION_ON_INIT)
      USBD_INVALIDATE_PTR_ON_INIT(USBD_DEV_Port_Handler_XT, usbd->dev.core.data.port);
      USBD_INVALIDATE_PTR_ON_INIT(void,                     usbd->dev.core.data.active_config);
      USBD_INVALIDATE_PTR_ON_INIT(uint8_t,                  usbd->dev.core.data.active_config_desc);

      for(i = 0; i < USBD_MAX_NUM_ENDPOINTS; i++)
      {
         USBD_INVALIDATE_PTR_ON_INIT(void,                  usbd->dev.core.data.ep[i].ep_in_desc);
         USBD_INVALIDATE_PTR_ON_INIT(void,                  usbd->dev.core.data.ep[i].ep_out_desc);
      }
#endif

#if(USBD_MAX_NUM_STRINGS > 0)
      USBD_DEV_Init_Structure(usbd);
#endif
#endif

#if (defined(USBD_IO_PRESENT) && (USBD_FEATURE_NOT_PRESENT != USBD_USE_INVALIDATION_ON_INIT))
      for(i = 0; i < USBD_MAX_NUM_ENDPOINTS; i++)
      {
         USBD_INVALIDATE_PTR_ON_INIT(USBD_IO_UP_EP_Handlers_XT    ,usbd->io.in[i].up_link.handlers.handlers);
         USBD_INVALIDATE_PTR_ON_INIT(USBD_IO_UP_EP_Handlers_XT    ,usbd->io.out[i].up_link.handlers.handlers);

         USBD_INVALIDATE_PTR_ON_INIT(void                         ,usbd->io.in[i].up_link.data.tp_params);
         USBD_INVALIDATE_PTR_ON_INIT(void                         ,usbd->io.out[i].up_link.data.tp_params);
         USBD_INVALIDATE_PTR_ON_INIT(void                         ,usbd->io.in[i].up_link.data.tp_owner);
         USBD_INVALIDATE_PTR_ON_INIT(void                         ,usbd->io.out[i].up_link.data.tp_owner);
      }
      USBD_INVALIDATE_PTR_ON_INIT(USBD_IO_DOWN_Common_Handlers_XT ,usbd->io.down_link.common.handlers);
#endif

#ifdef USBD_REQUEST_PRESENT
#if (USBD_FEATURE_NOT_PRESENT != USBD_USE_INVALIDATION_ON_INIT)
      USBD_INVALIDATE_PTR_ON_INIT(USBD_REQUEST_Port_Callbacks_XT  ,usbd->request.core.data.port_cbk);
#endif
      USBD_IOTP_EVENT_Init(usbd, USBD_MAKE_INVALID_PTR(void), 0, USB_EP_DIRECTION_IN, USBD_REQUEST_GET_EP0_IN_IOTP(usbd));
      USBD_IOTP_EVENT_Init(usbd, USBD_MAKE_INVALID_PTR(void), 0, USB_EP_DIRECTION_OUT, USBD_REQUEST_GET_EP0_OUT_IOTP(usbd));
#endif
   }

   USBD_EXIT_FUNC(USBD_DBG);
} /* USBD_Init */

#ifdef USBD_POST_IMP_INCLUDE
#include "usbd_post_imp.h"
#endif
