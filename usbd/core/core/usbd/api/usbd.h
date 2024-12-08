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
 *  \brief    USB Device IO module
 *  \details   This module defines IN-OUT API which is part of USBD software layer
 *  \author   Piotr Wojtowicz
 *  \version   1.3.0.0
 *  \date     2012
 *  \pre      First initialize the system.
 *  \warning   Improper use can crash your application
 *  \copyright GNU Public License.
 */

#ifndef USBD_H_
#define USBD_H_

#ifndef STD_LIBS_H_
#include "std_libs.h"
#endif

#ifndef USBD_CFG_H_
#include "usbd_cfg.h"
#endif

#ifndef USBD_DBG_H_
#include "usbd_dbg.h"
#endif

#ifndef USBD_COMMON_TYPES_H_
#include "usbd_common_types.h"
#endif

#ifndef USBD_PUBLIC_MACRO_H_
#include "usbd_public_macro.h"
#endif

#ifndef USB_CDP_H_
#include "usb_cdp.h"
#endif

struct USBD_params_eXtended_Tag;
struct USBDC_params_eXtended_Tag;

#ifndef USBD_DEV_TYPES_H_
#include "usbd_dev_types.h"
#endif

#ifndef USBD_IO_TYPES_H_
#include "usbd_io_types.h"
#endif

#ifndef USBD_EVENT_TYPES_H_
#include "usbd_event_types.h"
#endif

#ifndef USBD_IOTP_EVENT_TYPES_H_
#include "usbd_iotp_event_types.h"
#endif

#ifndef USBD_IOTP_BUFF_TYPES_H_
#ifdef  USBD_IOTP_BUFF_USE
#include "usbd_iotp_buff_types.h"
#endif
#endif

#ifndef USBD_REQUEST_TYPES_H_
#include "usbd_request_types.h"
#endif


typedef struct USBD_params_eXtended_Tag
{
#ifdef USBD_DEV_PRESENT
   USBD_DEV_Params_XT dev;
#endif
#ifdef USBD_IO_PRESENT
   USBD_IO_Params_XT io;
#endif
#ifdef USBD_EVENT_PRESENT
   USBD_EVENT_Params_XT event;
#endif
#ifdef USBD_REQUEST_PRESENT
   USBD_REQUEST_Params_XT request;
#endif
}USBD_Params_XT;



#ifndef USBDC_H_
#include "usbdc.h"
#endif

#ifndef USBD_DEV_H_
#include "usbd_dev.h"
#endif

#ifndef USBD_IO_H_
#include "usbd_io.h"
#endif

#ifndef USBD_EVENT_H_
#include "usbd_event.h"
#endif

#ifndef USBD_IOTP_EVENT_H_
#include "usbd_iotp_event.h"
#endif

#ifndef USBD_IOTP_BUFF_H_
#ifdef  USBD_IOTP_BUFF_USE
#include "usbd_iotp_buff.h"
#endif
#endif

#ifndef USBD_REQUEST_H_
#include "usbd_request.h"
#endif


#ifdef USBD_IO_PRESENT
#if (!defined(USBD_DEV_PRESENT))
#error "USBD DEV module required for USBD IO module is missing!"
#endif
#endif


#ifdef USBD_EVENT_PRESENT
#if (!defined(USBD_DEV_PRESENT))
#error "USBD DEV module required for USBD EVENT module is missing!"
#endif

#if (!defined(USBDC_PRESENT))
#error "USBD config module required for USBD EVENT module is missing!"
#endif
#endif


#ifdef USBD_REQUEST_PRESENT
#if (!defined(USBD_DEV_PRESENT))
#error "USBD DEV module required for USBD REQUEST module is missing!"
#endif

#if (!defined(USBDC_PRESENT))
#error "USBD config module required for USBD REQUEST module is missing!"
#endif

#if (!defined(USBD_IO_PRESENT))
#error "USBD IO module required for USBD REQUEST module is missing!"
#endif

#if (!defined(USBD_IOTP_EVENT_PRESENT))
#error "USBD IOTP EVENT module required for USBD REQUEST module is missing!"
#endif
#endif


#if(USBD_MAX_NUM_ENDPOINTS > 16)
#error "USBD_MAX_NUM_ENDPOINTS too big!!! max possible is 16 endpoints!"
#endif

#if(USBD_MAX_NUM_INTERFACES >= 256)
#error "USBD_MAX_NUM_INTERFACES too big!!! max possible is 255 interfaces!"
#endif


/**
 * Initializes whole USBD_params_XT structure
 *
 * \param usbd pointer to usb device
 */
void USBD_Init(
      USBD_Params_XT *usbd);


#endif
