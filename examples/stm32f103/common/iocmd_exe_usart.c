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
#include "cmd_line.h"

static int usart_print_text(void *dev, const char *string);
static int usart_print_text_repeat(void *dev, const char *string, int num_repeats);
static int usart_print_text_len(void *dev, const char *string, int str_len);
static void usart_print_endline_repeat(void *dev, int num_repeats);
static void usart_print_cariage_return(void *dev);
static void usart_refresh_line(void *dev);

static const IOCMD_Print_Exe_Params_XT exe_usart =
{
#if(2 == IOCMD_EXE_USART_NUM)
   .dev                 = USART2,
#elif(3 == IOCMD_EXE_USART_NUM)
   .dev                 = USART3,
#else
   .dev                 = USART1,
#endif
   .print_string        = usart_print_text,
   .print_string_repeat = usart_print_text_repeat,
   .print_string_len    = usart_print_text_len,
   .print_endl_repeat   = usart_print_endline_repeat,
   .print_cariage_return= usart_print_cariage_return,
   .refresh_line        = usart_refresh_line,
};

static int usart_print_text(void *dev, const char *string)
{
   USART_TypeDef* USARTx = (NULL != dev) ? (USART_TypeDef*)dev : USART1;
   int result = 0;

   if(NULL != string)
   {
      while('\0' != string[result])
      {
         if (USARTx->SR & 0x80)
         {
            USARTx->DR = string[result];
            result++;
         }
      }
   }

   return result;
}

static int usart_print_text_repeat(void *dev, const char *string, int num_repeats)
{
   USART_TypeDef* USARTx = (NULL != dev) ? (USART_TypeDef*)dev : USART1;
   const char *text;
   int result = 0;

   if(NULL != string)
   {
      while(0 != num_repeats)
      {
         text = string;

         while('\0' != *text)
         {
            if (USARTx->SR & 0x80)
            {
               USARTx->DR = *text;
               text++;
               result++;
            }
         }

         num_repeats--;
      }
   }

   return result;
}

static int usart_print_text_len(void *dev, const char *string, int str_len)
{
   USART_TypeDef* USARTx = (NULL != dev) ? (USART_TypeDef*)dev : USART1;
   int result = 0;

   if(NULL != string)
   {
      if(0 == str_len)
      {
         while('\0' != string[result])
         {
            if (USARTx->SR & 0x80)
            {
               USARTx->DR = string[result];
               result++;
            }
         }
      }
      else
      {
         while(('\0' != string[result]) && (0 != str_len))
         {
            if (USARTx->SR & 0x80)
            {
               USARTx->DR = string[result];
               str_len--;
               result++;
            }
         }
      }
   }

   return result;
}

static void usart_print_endline_repeat(void *dev, int num_repeats)
{
   USART_TypeDef* USARTx = (NULL != dev) ? (USART_TypeDef*)dev : USART1;
   static const char endl[] = "\n\r";
   const char *string;

   while(0 != num_repeats)
   {
      string = endl;

      while('\0' != *string)
      {
         if (USARTx->SR & 0x80)
         {
            USARTx->DR = *string;
            string++;
         }
      }
      num_repeats--;
   }
}

static void usart_print_cariage_return(void *dev)
{
   USART_TypeDef* USARTx = (NULL != dev) ? (USART_TypeDef*)dev : USART1;
   const char *cariage_ret = "\r";

   while('\0' != *cariage_ret)
   {
      if (USARTx->SR & 0x80)
      {
         USARTx->DR = *cariage_ret;
         cariage_ret++;
      }
   }
}

static void usart_refresh_line(void *dev)
{
   USART_TypeDef* USARTx = (NULL != dev) ? (USART_TypeDef*)dev : USART1;
   const char *input_line = Cmd_Parser_Get_Current_Line();

   usart_print_text(dev, ">");

   while('\0' != *input_line)
   {
      if (USARTx->SR & 0x80)
      {
         USARTx->DR = *input_line;
         input_line++;
      }
   }
}

const IOCMD_Print_Exe_Params_XT *usart_get_exe(void)
{
   return &exe_usart;
}

