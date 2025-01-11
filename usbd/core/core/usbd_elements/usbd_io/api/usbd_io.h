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

#ifndef USBD_IO_H_
#define USBD_IO_H_

/** @defgroup USBD_IO
 * @{
 */

/**
 * Locks permanently tp_params. It means, that there will not be possible
 * to change tp_params for specified endpoint. It can be cleared on USBD_Init or in one of situations described below.
 * If TP is not locked then it is completely removed on following events:
 * - USBD activated (USBD_DEV_Activate was called)
 * - USBD deactivated (USBD_DEV_Deactivate was called)
 * - USB power ON
 * - USB power OFF
 * - USB reset
 * - configuration set
 * - interface set.
 * It should to be installed only on USBD_EVENT_REASON_CONFIGURED or USBD_EVENT_REASON_INTERFACE_SET event.
 * If TP is locked then it is completely removed only on following events:
 * - USBD activated (USBD_DEV_Activate was called)
 * - USBD deactivated (USBD_DEV_Deactivate was called)
 * It should to be installed and locked after USBD activating (after USBD_DEV_Activate was called).
 *
 * There is no possible to install any TP for endpoint 0 (Default Control Pipe) when USBD_REQUEST module is used.
 * In this situation even if any TP was installed and locked just after USBD activating, it is removed when USB reset
 * is received on bus. Only IOTP_EVENT-type TP is installed automatically on USB reset for EP0 IN & OUT.
 *
 * Called by upper level layer
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 */
void USBD_IO_UP_Lock_TP_Params(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir);

/**
 * Checks if TP is locked
 *
 * Called by upper level layer
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \return USBD_TRUE if tp_params is locked, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_IO_UP_Is_TP_Locked(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir);

/**
 * Checks if endpoint is active
 *
 * Called by upper level layer
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \return USBD_TRUE if endpoint is active, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_IO_UP_Is_EP_Active(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir);

/**
 * Sets transport protocol all parameters
 *
 * Called by upper level layer
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \param handlers pointer to structure with handlers to methods used for IO irqs handling
 * \param tp_params pointer to structure which contains transport protocol data
 * \param tp_owner pointer to tp owner specified data - can be INVALID. Usually
 *      TP stores here some its internal pointer used to identify if is connected
 *      to this endpoint
 */
void USBD_IO_UP_Set_TP(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      const USBD_IO_UP_EP_Handlers_XT *handlers,
      void *tp_params,
      void *tp_owner);

/**
 * Gets transport protocol parameters
 *
 * Called by upper level layer
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \return tp_params - pointer to structure which contains transport protocol data
 */
void *USBD_IO_UP_Get_TP_Params(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir);

/**
 * Gets transport protocol parameters for OUT pipe
 *
 * Called by upper level layer
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \return tp_params - pointer to structure which contains transport protocol data
 */
void *USBD_IO_UP_Get_OUT_TP_Params(
      USBD_Params_XT *usbd,
      uint8_t ep_num);

/**
 * Gets transport protocol parameters for IN pipe
 *
 * Called by upper level layer
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \return tp_params - pointer to structure which contains transport protocol data
 */
void *USBD_IO_UP_Get_IN_TP_Params(
      USBD_Params_XT *usbd,
      uint8_t ep_num);

/**
 * Gets tp owner parameters
 *
 * Called by upper level layer
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \return tp_owner - pointer to owner specified data - can be INVALID
 */
void *USBD_IO_UP_Get_TP_Owner(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir);

/**
 * Gets tp owner parameters for OUT pipe
 *
 * Called by upper level layer
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \return tp_owner - pointer to owner specified data - can be INVALID
 */
void *USBD_IO_UP_Get_OUT_TP_Owner(
      USBD_Params_XT *usbd,
      uint8_t ep_num);

/**
 * Gets tp owner parameters for IN pipe
 *
 * Called by upper level layer
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \return tp_owner - pointer to owner specified data - can be INVALID
 */
void *USBD_IO_UP_Get_IN_TP_Owner(
      USBD_Params_XT *usbd,
      uint8_t ep_num);



/**
 * For IN:
 *
 * Starts IN transaction over specified IN endpoint.
 * Also used to send STATUS to HOST (STATUS is DATA IN frame with no data - zero length data).
 * Direction parameter is not needed because "IN" transaction can be performed only on IN endpoint type.
 * This function only triggers sending IN data over specified endpoint.
 * Endpoint calls "provide" or "mem_cpy" handler from transaction params to obtain linear data to be send ("provide") / copy piece
 * of user data (not bigger than MPS) directly to IN buffer by upper layer ("mem_cpy").
 *
 * Called by upper level layer
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. USB_EP_DIRECTION_IN shall be provided.
 * \param dont_wait NOT USED
 * \return USBD_TRUE if triggering was possible, USBD_FALSE otherwise
 *
 *
 * For OUT:
 * Triggers receiving next OUT packets through specific endpoint.
 * Direction parameter is not needed because "read" can be performed only on OUT endpoint.
 *
 * Called by upper level layer
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. USB_EP_DIRECTION_OUT shall be provided.
 * \param dont_wait if USBD_TRUE then receiving next packets shall be allowed even "wait" condition is met,
 *      if USBD_FALSE then endpoint will lock receiving next packets if "wait" condtion is met. Not used for IN transactions.
 *      Wait condition occurs mandatorily, when:
 *       - memcpy port method was called with parameter is_last_part set to false - port will wait for call
 *         of memcpy method with parameter is_last_part set to true or call of USBD_IO_UP_Trigger_OUT.
 *      Wait condition occurs if "dont_wait" parameter was set to "USBD_FALSE" during calling USBD_IO_UP_Trigger_OUT && next:
 *       - port received data but neighter "provide" nor "mem_cpy" iotp handlers are valid. In this case port will call
 *         data event. If during that event neighter "mem_cpy" nor USBD_IO_UP_Trigger_OUT was executed
 *         then wait condition is met.
 *       - port received 0-length OUT packet through CONTROL endpoint during IN transaction (termination packet
 *         specified by USB standard)
 *       - port finished transaction requested by "provide" method but processed size is smaller
 *         than it was originally ackwnoledged by port
 *      Wait condition not occurs when:
 *       - port finished transaction requested by "provide" method and whole size was processed - endpoint is unlocked
 *         for receiving next packets and data event is called.
 * \return USBD_TRUE if triggering was possible, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_IO_UP_Trigger_INOUT(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_Bool_DT dont_wait);

/**
 * Gets size of data which has been loaded to tx buffers but has not yet been send to HOST.
 * Returns buffered data size or -1 if there is no IN data in tx buffers.
 *
 * Called by upper level layer
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 */
USBD_IO_Inout_Data_Size_DT USBD_IO_UP_EP_IN_Get_Buffered_Data_Size(
      USBD_Params_XT *usbd,
      uint8_t ep_num);

/**
 * Gets endpoint OUT waiting data size or -1 if there is no OUT packet waiting.
 *
 * Called by upper level layer
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param left_size if USBD_TRUE then returns size of data waiting in buffer, otherwise whole packet size
 * \return requested size which can be:
 *   - if USBD_TRUE == left_size:
 *     - if all data was read then (-1) - this works also when 0-length packet was received: if memcpy port method
 *      was not called, then 0 is returned, if memcpy port method was already called then (-1) is returned.
 *     - not all data was read - returns size of data still waiting in OUT buffer
 *   - if USBD_FALSE == left_size:
 *     - if endpoint is still waiting (memcpy port method was not called yet
 *      or was called with parameter is_last_part set to USBD_FALSE) then size of whole packet is returned
 *     - if endpoint is not waiting (memcpy port method was already called with is_last_part set to USBD_TRUE
 *      or USBD_IO_UP_Trigger_OUT was called) then (-1) is returned
 */
USBD_IO_Inout_Data_Size_DT USBD_IO_UP_EP_OUT_Get_Waiting_Data_Size(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USBD_Bool_DT left_size);



/**
 * Sends STALL message over specified endpoint. After successfully
 * processed STALL endpoint's data transfer is automatically
 * finalyzed by USBD port.
 *
 * Called by upper level layer
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 */
void USBD_IO_UP_Respond_Stall(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir);



/**
 * Gets transaction parameters for specific IN endpoint
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 */
USBD_IO_UP_DOWN_Transaction_Params_XT *USBD_IO_Get_IN_Transaction_Params(
      USBD_Params_XT *usbd,
      uint8_t ep_num);

/**
 * Gets transaction parameters for specific OUT endpoint
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 */
USBD_IO_UP_DOWN_Transaction_Params_XT *USBD_IO_Get_OUT_Transaction_Params(
      USBD_Params_XT *usbd,
      uint8_t ep_num);

/**
 * Aborts transfer on specific endpoint by calling down and upper layer "abort" callbacks.
 * This function doesn't call any down layer functions so if needed then it must be processed by "abort" callback.
 *
 * Called only by low level layer (port) or by usbd layer
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \param flush_hw_bufs if USBD_TRUE then hardware buffers will be cleared by port, USBD_FALSE otherwise.
 */
void USBD_IO_Abort(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_Bool_DT flush_hw_bufs);



/**
 * Sets collection of all down layer (port) common callbacks used by single endpoint.
 *
 * Called only by low level layer (port)
 *
 * \param usbd pointer to usb device
 * \param common_handlers structure which contains handlers for endpoint's methods
 */
void USBD_IO_DOWN_Set_Common_Handlers(
      USBD_Params_XT *usbd,
      const USBD_IO_DOWN_Common_Handlers_XT *common_handlers);

/**
 * Processes IN data event from PORT to TP layer.
 * For IN transactions this function shall be called when all buffers has been freed (al data sent to HOST).
 *
 * Called only by low level layer (port)
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param size size of data informed to TP. For IN transactions size indicates how many bytes are still in IN buffer.
 *      If this parameter is set to -1 then all data has been sent and IN buffer is empty. For OUT transactions size indicates
 *      how big data is waiting in OUT bufffer to be processed.
 * \param mem_cpy
 */
void USBD_IO_DOWN_Process_IN_Data_Event(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USBD_IO_Inout_Data_Size_DT size,
      USBD_IO_IN_Data_Method_Port_HT mem_cpy);

/**
 * Processes OUT data event from PORT to TP layer.
 * For IN transactions this function shall be called when all buffers has been freed (al data sent to HOST).
 *
 * Called only by low level layer (port)
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param size size of data informed to TP. For IN transactions size indicates how many bytes are still in IN buffer.
 *      If this parameter is set to -1 then all data has been sent and IN buffer is empty. For OUT transactions size indicates
 *      how big data is waiting in OUT bufffer to be processed.
 * \param mem_cpy
 */
void USBD_IO_DOWN_Process_OUT_Data_Event(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USBD_IO_Inout_Data_Size_DT size,
      USBD_IO_OUT_Data_Method_Port_HT mem_cpy);

/**
 * Processes DATA error for CONTROL, BULK and INTERRUPT transfer. Has to be called by down layer (port)
 * to inform upper layer that transfer error occured.
 * In this situation endpoint goes to HALT state, upper layer transfer is aborted and "abort" callback
 * is called.
 *
 * Called only by low level layer (port)
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 */
void USBD_IO_DOWN_Process_Error_CBI(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir);


#if(USBD_FEATURE_PRESENT == USBD_IO_ISOCHRONOUS_TRANSFER_SUPPORTED)
/**
 * Processes DATA error for ISOCHRONOUS IN transfer. Has to be called by down layer (port)
 * to inform upper layer that previous packet has not been sent successfully.
 * "USBD_IO_Process_IN" has to be normally called after this function to perform next packet
 * and upper layer has to decide if transfer shall be continued or not.
 *
 * Called only by low level layer (port)
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \param size size of corrupted packet or zero
 */
void USBD_IO_DOWN_Process_Error_Iso(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_IO_Inout_Data_Size_DT size);
#endif

/** @} */ /* USBD_IO */


#endif
