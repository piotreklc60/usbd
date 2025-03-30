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

#ifndef CDC_VCOM_H_
#define CDC_VCOM_H_

#ifndef STD_LIBS_H_
#include "std_libs.h"
#endif

#ifndef USBD_H_
#include "usbd.h"
#endif

#if( (!defined( USBD_IOTP_SUPPORT_RING_INFINITE_BUFFERS)) || (!USBD_IOTP_SUPPORT_RING_INFINITE_BUFFERS))
#error "USBD_IOTP_SUPPORT_RING_INFINITE_BUFFERS not enabled! Please define USBD_IOTP_SUPPORT_RING_INFINITE_BUFFERS in usbd_cfg.h"
#endif

/**
 * Following debug groups are used by CDC_VCOM library and must be configured correctly:
 * CDC_VCOM_INIT
 * CDC_VCOM_STATE
 * CDC_VCOM_ONOFF
 * CDC_VCOM_EVENT
 * CDC_VCOM_REQ
 * CDC_VCOM_INVALID_PARAMS
 */

#ifndef CDC_VCOM_CCI_EP_BINTERVAL
#define CDC_VCOM_CCI_EP_BINTERVAL               1
#endif

#ifndef CDC_VCOM_DCI_EP_IN_BINTERVAL
#define CDC_VCOM_DCI_EP_IN_BINTERVAL            10
#endif

#ifndef CDC_VCOM_DCI_EP_OUT_BINTERVAL
#define CDC_VCOM_DCI_EP_OUT_BINTERVAL           10
#endif

#define CDC_VCOM_FUNCTIONAL_DESCRPTORS_SIZE     19

/* VCOM supports only data transfer, without DTR, RTS, etc. */
#define CDC_VCOM_MODE_DATA_ONLY                 0
/* VCOM supports data transfer and control signals, line DTR, RTC, etc. */
#define CDC_VCOM_MODE_DATA_AND_SIGNALS          1

#ifndef CDC_VCOM_MODE
#define CDC_VCOM_MODE                           CDC_VCOM_MODE_DATA_AND_SIGNALS
#endif


#define CDC_VCOM_CCI_DESC(_notif_if_num, _notif_iInterface, _ep_notif, _ep_notif_mps, _data_if_num) \
   /* Communication Class Interface */ \
   \
   \
   \
   /* Interface 0 Descriptor */ \
   USB_INTERFACE_DESC_TABLE_FILL( \
   _notif_if_num,                               /* bInterfaceNumber */ \
   0x00,                                        /* bAlternateSetting */ \
   0x01,                                        /* bNumEndpoints */ \
   USB_CLASS_COMMUNICATION,                     /* bInterfaceClass   Communication Class Interface */ \
   USB_SUBCLASS_COMM_ABSTRACT_CONTROL_MODEL,    /* bInterfaceSubclass */ \
   USB_PROTOCOL_COMM_COMMON_AT_COMMANDS,        /* bInterfaceProtocol */ \
   _notif_iInterface),                          /* iInterface */ \
   \
   /* Functional Descriptors */ \
   0x05,                                        /* bFunctionLength */ \
   0x24,                                        /* bDescriptorType - CS_INTERFACE */ \
   0x00,                                        /* bDescriptorSubtype - Header */ \
   0x10,                                        \
   0x01,                                        /* bcdCDC */ \
   \
   0x04,                                        /* bFunctionLength */ \
   0x24,                                        /* bDescriptorType - CS_INTERFACE */ \
   0x02,                                        /* bDescriptorSubtype - Abstract Control Management Functional Descriptor */ \
   0x07,                                        /* bmCapabilities */ \
   \
   0x05,                                        /* bFunctionLength */ \
   0x24,                                        /* bDescriptorType - CS_INTERFACE */ \
   0x06,                                        /* bDescriptorSubtype - Union Descriptor Functional Descriptor */ \
   _notif_if_num,                               /* bMasterInterface */ \
   _data_if_num,                                /* bSlaveInterface0 */ \
   \
   0x05,                                        /* bFunctionLength */ \
   0x24,                                        /* bDescriptorType - CS_INTERFACE */ \
   0x01,                                        /* bDescriptorSubtype - Call Management Functional Descriptor */ \
   0x01,                                        /* bmCapabilities */ \
   _data_if_num,                                /* bDataInterface */ \
   \
   /* Endpoint 1 descriptor */ \
   USB_ENDPOINT_DESC_TABLE_FILL( \
   _ep_notif,                                   /* Endpoint Address */ \
   USB_EP_DESC_DIR_IN,                          /* Endpoint direction: IN */ \
   USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,         /* Endpoint type:      INTERRUPT */ \
   0,                                           /* Endpoint synchronization: NOT USED */ \
   0,                                           /* Endpoint Usage type:      NOT USED */ \
   _ep_notif_mps,                               /*  wMaxPacketSize */ \
   CDC_VCOM_CCI_EP_BINTERVAL)                   /* bInterval */

#define CDC_VCOM_DCI_DESC(_data_if_num, _data_iInterface, _ep_in, _ep_in_mps, _ep_out, _ep_out_mps) \
   /* Data Class Interface */ \
   \
   \
   \
   /* Interface 1 Descriptor */ \
   USB_INTERFACE_DESC_TABLE_FILL( \
   _data_if_num,                                /* bInterfaceNumber */ \
   0x00,                                        /* bAlternateSetting */ \
   0x02,                                        /* bNumEndpoints */ \
   USB_CLASS_CDC_DATA,                          /* bInterfaceClass   Communication Class Interface */ \
   USB_SUBCLASS_CDC_DATA,                       /* bInterfaceSubclass */ \
   USB_PROTOCOL_CDC_DATA_NO_CLASS_SPECIFIC,     /* bInterfaceProtocol */ \
   _data_iInterface),                           /* iInterface */ \
   \
   /* Endpoint 2 descriptor */ \
   USB_ENDPOINT_DESC_TABLE_FILL( \
   _ep_in,                                      /* Endpoint Address */ \
   USB_EP_DESC_DIR_IN,                          /* Endpoint direction: IN */ \
   USB_EP_DESC_TRANSFER_TYPE_BULK,              /* Endpoint type:      BULK */ \
   0,                                           /* Endpoint synchronization: NOT USED */ \
   0,                                           /* Endpoint Usage type:      NOT USED */ \
   _ep_in_mps,                                  /*  wMaxPacketSize */ \
   CDC_VCOM_DCI_EP_IN_BINTERVAL),               /* bInterval */ \
   \
   /* Endpoint 3 descriptor */ \
   USB_ENDPOINT_DESC_TABLE_FILL( \
   _ep_out,                                     /* Endpoint Address */ \
   USB_EP_DESC_DIR_OUT,                         /* Endpoint direction: OUT */ \
   USB_EP_DESC_TRANSFER_TYPE_BULK,              /* Endpoint type:      BULK */ \
   0,                                           /* Endpoint synchronization: NOT USED */ \
   0,                                           /* Endpoint Usage type:      NOT USED */ \
   _ep_out_mps,                                 /*  wMaxPacketSize */ \
   CDC_VCOM_DCI_EP_OUT_BINTERVAL)               /* bInterval */

/**
 * @brief defines how many bytes are needed in configuration descriptor for single virtual COM port
 *
 * @return value 66 which says how many bytes of configuration descriptor are needed to describe fully single virtual COM port
 */
#define CDC_VCOM_CONFIG_DESC_PART_SIZE    (USB_DESC_TYPE_INTERFACE_ASSOCIATION_SIZE + CDC_VCOM_FUNCTIONAL_DESCRPTORS_SIZE + (2 * USB_DESC_TYPE_INTERFACE_SIZE) + (3 * USB_DESC_TYPE_ENDPOINT_SIZE))

/**
 * @brief defines whole structure of VCOM part of configuration descriptor.
 * This macro can be used to fill context of configuration descriptor on initialization.
 *
 * @return 66 bytes of configuration descriptor part which describe fully single virtual COM port
 *
 * @param _iFunction number of string which described VCOM. If not used, 0 shall be provided
 * @param _notif_if_num number of Communication Class Interface which is used to notify HOST about some actions (signals change, etc)
 * @param _notif_iInterface number of string which describes NOTIFICATION interface. If not used, 0 shall be provided
 * @param _ep_notif endpoint number used by Communication Class Interface. It is always INTERRUPT IN endpont. Called later as "NOTIFICATION" endpoint
 * @param _ep_notif_mps NOTIFICATION endpoint max packet size.
 * @param _data_if_num number of Data Class Interface which is used to to send VCOM data from and to HOST
 * @param _data_iInterface number of string which describes DATA interface. If not used, 0 shall be provided
 * @param _ep_in endpoint number used by Data Class Interface to send VCOM data to HOST. Called later as "DATA IN" endpoint
 * @param _ep_in_mps DATA IN endpoint max packet size
 * @param _ep_out endpoint number used by Data Class Interface to receive VCOM data from HOST. Called later as "DATA OUT" endpoint
 * @param _ep_out_mps DATA OUT endpoint max packet size
 */
#define CDC_VCOM_CONFIG_DESC_PART(_iFunction, _notif_if_num, _notif_iInterface, _ep_notif, _ep_notif_mps, _data_if_num, _data_iInterface, _ep_in, _ep_in_mps, _ep_out, _ep_out_mps) \
   USB_INTERFACE_ASSOCIATION_DESC_TABLE_FILL( \
   _notif_if_num,                               /* bFirstInterface */ \
   0x02,                                        /* bInterfaceCount */ \
   USB_CLASS_COMMUNICATION,                     /* bFunctionClass - Communication Class Interface */ \
   USB_SUBCLASS_COMM_ABSTRACT_CONTROL_MODEL,    /* bFunctionSubClass */ \
   USB_PROTOCOL_COMM_COMMON_AT_COMMANDS,        /* bFunctionProtocol */ \
   _iFunction),                                 /* iFunction */ \
   /* Communication Class Interface */ \
   CDC_VCOM_CCI_DESC(_notif_if_num, _notif_iInterface, _ep_notif, _ep_notif_mps, _data_if_num), \
   /* Data Class Interface */ \
   CDC_VCOM_DCI_DESC(_data_if_num, _data_iInterface, _ep_in, _ep_in_mps, _ep_out, _ep_out_mps)



#define CDC_VCOM_LINE_CODING_CONTAINER_SIZE  (sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t))

typedef struct CDC_VCOM_Physical_Params_DataTag
{
   uint32_t baudrate;
   uint8_t stop_bits;
   uint8_t partity;
   uint8_t data_bits;
#if(CDC_VCOM_MODE_DATA_AND_SIGNALS == CDC_VCOM_MODE)
   USBD_Bool_DT dtr;
   USBD_Bool_DT rts;
#endif
}CDC_VCOM_Physical_Params_DT;

typedef struct CDC_VCOM_Params_eXtendedTag
{
   struct
   {
      void (*change_params) (struct CDC_VCOM_Params_eXtendedTag *vcom);
#if(CDC_VCOM_MODE_DATA_AND_SIGNALS == CDC_VCOM_MODE)
      void (*change_dtr) (struct CDC_VCOM_Params_eXtendedTag *vcom);
      void (*change_rts) (struct CDC_VCOM_Params_eXtendedTag *vcom);
#endif
   }handlers;
   struct
   {
      USBDC_Params_XT *usbdc;
      uint16_t abstract_state;
      uint16_t country_setting;
      uint16_t duration_of_break;
#if(CDC_VCOM_MODE_DATA_AND_SIGNALS == CDC_VCOM_MODE)
      struct
      {
         USBD_REQUEST_Req_DT req;
         uint16_t data;
         USBD_Atomic_Bool_DT ring;
         USBD_Atomic_Bool_DT dsr;
         USBD_Atomic_Bool_DT dcd;
         USBD_Atomic_Bool_DT was_changed;
         USBD_Bool_DT send_ongoing;
      }serial_state_notification;
#endif
      CDC_VCOM_Physical_Params_DT comm_physical_params;
      struct
      {
         USBD_IOTP_Params_Ring_Infinite_Only_XT in;
         USBD_IOTP_Params_Ring_Infinite_Only_XT out;
         USBD_IOTP_Params_XT notif;
      }iotp;
      struct
      {
         uint8_t if_notif;
         uint8_t ep_notif;
         uint8_t ep_in;
         uint8_t ep_out;
      }hw;
      const char *name;
   }data;
}CDC_VCOM_Params_XT;



typedef void (*CDC_VCOM_On_Params_Change_HT)(CDC_VCOM_Params_XT *vcom);
typedef void (*CDC_VCOM_On_DTR_Change_HT)(CDC_VCOM_Params_XT *vcom);
typedef void (*CDC_VCOM_On_RTS_Change_HT)(CDC_VCOM_Params_XT *vcom);



void CDC_VCOM_Init(CDC_VCOM_Params_XT *cdc_vcom, Buff_Ring_XT *buff_in, Buff_Ring_XT *buff_out, const char *name);

void CDC_VCOM_Install_In_Config(USBDC_Params_XT *usbdc, CDC_VCOM_Params_XT *cdc_vcom, uint8_t notif_if_num);

const CDC_VCOM_Physical_Params_DT *CDC_VCOM_Get_Com_Physical_Params(CDC_VCOM_Params_XT *cdc_vcom);

void CDC_VCOM_Set_Change_Params_Event(CDC_VCOM_Params_XT *cdc_vcom, CDC_VCOM_On_Params_Change_HT on_params_change);

#if(CDC_VCOM_MODE_DATA_AND_SIGNALS == CDC_VCOM_MODE)

void CDC_VCOM_Set_Change_Dtr_Event(CDC_VCOM_Params_XT *cdc_vcom, CDC_VCOM_On_DTR_Change_HT on_dtr_change);

void CDC_VCOM_Set_Change_Rts_Event(CDC_VCOM_Params_XT *cdc_vcom, CDC_VCOM_On_RTS_Change_HT on_rts_change);

USBD_Bool_DT CDC_VCOM_Get_Dtr(CDC_VCOM_Params_XT *cdc_vcom);

USBD_Bool_DT CDC_VCOM_Get_Rts(CDC_VCOM_Params_XT *cdc_vcom);

void CDC_VCOM_Set_Ring(CDC_VCOM_Params_XT *cdc_vcom, USBD_Bool_DT state);

void CDC_VCOM_Set_Dsr(CDC_VCOM_Params_XT *cdc_vcom, USBD_Bool_DT state);

void CDC_VCOM_Set_Dcd(CDC_VCOM_Params_XT *cdc_vcom, USBD_Bool_DT state);

#else

#define CDC_VCOM_Set_Change_Dtr_Event(cdc_vcom, on_dtr_change)

#define CDC_VCOM_Set_Change_Rts_Event(cdc_vcom, on_rts_change)

#define CDC_VCOM_Get_Dtr(cdc_vcom)     USBD_TRUE

#define CDC_VCOM_Get_Rts(cdc_vcom)     USBD_TRUE

#define CDC_VCOM_Set_Ring(cdc_vcom, state)

#define CDC_VCOM_Set_Dsr(cdc_vcom, state)

#define CDC_VCOM_Set_Dcd(cdc_vcom, state)

#endif

Buff_Ring_XT *CDC_Vcom_Get_In_Buf(CDC_VCOM_Params_XT *cdc_vcom);

Buff_Ring_XT *CDC_Vcom_Get_Out_Buf(CDC_VCOM_Params_XT *cdc_vcom);



#endif

