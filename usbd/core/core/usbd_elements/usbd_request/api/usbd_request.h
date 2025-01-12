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

#ifndef USBD_REQUEST_H_
#define USBD_REQUEST_H_

/** @defgroup USBD_REQUEST
 * @{
 */

/**
 * Installs USBD_REQUEST_Port_Callbacks_XT structure pointer to usbd
 *
 * \param usbd pointer to usb device
 * \param callbacks pointer to structure with callbacks for standard requests
 */
void USBD_REQUEST_Install_Port_Callbacks(
      USBD_Params_XT *usbd,
      const USBD_REQUEST_Port_Callbacks_XT *callbacks);

/**
 * Processes SETUP packet.
 * This function has to be called from port when SETUP packet is received
 *
 * \param usbd pointer to usb device
 * \param ep_num number of endpoint which received request
 * \param req pointer to request parameters
 */
void USBD_REQUEST_Process_Req(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USBD_REQUEST_Req_DT *req);

/** @} */ /* USBD_REQUEST */



/** @defgroup USBDC_REQUEST
 * @{
 */

#if(USBD_FEATURE_PRESENT == USBD_REQ_VENDOR_SUPPORTED)
/**
 * Installs handler to function used to process vendor requests
 *
 * \param usbdc pointer to configuration
 * \param irq pointer to function called when vendor request is received
 */
void USBDC_REQUEST_Vendor_Irq_Install(
      USBDC_Params_XT *usbdc,
      USBD_REQUEST_Vendor_HT irq);
#endif

/**
 * Installs handler to function used to process interface requests
 *
 * \param usbdc pointer to configuration
 * \param irq pointer unfction called when interface request is received
 * \param interface_num interface number
 */
void USBDC_REQUEST_Interface_Irq_Install(
      USBDC_Params_XT *usbdc,
      USBD_REQUEST_Interface_HT irq,
      uint8_t interface_num);



#if(USBD_FEATURE_PRESENT == USBD_REQ_SET_CLEAR_FEATURE_SUPPORTED)
/**
 * Locks or unlocks processing SET/CLEAR FEATURE requests by USBD layer
 *
 * \param usbdc pointer to configuration
 * \param ep_num endpoint number
 * \param state USBD_TRUE if SET_FEATURE/CLEAR_FEATURE request for specified endpoint
 *      shall be consumed by interface callback instead of USBD, USBD_FALSE if
 *      USBD shall consume these requests.
 */
void USBDC_REQUEST_Set_EP_Halt_Mask(
      USBDC_Params_XT *usbdc,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_Bool_DT state);
#endif

/** @} */ /* USBDC_REQUEST */


#endif
