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

#include "iocmd_exe_usart.h"

#define EXCEPTIONS_DEBUG      0

#if(EXCEPTIONS_DEBUG)

void print_scb_regs(const IOCMD_Print_Exe_Params_XT *exe)
{
   char temp[11];
   exe->print_string(exe->dev, "HFSR 0x");
   (void)IOCMD_Utoa32(temp, (uint32_t)SCB->HFSR, 16, 8, '0', true);
   exe->print_string(exe->dev, temp);
   exe->print_endl_repeat(exe->dev, 1);
   exe->print_string(exe->dev, "CFSR 0x");
   (void)IOCMD_Utoa32(temp, (uint32_t)SCB->CFSR, 16, 8, '0', true);
   exe->print_string(exe->dev, temp);
   exe->print_endl_repeat(exe->dev, 1);
   exe->print_string(exe->dev, "MMFAR 0x");
   (void)IOCMD_Utoa32(temp, (uint32_t)SCB->MMFAR, 16, 8, '0', true);
   exe->print_string(exe->dev, temp);
   exe->print_endl_repeat(exe->dev, 1);
   exe->print_string(exe->dev, "BFAR 0x");
   (void)IOCMD_Utoa32(temp, (uint32_t)SCB->BFAR, 16, 8, '0', true);
   exe->print_string(exe->dev, temp);
   exe->print_endl_repeat(exe->dev, 1);
   exe->print_string(exe->dev, "context type: ");
   (void)IOCMD_Utoa32(temp, OS_Get_Current_Context_Type(), 16, 8, '0', true);
   exe->print_string(exe->dev, temp);
   exe->print_endl_repeat(exe->dev, 1);
   exe->print_string(exe->dev, "context id: ");
   (void)IOCMD_Utoa32(temp, OS_Get_Current_Context_Id(), 16, 8, '0', true);
   exe->print_string(exe->dev, temp);
   exe->print_endl_repeat(exe->dev, 1);
}
#endif

void NMI_Handler(void)
{
#if(EXCEPTIONS_DEBUG)
   const IOCMD_Print_Exe_Params_XT *exe = usart_get_exe();
   exe->print_endl_repeat(exe->dev, 1);
   exe->print_string(exe->dev, __FUNCTION__);
   exe->print_endl_repeat(exe->dev, 1);
   print_scb_regs(exe);
#endif

   while(1);
}

void HardFault_Handler(void)
{
#if(EXCEPTIONS_DEBUG)
   const IOCMD_Print_Exe_Params_XT *exe = usart_get_exe();
   exe->print_endl_repeat(exe->dev, 1);
   exe->print_string(exe->dev, __FUNCTION__);
   if(0 != (0x40000000 & SCB->HFSR))
   {
      exe->print_string(exe->dev, " generated by other fault currently masked");
   }
   if(0 != (0x2 & SCB->HFSR))
   {
      exe->print_string(exe->dev, " generated by vector table read");
   }
   exe->print_endl_repeat(exe->dev, 1);
   print_scb_regs(exe);
#endif

   while(1);
}

void MemManage_Handler(void)
{
#if(EXCEPTIONS_DEBUG)
   const IOCMD_Print_Exe_Params_XT *exe = usart_get_exe();
   exe->print_endl_repeat(exe->dev, 1);
   exe->print_string(exe->dev, __FUNCTION__);
   exe->print_endl_repeat(exe->dev, 1);
   print_scb_regs(exe);
#endif

   while(1);
}

void BusFault_Handler(void)
{
#if(EXCEPTIONS_DEBUG)
   const IOCMD_Print_Exe_Params_XT *exe = usart_get_exe();
   exe->print_endl_repeat(exe->dev, 1);
   exe->print_string(exe->dev, __FUNCTION__);
   exe->print_endl_repeat(exe->dev, 1);
   print_scb_regs(exe);
#endif

   while(1);
}

void UsageFault_Handler(void)
{
#if(EXCEPTIONS_DEBUG)
   const IOCMD_Print_Exe_Params_XT *exe = usart_get_exe();
   exe->print_endl_repeat(exe->dev, 1);
   exe->print_string(exe->dev, __FUNCTION__);
   exe->print_endl_repeat(exe->dev, 1);
   print_scb_regs(exe);
#endif

   while(1);
}

void DebugMon_Handler(void)
{
#if(EXCEPTIONS_DEBUG)
   const IOCMD_Print_Exe_Params_XT *exe = usart_get_exe();
   exe->print_endl_repeat(exe->dev, 1);
   exe->print_string(exe->dev, __FUNCTION__);
   exe->print_endl_repeat(exe->dev, 1);
   print_scb_regs(exe);
#endif

   while(1);
}

