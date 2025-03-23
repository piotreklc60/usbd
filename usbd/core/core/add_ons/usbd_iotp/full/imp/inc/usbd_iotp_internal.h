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

#ifndef USBD_IOTP_INTERNAL_H_
#define USBD_IOTP_INTERNAL_H_



#define USBD_IOTP_CHECK_READY_HANDLER(tp)                USBD_CHECK_HANDLER(USBD_IOTP_Callback_HT, (tp)->up_link.handlers.ready)
#define USBD_IOTP_CHECK_BUF_EMPTY_HANDLER(tp)            USBD_CHECK_HANDLER(USBD_IOTP_Callback_HT, (tp)->up_link.handlers.buf_empty)
#define USBD_IOTP_CHECK_ERROR_HANDLER(tp)                USBD_CHECK_HANDLER(USBD_IOTP_Callback_HT, (tp)->up_link.handlers.error)
#define USBD_IOTP_CHECK_ABORT_HANDLER(tp)                USBD_CHECK_HANDLER(USBD_IOTP_Callback_HT, (tp)->up_link.handlers.abort)


#define USBD_IOTP_GET_READY_HANDLER(tp)                  USBD_GET_HANDLER(USBD_IOTP_Callback_HT,   (tp)->up_link.handlers.ready)
#define USBD_IOTP_GET_BUF_EMPTY_HANDLER(tp)              USBD_GET_HANDLER(USBD_IOTP_Callback_HT,   (tp)->up_link.handlers.buf_empty)
#define USBD_IOTP_GET_ERROR_HANDLER(tp)                  USBD_GET_HANDLER(USBD_IOTP_Callback_HT,   (tp)->up_link.handlers.error)
#define USBD_IOTP_GET_ABORT_HANDLER(tp)                  USBD_GET_HANDLER(USBD_IOTP_Callback_HT,   (tp)->up_link.handlers.abort)
#define USBD_IOTP_GET_VENDOR_DATA_PTR(tp)                (&((tp)->up_link.data.vendor_data[0]))



#define USBD_IOTP_SET_USBD_INSIDE_TP(tp, new_value)      USBD_SET_PTR(void, (tp)->core.pipe_params.data.usbd, (new_value))
#define USBD_IOTP_SET_USBDC_INSIDE_TP(tp, new_value)     USBD_SET_PTR(void, (tp)->core.pipe_params.data.usbdc, (new_value))
#define USBD_IOTP_SET_EP_NUM_INSIDE_TP(tp, new_value)    (tp)->core.pipe_params.data.ep_num = (new_value)
#define USBD_IOTP_SET_EP_DIR_INSIDE_TP(tp, new_value)    (tp)->core.pipe_params.data.dir = (new_value)


#define USBD_IOTP_GET_INVOKE_PARAMS(tp)                  (tp)->core.pipe_params.invoke.invoke
#define USBD_IOTP_GET_INVOKE_REQ(tp)                     (tp)->core.pipe_params.invoke.req


#define USBD_IOTP_SET_READY_HANDLER(tp, new_value) \
            USBD_SET_HANDLER( \
               USBD_IOTP_Callback_HT, \
               (tp)->up_link.handlers.ready, \
               (new_value))

#define USBD_IOTP_SET_BUF_EMPTY_HANDLER(tp, new_value) \
            USBD_SET_HANDLER( \
               USBD_IOTP_Callback_HT, \
               (tp)->up_link.handlers.buf_empty, \
               (new_value))

#define USBD_IOTP_SET_ERROR_HANDLER(tp, new_value) \
            USBD_SET_HANDLER( \
               USBD_IOTP_Callback_HT, \
               (tp)->up_link.handlers.error, \
               (new_value))

#define USBD_IOTP_SET_ABORT_HANDLER(tp, new_value) \
            USBD_SET_HANDLER( \
               USBD_IOTP_Callback_HT, \
               (tp)->up_link.handlers.abort, \
               (new_value))


#define USBD_IOTP_CALL_READY(usbd, ep_num, dir, tp, size) \
            USBD_CALL_HANDLER(USBD_IOTP_Callback_HT, (tp)->up_link.handlers.ready)( \
               ((USBD_IOTP_Params_XT*)(tp)), \
               (dir), \
               (size))

#define USBD_IOTP_CALL_BUF_EMPTY(usbd, ep_num, dir, tp, size) \
            USBD_CALL_HANDLER(USBD_IOTP_Callback_HT, (tp)->up_link.handlers.buf_empty)( \
               ((USBD_IOTP_Params_XT*)(tp)), \
               (dir), \
               (size))

#define USBD_IOTP_CHECK_AND_CALL_ERROR(usbd, ep_num, dir, tp, size) \
            USBD_CHECK_AND_CALL_HANDLER(USBD_IOTP_Callback_HT, (tp)->up_link.handlers.error)( \
               ((USBD_IOTP_Params_XT*)(tp)), \
               (dir), \
               (size))

#define USBD_IOTP_CHECK_AND_CALL_ABORT(usbd, ep_num, dir, tp, size) \
            USBD_CHECK_AND_CALL_HANDLER(USBD_IOTP_Callback_HT, (tp)->up_link.handlers.abort)( \
               ((USBD_IOTP_Params_XT*)(tp)), \
               (dir), \
               (size))



typedef struct USBD_IOTP_Vendor_Memcpy_eXtended_Tag
{
   USBD_IOTP_Params_XT                *tp;
   union
   {
      USBD_IO_IN_Data_Method_Port_HT   in;
      USBD_IO_OUT_Data_Method_Port_HT  out;
   }data_method;
   USBD_IO_Inout_Data_Size_DT          out_left_data_size;
   USBD_Bool_DT                        is_last_part;
}USBD_IOTP_Vendor_Memcpy_XT;



/**
 * Instals IN and OUT TP structures and handlers in USB device for 0 endpont (default control pipe)
 * Pipe must be unlocked before calling it!!!
 *
 * \param usbd pointer to USBD_Params_XT structure which is a pointer to USB device
 * \param tp_params_in pointer to IN USBD_IOTP_Params_XT structure which will be installed
 * \param tp_params_out pointer to OUT USBD_IOTP_Params_XT structure which will be installed
 * \return USBD_TRUE when TP installed successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_IOTP_Install_Default_Control_Pipe(
      USBD_Params_XT *usbd,
      USBD_IOTP_Params_XT *tp_in,
      USBD_IOTP_Params_XT *tp_out);



#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_COMPLEX_BUFFERS)
/**
 * Helper function used by complex buffer types as vendor memcpy function for IN transactions.
 *
 * \param params parameters provided to the function containing such information like source ptr, destination, etc.
 * \return size of copied data
 */
Buff_Size_DT USBD_IOTP_Ring_Vendor_Memcpy_In(const Buff_Memcpy_Params_XT *params);

/**
 * Helper function used by complex buffer types as vendor memcpy function for OUT transactions.
 *
 * \param params parameters provided to the function containing such information like source ptr, destination, etc.
 * \return size of copied data
 */
Buff_Size_DT USBD_IOTP_Ring_Vendor_Memcpy_Out(const Buff_Memcpy_Params_XT *params);

#if(USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS == USBD_FEATURE_PRESENT)
/**
 * Function to initialize RING_INFINITE transfer type
 *
 * \param tp pointer to transport protocol structure to be initialized
 * \param ring pointer to ring buffer to be connected
 * \param result USBD_TRUE if succeeded, USBD_FALSE otherwise.
 * \return always USBD_TRUE as this is callback for INVOKE mechanism where always USBD_TRUE is expected.
 */
USBD_Bool_DT USBD_IOTP_Ring_Start_Transfer_Infinitely_Invoked(
   USBD_IOTP_Params_XT *tp, Buff_Ring_XT *ring, USBD_Bool_DT *result);

/**
 * Function to trigger IN transaction on the pipe.
 *
 * \param usbd pointer to USBD_Params_XT structure
 * \param ep_num number of the endpoint for which we do the trigger
 * \return always USBD_TRUE as this is callback for INVOKE mechanism where always USBD_TRUE is expected.
 */
USBD_Bool_DT USBD_IOTP_Trigger_In_Invoked(USBD_Params_XT *usbd, uint8_t ep_num);

/**
 * Function to trigger OUT transaction on the pipe.
 *
 * \param usbd pointer to USBD_Params_XT structure
 * \param ep_num number of the endpoint for which we do the trigger
 * \return always USBD_TRUE as this is callback for INVOKE mechanism where always USBD_TRUE is expected.
 */
USBD_Bool_DT USBD_IOTP_Trigger_Out_Invoked(USBD_Params_XT *usbd, uint8_t ep_num);

/**
 * Function to clear the ring buffer and connect all functions needed for it.
 *
 * \param tp pointer transport protocol structure for which hbuffer shall be cleared.
 * \param transaction pointer to transaction parameters
 * \param set_valid_handlers if USBD_TRUE then valid communication callbacks shall be connected, for USBD_FALSE invalid will be used.
 */
void USBD_IOTP_Ring_Clear_Buff(
   USBD_IOTP_Params_XT *tp, USBD_IO_UP_DOWN_Transaction_Params_XT *transaction, USBD_Bool_DT set_valid_handlers);

/**
 * Callback from USBD_IO layer called when OUT data arrives and no PROVIDE neighter MEMCPY handlers are connected to the transaction.
 *
 * \param tp pointer transport protocol structure attached to the pipe during initialization using USBD_IO_UP_Set_TP.
 * \param transaction pointer to the transaction structure which c ontain pointers to fast functions like PROVIDE and MEMCPY
 * \param size size of received data
 * \param mem_cpy pointer to port MEMCPY function called to process incomming data
 */
void USBD_IOTP_Ring_Io_Evdata_Out(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_OUT_Data_Method_Port_HT mem_cpy);

/**
 * Callback from USBD_IO layer called when IN data has been sent and no PROVIDE neighter MEMCPY handlers are connected to the transaction.
 *
 * \param tp pointer transport protocol structure attached to the pipe during initialization using USBD_IO_UP_Set_TP.
 * \param transaction pointer to the transaction structure which c ontain pointers to fast functions like PROVIDE and MEMCPY
 * \param size size of data possible to be transmited
 * \param mem_cpy pointer to port MEMCPY function called to initialize next IN data packet
 */
void USBD_IOTP_Ring_Io_Evdata_In(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_IN_Data_Method_Port_HT mem_cpy);
#endif
#endif

/**
 * Callback from USBD_IO layer called when OUT data arrives and no PROVIDE neighter MEMCPY handlers are connected to the transaction.
 *
 * \param tp pointer transport protocol structure attached to the pipe during initialization using USBD_IO_UP_Set_TP.
 * \param transaction pointer to the transaction structure which c ontain pointers to fast functions like PROVIDE and MEMCPY
 * \param size size of received data
 * \param mem_cpy pointer to port MEMCPY function called to process incomming data
 */
void USBD_IOTP_Io_Evdata_Out(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_OUT_Data_Method_Port_HT mem_cpy);

/**
 * Callback from USBD_IO layer called when IN data has been sent and no PROVIDE neighter MEMCPY handlers are connected to the transaction.
 *
 * \param tp pointer transport protocol structure attached to the pipe during initialization using USBD_IO_UP_Set_TP.
 * \param transaction pointer to the transaction structure which c ontain pointers to fast functions like PROVIDE and MEMCPY
 * \param size size of data possible to be transmited
 * \param mem_cpy pointer to port MEMCPY function called to initialize next IN data packet
 */
void USBD_IOTP_Io_Evdata_In(
   void *tp_params,
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_IN_Data_Method_Port_HT mem_cpy);

extern USBD_Atomic_Bool_DT USBD_IOTP_refresh_trigger;
extern const uint8_t USBD_IOTP_not_ring_infinite_owner[1];
#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS)
extern const uint8_t USBD_IOTP_ring_infinite_owner[1];
#endif

#if(USBD_FEATURE_PRESENT == USBD_IOTP_SUPPORT_RING_INFINITIVE_BUFFERS)
/**
 * Helper function to check TP_OWNER field
 *
 * \param tp_owner pointer to the data to be checked
 * \return USBD_TRUE if tp_owner pointer is valid for this IOTP, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_IOTP_check_both_tp_owners(void *tp_owner);
#else
#define USBD_IOTP_check_both_tp_owners(_tp_owner)  USBD_COMPARE_PTRS(void, _tp_owner, void, USBD_IOTP_not_ring_infinite_owner)
#endif


#endif
