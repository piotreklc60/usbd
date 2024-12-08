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
 *  \brief    USB Device DEV module
 *  \details   This module defines device dependant API which is part of USBD software layer
 *  \author   Piotr Wojtowicz
 *  \version   1.3.0.0
 *  \date     2012
 *  \pre      First initialize the system.
 *  \warning   Improper use can crash your application
 *  \copyright GNU Public License.
 */

#ifndef USBD_DEV_INTERNAL_H_
#define USBD_DEV_INTERNAL_H_


#ifndef USBD_H_
#include "usbd.h"
#endif


#define USBD_CHECK_PORT_PTR(usbd)                              (USBD_CHECK_PTR(const USBD_DEV_Port_Handler_XT,                (usbd)->dev.core.data.port))

#define USBD_DEV_CHECK_PORT_ACTIVATE_HANDLER(port)             (USBD_CHECK_HANDLER(USBD_DEV_PORT_Activate_Deactivate_HT,      (port)->handlers.activate))
#define USBD_DEV_CHECK_PORT_PARSE_EP_HANDLER(port)             (USBD_CHECK_HANDLER(USBD_DEV_PORT_Parse_EP_Desc_Variants_HT,   (port)->handlers.ep_parse))
#define USBD_DEV_CHECK_PORT_PARSE_CFG_HANDLER(port)            (USBD_CHECK_HANDLER(USBD_DEV_PORT_Parse_Cfg_Desc_HT,           (port)->handlers.cfg_parse))
#define USBD_DEV_CHECK_PORT_SUPPORTED_SPEED_HANDLER(port)      (USBD_CHECK_HANDLER(USBD_DEV_PORT_Get_Speed_HT,                (port)->handlers.get_supported_speed))
#define USBD_DEV_CHECK_PORT_CURRENT_SPEED_HANDLER(port)        (USBD_CHECK_HANDLER(USBD_DEV_PORT_Get_Speed_HT,                (port)->handlers.get_current_speed))
#define USBD_DEV_CHECK_PORT_DEVICE_STATUS_HANDLER(port)        (USBD_CHECK_HANDLER(USBD_DEV_PORT_Get_U16_Data_HT,             (port)->handlers.get_device_status))
#define USBD_DEV_CHECK_PORT_FRAME_NUM_HANDLER(port)            (USBD_CHECK_HANDLER(USBD_DEV_PORT_Get_U16_Data_HT,             (port)->handlers.get_frame_num))
#define USBD_DEV_CHECK_PORT_EP0_LOW_FULL_SPEED_HANDLER(port)   (USBD_CHECK_HANDLER(USBD_DEV_PORT_Get_EP_Desc_HT,              (port)->handlers.get_ep0_low_full_speed))
#define USBD_DEV_CHECK_PORT_EP0_HIGH_SPEED_HANDLER(port)       (USBD_CHECK_HANDLER(USBD_DEV_PORT_Get_EP_Desc_HT,              (port)->handlers.get_ep0_high_speed))



#define USBD_DEV_CHECK_ACTIVE_CONFIG_PTR(usbd)                 (USBD_CHECK_PTR(USBDC_Params_XT,                               (usbd)->dev.core.data.active_config))
#define USBD_DEV_CHECK_ACTIVE_CONFIG_DESC_PTR(usbd)            (USBD_CHECK_PTR(const uint8_t,                                 (usbd)->dev.core.data.active_config_desc))



#define USBD_DEV_GET_PORT_PTR(usbd)                            USBD_GET_PTR(const USBD_DEV_Port_Handler_XT,                   (usbd)->dev.core.data.port)

#define USBD_DEV_GET_PORT_ACTIVATE_HANDLER(port)               USBD_GET_HANDLER(USBD_DEV_PORT_Activate_Deactivate_HT,         (port)->handlers.activate)
#define USBD_DEV_GET_PORT_SUPPORTED_SPEED(usbd, port)          (USBD_DEV_CALL_PORT_SUPPORTED_SPEED((usbd), (port)))
#define USBD_DEV_GET_PORT_CURRENT_SPEED(usbd, port)            (USBD_DEV_CALL_PORT_CURRENT_SPEED((usbd), (port)))
#define USBD_DEV_GET_PORT_DEVICE_STATUS(usbd, port)            (USBD_DEV_CALL_PORT_DEVICE_STATUS((usbd), (port)))
#define USBD_DEV_GET_PORT_FRAME_NUM(usbd, port)                (USBD_DEV_CALL_PORT_FRAME_NUM((usbd), (port)))
#define USBD_DEV_GET_PORT_EP0_LOW_FULL_SPEED_DESC(usbd, port)  (USBD_DEV_CALL_PORT_GET_EP0_LOW_FULL_SPEED_DESC((usbd), (port)))
#define USBD_DEV_GET_PORT_EP0_HIGH_SPEED_DESC(usbd, port)      (USBD_DEV_CALL_PORT_GET_EP0_HIGH_SPEED_DESC((usbd), (port)))

#define USBD_DEV_GET_CONFIG_TAB_PTR(usbd)                      USBD_GET_PTR(USBD_DEV_Config_Header_XT,                        (usbd)->dev.core.data.config_tab)
#define USBD_DEV_GET_STRING_TAB_PTR(usbd)                      USBD_GET_PTR(USBD_DEV_String_Header_XT,                        (usbd)->dev.core.data.string_tab)
#define USBD_DEV_GET_INTERFACE_TAB_PTR(usbd)                   USBD_GET_PTR(USBD_DEV_Interface_Header_XT,                     (usbd)->dev.core.data.interface_tab)

#define USBD_DEV_GET_ACTIVE_CONFIG_NUM(usbd)                   ((usbd)->dev.core.data.active_config_num)
#define USBD_DEV_GET_ACTIVE_CONFIG_PTR(usbd)                   USBD_GET_PTR(USBDC_Params_XT,                                  (usbd)->dev.core.data.active_config)
#define USBD_DEV_GET_ACTIVE_CONFIG_DESC_PTR(usbd)              USBD_GET_PTR(const uint8_t,                                    (usbd)->dev.core.data.active_config_desc)
#define USBD_DEV_GET_EP_IN_INTERFACE(usbd, ep_num)             ((usbd)->dev.core.data.ep_in_interface[(ep_num)])
#define USBD_DEV_GET_EP_OUT_INTERFACE(usbd, ep_num)            ((usbd)->dev.core.data.ep_out_interface[(ep_num)])
#define USBD_DEV_GET_EP_IN_STATE(usbd, ep_num)                 ( (USBD_DEV_EP_States_ET)((usbd)->dev.core.data.ep_in_state[(ep_num)]) )
#define USBD_DEV_GET_EP_OUT_STATE(usbd, ep_num)                ( (USBD_DEV_EP_States_ET)((usbd)->dev.core.data.ep_out_state[(ep_num)]) )



#define USBD_DEV_SET_PORT_PTR(usbd, new_value)                 USBD_SET_PTR(const USBD_DEV_Port_Handler_XT,(usbd)->dev.core.data.port, (new_value))

#define USBD_DEV_SET_ACTIVE_CONFIG_NUM(usbd, new_value)        (usbd)->dev.core.data.active_config_num            = (new_value)
#define USBD_DEV_SET_ACTIVE_CONFIG_PTR(usbd, new_value)        USBD_SET_PTR(USBDC_Params_XT,  (usbd)->dev.core.data.active_config,    (new_value))
#define USBD_DEV_SET_ACTIVE_CONFIG_DESC_PTR(usbd, new_value)   USBD_SET_PTR(const uint8_t,    (usbd)->dev.core.data.active_config_desc,(new_value))
#define USBD_DEV_SET_EP_IN_INTERFACE(usbd, ep_num, new_value)  (usbd)->dev.core.data.ep_in_interface[(ep_num)]    = (new_value)
#define USBD_DEV_SET_EP_OUT_INTERFACE(usbd, ep_num, new_value) (usbd)->dev.core.data.ep_out_interface[(ep_num)]   = (new_value)
#define USBD_DEV_SET_EP_IN_STATE(usbd, ep_num, new_value)      (usbd)->dev.core.data.ep_in_state[(ep_num)]        = (new_value)
#define USBD_DEV_SET_EP_OUT_STATE(usbd, ep_num, new_value)     (usbd)->dev.core.data.ep_out_state[(ep_num)]       = (new_value)


#define USBD_DEV_CALL_PORT_ACTIVATE_HANDLER(usbd, port, state) \
            USBD_CALL_HANDLER(USBD_DEV_PORT_Activate_Deactivate_HT, (port)->handlers.activate)( \
               (usbd), \
               (state))

#define USBD_DEV_CALL_PORT_PARSE_EP_HANDLER(usbd, port, ep_num, if_desc, ep_desc) \
            USBD_CALL_HANDLER(USBD_DEV_PORT_Parse_EP_Desc_Variants_HT, (port)->handlers.ep_parse)( \
               (usbd), \
               (ep_num), \
               (if_desc), \
               (ep_desc))

#define USBD_DEV_CALL_PORT_PARSE_CFG_HANDLER(usbd, port, desc, desc_size, presult) \
            USBD_CALL_HANDLER(USBD_DEV_PORT_Parse_Cfg_Desc_HT, (port)->handlers.cfg_parse)( \
               (usbd), \
               (desc), \
               (desc_size), \
               (presult))

#define USBD_DEV_CALL_PORT_SUPPORTED_SPEED(usbd, port) \
            USBD_CALL_HANDLER(USBD_DEV_PORT_Get_Speed_HT, (port)->handlers.get_supported_speed)( \
               (usbd))

#define USBD_DEV_CALL_PORT_CURRENT_SPEED(usbd, port) \
            USBD_CALL_HANDLER(USBD_DEV_PORT_Get_Speed_HT, (port)->handlers.get_current_speed)( \
               (usbd))

#define USBD_DEV_CALL_PORT_DEVICE_STATUS(usbd, port) \
            USBD_CALL_HANDLER(USBD_DEV_PORT_Get_U16_Data_HT, (port)->handlers.get_device_status)( \
               (usbd))

#define USBD_DEV_CALL_PORT_FRAME_NUM(usbd, port) \
            USBD_CALL_HANDLER(USBD_DEV_PORT_Get_U16_Data_HT, (port)->handlers.get_frame_num)( \
               (usbd))

#define USBD_DEV_CALL_PORT_GET_EP0_LOW_FULL_SPEED_DESC(usbd, port) \
            USBD_CALL_HANDLER(USBD_DEV_PORT_Get_EP_Desc_HT, (port)->handlers.get_ep0_low_full_speed)( \
               (usbd))

#define USBD_DEV_CALL_PORT_GET_EP0_HIGH_SPEED_DESC(usbd, port) \
            USBD_CALL_HANDLER(USBD_DEV_PORT_Get_EP_Desc_HT, (port)->handlers.get_ep0_high_speed)( \
               (usbd))


#define USBD_DEV_CHECK_EP_IN_DESC_PTR(usbd, ep_num)            USBD_CHECK_PTR(const USB_Endpoint_Desc_DT, (usbd)->dev.core.data.ep[(ep_num)].ep_in_desc)
#define USBD_DEV_CHECK_EP_OUT_DESC_PTR(usbd, ep_num)           USBD_CHECK_PTR(const USB_Endpoint_Desc_DT, (usbd)->dev.core.data.ep[(ep_num)].ep_out_desc)



#define USBD_DEV_GET_EP_IN_DESC_PTR(usbd, ep_num)              USBD_GET_PTR(const USB_Endpoint_Desc_DT,   (usbd)->dev.core.data.ep[(ep_num)].ep_in_desc)
#define USBD_DEV_GET_EP_OUT_DESC_PTR(usbd, ep_num)             USBD_GET_PTR(const USB_Endpoint_Desc_DT,   (usbd)->dev.core.data.ep[(ep_num)].ep_out_desc)



#define USBD_DEV_SET_EP_IN_DESC_PTR(usbd, ep_num, new_value)   USBD_SET_PTR(const USB_Endpoint_Desc_DT,   (usbd)->dev.core.data.ep[(ep_num)].ep_in_desc, (new_value))
#define USBD_DEV_SET_EP_OUT_DESC_PTR(usbd, ep_num, new_value)  USBD_SET_PTR(const USB_Endpoint_Desc_DT,   (usbd)->dev.core.data.ep[(ep_num)].ep_out_desc, (new_value))

#endif
