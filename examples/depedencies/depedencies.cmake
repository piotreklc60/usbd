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

include(FetchContent)

# -----------------------------------------------------------------------------------------------------------------------------
# ------------------------------------- depedencies declaration ---------------------------------------------------------------
# -----------------------------------------------------------------------------------------------------------------------------

if(DEFINED DEPEDENCY_USE_BUFF)
    #Add buff library
    FetchContent_Declare(buff_repository
        GIT_REPOSITORY      https://github.com/piotreklc60/buff.git
        GIT_TAG             master
        SOURCE_DIR          ${CMAKE_CURRENT_LIST_DIR}/buff
        UPDATE_DISCONNECTED ON
    )
endif()

if(DEFINED DEPEDENCY_USE_IOCMD)
    #Add iocmd library
    FetchContent_Declare(iocmd_repository
        GIT_REPOSITORY      https://github.com/piotreklc60/iocmd.git
        GIT_TAG             main
        SOURCE_DIR          ${CMAKE_CURRENT_LIST_DIR}/iocmd
        UPDATE_DISCONNECTED ON
    )
endif()

if(DEFINED DEPEDENCY_USE_OSAL)
    #Add osal library
    FetchContent_Declare(osal_repository
        GIT_REPOSITORY      https://github.com/piotreklc60/osal.git
        GIT_TAG             main
        SOURCE_DIR          ${CMAKE_CURRENT_LIST_DIR}/osal
        UPDATE_DISCONNECTED ON
    )
endif()

# -----------------------------------------------------------------------------------------------------------------------------
# ------------------------------------- depedencies fetching ------------------------------------------------------------------
# -----------------------------------------------------------------------------------------------------------------------------

if(DEFINED DEPEDENCY_USE_BUFF)
    FetchContent_GetProperties(buff_repository)
    if(NOT buff_repository_POPULATED)
        FetchContent_MakeAvailable(buff_repository)
    endif()
endif()

if(DEFINED DEPEDENCY_USE_IOCMD)
    FetchContent_GetProperties(iocmd_repository)
    if(NOT iocmd_repository_POPULATED)
        FetchContent_MakeAvailable(iocmd_repository)
    endif()
endif()

if(DEFINED DEPEDENCY_USE_OSAL)
    FetchContent_GetProperties(osal_repository)
    if(NOT osal_repository_POPULATED)
        FetchContent_MakeAvailable(osal_repository)
    endif()
endif()
