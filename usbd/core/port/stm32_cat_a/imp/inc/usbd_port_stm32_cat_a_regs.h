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

#ifndef USBD_PORT_STM32_CAT_A_REGS_H_
#define USBD_PORT_STM32_CAT_A_REGS_H_

#include "std_libs.h"


/*  non-USB part - taken from STM32 libs */

#define STM32_RCC_BASE      ((uint32_t)0x40021000)

typedef struct
{
  volatile uint32_t CR;
  volatile uint32_t CFGR;
  volatile uint32_t CIR;
  volatile uint32_t APB2RSTR;
  volatile uint32_t APB1RSTR;
  volatile uint32_t AHBENR;
  volatile uint32_t APB2ENR;
  volatile uint32_t APB1ENR;
  volatile uint32_t BDCR;
  volatile uint32_t CSR;
} STM32_RCC_TypeDef;

#define STM32_RCC   ((STM32_RCC_TypeDef *) STM32_RCC_BASE)




/*  USB part  */
/**
 * Registers bits definitions
 */
#define USBD_STM32_CAT_A_REG_BIT_15        (1 << 15)
#define USBD_STM32_CAT_A_REG_BIT_14        (1 << 14)
#define USBD_STM32_CAT_A_REG_BIT_13        (1 << 13)
#define USBD_STM32_CAT_A_REG_BIT_12        (1 << 12)
#define USBD_STM32_CAT_A_REG_BIT_11        (1 << 11)
#define USBD_STM32_CAT_A_REG_BIT_10        (1 << 10)
#define USBD_STM32_CAT_A_REG_BIT_9         (1 <<  9)
#define USBD_STM32_CAT_A_REG_BIT_8         (1 <<  8)
#define USBD_STM32_CAT_A_REG_BIT_7         (1 <<  7)
#define USBD_STM32_CAT_A_REG_BIT_6         (1 <<  6)
#define USBD_STM32_CAT_A_REG_BIT_5         (1 <<  5)
#define USBD_STM32_CAT_A_REG_BIT_4         (1 <<  4)
#define USBD_STM32_CAT_A_REG_BIT_3         (1 <<  3)
#define USBD_STM32_CAT_A_REG_BIT_2         (1 <<  2)
#define USBD_STM32_CAT_A_REG_BIT_1         (1 <<  1)
#define USBD_STM32_CAT_A_REG_BIT_0         (1 <<  0)

#define USBD_STM32_CAT_A_REG_1BIT(__bit)   (0x1  <<  __bit)
#define USBD_STM32_CAT_A_REG_2BIT(__bit)   (0x3  <<  __bit)
#define USBD_STM32_CAT_A_REG_3BIT(__bit)   (0x7  <<  __bit)
#define USBD_STM32_CAT_A_REG_4BIT(__bit)   (0xF  <<  __bit)

#define USBD_STM32_CAT_A_REG_GET_1BIT(__reg, __bit)   (((__reg) >> (__bit)) & 0x1)
#define USBD_STM32_CAT_A_REG_GET_2BIT(__reg, __bit)   (((__reg) >> (__bit)) & 0x3)
#define USBD_STM32_CAT_A_REG_GET_3BIT(__reg, __bit)   (((__reg) >> (__bit)) & 0x7)
#define USBD_STM32_CAT_A_REG_GET_4BIT(__reg, __bit)   (((__reg) >> (__bit)) & 0xF)


#define USBD_STM32_CAT_A_REG_BITS_3_TO_0    (USBD_STM32_CAT_A_REG_BIT_3 | USBD_STM32_CAT_A_REG_BIT_2 | USBD_STM32_CAT_A_REG_BIT_1 | USBD_STM32_CAT_A_REG_BIT_0)
#define USBD_STM32_CAT_A_REG_BITS_6_TO_0    (USBD_STM32_CAT_A_REG_BIT_6 | USBD_STM32_CAT_A_REG_BIT_5 | USBD_STM32_CAT_A_REG_BIT_4 | USBD_STM32_CAT_A_REG_BITS_3_TO_0)
#define USBD_STM32_CAT_A_REG_BITS_10_TO_0   (USBD_STM32_CAT_A_REG_BIT_10 | USBD_STM32_CAT_A_REG_BIT_9 | USBD_STM32_CAT_A_REG_BIT_8 | USBD_STM32_CAT_A_REG_BIT_7 | USBD_STM32_CAT_A_REG_BITS_6_TO_0)


/**
 * ISTR register bits definitions
 */
#define USBD_STM32_CAT_A_ISTR_CTR         (USBD_STM32_CAT_A_REG_BIT_15)      // Correct TRansfer
#define USBD_STM32_CAT_A_ISTR_PMAOVR      (USBD_STM32_CAT_A_REG_BIT_14)      // DMA Over/underrun
#define USBD_STM32_CAT_A_ISTR_ERR         (USBD_STM32_CAT_A_REG_BIT_13)      // Error
#define USBD_STM32_CAT_A_ISTR_WKUP        (USBD_STM32_CAT_A_REG_BIT_12)      // WaKe UP
#define USBD_STM32_CAT_A_ISTR_SUSP        (USBD_STM32_CAT_A_REG_BIT_11)      // Suspend
#define USBD_STM32_CAT_A_ISTR_RESET       (USBD_STM32_CAT_A_REG_BIT_10)      // Reset
#define USBD_STM32_CAT_A_ISTR_SOF         (USBD_STM32_CAT_A_REG_BIT_9)       // Start Of Frame
#define USBD_STM32_CAT_A_ISTR_ESOF        (USBD_STM32_CAT_A_REG_BIT_8)       // Expected Start Of Frame
#define USBD_STM32_CAT_A_ISTR_DIR         (USBD_STM32_CAT_A_REG_BIT_4)       // Direction of transaction
#define USBD_STM32_CAT_A_ISTR_EP_ID       (USBD_STM32_CAT_A_REG_BITS_3_TO_0) // Endpoint Identifier


/**
 * CNTR register bits definitions
 */
#define USBD_STM32_CAT_A_CNTR_CTRM        (USBD_STM32_CAT_A_REG_BIT_15)      // Correct Transfer Mask
#define USBD_STM32_CAT_A_CNTR_PMAOVRM     (USBD_STM32_CAT_A_REG_BIT_14)      // DMA Over/underrun Mask
#define USBD_STM32_CAT_A_CNTR_ERRM        (USBD_STM32_CAT_A_REG_BIT_13)      // Error Mask
#define USBD_STM32_CAT_A_CNTR_WKUPM       (USBD_STM32_CAT_A_REG_BIT_12)      // WaKe UP Mask
#define USBD_STM32_CAT_A_CNTR_SUSPM       (USBD_STM32_CAT_A_REG_BIT_11)      // Suspend Mask
#define USBD_STM32_CAT_A_CNTR_RESETM      (USBD_STM32_CAT_A_REG_BIT_10)      // RESET Mask
#define USBD_STM32_CAT_A_CNTR_SOFM        (USBD_STM32_CAT_A_REG_BIT_9)       // Start Of Frame Mask
#define USBD_STM32_CAT_A_CNTR_ESOFM       (USBD_STM32_CAT_A_REG_BIT_8)       // Expected Start Of Frame Mask
                                          // USBD_STM32_CAT_A_REG_BIT_7 reserved
                                          // USBD_STM32_CAT_A_REG_BIT_6 reserved
                                          // USBD_STM32_CAT_A_REG_BIT_5 reserved
#define USBD_STM32_CAT_A_CNTR_RESUME      (USBD_STM32_CAT_A_REG_BIT_4)       // RESUME request
#define USBD_STM32_CAT_A_CNTR_FSUSP       (USBD_STM32_CAT_A_REG_BIT_3)       // Force Suspend
#define USBD_STM32_CAT_A_CNTR_LPMODE      (USBD_STM32_CAT_A_REG_BIT_2)       // Low-power Mode
#define USBD_STM32_CAT_A_CNTR_PDWN        (USBD_STM32_CAT_A_REG_BIT_1)       // Power Down
#define USBD_STM32_CAT_A_CNTR_FRES        (USBD_STM32_CAT_A_REG_BIT_0)       // Force USB Reset


/**
 * FNR register bits definitions
 */
#define USBD_STM32_CAT_A_FNR_RXDP         (USBD_STM32_CAT_A_REG_BIT_15)                             // status of D+ data line
#define USBD_STM32_CAT_A_FNR_RXDM         (USBD_STM32_CAT_A_REG_BIT_14)                             // status of D- data line
#define USBD_STM32_CAT_A_FNR_LCK          (USBD_STM32_CAT_A_REG_BIT_13)                             // Locked
#define USBD_STM32_CAT_A_FNR_LSOF         (USBD_STM32_CAT_A_REG_BIT_12 | USBD_STM32_CAT_A_REG_BIT_11) // Lost SOF
#define USBD_STM32_CAT_A_FNR_FN           (USBD_STM32_CAT_A_REG_BITS_10_TO_0)                       // Frame Number


/**
 * DADDR register bits definitions
 */
#define USBD_STM32_CAT_A_DADDR_EF         (USBD_STM32_CAT_A_REG_BIT_7)                              // Enable Function
#define USBD_STM32_CAT_A_DADDR_ADD        (USBD_STM32_CAT_A_REG_BITS_6_TO_0)                        // Device Address


/**
 * Endpoint register bits definitions
 */
#define USBDEP_STM32_CAT_A_BIT__CTR_RX    (15)     // Endpoint Correct TRansfer RX
#define USBDEP_STM32_CAT_A_BIT__DTOG_RX   (14)     // Endpoint Data Toggle RX
#define USBDEP_STM32_CAT_A_BIT__STAT_RX   (12)     // Endpoint RX Status Field
#define USBDEP_STM32_CAT_A_BIT__SETUP     (11)     // Endpoint Setup
#define USBDEP_STM32_CAT_A_BIT__TYPE      (9)      // Endpoint Type
#define USBDEP_STM32_CAT_A_BIT__KIND      (8)      // Endpoint Kind
#define USBDEP_STM32_CAT_A_BIT__CTR_TX    (7)      // Endpoint Correct TRansfer TX
#define USBDEP_STM32_CAT_A_BIT__DTOG_TX   (6)      // Endpoint Data Toggle TX
#define USBDEP_STM32_CAT_A_BIT__STAT_TX   (4)      // Endpoint TX Status Field
#define USBDEP_STM32_CAT_A_BIT__ADDR      (0)      // Endpoint Address Field

#define USBDEP_STM32_CAT_A__CTR_RX    (USBD_STM32_CAT_A_REG_1BIT(USBDEP_STM32_CAT_A_BIT__CTR_RX))     // Endpoint Correct TRansfer RX
#define USBDEP_STM32_CAT_A__DTOG_RX   (USBD_STM32_CAT_A_REG_1BIT(USBDEP_STM32_CAT_A_BIT__DTOG_RX))    // Endpoint Data Toggle RX
#define USBDEP_STM32_CAT_A__STAT_RX   (USBD_STM32_CAT_A_REG_2BIT(USBDEP_STM32_CAT_A_BIT__STAT_RX))    // Endpoint RX Status Field
#define USBDEP_STM32_CAT_A__SETUP     (USBD_STM32_CAT_A_REG_1BIT(USBDEP_STM32_CAT_A_BIT__SETUP))      // Endpoint Setup
#define USBDEP_STM32_CAT_A__TYPE      (USBD_STM32_CAT_A_REG_2BIT(USBDEP_STM32_CAT_A_BIT__TYPE))       // Endpoint Type
#define USBDEP_STM32_CAT_A__KIND      (USBD_STM32_CAT_A_REG_1BIT(USBDEP_STM32_CAT_A_BIT__KIND))       // Endpoint Kind
#define USBDEP_STM32_CAT_A__CTR_TX    (USBD_STM32_CAT_A_REG_1BIT(USBDEP_STM32_CAT_A_BIT__CTR_TX))     // Endpoint Correct TRansfer TX
#define USBDEP_STM32_CAT_A__DTOG_TX   (USBD_STM32_CAT_A_REG_1BIT(USBDEP_STM32_CAT_A_BIT__DTOG_TX))    // Endpoint Data Toggle TX
#define USBDEP_STM32_CAT_A__STAT_TX   (USBD_STM32_CAT_A_REG_2BIT(USBDEP_STM32_CAT_A_BIT__STAT_TX))    // Endpoint TX Status Field
#define USBDEP_STM32_CAT_A__ADDR      (USBD_STM32_CAT_A_REG_4BIT(USBDEP_STM32_CAT_A_BIT__ADDR))       // Endpoint Address Field

#define USBDEP_STM32_CAT_A__ADDR_KIND_TYPE  (USBDEP_STM32_CAT_A__ADDR | USBDEP_STM32_CAT_A__KIND | USBDEP_STM32_CAT_A__TYPE)
#define USBDEP_STM32_CAT_A__ADDR_TYPE       (USBDEP_STM32_CAT_A__ADDR                          | USBDEP_STM32_CAT_A__TYPE)



/**
 * Endpoint Type Field Definitions
 */
#define USBDEP_STM32_CAT_A__BULK         (0x0000)                                                 // Endpoint BULK
#define USBDEP_STM32_CAT_A__CONTROL      (USBD_STM32_CAT_A_REG_BIT_9)                               // Endpoint CONTROL
#define USBDEP_STM32_CAT_A__ISOCHRONOUS  (USBD_STM32_CAT_A_REG_BIT_10)                              // Endpoint ISOCHRONOUS
#define USBDEP_STM32_CAT_A__INTERRUPT    (USBD_STM32_CAT_A_REG_BIT_10 | USBD_STM32_CAT_A_REG_BIT_9)   // Endpoint INTERRUPT


/**
 * Endpoint TX Status Field Definitions
 */
#define USBDEP_STM32_CAT_A__TX_DIS     (0x0000)                                                   // Endpoint Tx DisSabled
#define USBDEP_STM32_CAT_A__TX_STALL   (USBD_STM32_CAT_A_REG_BIT_4)                                 // Endpoint Tx Stalled
#define USBDEP_STM32_CAT_A__TX_NAK     (USBD_STM32_CAT_A_REG_BIT_5)                                 // Endpoint Tx Naked
#define USBDEP_STM32_CAT_A__TX_VALID   (USBD_STM32_CAT_A_REG_BIT_5 | USBD_STM32_CAT_A_REG_BIT_4)      // Endpoint Tx Valid


/**
 * Endpoint RX Status Field Definitions
 */
#define USBDEP_STM32_CAT_A__RX_DIS     (0x0000)                                                   // Endpoint RX DISabled
#define USBDEP_STM32_CAT_A__RX_STALL   (USBD_STM32_CAT_A_REG_BIT_12)                                // Endpoint RX STALLed
#define USBDEP_STM32_CAT_A__RX_NAK     (USBD_STM32_CAT_A_REG_BIT_13)                                // Endpoint RX NAKed
#define USBDEP_STM32_CAT_A__RX_VALID   (USBD_STM32_CAT_A_REG_BIT_13 | USBD_STM32_CAT_A_REG_BIT_12)    // Endpoint RX VALID


#define USBD_REG_BASE         0x40005C00
#define USBD_BUFFER_BASE      0x40006000


typedef struct
{
   volatile uint32_t EP_REG[8];
   volatile uint32_t regs_reserved[8];
   volatile uint32_t CNTR;
   volatile uint32_t ISTR;
   volatile uint32_t FNR;
   volatile uint32_t DADDR;
   volatile uint32_t BTABLE;
}USBD_STM32_Registers;


typedef union
{
   struct
   {
      volatile uint32_t tx_addr;
      volatile uint32_t tx_count;
      volatile uint32_t rx_addr;
      volatile uint32_t rx_count;
   }sb;
   struct
   {
      volatile uint32_t addr;
      volatile uint32_t count;
   }db[2];
}USBDEP_STM32_CAT_A_Mem_Descriptor;


#define USBD_STM32_REG     ((USBD_STM32_Registers *) USBD_REG_BASE)

#define USBD_STM32_EPMEM   ((USBDEP_STM32_CAT_A_Mem_Descriptor *) USBD_BUFFER_BASE)







#define USBDEP_SB_SET_TX_COUNT(_ep_number, _count)  USBD_STM32_EPMEM[_ep_number].sb.tx_count = _count
#define USBDEP_SB_CLR_TX_COUNT(_ep_number)          USBD_STM32_EPMEM[_ep_number].sb.tx_count = 0
#define USBDEP_DB_CLR_TX_COUNT(_ep_number)          USBD_STM32_EPMEM[_ep_number].sb.tx_count = 0;    USBD_STM32_EPMEM[_ep_number].sb.rx_count = 0
#define USBDEP_DB_SET_TX_COUNT(_ep_number, _count)  USBD_STM32_EPMEM[_ep_number].db[(USBD_STM32_REG->EP_REG[_ep_number] & USBDEP_STM32_CAT_A__DTOG_RX) >> 14].count = _count
#define USBDEP_ISO_SET_TX_COUNT(_ep_number, _count) USBD_STM32_EPMEM[_ep_number].db[((USBD_STM32_REG->EP_REG[_ep_number] ^ USBDEP_STM32_CAT_A__DTOG_TX) & USBDEP_STM32_CAT_A__DTOG_TX) >> 6].count = _count

#define USBDEP_DB_GET_TX_COUNT_HB(_ep_number)      ((uint16_t)(USBD_STM32_EPMEM[_ep_number].db[((USBD_STM32_REG->EP_REG[_ep_number] ^ USBDEP_STM32_CAT_A__DTOG_RX) & USBDEP_STM32_CAT_A__DTOG_RX) >> 14].count) & 0x3ff)
#define USBDEP_ISO_GET_TX_COUNT_HB(_ep_number)     ((uint16_t)(USBD_STM32_EPMEM[_ep_number].db[((USBD_STM32_REG->EP_REG[_ep_number]) & USBDEP_STM32_CAT_A__DTOG_TX) >> 6].count) & 0x3ff)

#define USBDEP_SB_GET_RX_COUNT(_ep_number)         ((uint16_t)(USBD_STM32_EPMEM[_ep_number].sb.rx_count) & 0x3ff)
#define USBDEP_DB_GET_RX_COUNT(_ep_number)         ((uint16_t)(USBD_STM32_EPMEM[_ep_number].db[(USBD_STM32_REG->EP_REG[_ep_number] & USBDEP_STM32_CAT_A__DTOG_TX) >> 6].count) & 0x3ff)
#define USBDEP_DB_GET_RX_COUNT_HB(_ep_number)      ((uint16_t)(USBD_STM32_EPMEM[_ep_number].db[((USBD_STM32_REG->EP_REG[_ep_number] ^ USBDEP_STM32_CAT_A__DTOG_TX) & USBDEP_STM32_CAT_A__DTOG_TX) >> 6].count) & 0x3ff)
#define USBDEP_ISO_GET_RX_COUNT(_ep_number)        ((uint16_t)(USBD_STM32_EPMEM[_ep_number].db[((USBD_STM32_REG->EP_REG[_ep_number] ^ USBDEP_STM32_CAT_A__DTOG_RX) & USBDEP_STM32_CAT_A__DTOG_RX) >> 14].count) & 0x3ff)





/**
 * CAUTION!!!
 * Be careful when using this macro - it needs temporary variable "_count" which will be modified by this macro!
 */
#define USBDEP_SB_SET_RX_MAX_POSSIBLE_COUNT(_ep_number, _count) {\
   if(_count > 62) {\
      _count >>= 5;\
      _count--;\
      _count <<= 10;\
      _count |= (1 << 15);\
   }\
   else {\
      _count <<= 9;\
   }\
   USBD_STM32_EPMEM[_ep_number].sb.rx_count = (_count & 0xFC00);\
}

/**
 * CAUTION!!!
 * Be careful when using this macro - it needs temporary variable "_count" which will be modified by this macro!
 */
#define USBDEP_DB_SET_RX_MAX_POSSIBLE_COUNT(_ep_number, _count) {\
   if(_count > 62) {\
      _count >>= 5;\
      _count--;\
      _count <<= 10;\
      _count |= (1 << 15);\
   }\
   else {\
      _count <<= 9;\
   }\
   USBD_STM32_EPMEM[_ep_number].db[0].count = (_count & 0xFC00);\
   USBD_STM32_EPMEM[_ep_number].db[1].count = (_count & 0xFC00);\
}




#define USBDEP_DB_GET_TX_BUFFER_ADDR(_ep_number)      (USBD_BUFFER_BASE + (uint8_t *)(USBD_STM32_EPMEM[_ep_number].db[(USBD_STM32_REG->EP_REG[_ep_number] & USBDEP_STM32_CAT_A__DTOG_RX) >> 14].addr * 2))
#define USBDEP_ISO_GET_TX_BUFFER_ADDR(_ep_number)     (USBD_BUFFER_BASE + (uint8_t *)(USBD_STM32_EPMEM[_ep_number].db[((USBD_STM32_REG->EP_REG[_ep_number] ^ USBDEP_STM32_CAT_A__DTOG_TX) & USBDEP_STM32_CAT_A__DTOG_TX) >> 6].addr * 2))
#define USBDEP_SB_GET_TX_BUFFER_ADDR(_ep_number)      (USBD_BUFFER_BASE + (uint8_t *)(USBD_STM32_EPMEM[_ep_number].sb.tx_addr * 2))

#define USBDEP_DB_GET_RX_BUFFER_ADDR(_ep_number)      (USBD_BUFFER_BASE + (uint8_t *)(USBD_STM32_EPMEM[_ep_number].db[(USBD_STM32_REG->EP_REG[_ep_number] & USBDEP_STM32_CAT_A__DTOG_TX) >> 6].addr * 2))
#define USBDEP_DB_GET_RX_BUFFER_ADDR_HB(_ep_number)   (USBD_BUFFER_BASE + (uint8_t *)(USBD_STM32_EPMEM[_ep_number].db[((USBD_STM32_REG->EP_REG[_ep_number] ^ USBDEP_STM32_CAT_A__DTOG_TX) & USBDEP_STM32_CAT_A__DTOG_TX) >> 6].addr * 2))
#define USBDEP_ISO_GET_RX_BUFFER_ADDR(_ep_number)     (USBD_BUFFER_BASE + (uint8_t *)(USBD_STM32_EPMEM[_ep_number].db[((USBD_STM32_REG->EP_REG[_ep_number] ^ USBDEP_STM32_CAT_A__DTOG_RX) & USBDEP_STM32_CAT_A__DTOG_RX) >> 14].addr * 2))
#define USBDEP_SB_GET_RX_BUFFER_ADDR(_ep_number)      (USBD_BUFFER_BASE + (uint8_t *)(USBD_STM32_EPMEM[_ep_number].sb.rx_addr * 2))



#define USBDEP_SET_RX_BUF_ADDR(_ep_number, _addr)     USBD_STM32_EPMEM[_ep_number].sb.rx_addr = ((_addr) & 0xFFFE)
#define USBDEP_SET_TX_BUF_ADDR(_ep_number, _addr)     USBD_STM32_EPMEM[_ep_number].sb.tx_addr = ((_addr) & 0xFFFE)


#ifdef USBD_USE_IOCMD
#define USBDEP_REG_MODIFY(_ep_reg_num, _ep_reg, _temp, _before, _after, ex_or_mask, and_mask, or_mask) \
   (_temp)   = _ep_reg;\
   (_before) = (_temp);\
   (_temp)  ^= (ex_or_mask);\
   (_temp)  &= (and_mask);\
   (_temp)  |= (or_mask);\
   _ep_reg   = (_temp);\
   (_after)  = _ep_reg;\
   port_stm32_cat_a_print_ep_reg_state("-> ", (_before), __LINE__, (_ep_reg_num));\
   port_stm32_cat_a_print_ep_reg_state("<- ", (_after), __LINE__, (_ep_reg_num))

#else
#define USBDEP_REG_MODIFY(_ep_reg_num, _ep_reg, _temp, _before, _after, ex_or_mask, and_mask, or_mask) \
   (_temp)   = _ep_reg;\
   (_temp)  ^= (ex_or_mask);\
   (_temp)  &= (and_mask);\
   (_temp)  |= (or_mask);\
   _ep_reg   = (_temp);

#endif


#ifndef RCC_APB1Periph_USB
#define RCC_APB1Periph_USB            ((uint32_t)0x00800000)
#endif

#ifndef USBD_STM32_CAT_A_RESET_AND_ENABLE_PERIPH
#define USBD_STM32_CAT_A_RESET_AND_ENABLE_PERIPH() \
{ \
   /* turn ON USBD peripherial clock */                           \
   STM32_RCC->APB1ENR |= RCC_APB1Periph_USB;                      \
                                                                  \
   /* enable peripherial reset on APB bus layer */                \
   STM32_RCC->APB1RSTR |= RCC_APB1Periph_USB;                     \
                                                                  \
   /* disable peripherial reset on APB bus layer */               \
   STM32_RCC->APB1RSTR &= ~RCC_APB1Periph_USB;                    \
                                                                  \
   /* turn OFF peripherial power down mode */                     \
   USBD_STM32_REG->CNTR &= ~((uint32_t)USBD_STM32_CAT_A_CNTR_PDWN); \
   while(USBD_STM32_REG->CNTR & USBD_STM32_CAT_A_CNTR_PDWN)         \
   {                                                              \
      ;                                                           \
   }                                                              \
                                                                  \
   /* disable peripherial reset */                                \
   USBD_STM32_REG->CNTR &= ~((uint32_t)USBD_STM32_CAT_A_CNTR_FRES); \
                                                                  \
   /* clear all interruptions flags */                            \
   USBD_STM32_REG->ISTR = 0;                                      \
                                                                  \
   /* enable interruption from host reset */                      \
   USBD_STM32_REG->CNTR = USBD_STM32_CAT_A_CNTR_RESETM;           \
}

#endif

#ifndef USBD_STM32_CAT_A_DISABLE
#define USBD_STM32_CAT_A_DISABLE() \
{ \
    /* disable all interruptions, forces peripheral reset */      \
    /* and turns OFF transceiver */                               \
   USBD_STM32_REG->CNTR =                                         \
      USBD_STM32_CAT_A_CNTR_PDWN | USBD_STM32_CAT_A_CNTR_FRES;    \
                                                                  \
   /* enable peripherial reset on APB bus layer */                \
   STM32_RCC->APB1RSTR |= RCC_APB1Periph_USB;                     \
                                                                  \
   /* disable peripherial reset on APB bus layer */               \
   STM32_RCC->APB1RSTR &= ~RCC_APB1Periph_USB;                    \
                                                                  \
   /* turn OFF USBD peripherial clock */                          \
   STM32_RCC->APB1ENR &= ~RCC_APB1Periph_USB;                     \
}

#endif


#endif /*USBD_PORT_STM32_CAT_A_REGS_H_*/
