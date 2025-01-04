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

#ifndef USBDC_H_
#define USBDC_H_

#define USBDC_PRESENT      1


/** @defgroup USBDC
 * @{
 */

typedef struct USBDC_Interface_Header_eXtended_Tag
{
   /** this structure is accessed only by upper layer (configuration) */
   USBD_Vendor_Data_XT        vendor;
   /** this structure is accessed only by upper layer (configuration) */
   USBD_Vendor_Data_XT        vendor_data;
}USBDC_Interface_Header_XT;


typedef struct USBDC_params_eXtended_Tag
{
   void                      *usbd;
   const uint8_t             *config_desc;
#if(defined(USBD_EVENT_PRESENT) && (USBDC_MAX_NUM_EVENTS > 0))
   USBDC_EVENT_Params_XT      event;
#endif
#ifdef USBD_REQUEST_PRESENT
   USBDC_REQUEST_Params_XT    request;
#endif

   USBDC_Interface_Header_XT  interface_tab[USBD_MAX_NUM_INTERFACES];
}USBDC_Params_XT;


/**
 * Initializes whole USBDC_Params_XT structute
 *
 * @param usbdc pointer to usb configuration
 */
void USBDC_Init(
      USBDC_Params_XT *usbdc);

/**
 * Connects USBD_Params_XT structure to configuration structure
 *
 * @param usbdc pointer to usb configuration
 * @param usbd pointer to usb device structure
 */
void USBDC_Bind_Usbd(
      USBDC_Params_XT *usbdc,
      USBD_Params_XT *usbd);

/**
 * Gets USBD_Params_XT structure from configuration structure
 *
 * @return pointer to usb device structure
 *
 * @param usbdc pointer to usb configuration
 */
USBD_Params_XT *USBDC_Get_Usbd(
      USBDC_Params_XT *usbdc);

/**
 * Connects configuration descriptor to configuration structure
 *
 * @param usbdc pointer to usb configuration
 * @param config_desc pointer to configuration descriptor
 */
void USBDC_Bind_Config_Desc(
      USBDC_Params_XT *usbdc,
      const uint8_t *config_desc);

/**
 * Gets configuration descriptor from configuration structure
 *
 * @return pointer to configuration descriptor
 *
 * @param usbdc pointer to usb configuration
 */
const uint8_t *USBDC_Get_Config_Desc(
      USBDC_Params_XT *usbdc);

/**
 * Gets pointer to parameters of specified interface
 *
 * @param usbdc pointer to usb configuration
 * @param interface_num number of requested interface
 * @return pointer to interface parameters
 *  or USBD_MAKE_INVALID_PTR(USBDC_Interface_Header_XT) if parameters are inproper
 */
USBDC_Interface_Header_XT *USBDC_Get_Interface_Params(
      USBDC_Params_XT *usbdc,
      uint8_t interface_num);
/** @} */ /* USBDC */


#endif
