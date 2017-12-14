/*------------------------------------------------------------------------------
--                                                                                                                               --
--       This software is confidential and proprietary and may be used                                   --
--        only as expressly authorized by a licensing agreement from                                     --
--                                                                                                                               --
--                            Verisilicon.                                                                                    --
--                                                                                                                               --
--                   (C) COPYRIGHT 2014 VERISILICON                                                            --
--                            ALL RIGHTS RESERVED                                                                    --
--                                                                                                                               --
--                 The entire notice above must be reproduced                                                 --
--                  on all copies and should not be removed.                                                    --
--                                                                                                                               --
--------------------------------------------------------------------------------*/

#ifndef BASE_TYPE_H
#define BASE_TYPE_H

#include <stdint.h>
#include <stdio.h>
#ifndef NDEBUG
#include <assert.h>
#endif

typedef int8_t    i8;
typedef uint8_t   u8;
typedef int16_t   i16;
typedef uint16_t  u16;
typedef int32_t   i32;
typedef uint32_t  u32;
typedef int64_t   i64;
typedef uint64_t  u64;

#define INLINE inline

#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else /*  */
#define NULL    ((void *)0)
#endif /*  */
#endif

typedef       short               Short;
typedef       unsigned char       Pel;        ///< 8-bit pixel type
typedef       int                 Int;
typedef       unsigned int        UInt;


#ifndef __cplusplus
/*enum {
  false = 0,
  true  = 1
};
typedef _Bool           bool;*/
#if 0
typedef enum
{
  false = 0,
  true  = 1
} bool; 
#endif

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#ifndef bool
#define bool int
#endif

enum
{
  OK  = 0,
  NOK = -1
};
#endif

// Options
#define INLOOP_CHROMA_MODE_SELECTION

/* ASSERT */
#ifndef NDEBUG
#define ASSERT(x) assert(x)
#else
#define ASSERT(x)
#endif

#define ENABLE_DEBLOCK 1
/* TEST_DATA */
#ifndef TEST_DATA
#define COMMENT(b,...)
#define WRITE_CABAC_STATUS(type, bin_value,ctxid,mps_new,pstate_new,codlow_new,codrange_new,vc_len,vc,outstandingbit)
#define WRITE_SLICE_DATA(syntax_element_name,syntax_element_value)
#define WRITE_SLICE_DATA_BEFORE_BINARIZATION( syntax_element_name,syntax_element_value)
#define WRITE_SLICE_HEADER(syntax_element_name,syntax_element_value)
#define TRACE_SCANTOEMC_COMMON_CU_CTRL(cu,ctb,p)

#define WRITE_CTU_POSITION(CTU_X,CTU_Y)

#define GET_START_SUBBLOCKPOSTITION(poc)
#define GET_TU_CTRL_PARAMETER_QT(parameterId,pic,ctb,cu,tr,x, y,c_idx)
#define WRITE_RESIDUAL_DATA_QT(cu,tu,block,block_size,stride,luma)
#define WRITE_TRANSFORM_INPUT_1ST_FILE(block,block_size,stride)
#define WRITE_TU_POSITION(CU_X,CU_Y,TU_X,TU_Y,c_idx)
#define WRITE_TRANSFORM_1ST_FILE(block,block_size)
#define WRITE_TRANSFORM_INPUT_2ND_FILE(block,block_size)
#define WRITE_TRANSFORM_2ND_FILE(block,block_size, stride)
#define WRITE_SCALER_DATA_FILE(block,block_size)
#define WRITE_TU_POSITION_SCALE(CU_X,CU_Y,TU_X,TU_Y,c_idx)
#define WRITE_SCALED_RESULT_FILE(block,block_size, stride)
#define WRITE_TU_CONTROL_OUT_QT(shift,c_idx)
#define WRITE_SCALINGLIST_QT(sps)

#define CLEAN_SCAN_TU_CTRL()
#define WRITE_SCAN_TU_CTRL()
#define SCAN_WRITE_SCALED_RESULT_FILE( block,block_size, stride,c_idx)
//RQT

#define WRITE_RESID_FILE_RQT(tr, pred_lum,pred_stride,qp,lambda,log2_size)

#define WRITE_TRANSFORM_INPUT_NXN_1ST_FILE_RQT(block,block_size,stride)
#define WRITE_TRANSFORM_NXN_1ST_FILE_RQT(block,block_size)

#define WRITE_TRANSFORM_INPUT_NXN_2ND_FILE_RQT(block,block_size)
#define WRITE_TRANSFORM_NXN_2ND_FILE_RQT(block,block_size, stride)
#define RESET_CU_BLOCK_NUMBER_COUNT()

#define ENABLE_WRITE_DATA_RQT(enable)
#define WRITE_SCALED_RESULT_FILE_RQT(block,block_size, stride)

#define ENABLE_WRITE_DATA_QT(enable)

#define WRITE_CU_BLOCK_POSITION_RQT(cu,tr)


#define WRITE_ISCALED_RESULT_FILE_RQT(x,iscaled_value,block_size)

#define WRITE_SLICE_DATA_BEFORE_BINARIZATION_RQT(syntax_element_name,syntax_element_value,block_size)

#define WRITE_BIN_CNT_NXN_RQT(bitCount,block_size)

#define WRITE_ZERO_RESID_COST_FILE_RQT(zero_resid_cost,block_size)

#define WRITE_TU_TREE_OUT_FILE_RQT(root_tr,rdcost,log2_size)

#define WRITE_LUMA_DISTORTION_NXN_FILE_RQT(luma_distortion,block_size)

#define WRITE_TU_SPLIT_UNION_COST_OUT_FILE_RQT(type,cost,log2_size)

//to trace software control registers
#define TRACE_SOFTWARE_CONTROL_REGISTERS()

//to trace EMC modual
#define ASSIGN_SCBF_VALUE(sbf,c_idx,xs,ys )
#define RESET_SCBF_STRUCT()
#define TRACE_CBF_FLAG()

#define TRACE_SCANTOEMC_RESIDUAL_LEVEL(block,stride,x1,y1)

#define GET_TU_CTRL_PARAMETER(parameterId,tr,x, y,c_idx)
#define TRACE_SCANTOEMC_TU_CTRL()
#define TRACE_METOEMC_MVD(ref_idx,mvd_x,mvd_y,mvp_lx_flag)

#define TRACE_SAODECTOVPC_DATA(p)
#define TRACE_DOWNSAMPLETOVPC_DATA(p)
#define TRACE_DOWNSAMPLETOVPC_DATA_2CTU(p)



#define TRACE_TU_POSITION_ISCALE(CU_X, CU_Y, TU_X, TU_Y, c_idx)
#define TRACE_IQTTOINTERNAL_ISCALED_RESULT(block, block_size, stride)
#define TRACE_TU_POSITION_ITRANSFORM(CU_X, CU_Y, TU_X, TU_Y, c_idx)
#define TRACE_IQTTOINTERNAL_ITRANSFORM_RESULT(block, block_size, stride)
#define TRACE_EMCTOVPC_OUTPUT_STREAM(stream_data,length)
#define TRACE_PRP_CROPPING_FILLING_Y_PADDING(pp,expandedImage)

#define SCAN_WRITE_SCALED_RESULT_FILE_WITHOUT_FAKE(block,block_size,stride,c_idx)
#define TRACE_PRP_SCALE_INPUT_DATA(pp,  lum, cb, cr,stride,width,height)
#define TRACE_PRP_SCALE_VERTICAL_INTERMEDIATE_DATA( lum,cb, cr,row,width, height,scalewidth,scaleheight,save_continue , pixel_number,gotpixelnumber)
#define TRACE_PRP_SCALE_ROW_BUFFER_DATA( lum, cb, cr,endrow,width,height,scalewidth,scaleheight, row)
#define TRACE_PRP_SCALE_CROW(crow)
#define TRACE_PRP_SCALE_ROW_MEMORY_DATA( end,save_data,lum_cb_cr,lum, cb, cr,scalerow,width, height, scalewidth, scaleheight,inputrow)




//inter_mode.c
#define Trace_CU_Tree(c)
#define TraceMV(c)
#define Trace_CTU_Luma_Chroma(c)
#define Trace_ME_Search_Window(pcCU, piRefY, iRefStride, iSrchRngHorLeft_1N2N, iSrchRngHorRight_1N2N, iSrchRngVerTop_1N2N, iSrchRngVerBottom_1N2N, stage)

//deblock.c
#define trace_BsdOut(bS)

#else


#define COMMENT(b,...)  if ((b)->stream_trace) { \
        char buffer[128]; \
        snprintf(buffer, sizeof(buffer), ##__VA_ARGS__); \
        ASSERT(strlen((b)->stream_trace->comment) + strlen(buffer) < \
          sizeof((b)->stream_trace->comment)); \
        strcat((b)->stream_trace->comment, buffer); \
      }


#define WRITE_CABAC_STATUS(type, bin_value,ctxid,mps_new,pstate_new,codlow_new,codrange_new,vc_len,vc,outstandingbit) \
    write_cabac_status(type, bin_value,ctxid,mps_new,pstate_new,codlow_new,codrange_new,vc_len,vc,outstandingbit)
#define WRITE_SLICE_DATA(syntax_element_name,syntax_element_value) \
    write_slice_data(syntax_element_name,syntax_element_value)
#define WRITE_SLICE_DATA_BEFORE_BINARIZATION( syntax_element_name,syntax_element_value) \
    write_slice_data_before_binarization(syntax_element_name,syntax_element_value)
#define WRITE_SLICE_HEADER(syntax_element_name,syntax_element_value) \
    write_slice_header(syntax_element_name,syntax_element_value)
#define TRACE_SCANTOEMC_COMMON_CU_CTRL(cu,ctb,p) trace_ScanToEmc_Common_Cu_Ctrl(cu,ctb,p)
#define WRITE_CTU_POSITION(CTU_X,CTU_Y) write_CTU_position(CTU_X,CTU_Y)
#define GET_START_SUBBLOCKPOSTITION(poc) get_start_subblockpostition(poc)


#define CLEAN_SCAN_TU_CTRL() clean_scan_tu_ctrl()
#define WRITE_SCAN_TU_CTRL() write_scan_tu_ctrl()
#define SCAN_WRITE_SCALED_RESULT_FILE( block,block_size, stride,c_idx) \
    scan_write_scaled_result_file(block,block_size, stride,c_idx)


#define GET_TU_CTRL_PARAMETER_QT(parameterId,pic,ctb,cu,tr,x, y,c_idx) \
    get_tu_ctrl_parameter_QT(parameterId,pic,ctb,cu,tr,x, y,c_idx)
#define WRITE_RESIDUAL_DATA_QT(cu,tu,block,block_size,stride,luma) write_residual_data_QT(cu,tu,block,block_size,stride,luma)
#define WRITE_TRANSFORM_INPUT_1ST_FILE(block,block_size,stride) write_transform_input_1st_file(block,block_size,stride)
#define WRITE_TU_POSITION(CU_X,CU_Y,TU_X,TU_Y,c_idx) write_TU_position(CU_X,CU_Y,TU_X,TU_Y,c_idx)
#define WRITE_TRANSFORM_1ST_FILE(block,block_size) write_transform_1st_file(block,block_size)
#define WRITE_TRANSFORM_INPUT_2ND_FILE(block,block_size) write_transform_input_2nd_file( block,block_size);
#define WRITE_TRANSFORM_2ND_FILE(block,block_size, stride) write_transform_2nd_file(block,block_size,stride)
#define WRITE_SCALER_DATA_FILE(block,block_size) write_scaler_data_file(block,block_size)
#define WRITE_TU_POSITION_SCALE(CU_X,CU_Y,TU_X,TU_Y,c_idx) write_TU_position_scale(CU_X,CU_Y,TU_X,TU_Y,c_idx)
#define WRITE_SCALED_RESULT_FILE(block,block_size, stride) write_scaled_result_file(block,block_size, stride)
#define WRITE_TU_CONTROL_OUT_QT(shift,c_idx) \
    write_tu_control_out_QT(shift,c_idx)

#define ENABLE_WRITE_DATA_QT(enable) enable_write_data_QT(enable)


#define WRITE_SCALINGLIST_QT(sps) write_scalinglist_QT(sps)


#define WRITE_RESID_FILE_RQT(tr, pred_lum,pred_stride,qp,lambda,log2_size) write_resid_file_RQT(tr,pred_lum,pred_stride,qp,lambda,log2_size)

#define WRITE_TRANSFORM_INPUT_NXN_1ST_FILE_RQT(block,block_size,stride) write_transform_input_nxn_1st_file_RQT(block,block_size,stride)

#define WRITE_TRANSFORM_NXN_1ST_FILE_RQT(block,block_size) write_transform_nxn_1st_file_RQT( block,block_size)


#define WRITE_TRANSFORM_INPUT_NXN_2ND_FILE_RQT(block,block_size) write_transform_input_nxn_2nd_file_RQT(block,block_size)

#define WRITE_TRANSFORM_NXN_2ND_FILE_RQT(block,block_size, stride) write_transform_nxn_2nd_file_RQT(block,block_size,stride)

#define RESET_CU_BLOCK_NUMBER_COUNT() reset_cu_block_number_count()

#define WRITE_CU_BLOCK_POSITION_RQT(cu,tr) write_CU_block_position_RQT(cu,tr)


#define ENABLE_WRITE_DATA_RQT(enable) enable_write_data_RQT(enable)

#define WRITE_SCALED_RESULT_FILE_RQT(block,block_size, stride) write_scaled_result_file_RQT( block,block_size, stride)

#define WRITE_ISCALED_RESULT_FILE_RQT(x,iscaled_value,block_size) write_iscaled_result_file_RQT(x,iscaled_value,block_size)

#define WRITE_SLICE_DATA_BEFORE_BINARIZATION_RQT(syntax_element_name,syntax_element_value,block_size) write_slice_data_before_binarization_RQT(syntax_element_name,syntax_element_value,block_size)

#define WRITE_BIN_CNT_NXN_RQT(bitCount,block_size) write_bin_cnt_nxn_RQT(bitCount,block_size)

#define WRITE_ZERO_RESID_COST_FILE_RQT(zero_resid_cost,block_size) write_zero_resid_cost_file_RQT(zero_resid_cost,block_size)

#define WRITE_TU_TREE_OUT_FILE_RQT(root_tr,rdcost,log2_size) write_tu_tree_out_file_RQT(root_tr,rdcost,log2_size)
#define WRITE_LUMA_DISTORTION_NXN_FILE_RQT(luma_distortion,block_size) write_luma_distortion_nxn_file_RQT(luma_distortion,block_size)


#define WRITE_TU_SPLIT_UNION_COST_OUT_FILE_RQT(type,cost,log2_size) write_tu_split_union_cost_out_file_RQT(type,cost,log2_size)

//to trace software control registers

#define TRACE_SOFTWARE_CONTROL_REGISTERS() trace_Software_Control_Registers()

//to trace EMC modual
#define RESET_SCBF_STRUCT() reset_scbf_struct()
#define ASSIGN_SCBF_VALUE(sbf,c_idx,xs,ys ) assign_scbf_value(sbf,c_idx,xs,ys )
#define TRACE_CBF_FLAG() trace_CBF_Flag()
#define TRACE_SCANTOEMC_RESIDUAL_LEVEL(block,stride,x1,y1) trace_ScanToEmc_Residual_Level(block,stride,x1,y1)
#define GET_TU_CTRL_PARAMETER(parameterId,tr,x, y,c_idx) \
    get_tu_ctrl_parameter(parameterId,tr,x, y,c_idx)

#define TRACE_SCANTOEMC_TU_CTRL() trace_ScanToEmc_Tu_Ctrl()
#define TRACE_METOEMC_MVD(ref_idx,mvd_x,mvd_y,mvp_lx_flag) trace_MeToEmc_MVD(ref_idx,mvd_x,mvd_y,mvp_lx_flag)
#define TRACE_SAODECTOVPC_DATA(p) trace_SAODecToVPC_data(p)
#define TRACE_TU_POSITION_ISCALE(CU_X, CU_Y, TU_X, TU_Y, c_idx) trace_TU_position_iscale(CU_X, CU_Y, TU_X, TU_Y, c_idx)
#define TRACE_IQTTOINTERNAL_ISCALED_RESULT(block, block_size, stride) trace_IqtToInternal_iscaled_result(block, block_size, stride);
#define TRACE_IQTTOINTERNAL_ITRANSFORM_RESULT(block, block_size, stride) trace_IqtToInternal_iTransform_result(block, block_size, stride)
#define TRACE_TU_POSITION_ITRANSFORM(CU_X, CU_Y, TU_X, TU_Y, c_idx) trace_TU_position_iTransform(CU_X, CU_Y, TU_X, TU_Y, c_idx)
#define TRACE_DOWNSAMPLETOVPC_DATA_2CTU(p) trace_DownSampleToVPC_data_2ctu(p)
#define TRACE_DOWNSAMPLETOVPC_DATA(p) trace_DownSampleToVPC_data(p)

#define TRACE_EMCTOVPC_OUTPUT_STREAM(stream_data,length) trace_EmcToVPC_output_stream(stream_data,length)
#define TRACE_PRP_CROPPING_FILLING_Y_PADDING(pp,expandedImage) trace_Prp_Cropping_Filling_Y_padding(pp,expandedImage)
#define SCAN_WRITE_SCALED_RESULT_FILE_WITHOUT_FAKE(block,block_size,stride,c_idx) scan_write_scaled_result_file_without_fake(block,block_size,stride, c_idx)
#define TRACE_PRP_SCALE_INPUT_DATA(pp,  lum, cb, cr,stride,width,height) /*trace_Prp_Scale_Input_data(pp,lum,cb,cr,stride,width,height)*/
#define TRACE_PRP_SCALE_VERTICAL_INTERMEDIATE_DATA( lum,cb, cr,row,width, height,scalewidth,scaleheight,save_continue, pixel_number,gotpixelnumber) /*trace_Prp_Scale_Vertical_Intermediate_data(lum, cb, cr, row,width, height, scalewidth,scaleheight,save_continue, pixel_number,gotpixelnumber)*/
#define TRACE_PRP_SCALE_ROW_BUFFER_DATA( lum, cb, cr,endrow,width,height,scalewidth,scaleheight, row) /*trace_Prp_Scale_row_buffer_data( lum, cb, cr,endrow,width,height,scalewidth,scaleheight, row)*/
#define TRACE_PRP_SCALE_CROW(crow)  /*trace_Prp_Scale_cRow(crow)*/
#define TRACE_PRP_SCALE_ROW_MEMORY_DATA( end,save_data,lum_cb_cr,lum, cb, cr,scalerow,width, height, scalewidth, scaleheight,inputrow) /*trace_Prp_Scale_row_memory_data( end,save_data,lum_cb_cr,lum, cb, cr,scalerow,width,height,scalewidth,scaleheight,inputrow)*/

#endif

/* General tools */
#define ABS(x)    ((x) < (0) ? -(x) : (x))
#define MAX(a, b) ((a) > (b) ?  (a) : (b))
#define MIN(a, b) ((a) < (b) ?  (a) : (b))
#define SIGN(a)   ((a) < (0) ? (-1) : (1))
#define CLIP3(x, y, z)  ((z) < (x) ? (x) : ((z) > (y) ? (y) : (z)))

#endif
