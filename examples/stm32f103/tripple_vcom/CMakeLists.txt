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

#
# This file is a modified version of generated CMakeLists.txt
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME tripple_vcom_stm32f103${VARIANT})

# Include toolchain file
include("stm32cubemx_generated_stm32f103${VARIANT}/cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(stm32cubemx_generated_stm32f103${VARIANT}/cmake/stm32cubemx)
add_subdirectory(sources_app)

# usbd source files
set(USBD_PATH ../../../usbd)
set(USBD_USE_CDC_VCOM ON)
set(USBD_USE_DFU ON)
set(USBD_USE_PORT_STM32_CATEGORY_A ON)
include(${USBD_PATH}/usbd_lib.cmake)
target_link_libraries(${CMAKE_PROJECT_NAME} usbd_lib)

# external depedencies - buff and iocmd
set(DEPEDENCIES_PATH ../../depedencies)
set(DEPEDENCY_USE_BUFF ON)
set(DEPEDENCY_USE_IOCMD ON)
set(IOCMD_USE_OUT ON)
set(IOCMD_USE_IN  ON)
if(${VARIANT} MATCHES "c8t6_bluepill")
set(IOCMD_USE_LOG OFF)
set(IOCMD_USE_CMD OFF)
else()
set(IOCMD_USE_LOG ON)
set(IOCMD_USE_CMD ON)
endif()
include(${DEPEDENCIES_PATH}/depedencies.cmake)
target_link_libraries(${CMAKE_PROJECT_NAME} buff_lib iocmd_lib)

# definitions from presets for vendor ID and product ID
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    USBD_VENDOR_ID=${VENDOR_ID}
    USBD_PRODUCT_ID=${PRODUCT_ID}
)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

if(${REMOVE_PATHS_FROM_SYMBOLS})
    # loop below is to add compilation flags -f*-prefix-map= for each directory of the code
    get_property(sources_list TARGET ${CMAKE_PROJECT_NAME} PROPERTY SOURCES)
    foreach(src_file IN LISTS sources_list)
        get_filename_component(abs_path ${src_file} ABSOLUTE)
        get_filename_component(file_dir ${abs_path} DIRECTORY)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmacro-prefix-map=${file_dir}=. -fdebug-prefix-map=${file_dir}=. -fprofile-prefix-map=${file_dir}=.")
    endforeach()
endif()

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

# Convert elf output to hex format
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex
)

# Convert elf output to binary format
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
)
