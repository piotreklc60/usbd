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
 *  \brief    USB Device Config module
 *  \details   This module defines device dependant API which is part of USBD software layer
 *  \author   Piotr Wojtowicz
 *  \version   1.3.0.0
 *  \date     2012
 *  \pre      First initialize the system.
 *  \warning   Improper use can crash your application
 *  \copyright GNU Public License.
 */

#ifndef USBDC_INTERNAL_H_
#define USBDC_INTERNAL_H_


#ifndef USBD_H_
#include "usbd.h"
#endif


#ifndef USBDC_GET_INTERFACE_PARAMS_PTR
#define USBDC_GET_INTERFACE_PARAMS_PTR(_usbdc, _if_num) ((USBDC_Interface_Header_XT*)( &(((USBDC_Params_XT*)(_usbdc))->interface_tab[_if_num]) ))
#endif



#ifndef USBDC_GET_USBD
#define USBDC_GET_USBD(_usbdc)                     USBD_GET_PTR(USBD_Params_XT, ((USBDC_Params_XT*)(usbdc))->usbd )
#endif

#ifndef USBDC_GET_CONFIG_DESC
#define USBDC_GET_CONFIG_DESC(_usbdc)              (((USBDC_Params_XT*)(usbdc))->config_desc)
#endif



#ifndef USBDC_SET_USBD
#define USBDC_SET_USBD(_usbdc, _new_usbd)          USBD_SET_PTR(USBD_Params_XT, ((USBDC_Params_XT*)(_usbdc))->usbd, (_new_usbd))
#endif

#ifndef USBDC_SET_CONFIG_DESC
#define USBDC_SET_CONFIG_DESC(_usbdc, _new_desc)   ((USBDC_Params_XT*)(_usbdc))->config_desc = (_new_desc)
#endif


#endif
