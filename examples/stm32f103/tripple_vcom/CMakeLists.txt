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

cmake_minimum_required(VERSION 3.25)

set(TARGET_NAME demoapp)

# -----------------------------------------------------------------------------------------------------------------------------
# basic project definitions
project(${TARGET_NAME} C ASM)
add_executable(${TARGET_NAME})
enable_language(C ASM)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_USE_RELATIVE_PATHS ON)

# -----------------------------------------------------------------------------------------------------------------------------
# below definition turns ON or OFF logging all the information during building process
set(CMAKE_VERBOSE_MAKEFILE ON)

# -----------------------------------------------------------------------------------------------------------------------------
# ------------------------------------- compiler paths ------------------------------------------------------------------------
# -----------------------------------------------------------------------------------------------------------------------------
set(TOOLCHAIN_PREFIX ${COMPILER_PATH}arm-none-eabi-)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE ${TOOLCHAIN_PREFIX}size)
set(CMAKE_OBJDUMP ${TOOLCHAIN_PREFIX}objdump)

# -----------------------------------------------------------------------------------------------------------------------------
# ------------------------------------- files paths and source files ----------------------------------------------------------
# -----------------------------------------------------------------------------------------------------------------------------
# files paths
set(OS_PATH common/os)
set(OS_API_PATH ${OS_PATH}/api)
set(OS_IMP_PATH ${OS_PATH}/imp/FreeRTOS)
set(FREERTOS_PATH common/FreeRTOS/Source)
set(STDLIB_PATH common/STM32F10xFWLib/FWLib/library)
set(FREERTOS_PORT_PATH ${FREERTOS_PATH}/portable/GCC/ARM_CM3)
set(SYSTEM_TOOLS_PATH common/system_tools)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/${VARIANT_DIR}/
    ${STDLIB_PATH}/inc/
    ${FREERTOS_PORT_PATH}
    ${FREERTOS_PATH}/include/
    ${SYSTEM_TOOLS_PATH}
    ${OS_API_PATH}
    ${OS_IMP_PATH}/inc/
)

# project source files
target_sources(${TARGET_NAME} PUBLIC

    # Application ASM source files:
    cortexm3_macro.S

    # Application C source files:
    crt0_STM32x.c
    main.c
    stm32_usart_init.c
    main_usbd_init.c
    iocmd_exe_usart.c
    iocmd_exe_vcom.c
    cmd_line.c
    exceptions.c
    usbd_port_stm32f103_board_settings.c

    # Low level C source files:
    ${SYSTEM_TOOLS_PATH}/stdmem.c ${SYSTEM_TOOLS_PATH}/syscalls.c ${SYSTEM_TOOLS_PATH}/heap_3.c

    # FreeRTOS source files:
    ${FREERTOS_PORT_PATH}/port.c
    ${FREERTOS_PATH}/list.c ${FREERTOS_PATH}/tasks.c ${FREERTOS_PATH}/queue.c

    # ST library:
    ${STDLIB_PATH}/src/stm32f10x_nvic.c ${STDLIB_PATH}/src/stm32f10x_systick.c ${STDLIB_PATH}/src/stm32f10x_rcc.c ${STDLIB_PATH}/src/stm32f10x_flash.c ${STDLIB_PATH}/src/stm32f10x_gpio.c ${STDLIB_PATH}/src/stm32f10x_usart.c ${STDLIB_PATH}/src/stm32f10x_exti.c

    #os library
    ${OS_IMP_PATH}/src/os.c
)

# usbd source files
set(USBD_PATH ../../../usbd)
set(USBD_USE_CDC_VCOM ON)
set(USBD_USE_PORT_STM32_CATEGORY_A ON)
include(${USBD_PATH}/usbd_lib.cmake)
target_link_libraries(${TARGET_NAME} usbd_lib)

# external depedencies - buff and iocmd
set(DEPEDENCIES_PATH ../../depedencies)
set(DEPEDENCY_USE_BUFF ON)
set(DEPEDENCY_USE_IOCMD ON)
set(IOCMD_USE_OUT ON)
set(IOCMD_USE_LOG ON)
set(IOCMD_USE_IN  ON)
set(IOCMD_USE_CMD ON)
include(${DEPEDENCIES_PATH}/depedencies.cmake)
target_link_libraries(${TARGET_NAME} buff_lib iocmd_lib)

# definitions from presets for vendor ID and product ID
target_compile_definitions(${TARGET_NAME} PRIVATE
    USBD_VENDOR_ID=${VENDOR_ID}
    USBD_PRODUCT_ID=${PRODUCT_ID}
)

# -----------------------------------------------------------------------------------------------------------------------------
# ------------------------------------- compilation options -------------------------------------------------------------------
# -----------------------------------------------------------------------------------------------------------------------------

# microcontroller information
set(CPU_PARAMETERS
    -mcpu=cortex-m3
    -mthumb
)
set(OPTIMIZATION -Os)
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/${VARIANT_DIR}/flash.ld)

if(${REMOVE_UNUSED_CODE})
set(REMOVE_UNUSED_CODE__C_FLAGS "-ffunction-sections")
set(REMOVE_UNUSED_CODE__LINKER_FLAGS "-Wl,--gc-sections")
endif()

# common compilation options for all languages
target_compile_options(${TARGET_NAME} PRIVATE
    ${CPU_PARAMETERS}
    ${OPTIMIZATION}
    -fverbose-asm
    -Wall
    -Wstrict-prototypes
)

# C language specific compilation options
set(CMAKE_C_FLAGS
    "-gdwarf-2 -fomit-frame-pointer -std=gnu99 ${REMOVE_UNUSED_CODE__C_FLAGS}"
)
if(${REMOVE_PATHS_FROM_SYMBOLS})
    # loop below is to add compilation flags -f*-prefix-map= for each directory of the code
    get_property(sources_list TARGET ${TARGET_NAME} PROPERTY SOURCES)
    foreach(src_file IN LISTS sources_list)
        get_filename_component(abs_path ${src_file} ABSOLUTE)
        get_filename_component(file_dir ${abs_path} DIRECTORY)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmacro-prefix-map=${file_dir}=. -fdebug-prefix-map=${file_dir}=. -fprofile-prefix-map=${file_dir}=.")
    endforeach()
endif()

# linker options
target_link_options(${TARGET_NAME} PRIVATE
    -T${LINKER_SCRIPT}
    ${CPU_PARAMETERS}
    -nostartfiles
    -Wl,-Map=${TARGET_NAME}.map
    -Wl,--print-memory-usage
    ${REMOVE_UNUSED_CODE__LINKER_FLAGS}
)

# generate output files - hex and bin
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${TARGET_NAME}>
    COMMAND ${CMAKE_OBJCOPY} -j .isr_vector -j .text -j .ARM.exidx -j .data -O ihex $<TARGET_FILE:${TARGET_NAME}> ${TARGET_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -j .isr_vector -j .text -j .ARM.exidx -j .data -O binary $<TARGET_FILE:${TARGET_NAME}> ${TARGET_NAME}.bin
)

# -----------------------------------------------------------------------------------------------------------------------------
# ------------------------------------- add extra target for programming using DFU --------------------------------------------
# -----------------------------------------------------------------------------------------------------------------------------
add_custom_target(dfu
    sudo dfu-util --device ,0483:0007 -a 0 -R -D ${TARGET_NAME}.bin
)
