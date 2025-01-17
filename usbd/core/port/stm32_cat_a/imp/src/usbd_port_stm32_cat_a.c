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

#ifndef USBD_PORT_STM32_CAT_A_C_
#define USBD_PORT_STM32_CAT_A_C_
#endif

#ifndef USBD_PORT_STM32_CAT_A_H_
#include "usbd_port_stm32_cat_a.h"
#endif

#ifndef USBD_PORT_STM32_CAT_A_BOARD_SETTINGS_H_
#include "usbd_port_stm32_cat_a_board_settings.h"
#endif

#ifndef USBD_PORT_STM32_CAT_A_REGS_H_
#include "usbd_port_stm32_cat_a_regs.h"
#endif

#ifndef USBD_DEV_PRESENT
#error "DEV module must be present to use this port implementation"
#endif

#ifndef USBD_IO_PRESENT
#error "IO module must be present to use this port implementation"
#endif

#ifndef USBD_REQUEST_PRESENT
#error "REQUEST module must be present to use this port implementation"
#endif

#if((USBD_FEATURE_PRESENT != USBD_DEV_SUPPORT_CONFIG_VALIDATION) && (USBD_MAX_NUM_ENDPOINTS > 1))
#error "USBD_DEV_SUPPORT_CONFIG_VALIDATION must be enabled when more than endpoint ) is used to allocate EPs shared memory properly!"
#endif



#define USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS               8
#define USBD_PORT_STM32_CAT_A_MAX_NUM_EP_BUFFERS              16
#define USBD_PORT_STM32_CAT_A_EPS_SHARED_MEM_SIZE             512
#define USBD_PORT_STM32_CAT_A_EPS_SHARED_MEM_ALOC_TAB_SIZE    64
#define USBD_PORT_STM32_CAT_A_EPS_SHARED_MEM_START_POINT      \
   (USBD_PORT_STM32_CAT_A_EPS_SHARED_MEM_SIZE - (USBD_PORT_STM32_CAT_A_EPS_SHARED_MEM_ALOC_TAB_SIZE + (2 * USBD_PORT_STM32_CAT_A_EP0_MPS)))

#define USBD_PORT_STM32_CAT_A_DUMP_LIMIT_PRINTS               10

/** ******************************************************************************************************************************
 * USBD DEV part types
 ********************************************************************************************************************************/

typedef struct port_stm32_cat_a_dev_if_estimation_Data_Tag
{
   size_t max_buffer_reservation;
   uint_fast8_t num_buffers;
}port_stm32_cat_a_dev_if_estimation_DT;

typedef struct port_stm32_cat_a_dev_ep_estimation_Data_Tag
{
   struct
   {
      uint8_t  in;
      uint8_t  out;
   }num_bufs;
}port_stm32_cat_a_dev_ep_estimation_DT;

typedef struct port_stm32_cat_a_dev_interface_shared_mem_start_point_Data_Tag
{
   uint16_t start_point;
   uint16_t size;
   uint16_t current_offset;
}port_stm32_cat_a_dev_interface_shared_mem_start_point_DT;

typedef struct port_stm32_cat_a_dev_params_eXtended_Tag
{
   uint16_t device_status;
   uint8_t bulk_eps_num_bufs;
   USBD_Bool_DT out_on_delay;
}port_stm32_cat_a_dev_params_XT;

typedef struct port_stm32_cat_a_req_params_eXtended_Tag
{
#if(USBD_FEATURE_PRESENT == USBD_DEV_SUPPORT_CONFIG_VALIDATION)
   USBD_Bool_DT set_configuration_ongoing;
#endif
#if(USBD_MAX_NUM_ALTERNATE_INTERFACE_SETTINGS > 0)
   uint8_t interface_set_ongoing;
#endif
   uint8_t ep_setup_ongoing;
}port_stm32_cat_a_req_params_XT;

/** ******************************************************************************************************************************
 * USBD IO part types
 ********************************************************************************************************************************/

typedef struct port_stm32_cat_a_io_ep_eXtended_Tag
{
   struct
   {
      struct
      {
         void                         *data;
         USBD_IO_Inout_Data_Size_DT    size;
         USBD_IO_Inout_Data_Size_DT    packet_size;
      }buf;
#if(USBD_PORT_STM32_CAT_A_DMA_TYPE == PORT_STM32_CAT_A_DMA_HALF_DMA)
      struct
      {
         void                         *data;
         USBD_IO_Inout_Data_Size_DT    size;
         USBD_IO_Inout_Data_Size_DT    total_size;
      }dma;
#endif
      uint16_t                         mps;
      uint8_t                          ep_type;
      USBD_Bool_DT                     out_on_delay;
      USBD_Bool_DT                     dont_wait_for_ready;
      USBD_Bool_DT                     is_ep_processed;
      USBD_Bool_DT                     is_ep_waiting;
      struct
      {
         volatile uint8_t              ep_reg_num;
      }hw;
   }out;
   struct
   {
#if(USBD_PORT_STM32_CAT_A_DMA_TYPE == PORT_STM32_CAT_A_DMA_HALF_DMA)
      struct
      {
         const void                   *data;
         USBD_IO_Inout_Data_Size_DT    size;
      }dma;
#endif
      struct
      {
         uint16_t                      size;
      }buf0;
#if(USBD_MAX_NUM_ENDPOINTS > 1)
      struct
      {
         uint16_t                      size;
      }buf1;
#endif
      uint16_t                         mps;
      uint8_t                          ep_type;
      uint8_t                          max_num_bufs;
      uint8_t                          num_used_bufs;
      USBD_Bool_DT                     data_provided;
      struct
      {
         volatile uint8_t              ep_reg_num;
      }hw;
   }in;
} port_stm32_cat_a_io_ep_XT;

typedef void (*port_stm32_cat_a_io_disable_ep_HT)(uint8_t ep_reg_num, volatile uint32_t *ep_reg);

/** ******************************************************************************************************************************
 * USBD DEV part local functions declaration & variables
 ********************************************************************************************************************************/

static void                         port_stm32_cat_a_dev_activate_deactivate (USBD_Params_XT *usbd, USBD_Bool_DT state);
#if(USBD_FEATURE_PRESENT == USBD_DEV_SUPPORT_CONFIG_VALIDATION)
static USBD_Bool_DT                 port_stm32_cat_a_dev_parse_cfg           (USBD_Params_XT *usbd, const uint8_t *desc, uint16_t desc_size, USBD_DEV_Config_Desc_Check_Result_XT *details);
#endif
static uint16_t                     port_stm32_cat_a_dev_get_dev_status      (USBD_Params_XT *usbd);
static uint16_t                     port_stm32_cat_a_dev_get_frame_nr        (USBD_Params_XT *usbd);
static const USB_Endpoint_Desc_DT*  port_stm32_cat_a_dev_get_ep0_desc        (USBD_Params_XT *usbd);

static const USB_Endpoint_Desc_DT   port_stm32_cat_a_dev_ep0_full = USB_ENDPOINT_DESC_STRUCT_FILL(
   0 /* EP number */,
   USB_EP_DESC_DIR_INOUT_CONTROL,
   USB_EP_DESC_TRANSFER_TYPE_CONTROL,
   USB_EP_DESC_SYNC_TYPE_NOT_USED,
   USB_EP_DESC_USAGE_TYPE_NOT_USED,
   USBD_PORT_STM32_CAT_A_EP0_MPS,
   0xFF /* bInterval */);
static const USBD_DEV_Port_Handler_XT port_stm32_cat_a_dev =
{
   .handlers.activate               = port_stm32_cat_a_dev_activate_deactivate,
#if(USBD_FEATURE_PRESENT == USBD_DEV_SUPPORT_CONFIG_VALIDATION)
   .handlers.ep_parse               = USBD_MAKE_INVALID_HANDLER(USBD_DEV_PORT_Parse_EP_Desc_Variants_HT),
   .handlers.cfg_parse              = port_stm32_cat_a_dev_parse_cfg,
#endif
   .handlers.get_device_status      = port_stm32_cat_a_dev_get_dev_status,
   .handlers.get_frame_num          = port_stm32_cat_a_dev_get_frame_nr,
   .handlers.get_ep0_low_full_speed = port_stm32_cat_a_dev_get_ep0_desc,
   .data.ep_both_directions_must_have_same_type = USBD_TRUE
};
static USBD_Params_XT *port_stm32_cat_a_usbd = USBD_MAKE_INVALID_PTR(USBD_Params_XT);

#if(USBD_FEATURE_PRESENT == USBD_DEV_SUPPORT_CONFIG_VALIDATION)
static port_stm32_cat_a_dev_interface_shared_mem_start_point_DT port_stm32_cat_a_dev_interface_shared_mem_start_point[USBD_MAX_NUM_INTERFACES];
#endif

port_stm32_cat_a_dev_params_XT port_stm32_cat_a_dev_prams;
USBD_Atomic_Bool_DT port_stm32_cat_a_irq_active;


/** ******************************************************************************************************************************
 * USBD IO part local functions declaration & variables
 ********************************************************************************************************************************/

static void                         port_stm32_cat_a_io_load_data_to_buffer     (const uint8_t* data, uint32_t* buffer, USBD_IO_Inout_Data_Size_DT length, USBD_IO_Inout_Data_Size_DT offset);
static USBD_IO_Inout_Data_Size_DT   port_stm32_cat_a_io_data_in_process_bufs    (port_stm32_cat_a_io_ep_XT *ep, const void* data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);
static USBD_IO_Inout_Data_Size_DT   port_stm32_cat_a_io_process_data_out        (port_stm32_cat_a_io_ep_XT *ep, void* data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);
#if(USBD_PORT_STM32_CAT_A_DMA_TYPE == PORT_STM32_CAT_A_DMA_HALF_DMA)
static USBD_IO_Inout_Data_Size_DT   port_stm32_cat_a_io_in_provide              (USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const void *data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);
#endif
static USBD_IO_Inout_Data_Size_DT   port_stm32_cat_a_io_in_memcpy               (USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const void *data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);
#if(USBD_PORT_STM32_CAT_A_DMA_TYPE == PORT_STM32_CAT_A_DMA_HALF_DMA)
static USBD_IO_Inout_Data_Size_DT   port_stm32_cat_a_io_out_provide             (USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, void *data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);
#endif
static USBD_IO_Inout_Data_Size_DT   port_stm32_cat_a_io_out_memcpy              (USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, void *data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);
static USBD_Bool_DT                 port_stm32_cat_a_io_trigger                 (USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, USBD_Bool_DT dont_wait_for_ready);
static USBD_IO_Inout_Data_Size_DT   port_stm32_cat_a_io_get_ep_out_waiting_size (USBD_Params_XT *usbd, uint8_t ep_num, USBD_Bool_DT left_size);
static USBD_IO_Inout_Data_Size_DT   port_stm32_cat_a_io_get_ep_in_buffered_size (USBD_Params_XT *usbd, uint8_t ep_num);
static void                         port_stm32_cat_a_io_stall                   (USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir);
static void                         port_stm32_cat_a_io_abort                   (USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, USBD_Bool_DT flush_hw_bufs);
#if(USBD_FEATURE_PRESENT == USBD_EP_HALT_SUPPORTED)
static void                         port_stm32_cat_a_io_halt                    (USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const USB_Endpoint_Desc_DT *ep_desc, USBD_Bool_DT state);
#endif
#if(USBD_MAX_NUM_ENDPOINTS > 1)
static void                         port_stm32_cat_a_io_configure               (USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const USB_Endpoint_Desc_DT *ep_desc, USBD_Bool_DT state);
static void                         port_stm32_cat_a_io_disable_ep_ctrl_int_in  (uint8_t ep_reg_num, volatile uint32_t *ep_reg);
static void                         port_stm32_cat_a_io_disable_ep_ctrl_int_out (uint8_t ep_reg_num, volatile uint32_t *ep_reg);
static void                         port_stm32_cat_a_io_disable_ep_isochronous  (uint8_t ep_reg_num, volatile uint32_t *ep_reg);
static void                         port_stm32_cat_a_io_disable_ep_bulk_in      (uint8_t ep_reg_num, volatile uint32_t *ep_reg);
static void                         port_stm32_cat_a_io_disable_ep_bulk_out     (uint8_t ep_reg_num, volatile uint32_t *ep_reg);
#endif
static void                         port_stm32_cat_a_io_process_in              (uint8_t ep_num);
static void                         port_stm32_cat_a_io_process_out             (uint8_t ep_num);

static port_stm32_cat_a_io_ep_XT               port_stm32_cat_a_io_ep[USBD_MAX_NUM_ENDPOINTS];
static const USBD_IO_DOWN_Common_Handlers_XT   port_stm32_cat_a_io_common =
{
   .trigger                   = port_stm32_cat_a_io_trigger,
   .get_ep_out_waiting_size   = port_stm32_cat_a_io_get_ep_out_waiting_size,
   .get_ep_in_buffered_size   = port_stm32_cat_a_io_get_ep_in_buffered_size,
   .abort                     = port_stm32_cat_a_io_abort,
   .stall                     = port_stm32_cat_a_io_stall,
#if(USBD_FEATURE_PRESENT == USBD_EP_HALT_SUPPORTED)
   .halt                      = port_stm32_cat_a_io_halt,
#endif
#if(USBD_MAX_NUM_ENDPOINTS > 1)
   .configure                 = port_stm32_cat_a_io_configure
#endif
};

static uint8_t port_stm32_cat_a_io_ep_reg_num_ep[USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS];

#if(USBD_MAX_NUM_ENDPOINTS > 1)

static const port_stm32_cat_a_io_disable_ep_HT port_stm32_cat_a_io_disable_ep_in[4] =
{
   port_stm32_cat_a_io_disable_ep_ctrl_int_in,
   port_stm32_cat_a_io_disable_ep_isochronous,
   port_stm32_cat_a_io_disable_ep_bulk_in,
   port_stm32_cat_a_io_disable_ep_ctrl_int_in
};
static const port_stm32_cat_a_io_disable_ep_HT port_stm32_cat_a_io_disable_ep_out[4] =
{
   port_stm32_cat_a_io_disable_ep_ctrl_int_out,
   port_stm32_cat_a_io_disable_ep_isochronous,
   port_stm32_cat_a_io_disable_ep_bulk_out,
   port_stm32_cat_a_io_disable_ep_ctrl_int_out
};

#endif

#ifdef USBD_USE_IOCMD
static const char * const port_stm32_cat_a_io_ep_type_string[4]         = {"BULK", "CTRL", "ISO ", "INT "};
static const char * const port_stm32_cat_a_io_transfer_status_string[4] = {"DISAB", "STALL", "NAK  ", "VALID"};
#endif

/** ******************************************************************************************************************************
 * USBD REQUEST part local functions declaration & variables
 ********************************************************************************************************************************/

static void port_stm32_cat_a_req_process_incomming_setup(uint8_t ep_num, uint8_t ep_reg_num);

static USBD_Bool_DT port_stm32_cat_a_req_set_address       (USBD_Params_XT *usbd, uint8_t ep_num, USBD_REQUEST_Req_DT *req);
#if(USBD_FEATURE_PRESENT == USBD_DEV_SUPPORT_CONFIG_VALIDATION)
static USBD_Bool_DT port_stm32_cat_a_req_set_configuration (USBD_Params_XT *usbd, uint8_t ep_num, USBD_REQUEST_Req_DT *req);
#endif
#if(USBD_MAX_NUM_ALTERNATE_INTERFACE_SETTINGS > 0)
static USBD_Bool_DT port_stm32_cat_a_req_set_interface     (USBD_Params_XT *usbd, uint8_t ep_num, USBD_REQUEST_Req_DT *req);
#endif
static const USBD_REQUEST_Port_Callbacks_XT port_stm32_cat_a_req_handlers =
{
   .set_address = port_stm32_cat_a_req_set_address,
#if(USBD_FEATURE_PRESENT == USBD_DEV_SUPPORT_CONFIG_VALIDATION)
   .set_configuration = port_stm32_cat_a_req_set_configuration,
#endif
#if(USBD_MAX_NUM_ALTERNATE_INTERFACE_SETTINGS > 0)
   .set_interface = port_stm32_cat_a_req_set_interface,
#endif
};

static port_stm32_cat_a_req_params_XT port_stm32_cat_a_req_params;

/** ******************************************************************************************************************************
 * USBD DEV part local functions definition
 ********************************************************************************************************************************/

static void port_stm32_cat_a_dev_reset_internal_structures(void)
{
   port_stm32_cat_a_io_ep_XT *ep;
   uint8_t ep_num;

#if(USBD_FEATURE_PRESENT == USBD_DEV_SUPPORT_CONFIG_VALIDATION)
   port_stm32_cat_a_req_params.set_configuration_ongoing   = USBD_FALSE;
#endif
#if(USBD_MAX_NUM_ALTERNATE_INTERFACE_SETTINGS > 0)
   port_stm32_cat_a_req_params.interface_set_ongoing       = USBD_MAX_NUM_INTERFACES;
#endif
   memset(port_stm32_cat_a_io_ep, 0, sizeof(port_stm32_cat_a_io_ep));
#if(USBD_FEATURE_PRESENT == USBD_DEV_SUPPORT_CONFIG_VALIDATION)
   memset(port_stm32_cat_a_dev_interface_shared_mem_start_point, 0, sizeof(port_stm32_cat_a_dev_interface_shared_mem_start_point));
#endif
   memset(&port_stm32_cat_a_dev_prams, 0, sizeof(port_stm32_cat_a_dev_prams));
   port_stm32_cat_a_dev_prams.bulk_eps_num_bufs = 1;
   port_stm32_cat_a_dev_prams.out_on_delay = USBD_FALSE;

   for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
   {
      ep = &port_stm32_cat_a_io_ep[ep_num];

      ep->out.is_ep_waiting      = USBD_FALSE;
      ep->out.buf.size           = (-1);
      ep->out.buf.packet_size    = (-1);
      ep->out.buf.data           = USBD_MAKE_INVALID_PTR(uint8_t);

      ep->in.max_num_bufs        = 1;
      ep->in.data_provided       = USBD_FALSE;
   }

   for(ep_num = 0; ep_num < USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS; ep_num++)
   {
      port_stm32_cat_a_io_ep_reg_num_ep[ep_num] = USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS;
   }
} /* port_stm32_cat_a_dev_reset_internal_structures */

static void port_stm32_cat_a_dev_activate_deactivate(USBD_Params_XT* usbd, USBD_Bool_DT state)
{

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   if(USBD_BOOL_IS_TRUE(state))
   {
      USBD_IO_DOWN_Set_Common_Handlers(usbd, &port_stm32_cat_a_io_common);

      USBD_REQUEST_Install_Port_Callbacks(usbd, &port_stm32_cat_a_req_handlers);
      port_stm32_cat_a_dev_reset_internal_structures();

      port_stm32_cat_a_usbd = usbd;

#if (USBD_FEATURE_PRESENT == USBD_PORT_STM32_CAT_A_USE_PULL_UP)
      USBD_Port_STM32_CAT_A_Configure_Pull_Up(USBD_TRUE);
#endif
      USBD_Port_STM32_CAT_A_Configure_USBCLKSource();
      USBD_Port_STM32_CAT_A_Configure_USB_Irqs(USBD_TRUE);
#if (USBD_FEATURE_PRESENT == USBD_PORT_STM32_CAT_A_DETECT_VBUS_CHANGE)
      USBD_Port_STM32_CAT_A_Configure_Vbus_Detection_Irq(USBD_TRUE);
#endif

#if (USBD_FEATURE_PRESENT != USBD_PORT_STM32_CAT_A_DETECT_VBUS_CHANGE)
      USBD_DEV_Powered(usbd, USBD_TRUE);

      USBD_STM32_CAT_A_RESET_AND_ENABLE_PERIPH();

#if (USBD_FEATURE_PRESENT == USBD_PORT_STM32_CAT_A_USE_PULL_UP)
      USBD_Port_STM32_CAT_A_Pull_Up_Change(USBD_TRUE);
#endif
#endif
   }
   else
   {
      USBD_ENTER_CRITICAL();

#if (USBD_FEATURE_PRESENT == USBD_PORT_STM32_CAT_A_USE_PULL_UP)
      USBD_Port_STM32_CAT_A_Pull_Up_Change(USBD_FALSE);
#else
      USBD_ALERT(USBD_DBG_PORT_DEV,"no posibility to disconnect D+ resistor, host will not recognize device disconnection!");
#endif

      USBD_Port_STM32_CAT_A_Configure_USB_Irqs(USBD_FALSE);
#if (USBD_FEATURE_PRESENT == USBD_PORT_STM32_CAT_A_DETECT_VBUS_CHANGE)
      USBD_Port_STM32_CAT_A_Configure_Vbus_Detection_Irq(USBD_FALSE);
#endif

      USBD_STM32_CAT_A_DISABLE();

      port_stm32_cat_a_usbd = USBD_MAKE_INVALID_PTR(void);

#if (USBD_FEATURE_PRESENT == USBD_PORT_STM32_CAT_A_USE_PULL_UP)
      USBD_Port_STM32_CAT_A_Configure_Pull_Up(USBD_FALSE);
#endif

      USBD_EXIT_CRITICAL();
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
} /* port_stm32_cat_a_dev_activate_deactivate */

#if(USBD_FEATURE_PRESENT == USBD_DEV_SUPPORT_CONFIG_VALIDATION)
USBD_Bool_DT port_stm32_cat_a_does_estimation_fit_hw(port_stm32_cat_a_dev_if_estimation_DT *estimation)
{
   uint_fast8_t if_num;
   uint_fast16_t cntr = 0;
   USBD_Bool_DT result = USBD_TRUE;

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   for(if_num = 0; if_num < USBD_MAX_NUM_INTERFACES; if_num++)
   {
      cntr += estimation[if_num].num_buffers;
   }

   if(cntr > USBD_PORT_STM32_CAT_A_MAX_NUM_EP_BUFFERS)
   {
      result = USBD_FALSE;

      USBD_WARN_2(USBD_DBG_PORT_DEV, "parsing estimation failed because of %s: %d", "too many buffers", cntr);
   }

   if(USBD_BOOL_IS_TRUE(result))
   {
      cntr = 0;

      for(if_num = 0; if_num < USBD_MAX_NUM_INTERFACES; if_num++)
      {
         cntr += estimation[if_num].max_buffer_reservation;
      }

      if(cntr > USBD_PORT_STM32_CAT_A_EPS_SHARED_MEM_START_POINT)
      {
         result = USBD_FALSE;

         USBD_WARN_2(USBD_DBG_PORT_DEV, "parsing estimation failed because of %s: %d", "too big mem required", cntr);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return result;
} /* port_stm32_cat_a_does_estimation_fit_hw */

static USBD_Bool_DT port_stm32_cat_a_dev_parse_cfg_internal(
   port_stm32_cat_a_dev_if_estimation_DT *if_estimation,
   port_stm32_cat_a_dev_ep_estimation_DT *ep_estimation,
   const uint8_t *desc,
   uint16_t desc_size,
   uint_fast8_t num_buffers_per_bulk)
{
   USB_Interface_Desc_DT *if_ptr;
   USB_Endpoint_Desc_DT  *ep_ptr;
   size_t max_buffer_reservation;
   uint_fast16_t pos;
   uint_fast16_t pos_next_if;
   uint_fast16_t pos_ep;
   uint_fast16_t mps;
   uint_fast8_t ep_type;
   uint_fast8_t ep_num;
   uint_fast8_t buffers;
   uint_fast8_t num_buffers;
   USBD_Bool_DT result = USBD_TRUE;

#if(USBD_FEATURE_PRESENT != USBD_IO_BULK_TRANSFER_SUPPORTED)
   USBD_UNUSED_PARAM(num_buffers_per_bulk);
#endif

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   memset(if_estimation, 0, sizeof(port_stm32_cat_a_dev_if_estimation_DT) * USBD_MAX_NUM_INTERFACES);
   memset(ep_estimation, 0, sizeof(port_stm32_cat_a_dev_ep_estimation_DT) * USBD_MAX_NUM_ENDPOINTS);

   pos = USB_CDP_Find_Next_Interface(desc, desc_size, 0);

   /* parse all interfaces */
   while((0 != pos) && (pos < desc_size))
   {
      pos_next_if = USB_CDP_Find_Next_Interface(desc, desc_size, pos);

      if_ptr = (USB_Interface_Desc_DT*)(&desc[pos]);

      num_buffers = 0;
      max_buffer_reservation = 0;
      pos_ep = USB_CDP_Find_Next_Endpoint(desc, desc_size, pos);

      /* calculate max memory area and number of buffers used by interface */
      while((0 != pos_ep) && (pos_ep < pos_next_if) && (pos_ep < desc_size))
      {
         ep_ptr = (USB_Endpoint_Desc_DT*)(&desc[pos_ep]);
         ep_num = ep_ptr->bEndpointAddress & USB_EP_DESC_ADDR_MASK;

         if(ep_num > 0)
         {
            if(ep_num >= USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS)
            {
               result = USBD_FALSE;
               USBD_WARN_2(USBD_DBG_PORT_DEV, "parsing estimation failed because of %s: %d", "invalid endpoint number", ep_num);
               break;
            }
            mps      = (uint_fast16_t)(ep_ptr->wMaxPacketSize.H);
            mps    <<= 8;
            mps     += ep_ptr->wMaxPacketSize.L;
            ep_type  = ep_ptr->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK;

            if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == ep_type)
            {
               buffers = 2;
            }
#if(USBD_FEATURE_PRESENT == USBD_IO_ISOCHRONOUS_TRANSFER_SUPPORTED)
            else if(USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS == ep_type)
            {
               buffers = 2;
            }
#endif
#if(USBD_FEATURE_PRESENT == USBD_IO_BULK_TRANSFER_SUPPORTED)
            else if(USB_EP_DESC_TRANSFER_TYPE_BULK == ep_type)
            {
               buffers = num_buffers_per_bulk;
            }
#endif
            else
            {
               buffers = 1;
            }
            max_buffer_reservation += mps * buffers;
            num_buffers            += buffers;

            if(USB_EP_DESC_TRANSFER_TYPE_CONTROL == ep_type)
            {
               buffers = 1;
            }

            if(USB_EP_DESC_DIR_OUT != (ep_ptr->bEndpointAddress & USB_EP_DESC_DIR_MASK))
            {
               ep_estimation[ep_num].num_bufs.in = buffers;
            }
            else
            {
               ep_estimation[ep_num].num_bufs.out = buffers;
            }
         }

         pos_ep = USB_CDP_Find_Next_Endpoint(desc, desc_size, pos_ep);
      }

      if(USBD_BOOL_IS_FALSE(result))
      {
         break;
      }

      if(max_buffer_reservation > if_estimation[if_ptr->bInterfaceNumber].max_buffer_reservation)
      {
         if_estimation[if_ptr->bInterfaceNumber].max_buffer_reservation = max_buffer_reservation;
      }
      if(num_buffers > if_estimation[if_ptr->bInterfaceNumber].num_buffers)
      {
         if_estimation[if_ptr->bInterfaceNumber].num_buffers = num_buffers;
      }

      pos = pos_next_if;
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return result;
} /* port_stm32_cat_a_dev_parse_cfg_internal */

static USBD_Bool_DT port_stm32_cat_a_dev_parse_cfg(
   USBD_Params_XT* usbd,
   const uint8_t *desc,
   uint16_t desc_size,
   USBD_DEV_Config_Desc_Check_Result_XT *details)
{
   port_stm32_cat_a_dev_if_estimation_DT if_estimation[USBD_MAX_NUM_INTERFACES];
   port_stm32_cat_a_dev_ep_estimation_DT ep_estimation[USBD_MAX_NUM_ENDPOINTS];
   uint_fast8_t cntr;
   uint_fast8_t bulk_eps_num_bufs;
   uint8_t ep_reg_num;
   uint8_t num_ocupied_registers;
   USBD_Bool_DT result = USBD_TRUE;

   USBD_UNUSED_PARAM(details);

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   bulk_eps_num_bufs = 2;

   result = port_stm32_cat_a_dev_parse_cfg_internal(if_estimation, ep_estimation, desc, desc_size, bulk_eps_num_bufs);

   if(USBD_BOOL_IS_TRUE(result) && USBD_BOOL_IS_FALSE(port_stm32_cat_a_does_estimation_fit_hw(if_estimation)))
   {
      bulk_eps_num_bufs = 1;
      result = port_stm32_cat_a_dev_parse_cfg_internal(if_estimation, ep_estimation, desc, desc_size, bulk_eps_num_bufs);

      if(USBD_BOOL_IS_TRUE(result) && USBD_BOOL_IS_FALSE(port_stm32_cat_a_does_estimation_fit_hw(if_estimation)))
      {
         result = USBD_FALSE;
      }
   }

   if(USBD_BOOL_IS_TRUE(result))
   {
      if(USBD_COMPARE_PTRS(USBD_Params_XT, port_stm32_cat_a_usbd, USBD_Params_XT, usbd)
         && USBD_BOOL_IS_TRUE(port_stm32_cat_a_req_params.set_configuration_ongoing)
         )
      {
         port_stm32_cat_a_req_params.set_configuration_ongoing = USBD_FALSE;

         port_stm32_cat_a_dev_prams.bulk_eps_num_bufs = bulk_eps_num_bufs;

         port_stm32_cat_a_dev_interface_shared_mem_start_point[0].start_point =
            USBD_PORT_STM32_CAT_A_EPS_SHARED_MEM_ALOC_TAB_SIZE + (2 * USBD_PORT_STM32_CAT_A_EP0_MPS);
         port_stm32_cat_a_dev_interface_shared_mem_start_point[0].size = if_estimation[0].max_buffer_reservation;
         port_stm32_cat_a_dev_interface_shared_mem_start_point[0].current_offset = 0;
         for(cntr = 1; cntr < USBD_MAX_NUM_INTERFACES; cntr++)
         {
            port_stm32_cat_a_dev_interface_shared_mem_start_point[cntr].start_point =
               port_stm32_cat_a_dev_interface_shared_mem_start_point[cntr - 1].start_point
               + port_stm32_cat_a_dev_interface_shared_mem_start_point[cntr - 1].size;
            port_stm32_cat_a_dev_interface_shared_mem_start_point[cntr].size = if_estimation[cntr].max_buffer_reservation;
            port_stm32_cat_a_dev_interface_shared_mem_start_point[cntr].current_offset = 0;
         }

         port_stm32_cat_a_io_ep[0].in.hw.ep_reg_num  = 0;
         port_stm32_cat_a_io_ep[0].out.hw.ep_reg_num = 0;
         ep_reg_num = 1;

         for(cntr = 1; cntr < USBD_MAX_NUM_ENDPOINTS; cntr++)
         {
            num_ocupied_registers = 0;
            if(0 != ep_estimation[cntr].num_bufs.in)
            {
               port_stm32_cat_a_io_ep[cntr].in.hw.ep_reg_num = ep_reg_num;

               if(2 == ep_estimation[cntr].num_bufs.in)
               {
                  num_ocupied_registers++;
                  ep_reg_num++;
               }
            }
            else
            {
               port_stm32_cat_a_io_ep[cntr].in.hw.ep_reg_num = USBD_MAX_NUM_ENDPOINTS;
            }

            if(0 != ep_estimation[cntr].num_bufs.out)
            {
               port_stm32_cat_a_io_ep[cntr].out.hw.ep_reg_num = ep_reg_num;

               if(2 == ep_estimation[cntr].num_bufs.out)
               {
                  num_ocupied_registers++;
                  ep_reg_num++;
               }
            }
            else
            {
               port_stm32_cat_a_io_ep[cntr].out.hw.ep_reg_num = USBD_MAX_NUM_ENDPOINTS;
            }
            if((0 == num_ocupied_registers) && ((0 != ep_estimation[cntr].num_bufs.in) || (0 != ep_estimation[cntr].num_bufs.out)))
            {
               ep_reg_num++;
            }
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return result;
} /* port_stm32_cat_a_dev_parse_cfg */
#endif

static uint16_t port_stm32_cat_a_dev_get_dev_status(USBD_Params_XT *usbd)
{
   USBD_UNUSED_PARAM(usbd);

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return port_stm32_cat_a_dev_prams.device_status;
} /* port_stm32_cat_a_dev_get_dev_status */

static uint16_t port_stm32_cat_a_dev_get_frame_nr(USBD_Params_XT *usbd)
{
   uint16_t result;

   USBD_UNUSED_PARAM(usbd);

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   result = USBD_STM32_REG->FNR & 0x7FF;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return result;
} /* port_stm32_cat_a_dev_get_frame_nr */

static const USB_Endpoint_Desc_DT* port_stm32_cat_a_dev_get_ep0_desc(USBD_Params_XT* usbd)
{
   USBD_UNUSED_PARAM(usbd);

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return (&port_stm32_cat_a_dev_ep0_full);
} /* port_stm32_cat_a_dev_get_ep0_desc */



/** ******************************************************************************************************************************
 * USBD IO part local functions definition
 ********************************************************************************************************************************/

#ifdef USBD_USE_IOCMD
static void port_stm32_cat_a_io_print_ep_reg_state(const char *desc, uint32_t reg, uint16_t line, uint8_t ep_reg_num)
{
#ifndef USBD_USE_IOCMD
   USBD_UNUSED_PARAM(desc);
   USBD_UNUSED_PARAM(reg);
   USBD_UNUSED_PARAM(line);
   USBD_UNUSED_PARAM(ep_reg_num);
#endif

#ifdef IOCMD__LINE__LOCAL
#undef IOCMD__LINE__LOCAL
#endif
#define IOCMD__LINE__LOCAL      line

   USBD_DEBUG_MID_13(USBD_DBG_PORT_IO,
      "%s EP_REG[%d] = 0x%04X (RX:CTR=%d, DTOG=%d, STAT=%s - SETUP=%d, EP_TYPE=%s, EP_KIND=%d - TX:CTR=%d, DTOG=%d, STAT=%s - EA=%d)",
      desc,
      ep_reg_num,
      reg,
      USBD_STM32_CAT_A_REG_GET_1BIT(reg, USBDEP_STM32_CAT_A_BIT__CTR_RX),
      USBD_STM32_CAT_A_REG_GET_1BIT(reg, USBDEP_STM32_CAT_A_BIT__DTOG_RX),
      port_stm32_cat_a_io_transfer_status_string[USBD_STM32_CAT_A_REG_GET_2BIT(reg, USBDEP_STM32_CAT_A_BIT__STAT_RX)],
      USBD_STM32_CAT_A_REG_GET_1BIT(reg, USBDEP_STM32_CAT_A_BIT__SETUP),
      port_stm32_cat_a_io_ep_type_string[USBD_STM32_CAT_A_REG_GET_2BIT(reg, USBDEP_STM32_CAT_A_BIT__TYPE)],
      USBD_STM32_CAT_A_REG_GET_1BIT(reg, USBDEP_STM32_CAT_A_BIT__KIND),
      USBD_STM32_CAT_A_REG_GET_1BIT(reg, USBDEP_STM32_CAT_A_BIT__CTR_TX),
      USBD_STM32_CAT_A_REG_GET_1BIT(reg, USBDEP_STM32_CAT_A_BIT__DTOG_TX),
      port_stm32_cat_a_io_transfer_status_string[USBD_STM32_CAT_A_REG_GET_2BIT(reg, USBDEP_STM32_CAT_A_BIT__STAT_TX)],
      USBD_STM32_CAT_A_REG_GET_4BIT(reg, USBDEP_STM32_CAT_A_BIT__ADDR));

#undef IOCMD__LINE__LOCAL
#define IOCMD__LINE__LOCAL      IOCMD__LINE__
} /* port_stm32_cat_a_io_print_ep_reg_state */
#else
#define port_stm32_cat_a_io_print_ep_reg_state(desc, reg, line, ep_reg_num)
#endif

static void port_stm32_cat_a_io_load_data_to_buffer (
   const uint8_t* data, uint32_t* buffer, USBD_IO_Inout_Data_Size_DT length, USBD_IO_Inout_Data_Size_DT offset)
{
   uint8_t additional_byte;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   if(length > 0)
   {
      USBD_LOG_DATA_DEBUG_MID_1(USBD_DBG_PORT_IO, data, length,
         "data IN; size: %d", length);
      /**
       * buffer points always to begining of shared memory container. If we are in next call of "memcpy" method (previous was
       * with parameter "is_last_part" set to USBD_FALSE) then we continue adding to buffer.
       */
      buffer += offset >> 1;

      /* previously we added odd number of bytes so next shall be added to previous uint16_t location */
      if(0 != (offset & 0x1))
      {
         *buffer = (*buffer & 0xFF) | (((uint32_t)data[1]) << 8);
         length--;
         data++;
         buffer++;
      }

      /**
       * variable additional_byte says, if length is odd.
       * If yes then lasy byte shall be loaded to separated uint16_t word
       */
      additional_byte = (uint8_t)(length & 0x1);

      /* divide length by 2 - we are sending 2 bytes in single uint16_t word */
      length >>= 1;

      /* load data to buffer - 2 bytes per loop iteration */
      while (length)
      {
         *buffer = ((uint32_t)*data) | (((uint32_t)data[1]) << 8);
         data += 2;
         buffer++;
         length--;
      }
      /* load last single byte if length was odd */
      if(additional_byte)
      {
         *buffer = (uint32_t)*data;
         data++;
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);
} /* port_stm32_cat_a_io_load_data_to_buffer */

#if(USBD_MAX_NUM_ENDPOINTS > 1)
static inline void port_stm32_cat_a_io_force_valid_in_stat(port_stm32_cat_a_io_ep_XT *ep)
{
   volatile uint32_t temp;
#ifdef USBD_USE_IOCMD
   volatile uint32_t temp_before;
   volatile uint32_t temp_after;
#endif

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   /* allow for sending already prepared buffer */
   while(USBDEP_STM32_CAT_A__TX_NAK == (
      USBD_STM32_REG->EP_REG[ep->in.hw.ep_reg_num] & (USBDEP_STM32_CAT_A__STAT_TX | USBDEP_STM32_CAT_A__CTR_TX)))
   {
      USBD_DEBUG_HI_1(USBD_DBG_PORT_IO, "trigger TX EP 2 register again", ep->in.hw.ep_reg_num);
      /* modify EP control register to send the data */
      USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
         ep->in.hw.ep_reg_num,
         USBD_STM32_REG->EP_REG[ep->in.hw.ep_reg_num],
         temp,
         temp_before,
         temp_after,
         /*_EX_OR*/USBDEP_STM32_CAT_A__TX_VALID,
         /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE | USBDEP_STM32_CAT_A__STAT_TX,
         /*_OR   */USBDEP_STM32_CAT_A__CTR_RX);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);
} /* port_stm32_cat_a_io_force_valid_in_stat */
#endif

static USBD_IO_Inout_Data_Size_DT port_stm32_cat_a_io_data_in_process_bufs(
   port_stm32_cat_a_io_ep_XT *ep,
   const void* data,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_Bool_DT is_last_part)
{
   USBD_IO_Inout_Data_Size_DT  part;
   USBD_IO_Inout_Data_Size_DT  result = 0;
   volatile uint32_t temp;
#ifdef USBD_USE_IOCMD
   volatile uint32_t temp_before;
   volatile uint32_t temp_after;
#endif

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   ep->in.data_provided = USBD_TRUE;

#if(USBD_MAX_NUM_ENDPOINTS > 1)
   if(2 == ep->in.max_num_bufs)
   {
      USBD_DEBUG_HI_1(USBD_DBG_PORT_IO, "2 bufs; used bufs: %d", ep->in.num_used_bufs);

      if(0 == ep->in.num_used_bufs)
      {
         /* calculate amount of data to be loaded to buffer */
         part = (size > (USBD_IO_Inout_Data_Size_DT)(ep->in.mps - ep->in.buf0.size)) ?
            (USBD_IO_Inout_Data_Size_DT)(ep->in.mps - ep->in.buf0.size) : size;

         /* load data to buffer */
         port_stm32_cat_a_io_load_data_to_buffer (
            data, (uint32_t*)USBDEP_DB_GET_TX_BUFFER_ADDR(ep->in.hw.ep_reg_num), part, ep->in.buf0.size);

         result            = part;
         ep->in.buf0.size += (uint16_t)part;

         if(USBD_BOOL_IS_TRUE(is_last_part) || (size >= ((USBD_IO_Inout_Data_Size_DT)(ep->in.mps))))
         {
            ep->in.num_used_bufs = 1;
            USBDEP_DB_SET_TX_COUNT(ep->in.hw.ep_reg_num, ep->in.buf0.size);

            /* modify EP control register to send the data */
            USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
               ep->in.hw.ep_reg_num,
               USBD_STM32_REG->EP_REG[ep->in.hw.ep_reg_num],
               temp,
               temp_before,
               temp_after,
               /*_EX_OR*/USBDEP_STM32_CAT_A__TX_VALID | USBDEP_STM32_CAT_A__DTOG_RX,
               /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE | USBDEP_STM32_CAT_A__STAT_TX | USBDEP_STM32_CAT_A__DTOG_RX,
               /*_OR   */USBDEP_STM32_CAT_A__DTOG_RX);

            port_stm32_cat_a_io_force_valid_in_stat(ep);
         }

         if(size > part)
         {
            size -= part;
            data  = &((uint8_t*)data)[part];

            /* calculate amount of data to be loaded to buffer */
            part = (size > (USBD_IO_Inout_Data_Size_DT)(ep->in.mps - ep->in.buf1.size)) ?
               (USBD_IO_Inout_Data_Size_DT)(ep->in.mps - ep->in.buf1.size) : size;

            /* load data to buffer */
            port_stm32_cat_a_io_load_data_to_buffer (
               data, (uint32_t*)USBDEP_DB_GET_TX_BUFFER_ADDR(ep->in.hw.ep_reg_num), part, ep->in.buf1.size);

            result            += part;
            ep->in.buf1.size  += (uint16_t)part;

            if(USBD_BOOL_IS_TRUE(is_last_part) || (size >= ((USBD_IO_Inout_Data_Size_DT)(ep->in.mps))))
            {
               ep->in.num_used_bufs = 2;
               USBDEP_DB_SET_TX_COUNT(ep->in.hw.ep_reg_num, ep->in.buf1.size);
            }
         }
      }
      else if(1 == ep->in.num_used_bufs)
      {
         /* calculate amount of data to be loaded to buffer */
         part = (size > (USBD_IO_Inout_Data_Size_DT)(ep->in.mps - ep->in.buf1.size)) ?
            (USBD_IO_Inout_Data_Size_DT)(ep->in.mps - ep->in.buf1.size) : size;

         /* load data to buffer */
         port_stm32_cat_a_io_load_data_to_buffer (
            data, (uint32_t*)USBDEP_DB_GET_TX_BUFFER_ADDR(ep->in.hw.ep_reg_num), part, ep->in.buf1.size);

         result            = part;
         ep->in.buf1.size += (uint16_t)part;

         if(USBD_BOOL_IS_TRUE(is_last_part) || (size >= ((USBD_IO_Inout_Data_Size_DT)(ep->in.mps))))
         {
            ep->in.num_used_bufs = 2;
            USBDEP_DB_SET_TX_COUNT(ep->in.hw.ep_reg_num, ep->in.buf1.size);
         }
      }
   } /* (2 == ep->in.max_num_bufs) */
   else if(0 == ep->in.num_used_bufs)
#endif
   {
      USBD_DEBUG_HI_1(USBD_DBG_PORT_IO, "1 buf; used bufs: %d", ep->in.num_used_bufs);

      /* calculate amount of data to be loaded to buffer */
      part = (size > (USBD_IO_Inout_Data_Size_DT)(ep->in.mps - ep->in.buf0.size)) ?
         (USBD_IO_Inout_Data_Size_DT)(ep->in.mps - ep->in.buf0.size) : size;

      /* load data to buffer */
      port_stm32_cat_a_io_load_data_to_buffer (
         data, (uint32_t*)USBDEP_SB_GET_TX_BUFFER_ADDR(ep->in.hw.ep_reg_num), part, ep->in.buf0.size);

      result            = part;
      ep->in.buf0.size += (uint16_t)part;

      if(USBD_BOOL_IS_TRUE(is_last_part) || (size >= ((USBD_IO_Inout_Data_Size_DT)(ep->in.mps))))
      {
         ep->in.num_used_bufs = 1;
         USBDEP_SB_SET_TX_COUNT(ep->in.hw.ep_reg_num, ep->in.buf0.size);

         /* modify EP control register to send the data */
         USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
            ep->in.hw.ep_reg_num,
            USBD_STM32_REG->EP_REG[ep->in.hw.ep_reg_num],
            temp,
            temp_before,
            temp_after,
            /*_EX_OR*/USBDEP_STM32_CAT_A__TX_VALID,
            /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE | USBDEP_STM32_CAT_A__STAT_TX,
            /*_OR   */USBDEP_STM32_CAT_A__CTR_RX);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return result;
} /* port_stm32_cat_a_io_data_in_process_bufs */

static void port_stm32_cat_a_io_process_in(uint8_t ep_num)
{
   port_stm32_cat_a_io_ep_XT *ep;
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction;
   void *tp_params;
   volatile uint32_t temp;
#ifdef USBD_USE_IOCMD
   volatile uint32_t temp_before;
   volatile uint32_t temp_after;
#endif
   USBD_IO_Inout_Data_Size_DT size;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   if(USBD_CHECK_PTR(void, port_stm32_cat_a_usbd) && (ep_num < USBD_MAX_NUM_ENDPOINTS))
   {
      ep = &port_stm32_cat_a_io_ep[ep_num];
      transaction = USBD_IO_GET_IN_TRANSACATION_PARAMS(port_stm32_cat_a_usbd, ep_num);
      tp_params   = USBD_IO_GET_IN_TP_PARAMS_PTR(port_stm32_cat_a_usbd, ep_num);

      ep->in.data_provided = USBD_FALSE;

      if(ep->in.num_used_bufs > 0)
      {
         (ep->in.num_used_bufs)--;
      }
#if(USBD_MAX_NUM_ENDPOINTS > 1)
      ep->in.buf0.size      = ep->in.buf1.size;
      ep->in.buf1.size      = 0;
#else
      ep->in.buf0.size      = 0;
#endif

#if(USBD_FEATURE_PRESENT == USBD_IO_BULK_TRANSFER_SUPPORTED)
      /* double buffered BULK EP - there is a second data buffer prepared in previous IRQ; allow for sending */
      if((ep->in.num_used_bufs > 0) && (USB_EP_DESC_TRANSFER_TYPE_BULK == ep->in.ep_type))
      {
         /* clear TX IRQ flag and allow for sending already prepared buffer */
         USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
            ep->in.hw.ep_reg_num,
            USBD_STM32_REG->EP_REG[ep->in.hw.ep_reg_num],
            temp,
            temp_before,
            temp_after,
            /*_EX_OR*/USBDEP_STM32_CAT_A__TX_VALID,
            /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE | USBDEP_STM32_CAT_A__STAT_TX,
            /*_OR   */USBDEP_STM32_CAT_A__DTOG_RX);

         /* don't ask why register is being modified twice - I also don't understand it
          * but for some reason this processor needs 2 REG modifications to set it from NAK to VALID
          */
         port_stm32_cat_a_io_force_valid_in_stat(ep);
      }
      else
#endif
      {
         /* clear only TX IRQ flag */
         USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
            ep->in.hw.ep_reg_num,
            USBD_STM32_REG->EP_REG[ep->in.hw.ep_reg_num],
            temp,
            temp_before,
            temp_after,
            /*_EX_OR*/((ep->in.max_num_bufs > 1) ? USBDEP_STM32_CAT_A__TX_VALID : USBDEP_STM32_CAT_A__TX_NAK),
            /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE | USBDEP_STM32_CAT_A__STAT_TX,
            /*_OR   */USBDEP_STM32_CAT_A__CTR_RX);
      }

#if(USBD_PORT_STM32_CAT_A_DMA_TYPE == PORT_STM32_CAT_A_DMA_HALF_DMA)
      USBD_INFO_LO_2(USBD_DBG_PORT_IO, "irq: in: half_dma; num_used_bufs: %d; dma_size: %d", ep->in.num_used_bufs, ep->in.dma.size);

      if(ep->in.dma.size > 0)
      {
         size = port_stm32_cat_a_io_data_in_process_bufs(ep, ep->in.dma.data, ep->in.dma.size, USBD_TRUE);

         ep->in.dma.data  = &(((uint8_t*)(ep->in.dma.data))[size]);
         ep->in.dma.size -= size;
      }

      if(0 == ep->in.dma.size)
      {
         if((ep->in.num_used_bufs < ep->in.max_num_bufs) && USBD_IO_CHECK_IN_PROVIDE_HANDLER(transaction))
         {
            USBD_DEBUG_HI(USBD_DBG_PORT_IO, "provide IN");
            USBD_IO_CALL_IN_PROVIDE_HANDLER(port_stm32_cat_a_usbd, tp_params, transaction, port_stm32_cat_a_io_in_provide);
         }
         if((ep->in.num_used_bufs < ep->in.max_num_bufs) && USBD_IO_CHECK_IN_MEMCPY_HANDLER(transaction))
         {
            USBD_DEBUG_HI(USBD_DBG_PORT_IO, "memcpy IN");
            USBD_IO_CALL_IN_MEMCPY_HANDLER(port_stm32_cat_a_usbd, tp_params, transaction, port_stm32_cat_a_io_in_memcpy);
         }
         if(USBD_BOOL_IS_FALSE(ep->in.data_provided))
         {
            if(ep->in.num_used_bufs > 0)
            {
               size = (USBD_IO_Inout_Data_Size_DT)(ep->in.buf0.size) + (USBD_IO_Inout_Data_Size_DT)(ep->in.buf1.size);
            }
            else
            {
               size = (-1);
            }
            USBD_DEBUG_HI_1(USBD_DBG_PORT_IO, "data_event with size: %d", size);
            USBD_IO_DOWN_Process_IN_Data_Event(port_stm32_cat_a_usbd, ep_num, size, port_stm32_cat_a_io_in_memcpy);
         }
      }
#else
      if(USBD_IO_CHECK_IN_MEMCPY_HANDLER(transaction))
      {
         USBD_DEBUG_HI(USBD_DBG_PORT_IO, "memcpy IN");
         USBD_IO_CALL_IN_MEMCPY_HANDLER(port_stm32_cat_a_usbd, tp_params, transaction, port_stm32_cat_a_io_in_memcpy);
      }
      if(USBD_BOOL_IS_FALSE(ep->in.data_provided))
      {
         if(ep->in.num_used_bufs > 0)
         {
#if(USBD_MAX_NUM_ENDPOINTS > 1)
            size = (USBD_IO_Inout_Data_Size_DT)(ep->in.buf0.size) + (USBD_IO_Inout_Data_Size_DT)(ep->in.buf1.size);
#else
            size = (USBD_IO_Inout_Data_Size_DT)(ep->in.buf0.size);
#endif
         }
         else
         {
            size = (-1);
         }
         USBD_DEBUG_HI_1(USBD_DBG_PORT_IO, "data_event with size: %d", size);
         USBD_IO_DOWN_Process_IN_Data_Event(port_stm32_cat_a_usbd, ep_num, size, port_stm32_cat_a_io_in_memcpy);
      }
#endif
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "port test proc in - params invalid! ep_num = %d, port_stm32_cat_a_usbd = %p",
         ep_num, port_stm32_cat_a_usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);
} /* port_stm32_cat_a_io_process_in */




static void port_stm32_cat_a_load_data_from_buffer (
   uint8_t* data, uint32_t* buffer, USBD_IO_Inout_Data_Size_DT length, USBD_IO_Inout_Data_Size_DT offset)
{
#ifdef USBD_USE_IOCMD
   uint8_t *odata = data;
   USBD_IO_Inout_Data_Size_DT olength = length;
#endif
   uint8_t additional_byte;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   USBD_DEBUG_MID_4(USBD_DBG_PORT_IO, "OUT data: %p, buffer: %p; length: %d; offset: %d", data, buffer, length, offset);

   if(length > 0)
   {
      /**
       * buffer points always to the begining of the shared memory container. If we are in the next call of the "memcpy" method
       * (previous was with parameter "is_last_part" set to USBD_FALSE) then we continue reading from the buffer.
       */
      buffer += offset >> 1;

      /* previously we added odd number of bytes so next shall be added to previous uint16_t location */
      if(0 != (offset & 0x1))
      {
         *data = ((uint8_t*)buffer)[1];
         length--;
         data++;
         buffer++;
      }

      /**
       * variable additional_byte says, if length is odd.
       * If yes then last byte shall be loaded to separated uint16_t word
       */
      additional_byte = (uint8_t)(length & 0x1);

      /* divide length by 2 - we are reading 2 bytes in a single uint16_t word */
      length >>= 1;

      /* load data from buffer - 2 bytes per loop iteration */
      while (length)
      {
         *data = *((uint8_t*)buffer);
         data++;
         *data = ((uint8_t*)buffer)[1];
         data++;
         buffer++;
         length--;
      }

      /* load last single byte if length was odd */
      if(additional_byte)
      {
         *data = *((uint8_t*)buffer);
      }

      USBD_LOG_DATA_DEBUG_MID_1(USBD_DBG_PORT_IO, odata, olength,
         "data OUT; size: %d", olength);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);
} /* port_stm32_cat_a_load_data_from_buffer */

void port_stm32_cat_a_release_out_buffer(port_stm32_cat_a_io_ep_XT *ep)
{
   uint32_t temp;
#ifdef USBD_USE_IOCMD
   uint32_t temp_before;
   uint32_t temp_after;
#endif

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

#if(USBD_FEATURE_PRESENT == USBD_IO_BULK_TRANSFER_SUPPORTED)
   if((2 == port_stm32_cat_a_dev_prams.bulk_eps_num_bufs) && (USB_EP_DESC_TRANSFER_TYPE_BULK == ep->out.ep_type))
   {
      /* modify EP control register to allow receiving next data packets */
      USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
         ep->out.hw.ep_reg_num,
         USBD_STM32_REG->EP_REG[ep->out.hw.ep_reg_num],
         temp,
         temp_before,
         temp_after,
         /*_EX_OR*/USBDEP_STM32_CAT_A__RX_VALID,
         /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE | USBDEP_STM32_CAT_A__STAT_RX,
         /*_OR   */USBDEP_STM32_CAT_A__CTR_RX /* there could just came next OUT packet - don't clear new IRQ trigger */);
   }
   else
#endif
   {
      /* modify EP control register to allow receiving next data packets */
      USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
         ep->out.hw.ep_reg_num,
         USBD_STM32_REG->EP_REG[ep->out.hw.ep_reg_num],
         temp,
         temp_before,
         temp_after,
         /*_EX_OR*/USBDEP_STM32_CAT_A__RX_VALID,
         /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE | USBDEP_STM32_CAT_A__STAT_RX,
         /*_OR   */USBDEP_STM32_CAT_A__CTR_TX);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);
} /* port_stm32_cat_a_release_out_buffer */

void port_stm32_cat_a_read_out_packet_params(port_stm32_cat_a_io_ep_XT *ep)
{
   uint32_t temp;
#ifdef USBD_USE_IOCMD
   uint32_t temp_before;
   uint32_t temp_after;
#endif

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

#if(USBD_FEATURE_PRESENT == USBD_IO_BULK_TRANSFER_SUPPORTED)
   if((2 == port_stm32_cat_a_dev_prams.bulk_eps_num_bufs) && (USB_EP_DESC_TRANSFER_TYPE_BULK == ep->out.ep_type))
   {
      /* modify EP control register to clear IRQ flag and allow receiving next packet */
      USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
         ep->out.hw.ep_reg_num,
         USBD_STM32_REG->EP_REG[ep->out.hw.ep_reg_num],
         temp,
         temp_before,
         temp_after,
         /*_EX_OR*/USBDEP_STM32_CAT_A__RX_VALID,
         /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE | USBDEP_STM32_CAT_A__STAT_RX,
         /*_OR   */USBDEP_STM32_CAT_A__DTOG_TX);
      ep->out.buf.size        = USBDEP_DB_GET_RX_COUNT(ep->out.hw.ep_reg_num);
      ep->out.buf.data        = (void*)USBDEP_DB_GET_RX_BUFFER_ADDR(ep->out.hw.ep_reg_num);
   }
   else
#endif
   {
#if(USBD_FEATURE_PRESENT == USBD_IO_ISOCHRONOUS_TRANSFER_SUPPORTED)
      if(USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS == ep->out.ep_type)
      {
         ep->out.buf.size        = USBDEP_ISO_GET_RX_COUNT(ep->out.hw.ep_reg_num);
         ep->out.buf.data        = (void*)USBDEP_ISO_GET_RX_BUFFER_ADDR(ep->out.hw.ep_reg_num);
      }
      else
#endif
      {
         ep->out.buf.size        = USBDEP_SB_GET_RX_COUNT(ep->out.hw.ep_reg_num);
         ep->out.buf.data        = (void*)USBDEP_SB_GET_RX_BUFFER_ADDR(ep->out.hw.ep_reg_num);
      }

      /* modify EP control register to clear OUT IRQ flag */
      USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
         ep->out.hw.ep_reg_num,
         USBD_STM32_REG->EP_REG[ep->out.hw.ep_reg_num],
         temp,
         temp_before,
         temp_after,
         /*_EX_OR*/0,
         /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE,
         /*_OR   */USBDEP_STM32_CAT_A__CTR_TX);
   }

   ep->out.buf.packet_size    = ep->out.buf.size;

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);
} /* port_stm32_cat_a_read_out_packet_params */

static USBD_IO_Inout_Data_Size_DT port_stm32_cat_a_io_process_data_out(
   port_stm32_cat_a_io_ep_XT *ep,
   void* data,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_Bool_DT is_last_part)
{
#ifdef USBD_USE_IOCMD
   uint8_t ep_num = (ep - port_stm32_cat_a_io_ep) / sizeof(port_stm32_cat_a_io_ep_XT);
#endif
   USBD_IO_Inout_Data_Size_DT buff_size;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   if(ep->out.buf.size >= 0)
   {
      buff_size = ep->out.buf.size;
      /* protection and setting ep->out.buf.size to -1 in case when whole data is read */
      if(ep->out.buf.size <= size)
      {
         size = ep->out.buf.size;

         ep->out.buf.size -= 1;
      }

      if(USBD_CHECK_PTR(void, ep->out.buf.data))
      {
         port_stm32_cat_a_load_data_from_buffer(data, ep->out.buf.data, size, ep->out.buf.packet_size - buff_size);
      }
      else
      {
         memset(data, 0, size);
         USBD_DEBUG_HI_2(USBD_DBG_PORT_IO, "EP: %d; buf data pointer invalid, memset %d bytes to 0", ep_num, size);
      }

      ep->out.buf.size -= size;
   }

   ep->out.is_ep_processed = USBD_TRUE;

   if(USBD_BOOL_IS_TRUE(is_last_part))
   {
      ep->out.buf.data      = USBD_MAKE_INVALID_PTR(void);
      ep->out.is_ep_waiting = USBD_FALSE;

      /* set registers to allow receiving next packets */
      port_stm32_cat_a_release_out_buffer(ep);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return size;
}/* port_stm32_cat_a_io_process_data_out */

static void port_stm32_cat_a_io_process_out(uint8_t ep_num)
{
   port_stm32_cat_a_io_ep_XT *ep;
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction;
   void *tp_params;
   uint32_t temp;
#ifdef USBD_USE_IOCMD
   uint32_t temp_before;
   uint32_t temp_after;
#endif
#if(USBD_PORT_STM32_CAT_A_DMA_TYPE == PORT_STM32_CAT_A_DMA_HALF_DMA)
   USBD_IO_Inout_Data_Size_DT size;
#endif

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   if(USBD_CHECK_PTR(void, port_stm32_cat_a_usbd) && (ep_num < USBD_MAX_NUM_ENDPOINTS))
   {
      ep = &port_stm32_cat_a_io_ep[ep_num];
      transaction = USBD_IO_GET_OUT_TRANSACATION_PARAMS(port_stm32_cat_a_usbd, ep_num);
      tp_params   = USBD_IO_GET_OUT_TP_PARAMS_PTR(port_stm32_cat_a_usbd, ep_num);

      /* control transfer, SETUP packet (zero-length packet) has been received; abort IN transfer and finish processing OUT packet */
      if((USB_EP_DESC_TRANSFER_TYPE_CONTROL == ep->out.ep_type)
         && (0 != (USBD_STM32_REG->EP_REG[ep->out.hw.ep_reg_num] & USBDEP_STM32_CAT_A__KIND)))
      {
         USBD_DEBUG_HI_1(USBD_DBG_PORT_IO, "OUT packet on CONTROL IN transfer; abort IN %s",
            ((ep->in.num_used_bufs > 0) || (ep->in.dma.size > 0)) ? "earlier" : "");

         /* modify EP control register to allow receiving next data packets */
         USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
            ep->out.hw.ep_reg_num,
            USBD_STM32_REG->EP_REG[ep->out.hw.ep_reg_num],
            temp,
            temp_before,
            temp_after,
            /*_EX_OR*/USBDEP_STM32_CAT_A__RX_VALID,
            /*_AND  */USBDEP_STM32_CAT_A__ADDR_TYPE | USBDEP_STM32_CAT_A__STAT_RX,
            /*_OR   */0);

         USBD_IO_Abort(port_stm32_cat_a_usbd, ep_num, USB_EP_DIRECTION_IN, USBD_TRUE);
      }
      else if(USBD_BOOL_IS_FALSE(ep->out.is_ep_waiting))
      {
#if(USBD_PORT_STM32_CAT_A_DMA_TYPE == PORT_STM32_CAT_A_DMA_HALF_DMA)
         /* reads parameters of received OUT packet */
         port_stm32_cat_a_read_out_packet_params(ep);

         /**
          * set two markers:
          * - is_ep_processed is used to recognize if any action was performed on received data
          *   (if not then packet is terminated by this function)
          * - is_ep_waiting is used to recognize if buffer has been released by upper layer
          *   of if port must wait for further actions before allowing for next packets reception
          */
         ep->out.is_ep_processed = USBD_FALSE;
         ep->out.is_ep_waiting   = USBD_TRUE;

         if(ep->out.dma.size > 0)
         {
            if(ep->out.buf.size >= ((USBD_IO_Inout_Data_Size_DT)(ep->out.mps) & 0xFFFF))
            {
               size = port_stm32_cat_a_io_process_data_out(ep, ep->out.dma.data, ep->out.dma.size, USBD_TRUE);

               ep->out.dma.size -= size;
               ep->out.dma.data  = &((uint8_t*)(ep->out.dma.data))[size];
            }
            else
            {
               size = port_stm32_cat_a_io_process_data_out(ep, ep->out.dma.data, ep->out.dma.size, ep->out.dont_wait_for_ready);

               ep->out.dma.size -= size;

               ep->out.dma.total_size -= ep->out.dma.size;
               ep->out.dma.size        = 0;
            }

            if(0 == ep->out.dma.size)
            {
               USBD_IO_SET_OUT_TRANSFERRED_SIZE(transaction, ep->out.dma.total_size);

               ep->out.dma.total_size  = 0;

               USBD_DEBUG_HI_1(USBD_DBG_PORT_IO, "data_event with size: %d", 0);

               /**
                * call with last parameter USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_Port_HT)
                * instead of port_stm32_cat_a_io_out_memcpy because DMA transfer has been finished but nothing more - just
                * declared space was filled by data. there is no data waiting in EP buffer so upper layer
                * cannot perform memcpy operation.
                */
               USBD_IO_DOWN_Process_OUT_Data_Event(
                  port_stm32_cat_a_usbd, ep_num, (-1), USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_Port_HT));
            }
         }
         else
         {
            ep->out.dma.total_size  = 0;
            ep->out.dma.size        = 0;

            if(USBD_IO_CHECK_OUT_MEMCPY_HANDLER(transaction))
            {
               USBD_DEBUG_HI_1(USBD_DBG_PORT_IO, "memcpy OUT with size: %d", ep->out.buf.size);
               USBD_IO_CALL_OUT_MEMCPY_HANDLER(
                  port_stm32_cat_a_usbd,
                  tp_params,
                  transaction,
                  ep->out.buf.packet_size,
                  ep->out.buf.size,
                  port_stm32_cat_a_io_out_memcpy);
            }
            else
            {
               USBD_DEBUG_HI_1(USBD_DBG_PORT_IO, "data_event with size: %d", ep->out.buf.size);
               USBD_IO_DOWN_Process_OUT_Data_Event(port_stm32_cat_a_usbd, ep_num, ep->out.buf.size, port_stm32_cat_a_io_out_memcpy);
            }
         }

         if(USBD_BOOL_IS_FALSE(ep->out.is_ep_processed))
         {
            USBD_DEBUG_HI_1(USBD_DBG_PORT_IO, "abort OUT packet on EP %d", ep_num);

            ep->out.is_ep_processed = USBD_TRUE;

            ep->out.buf.data        = USBD_MAKE_INVALID_PTR(void);
            ep->out.is_ep_waiting   = USBD_FALSE;

            /* set registers to allow receiving next packets */
            port_stm32_cat_a_release_out_buffer(ep);
         }
#else
         /* reads parameters of received OUT packet */
         port_stm32_cat_a_read_out_packet_params(ep);

         /**
          * set two markers:
          * - is_ep_processed is used to recognize if any action was performed on received data
          *   (if not then packet is terminated by this function)
          * - is_ep_waiting is used to recognize if buffer has been released by upper layer
          *   of if port must wait for further actions before allowing for next packets reception
          */
         ep->out.is_ep_processed = USBD_FALSE;
         ep->out.is_ep_waiting   = USBD_TRUE;

         /* update size transferred by provide method (to by used by upper layer) */
         USBD_IO_SET_OUT_TRANSFERRED_SIZE(transaction, 0);

         if(USBD_IO_CHECK_OUT_MEMCPY_HANDLER(transaction))
         {
            USBD_DEBUG_HI_1(USBD_DBG_PORT_IO, "memcpy OUT with size: %d", ep->out.buf.size);
            USBD_IO_CALL_OUT_MEMCPY_HANDLER(
               port_stm32_cat_a_usbd,
               tp_params,
               transaction,
               ep->out.buf.packet_size,
               ep->out.buf.size,
               port_stm32_cat_a_io_out_memcpy);
         }
         else
         {
            USBD_DEBUG_HI_1(USBD_DBG_PORT_IO, "data_event with size: %d", ep->out.buf.size);
            USBD_IO_DOWN_Process_OUT_Data_Event(port_stm32_cat_a_usbd, ep_num, ep->out.buf.size, port_stm32_cat_a_io_out_memcpy);
         }

         if(USBD_BOOL_IS_FALSE(ep->out.is_ep_processed))
         {
            USBD_DEBUG_HI_1(USBD_DBG_PORT_IO, "abort OUT packet on EP %d", ep_num);

            ep->out.is_ep_processed = USBD_TRUE;

            ep->out.buf.data     = USBD_MAKE_INVALID_PTR(void);
            ep->out.is_ep_waiting = USBD_FALSE;

            /* set registers to allow receiving next packets */
            port_stm32_cat_a_release_out_buffer(ep);
         }
#endif
      }
      else
      {
         USBD_NOTICE_1(USBD_DBG_PORT_IRQ, "EP_REG[%d] received data but EP is waiting", ep->out.hw.ep_reg_num);

         /* clear RX IRQ flag */
         USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
            ep->out.hw.ep_reg_num,
            USBD_STM32_REG->EP_REG[ep->out.hw.ep_reg_num],
            temp,
            temp_before,
            temp_after,
            /*_EX_OR*/0,
            /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE,
            /*_OR   */USBDEP_STM32_CAT_A__CTR_TX);

         ep->out.out_on_delay = USBD_TRUE;
         port_stm32_cat_a_dev_prams.out_on_delay = USBD_TRUE;
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "port test proc out - params invalid! ep_num = %d, port_stm32_cat_a_usbd = %p",
         ep_num, port_stm32_cat_a_usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);
} /* port_stm32_cat_a_io_process_out */






#if(USBD_PORT_STM32_CAT_A_DMA_TYPE == PORT_STM32_CAT_A_DMA_HALF_DMA)

static USBD_IO_Inout_Data_Size_DT port_stm32_cat_a_io_in_provide(
   USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const void *data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part)
{
   port_stm32_cat_a_io_ep_XT *ep;
   USBD_IO_Inout_Data_Size_DT result = 0;

   USBD_UNUSED_PARAM(usbd);
   USBD_UNUSED_PARAM(dir);
   USBD_UNUSED_PARAM(is_last_part);

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && USBD_CHECK_PTR(void, data))
   {
      ep = &port_stm32_cat_a_io_ep[ep_num];

      if(0 == ep->in.dma.size)
      {
         size               /= ep->in.mps;
         size               *= ep->in.mps;
         ep->in.dma.data       = data;
         ep->in.dma.size       = size;
      //    ep->in.dma.total_size   = size + ep->in.buf0.size + ep->in.buf1.size;
         result              = size;

         USBD_DEBUG_HI_1(USBD_DBG_PORT_IO, "%d bytes taken by dma", result);

         if(ep->in.num_used_bufs < ep->in.max_num_bufs)
         {
            size = port_stm32_cat_a_io_data_in_process_bufs(ep, data, size, USBD_TRUE);

            ep->in.dma.data  = &(((uint8_t*)(ep->in.dma.data))[size]);
            ep->in.dma.size -= size;
         }
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "invalid params! data: %p, ep_num: %d", data, ep_num);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return result;
} /* port_stm32_cat_a_io_in_provide */

#endif

static USBD_IO_Inout_Data_Size_DT port_stm32_cat_a_io_in_memcpy(
   USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const void *data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part)
{
   USBD_UNUSED_PARAM(usbd);
   USBD_UNUSED_PARAM(dir);

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   size = port_stm32_cat_a_io_data_in_process_bufs(&port_stm32_cat_a_io_ep[ep_num], data, size, is_last_part);

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return size;
} /* port_stm32_cat_a_io_in_memcpy */

#if(USBD_PORT_STM32_CAT_A_DMA_TYPE == PORT_STM32_CAT_A_DMA_HALF_DMA)

static USBD_IO_Inout_Data_Size_DT port_stm32_cat_a_io_out_provide(
   USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, void *data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part)
{
   port_stm32_cat_a_io_ep_XT *ep;
   USBD_IO_Inout_Data_Size_DT result = 0;

   USBD_UNUSED_PARAM(usbd);
   USBD_UNUSED_PARAM(dir);
   USBD_UNUSED_PARAM(is_last_part);

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && USBD_CHECK_PTR(void, data))
   {
      ep = &port_stm32_cat_a_io_ep[ep_num];

      if(0 == ep->out.dma.size)
      {
         size                   /= ep->out.mps;
         size                   *= ep->out.mps;
         ep->out.dma.data        = data;
         ep->out.dma.size        = size;
         ep->out.dma.total_size  = size;
         result                  = size;

         USBD_DEBUG_HI_1(USBD_DBG_PORT_IO, "%d bytes taken by dma", result);
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "invalid params! data: %p, ep_num: %d", data, ep_num);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return result;
} /* port_stm32_cat_a_io_out_provide */

#endif

static USBD_IO_Inout_Data_Size_DT port_stm32_cat_a_io_out_memcpy(
   USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, void *data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part)
{
   USBD_UNUSED_PARAM(usbd);
   USBD_UNUSED_PARAM(dir);

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   size = port_stm32_cat_a_io_process_data_out(&port_stm32_cat_a_io_ep[ep_num], data, size, is_last_part);

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return size;
} /* port_stm32_cat_a_io_out_memcpy */

static USBD_Bool_DT port_stm32_cat_a_io_trigger(USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, USBD_Bool_DT dont_wait_for_ready)
{
   port_stm32_cat_a_io_ep_XT *ep;
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction;
   void *tp_params;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd) && (ep_num < USBD_MAX_NUM_ENDPOINTS))
   {
      result = USBD_TRUE;
      ep = &port_stm32_cat_a_io_ep[ep_num];

      if(USB_EP_DIRECTION_OUT == dir)
      {
         transaction = USBD_IO_GET_OUT_TRANSACATION_PARAMS(usbd, ep_num);
         tp_params   = USBD_IO_GET_OUT_TP_PARAMS_PTR(usbd, ep_num);

         ep->out.dont_wait_for_ready   = dont_wait_for_ready;
         ep->out.is_ep_processed       = USBD_TRUE;

#if(USBD_PORT_STM32_CAT_A_DMA_TYPE == PORT_STM32_CAT_A_DMA_HALF_DMA)
         if((ep->out.buf.size > 0) && USBD_IO_CHECK_OUT_MEMCPY_HANDLER(transaction))
         {
            USBD_DEBUG_HI_1(USBD_DBG_PORT_IO, "memcpy OUT with size: %d", ep->out.buf.size);

            USBD_IO_CALL_OUT_MEMCPY_HANDLER(
               usbd, tp_params, transaction, ep->out.buf.packet_size, ep->out.buf.size, port_stm32_cat_a_io_out_memcpy);

            if(USBD_BOOL_IS_TRUE(ep->out.is_ep_waiting) && (ep->out.buf.size <= 0))
            {
               ep->out.buf.data        = USBD_MAKE_INVALID_PTR(void);
               ep->out.buf.size        = 0;
               ep->out.is_ep_waiting   = USBD_FALSE;

               /* set registers to allow receiving next packets */
               port_stm32_cat_a_release_out_buffer(ep);
            }
         }
         else
         {
            ep->out.buf.data           = USBD_MAKE_INVALID_PTR(void);
            ep->out.buf.size           = 0;
            ep->out.is_ep_waiting      = USBD_FALSE;

            if((port_stm32_cat_a_req_params.ep_setup_ongoing != ep_num)
            || (0 == (USBD_STM32_REG->EP_REG[ep->out.hw.ep_reg_num] & USBDEP_STM32_CAT_A__CTR_RX)))
            {
               /* set registers to allow receiving next packets */
               port_stm32_cat_a_release_out_buffer(ep);
            }
         }

         if(USBD_BOOL_IS_FALSE(ep->out.is_ep_waiting) && USBD_IO_CHECK_OUT_PROVIDE_HANDLER(transaction))
         {
            USBD_DEBUG_HI_1(USBD_DBG_PORT_IO, "provide OUT with size: %d", (-1));

            USBD_IO_CALL_OUT_PROVIDE_HANDLER(
               usbd, tp_params, transaction, ep->out.buf.packet_size, (-1), port_stm32_cat_a_io_out_provide);
         }
#else
         if((ep->out.buf.size > 0) && USBD_IO_CHECK_OUT_MEMCPY_HANDLER(transaction))
         {
            USBD_DEBUG_HI_1(USBD_DBG_PORT_IO, "memcpy OUT with size: %d", ep->out.buf.size);

            USBD_IO_CALL_OUT_MEMCPY_HANDLER(
               usbd, tp_params, transaction, ep->out.buf.packet_size, ep->out.buf.size, port_stm32_cat_a_io_out_memcpy);

            if(USBD_BOOL_IS_TRUE(ep->out.is_ep_waiting) && (ep->out.buf.size <= 0))
            {
               ep->out.buf.data        = USBD_MAKE_INVALID_PTR(void);
               ep->out.buf.size        = 0;
               ep->out.is_ep_waiting   = USBD_FALSE;

               /* set registers to allow receiving next packets */
               port_stm32_cat_a_release_out_buffer(ep);
            }
         }
         else
         {
            ep->out.buf.data           = USBD_MAKE_INVALID_PTR(void);
            ep->out.buf.size           = 0;
            ep->out.is_ep_waiting      = USBD_FALSE;

            /* set registers to allow receiving next packets */
            port_stm32_cat_a_release_out_buffer(ep);
         }
#endif
      }
      else
      /* USB_EP_DIRECTION_IN */
      {
         transaction = USBD_IO_GET_IN_TRANSACATION_PARAMS(usbd, ep_num);
         tp_params   = USBD_IO_GET_IN_TP_PARAMS_PTR(usbd, ep_num);

#if(USBD_PORT_STM32_CAT_A_DMA_TYPE == PORT_STM32_CAT_A_DMA_HALF_DMA)
         if((ep->in.num_used_bufs < ep->in.max_num_bufs) && USBD_IO_CHECK_IN_PROVIDE_HANDLER(transaction))
         {
            USBD_DEBUG_HI(USBD_DBG_PORT_IO, "provide IN");
            USBD_IO_CALL_IN_PROVIDE_HANDLER(port_stm32_cat_a_usbd, tp_params, transaction, port_stm32_cat_a_io_in_provide);
         }
#endif
         if(ep->in.num_used_bufs < ep->in.max_num_bufs)
         {
            if(USBD_IO_CHECK_IN_MEMCPY_HANDLER(transaction))
            {
               USBD_DEBUG_HI(USBD_DBG_PORT_IO, "memcpy IN");
               USBD_IO_CALL_IN_MEMCPY_HANDLER(usbd, tp_params, transaction, port_stm32_cat_a_io_in_memcpy);
            }
            else if( ((0 == ep->in.num_used_bufs) && (ep->in.buf0.size > 0))
#if(USBD_MAX_NUM_ENDPOINTS > 1)
               || ((1 == ep->in.num_used_bufs) && (ep->in.buf1.size > 0))
#endif
            )
            {
               (void)port_stm32_cat_a_io_data_in_process_bufs(ep, &ep_num, 0, USBD_TRUE);
            }
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return result;
} /* port_stm32_cat_a_io_trigger */

static USBD_IO_Inout_Data_Size_DT port_stm32_cat_a_io_get_ep_out_waiting_size(USBD_Params_XT *usbd, uint8_t ep_num, USBD_Bool_DT left_size)
{
   port_stm32_cat_a_io_ep_XT *ep;
   USBD_IO_Inout_Data_Size_DT result = (-1);

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd) && (ep_num < USBD_MAX_NUM_ENDPOINTS))
   {
      ep = &port_stm32_cat_a_io_ep[ep_num];

      if(USBD_BOOL_IS_TRUE(ep->out.is_ep_waiting))
      {
         if(USBD_BOOL_IS_TRUE(left_size))
         {
            result = ep->out.buf.size;
         }
         else
         {
            result = ep->out.buf.packet_size;
         }
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "invalid params! usbd: %p, ep_num: %d", usbd, ep_num);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return result;
} /* port_stm32_cat_a_io_get_ep_out_waiting_size */

static USBD_IO_Inout_Data_Size_DT port_stm32_cat_a_io_get_ep_in_buffered_size(USBD_Params_XT *usbd, uint8_t ep_num)
{
   port_stm32_cat_a_io_ep_XT *ep;
   USBD_IO_Inout_Data_Size_DT result = (-1);

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd) && (ep_num < USBD_MAX_NUM_ENDPOINTS))
   {
      ep = &port_stm32_cat_a_io_ep[ep_num];

#if(USBD_PORT_STM32_CAT_A_DMA_TYPE == PORT_STM32_CAT_A_DMA_HALF_DMA)
      if((ep->in.dma.size > 0) || (ep->in.num_used_bufs > 0))
      {
         result =
            (USBD_IO_Inout_Data_Size_DT)(ep->in.buf0.size) + (USBD_IO_Inout_Data_Size_DT)(ep->in.buf1.size) + ep->in.dma.size;
      }
#else
      if(ep->in.num_used_bufs > 0)
      {
#if(USBD_MAX_NUM_ENDPOINTS > 1)
         result = (USBD_IO_Inout_Data_Size_DT)(ep->in.buf0.size) + (USBD_IO_Inout_Data_Size_DT)(ep->in.buf1.size);
#else
         result = (USBD_IO_Inout_Data_Size_DT)(ep->in.buf0.size);
#endif
      }
#endif
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "invalid params! usbd: %p, ep_num: %d", usbd, ep_num);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return result;
} /* port_stm32_cat_a_io_get_ep_in_buffered_size */

static void port_stm32_cat_a_io_stall(USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir)
{
   USBD_UNUSED_PARAM(usbd);
#ifndef USBD_USE_IOCMD
   USBD_UNUSED_PARAM(ep_num);
   USBD_UNUSED_PARAM(dir);
#endif

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   USBD_EMERG_3(USBD_DBG_PORT_IO, "EP: %d; dir: %3s - %s", ep_num, (USB_EP_DIRECTION_OUT == dir) ? "OUT" : "IN", "STALL");

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);
} /* port_stm32_cat_a_io_stall */

static void port_stm32_cat_a_io_abort(USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, USBD_Bool_DT flush_hw_bufs)
{
   port_stm32_cat_a_io_ep_XT *ep;
   uint32_t temp;
#ifdef USBD_USE_IOCMD
   uint32_t temp_before;
   uint32_t temp_after;
#endif

   USBD_UNUSED_PARAM(usbd);

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   USBD_DEBUG_HI_3(USBD_DBG_PORT_IO, "EP: %d; dir: %3s - %s", ep_num, (USB_EP_DIRECTION_OUT == dir) ? "OUT" : "IN", "ABORT");

   if(ep_num < USBD_MAX_NUM_ENDPOINTS)
   {
      ep = &port_stm32_cat_a_io_ep[ep_num];

      if(USB_EP_DIRECTION_OUT == dir)
      {
         ep->out.buf.size        = (-1);
         ep->out.buf.packet_size = (-1);
         ep->out.buf.data        = USBD_MAKE_INVALID_PTR(uint8_t);
#if(USBD_PORT_STM32_CAT_A_DMA_TYPE == PORT_STM32_CAT_A_DMA_HALF_DMA)
         ep->out.dma.size        = 0;
         ep->out.dma.total_size  = 0;
#endif
         if(USBD_BOOL_IS_TRUE(ep->out.is_ep_waiting))
         {
            ep->out.is_ep_waiting= USBD_FALSE;

            /* set registers to allow receiving next packets */
            port_stm32_cat_a_release_out_buffer(ep);
         }
      }
      else
      {
         if(USBD_BOOL_IS_TRUE(flush_hw_bufs))
         {
            ep->in.num_used_bufs = 0;
            ep->in.buf0.size     = 0;
#if(USBD_MAX_NUM_ENDPOINTS > 1)
            ep->in.buf1.size     = 0;
#endif
            ep->in.data_provided = USBD_FALSE;
#if(USBD_PORT_STM32_CAT_A_DMA_TYPE == PORT_STM32_CAT_A_DMA_HALF_DMA)
            ep->in.dma.size      = 0;
#endif
         /* clear only TX IRQ flag */
         USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
            ep->in.hw.ep_reg_num,
            USBD_STM32_REG->EP_REG[ep->in.hw.ep_reg_num],
            temp,
            temp_before,
            temp_after,
            /*_EX_OR*/0,
            /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE,
            /*_OR   */USBDEP_STM32_CAT_A__CTR_RX);
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);
} /* port_stm32_cat_a_io_stall */

#if(USBD_FEATURE_PRESENT == USBD_EP_HALT_SUPPORTED)
static void port_stm32_cat_a_io_halt(
   USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const USB_Endpoint_Desc_DT *ep_desc, USBD_Bool_DT state)
{
   port_stm32_cat_a_io_ep_XT *ep;
   volatile uint32_t *ep_reg;
   uint32_t temp;
#ifdef USBD_USE_IOCMD
   uint32_t temp_before;
   uint32_t temp_after;
#endif
   uint16_t mps;
   uint8_t ep_type;
   uint8_t num;

   USBD_UNUSED_PARAM(usbd);
   USBD_UNUSED_PARAM(ep_desc);

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO_ONOFF);

   ep = &port_stm32_cat_a_io_ep[ep_num];

   if((USB_EP_DIRECTION_OUT == dir) && (ep->out.hw.ep_reg_num < USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS))
   {
      ep_reg = &USBD_STM32_REG->EP_REG[ep->out.hw.ep_reg_num];

      if(0 != (ep_reg[0] & USBDEP_STM32_CAT_A__STAT_RX))
      {
         if(USBD_BOOL_IS_TRUE(state))
         {
            USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
               ep->out.hw.ep_reg_num,
               ep_reg[0],
               temp,
               temp_before,
               temp_after,
               /*_EX_OR*/USBDEP_STM32_CAT_A__RX_STALL,
               /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE,
               /*_OR   */USBDEP_STM32_CAT_A__CTR_TX);
         }
         else
         {
            /* set zeroes to all EP fields except EP register number and non-zezroes to specific fields */
            num                        = ep->out.hw.ep_reg_num;
            mps                        = ep->out.mps;
            ep_type                    = ep->out.ep_type;
            memset(&(ep->out), 0, sizeof(ep->out));
            ep->out.hw.ep_reg_num      = num;
            ep->out.mps                = mps;
            ep->out.ep_type            = ep_type;
            ep->out.out_on_delay       = USBD_FALSE;
            ep->out.is_ep_waiting      = USBD_FALSE;
            ep->out.buf.size           = (-1);
            ep->out.buf.packet_size    = (-1);
            ep->out.buf.data           = USBD_MAKE_INVALID_PTR(uint8_t);

#if((USBD_FEATURE_PRESENT == USBD_IO_ISOCHRONOUS_TRANSFER_SUPPORTED) || (USBD_FEATURE_PRESENT == USBD_IO_BULK_TRANSFER_SUPPORTED))
            if( (USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS == ep_type)
               || ((USB_EP_DESC_TRANSFER_TYPE_BULK == ep_type) && (1 != port_stm32_cat_a_dev_prams.bulk_eps_num_bufs)) )
            {
               USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
                  ep->out.hw.ep_reg_num,
                  ep_reg[0],
                  temp,
                  temp_before,
                  temp_after,
                  /*_EX_OR*/USBDEP_STM32_CAT_A__RX_VALID,
                  /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE | USBDEP_STM32_CAT_A__DTOG_RX | USBDEP_STM32_CAT_A__DTOG_TX,
                  /*_OR   */0);
            }
            else
#endif
            {
               USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
                  ep->out.hw.ep_reg_num,
                  ep_reg[0],
                  temp,
                  temp_before,
                  temp_after,
                  /*_EX_OR*/USBDEP_STM32_CAT_A__RX_VALID,
                  /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE | USBDEP_STM32_CAT_A__DTOG_RX,
                  /*_OR   */USBDEP_STM32_CAT_A__CTR_TX);
            }
         }
      }
   }
   else if((USB_EP_DIRECTION_IN == dir) && (ep->in.hw.ep_reg_num < USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS))
   {
      ep_reg = &USBD_STM32_REG->EP_REG[ep->in.hw.ep_reg_num];

      if(0 != (ep_reg[0] & USBDEP_STM32_CAT_A__STAT_TX))
      {
         if(USBD_BOOL_IS_TRUE(state))
         {
            USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
               ep->in.hw.ep_reg_num,
               ep_reg[0],
               temp,
               temp_before,
               temp_after,
               /*_EX_OR*/USBDEP_STM32_CAT_A__TX_STALL,
               /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE,
               /*_OR   */USBDEP_STM32_CAT_A__CTR_RX);
         }
         else
         {
            /* set zeroes to all EP fields except EP register number and non-zezroes to specific fields */
            num                        = ep->in.hw.ep_reg_num;
            mps                        = ep->in.mps;
            ep_type                    = ep->in.ep_type;
            memset(&(ep->in), 0, sizeof(ep->in));
            ep->in.hw.ep_reg_num       = num;
            ep->in.mps                 = mps;
            ep->in.ep_type             = ep_type;
            ep->in.max_num_bufs        = 1;
            ep->in.data_provided       = USBD_FALSE;

#if((USBD_FEATURE_PRESENT == USBD_IO_ISOCHRONOUS_TRANSFER_SUPPORTED) || (USBD_FEATURE_PRESENT == USBD_IO_BULK_TRANSFER_SUPPORTED))
            if( (USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS == ep_type)
               || ((USB_EP_DESC_TRANSFER_TYPE_BULK == ep_type) && (1 != port_stm32_cat_a_dev_prams.bulk_eps_num_bufs)) )
            {
               USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
                  ep->in.hw.ep_reg_num,
                  ep_reg[0],
                  temp,
                  temp_before,
                  temp_after,
                  /*_EX_OR*/USBDEP_STM32_CAT_A__TX_NAK,
                  /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE | USBDEP_STM32_CAT_A__DTOG_RX | USBDEP_STM32_CAT_A__DTOG_TX,
                  /*_OR   */0);
            }
            else
#endif
            {
               USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
                  ep->in.hw.ep_reg_num,
                  ep_reg[0],
                  temp,
                  temp_before,
                  temp_after,
                  /*_EX_OR*/USBDEP_STM32_CAT_A__TX_NAK,
                  /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE | USBDEP_STM32_CAT_A__DTOG_TX,
                  /*_OR   */USBDEP_STM32_CAT_A__CTR_RX);
            }
         }
      }
   }

   USBD_ALERT_3(USBD_DBG_PORT_IO_ONOFF, "EP: %d; dir: %3s - %s",
         ep_num, (USB_EP_DIRECTION_OUT == dir) ? "OUT" : "IN", USBD_BOOL_IS_TRUE(state) ? "HALTED" : "NOT_HALTED");

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO_ONOFF);
} /* port_stm32_cat_a_io_halt */
#endif

#if(USBD_MAX_NUM_ENDPOINTS > 1)

static void port_stm32_cat_a_io_disable_ep_ctrl_int_in(uint8_t ep_reg_num, volatile uint32_t *ep_reg)
{
   uint32_t temp;
#ifdef USBD_USE_IOCMD
   uint32_t temp_before;
   uint32_t temp_after;
#else
   USBD_UNUSED_PARAM(ep_reg_num);
#endif

   if(0 != USBD_STM32_CAT_A_REG_GET_2BIT(ep_reg[0], USBDEP_STM32_CAT_A_BIT__STAT_RX))
   {
      USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
         ep_reg_num,
         ep_reg[0],
         temp,
         temp_before,
         temp_after,
         /*_EX_OR*/0,
         /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE | USBDEP_STM32_CAT_A__STAT_TX | USBDEP_STM32_CAT_A__DTOG_TX,
         /*_OR   */USBDEP_STM32_CAT_A__CTR_RX);
   }
   else
   {
      port_stm32_cat_a_io_print_ep_reg_state("-> ", ep_reg[0], __LINE__, ep_reg_num);
      ep_reg[0] &= USBDEP_STM32_CAT_A__DTOG_RX | USBDEP_STM32_CAT_A__STAT_RX | USBDEP_STM32_CAT_A__DTOG_TX | USBDEP_STM32_CAT_A__STAT_TX | USBDEP_STM32_CAT_A__ADDR;
      port_stm32_cat_a_io_print_ep_reg_state("<- ", ep_reg[0], __LINE__, ep_reg_num);
   }
} /* port_stm32_cat_a_io_disable_ep_ctrl_int_in */

static void port_stm32_cat_a_io_disable_ep_ctrl_int_out(uint8_t ep_reg_num, volatile uint32_t *ep_reg)
{
   uint32_t temp;
#ifdef USBD_USE_IOCMD
   uint32_t temp_before;
   uint32_t temp_after;
#else
   USBD_UNUSED_PARAM(ep_reg_num);
#endif

   if(0 != USBD_STM32_CAT_A_REG_GET_2BIT(ep_reg[0], USBDEP_STM32_CAT_A_BIT__STAT_TX))
   {
      USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
         ep_reg_num,
         ep_reg[0],
         temp,
         temp_before,
         temp_after,
         /*_EX_OR*/0,
         /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE | USBDEP_STM32_CAT_A__STAT_RX | USBDEP_STM32_CAT_A__DTOG_RX,
         /*_OR   */USBDEP_STM32_CAT_A__CTR_TX);
   }
   else
   {
      port_stm32_cat_a_io_print_ep_reg_state("-> ", ep_reg[0], __LINE__, ep_reg_num);
      ep_reg[0] &= USBDEP_STM32_CAT_A__DTOG_RX | USBDEP_STM32_CAT_A__STAT_RX | USBDEP_STM32_CAT_A__DTOG_TX | USBDEP_STM32_CAT_A__STAT_TX | USBDEP_STM32_CAT_A__ADDR;
      port_stm32_cat_a_io_print_ep_reg_state("<- ", ep_reg[0], __LINE__, ep_reg_num);
   }
} /* port_stm32_cat_a_io_disable_ep_ctrl_int_out */

static void port_stm32_cat_a_io_disable_ep_isochronous(uint8_t ep_reg_num, volatile uint32_t *ep_reg)
{
#if(USBD_FEATURE_PRESENT != USBD_IO_ISOCHRONOUS_TRANSFER_SUPPORTED)

   USBD_UNUSED_PARAM(ep_reg_num);
   USBD_UNUSED_PARAM(ep_reg);

#else

#ifndef USBD_USE_IOCMD
   USBD_UNUSED_PARAM(ep_reg_num);
#endif
   port_stm32_cat_a_io_print_ep_reg_state("-> ", ep_reg[0], __LINE__, ep_reg_num);
   ep_reg[0] &= USBDEP_STM32_CAT_A__DTOG_RX | USBDEP_STM32_CAT_A__STAT_RX | USBDEP_STM32_CAT_A__DTOG_TX | USBDEP_STM32_CAT_A__STAT_TX | USBDEP_STM32_CAT_A__ADDR;
   port_stm32_cat_a_io_print_ep_reg_state("<- ", ep_reg[0], __LINE__, ep_reg_num);

#endif
} /* port_stm32_cat_a_io_disable_ep_isochronous */

static void port_stm32_cat_a_io_disable_ep_bulk_in(uint8_t ep_reg_num, volatile uint32_t *ep_reg)
{
#if(USBD_FEATURE_PRESENT != USBD_IO_BULK_TRANSFER_SUPPORTED)

   USBD_UNUSED_PARAM(ep_reg_num);
   USBD_UNUSED_PARAM(ep_reg);

#else

   uint32_t temp;
#ifdef USBD_USE_IOCMD
   uint32_t temp_before;
   uint32_t temp_after;
#else
   USBD_UNUSED_PARAM(ep_reg_num);
#endif

   if((1 == port_stm32_cat_a_dev_prams.bulk_eps_num_bufs)
      && (0 != USBD_STM32_CAT_A_REG_GET_2BIT(ep_reg[0], USBDEP_STM32_CAT_A_BIT__STAT_RX)))
   {
      USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
         ep_reg_num,
         ep_reg[0],
         temp,
         temp_before,
         temp_after,
         /*_EX_OR*/0,
         /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE | USBDEP_STM32_CAT_A__STAT_TX | USBDEP_STM32_CAT_A__DTOG_TX,
         /*_OR   */USBDEP_STM32_CAT_A__CTR_RX);
   }
   else
   {
      port_stm32_cat_a_io_print_ep_reg_state("-> ", ep_reg[0], __LINE__, ep_reg_num);
      ep_reg[0] &= USBDEP_STM32_CAT_A__DTOG_RX | USBDEP_STM32_CAT_A__STAT_RX | USBDEP_STM32_CAT_A__DTOG_TX | USBDEP_STM32_CAT_A__STAT_TX | USBDEP_STM32_CAT_A__ADDR;
      port_stm32_cat_a_io_print_ep_reg_state("<- ", ep_reg[0], __LINE__, ep_reg_num);
   }

#endif
} /* port_stm32_cat_a_io_disable_ep_bulk_in */

static void port_stm32_cat_a_io_disable_ep_bulk_out(uint8_t ep_reg_num, volatile uint32_t *ep_reg)
{
#if(USBD_FEATURE_PRESENT != USBD_IO_BULK_TRANSFER_SUPPORTED)

   USBD_UNUSED_PARAM(ep_reg_num);
   USBD_UNUSED_PARAM(ep_reg);

#else

   uint32_t temp;
#ifdef USBD_USE_IOCMD
   uint32_t temp_before;
   uint32_t temp_after;
#else
   USBD_UNUSED_PARAM(ep_reg_num);
#endif

   if((1 == port_stm32_cat_a_dev_prams.bulk_eps_num_bufs)
      && (0 != USBD_STM32_CAT_A_REG_GET_2BIT(ep_reg[0], USBDEP_STM32_CAT_A_BIT__STAT_TX)))
   {
      USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
         ep_reg_num,
         ep_reg[0],
         temp,
         temp_before,
         temp_after,
         /*_EX_OR*/0,
         /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE | USBDEP_STM32_CAT_A__STAT_RX | USBDEP_STM32_CAT_A__DTOG_RX,
         /*_OR   */USBDEP_STM32_CAT_A__CTR_TX);
   }
   else
   {
      port_stm32_cat_a_io_print_ep_reg_state("-> ", ep_reg[0], __LINE__, ep_reg_num);
      ep_reg[0] &= USBDEP_STM32_CAT_A__DTOG_RX | USBDEP_STM32_CAT_A__STAT_RX | USBDEP_STM32_CAT_A__DTOG_TX | USBDEP_STM32_CAT_A__STAT_TX | USBDEP_STM32_CAT_A__ADDR;
      port_stm32_cat_a_io_print_ep_reg_state("<- ", ep_reg[0], __LINE__, ep_reg_num);
   }

#endif
} /* port_stm32_cat_a_io_disable_ep_bulk_out */

static void port_stm32_cat_a_io_configure(
   USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const USB_Endpoint_Desc_DT *ep_desc, USBD_Bool_DT state)
{
   port_stm32_cat_a_io_ep_XT *ep;
   volatile uint32_t *ep_reg;
   uint32_t count;
   uint16_t mps;
   uint8_t if_num;
   uint8_t ep_type;
   uint8_t num;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO_ONOFF);

   USBD_DEBUG_HI_3(USBD_DBG_PORT_IO_ONOFF, "EP: %d; dir: %3s - %s",
         ep_num, (USB_EP_DIRECTION_OUT == dir) ? "OUT" : "IN", USBD_BOOL_IS_TRUE(state) ? "CONFIGURE" : "DE-CONFIGURE");

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && USBD_CHECK_PTR(USB_Endpoint_Desc_DT, ep_desc))
   {
      ep = &port_stm32_cat_a_io_ep[ep_num];

      mps   = (uint16_t)ep_desc->wMaxPacketSize.H;
      mps <<= 8;
      mps  += ep_desc->wMaxPacketSize.L;
      ep_type = ep_desc->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK;

#if(USBD_MAX_NUM_ALTERNATE_INTERFACE_SETTINGS > 0)
      if(port_stm32_cat_a_req_params.interface_set_ongoing < USBD_MAX_NUM_INTERFACES)
      {
         port_stm32_cat_a_dev_interface_shared_mem_start_point[port_stm32_cat_a_req_params.interface_set_ongoing].current_offset =0;
         port_stm32_cat_a_req_params.interface_set_ongoing = USBD_MAX_NUM_INTERFACES;
      }
#endif

      /* deconfigure EP at first */
      if((USB_EP_DIRECTION_OUT == dir) && (ep->out.hw.ep_reg_num < USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS))
      {
         ep_reg = &USBD_STM32_REG->EP_REG[ep->out.hw.ep_reg_num];

         port_stm32_cat_a_io_disable_ep_out[ep_type](ep->out.hw.ep_reg_num, ep_reg);

         USBD_DEBUG_HI_4(USBD_DBG_PORT_IO_ONOFF, "EP: %d; dir: %3s; reg %s: 0x%08X",
               ep_num, (USB_EP_DIRECTION_OUT == dir) ? "OUT" : "IN", "disabled", ep_reg[0]);

         /* set zeroes to all EP fields except EP register number and non-zezroes to specific fields */
         num                        = ep->out.hw.ep_reg_num;
         memset(&(ep->out), 0, sizeof(ep->out));
         ep->out.hw.ep_reg_num      = num;
         ep->out.out_on_delay       = USBD_FALSE;
         ep->out.is_ep_waiting      = USBD_FALSE;
         ep->out.buf.size           = (-1);
         ep->out.buf.packet_size    = (-1);
         ep->out.buf.data           = USBD_MAKE_INVALID_PTR(uint8_t);

         if(USBD_BOOL_IS_TRUE(state))
         {
            ep->out.mps             = mps;
            ep->out.ep_type         = ep_type;

            port_stm32_cat_a_io_ep_reg_num_ep[num] = ep_num;

            if_num = USBD_DEV_Get_EP_Interface_Num(usbd, ep_num, dir);

            if((0 == ep_num) || (if_num < USBD_MAX_NUM_INTERFACES))
            {
               /* ok, lets continue */
            }
            else
            {
               USBD_ERROR_1(USBD_DBG_PORT_IO_ONOFF, "invalid interface number %d!!!", if_num);
               // TODO: USBD_panic

               state = USBD_FALSE;
            }
         }
         else
         {
            port_stm32_cat_a_io_ep_reg_num_ep[num] = USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS;
         }
      }
      else if((USB_EP_DIRECTION_IN == dir) && (ep->in.hw.ep_reg_num < USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS))
      {
         ep_reg = &USBD_STM32_REG->EP_REG[ep->in.hw.ep_reg_num];

         port_stm32_cat_a_io_disable_ep_in[ep_type](ep->in.hw.ep_reg_num, ep_reg);

         USBD_DEBUG_HI_4(USBD_DBG_PORT_IO_ONOFF, "EP: %d; dir: %3s; reg %s: 0x%08X",
               ep_num, (USB_EP_DIRECTION_OUT == dir) ? "OUT" : "IN", "disabled", ep_reg[0]);

         /* set zeroes to all EP fields except EP register number and non-zezroes to specific fields */
         num                        = ep->in.hw.ep_reg_num;
         memset(&(ep->in), 0, sizeof(ep->in));
         ep->in.hw.ep_reg_num       = num;
         if(USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS == ep_type)
         {
            ep->in.max_num_bufs     = 2;
         }
         else if(USB_EP_DESC_TRANSFER_TYPE_BULK == ep_type)
         {
            ep->in.max_num_bufs     = port_stm32_cat_a_dev_prams.bulk_eps_num_bufs;
         }
         else
         {
            ep->in.max_num_bufs     = 1;
         }
         ep->in.data_provided       = USBD_FALSE;

         if(USBD_BOOL_IS_TRUE(state))
         {
            ep->in.mps              = mps;
            ep->in.ep_type          = ep_type;

            port_stm32_cat_a_io_ep_reg_num_ep[num] = ep_num;

            if_num = USBD_DEV_Get_EP_Interface_Num(usbd, ep_num, dir);

            if((0 == ep_num) || (if_num < USBD_MAX_NUM_INTERFACES))
            {
               /* ok, lets continue */
            }
            else
            {
               USBD_ERROR_1(USBD_DBG_PORT_IO_ONOFF, "invalid interface number %d!!!", if_num);
               // TODO: USPD_panic

               state = USBD_FALSE;
            }
         }
         else
         {
            port_stm32_cat_a_io_ep_reg_num_ep[num] = USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS;
         }
      }
      else
      {
         USBD_ERROR(USBD_DBG_PORT_IO_ONOFF, "invalid register pointer!!!");
         // TODO: USBD_panic

         state = USBD_FALSE;
      }

      /* EP to be configured */
      if(USBD_BOOL_IS_TRUE(state))
      {
         switch(ep_type)
         {
            case USB_EP_DESC_TRANSFER_TYPE_CONTROL:
               if(USB_EP_DIRECTION_OUT == dir)
               {
                  if(0 == ep_num)
                  {
                     USBDEP_SET_RX_BUF_ADDR(0, USBD_PORT_STM32_CAT_A_EPS_SHARED_MEM_ALOC_TAB_SIZE);
                     count = USBD_PORT_STM32_CAT_A_EP0_MPS;
                  }
                  else
                  {
                     USBDEP_SET_RX_BUF_ADDR(
                        num,
                        port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].start_point
                           + port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset);
                     port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset += mps;

                     count = (uint32_t)mps;
                  }

                  USBDEP_SB_SET_RX_MAX_POSSIBLE_COUNT(ep->out.hw.ep_reg_num, count);

                  /* set EP control register to enable it */
                  port_stm32_cat_a_io_print_ep_reg_state("-> ", ep_reg[0], __LINE__, ep_num);
                  ep_reg[0] =
                     USBDEP_STM32_CAT_A__CTR_TX | USBDEP_STM32_CAT_A__RX_VALID | USBDEP_STM32_CAT_A__CONTROL | (uint32_t)ep_num;
                  port_stm32_cat_a_io_print_ep_reg_state("<- ", ep_reg[0], __LINE__, ep_num);
               }
               else /* USB_EP_DIRECTION_IN */
               {
                  if(0 == ep_num)
                  {
                     USBDEP_SET_TX_BUF_ADDR(0, USBD_PORT_STM32_CAT_A_EPS_SHARED_MEM_ALOC_TAB_SIZE + USBD_PORT_STM32_CAT_A_EP0_MPS);
                  }
                  else
                  {
                     USBDEP_SET_TX_BUF_ADDR(
                        num,
                        port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].start_point
                           + port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset);
                     port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset += mps;
                  }

                  USBDEP_SB_CLR_TX_COUNT(num);

                  /* set EP control register to enable it */
                  port_stm32_cat_a_io_print_ep_reg_state("-> ", ep_reg[0], __LINE__, ep_num);
                  ep_reg[0] =
                     USBDEP_STM32_CAT_A__CTR_RX | USBDEP_STM32_CAT_A__TX_NAK | USBDEP_STM32_CAT_A__CONTROL | (uint32_t)ep_num;
                  port_stm32_cat_a_io_print_ep_reg_state("<- ", ep_reg[0], __LINE__, ep_num);
               }
               break;

#if(USBD_FEATURE_PRESENT == USBD_IO_ISOCHRONOUS_TRANSFER_SUPPORTED)
            case USB_EP_DESC_TRANSFER_TYPE_ISOCHRONOUS:
               if(USB_EP_DIRECTION_OUT == dir)
               {
                  USBDEP_SET_RX_BUF_ADDR(
                     num,
                     port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].start_point
                        + port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset);

                  port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset += mps;

                  USBDEP_SET_TX_BUF_ADDR(
                     num,
                     port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].start_point
                        + port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset);

                  port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset += mps;

                  count = (uint32_t)mps;

                  USBDEP_DB_SET_RX_MAX_POSSIBLE_COUNT(ep->out.hw.ep_reg_num, count);

                  /* set EP control register to enable it */
                  port_stm32_cat_a_io_print_ep_reg_state("-> ", ep_reg[0], __LINE__, ep_num);
                  ep_reg[0] =
                     USBDEP_STM32_CAT_A__RX_VALID | USBDEP_STM32_CAT_A__TX_DIS | USBDEP_STM32_CAT_A__ISOCHRONOUS | (uint32_t)ep_num;
                  port_stm32_cat_a_io_print_ep_reg_state("<- ", ep_reg[0], __LINE__, ep_num);
               }
               else /* USB_EP_DIRECTION_IN */
               {
                  USBDEP_SET_RX_BUF_ADDR(
                     num,
                     port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].start_point
                        + port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset);

                  port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset += mps;

                  USBDEP_SET_TX_BUF_ADDR(
                     num,
                     port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].start_point
                        + port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset);

                  USBDEP_DB_CLR_TX_COUNT(num);

                  port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset += mps;

                  /* set EP control register to enable it */
                  port_stm32_cat_a_io_print_ep_reg_state("-> ", ep_reg[0], __LINE__, ep_num);
                  ep_reg[0] =
                     USBDEP_STM32_CAT_A__RX_DIS | USBDEP_STM32_CAT_A__TX_VALID | USBDEP_STM32_CAT_A__ISOCHRONOUS | (uint32_t)ep_num;
                  port_stm32_cat_a_io_print_ep_reg_state("<- ", ep_reg[0], __LINE__, ep_num);
               }
               break;
#endif

#if(USBD_FEATURE_PRESENT == USBD_IO_BULK_TRANSFER_SUPPORTED)
            case USB_EP_DESC_TRANSFER_TYPE_BULK:
               if(USB_EP_DIRECTION_OUT == dir)
               {
                  USBDEP_SET_RX_BUF_ADDR(
                     num,
                     port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].start_point
                        + port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset);

                  port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset += mps;

                  if(1 == port_stm32_cat_a_dev_prams.bulk_eps_num_bufs)
                  {
                     count = (uint32_t)mps;

                     USBDEP_SB_SET_RX_MAX_POSSIBLE_COUNT(ep->out.hw.ep_reg_num, count);

                     /* set EP control register to enable it */
                     port_stm32_cat_a_io_print_ep_reg_state("-> ", ep_reg[0], __LINE__, ep_num);
                     ep_reg[0] =
                        USBDEP_STM32_CAT_A__CTR_TX | USBDEP_STM32_CAT_A__RX_VALID | USBDEP_STM32_CAT_A__BULK | (uint32_t)ep_num;
                     port_stm32_cat_a_io_print_ep_reg_state("<- ", ep_reg[0], __LINE__, ep_num);
                  }
                  else
                  {
                     USBDEP_SET_TX_BUF_ADDR(
                        num,
                        port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].start_point
                           + port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset);

                     port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset += mps;

                     count = (uint32_t)mps;

                     USBDEP_DB_SET_RX_MAX_POSSIBLE_COUNT(ep->out.hw.ep_reg_num, count);

                     /* set EP control register to enable it */
                     port_stm32_cat_a_io_print_ep_reg_state("-> ", ep_reg[0], __LINE__, ep_num);
                     ep_reg[0] =
                        USBDEP_STM32_CAT_A__RX_VALID | USBDEP_STM32_CAT_A__TX_DIS | USBDEP_STM32_CAT_A__DTOG_TX
                          | USBDEP_STM32_CAT_A__KIND | USBDEP_STM32_CAT_A__BULK   | (uint32_t)ep_num;
                     port_stm32_cat_a_io_print_ep_reg_state("<- ", ep_reg[0], __LINE__, ep_num);
                  }
               }
               else /* USB_EP_DIRECTION_IN */
               {
                  USBDEP_SET_TX_BUF_ADDR(
                     num,
                     port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].start_point
                        + port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset);

                  port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset += mps;

                  if(1 == port_stm32_cat_a_dev_prams.bulk_eps_num_bufs)
                  {
                     USBDEP_SB_CLR_TX_COUNT(num);

                     /* set EP control register to enable it */
                     port_stm32_cat_a_io_print_ep_reg_state("-> ", ep_reg[0], __LINE__, ep_num);
                     ep_reg[0] =
                        USBDEP_STM32_CAT_A__CTR_RX | USBDEP_STM32_CAT_A__TX_NAK | USBDEP_STM32_CAT_A__BULK | (uint32_t)ep_num;
                     port_stm32_cat_a_io_print_ep_reg_state("<- ", ep_reg[0], __LINE__, ep_num);
                  }
                  else
                  {
                     USBDEP_SET_RX_BUF_ADDR(
                        num,
                        port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].start_point
                           + port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset);

                     USBDEP_DB_CLR_TX_COUNT(num);

                     port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset += mps;

                     /* set EP control register to enable it */
                     port_stm32_cat_a_io_print_ep_reg_state("-> ", ep_reg[0], __LINE__, ep_num);
                     ep_reg[0] =
                        USBDEP_STM32_CAT_A__RX_DIS | USBDEP_STM32_CAT_A__TX_VALID
                        | USBDEP_STM32_CAT_A__KIND | USBDEP_STM32_CAT_A__BULK   | (uint32_t)ep_num;
                     port_stm32_cat_a_io_print_ep_reg_state("<- ", ep_reg[0], __LINE__, ep_num);
                  }
               }
               break;
#endif

#if(USBD_FEATURE_PRESENT == USBD_IO_INTERRUPT_TRANSFER_SUPPORTED)
            case USB_EP_DESC_TRANSFER_TYPE_INTERRUPT:
               if(USB_EP_DIRECTION_OUT == dir)
               {
                  USBDEP_SET_RX_BUF_ADDR(
                     num,
                     port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].start_point
                        + port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset);

                  port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset += mps;

                  count = (uint32_t)mps;

                  USBDEP_SB_SET_RX_MAX_POSSIBLE_COUNT(ep->out.hw.ep_reg_num, count);

                  /* set EP control register to enable it */
                  port_stm32_cat_a_io_print_ep_reg_state("-> ", ep_reg[0], __LINE__, ep_num);
                  ep_reg[0] =
                     USBDEP_STM32_CAT_A__CTR_TX | USBDEP_STM32_CAT_A__RX_VALID | USBDEP_STM32_CAT_A__INTERRUPT | (uint32_t)ep_num;
                  port_stm32_cat_a_io_print_ep_reg_state("<- ", ep_reg[0], __LINE__, ep_num);
               }
               else /* USB_EP_DIRECTION_IN */
               {
                  USBDEP_SET_TX_BUF_ADDR(
                     num,
                     port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].start_point
                        + port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset);

                  USBDEP_SB_CLR_TX_COUNT(num);

                  port_stm32_cat_a_dev_interface_shared_mem_start_point[if_num].current_offset += mps;

                  /* set EP control register to enable it */
                  port_stm32_cat_a_io_print_ep_reg_state("-> ", ep_reg[0], __LINE__, ep_num);
                  ep_reg[0] =
                     USBDEP_STM32_CAT_A__CTR_RX | USBDEP_STM32_CAT_A__TX_NAK | USBDEP_STM32_CAT_A__INTERRUPT | (uint32_t)ep_num;
                  port_stm32_cat_a_io_print_ep_reg_state("<- ", ep_reg[0], __LINE__, ep_num);
               }
               break;
#endif

               default:
                  break;
         }

         USBD_DEBUG_HI_5(USBD_DBG_PORT_IO_ONOFF, "EP: %d; dir: %3s; type: %d; reg %s: 0x%08X",
               ep_num, (USB_EP_DIRECTION_OUT == dir) ? "OUT" : "IN", ep_type, "enabled", ep_reg[0]);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO_ONOFF);
} /* port_stm32_cat_a_io_configure */

#endif

/** ******************************************************************************************************************************
 * USBD REQUEST local functions definition
 ********************************************************************************************************************************/
static void port_stm32_cat_a_req_process_incomming_setup(uint8_t ep_num, uint8_t ep_reg_num)
{
   uint32_t *data;
   USBD_REQUEST_Req_DT req;
   uint32_t temp;
#ifdef USBD_USE_IOCMD
   uint32_t temp_before;
   uint32_t temp_after;
#endif
   uint32_t waiting_setup = 0;

   USBD_ENTER_FUNC(USBD_DBG_PORT_REQ);

   data = (uint32_t*)USBDEP_SB_GET_RX_BUFFER_ADDR(ep_reg_num);

   USBD_LOG_DATA_DEBUG_LO_2(USBD_DBG_PORT_REQ, (void*)data, USBDEP_SB_GET_RX_COUNT(ep_reg_num) * 2,
      "rx data context on SETUP for EP: %d; EP reg num: %d", ep_num, ep_reg_num);

   /* read SETUP data from EP */
   req.bmRequestType = ((uint8_t*)data)[0];
   req.bRequest      = ((uint8_t*)data)[1];
   data++;
   req.wValue        = (uint16_t)(*data);
   data++;
   req.wIndex        = (uint16_t)(*data);
   data++;
   req.wLength       = (uint16_t)(*data);

   if(USBD_BMREQUESTTYPE_IN == (req.bmRequestType & USBD_BMREQUESTTYPE_DIRECTION_MASK))
   {
      waiting_setup = USBDEP_STM32_CAT_A__KIND;
   }

   /* release EP to receive next data */
   USBDEP_REG_MODIFY( /* reg = ( (reg ^ _EX_OR) & _AND) | _OR */
      ep_reg_num,
      USBD_STM32_REG->EP_REG[ep_reg_num],
      temp,
      temp_before,
      temp_after,
      /*_EX_OR*/USBDEP_STM32_CAT_A__RX_VALID,
      /*_AND  */USBDEP_STM32_CAT_A__ADDR_KIND_TYPE | USBDEP_STM32_CAT_A__STAT_RX,
      /*_OR   */USBDEP_STM32_CAT_A__CTR_TX | waiting_setup);

   port_stm32_cat_a_req_params.ep_setup_ongoing = ep_num;

   /* process request by USBD core */
   USBD_REQUEST_Process_Req(port_stm32_cat_a_usbd, ep_num, &req);

   port_stm32_cat_a_req_params.ep_setup_ongoing = 0xFF;

#if(USBD_FEATURE_PRESENT == USBD_DEV_SUPPORT_CONFIG_VALIDATION)
   port_stm32_cat_a_req_params.set_configuration_ongoing   = USBD_FALSE;
#endif
#if(USBD_MAX_NUM_ALTERNATE_INTERFACE_SETTINGS > 0)
   port_stm32_cat_a_req_params.interface_set_ongoing       = USBD_MAX_NUM_INTERFACES;
#endif

   USBD_EXIT_FUNC(USBD_DBG_PORT_REQ);
} /* port_stm32_cat_a_req_process_incomming_setup */

static USBD_Bool_DT port_stm32_cat_a_req_set_address (USBD_Params_XT *usbd, uint8_t ep_num, USBD_REQUEST_Req_DT *req)
{
   USBD_UNUSED_PARAM(usbd);
   USBD_UNUSED_PARAM(ep_num);

   USBD_ENTER_FUNC(USBD_DBG_PORT_REQ);

   /* set new device address */
   USBD_STM32_REG->DADDR = USBD_STM32_CAT_A_DADDR_EF | ( ((uint32_t)(req->wValue)) & 0x7F);

   USBD_EXIT_FUNC(USBD_DBG_PORT_REQ);

   return USBD_TRUE;
}

#if(USBD_FEATURE_PRESENT == USBD_DEV_SUPPORT_CONFIG_VALIDATION)
static USBD_Bool_DT port_stm32_cat_a_req_set_configuration (USBD_Params_XT *usbd, uint8_t ep_num, USBD_REQUEST_Req_DT *req)
{
   USBD_UNUSED_PARAM(usbd);
   USBD_UNUSED_PARAM(ep_num);
   USBD_UNUSED_PARAM(req);

   USBD_ENTER_FUNC(USBD_DBG_PORT_REQ);

   port_stm32_cat_a_req_params.set_configuration_ongoing = USBD_TRUE;

   USBD_EXIT_FUNC(USBD_DBG_PORT_REQ);

   return USBD_TRUE;
} /* port_stm32_cat_a_req_set_configuration */
#endif

#if(USBD_MAX_NUM_ALTERNATE_INTERFACE_SETTINGS > 0)
static USBD_Bool_DT port_stm32_cat_a_req_set_interface (USBD_Params_XT *usbd, uint8_t ep_num, USBD_REQUEST_Req_DT *req)
{
   USBD_UNUSED_PARAM(usbd);
   USBD_UNUSED_PARAM(ep_num);

   USBD_ENTER_FUNC(USBD_DBG_PORT_REQ);

   if(USBD_CHECK_PTR(USBD_Params_XT, port_stm32_cat_a_usbd))
   {
      if((uint8_t)(req->wIndex) < USBD_DEV_Get_Num_Interfaces_In_Active_Config(port_stm32_cat_a_usbd))
      {
         port_stm32_cat_a_req_params.interface_set_ongoing = (uint8_t)(req->wIndex);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_REQ);

   return USBD_TRUE;
} /* port_stm32_cat_a_req_set_interface */
#endif




/** ******************************************************************************************************************************
 * PUBLIC INTERFACES
 ********************************************************************************************************************************/

const USBD_DEV_Port_Handler_XT *USBD_Port_STM32_CAT_A_Get_Port(void)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return(&port_stm32_cat_a_dev);
} /* USBD_Port_STM32_CAT_A_Get_Port */



#if (USBD_FEATURE_PRESENT == USBD_PORT_STM32_CAT_A_DETECT_VBUS_CHANGE)
void USBD_Port_STM32_CAT_A_Vbus_Detection(USBD_Bool_DT vbus_5V_present)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_NOTICE_1(USBD_DBG_PORT_DEV, "Vbus changed to %s", USBD_BOOL_IS_TRUE(vbus_5V_present) ? "ON" : "OFF");

   if(USBD_CHECK_PTR(void, port_stm32_cat_a_usbd))
   {
      if(USBD_BOOL_IS_TRUE(vbus_5V_present))
      {
         USBD_DEV_Powered(port_stm32_cat_a_usbd, USBD_TRUE);

         USBD_STM32_CAT_A_RESET_AND_ENABLE_PERIPH();

#if (USBD_FEATURE_PRESENT == USBD_PORT_STM32_CAT_A_USE_PULL_UP)
         USBD_Port_STM32_CAT_A_Pull_Up_Change(USBD_TRUE);
#endif
      }
      else
      {
#if (USBD_FEATURE_PRESENT == USBD_PORT_STM32_CAT_A_USE_PULL_UP)
         USBD_Port_STM32_CAT_A_Pull_Up_Change(USBD_FALSE);
#else
         USBD_ALERT(USBD_DBG_PORT_DEV,"no posibility to disconnect D+ resistor, host will not recognize device disconnection!");
#endif

         USBD_STM32_CAT_A_DISABLE();

         USBD_DEV_Powered(port_stm32_cat_a_usbd, USBD_FALSE);
      }
   }
   else
   {
      USBD_WARN_1(USBD_DBG_PORT_INVALID_PARAMS, "function invalid parameter! usbd: %p", port_stm32_cat_a_usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
} /* USBD_Port_STM32_CAT_A_Vbus_Detection */
#endif

static void port_stm32_cat_a_check_out_on_delay(void)
{
   uint8_t ep_num = 0;
   USBD_Bool_DT keep_out_on_delay_flag = USBD_FALSE;

   do
   {
      if(USBD_BOOL_IS_TRUE(port_stm32_cat_a_io_ep[ep_num].out.out_on_delay))
      {
         if(USBD_BOOL_IS_FALSE(port_stm32_cat_a_io_ep[ep_num].out.is_ep_waiting))
         {
            port_stm32_cat_a_io_ep[ep_num].out.out_on_delay = USBD_FALSE;
            port_stm32_cat_a_io_process_out(ep_num);
         }
         else
         {
            keep_out_on_delay_flag = USBD_TRUE;
         }
      }
      ep_num++;
   }while(ep_num < USBD_MAX_NUM_ENDPOINTS);

   port_stm32_cat_a_dev_prams.out_on_delay = keep_out_on_delay_flag;
} /* port_stm32_cat_a_check_out_on_delay */

void USBD_Port_STM32_CAT_A_Print_HW_Dump(const char *desc, uint16_t line)
{
#ifndef USBD_USE_IOCMD
   USBD_UNUSED_PARAM(desc);
   USBD_UNUSED_PARAM(line);
#else
   USBDEP_STM32_CAT_A_Mem_Descriptor *mem = USBD_STM32_EPMEM;
   uint32_t reg;
   uint_fast8_t ep_reg_num;
#ifdef IOCMD__LINE__LOCAL
#undef IOCMD__LINE__LOCAL
#endif
#define IOCMD__LINE__LOCAL      line

   for(ep_reg_num = 0; ep_reg_num < USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS; ep_reg_num++)
   {
      reg = USBD_STM32_REG->EP_REG[ep_reg_num];
      USBD_EMERG_13(USBD_DBG_PORT_INVALID_PARAMS,
         "%s EP_REG[%d] = 0x%04X (RX:CTR=%d, DTOG=%d, STAT=%s - SETUP=%d, EP_TYPE=%s, EP_KIND=%d - TX:CTR=%d, DTOG=%d, STAT=%s - EA=%d)",
         desc,
         ep_reg_num,
         reg,
         USBD_STM32_CAT_A_REG_GET_1BIT(reg, USBDEP_STM32_CAT_A_BIT__CTR_RX),
         USBD_STM32_CAT_A_REG_GET_1BIT(reg, USBDEP_STM32_CAT_A_BIT__DTOG_RX),
         port_stm32_cat_a_io_transfer_status_string[USBD_STM32_CAT_A_REG_GET_2BIT(reg, USBDEP_STM32_CAT_A_BIT__STAT_RX)],
         USBD_STM32_CAT_A_REG_GET_1BIT(reg, USBDEP_STM32_CAT_A_BIT__SETUP),
         port_stm32_cat_a_io_ep_type_string[USBD_STM32_CAT_A_REG_GET_2BIT(reg, USBDEP_STM32_CAT_A_BIT__TYPE)],
         USBD_STM32_CAT_A_REG_GET_1BIT(reg, USBDEP_STM32_CAT_A_BIT__KIND),
         USBD_STM32_CAT_A_REG_GET_1BIT(reg, USBDEP_STM32_CAT_A_BIT__CTR_TX),
         USBD_STM32_CAT_A_REG_GET_1BIT(reg, USBDEP_STM32_CAT_A_BIT__DTOG_TX),
         port_stm32_cat_a_io_transfer_status_string[USBD_STM32_CAT_A_REG_GET_2BIT(reg, USBDEP_STM32_CAT_A_BIT__STAT_TX)],
         USBD_STM32_CAT_A_REG_GET_4BIT(reg, USBDEP_STM32_CAT_A_BIT__ADDR));
   }

   for(ep_reg_num = 0; ep_reg_num < USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS; ep_reg_num++)
   {
      reg = USBD_STM32_REG->EP_REG[ep_reg_num];
      USBD_EMERG_11(USBD_DBG_PORT_INVALID_PARAMS,
         "%s EP[%d](%s.%s/%s) MEM::   DB[0] / TX: ADDR: 0x%08X, COUNT: 0x%04X(%5d);   DB[1] / RX: ADDR: 0x%08X, COUNT: 0x%04X(%5d)",
         desc,
         ep_reg_num,
         port_stm32_cat_a_io_ep_type_string[USBD_STM32_CAT_A_REG_GET_2BIT(reg, USBDEP_STM32_CAT_A_BIT__TYPE)],
         (0 != USBD_STM32_CAT_A_REG_GET_2BIT(reg, USBDEP_STM32_CAT_A_BIT__STAT_TX)) ? " IN" : " - ",
         (0 != USBD_STM32_CAT_A_REG_GET_2BIT(reg, USBDEP_STM32_CAT_A_BIT__STAT_RX)) ? "OUT" : " - ",
         mem->sb.tx_addr,
         mem->sb.tx_count,
         mem->sb.tx_count,
         mem->sb.rx_addr,
         mem->sb.rx_count,
         mem->sb.rx_count);
      mem++;
   }

   for(ep_reg_num = 0; ep_reg_num < USBD_MAX_NUM_ENDPOINTS; ep_reg_num++)
   {
      USBD_EMERG_15(USBD_DBG_PORT_INVALID_PARAMS,
         "%s EP[%d].OUT:: mps: %4d, ep_type: %d, ep_reg_num: %d; OND: %d, DWFR: %d, IEP: %d, IEW: %d; buf: data: %08X; size: %5d, packet_size: %5d; dma: data: %08X; size: %5d, total_size: %5d",
         desc,
         ep_reg_num,
         port_stm32_cat_a_io_ep[ep_reg_num].out.mps,
         port_stm32_cat_a_io_ep[ep_reg_num].out.ep_type,
         port_stm32_cat_a_io_ep[ep_reg_num].out.hw.ep_reg_num,
         port_stm32_cat_a_io_ep[ep_reg_num].out.out_on_delay,
         port_stm32_cat_a_io_ep[ep_reg_num].out.dont_wait_for_ready,
         port_stm32_cat_a_io_ep[ep_reg_num].out.is_ep_processed,
         port_stm32_cat_a_io_ep[ep_reg_num].out.is_ep_waiting,
         port_stm32_cat_a_io_ep[ep_reg_num].out.buf.data,
         port_stm32_cat_a_io_ep[ep_reg_num].out.buf.size,
         port_stm32_cat_a_io_ep[ep_reg_num].out.buf.packet_size,
         port_stm32_cat_a_io_ep[ep_reg_num].out.dma.data,
         port_stm32_cat_a_io_ep[ep_reg_num].out.dma.size,
         port_stm32_cat_a_io_ep[ep_reg_num].out.dma.total_size);
   }

   for(ep_reg_num = 0; ep_reg_num < USBD_MAX_NUM_ENDPOINTS; ep_reg_num++)
   {
      USBD_EMERG_12(USBD_DBG_PORT_INVALID_PARAMS,
         "%s EP[%d].IN :: mps: %4d, ep_type: %d, ep_reg_num: %d; max_num_bufs: %d, num_used_bufs: %d, data_provided: %d; buf0: size: %5d; buf1: size: %5d; dma: data: %08X; size: %5d",
         desc,
         ep_reg_num,
         port_stm32_cat_a_io_ep[ep_reg_num].in.mps,
         port_stm32_cat_a_io_ep[ep_reg_num].in.ep_type,
         port_stm32_cat_a_io_ep[ep_reg_num].in.hw.ep_reg_num,
         port_stm32_cat_a_io_ep[ep_reg_num].in.max_num_bufs,
         port_stm32_cat_a_io_ep[ep_reg_num].in.num_used_bufs,
         port_stm32_cat_a_io_ep[ep_reg_num].in.data_provided,
         port_stm32_cat_a_io_ep[ep_reg_num].in.buf0.size,
         port_stm32_cat_a_io_ep[ep_reg_num].in.buf1.size,
         port_stm32_cat_a_io_ep[ep_reg_num].in.dma.data,
         port_stm32_cat_a_io_ep[ep_reg_num].in.dma.size);
   }
   USBD_EMERG_2(USBD_DBG_PORT_INVALID_PARAMS,
      "%s CNTR: 0x%04X",
      desc,
      USBD_STM32_REG->CNTR);
   USBD_EMERG_2(USBD_DBG_PORT_INVALID_PARAMS,
      "%s ISTR: 0x%04X",
      desc,
      USBD_STM32_REG->ISTR);
   USBD_EMERG_2(USBD_DBG_PORT_INVALID_PARAMS,
      "%s FNR: 0x%04X",
      desc,
      USBD_STM32_REG->FNR);
   USBD_EMERG_3(USBD_DBG_PORT_INVALID_PARAMS,
      "%s DADDR: 0x%04X (ADDR = %d)",
      desc,
      USBD_STM32_REG->DADDR,
      USBD_STM32_REG->DADDR & USBD_STM32_CAT_A_DADDR_ADD);
   USBD_EMERG_2(USBD_DBG_PORT_INVALID_PARAMS,
      "%s BTABLE: 0x%04X",
      desc,
      USBD_STM32_REG->BTABLE);

#undef IOCMD__LINE__LOCAL
#define IOCMD__LINE__LOCAL      IOCMD__LINE__
#endif
} /* USBD_Port_STM32_CAT_A_Print_HW_Dump */

void USB_LP_CAN1_RX0_IRQHandler(void)
{
#if(1 == USBD_MAX_NUM_ENDPOINTS)
   uint32_t count;
#endif
   uint8_t ep_reg_num = 0;

   USBD_ATOMIC_BOOL_SET(port_stm32_cat_a_irq_active, USBD_TRUE);

   USBD_ENTER_FUNC(USBD_DBG_PORT_IRQ);

   USBD_DEBUG_LO_1(USBD_DBG_PORT_IRQ, "USB_LP; ISTR: %04X", USBD_STM32_REG->ISTR);

   if(USBD_CHECK_PTR(void, port_stm32_cat_a_usbd))
   {
      if(USBD_BOOL_IS_TRUE(port_stm32_cat_a_dev_prams.out_on_delay))
      {
         port_stm32_cat_a_check_out_on_delay();
      }

      /* data transmission */
      if(USBD_STM32_REG->ISTR & USBD_STM32_CAT_A_ISTR_CTR)
      {
         do
         {
            /* OUT */
            if(0 != (USBD_STM32_REG->EP_REG[ep_reg_num] & USBDEP_STM32_CAT_A__CTR_RX))
            {
               USBD_DEBUG_HI_2(USBD_DBG_PORT_IO, "%s IRQ on reg %d", "OUT", ep_reg_num);

               if(port_stm32_cat_a_io_ep_reg_num_ep[ep_reg_num] < USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS)
               {
                  /* normal data transfer */
                  if((USBDEP_STM32_CAT_A__CONTROL | USBDEP_STM32_CAT_A__SETUP)
                     != (USBD_STM32_REG->EP_REG[ep_reg_num] & (USBDEP_STM32_CAT_A__TYPE | USBDEP_STM32_CAT_A__SETUP)))
                  {
                     port_stm32_cat_a_io_process_out(port_stm32_cat_a_io_ep_reg_num_ep[ep_reg_num]);
                  }
                  /* it is control endpoint and SETUP packet has been received */
                  else
                  {
                     port_stm32_cat_a_req_process_incomming_setup(port_stm32_cat_a_io_ep_reg_num_ep[ep_reg_num], ep_reg_num);
                  }
               }
               else
               {
                  USBD_ERROR_1(USBD_DBG_PORT_IRQ, "EP_REG[%d] is active but was not configured! deactivate", ep_reg_num);

                  USBD_STM32_REG->EP_REG[ep_reg_num] &=
                     USBDEP_STM32_CAT_A__DTOG_RX | USBDEP_STM32_CAT_A__STAT_RX | USBDEP_STM32_CAT_A__DTOG_TX | USBDEP_STM32_CAT_A__STAT_TX;
               }
            }
            /* IN */
            else if(0 != (USBD_STM32_REG->EP_REG[ep_reg_num] & USBDEP_STM32_CAT_A__CTR_TX))
            {
               USBD_DEBUG_HI_2(USBD_DBG_PORT_IO, "%s IRQ on reg %d", "IN", ep_reg_num);

               if(port_stm32_cat_a_io_ep_reg_num_ep[ep_reg_num] < USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS)
               {
                  port_stm32_cat_a_io_process_in(port_stm32_cat_a_io_ep_reg_num_ep[ep_reg_num]);
               }
               else
               {
                  USBD_ERROR_1(USBD_DBG_PORT_IRQ, "EP_REG[%d] is active but was not configured! deactivate", ep_reg_num);

                  USBD_STM32_REG->EP_REG[ep_reg_num] &=
                     USBDEP_STM32_CAT_A__DTOG_RX | USBDEP_STM32_CAT_A__STAT_RX | USBDEP_STM32_CAT_A__DTOG_TX | USBDEP_STM32_CAT_A__STAT_TX;
               }
            }

            ep_reg_num++;
         }while((USBD_STM32_REG->ISTR & USBD_STM32_CAT_A_ISTR_CTR)
            && (ep_reg_num < (USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS > USBD_MAX_NUM_ENDPOINTS ? USBD_MAX_NUM_ENDPOINTS : USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS)));
      }
      /* HOST reset IRQ */
      if(USBD_STM32_REG->ISTR & (uint32_t)USBD_STM32_CAT_A_ISTR_RESET)
      {
         /* temporary - disable all USB IRQs */
         USBD_STM32_REG->CNTR = 0;
         /* clear IRQ status */
         USBD_STM32_REG->ISTR &= ~((uint32_t)USBD_STM32_CAT_A_ISTR_RESET);
         /* set default address */
         USBD_STM32_REG->DADDR  = USBD_STM32_CAT_A_DADDR_EF;

         USBD_STM32_REG->EP_REG[0] &= USBDEP_STM32_CAT_A__DTOG_RX | USBDEP_STM32_CAT_A__STAT_RX | USBDEP_STM32_CAT_A__DTOG_TX | USBDEP_STM32_CAT_A__STAT_TX;
         USBD_STM32_REG->EP_REG[1] &= USBDEP_STM32_CAT_A__DTOG_RX | USBDEP_STM32_CAT_A__STAT_RX | USBDEP_STM32_CAT_A__DTOG_TX | USBDEP_STM32_CAT_A__STAT_TX;
         USBD_STM32_REG->EP_REG[2] &= USBDEP_STM32_CAT_A__DTOG_RX | USBDEP_STM32_CAT_A__STAT_RX | USBDEP_STM32_CAT_A__DTOG_TX | USBDEP_STM32_CAT_A__STAT_TX;
         USBD_STM32_REG->EP_REG[3] &= USBDEP_STM32_CAT_A__DTOG_RX | USBDEP_STM32_CAT_A__STAT_RX | USBDEP_STM32_CAT_A__DTOG_TX | USBDEP_STM32_CAT_A__STAT_TX;
         USBD_STM32_REG->EP_REG[4] &= USBDEP_STM32_CAT_A__DTOG_RX | USBDEP_STM32_CAT_A__STAT_RX | USBDEP_STM32_CAT_A__DTOG_TX | USBDEP_STM32_CAT_A__STAT_TX;
         USBD_STM32_REG->EP_REG[5] &= USBDEP_STM32_CAT_A__DTOG_RX | USBDEP_STM32_CAT_A__STAT_RX | USBDEP_STM32_CAT_A__DTOG_TX | USBDEP_STM32_CAT_A__STAT_TX;
         USBD_STM32_REG->EP_REG[6] &= USBDEP_STM32_CAT_A__DTOG_RX | USBDEP_STM32_CAT_A__STAT_RX | USBDEP_STM32_CAT_A__DTOG_TX | USBDEP_STM32_CAT_A__STAT_TX;
         USBD_STM32_REG->EP_REG[7] &= USBDEP_STM32_CAT_A__DTOG_RX | USBDEP_STM32_CAT_A__STAT_RX | USBDEP_STM32_CAT_A__DTOG_TX | USBDEP_STM32_CAT_A__STAT_TX;

         /* set endpoints table offset to 0 */
         USBD_STM32_REG->BTABLE = 0;

         USBDEP_SET_RX_BUF_ADDR(0, USBD_PORT_STM32_CAT_A_EPS_SHARED_MEM_ALOC_TAB_SIZE);
         USBDEP_SET_TX_BUF_ADDR(0, USBD_PORT_STM32_CAT_A_EPS_SHARED_MEM_ALOC_TAB_SIZE + USBD_PORT_STM32_CAT_A_EP0_MPS);

         for(ep_reg_num = 1; ep_reg_num < USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS; ++ep_reg_num)
         {
            USBDEP_SET_TX_BUF_ADDR(ep_reg_num, 0);
            USBDEP_SET_RX_BUF_ADDR(ep_reg_num, 0);
         }

         /* set proper numbers to EP0 registers */
         port_stm32_cat_a_dev_reset_internal_structures();

         /* In case we are using only EP 0 then io_configure callback does not exist, enable EP0 here. */
#if(1 == USBD_MAX_NUM_ENDPOINTS)

         /* set zeroes to all EP fields except EP register number and non-zezroes to specific fields */
         port_stm32_cat_a_io_ep[0].out.buf.size           = (-1);
         port_stm32_cat_a_io_ep[0].out.buf.packet_size    = (-1);

         port_stm32_cat_a_io_ep[0].out.mps                = USBD_PORT_STM32_CAT_A_EP0_MPS;
         port_stm32_cat_a_io_ep[0].out.ep_type            = USB_EP_DESC_TRANSFER_TYPE_CONTROL;

         port_stm32_cat_a_io_ep[0].in.max_num_bufs        = 1;
         port_stm32_cat_a_io_ep[0].in.data_provided       = USBD_FALSE;

         port_stm32_cat_a_io_ep[0].in.mps                 = USBD_PORT_STM32_CAT_A_EP0_MPS;
         port_stm32_cat_a_io_ep[0].in.ep_type             = USB_EP_DESC_TRANSFER_TYPE_CONTROL;

         port_stm32_cat_a_io_ep_reg_num_ep[0] = 0;

         count = USBD_PORT_STM32_CAT_A_EP0_MPS;
         USBDEP_SB_SET_RX_MAX_POSSIBLE_COUNT(0, count);
         USBDEP_SB_CLR_TX_COUNT(0);

         /* set EP control register to enable it */
         USBD_STM32_REG->EP_REG[0] =
            USBDEP_STM32_CAT_A__RX_VALID | USBDEP_STM32_CAT_A__TX_NAK | USBDEP_STM32_CAT_A__CONTROL | (uint32_t)0;

#endif

         /* call USBD engine RESET processor */
         USBD_DEV_Reset(port_stm32_cat_a_usbd);

         /* if USBD_DEV_Reset finished successfully then port is still active - check it before IRQs enabling */
         if(USBD_CHECK_PTR(void, port_stm32_cat_a_usbd))
         {
            /* enable interruptions - data reception, HOST reset, SUSPEND req, SOF reception */
            USBD_STM32_REG->CNTR = USBD_STM32_CAT_A_CNTR_CTRM | USBD_STM32_CAT_A_CNTR_RESETM | USBD_STM32_CAT_A_CNTR_SUSPM
               | USBD_STM32_CAT_A_CNTR_SOFM | USBD_STM32_CAT_A_CNTR_ESOFM;
         }
      }
      /* SOF received/expected */
      if(USBD_STM32_REG->ISTR & (USBD_STM32_CAT_A_ISTR_SOF | USBD_STM32_CAT_A_ISTR_ESOF))
      {
         USBD_STM32_REG->ISTR &= ~((uint32_t)(USBD_STM32_CAT_A_ISTR_SOF | USBD_STM32_CAT_A_ISTR_ESOF));

         USBD_DEV_SOF_Received(port_stm32_cat_a_usbd);
      }
      /* SUSPEND state has been forced by HOST (no SOF received) */
      if(USBD_STM32_REG->ISTR & USBD_STM32_CAT_A_ISTR_SUSP)
      {
         USBD_STM32_REG->ISTR &= ~((uint32_t)USBD_STM32_CAT_A_ISTR_SUSP);
         USBD_STM32_REG->CNTR &= ~((uint32_t)USBD_STM32_CAT_A_CNTR_SUSPM);
         USBD_STM32_REG->CNTR |= USBD_STM32_CAT_A_CNTR_WKUPM | USBD_STM32_CAT_A_CNTR_FSUSP;
         USBD_STM32_REG->CNTR |= USBD_STM32_CAT_A_CNTR_LPMODE;

         USBD_DEV_Suspended(port_stm32_cat_a_usbd);

         // TODO: anything else?
      }
      if(USBD_STM32_REG->ISTR & (USBD_STM32_CAT_A_ISTR_PMAOVR | USBD_STM32_CAT_A_ISTR_ERR))
      {
         static uint8_t dump_limit = 0;
         if(dump_limit < USBD_PORT_STM32_CAT_A_DUMP_LIMIT_PRINTS)
         {
            dump_limit++;
            USBD_EMERG_2(USBD_DBG_PORT_INVALID_PARAMS, "unsupported; PMAOVR: %d, ERR: %d",
               0 != (USBD_STM32_REG->ISTR & USBD_STM32_CAT_A_ISTR_PMAOVR), 0 != (USBD_STM32_REG->ISTR & USBD_STM32_CAT_A_ISTR_ERR));

            USBD_Port_STM32_CAT_A_Print_HW_Dump("STATE", __LINE__);

            if(USBD_PORT_STM32_CAT_A_DUMP_LIMIT_PRINTS == dump_limit)
            {
               USBD_EMERG(USBD_DBG_PORT_INVALID_PARAMS,
                  "Error ocured too many times! this dump will no longer be printed! fix your SW!!!");
            }
         }
         USBD_STM32_REG->ISTR &= ~((uint32_t)(USBD_STM32_CAT_A_ISTR_PMAOVR | USBD_STM32_CAT_A_ISTR_ERR));
         // TODO:
      }
   }
   else
   {
      USBD_EMERG_1(USBD_DBG_PORT_INVALID_PARAMS, "function invalid parameter during IRQ - turn OFF USBD!!!! usbd: %p",
         port_stm32_cat_a_usbd);
      USBD_STM32_REG->ISTR  = 0;
      USBD_STM32_CAT_A_DISABLE();
   }

   if(USBD_BOOL_IS_TRUE(port_stm32_cat_a_dev_prams.out_on_delay))
   {
      port_stm32_cat_a_check_out_on_delay();
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IRQ);

   USBD_ATOMIC_BOOL_SET(port_stm32_cat_a_irq_active, USBD_FALSE);
} /* USB_LP_CAN1_RX0_IRQHandler */



#if(USBD_MAX_NUM_ENDPOINTS > 1)
void USB_HP_CAN1_TX_IRQHandler(void)
{
   uint8_t ep_reg_num = 1;

   USBD_ATOMIC_BOOL_SET(port_stm32_cat_a_irq_active, USBD_TRUE);

   USBD_ENTER_FUNC(USBD_DBG_PORT_IRQ);

   USBD_DEBUG_LO_1(USBD_DBG_PORT_IRQ, "USB_HP; ISTR: %04X", USBD_STM32_REG->ISTR);

   if(0 != USBD_STM32_REG->ISTR)
   {
      do
      {
         /* IN */
         if(0 != (USBD_STM32_REG->EP_REG[ep_reg_num] & USBDEP_STM32_CAT_A__CTR_TX))
         {
            if(port_stm32_cat_a_io_ep_reg_num_ep[ep_reg_num] < USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS)
            {
               port_stm32_cat_a_io_process_in(port_stm32_cat_a_io_ep_reg_num_ep[ep_reg_num]);
            }
            else
            {
               USBD_ERROR_1(USBD_DBG_PORT_IRQ, "EP_REG[%d] is active but was not configured! deactivate", ep_reg_num);

               USBD_STM32_REG->EP_REG[ep_reg_num] &=
                  USBDEP_STM32_CAT_A__DTOG_RX | USBDEP_STM32_CAT_A__STAT_RX | USBDEP_STM32_CAT_A__DTOG_TX | USBDEP_STM32_CAT_A__STAT_TX;
            }
         }
         /* OUT */
         if(0 != (USBD_STM32_REG->EP_REG[ep_reg_num] & USBDEP_STM32_CAT_A__CTR_RX))
         {
            if(port_stm32_cat_a_io_ep_reg_num_ep[ep_reg_num] < USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS)
            {
               /* normal data transfer */
               if((USBDEP_STM32_CAT_A__CONTROL | USBDEP_STM32_CAT_A__SETUP)
                  != (USBD_STM32_REG->EP_REG[ep_reg_num] & (USBDEP_STM32_CAT_A__TYPE | USBDEP_STM32_CAT_A__SETUP)))
               {
                  port_stm32_cat_a_io_process_out(port_stm32_cat_a_io_ep_reg_num_ep[ep_reg_num]);
               }
               /* it is control endpoint and SETUP packet has been received */
               else
               {
                  port_stm32_cat_a_req_process_incomming_setup(port_stm32_cat_a_io_ep_reg_num_ep[ep_reg_num], ep_reg_num);
               }
            }
            else
            {
               USBD_ERROR_1(USBD_DBG_PORT_IRQ, "EP_REG[%d] is active but was not configured! deactivate", ep_reg_num);

               USBD_STM32_REG->EP_REG[ep_reg_num] &=
                  USBDEP_STM32_CAT_A__DTOG_RX | USBDEP_STM32_CAT_A__STAT_RX | USBDEP_STM32_CAT_A__DTOG_TX | USBDEP_STM32_CAT_A__STAT_TX;
            }
         }

         ep_reg_num++;
      }while((USBD_STM32_REG->ISTR & USBD_STM32_CAT_A_ISTR_CTR)
         && (ep_reg_num < (USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS > USBD_MAX_NUM_ENDPOINTS ? USBD_MAX_NUM_ENDPOINTS : USBD_PORT_STM32_CAT_A_MAX_NUM_ENDPOINTS)));
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IRQ);

   USBD_ATOMIC_BOOL_SET(port_stm32_cat_a_irq_active, USBD_FALSE);
} /* USB_HP_CAN1_TX_IRQHandler */
#endif



void USBWakeUp_IRQHandler(void)
{
   USBD_ATOMIC_BOOL_SET(port_stm32_cat_a_irq_active, USBD_TRUE);

   USBD_ENTER_FUNC(USBD_DBG_PORT_IRQ);

   USBD_Port_STM32_CAT_A_Clear_Exti_Line_18_Irq_Status();

   USBD_DEBUG_LO_1(USBD_DBG_PORT_IRQ, "USB_WakeUp; ISTR: %04X", USBD_STM32_REG->ISTR);

   if(0 != USBD_STM32_REG->ISTR)
   {
      USBD_STM32_REG->ISTR &= ~((uint32_t)USBD_STM32_CAT_A_ISTR_WKUP);
      USBD_STM32_REG->CNTR &= ~((uint32_t)USBD_STM32_CAT_A_ISTR_WKUP);
      USBD_STM32_REG->CNTR |= USBD_STM32_CAT_A_CNTR_SUSPM;
      if(0 != USBD_STM32_REG->ISTR)
      {
         USBD_WARN_1(USBD_DBG_PORT_IRQ, "still valid IRQ: 0x%08X", USBD_STM32_REG->ISTR);
      }
      USBD_DEV_Resumed(port_stm32_cat_a_usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IRQ);

   USBD_ATOMIC_BOOL_SET(port_stm32_cat_a_irq_active, USBD_FALSE);
} /* USBWakeUp_IRQHandler */

