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

#ifndef USBD_EVENT_H_
#define USBD_EVENT_H_

/** @defgroup USBD_EVENT
 * @{
 */

#if(USBD_MAX_NUM_EVENTS > 0)
/**
 * Installs event in device's events list
 *
 * @param usbd pointer to usb device
 * @param event pointer to event which shall be installed
 * @return pointer t event's parameters if installed successfully,
 *       USBD_MAKE_INVALID_PTR(USBD_EVENT_Event_HT) otherwise
 */
USBD_EVENT_Event_Header_XT *USBD_EVENT_Install(
      USBD_Params_XT *usbd,
      USBD_EVENT_Event_HT event,
      USBD_EVENT_Reason_ET mask);

#if(USBD_FEATURE_PRESENT == USBD_MULTI_SESSION_SUPPORTED)
/**
 * Removes previously installed event from list of installed events.
 *
 * @param usbd pointer to usb device
 * @param event pointer to event which shall be removed
 * @return USBD_TRUE if event removed successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_EVENT_Remove_Event(
      USBD_Params_XT *usbd,
      USBD_EVENT_Event_HT event);

/**
 * Removes all installed events
 *
 * @param usbd pointer to usb device
 * @return USBD_TRUE if all events removed successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_EVENT_Remove_All_Events(
      USBD_Params_XT *usbd);
#endif

/**
 * Checks how many events are installed
 *
 * @param usbd pointer to usb device
 * @return number of installed events if usbd pointer is valid, 0 otherwise
 */
size_t USBD_EVENT_Get_Num_Installed_Events(
      USBD_Params_XT *usbd);
#endif

/** @} */ /* USBD_EVENT */



/** @defgroup USBDC_EVENT
 * @{
 */

#if(USBDC_MAX_NUM_EVENTS > 0)
/**
 * Installs event in configuration's events list
 *
 * @param usbdc pointer to usb configuration
 * @param event pointer to event which shall be installed
 * @return pointer t event's parameters if installed successfully,
 *       USBD_MAKE_INVALID_PTR(USBD_EVENT_Event_HT) otherwise
 */
USBD_EVENT_Event_Header_XT *USBDC_EVENT_Install(
      USBDC_Params_XT *usbdc,
      USBD_EVENT_Event_HT event,
      USBD_EVENT_Reason_ET mask);

#if(USBD_FEATURE_PRESENT == USBD_MULTI_SESSION_SUPPORTED)
/**
 * Removes previously installed event from list of installed events.
 *
 * @param usbdc pointer to usb confguration
 * @param event pointer to event which shall be removed
 * @return USBD_TRUE if event removed successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBDC_EVENT_Remove_Event(
      USBDC_Params_XT *usbdc,
      USBD_EVENT_Event_HT event);

/**
 * Removes all installed events
 *
 * @param usbdc pointer to usb confguration
 * @return USBD_TRUE if all events removed successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBDC_EVENT_Remove_All_Events(
      USBDC_Params_XT *usbdc);
#endif

/**
 * Checks how many events are installed
 *
 * @param usbdc pointer to usb confguration
 * @return number of installed events if usbdc pointer is valid, 0 otherwise
 */
size_t USBDC_EVENT_Get_Num_Installed_Events(
      USBDC_Params_XT *usbdc);
#endif

/** @} */ /* USBDC_EVENT */


#endif
