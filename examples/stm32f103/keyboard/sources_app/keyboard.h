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

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "usbd.h"

typedef struct
{
   uint8_t left_ctrl    : 1;
   uint8_t left_shift   : 1;
   uint8_t left_alt     : 1;
   uint8_t left_gui     : 1;
   uint8_t right_ctrl   : 1;
   uint8_t right_shift  : 1;
   uint8_t right_alt    : 1;
   uint8_t right_gui    : 1;
   uint8_t reserved;
   uint8_t keys[6];
} Keyboard_Report_In_DT;

typedef struct
{
   uint8_t num_lock     : 1;
   uint8_t caps_lock    : 1;
   uint8_t scrool_lock  : 1;
   uint8_t compose      : 1;
   uint8_t kana         : 1;
   uint8_t reserved     : 3;
} Keyboard_Report_Out_DT;

void Keyboard_In_Report_Done(HID_Report_XT *report, USBD_Bool_DT is_feature);

void Keyboard_Out_Report_Done(HID_Report_XT *report, USBD_Bool_DT is_feature);

void Keybord_Send_Ascii_Char(char key);

#endif
