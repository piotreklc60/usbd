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

#ifndef DFU_H_
#define DFU_H_

#ifndef USBD_H_
#include "usbd.h"
#endif

/**
 * Following debug groups are used by DFU library and must be configured correctly:
 * DFU_INIT
 * DFU_STATE
 * DFU_ONOFF
 * DFU_EVENT
 * DFU_REQ
 * DFU_INVALID_PARAMS
 */



#ifndef DFU_APP_MODE_SUPPORT
#define DFU_APP_MODE_SUPPORT                 USBD_FEATURE_PRESENT
#endif


#ifndef DFU_DFU_MODE_SUPPORT
#define DFU_DFU_MODE_SUPPORT                 USBD_FEATURE_PRESENT
#endif


#ifndef DFU_UPLOAD_SUPPORT
#define DFU_UPLOAD_SUPPORT                   USBD_FEATURE_PRESENT
#endif


#ifndef DFU_DNLOAD_SUPPORT
#define DFU_DNLOAD_SUPPORT                   USBD_FEATURE_PRESENT
#endif


#ifndef DFU_WILL_DETACH_SUPPORT
#define DFU_WILL_DETACH_SUPPORT              USBD_FEATURE_PRESENT
#endif


#ifndef DFU_MULTIPLE_MEMORIES_SUPPORT
#if(USBD_MAX_NUM_ALTERNATE_INTERFACE_SETTINGS > 0)
#define DFU_MULTIPLE_MEMORIES_SUPPORT        USBD_FEATURE_PRESENT
#else
#define DFU_MULTIPLE_MEMORIES_SUPPORT        USBD_FEATURE_NOT_PRESENT
#endif
#endif


#ifndef DFU_W_TRANSFER_SIZE
#define DFU_W_TRANSFER_SIZE                  2048
#endif


#ifndef DFU_W_DETACH_TIMEOUT
/**
 * @brief time in miliseconds in which device will switch to DFU/APP mode
 */
#define DFU_W_DETACH_TIMEOUT                 100
#endif


#ifndef DFU_BCD_DFU_VERSION
#define DFU_BCD_DFU_VERSION                  0x100
#endif


#if(USBD_FEATURE_PRESENT == DFU_DNLOAD_SUPPORT)
#define DFU_BMATTRIBUTES_BIT_CAN_DNLOAD               (1 << 0)
#else
#define DFU_BMATTRIBUTES_BIT_CAN_DNLOAD               (0 << 0)
#endif
#if(USBD_FEATURE_PRESENT == DFU_UPLOAD_SUPPORT)
#define DFU_BMATTRIBUTES_BIT_CAN_UPLOAD               (1 << 1)
#else
#define DFU_BMATTRIBUTES_BIT_CAN_UPLOAD               (0 << 1)
#endif
#define DFU_BMATTRIBUTES_BIT_MANIFESTATION_TOLERANT   (1 << 2)
#if(USBD_FEATURE_PRESENT == DFU_WILL_DETACH_SUPPORT)
#define DFU_BMATTRIBUTES_BIT_WILL_DETACH              (1 << 3)
#else
#define DFU_BMATTRIBUTES_BIT_WILL_DETACH              (0 << 3)
#endif



#define DFU_APP_MODE_DESC_PART(_dfu_if_num, _dfu_iInterface) \
   /* Interface <_dfu_if_num> Descriptor */ \
   USB_INTERFACE_DESC_TABLE_FILL( \
   _dfu_if_num,                                 /* bInterfaceNumber */  \
   0x00,                                        /* bAlternateSetting */ \
   0x00,                                        /* bNumEndpoints */     \
   0xFE,                                        /* bInterfaceClass   Communication Class Interface */ \
   0x01,                                        /* bInterfaceSubclass */ \
   0x01,                                        /* bInterfaceProtocol */ \
   _dfu_iInterface),                            /* iInterface */ \
   \
   /* Run-Time DFU Functional Descriptor */ \
   0x09,                                        /* bLength */ \
   0x21,                                        /* DFU  FUNCTIONAL descriptor type */ \
   DFU_BMATTRIBUTES_BIT_CAN_DNLOAD \
   | DFU_BMATTRIBUTES_BIT_CAN_UPLOAD \
   | DFU_BMATTRIBUTES_BIT_MANIFESTATION_TOLERANT \
   | DFU_BMATTRIBUTES_BIT_WILL_DETACH,          /* bmAttributes */ \
   (uint8_t)(DFU_W_DETACH_TIMEOUT & 0xFF),      /* USBDC_DFU_wDetachTimeOut_L */ \
   (uint8_t)((DFU_W_DETACH_TIMEOUT>>8) & 0xFF), /* USBDC_DFU_wDetachTimeOut_H */ \
   (uint8_t)(DFU_W_TRANSFER_SIZE & 0xFF),       /* USBDC_DFU_wTransferSize_L */  \
   (uint8_t)((DFU_W_TRANSFER_SIZE>>8) & 0xFF),  /* USBDC_DFU_wTransferSize_H */  \
   (uint8_t)(DFU_BCD_DFU_VERSION & 0xFF),       /* USBDC_DFU_bcdDFUVersion_L */  \
   (uint8_t)((DFU_BCD_DFU_VERSION>>8) & 0xFF)   /* USBDC_DFU_bcdDFUVersion_H */



#define DFU_DFU_MODE_DESC_PART(_dfu_memory_id, _dfu_iInterface) \
   /* Interface <_dfu_if_num> Descriptor */ \
   USB_INTERFACE_DESC_TABLE_FILL( \
   0x00,                                        /* bInterfaceNumber */  \
   _dfu_memory_id,                              /* bAlternateSetting */ \
   0x00,                                        /* bNumEndpoints */     \
   0xFE,                                        /* bInterfaceClass   Communication Class Interface */ \
   0x01,                                        /* bInterfaceSubclass */ \
   0x02,                                        /* bInterfaceProtocol */ \
   _dfu_iInterface),                            /* iInterface */ \
   \
   /* Run-Time DFU Functional Descriptor */ \
   0x09,                                        /* bLength */ \
   0x21,                                        /* DFU  FUNCTIONAL descriptor type */ \
   DFU_BMATTRIBUTES_BIT_CAN_DNLOAD \
   | DFU_BMATTRIBUTES_BIT_CAN_UPLOAD \
   | DFU_BMATTRIBUTES_BIT_MANIFESTATION_TOLERANT \
   | DFU_BMATTRIBUTES_BIT_WILL_DETACH,          /* bmAttributes */ \
   (uint8_t)(DFU_W_DETACH_TIMEOUT & 0xFF),      /* USBDC_DFU_wDetachTimeOut_L */ \
   (uint8_t)((DFU_W_DETACH_TIMEOUT>>8) & 0xFF), /* USBDC_DFU_wDetachTimeOut_H */ \
   (uint8_t)(DFU_W_TRANSFER_SIZE & 0xFF),       /* USBDC_DFU_wTransferSize_L */  \
   (uint8_t)((DFU_W_TRANSFER_SIZE>>8) & 0xFF),  /* USBDC_DFU_wTransferSize_H */  \
   (uint8_t)(DFU_BCD_DFU_VERSION & 0xFF),       /* USBDC_DFU_bcdDFUVersion_L */  \
   (uint8_t)((DFU_BCD_DFU_VERSION>>8) & 0xFF)   /* USBDC_DFU_bcdDFUVersion_H */

#define DFU_FUNCTIONAL_DESCRIPTOR_SIZE       9

/**
 * @brief defines how many bytes are needed in configuration descriptor for DFU APP-mode descriptor
 *
 * @return value 18 which says how many bytes of configuration descriptor are needed to describe DFU feature
 */
#define DFU_CONFIG_DESC_PART_SIZE            (USB_DESC_TYPE_INTERFACE_SIZE + DFU_FUNCTIONAL_DESCRIPTOR_SIZE)


#define DFU_SET_PART_DOWNLOAD_READY_HANDLER(_dfu, _handler)    (_dfu)->user_data.handlers.part_download_ready = (_handler)
#define DFU_SET_PART_UPLOAD_PREPARE_HANDLER(_dfu, _handler)    (_dfu)->user_data.handlers.part_upload_prepare = (_handler)
#define DFU_SET_SET_MEMORY_ID_HANDLER(_dfu, _handler)          (_dfu)->user_data.handlers.set_memory_id = (_handler)
#define DFU_SET_USER_EVENT_HANDLER(_dfu, _handler)             (_dfu)->user_data.handlers.user_event = (_handler)
#define DFU_SET_IS_APP_CORRECT_HANDLER(_dfu, _handler)         (_dfu)->user_data.handlers.is_app_correct = (_handler)

#define DFU_GET_STATE(_dfu)                                    ((DFU_bState_ET)((_dfu)->core.status.bState))


typedef enum
{
   DFU_BSTATE_APP_IDLE                 = 0,
   DFU_BSTATE_APP_DETACH               = 1,
   DFU_BSTATE_DFU_IDLE                 = 2,
   DFU_BSTATE_DFU_DNLOAD_SYNC          = 3,
   DFU_BSTATE_DFU_DNBUSY               = 4,
   DFU_BSTATE_DFU_DNLOAD_IDLE          = 5,
   DFU_BSTATE_DFU_MANIFEST_SYNC        = 6,
   DFU_BSTATE_DFU_MANIFEST             = 7,
   DFU_BSTATE_DFU_MANIFEST_WAIT_RESET  = 8,
   DFU_BSTATE_DFU_UPLOAD_IDLE          = 9,
   DFU_BSTATE_DFU_ERROR                = 10
}DFU_bState_ET;

typedef enum
{
   DFU_BSTATUS_OK,
   DFU_BSTATUS_ERR_TARGET,
   DFU_BSTATUS_ERR_FILE,
   DFU_BSTATUS_ERR_WRITE,
   DFU_BSTATUS_ERR_ERASE,
   DFU_BSTATUS_ERR_CHECK_ERASED,
   DFU_BSTATUS_ERR_PROG,
   DFU_BSTATUS_ERR_VERIFY,
   DFU_BSTATUS_ERR_ADDRESS,
   DFU_BSTATUS_ERR_NOTDONE,
   DFU_BSTATUS_ERR_FIRMWARE,
   DFU_BSTATUS_ERR_VENDOR,
   DFU_BSTATUS_ERR_USBR,
   DFU_BSTATUS_ERR_POR,
   DFU_BSTATUS_ERR_UNKNOWN,
   DFU_BSTATUS_ERR_STALLEDPKT
}DFU_bStatus_ET;

typedef struct
{
   uint8_t bStatus;
   uint8_t bwPollTimeout_L;
   uint8_t bwPollTimeout_M;
   uint8_t bwPollTimeout_H;
   uint8_t bState;
   uint8_t iString;
}DFU_Status_DT;


typedef enum
{
#if(USBD_FEATURE_PRESENT == DFU_WILL_DETACH_SUPPORT)
   /* user shall deconfigure USBD, delay and configure in DFU mode (or jump to bootloader) */
   DFU_USER_EVENT_DETACH_GO_TO_APP  = 0,
   /* user shall deconfigure USBD, delay and configure in APP mode (or jump to application) */
   DFU_USER_EVENT_DETACH_GO_TO_DFU  = 1,
#else
   /* user shall configure in DFU mode (or jump to bootloader) */
   DFU_USER_EVENT_RESET_GO_TO_APP   = 0,
   /* user shall configure in APP mode (or jump to application) */
   DFU_USER_EVENT_RESET_GO_TO_DFU   = 1,
#endif
   /* image download has been completed and now shall occur installation (if not done yet during download process) */
   DFU_USER_EVENT_MANIFEST_IMAGE    = 2,

}DFU_Event_ET;

struct DFU_Params_eXtendedTag;

/**
 * @brief Pointer to function called in one of cases described in DFU_Event_ET
 */
typedef void (*DFU_Event_HT) (struct DFU_Params_eXtendedTag *dfu, DFU_Event_ET event);

#if(USBD_FEATURE_PRESENT == DFU_MULTIPLE_MEMORIES_SUPPORT)
/**
 * @brief Pointer to function called when HOST changes DFU Interface alternate setting.
 * It is done when different memory is intended to be programmed.
 *
 * @param number of memory to be set
 *
 * @return function shall return state of memory. If setting memory succeeded and memory contain correct image
 * then DFU_BSTATUS_OK shall be returned. If something went wrong, then error acording to DFU_bStatus_ET values.
 */
typedef DFU_bStatus_ET (*DFU_Set_Memory_Id_HT) (struct DFU_Params_eXtendedTag *dfu, uint8_t memory_id);
#endif

/**
 * @brief Type of user download handler called at the end of each packet download.
 * For download it is called when piece of data has been successfully received.
 *
 * @param buffer pointer to the pointer. During DFU_DFU_Init there was provided firstly used download buffer.
 * During DOWNLOAD here we receive pointer to a variable where library keeps pointer to user buffer. User can change it during
 * this function execution (for example if we use two buffers - one for receiving from HOST,
 * second one is used to program FLASH so here we can swap them).
 * @param length length of the data downloaded
 * @param packet_number number of the current packet
 *
 * @return function shall return status of the operation. If operation succeeded then DFU_BSTATUS_OK shall be returned.
 * If something went wrong, then error acording to DFU_bStatus_ET values.
 */
typedef DFU_bStatus_ET (*DFU_Download_HT)(struct DFU_Params_eXtendedTag *dfu, uint8_t **buffer, uint16_t length, uint16_t packet_number);

/**
 * @brief Type of user upload or download handler called on the begining on each action.
 * For upload it is called on the begining of sending piece of data to the HOST - user must provide piece of data.
 *
 * @param buffer pointer to the pointer.
 * During UPLOAD user must provide pointer to the buffer with part of the data to be sent to the HOST. Here is also up to the user
 * if he uses only one buffer or more.
 * @param length pointer to a variable where uploaded data length shall be returned (max DFU_W_TRANSFER_SIZE)
 * @param packet_number number of the current packet
 *
 * @return function shall return status of the operation. If operation succeeded then DFU_BSTATUS_OK shall be returned.
 * If something went wrong, then error acording to DFU_bStatus_ET values.
 */
typedef DFU_bStatus_ET (*DFU_Upload_HT)(struct DFU_Params_eXtendedTag *dfu, uint8_t **buffer, uint16_t *length, uint16_t packet_number);


typedef struct DFU_Params_eXtendedTag
{
   struct
   {
      struct
      {
#if(USBD_FEATURE_PRESENT == DFU_DFU_MODE_SUPPORT)
#if(USBD_FEATURE_PRESENT == DFU_DNLOAD_SUPPORT)
         /**
          * pointer to a function called when DNLOAD data packet
          * has beed succesfully received
          */
         DFU_Download_HT         part_download_ready;
#endif

         /**
          * pointer to function called when UPLOAD request
          * has been received from Host
          */
#if(USBD_FEATURE_PRESENT == DFU_UPLOAD_SUPPORT)
         DFU_Upload_HT           part_upload_prepare;
#endif

#if(USBD_FEATURE_PRESENT == DFU_MULTIPLE_MEMORIES_SUPPORT)
         DFU_Set_Memory_Id_HT    set_memory_id;
#endif
#endif

         DFU_Event_HT            user_event;
      }handlers;
   }user_data;



   struct
   {
      USBDC_Params_XT *usbdc;
#if(USBD_FEATURE_PRESENT == DFU_DFU_MODE_SUPPORT)
      /**
       * pointer to buffer for data received from Host in DNLOAD
       * mode or for data to be send in UPLOAD mode.
       */
      uint8_t *transfer_buffer;
      uint16_t transfer_data_size;
      /* number of packet received from Host */
      uint16_t packet_number;
#endif

#if(USBD_FEATURE_PRESENT == USBD_SOF_TICKS_SUPPORTED)
      uint32_t bwPollTimeout;
#endif

      DFU_Status_DT status;

      /* number of the DFU interface */
      uint8_t interface_num;
      uint8_t memory_id;

      USBD_Bool_DT manifestation_done;
#if((USBD_FEATURE_PRESENT != USBD_SOF_TICKS_SUPPORTED) && (USBD_FEATURE_PRESENT != DFU_WILL_DETACH_SUPPORT))
      USBD_Bool_DT wait_for_reset;
#endif
   }core;
}DFU_Params_XT;






#if 0
typedef struct
{
   struct
   {
#if(USBD_FEATURE_PRESENT == DFU_DFU_MODE_SUPPORT)
#if(USBD_FEATURE_PRESENT == DFU_DNLOAD_SUPPORT)
      /**
       * pointer to a function called when DNLOAD data packet
       * has beed succesfully received
       */
      DFU_UpDownload_HT DFU_DNLOAD_handler;
#endif

      // pointer to function called when UPLOAD request
      // has been received from Host
#if(USBD_FEATURE_PRESENT == DFU_UPLOAD_SUPPORT)
      DFU_UpDownload_HT DFU_UPLOAD_handler;
#endif

      DFU_Event_HT user_event;

      DFU_Is_App_Correct_HT is_app_correct;

      DFU_Set_Memory_Id_HT set_memory_id;
#endif

#if((DFU_APP_MODE_SUPPORT == USBD_FEATURE_ON) && (DFU_WILL_DETACH_SUPPORT == USBD_FEATURE_ON))
      uint32_t DFU_detach_timeout;
#endif

      uint8_t DFU_bmAttributes;
   }user_data;



   struct
   {
      DFU_Params_XT *usbdc;

      int32_t DFU_event_ID;

      DFU_status_type DFU_state;

#if(DFU_DFU_MODE_SUPPORT == USBD_FEATURE_ON)
      // number of packet received from Host
      uint16_t DFU_transfer_packet_number;

      // pointer to buffer for data received from Host in DNLOAD
      // mode or for data to be send in UPLOAD mode.
      uint8_t *DFU_transfer_buffer;

      // indicates that device is in manifestation phase or not
#if((DFU_DNLOAD_SUPPORT == USBD_FEATURE_ON) && (DFU_MANIFESTATION_TOLERANT_SUPPORT == USBD_FEATURE_ON))
      uint8_t DFU_manifestation_phase = 0;
#endif
#endif

      // number of the DFU interface
      uint8_t DFU_interface;
   }extended_data;
}DFU_Params_XT;
#endif


void DFU_Init(DFU_Params_XT *dfu);


#if(USBD_FEATURE_PRESENT == DFU_APP_MODE_SUPPORT)
void DFU_APP_Install_In_Config(USBDC_Params_XT *usbdc, DFU_Params_XT *dfu, uint8_t interface_num);
#endif


#if(USBD_FEATURE_PRESENT == DFU_DFU_MODE_SUPPORT)
void DFU_DFU_Install_In_Config(USBDC_Params_XT *usbdc, DFU_Params_XT *dfu, uint8_t *buffer);
#endif



#if(USBD_FEATURE_PRESENT == USBD_SOF_TICKS_SUPPORTED)
void DFU_set_bwPollTimeout(DFU_Params_XT *dfu, uint32_t timeout);
#endif


#endif /*DFU_H_*/
