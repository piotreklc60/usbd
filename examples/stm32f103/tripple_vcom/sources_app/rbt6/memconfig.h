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

#ifndef MEMCONFIG_H_
#define MEMCONFIG_H_

#define INTERNAL_RAM_START       SRAM_BASE
#define INTERNAL_RAM_SIZE        (20 * 1024)

#define INTERNAL_FLASH_START     0x08000000
#define INTERNAL_FLASH_SIZE_KB   128

#define BOOTLOADER_START         INTERNAL_FLASH_START                                     /* 0x08000000 */
#define BOOTLOADER_END           (BOOTLOADER_START + (1024 * BOOTLOADER_SIZE_KB) - 1)     /* 0x080027FF */
#define BOOTLOADER_SIZE_KB       10                                                       /* 10 kB */
#define APP_START                (BOOTLOADER_START + (1024 * BOOTLOADER_SIZE_KB))         /* 0x08002800 */
#define APP_END                  (APP_START        + (1024 * APP_SIZE_KB)        - 1)     /* 0x0801FFFF */
#define APP_SIZE_KB              (INTERNAL_FLASH_SIZE_KB - BOOTLOADER_SIZE_KB)            /* 118 kB */

#define IOCMD_LOG_MAIN_BUF_SIZE  2900

#define HEAP_SIZE                (6 * 1024)

#define STACK_SIZE               (1024)

#endif

