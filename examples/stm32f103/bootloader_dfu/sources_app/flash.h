/** ---------------------------------------- LICENSE SECTION -------------------------------------------------------------------
 *
 * This file is modified version of STMicroelectronics software.
 *
 * Copyright (c) 2025 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
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

#ifndef FLASH_H_
#define FLASH_H_

#include <stdint.h>

typedef enum
{
   PROGRAM_FLASH_BUSY = 1,
   PROGRAM_FLASH_ERROR_PG,
   PROGRAM_FLASH_ERROR_WRP,
   PROGRAM_FLASH_COMPLETE,
   PROGRAM_FLASH_TIMEOUT
}PROGRAM_FLASH_Status_ET;

void Program_Flash_Unlock(void);

void Program_Flash_Lock(void);

__attribute__ ((long_call))
__attribute__ ((section(".fcpy_section")))
PROGRAM_FLASH_Status_ET Program_Flash_Get_Status(void);

__attribute__ ((long_call))
__attribute__ ((section(".fcpy_section")))
uint32_t Program_Flash_Erase_Page(uint32_t page_address);

__attribute__ ((long_call))
__attribute__ ((section(".fcpy_section")))
uint32_t Program_Flash_Write_Page(uint32_t page_address, const uint8_t *chunk, uint32_t chunk_len);

#endif /*FLASH_H_*/
