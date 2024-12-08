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

#include <stdio.h>
#include "main_get_exe.h"

#if(MAIN_EXE_LOG_TO_FILE)
FILE *log_file;
#define MAIN_LOG_OUT   log_file
#else
#define MAIN_LOG_OUT   stdout
#endif

static uint8_t working_buf[IOCMD_WORKING_BUF_RECOMMENDED_SIZE];

int main_print_text(void *dev, const char *string)
{
   int result = 0;

#if(MAIN_EXE_LOG_TO_FILE)
   log_file = fopen("./log.txt", "a");
#endif

   if(NULL != string)
   {
      result = strlen(string);

      fprintf(MAIN_LOG_OUT, "%s", string);
   }

#if(MAIN_EXE_LOG_TO_FILE)
   fclose(log_file);
#endif

   return result;
}

int main_print_text_repeat(void *dev, const char *string, int num_repeats)
{
   int result = 0;

#if(MAIN_EXE_LOG_TO_FILE)
   log_file = fopen("./log.txt", "a");
#endif

   if(NULL != string)
   {
      while(0 != num_repeats)
      {
         result += strlen(string);

         fprintf(MAIN_LOG_OUT, "%s", string);
         num_repeats--;
      }
   }

#if(MAIN_EXE_LOG_TO_FILE)
   fclose(log_file);
#endif

   return result;
}

int main_print_text_len(void *dev, const char *string, int str_len)
{
   int result = 0;

#if(MAIN_EXE_LOG_TO_FILE)
   log_file = fopen("./log.txt", "a");
#endif

   if(NULL != string)
   {
      if(0 == str_len)
      {
         result = strlen(string);

         fprintf(MAIN_LOG_OUT, "%s", string);
      }
      else
      {
         result = str_len;

         fprintf(MAIN_LOG_OUT, "%.*s", str_len, string);
      }
   }

#if(MAIN_EXE_LOG_TO_FILE)
   fclose(log_file);
#endif

   return result;
}

void main_print_endline_repeat(void *dev, int num_repeats)
{
#if(MAIN_EXE_LOG_TO_FILE)
   log_file = fopen("./log.txt", "a");
#endif

   while(0 != num_repeats)
   {
      fprintf(MAIN_LOG_OUT, "\n\r");
      num_repeats--;
   }

#if(MAIN_EXE_LOG_TO_FILE)
   fclose(log_file);
#endif
}

void main_print_cariage_return(void *dev)
{
#if(MAIN_EXE_LOG_TO_FILE)
   log_file = fopen("./log.txt", "a");
#endif

   fprintf(MAIN_LOG_OUT, "\r");

#if(MAIN_EXE_LOG_TO_FILE)
   fclose(log_file);
#endif
}

static IOCMD_Print_Exe_Params_XT main_exe_params = {
   NULL, main_print_text, main_print_text_repeat, main_print_text_len, main_print_endline_repeat, main_print_cariage_return};

IOCMD_Print_Exe_Params_XT *main_get_exe(void)
{
   return &main_exe_params;
}

void main_exe_print_results(void)
{
   IOCMD_Proc_Buffered_Logs(true, main_get_exe(), working_buf, sizeof(working_buf));
}
