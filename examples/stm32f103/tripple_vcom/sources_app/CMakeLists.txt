cmake_minimum_required(VERSION 3.22)

set(OS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/os)
set(OS_API_PATH ${OS_PATH}/api)
set(OS_IMP_PATH ${OS_PATH}/imp/FreeRTOS)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/${VARIANT}/
    ${OS_API_PATH}
    ${OS_IMP_PATH}/inc/
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    main_app.c
    stm32_usart_init.c
    main_usbd_init.c
    iocmd_exe_usart.c
    iocmd_exe_vcom.c
    cmd_line.c
    exceptions.c
    usbd_port_stm32f103_board_settings.c

    #os library
    ${OS_IMP_PATH}/src/os.c
)
