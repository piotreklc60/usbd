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

#ifndef USBD_PUBLIC_MACRO_H_
#define USBD_PUBLIC_MACRO_H_

#ifndef USBD_FEATURE_PRESENT
#define USBD_FEATURE_PRESENT                                      (1)
#endif

#ifndef USBD_FEATURE_NOT_PRESENT
#define USBD_FEATURE_NOT_PRESENT                                  (0)
#endif

#ifndef USBD_BOOL_IS_TRUE
#define USBD_BOOL_IS_TRUE(bl)                                     (bl)
#endif

#ifndef USBD_BOOL_IS_FALSE
#define USBD_BOOL_IS_FALSE(bl)                                    (!(bl))
#endif

#ifndef USBD_BOOL_TOGGLE
#define USBD_BOOL_TOGGLE(bl)                                      (bl) = (!bl)
#endif

#ifndef USBD_ATOMIC_BOOL_IS_TRUE
#define USBD_ATOMIC_BOOL_IS_TRUE(bl)                              (bl)
#endif

#ifndef USBD_ATOMIC_BOOL_IS_FALSE
#define USBD_ATOMIC_BOOL_IS_FALSE(bl)                             (!(bl))
#endif

#ifndef USBD_ATOMIC_BOOL_GET
#define USBD_ATOMIC_BOOL_GET(_data)                               (USBD_Bool_DT)(_data)
#endif

#ifndef USBD_ATOMIC_BOOL_SET
#define USBD_ATOMIC_BOOL_SET(_data, _value)                       (_data) = (_value)
#endif

#ifndef USBD_ATOMIC_UINT8_GET
#define USBD_ATOMIC_UINT8_GET(_data)                              (uint8_t)(_data)
#endif

#ifndef USBD_ATOMIC_UINT8_SET
#define USBD_ATOMIC_UINT8_SET(_data, _value)                      (_data) = (_value)
#endif

#ifndef USBD_ATOMIC_UINT16_GET
#define USBD_ATOMIC_UINT16_GET(_data)                             (uint16_t)(_data)
#endif

#ifndef USBD_ATOMIC_UINT16_SET
#define USBD_ATOMIC_UINT16_SET(_data, _value)                     (_data) = (_value)
#endif

#ifndef USBD_ATOMIC_UINT32_GET
#define USBD_ATOMIC_UINT32_GET(_data)                             (uint32_t)(_data)
#endif

#ifndef USBD_ATOMIC_UINT32_SET
#define USBD_ATOMIC_UINT32_SET(_data, _value)                     (_data) = (_value)
#endif

#ifndef USBD_ATOMIC_UINT64_GET
#define USBD_ATOMIC_UINT64_GET(_data)                             (uint64_t)(_data)
#endif

#ifndef USBD_ATOMIC_UINT64_SET
#define USBD_ATOMIC_UINT64_SET(_data, _value)                     (_data) = (_value)
#endif

#ifndef USBD_CHECK_PTR
#define USBD_CHECK_PTR(type, ptr)                                 (((type*)0) != (ptr))
#endif

#ifndef USBD_CHECK_HANDLER
#define USBD_CHECK_HANDLER(type, handler)                         (((type)0) != (handler))
#endif

/** true when pointers are equal */
#ifndef USBD_COMPARE_PTRS
#define USBD_COMPARE_PTRS(type1, ptr1, type2, ptr2)               (((type1*)(ptr1)) == ((type2*)(ptr2)))
#endif

/** true when handlers are equal */
#ifndef USBD_COMPARE_HANDLERS
#define USBD_COMPARE_HANDLERS(type1, handler1, type2, handler2)   (((type1)(handler1)) == ((type2)(handler2)))
#endif

#ifndef USBD_GET_PTR
#define USBD_GET_PTR(type, ptr)                                   (/*(type*)*/(ptr))
#endif

#ifndef USBD_GET_HANDLER
#define USBD_GET_HANDLER(type, handler)                           (/*(type)*/(handler))
#endif

#ifndef USBD_SET_PTR
#define USBD_SET_PTR(type, ptr, new_value)                        (ptr) = /*(type*)*/(new_value)
#endif

#ifndef USBD_SET_HANDLER
#define USBD_SET_HANDLER(type, handler, new_value)                (handler) = /*(type)*/(new_value)
#endif

#ifndef USBD_CALL_HANDLER
#define USBD_CALL_HANDLER(type, handler)                          ((type)(handler))
#endif

#ifndef USBD_MAKE_INVALID_PTR
#define USBD_MAKE_INVALID_PTR(type)                               ((type*)(NULL))
#endif

#ifndef USBD_MAKE_INVALID_HANDLER
#define USBD_MAKE_INVALID_HANDLER(type)                           ((type)(NULL))
#endif

#ifndef USBD_INVALIDATE_PTR
#define USBD_INVALIDATE_PTR(type, ptr)                            (ptr) = USBD_MAKE_INVALID_PTR(type)
#endif

#ifndef USBD_INVALIDATE_HANDLER
#define USBD_INVALIDATE_HANDLER(type, handler)                    (handler) = USBD_MAKE_INVALID_HANDLER(type)
#endif

#ifndef USBD_UNUSED_PARAM
#define USBD_UNUSED_PARAM(param)                                  (void)(param)
#endif

#ifndef USBD_USE_INVALIDATION_ON_INIT
#define USBD_USE_INVALIDATION_ON_INIT                             USBD_FEATURE_PRESENT
#endif

#if (USBD_FEATURE_NOT_PRESENT == USBD_USE_INVALIDATION_ON_INIT)

#ifndef USBD_INVALIDATE_PTR_ON_INIT
#define USBD_INVALIDATE_PTR_ON_INIT(type, ptr)
#endif
#ifndef USBD_INVALIDATE_HANDLER_ON_INIT
#define USBD_INVALIDATE_HANDLER_ON_INIT(type, handler)
#endif

#else

#ifndef USBD_INVALIDATE_PTR_ON_INIT
#define USBD_INVALIDATE_PTR_ON_INIT(type, ptr)                    (ptr) = USBD_MAKE_INVALID_PTR(type)
#endif
#ifndef USBD_INVALIDATE_HANDLER_ON_INIT
#define USBD_INVALIDATE_HANDLER_ON_INIT(type, handler)            (handler) = USBD_MAKE_INVALID_HANDLER(type)
#endif

#endif


#ifndef USBD_ENTER_FUNC
#define USBD_ENTER_FUNC()
#endif


#ifndef USBD_EXIT_FUNC
#define USBD_EXIT_FUNC()
#endif


#ifndef USB_ENTER_FUNC
#define USB_ENTER_FUNC(id)      USBD_ENTER_FUNC(id)
#endif


#ifndef USB_EXIT_FUNC
#define USB_EXIT_FUNC(id)       USBD_EXIT_FUNC(id)
#endif


#ifndef USBD_ENTER_CRITICAL
#define USBD_ENTER_CRITICAL()
#endif


#ifndef USBD_EXIT_CRITICAL
#define USBD_EXIT_CRITICAL()
#endif


#ifndef USBD_DECLARE_INVOKE_PARAMS
#define USBD_DECLARE_INVOKE_PARAMS(_invoke_params)
#endif


#ifndef USBD_INIT_INVOKE_PARAMS
#define USBD_INIT_INVOKE_PARAMS(_invoke_params)
#endif


#ifndef USBD_MARK_INVOKE_DESTINATION
#define USBD_MARK_INVOKE_DESTINATION(_invoke_params)
#endif


#ifndef USBD_IS_INVOKE_NEEDED
#define USBD_IS_INVOKE_NEEDED(_invoke_params)      USBD_FALSE
#endif


#ifndef USBD_INVOKE
/* action MUST return boolean! */
#define USBD_INVOKE(_invoke_params, _action)       ((!USBD_IS_INVOKE_NEEDED(_invoke_params)) && _action)
#endif


#endif
