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

#ifndef USBD_IOTP_H_
#define USBD_IOTP_H_

/**
 * Initializes TP structure and its basic parameters
 *
 * \param usbd pointer to USBD_Params_XT structure which is a pointer to USB device
 * \param usbdc pointer to USBDC_Params_XT structure which is a pointer to USB device configuration
 * \param ep_num number of endpoint for which TP is installed
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \param tp pointer to USBD_IOTP_Params_XT structure which will be initialized
 */
void USBD_IOTP_Init(
      USBD_Params_XT *usbd,
      USBDC_Params_XT *usbdc,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_IOTP_Params_XT *tp);

/**
 * Instals TP structure and handlers in device.
 * This function shall be called on CONFIGURED event by TP user (upper layer, for example class library)
 * after active configuration was set by the HOST.
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure which will be installed
 * \return USBD_TRUE when TP installed successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_IOTP_Install(
      USBD_IOTP_Params_XT *tp);



/**
 * Gets pointer to USBD_Params_XT structure(usbd).
 * usbd structure contains USB device operating parameters. TP is upper layer for USBD.
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \return pointer to "usbd" when "tp" is valid, USBD_MAKE_INVALID_PTR(USBD_Params_XT) otherwise
 */
USBD_Params_XT *USBD_IOTP_Get_USBD(
      USBD_IOTP_Params_XT *tp);

/**
 * Gets pointer to USBDC_Params_XT structure(usbd).
 * usbdc structure contains all parameters of configuration in which TP is installed.
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \return pointer to "usbdc" when "tp" is valid, USBD_MAKE_INVALID_PTR(USBDC_Params_XT) otherwise
 */
USBDC_Params_XT *USBD_IOTP_Get_USBDC(
      USBD_IOTP_Params_XT *tp);

/**
 * Gets number of endpoint through which TP communicates with HOST
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \return endpoint number when "tp" is valid, USBD_MAX_NUM_ENDPOINTS otherwise
 */
uint8_t USBD_IOTP_Get_EP_Num(
      USBD_IOTP_Params_XT *tp);

/**
 * Gets direction of endpoint through which TP communicates with HOST
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \return endpoint direction when "tp" is valid, USB_EP_DIRECTION_OUT otherwise by default
 */
USB_EP_Direction_ET USBD_IOTP_Get_Dir(
      USBD_IOTP_Params_XT *tp);

/**
 * Gets size of data which has been successfully transferred
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \return size of data which has been successfully transferred
 */
USBD_IO_Inout_Data_Size_DT USBD_IOTP_Get_Transferred_Size(
      USBD_IOTP_Params_XT *tp);

/**
 * Installs all handlers in TP structure
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \param ready handler to function called after successfully processed data packet
 * \param buf_empty handler to function called when IN buffer is empty and whole data has been successfully sent to HOST
 * \param error handler to function called when some error occured
 * \param abort handler to function called when transmission is aborted
 */
void USBD_IOTP_Set_Handlers(
      USBD_IOTP_Params_XT  *tp,
      USBD_IOTP_Callback_HT ready,
      USBD_IOTP_Callback_HT buf_empty,
      USBD_IOTP_Callback_HT error,
      USBD_IOTP_Callback_HT abort);

/**
 * Installs "ready" handler in TP structure. Ready handler is called when all the data has been taken from user buffer
 * and next data can be provided by the user. It doesn't mean IN HW buffer is empty.
 * It means only data provided earlier by the user has been successfully copied to IN HW buffers.
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \param ready handler to function called after successfully processed data packet
 */
void USBD_IOTP_Set_Ready_Handler(
      USBD_IOTP_Params_XT *tp,
      USBD_IOTP_Callback_HT ready);

/**
 * Installs "buf_empty" handler in TP structure. Buf_empty handler is called when IN data has been successfully transferred to the HOST.
 * There is nothing in IN HW buffers. It is usefull handler when after sending HW must be reconfigured, for example
 * change device address or totally turn OFF USB Device on some request from the HOST (first we successfully responded, only after that
 * we can do the action).
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \param buf_empty handler to function called when IN buffer is empty and whole data has been successfully sent to HOST
 */
void USBD_IOTP_Set_Buf_Empty_Handler(
      USBD_IOTP_Params_XT *tp,
      USBD_IOTP_Callback_HT buf_empty);

/**
 * Installs "error" handler in TP structure
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \param error handler to function called when some error occured
 */
void USBD_IOTP_Set_Error_Handler(
      USBD_IOTP_Params_XT *tp,
      USBD_IOTP_Callback_HT error);

/**
 * Installs "abort" handler in TP structure
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \param abort handler to function called when transmission is aborted
 */
void USBD_IOTP_Set_Abort_Handler(
      USBD_IOTP_Params_XT *tp,
      USBD_IOTP_Callback_HT abort);

/**
 * Gets pointer to USBD_Vendor_Data_XT type container. That container is used by upper layer in its defined way.
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 */
USBD_Vendor_Data_XT *USBD_IOTP_Get_Vendor_Data_Container(USBD_IOTP_Params_XT *tp);



/**
 * Checks if transfer is active for specified TP
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \return USBD_TRUE when transfer is active, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_IOTP_Is_Transfer_Active(
      USBD_IOTP_Params_XT *tp);

/**
 * Aborts transfer on specified endpoint
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \param flush_hw_bufs if USBD_TRUE then hardware buffers will be cleared by port, USBD_FALSE otherwise.
 * \return USBD_TRUE when "abort" requested successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_IOTP_Abort(
      USBD_IOTP_Params_XT *tp,
      USBD_Bool_DT flush_hw_bufs);

/**
 * Sends status packet over specified endpoint
 * Can be instead of calling @see USBD_IOTP_Send with size 0.
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \param size_left pointer to returned size - size of data which is still waiting to be sent.
 *      If 0 is returned then zero-length packet has been initialized to be send.
 *      If not 0 is returned then zero-length packet has not been yet initialized to be send - it will be done in next irq.
 *      Zero is returned here in same situation like call of 'ready' handler.
 *      Unfortunately, 'ready' handler cannot be called from inside of @see USBD_IOTP_Send_Status
 *      to protect system against recursive calling of @see USBD_IOTP_Send_Status.
 *      This pointer can be USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT) - in this situation nothing will be returned.
 * \return USBD_TRUE when send status requested successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_IOTP_Send_Status(
      USBD_IOTP_Params_XT        *tp,
      USBD_IO_Inout_Data_Size_DT *size_left);


/**
 * Sends status packet over specified endpoint BUT using OUT-TP.
 * It is useful function as a configmation after receiving some data from OUT callback.
 *
 * \param tp_out pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \param size_left pointer to returned size - size of data which is still waiting to be sent.
 *      If 0 is returned then zero-length packet has been initialized to be send.
 *      If not 0 is returned then zero-length packet has not been yet initialized to be send - it will be done in next irq.
 *      Zero is returned here in same situation like call of 'ready' handler.
 *      Unfortunately, 'ready' handler cannot be called from inside of @see USBD_IOTP_Send_Status
 *      to protect system against recursive calling of @see USBD_IOTP_Send_Status.
 *      This pointer can be USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT) - in this situation nothing will be returned.
 * \return USBD_TRUE when send status requested successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_IOTP_Send_Status_For_Out_Tp(
      USBD_IOTP_Params_XT       *tp_out,
      USBD_IO_Inout_Data_Size_DT*size_left,
      USBD_IOTP_Callback_HT      buf_empty,
      USBD_Vendor_Data_XT       *vendor_data);

/**
 * Sends data over specified endpoint from linear buffer (single, contineout memory area).
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \param data pointer to data which shall be send
 * \param size size of data which shall be send. This function can be also used for sending data with size = 0.
 * \param size_left pointer to returned size - size of data which is still waiting to be sent.
 *      If (-1) is returned then all data has been copied to the buffer.
 *      (-1) is returned here in same situation like call of 'ready' handler.
 *      Unfortunately, this handler cannot be called from inside of @see USBD_IOTP_Send
 *      to protect system against recursive calling of @see USBD_IOTP_Send.
 *      This pointer can be USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT) - in this situation nothing will be returned.
 * \return USBD_TRUE when "send" requested successfully, USBD_FALSE otherwise
 *
 */
USBD_Bool_DT USBD_IOTP_Send(
      USBD_IOTP_Params_XT          *tp,
      const void                   *data,
      USBD_IO_Inout_Data_Size_DT    size,
      USBD_IO_Inout_Data_Size_DT   *size_left);

/**
 * Sends data over specified endpoint from a vector (list of linear data pieces).
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \param data pointer to data vector which shall be send
 * \patam data_num_elems number of elements in data vector
 * \param size size of data which shall be send. This function can be also used for sending data with size = 0.
 * \param size_left pointer to returned size - size of data which is still waiting to be sent.
 *      If (-1) is returned then all data has been copied to the buffer.
 *      (-1) is returned here in same situation like call of 'ready' handler.
 *      Unfortunately, this handler cannot be called from inside of @see USBD_IOTP_Send
 *      to protect system against recursive calling of @see USBD_IOTP_Send.
 *      This pointer can be USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT) - in this situation nothing will be returned.
 * \return USBD_TRUE when "send" requested successfully, USBD_FALSE otherwise
 *
 */
USBD_Bool_DT USBD_IOTP_Send_From_Vector(
      USBD_IOTP_Params_XT          *tp,
      const Buff_Readable_Vector_XT*data,
      Buff_Num_Elems_DT             data_num_elems,
      USBD_IO_Inout_Data_Size_DT    size,
      USBD_IO_Inout_Data_Size_DT   *size_left);

/**
 * Sends data over specified endpoint from a tree (list of vectors).
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \param data pointer to data tree which shall be send
 * \patam data_num_elems number of elements in data vector
 * \param size size of data which shall be send. This function can be also used for sending data with size = 0.
 * \param size_left pointer to returned size - size of data which is still waiting to be sent.
 *      If (-1) is returned then all data has been copied to the buffer.
 *      (-1) is returned here in same situation like call of 'ready' handler.
 *      Unfortunately, this handler cannot be called from inside of @see USBD_IOTP_Send
 *      to protect system against recursive calling of @see USBD_IOTP_Send.
 *      This pointer can be USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT) - in this situation nothing will be returned.
 * \return USBD_TRUE when "send" requested successfully, USBD_FALSE otherwise
 *
 */
USBD_Bool_DT USBD_IOTP_Send_From_Tree(
      USBD_IOTP_Params_XT          *tp,
      const Buff_Readable_Tree_XT  *data,
      Buff_Num_Elems_DT             data_num_elems,
      USBD_IO_Inout_Data_Size_DT    size,
      USBD_IO_Inout_Data_Size_DT   *size_left);

/**
 * Sends data over specified endpoint from a ring buffer.
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \param ring pointer to ring buffer from which data shall be sent
 * \param size size of data which shall be send.
 * \param size_left pointer to returned size - size of data which is still waiting to be sent.
 *      If (-1) is returned then all data has been copied to the buffer.
 *      (-1) is returned here in same situation like call of 'ready' handler.
 *      Unfortunately, this handler cannot be called from inside of @see USBD_IOTP_Send
 *      to protect system against recursive calling of @see USBD_IOTP_Send.
 *      This pointer can be USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT) - in this situation nothing will be returned.
 * \return USBD_TRUE when "send" requested successfully, USBD_FALSE otherwise
 *
 */
USBD_Bool_DT USBD_IOTP_Send_From_Ring(
      USBD_IOTP_Params_XT          *tp,
      Buff_Ring_XT                 *ring,
      USBD_IO_Inout_Data_Size_DT    size,
      USBD_IO_Inout_Data_Size_DT   *size_left);

/**
 * Keeps sending data over specified endpoint from a ring buffer.
 * When there is no data in the buffer, NAK is responded until more dat awill be added to the buffer.
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \param ring pointer to ring buffer from which data shall be sent
 * \return USBD_TRUE when "send" requested successfully, USBD_FALSE otherwise
 *
 */
USBD_Bool_DT USBD_IOTP_Send_From_Ring_Infinitely(
      USBD_IOTP_Params_XT          *tp,
      Buff_Ring_XT                 *ring);


/**
 * Receives data over specified endpoint and allows for next reception
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \param data pointer to data which shall be send
 * \param size size of data which shall be send
 * \param size_left pointer to returned size - size of data which is still waiting to be received.
 *      If -1 is returned then whole buffer has been filled with received data
 *      It is same situation like call of @see ready handler.
 *      Unfortunately, this handler is not called now from inside of @see USBD_IOTP_Recv_And_Ready
 *      to protect system from recursive calling of @see USBD_IOTP_Recv_And_Ready.
 *      This pointer can be USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT) - in this situation nothing will be returned.
 * \return USBD_TRUE when "recv" requested successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_IOTP_Recv_And_Ready(
      USBD_IOTP_Params_XT *tp,
      void *data,
      USBD_IO_Inout_Data_Size_DT size,
      USBD_IO_Inout_Data_Size_DT *size_left);

/**
 * Receives data over specified endpoint and waits for confirmation before allows for next reception
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \param data pointer to data which shall be send
 * \param size size of data which shall be send
 * \param size_left pointer to returned size - size of data which is still waiting to be received.
 *      If -1 is returned then whole buffer has been filled with received data
 *      It is same situation like call of @see ready handler.
 *      Unfortunately, this handler is not called now from inside of @see USBD_IOTP_Recv_And_Wait
 *      to protect system from recursive calling of @see USBD_IOTP_Recv_And_Wait.
 *      This pointer can be USBD_MAKE_INVALID_PTR(USBD_IO_Inout_Data_Size_DT) - in this situation nothing will be returned.
 * \return USBD_TRUE when "recv" requested successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_IOTP_Recv_And_Wait(
      USBD_IOTP_Params_XT *tp,
      void *data,
      USBD_IO_Inout_Data_Size_DT size,
      USBD_IO_Inout_Data_Size_DT *size_left);

/**
 * Enables receiving next data over specified endpoint.
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \param data pointer to data which shall be send
 * \param size size of data which shall be send
 * \return USBD_TRUE when OUT PIPE unlocked successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_IOTP_Recv_Ready(
      USBD_IOTP_Params_XT *tp);

/**
 * Sends stall packet over specified endpoint
 *
 * \param tp pointer to USBD_IOTP_Params_XT structure - EVENT-type TP params container
 * \return USBD_TRUE when response stall requested successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_IOTP_Send_Stall(
      USBD_IOTP_Params_XT *tp);

#endif
