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

#include "mouse.h"
#include "main_usbd_init.h"
#include "iocmd_exe_vcom.h"

#ifndef MOUSE_STEP
#define MOUSE_STEP         10
#endif

static USBD_Bool_DT up(void);
static USBD_Bool_DT down(void);
static USBD_Bool_DT left(void);
static USBD_Bool_DT right(void);
static USBD_Bool_DT left_clk(void);
static USBD_Bool_DT right_clk(void);

typedef USBD_Bool_DT (*mouse_action_HT)(void);

typedef struct
{
   const char       *pattern;
   uint8_t           size;
   mouse_action_HT   action;
}mouse_map_XT;

static mouse_map_XT mouse_map[] =
{
   {"\x1B\x5B\x41", 3, up},            /* up arrow */
   {"\x1B\x5B\x42", 3, down},          /* down arrow */
   {"\x1B\x5B\x44", 3, left},          /* left arrow */
   {"\x1B\x5B\x43", 3, right},         /* right arrow */
   {"\x5B\x33\x7E", 3, left_clk},      /* delete */
   {"\x1B\x5B\x36\x7E", 4, right_clk}  /* page down */
};

static USBD_Bool_DT up(void)
{
   mouse_report.y -= MOUSE_STEP;
   return USBD_TRUE;
}
static USBD_Bool_DT down(void)
{
   mouse_report.y += MOUSE_STEP;
   return USBD_TRUE;
}
static USBD_Bool_DT left(void)
{
   mouse_report.x -= MOUSE_STEP;
   return USBD_TRUE;
}
static USBD_Bool_DT right(void)
{
   mouse_report.x += MOUSE_STEP;
   return USBD_TRUE;
}
static USBD_Bool_DT left_clk(void)
{
   mouse_report.button_1 = !mouse_report.button_1;
   return USBD_TRUE;
}
static USBD_Bool_DT right_clk(void)
{
   mouse_report.button_2 = !mouse_report.button_2;
   return USBD_TRUE;
}

void Mouse_Parse_Cmd_Bytes(uint8_t *recv_bytes, size_t num_recv_bytes)
{
   uint8_t cntr;
   USBD_Bool_DT has_changed = USBD_FALSE;

   if(USBD_CHECK_PTR(uint8_t, recv_bytes))
   {
      do
      {
         for(cntr = 0; cntr < Num_Elems(mouse_map); ++cntr)
         {
            if(num_recv_bytes >= mouse_map[cntr].size)
            {
               if(0 == memcmp(mouse_map[cntr].pattern, recv_bytes, mouse_map[cntr].size))
               {
                  has_changed = mouse_map[cntr].action();
                  recv_bytes     += mouse_map[cntr].size - 1;
                  num_recv_bytes -= mouse_map[cntr].size - 1;
                  break;
               }
            }
         }

         ++recv_bytes;
         --num_recv_bytes;
      }while(num_recv_bytes > 0);

      if(USBD_BOOL_IS_TRUE(has_changed))
      {
         HID_Mark_Report_In_Changed(&hid, 0);
      }
   }
} /* Mouse_Parse_Cmd_Bytes */

void Mouse_In_Report_Done(HID_Report_XT *report, USBD_Bool_DT is_feature)
{
   USBD_UNUSED_PARAM(report);
   USBD_UNUSED_PARAM(is_feature);

   mouse_report.x = 0;
   mouse_report.y = 0;
} /* Mouse_In_Report_Done */

