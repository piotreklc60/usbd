# ---------------------------------------- LICENSE SECTION -------------------------------------------------------------------
#
# Copyright (c) 2025 Piotr Wojtowicz
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

set(COMMON_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../common)
set(OS_PATH ${COMMON_PATH}/os)
set(OS_API_PATH ${OS_PATH}/api)
set(OS_IMP_PATH ${OS_PATH}/imp/FreeRTOS)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/${VARIANT}/
    ${COMMON_PATH}
    ${OS_API_PATH}
    ${OS_IMP_PATH}/inc/
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    main_app.c
    main_usbd_init.c
    ${COMMON_PATH}/stm32_usart_init.c
    ${COMMON_PATH}/iocmd_exe_usart.c
    ${COMMON_PATH}/iocmd_exe_vcom.c
    cmd_line.c
    exceptions.c
    usbd_port_stm32_cat_a_board_settings.c

    #os library
    ${OS_IMP_PATH}/src/os.c
)
