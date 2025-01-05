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

#ifndef USBD_EVENT_TYPES_H_
#define USBD_EVENT_TYPES_H_

#define USBD_EVENT_PRESENT      1
/**
 * Requied modules:
 * - USBD_DEV
 * - USBDC
 */

#ifndef USBD_EVENT_REASON_DETACHED_SUPPORTED
#define USBD_EVENT_REASON_DETACHED_SUPPORTED          USBD_FEATURE_PRESENT
#endif

#ifndef USBD_EVENT_REASON_ATTACHED_SUPPORTED
#define USBD_EVENT_REASON_ATTACHED_SUPPORTED          USBD_FEATURE_PRESENT
#endif

#ifndef USBD_EVENT_REASON_UNPOWERED_SUPPORTED
#define USBD_EVENT_REASON_UNPOWERED_SUPPORTED         USBD_FEATURE_PRESENT
#endif

#ifndef USBD_EVENT_REASON_POWERED_SUPPORTED
#define USBD_EVENT_REASON_POWERED_SUPPORTED           USBD_FEATURE_PRESENT
#endif

#ifndef USBD_EVENT_REASON_RESET_SUPPORTED
#define USBD_EVENT_REASON_RESET_SUPPORTED             USBD_FEATURE_PRESENT
#endif

#ifndef USBD_EVENT_REASON_ADDRESSED_SUPPORTED
#define USBD_EVENT_REASON_ADDRESSED_SUPPORTED         USBD_FEATURE_PRESENT
#endif

#ifndef USBD_EVENT_REASON_UNCONFIGURED_SUPPORTED
#define USBD_EVENT_REASON_UNCONFIGURED_SUPPORTED      USBD_FEATURE_PRESENT
#endif

#ifndef USBD_EVENT_REASON_CONFIGURED_SUPPORTED
#define USBD_EVENT_REASON_CONFIGURED_SUPPORTED        USBD_FEATURE_PRESENT
#endif

#ifndef USBD_EVENT_REASON_INTERFACE_SET_SUPPORTED
#if(USBD_MAX_NUM_ALTERNATE_INTERFACE_SETTINGS > 0)
#define USBD_EVENT_REASON_INTERFACE_SET_SUPPORTED     USBD_FEATURE_PRESENT
#else
#define USBD_EVENT_REASON_INTERFACE_SET_SUPPORTED     USBD_FEATURE_NOT_PRESENT
#endif
#endif

#ifndef USBD_EVENT_REASON_SOF_RECEIVED_SUPPORTED
#define USBD_EVENT_REASON_SOF_RECEIVED_SUPPORTED      USBD_FEATURE_PRESENT
#endif

#ifndef USBD_EVENT_REASON_SUSPENDED_SUPPORTED
#define USBD_EVENT_REASON_SUSPENDED_SUPPORTED         USBD_FEATURE_PRESENT
#endif

#ifndef USBD_EVENT_REASON_RESUMED_SUPPORTED
#define USBD_EVENT_REASON_RESUMED_SUPPORTED           USBD_FEATURE_PRESENT
#endif

/** @defgroup USBD_EVENT_TYPES
 * @{
 */

/**
 * @enum USBD_EVENT_Reason_ET
 * defines event reasons
 */
typedef enum USBD_EVENT_Reason_Enum_Tag
{
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_DETACHED_SUPPORTED)
   USBD_EVENT_REASON_DETACHED       = (1 << 0),
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_ATTACHED_SUPPORTED)
   USBD_EVENT_REASON_ATTACHED       = (1 << 1),
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_UNPOWERED_SUPPORTED)
   USBD_EVENT_REASON_UNPOWERED      = (1 << 2),
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_POWERED_SUPPORTED)
   USBD_EVENT_REASON_POWERED        = (1 << 3),
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_RESET_SUPPORTED)
   USBD_EVENT_REASON_RESET          = (1 << 4),
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_ADDRESSED_SUPPORTED)
   USBD_EVENT_REASON_ADDRESSED      = (1 << 5),
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_UNCONFIGURED_SUPPORTED)
   USBD_EVENT_REASON_UNCONFIGURED   = (1 << 6),
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_CONFIGURED_SUPPORTED)
   USBD_EVENT_REASON_CONFIGURED     = (1 << 7),
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_INTERFACE_SET_SUPPORTED)
   USBD_EVENT_REASON_INTERFACE_SET  = (1 << 8),
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_SOF_RECEIVED_SUPPORTED)
   USBD_EVENT_REASON_SOF_RECEIVED   = (1 << 9),
   #endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_SUSPENDED_SUPPORTED)
   USBD_EVENT_REASON_SUSPENDED      = (1 << 10),
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_RESUMED_SUPPORTED)
   USBD_EVENT_REASON_RESUMED        = (1 << 11),
#endif


   USBD_EVENT_REASON_ALL   = ( 0
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_DETACHED_SUPPORTED)
      | USBD_EVENT_REASON_DETACHED
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_ATTACHED_SUPPORTED)
      | USBD_EVENT_REASON_ATTACHED
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_UNPOWERED_SUPPORTED)
      | USBD_EVENT_REASON_UNPOWERED
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_POWERED_SUPPORTED)
      | USBD_EVENT_REASON_POWERED
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_RESET_SUPPORTED)
      | USBD_EVENT_REASON_RESET
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_ADDRESSED_SUPPORTED)
      | USBD_EVENT_REASON_ADDRESSED
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_UNCONFIGURED_SUPPORTED)
      | USBD_EVENT_REASON_UNCONFIGURED
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_CONFIGURED_SUPPORTED)
      | USBD_EVENT_REASON_CONFIGURED
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_INTERFACE_SET_SUPPORTED)
      | USBD_EVENT_REASON_INTERFACE_SET
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_SOF_RECEIVED_SUPPORTED)
      | USBD_EVENT_REASON_SOF_RECEIVED
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_SUSPENDED_SUPPORTED)
      | USBD_EVENT_REASON_SUSPENDED
#endif
#if(USBD_FEATURE_PRESENT == USBD_EVENT_REASON_RESUMED_SUPPORTED)
      | USBD_EVENT_REASON_RESUMED
#endif
   ),


   /* with this flag event will be installed only once in usbd/usbdc even USBD_EVENT_Install/USBDC_EVENT_Install is called multiple times with the same handler */
   USBD_EVENT_INSTALL_ONLY_ONCE     = (1 << 12)
}USBD_EVENT_Reason_ET;

struct USBD_EVENT_Event_Header_eXtended_Tag;

/**
 * @typedef USBD_EVENT_Reason_ET
 * defines event's format
 * Event is a function called in specific situations. \see USBD_EVENT_Reason_ET
 *
 * @param usbd pointer to USBD_Params_XT structure - USBD params handler. POINTER ALWAYS VALID
 * @param usbdc pointer to USBDC_Params_XT structure - configuration handler. For USBD events INVALID, for USBDC events ALWAYS VALID POINTER
 * @param event_params pointer to USBD_EVENT_Event_Header_XT structure which describes called handler. POINTER ALWAYS VALID
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

#if(USBD_MAX_NUM_EVENTS > 0)
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
#endif

/** @} */ /* USBD_EVENT_TYPES */



/** @defgroup USBDC_EVENT_TYPES
 * @{
 */

#if(USBDC_MAX_NUM_EVENTS > 0)
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
#endif

/** @} */ /* USBDC_EVENT_TYPES */


#endif

