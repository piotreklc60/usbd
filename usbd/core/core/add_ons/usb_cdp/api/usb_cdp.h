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

#ifndef USB_CDP_H_
#define USB_CDP_H_

/*
#ifndef USBD_CDP_USED_JUST_BY_USBD_CORE
#define USBD_CDP_USED_JUST_BY_USBD_CORE            0
#endif
*/

#define USB_STRING_COMPOSE_LENGTH_AND_TYPE(num_elems)   ((USB_DESC_TYPE_STRING * 256) + ((num_elems) * 2))

/* *****************************************************************************
            STANDARD DESCRIPTORS TYPES
   ***************************************************************************** */
#define USB_DESC_TYPE_DEVICE                      1
#define USB_DESC_TYPE_CONFIGURATION               2
#define USB_DESC_TYPE_STRING                      3
#define USB_DESC_TYPE_INTERFACE                   4
#define USB_DESC_TYPE_ENDPOINT                    5
#define USB_DESC_TYPE_DEVICE_QUALIFIER            6
#define USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION   7
#define USB_DESC_TYPE_INTERFACE_POWER             8
#define USB_DESC_TYPE_OTG                         9
#define USB_DESC_TYPE_DEBUG                       10
#define USB_DESC_TYPE_INTERFACE_ASSOCIATION       11

/* *****************************************************************************
            STANDARD DESCRIPTORS SIZES
   ***************************************************************************** */

#define USB_DESC_TYPE_DEVICE_SIZE                 18
#define USB_DESC_TYPE_CONFIGURATION_SIZE          9
#define USB_DESC_TYPE_INTERFACE_SIZE              9
#define USB_DESC_TYPE_ENDPOINT_SIZE               7
#define USB_DESC_TYPE_DEVICE_QUALIFIER_SIZE       10
#define USB_DESC_TYPE_INTERFACE_ASSOCIATION_SIZE  8

/* *****************************************************************************
            CONFIGURATION DESCRIPTOR TRANSFER TYPES (USB_Configuration_Desc_DT.bmAttributes.6,5)
   ***************************************************************************** */

#define USB_CFG_DESC_SELF_POWERED                 (0x40)
#define USB_CFG_DESC_NO_SELF_POWERED              0
#define USB_CFG_DESC_SELF_POWERED_MASK            (0x40)

#define USB_CFG_DESC_REMOTE_WAKEUP                (0x20)
#define USB_CFG_DESC_NO_REMOTE_WAKEUP             0
#define USB_CFG_DESC_REMOTE_WAKEUP_MASK           (0x20)

/* *****************************************************************************
            ENDPOINT DESCRIPTOR TRANSFER TYPES (USB_Endpoint_Desc_DT.bmAttributes.1-0)
   ***************************************************************************** */
#define USB_EP_DESC_TRANSFER_TYPE_CONTROL          0
#define USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS      1
#define USB_EP_DESC_TRANSFER_TYPE_BULK             2
#define USB_EP_DESC_TRANSFER_TYPE_INTERRUPT        3

#define USB_EP_DESC_TRANSFER_TYPE_MASK             0x3

/* *****************************************************************************
            ENDPOINT DESCRIPTOR SYNCHRONIZATION TYPES (USB_Endpoint_Desc_DT.bmAttributes.3-2 - Isochronous only)
   ***************************************************************************** */

#define USB_EP_DESC_SYNC_TYPE_NOT_USED             (0 * 4)
#define USB_EP_DESC_SYNC_TYPE_ISO_NO_SYNC          (0 * 4)
#define USB_EP_DESC_SYNC_TYPE_ISO_ASYNCHRONOUS     (1 * 4)
#define USB_EP_DESC_SYNC_TYPE_ISO_ADAPTIVE         (2 * 4)
#define USB_EP_DESC_SYNC_TYPE_ISO_SYNCHRONOUS      (3 * 4)
#define USB_EP_DESC_SYNC_TYPE_MASK                 (3 * 4)

/* *****************************************************************************
            ENDPOINT DESCRIPTOR USAGE TYPES (USB_Endpoint_Desc_DT.bmAttributes.5-4 - Isochronous and Interrupt(USB 3.0) only)
   ***************************************************************************** */

#define USB_EP_DESC_USAGE_TYPE_NOT_USED                        (0 * 16)
#define USB_EP_DESC_USAGE_TYPE_ISO_DATA_EP                     (0 * 16)
#define USB_EP_DESC_USAGE_TYPE_ISO_FEEDBACK_EP                 (1 * 16)
#define USB_EP_DESC_USAGE_TYPE_ISO_IMPLICIT_FEEDBACK_DATA_EP   (2 * 16)
#define USB_EP_DESC_USAGE_TYPE_IRQ_PERIODIC                    (0 * 16)
#define USB_EP_DESC_USAGE_TYPE_IRQ_NOTIFICATION                (1 * 16)
#define USB_EP_DESC_USAGE_TYPE_MASK                            (3 * 16)

/* *****************************************************************************
            ENDPOINT DESCRIPTOR DIRECTION TYPES (USB_Endpoint_Desc_DT.bEndpointAddress.7)
   ***************************************************************************** */

#define USB_EP_DESC_DIR_IN             (1 * 128)
#define USB_EP_DESC_DIR_OUT            (0 * 128)
#define USB_EP_DESC_DIR_INOUT_CONTROL  (0 * 128)
#define USB_EP_DESC_DIR_MASK           (1 * 128)

/* *****************************************************************************
            ENDPOINT DESCRIPTOR ADDRESS (USB_Endpoint_Desc_DT.bEndpointAddress.3-0)
   ***************************************************************************** */

#define USB_EP_DESC_ADDR_MASK    0x0F

/* *****************************************************************************
            FILL MACROES
   ***************************************************************************** */

#define USB_DEVICE_DESC_STRUCT_FILL(_bcdUSB, _bDeviceClass, _bDeviceSubclass, _bDeviceProtocol, _bMaxPacketSize0, _idVendor, _idProduct, _bcdDevice, _iManufacturer, _iProduct, _iSerialNumber, _bNumConfigurations) \
{ \
   (uint8_t)(USB_DESC_TYPE_DEVICE_SIZE), \
   (uint8_t)(USB_DESC_TYPE_DEVICE),      \
   (uint16_t)(_bcdUSB),          \
   (uint8_t )(_bDeviceClass),    \
   (uint8_t )(_bDeviceSubclass), \
   (uint8_t )(_bDeviceProtocol), \
   (uint8_t )(_bMaxPacketSize0), \
   (uint16_t)(_idVendor),        \
   (uint16_t)(_idProduct),       \
   (uint16_t)(_bcdDevice),       \
   (uint8_t )(_iManufacturer),   \
   (uint8_t )(_iProduct),        \
   (uint8_t )(_iSerialNumber),   \
   (uint8_t )(_bNumConfigurations) \
}

#define USB_SET_CONFIGURATION_DESC_W_TOTAL_LENGTH(_desc, _wTotalLength)    (_desc)[2] = (_wTotalLength) & 0xFF; (_desc)[3] = ((_wTotalLength) >> 8) & 0xFF

#define USB_CONFIGURATION_DESC_TABLE_FILL(_wTotalLength, _bNumInterfaces, _bConfigurationValue, _iConfiguration, _self_powered, _remote_wakeup, _max_current_mA) \
   (uint8_t)(USB_DESC_TYPE_CONFIGURATION_SIZE),\
   (uint8_t)(USB_DESC_TYPE_CONFIGURATION),     \
   (uint8_t)(_wTotalLength & 0xFF), \
   (uint8_t)(_wTotalLength / 256),  \
   (uint8_t)(_bNumInterfaces),      \
   (uint8_t)(_bConfigurationValue), \
   (uint8_t)(_iConfiguration),      \
   (uint8_t)(((_self_powered) & USB_CFG_DESC_SELF_POWERED_MASK) | ((_remote_wakeup) & USB_CFG_DESC_REMOTE_WAKEUP_MASK) | 0x80), \
   (uint8_t)((_max_current_mA) * 2)

#define USB_CONFIGURATION_DESC_STRUCT_FILL(_wTotalLength, _bNumInterfaces, _bConfigurationValue, _iConfiguration, _self_powered, _remote_wakeup, _max_current_mA) \
{ \
   (uint8_t)(USB_DESC_TYPE_CONFIGURATION_SIZE),\
   (uint8_t)(USB_DESC_TYPE_CONFIGURATION),     \
   { \
      (uint8_t)(_wTotalLength & 0xFF), \
      (uint8_t)(_wTotalLength / 256)  \
   }, \
   (uint8_t)(_bNumInterfaces),      \
   (uint8_t)(_bConfigurationValue), \
   (uint8_t)(_iConfiguration),      \
   (uint8_t)(((_self_powered) & USB_CFG_DESC_SELF_POWERED_MASK) | ((_remote_wakeup) & USB_CFG_DESC_REMOTE_WAKEUP_MASK) | 0x80), \
   (uint8_t)((_max_current_mA) * 2) \
}

#define USB_INTERFACE_ASSOCIATION_DESC_TABLE_FILL(_bFirstInterface, _bInterfaceCount, _bFunctionClass, _bFunctionSubClass, _bFunctionProtocol, _iFunction) \
   (uint8_t)(USB_DESC_TYPE_INTERFACE_ASSOCIATION_SIZE), \
   (uint8_t)(USB_DESC_TYPE_INTERFACE_ASSOCIATION),      \
   (uint8_t)(_bFirstInterface),     \
   (uint8_t)(_bInterfaceCount),     \
   (uint8_t)(_bFunctionClass),      \
   (uint8_t)(_bFunctionSubClass),   \
   (uint8_t)(_bFunctionProtocol),   \
   (uint8_t)(_iFunction)

#define USB_INTERFACE_DESC_TABLE_FILL(_bInterfaceNumber, _bAlternateSetting, _bNumEndpoints, _bInterfaceClass, _bFunctionSubClass, _bInterfaceProtocol, _iInterface) \
   (uint8_t)(USB_DESC_TYPE_INTERFACE_SIZE), \
   (uint8_t)(USB_DESC_TYPE_INTERFACE),      \
   (uint8_t)(_bInterfaceNumber),    \
   (uint8_t)(_bAlternateSetting),   \
   (uint8_t)(_bNumEndpoints),       \
   (uint8_t)(_bInterfaceClass),     \
   (uint8_t)(_bFunctionSubClass),   \
   (uint8_t)(_bInterfaceProtocol),  \
   (uint8_t)(_iInterface)

#define USB_ENDPOINT_DESC_TABLE_FILL(_ep_num, _ep_dir, _ep_type, _ep_sync/*iso*/, _ep_usage/*iso/irq3*/, _wMaxPacketSize, _bInterval) \
   (uint8_t)(USB_DESC_TYPE_ENDPOINT_SIZE),  \
   (uint8_t)(USB_DESC_TYPE_ENDPOINT),       \
   (uint8_t)(((_ep_num) & USB_EP_DESC_ADDR_MASK) | ((_ep_dir) & USB_EP_DESC_DIR_MASK)), \
   (uint8_t)(((_ep_type) & USB_EP_DESC_TRANSFER_TYPE_MASK) | ((_ep_sync)  & USB_EP_DESC_SYNC_TYPE_MASK) | ((_ep_usage)  & USB_EP_DESC_USAGE_TYPE_MASK)), \
   (uint8_t)(_wMaxPacketSize & 0xFF),        \
   (uint8_t)(_wMaxPacketSize / 256),         \
   (uint8_t)(_bInterval)

#define USB_ENDPOINT_DESC_STRUCT_FILL(_ep_num, _ep_dir, _ep_type, _ep_sync/*iso*/, _ep_usage/*iso/irq3*/, _wMaxPacketSize, _bInterval) \
{ \
   (uint8_t)(USB_DESC_TYPE_ENDPOINT_SIZE),  \
   (uint8_t)(USB_DESC_TYPE_ENDPOINT),       \
   (uint8_t)(((_ep_num) & USB_EP_DESC_ADDR_MASK) | ((_ep_dir) & USB_EP_DESC_DIR_MASK)), \
   (uint8_t)(((_ep_type) & USB_EP_DESC_TRANSFER_TYPE_MASK) | ((_ep_sync)  & USB_EP_DESC_SYNC_TYPE_MASK) | ((_ep_usage)  & USB_EP_DESC_USAGE_TYPE_MASK)), \
   { \
      (uint8_t)(_wMaxPacketSize & 0xFF),        \
      (uint8_t)(_wMaxPacketSize / 256)          \
   }, \
   (uint8_t)(_bInterval) \
}


typedef enum USB_EP_Direction_Enum_Tag
{
   USB_EP_DIRECTION_OUT   = USB_EP_DESC_DIR_OUT,
   USB_EP_DIRECTION_IN    = USB_EP_DESC_DIR_IN
}USB_EP_Direction_ET;

typedef enum USB_Max_Possible_Packet_Size_Enum_Tag
{
   USB_FS_CONTROL_MAX_POSSIBLE_PACKET_SIZE     = 64,
   USB_FS_INTERRUPT_MAX_POSSIBLE_PACKET_SIZE   = 64,
   USB_FS_BULK_MAX_POSSIBLE_PACKET_SIZE        = 64,
   USB_FS_ISOCHRONOUS_MAX_POSSIBLE_PACKET_SIZE = 1023,

   USB_HS_CONTROL_MAX_POSSIBLE_PACKET_SIZE     = 64,
   USB_HS_INTERRUPT_MAX_POSSIBLE_PACKET_SIZE   = 1024,
   USB_HS_BULK_MAX_POSSIBLE_PACKET_SIZE        = 512,
   USB_HS_ISOCHRONOUS_MAX_POSSIBLE_PACKET_SIZE = 1024
}USB_Max_Possible_Packet_Size_ET;

typedef struct USB_Device_Desc_Data_Tag
{
   uint8_t  bLength;
   uint8_t  bDescriptorType;
   uint16_t bcdUSB;
   uint8_t  bDeviceClass;
   uint8_t  bDeviceSubclass;
   uint8_t  bDeviceProtocol;
   uint8_t  bMaxPacketSize0;
   uint16_t idVendor;
   uint16_t idProduct;
   uint16_t bcdDevice;
   uint8_t  iManufacturer;
   uint8_t  iProduct;
   uint8_t  iSerialNumber;
   uint8_t  bNumConfigurations;
}USB_Device_Desc_DT;

typedef struct USB_Device_Qualifier_Desc_Data_Tag
{
   uint8_t  bLength;
   uint8_t  bDescriptorType;
   uint16_t bcdUSB;
   uint8_t  bDeviceClass;
   uint8_t  bDeviceSubclass;
   uint8_t  bDeviceProtocol;
   uint8_t  bMaxPacketSize0;
   uint8_t  bNumConfigurations;
   uint8_t  bReserved;
}USB_Device_Qualifier_Desc_DT;

typedef struct USB_Configuration_Desc_Data_Tag
{
   uint8_t  bLength;
   uint8_t  bDescriptorType;
   struct {
      uint8_t L;
      uint8_t H;
   } wTotalLength;
   uint8_t  bNumInterfaces;
   uint8_t  bConfigurationValue;
   uint8_t  iConfiguration;
   uint8_t  bmAttributes;
   uint8_t  bMaxPower;
}USB_Configuration_Desc_DT;

typedef struct USB_Interface_Association_Desc_Data_Tag
{
   uint8_t  bLength;
   uint8_t  bDescriptorType;
   uint8_t  bFirstInterface;
   uint8_t  bInterfaceCount;
   uint8_t  bFunctionClass;
   uint8_t  bFunctionSubClass;
   uint8_t  bFunctionProtocol;
   uint8_t  iFunction;
}USB_Interface_Association_Desc_DT;

typedef struct USB_Interface_Desc_Data_Tag
{
   uint8_t  bLength;
   uint8_t  bDescriptorType;
   uint8_t  bInterfaceNumber;
   uint8_t  bAlternateSetting;
   uint8_t  bNumEndpoints;
   uint8_t  bInterfaceClass;
   uint8_t  bInterfaceSubClass;
   uint8_t  bInterfaceProtocol;
   uint8_t  iInterface;
}USB_Interface_Desc_DT;

typedef struct USB_Endpoint_Desc_Data_Tag
{
   uint8_t  bLength;
   uint8_t  bDescriptorType;
   /**
    * from USB 3.0 standard:
    *
    * Bit 3...0: The endpoint number
    * Bit 7: Direction (ignored forcontrol endpoints):
    *    0 = OUT endpoint
    *    1 = IN endpoint
    */
   uint8_t  bEndpointAddress;
   /**
    * from USB 3.0 standard:
    *
    * This field describes the endpoint's attributes when it is configured using the bConfigurationValue.
    * Bits 1..0: Transfer Type
    *    00 = Control
    *    01 = Isochronous
    *    10 = Bulk
    *    11 = Interrupt
    *
    * If an interrupt endpoint, bits 5..2 are defined as follows (for USB2.0 - reserved for interrupt EPs):
    * Bits 3..2: Reserved
    * Bits 5..4: Usage Type
    *    00 = Periodic
    *    01 = Notification
    *    10 = Reserved
    *    11 = Reserved
    *
    * If isochronous, they are defined as follows:
    * Bits 3..2: Synchronization Type
    *    00 = No Synchronization
    *    01 = Asynchronous
    *    10 = Adaptive
    *    11 = Synchronous
    * Bits 5..4: Usage Type
    *    00 = Data endpoint
    *    01 = Feedback endpoint
    *    10 = Implicit feedback Data endpoint
    *    11 = Reserved
    *
    * If not an isochronous or interrupt endpoint, bits 5..2 are reserved and shall be set to zero.
    *
    * All other bits are reserved and shall be reset to zero. Reserved bits shall be ignored by the host.
    */
   uint8_t  bmAttributes;
   struct {
      uint8_t L;
      uint8_t H;
   }wMaxPacketSize;
   uint8_t  bInterval;
}USB_Endpoint_Desc_DT;


uint16_t USB_CDP_Get_Config_Desc_Total_Length(const uint8_t *desc);

uint16_t USB_CDP_Find_Interface (
      const uint8_t *desc,
      uint16_t desc_size,
      uint16_t start_address,
      uint8_t  interface_number,
      uint8_t  alternate_setting);

uint16_t USB_CDP_Find_Next_Interface (
      const uint8_t *desc,
      uint16_t desc_size,
      uint16_t start_address);

uint16_t USB_CDP_Find_Endpoint (
      const uint8_t *desc,
      uint16_t desc_size,
      uint16_t start_address,
      uint8_t  ep_num,
      USB_EP_Direction_ET dir);

uint16_t USB_CDP_Find_Next_Endpoint (
      const uint8_t *desc,
      uint16_t desc_size,
      uint16_t start_address);

void USB_CDP_Create_EP_Max_Used_Packet_Size_Table (
      uint8_t *descriptor,
      uint16_t *table);

#endif /*USBD_CCM_H_*/

