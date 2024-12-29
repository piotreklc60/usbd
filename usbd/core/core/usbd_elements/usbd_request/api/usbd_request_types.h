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

#ifndef USBD_REQUEST_TYPES_H_
#define USBD_REQUEST_TYPES_H_

#define USBD_REQUEST_PRESENT   1
/**
 * Required modules:
 * - USBD_DEV
 * - USBDC
 * - USBD_IO
 * - USBD_IOTP_EVENT
 */


/** @defgroup USBD_REQUEST_TYPES
 * @{
 */

typedef enum USBD_REQUEST_bRequest_Enum_Tag
{
   USBD_BREQUEST_GET_STATUS               = 0,
   USBD_BREQUEST_CLEAR_FEATURE            = 1,
//  RESERVED
   USBD_BREQUEST_SET_FEATURE              = 3,
//  RESERVED
   USBD_BREQUEST_SET_ADDRESS              = 5,
   USBD_BREQUEST_GET_DESCRIPTOR           = 6,
   USBD_BREQUEST_SET_DESCRIPTOR           = 7,
   USBD_BREQUEST_GET_CONFIGURATION        = 8,
   USBD_BREQUEST_SET_CONFIGURATION        = 9,
   USBD_BREQUEST_GET_INTERFACE            = 10,
   USBD_BREQUEST_SET_INTERFACE            = 11,
   USBD_BREQUEST_SYNCH_FRAME              = 12
} USBD_REQUEST_bRequest_ET;

typedef enum USBD_REQUEST_bmRequestType_Enum_Tag
{
   USBD_BMREQUESTTYPE_DEVICE              = 0x00,
   USBD_BMREQUESTTYPE_INTERFACE           = 0x01,
   USBD_BMREQUESTTYPE_ENDPOINT            = 0x02,
   USBD_BMREQUESTTYPE_OTHER               = 0x03,

   USBD_BMREQUESTTYPE_IN                  = USB_EP_DESC_DIR_IN,
   USBD_BMREQUESTTYPE_OUT                 = USB_EP_DESC_DIR_OUT,

   USBD_BMREQUESTTYPE_STANDARD            = 0x00,
   USBD_BMREQUESTTYPE_CLASS               = 0x20,
   USBD_BMREQUESTTYPE_VENDOR              = 0x40,

   USBD_BMREQUESTTYPE_DESTINATION_MASK    = 0x03,
   USBD_BMREQUESTTYPE_DIRECTION_MASK      = USB_EP_DESC_DIR_MASK,
   USBD_BMREQUESTTYPE_TYPE_MASK           = 0x60,
   USBD_BMREQUESTTYPE_RECIPIENT_MASK      = 0x1F,

   USBD_BMREQUESTTYPE_SET_CLEAR_DEVICE    = (USBD_BMREQUESTTYPE_OUT | USBD_BMREQUESTTYPE_DEVICE),
   USBD_BMREQUESTTYPE_SET_CLEAR_INTERFACE = (USBD_BMREQUESTTYPE_OUT | USBD_BMREQUESTTYPE_INTERFACE),
   USBD_BMREQUESTTYPE_SET_CLEAR_ENDPOINT  = (USBD_BMREQUESTTYPE_OUT | USBD_BMREQUESTTYPE_ENDPOINT),

   USBD_BMREQUESTTYPE_GET_SYNCH_DEVICE    = (USBD_BMREQUESTTYPE_IN  | USBD_BMREQUESTTYPE_DEVICE),
   USBD_BMREQUESTTYPE_GET_SYNCH_INTERFACE = (USBD_BMREQUESTTYPE_IN  | USBD_BMREQUESTTYPE_INTERFACE),
   USBD_BMREQUESTTYPE_GET_SYNCH_ENDPOINT  = (USBD_BMREQUESTTYPE_IN  | USBD_BMREQUESTTYPE_ENDPOINT)
}USBD_REQUEST_bmRequestType_ET;

typedef enum USBD_REQUEST_Standard_Requests_Enum_Tag
{
   USBD_STD_REQ_CLEAR_FEATURE_DEVICE      = (USBD_BMREQUESTTYPE_SET_CLEAR_DEVICE    | (USBD_BREQUEST_CLEAR_FEATURE << 8)),
   USBD_STD_REQ_CLEAR_FEATURE_INTERFACE   = (USBD_BMREQUESTTYPE_SET_CLEAR_INTERFACE | (USBD_BREQUEST_CLEAR_FEATURE << 8)),
   USBD_STD_REQ_CLEAR_FEATURE_ENDPOINT    = (USBD_BMREQUESTTYPE_SET_CLEAR_ENDPOINT  | (USBD_BREQUEST_CLEAR_FEATURE << 8)),

   USBD_STD_REQ_SET_FEATURE_DEVICE        = (USBD_BMREQUESTTYPE_SET_CLEAR_DEVICE    | (USBD_BREQUEST_SET_FEATURE << 8)),
   USBD_STD_REQ_SET_FEATURE_INTERFACE     = (USBD_BMREQUESTTYPE_SET_CLEAR_INTERFACE | (USBD_BREQUEST_SET_FEATURE << 8)),
   USBD_STD_REQ_SET_FEATURE_ENDPOINT      = (USBD_BMREQUESTTYPE_SET_CLEAR_ENDPOINT  | (USBD_BREQUEST_SET_FEATURE << 8)),

   USBD_STD_REQ_GET_STATUS_DEVICE         = (USBD_BMREQUESTTYPE_GET_SYNCH_DEVICE    | (USBD_BREQUEST_GET_STATUS << 8)),
   USBD_STD_REQ_GET_STATUS_INTERFACE      = (USBD_BMREQUESTTYPE_GET_SYNCH_INTERFACE | (USBD_BREQUEST_GET_STATUS << 8)),
   USBD_STD_REQ_GET_STATUS_ENDPOINT       = (USBD_BMREQUESTTYPE_GET_SYNCH_ENDPOINT  | (USBD_BREQUEST_GET_STATUS << 8)),

   USBD_STD_REQ_GET_CONFIGURATION_DEVICE  = (USBD_BMREQUESTTYPE_GET_SYNCH_DEVICE    | (USBD_BREQUEST_GET_CONFIGURATION << 8)),

   USBD_STD_REQ_SET_CONFIGURATION_DEVICE  = (USBD_BMREQUESTTYPE_SET_CLEAR_DEVICE    | (USBD_BREQUEST_SET_CONFIGURATION << 8)),

   USBD_STD_REQ_GET_DESCRIPTOR_DEVICE     = (USBD_BMREQUESTTYPE_GET_SYNCH_DEVICE    | (USBD_BREQUEST_GET_DESCRIPTOR << 8)),
   USBD_STD_REQ_GET_DESCRIPTOR_INTERFACE  = (USBD_BMREQUESTTYPE_GET_SYNCH_INTERFACE | (USBD_BREQUEST_GET_DESCRIPTOR << 8)),
   USBD_STD_REQ_GET_DESCRIPTOR_ENDPOINT   = (USBD_BMREQUESTTYPE_GET_SYNCH_ENDPOINT  | (USBD_BREQUEST_GET_DESCRIPTOR << 8)),

   USBD_STD_REQ_SET_DESCRIPTOR_DEVICE     = (USBD_BMREQUESTTYPE_SET_CLEAR_DEVICE    | (USBD_BREQUEST_SET_DESCRIPTOR << 8)),

   USBD_STD_REQ_GET_INTERFACE_INTERFACE   = (USBD_BMREQUESTTYPE_GET_SYNCH_INTERFACE | (USBD_BREQUEST_GET_INTERFACE << 8)),

   USBD_STD_REQ_SET_INTERFACE_INTERFACE   = (USBD_BMREQUESTTYPE_SET_CLEAR_INTERFACE | (USBD_BREQUEST_SET_INTERFACE << 8)),

   USBD_STD_REQ_SET_ADDRESS_DEVICE        = (USBD_BMREQUESTTYPE_SET_CLEAR_DEVICE    | (USBD_BREQUEST_SET_ADDRESS << 8)),

   USBD_STD_REQ_SYNCH_FRAME_ENDPOINT      = (USBD_BMREQUESTTYPE_GET_SYNCH_ENDPOINT  | (USBD_BREQUEST_SYNCH_FRAME << 8))
}USBD_REQUEST_Standard_Requests_ET;

/**
 * \struct USBD_REQUEST_Req_DT
 */
typedef struct USBD_REQUEST_Req_Data_Tag
{
   /**
    * composed from bmRequestType and bRequest by USBD_REQUEST_Process_Req using formula:
    * request = (((uint16_t)bRequest & 0xFF) * 256) | ((uint16_t)bmRequestType & 0xFF);
    */
   uint16_t request;
   /* below fields shall be set by the port before calling USBD_REQUEST_Process_Req */
   uint8_t  bmRequestType;
   uint8_t  bRequest;
   uint16_t wValue;
   uint16_t wIndex;
   uint16_t wLength;
}USBD_REQUEST_Req_DT;

/**
 * \typedef USBD_REQUEST_Interface_HT
 * type of function called when USB request is received and has to be consumed by interface
 *
 * \param usbdc pointer to active configuration. POINTER ALWAYS VALID.
 * \param ep_num endpoint number
 * \param interface_num number of interface to which request is addressed
 * \param req pointer to request parameters. POINTER ALWAYS VALID.
 * \param tp_in if request is processed on endpoint number 0 then points to IN TP params. For non-zero endpoints this pointer is invalid
 * \param tp_out if request is processed on endpoint number 0 then points to OUT TP params. For non-zero endpoints this pointer is invalid
 * \return USBD_TRUE if request consumed, USBD_FALSE otherwise
 */
typedef USBD_Bool_DT (*USBD_REQUEST_Interface_HT) (struct USBDC_params_eXtended_Tag *usbdc, uint8_t ep_num, uint8_t if_num, USBD_REQUEST_Req_DT *req, USBD_IOTP_EVENT_Params_XT *tp_in, USBD_IOTP_EVENT_Params_XT *tp_out);

/**
 * \typedef USBD_REQUEST_Vendor_HT
 * type of function called when USB request is received and has to be consumed in vendor specified way
 *
 * \param usbdc pointer to active configuration. POINTER ALWAYS VALID.
 * \param ep_num endpoint number
 * \param req pointer to request parameters. POINTER ALWAYS VALID.
 * \param tp_in if request is processed on endpoint number 0 then points to IN TP params. For non-zero endpoints this pointer is invalid
 * \param tp_out if request is processed on endpoint number 0 then points to OUT TP params. For non-zero endpoints this pointer is invalid
 * \return USBD_TRUE if request consumed, USBD_FALSE otherwise
 */
typedef USBD_Bool_DT (*USBD_REQUEST_Vendor_HT) (struct USBDC_params_eXtended_Tag *usbdc, uint8_t ep_num, USBD_REQUEST_Req_DT *req, USBD_IOTP_EVENT_Params_XT *tp_in, USBD_IOTP_EVENT_Params_XT *tp_out);

/**
 * \typedef USBD_REQUEST_Port_Callback_HT
 * port function called by USBD core after request has been received. This type of function is called
 * on the begining of processing standard request (exception is "set_address" which is called after response has been send to HOST).
 * If port function returns USBD_TRUE then request
 * is accepted by hardware and processing can be continued by USBD/upper layer.
 *
 * \param usbd pointer to usb device. POINTER ALWAYS VALID.
 * \param ep_num endpoint number
 * \param req pointer to request parameters. POINTER ALWAYS VALID.
 * \return USBD_TRUE is port accepted request and can be processed by USBD/upper layers. USBD_FALSE otherwise - USBD will reject request.
 */
typedef USBD_Bool_DT (*USBD_REQUEST_Port_Callback_HT) (struct USBD_params_eXtended_Tag *usbd, uint8_t ep_num, USBD_REQUEST_Req_DT *req);



typedef struct USBD_REQUEST_Port_Callbacks_eXtended_Tag
{
   USBD_REQUEST_Port_Callback_HT get_status;
   USBD_REQUEST_Port_Callback_HT clear_feature;
   USBD_REQUEST_Port_Callback_HT set_feature;
   USBD_REQUEST_Port_Callback_HT set_address;
   USBD_REQUEST_Port_Callback_HT get_descriptor;
   USBD_REQUEST_Port_Callback_HT set_descriptor;
   USBD_REQUEST_Port_Callback_HT get_configuration;
   USBD_REQUEST_Port_Callback_HT set_configuration;
   USBD_REQUEST_Port_Callback_HT get_interface;
   USBD_REQUEST_Port_Callback_HT set_interface;
   USBD_REQUEST_Port_Callback_HT synch_frame;
}USBD_REQUEST_Port_Callbacks_XT;


#define USBD_REQUEST_FILL_PORT_CALLBACKS(\
   _get_status, _clear_feature, _set_feature, _set_address, _get_descriptor,\
   _set_descriptor, _get_configuration, _set_configuration, _get_interface, _set_interface, _synch_frame) \
      { \
         (_get_status),\
         (_clear_feature),\
         (_set_feature),\
         (_set_address),\
         (_get_descriptor),\
         (_set_descriptor),\
         (_get_configuration),\
         (_set_configuration),\
         (_get_interface),\
         (_set_interface),\
         (_synch_frame)\
      }


/**
 * \struct USBD_REQUEST_Params_XT
 * REQUEST part of USBD
 */
typedef struct USBD_REQUEST_Params_eXtended_Tag
{
   struct
   {
      struct
      {
         const USBD_REQUEST_Port_Callbacks_XT  *port_cbk;
         USBD_IOTP_EVENT_Params_XT              ep0_in_iotp;
         USBD_IOTP_EVENT_Params_XT              ep0_out_iotp;
         union
         {
            uint8_t u8;
            uint16_t u16;
            USB_Device_Qualifier_Desc_DT qual;
         }req_data;
      }data;
   }core;
}USBD_REQUEST_Params_XT;

/** @} */ /* USBD_REQUEST_TYPES */



/** @defgroup USBDC_REQUEST_TYPES
 * @{
 */


/**
 * \struct USBDC_REQUEST_Params_XT
 * REQUEST part of USBDC
 */
typedef struct USBDC_REQUEST_Params_eXtended_Tag
{
   struct
   {
      struct
      {
         USBD_REQUEST_Vendor_HT     vendor_irq;
         USBD_REQUEST_Interface_HT  interface_irq_tab[USBD_MAX_NUM_INTERFACES];
      }handlers;
      struct
      {
         uint16_t                   ep_in_halt_mask;
         uint16_t                   ep_out_halt_mask;
      }data;
   }core;
}USBDC_REQUEST_Params_XT;

/** @} */ /* USBDC_REQUEST_TYPES */


#endif
