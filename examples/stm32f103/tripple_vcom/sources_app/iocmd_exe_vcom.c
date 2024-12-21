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

#include "iocmd_exe_vcom.h"
#include "main_usbd_init.h"
#include "cmd_line.h"

static int vcom_print_text(void *dev, const char *string);
static int vcom_print_text_repeat(void *dev, const char *string, int num_repeats);
static int vcom_print_text_len(void *dev, const char *string, int str_len);
static void vcom_print_endline_repeat(void *dev, int num_repeats);
static void vcom_print_cariage_return(void *dev);
static void vcom_refresh_line(void *dev);

static const IOCMD_Print_Exe_Params_XT exe_vcom =
{
   .dev                 = &buf_1_in,
   .print_string        = vcom_print_text,
   .print_string_repeat = vcom_print_text_repeat,
   .print_string_len    = vcom_print_text_len,
   .print_endl_repeat   = vcom_print_endline_repeat,
   .print_cariage_return= vcom_print_cariage_return,
   .refresh_line        = vcom_refresh_line,
};

static void wait_until_ready(Buff_Ring_XT* buff, Buff_Size_DT size)
{
   if(OS_CONTEXT_TYPE_TASK == OS_Get_Current_Context_Type())
   {
      while(BUFF_RING_GET_FREE_SIZE(buff) < size)
      {
         OS_Sleep_Ms(1);
      }
   }
}

static int vcom_print_text(void *dev, const char *string)
{
   Buff_Ring_XT* buff = (Buff_Ring_XT*)dev;
   int result = 0;

   if(NULL != string)
   {
      result = strlen(string);
      wait_until_ready(buff, (Buff_Size_DT)result);
      Buff_Ring_Write(buff, string, (Buff_Size_DT)result, BUFF_TRUE, BUFF_TRUE);
   }

   return result;
}

static int vcom_print_text_repeat(void *dev, const char *string, int num_repeats)
{
   Buff_Ring_XT* buff = (Buff_Ring_XT*)dev;
   Buff_Size_DT len;
   int result = 0;

   if(NULL != string)
   {
      len = strlen(string);
      while(0 != num_repeats)
      {
         wait_until_ready(buff, len);
         Buff_Ring_Write(buff, string, len, BUFF_TRUE, BUFF_TRUE);

         num_repeats--;
         result += len;
      }
   }

   return result;
}

static int vcom_print_text_len(void *dev, const char *string, int str_len)
{
   Buff_Ring_XT* buff = (Buff_Ring_XT*)dev;
   int result = 0;

   if(NULL != string)
   {
      result = strlen(string);
      if(0 != str_len)
      {
         result = MIN(result, str_len);
      }
      wait_until_ready(buff, (Buff_Size_DT)result);
      Buff_Ring_Write(buff, string, (Buff_Size_DT)result, BUFF_TRUE, BUFF_TRUE);
   }

   return result;
}

static void vcom_print_endline_repeat(void *dev, int num_repeats)
{
   Buff_Ring_XT* buff = (Buff_Ring_XT*)dev;
   static const char endl[] = "\n\r";

   while(0 != num_repeats)
   {
      wait_until_ready(buff, sizeof(endl) - 1);
      Buff_Ring_Write(buff, endl, sizeof(endl) - 1, BUFF_TRUE, BUFF_TRUE);
      num_repeats--;
   }
}

static void vcom_print_cariage_return(void *dev)
{
   Buff_Ring_XT* buff = (Buff_Ring_XT*)dev;
   const char cariage_ret[] = "\r";

   wait_until_ready(buff, sizeof(cariage_ret) - 1);
   Buff_Ring_Write(buff, cariage_ret, sizeof(cariage_ret) - 1, BUFF_TRUE, BUFF_TRUE);
}

static void vcom_refresh_line(void *dev)
{
   const char *input_line = Cmd_Parser_Get_Current_Line();

   vcom_print_text(dev, ">");
   vcom_print_text(dev, input_line);
}

const IOCMD_Print_Exe_Params_XT *vcom_get_exe(void)
{
   return &exe_vcom;
}

