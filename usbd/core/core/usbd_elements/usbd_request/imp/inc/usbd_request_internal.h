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
 *  \brief    USB Device REQUEST module
 *  \details   This module defines private request dependent API which is part of USBD software layer
 *  \author   Piotr Wojtowicz
 *  \version   1.3.0.0
 *  \date     2012
 *  \pre      First initialize the system.
 *  \warning   Improper use can crash your application
 *  \copyright GNU Public License.
 */

#ifndef USBD_REQUEST_INTERNAL_H_
#define USBD_REQUEST_INTERNAL_H_



#define USBD_REQUEST_CHECK_PORT_CBK_TAB_PTR(usbd)                    USBD_CHECK_PTR(USBD_REQUEST_Port_Callbacks_XT,    (usbd)->request.core.data.port_cbk)
#define USBD_REQUEST_CHECK_PORT_SET_ADDRESS_HANDLER(usbd)            USBD_CHECK_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->set_address)
#define USBD_REQUEST_CHECK_PORT_SET_FEATURE_HANDLER(usbd)            USBD_CHECK_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->set_feature)
#define USBD_REQUEST_CHECK_PORT_CLEAR_FEATURE_HANDLER(usbd)          USBD_CHECK_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->clear_feature)
#define USBD_REQUEST_CHECK_PORT_GET_STATUS_HANDLER(usbd)             USBD_CHECK_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->get_status)
#define USBD_REQUEST_CHECK_PORT_GET_DESCRIPTOR_HANDLER(usbd)         USBD_CHECK_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->get_descriptor)
#define USBD_REQUEST_CHECK_PORT_SET_DESCRIPTOR_HANDLER(usbd)         USBD_CHECK_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->set_descriptor)
#define USBD_REQUEST_CHECK_PORT_GET_CONFIGURATION_HANDLER(usbd)      USBD_CHECK_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->get_configuration)
#define USBD_REQUEST_CHECK_PORT_SET_CONFIGURATION_HANDLER(usbd)      USBD_CHECK_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->set_configuration)
#define USBD_REQUEST_CHECK_PORT_GET_INTERFACE_HANDLER(usbd)          USBD_CHECK_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->get_interface)
#define USBD_REQUEST_CHECK_PORT_SET_INTERFACE_HANDLER(usbd)          USBD_CHECK_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->set_interface)
#define USBD_REQUEST_CHECK_PORT_SYNCH_FRAME_HANDLER(usbd)            USBD_CHECK_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->synch_frame)


#define USBD_REQUEST_GET_PORT_SET_ADDRESS_HANDLER(usbd)              USBD_GET_HANDLER(USBD_REQUEST_Port_Callback_HT,   (usbd)->request.core.data.port_cbk->set_address)
#define USBD_REQUEST_GET_PORT_SET_FEATURE_HANDLER(usbd)              USBD_GET_HANDLER(USBD_REQUEST_Port_Callback_HT,   (usbd)->request.core.data.port_cbk->set_feature)
#define USBD_REQUEST_GET_PORT_CLEAR_FEATURE_HANDLER(usbd)            USBD_GET_HANDLER(USBD_REQUEST_Port_Callback_HT,   (usbd)->request.core.data.port_cbk->clear_feature)
#define USBD_REQUEST_GET_PORT_GET_STATUS_HANDLER(usbd)               USBD_GET_HANDLER(USBD_REQUEST_Port_Callback_HT,   (usbd)->request.core.data.port_cbk->get_status)
#define USBD_REQUEST_GET_PORT_GET_DESCRIPTOR_HANDLER(usbd)           USBD_GET_HANDLER(USBD_REQUEST_Port_Callback_HT,   (usbd)->request.core.data.port_cbk->get_descriptor)
#define USBD_REQUEST_GET_PORT_SET_DESCRIPTOR_HANDLER(usbd)           USBD_GET_HANDLER(USBD_REQUEST_Port_Callback_HT,   (usbd)->request.core.data.port_cbk->set_descriptor)
#define USBD_REQUEST_GET_PORT_GET_CONFIGURATION_HANDLER(usbd)        USBD_GET_HANDLER(USBD_REQUEST_Port_Callback_HT,   (usbd)->request.core.data.port_cbk->get_configuration)
#define USBD_REQUEST_GET_PORT_SET_CONFIGURATION_HANDLER(usbd)        USBD_GET_HANDLER(USBD_REQUEST_Port_Callback_HT,   (usbd)->request.core.data.port_cbk->set_configuration)
#define USBD_REQUEST_GET_PORT_GET_INTERFACE_HANDLER(usbd)            USBD_GET_HANDLER(USBD_REQUEST_Port_Callback_HT,   (usbd)->request.core.data.port_cbk->get_interface)
#define USBD_REQUEST_GET_PORT_SET_INTERFACE_HANDLER(usbd)            USBD_GET_HANDLER(USBD_REQUEST_Port_Callback_HT,   (usbd)->request.core.data.port_cbk->set_interface)
#define USBD_REQUEST_GET_PORT_SYNCH_FRAME_HANDLER(usbd)              USBD_GET_HANDLER(USBD_REQUEST_Port_Callback_HT,   (usbd)->request.core.data.port_cbk->synch_frame)


#define USBD_REQUEST_GET_EP0_IN_IOTP(usbd)                           (&((usbd)->request.core.data.ep0_in_iotp))
#define USBD_REQUEST_GET_EP0_OUT_IOTP(usbd)                          (&((usbd)->request.core.data.ep0_out_iotp))


#define USBD_REQUEST_CALL_PORT_SET_ADDRESS_HANDLER(_usbd, _ep_num, _req) \
            USBD_CALL_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->set_address)( \
               (_usbd), \
               (_ep_num), \
               (_req))

#define USBD_REQUEST_CALL_PORT_SET_FEATURE_HANDLER(_usbd, _ep_num, _req) \
            USBD_CALL_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->set_feature)( \
               (_usbd), \
               (_ep_num), \
               (_req))

#define USBD_REQUEST_CALL_PORT_CLEAR_FEATURE_HANDLER(_usbd, _ep_num, _req) \
            USBD_CALL_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->clear_feature)( \
               (_usbd), \
               (_ep_num), \
               (_req))

#define USBD_REQUEST_CALL_PORT_GET_STATUS_HANDLER(_usbd, _ep_num, _req) \
            USBD_CALL_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->get_status)( \
               (_usbd), \
               (_ep_num), \
               (_req))

#define USBD_REQUEST_CALL_PORT_GET_DESCRIPTOR_HANDLER(_usbd, _ep_num, _req) \
            USBD_CALL_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->get_descriptor)( \
               (_usbd), \
               (_ep_num), \
               (_req))

#define USBD_REQUEST_CALL_PORT_SET_DESCRIPTOR_HANDLER(_usbd, _ep_num, _req) \
            USBD_CALL_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->set_descriptor)( \
               (_usbd), \
               (_ep_num), \
               (_req))

#define USBD_REQUEST_CALL_PORT_GET_CONFIGURATION_HANDLER(_usbd, _ep_num, _req) \
            USBD_CALL_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->get_configuration)( \
               (_usbd), \
               (_ep_num), \
               (_req))

#define USBD_REQUEST_CALL_PORT_SET_CONFIGURATION_HANDLER(_usbd, _ep_num, _req) \
            USBD_CALL_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->set_configuration)( \
               (_usbd), \
               (_ep_num), \
               (_req))

#define USBD_REQUEST_CALL_PORT_GET_INTERFACE_HANDLER(_usbd, _ep_num, _req) \
            USBD_CALL_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->get_interface)( \
               (_usbd), \
               (_ep_num), \
               (_req))

#define USBD_REQUEST_CALL_PORT_SET_INTERFACE_HANDLER(_usbd, _ep_num, _req) \
            USBD_CALL_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->set_interface)( \
               (_usbd), \
               (_ep_num), \
               (_req))

#define USBD_REQUEST_CALL_PORT_SYNCH_FRAME_HANDLER(_usbd, _ep_num, _req) \
            USBD_CALL_HANDLER(USBD_REQUEST_Port_Callback_HT, (usbd)->request.core.data.port_cbk->synch_frame)( \
               (_usbd), \
               (_ep_num), \
               (_req))



#define USBDC_REQUEST_CHECK_VENDOR_HANDLER(usbdc)                          USBD_CHECK_HANDLER(USBD_REQUEST_Vendor_HT,   (usbdc)->request.core.handlers.vendor_irq)
#define USBDC_REQUEST_CHECK_INTERFACE_HANDLER(usbdc, _if_num)              USBD_CHECK_HANDLER(USBD_REQUEST_Interface_HT,(usbdc)->request.core.handlers.interface_irq_tab[(_if_num)])


#define USBDC_REQUEST_GET_VENDOR_HANDLER(usbdc)                            USBD_GET_HANDLER(USBD_REQUEST_Vendor_HT,     (usbdc)->request.core.handlers.vendor_irq)
#define USBDC_REQUEST_GET_INTERFACE_HANDLER(usbdc, _if_num)                USBD_GET_HANDLER(USBD_REQUEST_Interface_HT,  (usbdc)->request.core.handlers.interface_irq_tab[(_if_num)])


#define USBDC_REQUEST_SET_VENDOR_HANDLER(usbdc, _new_value)                USBD_SET_HANDLER(USBD_REQUEST_Vendor_HT,     (usbdc)->request.core.handlers.vendor_irq,  (_new_value))
#define USBDC_REQUEST_SET_INTERFACE_HANDLER(usbdc, _if_num, _new_value)    USBD_SET_HANDLER(USBD_REQUEST_Interface_HT,  (usbdc)->request.core.handlers.interface_irq_tab[(_if_num)], (_new_value))


#define USBDC_REQUEST_CALL_VENDOR_IRQ(_usbd, _usbdc, _ep_num, _req) \
            USBD_CALL_HANDLER(USBD_REQUEST_Vendor_HT, (_usbdc)->request.core.handlers.vendor_irq)( \
               (_usbdc), \
               (_ep_num), \
               (_req), \
               USBD_REQUEST_GET_EP0_IN_IOTP(_usbd), \
               USBD_REQUEST_GET_EP0_OUT_IOTP(_usbd))

#define USBDC_REQUEST_CALL_INTERFACE_IRQ(_usbd, _usbdc, _ep_num, _if_num, _req) \
            USBD_CALL_HANDLER(USBD_REQUEST_Interface_HT, (_usbdc)->request.core.handlers.interface_irq_tab[(_if_num)])( \
               (_usbdc), \
               (_ep_num), \
               (_if_num), \
               (_req), \
               USBD_REQUEST_GET_EP0_IN_IOTP(_usbd), \
               USBD_REQUEST_GET_EP0_OUT_IOTP(_usbd))

#define USBDC_REQUEST_CHECK_EP_OUT_HALT_MASK(usbdc, _ep_num)               (0 != ((usbdc)->request.core.data.ep_out_halt_mask & (1 << (_ep_num))))
#define USBDC_REQUEST_CHECK_EP_IN_HALT_MASK(usbdc, _ep_num)                (0 != ((usbdc)->request.core.data.ep_in_halt_mask  & (1 << (_ep_num))))

#define USBDC_REQUEST_SET_EP_OUT_HALT_MASK(usbdc, _ep_num)                 (usbdc)->request.core.data.ep_out_halt_mask |= (1 << (_ep_num))
#define USBDC_REQUEST_SET_EP_IN_HALT_MASK(usbdc, _ep_num)                  (usbdc)->request.core.data.ep_in_halt_mask  |= (1 << (_ep_num))

#define USBDC_REQUEST_CLEAR_EP_OUT_HALT_MASK(usbdc, _ep_num)               (usbdc)->request.core.data.ep_out_halt_mask &= (~((uint16_t)(1 << (_ep_num))))
#define USBDC_REQUEST_CLEAR_EP_IN_HALT_MASK(usbdc, _ep_num)                (usbdc)->request.core.data.ep_in_halt_mask  &= (~((uint16_t)(1 << (_ep_num))))

#endif
