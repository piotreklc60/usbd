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

/*!
 *  \brief    USB Device EVENT module
 *  \details   This module defines events calling API which is part of USBD software layer
 *  \author   Piotr Wojtowicz
 *  \version   1.3.0.0
 *  \date     2012
 *  \pre      First initialize the system.
 *  \warning   Improper use can crash your application
 *  \copyright GNU Public License.
 */

#ifndef USBD_EVENT_TYPES_H_
#define USBD_EVENT_TYPES_H_

#define USBD_EVENT_PRESENT      1
/**
 * Requied modules:
 * - USBD_DEV
 * - USBDC
 */


/** @defgroup USBD_EVENT_TYPES
 * @{
 */

/**
 * @enum USBD_EVENT_Reason_ET
 * defines event reasons
 */
typedef enum USBD_EVENT_Reason_Enum_Tag
{
   USBD_EVENT_REASON_DETACHED       = (1 << 0),
   USBD_EVENT_REASON_ATTACHED       = (1 << 1),
   USBD_EVENT_REASON_UNPOWERED      = (1 << 2),
   USBD_EVENT_REASON_POWERED        = (1 << 3),
   USBD_EVENT_REASON_RESET          = (1 << 4),
   USBD_EVENT_REASON_ADDRESSED      = (1 << 5),
   USBD_EVENT_REASON_UNCONFIGURED   = (1 << 6),
   USBD_EVENT_REASON_CONFIGURED     = (1 << 7),
   USBD_EVENT_REASON_INTERFACE_SET  = (1 << 8),
   USBD_EVENT_REASON_SOF_RECEIVED   = (1 << 9),
   USBD_EVENT_REASON_SUSPENDED      = (1 << 10),
   USBD_EVENT_REASON_RESUMED        = (1 << 11),
   USBD_EVENT_REASON_ALL   = ( USBD_EVENT_REASON_DETACHED     | USBD_EVENT_REASON_ATTACHED   | USBD_EVENT_REASON_UNPOWERED
                             | USBD_EVENT_REASON_POWERED      | USBD_EVENT_REASON_RESET      | USBD_EVENT_REASON_ADDRESSED
                             | USBD_EVENT_REASON_UNCONFIGURED | USBD_EVENT_REASON_CONFIGURED | USBD_EVENT_REASON_INTERFACE_SET
                             | USBD_EVENT_REASON_SOF_RECEIVED | USBD_EVENT_REASON_SUSPENDED  | USBD_EVENT_REASON_RESUMED ),
   /* with this flag event will be installed only once in usbd/usbdc even USBD_EVENT_Install/USBDC_EVENT_Install is called multiple times with the same handler */
   USBD_EVENT_INSTALL_ONLY_ONCE     = (1 << 12)
}USBD_EVENT_Reason_ET;

struct USBD_EVENT_Event_Header_eXtended_Tag;

/**
 * @typedef USBD_EVENT_Reason_ET
 * defines event's format
 * Event is a function called in specific situations. \see USBD_EVENT_Reason_ET
 *
 * @param usbd pointer to USBD_Params_XT structure - USBD params handler
 * @param usbdc pointer to USBDC_Params_XT structure - configuration handler
 * @param event_params pointer to USBD_EVENT_Event_Header_XT structure which describes called handler
 * @param reason reason of calling handler - possible values are described by enum USBD_EVENT_Reason_ET
 */
typedef void (*USBD_EVENT_Event_HT) (struct USBD_params_eXtended_Tag *usbd, struct USBDC_params_eXtended_Tag *usbdc, struct USBD_EVENT_Event_Header_eXtended_Tag *event_params, USBD_EVENT_Reason_ET reason);

/**
 * @struct USBD_EVENT_Event_Header_XT
 * defines parameters of an Event.
 * @param event handler to event function
 * @param vendor upper layer data
 * @param vendor_data upper layer data
 * @param mask bit mask. If some bit is set then event will be called
 *      for proper reason.
 */
typedef struct USBD_EVENT_Event_Header_eXtended_Tag
{
   USBD_EVENT_Event_HT    event;
   USBD_Vendor_Data_XT    vendor;
   USBD_Vendor_Data_XT    vendor_data;
   /* USBD_EVENT_Reason_ET */
   uint16_t               mask;
   /* for internal use only - usaed during events processing to mark which was already processed and which not */
   uint8_t                proc_marker;
}USBD_EVENT_Event_Header_XT;

typedef struct
{
   uint8_t        marker;
   USBD_Bool_DT   ongoing;
   USBD_Bool_DT   dirty;
}USBD_EVENT_Proc_Params_DT;

typedef struct
{
   size_t         num_installed_events;
   uint8_t        marker;
   USBD_Bool_DT   proc_ongoing;
   USBD_Bool_DT   proc_dirty;
   USBD_EVENT_Event_Header_XT event_tab[];
} USBD_EVENT_Proc_Params_XT;

/**
 * @struct USBD_EVENT_Params_XT
 * EVENT part of USBD
 */
typedef struct USBD_EVENT_Params_eXtended_Tag
{
   struct
   {
      /* USBD_EVENT_Proc_Params_XT */ size_t data[(sizeof(USBD_EVENT_Proc_Params_XT) + sizeof(size_t) - sizeof(uint8_t)) / sizeof(size_t)];
      USBD_EVENT_Event_Header_XT             event_tab[USBD_MAX_NUM_EVENTS];
   }core;
}USBD_EVENT_Params_XT;

/** @} */ /* USBD_EVENT_TYPES */



/** @defgroup USBDC_EVENT_TYPES
 * @{
 */

/**
 * @struct USBDC_EVENT_Params_XT
 * EVENT part of USBDC
 */
typedef struct USBDC_EVENT_Params_eXtended_Tag
{
   struct
   {
      /* USBD_EVENT_Proc_Params_XT */ size_t data[(sizeof(USBD_EVENT_Proc_Params_XT) + sizeof(size_t) - sizeof(uint8_t)) / sizeof(size_t)];
      USBD_EVENT_Event_Header_XT             event_tab[USBDC_MAX_NUM_EVENTS];
   }core;
}USBDC_EVENT_Params_XT;

/** @} */ /* USBDC_EVENT_TYPES */


#endif

