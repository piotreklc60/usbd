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

#ifndef IOCMD_CFG_H_
#define IOCMD_CFG_H_

#include "cfg.h"

#define IOCMD_USE_OUT
#define IOCMD_USE_LOG
#define IOCMD_USE_IN
#define IOCMD_USE_CMD

#define IOCMD_SUPPORT_LOGS_POSPONING      IOCMD_FEATURE_ENABLED

#define IOCMD_LOG_MAIN_BUF_SIZE                 1000000000
#define IOCMD_LOG_QUIET_BUF_SIZE                40000
#define IOCMD_PROGRAM_MEMORY_END                0xFFFFFFFFFFFFFFFF
#define IOCMD_IS_PTR_IN_PROGRAM_MEMORY(_ptr)    false



#define IOCMD_LOGS_TREE \
/*                   name                              compilation switch level   compilation switch entr     default level log           default silent level log   default entrances state     description */ \
IOCMD_LOG_ADD_MODULE(MAIN_APP,                         IOCMD_LOG_LEVEL_DEBUG_LO , IOCMD_ENTRANCE_ENABLED    , IOCMD_LOG_LEVEL_DEBUG_LO  , IOCMD_LOG_LEVEL_DEBUG_LO , IOCMD_ENTRANCE_ENABLED   , "logs printed from \"main\" function") \
IOCMD_LOG_ADD_GROUP( MAIN_APP_TEST,                    IOCMD_LOG_LEVEL_DEBUG_LO , IOCMD_ENTRANCE_ENABLED    , IOCMD_LOG_LEVEL_DEBUG_LO  , IOCMD_LOG_LEVEL_DEBUG_LO , IOCMD_ENTRANCE_ENABLED    , "logs printed from \"main\" function") \
IOCMD_LOG_ADD_ITEM(  MAIN_APP_TEST_INTRO,              IOCMD_LOG_LEVEL_DEBUG_LO , IOCMD_ENTRANCE_ENABLED    , IOCMD_LOG_LEVEL_DEBUG_LO  , IOCMD_LOG_LEVEL_DEBUG_LO , IOCMD_ENTRANCE_ENABLED   , "logs printed from \"main\" function") \
IOCMD_LOG_ADD_ITEM(  MAIN_APP_TEST_ERROR,              IOCMD_LOG_LEVEL_DEBUG_LO , IOCMD_ENTRANCE_ENABLED    , IOCMD_LOG_LEVEL_DEBUG_LO  , IOCMD_LOG_LEVEL_DEBUG_LO , IOCMD_ENTRANCE_ENABLED   , "logs printed from \"main\" function") \
\
IOCMD_LOG_ADD_MODULE(USBD_DBG,                         USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - debug module") \
IOCMD_LOG_ADD_GROUP( USB_DBG_CDP,                     USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - configuration device parser") \
IOCMD_LOG_ADD_GROUP( USBD_DBG_INIT,                    USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - lib init") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_DEV_INIT,                USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - device init") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_DEV_ONOFF,               USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - device on/off switching") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_DEV_ONOFF_SOF_RECEIVED,  USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - device on/off switching - SOF_RECEIVED function") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_DEV_STATE,               USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - device state") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_DEV_INVALID_PARAMS,      USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - device invalid parameters reporting") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_IO_ONOFF,                USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - io on/off switching") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_IO_STATE,                USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - io state") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_IO_PROCESSING,           USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - io transfer processing") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_IO_INVALID_PARAMS,       USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - io invalid parameters reporting") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_IOTPEV_ONOFF,            USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - iotp event-type on/off switching") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_IOTPEV_STATE,            USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - iotp event-type state") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_IOTPEV_EVENT,            USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - iotp event-type event for SOF") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_IOTPEV_PROCESSING,       USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - iotp event-type transfer processing") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_IOTPEV_INVALID_PARAMS,   USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - iotp event-type invalid parameters reporting") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_IOTPBF_ONOFF,            USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - iotp buff-type on/off switching") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_IOTPBF_STATE,            USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - iotp buff-type state") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_IOTPBF_EVENT,            USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - iotp buff-type event for SOF") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_IOTPBF_PROCESSING,       USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - iotp buff-type transfer processing") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_IOTPBF_INVALID_PARAMS,   USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - iotp buff-type invalid parameters reporting") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_EVENT_INIT,              USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - event init") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_EVENT_STATE,             USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - event state") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_EVENT_PROCESSING,        USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - event processing actions") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_EVENT_INVALID_PARAMS,    USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - event invalid parameters reporting") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_REQ_INIT,                USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - request init") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_REQ_PROCESSING,          USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - request processing") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_REQ_INVALID_PARAMS,      USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - request invalid parameters reporting") \
IOCMD_LOG_ADD_GROUP( USBD_DBG_USBDC,                   USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - usbd configuration") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_PORT_INIT,               USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - port init") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_PORT_DEV,                USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - port device") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_PORT_IO,                 USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - port io") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_PORT_IO_ONOFF,           USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - port io on/off switching") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_PORT_EVENT,              USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - port event") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_PORT_REQ,                USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - port request") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_PORT_INVALID_PARAMS,     USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - port invalid parameters reporting") \
IOCMD_LOG_ADD_ITEM(  USBD_DBG_PORT_IRQ,                USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - port interuptions") \
IOCMD_LOG_ADD_ITEM(  CDC_VCOM_INIT,                    USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - Virtual COM Library init") \
IOCMD_LOG_ADD_ITEM(  CDC_VCOM_ONOFF,                   USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - Virtual COM Library io on/off switching") \
IOCMD_LOG_ADD_ITEM(  CDC_VCOM_EVENT,                   USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - Virtual COM Library event") \
IOCMD_LOG_ADD_ITEM(  CDC_VCOM_REQ,                     USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - Virtual COM Library request") \
IOCMD_LOG_ADD_ITEM(  CDC_VCOM_INVALID_PARAMS,          USBD_LOG_COMP            , USBD_ENTR_COMP            , USBD_LOG_STATE            , USBD_LOG_SILENT_STATE    , USBD_ENTR_STATE          , "USBD - Virtual COM Library invalid parameters reporting") \

// IOCMD_LOG_LEVEL_EMERG

#define USBD_LOG_COMP           IOCMD_LOG_LEVEL_DEBUG_LO
#define USBD_LOG_STATE          IOCMD_LOG_LEVEL_DEBUG_LO
#define USBD_LOG_SILENT_STATE   IOCMD_LOG_LEVEL_DEBUG_LO

#define USBD_ENTR_COMP          IOCMD_ENTRANCE_ENABLED
#define USBD_ENTR_STATE         IOCMD_ENTRANCE_ENABLED

#endif
