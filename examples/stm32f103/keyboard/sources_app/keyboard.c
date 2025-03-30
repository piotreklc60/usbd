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

#define KEY_SHIFT_NO          0
#define KEY_SHIFT_YES         1
#define KEY_SHIFT_INVERT_CAPS 2
#define KEY_SHIFT_COPY_CAPS   3

#define KEYBOARD_ASCII_TO_KEY_CONVERT_TAB_OFFSET      32

typedef struct
{
   char    key;
   uint8_t shift;
}keyboard_ascii_to_key_convert_DT;

static keyboard_ascii_to_key_convert_DT keyboard_ascii_to_key_convert_tab[127 - KEYBOARD_ASCII_TO_KEY_CONVERT_TAB_OFFSET] =
{
   /* 32 */ {0x2C, KEY_SHIFT_NO},  /* SPACE */
   /* 33 */ {0x1E, KEY_SHIFT_YES}, /* ! */
   /* 34 */ {0x34, KEY_SHIFT_YES}, /* " */
   /* 35 */ {0x20, KEY_SHIFT_YES}, /* # */
   /* 36 */ {0x21, KEY_SHIFT_YES}, /* $ */
   /* 37 */ {0x22, KEY_SHIFT_YES}, /* % */
   /* 38 */ {0x24, KEY_SHIFT_YES}, /* & */
   /* 39 */ {0x34, KEY_SHIFT_NO},  /* ' */
   /* 40 */ {0x26, KEY_SHIFT_YES}, /* ( */
   /* 41 */ {0x27, KEY_SHIFT_YES}, /* ) */
   /* 42 */ {0x25, KEY_SHIFT_YES}, /* * */
   /* 43 */ {0x2E, KEY_SHIFT_YES}, /* + */
   /* 44 */ {0x36, KEY_SHIFT_NO},  /* , */
   /* 45 */ {0x2D, KEY_SHIFT_NO},  /* - */
   /* 46 */ {0x37, KEY_SHIFT_NO},  /* . */
   /* 47 */ {0x38, KEY_SHIFT_NO},  /* / */
   /* 48 */ {0x27, KEY_SHIFT_NO},  /* 0 */
   /* 49 */ {0x1E, KEY_SHIFT_NO},  /* 1 */
   /* 50 */ {0x1F, KEY_SHIFT_NO},  /* 2 */
   /* 51 */ {0x20, KEY_SHIFT_NO},  /* 3 */
   /* 52 */ {0x21, KEY_SHIFT_NO},  /* 4 */
   /* 53 */ {0x22, KEY_SHIFT_NO},  /* 5 */
   /* 54 */ {0x23, KEY_SHIFT_NO},  /* 6 */
   /* 55 */ {0x24, KEY_SHIFT_NO},  /* 7 */
   /* 56 */ {0x25, KEY_SHIFT_NO},  /* 8 */
   /* 57 */ {0x26, KEY_SHIFT_NO},  /* 9 */
   /* 58 */ {0x33, KEY_SHIFT_YES}, /* : */
   /* 59 */ {0x33, KEY_SHIFT_NO},  /* ; */
   /* 60 */ {0x36, KEY_SHIFT_YES}, /* < */
   /* 61 */ {0x2E, KEY_SHIFT_NO},  /* = */
   /* 62 */ {0x37, KEY_SHIFT_YES}, /* > */
   /* 63 */ {0x38, KEY_SHIFT_YES}, /* ? */
   /* 64 */ {0x1F, KEY_SHIFT_YES}, /* @ */
   /* 65 */ {0x04, KEY_SHIFT_INVERT_CAPS}, /* A */
   /* 66 */ {0x05, KEY_SHIFT_INVERT_CAPS}, /* B */
   /* 67 */ {0x06, KEY_SHIFT_INVERT_CAPS}, /* C */
   /* 68 */ {0x07, KEY_SHIFT_INVERT_CAPS}, /* D */
   /* 69 */ {0x08, KEY_SHIFT_INVERT_CAPS}, /* E */
   /* 70 */ {0x09, KEY_SHIFT_INVERT_CAPS}, /* F */
   /* 71 */ {0x0A, KEY_SHIFT_INVERT_CAPS}, /* G */
   /* 72 */ {0x0B, KEY_SHIFT_INVERT_CAPS}, /* H */
   /* 73 */ {0x0C, KEY_SHIFT_INVERT_CAPS}, /* I */
   /* 74 */ {0x0D, KEY_SHIFT_INVERT_CAPS}, /* J */
   /* 75 */ {0x0E, KEY_SHIFT_INVERT_CAPS}, /* K */
   /* 76 */ {0x0F, KEY_SHIFT_INVERT_CAPS}, /* L */
   /* 77 */ {0x10, KEY_SHIFT_INVERT_CAPS}, /* M */
   /* 78 */ {0x11, KEY_SHIFT_INVERT_CAPS}, /* N */
   /* 79 */ {0x12, KEY_SHIFT_INVERT_CAPS}, /* O */
   /* 80 */ {0x13, KEY_SHIFT_INVERT_CAPS}, /* P */
   /* 81 */ {0x14, KEY_SHIFT_INVERT_CAPS}, /* Q */
   /* 82 */ {0x15, KEY_SHIFT_INVERT_CAPS}, /* R */
   /* 83 */ {0x16, KEY_SHIFT_INVERT_CAPS}, /* S */
   /* 84 */ {0x17, KEY_SHIFT_INVERT_CAPS}, /* T */
   /* 85 */ {0x18, KEY_SHIFT_INVERT_CAPS}, /* U */
   /* 86 */ {0x19, KEY_SHIFT_INVERT_CAPS}, /* V */
   /* 87 */ {0x1A, KEY_SHIFT_INVERT_CAPS}, /* W */
   /* 88 */ {0x1B, KEY_SHIFT_INVERT_CAPS}, /* X */
   /* 89 */ {0x1C, KEY_SHIFT_INVERT_CAPS}, /* Y */
   /* 90 */ {0x1D, KEY_SHIFT_INVERT_CAPS}, /* Z */
   /* 91 */ {0x2F, KEY_SHIFT_NO},  /* [ */
   /* 92 */ {0x31, KEY_SHIFT_NO},  /* \ */
   /* 93 */ {0x30, KEY_SHIFT_NO},  /* ] */
   /* 94 */ {0x23, KEY_SHIFT_YES}, /* ^ */
   /* 95 */ {0x2D, KEY_SHIFT_YES}, /* _ */
   /* 96 */ {0x35, KEY_SHIFT_NO},  /* ` */
   /* 97 */ {0x04, KEY_SHIFT_COPY_CAPS}, /* a */
   /* 98 */ {0x05, KEY_SHIFT_COPY_CAPS}, /* b */
   /* 99 */ {0x06, KEY_SHIFT_COPY_CAPS}, /* c */
   /*100 */ {0x07, KEY_SHIFT_COPY_CAPS}, /* d */
   /*101 */ {0x08, KEY_SHIFT_COPY_CAPS}, /* e */
   /*102 */ {0x09, KEY_SHIFT_COPY_CAPS}, /* f */
   /*103 */ {0x0A, KEY_SHIFT_COPY_CAPS}, /* g */
   /*104 */ {0x0B, KEY_SHIFT_COPY_CAPS}, /* h */
   /*105 */ {0x0C, KEY_SHIFT_COPY_CAPS}, /* i */
   /*106 */ {0x0D, KEY_SHIFT_COPY_CAPS}, /* j */
   /*107 */ {0x0E, KEY_SHIFT_COPY_CAPS}, /* k */
   /*108 */ {0x0F, KEY_SHIFT_COPY_CAPS}, /* l */
   /*109 */ {0x10, KEY_SHIFT_COPY_CAPS}, /* m */
   /*110 */ {0x11, KEY_SHIFT_COPY_CAPS}, /* n */
   /*111 */ {0x12, KEY_SHIFT_COPY_CAPS}, /* o */
   /*112 */ {0x13, KEY_SHIFT_COPY_CAPS}, /* p */
   /*113 */ {0x14, KEY_SHIFT_COPY_CAPS}, /* q */
   /*114 */ {0x15, KEY_SHIFT_COPY_CAPS}, /* r */
   /*115 */ {0x16, KEY_SHIFT_COPY_CAPS}, /* s */
   /*116 */ {0x17, KEY_SHIFT_COPY_CAPS}, /* t */
   /*117 */ {0x18, KEY_SHIFT_COPY_CAPS}, /* u */
   /*118 */ {0x19, KEY_SHIFT_COPY_CAPS}, /* v */
   /*119 */ {0x1A, KEY_SHIFT_COPY_CAPS}, /* w */
   /*120 */ {0x1B, KEY_SHIFT_COPY_CAPS}, /* x */
   /*121 */ {0x1C, KEY_SHIFT_COPY_CAPS}, /* y */
   /*122 */ {0x1D, KEY_SHIFT_COPY_CAPS}, /* z */
   /*123 */ {0x2F, KEY_SHIFT_YES}, /* { */
   /*124 */ {0x31, KEY_SHIFT_YES}, /* | */
   /*125 */ {0x30, KEY_SHIFT_YES}, /* } */
   /*126 */ {0x35, KEY_SHIFT_YES}  /* ~ */
};
static USBD_Bool_DT keyboard_key_via_command_set;

void Keyboard_In_Report_Done(HID_Report_XT *report, USBD_Bool_DT is_feature)
{
   USBD_UNUSED_PARAM(report);
   USBD_UNUSED_PARAM(is_feature);

   memset(&keyboard_report_in, 0, sizeof(keyboard_report_in));

   if(USBD_BOOL_IS_TRUE(keyboard_key_via_command_set))
   {
      keyboard_key_via_command_set = USBD_FALSE;
      HID_Mark_Report_In_Changed(&hid, 0);
      USBD_NOTICE(MAIN_APP, "re-init IN report");
   }
} /* Keyboard_In_Report_Done */

void Keyboard_Out_Report_Done(HID_Report_XT *report, USBD_Bool_DT is_feature)
{
   USBD_UNUSED_PARAM(report);
   USBD_UNUSED_PARAM(is_feature);

#ifndef USBD_USE_IOCMD

   IOCMD_Printf_Line("");
   IOCMD_Printf_Line("%12s: %d", "num_lock",    keyboard_report_out.num_lock);
   IOCMD_Printf_Line("%12s: %d", "caps_lock",   keyboard_report_out.caps_lock);
   IOCMD_Printf_Line("%12s: %d", "scrool_lock", keyboard_report_out.scrool_lock);
   IOCMD_Printf_Line("%12s: %d", "compose",     keyboard_report_out.compose);
   IOCMD_Printf_Line("%12s: %d", "kana",        keyboard_report_out.kana);
   IOCMD_Printf_Line("");

#else

   USBD_NOTICE_2(MAIN_APP, "%12s: %d", "num_lock",    keyboard_report_out.num_lock);
   USBD_NOTICE_2(MAIN_APP, "%12s: %d", "caps_lock",   keyboard_report_out.caps_lock);
   USBD_NOTICE_2(MAIN_APP, "%12s: %d", "scrool_lock", keyboard_report_out.scrool_lock);
   USBD_NOTICE_2(MAIN_APP, "%12s: %d", "compose",     keyboard_report_out.compose);
   USBD_NOTICE_2(MAIN_APP, "%12s: %d", "kana",        keyboard_report_out.kana);

#endif
} /* Keyboard_Out_Report_Done */

void Keybord_Send_Ascii_Char(char key)
{
   uint8_t key_index = (uint8_t)key - KEYBOARD_ASCII_TO_KEY_CONVERT_TAB_OFFSET;

   if(key_index < Num_Elems(keyboard_ascii_to_key_convert_tab))
   {
      keyboard_report_in.keys[0] = keyboard_ascii_to_key_convert_tab[key_index].key;
      if(KEY_SHIFT_YES == keyboard_ascii_to_key_convert_tab[key_index].shift)
      {
         keyboard_report_in.left_shift = 1;
      }
      else if(KEY_SHIFT_INVERT_CAPS == keyboard_ascii_to_key_convert_tab[key_index].shift)
      {
         keyboard_report_in.left_shift = !keyboard_report_out.caps_lock;
      }
      else if(KEY_SHIFT_COPY_CAPS == keyboard_ascii_to_key_convert_tab[key_index].shift)
      {
         keyboard_report_in.left_shift = keyboard_report_out.caps_lock;
      }
      keyboard_key_via_command_set = USBD_TRUE;
      HID_Mark_Report_In_Changed(&hid, 0);
   }
} /* Keybord_Send_Ascii_Char */

