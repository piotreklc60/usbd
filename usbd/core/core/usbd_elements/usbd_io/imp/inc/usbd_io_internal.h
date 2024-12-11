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

#ifndef USBD_IO_INTERNAL_H_
#define USBD_IO_INTERNAL_H_


#ifndef USBD_H_
#include "usbd.h"
#endif


#define USBD_IO_UP_IN_CHECK_HANDLERS_PTR(usbd, _ep_num)           USBD_CHECK_PTR(USBD_IO_UP_EP_Handlers_XT,       (usbd)->io.up_link.ep[(_ep_num)].in.handlers.handlers)
#define USBD_IO_UP_IN_CHECK_TP_PARAMS_PTR(usbd, _ep_num)          USBD_CHECK_PTR(void,                            (usbd)->io.up_link.ep[(_ep_num)].in.data.tp_params)
#define USBD_IO_UP_IN_CHECK_TP_OWNER_PTR(usbd, _ep_num)           USBD_CHECK_PTR(void,                            (usbd)->io.up_link.ep[(_ep_num)].in.data.tp_owner)
#define USBD_IO_UP_OUT_CHECK_HANDLERS_PTR(usbd, _ep_num)          USBD_CHECK_PTR(USBD_IO_UP_EP_Handlers_XT,       (usbd)->io.up_link.ep[(_ep_num)].out.handlers.handlers)
#define USBD_IO_UP_OUT_CHECK_TP_PARAMS_PTR(usbd, _ep_num)         USBD_CHECK_PTR(void,                            (usbd)->io.up_link.ep[(_ep_num)].out.data.tp_params)
#define USBD_IO_UP_OUT_CHECK_TP_OWNER_PTR(usbd, _ep_num)          USBD_CHECK_PTR(void,                            (usbd)->io.up_link.ep[(_ep_num)].out.data.tp_owner)

#define USBD_IO_UP_CHECK_OUT_DATA_EVENT_HANDLER(ep_handlers)      USBD_CHECK_HANDLER(USBD_IO_UP_OUT_Data_Event_HT,(ep_handlers)->data_event.out)
#define USBD_IO_UP_CHECK_IN_DATA_EVENT_HANDLER(ep_handlers)       USBD_CHECK_HANDLER(USBD_IO_UP_IN_Data_Event_HT, (ep_handlers)->data_event.in)
#define USBD_IO_UP_CHECK_ERROR_HANDLER(ep_handlers)               USBD_CHECK_HANDLER(USBD_IO_UP_Error_HT,         (ep_handlers)->error)
#define USBD_IO_UP_CHECK_STALL_FIN_HANDLER(ep_handlers)           USBD_CHECK_HANDLER(USBD_IO_UP_Stall_Fin_HT,     (ep_handlers)->sfin)
#define USBD_IO_UP_CHECK_REINIT_HANDLER(ep_handlers)              USBD_CHECK_HANDLER(USBD_IO_UP_Reinit_HT,        (ep_handlers)->reinit)
#define USBD_IO_UP_CHECK_ABORT_HANDLER(ep_handlers)               USBD_CHECK_HANDLER(USBD_IO_UP_Abort_HT,         (ep_handlers)->abort)




#define USBD_IO_UP_IN_GET_HANDLERS_PTR(usbd, _ep_num)             USBD_GET_PTR(USBD_IO_UP_EP_Handlers_XT,         (usbd)->io.up_link.ep[(_ep_num)].in.handlers.handlers)
#define USBD_IO_UP_IN_GET_RECURSIVE_FLAGS(usbd, _ep_num)          ((usbd)->io.up_link.ep[(_ep_num)].in.handlers.recursive_call_protection)
#define USBD_IO_UP_IN_GET_TP_PARAMS_PTR(usbd, _ep_num)            USBD_GET_PTR(void,                              (usbd)->io.up_link.ep[(_ep_num)].in.data.tp_params)
#define USBD_IO_UP_IN_GET_TP_OWNER_PTR(usbd, _ep_num)             USBD_GET_PTR(void,                              (usbd)->io.up_link.ep[(_ep_num)].in.data.tp_owner)
#define USBD_IO_UP_OUT_GET_HANDLERS_PTR(usbd, _ep_num)            USBD_GET_PTR(USBD_IO_UP_EP_Handlers_XT,         (usbd)->io.up_link.ep[(_ep_num)].out.handlers.handlers)
#define USBD_IO_UP_OUT_GET_RECURSIVE_FLAGS(usbd, _ep_num)         ((usbd)->io.up_link.ep[(_ep_num)].out.handlers.recursive_call_protection)
#define USBD_IO_UP_OUT_GET_TP_PARAMS_PTR(usbd, _ep_num)           USBD_GET_PTR(void,                              (usbd)->io.up_link.ep[(_ep_num)].out.data.tp_params)
#define USBD_IO_UP_OUT_GET_TP_OWNER_PTR(usbd, _ep_num)            USBD_GET_PTR(void,                              (usbd)->io.up_link.ep[(_ep_num)].out.data.tp_owner)

#define USBD_IO_UP_GET_IN_DATA_EVENT_HANDLER(ep_handlers)         USBD_GET_HANDLER(USBD_IO_UP_IN_Data_Event_HT,   (ep_handlers)->data_event)
#define USBD_IO_UP_GET_OUT_DATA_EVENT_HANDLER(ep_handlers)        USBD_GET_HANDLER(USBD_IO_UP_OUT_Data_Event_HT,  (ep_handlers)->data_event)
#define USBD_IO_UP_GET_ERROR_HANDLER(ep_handlers)                 USBD_GET_HANDLER(USBD_IO_UP_Error_HT,           (ep_handlers)->error)
#define USBD_IO_UP_GET_STALL_FIN_HANDLER(ep_handlers)             USBD_GET_HANDLER(USBD_IO_UP_Stall_Fin_HT,       (ep_handlers)->sfin)
#define USBD_IO_UP_GET_REINIT_HANDLER(ep_handlers)                USBD_GET_HANDLER(USBD_IO_UP_Reinit_HT,          (ep_handlers)->reinit)
#define USBD_IO_UP_GET_ABORT_HANDLER(ep_handlers)                 USBD_GET_HANDLER(USBD_IO_UP_Abort_HT,           (ep_handlers)->abort)




#define USBD_IO_UP_IN_SET_HANDLERS_PTR(usbd, _ep_num, new_value)  USBD_SET_PTR(USBD_IO_UP_EP_Handlers_XT,         (usbd)->io.up_link.ep[(_ep_num)].in.handlers.handlers,  (new_value))
#define USBD_IO_UP_IN_SET_TP_PARAMS_PTR(usbd, _ep_num, new_value) USBD_SET_PTR(void,                              (usbd)->io.up_link.ep[(_ep_num)].in.data.tp_params, (new_value))
#define USBD_IO_UP_IN_SET_TP_OWNER_PTR(usbd, _ep_num, new_value)  USBD_SET_PTR(void,                              (usbd)->io.up_link.ep[(_ep_num)].in.data.tp_owner, (new_value))
#define USBD_IO_UP_OUT_SET_HANDLERS_PTR(usbd, _ep_num, new_value) USBD_SET_PTR(USBD_IO_UP_EP_Handlers_XT,         (usbd)->io.up_link.ep[(_ep_num)].out.handlers.handlers, (new_value))
#define USBD_IO_UP_OUT_SET_TP_PARAMS_PTR(usbd,_ep_num, new_value) USBD_SET_PTR(void,                              (usbd)->io.up_link.ep[(_ep_num)].out.data.tp_params, (new_value))
#define USBD_IO_UP_OUT_SET_TP_OWNER_PTR(usbd, _ep_num, new_value) USBD_SET_PTR(void,                              (usbd)->io.up_link.ep[(_ep_num)].out.data.tp_owner, (new_value))



#define USBD_IO_IN_GET_LOCK_TP_STATE(usbd, _ep_num)               ((usbd)->io.core.ep[(_ep_num)].in.data.tp_params_locked)
#define USBD_IO_IN_LOCK_TP(usbd, _ep_num)                         (usbd)->io.core.ep[(_ep_num)].in.data.tp_params_locked = USBD_TRUE
#define USBD_IO_IN_UNLOCK_TP(usbd, _ep_num)                       (usbd)->io.core.ep[(_ep_num)].in.data.tp_params_locked = USBD_FALSE
#define USBD_IO_OUT_GET_LOCK_TP_STATE(usbd, _ep_num)              ((usbd)->io.core.ep[(_ep_num)].out.data.tp_params_locked)
#define USBD_IO_OUT_LOCK_TP(usbd, _ep_num)                        (usbd)->io.core.ep[(_ep_num)].out.data.tp_params_locked = USBD_TRUE
#define USBD_IO_OUT_UNLOCK_TP(usbd, _ep_num)                      (usbd)->io.core.ep[(_ep_num)].out.data.tp_params_locked = USBD_FALSE



#define USBD_IO_CALL_UP_IN_DATA_EVENT(tp_params, ep_handlers, trn_params, size, pmemcpy, up_recursive_flags) \
   { \
      if(USBD_IO_EP_HANDLERS_CHECK_RECURSIVE_CALL_PROTECTION(up_recursive_flags.data_event)) \
      { \
         USBD_IO_EP_HANDLERS_SET_RECURSIVE_CALL_PROTECTION(up_recursive_flags.data_event); \
         \
         USBD_CALL_HANDLER(USBD_IO_UP_IN_Data_Event_HT, (ep_handlers)->data_event.in)( \
               (tp_params), \
               (trn_params), \
               (size), \
               (pmemcpy)); \
         \
         USBD_IO_EP_HANDLERS_CLEAR_RECURSIVE_CALL_PROTECTION(up_recursive_flags.data_event); \
      } \
   }

#define USBD_IO_CALL_UP_OUT_DATA_EVENT(tp_params, ep_handlers, trn_params, size, pmemcpy, up_recursive_flags) \
   { \
      if(USBD_IO_EP_HANDLERS_CHECK_RECURSIVE_CALL_PROTECTION(up_recursive_flags.data_event)) \
      { \
         USBD_IO_EP_HANDLERS_SET_RECURSIVE_CALL_PROTECTION(up_recursive_flags.data_event); \
         \
         USBD_CALL_HANDLER(USBD_IO_UP_OUT_Data_Event_HT, (ep_handlers)->data_event.out)( \
               (tp_params), \
               (trn_params), \
               (size), \
               (pmemcpy)); \
         \
         USBD_IO_EP_HANDLERS_CLEAR_RECURSIVE_CALL_PROTECTION(up_recursive_flags.data_event); \
      } \
   }

#define USBD_IO_CALL_UP_ERROR(tp_params, ep_handlers, trn_params, size, up_recursive_flags) \
   { \
      if(USBD_IO_EP_HANDLERS_CHECK_RECURSIVE_CALL_PROTECTION(up_recursive_flags.error)) \
      { \
         USBD_IO_EP_HANDLERS_SET_RECURSIVE_CALL_PROTECTION(up_recursive_flags.error); \
         \
         USBD_CALL_HANDLER(USBD_IO_UP_Error_HT, (ep_handlers)->error)( \
               (tp_params), \
               (trn_params), \
               (size)); \
         \
         USBD_IO_EP_HANDLERS_CLEAR_RECURSIVE_CALL_PROTECTION(up_recursive_flags.error); \
      } \
   }

#define USBD_IO_CALL_UP_STALL_FIN(tp_params, ep_handlers, trn_params, up_recursive_flags) \
   { \
      if(USBD_IO_EP_HANDLERS_CHECK_RECURSIVE_CALL_PROTECTION(up_recursive_flags.sfin)) \
      { \
         USBD_IO_EP_HANDLERS_SET_RECURSIVE_CALL_PROTECTION(up_recursive_flags.sfin); \
         \
         USBD_CALL_HANDLER(USBD_IO_UP_Stall_Fin_HT, (ep_handlers)->sfin)( \
               (tp_params), \
               (trn_params)); \
         USBD_IO_UP_SET_SFIN_HANDLER((ep_handlers), USBD_MAKE_INVALID_HANDLER(USBD_IO_UP_Stall_Fin_HT)); \
         \
         USBD_IO_EP_HANDLERS_CLEAR_RECURSIVE_CALL_PROTECTION(up_recursive_flags.sfin); \
      } \
   }

#define USBD_IO_CALL_UP_REINIT(tp_params, tp_owner, ep_handlers, trn_params, active, up_recursive_flags) \
   { \
      if(USBD_IO_EP_HANDLERS_CHECK_RECURSIVE_CALL_PROTECTION(up_recursive_flags.reinit)) \
      { \
         USBD_IO_EP_HANDLERS_SET_RECURSIVE_CALL_PROTECTION(up_recursive_flags.reinit); \
         \
         USBD_CALL_HANDLER(USBD_IO_UP_Reinit_HT, (ep_handlers)->reinit)( \
               (tp_params), \
               (tp_owner), \
               (trn_params), \
               (active)); \
         \
         USBD_IO_EP_HANDLERS_CLEAR_RECURSIVE_CALL_PROTECTION(up_recursive_flags.reinit); \
      } \
   }

#define USBD_IO_CALL_UP_ABORT(tp_params, ep_handlers, trn_params, up_recursive_flags) \
   { \
      if(USBD_IO_EP_HANDLERS_CHECK_RECURSIVE_CALL_PROTECTION(up_recursive_flags.abort)) \
      { \
         USBD_IO_EP_HANDLERS_SET_RECURSIVE_CALL_PROTECTION(up_recursive_flags.abort); \
         \
         USBD_CALL_HANDLER(USBD_IO_UP_Abort_HT, (ep_handlers)->abort)( \
               (tp_params), \
               (trn_params)); \
         \
         USBD_IO_EP_HANDLERS_CLEAR_RECURSIVE_CALL_PROTECTION(up_recursive_flags.abort); \
      } \
   }



#define USBD_IO_DOWN_CHECK_COMMON_HANDLERS_PTR(usbd)                 USBD_CHECK_PTR(USBD_IO_DOWN_Common_Handlers_XT,             (usbd)->io.down_link.common.handlers)

#define USBD_IO_DOWN_CHECK_TRIGGER_HANDLER(_common_handlers)         USBD_CHECK_HANDLER(USBD_IO_DOWN_Trigger_HT,                 (_common_handlers)->trigger)
#define USBD_IO_DOWN_CHECK_GET_EP_WAITING_HANDLER(_common_handlers)  USBD_CHECK_HANDLER(USBD_IO_DOWN_Get_EP_OUT_Waiting_Size_HT, (_common_handlers)->get_ep_out_waiting_size)
#define USBD_IO_DOWN_CHECK_GET_EP_BUFFERED_HANDLER(_common_handlers) USBD_CHECK_HANDLER(USBD_IO_DOWN_Get_EP_IN_Buffered_Size_HT, (_common_handlers)->get_ep_in_buffered_size)
#define USBD_IO_DOWN_CHECK_ABORT_HANDLER(_common_handlers)           USBD_CHECK_HANDLER(USBD_IO_DOWN_Abort_HT,                   (_common_handlers)->abort)
#define USBD_IO_DOWN_CHECK_STALL_HANDLER(_common_handlers)           USBD_CHECK_HANDLER(USBD_IO_DOWN_Stall_HT,                   (_common_handlers)->stall)
#define USBD_IO_DOWN_CHECK_HALT_HANDLER(_common_handlers)            USBD_CHECK_HANDLER(USBD_IO_DOWN_Halt_Configure_HT,          (_common_handlers)->halt)
#define USBD_IO_DOWN_CHECK_CONFIGURE_HANDLER(_common_handlers)       USBD_CHECK_HANDLER(USBD_IO_DOWN_Halt_Configure_HT,          (_common_handlers)->configure)



#define USBD_IO_DOWN_GET_COMMON_HANDLERS_PTR(usbd)                    USBD_GET_PTR(USBD_IO_DOWN_Common_Handlers_XT,              (usbd)->io.down_link.common.handlers)

#define USBD_IO_DOWN_GET_TRIGGER_HANDLER(_common_handlers)           USBD_GET_HANDLER(USBD_IO_DOWN_Trigger_HT,                   (_common_handlers)->trigger)
#define USBD_IO_DOWN_GET_GET_EP_WAITING_HANDLER(_common_handlers)    USBD_GET_HANDLER(USBD_IO_DOWN_Get_EP_OUT_Waiting_Size_HT,   (_common_handlers)->get_ep_out_waiting_size)
#define USBD_IO_DOWN_GET_ABORT_HANDLER(_common_handlers)             USBD_GET_HANDLER(USBD_IO_DOWN_Abort_HT,                     (_common_handlers)->abort)
#define USBD_IO_DOWN_GET_STALL_HANDLER(_common_handlers)             USBD_GET_HANDLER(USBD_IO_DOWN_Stall_HT,                     (_common_handlers)->stall)
#define USBD_IO_DOWN_GET_HALT_HANDLER(_common_handlers)              USBD_GET_HANDLER(USBD_IO_DOWN_Halt_Configure_HT,            (_common_handlers)->halt)
#define USBD_IO_DOWN_GET_CONFIGURE_HANDLER(_common_handlers)         USBD_GET_HANDLER(USBD_IO_DOWN_Halt_Configure_HT,            (_common_handlers)->configure)



#define USBD_IO_DOWN_SET_COMMON_HANDLERS_PTR(usbd, new_value)        USBD_SET_PTR(USBD_IO_DOWN_Common_Handlers_XT,               (usbd)->io.down_link.common.handlers,         (new_value))



#define USBD_IO_CALL_DOWN_TRIGGER(_usbd, _common_handlers, _ep_num, _direction, _dont_wait) \
   USBD_CALL_HANDLER(USBD_IO_DOWN_Trigger_HT,                  (_common_handlers)->trigger)( \
         (_usbd), \
         (_ep_num), \
         (_direction), \
         (_dont_wait))

#define USBD_IO_CALL_DOWN_GET_EP_WAITING(_usbd, _common_handlers, _ep_num, _left_size) \
   USBD_CALL_HANDLER(USBD_IO_DOWN_Get_EP_OUT_Waiting_Size_HT,  (_common_handlers)->get_ep_out_waiting_size)( \
         (_usbd), \
         (_ep_num), \
         (_left_size))

#define USBD_IO_CALL_DOWN_GET_EP_BUFFERED(_usbd, _common_handlers, _ep_num) \
   USBD_CALL_HANDLER(USBD_IO_DOWN_Get_EP_IN_Buffered_Size_HT,  (_common_handlers)->get_ep_in_buffered_size)( \
         (_usbd), \
         (_ep_num))

#define USBD_IO_CALL_DOWN_ABORT(_usbd, _common_handlers, _ep_num, _direction, flush_hw_bufs) \
   USBD_CALL_HANDLER(USBD_IO_DOWN_Abort_HT,                    (_common_handlers)->abort)( \
         (_usbd), \
         (_ep_num), \
         (_direction), \
         (flush_hw_bufs))

#define USBD_IO_CALL_DOWN_STALL(_usbd, _common_handlers, _ep_num, _direction) \
   USBD_CALL_HANDLER(USBD_IO_DOWN_Stall_HT,                    (_common_handlers)->stall)( \
         (_usbd), \
         (_ep_num), \
         (_direction))

#define USBD_IO_CALL_DOWN_HALT(_usbd, _common_handlers, _ep_num, _direction, _state) \
   USBD_CALL_HANDLER(USBD_IO_DOWN_Halt_Configure_HT,           (_common_handlers)->halt)( \
         (_usbd), \
         (_ep_num), \
         (_direction), \
         USBD_DEV_Get_EP_Desc((_usbd), (_ep_num), (_direction)), \
         (_state))

#define USBD_IO_CALL_DOWN_CONFIGURE(_usbd, _common_handlers, _ep_num, _direction, _state) \
   USBD_CALL_HANDLER(USBD_IO_DOWN_Halt_Configure_HT,           (_common_handlers)->configure)( \
         (_usbd), \
         (_ep_num), \
         (_direction), \
         USBD_DEV_Get_EP_Desc((_usbd), (_ep_num), (_direction)), \
         (_state))



#define USBD_IO_CORE_GET_IN_EP_ACTIVITY_MARKER(usbd, _ep_num)     ((usbd)->io.core.ep[(_ep_num)].in.data.ep_active)
#define USBD_IO_CORE_GET_OUT_EP_ACTIVITY_MARKER(usbd, _ep_num)    ((usbd)->io.core.ep[(_ep_num)].out.data.ep_active)
#define USBD_IO_CORE_SET_IN_EP_ACTIVITY_MARKER(usbd, _ep_num)     (usbd)->io.core.ep[(_ep_num)].in.data.ep_active  = USBD_TRUE
#define USBD_IO_CORE_SET_OUT_EP_ACTIVITY_MARKER(usbd, _ep_num)    (usbd)->io.core.ep[(_ep_num)].out.data.ep_active = USBD_TRUE
#define USBD_IO_CORE_CLEAR_IN_EP_ACTIVITY_MARKER(usbd, _ep_num)   (usbd)->io.core.ep[(_ep_num)].in.data.ep_active  = USBD_FALSE
#define USBD_IO_CORE_CLEAR_OUT_EP_ACTIVITY_MARKER(usbd, _ep_num)  (usbd)->io.core.ep[(_ep_num)].out.data.ep_active = USBD_FALSE



/**
 * Enables and configures specified endpoint. This function
 * is called by DEV layer when endpoint is configured on USB request or on
 * change interface request
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 */
void USBD_IO_EP_Enable_And_Configure(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir);

/**
 * Disables specified endpoint. This function
 * is called by DEV layer when endpoint is deconfigured
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \param force_tp_remove if set to USBD_TRUE then TP is removed even is locked, otherwise TP is not removed
 * \param force_hw_disabling if USBD_TRUE then port function "configure" will be called even status of endpoint is "disabled"
 */
void USBD_IO_EP_Disable(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_Bool_DT force_tp_remove,
      USBD_Bool_DT force_hw_disabling);

/**
 * Performs HALT condition for specified endpoint to IO module and disables/enables endpoint.
 * This function shall be called from DEV module only;
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \param state USBD_TRUE when HALT condition shall be set, USBD_FALSE otherwise.
 */
void USBD_IO_Perform_Halt(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_Bool_DT state);

#endif
