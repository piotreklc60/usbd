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

#include "flash.h"
#include "stm32f1xx_hal.h"

#define PROGRAM_FLASH_KEY1               ((uint32_t)0x45670123)
#define PROGRAM_FLASH_KEY2               ((uint32_t)0xCDEF89AB)
#define PROGRAM_FLASH_CR_LOCK            ((uint32_t)0x00000080)


/* Flash Access Control Register bits */
#define PROGRAM_FLASH_ACR_PRFTBS_Mask          ((uint32_t)0x00000020)

/* Flash Control Register bits */
#define PROGRAM_FLASH_CR_PG_Set                ((uint32_t)0x00000001)
#define PROGRAM_FLASH_CR_PG_Reset              ((uint32_t)0x00001FFE)

#define PROGRAM_FLASH_CR_PER_Set               ((uint32_t)0x00000002)
#define PROGRAM_FLASH_CR_PER_Reset             ((uint32_t)0x00001FFD)

#define PROGRAM_FLASH_CR_MER_Set               ((uint32_t)0x00000004)
#define PROGRAM_FLASH_CR_MER_Reset             ((uint32_t)0x00001FFB)

#define PROGRAM_FLASH_CR_OPTPG_Set             ((uint32_t)0x00000010)
#define PROGRAM_FLASH_CR_OPTPG_Reset           ((uint32_t)0x00001FEF)

#define PROGRAM_FLASH_CR_OPTER_Set             ((uint32_t)0x00000020)
#define PROGRAM_FLASH_CR_OPTER_Reset           ((uint32_t)0x00001FDF)

#define PROGRAM_FLASH_CR_STRT_Set              ((uint32_t)0x00000040)

#define PROGRAM_FLASH_CR_LOCK_Set              ((uint32_t)0x00000080)

/* FLASH Mask */
#define PROGRAM_FLASH_RDPRT_Mask               ((uint32_t)0x00000002)
#define PROGRAM_FLASH_WRP0_Mask                ((uint32_t)0x000000FF)
#define PROGRAM_FLASH_WRP1_Mask                ((uint32_t)0x0000FF00)
#define PROGRAM_FLASH_WRP2_Mask                ((uint32_t)0x00FF0000)
#define PROGRAM_FLASH_WRP3_Mask                ((uint32_t)0xFF000000)

/* FLASH Keys */
#define PROGRAM_FLASH_RDP_Key                  ((u16)0x00A5)

/* Delay definition */
#define PROGRAM_FLASH_EraseTimeout             ((uint32_t)0x00000FFF)
#define PROGRAM_FLASH_ProgramTimeout           ((uint32_t)0x0000000F)

#define PROGRAM_FLASH_FLAG_BSY           ((uint32_t)0x00000001)  /* FLASH Busy flag */
#define PROGRAM_FLASH_FLAG_EOP           ((uint32_t)0x00000020)  /* FLASH End of Operation flag */
#define PROGRAM_FLASH_FLAG_PGERR         ((uint32_t)0x00000004)  /* FLASH Program error flag */
#define PROGRAM_FLASH_FLAG_WRPRTERR      ((uint32_t)0x00000010)  /* FLASH Write protected error flag */
#define PROGRAM_FLASH_FLAG_OPTERR        ((uint32_t)0x00000001)  /* FLASH Option Byte error flag */



void Program_Flash_Unlock(void)
{
   FLASH->KEYR = PROGRAM_FLASH_KEY1;
   FLASH->KEYR = PROGRAM_FLASH_KEY2;
}

void Program_Flash_Lock(void)
{
   // set FLASH LOCK bit to Lock the memory
   FLASH->CR |= FLASH_CR_LOCK;
}

/*******************************************************************************
* Function Name  : FLASH_GetStatus
* Description    : Returns the FLASH Status.
* Input          : None
* Output         : None
* Return         : FLASH Status: The returned value can be: PROGRAM_FLASH_BUSY,
*                  PROGRAM_FLASH_ERROR_PG or PROGRAM_FLASH_ERROR_WRP or PROGRAM_FLASH_COMPLETE
*******************************************************************************/
__attribute__ ((long_call))
__attribute__ ((section(".fcpy_section")))
PROGRAM_FLASH_Status_ET Program_Flash_Get_Status(void)
{
    PROGRAM_FLASH_Status_ET flashstatus;

    if ((FLASH->SR & PROGRAM_FLASH_FLAG_BSY) == PROGRAM_FLASH_FLAG_BSY)
    {
      flashstatus = PROGRAM_FLASH_BUSY;
    }
    else
    {
      if (FLASH->SR & PROGRAM_FLASH_FLAG_PGERR)
      {
        flashstatus = PROGRAM_FLASH_ERROR_PG;
      }
      else
      {
        if (FLASH->SR & PROGRAM_FLASH_FLAG_WRPRTERR)
        {
          flashstatus = PROGRAM_FLASH_ERROR_WRP;
        }
        else
        {
          flashstatus = PROGRAM_FLASH_COMPLETE;
        }
      }
    }
    /* Return the Flash Status */
    return flashstatus;
}

__attribute__ ((long_call))
__attribute__ ((section(".fcpy_section")))
void Program_Flash_Mcu_Busy_Delay(uint32_t delay)
{
   for ( ; delay; delay--)
   {
      __asm volatile
         (
            "   nop                  \n"
            "   nop                  \n"
            "   nop                  \n"
            "   nop                  \n"
            "   nop                  \n"
         );
   }
}

/*******************************************************************************
* Function Name  : FLASH_WaitForLastOperation
* Description    : Waits for a Flash operation to complete or a TIMEOUT to occur.
* Input          : - Timeout: FLASH progamming Timeout
* Output         : None
* Return         : FLASH Status: The returned value can be: PROGRAM_FLASH_BUSY,
*                  PROGRAM_FLASH_ERROR_PG or PROGRAM_FLASH_ERROR_WRP or PROGRAM_FLASH_COMPLETE or
*                  PROGRAM_FLASH_TIMEOUT.
*******************************************************************************/
__attribute__ ((long_call))
__attribute__ ((section(".fcpy_section")))
PROGRAM_FLASH_Status_ET Program_Flash_Wait_For_Last_Operation(uint32_t Timeout)
{
    PROGRAM_FLASH_Status_ET status;

    /* Check for the Flash Status */
    status = Program_Flash_Get_Status();

    /* Wait for a Flash operation to complete or a TIMEOUT to occur */
    while ((status == PROGRAM_FLASH_BUSY) && (Timeout != 0x00))
    {
       Program_Flash_Mcu_Busy_Delay(50);
       status = Program_Flash_Get_Status();
       Timeout--;
    }

    if (Timeout == 0x00 )
    {
       status = PROGRAM_FLASH_TIMEOUT;
    }

    /* Return the operation status */
    return status;
}

/*******************************************************************************
* Function Name  : FLASH_ErasePage
* Description    : Erases a specified FLASH page.
* Input          : - Page_Address: The page address to be erased.
* Output         : None
* Return         : FLASH Status: The returned value can be: PROGRAM_FLASH_BUSY,
*                  PROGRAM_FLASH_ERROR_PG or PROGRAM_FLASH_ERROR_WRP or PROGRAM_FLASH_COMPLETE or
*                  PROGRAM_FLASH_TIMEOUT.
*******************************************************************************/
__attribute__ ((long_call))
__attribute__ ((section(".fcpy_section")))
uint32_t Program_Flash_Erase_Page(uint32_t page_address)
{
    PROGRAM_FLASH_Status_ET status;

    /* Wait for last operation to be completed */
    status = Program_Flash_Wait_For_Last_Operation(PROGRAM_FLASH_EraseTimeout);

    if (status == PROGRAM_FLASH_COMPLETE)
    {
      /* if the previous operation is completed, proceed to erase the page */
      FLASH->CR|= PROGRAM_FLASH_CR_PER_Set;
      FLASH->AR = page_address;
      FLASH->CR|= PROGRAM_FLASH_CR_STRT_Set;

      /* Wait for last operation to be completed */
      status = Program_Flash_Wait_For_Last_Operation(PROGRAM_FLASH_EraseTimeout);

      if (status != PROGRAM_FLASH_BUSY)
      {
        /* if the erase operation is completed, disable the PER Bit */
        FLASH->CR &= PROGRAM_FLASH_CR_PER_Reset;
      }
    }
    /* Return the Erase Status */
    return (status != PROGRAM_FLASH_COMPLETE);
}

__attribute__ ((long_call))
__attribute__ ((section(".fcpy_section")))
uint32_t Program_Flash_Write_Page(uint32_t page_address, const uint8_t *chunk, uint32_t chunk_len)
{
   PROGRAM_FLASH_Status_ET status;
   volatile uint16_t *wr_addr;
   uint16_t *data;
   uint32_t data_len;

   wr_addr = (volatile uint16_t*)page_address;
   data = (uint16_t*)chunk;
   data_len = chunk_len / 2U;

   /* Wait for last operation to be completed */
   status = Program_Flash_Wait_For_Last_Operation(PROGRAM_FLASH_ProgramTimeout);

   if (status == PROGRAM_FLASH_COMPLETE)
   {
       /* if the previous operation is completed, proceed to program the new data */
       FLASH->CR |= PROGRAM_FLASH_CR_PG_Set;
   }

   while (data_len)
   {
       *wr_addr = *data;
       /* Wait for last operation to be completed */
       status = Program_Flash_Wait_For_Last_Operation(PROGRAM_FLASH_ProgramTimeout);

      if (status == PROGRAM_FLASH_COMPLETE)
      {
         if (*wr_addr != *data)
         {
            status = PROGRAM_FLASH_ERROR_WRP;
            break;
         }
          data++;
          wr_addr++;
          data_len--;
      }
      else
      {
         break;
      }
   }

   if (status != PROGRAM_FLASH_BUSY)
   {
      /* if the program operation is completed, disable the PG Bit */
      FLASH->CR &= PROGRAM_FLASH_CR_PG_Reset;
   }

   return (status != PROGRAM_FLASH_COMPLETE);
}

