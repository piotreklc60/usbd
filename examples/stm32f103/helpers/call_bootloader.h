/** ---------------------------------------- LICENSE SECTION -------------------------------------------------------------------
 *
 * Copyright (c) 2025 Piotr Wojtowicz
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

#ifndef CALL_BOOTLOADER_H_
#define CALL_BOOTLOADER_H_

#include "stm32f1xx_hal.h"

#define BOOTLOADER_ACTIVATION_KEY         0x169D

#define CALL_BOOTLOADER() \
{ \
   /* Enable write access to Backup registers */               \
   RCC->APB1ENR   |= RCC_APB1ENR_BKPEN | RCC_APB1ENR_PWREN;    \
   PWR->CR        |= PWR_CR_DBP;                               \
                                                               \
   /* Trigger bootloader */                                    \
   BKP->DR1 = BOOTLOADER_ACTIVATION_KEY;                       \
                                                               \
   NVIC_SystemReset();                                         \
}

#endif
