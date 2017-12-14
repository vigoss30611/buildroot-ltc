/**
 * @file ctx_reg_precisions.h
 * @brief Auto-generated files containing all context register's precisions
 */

/// @brief Can be used to have shorter access to registers precisions using Module Config IMG_Fix_Clip() function
#define PREC(REG) REG##_INT, REG##_FRAC, REG##_SIGNED, #REG
/// @brief Can be used to have shorter access to registers precisions using Module Config IMG_clip() function
#define PREC_C(REG) REG##_INT+ REG##_FRAC, REG##_SIGNED, #REG


// CONTEXT_CONFIG
#define MAX_CONTEXT_WIDTH_SGL_FRAC 		0 ///< @brief number of fractional bits of MAX_CONTEXT_WIDTH_SGL
#define MAX_CONTEXT_WIDTH_SGL_INT 		16 ///< @brief number of integer bits of MAX_CONTEXT_WIDTH_SGL
#define MAX_CONTEXT_WIDTH_SGL_SIGNED 		0 ///< @brief is MAX_CONTEXT_WIDTH_SGL signed or unsigned

#define CONTEXT_BIT_DEPTH_FRAC 		0 ///< @brief number of fractional bits of CONTEXT_BIT_DEPTH
#define CONTEXT_BIT_DEPTH_INT 		3 ///< @brief number of integer bits of CONTEXT_BIT_DEPTH
#define CONTEXT_BIT_DEPTH_SIGNED 		0 ///< @brief is CONTEXT_BIT_DEPTH signed or unsigned


// CONTEXT_CONFIG_2
#define MAX_CONTEXT_HEIGHT_FRAC 		0 ///< @brief number of fractional bits of MAX_CONTEXT_HEIGHT
#define MAX_CONTEXT_HEIGHT_INT 		16 ///< @brief number of integer bits of MAX_CONTEXT_HEIGHT
#define MAX_CONTEXT_HEIGHT_SIGNED 		0 ///< @brief is MAX_CONTEXT_HEIGHT signed or unsigned

#define MAX_CONTEXT_WIDTH_MULT_FRAC 		0 ///< @brief number of fractional bits of MAX_CONTEXT_WIDTH_MULT
#define MAX_CONTEXT_WIDTH_MULT_INT 		16 ///< @brief number of integer bits of MAX_CONTEXT_WIDTH_MULT
#define MAX_CONTEXT_WIDTH_MULT_SIGNED 		0 ///< @brief is MAX_CONTEXT_WIDTH_MULT signed or unsigned


// INTERRUPT_STATUS
#define INT_START_OF_FRAME_RECEIVED_FRAC 		0 ///< @brief number of fractional bits of INT_START_OF_FRAME_RECEIVED
#define INT_START_OF_FRAME_RECEIVED_INT 		1 ///< @brief number of integer bits of INT_START_OF_FRAME_RECEIVED
#define INT_START_OF_FRAME_RECEIVED_SIGNED 		0 ///< @brief is INT_START_OF_FRAME_RECEIVED signed or unsigned

#define INT_START_OF_FRAME_SELECTED_FRAC 		0 ///< @brief number of fractional bits of INT_START_OF_FRAME_SELECTED
#define INT_START_OF_FRAME_SELECTED_INT 		1 ///< @brief number of integer bits of INT_START_OF_FRAME_SELECTED
#define INT_START_OF_FRAME_SELECTED_SIGNED 		0 ///< @brief is INT_START_OF_FRAME_SELECTED signed or unsigned

#define INT_CONFIGURATION_LOADED_FRAC 		0 ///< @brief number of fractional bits of INT_CONFIGURATION_LOADED
#define INT_CONFIGURATION_LOADED_INT 		1 ///< @brief number of integer bits of INT_CONFIGURATION_LOADED
#define INT_CONFIGURATION_LOADED_SIGNED 		0 ///< @brief is INT_CONFIGURATION_LOADED signed or unsigned

#define INT_FRAME_DONE_DE_FRAC 		0 ///< @brief number of fractional bits of INT_FRAME_DONE_DE
#define INT_FRAME_DONE_DE_INT 		1 ///< @brief number of integer bits of INT_FRAME_DONE_DE
#define INT_FRAME_DONE_DE_SIGNED 		0 ///< @brief is INT_FRAME_DONE_DE signed or unsigned

#define INT_FRAME_DONE_ENC_FRAC 		0 ///< @brief number of fractional bits of INT_FRAME_DONE_ENC
#define INT_FRAME_DONE_ENC_INT 		1 ///< @brief number of integer bits of INT_FRAME_DONE_ENC
#define INT_FRAME_DONE_ENC_SIGNED 		0 ///< @brief is INT_FRAME_DONE_ENC signed or unsigned

#define INT_FRAME_DONE_DISP_FRAC 		0 ///< @brief number of fractional bits of INT_FRAME_DONE_DISP
#define INT_FRAME_DONE_DISP_INT 		1 ///< @brief number of integer bits of INT_FRAME_DONE_DISP
#define INT_FRAME_DONE_DISP_SIGNED 		0 ///< @brief is INT_FRAME_DONE_DISP signed or unsigned

#define INT_FRAME_DONE_ALL_FRAC 		0 ///< @brief number of fractional bits of INT_FRAME_DONE_ALL
#define INT_FRAME_DONE_ALL_INT 		1 ///< @brief number of integer bits of INT_FRAME_DONE_ALL
#define INT_FRAME_DONE_ALL_SIGNED 		0 ///< @brief is INT_FRAME_DONE_ALL signed or unsigned

#define INT_ERROR_IGNORE_FRAC 		0 ///< @brief number of fractional bits of INT_ERROR_IGNORE
#define INT_ERROR_IGNORE_INT 		1 ///< @brief number of integer bits of INT_ERROR_IGNORE
#define INT_ERROR_IGNORE_SIGNED 		0 ///< @brief is INT_ERROR_IGNORE signed or unsigned

#define INT_POS_Y_INC_FRAC 		0 ///< @brief number of fractional bits of INT_POS_Y_INC
#define INT_POS_Y_INC_INT 		8 ///< @brief number of integer bits of INT_POS_Y_INC
#define INT_POS_Y_INC_SIGNED 		0 ///< @brief is INT_POS_Y_INC signed or unsigned


// INTERRUPT_ENABLE
#define INT_EN_START_OF_FRAME_RECEIVED_FRAC 		0 ///< @brief number of fractional bits of INT_EN_START_OF_FRAME_RECEIVED
#define INT_EN_START_OF_FRAME_RECEIVED_INT 		1 ///< @brief number of integer bits of INT_EN_START_OF_FRAME_RECEIVED
#define INT_EN_START_OF_FRAME_RECEIVED_SIGNED 		0 ///< @brief is INT_EN_START_OF_FRAME_RECEIVED signed or unsigned

#define INT_EN_START_OF_FRAME_SELECTED_FRAC 		0 ///< @brief number of fractional bits of INT_EN_START_OF_FRAME_SELECTED
#define INT_EN_START_OF_FRAME_SELECTED_INT 		1 ///< @brief number of integer bits of INT_EN_START_OF_FRAME_SELECTED
#define INT_EN_START_OF_FRAME_SELECTED_SIGNED 		0 ///< @brief is INT_EN_START_OF_FRAME_SELECTED signed or unsigned

#define INT_EN_CONFIGURATION_LOADED_FRAC 		0 ///< @brief number of fractional bits of INT_EN_CONFIGURATION_LOADED
#define INT_EN_CONFIGURATION_LOADED_INT 		1 ///< @brief number of integer bits of INT_EN_CONFIGURATION_LOADED
#define INT_EN_CONFIGURATION_LOADED_SIGNED 		0 ///< @brief is INT_EN_CONFIGURATION_LOADED signed or unsigned

#define INT_EN_FRAME_DONE_DE_FRAC 		0 ///< @brief number of fractional bits of INT_EN_FRAME_DONE_DE
#define INT_EN_FRAME_DONE_DE_INT 		1 ///< @brief number of integer bits of INT_EN_FRAME_DONE_DE
#define INT_EN_FRAME_DONE_DE_SIGNED 		0 ///< @brief is INT_EN_FRAME_DONE_DE signed or unsigned

#define INT_EN_FRAME_DONE_ENC_FRAC 		0 ///< @brief number of fractional bits of INT_EN_FRAME_DONE_ENC
#define INT_EN_FRAME_DONE_ENC_INT 		1 ///< @brief number of integer bits of INT_EN_FRAME_DONE_ENC
#define INT_EN_FRAME_DONE_ENC_SIGNED 		0 ///< @brief is INT_EN_FRAME_DONE_ENC signed or unsigned

#define INT_EN_FRAME_DONE_DISP_FRAC 		0 ///< @brief number of fractional bits of INT_EN_FRAME_DONE_DISP
#define INT_EN_FRAME_DONE_DISP_INT 		1 ///< @brief number of integer bits of INT_EN_FRAME_DONE_DISP
#define INT_EN_FRAME_DONE_DISP_SIGNED 		0 ///< @brief is INT_EN_FRAME_DONE_DISP signed or unsigned

#define INT_EN_FRAME_DONE_ALL_FRAC 		0 ///< @brief number of fractional bits of INT_EN_FRAME_DONE_ALL
#define INT_EN_FRAME_DONE_ALL_INT 		1 ///< @brief number of integer bits of INT_EN_FRAME_DONE_ALL
#define INT_EN_FRAME_DONE_ALL_SIGNED 		0 ///< @brief is INT_EN_FRAME_DONE_ALL signed or unsigned

#define INT_EN_ERROR_IGNORE_FRAC 		0 ///< @brief number of fractional bits of INT_EN_ERROR_IGNORE
#define INT_EN_ERROR_IGNORE_INT 		1 ///< @brief number of integer bits of INT_EN_ERROR_IGNORE
#define INT_EN_ERROR_IGNORE_SIGNED 		0 ///< @brief is INT_EN_ERROR_IGNORE signed or unsigned

#define INT_EN_POS_Y_INC_FRAC 		0 ///< @brief number of fractional bits of INT_EN_POS_Y_INC
#define INT_EN_POS_Y_INC_INT 		8 ///< @brief number of integer bits of INT_EN_POS_Y_INC
#define INT_EN_POS_Y_INC_SIGNED 		0 ///< @brief is INT_EN_POS_Y_INC signed or unsigned


// INTERRUPT_CLEAR
#define INT_CL_START_OF_FRAME_RECEIVED_FRAC 		0 ///< @brief number of fractional bits of INT_CL_START_OF_FRAME_RECEIVED
#define INT_CL_START_OF_FRAME_RECEIVED_INT 		1 ///< @brief number of integer bits of INT_CL_START_OF_FRAME_RECEIVED
#define INT_CL_START_OF_FRAME_RECEIVED_SIGNED 		0 ///< @brief is INT_CL_START_OF_FRAME_RECEIVED signed or unsigned

#define INT_CL_START_OF_FRAME_SELECTED_FRAC 		0 ///< @brief number of fractional bits of INT_CL_START_OF_FRAME_SELECTED
#define INT_CL_START_OF_FRAME_SELECTED_INT 		1 ///< @brief number of integer bits of INT_CL_START_OF_FRAME_SELECTED
#define INT_CL_START_OF_FRAME_SELECTED_SIGNED 		0 ///< @brief is INT_CL_START_OF_FRAME_SELECTED signed or unsigned

#define INT_CL_CONFIGURATION_LOADED_FRAC 		0 ///< @brief number of fractional bits of INT_CL_CONFIGURATION_LOADED
#define INT_CL_CONFIGURATION_LOADED_INT 		1 ///< @brief number of integer bits of INT_CL_CONFIGURATION_LOADED
#define INT_CL_CONFIGURATION_LOADED_SIGNED 		0 ///< @brief is INT_CL_CONFIGURATION_LOADED signed or unsigned

#define INT_CL_FRAME_DONE_DE_FRAC 		0 ///< @brief number of fractional bits of INT_CL_FRAME_DONE_DE
#define INT_CL_FRAME_DONE_DE_INT 		1 ///< @brief number of integer bits of INT_CL_FRAME_DONE_DE
#define INT_CL_FRAME_DONE_DE_SIGNED 		0 ///< @brief is INT_CL_FRAME_DONE_DE signed or unsigned

#define INT_CL_FRAME_DONE_ENC_FRAC 		0 ///< @brief number of fractional bits of INT_CL_FRAME_DONE_ENC
#define INT_CL_FRAME_DONE_ENC_INT 		1 ///< @brief number of integer bits of INT_CL_FRAME_DONE_ENC
#define INT_CL_FRAME_DONE_ENC_SIGNED 		0 ///< @brief is INT_CL_FRAME_DONE_ENC signed or unsigned

#define INT_CL_FRAME_DONE_DISP_FRAC 		0 ///< @brief number of fractional bits of INT_CL_FRAME_DONE_DISP
#define INT_CL_FRAME_DONE_DISP_INT 		1 ///< @brief number of integer bits of INT_CL_FRAME_DONE_DISP
#define INT_CL_FRAME_DONE_DISP_SIGNED 		0 ///< @brief is INT_CL_FRAME_DONE_DISP signed or unsigned

#define INT_CL_FRAME_DONE_ALL_FRAC 		0 ///< @brief number of fractional bits of INT_CL_FRAME_DONE_ALL
#define INT_CL_FRAME_DONE_ALL_INT 		1 ///< @brief number of integer bits of INT_CL_FRAME_DONE_ALL
#define INT_CL_FRAME_DONE_ALL_SIGNED 		0 ///< @brief is INT_CL_FRAME_DONE_ALL signed or unsigned

#define INT_CL_ERROR_IGNORE_FRAC 		0 ///< @brief number of fractional bits of INT_CL_ERROR_IGNORE
#define INT_CL_ERROR_IGNORE_INT 		1 ///< @brief number of integer bits of INT_CL_ERROR_IGNORE
#define INT_CL_ERROR_IGNORE_SIGNED 		0 ///< @brief is INT_CL_ERROR_IGNORE signed or unsigned

#define INT_CL_POS_Y_INC_FRAC 		0 ///< @brief number of fractional bits of INT_CL_POS_Y_INC
#define INT_CL_POS_Y_INC_INT 		8 ///< @brief number of integer bits of INT_CL_POS_Y_INC
#define INT_CL_POS_Y_INC_SIGNED 		0 ///< @brief is INT_CL_POS_Y_INC signed or unsigned


// CONTEXT_STATUS
#define CONTEXT_STATE_FRAC 		0 ///< @brief number of fractional bits of CONTEXT_STATE
#define CONTEXT_STATE_INT 		2 ///< @brief number of integer bits of CONTEXT_STATE
#define CONTEXT_STATE_SIGNED 		0 ///< @brief is CONTEXT_STATE signed or unsigned


// LAST_FRAME_INFO
#define LAST_CONTEXT_FRAMES_FRAC 		0 ///< @brief number of fractional bits of LAST_CONTEXT_FRAMES
#define LAST_CONTEXT_FRAMES_INT 		8 ///< @brief number of integer bits of LAST_CONTEXT_FRAMES
#define LAST_CONTEXT_FRAMES_SIGNED 		0 ///< @brief is LAST_CONTEXT_FRAMES signed or unsigned

#define LAST_CONTEXT_ERRORS_FRAC 		0 ///< @brief number of fractional bits of LAST_CONTEXT_ERRORS
#define LAST_CONTEXT_ERRORS_INT 		8 ///< @brief number of integer bits of LAST_CONTEXT_ERRORS
#define LAST_CONTEXT_ERRORS_SIGNED 		0 ///< @brief is LAST_CONTEXT_ERRORS signed or unsigned

#define LAST_CONTEXT_TAG_FRAC 		0 ///< @brief number of fractional bits of LAST_CONTEXT_TAG
#define LAST_CONTEXT_TAG_INT 		8 ///< @brief number of integer bits of LAST_CONTEXT_TAG
#define LAST_CONTEXT_TAG_SIGNED 		0 ///< @brief is LAST_CONTEXT_TAG signed or unsigned

#define LAST_CONFIG_TAG_FRAC 		0 ///< @brief number of fractional bits of LAST_CONFIG_TAG
#define LAST_CONFIG_TAG_INT 		8 ///< @brief number of integer bits of LAST_CONFIG_TAG
#define LAST_CONFIG_TAG_SIGNED 		0 ///< @brief is LAST_CONFIG_TAG signed or unsigned


// POS_Y_INT_CTRL
#define POS_Y_INT_CTRL_EXP_MIN2_FRAC 		0 ///< @brief number of fractional bits of POS_Y_INT_CTRL_EXP_MIN2
#define POS_Y_INT_CTRL_EXP_MIN2_INT 		16 ///< @brief number of integer bits of POS_Y_INT_CTRL_EXP_MIN2
#define POS_Y_INT_CTRL_EXP_MIN2_SIGNED 		0 ///< @brief is POS_Y_INT_CTRL_EXP_MIN2 signed or unsigned


// GCTRL_POS_Y
#define GCTRL_POS_Y_FRAC 		0 ///< @brief number of fractional bits of GCTRL_POS_Y
#define GCTRL_POS_Y_INT 		17 ///< @brief number of integer bits of GCTRL_POS_Y
#define GCTRL_POS_Y_SIGNED 		0 ///< @brief is GCTRL_POS_Y signed or unsigned


// CONTEXT_CONTROL
#define CAPTURE_MODE_FRAC 		0 ///< @brief number of fractional bits of CAPTURE_MODE
#define CAPTURE_MODE_INT 		2 ///< @brief number of integer bits of CAPTURE_MODE
#define CAPTURE_MODE_SIGNED 		0 ///< @brief is CAPTURE_MODE signed or unsigned

#define CAPTURE_STOP_FRAC 		0 ///< @brief number of fractional bits of CAPTURE_STOP
#define CAPTURE_STOP_INT 		1 ///< @brief number of integer bits of CAPTURE_STOP
#define CAPTURE_STOP_SIGNED 		0 ///< @brief is CAPTURE_STOP signed or unsigned

#define COUNTERS_CLEAR_FRAC 		0 ///< @brief number of fractional bits of COUNTERS_CLEAR
#define COUNTERS_CLEAR_INT 		1 ///< @brief number of integer bits of COUNTERS_CLEAR
#define COUNTERS_CLEAR_SIGNED 		0 ///< @brief is COUNTERS_CLEAR signed or unsigned

#define CRC_CLEAR_FRAC 		0 ///< @brief number of fractional bits of CRC_CLEAR
#define CRC_CLEAR_INT 		1 ///< @brief number of integer bits of CRC_CLEAR
#define CRC_CLEAR_SIGNED 		0 ///< @brief is CRC_CLEAR signed or unsigned

#define LINK_LIST_CLEAR_FRAC 		0 ///< @brief number of fractional bits of LINK_LIST_CLEAR
#define LINK_LIST_CLEAR_INT 		1 ///< @brief number of integer bits of LINK_LIST_CLEAR
#define LINK_LIST_CLEAR_SIGNED 		0 ///< @brief is LINK_LIST_CLEAR signed or unsigned


// MEMORY_CONTROL
#define ENC_CACHE_POLICY_FRAC 		0 ///< @brief number of fractional bits of ENC_CACHE_POLICY
#define ENC_CACHE_POLICY_INT 		2 ///< @brief number of integer bits of ENC_CACHE_POLICY
#define ENC_CACHE_POLICY_SIGNED 		0 ///< @brief is ENC_CACHE_POLICY signed or unsigned

#define DISP_DE_CACHE_POLICY_FRAC 		0 ///< @brief number of fractional bits of DISP_DE_CACHE_POLICY
#define DISP_DE_CACHE_POLICY_INT 		2 ///< @brief number of integer bits of DISP_DE_CACHE_POLICY
#define DISP_DE_CACHE_POLICY_SIGNED 		0 ///< @brief is DISP_DE_CACHE_POLICY signed or unsigned

#define REG_CACHE_POLICY_FRAC 		0 ///< @brief number of fractional bits of REG_CACHE_POLICY
#define REG_CACHE_POLICY_INT 		2 ///< @brief number of integer bits of REG_CACHE_POLICY
#define REG_CACHE_POLICY_SIGNED 		0 ///< @brief is REG_CACHE_POLICY signed or unsigned

#define LENS_CACHE_POLICY_FRAC 		0 ///< @brief number of fractional bits of LENS_CACHE_POLICY
#define LENS_CACHE_POLICY_INT 		2 ///< @brief number of integer bits of LENS_CACHE_POLICY
#define LENS_CACHE_POLICY_SIGNED 		0 ///< @brief is LENS_CACHE_POLICY signed or unsigned

#define MEM_DUMP_CACHE_POLICY_FRAC 		0 ///< @brief number of fractional bits of MEM_DUMP_CACHE_POLICY
#define MEM_DUMP_CACHE_POLICY_INT 		2 ///< @brief number of integer bits of MEM_DUMP_CACHE_POLICY
#define MEM_DUMP_CACHE_POLICY_SIGNED 		0 ///< @brief is MEM_DUMP_CACHE_POLICY signed or unsigned

#define ENS_CACHE_POLICY_FRAC 		0 ///< @brief number of fractional bits of ENS_CACHE_POLICY
#define ENS_CACHE_POLICY_INT 		2 ///< @brief number of integer bits of ENS_CACHE_POLICY
#define ENS_CACHE_POLICY_SIGNED 		0 ///< @brief is ENS_CACHE_POLICY signed or unsigned

#define RAW_2D_CACHE_POLICY_FRAC 		0 ///< @brief number of fractional bits of RAW_2D_CACHE_POLICY
#define RAW_2D_CACHE_POLICY_INT 		2 ///< @brief number of integer bits of RAW_2D_CACHE_POLICY
#define RAW_2D_CACHE_POLICY_SIGNED 		0 ///< @brief is RAW_2D_CACHE_POLICY signed or unsigned

#define HDF_RD_CACHE_POLICY_FRAC 		0 ///< @brief number of fractional bits of HDF_RD_CACHE_POLICY
#define HDF_RD_CACHE_POLICY_INT 		2 ///< @brief number of integer bits of HDF_RD_CACHE_POLICY
#define HDF_RD_CACHE_POLICY_SIGNED 		0 ///< @brief is HDF_RD_CACHE_POLICY signed or unsigned

#define DPF_READ_CACHE_POLICY_FRAC 		0 ///< @brief number of fractional bits of DPF_READ_CACHE_POLICY
#define DPF_READ_CACHE_POLICY_INT 		2 ///< @brief number of integer bits of DPF_READ_CACHE_POLICY
#define DPF_READ_CACHE_POLICY_SIGNED 		0 ///< @brief is DPF_READ_CACHE_POLICY signed or unsigned

#define DPF_WRITE_CACHE_POLICY_FRAC 		0 ///< @brief number of fractional bits of DPF_WRITE_CACHE_POLICY
#define DPF_WRITE_CACHE_POLICY_INT 		2 ///< @brief number of integer bits of DPF_WRITE_CACHE_POLICY
#define DPF_WRITE_CACHE_POLICY_SIGNED 		0 ///< @brief is DPF_WRITE_CACHE_POLICY signed or unsigned

#define HDF_WR_CACHE_POLICY_FRAC 		0 ///< @brief number of fractional bits of HDF_WR_CACHE_POLICY
#define HDF_WR_CACHE_POLICY_INT 		2 ///< @brief number of integer bits of HDF_WR_CACHE_POLICY
#define HDF_WR_CACHE_POLICY_SIGNED 		0 ///< @brief is HDF_WR_CACHE_POLICY signed or unsigned


// MEMORY_CONTROL2
#define ENC_FORCE_FENCE_FRAC 		0 ///< @brief number of fractional bits of ENC_FORCE_FENCE
#define ENC_FORCE_FENCE_INT 		1 ///< @brief number of integer bits of ENC_FORCE_FENCE
#define ENC_FORCE_FENCE_SIGNED 		0 ///< @brief is ENC_FORCE_FENCE signed or unsigned

#define DISP_DE_FORCE_FENCE_FRAC 		0 ///< @brief number of fractional bits of DISP_DE_FORCE_FENCE
#define DISP_DE_FORCE_FENCE_INT 		1 ///< @brief number of integer bits of DISP_DE_FORCE_FENCE
#define DISP_DE_FORCE_FENCE_SIGNED 		0 ///< @brief is DISP_DE_FORCE_FENCE signed or unsigned

#define HDF_WR_FORCE_FENCE_FRAC 		0 ///< @brief number of fractional bits of HDF_WR_FORCE_FENCE
#define HDF_WR_FORCE_FENCE_INT 		1 ///< @brief number of integer bits of HDF_WR_FORCE_FENCE
#define HDF_WR_FORCE_FENCE_SIGNED 		0 ///< @brief is HDF_WR_FORCE_FENCE signed or unsigned

#define RAW_2D_FORCE_FENCE_FRAC 		0 ///< @brief number of fractional bits of RAW_2D_FORCE_FENCE
#define RAW_2D_FORCE_FENCE_INT 		1 ///< @brief number of integer bits of RAW_2D_FORCE_FENCE
#define RAW_2D_FORCE_FENCE_SIGNED 		0 ///< @brief is RAW_2D_FORCE_FENCE signed or unsigned

#define MEM_DUMP_REMOVE_FENCE_FRAC 		0 ///< @brief number of fractional bits of MEM_DUMP_REMOVE_FENCE
#define MEM_DUMP_REMOVE_FENCE_INT 		1 ///< @brief number of integer bits of MEM_DUMP_REMOVE_FENCE
#define MEM_DUMP_REMOVE_FENCE_SIGNED 		0 ///< @brief is MEM_DUMP_REMOVE_FENCE signed or unsigned

#define ENC_L_REMOVE_FENCE_FRAC 		0 ///< @brief number of fractional bits of ENC_L_REMOVE_FENCE
#define ENC_L_REMOVE_FENCE_INT 		1 ///< @brief number of integer bits of ENC_L_REMOVE_FENCE
#define ENC_L_REMOVE_FENCE_SIGNED 		0 ///< @brief is ENC_L_REMOVE_FENCE signed or unsigned

#define DISP_DE_REMOVE_FENCE_FRAC 		0 ///< @brief number of fractional bits of DISP_DE_REMOVE_FENCE
#define DISP_DE_REMOVE_FENCE_INT 		1 ///< @brief number of integer bits of DISP_DE_REMOVE_FENCE
#define DISP_DE_REMOVE_FENCE_SIGNED 		0 ///< @brief is DISP_DE_REMOVE_FENCE signed or unsigned

#define OTHERS_REMOVE_FENCE_FRAC 		0 ///< @brief number of fractional bits of OTHERS_REMOVE_FENCE
#define OTHERS_REMOVE_FENCE_INT 		1 ///< @brief number of integer bits of OTHERS_REMOVE_FENCE
#define OTHERS_REMOVE_FENCE_SIGNED 		0 ///< @brief is OTHERS_REMOVE_FENCE signed or unsigned

#define HDF_WR_REMOVE_FENCE_FRAC 		0 ///< @brief number of fractional bits of HDF_WR_REMOVE_FENCE
#define HDF_WR_REMOVE_FENCE_INT 		1 ///< @brief number of integer bits of HDF_WR_REMOVE_FENCE
#define HDF_WR_REMOVE_FENCE_SIGNED 		0 ///< @brief is HDF_WR_REMOVE_FENCE signed or unsigned

#define RAW_2D_REMOVE_FENCE_FRAC 		0 ///< @brief number of fractional bits of RAW_2D_REMOVE_FENCE
#define RAW_2D_REMOVE_FENCE_INT 		1 ///< @brief number of integer bits of RAW_2D_REMOVE_FENCE
#define RAW_2D_REMOVE_FENCE_SIGNED 		0 ///< @brief is RAW_2D_REMOVE_FENCE signed or unsigned


// LS_BUFFER_ALLOCATION
#define LS_CONTEXT_OFFSET_FRAC 		0 ///< @brief number of fractional bits of LS_CONTEXT_OFFSET
#define LS_CONTEXT_OFFSET_INT 		14 ///< @brief number of integer bits of LS_CONTEXT_OFFSET
#define LS_CONTEXT_OFFSET_SIGNED 		0 ///< @brief is LS_CONTEXT_OFFSET signed or unsigned


// CONTEXT_LINK_EMPTYNESS
#define CONTEXT_LINK_EMPTYNESS_FRAC 		0 ///< @brief number of fractional bits of CONTEXT_LINK_EMPTYNESS
#define CONTEXT_LINK_EMPTYNESS_INT 		5 ///< @brief number of integer bits of CONTEXT_LINK_EMPTYNESS
#define CONTEXT_LINK_EMPTYNESS_SIGNED 		0 ///< @brief is CONTEXT_LINK_EMPTYNESS signed or unsigned


// CONTEXT_LINK_ADDR
#define CONTEXT_LINK_ADDR_FRAC 		0 ///< @brief number of fractional bits of CONTEXT_LINK_ADDR
#define CONTEXT_LINK_ADDR_INT 		26 ///< @brief number of integer bits of CONTEXT_LINK_ADDR
#define CONTEXT_LINK_ADDR_SIGNED 		0 ///< @brief is CONTEXT_LINK_ADDR signed or unsigned


// PADDING_SPACE_0
#define PADDING_SPACE_0_FRAC 		0 ///< @brief number of fractional bits of PADDING_SPACE_0
#define PADDING_SPACE_0_INT 		31 ///< @brief number of integer bits of PADDING_SPACE_0
#define PADDING_SPACE_0_SIGNED 		0 ///< @brief is PADDING_SPACE_0 signed or unsigned


// PADDING_SPACE_1
#define PADDING_SPACE_1_FRAC 		0 ///< @brief number of fractional bits of PADDING_SPACE_1
#define PADDING_SPACE_1_INT 		31 ///< @brief number of integer bits of PADDING_SPACE_1
#define PADDING_SPACE_1_SIGNED 		0 ///< @brief is PADDING_SPACE_1 signed or unsigned


// CONTEXT_TAG
#define CONTEXT_TAG_FRAC 		0 ///< @brief number of fractional bits of CONTEXT_TAG
#define CONTEXT_TAG_INT 		8 ///< @brief number of integer bits of CONTEXT_TAG
#define CONTEXT_TAG_SIGNED 		0 ///< @brief is CONTEXT_TAG signed or unsigned


// MEM_ENC_Y_ADDR
#define MEM_ENC_Y_ADDR_FRAC 		0 ///< @brief number of fractional bits of MEM_ENC_Y_ADDR
#define MEM_ENC_Y_ADDR_INT 		26 ///< @brief number of integer bits of MEM_ENC_Y_ADDR
#define MEM_ENC_Y_ADDR_SIGNED 		0 ///< @brief is MEM_ENC_Y_ADDR signed or unsigned


// MEM_ENC_Y_STR
#define MEM_ENC_Y_STR_FRAC 		0 ///< @brief number of fractional bits of MEM_ENC_Y_STR
#define MEM_ENC_Y_STR_INT 		26 ///< @brief number of integer bits of MEM_ENC_Y_STR
#define MEM_ENC_Y_STR_SIGNED 		0 ///< @brief is MEM_ENC_Y_STR signed or unsigned


// MEM_ENC_Y_LAST_PAGE
#define MEM_ENC_Y_LAST_PAGE_FRAC 		0 ///< @brief number of fractional bits of MEM_ENC_Y_LAST_PAGE
#define MEM_ENC_Y_LAST_PAGE_INT 		20 ///< @brief number of integer bits of MEM_ENC_Y_LAST_PAGE
#define MEM_ENC_Y_LAST_PAGE_SIGNED 		0 ///< @brief is MEM_ENC_Y_LAST_PAGE signed or unsigned


// MEM_ENC_C_ADDR
#define MEM_ENC_C_ADDR_FRAC 		0 ///< @brief number of fractional bits of MEM_ENC_C_ADDR
#define MEM_ENC_C_ADDR_INT 		26 ///< @brief number of integer bits of MEM_ENC_C_ADDR
#define MEM_ENC_C_ADDR_SIGNED 		0 ///< @brief is MEM_ENC_C_ADDR signed or unsigned


// MEM_ENC_C_STR
#define MEM_ENC_C_STR_FRAC 		0 ///< @brief number of fractional bits of MEM_ENC_C_STR
#define MEM_ENC_C_STR_INT 		26 ///< @brief number of integer bits of MEM_ENC_C_STR
#define MEM_ENC_C_STR_SIGNED 		0 ///< @brief is MEM_ENC_C_STR signed or unsigned


// MEM_ENC_C_LAST_PAGE
#define MEM_ENC_C_LAST_PAGE_FRAC 		0 ///< @brief number of fractional bits of MEM_ENC_C_LAST_PAGE
#define MEM_ENC_C_LAST_PAGE_INT 		20 ///< @brief number of integer bits of MEM_ENC_C_LAST_PAGE
#define MEM_ENC_C_LAST_PAGE_SIGNED 		0 ///< @brief is MEM_ENC_C_LAST_PAGE signed or unsigned


// MEM_DISP_DE_ADDR
#define MEM_DISP_DE_ADDR_FRAC 		0 ///< @brief number of fractional bits of MEM_DISP_DE_ADDR
#define MEM_DISP_DE_ADDR_INT 		26 ///< @brief number of integer bits of MEM_DISP_DE_ADDR
#define MEM_DISP_DE_ADDR_SIGNED 		0 ///< @brief is MEM_DISP_DE_ADDR signed or unsigned


// MEM_DISP_DE_STR
#define MEM_DISP_DE_STR_FRAC 		0 ///< @brief number of fractional bits of MEM_DISP_DE_STR
#define MEM_DISP_DE_STR_INT 		26 ///< @brief number of integer bits of MEM_DISP_DE_STR
#define MEM_DISP_DE_STR_SIGNED 		0 ///< @brief is MEM_DISP_DE_STR signed or unsigned


// MEM_DISP_DE_LAST_PAGE
#define MEM_DISP_DE_LAST_PAGE_FRAC 		0 ///< @brief number of fractional bits of MEM_DISP_DE_LAST_PAGE
#define MEM_DISP_DE_LAST_PAGE_INT 		20 ///< @brief number of integer bits of MEM_DISP_DE_LAST_PAGE
#define MEM_DISP_DE_LAST_PAGE_SIGNED 		0 ///< @brief is MEM_DISP_DE_LAST_PAGE signed or unsigned


// DPF_WR_MAP_ADDR
#define DPF_WR_MAP_ADDR_FRAC 		0 ///< @brief number of fractional bits of DPF_WR_MAP_ADDR
#define DPF_WR_MAP_ADDR_INT 		26 ///< @brief number of integer bits of DPF_WR_MAP_ADDR
#define DPF_WR_MAP_ADDR_SIGNED 		0 ///< @brief is DPF_WR_MAP_ADDR signed or unsigned


// ENS_WR_ADDR
#define ENS_WR_ADDR_FRAC 		0 ///< @brief number of fractional bits of ENS_WR_ADDR
#define ENS_WR_ADDR_INT 		26 ///< @brief number of integer bits of ENS_WR_ADDR
#define ENS_WR_ADDR_SIGNED 		0 ///< @brief is ENS_WR_ADDR signed or unsigned


// MEM_RAW_2D_ADDR
#define MEM_RAW_2D_ADDR_FRAC 		0 ///< @brief number of fractional bits of MEM_RAW_2D_ADDR
#define MEM_RAW_2D_ADDR_INT 		32 ///< @brief number of integer bits of MEM_RAW_2D_ADDR
#define MEM_RAW_2D_ADDR_SIGNED 		0 ///< @brief is MEM_RAW_2D_ADDR signed or unsigned


// MEM_RAW_2D_STR
#define MEM_RAW_2D_STR_FRAC 		0 ///< @brief number of fractional bits of MEM_RAW_2D_STR
#define MEM_RAW_2D_STR_INT 		32 ///< @brief number of integer bits of MEM_RAW_2D_STR
#define MEM_RAW_2D_STR_SIGNED 		0 ///< @brief is MEM_RAW_2D_STR signed or unsigned


// MEM_RAW_2D_LAST_PAGE
#define MEM_RAW_2D_LAST_PAGE_FRAC 		0 ///< @brief number of fractional bits of MEM_RAW_2D_LAST_PAGE
#define MEM_RAW_2D_LAST_PAGE_INT 		20 ///< @brief number of integer bits of MEM_RAW_2D_LAST_PAGE
#define MEM_RAW_2D_LAST_PAGE_SIGNED 		0 ///< @brief is MEM_RAW_2D_LAST_PAGE signed or unsigned


// MEM_HDF_WR_ADDR
#define MEM_HDF_WR_ADDR_FRAC 		0 ///< @brief number of fractional bits of MEM_HDF_WR_ADDR
#define MEM_HDF_WR_ADDR_INT 		26 ///< @brief number of integer bits of MEM_HDF_WR_ADDR
#define MEM_HDF_WR_ADDR_SIGNED 		0 ///< @brief is MEM_HDF_WR_ADDR signed or unsigned


// MEM_HDF_WR_STR
#define MEM_HDF_WR_STR_FRAC 		0 ///< @brief number of fractional bits of MEM_HDF_WR_STR
#define MEM_HDF_WR_STR_INT 		26 ///< @brief number of integer bits of MEM_HDF_WR_STR
#define MEM_HDF_WR_STR_SIGNED 		0 ///< @brief is MEM_HDF_WR_STR signed or unsigned


// MEM_HDF_WR_LAST_PAGE
#define MEM_HDF_WR_LAST_PAGE_FRAC 		0 ///< @brief number of fractional bits of MEM_HDF_WR_LAST_PAGE
#define MEM_HDF_WR_LAST_PAGE_INT 		20 ///< @brief number of integer bits of MEM_HDF_WR_LAST_PAGE
#define MEM_HDF_WR_LAST_PAGE_SIGNED 		0 ///< @brief is MEM_HDF_WR_LAST_PAGE signed or unsigned


// MEM_HDF_RD_ADDR
#define MEM_HDF_RD_ADDR_FRAC 		0 ///< @brief number of fractional bits of MEM_HDF_RD_ADDR
#define MEM_HDF_RD_ADDR_INT 		26 ///< @brief number of integer bits of MEM_HDF_RD_ADDR
#define MEM_HDF_RD_ADDR_SIGNED 		0 ///< @brief is MEM_HDF_RD_ADDR signed or unsigned


// MEM_HDF_RD_STR
#define MEM_HDF_RD_STR_FRAC 		0 ///< @brief number of fractional bits of MEM_HDF_RD_STR
#define MEM_HDF_RD_STR_INT 		26 ///< @brief number of integer bits of MEM_HDF_RD_STR
#define MEM_HDF_RD_STR_SIGNED 		0 ///< @brief is MEM_HDF_RD_STR signed or unsigned


// TILING_CONTROL
#define DISP_DE_TILE_EN_FRAC 		0 ///< @brief number of fractional bits of DISP_DE_TILE_EN
#define DISP_DE_TILE_EN_INT 		1 ///< @brief number of integer bits of DISP_DE_TILE_EN
#define DISP_DE_TILE_EN_SIGNED 		0 ///< @brief is DISP_DE_TILE_EN signed or unsigned

#define HDF_WR_TILE_EN_FRAC 		0 ///< @brief number of fractional bits of HDF_WR_TILE_EN
#define HDF_WR_TILE_EN_INT 		1 ///< @brief number of integer bits of HDF_WR_TILE_EN
#define HDF_WR_TILE_EN_SIGNED 		0 ///< @brief is HDF_WR_TILE_EN signed or unsigned

#define ENC_L_TILE_EN_FRAC 		0 ///< @brief number of fractional bits of ENC_L_TILE_EN
#define ENC_L_TILE_EN_INT 		1 ///< @brief number of integer bits of ENC_L_TILE_EN
#define ENC_L_TILE_EN_SIGNED 		0 ///< @brief is ENC_L_TILE_EN signed or unsigned

#define ENC_C_TILE_EN_FRAC 		0 ///< @brief number of fractional bits of ENC_C_TILE_EN
#define ENC_C_TILE_EN_INT 		1 ///< @brief number of integer bits of ENC_C_TILE_EN
#define ENC_C_TILE_EN_SIGNED 		0 ///< @brief is ENC_C_TILE_EN signed or unsigned


// LOAD_CONFIG_ADDR
#define LOAD_CONFIG_ADDR_FRAC 		0 ///< @brief number of fractional bits of LOAD_CONFIG_ADDR
#define LOAD_CONFIG_ADDR_INT 		26 ///< @brief number of integer bits of LOAD_CONFIG_ADDR
#define LOAD_CONFIG_ADDR_SIGNED 		0 ///< @brief is LOAD_CONFIG_ADDR signed or unsigned


// LOAD_CONFIG_FLAGS
#define CONFIG_TAG_FRAC 		0 ///< @brief number of fractional bits of CONFIG_TAG
#define CONFIG_TAG_INT 		8 ///< @brief number of integer bits of CONFIG_TAG
#define CONFIG_TAG_SIGNED 		0 ///< @brief is CONFIG_TAG signed or unsigned

#define LOAD_CONFIG_GRP_6_FRAC 		0 ///< @brief number of fractional bits of LOAD_CONFIG_GRP_6
#define LOAD_CONFIG_GRP_6_INT 		1 ///< @brief number of integer bits of LOAD_CONFIG_GRP_6
#define LOAD_CONFIG_GRP_6_SIGNED 		0 ///< @brief is LOAD_CONFIG_GRP_6 signed or unsigned

#define LOAD_CONFIG_GRP_5_FRAC 		0 ///< @brief number of fractional bits of LOAD_CONFIG_GRP_5
#define LOAD_CONFIG_GRP_5_INT 		1 ///< @brief number of integer bits of LOAD_CONFIG_GRP_5
#define LOAD_CONFIG_GRP_5_SIGNED 		0 ///< @brief is LOAD_CONFIG_GRP_5 signed or unsigned

#define LOAD_CONFIG_GRP_4_FRAC 		0 ///< @brief number of fractional bits of LOAD_CONFIG_GRP_4
#define LOAD_CONFIG_GRP_4_INT 		1 ///< @brief number of integer bits of LOAD_CONFIG_GRP_4
#define LOAD_CONFIG_GRP_4_SIGNED 		0 ///< @brief is LOAD_CONFIG_GRP_4 signed or unsigned

#define LOAD_CONFIG_GRP_3_FRAC 		0 ///< @brief number of fractional bits of LOAD_CONFIG_GRP_3
#define LOAD_CONFIG_GRP_3_INT 		1 ///< @brief number of integer bits of LOAD_CONFIG_GRP_3
#define LOAD_CONFIG_GRP_3_SIGNED 		0 ///< @brief is LOAD_CONFIG_GRP_3 signed or unsigned

#define LOAD_CONFIG_GRP_2_FRAC 		0 ///< @brief number of fractional bits of LOAD_CONFIG_GRP_2
#define LOAD_CONFIG_GRP_2_INT 		1 ///< @brief number of integer bits of LOAD_CONFIG_GRP_2
#define LOAD_CONFIG_GRP_2_SIGNED 		0 ///< @brief is LOAD_CONFIG_GRP_2 signed or unsigned

#define LOAD_CONFIG_GRP_1_FRAC 		0 ///< @brief number of fractional bits of LOAD_CONFIG_GRP_1
#define LOAD_CONFIG_GRP_1_INT 		1 ///< @brief number of integer bits of LOAD_CONFIG_GRP_1
#define LOAD_CONFIG_GRP_1_SIGNED 		0 ///< @brief is LOAD_CONFIG_GRP_1 signed or unsigned

#define LOAD_CONFIG_GRP_0_FRAC 		0 ///< @brief number of fractional bits of LOAD_CONFIG_GRP_0
#define LOAD_CONFIG_GRP_0_INT 		1 ///< @brief number of integer bits of LOAD_CONFIG_GRP_0
#define LOAD_CONFIG_GRP_0_SIGNED 		0 ///< @brief is LOAD_CONFIG_GRP_0 signed or unsigned


// SAVE_RESULTS_ADDR
#define SAVE_RESULTS_ADDR_FRAC 		0 ///< @brief number of fractional bits of SAVE_RESULTS_ADDR
#define SAVE_RESULTS_ADDR_INT 		26 ///< @brief number of integer bits of SAVE_RESULTS_ADDR
#define SAVE_RESULTS_ADDR_SIGNED 		0 ///< @brief is SAVE_RESULTS_ADDR signed or unsigned


// SAVE_CONFIG_FLAGS
#define AWS_ENABLE_FRAC 		0 ///< @brief number of fractional bits of AWS_ENABLE
#define AWS_ENABLE_INT 		1 ///< @brief number of integer bits of AWS_ENABLE
#define AWS_ENABLE_SIGNED 		0 ///< @brief is AWS_ENABLE signed or unsigned

#define HDF_WR_FORMAT_FRAC 		0 ///< @brief number of fractional bits of HDF_WR_FORMAT
#define HDF_WR_FORMAT_INT 		2 ///< @brief number of integer bits of HDF_WR_FORMAT
#define HDF_WR_FORMAT_SIGNED 		0 ///< @brief is HDF_WR_FORMAT signed or unsigned

#define HDF_WR_ENABLE_FRAC 		0 ///< @brief number of fractional bits of HDF_WR_ENABLE
#define HDF_WR_ENABLE_INT 		1 ///< @brief number of integer bits of HDF_WR_ENABLE
#define HDF_WR_ENABLE_SIGNED 		0 ///< @brief is HDF_WR_ENABLE signed or unsigned

#define HDF_RD_FORMAT_FRAC 		0 ///< @brief number of fractional bits of HDF_RD_FORMAT
#define HDF_RD_FORMAT_INT 		2 ///< @brief number of integer bits of HDF_RD_FORMAT
#define HDF_RD_FORMAT_SIGNED 		0 ///< @brief is HDF_RD_FORMAT signed or unsigned

#define HDF_RD_ENABLE_FRAC 		0 ///< @brief number of fractional bits of HDF_RD_ENABLE
#define HDF_RD_ENABLE_INT 		1 ///< @brief number of integer bits of HDF_RD_ENABLE
#define HDF_RD_ENABLE_SIGNED 		0 ///< @brief is HDF_RD_ENABLE signed or unsigned

#define RAW_2D_FORMAT_FRAC 		0 ///< @brief number of fractional bits of RAW_2D_FORMAT
#define RAW_2D_FORMAT_INT 		2 ///< @brief number of integer bits of RAW_2D_FORMAT
#define RAW_2D_FORMAT_SIGNED 		0 ///< @brief is RAW_2D_FORMAT signed or unsigned

#define RAW_2D_ENABLE_FRAC 		0 ///< @brief number of fractional bits of RAW_2D_ENABLE
#define RAW_2D_ENABLE_INT 		1 ///< @brief number of integer bits of RAW_2D_ENABLE
#define RAW_2D_ENABLE_SIGNED 		0 ///< @brief is RAW_2D_ENABLE signed or unsigned

#define DPF_WRITE_MAP_ENABLE_FRAC 		0 ///< @brief number of fractional bits of DPF_WRITE_MAP_ENABLE
#define DPF_WRITE_MAP_ENABLE_INT 		1 ///< @brief number of integer bits of DPF_WRITE_MAP_ENABLE
#define DPF_WRITE_MAP_ENABLE_SIGNED 		0 ///< @brief is DPF_WRITE_MAP_ENABLE signed or unsigned

#define DPF_READ_MAP_ENABLE_FRAC 		0 ///< @brief number of fractional bits of DPF_READ_MAP_ENABLE
#define DPF_READ_MAP_ENABLE_INT 		1 ///< @brief number of integer bits of DPF_READ_MAP_ENABLE
#define DPF_READ_MAP_ENABLE_SIGNED 		0 ///< @brief is DPF_READ_MAP_ENABLE signed or unsigned

#define ENS_ENABLE_FRAC 		0 ///< @brief number of fractional bits of ENS_ENABLE
#define ENS_ENABLE_INT 		1 ///< @brief number of integer bits of ENS_ENABLE
#define ENS_ENABLE_SIGNED 		0 ///< @brief is ENS_ENABLE signed or unsigned

#define TIME_STAMP_ENABLE_FRAC 		0 ///< @brief number of fractional bits of TIME_STAMP_ENABLE
#define TIME_STAMP_ENABLE_INT 		1 ///< @brief number of integer bits of TIME_STAMP_ENABLE
#define TIME_STAMP_ENABLE_SIGNED 		0 ///< @brief is TIME_STAMP_ENABLE signed or unsigned

#define CRC_ENABLE_FRAC 		0 ///< @brief number of fractional bits of CRC_ENABLE
#define CRC_ENABLE_INT 		1 ///< @brief number of integer bits of CRC_ENABLE
#define CRC_ENABLE_SIGNED 		0 ///< @brief is CRC_ENABLE signed or unsigned

#define FLD_ENABLE_FRAC 		0 ///< @brief number of fractional bits of FLD_ENABLE
#define FLD_ENABLE_INT 		1 ///< @brief number of integer bits of FLD_ENABLE
#define FLD_ENABLE_SIGNED 		0 ///< @brief is FLD_ENABLE signed or unsigned

#define HIS_REGION_ENABLE_FRAC 		0 ///< @brief number of fractional bits of HIS_REGION_ENABLE
#define HIS_REGION_ENABLE_INT 		1 ///< @brief number of integer bits of HIS_REGION_ENABLE
#define HIS_REGION_ENABLE_SIGNED 		0 ///< @brief is HIS_REGION_ENABLE signed or unsigned

#define HIS_GLOB_ENABLE_FRAC 		0 ///< @brief number of fractional bits of HIS_GLOB_ENABLE
#define HIS_GLOB_ENABLE_INT 		1 ///< @brief number of integer bits of HIS_GLOB_ENABLE
#define HIS_GLOB_ENABLE_SIGNED 		0 ///< @brief is HIS_GLOB_ENABLE signed or unsigned

#define WBS_ENABLE_FRAC 		0 ///< @brief number of fractional bits of WBS_ENABLE
#define WBS_ENABLE_INT 		1 ///< @brief number of integer bits of WBS_ENABLE
#define WBS_ENABLE_SIGNED 		0 ///< @brief is WBS_ENABLE signed or unsigned

#define FOS_ROI_ENABLE_FRAC 		0 ///< @brief number of fractional bits of FOS_ROI_ENABLE
#define FOS_ROI_ENABLE_INT 		1 ///< @brief number of integer bits of FOS_ROI_ENABLE
#define FOS_ROI_ENABLE_SIGNED 		0 ///< @brief is FOS_ROI_ENABLE signed or unsigned

#define FOS_GRID_ENABLE_FRAC 		0 ///< @brief number of fractional bits of FOS_GRID_ENABLE
#define FOS_GRID_ENABLE_INT 		1 ///< @brief number of integer bits of FOS_GRID_ENABLE
#define FOS_GRID_ENABLE_SIGNED 		0 ///< @brief is FOS_GRID_ENABLE signed or unsigned

#define EXS_GLOB_ENABLE_FRAC 		0 ///< @brief number of fractional bits of EXS_GLOB_ENABLE
#define EXS_GLOB_ENABLE_INT 		1 ///< @brief number of integer bits of EXS_GLOB_ENABLE
#define EXS_GLOB_ENABLE_SIGNED 		0 ///< @brief is EXS_GLOB_ENABLE signed or unsigned

#define EXS_REGION_ENABLE_FRAC 		0 ///< @brief number of fractional bits of EXS_REGION_ENABLE
#define EXS_REGION_ENABLE_INT 		1 ///< @brief number of integer bits of EXS_REGION_ENABLE
#define EXS_REGION_ENABLE_SIGNED 		0 ///< @brief is EXS_REGION_ENABLE signed or unsigned

#define DE_NO_DISP_FRAC 		0 ///< @brief number of fractional bits of DE_NO_DISP
#define DE_NO_DISP_INT 		1 ///< @brief number of integer bits of DE_NO_DISP
#define DE_NO_DISP_SIGNED 		0 ///< @brief is DE_NO_DISP signed or unsigned

#define DISP_DE_FORMAT_FRAC 		0 ///< @brief number of fractional bits of DISP_DE_FORMAT
#define DISP_DE_FORMAT_INT 		2 ///< @brief number of integer bits of DISP_DE_FORMAT
#define DISP_DE_FORMAT_SIGNED 		0 ///< @brief is DISP_DE_FORMAT signed or unsigned

#define DISP_DE_ENABLE_FRAC 		0 ///< @brief number of fractional bits of DISP_DE_ENABLE
#define DISP_DE_ENABLE_INT 		1 ///< @brief number of integer bits of DISP_DE_ENABLE
#define DISP_DE_ENABLE_SIGNED 		0 ///< @brief is DISP_DE_ENABLE signed or unsigned

#define ENC_FORMAT_FRAC 		0 ///< @brief number of fractional bits of ENC_FORMAT
#define ENC_FORMAT_INT 		2 ///< @brief number of integer bits of ENC_FORMAT
#define ENC_FORMAT_SIGNED 		0 ///< @brief is ENC_FORMAT signed or unsigned

#define ENC_ENABLE_FRAC 		0 ///< @brief number of fractional bits of ENC_ENABLE
#define ENC_ENABLE_INT 		1 ///< @brief number of integer bits of ENC_ENABLE
#define ENC_ENABLE_SIGNED 		0 ///< @brief is ENC_ENABLE signed or unsigned


// PADDING_SPACE_2
#define PADDING_SPACE_2_FRAC 		0 ///< @brief number of fractional bits of PADDING_SPACE_2
#define PADDING_SPACE_2_INT 		31 ///< @brief number of integer bits of PADDING_SPACE_2
#define PADDING_SPACE_2_SIGNED 		0 ///< @brief is PADDING_SPACE_2 signed or unsigned


// PADDING_SPACE_3
#define PADDING_SPACE_3_FRAC 		0 ///< @brief number of fractional bits of PADDING_SPACE_3
#define PADDING_SPACE_3_INT 		31 ///< @brief number of integer bits of PADDING_SPACE_3
#define PADDING_SPACE_3_SIGNED 		0 ///< @brief is PADDING_SPACE_3 signed or unsigned


// SYS_BLACK_LEVEL
#define SYS_BLACK_LEVEL_FRAC 		2 ///< @brief number of fractional bits of SYS_BLACK_LEVEL
#define SYS_BLACK_LEVEL_INT 		8 ///< @brief number of integer bits of SYS_BLACK_LEVEL
#define SYS_BLACK_LEVEL_SIGNED 		0 ///< @brief is SYS_BLACK_LEVEL signed or unsigned


// IMGR_CTRL
#define IMGR_SCALER_BOTTOM_BORDER_FRAC 		0 ///< @brief number of fractional bits of IMGR_SCALER_BOTTOM_BORDER
#define IMGR_SCALER_BOTTOM_BORDER_INT 		5 ///< @brief number of integer bits of IMGR_SCALER_BOTTOM_BORDER
#define IMGR_SCALER_BOTTOM_BORDER_SIGNED 		0 ///< @brief is IMGR_SCALER_BOTTOM_BORDER signed or unsigned

#define IMGR_INPUT_SEL_FRAC 		0 ///< @brief number of fractional bits of IMGR_INPUT_SEL
#define IMGR_INPUT_SEL_INT 		4 ///< @brief number of integer bits of IMGR_INPUT_SEL
#define IMGR_INPUT_SEL_SIGNED 		0 ///< @brief is IMGR_INPUT_SEL signed or unsigned

#define IMGR_BAYER_FORMAT_FRAC 		0 ///< @brief number of fractional bits of IMGR_BAYER_FORMAT
#define IMGR_BAYER_FORMAT_INT 		2 ///< @brief number of integer bits of IMGR_BAYER_FORMAT
#define IMGR_BAYER_FORMAT_SIGNED 		0 ///< @brief is IMGR_BAYER_FORMAT signed or unsigned

#define IMGR_BUF_THRESH_FRAC 		0 ///< @brief number of fractional bits of IMGR_BUF_THRESH
#define IMGR_BUF_THRESH_INT 		15 ///< @brief number of integer bits of IMGR_BUF_THRESH
#define IMGR_BUF_THRESH_SIGNED 		0 ///< @brief is IMGR_BUF_THRESH signed or unsigned


// IMGR_CAP_OFFSET
#define IMGR_CAP_OFFSET_Y_FRAC 		0 ///< @brief number of fractional bits of IMGR_CAP_OFFSET_Y
#define IMGR_CAP_OFFSET_Y_INT 		15 ///< @brief number of integer bits of IMGR_CAP_OFFSET_Y
#define IMGR_CAP_OFFSET_Y_SIGNED 		0 ///< @brief is IMGR_CAP_OFFSET_Y signed or unsigned

#define IMGR_CAP_OFFSET_X_FRAC 		0 ///< @brief number of fractional bits of IMGR_CAP_OFFSET_X
#define IMGR_CAP_OFFSET_X_INT 		15 ///< @brief number of integer bits of IMGR_CAP_OFFSET_X
#define IMGR_CAP_OFFSET_X_SIGNED 		0 ///< @brief is IMGR_CAP_OFFSET_X signed or unsigned


// IMGR_OUT_SIZE
#define IMGR_OUT_ROWS_CFA_FRAC 		0 ///< @brief number of fractional bits of IMGR_OUT_ROWS_CFA
#define IMGR_OUT_ROWS_CFA_INT 		14 ///< @brief number of integer bits of IMGR_OUT_ROWS_CFA
#define IMGR_OUT_ROWS_CFA_SIGNED 		0 ///< @brief is IMGR_OUT_ROWS_CFA signed or unsigned

#define IMGR_OUT_COLS_CFA_FRAC 		0 ///< @brief number of fractional bits of IMGR_OUT_COLS_CFA
#define IMGR_OUT_COLS_CFA_INT 		14 ///< @brief number of integer bits of IMGR_OUT_COLS_CFA
#define IMGR_OUT_COLS_CFA_SIGNED 		0 ///< @brief is IMGR_OUT_COLS_CFA signed or unsigned


// IMGR_IMAGE_DECIMATION
#define BLACK_BORDER_OFFSET_FRAC 		0 ///< @brief number of fractional bits of BLACK_BORDER_OFFSET
#define BLACK_BORDER_OFFSET_INT 		6 ///< @brief number of integer bits of BLACK_BORDER_OFFSET
#define BLACK_BORDER_OFFSET_SIGNED 		0 ///< @brief is BLACK_BORDER_OFFSET signed or unsigned

#define IMGR_COMP_SKIP_Y_FRAC 		0 ///< @brief number of fractional bits of IMGR_COMP_SKIP_Y
#define IMGR_COMP_SKIP_Y_INT 		4 ///< @brief number of integer bits of IMGR_COMP_SKIP_Y
#define IMGR_COMP_SKIP_Y_SIGNED 		0 ///< @brief is IMGR_COMP_SKIP_Y signed or unsigned

#define IMGR_COMP_SKIP_X_FRAC 		0 ///< @brief number of fractional bits of IMGR_COMP_SKIP_X
#define IMGR_COMP_SKIP_X_INT 		4 ///< @brief number of integer bits of IMGR_COMP_SKIP_X
#define IMGR_COMP_SKIP_X_SIGNED 		0 ///< @brief is IMGR_COMP_SKIP_X signed or unsigned


// IMGR_BUFFER_STATUS
#define IMGR_BUFFER_FULLNESS_FRAC 		0 ///< @brief number of fractional bits of IMGR_BUFFER_FULLNESS
#define IMGR_BUFFER_FULLNESS_INT 		15 ///< @brief number of integer bits of IMGR_BUFFER_FULLNESS
#define IMGR_BUFFER_FULLNESS_SIGNED 		0 ///< @brief is IMGR_BUFFER_FULLNESS signed or unsigned


// AWS_CURVE_COEFF
#define AWS_CURVE_X_COEFF_FRAC 		10 ///< @brief number of fractional bits of AWS_CURVE_X_COEFF
#define AWS_CURVE_X_COEFF_INT 		5 ///< @brief number of integer bits of AWS_CURVE_X_COEFF
#define AWS_CURVE_X_COEFF_SIGNED 		1 ///< @brief is AWS_CURVE_X_COEFF signed or unsigned

#define AWS_CURVE_Y_COEFF_FRAC 		10 ///< @brief number of fractional bits of AWS_CURVE_Y_COEFF
#define AWS_CURVE_Y_COEFF_INT 		5 ///< @brief number of integer bits of AWS_CURVE_Y_COEFF
#define AWS_CURVE_Y_COEFF_SIGNED 		1 ///< @brief is AWS_CURVE_Y_COEFF signed or unsigned


// AWS_CURVE_OFFSET
#define AWS_CURVE_OFFSET_FRAC 		10 ///< @brief number of fractional bits of AWS_CURVE_OFFSET
#define AWS_CURVE_OFFSET_INT 		5 ///< @brief number of integer bits of AWS_CURVE_OFFSET
#define AWS_CURVE_OFFSET_SIGNED 		1 ///< @brief is AWS_CURVE_OFFSET signed or unsigned


// AWS_CURVE_BOUNDARY
#define AWS_CURVE_BOUNDARY_FRAC 		10 ///< @brief number of fractional bits of AWS_CURVE_BOUNDARY
#define AWS_CURVE_BOUNDARY_INT 		5 ///< @brief number of integer bits of AWS_CURVE_BOUNDARY
#define AWS_CURVE_BOUNDARY_SIGNED 		1 ///< @brief is AWS_CURVE_BOUNDARY signed or unsigned


// LSH_CTRL
#define LSH_ENABLE_FRAC 		0 ///< @brief number of fractional bits of LSH_ENABLE
#define LSH_ENABLE_INT 		1 ///< @brief number of integer bits of LSH_ENABLE
#define LSH_ENABLE_SIGNED 		0 ///< @brief is LSH_ENABLE signed or unsigned

#define LSH_VERTEX_DIFF_BITS_FRAC 		0 ///< @brief number of fractional bits of LSH_VERTEX_DIFF_BITS
#define LSH_VERTEX_DIFF_BITS_INT 		4 ///< @brief number of integer bits of LSH_VERTEX_DIFF_BITS
#define LSH_VERTEX_DIFF_BITS_SIGNED 		0 ///< @brief is LSH_VERTEX_DIFF_BITS signed or unsigned


// LSH_COLOUR_ALIGNMENT_Y
#define LSH_COLOUR_SKIP_Y_FRAC 		0 ///< @brief number of fractional bits of LSH_COLOUR_SKIP_Y
#define LSH_COLOUR_SKIP_Y_INT 		4 ///< @brief number of integer bits of LSH_COLOUR_SKIP_Y
#define LSH_COLOUR_SKIP_Y_SIGNED 		0 ///< @brief is LSH_COLOUR_SKIP_Y signed or unsigned

#define LSH_COLOUR_OFFSET_Y_FRAC 		0 ///< @brief number of fractional bits of LSH_COLOUR_OFFSET_Y
#define LSH_COLOUR_OFFSET_Y_INT 		12 ///< @brief number of integer bits of LSH_COLOUR_OFFSET_Y
#define LSH_COLOUR_OFFSET_Y_SIGNED 		0 ///< @brief is LSH_COLOUR_OFFSET_Y signed or unsigned


// LSH_ALIGNMENT_Y
#define LSH_SKIP_Y_FRAC 		0 ///< @brief number of fractional bits of LSH_SKIP_Y
#define LSH_SKIP_Y_INT 		4 ///< @brief number of integer bits of LSH_SKIP_Y
#define LSH_SKIP_Y_SIGNED 		0 ///< @brief is LSH_SKIP_Y signed or unsigned

#define LSH_OFFSET_Y_FRAC 		0 ///< @brief number of fractional bits of LSH_OFFSET_Y
#define LSH_OFFSET_Y_INT 		13 ///< @brief number of integer bits of LSH_OFFSET_Y
#define LSH_OFFSET_Y_SIGNED 		0 ///< @brief is LSH_OFFSET_Y signed or unsigned


// LSH_GRID_TILE
#define TILE_SIZE_LOG2_FRAC 		0 ///< @brief number of fractional bits of TILE_SIZE_LOG2
#define TILE_SIZE_LOG2_INT 		3 ///< @brief number of integer bits of TILE_SIZE_LOG2
#define TILE_SIZE_LOG2_SIGNED 		0 ///< @brief is TILE_SIZE_LOG2 signed or unsigned


// LSH_GRID_BASE_ADDR
#define LSH_GRID_BASE_ADDR_FRAC 		0 ///< @brief number of fractional bits of LSH_GRID_BASE_ADDR
#define LSH_GRID_BASE_ADDR_INT 		26 ///< @brief number of integer bits of LSH_GRID_BASE_ADDR
#define LSH_GRID_BASE_ADDR_SIGNED 		0 ///< @brief is LSH_GRID_BASE_ADDR signed or unsigned


// LSH_GRID_STRIDE
#define LSH_GRID_STRIDE_FRAC 		0 ///< @brief number of fractional bits of LSH_GRID_STRIDE
#define LSH_GRID_STRIDE_INT 		26 ///< @brief number of integer bits of LSH_GRID_STRIDE
#define LSH_GRID_STRIDE_SIGNED 		0 ///< @brief is LSH_GRID_STRIDE signed or unsigned


// LSH_GRID_LINE_SIZE
#define LSH_GRID_LINE_SIZE_FRAC 		0 ///< @brief number of fractional bits of LSH_GRID_LINE_SIZE
#define LSH_GRID_LINE_SIZE_INT 		7 ///< @brief number of integer bits of LSH_GRID_LINE_SIZE
#define LSH_GRID_LINE_SIZE_SIGNED 		0 ///< @brief is LSH_GRID_LINE_SIZE signed or unsigned


// LSH_GRADIENTS_Y
#define SHADING_GRADIENT_Y_FRAC 		12 ///< @brief number of fractional bits of SHADING_GRADIENT_Y
#define SHADING_GRADIENT_Y_INT 		-1 ///< @brief number of integer bits of SHADING_GRADIENT_Y
#define SHADING_GRADIENT_Y_SIGNED 		1 ///< @brief is SHADING_GRADIENT_Y signed or unsigned


// DPF_RD_MAP_ADDR
#define DPF_RD_MAP_ADDR_FRAC 		0 ///< @brief number of fractional bits of DPF_RD_MAP_ADDR
#define DPF_RD_MAP_ADDR_INT 		26 ///< @brief number of integer bits of DPF_RD_MAP_ADDR
#define DPF_RD_MAP_ADDR_SIGNED 		0 ///< @brief is DPF_RD_MAP_ADDR signed or unsigned


// DPF_READ_SIZE_CACHE
#define DPF_READ_MAP_SIZE_FRAC 		0 ///< @brief number of fractional bits of DPF_READ_MAP_SIZE
#define DPF_READ_MAP_SIZE_INT 		18 ///< @brief number of integer bits of DPF_READ_MAP_SIZE
#define DPF_READ_MAP_SIZE_SIGNED 		0 ///< @brief is DPF_READ_MAP_SIZE signed or unsigned


// DPF_READ_BUF_SIZE
#define DPF_READ_BUF_CONTEXT_SIZE_FRAC 		0 ///< @brief number of fractional bits of DPF_READ_BUF_CONTEXT_SIZE
#define DPF_READ_BUF_CONTEXT_SIZE_INT 		12 ///< @brief number of integer bits of DPF_READ_BUF_CONTEXT_SIZE
#define DPF_READ_BUF_CONTEXT_SIZE_SIGNED 		0 ///< @brief is DPF_READ_BUF_CONTEXT_SIZE signed or unsigned


// DPF_SKIP
#define DPF_MAP_SKIP_X_FRAC 		0 ///< @brief number of fractional bits of DPF_MAP_SKIP_X
#define DPF_MAP_SKIP_X_INT 		4 ///< @brief number of integer bits of DPF_MAP_SKIP_X
#define DPF_MAP_SKIP_X_SIGNED 		0 ///< @brief is DPF_MAP_SKIP_X signed or unsigned

#define DPF_MAP_SKIP_Y_FRAC 		0 ///< @brief number of fractional bits of DPF_MAP_SKIP_Y
#define DPF_MAP_SKIP_Y_INT 		4 ///< @brief number of integer bits of DPF_MAP_SKIP_Y
#define DPF_MAP_SKIP_Y_SIGNED 		0 ///< @brief is DPF_MAP_SKIP_Y signed or unsigned


// DPF_OFFSET
#define DPF_MAP_OFFSET_X_FRAC 		0 ///< @brief number of fractional bits of DPF_MAP_OFFSET_X
#define DPF_MAP_OFFSET_X_INT 		15 ///< @brief number of integer bits of DPF_MAP_OFFSET_X
#define DPF_MAP_OFFSET_X_SIGNED 		0 ///< @brief is DPF_MAP_OFFSET_X signed or unsigned

#define DPF_MAP_OFFSET_Y_FRAC 		0 ///< @brief number of fractional bits of DPF_MAP_OFFSET_Y
#define DPF_MAP_OFFSET_Y_INT 		15 ///< @brief number of integer bits of DPF_MAP_OFFSET_Y
#define DPF_MAP_OFFSET_Y_SIGNED 		0 ///< @brief is DPF_MAP_OFFSET_Y signed or unsigned


// EXS_GRID_START_COORDS
#define EXS_GRID_COL_START_FRAC 		0 ///< @brief number of fractional bits of EXS_GRID_COL_START
#define EXS_GRID_COL_START_INT 		16 ///< @brief number of integer bits of EXS_GRID_COL_START
#define EXS_GRID_COL_START_SIGNED 		0 ///< @brief is EXS_GRID_COL_START signed or unsigned

#define EXS_GRID_ROW_START_FRAC 		0 ///< @brief number of fractional bits of EXS_GRID_ROW_START
#define EXS_GRID_ROW_START_INT 		16 ///< @brief number of integer bits of EXS_GRID_ROW_START
#define EXS_GRID_ROW_START_SIGNED 		0 ///< @brief is EXS_GRID_ROW_START signed or unsigned


// EXS_GRID_TILE
#define EXS_GRID_TILE_WIDTH_FRAC 		0 ///< @brief number of fractional bits of EXS_GRID_TILE_WIDTH
#define EXS_GRID_TILE_WIDTH_INT 		14 ///< @brief number of integer bits of EXS_GRID_TILE_WIDTH
#define EXS_GRID_TILE_WIDTH_SIGNED 		0 ///< @brief is EXS_GRID_TILE_WIDTH signed or unsigned

#define EXS_GRID_TILE_HEIGHT_FRAC 		0 ///< @brief number of fractional bits of EXS_GRID_TILE_HEIGHT
#define EXS_GRID_TILE_HEIGHT_INT 		14 ///< @brief number of integer bits of EXS_GRID_TILE_HEIGHT
#define EXS_GRID_TILE_HEIGHT_SIGNED 		0 ///< @brief is EXS_GRID_TILE_HEIGHT signed or unsigned


// EXS_PIXEL_MAX
#define EXS_PIXEL_MAX_FRAC 		4 ///< @brief number of fractional bits of EXS_PIXEL_MAX
#define EXS_PIXEL_MAX_INT 		8 ///< @brief number of integer bits of EXS_PIXEL_MAX
#define EXS_PIXEL_MAX_SIGNED 		0 ///< @brief is EXS_PIXEL_MAX signed or unsigned


// BLACK_CONTROL
#define BLACK_FRAME_FRAC 		0 ///< @brief number of fractional bits of BLACK_FRAME
#define BLACK_FRAME_INT 		1 ///< @brief number of integer bits of BLACK_FRAME
#define BLACK_FRAME_SIGNED 		0 ///< @brief is BLACK_FRAME signed or unsigned

#define BLACK_MODE_FRAC 		0 ///< @brief number of fractional bits of BLACK_MODE
#define BLACK_MODE_INT 		1 ///< @brief number of integer bits of BLACK_MODE
#define BLACK_MODE_SIGNED 		0 ///< @brief is BLACK_MODE signed or unsigned


// BLACK_ANALYSIS
#define BLACK_ROW_RECIPROCAL_FRAC 		9 ///< @brief number of fractional bits of BLACK_ROW_RECIPROCAL
#define BLACK_ROW_RECIPROCAL_INT 		1 ///< @brief number of integer bits of BLACK_ROW_RECIPROCAL
#define BLACK_ROW_RECIPROCAL_SIGNED 		0 ///< @brief is BLACK_ROW_RECIPROCAL signed or unsigned

#define BLACK_PIXEL_MAX_FRAC 		2 ///< @brief number of fractional bits of BLACK_PIXEL_MAX
#define BLACK_PIXEL_MAX_INT 		6 ///< @brief number of integer bits of BLACK_PIXEL_MAX
#define BLACK_PIXEL_MAX_SIGNED 		0 ///< @brief is BLACK_PIXEL_MAX signed or unsigned


// BLACK_FIXED_OFFSET
#define BLACK_FIXED_FRAC 		1 ///< @brief number of fractional bits of BLACK_FIXED
#define BLACK_FIXED_INT 		7 ///< @brief number of integer bits of BLACK_FIXED
#define BLACK_FIXED_SIGNED 		1 ///< @brief is BLACK_FIXED signed or unsigned


// RLT_CONTROL
#define RLT_ENABLE_FRAC 		0 ///< @brief number of fractional bits of RLT_ENABLE
#define RLT_ENABLE_INT 		1 ///< @brief number of integer bits of RLT_ENABLE
#define RLT_ENABLE_SIGNED 		0 ///< @brief is RLT_ENABLE signed or unsigned

#define RLT_INTERP_MODE_FRAC 		0 ///< @brief number of fractional bits of RLT_INTERP_MODE
#define RLT_INTERP_MODE_INT 		1 ///< @brief number of integer bits of RLT_INTERP_MODE
#define RLT_INTERP_MODE_SIGNED 		0 ///< @brief is RLT_INTERP_MODE signed or unsigned


// RLT_LUT_VALUES_0_POINTS
#define RLT_LUT_0_POINTS_ODD_1_TO_15_FRAC 		7 ///< @brief number of fractional bits of RLT_LUT_0_POINTS_ODD_1_TO_15
#define RLT_LUT_0_POINTS_ODD_1_TO_15_INT 		9 ///< @brief number of integer bits of RLT_LUT_0_POINTS_ODD_1_TO_15
#define RLT_LUT_0_POINTS_ODD_1_TO_15_SIGNED 		0 ///< @brief is RLT_LUT_0_POINTS_ODD_1_TO_15 signed or unsigned

#define RLT_LUT_0_POINTS_EVEN_2_TO_16_FRAC 		7 ///< @brief number of fractional bits of RLT_LUT_0_POINTS_EVEN_2_TO_16
#define RLT_LUT_0_POINTS_EVEN_2_TO_16_INT 		9 ///< @brief number of integer bits of RLT_LUT_0_POINTS_EVEN_2_TO_16
#define RLT_LUT_0_POINTS_EVEN_2_TO_16_SIGNED 		0 ///< @brief is RLT_LUT_0_POINTS_EVEN_2_TO_16 signed or unsigned


// RLT_LUT_VALUES_1_POINTS
#define RLT_LUT_1_POINTS_ODD_1_TO_15_FRAC 		7 ///< @brief number of fractional bits of RLT_LUT_1_POINTS_ODD_1_TO_15
#define RLT_LUT_1_POINTS_ODD_1_TO_15_INT 		9 ///< @brief number of integer bits of RLT_LUT_1_POINTS_ODD_1_TO_15
#define RLT_LUT_1_POINTS_ODD_1_TO_15_SIGNED 		0 ///< @brief is RLT_LUT_1_POINTS_ODD_1_TO_15 signed or unsigned

#define RLT_LUT_1_POINTS_EVEN_2_TO_16_FRAC 		7 ///< @brief number of fractional bits of RLT_LUT_1_POINTS_EVEN_2_TO_16
#define RLT_LUT_1_POINTS_EVEN_2_TO_16_INT 		9 ///< @brief number of integer bits of RLT_LUT_1_POINTS_EVEN_2_TO_16
#define RLT_LUT_1_POINTS_EVEN_2_TO_16_SIGNED 		0 ///< @brief is RLT_LUT_1_POINTS_EVEN_2_TO_16 signed or unsigned


// RLT_LUT_VALUES_2_POINTS
#define RLT_LUT_2_POINTS_ODD_1_TO_15_FRAC 		7 ///< @brief number of fractional bits of RLT_LUT_2_POINTS_ODD_1_TO_15
#define RLT_LUT_2_POINTS_ODD_1_TO_15_INT 		9 ///< @brief number of integer bits of RLT_LUT_2_POINTS_ODD_1_TO_15
#define RLT_LUT_2_POINTS_ODD_1_TO_15_SIGNED 		0 ///< @brief is RLT_LUT_2_POINTS_ODD_1_TO_15 signed or unsigned

#define RLT_LUT_2_POINTS_EVEN_2_TO_16_FRAC 		7 ///< @brief number of fractional bits of RLT_LUT_2_POINTS_EVEN_2_TO_16
#define RLT_LUT_2_POINTS_EVEN_2_TO_16_INT 		9 ///< @brief number of integer bits of RLT_LUT_2_POINTS_EVEN_2_TO_16
#define RLT_LUT_2_POINTS_EVEN_2_TO_16_SIGNED 		0 ///< @brief is RLT_LUT_2_POINTS_EVEN_2_TO_16 signed or unsigned


// RLT_LUT_VALUES_3_POINTS
#define RLT_LUT_3_POINTS_ODD_1_TO_15_FRAC 		7 ///< @brief number of fractional bits of RLT_LUT_3_POINTS_ODD_1_TO_15
#define RLT_LUT_3_POINTS_ODD_1_TO_15_INT 		9 ///< @brief number of integer bits of RLT_LUT_3_POINTS_ODD_1_TO_15
#define RLT_LUT_3_POINTS_ODD_1_TO_15_SIGNED 		0 ///< @brief is RLT_LUT_3_POINTS_ODD_1_TO_15 signed or unsigned

#define RLT_LUT_3_POINTS_EVEN_2_TO_16_FRAC 		7 ///< @brief number of fractional bits of RLT_LUT_3_POINTS_EVEN_2_TO_16
#define RLT_LUT_3_POINTS_EVEN_2_TO_16_INT 		9 ///< @brief number of integer bits of RLT_LUT_3_POINTS_EVEN_2_TO_16
#define RLT_LUT_3_POINTS_EVEN_2_TO_16_SIGNED 		0 ///< @brief is RLT_LUT_3_POINTS_EVEN_2_TO_16 signed or unsigned


// AWS_GRID_START_COORDS
#define AWS_GRID_START_COL_FRAC 		0 ///< @brief number of fractional bits of AWS_GRID_START_COL
#define AWS_GRID_START_COL_INT 		16 ///< @brief number of integer bits of AWS_GRID_START_COL
#define AWS_GRID_START_COL_SIGNED 		0 ///< @brief is AWS_GRID_START_COL signed or unsigned

#define AWS_GRID_START_ROW_FRAC 		0 ///< @brief number of fractional bits of AWS_GRID_START_ROW
#define AWS_GRID_START_ROW_INT 		16 ///< @brief number of integer bits of AWS_GRID_START_ROW
#define AWS_GRID_START_ROW_SIGNED 		0 ///< @brief is AWS_GRID_START_ROW signed or unsigned


// AWS_GRID_TILE
#define AWS_GRID_TILE_CFA_WIDTH_MIN1_FRAC 		0 ///< @brief number of fractional bits of AWS_GRID_TILE_CFA_WIDTH_MIN1
#define AWS_GRID_TILE_CFA_WIDTH_MIN1_INT 		12 ///< @brief number of integer bits of AWS_GRID_TILE_CFA_WIDTH_MIN1
#define AWS_GRID_TILE_CFA_WIDTH_MIN1_SIGNED 		0 ///< @brief is AWS_GRID_TILE_CFA_WIDTH_MIN1 signed or unsigned

#define AWS_GRID_TILE_CFA_HEIGHT_MIN1_FRAC 		0 ///< @brief number of fractional bits of AWS_GRID_TILE_CFA_HEIGHT_MIN1
#define AWS_GRID_TILE_CFA_HEIGHT_MIN1_INT 		12 ///< @brief number of integer bits of AWS_GRID_TILE_CFA_HEIGHT_MIN1
#define AWS_GRID_TILE_CFA_HEIGHT_MIN1_SIGNED 		0 ///< @brief is AWS_GRID_TILE_CFA_HEIGHT_MIN1 signed or unsigned


// AWS_LOG2_QEFF
#define AWS_LOG2_R_QEFF_FRAC 		5 ///< @brief number of fractional bits of AWS_LOG2_R_QEFF
#define AWS_LOG2_R_QEFF_INT 		8 ///< @brief number of integer bits of AWS_LOG2_R_QEFF
#define AWS_LOG2_R_QEFF_SIGNED 		1 ///< @brief is AWS_LOG2_R_QEFF signed or unsigned

#define AWS_LOG2_B_QEFF_FRAC 		5 ///< @brief number of fractional bits of AWS_LOG2_B_QEFF
#define AWS_LOG2_B_QEFF_INT 		8 ///< @brief number of integer bits of AWS_LOG2_B_QEFF
#define AWS_LOG2_B_QEFF_SIGNED 		1 ///< @brief is AWS_LOG2_B_QEFF signed or unsigned


// AWS_THRESHOLDS_0
#define AWS_R_DARK_THRESH_FRAC 		8 ///< @brief number of fractional bits of AWS_R_DARK_THRESH
#define AWS_R_DARK_THRESH_INT 		8 ///< @brief number of integer bits of AWS_R_DARK_THRESH
#define AWS_R_DARK_THRESH_SIGNED 		0 ///< @brief is AWS_R_DARK_THRESH signed or unsigned

#define AWS_G_DARK_THRESH_FRAC 		8 ///< @brief number of fractional bits of AWS_G_DARK_THRESH
#define AWS_G_DARK_THRESH_INT 		8 ///< @brief number of integer bits of AWS_G_DARK_THRESH
#define AWS_G_DARK_THRESH_SIGNED 		0 ///< @brief is AWS_G_DARK_THRESH signed or unsigned


// AWS_THRESHOLDS_1
#define AWS_B_DARK_THRESH_FRAC 		8 ///< @brief number of fractional bits of AWS_B_DARK_THRESH
#define AWS_B_DARK_THRESH_INT 		8 ///< @brief number of integer bits of AWS_B_DARK_THRESH
#define AWS_B_DARK_THRESH_SIGNED 		0 ///< @brief is AWS_B_DARK_THRESH signed or unsigned

#define AWS_R_CLIP_THRESH_FRAC 		8 ///< @brief number of fractional bits of AWS_R_CLIP_THRESH
#define AWS_R_CLIP_THRESH_INT 		8 ///< @brief number of integer bits of AWS_R_CLIP_THRESH
#define AWS_R_CLIP_THRESH_SIGNED 		0 ///< @brief is AWS_R_CLIP_THRESH signed or unsigned


// AWS_THRESHOLDS_2
#define AWS_G_CLIP_THRESH_FRAC 		8 ///< @brief number of fractional bits of AWS_G_CLIP_THRESH
#define AWS_G_CLIP_THRESH_INT 		8 ///< @brief number of integer bits of AWS_G_CLIP_THRESH
#define AWS_G_CLIP_THRESH_SIGNED 		0 ///< @brief is AWS_G_CLIP_THRESH signed or unsigned

#define AWS_B_CLIP_THRESH_FRAC 		8 ///< @brief number of fractional bits of AWS_B_CLIP_THRESH
#define AWS_B_CLIP_THRESH_INT 		8 ///< @brief number of integer bits of AWS_B_CLIP_THRESH
#define AWS_B_CLIP_THRESH_SIGNED 		0 ///< @brief is AWS_B_CLIP_THRESH signed or unsigned


// AWS_BB_DIST
#define AWS_BB_DIST_FRAC 		5 ///< @brief number of fractional bits of AWS_BB_DIST
#define AWS_BB_DIST_INT 		7 ///< @brief number of integer bits of AWS_BB_DIST
#define AWS_BB_DIST_SIGNED 		0 ///< @brief is AWS_BB_DIST signed or unsigned


// AWS_DEBUG_BITMAP
#define AWS_DEBUG_BITMAP_ENABLE_FRAC 		0 ///< @brief number of fractional bits of AWS_DEBUG_BITMAP_ENABLE
#define AWS_DEBUG_BITMAP_ENABLE_INT 		1 ///< @brief number of integer bits of AWS_DEBUG_BITMAP_ENABLE
#define AWS_DEBUG_BITMAP_ENABLE_SIGNED 		0 ///< @brief is AWS_DEBUG_BITMAP_ENABLE signed or unsigned


// LSH_COLOUR_ALIGNMENT_X
#define LSH_COLOUR_SKIP_X_FRAC 		0 ///< @brief number of fractional bits of LSH_COLOUR_SKIP_X
#define LSH_COLOUR_SKIP_X_INT 		4 ///< @brief number of integer bits of LSH_COLOUR_SKIP_X
#define LSH_COLOUR_SKIP_X_SIGNED 		0 ///< @brief is LSH_COLOUR_SKIP_X signed or unsigned

#define LSH_COLOUR_OFFSET_X_FRAC 		0 ///< @brief number of fractional bits of LSH_COLOUR_OFFSET_X
#define LSH_COLOUR_OFFSET_X_INT 		12 ///< @brief number of integer bits of LSH_COLOUR_OFFSET_X
#define LSH_COLOUR_OFFSET_X_SIGNED 		0 ///< @brief is LSH_COLOUR_OFFSET_X signed or unsigned


// LSH_ALIGNMENT_X
#define LSH_SKIP_X_FRAC 		0 ///< @brief number of fractional bits of LSH_SKIP_X
#define LSH_SKIP_X_INT 		4 ///< @brief number of integer bits of LSH_SKIP_X
#define LSH_SKIP_X_SIGNED 		0 ///< @brief is LSH_SKIP_X signed or unsigned

#define LSH_OFFSET_X_FRAC 		0 ///< @brief number of fractional bits of LSH_OFFSET_X
#define LSH_OFFSET_X_INT 		13 ///< @brief number of integer bits of LSH_OFFSET_X
#define LSH_OFFSET_X_SIGNED 		0 ///< @brief is LSH_OFFSET_X signed or unsigned


// LSH_GAIN_0
#define WHITE_BALANCE_GAIN_0_FRAC 		10 ///< @brief number of fractional bits of WHITE_BALANCE_GAIN_0
#define WHITE_BALANCE_GAIN_0_INT 		6 ///< @brief number of integer bits of WHITE_BALANCE_GAIN_0
#define WHITE_BALANCE_GAIN_0_SIGNED 		0 ///< @brief is WHITE_BALANCE_GAIN_0 signed or unsigned

#define WHITE_BALANCE_GAIN_1_FRAC 		10 ///< @brief number of fractional bits of WHITE_BALANCE_GAIN_1
#define WHITE_BALANCE_GAIN_1_INT 		6 ///< @brief number of integer bits of WHITE_BALANCE_GAIN_1
#define WHITE_BALANCE_GAIN_1_SIGNED 		0 ///< @brief is WHITE_BALANCE_GAIN_1 signed or unsigned


// LSH_GAIN_1
#define WHITE_BALANCE_GAIN_2_FRAC 		10 ///< @brief number of fractional bits of WHITE_BALANCE_GAIN_2
#define WHITE_BALANCE_GAIN_2_INT 		6 ///< @brief number of integer bits of WHITE_BALANCE_GAIN_2
#define WHITE_BALANCE_GAIN_2_SIGNED 		0 ///< @brief is WHITE_BALANCE_GAIN_2 signed or unsigned

#define WHITE_BALANCE_GAIN_3_FRAC 		10 ///< @brief number of fractional bits of WHITE_BALANCE_GAIN_3
#define WHITE_BALANCE_GAIN_3_INT 		6 ///< @brief number of integer bits of WHITE_BALANCE_GAIN_3
#define WHITE_BALANCE_GAIN_3_SIGNED 		0 ///< @brief is WHITE_BALANCE_GAIN_3 signed or unsigned


// LSH_CLIP_0
#define WHITE_BALANCE_CLIP_0_FRAC 		4 ///< @brief number of fractional bits of WHITE_BALANCE_CLIP_0
#define WHITE_BALANCE_CLIP_0_INT 		9 ///< @brief number of integer bits of WHITE_BALANCE_CLIP_0
#define WHITE_BALANCE_CLIP_0_SIGNED 		0 ///< @brief is WHITE_BALANCE_CLIP_0 signed or unsigned

#define WHITE_BALANCE_CLIP_1_FRAC 		4 ///< @brief number of fractional bits of WHITE_BALANCE_CLIP_1
#define WHITE_BALANCE_CLIP_1_INT 		9 ///< @brief number of integer bits of WHITE_BALANCE_CLIP_1
#define WHITE_BALANCE_CLIP_1_SIGNED 		0 ///< @brief is WHITE_BALANCE_CLIP_1 signed or unsigned


// LSH_CLIP_1
#define WHITE_BALANCE_CLIP_2_FRAC 		4 ///< @brief number of fractional bits of WHITE_BALANCE_CLIP_2
#define WHITE_BALANCE_CLIP_2_INT 		9 ///< @brief number of integer bits of WHITE_BALANCE_CLIP_2
#define WHITE_BALANCE_CLIP_2_SIGNED 		0 ///< @brief is WHITE_BALANCE_CLIP_2 signed or unsigned

#define WHITE_BALANCE_CLIP_3_FRAC 		4 ///< @brief number of fractional bits of WHITE_BALANCE_CLIP_3
#define WHITE_BALANCE_CLIP_3_INT 		9 ///< @brief number of integer bits of WHITE_BALANCE_CLIP_3
#define WHITE_BALANCE_CLIP_3_SIGNED 		0 ///< @brief is WHITE_BALANCE_CLIP_3 signed or unsigned


// LSH_GRADIENTS_X
#define SHADING_GRADIENT_X_FRAC 		12 ///< @brief number of fractional bits of SHADING_GRADIENT_X
#define SHADING_GRADIENT_X_INT 		-1 ///< @brief number of integer bits of SHADING_GRADIENT_X
#define SHADING_GRADIENT_X_SIGNED 		1 ///< @brief is SHADING_GRADIENT_X signed or unsigned


// FOS_GRID_START_COORDS
#define FOS_GRID_START_COL_FRAC 		0 ///< @brief number of fractional bits of FOS_GRID_START_COL
#define FOS_GRID_START_COL_INT 		16 ///< @brief number of integer bits of FOS_GRID_START_COL
#define FOS_GRID_START_COL_SIGNED 		0 ///< @brief is FOS_GRID_START_COL signed or unsigned

#define FOS_GRID_START_ROW_FRAC 		0 ///< @brief number of fractional bits of FOS_GRID_START_ROW
#define FOS_GRID_START_ROW_INT 		16 ///< @brief number of integer bits of FOS_GRID_START_ROW
#define FOS_GRID_START_ROW_SIGNED 		0 ///< @brief is FOS_GRID_START_ROW signed or unsigned


// FOS_GRID_TILE
#define FOS_GRID_TILE_WIDTH_FRAC 		0 ///< @brief number of fractional bits of FOS_GRID_TILE_WIDTH
#define FOS_GRID_TILE_WIDTH_INT 		14 ///< @brief number of integer bits of FOS_GRID_TILE_WIDTH
#define FOS_GRID_TILE_WIDTH_SIGNED 		0 ///< @brief is FOS_GRID_TILE_WIDTH signed or unsigned

#define FOS_GRID_TILE_HEIGHT_FRAC 		0 ///< @brief number of fractional bits of FOS_GRID_TILE_HEIGHT
#define FOS_GRID_TILE_HEIGHT_INT 		14 ///< @brief number of integer bits of FOS_GRID_TILE_HEIGHT
#define FOS_GRID_TILE_HEIGHT_SIGNED 		0 ///< @brief is FOS_GRID_TILE_HEIGHT signed or unsigned


// FOS_ROI_COORDS_START
#define FOS_ROI_START_COL_FRAC 		0 ///< @brief number of fractional bits of FOS_ROI_START_COL
#define FOS_ROI_START_COL_INT 		15 ///< @brief number of integer bits of FOS_ROI_START_COL
#define FOS_ROI_START_COL_SIGNED 		0 ///< @brief is FOS_ROI_START_COL signed or unsigned

#define FOS_ROI_START_ROW_FRAC 		0 ///< @brief number of fractional bits of FOS_ROI_START_ROW
#define FOS_ROI_START_ROW_INT 		15 ///< @brief number of integer bits of FOS_ROI_START_ROW
#define FOS_ROI_START_ROW_SIGNED 		0 ///< @brief is FOS_ROI_START_ROW signed or unsigned


// FOS_ROI_COORDS_END
#define FOS_ROI_END_COL_FRAC 		0 ///< @brief number of fractional bits of FOS_ROI_END_COL
#define FOS_ROI_END_COL_INT 		15 ///< @brief number of integer bits of FOS_ROI_END_COL
#define FOS_ROI_END_COL_SIGNED 		0 ///< @brief is FOS_ROI_END_COL signed or unsigned

#define FOS_ROI_END_ROW_FRAC 		0 ///< @brief number of fractional bits of FOS_ROI_END_ROW
#define FOS_ROI_END_ROW_INT 		15 ///< @brief number of integer bits of FOS_ROI_END_ROW
#define FOS_ROI_END_ROW_SIGNED 		0 ///< @brief is FOS_ROI_END_ROW signed or unsigned


// PIX_THRESH_LUT_SEC0_POINTS
#define PIX_THRESH_LUT_POINTS_0TO7_0_FRAC 		5 ///< @brief number of fractional bits of PIX_THRESH_LUT_POINTS_0TO7_0
#define PIX_THRESH_LUT_POINTS_0TO7_0_INT 		8 ///< @brief number of integer bits of PIX_THRESH_LUT_POINTS_0TO7_0
#define PIX_THRESH_LUT_POINTS_0TO7_0_SIGNED 		0 ///< @brief is PIX_THRESH_LUT_POINTS_0TO7_0 signed or unsigned

#define PIX_THRESH_LUT_POINTS_0TO7_1_FRAC 		5 ///< @brief number of fractional bits of PIX_THRESH_LUT_POINTS_0TO7_1
#define PIX_THRESH_LUT_POINTS_0TO7_1_INT 		8 ///< @brief number of integer bits of PIX_THRESH_LUT_POINTS_0TO7_1
#define PIX_THRESH_LUT_POINTS_0TO7_1_SIGNED 		0 ///< @brief is PIX_THRESH_LUT_POINTS_0TO7_1 signed or unsigned


// PIX_THRESH_LUT_SEC1_POINTS
#define PIX_THRESH_LUT_POINTS_8TO13_0_FRAC 		5 ///< @brief number of fractional bits of PIX_THRESH_LUT_POINTS_8TO13_0
#define PIX_THRESH_LUT_POINTS_8TO13_0_INT 		8 ///< @brief number of integer bits of PIX_THRESH_LUT_POINTS_8TO13_0
#define PIX_THRESH_LUT_POINTS_8TO13_0_SIGNED 		0 ///< @brief is PIX_THRESH_LUT_POINTS_8TO13_0 signed or unsigned

#define PIX_THRESH_LUT_POINTS_8TO13_1_FRAC 		5 ///< @brief number of fractional bits of PIX_THRESH_LUT_POINTS_8TO13_1
#define PIX_THRESH_LUT_POINTS_8TO13_1_INT 		8 ///< @brief number of integer bits of PIX_THRESH_LUT_POINTS_8TO13_1
#define PIX_THRESH_LUT_POINTS_8TO13_1_SIGNED 		0 ///< @brief is PIX_THRESH_LUT_POINTS_8TO13_1 signed or unsigned


// PIX_THRESH_LUT_SEC2_POINTS
#define PIX_THRESH_LUT_POINTS_14TO19_0_FRAC 		5 ///< @brief number of fractional bits of PIX_THRESH_LUT_POINTS_14TO19_0
#define PIX_THRESH_LUT_POINTS_14TO19_0_INT 		8 ///< @brief number of integer bits of PIX_THRESH_LUT_POINTS_14TO19_0
#define PIX_THRESH_LUT_POINTS_14TO19_0_SIGNED 		0 ///< @brief is PIX_THRESH_LUT_POINTS_14TO19_0 signed or unsigned

#define PIX_THRESH_LUT_POINTS_14TO19_1_FRAC 		5 ///< @brief number of fractional bits of PIX_THRESH_LUT_POINTS_14TO19_1
#define PIX_THRESH_LUT_POINTS_14TO19_1_INT 		8 ///< @brief number of integer bits of PIX_THRESH_LUT_POINTS_14TO19_1
#define PIX_THRESH_LUT_POINTS_14TO19_1_SIGNED 		0 ///< @brief is PIX_THRESH_LUT_POINTS_14TO19_1 signed or unsigned


// PIX_THRESH_LUT_SEC3_POINTS
#define PIX_THRESH_LUT_POINT_20_FRAC 		5 ///< @brief number of fractional bits of PIX_THRESH_LUT_POINT_20
#define PIX_THRESH_LUT_POINT_20_INT 		8 ///< @brief number of integer bits of PIX_THRESH_LUT_POINT_20
#define PIX_THRESH_LUT_POINT_20_SIGNED 		0 ///< @brief is PIX_THRESH_LUT_POINT_20 signed or unsigned


// DENOISE_CONTROL
#define DENOISE_COMBINE_1_2_FRAC 		0 ///< @brief number of fractional bits of DENOISE_COMBINE_1_2
#define DENOISE_COMBINE_1_2_INT 		1 ///< @brief number of integer bits of DENOISE_COMBINE_1_2
#define DENOISE_COMBINE_1_2_SIGNED 		0 ///< @brief is DENOISE_COMBINE_1_2 signed or unsigned

#define LOG2_GREYSC_PIXTHRESH_MULT_FRAC 		3 ///< @brief number of fractional bits of LOG2_GREYSC_PIXTHRESH_MULT
#define LOG2_GREYSC_PIXTHRESH_MULT_INT 		3 ///< @brief number of integer bits of LOG2_GREYSC_PIXTHRESH_MULT
#define LOG2_GREYSC_PIXTHRESH_MULT_SIGNED 		1 ///< @brief is LOG2_GREYSC_PIXTHRESH_MULT signed or unsigned


// DPF_ENABLE
#define DPF_DETECT_ENABLE_FRAC 		0 ///< @brief number of fractional bits of DPF_DETECT_ENABLE
#define DPF_DETECT_ENABLE_INT 		1 ///< @brief number of integer bits of DPF_DETECT_ENABLE
#define DPF_DETECT_ENABLE_SIGNED 		0 ///< @brief is DPF_DETECT_ENABLE signed or unsigned


// DPF_SENSITIVITY
#define DPF_THRESHOLD_FRAC 		0 ///< @brief number of fractional bits of DPF_THRESHOLD
#define DPF_THRESHOLD_INT 		8 ///< @brief number of integer bits of DPF_THRESHOLD
#define DPF_THRESHOLD_SIGNED 		0 ///< @brief is DPF_THRESHOLD signed or unsigned

#define DPF_WEIGHT_FRAC 		0 ///< @brief number of fractional bits of DPF_WEIGHT
#define DPF_WEIGHT_INT 		6 ///< @brief number of integer bits of DPF_WEIGHT
#define DPF_WEIGHT_SIGNED 		0 ///< @brief is DPF_WEIGHT signed or unsigned


// DPF_MODIFICATIONS_MAP_SIZE_CACHE
#define DPF_WRITE_MAP_SIZE_FRAC 		0 ///< @brief number of fractional bits of DPF_WRITE_MAP_SIZE
#define DPF_WRITE_MAP_SIZE_INT 		15 ///< @brief number of integer bits of DPF_WRITE_MAP_SIZE
#define DPF_WRITE_MAP_SIZE_SIGNED 		0 ///< @brief is DPF_WRITE_MAP_SIZE signed or unsigned


// DPF_MODIFICATIONS_THRESHOLD
#define DPF_WRITE_MAP_POS_THR_FRAC 		0 ///< @brief number of fractional bits of DPF_WRITE_MAP_POS_THR
#define DPF_WRITE_MAP_POS_THR_INT 		8 ///< @brief number of integer bits of DPF_WRITE_MAP_POS_THR
#define DPF_WRITE_MAP_POS_THR_SIGNED 		0 ///< @brief is DPF_WRITE_MAP_POS_THR signed or unsigned

#define DPF_WRITE_MAP_NEG_THR_FRAC 		0 ///< @brief number of fractional bits of DPF_WRITE_MAP_NEG_THR
#define DPF_WRITE_MAP_NEG_THR_INT 		8 ///< @brief number of integer bits of DPF_WRITE_MAP_NEG_THR
#define DPF_WRITE_MAP_NEG_THR_SIGNED 		0 ///< @brief is DPF_WRITE_MAP_NEG_THR signed or unsigned


// LAT_CA_DELTA_COEFF_RED
#define LAT_CA_COEFF_Y_RED_FRAC 		7 ///< @brief number of fractional bits of LAT_CA_COEFF_Y_RED
#define LAT_CA_COEFF_Y_RED_INT 		5 ///< @brief number of integer bits of LAT_CA_COEFF_Y_RED
#define LAT_CA_COEFF_Y_RED_SIGNED 		1 ///< @brief is LAT_CA_COEFF_Y_RED signed or unsigned

#define LAT_CA_COEFF_X_RED_FRAC 		7 ///< @brief number of fractional bits of LAT_CA_COEFF_X_RED
#define LAT_CA_COEFF_X_RED_INT 		5 ///< @brief number of integer bits of LAT_CA_COEFF_X_RED
#define LAT_CA_COEFF_X_RED_SIGNED 		1 ///< @brief is LAT_CA_COEFF_X_RED signed or unsigned


// LAT_CA_DELTA_COEFF_BLUE
#define LAT_CA_COEFF_Y_BLUE_FRAC 		7 ///< @brief number of fractional bits of LAT_CA_COEFF_Y_BLUE
#define LAT_CA_COEFF_Y_BLUE_INT 		5 ///< @brief number of integer bits of LAT_CA_COEFF_Y_BLUE
#define LAT_CA_COEFF_Y_BLUE_SIGNED 		1 ///< @brief is LAT_CA_COEFF_Y_BLUE signed or unsigned

#define LAT_CA_COEFF_X_BLUE_FRAC 		7 ///< @brief number of fractional bits of LAT_CA_COEFF_X_BLUE
#define LAT_CA_COEFF_X_BLUE_INT 		5 ///< @brief number of integer bits of LAT_CA_COEFF_X_BLUE
#define LAT_CA_COEFF_X_BLUE_SIGNED 		1 ///< @brief is LAT_CA_COEFF_X_BLUE signed or unsigned


// LAT_CA_OFFSET_RED
#define LAT_CA_OFFSET_Y_RED_FRAC 		0 ///< @brief number of fractional bits of LAT_CA_OFFSET_Y_RED
#define LAT_CA_OFFSET_Y_RED_INT 		14 ///< @brief number of integer bits of LAT_CA_OFFSET_Y_RED
#define LAT_CA_OFFSET_Y_RED_SIGNED 		1 ///< @brief is LAT_CA_OFFSET_Y_RED signed or unsigned

#define LAT_CA_OFFSET_X_RED_FRAC 		0 ///< @brief number of fractional bits of LAT_CA_OFFSET_X_RED
#define LAT_CA_OFFSET_X_RED_INT 		14 ///< @brief number of integer bits of LAT_CA_OFFSET_X_RED
#define LAT_CA_OFFSET_X_RED_SIGNED 		1 ///< @brief is LAT_CA_OFFSET_X_RED signed or unsigned


// LAT_CA_OFFSET_BLUE
#define LAT_CA_OFFSET_Y_BLUE_FRAC 		0 ///< @brief number of fractional bits of LAT_CA_OFFSET_Y_BLUE
#define LAT_CA_OFFSET_Y_BLUE_INT 		14 ///< @brief number of integer bits of LAT_CA_OFFSET_Y_BLUE
#define LAT_CA_OFFSET_Y_BLUE_SIGNED 		1 ///< @brief is LAT_CA_OFFSET_Y_BLUE signed or unsigned

#define LAT_CA_OFFSET_X_BLUE_FRAC 		0 ///< @brief number of fractional bits of LAT_CA_OFFSET_X_BLUE
#define LAT_CA_OFFSET_X_BLUE_INT 		14 ///< @brief number of integer bits of LAT_CA_OFFSET_X_BLUE
#define LAT_CA_OFFSET_X_BLUE_SIGNED 		1 ///< @brief is LAT_CA_OFFSET_X_BLUE signed or unsigned


// LAT_CA_NORMALIZATION
#define LAT_CA_SHIFT_Y_FRAC 		0 ///< @brief number of fractional bits of LAT_CA_SHIFT_Y
#define LAT_CA_SHIFT_Y_INT 		2 ///< @brief number of integer bits of LAT_CA_SHIFT_Y
#define LAT_CA_SHIFT_Y_SIGNED 		0 ///< @brief is LAT_CA_SHIFT_Y signed or unsigned

#define LAT_CA_DEC_Y_FRAC 		0 ///< @brief number of fractional bits of LAT_CA_DEC_Y
#define LAT_CA_DEC_Y_INT 		4 ///< @brief number of integer bits of LAT_CA_DEC_Y
#define LAT_CA_DEC_Y_SIGNED 		0 ///< @brief is LAT_CA_DEC_Y signed or unsigned

#define LAT_CA_SHIFT_X_FRAC 		0 ///< @brief number of fractional bits of LAT_CA_SHIFT_X
#define LAT_CA_SHIFT_X_INT 		2 ///< @brief number of integer bits of LAT_CA_SHIFT_X
#define LAT_CA_SHIFT_X_SIGNED 		0 ///< @brief is LAT_CA_SHIFT_X signed or unsigned

#define LAT_CA_DEC_X_FRAC 		0 ///< @brief number of fractional bits of LAT_CA_DEC_X
#define LAT_CA_DEC_X_INT 		4 ///< @brief number of integer bits of LAT_CA_DEC_X
#define LAT_CA_DEC_X_SIGNED 		0 ///< @brief is LAT_CA_DEC_X signed or unsigned


// WBC_GAIN
#define WBC_GAIN_0_FRAC 		4 ///< @brief number of fractional bits of WBC_GAIN_0
#define WBC_GAIN_0_INT 		4 ///< @brief number of integer bits of WBC_GAIN_0
#define WBC_GAIN_0_SIGNED 		0 ///< @brief is WBC_GAIN_0 signed or unsigned

#define WBC_GAIN_1_FRAC 		4 ///< @brief number of fractional bits of WBC_GAIN_1
#define WBC_GAIN_1_INT 		4 ///< @brief number of integer bits of WBC_GAIN_1
#define WBC_GAIN_1_SIGNED 		0 ///< @brief is WBC_GAIN_1 signed or unsigned

#define WBC_GAIN_2_FRAC 		4 ///< @brief number of fractional bits of WBC_GAIN_2
#define WBC_GAIN_2_INT 		4 ///< @brief number of integer bits of WBC_GAIN_2
#define WBC_GAIN_2_SIGNED 		0 ///< @brief is WBC_GAIN_2 signed or unsigned


// WBC_THRESHOLD
#define WBC_THRES_0_FRAC 		0 ///< @brief number of fractional bits of WBC_THRES_0
#define WBC_THRES_0_INT 		9 ///< @brief number of integer bits of WBC_THRES_0
#define WBC_THRES_0_SIGNED 		0 ///< @brief is WBC_THRES_0 signed or unsigned

#define WBC_THRES_1_FRAC 		0 ///< @brief number of fractional bits of WBC_THRES_1
#define WBC_THRES_1_INT 		9 ///< @brief number of integer bits of WBC_THRES_1
#define WBC_THRES_1_SIGNED 		0 ///< @brief is WBC_THRES_1 signed or unsigned

#define WBC_THRES_2_FRAC 		0 ///< @brief number of fractional bits of WBC_THRES_2
#define WBC_THRES_2_INT 		9 ///< @brief number of integer bits of WBC_THRES_2
#define WBC_THRES_2_SIGNED 		0 ///< @brief is WBC_THRES_2 signed or unsigned

#define WBC_MODE_FRAC 		0 ///< @brief number of fractional bits of WBC_MODE
#define WBC_MODE_INT 		1 ///< @brief number of integer bits of WBC_MODE
#define WBC_MODE_SIGNED 		0 ///< @brief is WBC_MODE signed or unsigned


// CC_COEFFS_A
#define CC_COEFF_0_FRAC 		9 ///< @brief number of fractional bits of CC_COEFF_0
#define CC_COEFF_0_INT 		3 ///< @brief number of integer bits of CC_COEFF_0
#define CC_COEFF_0_SIGNED 		1 ///< @brief is CC_COEFF_0 signed or unsigned

#define CC_COEFF_1_FRAC 		9 ///< @brief number of fractional bits of CC_COEFF_1
#define CC_COEFF_1_INT 		3 ///< @brief number of integer bits of CC_COEFF_1
#define CC_COEFF_1_SIGNED 		1 ///< @brief is CC_COEFF_1 signed or unsigned


// CC_COEFFS_B
#define CC_COEFF_2_FRAC 		9 ///< @brief number of fractional bits of CC_COEFF_2
#define CC_COEFF_2_INT 		3 ///< @brief number of integer bits of CC_COEFF_2
#define CC_COEFF_2_SIGNED 		1 ///< @brief is CC_COEFF_2 signed or unsigned

#define CC_COEFF_3_FRAC 		9 ///< @brief number of fractional bits of CC_COEFF_3
#define CC_COEFF_3_INT 		3 ///< @brief number of integer bits of CC_COEFF_3
#define CC_COEFF_3_SIGNED 		1 ///< @brief is CC_COEFF_3 signed or unsigned


// CC_OFFSETS_1_0
#define CC_OFFSET_0_FRAC 		3 ///< @brief number of fractional bits of CC_OFFSET_0
#define CC_OFFSET_0_INT 		9 ///< @brief number of integer bits of CC_OFFSET_0
#define CC_OFFSET_0_SIGNED 		1 ///< @brief is CC_OFFSET_0 signed or unsigned

#define CC_OFFSET_1_FRAC 		3 ///< @brief number of fractional bits of CC_OFFSET_1
#define CC_OFFSET_1_INT 		9 ///< @brief number of integer bits of CC_OFFSET_1
#define CC_OFFSET_1_SIGNED 		1 ///< @brief is CC_OFFSET_1 signed or unsigned


// CC_OFFSETS_2
#define CC_OFFSET_2_FRAC 		3 ///< @brief number of fractional bits of CC_OFFSET_2
#define CC_OFFSET_2_INT 		9 ///< @brief number of integer bits of CC_OFFSET_2
#define CC_OFFSET_2_SIGNED 		1 ///< @brief is CC_OFFSET_2 signed or unsigned


// MGM_CLIP_IN
#define MGM_CLIP_MIN_FRAC 		2 ///< @brief number of fractional bits of MGM_CLIP_MIN
#define MGM_CLIP_MIN_INT 		10 ///< @brief number of integer bits of MGM_CLIP_MIN
#define MGM_CLIP_MIN_SIGNED 		1 ///< @brief is MGM_CLIP_MIN signed or unsigned

#define MGM_CLIP_MAX_FRAC 		2 ///< @brief number of fractional bits of MGM_CLIP_MAX
#define MGM_CLIP_MAX_INT 		10 ///< @brief number of integer bits of MGM_CLIP_MAX
#define MGM_CLIP_MAX_SIGNED 		1 ///< @brief is MGM_CLIP_MAX signed or unsigned


// MGM_CORE_IN_OUT
#define MGM_SRC_NORM_FRAC 		2 ///< @brief number of fractional bits of MGM_SRC_NORM
#define MGM_SRC_NORM_INT 		10 ///< @brief number of integer bits of MGM_SRC_NORM
#define MGM_SRC_NORM_SIGNED 		1 ///< @brief is MGM_SRC_NORM signed or unsigned


// MGM_SLOPE_0_1
#define MGM_SLOPE_0_FRAC 		10 ///< @brief number of fractional bits of MGM_SLOPE_0
#define MGM_SLOPE_0_INT 		2 ///< @brief number of integer bits of MGM_SLOPE_0
#define MGM_SLOPE_0_SIGNED 		0 ///< @brief is MGM_SLOPE_0 signed or unsigned

#define MGM_SLOPE_1_FRAC 		10 ///< @brief number of fractional bits of MGM_SLOPE_1
#define MGM_SLOPE_1_INT 		2 ///< @brief number of integer bits of MGM_SLOPE_1
#define MGM_SLOPE_1_SIGNED 		0 ///< @brief is MGM_SLOPE_1 signed or unsigned


// MGM_SLOPE_2_R
#define MGM_SLOPE_2_FRAC 		10 ///< @brief number of fractional bits of MGM_SLOPE_2
#define MGM_SLOPE_2_INT 		2 ///< @brief number of integer bits of MGM_SLOPE_2
#define MGM_SLOPE_2_SIGNED 		0 ///< @brief is MGM_SLOPE_2 signed or unsigned


// MGM_COEFFS_0_3
#define MGM_COEFF_0_FRAC 		6 ///< @brief number of fractional bits of MGM_COEFF_0
#define MGM_COEFF_0_INT 		2 ///< @brief number of integer bits of MGM_COEFF_0
#define MGM_COEFF_0_SIGNED 		1 ///< @brief is MGM_COEFF_0 signed or unsigned

#define MGM_COEFF_1_FRAC 		6 ///< @brief number of fractional bits of MGM_COEFF_1
#define MGM_COEFF_1_INT 		2 ///< @brief number of integer bits of MGM_COEFF_1
#define MGM_COEFF_1_SIGNED 		1 ///< @brief is MGM_COEFF_1 signed or unsigned

#define MGM_COEFF_2_FRAC 		6 ///< @brief number of fractional bits of MGM_COEFF_2
#define MGM_COEFF_2_INT 		2 ///< @brief number of integer bits of MGM_COEFF_2
#define MGM_COEFF_2_SIGNED 		1 ///< @brief is MGM_COEFF_2 signed or unsigned

#define MGM_COEFF_3_FRAC 		6 ///< @brief number of fractional bits of MGM_COEFF_3
#define MGM_COEFF_3_INT 		2 ///< @brief number of integer bits of MGM_COEFF_3
#define MGM_COEFF_3_SIGNED 		1 ///< @brief is MGM_COEFF_3 signed or unsigned


// MGM_COEFFS_4_5
#define MGM_COEFF_4_FRAC 		6 ///< @brief number of fractional bits of MGM_COEFF_4
#define MGM_COEFF_4_INT 		2 ///< @brief number of integer bits of MGM_COEFF_4
#define MGM_COEFF_4_SIGNED 		1 ///< @brief is MGM_COEFF_4 signed or unsigned

#define MGM_COEFF_5_FRAC 		6 ///< @brief number of fractional bits of MGM_COEFF_5
#define MGM_COEFF_5_INT 		2 ///< @brief number of integer bits of MGM_COEFF_5
#define MGM_COEFF_5_SIGNED 		1 ///< @brief is MGM_COEFF_5 signed or unsigned


// HDF_RESERVED_VALS
#define HDF_RESERVED_VALS_FRAC 		0 ///< @brief number of fractional bits of HDF_RESERVED_VALS
#define HDF_RESERVED_VALS_INT 		32 ///< @brief number of integer bits of HDF_RESERVED_VALS
#define HDF_RESERVED_VALS_SIGNED 		0 ///< @brief is HDF_RESERVED_VALS signed or unsigned


// GMA_BYPASS
#define GMA_BYPASS_FRAC 		0 ///< @brief number of fractional bits of GMA_BYPASS
#define GMA_BYPASS_INT 		1 ///< @brief number of integer bits of GMA_BYPASS
#define GMA_BYPASS_SIGNED 		0 ///< @brief is GMA_BYPASS signed or unsigned


// WBS_ROI_START_COORDS
#define WBS_ROI_COL_START_FRAC 		0 ///< @brief number of fractional bits of WBS_ROI_COL_START
#define WBS_ROI_COL_START_INT 		15 ///< @brief number of integer bits of WBS_ROI_COL_START
#define WBS_ROI_COL_START_SIGNED 		0 ///< @brief is WBS_ROI_COL_START signed or unsigned

#define WBS_ROI_ROW_START_FRAC 		0 ///< @brief number of fractional bits of WBS_ROI_ROW_START
#define WBS_ROI_ROW_START_INT 		15 ///< @brief number of integer bits of WBS_ROI_ROW_START
#define WBS_ROI_ROW_START_SIGNED 		0 ///< @brief is WBS_ROI_ROW_START signed or unsigned


// WBS_ROI_END_COORDS
#define WBS_ROI_COL_END_FRAC 		0 ///< @brief number of fractional bits of WBS_ROI_COL_END
#define WBS_ROI_COL_END_INT 		15 ///< @brief number of integer bits of WBS_ROI_COL_END
#define WBS_ROI_COL_END_SIGNED 		0 ///< @brief is WBS_ROI_COL_END signed or unsigned

#define WBS_ROI_ROW_END_FRAC 		0 ///< @brief number of fractional bits of WBS_ROI_ROW_END
#define WBS_ROI_ROW_END_INT 		15 ///< @brief number of integer bits of WBS_ROI_ROW_END
#define WBS_ROI_ROW_END_SIGNED 		0 ///< @brief is WBS_ROI_ROW_END signed or unsigned


// WBS_MISC
#define WBS_ROI_ACT_FRAC 		0 ///< @brief number of fractional bits of WBS_ROI_ACT
#define WBS_ROI_ACT_INT 		1 ///< @brief number of integer bits of WBS_ROI_ACT
#define WBS_ROI_ACT_SIGNED 		0 ///< @brief is WBS_ROI_ACT signed or unsigned

#define WBS_RGB_OFFSET_FRAC 		4 ///< @brief number of fractional bits of WBS_RGB_OFFSET
#define WBS_RGB_OFFSET_INT 		9 ///< @brief number of integer bits of WBS_RGB_OFFSET
#define WBS_RGB_OFFSET_SIGNED 		1 ///< @brief is WBS_RGB_OFFSET signed or unsigned

#define WBS_Y_OFFSET_FRAC 		4 ///< @brief number of fractional bits of WBS_Y_OFFSET
#define WBS_Y_OFFSET_INT 		9 ///< @brief number of integer bits of WBS_Y_OFFSET
#define WBS_Y_OFFSET_SIGNED 		1 ///< @brief is WBS_Y_OFFSET signed or unsigned


// WBS_RGBY_TH_0
#define WBS_RMAX_TH_FRAC 		0 ///< @brief number of fractional bits of WBS_RMAX_TH
#define WBS_RMAX_TH_INT 		12 ///< @brief number of integer bits of WBS_RMAX_TH
#define WBS_RMAX_TH_SIGNED 		0 ///< @brief is WBS_RMAX_TH signed or unsigned

#define WBS_GMAX_TH_FRAC 		0 ///< @brief number of fractional bits of WBS_GMAX_TH
#define WBS_GMAX_TH_INT 		12 ///< @brief number of integer bits of WBS_GMAX_TH
#define WBS_GMAX_TH_SIGNED 		0 ///< @brief is WBS_GMAX_TH signed or unsigned


// WBS_RGBY_TH_1
#define WBS_BMAX_TH_FRAC 		0 ///< @brief number of fractional bits of WBS_BMAX_TH
#define WBS_BMAX_TH_INT 		12 ///< @brief number of integer bits of WBS_BMAX_TH
#define WBS_BMAX_TH_SIGNED 		0 ///< @brief is WBS_BMAX_TH signed or unsigned

#define WBS_YHLW_TH_FRAC 		0 ///< @brief number of fractional bits of WBS_YHLW_TH
#define WBS_YHLW_TH_INT 		12 ///< @brief number of integer bits of WBS_YHLW_TH
#define WBS_YHLW_TH_SIGNED 		0 ///< @brief is WBS_YHLW_TH signed or unsigned


// RGB_TO_YCC_MATRIX_COEFFS
#define RGB_TO_YCC_COEFF_COL_0_FRAC 		10 ///< @brief number of fractional bits of RGB_TO_YCC_COEFF_COL_0
#define RGB_TO_YCC_COEFF_COL_0_INT 		2 ///< @brief number of integer bits of RGB_TO_YCC_COEFF_COL_0
#define RGB_TO_YCC_COEFF_COL_0_SIGNED 		1 ///< @brief is RGB_TO_YCC_COEFF_COL_0 signed or unsigned

#define RGB_TO_YCC_COEFF_COL_1_FRAC 		10 ///< @brief number of fractional bits of RGB_TO_YCC_COEFF_COL_1
#define RGB_TO_YCC_COEFF_COL_1_INT 		2 ///< @brief number of integer bits of RGB_TO_YCC_COEFF_COL_1
#define RGB_TO_YCC_COEFF_COL_1_SIGNED 		1 ///< @brief is RGB_TO_YCC_COEFF_COL_1 signed or unsigned


// RGB_TO_YCC_MATRIX_COEFF_OFFSET
#define RGB_TO_YCC_COEFF_COL_2_FRAC 		10 ///< @brief number of fractional bits of RGB_TO_YCC_COEFF_COL_2
#define RGB_TO_YCC_COEFF_COL_2_INT 		2 ///< @brief number of integer bits of RGB_TO_YCC_COEFF_COL_2
#define RGB_TO_YCC_COEFF_COL_2_SIGNED 		1 ///< @brief is RGB_TO_YCC_COEFF_COL_2 signed or unsigned

#define RGB_TO_YCC_OFFSET_FRAC 		2 ///< @brief number of fractional bits of RGB_TO_YCC_OFFSET
#define RGB_TO_YCC_OFFSET_INT 		8 ///< @brief number of integer bits of RGB_TO_YCC_OFFSET
#define RGB_TO_YCC_OFFSET_SIGNED 		1 ///< @brief is RGB_TO_YCC_OFFSET signed or unsigned


// HIS_INPUT_RANGE
#define HIS_INPUT_OFFSET_FRAC 		2 ///< @brief number of fractional bits of HIS_INPUT_OFFSET
#define HIS_INPUT_OFFSET_INT 		8 ///< @brief number of integer bits of HIS_INPUT_OFFSET
#define HIS_INPUT_OFFSET_SIGNED 		0 ///< @brief is HIS_INPUT_OFFSET signed or unsigned

#define HIS_INPUT_SCALE_FRAC 		14 ///< @brief number of fractional bits of HIS_INPUT_SCALE
#define HIS_INPUT_SCALE_INT 		2 ///< @brief number of integer bits of HIS_INPUT_SCALE
#define HIS_INPUT_SCALE_SIGNED 		0 ///< @brief is HIS_INPUT_SCALE signed or unsigned


// HIS_REGION_GRID_START_COORDS
#define HIS_REGION_GRID_COL_START_FRAC 		0 ///< @brief number of fractional bits of HIS_REGION_GRID_COL_START
#define HIS_REGION_GRID_COL_START_INT 		14 ///< @brief number of integer bits of HIS_REGION_GRID_COL_START
#define HIS_REGION_GRID_COL_START_SIGNED 		0 ///< @brief is HIS_REGION_GRID_COL_START signed or unsigned

#define HIS_REGION_GRID_ROW_START_FRAC 		0 ///< @brief number of fractional bits of HIS_REGION_GRID_ROW_START
#define HIS_REGION_GRID_ROW_START_INT 		14 ///< @brief number of integer bits of HIS_REGION_GRID_ROW_START
#define HIS_REGION_GRID_ROW_START_SIGNED 		0 ///< @brief is HIS_REGION_GRID_ROW_START signed or unsigned


// HIS_REGION_GRID_TILE
#define HIS_REGION_GRID_TILE_WIDTH_FRAC 		0 ///< @brief number of fractional bits of HIS_REGION_GRID_TILE_WIDTH
#define HIS_REGION_GRID_TILE_WIDTH_INT 		14 ///< @brief number of integer bits of HIS_REGION_GRID_TILE_WIDTH
#define HIS_REGION_GRID_TILE_WIDTH_SIGNED 		0 ///< @brief is HIS_REGION_GRID_TILE_WIDTH signed or unsigned

#define HIS_REGION_GRID_TILE_HEIGHT_FRAC 		0 ///< @brief number of fractional bits of HIS_REGION_GRID_TILE_HEIGHT
#define HIS_REGION_GRID_TILE_HEIGHT_INT 		14 ///< @brief number of integer bits of HIS_REGION_GRID_TILE_HEIGHT
#define HIS_REGION_GRID_TILE_HEIGHT_SIGNED 		0 ///< @brief is HIS_REGION_GRID_TILE_HEIGHT signed or unsigned


// MIE_VIB_PARAMS_0
#define MIE_MC_ON_FRAC 		0 ///< @brief number of fractional bits of MIE_MC_ON
#define MIE_MC_ON_INT 		1 ///< @brief number of integer bits of MIE_MC_ON
#define MIE_MC_ON_SIGNED 		0 ///< @brief is MIE_MC_ON signed or unsigned

#define MIE_VIB_ON_FRAC 		0 ///< @brief number of fractional bits of MIE_VIB_ON
#define MIE_VIB_ON_INT 		1 ///< @brief number of integer bits of MIE_VIB_ON
#define MIE_VIB_ON_SIGNED 		0 ///< @brief is MIE_VIB_ON signed or unsigned

#define MIE_BLACK_LEVEL_FRAC 		2 ///< @brief number of fractional bits of MIE_BLACK_LEVEL
#define MIE_BLACK_LEVEL_INT 		8 ///< @brief number of integer bits of MIE_BLACK_LEVEL
#define MIE_BLACK_LEVEL_SIGNED 		0 ///< @brief is MIE_BLACK_LEVEL signed or unsigned


// MIE_VIB_FUN
#define MIE_VIB_SATMULT_0_FRAC 		8 ///< @brief number of fractional bits of MIE_VIB_SATMULT_0
#define MIE_VIB_SATMULT_0_INT 		4 ///< @brief number of integer bits of MIE_VIB_SATMULT_0
#define MIE_VIB_SATMULT_0_SIGNED 		0 ///< @brief is MIE_VIB_SATMULT_0 signed or unsigned

#define MIE_VIB_SATMULT_1_FRAC 		8 ///< @brief number of fractional bits of MIE_VIB_SATMULT_1
#define MIE_VIB_SATMULT_1_INT 		4 ///< @brief number of integer bits of MIE_VIB_SATMULT_1
#define MIE_VIB_SATMULT_1_SIGNED 		0 ///< @brief is MIE_VIB_SATMULT_1 signed or unsigned


// MIE_MC_PARAMS_Y
#define MIE_MC_YOFF_FRAC 		2 ///< @brief number of fractional bits of MIE_MC_YOFF
#define MIE_MC_YOFF_INT 		12 ///< @brief number of integer bits of MIE_MC_YOFF
#define MIE_MC_YOFF_SIGNED 		1 ///< @brief is MIE_MC_YOFF signed or unsigned

#define MIE_MC_YSCALE_FRAC 		4 ///< @brief number of fractional bits of MIE_MC_YSCALE
#define MIE_MC_YSCALE_INT 		2 ///< @brief number of integer bits of MIE_MC_YSCALE
#define MIE_MC_YSCALE_SIGNED 		0 ///< @brief is MIE_MC_YSCALE signed or unsigned


// MIE_MC_PARAMS_C
#define MIE_MC_COFF_FRAC 		1 ///< @brief number of fractional bits of MIE_MC_COFF
#define MIE_MC_COFF_INT 		9 ///< @brief number of integer bits of MIE_MC_COFF
#define MIE_MC_COFF_SIGNED 		1 ///< @brief is MIE_MC_COFF signed or unsigned

#define MIE_MC_CSCALE_FRAC 		4 ///< @brief number of fractional bits of MIE_MC_CSCALE
#define MIE_MC_CSCALE_INT 		4 ///< @brief number of integer bits of MIE_MC_CSCALE
#define MIE_MC_CSCALE_SIGNED 		0 ///< @brief is MIE_MC_CSCALE signed or unsigned


// MIE_MC_ROT_PARAMS
#define MIE_MC_ROT00_FRAC 		7 ///< @brief number of fractional bits of MIE_MC_ROT00
#define MIE_MC_ROT00_INT 		1 ///< @brief number of integer bits of MIE_MC_ROT00
#define MIE_MC_ROT00_SIGNED 		1 ///< @brief is MIE_MC_ROT00 signed or unsigned

#define MIE_MC_ROT01_FRAC 		7 ///< @brief number of fractional bits of MIE_MC_ROT01
#define MIE_MC_ROT01_INT 		1 ///< @brief number of integer bits of MIE_MC_ROT01
#define MIE_MC_ROT01_SIGNED 		1 ///< @brief is MIE_MC_ROT01 signed or unsigned


// MIE_MC_GAUSS_PARAMS_0
#define MIE_MC_GAUSS_MINY_FRAC 		2 ///< @brief number of fractional bits of MIE_MC_GAUSS_MINY
#define MIE_MC_GAUSS_MINY_INT 		8 ///< @brief number of integer bits of MIE_MC_GAUSS_MINY
#define MIE_MC_GAUSS_MINY_SIGNED 		0 ///< @brief is MIE_MC_GAUSS_MINY signed or unsigned

#define MIE_MC_GAUSS_YSCALE_FRAC 		4 ///< @brief number of fractional bits of MIE_MC_GAUSS_YSCALE
#define MIE_MC_GAUSS_YSCALE_INT 		4 ///< @brief number of integer bits of MIE_MC_GAUSS_YSCALE
#define MIE_MC_GAUSS_YSCALE_SIGNED 		0 ///< @brief is MIE_MC_GAUSS_YSCALE signed or unsigned


// MIE_MC_GAUSS_PARAMS_1
#define MIE_MC_GAUSS_CB0_FRAC 		2 ///< @brief number of fractional bits of MIE_MC_GAUSS_CB0
#define MIE_MC_GAUSS_CB0_INT 		8 ///< @brief number of integer bits of MIE_MC_GAUSS_CB0
#define MIE_MC_GAUSS_CB0_SIGNED 		1 ///< @brief is MIE_MC_GAUSS_CB0 signed or unsigned

#define MIE_MC_GAUSS_CR0_FRAC 		2 ///< @brief number of fractional bits of MIE_MC_GAUSS_CR0
#define MIE_MC_GAUSS_CR0_INT 		8 ///< @brief number of integer bits of MIE_MC_GAUSS_CR0
#define MIE_MC_GAUSS_CR0_SIGNED 		1 ///< @brief is MIE_MC_GAUSS_CR0 signed or unsigned


// MIE_MC_GAUSS_PARAMS_2
#define MIE_MC_GAUSS_R00_FRAC 		7 ///< @brief number of fractional bits of MIE_MC_GAUSS_R00
#define MIE_MC_GAUSS_R00_INT 		1 ///< @brief number of integer bits of MIE_MC_GAUSS_R00
#define MIE_MC_GAUSS_R00_SIGNED 		1 ///< @brief is MIE_MC_GAUSS_R00 signed or unsigned

#define MIE_MC_GAUSS_R01_FRAC 		7 ///< @brief number of fractional bits of MIE_MC_GAUSS_R01
#define MIE_MC_GAUSS_R01_INT 		1 ///< @brief number of integer bits of MIE_MC_GAUSS_R01
#define MIE_MC_GAUSS_R01_SIGNED 		1 ///< @brief is MIE_MC_GAUSS_R01 signed or unsigned


// MIE_MC_GAUSS_PARAMS_3
#define MIE_MC_GAUSS_KB_FRAC 		0 ///< @brief number of fractional bits of MIE_MC_GAUSS_KB
#define MIE_MC_GAUSS_KB_INT 		8 ///< @brief number of integer bits of MIE_MC_GAUSS_KB
#define MIE_MC_GAUSS_KB_SIGNED 		0 ///< @brief is MIE_MC_GAUSS_KB signed or unsigned

#define MIE_MC_GAUSS_KR_FRAC 		0 ///< @brief number of fractional bits of MIE_MC_GAUSS_KR
#define MIE_MC_GAUSS_KR_INT 		8 ///< @brief number of integer bits of MIE_MC_GAUSS_KR
#define MIE_MC_GAUSS_KR_SIGNED 		0 ///< @brief is MIE_MC_GAUSS_KR signed or unsigned


// MIE_MC_GAUSS_PARAMS_4
#define MIE_MC_GAUSS_S0_FRAC 		8 ///< @brief number of fractional bits of MIE_MC_GAUSS_S0
#define MIE_MC_GAUSS_S0_INT 		0 ///< @brief number of integer bits of MIE_MC_GAUSS_S0
#define MIE_MC_GAUSS_S0_SIGNED 		0 ///< @brief is MIE_MC_GAUSS_S0 signed or unsigned

#define MIE_MC_GAUSS_S1_FRAC 		8 ///< @brief number of fractional bits of MIE_MC_GAUSS_S1
#define MIE_MC_GAUSS_S1_INT 		0 ///< @brief number of integer bits of MIE_MC_GAUSS_S1
#define MIE_MC_GAUSS_S1_SIGNED 		0 ///< @brief is MIE_MC_GAUSS_S1 signed or unsigned


// MIE_MC_GAUSS_PARAMS_5
#define MIE_MC_GAUSS_G0_FRAC 		8 ///< @brief number of fractional bits of MIE_MC_GAUSS_G0
#define MIE_MC_GAUSS_G0_INT 		0 ///< @brief number of integer bits of MIE_MC_GAUSS_G0
#define MIE_MC_GAUSS_G0_SIGNED 		0 ///< @brief is MIE_MC_GAUSS_G0 signed or unsigned

#define MIE_MC_GAUSS_G1_FRAC 		8 ///< @brief number of fractional bits of MIE_MC_GAUSS_G1
#define MIE_MC_GAUSS_G1_INT 		0 ///< @brief number of integer bits of MIE_MC_GAUSS_G1
#define MIE_MC_GAUSS_G1_SIGNED 		0 ///< @brief is MIE_MC_GAUSS_G1 signed or unsigned


// TNM_GLOBAL_CURVE
#define TNM_CURVE_POINT_0_FRAC 		11 ///< @brief number of fractional bits of TNM_CURVE_POINT_0
#define TNM_CURVE_POINT_0_INT 		1 ///< @brief number of integer bits of TNM_CURVE_POINT_0
#define TNM_CURVE_POINT_0_SIGNED 		0 ///< @brief is TNM_CURVE_POINT_0 signed or unsigned

#define TNM_CURVE_POINT_1_FRAC 		11 ///< @brief number of fractional bits of TNM_CURVE_POINT_1
#define TNM_CURVE_POINT_1_INT 		1 ///< @brief number of integer bits of TNM_CURVE_POINT_1
#define TNM_CURVE_POINT_1_SIGNED 		0 ///< @brief is TNM_CURVE_POINT_1 signed or unsigned


// TNM_BYPASS
#define TNM_BYPASS_FRAC 		0 ///< @brief number of fractional bits of TNM_BYPASS
#define TNM_BYPASS_INT 		1 ///< @brief number of integer bits of TNM_BYPASS
#define TNM_BYPASS_SIGNED 		0 ///< @brief is TNM_BYPASS signed or unsigned


// TNM_HIST_FLATTEN_0
#define TNM_HIST_FLATTEN_MIN_FRAC 		11 ///< @brief number of fractional bits of TNM_HIST_FLATTEN_MIN
#define TNM_HIST_FLATTEN_MIN_INT 		0 ///< @brief number of integer bits of TNM_HIST_FLATTEN_MIN
#define TNM_HIST_FLATTEN_MIN_SIGNED 		0 ///< @brief is TNM_HIST_FLATTEN_MIN signed or unsigned

#define TNM_HIST_FLATTEN_THRES_FRAC 		11 ///< @brief number of fractional bits of TNM_HIST_FLATTEN_THRES
#define TNM_HIST_FLATTEN_THRES_INT 		0 ///< @brief number of integer bits of TNM_HIST_FLATTEN_THRES
#define TNM_HIST_FLATTEN_THRES_SIGNED 		0 ///< @brief is TNM_HIST_FLATTEN_THRES signed or unsigned


// TNM_HIST_FLATTEN_1
#define TNM_HIST_FLATTEN_RECIP_FRAC 		12 ///< @brief number of fractional bits of TNM_HIST_FLATTEN_RECIP
#define TNM_HIST_FLATTEN_RECIP_INT 		4 ///< @brief number of integer bits of TNM_HIST_FLATTEN_RECIP
#define TNM_HIST_FLATTEN_RECIP_SIGNED 		0 ///< @brief is TNM_HIST_FLATTEN_RECIP signed or unsigned


// TNM_HIST_NORM
#define TNM_HIST_NORM_FACTOR_LAST_FRAC 		16 ///< @brief number of fractional bits of TNM_HIST_NORM_FACTOR_LAST
#define TNM_HIST_NORM_FACTOR_LAST_INT 		0 ///< @brief number of integer bits of TNM_HIST_NORM_FACTOR_LAST
#define TNM_HIST_NORM_FACTOR_LAST_SIGNED 		0 ///< @brief is TNM_HIST_NORM_FACTOR_LAST signed or unsigned

#define TNM_HIST_NORM_FACTOR_FRAC 		16 ///< @brief number of fractional bits of TNM_HIST_NORM_FACTOR
#define TNM_HIST_NORM_FACTOR_INT 		0 ///< @brief number of integer bits of TNM_HIST_NORM_FACTOR
#define TNM_HIST_NORM_FACTOR_SIGNED 		0 ///< @brief is TNM_HIST_NORM_FACTOR signed or unsigned


// TNM_CHR_0
#define TNM_CHR_SAT_SCALE_FRAC 		8 ///< @brief number of fractional bits of TNM_CHR_SAT_SCALE
#define TNM_CHR_SAT_SCALE_INT 		4 ///< @brief number of integer bits of TNM_CHR_SAT_SCALE
#define TNM_CHR_SAT_SCALE_SIGNED 		0 ///< @brief is TNM_CHR_SAT_SCALE signed or unsigned

#define TNM_CHR_CONF_SCALE_FRAC 		10 ///< @brief number of fractional bits of TNM_CHR_CONF_SCALE
#define TNM_CHR_CONF_SCALE_INT 		6 ///< @brief number of integer bits of TNM_CHR_CONF_SCALE
#define TNM_CHR_CONF_SCALE_SIGNED 		0 ///< @brief is TNM_CHR_CONF_SCALE signed or unsigned


// TNM_CHR_1
#define TNM_CHR_IO_SCALE_FRAC 		14 ///< @brief number of fractional bits of TNM_CHR_IO_SCALE
#define TNM_CHR_IO_SCALE_INT 		2 ///< @brief number of integer bits of TNM_CHR_IO_SCALE
#define TNM_CHR_IO_SCALE_SIGNED 		0 ///< @brief is TNM_CHR_IO_SCALE signed or unsigned


// TNM_LOCAL_COL
#define TNM_LOCAL_COL_WIDTH_FRAC 		0 ///< @brief number of fractional bits of TNM_LOCAL_COL_WIDTH
#define TNM_LOCAL_COL_WIDTH_INT 		14 ///< @brief number of integer bits of TNM_LOCAL_COL_WIDTH
#define TNM_LOCAL_COL_WIDTH_SIGNED 		0 ///< @brief is TNM_LOCAL_COL_WIDTH signed or unsigned

#define TNM_LOCAL_COL_WIDTH_RECIP_FRAC 		16 ///< @brief number of fractional bits of TNM_LOCAL_COL_WIDTH_RECIP
#define TNM_LOCAL_COL_WIDTH_RECIP_INT 		0 ///< @brief number of integer bits of TNM_LOCAL_COL_WIDTH_RECIP
#define TNM_LOCAL_COL_WIDTH_RECIP_SIGNED 		0 ///< @brief is TNM_LOCAL_COL_WIDTH_RECIP signed or unsigned


// TNM_CTX_LOCAL_COL
#define TNM_CTX_LOCAL_COL_IDX_FRAC 		0 ///< @brief number of fractional bits of TNM_CTX_LOCAL_COL_IDX
#define TNM_CTX_LOCAL_COL_IDX_INT 		7 ///< @brief number of integer bits of TNM_CTX_LOCAL_COL_IDX
#define TNM_CTX_LOCAL_COL_IDX_SIGNED 		0 ///< @brief is TNM_CTX_LOCAL_COL_IDX signed or unsigned

#define TNM_CTX_LOCAL_COLS_FRAC 		0 ///< @brief number of fractional bits of TNM_CTX_LOCAL_COLS
#define TNM_CTX_LOCAL_COLS_INT 		5 ///< @brief number of integer bits of TNM_CTX_LOCAL_COLS
#define TNM_CTX_LOCAL_COLS_SIGNED 		0 ///< @brief is TNM_CTX_LOCAL_COLS signed or unsigned


// TNM_WEIGHTS
#define TNM_LOCAL_WEIGHT_FRAC 		10 ///< @brief number of fractional bits of TNM_LOCAL_WEIGHT
#define TNM_LOCAL_WEIGHT_INT 		0 ///< @brief number of integer bits of TNM_LOCAL_WEIGHT
#define TNM_LOCAL_WEIGHT_SIGNED 		0 ///< @brief is TNM_LOCAL_WEIGHT signed or unsigned

#define TNM_CURVE_UPDATE_WEIGHT_FRAC 		16 ///< @brief number of fractional bits of TNM_CURVE_UPDATE_WEIGHT
#define TNM_CURVE_UPDATE_WEIGHT_INT 		0 ///< @brief number of integer bits of TNM_CURVE_UPDATE_WEIGHT
#define TNM_CURVE_UPDATE_WEIGHT_SIGNED 		0 ///< @brief is TNM_CURVE_UPDATE_WEIGHT signed or unsigned


// TNM_INPUT_LUMA
#define TNM_INPUT_LUMA_OFFSET_FRAC 		4 ///< @brief number of fractional bits of TNM_INPUT_LUMA_OFFSET
#define TNM_INPUT_LUMA_OFFSET_INT 		8 ///< @brief number of integer bits of TNM_INPUT_LUMA_OFFSET
#define TNM_INPUT_LUMA_OFFSET_SIGNED 		0 ///< @brief is TNM_INPUT_LUMA_OFFSET signed or unsigned

#define TNM_INPUT_LUMA_SCALE_FRAC 		12 ///< @brief number of fractional bits of TNM_INPUT_LUMA_SCALE
#define TNM_INPUT_LUMA_SCALE_INT 		4 ///< @brief number of integer bits of TNM_INPUT_LUMA_SCALE
#define TNM_INPUT_LUMA_SCALE_SIGNED 		0 ///< @brief is TNM_INPUT_LUMA_SCALE signed or unsigned


// TNM_OUTPUT_LUMA
#define TNM_OUTPUT_LUMA_OFFSET_FRAC 		4 ///< @brief number of fractional bits of TNM_OUTPUT_LUMA_OFFSET
#define TNM_OUTPUT_LUMA_OFFSET_INT 		8 ///< @brief number of integer bits of TNM_OUTPUT_LUMA_OFFSET
#define TNM_OUTPUT_LUMA_OFFSET_SIGNED 		0 ///< @brief is TNM_OUTPUT_LUMA_OFFSET signed or unsigned

#define TNM_OUTPUT_LUMA_SCALE_FRAC 		12 ///< @brief number of fractional bits of TNM_OUTPUT_LUMA_SCALE
#define TNM_OUTPUT_LUMA_SCALE_INT 		4 ///< @brief number of integer bits of TNM_OUTPUT_LUMA_SCALE
#define TNM_OUTPUT_LUMA_SCALE_SIGNED 		0 ///< @brief is TNM_OUTPUT_LUMA_SCALE signed or unsigned


// FLD_FRAC_STEP
#define FLD_FRAC_STEP_50_FRAC 		14 ///< @brief number of fractional bits of FLD_FRAC_STEP_50
#define FLD_FRAC_STEP_50_INT 		1 ///< @brief number of integer bits of FLD_FRAC_STEP_50
#define FLD_FRAC_STEP_50_SIGNED 		0 ///< @brief is FLD_FRAC_STEP_50 signed or unsigned

#define FLD_FRAC_STEP_60_FRAC 		14 ///< @brief number of fractional bits of FLD_FRAC_STEP_60
#define FLD_FRAC_STEP_60_INT 		1 ///< @brief number of integer bits of FLD_FRAC_STEP_60
#define FLD_FRAC_STEP_60_SIGNED 		0 ///< @brief is FLD_FRAC_STEP_60 signed or unsigned


// FLD_TH
#define FLD_COEF_DIFF_TH_FRAC 		0 ///< @brief number of fractional bits of FLD_COEF_DIFF_TH
#define FLD_COEF_DIFF_TH_INT 		15 ///< @brief number of integer bits of FLD_COEF_DIFF_TH
#define FLD_COEF_DIFF_TH_SIGNED 		0 ///< @brief is FLD_COEF_DIFF_TH signed or unsigned

#define FLD_NF_TH_FRAC 		0 ///< @brief number of fractional bits of FLD_NF_TH
#define FLD_NF_TH_INT 		15 ///< @brief number of integer bits of FLD_NF_TH
#define FLD_NF_TH_SIGNED 		0 ///< @brief is FLD_NF_TH signed or unsigned


// FLD_SCENE_CHANGE_TH
#define FLD_SCENE_CHANGE_TH_FRAC 		0 ///< @brief number of fractional bits of FLD_SCENE_CHANGE_TH
#define FLD_SCENE_CHANGE_TH_INT 		20 ///< @brief number of integer bits of FLD_SCENE_CHANGE_TH
#define FLD_SCENE_CHANGE_TH_SIGNED 		0 ///< @brief is FLD_SCENE_CHANGE_TH signed or unsigned


// FLD_DFT
#define FLD_RSHIFT_FRAC 		0 ///< @brief number of fractional bits of FLD_RSHIFT
#define FLD_RSHIFT_INT 		6 ///< @brief number of integer bits of FLD_RSHIFT
#define FLD_RSHIFT_SIGNED 		0 ///< @brief is FLD_RSHIFT signed or unsigned

#define FLD_MIN_PN_FRAC 		0 ///< @brief number of fractional bits of FLD_MIN_PN
#define FLD_MIN_PN_INT 		6 ///< @brief number of integer bits of FLD_MIN_PN
#define FLD_MIN_PN_SIGNED 		0 ///< @brief is FLD_MIN_PN signed or unsigned

#define FLD_PN_FRAC 		0 ///< @brief number of fractional bits of FLD_PN
#define FLD_PN_INT 		6 ///< @brief number of integer bits of FLD_PN
#define FLD_PN_SIGNED 		0 ///< @brief is FLD_PN signed or unsigned


// FLD_RESET
#define FLD_RESET_FRAC 		0 ///< @brief number of fractional bits of FLD_RESET
#define FLD_RESET_INT 		1 ///< @brief number of integer bits of FLD_RESET
#define FLD_RESET_SIGNED 		0 ///< @brief is FLD_RESET signed or unsigned


// SHA_PARAMS_0
#define SHA_THRESH_FRAC 		0 ///< @brief number of fractional bits of SHA_THRESH
#define SHA_THRESH_INT 		8 ///< @brief number of integer bits of SHA_THRESH
#define SHA_THRESH_SIGNED 		0 ///< @brief is SHA_THRESH signed or unsigned

#define SHA_STRENGTH_FRAC 		4 ///< @brief number of fractional bits of SHA_STRENGTH
#define SHA_STRENGTH_INT 		4 ///< @brief number of integer bits of SHA_STRENGTH
#define SHA_STRENGTH_SIGNED 		0 ///< @brief is SHA_STRENGTH signed or unsigned

#define SHA_DETAIL_FRAC 		6 ///< @brief number of fractional bits of SHA_DETAIL
#define SHA_DETAIL_INT 		0 ///< @brief number of integer bits of SHA_DETAIL
#define SHA_DETAIL_SIGNED 		0 ///< @brief is SHA_DETAIL signed or unsigned

#define SHA_DENOISE_BYPASS_FRAC 		0 ///< @brief number of fractional bits of SHA_DENOISE_BYPASS
#define SHA_DENOISE_BYPASS_INT 		1 ///< @brief number of integer bits of SHA_DENOISE_BYPASS
#define SHA_DENOISE_BYPASS_SIGNED 		0 ///< @brief is SHA_DENOISE_BYPASS signed or unsigned


// SHA_PARAMS_1
#define SHA_GWEIGHT_0_FRAC 		6 ///< @brief number of fractional bits of SHA_GWEIGHT_0
#define SHA_GWEIGHT_0_INT 		0 ///< @brief number of integer bits of SHA_GWEIGHT_0
#define SHA_GWEIGHT_0_SIGNED 		0 ///< @brief is SHA_GWEIGHT_0 signed or unsigned

#define SHA_GWEIGHT_1_FRAC 		6 ///< @brief number of fractional bits of SHA_GWEIGHT_1
#define SHA_GWEIGHT_1_INT 		0 ///< @brief number of integer bits of SHA_GWEIGHT_1
#define SHA_GWEIGHT_1_SIGNED 		0 ///< @brief is SHA_GWEIGHT_1 signed or unsigned

#define SHA_GWEIGHT_2_FRAC 		6 ///< @brief number of fractional bits of SHA_GWEIGHT_2
#define SHA_GWEIGHT_2_INT 		0 ///< @brief number of integer bits of SHA_GWEIGHT_2
#define SHA_GWEIGHT_2_SIGNED 		0 ///< @brief is SHA_GWEIGHT_2 signed or unsigned


// SHA_ELW_SCALEOFF
#define SHA_ELW_SCALE_FRAC 		4 ///< @brief number of fractional bits of SHA_ELW_SCALE
#define SHA_ELW_SCALE_INT 		4 ///< @brief number of integer bits of SHA_ELW_SCALE
#define SHA_ELW_SCALE_SIGNED 		0 ///< @brief is SHA_ELW_SCALE signed or unsigned

#define SHA_ELW_OFFS_FRAC 		0 ///< @brief number of fractional bits of SHA_ELW_OFFS
#define SHA_ELW_OFFS_INT 		7 ///< @brief number of integer bits of SHA_ELW_OFFS
#define SHA_ELW_OFFS_SIGNED 		1 ///< @brief is SHA_ELW_OFFS signed or unsigned


// SHA_DN_EDGE_SIMIL_COMP_PTS
#define SHA_DN_EDGE_SIMIL_COMP_PTS_FRAC 		0 ///< @brief number of fractional bits of SHA_DN_EDGE_SIMIL_COMP_PTS
#define SHA_DN_EDGE_SIMIL_COMP_PTS_INT 		8 ///< @brief number of integer bits of SHA_DN_EDGE_SIMIL_COMP_PTS
#define SHA_DN_EDGE_SIMIL_COMP_PTS_SIGNED 		0 ///< @brief is SHA_DN_EDGE_SIMIL_COMP_PTS signed or unsigned


// SHA_DN_EDGE_AVOID_COMP_PTS
#define SHA_DN_EDGE_AVOID_COMP_PTS_FRAC 		0 ///< @brief number of fractional bits of SHA_DN_EDGE_AVOID_COMP_PTS
#define SHA_DN_EDGE_AVOID_COMP_PTS_INT 		8 ///< @brief number of integer bits of SHA_DN_EDGE_AVOID_COMP_PTS
#define SHA_DN_EDGE_AVOID_COMP_PTS_SIGNED 		0 ///< @brief is SHA_DN_EDGE_AVOID_COMP_PTS signed or unsigned


// ENC_SCAL_V_PITCH
#define ENC_SCAL_V_PITCH_SUBPHASE_FRAC 		15 ///< @brief number of fractional bits of ENC_SCAL_V_PITCH_SUBPHASE
#define ENC_SCAL_V_PITCH_SUBPHASE_INT 		-3 ///< @brief number of integer bits of ENC_SCAL_V_PITCH_SUBPHASE
#define ENC_SCAL_V_PITCH_SUBPHASE_SIGNED 		0 ///< @brief is ENC_SCAL_V_PITCH_SUBPHASE signed or unsigned

#define ENC_SCAL_V_PITCH_PHASE_FRAC 		3 ///< @brief number of fractional bits of ENC_SCAL_V_PITCH_PHASE
#define ENC_SCAL_V_PITCH_PHASE_INT 		0 ///< @brief number of integer bits of ENC_SCAL_V_PITCH_PHASE
#define ENC_SCAL_V_PITCH_PHASE_SIGNED 		0 ///< @brief is ENC_SCAL_V_PITCH_PHASE signed or unsigned

#define ENC_SCAL_V_PITCH_INTEGER_FRAC 		0 ///< @brief number of fractional bits of ENC_SCAL_V_PITCH_INTEGER
#define ENC_SCAL_V_PITCH_INTEGER_INT 		6 ///< @brief number of integer bits of ENC_SCAL_V_PITCH_INTEGER
#define ENC_SCAL_V_PITCH_INTEGER_SIGNED 		0 ///< @brief is ENC_SCAL_V_PITCH_INTEGER signed or unsigned


// ENC_SCAL_V_SETUP
#define ENC_SCAL_422_NOT_420_FRAC 		0 ///< @brief number of fractional bits of ENC_SCAL_422_NOT_420
#define ENC_SCAL_422_NOT_420_INT 		1 ///< @brief number of integer bits of ENC_SCAL_422_NOT_420
#define ENC_SCAL_422_NOT_420_SIGNED 		0 ///< @brief is ENC_SCAL_422_NOT_420 signed or unsigned

#define ENC_SCAL_V_OFFSET_FRAC 		0 ///< @brief number of fractional bits of ENC_SCAL_V_OFFSET
#define ENC_SCAL_V_OFFSET_INT 		15 ///< @brief number of integer bits of ENC_SCAL_V_OFFSET
#define ENC_SCAL_V_OFFSET_SIGNED 		0 ///< @brief is ENC_SCAL_V_OFFSET signed or unsigned

#define ENC_SCAL_OUTPUT_ROWS_FRAC 		0 ///< @brief number of fractional bits of ENC_SCAL_OUTPUT_ROWS
#define ENC_SCAL_OUTPUT_ROWS_INT 		15 ///< @brief number of integer bits of ENC_SCAL_OUTPUT_ROWS
#define ENC_SCAL_OUTPUT_ROWS_SIGNED 		0 ///< @brief is ENC_SCAL_OUTPUT_ROWS signed or unsigned


// DISP_SCAL_V_PITCH
#define DISP_SCAL_V_PITCH_SUBPHASE_FRAC 		15 ///< @brief number of fractional bits of DISP_SCAL_V_PITCH_SUBPHASE
#define DISP_SCAL_V_PITCH_SUBPHASE_INT 		-3 ///< @brief number of integer bits of DISP_SCAL_V_PITCH_SUBPHASE
#define DISP_SCAL_V_PITCH_SUBPHASE_SIGNED 		0 ///< @brief is DISP_SCAL_V_PITCH_SUBPHASE signed or unsigned

#define DISP_SCAL_V_PITCH_PHASE_FRAC 		3 ///< @brief number of fractional bits of DISP_SCAL_V_PITCH_PHASE
#define DISP_SCAL_V_PITCH_PHASE_INT 		0 ///< @brief number of integer bits of DISP_SCAL_V_PITCH_PHASE
#define DISP_SCAL_V_PITCH_PHASE_SIGNED 		0 ///< @brief is DISP_SCAL_V_PITCH_PHASE signed or unsigned

#define DISP_SCAL_V_PITCH_INTEGER_FRAC 		0 ///< @brief number of fractional bits of DISP_SCAL_V_PITCH_INTEGER
#define DISP_SCAL_V_PITCH_INTEGER_INT 		6 ///< @brief number of integer bits of DISP_SCAL_V_PITCH_INTEGER
#define DISP_SCAL_V_PITCH_INTEGER_SIGNED 		0 ///< @brief is DISP_SCAL_V_PITCH_INTEGER signed or unsigned


// DISP_SCAL_V_SETUP
#define DISP_SCAL_V_OFFSET_FRAC 		0 ///< @brief number of fractional bits of DISP_SCAL_V_OFFSET
#define DISP_SCAL_V_OFFSET_INT 		15 ///< @brief number of integer bits of DISP_SCAL_V_OFFSET
#define DISP_SCAL_V_OFFSET_SIGNED 		0 ///< @brief is DISP_SCAL_V_OFFSET signed or unsigned

#define DISP_SCAL_OUTPUT_ROWS_FRAC 		0 ///< @brief number of fractional bits of DISP_SCAL_OUTPUT_ROWS
#define DISP_SCAL_OUTPUT_ROWS_INT 		15 ///< @brief number of integer bits of DISP_SCAL_OUTPUT_ROWS
#define DISP_SCAL_OUTPUT_ROWS_SIGNED 		0 ///< @brief is DISP_SCAL_OUTPUT_ROWS signed or unsigned


// ENC_SCAL_V_LUMA_TAPS_0_TO_3
#define ENC_SCAL_V_LUMA_TAP_0_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_V_LUMA_TAP_0
#define ENC_SCAL_V_LUMA_TAP_0_INT 		-1 ///< @brief number of integer bits of ENC_SCAL_V_LUMA_TAP_0
#define ENC_SCAL_V_LUMA_TAP_0_SIGNED 		1 ///< @brief is ENC_SCAL_V_LUMA_TAP_0 signed or unsigned

#define ENC_SCAL_V_LUMA_TAP_1_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_V_LUMA_TAP_1
#define ENC_SCAL_V_LUMA_TAP_1_INT 		-1 ///< @brief number of integer bits of ENC_SCAL_V_LUMA_TAP_1
#define ENC_SCAL_V_LUMA_TAP_1_SIGNED 		1 ///< @brief is ENC_SCAL_V_LUMA_TAP_1 signed or unsigned

#define ENC_SCAL_V_LUMA_TAP_2_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_V_LUMA_TAP_2
#define ENC_SCAL_V_LUMA_TAP_2_INT 		0 ///< @brief number of integer bits of ENC_SCAL_V_LUMA_TAP_2
#define ENC_SCAL_V_LUMA_TAP_2_SIGNED 		1 ///< @brief is ENC_SCAL_V_LUMA_TAP_2 signed or unsigned

#define ENC_SCAL_V_LUMA_TAP_3_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_V_LUMA_TAP_3
#define ENC_SCAL_V_LUMA_TAP_3_INT 		1 ///< @brief number of integer bits of ENC_SCAL_V_LUMA_TAP_3
#define ENC_SCAL_V_LUMA_TAP_3_SIGNED 		0 ///< @brief is ENC_SCAL_V_LUMA_TAP_3 signed or unsigned


// ENC_SCAL_V_LUMA_TAPS_4_TO_7
#define ENC_SCAL_V_LUMA_TAP_4_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_V_LUMA_TAP_4
#define ENC_SCAL_V_LUMA_TAP_4_INT 		1 ///< @brief number of integer bits of ENC_SCAL_V_LUMA_TAP_4
#define ENC_SCAL_V_LUMA_TAP_4_SIGNED 		0 ///< @brief is ENC_SCAL_V_LUMA_TAP_4 signed or unsigned

#define ENC_SCAL_V_LUMA_TAP_5_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_V_LUMA_TAP_5
#define ENC_SCAL_V_LUMA_TAP_5_INT 		0 ///< @brief number of integer bits of ENC_SCAL_V_LUMA_TAP_5
#define ENC_SCAL_V_LUMA_TAP_5_SIGNED 		1 ///< @brief is ENC_SCAL_V_LUMA_TAP_5 signed or unsigned

#define ENC_SCAL_V_LUMA_TAP_6_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_V_LUMA_TAP_6
#define ENC_SCAL_V_LUMA_TAP_6_INT 		-1 ///< @brief number of integer bits of ENC_SCAL_V_LUMA_TAP_6
#define ENC_SCAL_V_LUMA_TAP_6_SIGNED 		1 ///< @brief is ENC_SCAL_V_LUMA_TAP_6 signed or unsigned

#define ENC_SCAL_V_LUMA_TAP_7_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_V_LUMA_TAP_7
#define ENC_SCAL_V_LUMA_TAP_7_INT 		-1 ///< @brief number of integer bits of ENC_SCAL_V_LUMA_TAP_7
#define ENC_SCAL_V_LUMA_TAP_7_SIGNED 		1 ///< @brief is ENC_SCAL_V_LUMA_TAP_7 signed or unsigned


// ENC_SCAL_V_CHROMA_TAPS_0_TO_3
#define ENC_SCAL_V_CHROMA_TAP_0_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_V_CHROMA_TAP_0
#define ENC_SCAL_V_CHROMA_TAP_0_INT 		0 ///< @brief number of integer bits of ENC_SCAL_V_CHROMA_TAP_0
#define ENC_SCAL_V_CHROMA_TAP_0_SIGNED 		1 ///< @brief is ENC_SCAL_V_CHROMA_TAP_0 signed or unsigned

#define ENC_SCAL_V_CHROMA_TAP_1_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_V_CHROMA_TAP_1
#define ENC_SCAL_V_CHROMA_TAP_1_INT 		1 ///< @brief number of integer bits of ENC_SCAL_V_CHROMA_TAP_1
#define ENC_SCAL_V_CHROMA_TAP_1_SIGNED 		0 ///< @brief is ENC_SCAL_V_CHROMA_TAP_1 signed or unsigned

#define ENC_SCAL_V_CHROMA_TAP_2_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_V_CHROMA_TAP_2
#define ENC_SCAL_V_CHROMA_TAP_2_INT 		1 ///< @brief number of integer bits of ENC_SCAL_V_CHROMA_TAP_2
#define ENC_SCAL_V_CHROMA_TAP_2_SIGNED 		0 ///< @brief is ENC_SCAL_V_CHROMA_TAP_2 signed or unsigned

#define ENC_SCAL_V_CHROMA_TAP_3_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_V_CHROMA_TAP_3
#define ENC_SCAL_V_CHROMA_TAP_3_INT 		0 ///< @brief number of integer bits of ENC_SCAL_V_CHROMA_TAP_3
#define ENC_SCAL_V_CHROMA_TAP_3_SIGNED 		1 ///< @brief is ENC_SCAL_V_CHROMA_TAP_3 signed or unsigned


// ENC_SCAL_H_LUMA_TAPS_0_TO_3
#define ENC_SCAL_H_LUMA_TAP_0_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_LUMA_TAP_0
#define ENC_SCAL_H_LUMA_TAP_0_INT 		-2 ///< @brief number of integer bits of ENC_SCAL_H_LUMA_TAP_0
#define ENC_SCAL_H_LUMA_TAP_0_SIGNED 		1 ///< @brief is ENC_SCAL_H_LUMA_TAP_0 signed or unsigned

#define ENC_SCAL_H_LUMA_TAP_1_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_LUMA_TAP_1
#define ENC_SCAL_H_LUMA_TAP_1_INT 		-2 ///< @brief number of integer bits of ENC_SCAL_H_LUMA_TAP_1
#define ENC_SCAL_H_LUMA_TAP_1_SIGNED 		1 ///< @brief is ENC_SCAL_H_LUMA_TAP_1 signed or unsigned

#define ENC_SCAL_H_LUMA_TAP_2_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_LUMA_TAP_2
#define ENC_SCAL_H_LUMA_TAP_2_INT 		-2 ///< @brief number of integer bits of ENC_SCAL_H_LUMA_TAP_2
#define ENC_SCAL_H_LUMA_TAP_2_SIGNED 		1 ///< @brief is ENC_SCAL_H_LUMA_TAP_2 signed or unsigned

#define ENC_SCAL_H_LUMA_TAP_3_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_LUMA_TAP_3
#define ENC_SCAL_H_LUMA_TAP_3_INT 		-2 ///< @brief number of integer bits of ENC_SCAL_H_LUMA_TAP_3
#define ENC_SCAL_H_LUMA_TAP_3_SIGNED 		1 ///< @brief is ENC_SCAL_H_LUMA_TAP_3 signed or unsigned


// ENC_SCAL_H_LUMA_TAPS_4_TO_7
#define ENC_SCAL_H_LUMA_TAP_4_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_LUMA_TAP_4
#define ENC_SCAL_H_LUMA_TAP_4_INT 		-1 ///< @brief number of integer bits of ENC_SCAL_H_LUMA_TAP_4
#define ENC_SCAL_H_LUMA_TAP_4_SIGNED 		1 ///< @brief is ENC_SCAL_H_LUMA_TAP_4 signed or unsigned

#define ENC_SCAL_H_LUMA_TAP_5_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_LUMA_TAP_5
#define ENC_SCAL_H_LUMA_TAP_5_INT 		-1 ///< @brief number of integer bits of ENC_SCAL_H_LUMA_TAP_5
#define ENC_SCAL_H_LUMA_TAP_5_SIGNED 		1 ///< @brief is ENC_SCAL_H_LUMA_TAP_5 signed or unsigned

#define ENC_SCAL_H_LUMA_TAP_6_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_LUMA_TAP_6
#define ENC_SCAL_H_LUMA_TAP_6_INT 		0 ///< @brief number of integer bits of ENC_SCAL_H_LUMA_TAP_6
#define ENC_SCAL_H_LUMA_TAP_6_SIGNED 		1 ///< @brief is ENC_SCAL_H_LUMA_TAP_6 signed or unsigned

#define ENC_SCAL_H_LUMA_TAP_7_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_LUMA_TAP_7
#define ENC_SCAL_H_LUMA_TAP_7_INT 		1 ///< @brief number of integer bits of ENC_SCAL_H_LUMA_TAP_7
#define ENC_SCAL_H_LUMA_TAP_7_SIGNED 		0 ///< @brief is ENC_SCAL_H_LUMA_TAP_7 signed or unsigned


// ENC_SCAL_H_LUMA_TAPS_8_TO_11
#define ENC_SCAL_H_LUMA_TAP_8_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_LUMA_TAP_8
#define ENC_SCAL_H_LUMA_TAP_8_INT 		1 ///< @brief number of integer bits of ENC_SCAL_H_LUMA_TAP_8
#define ENC_SCAL_H_LUMA_TAP_8_SIGNED 		0 ///< @brief is ENC_SCAL_H_LUMA_TAP_8 signed or unsigned

#define ENC_SCAL_H_LUMA_TAP_9_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_LUMA_TAP_9
#define ENC_SCAL_H_LUMA_TAP_9_INT 		0 ///< @brief number of integer bits of ENC_SCAL_H_LUMA_TAP_9
#define ENC_SCAL_H_LUMA_TAP_9_SIGNED 		1 ///< @brief is ENC_SCAL_H_LUMA_TAP_9 signed or unsigned

#define ENC_SCAL_H_LUMA_TAP_10_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_LUMA_TAP_10
#define ENC_SCAL_H_LUMA_TAP_10_INT 		-1 ///< @brief number of integer bits of ENC_SCAL_H_LUMA_TAP_10
#define ENC_SCAL_H_LUMA_TAP_10_SIGNED 		1 ///< @brief is ENC_SCAL_H_LUMA_TAP_10 signed or unsigned

#define ENC_SCAL_H_LUMA_TAP_11_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_LUMA_TAP_11
#define ENC_SCAL_H_LUMA_TAP_11_INT 		-1 ///< @brief number of integer bits of ENC_SCAL_H_LUMA_TAP_11
#define ENC_SCAL_H_LUMA_TAP_11_SIGNED 		1 ///< @brief is ENC_SCAL_H_LUMA_TAP_11 signed or unsigned


// ENC_SCAL_H_LUMA_TAPS_12_TO_15
#define ENC_SCAL_H_LUMA_TAP_12_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_LUMA_TAP_12
#define ENC_SCAL_H_LUMA_TAP_12_INT 		-2 ///< @brief number of integer bits of ENC_SCAL_H_LUMA_TAP_12
#define ENC_SCAL_H_LUMA_TAP_12_SIGNED 		1 ///< @brief is ENC_SCAL_H_LUMA_TAP_12 signed or unsigned

#define ENC_SCAL_H_LUMA_TAP_13_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_LUMA_TAP_13
#define ENC_SCAL_H_LUMA_TAP_13_INT 		-2 ///< @brief number of integer bits of ENC_SCAL_H_LUMA_TAP_13
#define ENC_SCAL_H_LUMA_TAP_13_SIGNED 		1 ///< @brief is ENC_SCAL_H_LUMA_TAP_13 signed or unsigned

#define ENC_SCAL_H_LUMA_TAP_14_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_LUMA_TAP_14
#define ENC_SCAL_H_LUMA_TAP_14_INT 		-2 ///< @brief number of integer bits of ENC_SCAL_H_LUMA_TAP_14
#define ENC_SCAL_H_LUMA_TAP_14_SIGNED 		1 ///< @brief is ENC_SCAL_H_LUMA_TAP_14 signed or unsigned

#define ENC_SCAL_H_LUMA_TAP_15_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_LUMA_TAP_15
#define ENC_SCAL_H_LUMA_TAP_15_INT 		-2 ///< @brief number of integer bits of ENC_SCAL_H_LUMA_TAP_15
#define ENC_SCAL_H_LUMA_TAP_15_SIGNED 		1 ///< @brief is ENC_SCAL_H_LUMA_TAP_15 signed or unsigned


// ENC_SCAL_H_CHROMA_TAPS_0_TO_3
#define ENC_SCAL_H_CHROMA_TAP_0_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_CHROMA_TAP_0
#define ENC_SCAL_H_CHROMA_TAP_0_INT 		-1 ///< @brief number of integer bits of ENC_SCAL_H_CHROMA_TAP_0
#define ENC_SCAL_H_CHROMA_TAP_0_SIGNED 		1 ///< @brief is ENC_SCAL_H_CHROMA_TAP_0 signed or unsigned

#define ENC_SCAL_H_CHROMA_TAP_1_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_CHROMA_TAP_1
#define ENC_SCAL_H_CHROMA_TAP_1_INT 		-1 ///< @brief number of integer bits of ENC_SCAL_H_CHROMA_TAP_1
#define ENC_SCAL_H_CHROMA_TAP_1_SIGNED 		1 ///< @brief is ENC_SCAL_H_CHROMA_TAP_1 signed or unsigned

#define ENC_SCAL_H_CHROMA_TAP_2_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_CHROMA_TAP_2
#define ENC_SCAL_H_CHROMA_TAP_2_INT 		0 ///< @brief number of integer bits of ENC_SCAL_H_CHROMA_TAP_2
#define ENC_SCAL_H_CHROMA_TAP_2_SIGNED 		1 ///< @brief is ENC_SCAL_H_CHROMA_TAP_2 signed or unsigned

#define ENC_SCAL_H_CHROMA_TAP_3_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_CHROMA_TAP_3
#define ENC_SCAL_H_CHROMA_TAP_3_INT 		1 ///< @brief number of integer bits of ENC_SCAL_H_CHROMA_TAP_3
#define ENC_SCAL_H_CHROMA_TAP_3_SIGNED 		0 ///< @brief is ENC_SCAL_H_CHROMA_TAP_3 signed or unsigned


// ENC_SCAL_H_CHROMA_TAPS_4_TO_7
#define ENC_SCAL_H_CHROMA_TAP_4_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_CHROMA_TAP_4
#define ENC_SCAL_H_CHROMA_TAP_4_INT 		1 ///< @brief number of integer bits of ENC_SCAL_H_CHROMA_TAP_4
#define ENC_SCAL_H_CHROMA_TAP_4_SIGNED 		0 ///< @brief is ENC_SCAL_H_CHROMA_TAP_4 signed or unsigned

#define ENC_SCAL_H_CHROMA_TAP_5_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_CHROMA_TAP_5
#define ENC_SCAL_H_CHROMA_TAP_5_INT 		0 ///< @brief number of integer bits of ENC_SCAL_H_CHROMA_TAP_5
#define ENC_SCAL_H_CHROMA_TAP_5_SIGNED 		1 ///< @brief is ENC_SCAL_H_CHROMA_TAP_5 signed or unsigned

#define ENC_SCAL_H_CHROMA_TAP_6_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_CHROMA_TAP_6
#define ENC_SCAL_H_CHROMA_TAP_6_INT 		-1 ///< @brief number of integer bits of ENC_SCAL_H_CHROMA_TAP_6
#define ENC_SCAL_H_CHROMA_TAP_6_SIGNED 		1 ///< @brief is ENC_SCAL_H_CHROMA_TAP_6 signed or unsigned

#define ENC_SCAL_H_CHROMA_TAP_7_FRAC 		6 ///< @brief number of fractional bits of ENC_SCAL_H_CHROMA_TAP_7
#define ENC_SCAL_H_CHROMA_TAP_7_INT 		-1 ///< @brief number of integer bits of ENC_SCAL_H_CHROMA_TAP_7
#define ENC_SCAL_H_CHROMA_TAP_7_SIGNED 		1 ///< @brief is ENC_SCAL_H_CHROMA_TAP_7 signed or unsigned


// ENC_SCAL_H_PITCH
#define ENC_SCAL_H_PITCH_SUBPHASE_FRAC 		15 ///< @brief number of fractional bits of ENC_SCAL_H_PITCH_SUBPHASE
#define ENC_SCAL_H_PITCH_SUBPHASE_INT 		-3 ///< @brief number of integer bits of ENC_SCAL_H_PITCH_SUBPHASE
#define ENC_SCAL_H_PITCH_SUBPHASE_SIGNED 		0 ///< @brief is ENC_SCAL_H_PITCH_SUBPHASE signed or unsigned

#define ENC_SCAL_H_PITCH_PHASE_FRAC 		3 ///< @brief number of fractional bits of ENC_SCAL_H_PITCH_PHASE
#define ENC_SCAL_H_PITCH_PHASE_INT 		0 ///< @brief number of integer bits of ENC_SCAL_H_PITCH_PHASE
#define ENC_SCAL_H_PITCH_PHASE_SIGNED 		0 ///< @brief is ENC_SCAL_H_PITCH_PHASE signed or unsigned

#define ENC_SCAL_H_PITCH_INTEGER_FRAC 		0 ///< @brief number of fractional bits of ENC_SCAL_H_PITCH_INTEGER
#define ENC_SCAL_H_PITCH_INTEGER_INT 		6 ///< @brief number of integer bits of ENC_SCAL_H_PITCH_INTEGER
#define ENC_SCAL_H_PITCH_INTEGER_SIGNED 		0 ///< @brief is ENC_SCAL_H_PITCH_INTEGER signed or unsigned


// ENC_SCAL_H_SETUP
#define ENC_SCAL_BYPASS_FRAC 		0 ///< @brief number of fractional bits of ENC_SCAL_BYPASS
#define ENC_SCAL_BYPASS_INT 		1 ///< @brief number of integer bits of ENC_SCAL_BYPASS
#define ENC_SCAL_BYPASS_SIGNED 		0 ///< @brief is ENC_SCAL_BYPASS signed or unsigned

#define ENC_SCAL_CHROMA_MODE_FRAC 		0 ///< @brief number of fractional bits of ENC_SCAL_CHROMA_MODE
#define ENC_SCAL_CHROMA_MODE_INT 		1 ///< @brief number of integer bits of ENC_SCAL_CHROMA_MODE
#define ENC_SCAL_CHROMA_MODE_SIGNED 		0 ///< @brief is ENC_SCAL_CHROMA_MODE signed or unsigned

#define ENC_SCAL_H_OFFSET_FRAC 		0 ///< @brief number of fractional bits of ENC_SCAL_H_OFFSET
#define ENC_SCAL_H_OFFSET_INT 		14 ///< @brief number of integer bits of ENC_SCAL_H_OFFSET
#define ENC_SCAL_H_OFFSET_SIGNED 		0 ///< @brief is ENC_SCAL_H_OFFSET signed or unsigned

#define ENC_SCAL_OUTPUT_COLUMNS_FRAC 		0 ///< @brief number of fractional bits of ENC_SCAL_OUTPUT_COLUMNS
#define ENC_SCAL_OUTPUT_COLUMNS_INT 		13 ///< @brief number of integer bits of ENC_SCAL_OUTPUT_COLUMNS
#define ENC_SCAL_OUTPUT_COLUMNS_SIGNED 		0 ///< @brief is ENC_SCAL_OUTPUT_COLUMNS signed or unsigned


// ENC_422_TO_420_CTRL
#define ENC_422_TO_420_ENABLE_FRAC 		0 ///< @brief number of fractional bits of ENC_422_TO_420_ENABLE
#define ENC_422_TO_420_ENABLE_INT 		1 ///< @brief number of integer bits of ENC_422_TO_420_ENABLE
#define ENC_422_TO_420_ENABLE_SIGNED 		0 ///< @brief is ENC_422_TO_420_ENABLE signed or unsigned


// ENS_NCOUPLES
#define ENS_NCOUPLES_EXP_FRAC 		0 ///< @brief number of fractional bits of ENS_NCOUPLES_EXP
#define ENS_NCOUPLES_EXP_INT 		3 ///< @brief number of integer bits of ENS_NCOUPLES_EXP
#define ENS_NCOUPLES_EXP_SIGNED 		0 ///< @brief is ENS_NCOUPLES_EXP signed or unsigned


// ENS_REGION_REGS
#define ENS_SUBS_EXP_FRAC 		0 ///< @brief number of fractional bits of ENS_SUBS_EXP
#define ENS_SUBS_EXP_INT 		3 ///< @brief number of integer bits of ENS_SUBS_EXP
#define ENS_SUBS_EXP_SIGNED 		0 ///< @brief is ENS_SUBS_EXP signed or unsigned


// DISP_SCAL_V_LUMA_TAPS_0_TO_3
#define DISP_SCAL_V_LUMA_TAP_0_FRAC 		6 ///< @brief number of fractional bits of DISP_SCAL_V_LUMA_TAP_0
#define DISP_SCAL_V_LUMA_TAP_0_INT 		0 ///< @brief number of integer bits of DISP_SCAL_V_LUMA_TAP_0
#define DISP_SCAL_V_LUMA_TAP_0_SIGNED 		1 ///< @brief is DISP_SCAL_V_LUMA_TAP_0 signed or unsigned

#define DISP_SCAL_V_LUMA_TAP_1_FRAC 		6 ///< @brief number of fractional bits of DISP_SCAL_V_LUMA_TAP_1
#define DISP_SCAL_V_LUMA_TAP_1_INT 		1 ///< @brief number of integer bits of DISP_SCAL_V_LUMA_TAP_1
#define DISP_SCAL_V_LUMA_TAP_1_SIGNED 		0 ///< @brief is DISP_SCAL_V_LUMA_TAP_1 signed or unsigned

#define DISP_SCAL_V_LUMA_TAP_2_FRAC 		6 ///< @brief number of fractional bits of DISP_SCAL_V_LUMA_TAP_2
#define DISP_SCAL_V_LUMA_TAP_2_INT 		1 ///< @brief number of integer bits of DISP_SCAL_V_LUMA_TAP_2
#define DISP_SCAL_V_LUMA_TAP_2_SIGNED 		0 ///< @brief is DISP_SCAL_V_LUMA_TAP_2 signed or unsigned

#define DISP_SCAL_V_LUMA_TAP_3_FRAC 		6 ///< @brief number of fractional bits of DISP_SCAL_V_LUMA_TAP_3
#define DISP_SCAL_V_LUMA_TAP_3_INT 		0 ///< @brief number of integer bits of DISP_SCAL_V_LUMA_TAP_3
#define DISP_SCAL_V_LUMA_TAP_3_SIGNED 		1 ///< @brief is DISP_SCAL_V_LUMA_TAP_3 signed or unsigned


// DISP_SCAL_V_CHROMA_TAPS_0_TO_1
#define DISP_SCAL_V_CHROMA_TAP_0_FRAC 		6 ///< @brief number of fractional bits of DISP_SCAL_V_CHROMA_TAP_0
#define DISP_SCAL_V_CHROMA_TAP_0_INT 		1 ///< @brief number of integer bits of DISP_SCAL_V_CHROMA_TAP_0
#define DISP_SCAL_V_CHROMA_TAP_0_SIGNED 		0 ///< @brief is DISP_SCAL_V_CHROMA_TAP_0 signed or unsigned

#define DISP_SCAL_V_CHROMA_TAP_1_FRAC 		6 ///< @brief number of fractional bits of DISP_SCAL_V_CHROMA_TAP_1
#define DISP_SCAL_V_CHROMA_TAP_1_INT 		1 ///< @brief number of integer bits of DISP_SCAL_V_CHROMA_TAP_1
#define DISP_SCAL_V_CHROMA_TAP_1_SIGNED 		0 ///< @brief is DISP_SCAL_V_CHROMA_TAP_1 signed or unsigned


// DISP_SCAL_H_LUMA_TAPS_0_TO_3
#define DISP_SCAL_H_LUMA_TAP_0_FRAC 		6 ///< @brief number of fractional bits of DISP_SCAL_H_LUMA_TAP_0
#define DISP_SCAL_H_LUMA_TAP_0_INT 		-1 ///< @brief number of integer bits of DISP_SCAL_H_LUMA_TAP_0
#define DISP_SCAL_H_LUMA_TAP_0_SIGNED 		1 ///< @brief is DISP_SCAL_H_LUMA_TAP_0 signed or unsigned

#define DISP_SCAL_H_LUMA_TAP_1_FRAC 		6 ///< @brief number of fractional bits of DISP_SCAL_H_LUMA_TAP_1
#define DISP_SCAL_H_LUMA_TAP_1_INT 		-1 ///< @brief number of integer bits of DISP_SCAL_H_LUMA_TAP_1
#define DISP_SCAL_H_LUMA_TAP_1_SIGNED 		1 ///< @brief is DISP_SCAL_H_LUMA_TAP_1 signed or unsigned

#define DISP_SCAL_H_LUMA_TAP_2_FRAC 		6 ///< @brief number of fractional bits of DISP_SCAL_H_LUMA_TAP_2
#define DISP_SCAL_H_LUMA_TAP_2_INT 		0 ///< @brief number of integer bits of DISP_SCAL_H_LUMA_TAP_2
#define DISP_SCAL_H_LUMA_TAP_2_SIGNED 		1 ///< @brief is DISP_SCAL_H_LUMA_TAP_2 signed or unsigned

#define DISP_SCAL_H_LUMA_TAP_3_FRAC 		6 ///< @brief number of fractional bits of DISP_SCAL_H_LUMA_TAP_3
#define DISP_SCAL_H_LUMA_TAP_3_INT 		1 ///< @brief number of integer bits of DISP_SCAL_H_LUMA_TAP_3
#define DISP_SCAL_H_LUMA_TAP_3_SIGNED 		0 ///< @brief is DISP_SCAL_H_LUMA_TAP_3 signed or unsigned


// DISP_SCAL_H_LUMA_TAPS_4_TO_7
#define DISP_SCAL_H_LUMA_TAP_4_FRAC 		6 ///< @brief number of fractional bits of DISP_SCAL_H_LUMA_TAP_4
#define DISP_SCAL_H_LUMA_TAP_4_INT 		1 ///< @brief number of integer bits of DISP_SCAL_H_LUMA_TAP_4
#define DISP_SCAL_H_LUMA_TAP_4_SIGNED 		0 ///< @brief is DISP_SCAL_H_LUMA_TAP_4 signed or unsigned

#define DISP_SCAL_H_LUMA_TAP_5_FRAC 		6 ///< @brief number of fractional bits of DISP_SCAL_H_LUMA_TAP_5
#define DISP_SCAL_H_LUMA_TAP_5_INT 		0 ///< @brief number of integer bits of DISP_SCAL_H_LUMA_TAP_5
#define DISP_SCAL_H_LUMA_TAP_5_SIGNED 		1 ///< @brief is DISP_SCAL_H_LUMA_TAP_5 signed or unsigned

#define DISP_SCAL_H_LUMA_TAP_6_FRAC 		6 ///< @brief number of fractional bits of DISP_SCAL_H_LUMA_TAP_6
#define DISP_SCAL_H_LUMA_TAP_6_INT 		-1 ///< @brief number of integer bits of DISP_SCAL_H_LUMA_TAP_6
#define DISP_SCAL_H_LUMA_TAP_6_SIGNED 		1 ///< @brief is DISP_SCAL_H_LUMA_TAP_6 signed or unsigned

#define DISP_SCAL_H_LUMA_TAP_7_FRAC 		6 ///< @brief number of fractional bits of DISP_SCAL_H_LUMA_TAP_7
#define DISP_SCAL_H_LUMA_TAP_7_INT 		-1 ///< @brief number of integer bits of DISP_SCAL_H_LUMA_TAP_7
#define DISP_SCAL_H_LUMA_TAP_7_SIGNED 		1 ///< @brief is DISP_SCAL_H_LUMA_TAP_7 signed or unsigned


// DISP_SCAL_H_CHROMA_TAPS_0_TO_3
#define DISP_SCAL_H_CHROMA_TAP_0_FRAC 		6 ///< @brief number of fractional bits of DISP_SCAL_H_CHROMA_TAP_0
#define DISP_SCAL_H_CHROMA_TAP_0_INT 		0 ///< @brief number of integer bits of DISP_SCAL_H_CHROMA_TAP_0
#define DISP_SCAL_H_CHROMA_TAP_0_SIGNED 		1 ///< @brief is DISP_SCAL_H_CHROMA_TAP_0 signed or unsigned

#define DISP_SCAL_H_CHROMA_TAP_1_FRAC 		6 ///< @brief number of fractional bits of DISP_SCAL_H_CHROMA_TAP_1
#define DISP_SCAL_H_CHROMA_TAP_1_INT 		1 ///< @brief number of integer bits of DISP_SCAL_H_CHROMA_TAP_1
#define DISP_SCAL_H_CHROMA_TAP_1_SIGNED 		0 ///< @brief is DISP_SCAL_H_CHROMA_TAP_1 signed or unsigned

#define DISP_SCAL_H_CHROMA_TAP_2_FRAC 		6 ///< @brief number of fractional bits of DISP_SCAL_H_CHROMA_TAP_2
#define DISP_SCAL_H_CHROMA_TAP_2_INT 		1 ///< @brief number of integer bits of DISP_SCAL_H_CHROMA_TAP_2
#define DISP_SCAL_H_CHROMA_TAP_2_SIGNED 		0 ///< @brief is DISP_SCAL_H_CHROMA_TAP_2 signed or unsigned

#define DISP_SCAL_H_CHROMA_TAP_3_FRAC 		6 ///< @brief number of fractional bits of DISP_SCAL_H_CHROMA_TAP_3
#define DISP_SCAL_H_CHROMA_TAP_3_INT 		0 ///< @brief number of integer bits of DISP_SCAL_H_CHROMA_TAP_3
#define DISP_SCAL_H_CHROMA_TAP_3_SIGNED 		1 ///< @brief is DISP_SCAL_H_CHROMA_TAP_3 signed or unsigned


// DISP_SCAL_H_PITCH
#define DISP_SCAL_H_PITCH_SUBPHASE_FRAC 		15 ///< @brief number of fractional bits of DISP_SCAL_H_PITCH_SUBPHASE
#define DISP_SCAL_H_PITCH_SUBPHASE_INT 		-3 ///< @brief number of integer bits of DISP_SCAL_H_PITCH_SUBPHASE
#define DISP_SCAL_H_PITCH_SUBPHASE_SIGNED 		0 ///< @brief is DISP_SCAL_H_PITCH_SUBPHASE signed or unsigned

#define DISP_SCAL_H_PITCH_PHASE_FRAC 		3 ///< @brief number of fractional bits of DISP_SCAL_H_PITCH_PHASE
#define DISP_SCAL_H_PITCH_PHASE_INT 		0 ///< @brief number of integer bits of DISP_SCAL_H_PITCH_PHASE
#define DISP_SCAL_H_PITCH_PHASE_SIGNED 		0 ///< @brief is DISP_SCAL_H_PITCH_PHASE signed or unsigned

#define DISP_SCAL_H_PITCH_INTEGER_FRAC 		0 ///< @brief number of fractional bits of DISP_SCAL_H_PITCH_INTEGER
#define DISP_SCAL_H_PITCH_INTEGER_INT 		6 ///< @brief number of integer bits of DISP_SCAL_H_PITCH_INTEGER
#define DISP_SCAL_H_PITCH_INTEGER_SIGNED 		0 ///< @brief is DISP_SCAL_H_PITCH_INTEGER signed or unsigned


// DISP_SCAL_H_SETUP
#define DISP_SCAL_BYPASS_FRAC 		0 ///< @brief number of fractional bits of DISP_SCAL_BYPASS
#define DISP_SCAL_BYPASS_INT 		1 ///< @brief number of integer bits of DISP_SCAL_BYPASS
#define DISP_SCAL_BYPASS_SIGNED 		0 ///< @brief is DISP_SCAL_BYPASS signed or unsigned

#define DISP_SCAL_H_OFFSET_FRAC 		0 ///< @brief number of fractional bits of DISP_SCAL_H_OFFSET
#define DISP_SCAL_H_OFFSET_INT 		14 ///< @brief number of integer bits of DISP_SCAL_H_OFFSET
#define DISP_SCAL_H_OFFSET_SIGNED 		0 ///< @brief is DISP_SCAL_H_OFFSET signed or unsigned

#define DISP_SCAL_OUTPUT_COLUMNS_FRAC 		0 ///< @brief number of fractional bits of DISP_SCAL_OUTPUT_COLUMNS
#define DISP_SCAL_OUTPUT_COLUMNS_INT 		13 ///< @brief number of integer bits of DISP_SCAL_OUTPUT_COLUMNS
#define DISP_SCAL_OUTPUT_COLUMNS_SIGNED 		0 ///< @brief is DISP_SCAL_OUTPUT_COLUMNS signed or unsigned


// YCC_TO_RGB_MATRIX_COEFFS
#define YCC_TO_RGB_COEFF_COL_0_FRAC 		9 ///< @brief number of fractional bits of YCC_TO_RGB_COEFF_COL_0
#define YCC_TO_RGB_COEFF_COL_0_INT 		3 ///< @brief number of integer bits of YCC_TO_RGB_COEFF_COL_0
#define YCC_TO_RGB_COEFF_COL_0_SIGNED 		1 ///< @brief is YCC_TO_RGB_COEFF_COL_0 signed or unsigned

#define YCC_TO_RGB_COEFF_COL_1_FRAC 		9 ///< @brief number of fractional bits of YCC_TO_RGB_COEFF_COL_1
#define YCC_TO_RGB_COEFF_COL_1_INT 		3 ///< @brief number of integer bits of YCC_TO_RGB_COEFF_COL_1
#define YCC_TO_RGB_COEFF_COL_1_SIGNED 		1 ///< @brief is YCC_TO_RGB_COEFF_COL_1 signed or unsigned


// YCC_TO_RGB_MATRIX_COEFF_OFFSET
#define YCC_TO_RGB_COEFF_COL_2_FRAC 		9 ///< @brief number of fractional bits of YCC_TO_RGB_COEFF_COL_2
#define YCC_TO_RGB_COEFF_COL_2_INT 		3 ///< @brief number of integer bits of YCC_TO_RGB_COEFF_COL_2
#define YCC_TO_RGB_COEFF_COL_2_SIGNED 		1 ///< @brief is YCC_TO_RGB_COEFF_COL_2 signed or unsigned

#define YCC_TO_RGB_OFFSET_FRAC 		2 ///< @brief number of fractional bits of YCC_TO_RGB_OFFSET
#define YCC_TO_RGB_OFFSET_INT 		9 ///< @brief number of integer bits of YCC_TO_RGB_OFFSET
#define YCC_TO_RGB_OFFSET_SIGNED 		1 ///< @brief is YCC_TO_RGB_OFFSET signed or unsigned


// DGM_CLIP_IN
#define DGM_CLIP_MIN_FRAC 		2 ///< @brief number of fractional bits of DGM_CLIP_MIN
#define DGM_CLIP_MIN_INT 		10 ///< @brief number of integer bits of DGM_CLIP_MIN
#define DGM_CLIP_MIN_SIGNED 		1 ///< @brief is DGM_CLIP_MIN signed or unsigned

#define DGM_CLIP_MAX_FRAC 		2 ///< @brief number of fractional bits of DGM_CLIP_MAX
#define DGM_CLIP_MAX_INT 		10 ///< @brief number of integer bits of DGM_CLIP_MAX
#define DGM_CLIP_MAX_SIGNED 		1 ///< @brief is DGM_CLIP_MAX signed or unsigned


// DGM_CORE_IN_OUT
#define DGM_SRC_NORM_FRAC 		2 ///< @brief number of fractional bits of DGM_SRC_NORM
#define DGM_SRC_NORM_INT 		10 ///< @brief number of integer bits of DGM_SRC_NORM
#define DGM_SRC_NORM_SIGNED 		1 ///< @brief is DGM_SRC_NORM signed or unsigned


// DGM_SLOPE_0_1
#define DGM_SLOPE_0_FRAC 		10 ///< @brief number of fractional bits of DGM_SLOPE_0
#define DGM_SLOPE_0_INT 		2 ///< @brief number of integer bits of DGM_SLOPE_0
#define DGM_SLOPE_0_SIGNED 		0 ///< @brief is DGM_SLOPE_0 signed or unsigned

#define DGM_SLOPE_1_FRAC 		10 ///< @brief number of fractional bits of DGM_SLOPE_1
#define DGM_SLOPE_1_INT 		2 ///< @brief number of integer bits of DGM_SLOPE_1
#define DGM_SLOPE_1_SIGNED 		0 ///< @brief is DGM_SLOPE_1 signed or unsigned


// DGM_SLOPE_2_R
#define DGM_SLOPE_2_FRAC 		10 ///< @brief number of fractional bits of DGM_SLOPE_2
#define DGM_SLOPE_2_INT 		2 ///< @brief number of integer bits of DGM_SLOPE_2
#define DGM_SLOPE_2_SIGNED 		0 ///< @brief is DGM_SLOPE_2 signed or unsigned


// DGM_COEFFS_0_3
#define DGM_COEFF_0_FRAC 		6 ///< @brief number of fractional bits of DGM_COEFF_0
#define DGM_COEFF_0_INT 		2 ///< @brief number of integer bits of DGM_COEFF_0
#define DGM_COEFF_0_SIGNED 		1 ///< @brief is DGM_COEFF_0 signed or unsigned

#define DGM_COEFF_1_FRAC 		6 ///< @brief number of fractional bits of DGM_COEFF_1
#define DGM_COEFF_1_INT 		2 ///< @brief number of integer bits of DGM_COEFF_1
#define DGM_COEFF_1_SIGNED 		1 ///< @brief is DGM_COEFF_1 signed or unsigned

#define DGM_COEFF_2_FRAC 		6 ///< @brief number of fractional bits of DGM_COEFF_2
#define DGM_COEFF_2_INT 		2 ///< @brief number of integer bits of DGM_COEFF_2
#define DGM_COEFF_2_SIGNED 		1 ///< @brief is DGM_COEFF_2 signed or unsigned

#define DGM_COEFF_3_FRAC 		6 ///< @brief number of fractional bits of DGM_COEFF_3
#define DGM_COEFF_3_INT 		2 ///< @brief number of integer bits of DGM_COEFF_3
#define DGM_COEFF_3_SIGNED 		1 ///< @brief is DGM_COEFF_3 signed or unsigned


// DGM_COEFFS_4_5
#define DGM_COEFF_4_FRAC 		6 ///< @brief number of fractional bits of DGM_COEFF_4
#define DGM_COEFF_4_INT 		2 ///< @brief number of integer bits of DGM_COEFF_4
#define DGM_COEFF_4_SIGNED 		1 ///< @brief is DGM_COEFF_4 signed or unsigned

#define DGM_COEFF_5_FRAC 		6 ///< @brief number of fractional bits of DGM_COEFF_5
#define DGM_COEFF_5_INT 		2 ///< @brief number of integer bits of DGM_COEFF_5
#define DGM_COEFF_5_SIGNED 		1 ///< @brief is DGM_COEFF_5 signed or unsigned

