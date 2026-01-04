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

#ifndef OSAL_CFG_H_
#define OSAL_CFG_H_


#include "std_libs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

//#define OSAL_USE_IOCMD
#define OSAL_IDENTIFY_CONTEXT_BY_PATH_SUPPORTED    OSAL_FEATURE_NOT_PRESENT
#define OSAL_BASIC_PARAMS_CHECK(expresion)         (1)

#define OSAL_NUM_THREADS       10
#define OSAL_NUM_INTERRUPTS    240
#define OSAL_NUM_PRE_ALOCATED_BRANCHES             17
#define OSAL_NUM_PRE_ALOCATED_BRANCHES_DESC_BUF    89


#endif
