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

#ifndef USBD_DBG_H_
#define USBD_DBG_H_



#ifdef USBD_USE_IOCMD
#include "iocmd.h"

/**
 * Following groups are used by USBD library and must be configured correctly:
 * USBD_DBG
 * USB_DBG_CDP
 * USBD_DBG_INIT
 * USBD_DBG_DEV_INIT
 * USBD_DBG_DEV_ONOFF
 * USBD_DBG_DEV_ONOFF_SOF_RECEIVED
 * USBD_DBG_DEV_STATE
 * USBD_DBG_DEV_INVALID_PARAMS
 * USBD_DBG_IO_ONOFF
 * USBD_DBG_IO_STATE
 * USBD_DBG_IO_PROCESSING
 * USBD_DBG_IO_INVALID_PARAMS
 * USBD_DBG_IOTPEV_ONOFF
 * USBD_DBG_IOTPEV_STATE
 * USBD_DBG_IOTPEV_PROCESSING
 * USBD_DBG_IOTPEV_INVALID_PARAMS
 * USBD_DBG_EVENT_INIT
 * USBD_DBG_EVENT_STATE
 * USBD_DBG_EVENT_PROCESSING
 * USBD_DBG_EVENT_INVALID_PARAMS
 * USBD_DBG_REQ_INIT
 * USBD_DBG_REQ_PROCESSING
 * USBD_DBG_REQ_INVALID_PARAMS
 * USBD_DBG_USBDC
 * USBD_DBG_PORT_INIT
 * USBD_DBG_PORT_DEV
 * USBD_DBG_PORT_IO
 * USBD_DBG_PORT_IO_ONOFF
 * USBD_DBG_PORT_EVENT
 * USBD_DBG_PORT_REQ
 * USBD_DBG_PORT_INVALID_PARAMS
 * USBD_DBG_PORT_IRQ
 * optional - for usbd_iotp_buff:
 * USBD_DBG_IOTPBF_ONOFF
 * USBD_DBG_IOTPBF_STATE
 * USBD_DBG_IOTPBF_EVENT
 * USBD_DBG_IOTPBF_PROCESSING
 * USBD_DBG_IOTPBF_INVALID_PARAMS
 */

#define USBD_COMPILATION_SWITCH_LOG(id, level)                                                        IOCMD_COMPILATION_SWITCH_LOG(id, level)
#define USBD_COMPILATION_SWITCH_ENT(id)                                                               IOCMD_COMPILATION_SWITCH_ENT(id)

#define USBD_ENTER_FUNC(id)                                                                           IOCMD_ENTER_FUNC(id)

#define USBD_EXIT_FUNC(id)                                                                            IOCMD_EXIT_FUNC(id)

#define USBD_EMERG(id, format)                                                                        IOCMD_EMERG(id, format)
#define USBD_EMERG_1(id,  format, a1)                                                                 IOCMD_EMERG_1(id,  format, a1)
#define USBD_EMERG_2(id,  format, a1, a2)                                                             IOCMD_EMERG_2(id,  format, a1, a2)
#define USBD_EMERG_3(id,  format, a1, a2, a3)                                                         IOCMD_EMERG_3(id,  format, a1, a2, a3)
#define USBD_EMERG_4(id,  format, a1, a2, a3, a4)                                                     IOCMD_EMERG_4(id,  format, a1, a2, a3, a4)
#define USBD_EMERG_5(id,  format, a1, a2, a3, a4, a5)                                                 IOCMD_EMERG_5(id,  format, a1, a2, a3, a4, a5)
#define USBD_EMERG_6(id,  format, a1, a2, a3, a4, a5, a6)                                             IOCMD_EMERG_6(id,  format, a1, a2, a3, a4, a5, a6)
#define USBD_EMERG_7(id,  format, a1, a2, a3, a4, a5, a6, a7)                                         IOCMD_EMERG_7(id,  format, a1, a2, a3, a4, a5, a6, a7)
#define USBD_EMERG_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)                                     IOCMD_EMERG_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)
#define USBD_EMERG_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)                                 IOCMD_EMERG_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#define USBD_EMERG_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)                             IOCMD_EMERG_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#define USBD_EMERG_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)                         IOCMD_EMERG_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#define USBD_EMERG_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)                     IOCMD_EMERG_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#define USBD_EMERG_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)                 IOCMD_EMERG_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#define USBD_EMERG_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)             IOCMD_EMERG_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#define USBD_EMERG_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)         IOCMD_EMERG_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#define USBD_EMERG_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)     IOCMD_EMERG_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#define USBD_ALERT(id, format)                                                                        IOCMD_ALERT(id, format)
#define USBD_ALERT_1(id,  format, a1)                                                                 IOCMD_ALERT_1(id,  format, a1)
#define USBD_ALERT_2(id,  format, a1, a2)                                                             IOCMD_ALERT_2(id,  format, a1, a2)
#define USBD_ALERT_3(id,  format, a1, a2, a3)                                                         IOCMD_ALERT_3(id,  format, a1, a2, a3)
#define USBD_ALERT_4(id,  format, a1, a2, a3, a4)                                                     IOCMD_ALERT_4(id,  format, a1, a2, a3, a4)
#define USBD_ALERT_5(id,  format, a1, a2, a3, a4, a5)                                                 IOCMD_ALERT_5(id,  format, a1, a2, a3, a4, a5)
#define USBD_ALERT_6(id,  format, a1, a2, a3, a4, a5, a6)                                             IOCMD_ALERT_6(id,  format, a1, a2, a3, a4, a5, a6)
#define USBD_ALERT_7(id,  format, a1, a2, a3, a4, a5, a6, a7)                                         IOCMD_ALERT_7(id,  format, a1, a2, a3, a4, a5, a6, a7)
#define USBD_ALERT_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)                                     IOCMD_ALERT_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)
#define USBD_ALERT_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)                                 IOCMD_ALERT_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#define USBD_ALERT_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)                             IOCMD_ALERT_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#define USBD_ALERT_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)                         IOCMD_ALERT_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#define USBD_ALERT_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)                     IOCMD_ALERT_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#define USBD_ALERT_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)                 IOCMD_ALERT_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#define USBD_ALERT_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)             IOCMD_ALERT_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#define USBD_ALERT_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)         IOCMD_ALERT_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#define USBD_ALERT_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)     IOCMD_ALERT_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#define USBD_CRIT(id, format)                                                                         IOCMD_CRIT(id, format)
#define USBD_CRIT_1(id,  format, a1)                                                                  IOCMD_CRIT_1(id,  format, a1)
#define USBD_CRIT_2(id,  format, a1, a2)                                                              IOCMD_CRIT_2(id,  format, a1, a2)
#define USBD_CRIT_3(id,  format, a1, a2, a3)                                                          IOCMD_CRIT_3(id,  format, a1, a2, a3)
#define USBD_CRIT_4(id,  format, a1, a2, a3, a4)                                                      IOCMD_CRIT_4(id,  format, a1, a2, a3, a4)
#define USBD_CRIT_5(id,  format, a1, a2, a3, a4, a5)                                                  IOCMD_CRIT_5(id,  format, a1, a2, a3, a4, a5)
#define USBD_CRIT_6(id,  format, a1, a2, a3, a4, a5, a6)                                              IOCMD_CRIT_6(id,  format, a1, a2, a3, a4, a5, a6)
#define USBD_CRIT_7(id,  format, a1, a2, a3, a4, a5, a6, a7)                                          IOCMD_CRIT_7(id,  format, a1, a2, a3, a4, a5, a6, a7)
#define USBD_CRIT_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)                                      IOCMD_CRIT_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)
#define USBD_CRIT_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)                                  IOCMD_CRIT_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#define USBD_CRIT_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)                              IOCMD_CRIT_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#define USBD_CRIT_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)                          IOCMD_CRIT_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#define USBD_CRIT_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)                      IOCMD_CRIT_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#define USBD_CRIT_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)                  IOCMD_CRIT_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#define USBD_CRIT_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)              IOCMD_CRIT_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#define USBD_CRIT_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)          IOCMD_CRIT_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#define USBD_CRIT_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)      IOCMD_CRIT_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#define USBD_ERROR(id, format)                                                                        IOCMD_ERROR(id, format)
#define USBD_ERROR_1(id,  format, a1)                                                                 IOCMD_ERROR_1(id,  format, a1)
#define USBD_ERROR_2(id,  format, a1, a2)                                                             IOCMD_ERROR_2(id,  format, a1, a2)
#define USBD_ERROR_3(id,  format, a1, a2, a3)                                                         IOCMD_ERROR_3(id,  format, a1, a2, a3)
#define USBD_ERROR_4(id,  format, a1, a2, a3, a4)                                                     IOCMD_ERROR_4(id,  format, a1, a2, a3, a4)
#define USBD_ERROR_5(id,  format, a1, a2, a3, a4, a5)                                                 IOCMD_ERROR_5(id,  format, a1, a2, a3, a4, a5)
#define USBD_ERROR_6(id,  format, a1, a2, a3, a4, a5, a6)                                             IOCMD_ERROR_6(id,  format, a1, a2, a3, a4, a5, a6)
#define USBD_ERROR_7(id,  format, a1, a2, a3, a4, a5, a6, a7)                                         IOCMD_ERROR_7(id,  format, a1, a2, a3, a4, a5, a6, a7)
#define USBD_ERROR_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)                                     IOCMD_ERROR_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)
#define USBD_ERROR_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)                                 IOCMD_ERROR_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#define USBD_ERROR_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)                             IOCMD_ERROR_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#define USBD_ERROR_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)                         IOCMD_ERROR_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#define USBD_ERROR_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)                     IOCMD_ERROR_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#define USBD_ERROR_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)                 IOCMD_ERROR_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#define USBD_ERROR_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)             IOCMD_ERROR_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#define USBD_ERROR_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)         IOCMD_ERROR_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#define USBD_ERROR_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)     IOCMD_ERROR_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#define USBD_WARN(id, format)                                                                         IOCMD_WARN(id, format)
#define USBD_WARN_1(id,  format, a1)                                                                  IOCMD_WARN_1(id,  format, a1)
#define USBD_WARN_2(id,  format, a1, a2)                                                              IOCMD_WARN_2(id,  format, a1, a2)
#define USBD_WARN_3(id,  format, a1, a2, a3)                                                          IOCMD_WARN_3(id,  format, a1, a2, a3)
#define USBD_WARN_4(id,  format, a1, a2, a3, a4)                                                      IOCMD_WARN_4(id,  format, a1, a2, a3, a4)
#define USBD_WARN_5(id,  format, a1, a2, a3, a4, a5)                                                  IOCMD_WARN_5(id,  format, a1, a2, a3, a4, a5)
#define USBD_WARN_6(id,  format, a1, a2, a3, a4, a5, a6)                                              IOCMD_WARN_6(id,  format, a1, a2, a3, a4, a5, a6)
#define USBD_WARN_7(id,  format, a1, a2, a3, a4, a5, a6, a7)                                          IOCMD_WARN_7(id,  format, a1, a2, a3, a4, a5, a6, a7)
#define USBD_WARN_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)                                      IOCMD_WARN_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)
#define USBD_WARN_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)                                  IOCMD_WARN_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#define USBD_WARN_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)                              IOCMD_WARN_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#define USBD_WARN_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)                          IOCMD_WARN_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#define USBD_WARN_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)                      IOCMD_WARN_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#define USBD_WARN_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)                  IOCMD_WARN_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#define USBD_WARN_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)              IOCMD_WARN_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#define USBD_WARN_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)          IOCMD_WARN_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#define USBD_WARN_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)      IOCMD_WARN_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#define USBD_NOTICE(id, format)                                                                       IOCMD_NOTICE(id, format)
#define USBD_NOTICE_1(id,  format, a1)                                                                IOCMD_NOTICE_1(id,  format, a1)
#define USBD_NOTICE_2(id,  format, a1, a2)                                                            IOCMD_NOTICE_2(id,  format, a1, a2)
#define USBD_NOTICE_3(id,  format, a1, a2, a3)                                                        IOCMD_NOTICE_3(id,  format, a1, a2, a3)
#define USBD_NOTICE_4(id,  format, a1, a2, a3, a4)                                                    IOCMD_NOTICE_4(id,  format, a1, a2, a3, a4)
#define USBD_NOTICE_5(id,  format, a1, a2, a3, a4, a5)                                                IOCMD_NOTICE_5(id,  format, a1, a2, a3, a4, a5)
#define USBD_NOTICE_6(id,  format, a1, a2, a3, a4, a5, a6)                                            IOCMD_NOTICE_6(id,  format, a1, a2, a3, a4, a5, a6)
#define USBD_NOTICE_7(id,  format, a1, a2, a3, a4, a5, a6, a7)                                        IOCMD_NOTICE_7(id,  format, a1, a2, a3, a4, a5, a6, a7)
#define USBD_NOTICE_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)                                    IOCMD_NOTICE_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)
#define USBD_NOTICE_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)                                IOCMD_NOTICE_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#define USBD_NOTICE_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)                            IOCMD_NOTICE_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#define USBD_NOTICE_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)                        IOCMD_NOTICE_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#define USBD_NOTICE_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)                    IOCMD_NOTICE_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#define USBD_NOTICE_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)                IOCMD_NOTICE_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#define USBD_NOTICE_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)            IOCMD_NOTICE_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#define USBD_NOTICE_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)        IOCMD_NOTICE_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#define USBD_NOTICE_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)    IOCMD_NOTICE_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#define USBD_INFO_HI(id, format)                                                                      IOCMD_INFO_HI(id, format)
#define USBD_INFO_HI_1(id,  format, a1)                                                               IOCMD_INFO_HI_1(id,  format, a1)
#define USBD_INFO_HI_2(id,  format, a1, a2)                                                           IOCMD_INFO_HI_2(id,  format, a1, a2)
#define USBD_INFO_HI_3(id,  format, a1, a2, a3)                                                       IOCMD_INFO_HI_3(id,  format, a1, a2, a3)
#define USBD_INFO_HI_4(id,  format, a1, a2, a3, a4)                                                   IOCMD_INFO_HI_4(id,  format, a1, a2, a3, a4)
#define USBD_INFO_HI_5(id,  format, a1, a2, a3, a4, a5)                                               IOCMD_INFO_HI_5(id,  format, a1, a2, a3, a4, a5)
#define USBD_INFO_HI_6(id,  format, a1, a2, a3, a4, a5, a6)                                           IOCMD_INFO_HI_6(id,  format, a1, a2, a3, a4, a5, a6)
#define USBD_INFO_HI_7(id,  format, a1, a2, a3, a4, a5, a6, a7)                                       IOCMD_INFO_HI_7(id,  format, a1, a2, a3, a4, a5, a6, a7)
#define USBD_INFO_HI_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)                                   IOCMD_INFO_HI_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)
#define USBD_INFO_HI_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)                               IOCMD_INFO_HI_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#define USBD_INFO_HI_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)                           IOCMD_INFO_HI_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#define USBD_INFO_HI_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)                       IOCMD_INFO_HI_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#define USBD_INFO_HI_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)                   IOCMD_INFO_HI_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#define USBD_INFO_HI_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)               IOCMD_INFO_HI_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#define USBD_INFO_HI_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)           IOCMD_INFO_HI_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#define USBD_INFO_HI_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)       IOCMD_INFO_HI_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#define USBD_INFO_HI_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)   IOCMD_INFO_HI_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#define USBD_INFO_MID(id, format)                                                                     IOCMD_INFO_MID(id, format)
#define USBD_INFO_MID_1(id,  format, a1)                                                              IOCMD_INFO_MID_1(id,  format, a1)
#define USBD_INFO_MID_2(id,  format, a1, a2)                                                          IOCMD_INFO_MID_2(id,  format, a1, a2)
#define USBD_INFO_MID_3(id,  format, a1, a2, a3)                                                      IOCMD_INFO_MID_3(id,  format, a1, a2, a3)
#define USBD_INFO_MID_4(id,  format, a1, a2, a3, a4)                                                  IOCMD_INFO_MID_4(id,  format, a1, a2, a3, a4)
#define USBD_INFO_MID_5(id,  format, a1, a2, a3, a4, a5)                                              IOCMD_INFO_MID_5(id,  format, a1, a2, a3, a4, a5)
#define USBD_INFO_MID_6(id,  format, a1, a2, a3, a4, a5, a6)                                          IOCMD_INFO_MID_6(id,  format, a1, a2, a3, a4, a5, a6)
#define USBD_INFO_MID_7(id,  format, a1, a2, a3, a4, a5, a6, a7)                                      IOCMD_INFO_MID_7(id,  format, a1, a2, a3, a4, a5, a6, a7)
#define USBD_INFO_MID_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)                                  IOCMD_INFO_MID_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)
#define USBD_INFO_MID_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)                              IOCMD_INFO_MID_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#define USBD_INFO_MID_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)                          IOCMD_INFO_MID_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#define USBD_INFO_MID_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)                      IOCMD_INFO_MID_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#define USBD_INFO_MID_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)                  IOCMD_INFO_MID_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#define USBD_INFO_MID_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)              IOCMD_INFO_MID_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#define USBD_INFO_MID_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)          IOCMD_INFO_MID_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#define USBD_INFO_MID_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)      IOCMD_INFO_MID_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#define USBD_INFO_MID_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)  IOCMD_INFO_MID_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#define USBD_INFO_LO(id, format)                                                                      IOCMD_INFO_LO(id, format)
#define USBD_INFO_LO_1(id,  format, a1)                                                               IOCMD_INFO_LO_1(id,  format, a1)
#define USBD_INFO_LO_2(id,  format, a1, a2)                                                           IOCMD_INFO_LO_2(id,  format, a1, a2)
#define USBD_INFO_LO_3(id,  format, a1, a2, a3)                                                       IOCMD_INFO_LO_3(id,  format, a1, a2, a3)
#define USBD_INFO_LO_4(id,  format, a1, a2, a3, a4)                                                   IOCMD_INFO_LO_4(id,  format, a1, a2, a3, a4)
#define USBD_INFO_LO_5(id,  format, a1, a2, a3, a4, a5)                                               IOCMD_INFO_LO_5(id,  format, a1, a2, a3, a4, a5)
#define USBD_INFO_LO_6(id,  format, a1, a2, a3, a4, a5, a6)                                           IOCMD_INFO_LO_6(id,  format, a1, a2, a3, a4, a5, a6)
#define USBD_INFO_LO_7(id,  format, a1, a2, a3, a4, a5, a6, a7)                                       IOCMD_INFO_LO_7(id,  format, a1, a2, a3, a4, a5, a6, a7)
#define USBD_INFO_LO_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)                                   IOCMD_INFO_LO_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)
#define USBD_INFO_LO_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)                               IOCMD_INFO_LO_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#define USBD_INFO_LO_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)                           IOCMD_INFO_LO_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#define USBD_INFO_LO_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)                       IOCMD_INFO_LO_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#define USBD_INFO_LO_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)                   IOCMD_INFO_LO_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#define USBD_INFO_LO_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)               IOCMD_INFO_LO_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#define USBD_INFO_LO_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)           IOCMD_INFO_LO_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#define USBD_INFO_LO_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)       IOCMD_INFO_LO_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#define USBD_INFO_LO_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)   IOCMD_INFO_LO_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#define USBD_DEBUG_HI(id, format)                                                                     IOCMD_DEBUG_HI(id, format)
#define USBD_DEBUG_HI_1(id,  format, a1)                                                              IOCMD_DEBUG_HI_1(id,  format, a1)
#define USBD_DEBUG_HI_2(id,  format, a1, a2)                                                          IOCMD_DEBUG_HI_2(id,  format, a1, a2)
#define USBD_DEBUG_HI_3(id,  format, a1, a2, a3)                                                      IOCMD_DEBUG_HI_3(id,  format, a1, a2, a3)
#define USBD_DEBUG_HI_4(id,  format, a1, a2, a3, a4)                                                  IOCMD_DEBUG_HI_4(id,  format, a1, a2, a3, a4)
#define USBD_DEBUG_HI_5(id,  format, a1, a2, a3, a4, a5)                                              IOCMD_DEBUG_HI_5(id,  format, a1, a2, a3, a4, a5)
#define USBD_DEBUG_HI_6(id,  format, a1, a2, a3, a4, a5, a6)                                          IOCMD_DEBUG_HI_6(id,  format, a1, a2, a3, a4, a5, a6)
#define USBD_DEBUG_HI_7(id,  format, a1, a2, a3, a4, a5, a6, a7)                                      IOCMD_DEBUG_HI_7(id,  format, a1, a2, a3, a4, a5, a6, a7)
#define USBD_DEBUG_HI_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)                                  IOCMD_DEBUG_HI_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)
#define USBD_DEBUG_HI_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)                              IOCMD_DEBUG_HI_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#define USBD_DEBUG_HI_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)                          IOCMD_DEBUG_HI_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#define USBD_DEBUG_HI_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)                      IOCMD_DEBUG_HI_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#define USBD_DEBUG_HI_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)                  IOCMD_DEBUG_HI_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#define USBD_DEBUG_HI_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)              IOCMD_DEBUG_HI_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#define USBD_DEBUG_HI_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)          IOCMD_DEBUG_HI_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#define USBD_DEBUG_HI_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)      IOCMD_DEBUG_HI_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#define USBD_DEBUG_HI_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)  IOCMD_DEBUG_HI_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#define USBD_DEBUG_MID(id, format)                                                                    IOCMD_DEBUG_MID(id, format)
#define USBD_DEBUG_MID_1(id,  format, a1)                                                             IOCMD_DEBUG_MID_1(id,  format, a1)
#define USBD_DEBUG_MID_2(id,  format, a1, a2)                                                         IOCMD_DEBUG_MID_2(id,  format, a1, a2)
#define USBD_DEBUG_MID_3(id,  format, a1, a2, a3)                                                     IOCMD_DEBUG_MID_3(id,  format, a1, a2, a3)
#define USBD_DEBUG_MID_4(id,  format, a1, a2, a3, a4)                                                 IOCMD_DEBUG_MID_4(id,  format, a1, a2, a3, a4)
#define USBD_DEBUG_MID_5(id,  format, a1, a2, a3, a4, a5)                                             IOCMD_DEBUG_MID_5(id,  format, a1, a2, a3, a4, a5)
#define USBD_DEBUG_MID_6(id,  format, a1, a2, a3, a4, a5, a6)                                         IOCMD_DEBUG_MID_6(id,  format, a1, a2, a3, a4, a5, a6)
#define USBD_DEBUG_MID_7(id,  format, a1, a2, a3, a4, a5, a6, a7)                                     IOCMD_DEBUG_MID_7(id,  format, a1, a2, a3, a4, a5, a6, a7)
#define USBD_DEBUG_MID_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)                                 IOCMD_DEBUG_MID_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)
#define USBD_DEBUG_MID_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)                             IOCMD_DEBUG_MID_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#define USBD_DEBUG_MID_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)                         IOCMD_DEBUG_MID_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#define USBD_DEBUG_MID_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)                     IOCMD_DEBUG_MID_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#define USBD_DEBUG_MID_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)                 IOCMD_DEBUG_MID_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#define USBD_DEBUG_MID_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)             IOCMD_DEBUG_MID_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#define USBD_DEBUG_MID_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)         IOCMD_DEBUG_MID_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#define USBD_DEBUG_MID_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)     IOCMD_DEBUG_MID_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#define USBD_DEBUG_MID_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG) IOCMD_DEBUG_MID_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#define USBD_DEBUG_LO(id, format)                                                                     IOCMD_DEBUG_LO(id, format)
#define USBD_DEBUG_LO_1(id,  format, a1)                                                              IOCMD_DEBUG_LO_1(id,  format, a1)
#define USBD_DEBUG_LO_2(id,  format, a1, a2)                                                          IOCMD_DEBUG_LO_2(id,  format, a1, a2)
#define USBD_DEBUG_LO_3(id,  format, a1, a2, a3)                                                      IOCMD_DEBUG_LO_3(id,  format, a1, a2, a3)
#define USBD_DEBUG_LO_4(id,  format, a1, a2, a3, a4)                                                  IOCMD_DEBUG_LO_4(id,  format, a1, a2, a3, a4)
#define USBD_DEBUG_LO_5(id,  format, a1, a2, a3, a4, a5)                                              IOCMD_DEBUG_LO_5(id,  format, a1, a2, a3, a4, a5)
#define USBD_DEBUG_LO_6(id,  format, a1, a2, a3, a4, a5, a6)                                          IOCMD_DEBUG_LO_6(id,  format, a1, a2, a3, a4, a5, a6)
#define USBD_DEBUG_LO_7(id,  format, a1, a2, a3, a4, a5, a6, a7)                                      IOCMD_DEBUG_LO_7(id,  format, a1, a2, a3, a4, a5, a6, a7)
#define USBD_DEBUG_LO_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)                                  IOCMD_DEBUG_LO_8(id,  format, a1, a2, a3, a4, a5, a6, a7, a8)
#define USBD_DEBUG_LO_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)                              IOCMD_DEBUG_LO_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#define USBD_DEBUG_LO_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)                          IOCMD_DEBUG_LO_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#define USBD_DEBUG_LO_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)                      IOCMD_DEBUG_LO_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#define USBD_DEBUG_LO_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)                  IOCMD_DEBUG_LO_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#define USBD_DEBUG_LO_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)              IOCMD_DEBUG_LO_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#define USBD_DEBUG_LO_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)          IOCMD_DEBUG_LO_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#define USBD_DEBUG_LO_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)      IOCMD_DEBUG_LO_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#define USBD_DEBUG_LO_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)  IOCMD_DEBUG_LO_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)

#define USBD_LOG_DATA_EMERG(      id, data, size, description)                                        IOCMD_LOG_DATA_EMERG(      id, data, size, description)
#define USBD_LOG_DATA_EMERG_1(    id, data, size, description, a1)                                    IOCMD_LOG_DATA_EMERG_1(    id, data, size, description, a1)
#define USBD_LOG_DATA_EMERG_2(    id, data, size, description, a1, a2)                                IOCMD_LOG_DATA_EMERG_2(    id, data, size, description, a1, a2)
#define USBD_LOG_DATA_EMERG_3(    id, data, size, description, a1, a2, a3)                            IOCMD_LOG_DATA_EMERG_3(    id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_ALERT(      id, data, size, description)                                        IOCMD_LOG_DATA_ALERT(      id, data, size, description)
#define USBD_LOG_DATA_ALERT_1(    id, data, size, description, a1)                                    IOCMD_LOG_DATA_ALERT_1(    id, data, size, description, a1)
#define USBD_LOG_DATA_ALERT_2(    id, data, size, description, a1, a2)                                IOCMD_LOG_DATA_ALERT_2(    id, data, size, description, a1, a2)
#define USBD_LOG_DATA_ALERT_3(    id, data, size, description, a1, a2, a3)                            IOCMD_LOG_DATA_ALERT_3(    id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_CRIT(       id, data, size, description)                                        IOCMD_LOG_DATA_CRIT(       id, data, size, description)
#define USBD_LOG_DATA_CRIT_1(     id, data, size, description, a1)                                    IOCMD_LOG_DATA_CRIT_1(     id, data, size, description, a1)
#define USBD_LOG_DATA_CRIT_2(     id, data, size, description, a1, a2)                                IOCMD_LOG_DATA_CRIT_2(     id, data, size, description, a1, a2)
#define USBD_LOG_DATA_CRIT_3(     id, data, size, description, a1, a2, a3)                            IOCMD_LOG_DATA_CRIT_3(     id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_ERROR(      id, data, size, description)                                        IOCMD_LOG_DATA_ERROR(      id, data, size, description)
#define USBD_LOG_DATA_ERROR_1(    id, data, size, description, a1)                                    IOCMD_LOG_DATA_ERROR_1(    id, data, size, description, a1)
#define USBD_LOG_DATA_ERROR_2(    id, data, size, description, a1, a2)                                IOCMD_LOG_DATA_ERROR_2(    id, data, size, description, a1, a2)
#define USBD_LOG_DATA_ERROR_3(    id, data, size, description, a1, a2, a3)                            IOCMD_LOG_DATA_ERROR_3(    id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_WARN(       id, data, size, description)                                        IOCMD_LOG_DATA_WARN(       id, data, size, description)
#define USBD_LOG_DATA_WARN_1(     id, data, size, description, a1)                                    IOCMD_LOG_DATA_WARN_1(     id, data, size, description, a1)
#define USBD_LOG_DATA_WARN_2(     id, data, size, description, a1, a2)                                IOCMD_LOG_DATA_WARN_2(     id, data, size, description, a1, a2)
#define USBD_LOG_DATA_WARN_3(     id, data, size, description, a1, a2, a3)                            IOCMD_LOG_DATA_WARN_3(     id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_NOTICE(     id, data, size, description)                                        IOCMD_LOG_DATA_NOTICE(     id, data, size, description)
#define USBD_LOG_DATA_NOTICE_1(   id, data, size, description, a1)                                    IOCMD_LOG_DATA_NOTICE_1(   id, data, size, description, a1)
#define USBD_LOG_DATA_NOTICE_2(   id, data, size, description, a1, a2)                                IOCMD_LOG_DATA_NOTICE_2(   id, data, size, description, a1, a2)
#define USBD_LOG_DATA_NOTICE_3(   id, data, size, description, a1, a2, a3)                            IOCMD_LOG_DATA_NOTICE_3(   id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_INFO_HI(    id, data, size, description)                                        IOCMD_LOG_DATA_INFO_HI(    id, data, size, description)
#define USBD_LOG_DATA_INFO_HI_1(  id, data, size, description, a1)                                    IOCMD_LOG_DATA_INFO_HI_1(  id, data, size, description, a1)
#define USBD_LOG_DATA_INFO_HI_2(  id, data, size, description, a1, a2)                                IOCMD_LOG_DATA_INFO_HI_2(  id, data, size, description, a1, a2)
#define USBD_LOG_DATA_INFO_HI_3(  id, data, size, description, a1, a2, a3)                            IOCMD_LOG_DATA_INFO_HI_3(  id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_INFO_MID(   id, data, size, description)                                        IOCMD_LOG_DATA_INFO_MID(   id, data, size, description)
#define USBD_LOG_DATA_INFO_MID_1( id, data, size, description, a1)                                    IOCMD_LOG_DATA_INFO_MID_1( id, data, size, description, a1)
#define USBD_LOG_DATA_INFO_MID_2( id, data, size, description, a1, a2)                                IOCMD_LOG_DATA_INFO_MID_2( id, data, size, description, a1, a2)
#define USBD_LOG_DATA_INFO_MID_3( id, data, size, description, a1, a2, a3)                            IOCMD_LOG_DATA_INFO_MID_3( id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_INFO_LO(    id, data, size, description)                                        IOCMD_LOG_DATA_INFO_LO(    id, data, size, description)
#define USBD_LOG_DATA_INFO_LO_1(  id, data, size, description, a1)                                    IOCMD_LOG_DATA_INFO_LO_1(  id, data, size, description, a1)
#define USBD_LOG_DATA_INFO_LO_2(  id, data, size, description, a1, a2)                                IOCMD_LOG_DATA_INFO_LO_2(  id, data, size, description, a1, a2)
#define USBD_LOG_DATA_INFO_LO_3(  id, data, size, description, a1, a2, a3)                            IOCMD_LOG_DATA_INFO_LO_3(  id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_DEBUG_HI(   id, data, size, description)                                        IOCMD_LOG_DATA_DEBUG_HI(   id, data, size, description)
#define USBD_LOG_DATA_DEBUG_HI_1( id, data, size, description, a1)                                    IOCMD_LOG_DATA_DEBUG_HI_1( id, data, size, description, a1)
#define USBD_LOG_DATA_DEBUG_HI_2( id, data, size, description, a1, a2)                                IOCMD_LOG_DATA_DEBUG_HI_2( id, data, size, description, a1, a2)
#define USBD_LOG_DATA_DEBUG_HI_3( id, data, size, description, a1, a2, a3)                            IOCMD_LOG_DATA_DEBUG_HI_3( id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_DEBUG_MID(  id, data, size, description)                                        IOCMD_LOG_DATA_DEBUG_MID(  id, data, size, description)
#define USBD_LOG_DATA_DEBUG_MID_1(id, data, size, description, a1)                                    IOCMD_LOG_DATA_DEBUG_MID_1(id, data, size, description, a1)
#define USBD_LOG_DATA_DEBUG_MID_2(id, data, size, description, a1, a2)                                IOCMD_LOG_DATA_DEBUG_MID_2(id, data, size, description, a1, a2)
#define USBD_LOG_DATA_DEBUG_MID_3(id, data, size, description, a1, a2, a3)                            IOCMD_LOG_DATA_DEBUG_MID_3(id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_DEBUG_LO(   id, data, size, description)                                        IOCMD_LOG_DATA_DEBUG_LO(   id, data, size, description)
#define USBD_LOG_DATA_DEBUG_LO_1( id, data, size, description, a1)                                    IOCMD_LOG_DATA_DEBUG_LO_1( id, data, size, description, a1)
#define USBD_LOG_DATA_DEBUG_LO_2( id, data, size, description, a1, a2)                                IOCMD_LOG_DATA_DEBUG_LO_2( id, data, size, description, a1, a2)
#define USBD_LOG_DATA_DEBUG_LO_3( id, data, size, description, a1, a2, a3)                            IOCMD_LOG_DATA_DEBUG_LO_3( id, data, size, description, a1, a2, a3)


#define USBD_COMPARE_DATA_EMERG(      id, data1, size1, data2, size2, description)                    IOCMD_COMPARE_DATA_EMERG(      id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_EMERG_1(    id, data1, size1, data2, size2, description, a1)                IOCMD_COMPARE_DATA_EMERG_1(    id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_EMERG_2(    id, data1, size1, data2, size2, description, a1, a2)            IOCMD_COMPARE_DATA_EMERG_2(    id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_EMERG_3(    id, data1, size1, data2, size2, description, a1, a2, a3)        IOCMD_COMPARE_DATA_EMERG_3(    id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_ALERT(      id, data1, size1, data2, size2, description)                    IOCMD_COMPARE_DATA_ALERT(      id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_ALERT_1(    id, data1, size1, data2, size2, description, a1)                IOCMD_COMPARE_DATA_ALERT_1(    id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_ALERT_2(    id, data1, size1, data2, size2, description, a1, a2)            IOCMD_COMPARE_DATA_ALERT_2(    id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_ALERT_3(    id, data1, size1, data2, size2, description, a1, a2, a3)        IOCMD_COMPARE_DATA_ALERT_3(    id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_CRIT(       id, data1, size1, data2, size2, description)                    IOCMD_COMPARE_DATA_CRIT(       id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_CRIT_1(     id, data1, size1, data2, size2, description, a1)                IOCMD_COMPARE_DATA_CRIT_1(     id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_CRIT_2(     id, data1, size1, data2, size2, description, a1, a2)            IOCMD_COMPARE_DATA_CRIT_2(     id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_CRIT_3(     id, data1, size1, data2, size2, description, a1, a2, a3)        IOCMD_COMPARE_DATA_CRIT_3(     id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_ERROR(      id, data1, size1, data2, size2, description)                    IOCMD_COMPARE_DATA_ERROR(      id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_ERROR_1(    id, data1, size1, data2, size2, description, a1)                IOCMD_COMPARE_DATA_ERROR_1(    id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_ERROR_2(    id, data1, size1, data2, size2, description, a1, a2)            IOCMD_COMPARE_DATA_ERROR_2(    id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_ERROR_3(    id, data1, size1, data2, size2, description, a1, a2, a3)        IOCMD_COMPARE_DATA_ERROR_3(    id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_WARN(       id, data1, size1, data2, size2, description)                    IOCMD_COMPARE_DATA_WARN(       id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_WARN_1(     id, data1, size1, data2, size2, description, a1)                IOCMD_COMPARE_DATA_WARN_1(     id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_WARN_2(     id, data1, size1, data2, size2, description, a1, a2)            IOCMD_COMPARE_DATA_WARN_2(     id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_WARN_3(     id, data1, size1, data2, size2, description, a1, a2, a3)        IOCMD_COMPARE_DATA_WARN_3(     id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_NOTICE(     id, data1, size1, data2, size2, description)                    IOCMD_COMPARE_DATA_NOTICE(     id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_NOTICE_1(   id, data1, size1, data2, size2, description, a1)                IOCMD_COMPARE_DATA_NOTICE_1(   id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_NOTICE_2(   id, data1, size1, data2, size2, description, a1, a2)            IOCMD_COMPARE_DATA_NOTICE_2(   id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_NOTICE_3(   id, data1, size1, data2, size2, description, a1, a2, a3)        IOCMD_COMPARE_DATA_NOTICE_3(   id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_INFO_HI(    id, data1, size1, data2, size2, description)                    IOCMD_COMPARE_DATA_INFO_HI(    id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_INFO_HI_1(  id, data1, size1, data2, size2, description, a1)                IOCMD_COMPARE_DATA_INFO_HI_1(  id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_INFO_HI_2(  id, data1, size1, data2, size2, description, a1, a2)            IOCMD_COMPARE_DATA_INFO_HI_2(  id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_INFO_HI_3(  id, data1, size1, data2, size2, description, a1, a2, a3)        IOCMD_COMPARE_DATA_INFO_HI_3(  id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_INFO_MID(   id, data1, size1, data2, size2, description)                    IOCMD_COMPARE_DATA_INFO_MID(   id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_INFO_MID_1( id, data1, size1, data2, size2, description, a1)                IOCMD_COMPARE_DATA_INFO_MID_1( id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_INFO_MID_2( id, data1, size1, data2, size2, description, a1, a2)            IOCMD_COMPARE_DATA_INFO_MID_2( id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_INFO_MID_3( id, data1, size1, data2, size2, description, a1, a2, a3)        IOCMD_COMPARE_DATA_INFO_MID_3( id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_INFO_LO(    id, data1, size1, data2, size2, description)                    IOCMD_COMPARE_DATA_INFO_LO(    id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_INFO_LO_1(  id, data1, size1, data2, size2, description, a1)                IOCMD_COMPARE_DATA_INFO_LO_1(  id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_INFO_LO_2(  id, data1, size1, data2, size2, description, a1, a2)            IOCMD_COMPARE_DATA_INFO_LO_2(  id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_INFO_LO_3(  id, data1, size1, data2, size2, description, a1, a2, a3)        IOCMD_COMPARE_DATA_INFO_LO_3(  id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_DEBUG_HI(   id, data1, size1, data2, size2, description)                    IOCMD_COMPARE_DATA_DEBUG_HI(   id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_DEBUG_HI_1( id, data1, size1, data2, size2, description, a1)                IOCMD_COMPARE_DATA_DEBUG_HI_1( id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_DEBUG_HI_2( id, data1, size1, data2, size2, description, a1, a2)            IOCMD_COMPARE_DATA_DEBUG_HI_2( id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_DEBUG_HI_3( id, data1, size1, data2, size2, description, a1, a2, a3)        IOCMD_COMPARE_DATA_DEBUG_HI_3( id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_DEBUG_MID(  id, data1, size1, data2, size2, description)                    IOCMD_COMPARE_DATA_DEBUG_MID(  id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_DEBUG_MID_1(id, data1, size1, data2, size2, description, a1)                IOCMD_COMPARE_DATA_DEBUG_MID_1(id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_DEBUG_MID_2(id, data1, size1, data2, size2, description, a1, a2)            IOCMD_COMPARE_DATA_DEBUG_MID_2(id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_DEBUG_MID_3(id, data1, size1, data2, size2, description, a1, a2, a3)        IOCMD_COMPARE_DATA_DEBUG_MID_3(id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_DEBUG_LO(   id, data1, size1, data2, size2, description)                    IOCMD_COMPARE_DATA_DEBUG_LO(   id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_DEBUG_LO_1( id, data1, size1, data2, size2, description, a1)                IOCMD_COMPARE_DATA_DEBUG_LO_1( id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_DEBUG_LO_2( id, data1, size1, data2, size2, description, a1, a2)            IOCMD_COMPARE_DATA_DEBUG_LO_2( id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_DEBUG_LO_3( id, data1, size1, data2, size2, description, a1, a2, a3)        IOCMD_COMPARE_DATA_DEBUG_LO_3( id, data1, size1, data2, size2, description, a1, a2, a3)

#else

#define USBD_COMPILATION_SWITCH_LOG(id, level)                                                        0
#define USBD_COMPILATION_SWITCH_ENT(id)                                                               0

#ifndef USBD_ENTER_FUNC
#define USBD_ENTER_FUNC(id)
#endif

#ifndef USBD_EXIT_FUNC
#define USBD_EXIT_FUNC(id)
#endif

#ifndef USBD_EMERG
#define USBD_EMERG(id, format)
#endif

#ifndef USBD_EMERG_1
#define USBD_EMERG_1(id, format, a1)
#endif

#ifndef USBD_EMERG_2
#define USBD_EMERG_2(id, format, a1, a2)
#endif

#ifndef USBD_EMERG_3
#define USBD_EMERG_3(id, format, a1, a2, a3)
#endif

#ifndef USBD_EMERG_4
#define USBD_EMERG_4(id, format, a1, a2, a3, a4)
#endif

#ifndef USBD_EMERG_5
#define USBD_EMERG_5(id, format, a1, a2, a3, a4, a5)
#endif

#ifndef USBD_EMERG_6
#define USBD_EMERG_6(id, format, a1, a2, a3, a4, a5, a6)
#endif

#ifndef USBD_EMERG_7
#define USBD_EMERG_7(id, format, a1, a2, a3, a4, a5, a6, a7)
#endif

#ifndef USBD_EMERG_8
#define USBD_EMERG_8(id, format, a1, a2, a3, a4, a5, a6, a7, a8)
#endif

#ifndef USBD_EMERG_9
#define USBD_EMERG_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#endif

#ifndef USBD_EMERG_10
#define USBD_EMERG_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#endif

#ifndef USBD_EMERG_11
#define USBD_EMERG_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#endif

#ifndef USBD_EMERG_12
#define USBD_EMERG_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#endif

#ifndef USBD_EMERG_13
#define USBD_EMERG_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#endif

#ifndef USBD_EMERG_14
#define USBD_EMERG_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#endif

#ifndef USBD_EMERG_15
#define USBD_EMERG_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#endif

#ifndef USBD_EMERG_16
#define USBD_EMERG_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#endif

#ifndef USBD_ALERT
#define USBD_ALERT(id, format)
#endif

#ifndef USBD_ALERT_1
#define USBD_ALERT_1(id, format, a1)
#endif

#ifndef USBD_ALERT_2
#define USBD_ALERT_2(id, format, a1, a2)
#endif

#ifndef USBD_ALERT_3
#define USBD_ALERT_3(id, format, a1, a2, a3)
#endif

#ifndef USBD_ALERT_4
#define USBD_ALERT_4(id, format, a1, a2, a3, a4)
#endif

#ifndef USBD_ALERT_5
#define USBD_ALERT_5(id, format, a1, a2, a3, a4, a5)
#endif

#ifndef USBD_ALERT_6
#define USBD_ALERT_6(id, format, a1, a2, a3, a4, a5, a6)
#endif

#ifndef USBD_ALERT_7
#define USBD_ALERT_7(id, format, a1, a2, a3, a4, a5, a6, a7)
#endif

#ifndef USBD_ALERT_8
#define USBD_ALERT_8(id, format, a1, a2, a3, a4, a5, a6, a7, a8)
#endif

#ifndef USBD_ALERT_9
#define USBD_ALERT_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#endif

#ifndef USBD_ALERT_10
#define USBD_ALERT_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#endif

#ifndef USBD_ALERT_11
#define USBD_ALERT_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#endif

#ifndef USBD_ALERT_12
#define USBD_ALERT_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#endif

#ifndef USBD_ALERT_13
#define USBD_ALERT_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#endif

#ifndef USBD_ALERT_14
#define USBD_ALERT_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#endif

#ifndef USBD_ALERT_15
#define USBD_ALERT_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#endif

#ifndef USBD_ALERT_16
#define USBD_ALERT_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#endif

#ifndef USBD_CRIT
#define USBD_CRIT(id, format)
#endif

#ifndef USBD_CRIT_1
#define USBD_CRIT_1(id, format, a1)
#endif

#ifndef USBD_CRIT_2
#define USBD_CRIT_2(id, format, a1, a2)
#endif

#ifndef USBD_CRIT_3
#define USBD_CRIT_3(id, format, a1, a2, a3)
#endif

#ifndef USBD_CRIT_4
#define USBD_CRIT_4(id, format, a1, a2, a3, a4)
#endif

#ifndef USBD_CRIT_5
#define USBD_CRIT_5(id, format, a1, a2, a3, a4, a5)
#endif

#ifndef USBD_CRIT_6
#define USBD_CRIT_6(id, format, a1, a2, a3, a4, a5, a6)
#endif

#ifndef USBD_CRIT_7
#define USBD_CRIT_7(id, format, a1, a2, a3, a4, a5, a6, a7)
#endif

#ifndef USBD_CRIT_8
#define USBD_CRIT_8(id, format, a1, a2, a3, a4, a5, a6, a7, a8)
#endif

#ifndef USBD_CRIT_9
#define USBD_CRIT_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#endif

#ifndef USBD_CRIT_10
#define USBD_CRIT_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#endif

#ifndef USBD_CRIT_11
#define USBD_CRIT_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#endif

#ifndef USBD_CRIT_12
#define USBD_CRIT_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#endif

#ifndef USBD_CRIT_13
#define USBD_CRIT_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#endif

#ifndef USBD_CRIT_14
#define USBD_CRIT_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#endif

#ifndef USBD_CRIT_15
#define USBD_CRIT_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#endif

#ifndef USBD_CRIT_16
#define USBD_CRIT_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#endif

#ifndef USBD_ERROR
#define USBD_ERROR(id, format)
#endif

#ifndef USBD_ERROR_1
#define USBD_ERROR_1(id, format, a1)
#endif

#ifndef USBD_ERROR_2
#define USBD_ERROR_2(id, format, a1, a2)
#endif

#ifndef USBD_ERROR_3
#define USBD_ERROR_3(id, format, a1, a2, a3)
#endif

#ifndef USBD_ERROR_4
#define USBD_ERROR_4(id, format, a1, a2, a3, a4)
#endif

#ifndef USBD_ERROR_5
#define USBD_ERROR_5(id, format, a1, a2, a3, a4, a5)
#endif

#ifndef USBD_ERROR_6
#define USBD_ERROR_6(id, format, a1, a2, a3, a4, a5, a6)
#endif

#ifndef USBD_ERROR_7
#define USBD_ERROR_7(id, format, a1, a2, a3, a4, a5, a6, a7)
#endif

#ifndef USBD_ERROR_8
#define USBD_ERROR_8(id, format, a1, a2, a3, a4, a5, a6, a7, a8)
#endif

#ifndef USBD_ERROR_9
#define USBD_ERROR_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#endif

#ifndef USBD_ERROR_10
#define USBD_ERROR_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#endif

#ifndef USBD_ERROR_11
#define USBD_ERROR_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#endif

#ifndef USBD_ERROR_12
#define USBD_ERROR_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#endif

#ifndef USBD_ERROR_13
#define USBD_ERROR_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#endif

#ifndef USBD_ERROR_14
#define USBD_ERROR_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#endif

#ifndef USBD_ERROR_15
#define USBD_ERROR_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#endif

#ifndef USBD_ERROR_16
#define USBD_ERROR_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#endif

#ifndef USBD_WARN
#define USBD_WARN(id, format)
#endif

#ifndef USBD_WARN_1
#define USBD_WARN_1(id, format, a1)
#endif

#ifndef USBD_WARN_2
#define USBD_WARN_2(id, format, a1, a2)
#endif

#ifndef USBD_WARN_3
#define USBD_WARN_3(id, format, a1, a2, a3)
#endif

#ifndef USBD_WARN_4
#define USBD_WARN_4(id, format, a1, a2, a3, a4)
#endif

#ifndef USBD_WARN_5
#define USBD_WARN_5(id, format, a1, a2, a3, a4, a5)
#endif

#ifndef USBD_WARN_6
#define USBD_WARN_6(id, format, a1, a2, a3, a4, a5, a6)
#endif

#ifndef USBD_WARN_7
#define USBD_WARN_7(id, format, a1, a2, a3, a4, a5, a6, a7)
#endif

#ifndef USBD_WARN_8
#define USBD_WARN_8(id, format, a1, a2, a3, a4, a5, a6, a7, a8)
#endif

#ifndef USBD_WARN_9
#define USBD_WARN_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#endif

#ifndef USBD_WARN_10
#define USBD_WARN_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#endif

#ifndef USBD_WARN_11
#define USBD_WARN_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#endif

#ifndef USBD_WARN_12
#define USBD_WARN_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#endif

#ifndef USBD_WARN_13
#define USBD_WARN_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#endif

#ifndef USBD_WARN_14
#define USBD_WARN_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#endif

#ifndef USBD_WARN_15
#define USBD_WARN_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#endif

#ifndef USBD_WARN_16
#define USBD_WARN_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#endif

#ifndef USBD_NOTICE
#define USBD_NOTICE(id, format)
#endif

#ifndef USBD_NOTICE_1
#define USBD_NOTICE_1(id, format, a1)
#endif

#ifndef USBD_NOTICE_2
#define USBD_NOTICE_2(id, format, a1, a2)
#endif

#ifndef USBD_NOTICE_3
#define USBD_NOTICE_3(id, format, a1, a2, a3)
#endif

#ifndef USBD_NOTICE_4
#define USBD_NOTICE_4(id, format, a1, a2, a3, a4)
#endif

#ifndef USBD_NOTICE_5
#define USBD_NOTICE_5(id, format, a1, a2, a3, a4, a5)
#endif

#ifndef USBD_NOTICE_6
#define USBD_NOTICE_6(id, format, a1, a2, a3, a4, a5, a6)
#endif

#ifndef USBD_NOTICE_7
#define USBD_NOTICE_7(id, format, a1, a2, a3, a4, a5, a6, a7)
#endif

#ifndef USBD_NOTICE_8
#define USBD_NOTICE_8(id, format, a1, a2, a3, a4, a5, a6, a7, a8)
#endif

#ifndef USBD_NOTICE_9
#define USBD_NOTICE_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#endif

#ifndef USBD_NOTICE_10
#define USBD_NOTICE_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#endif

#ifndef USBD_NOTICE_11
#define USBD_NOTICE_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#endif

#ifndef USBD_NOTICE_12
#define USBD_NOTICE_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#endif

#ifndef USBD_NOTICE_13
#define USBD_NOTICE_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#endif

#ifndef USBD_NOTICE_14
#define USBD_NOTICE_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#endif

#ifndef USBD_NOTICE_15
#define USBD_NOTICE_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#endif

#ifndef USBD_NOTICE_16
#define USBD_NOTICE_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#endif

#ifndef USBD_INFO_HI
#define USBD_INFO_HI(id, format)
#endif

#ifndef USBD_INFO_HI_1
#define USBD_INFO_HI_1(id, format, a1)
#endif

#ifndef USBD_INFO_HI_2
#define USBD_INFO_HI_2(id, format, a1, a2)
#endif

#ifndef USBD_INFO_HI_3
#define USBD_INFO_HI_3(id, format, a1, a2, a3)
#endif

#ifndef USBD_INFO_HI_4
#define USBD_INFO_HI_4(id, format, a1, a2, a3, a4)
#endif

#ifndef USBD_INFO_HI_5
#define USBD_INFO_HI_5(id, format, a1, a2, a3, a4, a5)
#endif

#ifndef USBD_INFO_HI_6
#define USBD_INFO_HI_6(id, format, a1, a2, a3, a4, a5, a6)
#endif

#ifndef USBD_INFO_HI_7
#define USBD_INFO_HI_7(id, format, a1, a2, a3, a4, a5, a6, a7)
#endif

#ifndef USBD_INFO_HI_8
#define USBD_INFO_HI_8(id, format, a1, a2, a3, a4, a5, a6, a7, a8)
#endif

#ifndef USBD_INFO_HI_9
#define USBD_INFO_HI_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#endif

#ifndef USBD_INFO_HI_10
#define USBD_INFO_HI_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#endif

#ifndef USBD_INFO_HI_11
#define USBD_INFO_HI_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#endif

#ifndef USBD_INFO_HI_12
#define USBD_INFO_HI_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#endif

#ifndef USBD_INFO_HI_13
#define USBD_INFO_HI_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#endif

#ifndef USBD_INFO_HI_14
#define USBD_INFO_HI_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#endif

#ifndef USBD_INFO_HI_15
#define USBD_INFO_HI_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#endif

#ifndef USBD_INFO_HI_16
#define USBD_INFO_HI_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#endif

#ifndef USBD_INFO_MID
#define USBD_INFO_MID(id, format)
#endif

#ifndef USBD_INFO_MID_1
#define USBD_INFO_MID_1(id, format, a1)
#endif

#ifndef USBD_INFO_MID_2
#define USBD_INFO_MID_2(id, format, a1, a2)
#endif

#ifndef USBD_INFO_MID_3
#define USBD_INFO_MID_3(id, format, a1, a2, a3)
#endif

#ifndef USBD_INFO_MID_4
#define USBD_INFO_MID_4(id, format, a1, a2, a3, a4)
#endif

#ifndef USBD_INFO_MID_5
#define USBD_INFO_MID_5(id, format, a1, a2, a3, a4, a5)
#endif

#ifndef USBD_INFO_MID_6
#define USBD_INFO_MID_6(id, format, a1, a2, a3, a4, a5, a6)
#endif

#ifndef USBD_INFO_MID_7
#define USBD_INFO_MID_7(id, format, a1, a2, a3, a4, a5, a6, a7)
#endif

#ifndef USBD_INFO_MID_8
#define USBD_INFO_MID_8(id, format, a1, a2, a3, a4, a5, a6, a7, a8)
#endif

#ifndef USBD_INFO_MID_9
#define USBD_INFO_MID_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#endif

#ifndef USBD_INFO_MID_10
#define USBD_INFO_MID_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#endif

#ifndef USBD_INFO_MID_11
#define USBD_INFO_MID_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#endif

#ifndef USBD_INFO_MID_12
#define USBD_INFO_MID_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#endif

#ifndef USBD_INFO_MID_13
#define USBD_INFO_MID_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#endif

#ifndef USBD_INFO_MID_14
#define USBD_INFO_MID_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#endif

#ifndef USBD_INFO_MID_15
#define USBD_INFO_MID_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#endif

#ifndef USBD_INFO_MID_16
#define USBD_INFO_MID_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#endif

#ifndef USBD_INFO_LO
#define USBD_INFO_LO(id, format)
#endif

#ifndef USBD_INFO_LO_1
#define USBD_INFO_LO_1(id, format, a1)
#endif

#ifndef USBD_INFO_LO_2
#define USBD_INFO_LO_2(id, format, a1, a2)
#endif

#ifndef USBD_INFO_LO_3
#define USBD_INFO_LO_3(id, format, a1, a2, a3)
#endif

#ifndef USBD_INFO_LO_4
#define USBD_INFO_LO_4(id, format, a1, a2, a3, a4)
#endif

#ifndef USBD_INFO_LO_5
#define USBD_INFO_LO_5(id, format, a1, a2, a3, a4, a5)
#endif

#ifndef USBD_INFO_LO_6
#define USBD_INFO_LO_6(id, format, a1, a2, a3, a4, a5, a6)
#endif

#ifndef USBD_INFO_LO_7
#define USBD_INFO_LO_7(id, format, a1, a2, a3, a4, a5, a6, a7)
#endif

#ifndef USBD_INFO_LO_8
#define USBD_INFO_LO_8(id, format, a1, a2, a3, a4, a5, a6, a7, a8)
#endif

#ifndef USBD_INFO_LO_9
#define USBD_INFO_LO_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#endif

#ifndef USBD_INFO_LO_10
#define USBD_INFO_LO_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#endif

#ifndef USBD_INFO_LO_11
#define USBD_INFO_LO_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#endif

#ifndef USBD_INFO_LO_12
#define USBD_INFO_LO_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#endif

#ifndef USBD_INFO_LO_13
#define USBD_INFO_LO_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#endif

#ifndef USBD_INFO_LO_14
#define USBD_INFO_LO_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#endif

#ifndef USBD_INFO_LO_15
#define USBD_INFO_LO_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#endif

#ifndef USBD_INFO_LO_16
#define USBD_INFO_LO_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#endif

#ifndef USBD_DEBUG_HI
#define USBD_DEBUG_HI(id, format)
#endif

#ifndef USBD_DEBUG_HI_1
#define USBD_DEBUG_HI_1(id, format, a1)
#endif

#ifndef USBD_DEBUG_HI_2
#define USBD_DEBUG_HI_2(id, format, a1, a2)
#endif

#ifndef USBD_DEBUG_HI_3
#define USBD_DEBUG_HI_3(id, format, a1, a2, a3)
#endif

#ifndef USBD_DEBUG_HI_4
#define USBD_DEBUG_HI_4(id, format, a1, a2, a3, a4)
#endif

#ifndef USBD_DEBUG_HI_5
#define USBD_DEBUG_HI_5(id, format, a1, a2, a3, a4, a5)
#endif

#ifndef USBD_DEBUG_HI_6
#define USBD_DEBUG_HI_6(id, format, a1, a2, a3, a4, a5, a6)
#endif

#ifndef USBD_DEBUG_HI_7
#define USBD_DEBUG_HI_7(id, format, a1, a2, a3, a4, a5, a6, a7)
#endif

#ifndef USBD_DEBUG_HI_8
#define USBD_DEBUG_HI_8(id, format, a1, a2, a3, a4, a5, a6, a7, a8)
#endif

#ifndef USBD_DEBUG_HI_9
#define USBD_DEBUG_HI_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#endif

#ifndef USBD_DEBUG_HI_10
#define USBD_DEBUG_HI_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#endif

#ifndef USBD_DEBUG_HI_11
#define USBD_DEBUG_HI_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#endif

#ifndef USBD_DEBUG_HI_12
#define USBD_DEBUG_HI_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#endif

#ifndef USBD_DEBUG_HI_13
#define USBD_DEBUG_HI_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#endif

#ifndef USBD_DEBUG_HI_14
#define USBD_DEBUG_HI_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#endif

#ifndef USBD_DEBUG_HI_15
#define USBD_DEBUG_HI_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#endif

#ifndef USBD_DEBUG_HI_16
#define USBD_DEBUG_HI_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#endif

#ifndef USBD_DEBUG_MID
#define USBD_DEBUG_MID(id, format)
#endif

#ifndef USBD_DEBUG_MID_1
#define USBD_DEBUG_MID_1(id, format, a1)
#endif

#ifndef USBD_DEBUG_MID_2
#define USBD_DEBUG_MID_2(id, format, a1, a2)
#endif

#ifndef USBD_DEBUG_MID_3
#define USBD_DEBUG_MID_3(id, format, a1, a2, a3)
#endif

#ifndef USBD_DEBUG_MID_4
#define USBD_DEBUG_MID_4(id, format, a1, a2, a3, a4)
#endif

#ifndef USBD_DEBUG_MID_5
#define USBD_DEBUG_MID_5(id, format, a1, a2, a3, a4, a5)
#endif

#ifndef USBD_DEBUG_MID_6
#define USBD_DEBUG_MID_6(id, format, a1, a2, a3, a4, a5, a6)
#endif

#ifndef USBD_DEBUG_MID_7
#define USBD_DEBUG_MID_7(id, format, a1, a2, a3, a4, a5, a6, a7)
#endif

#ifndef USBD_DEBUG_MID_8
#define USBD_DEBUG_MID_8(id, format, a1, a2, a3, a4, a5, a6, a7, a8)
#endif

#ifndef USBD_DEBUG_MID_9
#define USBD_DEBUG_MID_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#endif

#ifndef USBD_DEBUG_MID_10
#define USBD_DEBUG_MID_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#endif

#ifndef USBD_DEBUG_MID_11
#define USBD_DEBUG_MID_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#endif

#ifndef USBD_DEBUG_MID_12
#define USBD_DEBUG_MID_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#endif

#ifndef USBD_DEBUG_MID_13
#define USBD_DEBUG_MID_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#endif

#ifndef USBD_DEBUG_MID_14
#define USBD_DEBUG_MID_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#endif

#ifndef USBD_DEBUG_MID_15
#define USBD_DEBUG_MID_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#endif

#ifndef USBD_DEBUG_MID_16
#define USBD_DEBUG_MID_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#endif

#ifndef USBD_DEBUG_LO
#define USBD_DEBUG_LO(id, format)
#endif

#ifndef USBD_DEBUG_LO_1
#define USBD_DEBUG_LO_1(id, format, a1)
#endif

#ifndef USBD_DEBUG_LO_2
#define USBD_DEBUG_LO_2(id, format, a1, a2)
#endif

#ifndef USBD_DEBUG_LO_3
#define USBD_DEBUG_LO_3(id, format, a1, a2, a3)
#endif

#ifndef USBD_DEBUG_LO_4
#define USBD_DEBUG_LO_4(id, format, a1, a2, a3, a4)
#endif

#ifndef USBD_DEBUG_LO_5
#define USBD_DEBUG_LO_5(id, format, a1, a2, a3, a4, a5)
#endif

#ifndef USBD_DEBUG_LO_6
#define USBD_DEBUG_LO_6(id, format, a1, a2, a3, a4, a5, a6)
#endif

#ifndef USBD_DEBUG_LO_7
#define USBD_DEBUG_LO_7(id, format, a1, a2, a3, a4, a5, a6, a7)
#endif

#ifndef USBD_DEBUG_LO_8
#define USBD_DEBUG_LO_8(id, format, a1, a2, a3, a4, a5, a6, a7, a8)
#endif

#ifndef USBD_DEBUG_LO_9
#define USBD_DEBUG_LO_9(id,  format, a1, a2, a3, a4, a5, a6, a7, a8, a9)
#endif

#ifndef USBD_DEBUG_LO_10
#define USBD_DEBUG_LO_10(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA)
#endif

#ifndef USBD_DEBUG_LO_11
#define USBD_DEBUG_LO_11(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB)
#endif

#ifndef USBD_DEBUG_LO_12
#define USBD_DEBUG_LO_12(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC)
#endif

#ifndef USBD_DEBUG_LO_13
#define USBD_DEBUG_LO_13(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD)
#endif

#ifndef USBD_DEBUG_LO_14
#define USBD_DEBUG_LO_14(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE)
#endif

#ifndef USBD_DEBUG_LO_15
#define USBD_DEBUG_LO_15(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF)
#endif

#ifndef USBD_DEBUG_LO_16
#define USBD_DEBUG_LO_16(id, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, aA, aB, aC, aD, aE, aF, aG)
#endif

#define USBD_LOG_DATA_EMERG(      id, data, size, description)
#define USBD_LOG_DATA_EMERG_1(    id, data, size, description, a1)
#define USBD_LOG_DATA_EMERG_2(    id, data, size, description, a1, a2)
#define USBD_LOG_DATA_EMERG_3(    id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_ALERT(      id, data, size, description)
#define USBD_LOG_DATA_ALERT_1(    id, data, size, description, a1)
#define USBD_LOG_DATA_ALERT_2(    id, data, size, description, a1, a2)
#define USBD_LOG_DATA_ALERT_3(    id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_CRIT(       id, data, size, description)
#define USBD_LOG_DATA_CRIT_1(     id, data, size, description, a1)
#define USBD_LOG_DATA_CRIT_2(     id, data, size, description, a1, a2)
#define USBD_LOG_DATA_CRIT_3(     id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_ERROR(      id, data, size, description)
#define USBD_LOG_DATA_ERROR_1(    id, data, size, description, a1)
#define USBD_LOG_DATA_ERROR_2(    id, data, size, description, a1, a2)
#define USBD_LOG_DATA_ERROR_3(    id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_WARN(       id, data, size, description)
#define USBD_LOG_DATA_WARN_1(     id, data, size, description, a1)
#define USBD_LOG_DATA_WARN_2(     id, data, size, description, a1, a2)
#define USBD_LOG_DATA_WARN_3(     id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_NOTICE(     id, data, size, description)
#define USBD_LOG_DATA_NOTICE_1(   id, data, size, description, a1)
#define USBD_LOG_DATA_NOTICE_2(   id, data, size, description, a1, a2)
#define USBD_LOG_DATA_NOTICE_3(   id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_INFO_HI(    id, data, size, description)
#define USBD_LOG_DATA_INFO_HI_1(  id, data, size, description, a1)
#define USBD_LOG_DATA_INFO_HI_2(  id, data, size, description, a1, a2)
#define USBD_LOG_DATA_INFO_HI_3(  id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_INFO_MID(   id, data, size, description)
#define USBD_LOG_DATA_INFO_MID_1( id, data, size, description, a1)
#define USBD_LOG_DATA_INFO_MID_2( id, data, size, description, a1, a2)
#define USBD_LOG_DATA_INFO_MID_3( id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_INFO_LO(    id, data, size, description)
#define USBD_LOG_DATA_INFO_LO_1(  id, data, size, description, a1)
#define USBD_LOG_DATA_INFO_LO_2(  id, data, size, description, a1, a2)
#define USBD_LOG_DATA_INFO_LO_3(  id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_DEBUG_HI(   id, data, size, description)
#define USBD_LOG_DATA_DEBUG_HI_1( id, data, size, description, a1)
#define USBD_LOG_DATA_DEBUG_HI_2( id, data, size, description, a1, a2)
#define USBD_LOG_DATA_DEBUG_HI_3( id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_DEBUG_MID(  id, data, size, description)
#define USBD_LOG_DATA_DEBUG_MID_1(id, data, size, description, a1)
#define USBD_LOG_DATA_DEBUG_MID_2(id, data, size, description, a1, a2)
#define USBD_LOG_DATA_DEBUG_MID_3(id, data, size, description, a1, a2, a3)

#define USBD_LOG_DATA_DEBUG_LO(   id, data, size, description)
#define USBD_LOG_DATA_DEBUG_LO_1( id, data, size, description, a1)
#define USBD_LOG_DATA_DEBUG_LO_2( id, data, size, description, a1, a2)
#define USBD_LOG_DATA_DEBUG_LO_3( id, data, size, description, a1, a2, a3)


#define USBD_COMPARE_DATA_EMERG(      id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_EMERG_1(    id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_EMERG_2(    id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_EMERG_3(    id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_ALERT(      id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_ALERT_1(    id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_ALERT_2(    id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_ALERT_3(    id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_CRIT(       id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_CRIT_1(     id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_CRIT_2(     id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_CRIT_3(     id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_ERROR(      id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_ERROR_1(    id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_ERROR_2(    id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_ERROR_3(    id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_WARN(       id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_WARN_1(     id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_WARN_2(     id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_WARN_3(     id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_NOTICE(     id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_NOTICE_1(   id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_NOTICE_2(   id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_NOTICE_3(   id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_INFO_HI(    id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_INFO_HI_1(  id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_INFO_HI_2(  id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_INFO_HI_3(  id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_INFO_MID(   id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_INFO_MID_1( id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_INFO_MID_2( id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_INFO_MID_3( id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_INFO_LO(    id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_INFO_LO_1(  id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_INFO_LO_2(  id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_INFO_LO_3(  id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_DEBUG_HI(   id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_DEBUG_HI_1( id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_DEBUG_HI_2( id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_DEBUG_HI_3( id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_DEBUG_MID(  id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_DEBUG_MID_1(id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_DEBUG_MID_2(id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_DEBUG_MID_3(id, data1, size1, data2, size2, description, a1, a2, a3)

#define USBD_COMPARE_DATA_DEBUG_LO(   id, data1, size1, data2, size2, description)
#define USBD_COMPARE_DATA_DEBUG_LO_1( id, data1, size1, data2, size2, description, a1)
#define USBD_COMPARE_DATA_DEBUG_LO_2( id, data1, size1, data2, size2, description, a1, a2)
#define USBD_COMPARE_DATA_DEBUG_LO_3( id, data1, size1, data2, size2, description, a1, a2, a3)

#endif

#endif
