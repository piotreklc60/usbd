# ---------------------------------------- LICENSE SECTION -------------------------------------------------------------------
#
# Copyright (c) 2024 Piotr Wojtowicz
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# -----------------------------------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.22)

add_library(usbd_lib INTERFACE)

# -----------------------------------------------------------------------------------------------------------------------------
# ------------------------------------- files paths ---------------------------------------------------------------------------
# -----------------------------------------------------------------------------------------------------------------------------
# files paths
set(USBD_PATH ${CMAKE_CURRENT_LIST_DIR})
set(USBD_CORE_USBD_PATH ${USBD_PATH}/core/core/usbd)
set(USBD_CORE_ADD_ONS_PATH ${USBD_PATH}/core/core/add_ons)
set(USBD_CORE_ADD_ONS_CDP_PATH ${USBD_CORE_ADD_ONS_PATH}/usb_cdp)
if(DEFINED USBD_IOTP_LIGHT_ONLY AND USBD_IOTP_LIGHT_ONLY MATCHES ON)
set(USBD_CORE_IOTP_PATH ${USBD_CORE_ADD_ONS_PATH}/usbd_iotp/light)
else()
set(USBD_CORE_IOTP_PATH ${USBD_CORE_ADD_ONS_PATH}/usbd_iotp/full)
endif()
set(USBD_CORE_IOTP_BUFF_PATH ${USBD_CORE_ADD_ONS_PATH}/usbd_iotp/buff)
set(USBD_CORE_USBD_ELEMENTS_PATH ${USBD_PATH}/core/core/usbd_elements)
set(USBD_CORE_USBD_DEV_PATH ${USBD_CORE_USBD_ELEMENTS_PATH}/usbd_dev)
set(USBD_CORE_USBD_REQUEST_PATH ${USBD_CORE_USBD_ELEMENTS_PATH}/usbd_request)
set(USBD_CORE_USBD_IO_PATH ${USBD_CORE_USBD_ELEMENTS_PATH}/usbd_io)
set(USBD_CORE_USBD_EVENT_PATH ${USBD_CORE_USBD_ELEMENTS_PATH}/usbd_event)
set(USBD_CORE_USBDC_PATH ${USBD_CORE_USBD_ELEMENTS_PATH}/usbdc)

set(MASS_STORAGE_PATH ${USBD_PATH}/classes/mass_storage)
set(CDC_VCOM_PATH ${USBD_PATH}/classes/cdc_vcom)
set(HID_PATH ${USBD_PATH}/classes/hid)
set(DFU_PATH ${USBD_PATH}/classes/dfu)

set(USBD_PORT_PATH ${USBD_PATH}/core/port)
set(USBD_PORT_TEST_PATH ${USBD_PORT_PATH}/test)
set(USBD_PORT_STM32_CATEGORY_A_PATH ${USBD_PORT_PATH}/stm32_cat_a)

# -----------------------------------------------------------------------------------------------------------------------------
# ------------------------------------- USBD CORE source files - by default ALL ADDED, ----------------------------------------
# ------------------------------------- can be removed by defining USBD_DO_NOT_USE_<module name> ------------------------------
# -----------------------------------------------------------------------------------------------------------------------------

target_include_directories(usbd_lib INTERFACE
    ${USBD_CORE_USBD_PATH}/api/
    ${USBD_CORE_USBD_PATH}/imp/inc/
    ${USBD_CORE_USBD_DEV_PATH}/api/
    ${USBD_CORE_USBD_DEV_PATH}/imp/inc/
    ${USBD_CORE_USBD_REQUEST_PATH}/api/
    ${USBD_CORE_USBD_REQUEST_PATH}/imp/inc/
    ${USBD_CORE_USBD_IO_PATH}/api/
    ${USBD_CORE_USBD_IO_PATH}/imp/inc/
    ${USBD_CORE_USBD_EVENT_PATH}/api/
    ${USBD_CORE_USBD_EVENT_PATH}/imp/inc/
    ${USBD_CORE_USBDC_PATH}/api/
    ${USBD_CORE_USBDC_PATH}/imp/inc/
    ${USBD_CORE_ADD_ONS_CDP_PATH}/api/
    ${USBD_CORE_IOTP_PATH}/api/
    ${USBD_CORE_IOTP_PATH}/imp/inc/
    ${USBD_CORE_IOTP_BUFF_PATH}/api/
    ${USBD_CORE_IOTP_BUFF_PATH}/imp/inc/
)

target_sources(usbd_lib INTERFACE
    ${USBD_CORE_USBD_PATH}/imp/src/usbd.c
)

if(DEFINED USBD_DO_NOT_USE_CDP AND USBD_DO_NOT_USE_CDP MATCHES ON)
    target_compile_definitions(usbd_lib INTERFACE
        USB_CDP_H_
    )
else()
    target_sources(usbd_lib INTERFACE
        ${USBD_CORE_ADD_ONS_CDP_PATH}/imp/src/usb_cdp.c
    )
endif()

if(DEFINED USBD_DO_NOT_USE_USBDC AND USBD_DO_NOT_USE_USBDC MATCHES ON)
    target_compile_definitions(usbd_lib INTERFACE
        USBDC_H_
    )
else()
    target_sources(usbd_lib INTERFACE
        ${USBD_CORE_USBDC_PATH}/imp/src/usbdc.c
    )
endif()

if(DEFINED USBD_DO_NOT_USE_DEV AND USBD_DO_NOT_USE_DEV MATCHES ON)
    target_compile_definitions(usbd_lib INTERFACE
        USBD_DEV_TYPES_H_
        USBD_DEV_H_
    )
else()
    target_sources(usbd_lib INTERFACE
        ${USBD_CORE_USBD_DEV_PATH}/imp/src/usbd_dev.c
    )
endif()

if(DEFINED USBD_DO_NOT_USE_EVENT AND USBD_DO_NOT_USE_EVENT MATCHES ON)
    target_compile_definitions(usbd_lib INTERFACE
        USBD_EVENT_TYPES_H_
        USBD_EVENT_H_
    )
else()
    target_sources(usbd_lib INTERFACE
        ${USBD_CORE_USBD_EVENT_PATH}/imp/src/usbd_event.c
    )
endif()

if(DEFINED USBD_DO_NOT_USE_IO AND USBD_DO_NOT_USE_IO MATCHES ON)
    target_compile_definitions(usbd_lib INTERFACE
        USBD_IO_TYPES_H_
        USBD_IO_H_
    )
else()
    target_sources(usbd_lib INTERFACE
        ${USBD_CORE_USBD_IO_PATH}/imp/src/usbd_io.c
    )
endif()

if(DEFINED USBD_DO_NOT_USE_IOTP AND USBD_DO_NOT_USE_IOTP MATCHES ON)
    target_compile_definitions(usbd_lib INTERFACE
        USBD_IOTP_TYPES_H_
        USBD_IOTP_H_
    )
else()
    if(DEFINED USBD_IOTP_LIGHT_ONLY AND USBD_IOTP_LIGHT_ONLY MATCHES ON)
        target_sources(usbd_lib INTERFACE
            ${USBD_CORE_IOTP_PATH}/imp/src/usbd_iotp.c
        )
    else()
        target_sources(usbd_lib INTERFACE
            ${USBD_CORE_IOTP_PATH}/imp/src/usbd_iotp.c
            ${USBD_CORE_IOTP_PATH}/imp/src/usbd_iotp_finite_in.c
            ${USBD_CORE_IOTP_PATH}/imp/src/usbd_iotp_finite_out.c
            ${USBD_CORE_IOTP_PATH}/imp/src/usbd_iotp_ring_infinite.c
        )
    endif()
endif()

if(DEFINED USBD_DO_NOT_USE_REQUEST AND USBD_DO_NOT_USE_REQUEST MATCHES ON)
    target_compile_definitions(usbd_lib INTERFACE
        USBD_REQUEST_TYPES_H_
        USBD_REQUEST_H_
    )
else()
    target_sources(usbd_lib INTERFACE
        ${USBD_CORE_USBD_REQUEST_PATH}/imp/src/usbd_request.c
    )
endif()

# -----------------------------------------------------------------------------------------------------------------------------
# ------------------------------------- USBD INTERFACE source files - by default NOTHING ADDED, -------------------------------
# ------------------------------------- can be added by defining USBD_USE_<module name> ---------------------------------------
# -----------------------------------------------------------------------------------------------------------------------------

if(DEFINED USBD_USE_MASS_STORAGE AND USBD_USE_MASS_STORAGE MATCHES ON)
    target_sources(usbd_lib INTERFACE
        ${MASS_STORAGE_PATH}/imp/src/mass_storage.c
    )
    target_include_directories(usbd_lib INTERFACE
        ${MASS_STORAGE_PATH}/api/
        ${MASS_STORAGE_PATH}/imp/inc/
    )
    target_compile_definitions(usbd_lib INTERFACE
        USBD_MASS_STORAGE_PRESENT
    )
endif()

if(DEFINED USBD_USE_CDC_VCOM AND USBD_USE_CDC_VCOM MATCHES ON)
    target_sources(usbd_lib INTERFACE
        ${CDC_VCOM_PATH}/imp/src/cdc_vcom.c
    )
    target_include_directories(usbd_lib INTERFACE
        ${CDC_VCOM_PATH}/api/
        ${CDC_VCOM_PATH}/imp/inc/
    )
    target_compile_definitions(usbd_lib INTERFACE
        USBD_CDC_VCOM_PRESENT
    )
endif()

if(DEFINED USBD_USE_DFU AND USBD_USE_DFU MATCHES ON)
    target_sources(usbd_lib INTERFACE
        ${DFU_PATH}/imp/src/dfu.c
    )
    target_include_directories(usbd_lib INTERFACE
        ${DFU_PATH}/api/
        ${DFU_PATH}/imp/inc/
    )
    target_compile_definitions(usbd_lib INTERFACE
        USBD_DFU_PRESENT
    )
endif()

if(DEFINED USBD_USE_HID AND USBD_USE_HID MATCHES ON)
    target_sources(usbd_lib INTERFACE
        ${HID_PATH}/imp/src/hid.c
    )
    target_include_directories(usbd_lib INTERFACE
        ${HID_PATH}/api/
        ${HID_PATH}/imp/inc/
    )
    target_compile_definitions(usbd_lib INTERFACE
        USBD_HID_PRESENT
    )
endif()

# -----------------------------------------------------------------------------------------------------------------------------
# ------------------------------------- USBD PORT source files - by default NOTHING ADDED, ------------------------------------
# ------------------------------------- can be added by defining USBD_USE_<module name> ---------------------------------------
# -----------------------------------------------------------------------------------------------------------------------------

if(DEFINED USBD_USE_PORT_TEST AND USBD_USE_PORT_TEST MATCHES ON)
    target_sources(usbd_lib INTERFACE
        ${USBD_PORT_TEST_PATH}/imp/src/port_test.c
    )
    target_include_directories(usbd_lib INTERFACE
        ${USBD_PORT_TEST_PATH}/api/
        ${USBD_PORT_TEST_PATH}/imp/inc/
    )
endif()

if(DEFINED USBD_USE_PORT_STM32_CATEGORY_A AND USBD_USE_PORT_STM32_CATEGORY_A MATCHES ON)
    target_sources(usbd_lib INTERFACE
        ${USBD_PORT_STM32_CATEGORY_A_PATH}/imp/src/usbd_port_stm32_cat_a.c
    )
    target_include_directories(usbd_lib INTERFACE
        ${USBD_PORT_STM32_CATEGORY_A_PATH}/api/
        ${USBD_PORT_STM32_CATEGORY_A_PATH}/imp/inc/
    )
endif()
