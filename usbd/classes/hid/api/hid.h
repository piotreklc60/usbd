/** ---------------------------------------- LICENSE SECTION -------------------------------------------------------------------
 *
 * Copyright (c) 2025 Piotr Wojtowicz
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

#ifndef HID_H_
#define HID_H_

#ifndef STD_LIBS_H_
#include "std_libs.h"
#endif

#ifndef USBD_H_
#include "usbd.h"
#endif

/**
 * Following debug groups are used by DFU library and must be configured correctly:
 * HID_INIT
 * HID_IO
 * HID_EVENT
 * HID_REQ
 * HID_INVALID_PARAMS
 */



#ifndef HID_PHYSICAL_DESCRIPTORS_SUPPORT
#define HID_PHYSICAL_DESCRIPTORS_SUPPORT        USBD_FEATURE_PRESENT
#endif

#ifndef HID_REPORTS_DOUBLE_BUFFERING_SUPPORT
#define HID_REPORTS_DOUBLE_BUFFERING_SUPPORT    USBD_FEATURE_PRESENT
#endif

#ifndef HID_DEFAULT_IN_BINTERVAL
#define HID_DEFAULT_IN_BINTERVAL                1
#endif

#ifndef HID_DEFAULT_OUT_BINTERVAL
#define HID_DEFAULT_OUT_BINTERVAL               1
#endif



#define USBD_DESC_TYPE_HID                      0x21
#define USBD_DESC_TYPE_REPORT                   0x22
#define USBD_DESC_TYPE_PHYSICAL                 0x23

#define USBD_REPORT_REQ_INPUT                   0x0100
#define USBD_REPORT_REQ_OUTPUT                  0x0200
#define USBD_REPORT_REQ_FEATURE                 0x0300



#define HID_BINTERFACEPROTOCOL_TYPE_NONE        0
#define HID_BINTERFACEPROTOCOL_TYPE_KEYBOARD    1
#define HID_BINTERFACEPROTOCOL_TYPE_MOUSE       2



/**
 * Country codes. If you do not know which one shall be used, HID_COUNTRY_CODE_NOT_SUPPORTED is best for you.
 */
#define HID_COUNTRY_CODE_NOT_SUPPORTED          0
#define HID_COUNTRY_CODE_ARABIC                 1
#define HID_COUNTRY_CODE_BELGIAN                2
#define HID_COUNTRY_CODE_CANADIAN_BILINGUAL     3
#define HID_COUNTRY_CODE_CANADIAN_FRENCH        4
#define HID_COUNTRY_CODE_CZECH_REPUBLIC         5
#define HID_COUNTRY_CODE_DANISH                 6
#define HID_COUNTRY_CODE_FINNISH                7
#define HID_COUNTRY_CODE_FRENCH                 8
#define HID_COUNTRY_CODE_GERMAN                 9
#define HID_COUNTRY_CODE_GREEK                  10
#define HID_COUNTRY_CODE_HEBREW                 11
#define HID_COUNTRY_CODE_HUNGARY                12
#define HID_COUNTRY_CODE_INTERNATIONAL__ISO     13
#define HID_COUNTRY_CODE_ITALIAN                14
#define HID_COUNTRY_CODE_JAPAN_KATAKANA         15
#define HID_COUNTRY_CODE_KOREAN                 16
#define HID_COUNTRY_CODE_LATIN_AMERICAN         17
#define HID_COUNTRY_CODE_NETHERLANDS_DUTCH      18
#define HID_COUNTRY_CODE_NORWEGIAN              19
#define HID_COUNTRY_CODE_PERSIAN_FARSI          20
#define HID_COUNTRY_CODE_POLAND                 21
#define HID_COUNTRY_CODE_PORTUGUESE             22
#define HID_COUNTRY_CODE_RUSSIA                 23
#define HID_COUNTRY_CODE_SLOVAKIA               24
#define HID_COUNTRY_CODE_SPANISH                25
#define HID_COUNTRY_CODE_SWEDISH                26
#define HID_COUNTRY_CODE_SWISS_FRENCH           27
#define HID_COUNTRY_CODE_SWISS_GERMAN           28
#define HID_COUNTRY_CODE_SWITZERLAND            29
#define HID_COUNTRY_CODE_TAIWAN                 30
#define HID_COUNTRY_CODE_TURKISH_Q              31
#define HID_COUNTRY_CODE_UK                     32
#define HID_COUNTRY_CODE_US                     33
#define HID_COUNTRY_CODE_YOGOSLAVIA             34
#define HID_COUNTRY_CODE_TURKISH_F              35


/**
 * @brief defines whole structure of HID part of configuration descriptor.
 * This macro can be used to fill context of configuration descriptor on initialization.
 *
 * @return 25 bytes of configuration descriptor part which describe fully DFU function
 *
 * @param _hid_if_num number of Interface which is used by the HOST to commmunicate with DFU functionality
 * @param _hid_iInterface number of string which describes DFU interface. If not used, 0 shall be provided
 * @param _hid_protocol protocol used by the device. Possible: USB_PROTOCOL_HID_NOT_BOOTABLE, USB_PROTOCOL_HID_BOOTABLE_KEYBOARD and USB_PROTOCOL_HID_BOOTABLE_MOUSE.
 * @param _ep_in endpoint number used by to send reports to the HOST.
 * @param _ep_in_mps IN endpoint max packet size
 * @param _country_code code of the country for example for keyboards when specific keys are supported
 * @param _report_desc_length size of report descriptor
 */
#define HID_DESC_PART(_hid_if_num, _hid_iInterface, _hid_protocol, _ep_in, _ep_in_mps, _country_code, _report_desc_length) \
   /* Interface <_hid_if_num> Descriptor */ \
   USB_INTERFACE_DESC_TABLE_FILL( \
   _hid_if_num,                                 /* bInterfaceNumber */     \
   0x00,                                        /* bAlternateSetting */    \
   0x01,                                        /* bNumEndpoints */        \
   USB_CLASS_HID,                               /* bInterfaceClass   APP Specific Interface */ \
   ((_hid_protocol) == USB_PROTOCOL_HID_NOT_BOOTABLE) ? USB_SUBCLASS_HID_NOT_BOOTABLE : USB_SUBCLASS_HID_BOOTABLE,/* bInterfaceSubclass */  \
   _hid_protocol,                               /* bInterfaceProtocol */   \
   _hid_iInterface),                            /* iInterface */           \
   \
   /* Interface HID Descriptor */ \
   0x09,                                        /* bLength */              \
   USBD_DESC_TYPE_HID,                          /* bDescriptorType */      \
   0x01,                                        /* bcdHID */               \
   0x01, \
   _country_code,                               /* bCountryCode */         \
   0x01,                                        /* bNumDescriptors */      \
   USBD_DESC_TYPE_REPORT,                       /* bDescriptorType */      \
   (uint8_t)(_report_desc_length & 0xFF),       /* bDescriptorLength */    \
   (uint8_t)((_report_desc_length>>8) & 0xFF),                             \
   /* Endpoint <_ep_in> descriptor */ \
   USB_ENDPOINT_DESC_TABLE_FILL( \
   _ep_in,                                      /* Endpoint Address */        \
   USB_EP_DESC_DIR_IN,                          /* Endpoint direction: IN */  \
   USB_EP_DESC_TRANSFER_TYPE_INTERRUPT,         /* Endpoint type:      INTERRUPT */       \
   0,                                           /* Endpoint synchronization: NOT USED */  \
   0,                                           /* Endpoint Usage type:      NOT USED */  \
   _ep_in_mps,                                  /* wMaxPacketSize */       \
   HID_DEFAULT_IN_BINTERVAL)                    /* bInterval */


typedef struct HID_Descriptor_DataTag
{
   uint8_t bLength;
   uint8_t bDescriptorType;
   uint8_t bcdHID_L;
   uint8_t bcdHID_H;
   uint8_t bCountryCode;
   uint8_t bNumDescriptors;
   uint8_t bDescriptorType_2;
   uint8_t wDescriptorLength_L;
   uint8_t wDescriptorLength_H;
}HID_Descriptor_DT;



typedef struct HID_Report_eXtendedTag
{
   const char *name;
   struct
   {
      /**
       * function called after report has been sent to the HOST. It is used for IN and FEATURE reports.
       * Type of this function is @see HID_On_Report_HT
       */
      void (*in) (struct HID_Report_eXtendedTag *report, USBD_Bool_DT is_feature);
      /**
       * function called after report has been received from the HOST. It is used for OUT and FEATURE reports.
       * Type of this function is @see HID_On_Report_HT
       */
      void (*out) (struct HID_Report_eXtendedTag *report, USBD_Bool_DT is_feature);
   } report_events;
   struct
   {
      /**
       * pointer to the user buffer where IN report is being edited by the user
       */
      uint8_t *in;

      /**
       * pointer to the user buffer where OUT report is being parsed by the user
       */
      uint8_t *out;

      uint8_t *feature;
   } report;

#if(USBD_FEATURE_PRESENT == HID_REPORTS_DOUBLE_BUFFERING_SUPPORT)
   struct
   {
      /**
       * pointer to the second buffer where IN report is being mirrored for the moment of sending to the HOST.
       * It is also used to compare if the report has been changed.
       * This parameter is optional - if user want to use only on one buffer then this variable shall be set to USBD_MAKE_INVALID_PTR(uint8_t).
       */
      uint8_t *in;

      /**
       * pointer to the second buffer to which OUT report is being received from the HOST.
       * It is also used to compare if the report has been changed.
       * This parameter is optional - if user want to use only on one buffer then this variable shall be set to USBD_MAKE_INVALID_PTR(uint8_t).
       */
      uint8_t *out;

      /**
       * pointer to the second buffer to which report is being received from the HOST /
       * in which report is being mirrored for the moment of sending to the HOST.
       * It is also used to compare if the report has been changed.
       * This parameter is optional - if user want to use only on one buffer then this variable shall be set to USBD_MAKE_INVALID_PTR(uint8_t).
       */
      uint8_t *feature;
   } report_second_buffer;
#endif
   /**
    * Reports size. If reports contain report ID then first byte of the buffer must contain this field and below size variables must include it.
    */
   struct
   {
      uint8_t in;
      uint8_t out;
      uint8_t feature;
   } report_size;
   uint8_t  idle_rate;
   uint32_t last_report_time;
   uint8_t report_id;
   USBD_Atomic_Bool_DT in_has_been_changed;
   USBD_Atomic_Bool_DT out_has_been_changed;
   USBD_Atomic_Bool_DT is_sending;
} HID_Report_XT;

/**
 * @brief type of a function called after report has been updated (sent to the HOST / received from the HOST).
 *
 * @param report pointer to the report for which update is called
 * @param is_feature USBD_TRUE if update is called for FEATURE report, false for IN/OUT.
 */
typedef void (*HID_On_Report_HT) (HID_Report_XT *report, USBD_Bool_DT is_feature);


typedef struct HID_Params_eXtendedTag
{
   // struktura z polami najczesciej uzywanymi przez uzytkownika
   // wymaga ustawienia wszystkich pol, chyba ze zaznaczone jest inaczej
   struct
   {
      // wskaznik do deskryptora reportow
      const uint8_t *report_descriptor;

      // rozmiar deskryptora reportow
      uint16_t report_descriptor_size;

      // zmienna mowiaca o tym, czy pracujemy aktualnie
      // w trybie BOOT czy REPORT (dla urzadzen bootowalnych)
      // pole zerowane przy inicjowaniu struktury, nie wymaga
      // ustawiania wartosci poczatkowej
      uint8_t boot_protocol_value;

      // liczba reportow zaimplementowanych w interfejsie -
      // jezeli uzywany jest tylko jeden report
      // i nie uzywane sa numery reportow (Report_ID)
      // wowczas zmienna ta wynosi jeden
      // jezeli uzywane sa numery reportow, wowczas
      // ta zmienna musi uwzgledniac tez nieuzywany report 0
      // dlatego zmienna ta musi byc o jeden wieksza
      // od faktycznej liczby reportow, np dla 2 reportow
      // zmienna ta wynosi 3
      uint8_t num_reports;

      // wskaznik do tablicy zawierajacej wskazniki
      // do struktor opisujacych reporty
      HID_Report_XT *reports_table;

      uint32_t sof_tick;

      /* report which is currently during transfer by IN endpoint */
      uint8_t report_in_use;
   }report_params;

   struct
   {
      USBD_IOTP_EVENT_Params_XT in_iotp;
      uint8_t ep_in;
      uint8_t ep_out;
      uint8_t if_num;
      /**
       * copy of bInterfaceProtocol field form HID interface descriptor
       * for use when device is bootable
       */
      uint8_t bInterfaceProtocol;
   }hw;
} HID_Params_XT;



void HID_Reports_Tab_Init(HID_Report_XT *reports_tab, uint8_t reports_tab_num_elems);
void HID_Report_Set_Name(HID_Report_XT *reports_tab, uint8_t report_id, char *name);
void HID_Report_Set_In(HID_Report_XT *reports_tab, uint8_t report_id, void *report, void *report_second_buf, uint8_t size);
void HID_Report_Set_Out(HID_Report_XT *reports_tab, uint8_t report_id, void *report, void *report_second_buf, uint8_t size);
void HID_Report_Set_Feature(HID_Report_XT *reports_tab, uint8_t report_id, void *report, void *report_second_buf, uint8_t size);
void HID_Report_Set_In_Event(HID_Report_XT *reports_tab, uint8_t report_id, HID_On_Report_HT event);
void HID_Report_Set_Out_Event(HID_Report_XT *reports_tab, uint8_t report_id, HID_On_Report_HT event);
void HID_Report_Clear_In_After_Each_Update(HID_Report_XT *reports_tab, uint8_t report_id);

void HID_Init(
   HID_Params_XT *hid,
   const uint8_t *report_descriptor,
   uint16_t       report_descriptor_size,
   HID_Report_XT *reports_tab,
   uint8_t        num_reports);

void HID_Install_In_Config(USBDC_Params_XT *usbdc, HID_Params_XT *hid, uint8_t hid_if_num);

void HID_Mark_Report_In_Changed(HID_Params_XT *hid, uint8_t report_id);
void HID_Clear_Report_Out_Changed(HID_Params_XT *hid, uint8_t report_id);

#endif

