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

#include "keyboard.h"
#include "main_usbd_init.h"
#include "iocmd_exe_vcom.h"

void Keyboard_In_Report_Done(HID_Report_XT *report, USBD_Bool_DT is_feature)
{
   USBD_UNUSED_PARAM(report);
   USBD_UNUSED_PARAM(is_feature);

   memset(keyboard_report_in.keys, 0, sizeof(keyboard_report_in.keys));
} /* Keyboard_In_Report_Done */

void Keyboard_Out_Report_Done(HID_Report_XT *report, USBD_Bool_DT is_feature)
{
   USBD_UNUSED_PARAM(report);
   USBD_UNUSED_PARAM(is_feature);

} /* Keyboard_Out_Report_Done */

