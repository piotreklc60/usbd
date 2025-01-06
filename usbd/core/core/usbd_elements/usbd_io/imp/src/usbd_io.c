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

#ifndef USBD_IO_C_
#define USBD_IO_C_
#endif

#ifndef USBD_H_
#include "usbd.h"
#endif

#ifdef USBD_DEV_PRESENT
#ifndef USBD_DEV_INTERNAL_H_
#include "usbd_dev_internal.h"
#endif
#endif

#ifndef USBD_IO_INTERNAL_H_
#include "usbd_io_internal.h"
#endif

void USBD_IO_EP_Enable_And_Configure(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir)
{
#if(USBD_MAX_NUM_ENDPOINTS > 1)
   const USBD_IO_DOWN_Common_Handlers_XT *down_handlers;
#endif
   USBD_IO_Pipe_Params_XT                *pipe;
   const USBD_IO_UP_EP_Handlers_XT       *ep_handlers;
#ifdef USBD_USE_IOCMD
   const char                            *dir_desc;
#endif

   USBD_ENTER_FUNC(USBD_DBG_IO_ONOFF);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      if(USB_EP_DIRECTION_OUT == dir)
      {
#ifdef USBD_USE_IOCMD
         dir_desc = "OUT";
#endif
         pipe = USBD_IO_GET_OUT_PIPE_PARAMS(usbd, ep_num);
      }
      else
      {
#ifdef USBD_USE_IOCMD
         dir_desc = "IN";
#endif
         pipe = USBD_IO_GET_IN_PIPE_PARAMS(usbd, ep_num);
      }

      USBD_NOTICE_2(USBD_DBG_IO_ONOFF, "EP: %d.%s enabled", ep_num, dir_desc);

      /**
       * EP activity - used during TP setting
       */
      USBD_IO_CORE_CLEAR_PIPE_ACTIVITY_MARKER(pipe);

      ep_handlers = pipe->up_link.handlers.handlers;
      /**
      * there is needed to enable upper layer before enabling port to prevent from situation
      * when hardware will call IRQ when upper layer was not initialized yet
      */
      if(USBD_CHECK_PTR(const USBD_IO_UP_EP_Handlers_XT, ep_handlers))
      {
         if(USBD_IO_UP_CHECK_REINIT_HANDLER(ep_handlers))
         {
            USBD_IO_CALL_UP_REINIT(
               pipe->up_link.data.tp_params,
               pipe->up_link.data.tp_owner,
               ep_handlers,
               USBD_IO_GET_PIPE_TRANSACATION_PARAMS(pipe),
               USBD_TRUE,
               USBD_IO_UP_PIPE_GET_RECURSIVE_FLAGS(pipe));
         }
      }

      /**
       * EP activity - used during TP setting
       */
      USBD_IO_CORE_SET_PIPE_ACTIVITY_MARKER(pipe);

#if(USBD_MAX_NUM_ENDPOINTS > 1)
      /**
       * call "DOWN_CONFIGURE" to enable hardware
       */
      if(USBD_IO_DOWN_CHECK_COMMON_HANDLERS_PTR(usbd))
      {
         down_handlers = USBD_IO_DOWN_GET_COMMON_HANDLERS_PTR(usbd);

         if(USBD_IO_DOWN_CHECK_CONFIGURE_HANDLER(down_handlers))
         {
            USBD_IO_CALL_DOWN_CONFIGURE(usbd, down_handlers, ep_num, dir, USBD_TRUE);
         }
      }
#endif
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_ONOFF);
} /* USBD_IO_EP_Enable_And_Configure */

void USBD_IO_EP_Disable(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_Bool_DT force_tp_remove,
      USBD_Bool_DT force_hw_disabling)
{
#if(USBD_MAX_NUM_ENDPOINTS > 1)
   const USBD_IO_DOWN_Common_Handlers_XT *down_handlers;
#endif
   USBD_IO_Pipe_Params_XT                *pipe;
   const USBD_IO_UP_EP_Handlers_XT       *ep_handlers;
#ifdef USBD_USE_IOCMD
   const char                            *dir_desc;
#endif

   USBD_ENTER_FUNC(USBD_DBG_IO_ONOFF);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      if(USB_EP_DIRECTION_OUT == dir)
      {
#ifdef USBD_USE_IOCMD
         dir_desc = "OUT";
#endif
         pipe = USBD_IO_GET_OUT_PIPE_PARAMS(usbd, ep_num);
      }
      else
      {
#ifdef USBD_USE_IOCMD
         dir_desc = "IN";
#endif
         pipe = USBD_IO_GET_IN_PIPE_PARAMS(usbd, ep_num);
      }

      USBD_NOTICE_2(USBD_DBG_IO_ONOFF, "EP: %d.%s disabled", ep_num, dir_desc);

      /**
       * EP activity - used during TP setting
       */
      USBD_IO_CORE_CLEAR_PIPE_ACTIVITY_MARKER(pipe);

      if(USBD_BOOL_IS_TRUE(usbd->dev.core.data.active) || USBD_BOOL_IS_TRUE(force_hw_disabling))
      {
#if(USBD_MAX_NUM_ENDPOINTS > 1)
         /**
          * call "DOWN_CONFIGURE" to disable hardware
          */
         if(USBD_IO_DOWN_CHECK_COMMON_HANDLERS_PTR(usbd))
         {
            down_handlers = USBD_IO_DOWN_GET_COMMON_HANDLERS_PTR(usbd);

            if(USBD_IO_DOWN_CHECK_CONFIGURE_HANDLER(down_handlers))
            {
               USBD_IO_CALL_DOWN_CONFIGURE(usbd, down_handlers, ep_num, dir, USBD_FALSE);
            }
         }
#endif
         ep_handlers = pipe->up_link.handlers.handlers;

         if(USBD_CHECK_PTR(const USBD_IO_UP_EP_Handlers_XT, ep_handlers))
         {
            if(USBD_IO_UP_CHECK_REINIT_HANDLER(ep_handlers))
            {
               USBD_IO_CALL_UP_REINIT(
                  pipe->up_link.data.tp_params,
                  pipe->up_link.data.tp_owner,
                  ep_handlers,
                  USBD_IO_GET_PIPE_TRANSACATION_PARAMS(pipe),
                  USBD_FALSE,
                  USBD_IO_UP_PIPE_GET_RECURSIVE_FLAGS(pipe));
            }
         }
      }

      /**
       * If TP is not locked then it is completely removed on following events:
       * - USBD activated (USBD_DEV_Activate was called)
       * - USBD deactivated (USBD_DEV_Deactivate was called)
       * - USB power ON
       * - USB power OFF
       * - USB reset
       * - configuration set
       * - interface set.
       * It should to be installed again on USBD_EVENT_REASON_CONFIGURED or USBD_EVENT_REASON_INTERFACE_SET event.
       * If TP is locked then it is completely removed only on following events:
       * - USBD activated (USBD_DEV_Activate was called)
       * - USBD deactivated (USBD_DEV_Deactivate was called)
       * - USB power ON
       * - USB power OFF
       * - USB reset
       * - configuration set
       * It should be installed again and locked on USBD_EVENT_REASON_CONFIGURED.
       */
      if(USBD_BOOL_IS_FALSE(USBD_IO_PIPE_GET_LOCK_TP_STATE(pipe)) || USBD_BOOL_IS_TRUE(force_tp_remove))
      {
         USBD_IO_PIPE_UNLOCK_TP(pipe);
         USBD_IO_UP_PIPE_SET_HANDLERS_PTR(pipe,  USBD_MAKE_INVALID_PTR(const USBD_IO_UP_EP_Handlers_XT));
         USBD_IO_UP_PIPE_SET_TP_PARAMS_PTR(pipe, USBD_MAKE_INVALID_PTR(void));
         USBD_IO_UP_PIPE_SET_TP_OWNER_PTR(pipe,  USBD_MAKE_INVALID_PTR(void));
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_ONOFF);
} /* USBD_IO_EP_Disable */

void USBD_IO_Perform_Halt(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_Bool_DT state)
{
   USBD_IO_Pipe_Params_XT *pipe;
   const USBD_IO_DOWN_Common_Handlers_XT *common_handlers;
   const USBD_IO_UP_EP_Handlers_XT *ep_handlers;
#ifdef USBD_USE_IOCMD
   const char *dir_desc;
#endif
   USBD_Bool_DT call_down_layer = USBD_FALSE;

   USBD_ENTER_FUNC(USBD_DBG_IO_ONOFF);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      if(USB_EP_DIRECTION_OUT == dir)
      {
#ifdef USBD_USE_IOCMD
         dir_desc = "OUT";
#endif
         pipe = USBD_IO_GET_OUT_PIPE_PARAMS(usbd, ep_num);
      }
      else
      {
#ifdef USBD_USE_IOCMD
         dir_desc = "IN";
#endif
         pipe = USBD_IO_GET_IN_PIPE_PARAMS(usbd, ep_num);
      }

      common_handlers = USBD_IO_DOWN_GET_COMMON_HANDLERS_PTR(usbd);
      if(USBD_CHECK_PTR(const USBD_IO_DOWN_Common_Handlers_XT, common_handlers)
         && USBD_IO_DOWN_CHECK_HALT_HANDLER(common_handlers))
      {
         call_down_layer = USBD_TRUE;
      }

      USBD_NOTICE_3(USBD_DBG_IO_ONOFF, "halt set to '%s' for EP: %d.%s",
         USBD_BOOL_IS_FALSE(state) ? "false" : "true", ep_num, dir_desc);

      USBD_IO_CORE_CLEAR_PIPE_ACTIVITY_MARKER(pipe);

      /**
      * if state is true (halt shall be set) then "DOWN_CALL" shall be called
      * before "UP_REINIT" to stop hardware activities before stopping upper layer
      */
      if(USBD_BOOL_IS_TRUE(state) && USBD_BOOL_IS_TRUE(call_down_layer))
      {
         USBD_IO_CALL_DOWN_HALT(usbd, common_handlers, ep_num, dir, USBD_TRUE);
      }

      ep_handlers = pipe->up_link.handlers.handlers;

      if(USBD_CHECK_PTR(const USBD_IO_UP_EP_Handlers_XT, ep_handlers))
      {
         if(USBD_IO_UP_CHECK_REINIT_HANDLER(ep_handlers))
         {
            USBD_IO_CALL_UP_REINIT(
               pipe->up_link.data.tp_params,
               pipe->up_link.data.tp_owner,
               ep_handlers,
               USBD_IO_GET_PIPE_TRANSACATION_PARAMS(pipe),
               state,
               USBD_IO_UP_PIPE_GET_RECURSIVE_FLAGS(pipe));
         }
      }

      /**
      * if state is false (halt shall be cleared) then "DOWN_CALL" shall be called
      * after "UP_REINIT" to enable hardware activities when upper layer is prepared for processing IRQs
      */
      if(USBD_BOOL_IS_FALSE(state))
      {
         USBD_IO_CORE_SET_PIPE_ACTIVITY_MARKER(pipe);

         if(USBD_BOOL_IS_TRUE(call_down_layer))
         {
            USBD_IO_CALL_DOWN_HALT(usbd, common_handlers, ep_num, dir, USBD_FALSE);
         }
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_ONOFF);
} /* USBD_IO_Perform_Halt */

void USBD_IO_UP_Lock_TP_Params(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir)
{
   USBD_ENTER_FUNC(USBD_DBG_IO_STATE);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      USBD_INFO_HI_2(USBD_DBG_IO_STATE, "lock TP params for EP: %d.%s", ep_num, (USB_EP_DIRECTION_OUT == dir) ? "OUT" : "IN");

      if(USB_EP_DIRECTION_OUT == dir)
      {
         USBD_IO_OUT_LOCK_TP(usbd, ep_num);
      }
      else
      {
         USBD_IO_IN_LOCK_TP(usbd, ep_num);
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_STATE);
} /* USBD_IO_UP_Lock_TP_Params */

USBD_Bool_DT USBD_IO_UP_Is_TP_Locked(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir)
{
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_IO_STATE);

   result = USBD_FALSE;

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      result = (USB_EP_DIRECTION_OUT == dir) ?
         USBD_IO_OUT_GET_LOCK_TP_STATE(usbd, ep_num) : USBD_IO_IN_GET_LOCK_TP_STATE(usbd, ep_num);
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_STATE);

   return result;
} /* USBD_IO_UP_Is_TP_Locked */

USBD_Bool_DT USBD_IO_UP_Is_EP_Active(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir)
{
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_IO_STATE);

   result = USBD_FALSE;

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      result = (USB_EP_DIRECTION_OUT == dir) ?
         USBD_IO_CORE_GET_OUT_EP_ACTIVITY_MARKER(usbd, ep_num) :
         USBD_IO_CORE_GET_IN_EP_ACTIVITY_MARKER(usbd, ep_num);
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_STATE);

   return result;
} /* USBD_IO_UP_Is_EP_Active */


void USBD_IO_UP_Set_TP(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      const USBD_IO_UP_EP_Handlers_XT *handlers,
      void *tp_params,
      void *tp_owner)
{
   USBD_IO_Pipe_Params_XT                *pipe;
#ifdef USBD_USE_IOCMD
   const char                            *dir_desc;
#endif

   USBD_ENTER_FUNC(USBD_DBG_IO_STATE);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      if(USB_EP_DIRECTION_OUT == dir)
      {
#ifdef USBD_USE_IOCMD
         dir_desc = "OUT";
#endif
         pipe = USBD_IO_GET_OUT_PIPE_PARAMS(usbd, ep_num);
      }
      else
      {
#ifdef USBD_USE_IOCMD
         dir_desc = "IN";
#endif
         pipe = USBD_IO_GET_IN_PIPE_PARAMS(usbd, ep_num);
      }

      if(USBD_BOOL_IS_FALSE(USBD_IO_PIPE_GET_LOCK_TP_STATE(pipe)))
      {
         USBD_INFO_HI_5(USBD_DBG_IO_STATE, "set TP: params:%p; owner:%p; handlers:%p for EP: %d.%s",
            tp_params, tp_owner, handlers, ep_num, dir_desc);

         USBD_IO_UP_PIPE_SET_HANDLERS_PTR(pipe,  handlers);
         USBD_IO_UP_PIPE_SET_TP_PARAMS_PTR(pipe, tp_params);
         USBD_IO_UP_PIPE_SET_TP_OWNER_PTR(pipe,  tp_owner);

         /**
          * if endpoint was already active and we are only changing TP then TP must be immediately reactivated.
          */
         if(USBD_BOOL_IS_TRUE(USBD_IO_CORE_GET_PIPE_ACTIVITY_MARKER(pipe)))
         {
            if(USBD_CHECK_PTR(const USBD_IO_UP_EP_Handlers_XT, handlers))
            {
               if(USBD_IO_UP_CHECK_REINIT_HANDLER(handlers))
               {
                  USBD_IO_CALL_UP_REINIT(
                     tp_params,
                     tp_owner,
                     handlers,
                     USBD_IO_GET_PIPE_TRANSACATION_PARAMS(pipe),
                     USBD_TRUE,
                     USBD_IO_UP_PIPE_GET_RECURSIVE_FLAGS(pipe));
               }
            }
         }
      }
      else
      {
         USBD_WARN_2(USBD_DBG_IO_STATE, "set TP params for locked EP: %d.%s failed!", ep_num, dir_desc);
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_STATE);
} /* USBD_IO_UP_Set_TP */

void *USBD_IO_UP_Get_TP_Params(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir)
{
   void *result;

   USBD_ENTER_FUNC(USBD_DBG_IO_STATE);

   result = USBD_MAKE_INVALID_PTR(void);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      result = (USB_EP_DIRECTION_OUT == dir) ?
         USBD_IO_UP_OUT_GET_TP_PARAMS_PTR(usbd, ep_num)
         : USBD_IO_UP_IN_GET_TP_PARAMS_PTR(usbd, ep_num);
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_STATE);

   return result;
} /* USBD_IO_UP_Get_TP_Params */

void *USBD_IO_UP_Get_OUT_TP_Params(
      USBD_Params_XT *usbd,
      uint8_t ep_num)
{
   void *result;

   USBD_ENTER_FUNC(USBD_DBG_IO_STATE);

   result = USBD_MAKE_INVALID_PTR(void);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      result = USBD_IO_UP_OUT_GET_TP_PARAMS_PTR(usbd, ep_num);
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_STATE);

   return result;
} /* USBD_IO_UP_Get_OUT_TP_Params */

void *USBD_IO_UP_Get_IN_TP_Params(
      USBD_Params_XT *usbd,
      uint8_t ep_num)
{
   void *result;

   USBD_ENTER_FUNC(USBD_DBG_IO_STATE);

   result = USBD_MAKE_INVALID_PTR(void);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      result = USBD_IO_UP_IN_GET_TP_PARAMS_PTR(usbd, ep_num);
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_STATE);

   return result;
} /* USBD_IO_UP_Get_IN_TP_Params */

void *USBD_IO_UP_Get_TP_Owner(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir)
{
   void *result;

   USBD_ENTER_FUNC(USBD_DBG_IO_STATE);

   result = USBD_MAKE_INVALID_PTR(void);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      result = (USB_EP_DIRECTION_OUT == dir) ?
           USBD_IO_UP_OUT_GET_TP_OWNER_PTR(usbd, ep_num)
         : USBD_IO_UP_IN_GET_TP_OWNER_PTR(usbd, ep_num);
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_STATE);

   return result;
} /* USBD_IO_UP_Get_TP_Owner */

void *USBD_IO_UP_Get_OUT_TP_Owner(
      USBD_Params_XT *usbd,
      uint8_t ep_num)
{
   void *result;

   USBD_ENTER_FUNC(USBD_DBG_IO_STATE);

   result = USBD_MAKE_INVALID_PTR(void);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      result = USBD_IO_UP_OUT_GET_TP_OWNER_PTR(usbd, ep_num);
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_STATE);

   return result;
} /* USBD_IO_UP_Get_OUT_TP_Owner */

void *USBD_IO_UP_Get_IN_TP_Owner(
      USBD_Params_XT *usbd,
      uint8_t ep_num)
{
   void *result;

   USBD_ENTER_FUNC(USBD_DBG_IO_STATE);

   result = USBD_MAKE_INVALID_PTR(void);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      result = USBD_IO_UP_IN_GET_TP_OWNER_PTR(usbd, ep_num);
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_STATE);

   return result;
} /* USBD_IO_UP_Get_IN_TP_Owner */

USBD_Bool_DT USBD_IO_UP_Trigger_IN(
      USBD_Params_XT *usbd,
      uint8_t ep_num)
{
   const USBD_IO_DOWN_Common_Handlers_XT *common_handlers;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_IO_PROCESSING);

   result = USBD_FALSE;

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      if(USBD_IO_DOWN_CHECK_COMMON_HANDLERS_PTR(usbd)
         && USBD_BOOL_IS_TRUE(USBD_IO_CORE_GET_IN_EP_ACTIVITY_MARKER(usbd, ep_num)))
      {
         common_handlers = USBD_IO_DOWN_GET_COMMON_HANDLERS_PTR(usbd);

         if(USBD_IO_DOWN_CHECK_TRIGGER_HANDLER(common_handlers))
         {
            USBD_INFO_LO_2(USBD_DBG_IO_PROCESSING, "%s on EP: %d", "trigger IN", ep_num);

            result = USBD_IO_CALL_DOWN_TRIGGER(usbd, common_handlers, ep_num, USB_EP_DIRECTION_IN, USBD_FALSE);
         }
         else
         {
            USBD_NOTICE_3(USBD_DBG_IO_INVALID_PARAMS, "%s on EP: %d FAILED! %s!", "trigger IN", ep_num, "no port handler");
         }
      }
      else
      {
         USBD_NOTICE_3(USBD_DBG_IO_INVALID_PARAMS, "%s on EP: %d FAILED! %s!",
            "trigger IN", ep_num, "ep not active/no port handlers");
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_PROCESSING);

   return result;
} /* USBD_IO_UP_Trigger_IN */

USBD_IO_Inout_Data_Size_DT USBD_IO_UP_EP_IN_Get_Buffered_Data_Size(
      USBD_Params_XT *usbd,
      uint8_t ep_num)
{
   const USBD_IO_DOWN_Common_Handlers_XT *common_handlers;
   USBD_IO_Inout_Data_Size_DT result;

   USBD_ENTER_FUNC(USBD_DBG_IO_STATE);

   result = (-1);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      if(USBD_IO_DOWN_CHECK_COMMON_HANDLERS_PTR(usbd)
         && USBD_BOOL_IS_TRUE(USBD_IO_CORE_GET_IN_EP_ACTIVITY_MARKER(usbd, ep_num)))
      {
         common_handlers = USBD_IO_DOWN_GET_COMMON_HANDLERS_PTR(usbd);

         if(USBD_IO_DOWN_CHECK_GET_EP_BUFFERED_HANDLER(common_handlers))
         {
            USBD_DEBUG_HI_2(USBD_DBG_IO_STATE, "%s on EP: %d", "get_ep_buffered_size IN", ep_num);

            result = USBD_IO_CALL_DOWN_GET_EP_BUFFERED(usbd, common_handlers, ep_num);
         }
         else
         {
            USBD_NOTICE_3(USBD_DBG_IO_INVALID_PARAMS,
               "%s on EP: %d FAILED! %s!", "get_ep_buffered_size IN", ep_num, "no port handler");
         }
      }
      else
      {
         USBD_NOTICE_3(USBD_DBG_IO_INVALID_PARAMS, "%s on EP: %d FAILED! %s!",
            "get_ep_buffered_size IN", ep_num, "ep not active/no port handlers");
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_DEBUG_MID_2(USBD_DBG_IO_STATE, "EP: %d waiting_size: %d", ep_num, result);

   USBD_EXIT_FUNC(USBD_DBG_IO_STATE);

   return result;
} /* USBD_IO_UP_EP_IN_Get_Buffered_Data_Size */

USBD_Bool_DT USBD_IO_UP_Trigger_OUT(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USBD_Bool_DT dont_wait)
{
   const USBD_IO_DOWN_Common_Handlers_XT *common_handlers;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_IO_PROCESSING);

   result = USBD_FALSE;

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      if(USBD_IO_DOWN_CHECK_COMMON_HANDLERS_PTR(usbd)
         && USBD_BOOL_IS_TRUE(USBD_IO_CORE_GET_OUT_EP_ACTIVITY_MARKER(usbd, ep_num)))
      {
         common_handlers = USBD_IO_DOWN_GET_COMMON_HANDLERS_PTR(usbd);

         if(USBD_IO_DOWN_CHECK_TRIGGER_HANDLER(common_handlers))
         {
            USBD_INFO_LO_2(USBD_DBG_IO_PROCESSING, "%s on EP: %d", "trigger OUT", ep_num);

            result = USBD_IO_CALL_DOWN_TRIGGER(usbd, common_handlers, ep_num, USB_EP_DIRECTION_OUT, dont_wait);
         }
         else
         {
            USBD_NOTICE_3(USBD_DBG_IO_INVALID_PARAMS, "%s on EP: %d FAILED! %s!", "trigger OUT", ep_num, "no port handler");
         }
      }
      else
      {
         USBD_NOTICE_3(USBD_DBG_IO_INVALID_PARAMS, "%s on EP: %d FAILED! %s!",
            "trigger OUT", ep_num, "ep not active/no port handlers");
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_PROCESSING);

   return result;
} /* USBD_IO_UP_Trigger_OUT */

USBD_IO_Inout_Data_Size_DT USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USBD_Bool_DT left_size)
{
   const USBD_IO_DOWN_Common_Handlers_XT *common_handlers;
   USBD_IO_Inout_Data_Size_DT result;

   USBD_ENTER_FUNC(USBD_DBG_IO_STATE);

   result = (-1);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      if(USBD_IO_DOWN_CHECK_COMMON_HANDLERS_PTR(usbd)
         && USBD_BOOL_IS_TRUE(USBD_IO_CORE_GET_OUT_EP_ACTIVITY_MARKER(usbd, ep_num)))
      {
         common_handlers = USBD_IO_DOWN_GET_COMMON_HANDLERS_PTR(usbd);

         if(USBD_IO_DOWN_CHECK_GET_EP_WAITING_HANDLER(common_handlers))
         {
            USBD_DEBUG_HI_2(USBD_DBG_IO_STATE, "%s on EP: %d", "get_ep_waiting_size OUT", ep_num);

            result = USBD_IO_CALL_DOWN_GET_EP_WAITING(usbd, common_handlers, ep_num, left_size);
         }
         else
         {
            USBD_NOTICE_3(USBD_DBG_IO_INVALID_PARAMS,
               "%s on EP: %d FAILED! %s!", "get_ep_waiting_size OUT", ep_num, "no port handler");
         }
      }
      else
      {
         USBD_NOTICE_3(USBD_DBG_IO_INVALID_PARAMS, "%s on EP: %d FAILED! %s!",
            "get_ep_waiting_size OUT", ep_num, "ep not active/no port handlers");
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_DEBUG_MID_2(USBD_DBG_IO_STATE, "EP: %d waiting_size: %d", ep_num, result);

   USBD_EXIT_FUNC(USBD_DBG_IO_STATE);

   return result;
} /* USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size */

void USBD_IO_UP_Respond_Stall(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir)
{
   const USBD_IO_DOWN_Common_Handlers_XT *common_handlers;
   USBD_Bool_DT activity_marker;

   USBD_ENTER_FUNC(USBD_DBG_IO_PROCESSING);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      if(USB_EP_DIRECTION_OUT == dir)
      {
         activity_marker = USBD_IO_CORE_GET_OUT_EP_ACTIVITY_MARKER(usbd, ep_num);
      }
      else
      {
         activity_marker = USBD_IO_CORE_GET_IN_EP_ACTIVITY_MARKER(usbd, ep_num);
      }

      if(USBD_IO_DOWN_CHECK_COMMON_HANDLERS_PTR(usbd) && USBD_BOOL_IS_TRUE(activity_marker))
      {
         common_handlers = USBD_IO_DOWN_GET_COMMON_HANDLERS_PTR(usbd);

         if(USBD_IO_DOWN_CHECK_STALL_HANDLER(common_handlers))
         {
            USBD_WARN_1(USBD_DBG_IO_PROCESSING, "stall on EP: %d", ep_num);

            USBD_IO_CALL_DOWN_STALL(usbd, common_handlers, ep_num, dir);
         }
         else
         {
            if(USB_EP_DIRECTION_OUT == dir)
            {
               USBD_NOTICE_3(USBD_DBG_IO_INVALID_PARAMS, "%s on EP: %d FAILED! %s!", "stall OUT", ep_num, &"ep not active/no port handler"[14]);
            }
            else
            {
               USBD_NOTICE_3(USBD_DBG_IO_INVALID_PARAMS, "%s on EP: %d FAILED! %s!", "stall IN",  ep_num, &"ep not active/no port handler"[14]);
            }
         }
      }
      else
      {
         if(USB_EP_DIRECTION_OUT == dir)
         {
            USBD_NOTICE_3(USBD_DBG_IO_INVALID_PARAMS, "%s on EP: %d FAILED! %s!",
               "stall OUT", ep_num, "ep not active/no port handler");
         }
         else
         {
            USBD_NOTICE_3(USBD_DBG_IO_INVALID_PARAMS, "%s on EP: %d FAILED! %s!",
               "stall IN", ep_num, "ep not active/no port handler");

         }
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_PROCESSING);
} /* USBD_IO_UP_Respond_Stall */



USBD_IO_UP_DOWN_Transaction_Params_XT *USBD_IO_Get_IN_Transaction_Params(
      USBD_Params_XT *usbd,
      uint8_t ep_num)
{
   USBD_IO_UP_DOWN_Transaction_Params_XT *result = USBD_MAKE_INVALID_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT);

   USBD_ENTER_FUNC(USBD_DBG_IO_PROCESSING);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      result = USBD_IO_GET_IN_TRANSACATION_PARAMS(usbd, ep_num);
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_PROCESSING);

   return result;
} /* USBD_IO_Get_IN_Transaction_Params */

USBD_IO_UP_DOWN_Transaction_Params_XT *USBD_IO_Get_OUT_Transaction_Params(
      USBD_Params_XT *usbd,
      uint8_t ep_num)
{
   USBD_IO_UP_DOWN_Transaction_Params_XT *result = USBD_MAKE_INVALID_PTR(USBD_IO_UP_DOWN_Transaction_Params_XT);

   USBD_ENTER_FUNC(USBD_DBG_IO_PROCESSING);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      result = USBD_IO_GET_OUT_TRANSACATION_PARAMS(usbd, ep_num);
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_PROCESSING);

   return result;
} /* USBD_IO_Get_OUT_Transaction_Params */

void USBD_IO_Abort(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_Bool_DT flush_hw_bufs)
{
   const USBD_IO_DOWN_Common_Handlers_XT *common_handlers;
   USBD_IO_Pipe_Params_XT                *pipe;
   const USBD_IO_UP_EP_Handlers_XT       *ep_handlers;
#ifdef USBD_USE_IOCMD
   const char                            *dir_desc;
#endif

   USBD_ENTER_FUNC(USBD_DBG_IO_PROCESSING);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      if(USB_EP_DIRECTION_OUT == dir)
      {
#ifdef USBD_USE_IOCMD
         dir_desc = "OUT";
#endif
         pipe = USBD_IO_GET_OUT_PIPE_PARAMS(usbd, ep_num);
      }
      else
      {
#ifdef USBD_USE_IOCMD
         dir_desc = "IN";
#endif
         pipe = USBD_IO_GET_IN_PIPE_PARAMS(usbd, ep_num);
      }

      if(USBD_IO_DOWN_CHECK_COMMON_HANDLERS_PTR(usbd) && USBD_BOOL_IS_TRUE(USBD_IO_CORE_GET_PIPE_ACTIVITY_MARKER(pipe)))
      {
         common_handlers = USBD_IO_DOWN_GET_COMMON_HANDLERS_PTR(usbd);

         if(USBD_IO_DOWN_CHECK_ABORT_HANDLER(common_handlers))
         {
            USBD_DEBUG_HI_2(USBD_DBG_IO_PROCESSING, "abort %s on EP: %d", dir_desc, ep_num);

            USBD_IO_CALL_DOWN_ABORT(usbd, common_handlers, ep_num, dir, flush_hw_bufs);
         }
         else
         {
            USBD_NOTICE_4(USBD_DBG_IO_INVALID_PARAMS, "%s %s on EP: %d FAILED! %s!", "abort", dir_desc, ep_num, "no port handler");
         }
      }
      else
      {
         USBD_NOTICE_4(USBD_DBG_IO_INVALID_PARAMS, "%s %s on EP: %d FAILED! %s!",
            "abort", dir_desc, ep_num, "ep not active/no port handlers");
      }

      ep_handlers = pipe->up_link.handlers.handlers;

      if(USBD_CHECK_PTR(const USBD_IO_UP_EP_Handlers_XT, ep_handlers))
      {
         if(USBD_IO_UP_CHECK_ABORT_HANDLER(ep_handlers))
         {
            USBD_DEBUG_HI_2(USBD_DBG_IO_PROCESSING, "up abort on %s EP: %d", dir_desc, ep_num);

            USBD_IO_CALL_UP_ABORT(
               pipe->up_link.data.tp_params,
               ep_handlers,
               USBD_IO_GET_PIPE_TRANSACATION_PARAMS(pipe),
               USBD_IO_UP_PIPE_GET_RECURSIVE_FLAGS(pipe));
         }
         else
         {
            USBD_NOTICE_4(USBD_DBG_IO_INVALID_PARAMS, "%s %s on EP: %d FAILED! %s!", "abort", dir_desc, ep_num, "no 'abort' handler");
         }
      }
      else
      {
         USBD_NOTICE_4(USBD_DBG_IO_INVALID_PARAMS, "%s %s on EP: %d FAILED! %s!",
            "abort", dir_desc, ep_num, "no handlers collection");
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_PROCESSING);
} /* USBD_IO_Abort */



void USBD_IO_DOWN_Set_Common_Handlers(
      USBD_Params_XT *usbd,
      const USBD_IO_DOWN_Common_Handlers_XT *common_handlers)
{
   USBD_ENTER_FUNC(USBD_DBG_IO_STATE);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd))
   {
      USBD_INFO_HI_1(USBD_DBG_IO_STATE, "set down common handlers:%p",
         common_handlers);

      USBD_IO_DOWN_SET_COMMON_HANDLERS_PTR(usbd, common_handlers);
   }
   else
   {
      USBD_WARN_1(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! usbd: %p", usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_STATE);
} /* USBD_IO_DOWN_Set_Common_Handlers */



void USBD_IO_DOWN_Process_IN_Data_Event(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USBD_IO_Inout_Data_Size_DT size,
      USBD_IO_IN_Data_Method_Port_HT mem_cpy)
{
   volatile const USBD_IO_UP_EP_Handlers_XT *up_ep_handlers;

   USBD_ENTER_FUNC(USBD_DBG_IO_PROCESSING);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      up_ep_handlers = USBD_IO_UP_IN_GET_HANDLERS_PTR(usbd, ep_num);

      if(USBD_CHECK_PTR(const USBD_IO_UP_EP_Handlers_XT, up_ep_handlers))
      {
         if(USBD_IO_UP_CHECK_IN_DATA_EVENT_HANDLER(up_ep_handlers))
         {
            USBD_INFO_LO_3(USBD_DBG_IO_PROCESSING, "up data event on %s EP: %d; size = %d", "IN", ep_num, size);

            USBD_IO_CALL_UP_IN_DATA_EVENT(
               USBD_IO_UP_IN_GET_TP_PARAMS_PTR(usbd, ep_num),
               up_ep_handlers,
               USBD_IO_GET_IN_TRANSACATION_PARAMS(usbd, ep_num),
               size,
               mem_cpy,
               USBD_IO_UP_IN_GET_RECURSIVE_FLAGS(usbd, ep_num));
         }
         else
         {
            USBD_NOTICE_3(USBD_DBG_IO_INVALID_PARAMS, "%s on EP: %d FAILED! %s!",
               "up data event IN", ep_num, "no handler");
         }
      }
      else
      {
         USBD_NOTICE_3(USBD_DBG_IO_INVALID_PARAMS, "%s on EP: %d FAILED! %s!",
            "up ack IN", ep_num, "no handlers collection");
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_PROCESSING);
} /* USBD_IO_DOWN_Process_IN_Data_Event */



void USBD_IO_DOWN_Process_OUT_Data_Event(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USBD_IO_Inout_Data_Size_DT size,
      USBD_IO_OUT_Data_Method_Port_HT mem_cpy)
{
   volatile const USBD_IO_UP_EP_Handlers_XT *up_ep_handlers;

   USBD_ENTER_FUNC(USBD_DBG_IO_PROCESSING);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      up_ep_handlers = USBD_IO_UP_OUT_GET_HANDLERS_PTR(usbd, ep_num);

      if(USBD_CHECK_PTR(const USBD_IO_UP_EP_Handlers_XT, up_ep_handlers))
      {
         if(USBD_IO_UP_CHECK_OUT_DATA_EVENT_HANDLER(up_ep_handlers))
         {
            USBD_INFO_LO_3(USBD_DBG_IO_PROCESSING, "up data event on %s EP: %d; size = %d", "OUT", ep_num, size);

            USBD_IO_CALL_UP_OUT_DATA_EVENT(
               USBD_IO_UP_OUT_GET_TP_PARAMS_PTR(usbd, ep_num),
               up_ep_handlers,
               USBD_IO_GET_OUT_TRANSACATION_PARAMS(usbd, ep_num),
               size,
               mem_cpy,
               USBD_IO_UP_OUT_GET_RECURSIVE_FLAGS(usbd, ep_num));
         }
         else
         {
            USBD_NOTICE_3(USBD_DBG_IO_INVALID_PARAMS, "%s on EP: %d FAILED! %s!",
               "up data event OUT", ep_num, "no handler");
         }
      }
      else
      {
         USBD_NOTICE_3(USBD_DBG_IO_INVALID_PARAMS, "%s on EP: %d FAILED! %s!",
            "up ack OUT", ep_num, "no handlers collection");
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_PROCESSING);
} /* USBD_IO_DOWN_Process_OUT_Data_Event */

void USBD_IO_DOWN_Process_Error_CBI(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir)
{
   const USBD_IO_UP_EP_Handlers_XT *ep_handlers;
   USBD_IO_Pipe_Params_XT          *pipe;
#ifdef USBD_USE_IOCMD
   const char                      *dir_desc;
#endif

   USBD_ENTER_FUNC(USBD_DBG_IO_PROCESSING);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      if(USB_EP_DIRECTION_OUT == dir)
      {
#ifdef USBD_USE_IOCMD
         dir_desc = "OUT";
#endif
         pipe = USBD_IO_GET_OUT_PIPE_PARAMS(usbd, ep_num);
      }
      else
      {
#ifdef USBD_USE_IOCMD
         dir_desc = "IN";
#endif
         pipe = USBD_IO_GET_IN_PIPE_PARAMS(usbd, ep_num);
      }

      USBD_DEV_Set_EP_Halt(usbd, ep_num, dir, USBD_TRUE);

      ep_handlers = pipe->up_link.handlers.handlers;

      if(USBD_CHECK_PTR(const USBD_IO_UP_EP_Handlers_XT, ep_handlers))
      {
         if(USBD_IO_UP_CHECK_ERROR_HANDLER(ep_handlers))
         {
            USBD_INFO_LO_2(USBD_DBG_IO_PROCESSING, "up error cbi on %s EP: %d", dir_desc, ep_num);

            USBD_IO_CALL_UP_ERROR(
               pipe->up_link.data.tp_params,
               ep_handlers,
               USBD_IO_GET_PIPE_TRANSACATION_PARAMS(pipe),
               0,
               USBD_IO_UP_PIPE_GET_RECURSIVE_FLAGS(pipe));
         }
         else
         {
            USBD_NOTICE_4(USBD_DBG_IO_INVALID_PARAMS, "%s %s on EP: %d FAILED! %s!",
               "up abort CBI", dir_desc, ep_num, "no handler");
         }
      }
      else
      {
         USBD_NOTICE_4(USBD_DBG_IO_INVALID_PARAMS, "%s %s on EP: %d FAILED! %s!",
            "up abort CBI", dir_desc, ep_num, "no handlers collection");
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_PROCESSING);
} /* USBD_IO_DOWN_Process_Error_CBI */


#if(USBD_FEATURE_PRESENT == USBD_IO_ISOCHRONOUS_TRANSFER_SUPPORTED)
void USBD_IO_DOWN_Process_Error_Iso(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_IO_Inout_Data_Size_DT size)
{
   const USBD_IO_UP_EP_Handlers_XT *ep_handlers;
   USBD_IO_Pipe_Params_XT          *pipe;
#ifdef USBD_USE_IOCMD
   const char                      *dir_desc;
#endif

   USBD_ENTER_FUNC(USBD_DBG_IO_PROCESSING);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && (USBD_CHECK_PTR(USBD_Params_XT, usbd)))
   {
      if(USB_EP_DIRECTION_OUT == dir)
      {
#ifdef USBD_USE_IOCMD
         dir_desc = "OUT";
#endif
         pipe = USBD_IO_GET_OUT_PIPE_PARAMS(usbd, ep_num);
      }
      else
      {
#ifdef USBD_USE_IOCMD
         dir_desc = "IN";
#endif
         pipe = USBD_IO_GET_IN_PIPE_PARAMS(usbd, ep_num);
      }

      ep_handlers = pipe->up_link.handlers.handlers;

      if(USBD_CHECK_PTR(const USBD_IO_UP_EP_Handlers_XT, ep_handlers))
      {
         if(USBD_IO_UP_CHECK_ERROR_HANDLER(ep_handlers))
         {
            USBD_INFO_LO_3(USBD_DBG_IO_PROCESSING, "up error iso on %s EP: %d; size = %d", dir_desc, ep_num, size);

            USBD_IO_CALL_UP_ERROR(
               pipe->up_link.data.tp_params,
               ep_handlers,
               USBD_IO_GET_PIPE_TRANSACATION_PARAMS(pipe),
               size,
               USBD_IO_UP_PIPE_GET_RECURSIVE_FLAGS(pipe));
         }
         else
         {
            USBD_NOTICE_4(USBD_DBG_IO_INVALID_PARAMS, "%s %s on EP: %d FAILED! %s!",
               "up abort ISO", dir_desc, ep_num, "no handler");
         }
      }
      else
      {
         USBD_NOTICE_4(USBD_DBG_IO_INVALID_PARAMS, "%s %s on EP: %d FAILED! %s!",
            "up abort ISO", dir_desc, ep_num, "no handlers collection");
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_IO_INVALID_PARAMS, "function invalid parameters! ep_num: %d, usbd: %p", ep_num, usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_IO_PROCESSING);
} /* USBD_IO_DOWN_Process_Error_Iso */
#endif

#ifdef USBD_IO_POST_IMP_INCLUDE
#include "usbd_io_post_imp.h"
#endif
