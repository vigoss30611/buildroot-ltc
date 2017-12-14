#ifndef __CI_HW_AWB_H__
#define __CI_HW_AWB_H__


#if defined(__cplusplus)
extern "C" {
#endif //__cplusplus


#if defined(IMG_KERNEL_MODULE)
#define HW_AWB_LOG(...)        printk(KERN_DEBUG __VA_ARGS__)
#else
#define HW_AWB_LOG             printf
#endif //IMG_KERNEL_MODULE

#ifdef INFOTM_HW_AWB_METHOD


typedef int8_t                  AWB_INT8;
typedef int16_t                 AWB_INT16;
typedef int32_t                 AWB_INT32;
typedef int64_t                 AWB_INT64;

typedef uint8_t                 AWB_UINT8;
typedef uint16_t                AWB_UINT16;
typedef uint32_t                AWB_UINT32;
typedef uint64_t                AWB_UINT64;

typedef AWB_INT8 *           	AWB_PINT8;
typedef AWB_INT16 *          	AWB_PINT16;
typedef AWB_INT32 *          	AWB_PINT32;
typedef AWB_INT64 *          	AWB_PINT64;

typedef AWB_UINT8 *          	AWB_PUINT8;
typedef AWB_UINT16 *         	AWB_PUINT16;
typedef AWB_UINT32 *         	AWB_PUINT32;
typedef AWB_UINT64 *         	AWB_PUINT64;

typedef void                    AWB_VOID;
typedef void *                  AWB_PVOID;
typedef void *                  AWB_HANDLE;
typedef AWB_INT32            	AWB_RESULT;

typedef AWB_UINT8            	AWB_BOOL8;
typedef AWB_UINT8           	AWB_BYTE;
typedef size_t                  AWB_SIZE;
typedef uintptr_t               AWB_UINTPTR;
typedef ptrdiff_t               AWB_PTRDIFF;


#define AWB_DUMPFILE         0
#define AWB_LOADFILE         1

#define AWB_DISABLE          0
#define AWB_ENABLE           1

#define AWB_OFF              0
#define AWB_ON               1

#define AWB_FALSE            0
#define AWB_TRUE             1

#define AWB_DONE             0
#define AWB_BUSY             1

#define AWB_EXIT_FAILURE     -1
#define AWB_EXIT_SUCCESS     0

#define AWB_DUMP_TRIG_OFF    0
#define AWB_DUMP_TRIG_ON     2


// Hardware AWB summation type.
typedef enum _HW_AWB_SUMMATION_TYPE {
    HW_AWB_SUMMATION_TYPE_PIXEL = 0,
    HW_AWB_SUMMATION_TYPE_WEIGHTING,
    HW_AWB_SUMMATION_TYPE_MAX
} HW_AWB_SUMMATION_TYPE, *PHW_AWB_SUMMATION_TYPE;


typedef union _HW_AWB_UTDWORD {
    AWB_UINT32 dwValue;
    AWB_UINT8 byValue[4];
    struct {
        AWB_UINT32 wValue0:16;
        AWB_UINT32 wValue1:16;
    } tw;
    struct {
        AWB_UINT32 byValue0:8;
        AWB_UINT32 byValue1:8;
        AWB_UINT32 byValue2:8;
        AWB_UINT32 byValue3:8;
    } tb;
} HW_AWB_UTDWORD, *PHW_AWB_UTDWORD;


// Hardware AWB boundary range 8 bits.
typedef struct _HW_AWB_BOUNDARY_RANGE_8 {
    AWB_UINT8 ui8Min;
    AWB_UINT8 ui8Max;
} HW_AWB_BOUNDARY_RANGE_8, *PHW_AWB_BOUNDARY_RANGE_8;


// Hardware AWB boundary range 16 bits.
typedef struct _HW_AWB_BOUNDARY_RANGE_16 {
    AWB_UINT16 ui16Min;
    AWB_UINT16 ui16Max;
} HW_AWB_BOUNDARY_RANGE_16, *PHW_AWB_BOUNDARY_RANGE_16;


// Hardware AWB boundary.
typedef struct _HW_AWB_BOUNDARY {
    AWB_UINT8 ui8SummationType;
    HW_AWB_BOUNDARY_RANGE_8 ps_r;
    HW_AWB_BOUNDARY_RANGE_8 ps_g;
    HW_AWB_BOUNDARY_RANGE_8 ps_b;
    HW_AWB_BOUNDARY_RANGE_8 ps_y;
    HW_AWB_BOUNDARY_RANGE_16 ps_gr;
    HW_AWB_BOUNDARY_RANGE_16 ps_gb;
    HW_AWB_BOUNDARY_RANGE_16 ps_grb;
    HW_AWB_BOUNDARY_RANGE_8 ws_r;
    HW_AWB_BOUNDARY_RANGE_8 ws_b;
    HW_AWB_BOUNDARY_RANGE_8 ws_gr;
    HW_AWB_BOUNDARY_RANGE_8 ws_gb;
} HW_AWB_BOUNDARY, *PHW_AWB_BOUNDARY;


// Weight table struct
typedef union _AWB_WS_CW_0_VALUE {
	AWB_UINT32 ui32Weight_0;
	struct {
		AWB_UINT32 ui4Weight_0:4;
		AWB_UINT32 ui4Weight_1:4;
		AWB_UINT32 ui4Weight_2:4;
		AWB_UINT32 ui4Weight_3:4;
		AWB_UINT32 ui4Weight_4:4;
		AWB_UINT32 ui4Weight_5:4;
		AWB_UINT32 ui4Weight_6:4;
		AWB_UINT32 ui4Weight_7:4;
	};
} AWB_WS_CW_0_VALUE, *PAWB_WS_CW_0_VALUE;


typedef union _AWB_WS_CW_1_VALUE {
	AWB_UINT32 ui32Weight_1;
	struct {
		AWB_UINT32 ui4Weight_8:4;
		AWB_UINT32 ui4Weight_9:4;
		AWB_UINT32 ui4Weight_10:4;
		AWB_UINT32 ui4Weight_11:4;
		AWB_UINT32 ui4Weight_12:4;
	};
} AWB_WS_CW_1_VALUE, *PAWB_WS_CW_1_VALUE;


typedef struct _AWB_WEIGHT {
	AWB_WS_CW_0_VALUE stValue0_0;	//(0, 0) ~ (0, 7)
	AWB_WS_CW_1_VALUE stValue0_1;	//(0, 8) ~ (0, 12)
	
	AWB_WS_CW_0_VALUE stValue1_0;	//(1, 0) ~ (1, 7)
	AWB_WS_CW_1_VALUE stValue1_1;	//(1, 8) ~ (1, 12)
	
	AWB_WS_CW_0_VALUE stValue2_0;	//	...
	AWB_WS_CW_1_VALUE stValue2_1;
	
	AWB_WS_CW_0_VALUE stValue3_0;
	AWB_WS_CW_1_VALUE stValue3_1;
	
	AWB_WS_CW_0_VALUE stValue4_0;
	AWB_WS_CW_1_VALUE stValue4_1;
	
	AWB_WS_CW_0_VALUE stValue5_0;
	AWB_WS_CW_1_VALUE stValue5_1;
	
	AWB_WS_CW_0_VALUE stValue6_0;
	AWB_WS_CW_1_VALUE stValue6_1;
	
	AWB_WS_CW_0_VALUE stValue7_0;	// ...
	AWB_WS_CW_1_VALUE stValue7_1;
	
	AWB_WS_CW_0_VALUE stValue8_0;
	AWB_WS_CW_1_VALUE stValue8_1;
	
	AWB_WS_CW_0_VALUE stValue9_0;
	AWB_WS_CW_1_VALUE stValue9_1;	// ...
	
	AWB_WS_CW_0_VALUE stValue10_0;
	AWB_WS_CW_1_VALUE stValue10_1;
	
	AWB_WS_CW_0_VALUE stValue11_0;
	AWB_WS_CW_1_VALUE stValue11_1;
	
	AWB_WS_CW_0_VALUE stValue12_0;	//(12, 0) ~ (12, 7)
	AWB_WS_CW_1_VALUE stValue12_1;	//(12, 8) ~ (12, 12)
} AWB_WEIGHT, *PAWB_WEIGHT;


// V
typedef union _AWB_WS_IW_V_0_VALUE {
	AWB_UINT32 ui32WS_IW_V_0;
	struct {
		AWB_UINT32 ui4WS_IW_V_0:4;
		AWB_UINT32 ui4WS_IW_V_1:4;
		AWB_UINT32 ui4WS_IW_V_2:4;
		AWB_UINT32 ui4WS_IW_V_3:4;
		AWB_UINT32 ui4WS_IW_V_4:4;
		AWB_UINT32 ui4WS_IW_V_5:4;
		AWB_UINT32 ui4WS_IW_V_6:4;
		AWB_UINT32 ui4WS_IW_V_7:4;
	};
} AWB_WS_IW_V_0_VALUE, *PAWB_WS_IW_V_0_VALUE;

typedef union _AWB_WS_IW_V_1_VALUE {
	AWB_UINT32 ui32WS_IW_V_1;
	struct {
		AWB_UINT32 ui4WS_IW_V_8:4;
		AWB_UINT32 ui4WS_IW_V_9:4;
		AWB_UINT32 ui4WS_IW_V_10:4;
		AWB_UINT32 ui4WS_IW_V_11:4;
		AWB_UINT32 ui4WS_IW_V_12:4;
		AWB_UINT32 ui4WS_IW_V_13:4;
		AWB_UINT32 ui4WS_IW_V_14:4;
		AWB_UINT32 ui4WS_IW_V_15:4;
	};
} AWB_WS_IW_V_1_VALUE, *PAWB_WS_IW_V_1_VALUE;

typedef struct _AWB_V_CURVE {
	AWB_WS_IW_V_0_VALUE stValue0;
	AWB_WS_IW_V_1_VALUE stValue1;
} AWB_V_CURVE, *PAWB_V_CURVE;


// S
typedef union _AWB_WS_IW_S_0_VALUE {
	AWB_UINT32 ui32WS_IW_S_0;
	struct {
		AWB_UINT32 ui4WS_IW_S_0:8;
		AWB_UINT32 ui4WS_IW_S_1:8;
		AWB_UINT32 ui4WS_IW_S_2:8;
		AWB_UINT32 ui4WS_IW_S_3:8;
	};
} AWB_WS_IW_S_0_VALUE, *PAWB_WS_IW_S_0_VALUE;

typedef union _AWB_WS_IW_S_1_VALUE {
	AWB_UINT32 ui32WS_IW_S_1;
	struct {
		AWB_UINT32 ui4WS_IW_S_7:8;
		AWB_UINT32 ui4WS_IW_S_6:8;
		AWB_UINT32 ui4WS_IW_S_5:8;
		AWB_UINT32 ui4WS_IW_S_4:8;
	};
} AWB_WS_IW_S_1_VALUE, *PAWB_WS_IW_S_1_VALUE;

typedef union _AWB_WS_IW_S_2_VALUE {
	AWB_UINT32 ui32WS_IW_S_2;
	struct {
		AWB_UINT32 ui4WS_IW_S_11:8;
		AWB_UINT32 ui4WS_IW_S_10:8;
		AWB_UINT32 ui4WS_IW_S_9:8;
		AWB_UINT32 ui4WS_IW_S_8:8;
	};
} AWB_WS_IW_S_2_VALUE, *PAWB_WS_IW_S_2_VALUE;

typedef union _AWB_WS_IW_S_3_VALUE {
	AWB_UINT32 ui32WS_IW_S_3;
	struct {
		AWB_UINT32 ui4WS_IW_S_15:8;
		AWB_UINT32 ui4WS_IW_S_14:8;
		AWB_UINT32 ui4WS_IW_S_13:8;
		AWB_UINT32 ui4WS_IW_S_12:8;
	};
} AWB_WS_IW_S_3_VALUE, *PAWB_WS_IW_S_3_VALUE;


typedef struct _AWB_S_CURVE {
	AWB_WS_IW_S_0_VALUE stValue0;
	AWB_WS_IW_S_1_VALUE stValue1;
	AWB_WS_IW_S_2_VALUE stValue2;
	AWB_WS_IW_S_3_VALUE stValue3;	
} AWB_S_CURVE, *PAWB_S_CURVE;


extern AWB_UINT32 hw_awb_module_scd_status_error_get(void);

extern void hw_awb_module_scd_status_done_set(AWB_UINT32 done_set);
extern AWB_UINT32 hw_awb_module_scd_status_done_get(void);

extern void hw_awb_module_scd_addr_set(AWB_UINT32 scd_addr);
extern AWB_UINT32 hw_awb_module_scd_addr_get(void);

extern void hw_awb_module_scd_id_set(AWB_UINT32 scd_id);

extern void hw_awb_module_scd_irq_set(AWB_UINT32 scd_irq);
extern AWB_UINT32 hw_awb_module_scd_irq_get(void);


extern void hw_awb_module_sc_enable_set(AWB_UINT32 enable);
extern AWB_UINT32 hw_awb_module_sc_enable_get(void);


extern void hw_awb_module_sc_pixel_set(AWB_UINT32 sc_pixel);
extern void hw_awb_module_sc_cropping_set(AWB_UINT32 sc_vstart, AWB_UINT32 sc_hstart);
extern void hw_awb_module_sc_decimation_set(AWB_UINT32 sc_dev_vkeep, AWB_UINT32 sc_dev_vperiod, AWB_UINT32 sc_dev_hkeep, AWB_UINT32 sc_dev_hperiod);
extern void hw_awb_module_sc_windowing_set(AWB_UINT32 sc_height, AWB_UINT32 sc_width);

extern void hw_awb_module_sc_ps_boundary_g_r_get(AWB_UINT8 *ps_upper_g, AWB_UINT8 *ps_lower_g, AWB_UINT8 *ps_upper_r, AWB_UINT8 *ps_lower_r);
extern void hw_awb_module_sc_ps_boundary_g_r_set(AWB_UINT32 ps_upper_g, AWB_UINT32 ps_lower_g, AWB_UINT32 ps_upper_r, AWB_UINT32 ps_lower_r);
extern void hw_awb_module_sc_ps_boundary_y_b_get(AWB_UINT8 *ps_upper_y, AWB_UINT8 *ps_lower_y, AWB_UINT8 *ps_upper_b, AWB_UINT8 *ps_lower_b);
extern void hw_awb_module_sc_ps_boundary_y_b_set(AWB_UINT32 ps_upper_y, AWB_UINT32 ps_lower_y, AWB_UINT32 ps_upper_b, AWB_UINT32 ps_lower_b);
extern void hw_awb_module_sc_ps_boundary_gr_get(AWB_UINT16 *ps_upper_gr_ratio, AWB_UINT16 *ps_lower_gr_ratio);
extern void hw_awb_module_sc_ps_boundary_gr_set(AWB_UINT32 ps_upper_gr_ratio, AWB_UINT32 ps_lower_gr_ratio);
extern void hw_awb_module_sc_ps_boundary_gb_get(AWB_UINT16 *ps_upper_gb_ratio, AWB_UINT16 *ps_lower_gb_ratio);
extern void hw_awb_module_sc_ps_boundary_gb_set(AWB_UINT32 ps_upper_gb_ratio, AWB_UINT32 ps_lower_gb_ratio);
extern void hw_awb_module_sc_ps_boundary_grb_get(AWB_UINT16 *ps_upper_grb_ratio, AWB_UINT16 *ps_lower_grb_ratio);
extern void hw_awb_module_sc_ps_boundary_grb_set(AWB_UINT32 ps_upper_grb_ratio, AWB_UINT32 ps_lower_grb_ratio);
extern void hw_awb_module_sc_ws_boundary_gr_r_get(AWB_UINT8 *ws_upper_gr, AWB_UINT8 *ws_lower_gr, AWB_UINT8 *ws_upper_r, AWB_UINT8 *ws_lower_r);
extern void hw_awb_module_sc_ws_boundary_gr_r_set(AWB_UINT32 ws_upper_gr, AWB_UINT32 ws_lower_gr, AWB_UINT32 ws_upper_r, AWB_UINT32 ws_lower_r);
extern void hw_awb_module_sc_ws_boundary_b_gb_get(AWB_UINT8 *ws_upper_b, AWB_UINT8 *ws_lower_b, AWB_UINT8 *ws_upper_gb, AWB_UINT8 *ws_lower_gb);
extern void hw_awb_module_sc_ws_boundary_b_gb_set(AWB_UINT32 ws_upper_b, AWB_UINT32 ws_lower_b, AWB_UINT32 ws_upper_gb, AWB_UINT32 ws_lower_gb);

extern void hw_awb_module_init(void);

extern void hw_awb_module_register_dump(void);


#if defined(__cplusplus)
}
#endif //__cplusplus

#endif // INFOTM_HW_AWB_METHOD

#endif // __CI_HW_AWB_H__




