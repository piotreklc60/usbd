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

#ifndef USBD_PORT_STM32_CAT_A_H_
#define USBD_PORT_STM32_CAT_A_H_

#ifndef USBD_H_
#include "usbd.h"
#endif

#define USBD_PORT_STM32_CAT_A               USBD_Port_STM32_CAT_A_Get_Port()

#ifndef USBD_PORT_STM32_CAT_A_EP0_MPS
#define USBD_PORT_STM32_CAT_A_EP0_MPS       8
#endif

#define PORT_STM32_CAT_A_DMA_NO_DMA         0
#define PORT_STM32_CAT_A_DMA_HALF_DMA       1

#ifndef USBD_PORT_STM32_CAT_A_DMA_TYPE
#if(USBD_MAX_NUM_ENDPOINTS > 1)
#define USBD_PORT_STM32_CAT_A_DMA_TYPE      PORT_STM32_CAT_A_DMA_HALF_DMA
#else
#define USBD_PORT_STM32_CAT_A_DMA_TYPE      PORT_STM32_CAT_A_DMA_NO_DMA
#endif
#endif

/**
 * @brief returns pointer to port parameters. This pointer is neded to connect USBD structure to HW and enable it.
 *
 * @return pointer to port params.
 */
const USBD_DEV_Port_Handler_XT *USBD_Port_STM32_CAT_A_Get_Port(void);

/**
 * @brief Prints registers and buffers memory data. Useful for debugging.
 *
 * @param desc test printed together with data
 * @param line number of the line from which it is printed.
 */
void USBD_Port_STM32_CAT_A_Print_HW_Dump(const char *desc, uint16_t line);

#endif
