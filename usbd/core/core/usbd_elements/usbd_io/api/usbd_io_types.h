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

#ifndef USBD_IO_TYPES_H_
#define USBD_IO_TYPES_H_

#define USBD_IO_PRESENT       1
/**
 * Requied modules:
 * - USBD_DEV
 */


/** @defgroup USBD_IO_TYPES
 * @{
 */

#if(USBD_MAX_NUM_ENDPOINTS > 1)
#define USBD_IO_IIB_TRANSFER_TYPES_DEFAULT_SUPPORT    USBD_FEATURE_PRESENT
#else
#define USBD_IO_IIB_TRANSFER_TYPES_DEFAULT_SUPPORT    USBD_FEATURE_NOT_PRESENT
#endif

#ifndef USBD_IO_INTERRUPT_TRANSFER_SUPPORTED
#define USBD_IO_INTERRUPT_TRANSFER_SUPPORTED          USBD_IO_IIB_TRANSFER_TYPES_DEFAULT_SUPPORT
#endif

#ifndef USBD_IO_ISOCHRONOUS_TRANSFER_SUPPORTED
#define USBD_IO_ISOCHRONOUS_TRANSFER_SUPPORTED        USBD_IO_IIB_TRANSFER_TYPES_DEFAULT_SUPPORT
#endif

#ifndef USBD_IO_BULK_TRANSFER_SUPPORTED
#define USBD_IO_BULK_TRANSFER_SUPPORTED               USBD_IO_IIB_TRANSFER_TYPES_DEFAULT_SUPPORT
#endif

#ifndef USBD_IO_PROVIDE_METHODS_SUPPORTED
#define USBD_IO_PROVIDE_METHODS_SUPPORTED             USBD_FEATURE_PRESENT
#endif

#define USBD_IO_UP_SET_IN_DATA_EVENT_HANDLER(up_ep_handlers, new_handler)        USBD_SET_HANDLER(USBD_IO_UP_IN_Data_Event_HT,   (up_ep_handlers)->data_event.in,    (new_handler))
#define USBD_IO_UP_SET_OUT_DATA_EVENT_HANDLER(up_ep_handlers, new_handler)       USBD_SET_HANDLER(USBD_IO_UP_OUT_Data_Event_HT,  (up_ep_handlers)->data_event.out,   (new_handler))
#define USBD_IO_UP_SET_ERROR_HANDLER(up_ep_handlers, new_handler)                USBD_SET_HANDLER(USBD_IO_UP_Error_HT,           (up_ep_handlers)->error,            (new_handler))
#define USBD_IO_UP_SET_SFIN_HANDLER(up_ep_handlers, new_handler)                 USBD_SET_HANDLER(USBD_IO_UP_Stall_Fin_HT,       (up_ep_handlers)->sfin,             (new_handler))
#define USBD_IO_UP_SET_REINIT_HANDLER(up_ep_handlers, new_handler)               USBD_SET_HANDLER(USBD_IO_UP_Reinit_HT,          (up_ep_handlers)->reinit,           (new_handler))
#define USBD_IO_UP_SET_ABORT_HANDLER(up_ep_handlers, new_handler)                USBD_SET_HANDLER(USBD_IO_UP_Abort_HT,           (up_ep_handlers)->abort,            (new_handler))

#define USBD_IO_DOWN_SET_STALL_HANDLER(down_common_handlers, new_handler)        USBD_SET_HANDLER(USBD_IO_DOWN_Stall_HT,         (down_common_handlers)->stall,      (new_handler))
#define USBD_IO_DOWN_SET_ABORT_HANDLER(down_common_handlers, new_handler)        USBD_SET_HANDLER(USBD_IO_DOWN_Abort_HT,         (down_common_handlers)->abort,      (new_handler))
#define USBD_IO_DOWN_SET_HALT_HANDLER(down_common_handlers, new_handler)         USBD_SET_HANDLER(USBD_IO_DOWN_Halt_Configure_HT,(down_common_handlers)->halt,       (new_handler))
#define USBD_IO_DOWN_SET_CONFIGURE_HANDLER(down_common_handlers, new_handler)    USBD_SET_HANDLER(USBD_IO_DOWN_Halt_Configure_HT,(down_common_handlers)->configure,  (new_handler))

#define USBD_IO_DOWN_FILL_EP_HANDLERS(_first, _second, _finish, _get_buf_size)     {(_first), (_second), (_finish), (_get_buf_size)}
#define USBD_IO_DOWN_FILL_COMMON_HANDLERS(_trig, _get_ep_waiting, _get_ep_buffered, _stall, _abort, _halt, _configure) \
                                                               {(_trig), (_get_ep_waiting), (_get_ep_buffered), (_stall), (_abort), (_halt), (_configure)}



#define USBD_IO_EP_HANDLERS_CHECK_RECURSIVE_CALL_PROTECTION(recursive_flag)      USBD_BOOL_IS_FALSE(recursive_flag)
#define USBD_IO_EP_HANDLERS_SET_RECURSIVE_CALL_PROTECTION(recursive_flag)        recursive_flag = USBD_TRUE
#define USBD_IO_EP_HANDLERS_CLEAR_RECURSIVE_CALL_PROTECTION(recursive_flag)      recursive_flag = USBD_FALSE



#define USBD_IO_GET_IN_TP_PARAMS_PTR(_usbd, _ep_num)                             USBD_GET_PTR(void, (_usbd)->io.in[(_ep_num)].up_link.data.tp_params)
#define USBD_IO_GET_OUT_TP_PARAMS_PTR(_usbd, _ep_num)                            USBD_GET_PTR(void, (_usbd)->io.out[(_ep_num)].up_link.data.tp_params)

#define USBD_IO_GET_IN_TRANSACATION_PARAMS(_usbd, _ep_num)                       (&((_usbd)->io.in[(_ep_num)].transaction))
#define USBD_IO_GET_OUT_TRANSACATION_PARAMS(_usbd, _ep_num)                      (&((_usbd)->io.out[(_ep_num)].transaction))

#define USBD_IO_GET_PIPE_TRANSACATION_PARAMS(pipe)                               (&((pipe)->transaction))

#define USBD_IO_GET_IN_PROVIDE_HANDLER(_transaction)                             ((_transaction)->handlers.in.provide)
#define USBD_IO_GET_IN_MEMCPY_HANDLER(_transaction)                              ((_transaction)->handlers.in.mem_cpy)

#if(USBD_FEATURE_PRESENT == USBD_IO_PROVIDE_METHODS_SUPPORTED)
#define USBD_IO_SET_IN_PROVIDE_HANDLER(_transaction, new_handler)                USBD_SET_HANDLER(USBD_IO_IN_Data_Method_TP_HT, (_transaction)->handlers.in.provide, (new_handler))
#else
#define USBD_IO_SET_IN_PROVIDE_HANDLER(_transaction, new_handler)
#endif
#define USBD_IO_SET_IN_MEMCPY_HANDLER(_transaction, new_handler)                 USBD_SET_HANDLER(USBD_IO_IN_Data_Method_TP_HT, (_transaction)->handlers.in.mem_cpy, (new_handler))

#define USBD_IO_CHECK_IN_PROVIDE_HANDLER(_transaction)                           (USBD_CHECK_HANDLER(USBD_IO_IN_Data_Method_TP_HT, (_transaction)->handlers.in.provide))
#define USBD_IO_CHECK_IN_MEMCPY_HANDLER(_transaction)                            (USBD_CHECK_HANDLER(USBD_IO_IN_Data_Method_TP_HT, (_transaction)->handlers.in.mem_cpy))

#define USBD_IO_CALL_IN_PROVIDE_HANDLER(_usbd, _tp_params, _transaction, _data_method) \
   { \
      if(USBD_IO_EP_HANDLERS_CHECK_RECURSIVE_CALL_PROTECTION((_transaction)->data.in.provide_recursive_call_protection)) \
      { \
         USBD_IO_EP_HANDLERS_SET_RECURSIVE_CALL_PROTECTION((_transaction)->data.in.provide_recursive_call_protection); \
         \
         USBD_CALL_HANDLER(USBD_IO_IN_Data_Method_TP_HT, (_transaction)->handlers.in.provide)( \
               (_tp_params), \
               (_transaction), \
               (_data_method)); \
         \
         USBD_IO_EP_HANDLERS_CLEAR_RECURSIVE_CALL_PROTECTION((_transaction)->data.in.provide_recursive_call_protection); \
      } \
      else \
      { \
         USBD_DEBUG_MID(USBD_DBG_PORT_IO, "recursive protection for provide"); \
      } \
   }


#define USBD_IO_CALL_IN_MEMCPY_HANDLER(_usbd, _tp_params, _transaction, _data_method) \
   { \
      if(USBD_IO_EP_HANDLERS_CHECK_RECURSIVE_CALL_PROTECTION((_transaction)->data.in.mem_cpy_recursive_call_protection)) \
      { \
         USBD_IO_EP_HANDLERS_SET_RECURSIVE_CALL_PROTECTION((_transaction)->data.in.mem_cpy_recursive_call_protection); \
         \
         USBD_CALL_HANDLER(USBD_IO_IN_Data_Method_TP_HT, (_transaction)->handlers.in.mem_cpy)( \
               (_tp_params), \
               (_transaction), \
               (_data_method)); \
         \
         USBD_IO_EP_HANDLERS_CLEAR_RECURSIVE_CALL_PROTECTION((_transaction)->data.in.mem_cpy_recursive_call_protection); \
      } \
      else \
      { \
         USBD_DEBUG_MID(USBD_DBG_PORT_IO, "recursive protection for memcpy"); \
      } \
   }


#define USBD_IO_GET_OUT_TRANSFERRED_SIZE(_transaction)               ((_transaction)->data.out.transferred_size)
#define USBD_IO_GET_OUT_PROVIDE_HANDLER(_transaction)                ((_transaction)->handlers.out.provide)
#define USBD_IO_GET_OUT_MEMCPY_HANDLER(_transaction)                 ((_transaction)->handlers.out.mem_cpy)

#define USBD_IO_SET_OUT_TRANSFERRED_SIZE(_transaction, new_value)    (_transaction)->data.out.transferred_size = (new_value)
#if(USBD_FEATURE_PRESENT == USBD_IO_PROVIDE_METHODS_SUPPORTED)
#define USBD_IO_SET_OUT_PROVIDE_HANDLER(_transaction, new_handler)   USBD_SET_HANDLER(USBD_IO_OUT_Data_Method_TP_HT, (_transaction)->handlers.out.provide, (new_handler))
#else
#define USBD_IO_SET_OUT_PROVIDE_HANDLER(_transaction, new_handler)
#endif
#define USBD_IO_SET_OUT_MEMCPY_HANDLER(_transaction, new_handler)    USBD_SET_HANDLER(USBD_IO_OUT_Data_Method_TP_HT, (_transaction)->handlers.out.mem_cpy, (new_handler))

#define USBD_IO_CHECK_OUT_PROVIDE_HANDLER(_transaction)              (USBD_CHECK_HANDLER(USBD_IO_OUT_Data_Method_TP_HT, (_transaction)->handlers.out.provide))
#define USBD_IO_CHECK_OUT_MEMCPY_HANDLER(_transaction)               (USBD_CHECK_HANDLER(USBD_IO_OUT_Data_Method_TP_HT, (_transaction)->handlers.out.mem_cpy))

#define USBD_IO_CALL_OUT_PROVIDE_HANDLER(_usbd, _tp_params, _transaction, _packet_size, _left_size, _data_method) \
   { \
      if(USBD_IO_EP_HANDLERS_CHECK_RECURSIVE_CALL_PROTECTION((_transaction)->data.out.provide_recursive_call_protection)) \
      { \
         USBD_IO_EP_HANDLERS_SET_RECURSIVE_CALL_PROTECTION((_transaction)->data.out.provide_recursive_call_protection); \
         \
         USBD_CALL_HANDLER(USBD_IO_OUT_Data_Method_TP_HT, (_transaction)->handlers.out.provide)( \
               (_tp_params), \
               (_transaction), \
               (_packet_size), \
               (_left_size), \
               (_data_method)); \
         \
         USBD_IO_EP_HANDLERS_CLEAR_RECURSIVE_CALL_PROTECTION((_transaction)->data.out.provide_recursive_call_protection); \
      } \
      else \
      { \
         USBD_DEBUG_MID(USBD_DBG_PORT_IO, "recursive protection for provide"); \
      } \
   }


#define USBD_IO_CALL_OUT_MEMCPY_HANDLER(_usbd, _tp_params, _transaction, _packet_size, _left_size, _data_method) \
   { \
      if(USBD_IO_EP_HANDLERS_CHECK_RECURSIVE_CALL_PROTECTION((_transaction)->data.out.mem_cpy_recursive_call_protection)) \
      { \
         USBD_IO_EP_HANDLERS_SET_RECURSIVE_CALL_PROTECTION((_transaction)->data.out.mem_cpy_recursive_call_protection); \
         \
         USBD_CALL_HANDLER(USBD_IO_OUT_Data_Method_TP_HT, (_transaction)->handlers.out.mem_cpy)( \
               (_tp_params), \
               (_transaction), \
               (_packet_size), \
               (_left_size), \
               (_data_method)); \
         \
         USBD_IO_EP_HANDLERS_CLEAR_RECURSIVE_CALL_PROTECTION((_transaction)->data.out.mem_cpy_recursive_call_protection); \
      } \
      else \
      { \
         USBD_DEBUG_MID(USBD_DBG_PORT_IO, "recursive protection for memcpy"); \
      } \
   }


typedef int32_t USBD_IO_Inout_Data_Size_DT;



/*
 * UPPER LAYER and PORT interaction parameters
 * these are predeclared types for functions which has to exist in upper layer
 * are responsible for handling data transmission in upper layer
 */

/**
 * \typedef USBD_IO_IN_Data_Method_Port_HT
 * type of 2 functions called by TP and implemented on port layer: mem_cpy and provide.
 * These functions pointers are provided to TP as parameter of function USBD_IO_IN_Data_Method_TP_HT/USBD_IO_OUT_Data_Method_TP_HT.
 * Idea is as below:
 * 1. In structure: USBD_IO_Params_XT.transaction[ep_num].in/USBD_IO_Params_XT.transaction[ep_num].out
 *   there are two pointers: provide and mem_cpy. There are attached handlers
 *   to USBD_IO_IN_Data_Method_TP_HT/USBD_IO_OUT_Data_Method_TP_HT functions implemented in TP layer.
 *
 * 2. Port calls one of these handlers. Parameter data_method is a pointer to USBD_IO_IN_Data_Method_Port_HT function implemented in port.
 *   If TP provide method is called, then data_method is set to port provide function.
 *   If TP mem_cpy method is called, then data_method is set to port mem_cpy function.
 *
 * 3. For IN transactions: mem_cpy TP/port methods are used to copy part of TP data directly to IN buffer. Size of the data
 *   taken by mem_cpy function can be maximum MPS (if endpoint uses only single IN buffer) or maximum two MPS (if endpoint
 *   uses two IN buffers). After calling port mem_cpy piece of data which was taken by port has been copied to IN buffer(s).
 *   Every time data has been send and IN buffer is now free, port calls TP mem_cpy method. That method calls pointer to port
 *   mem_cpy method to load next piece of data to IN buffer.
 *   Provide TP/port methods are used to get from TP pointer to the data. Port saves on its side that pointer and informs
 *   TP that part of that data was taken to be send (usually much more than MPS).
 *   Port sends that data using DMA channel or does it traditonally without calling mem_cpy on each interruption.
 *   After executing these functions data is not yet loaded to hardware buffers. It is done later by DMA or in interruption
 *   without asking TP. When port finishes sending that data, then provide or mem_cpy is again called to obtain next data piece
 *   to be send.
 *
 * Example 1 for IN transaction:
 *
 *          TP                                USBD                                 PORT
 *   send   |                                   |                                   |
 *  ------->*                                   |                                   |
 *          * set mem_cpy pointer in structure  |                                   |
 *          * USBD_IO_Params_XT.transaction[ep_num].in                              |
 *          *                                   |                                   |
 *          *             USBD_IO_UP_Trigger_IN |                                   |
 *          *---------------------------------->*               trigger port method |
 *          |                                   *---------------------------------->*
 *          |                                   |                                   * if IN buffer is free, call TP method
 *          | mem_cpy<USBD_IO_IN_Data_Method_TP_HT>                                 *  - only mem_cpy ptr is valid
 *          *<----------------------------------------------------------------------*
 *          *                                   |                                   |
 *          *                                   |   mem_cpy<USBD_IO_IN_Data_Method_Port_HT>
 *          *---------------------------------------------------------------------->*
 *          |                                   |                           * copy piece of data to IN buffer
 *          |                                   |  ~mem_cpy<USBD_IO_IN_Data_Method_Port_HT>  configure HW for sending data
 *          *<- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
 *          * ~mem_cpy<USBD_IO_IN_Data_Method_TP_HT>                                |
 *          * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ->*
 *          |                                   |             ~trigger port method  *
 *          |            ~USBD_IO_UP_Trigger_IN *<- - - - - - - - - - - - - - - - - *
 *          *<- - - - - - - - - - - - - - - - - *                                   |
 *   <- - - *                                   |                                   |
 *          |                                   |                                   |    / IRQ
 *          |                                   |                                   |   /_
 *          |                                   |                                   |    /
 *          | mem_cpy<USBD_IO_IN_Data_Method_TP_HT>                                 |  |/_
 *          *<----------------------------------------------------------------------*
 *          *                                   |                                   |
 *          *                                   |   mem_cpy<USBD_IO_IN_Data_Method_Port_HT>
 *          *---------------------------------------------------------------------->*
 *          |                                   |                           * copy next piece of data to IN buffer
 *          |                                   |  ~mem_cpy<USBD_IO_IN_Data_Method_Port_HT>  configure HW for sending data
 *          *<- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
 *          * ~mem_cpy<USBD_IO_IN_Data_Method_TP_HT>                                |
 *          * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ->*    ~IRQ
 *          |                                   |                                   *- - ->
 *          |                                   |                                   |
 *          |                                   |                                   |
 *          |                                   |                                   |    / IRQ
 *          |                                   |                                   |   /_
 *          |                                   |                                   |    /
 *          | mem_cpy<USBD_IO_IN_Data_Method_TP_HT>                                 |  |/_
 *          *<----------------------------------------------------------------------*
 *          *                                   |                                   |
 *          *                                   |   mem_cpy<USBD_IO_IN_Data_Method_Port_HT>
 *          *---------------------------------------------------------------------->*
 *          |                                   |                                   * copy last piece of data to IN buffer
 *          |                                   |  ~mem_cpy<USBD_IO_IN_Data_Method_Port_HT>  configure HW for sending data
 *          *<- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
 *          * clear mem_cpy pointer in structure|                                   |
 *          * USBD_IO_Params_XT.transaction[ep_num].in                              |
 *          *                                   |                                   |
 *          * ~mem_cpy<USBD_IO_IN_Data_Method_TP_HT>                                |
 *          * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ->*     ~IRQ
 *          |                                   |                                   *- - ->
 *          |                                   |                                   |
 *          |                                   |                                   |
 *          |                                   |                                   |    / IRQ
 *          |                                   |                                   |   /_
 *          |                                   |                                   |    /
 *          |                                   | USBD_IO_DOWN_Process_Data_Event   |  |/_
 *          |                                   *<----------------------------------|
 *          | data_event<USBD_IO_UP_IN_Data_Event_HT>                               |
 * app_ntfy *<----------------------------------*                                   |
 * <--------*                                   |                                   |
 *  - - - ->* ~data_event<USBD_IO_UP_IN_Data_Event_HT>                              |
 *          * - - - - - - - - - - - - - - - - ->* ~USBD_IO_DOWN_Process_Data_Event  |
 *          |                                   *- - - - - - - - - - - - - - - - - >|     ~IRQ
 *          |                                   |                                   *- - ->
 *
 *
 * Example 2 for IN transaction:
 *
 *          TP                                USBD                                 PORT
 *   send   |                                   |                                   |
 *  ------->*                                   |                                   |
 *          * set provide and mem_cpy pointers in structure                         |
 *          * USBD_IO_Params_XT.transaction[ep_num].in                              |
 *          *                                   |                                   |
 *          *             USBD_IO_UP_Trigger_IN |                                   |
 *          *---------------------------------->*               trigger port method |
 *          |                                   *---------------------------------->*
 *          |                                   |                                   * if IN buffer is free, call TP method
 *          | provide<USBD_IO_IN_Data_Method_TP_HT>                                 * - provide and mem_cpy ptrs are valid
 *          *<----------------------------------------------------------------------*
 *          *                                   |                                   |
 *          *                                   |   provide<USBD_IO_IN_Data_Method_Port_HT>
 *          *---------------------------------------------------------------------->*
 *          |                                   |                                   * save data pointer, take piece of data
 *          |                                   |                                   * with size = data_size - (data_size % mps)
 *          |                                   |  ~provide<USBD_IO_IN_Data_Method_Port_HT>  configure DMA for sending data
 *          *<- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
 *          * clear provide pointer in structure|                                   |
 *          * USBD_IO_Params_XT.transaction[ep_num].in                              |
 *          * ~provide<USBD_IO_IN_Data_Method_TP_HT>                                |
 *          * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ->*
 *          |                                   |             ~trigger port method  *
 *          |            ~USBD_IO_UP_Trigger_IN *<- - - - - - - - - - - - - - - - - *
 *          *<- - - - - - - - - - - - - - - - - *                                   |
 *   <- - - *                                   |                                   |
 *          |                                   |                                   |    / DMA
 *          |                                   |                                   |   /_
 *          |                                   |                                   |    /
 *          |                                   |                                   |  |/_
 *          |                                   |                                   |
 *          |                                   |                                   |    / DMA
 *          |                                   |                                   |   /_
 *          |                                   |                                   |    /
 *          |                                   |                                   |  |/_
 *          |                                   |                                   |
 *          |                                   |                                   |    / DMA
 *          |                                   |                                   |   /_
 *          |                                   |                                   |    /
 *          |                                   |                                   |  |/_
 *          |                                   |                                   |
 *          |                                   |                                   |
 *          |                                   |                                   |
 *          |                                   |                                   |
 *          |                                   |                                   |    / IRQ
 *          |                                   |                                   |   /_
 *          |                                   |                                   |    /
 *          | mem_cpy<USBD_IO_IN_Data_Method_TP_HT>                                 |  |/_
 *          *<----------------------------------------------------------------------*
 *          *                                   |                                   |
 *          *                                   |   mem_cpy<USBD_IO_IN_Data_Method_Port_HT>
 *          *---------------------------------------------------------------------->*
 *          |                                   |                           * copy last piece of data to IN buffer
 *          |                                   |  ~mem_cpy<USBD_IO_IN_Data_Method_Port_HT>  configure HW for sending data
 *          *<- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
 *          * clear mem_cpy pointer in structure|                                   |
 *          * USBD_IO_Params_XT.transaction[ep_num].in                              |
 *          *                                   |                                   |
 *          * ~mem_cpy<USBD_IO_IN_Data_Method_TP_HT>                                |
 *          * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ->*     ~IRQ
 *          |                                   |                                   *- - ->
 *          |                                   |                                   |
 *          |                                   |                                   |
 *          |                                   |                                   |    / IRQ
 *          |                                   |                                   |   /_
 *          |                                   |                                   |    /
 *          |                                   | USBD_IO_DOWN_Process_Data_Event   |  |/_
 *          |                                   *<----------------------------------|
 *          | data_event<USBD_IO_UP_IN_Data_Event_HT>                               |
 * app_ntfy *<----------------------------------*                                   |
 * <--------*                                   |                                   |
 *  - - - ->* ~data_event<USBD_IO_UP_IN_Data_Event_HT>                              |
 *          * - - - - - - - - - - - - - - - - ->* ~USBD_IO_DOWN_Process_Data_Event  |
 *          |                                   *- - - - - - - - - - - - - - - - - >|     ~IRQ
 *          |                                   |                                   *- - ->
 *
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \param data data to pointer to be processed by port
 * \param size size of the data to be processed port.
 * \param is_last_part true if last part of current packet is now requested to port, false otherwise.
 *      This parameter is valid only for "memcpy". For "provide" it is ommited by port.
 * \return size of data which was taken to be processed by port.
 *       For "memcpy" it is equal to mps or 2 x mps, for "provide" it is usually n x mps.
 */
typedef USBD_IO_Inout_Data_Size_DT (*USBD_IO_IN_Data_Method_Port_HT) (
   struct USBD_params_eXtended_Tag *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const void *data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);

/**
 * \typedef USBD_IO_OUT_Data_Method_Port_HT
 * type of 2 functions called by TP and implemented on port layer: mem_cpy and provide.
 * These functions pointers are provided to TP as parameter of function USBD_IO_IN_Data_Method_TP_HT/USBD_IO_OUT_Data_Method_TP_HT.
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \param data data to pointer to be processed by port
 * \param size size of the data to be processed port.
 * \param is_last_part true if last part of current packet is now requested to port, false otherwise.
 *      This parameter is valid only for "memcpy". For "provide" it is ommited by port.
 * \return size of data which was taken to be processed by port.
 *       For "memcpy" it is equal to mps or 2 x mps, for "provide" it is usually n x mps.
 */
typedef USBD_IO_Inout_Data_Size_DT (*USBD_IO_OUT_Data_Method_Port_HT) (
   struct USBD_params_eXtended_Tag *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, void *data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);

struct USBD_IO_UP_DOWN_Transaction_Params_eXtended_Tag;

/**
 * \typedef USBD_IO_IN_Data_Method_TP_HT
 * type of function called directly by port for IN/OUT transfer.
 *
 * \param tp_params pointer to structure which contains transport_protocol parameters
 * \param transaction_params pointer to USBD_IO_UP_DOWN_Transaction_Params_XT structure of processed endpoint
 * \param max_size size of the data informed to TP. For IN transaction it can be maximum size of linear buffer
 *             which can be processed ("provide" function) or MPS ("memcpy" function).
 *             For "provide" size returned by "data_method" can be smaller than size requested as its parameter.
 *             For "memcpy" size returned by "data_method"is equal to its size parameter only if that size is not bigger
 *             than max_size.
 * \param data_method USBD_IO_IN_Data_Method_Port_HT type handler which is normally provided by port as
 *      parameter of function USBD_IO_UP_DOWN_Transaction_Params_XT.mem_cpy. Using this handler
 *      received already piece of data can be immediately processed (to free HW buffer) before calling START_OUT.
 */
typedef void (*USBD_IO_IN_Data_Method_TP_HT) (
   void *tp_params,
   struct USBD_IO_UP_DOWN_Transaction_Params_eXtended_Tag *transaction_params,
   USBD_IO_IN_Data_Method_Port_HT data_method);

/**
 * \typedef USBD_IO_OUT_Data_Method_TP_HT
 * type of function called directly by port for IN/OUT transfer.
 *
 * \param tp_params pointer to structure which contains transport_protocol parameters
 * \param transaction_params pointer to USBD_IO_UP_DOWN_Transaction_Params_XT structure of processed endpoint
 * \param max_size size of the data informed to TP. For IN transaction it can be maximum size of linear buffer
 *             which can be processed ("provide" function) or MPS ("memcpy" function).
 *             For "provide" size returned by "data_method" can be smaller than size requested as its parameter.
 *             For "memcpy" size returned by "data_method"is equal to its size parameter only if that size is not bigger
 *             than max_size.
 * \param data_method USBD_IO_OUT_Data_Method_Port_HT type handler which is normally provided by port as
 *      parameter of function USBD_IO_UP_DOWN_Transaction_Params_XT.mem_cpy. Using this handler
 *      received already piece of data can be immediately processed (to free HW buffer) before calling START_OUT.
 */
typedef void (*USBD_IO_OUT_Data_Method_TP_HT) (
   void *tp_params,
   struct USBD_IO_UP_DOWN_Transaction_Params_eXtended_Tag *transaction_params,
   USBD_IO_Inout_Data_Size_DT packet_size,
   USBD_IO_Inout_Data_Size_DT left_size,
   USBD_IO_OUT_Data_Method_Port_HT data_method);



/*
 * UPPER LAYER HANDLERS
 * these are predeclared types for functions which has to exist in upper layer
 * are responsible for handling data transmission in upper layer
 */

/**
 * \typedef USBD_IO_UP_IN_Data_Event_HT
 * type of function called as event for IN transfer. It is used to indicate
 * IN data packet has been sent.
 *
 * \param tp_params pointer to structure which contains transport_protocol parameters
 * \param transaction_params pointer to USBD_IO_UP_DOWN_Transaction_Params_XT structure of processed endpoint
 * \param size size of the data informed to TP. Size indicates how many bytes are still in IN buffer.
 *      If this parameter is set to -1 then all data has been sent and IN buffer is empty.
 * \param mem_cpy USBD_IO_IN_Data_Method_Port_HT type handler which is normally provided by port as
 *      parameter of function USBD_IO_UP_DOWN_Transaction_Params_XT.mem_cpy. Using this handler
 *      received already piece of data can be immediately processed (to free HW buffer) before calling START_OUT.
 */
typedef void (*USBD_IO_UP_IN_Data_Event_HT) (
   void *tp_params,
   struct USBD_IO_UP_DOWN_Transaction_Params_eXtended_Tag *transaction_params,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_IN_Data_Method_Port_HT mem_cpy);

/**
 * \typedef USBD_IO_UP_OUT_Data_Event_HT
 * type of function called as event for OUT transfer. It is used to indicate new data was received but
 * no mem_cpy/provide handlers are available to processs the data.
 *
 * \param tp_params pointer to structure which contains transport_protocol parameters
 * \param transaction_params pointer to USBD_IO_UP_DOWN_Transaction_Params_XT structure of processed endpoint
 * \param size size of the data informed to TP. Size indicates how big data is waiting in OUT bufffer to be processed.
 * \param mem_cpy USBD_IO_OUT_Data_Method_Port_HT type handler which is normally provided by port as
 *      parameter of function USBD_IO_UP_DOWN_Transaction_Params_XT.mem_cpy. Using this handler
 *      received already piece of data can be immediately processed (to free HW buffer) before calling START_OUT.
 */
typedef void (*USBD_IO_UP_OUT_Data_Event_HT) (
   void *tp_params,
   struct USBD_IO_UP_DOWN_Transaction_Params_eXtended_Tag *transaction_params,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_OUT_Data_Method_Port_HT mem_cpy);

/**
 * \typedef USBD_IO_UP_Error_HT
 * type of function called when some error occurs
 *
 * \param tp_params pointer to structure which contains transport_protocol parameters
 * \param size size of data
 */
typedef void (*USBD_IO_UP_Error_HT)          (
   void *tp_params, struct USBD_IO_UP_DOWN_Transaction_Params_eXtended_Tag *transaction_params, USBD_IO_Inout_Data_Size_DT size);
/**
 * \typedef USBD_IO_UP_Stall_Fin_HT
 * type of function called after STALL packet has been sent
 *
 * \param tp_params pointer to structure which contains transport_protocol parameters
 */
typedef void (*USBD_IO_UP_Stall_Fin_HT)      (
   void *tp_params, struct USBD_IO_UP_DOWN_Transaction_Params_eXtended_Tag *transaction_params);
/**
 * \typedef USBD_IO_UP_Reinit_HT
 * type of function called by core when upper layer should be reinitialized
 * for example after configuration change, ep_halt, ep_unhalt
 *
 * \param tp_params pointer to structure which contains transport_protocol parameters
 * \param tp_owner pointer to data that upper layer checks if it initialized TP
 * \param active if USBD_TRUE, then upper layer shall be deinitialized and initialized,
 *     if USBD_FALSE then upper layer TP is turned off (other TP will be connected to EP) and
 *     upper layer shall only deinitialize itself
 */
typedef void (*USBD_IO_UP_Reinit_HT)         (
   void *tp_params, void *tp_owner, struct USBD_IO_UP_DOWN_Transaction_Params_eXtended_Tag *transaction_params, USBD_Bool_DT active);
/**
 * \typedef USBD_IO_UP_Abort_HT
 * type of function called by core when all traffic should be aborted
 *
 * \param tp_params pointer to structure which contains transport_protocol parameters
 */
typedef void (*USBD_IO_UP_Abort_HT)          (
   void *tp_params, struct USBD_IO_UP_DOWN_Transaction_Params_eXtended_Tag *transaction_params);



/*
 * PORT HANDLERS
 * these are predeclared types for functions which have to be implemented in port
 * are responsible for managing basc operations on endpoint
 */

/**
 * \typedef USBD_IO_DOWN_Trigger_HT
 * type of port function called by core
 * - triggers transmitting next packets through specific endpoint.
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \param dont_wait if USBD_TRUE then receiving next packets shall be allowed even "wait" condition is met,
 *      if USBD_FALSE then endpoint will lock receiving next packets if "wait" condtion is met. Not used for IN transactions.
 * \return USBD_TRUE if triggering was possible, USBD_FALSE otherwise
 */
typedef USBD_Bool_DT (*USBD_IO_DOWN_Trigger_HT) (struct USBD_params_eXtended_Tag *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, USBD_Bool_DT dont_wait);

/**
 * \typedef USBD_IO_DOWN_Get_EP_OUT_Waiting_Size_HT
 * type of port function called by core
 * - Gets OUT endpoint waiting data size
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param left_size if USBD_TRUE then returns size of data waiting in buffer, otherwise whole packet size
 * \return size of waiting data, if any data is waiting in bufffer, -1 otherwise
 */
typedef USBD_IO_Inout_Data_Size_DT (*USBD_IO_DOWN_Get_EP_OUT_Waiting_Size_HT)(struct USBD_params_eXtended_Tag *usbd, uint8_t ep_num, USBD_Bool_DT left_size);

/**
 * \typedef USBD_IO_DOWN_Get_EP_IN_Buffered_Size_HT
 * type of port function called by core
 * - Gets size of the data which is currently procedded by IN endpoint buffers
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 */
typedef USBD_IO_Inout_Data_Size_DT (*USBD_IO_DOWN_Get_EP_IN_Buffered_Size_HT)(struct USBD_params_eXtended_Tag *usbd, uint8_t ep_num);

/**
 * \typedef USBD_IO_DOWN_Stall_HT
 * type of port function called by core to send STALL message on specific endpoint
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 */
typedef void (*USBD_IO_DOWN_Stall_HT)    (struct USBD_params_eXtended_Tag *usbd, uint8_t ep_num, USB_EP_Direction_ET dir);

/**
 * \typedef USBD_IO_DOWN_Abort_HT
 * type of port function called by core to abort transfer on specific endpoint
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \param flush_hw_bufs if USBD_TRUE then hardware buffers will be cleared by port, USBD_FALSE otherwise.
 */
typedef void (*USBD_IO_DOWN_Abort_HT)    (struct USBD_params_eXtended_Tag *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, USBD_Bool_DT flush_hw_bufs);
/**
 * \typedef USBD_IO_DOWN_Halt_Configure_HT
 * type of port function called by core when endpoint have to be configured/unconfigured
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \param state USBD_TRUE when endpoint shall be configured, USBD_FALSE when unconfigured
 */
typedef void (*USBD_IO_DOWN_Halt_Configure_HT)  (
   struct USBD_params_eXtended_Tag *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const USB_Endpoint_Desc_DT *ep_desc, USBD_Bool_DT state);

/**
 * \struct USBD_IO_UP_EP_Handlers_XT
 * Stores handlers to methods called by ISBD_IO when upper layer callback
 * is needed to be called to process data. This structure have to exist in upper layer
 * of IO software (transport protocol). Handlers contained by this structure are only
 * executed by USBD_IO layer but never modified. Upper layer (TP) can modify it
 * if needs.
 */
typedef struct USBD_IO_UP_EP_Handlers_eXtended_Tag
{
   union
   {
      USBD_IO_UP_OUT_Data_Event_HT  out;
      USBD_IO_UP_IN_Data_Event_HT   in;
   }data_event;
   USBD_IO_UP_Error_HT              error;
   USBD_IO_UP_Stall_Fin_HT          sfin;
   USBD_IO_UP_Reinit_HT             reinit;
   USBD_IO_UP_Abort_HT              abort;
}USBD_IO_UP_EP_Handlers_XT;

/**
 * \struct USBD_IO_UP_EP_Handlers_RCP_XT
 * Extended version of structure USBD_IO_UP_EP_Handlers_XT.
 * Stores additionally variables which protects recursive calling upper layer handlers.
 * This structure is used only internally.
 */
typedef struct USBD_IO_UP_EP_Handlers_RCP_eXtended_Tag
{
   const USBD_IO_UP_EP_Handlers_XT *handlers;
   struct
   {
      USBD_Bool_DT data_event;
      USBD_Bool_DT error;
      USBD_Bool_DT sfin;
      USBD_Bool_DT reinit;
      USBD_Bool_DT abort;
   }recursive_call_protection;
}USBD_IO_UP_EP_Handlers_RCP_XT;

/**
 * \struct USBD_IO_DOWN_Common_Handlers_XT
 * Stores handlers to methods called by ISBD_IO when access to any
 * endpoint is needed. This structure have to exist in low level layer
 * of IO software (port). Handlers contained by this structure are only
 * executed by USBD_IO layer but never modified. Port layer can modify it
 * if needs.
 */
typedef struct USBD_IO_DOWN_Common_Handlers_eXtended_Tag
{
   /** \brief
     * This function initializes transaction on specific endpoint.
     */
   USBD_IO_DOWN_Trigger_HT                   trigger;
   USBD_IO_DOWN_Get_EP_OUT_Waiting_Size_HT   get_ep_out_waiting_size;
   USBD_IO_DOWN_Get_EP_IN_Buffered_Size_HT   get_ep_in_buffered_size;
   USBD_IO_DOWN_Abort_HT                     abort;
   USBD_IO_DOWN_Stall_HT                     stall;
   USBD_IO_DOWN_Halt_Configure_HT            halt;
#if(USBD_MAX_NUM_ENDPOINTS > 1)
   USBD_IO_DOWN_Halt_Configure_HT            configure;
#endif
}USBD_IO_DOWN_Common_Handlers_XT;

/**
 * \struct USBD_IO_UP_DOWN_Transaction_Params_XT
 * Stores handlers to functions used for data transfer between DOWN layer and UPPER layer.
 * This structure is used only internally.
 */
typedef struct USBD_IO_UP_DOWN_Transaction_Params_eXtended_Tag
{
   union
   {
      struct
      {
#if(USBD_FEATURE_PRESENT == USBD_IO_PROVIDE_METHODS_SUPPORTED)
         USBD_IO_OUT_Data_Method_TP_HT       provide;
#endif
         USBD_IO_OUT_Data_Method_TP_HT       mem_cpy;
      }out;
      struct
      {
#if(USBD_FEATURE_PRESENT == USBD_IO_PROVIDE_METHODS_SUPPORTED)
         USBD_IO_IN_Data_Method_TP_HT        provide;
#endif
         USBD_IO_IN_Data_Method_TP_HT        mem_cpy;
      }in;
   }handlers;
   union
   {
      struct
      {
         /* used only for DMA transactions, otherwise always equal to 0 */
         USBD_IO_Inout_Data_Size_DT          transferred_size;
#if(USBD_FEATURE_PRESENT == USBD_IO_PROVIDE_METHODS_SUPPORTED)
         USBD_Bool_DT                        provide_recursive_call_protection;
#endif
         USBD_Bool_DT                        mem_cpy_recursive_call_protection;
      }out;
      struct
      {
#if(USBD_FEATURE_PRESENT == USBD_IO_PROVIDE_METHODS_SUPPORTED)
         USBD_Bool_DT                        provide_recursive_call_protection;
#endif
         USBD_Bool_DT                        mem_cpy_recursive_call_protection;
      }in;
   }data;
}USBD_IO_UP_DOWN_Transaction_Params_XT;

typedef struct USBD_IO_Pipe_Params_eXtendedTag
{
   /** \brief up link data */
   struct
   {
      USBD_IO_UP_EP_Handlers_RCP_XT handlers;
      struct
      {
         void *tp_params;
         void *tp_owner;
      }data;
   }up_link;
   USBD_IO_UP_DOWN_Transaction_Params_XT transaction;
   struct
   {
      struct
      {
         USBD_Bool_DT ep_active;
         USBD_Bool_DT tp_params_locked;
      }data;
   }core;
}USBD_IO_Pipe_Params_XT;

/**
 * \struct USBD_IO_Params_XT
 * IO part of USBD
 */
typedef struct USBD_IO_Params_eXtended_Tag
{
   /** \brief port (down link) data */
   struct
   {
      /** \brief common handlers for all endpoints */
      struct
      {
         const USBD_IO_DOWN_Common_Handlers_XT *handlers;
      }common;
   }down_link;
   USBD_IO_Pipe_Params_XT  in[USBD_MAX_NUM_ENDPOINTS];
   USBD_IO_Pipe_Params_XT out[USBD_MAX_NUM_ENDPOINTS];
}USBD_IO_Params_XT;

/** @} */ /* USBD_IO_TYPES */


#endif
