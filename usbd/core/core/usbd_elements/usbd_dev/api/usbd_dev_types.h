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

#ifndef USBD_DEV_TYPES_H_
#define USBD_DEV_TYPES_H_

#define USBD_DEV_PRESENT      1
/**
 * Required modules: NONE
 */


/** @defgroup USBD_DEV_TYPES
 * @{
 */

#ifndef USBD_DEV_SUPPORT_CONFIG_VALIDATION
#define USBD_DEV_SUPPORT_CONFIG_VALIDATION            USBD_FEATURE_PRESENT
#endif

#ifndef USBD_DEV_SUPPORTED_SPEED
#define USBD_DEV_SUPPORTED_SPEED                      (USBD_DEV_LOW_SPEED_SUPPORTED | USBD_DEV_FULL_SPEED_SUPPORTED | USBD_DEV_HIGH_SPEED_SUPPORTED)
#endif

#define USBD_DEV_LOW_SPEED_SUPPORTED                  (1 << 0)
#define USBD_DEV_FULL_SPEED_SUPPORTED                 (1 << 1)
#define USBD_DEV_HIGH_SPEED_SUPPORTED                 (1 << 2)
#define USBD_DEV_SUPER_SPEED_SUPPORTED                (1 << 3)

/**
 * \typedef USBD_DEV_States_ET
 * Device state types
 */
typedef enum USBD_DEV_States_Enum_Tag
{
   USBD_DEV_STATE_OFF            = 0,
   /* USBD_DEV_STATE_ATTACHED is not recognizable electrically so no need to define it */
   USBD_DEV_STATE_POWERED        = (1 << 6),
   USBD_DEV_STATE_DEFAULT        = (1 << 5),
   USBD_DEV_STATE_ADDRESSED      = (1 << 4),
   USBD_DEV_STATE_CONFIGURED     = (1 << 3),
   USBD_DEV_STATE_SUSPENDED      = (1 << 2)
}USBD_DEV_States_ET;

typedef enum USBD_DEV_EP_States_Enum_Tag
{
   USBD_DEV_EP_OFF  = 0,
   USBD_DEV_EP_ON   = 1,
   USBD_DEV_EP_HALT = 2
}USBD_DEV_EP_States_ET;

/**
 * \typedef USBD_DEV_Speed_ET
 * Device speed types
 */
typedef enum USBD_DEV_Speed_Enum_Tag
{
   USBD_DEV_SPEED_INVALID        = 0,
   USBD_DEV_LOW_SPEED            = USBD_DEV_LOW_SPEED_SUPPORTED,
   USBD_DEV_FULL_SPEED           = USBD_DEV_FULL_SPEED_SUPPORTED,
   USBD_DEV_HIGH_SPEED           = USBD_DEV_HIGH_SPEED_SUPPORTED,
   USBD_DEV_FULL_AND_HIGH_SPEED  = (USBD_DEV_HIGH_SPEED | USBD_DEV_FULL_SPEED),
   USBD_DEV_SPEED_MASK           = (USBD_DEV_HIGH_SPEED | USBD_DEV_FULL_SPEED | USBD_DEV_LOW_SPEED)
}USBD_DEV_Speed_ET;


typedef enum USBD_DEV_Installation_Result_Enum_Tag
{
   USBD_DEV_INSTALLATION_OK                              = 0,
   USBD_DEV_INSTALLATION_INVALID_PTR                     = 1,
   USBD_DEV_INSTALLATION_FORBIDDEN_STATE                 = 2,
   USBD_DEV_INSTALLATION_DATA_INVALID_STRUCTURE          = 3,
   USBD_DEV_INSTALLATION_INVALID_PARAMS                  = 4,
   USBD_DEV_INSTALLATION_INDEX_OUT_OF_RANGE              = 5
}USBD_DEV_Installation_Result_ET;

typedef enum USBD_DEV_Config_Desc_Check_Result_Enum_Tag
{
   USBD_DEV_CONFIG_DESC_OK                               = 0,
   USBD_DEV_CONFIG_DESC_INVALID_PARAMS                   = 1,
   USBD_DEV_CONFIG_DESC_INVALID_CONFIG_HEADER            = 2,
   USBD_DEV_CONFIG_DESC_INVALID_NUM_INTERFACES           = 3,
   USBD_DEV_CONFIG_DESC_INVALID_TOTAL_LENGTH             = 4,
   USBD_DEV_CONFIG_DESC_INVALID_INTERFACE_HEADER         = 5,
   USBD_DEV_CONFIG_DESC_INVALID_INTERFACE_NOT_FOUND      = 6,
   USBD_DEV_CONFIG_DESC_INVALID_INTERFACE_NUM            = 7,
   USBD_DEV_CONFIG_DESC_INVALID_ENDPOINT_HEADER          = 8,
   USBD_DEV_CONFIG_DESC_INVALID_ENDPOINT_NUM             = 9,
   USBD_DEV_CONFIG_DESC_INVALID_ENDPOINT_NOT_FOUND       = 10,
   USBD_DEV_CONFIG_DESC_INVALID_ENDPOINT_OWNER           = 11,
   USBD_DEV_CONFIG_DESC_INVALID_ENDPOINT_TYPE_MISMATCH   = 12,
   USBD_DEV_CONFIG_DESC_INVALID_ENDPOINT_PARAMS_FOR_HW   = 13,
   USBD_DEV_CONFIG_DESC_INVALID_CONFIG_STRUCTURE_FOR_HW  = 14
}USBD_DEV_Config_Desc_Check_Result_ET;

/**
 * \typedef USBD_DEV_Set_Interface_Result_ET
 * Possible results of function @see USBD_DEV_Set_Interface
 */
typedef enum USBD_DEV_Set_Interface_Result_Enum_Tag
{
   /** alternate setting was set correctly */
   USBD_DEV_SET_INTERFACE_RESULT_OK                      = 0,
   /** alternate setting or interface was not found */
   USBD_DEV_SET_INTERFACE_RESULT_SETTING_NOT_FOUND       = 1,
   /** alternate setting is not supported by current configuration/interface */
   USBD_DEV_SET_INTERFACE_RESULT_ALT_SETTING_NOT_SUPPORTED = 2
}USBD_DEV_Set_Interface_Result_ET;



/**
 * \struct USBD_DEV_Config_Desc_Check_Result_XT
 * This structure is returned as the result of configuration descriptor checking
 */
typedef struct USBD_DEV_Config_Desc_Check_Result_eXtended_Tag
{
   /**
    * result of descriptor parsing. possible values are defined
    * by enumerator @see USBD_DEV_Config_Desc_Check_Result_ET
    */
   uint16_t result;
   /**
    * index - place in table for which priblem is reported, 0xFFFF if not valid
    */
   uint16_t index;
   /**
    * number of interface for which problem is reported, 0xFF if not valid
    */
   uint8_t if_num;
   /**
    * number of version of interface (alternative setting) for which problem is reported, 0xFF if not valid
    */
   uint8_t if_alt;
   /**
    * number of endpoint for which problem is reported, 0xFF if not valid
    */
   uint8_t ep_num;
   /**
    * direction of endpoint for which problem is reported, if ep_num is equal to 0xFF then ep_dir is invalid
    */
   USB_EP_Direction_ET ep_dir;
}USBD_DEV_Config_Desc_Check_Result_XT;



/**
 * \typedef USBD_DEV_PORT_Activate_Deactivate_HT
 * Handler to function which have to exist in low level layer of software (port).
 * Called by USBD to connect universal part of usb software (usbd) to hardware
 * and to initialize/deinitialize hardware.
 *
 * \param usbd pointer to USBD structure
 */
typedef void (*USBD_DEV_PORT_Activate_Deactivate_HT)            (struct USBD_params_eXtended_Tag* usbd, USBD_Bool_DT state);

/**
 * \typedef USBD_DEV_PORT_Parse_EP_Desc_Variants_HT
 * Handler to function which can exist in low level layer of software (port).
 * Called by USBD to parse separated endpoint by port tocheck if it is possible to work with it.
 * If parsing procedure has depedency to other endpoints then it is recommended
 * to implement USBD_DEV_PORT_Parse_Cfg_Desc_HT instead.
 *
 * \param usbd pointer to USBD structure
 * \param ep_num number of endpoint for which descriptor is checked
 * \param if_desc pointer to interface descriptor for which parsed endpoint belongs
 * \param ep_desc pointer to endpoint descriptor which has to be parsed
 *
 * \return OSBD_TRUE if parsing succeeded, USBD_FALSE when failed.
 */
typedef USBD_Bool_DT (*USBD_DEV_PORT_Parse_EP_Desc_Variants_HT) (struct USBD_params_eXtended_Tag* usbd, uint8_t ep_num, const USB_Interface_Desc_DT *if_desc, const USB_Endpoint_Desc_DT *ep_desc);

/**
 * \typedef USBD_DEV_PORT_Parse_Cfg_Desc_HT
 * Handler to function which can exist in low level layer of software (port).
 * Called by USBD to parse whole configuration descriptor by port to check if it is possible to work with it.
 * If port doesn't need to parse while descriptor but it is enough to parse separately endpoints only
 * then it is recommended to use USBD_DEV_PORT_Parse_EP_Desc_Variants_HT instead.
 *
 * \param usbd pointer to USBD structure
 * \param desc pointer to configuration descriptor to be paarsed
 * \param desc_size size of configuration descriptor
 * \param details pointer to structure in which parsing details shall be returned
 *
 * \return OSBD_TRUE if parsing succeeded, USBD_FALSE when failed.
 */
typedef USBD_Bool_DT (*USBD_DEV_PORT_Parse_Cfg_Desc_HT)         (struct USBD_params_eXtended_Tag* usbd, const uint8_t *desc, uint16_t desc_size, USBD_DEV_Config_Desc_Check_Result_XT *details);

/**
 * \typedef USBD_DEV_Set_Configuration_Respond_HT
 * Handler to function responsible to send STATUS on SET_CONFIGURATION. This function handler shall be provided as paramter
 * of function USBD_DEV_Configured and will be called from inside of USBD_DEV_Configured.
 * The reason of not sending response after calling USBD_DEV_Configured is that USBD_DEV_Configured takes a lot of time
 * by calling upper layer events.
 */
typedef void (*USBD_DEV_Set_Configuration_Respond_HT)           (struct USBD_params_eXtended_Tag* usbd);

/**
 * \typedef USBD_DEV_PORT_Get_Speed_HT
 * Handler to function which have to exist in low level layer of software (port).
 * Called by USBD to get curent speed/suppored speed types of the device
 *
 * \param usbd pointer to USBD structure
 */
typedef USBD_DEV_Speed_ET (*USBD_DEV_PORT_Get_Speed_HT)         (struct USBD_params_eXtended_Tag* usbd);

/**
 * \typedef USBD_DEV_PORT_Get_U16_Data_HT
 * Handler to function which have to exist in low level layer of software (port).
 * Called by USBD to get device status or frame number
 *
 * \param usbd pointer to USBD structure
 */
typedef uint16_t (*USBD_DEV_PORT_Get_U16_Data_HT)               (struct USBD_params_eXtended_Tag* usbd);

typedef const USB_Endpoint_Desc_DT* USBD_DEV_PORT_Get_EP_Desc_Result_DT;
/**
 * \typedef USBD_DEV_PORT_Get_EP_Desc_HT
 * Handler to function which have to exist in low level layer of software (port).
 * Called by USBD to get pointer to endpoint 0 descriptor
 *
 * \param usbd pointer to USBD structure
 */
typedef USBD_DEV_PORT_Get_EP_Desc_Result_DT (*USBD_DEV_PORT_Get_EP_Desc_HT) (struct USBD_params_eXtended_Tag* usbd);


/**
 * \struct USBD_DEV_String_Header_XT
 */
typedef struct USBD_DEV_String_Header_eXtended_Tag
{
   const uint8_t                      *string;
}USBD_DEV_String_Header_XT;

/**
 * \struct USBD_DEV_Config_Header_XT
 */
typedef struct USBD_DEV_Config_Header_eXtended_Tag
{
   struct USBDC_params_eXtended_Tag   *usbdc;
   const uint8_t                      *config_desc;
}USBD_DEV_Config_Header_XT;

/**
 * \struct USBD_DEV_Interface_Header_XT
 */
typedef struct USBD_DEV_Interface_Header_eXtended_Tag
{
   /** this pointer is set by usbd core */
   const USB_Interface_Desc_DT        *desc;
}USBD_DEV_Interface_Header_XT;



/**
 * \struct USBD_DEV_Port_Handler_XT
 * This structure has to be declared in PORT layer.
 * It is used to attach USBD engine to hardware.
 */
typedef struct USBD_DEV_Port_Handler_eXtended_Tag
{
   struct
   {
      USBD_DEV_PORT_Activate_Deactivate_HT      activate;
#if(USBD_FEATURE_PRESENT == USBD_DEV_SUPPORT_CONFIG_VALIDATION)
      USBD_DEV_PORT_Parse_EP_Desc_Variants_HT   ep_parse;
      USBD_DEV_PORT_Parse_Cfg_Desc_HT           cfg_parse;
#endif
#if((USBD_DEV_LOW_SPEED_SUPPORTED != USBD_DEV_SUPPORTED_SPEED) && (USBD_DEV_FULL_SPEED_SUPPORTED != USBD_DEV_SUPPORTED_SPEED))
      USBD_DEV_PORT_Get_Speed_HT                get_supported_speed;
      USBD_DEV_PORT_Get_Speed_HT                get_current_speed;
#endif
      USBD_DEV_PORT_Get_U16_Data_HT             get_device_status;
      USBD_DEV_PORT_Get_U16_Data_HT             get_frame_num;
      USBD_DEV_PORT_Get_EP_Desc_HT              get_ep0_low_full_speed;
#if(USBD_DEV_SUPPORTED_SPEED >= USBD_DEV_HIGH_SPEED_SUPPORTED)
      USBD_DEV_PORT_Get_EP_Desc_HT              get_ep0_high_speed;
#endif
   }handlers;
   struct
   {
      USBD_Bool_DT                              ep_both_directions_must_have_same_type;
   }data;
}USBD_DEV_Port_Handler_XT;



/**
 * \struct USBD_DEV_Installation_Result_XT
 * This structure is returned as the result of string or configuration installation.
 */
typedef struct USBD_DEV_Installation_Result_eXtended_Tag
{
   /**
    * result of installation. possible values are defined
    * by enumerator @see USBD_DEV_Installation_Result_ET
    */
   uint8_t result;
   /**
    * index - place in table where object was installed
    */
   uint8_t index;
}USBD_DEV_Installation_Result_XT;




/**
 * \struct USBD_DEV_Params_XT
 * DEV part of USBD
 */
typedef struct USBD_DEV_Params_eXtended_Tag
{
   struct
   {
      struct
      {
         const USBD_DEV_Port_Handler_XT     *port;
#if(USBD_MAX_NUM_STRINGS > 0)
         USBD_DEV_String_Header_XT           string_tab[USBD_MAX_NUM_STRINGS];
#endif
         USBD_DEV_Config_Header_XT           config_tab[USBD_MAX_NUM_CONFIGURATIONS];
         USBD_DEV_Interface_Header_XT        interface_tab[USBD_MAX_NUM_INTERFACES];
         struct USBDC_params_eXtended_Tag   *active_config;
         const uint8_t                      *active_config_desc;
         struct
         {
            const USB_Endpoint_Desc_DT      *ep_in_desc;
            const USB_Endpoint_Desc_DT      *ep_out_desc;
         }ep[USBD_MAX_NUM_ENDPOINTS];
         USB_Device_Desc_DT                  dev_desc;
         USBD_DEV_States_ET                  state;
#if(USBD_MAX_NUM_ENDPOINTS > 1)
         uint8_t                             num_used_endpoints;
         uint8_t                             ep_in_interface[USBD_MAX_NUM_ENDPOINTS];
         uint8_t                             ep_out_interface[USBD_MAX_NUM_ENDPOINTS];
#endif
#if(USBD_MAX_NUM_STRINGS > 0)
         uint8_t                             num_installed_strings;
#endif
         uint8_t                             active_config_num;
         /* USBD_DEV_EP_States_ET */
         uint8_t                             ep_in_state[USBD_MAX_NUM_ENDPOINTS];
         uint8_t                             ep_out_state[USBD_MAX_NUM_ENDPOINTS];
         USBD_Bool_DT                        active;
      }data;
   }core;
}USBD_DEV_Params_XT;

/** @} */ /* USBD_DEV_TYPES */


#endif
