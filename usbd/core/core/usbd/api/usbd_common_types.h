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
 *  \brief    USB Device common types
 *  \details   This module defines common types used for all submodules
 *  \author   Piotr Wojtowicz
 *  \version   1.3.0.0
 *  \date     2012
 *  \pre      First initialize the system.
 *  \warning   Improper use can crash your application
 *  \copyright GNU Public License.
 */

#ifndef USBD_COMMON_TYPES_H_
#define USBD_COMMON_TYPES_H_

/** @defgroup USBD_COMMON_TYPES
 * @{
 */

#ifndef USBD_BOOL_TYPES_ET_EXTERNAL
/**
 * \typedef USBD_Bool_Types_ET
 */
typedef enum USBD_Bool_Types_Enum_Tag
{
   USBD_FALSE  = false,
   USBD_TRUE   = true
}USBD_Bool_Types_ET;
#endif



#ifndef USBD_BOOL_DT_EXTERNAL
/**
 * \typedef USBD_Bool_DT
 */
typedef bool_t USBD_Bool_DT;
#endif



#ifndef USBD_ATOMIC_BOOL_DT_EXTERNAL
/**
 * \typedef USBD_Atomic_Bool_DT
 */
typedef bool_t USBD_Atomic_Bool_DT;
#endif



#ifndef USBD_ATOMIC_UINT8_DT_EXTERNAL
/**
 * \typedef USBD_Atomic_Uint8_DT
 */
typedef uint8_t USBD_Atomic_Uint8_DT;
#endif



#ifndef USBD_ATOMIC_UINT16_DT_EXTERNAL
/**
 * \typedef USBD_Atomic_Uint16_DT
 */
typedef uint16_t USBD_Atomic_Uint16_DT;
#endif



#ifndef USBD_ATOMIC_UINT32_DT_EXTERNAL
/**
 * \typedef USBD_Atomic_Uint32_DT
 */
typedef uint32_t USBD_Atomic_Uint32_DT;
#endif



#ifndef USBD_ATOMIC_UINT64_DT_EXTERNAL
/**
 * \typedef USBD_Atomic_Uint64_DT
 */
typedef uint64_t USBD_Atomic_Uint64_DT;
#endif



#ifndef USBD_HVOID_HT_EXTERNAL
/**
 * \typedef USBD_Hvoid_HT
 */
typedef void (*USBD_Hvoid_HT) (void);
#endif



#ifndef USBD_VENDOR_DATA_XT_EXTERNAL
/**
 * \struct USBD_Vendor_Data_XT
 */
typedef union USBD_Vendor_Data_eXtended_Tag
{
   USBD_Hvoid_HT  hvoid;
   void          *pvoid;
   uint32_t       u32;
   uint16_t       u16;
   uint16_t       u16tab[2];
   uint8_t        u8;
   uint8_t        u8tab[4];
   USBD_Bool_DT   boolean;
   USBD_Atomic_Uint32_DT au32;
   USBD_Atomic_Uint16_DT au16;
   USBD_Atomic_Uint8_DT  au8;
   USBD_Atomic_Bool_DT   aboolean;
}USBD_Vendor_Data_XT;
#endif

/** @} */ /* USBD_COMMON_TYPES */


#endif
