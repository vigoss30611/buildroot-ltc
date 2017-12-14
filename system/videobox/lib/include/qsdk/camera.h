// Copyright (C) 2016 InfoTM, warits.wang@infotm.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __VIDEOBOX_API_H__
#error YOU SHOULD NOT INCLUDE THIS FILE DIRECTLY, INCLUDE <videobox.h> INSTEAD
#endif

#ifndef __VB_CAMERA_API_H__
#define __VB_CAMERA_API_H__


#define CAM_ISP_YUV_GAMMA_COUNT     63
#define MAX_CAM_FCEFILE_LIST        5


enum cam_mirror_state {
    CAM_MIRROR_NONE,
    CAM_MIRROR_H,
    CAM_MIRROR_V,
    CAM_MIRROR_HV,
};

enum cam_direction {
    CAM_UP,
    CAM_DOWN,
    CAM_LEFT,
    CAM_RIGHT,
};

struct cam_zoom_info {
    double a_multiply;
    double d_multiply;
};

enum cam_wb_mode {
    CAM_WB_AUTO,
    CAM_WB_2500K,
    CAM_WB_4000K,
    CAM_WB_5300K,
    CAM_WB_6800K
};

enum cam_scenario {
    CAM_DAY,
    CAM_NIGHT
};

enum cam_day_mode {
    CAM_DAY_MODE_NIGHT,
    CAM_DAY_MODE_DAY
};

#define MAX_CAM_RES (5)

typedef enum __cam_common_mode_e
{
    CAM_CMN_AUTO,
    CAM_CMN_MANUAL,
    CAM_CMN_MAX
}cam_common_mode_e;

typedef enum __cam_data_format_e {
    CAM_DATA_FMT_NONE,
    CAM_DATA_FMT_BAYER_FLX = 1<<0,
    CAM_DATA_FMT_BAYER_RAW = 1<<1,
    CAM_DATA_FMT_YUV = 1<<2
}cam_data_format_e;

typedef enum __cam_grid_size_e
{
    CAM_GRID_SIZE_32,
    CAM_GRID_SIZE_16,
    CAM_GRID_SIZE_8,
    CAM_GRID_SIZE_MAX
}cam_grid_size_e;

typedef enum __cam_fce_port_type_e
{
    CAM_FCE_PORT_NONE,
    CAM_FCE_PORT_UO,
    CAM_FCE_PORT_SS0,
    CAM_FCE_PORT_SS1,
    CAM_FCE_PORT_DN,
    CAM_FCE_PORT_MAX
}cam_fce_port_type_e;

typedef enum __cam_fcedata_fisheye_mode_e
{
    CAM_FE_MODE_DOWNVIEW,
    CAM_FE_MODE_FRONTVIEW,
    CAM_FE_MODE_CENTERSCALE,
    CAM_FE_MODE_ORIGINSCALE,
    CAM_FE_MODE_ROTATE,
    CAM_FE_MODE_UPVIEW,
    CAM_FE_MODE_MERCATOR,
    CAM_FE_MODE_PERSPECTIVE,
    CAM_FE_MODE_CYLINDER,
    CAM_FE_MODE_MAX
}cam_fcedata_fisheye_mode_e;

typedef struct __res_t
{
    int W;
    int H;
}cam_res_t;

typedef struct __reslist_t
{
    cam_res_t res[MAX_CAM_RES];
    int count;
}cam_reslist_t;

typedef struct __cam_rgb_t
{
    double r;
    double g;
    double b;
}cam_rgb_t;

typedef struct __cam_common_t
{
    int mdl_en; //module enable, disable
    int version; // LSB 0x0101 = ver1.1
    cam_common_mode_e mode;  //auto, manual
}cam_common_t;

/*Defective Pixels Fixing*/
typedef struct __cam_dpf_attr_t
{
    cam_common_t cmn;
    int threshold;
    double weight;
}cam_dpf_attr_t;

/*Primary denoiser*/
typedef struct __cam_dns_attr_t
{
    cam_common_t cmn;
    int is_combine_green;
    double strength;
    double greyscale_threshold;
    double sensor_gain;
    int sensor_bitdepth;
    int sensor_well_depth;
    double sensor_read_noise;
}cam_dns_attr_t;

/*Sharpening denoiser*/
typedef struct __cam_sha_dns_attr_t
{
    cam_common_t cmn;
    double strength;
    double smooth;
}cam_sha_dns_attr_t;

/*Manual White Balance*/
typedef struct __cam_wb_man_attr_t
{
    cam_rgb_t rgb;
}cam_wb_man_attr_t;

/*White Balance*/
typedef struct __cam_wb_attr_t
{
    cam_common_t cmn;
    cam_wb_man_attr_t man;
}cam_wb_attr_t;

/*3D Denoiser*/
typedef struct __cam_3d_dns_attr_t
{
    cam_common_t cmn;
    int y_threshold;
    int u_threshold;
    int v_threshold;
    int weight;
}cam_3d_dns_attr_t;

/*YUV Gamma*/
typedef struct __cam_yuv_gamma_attr_t
{
    cam_common_t cmn;
    float curve[CAM_ISP_YUV_GAMMA_COUNT];
}cam_yuv_gamma_attr_t;

/*range fps*/
typedef struct __cam_fps_range_t
{
    float min_fps;
    float max_fps;
}cam_fps_range_t;

typedef struct __cam_save_data_t
{
    cam_data_format_e fmt;
    char file_path[100];
    char file_name[100];
}cam_save_data_t;

typedef struct __cam_fcedata_common_t
{
    int out_width;
    int out_height;
    cam_grid_size_e grid_size;
    int fisheye_center_x;
    int fisheye_center_y;
}cam_fcedata_common_t;

typedef struct __cam_fisheye_correction_t
{
    int fisheye_mode;
    int fisheye_gridsize;

    double fisheye_start_theta;
    double fisheye_end_theta;
    double fisheye_start_phi;
    double fisheye_end_phi;

    int fisheye_radius;

    int rect_center_x;
    int rect_center_y;

    int fisheye_flip_mirror;
    double scaling_width;
    double scaling_height;
    double fisheye_rotate_angle;
    double fisheye_rotate_scale;

    double fisheye_heading_angle;
    double fisheye_pitch_angle;
    double fisheye_fov_angle;

    double coefficient_horizontal_curve;
    double coefficient_vertical_curve;

    double fisheye_theta1;
    double fisheye_theta2;
    double fisheye_translation1;
    double fisheye_translation2;
    double fisheye_translation3;
    double fisheye_center_x2;
    double fisheye_center_y2;
    double fisheye_rotate_angle2;

    int debug_info;
}cam_fisheye_correction_t;

typedef struct __cam_fcedata_param_t
{
    cam_fcedata_common_t common;
    cam_fisheye_correction_t fec;
}cam_fcedata_param_t;

typedef struct __cam_position_t
{
    int x;
    int y;
    int width;
    int height;
}cam_position_t;

typedef struct __cam_fcefile_port_t
{
    cam_fce_port_type_e type;
    cam_position_t pos;
}cam_fcefile_port_t;

typedef struct __cam_fcefile_file_t
{
    char file_grid[128];
    char file_geo[128];
    int outport_number;
    int need_alloc_buf;
    int ov_stitch;
    int ov0_x;
    int ov0_y;
    int ov0_width;
    int ov0_height;
    cam_fcefile_port_t outport_list[CAM_FCE_PORT_MAX - 1];
}cam_fcefile_file_t;

typedef struct __cam_fcefile_mode_t
{
    int file_number;
    int in_w;
    int in_h;
    int out_w;
    int out_h;
    cam_fcefile_file_t file_list[MAX_CAM_FCEFILE_LIST];
}cam_fcefile_mode_t;

typedef struct __cam_fcefile_param_t
{
    int mode_number;
    cam_fcefile_mode_t mode_list[MAX_CAM_FCEFILE_LIST];
}cam_fcefile_param_t;

typedef struct __cam_save_fcedata_param_t
{
    char file_name[100];
}cam_save_fcedata_param_t;

typedef struct cam_list_st {
    char name[13][12];
}cam_list_t;

typedef struct _cam_gamcurve_t
{
    uint16_t curve[63];
}cam_gamcurve_t;

int camera_load_parameters(const char *cam, const char *js, int len);
int camera_get_direction(const char *cam);
int camera_set_direction(const char *cam, enum cam_direction);
int camera_get_mirror(const char *cam, enum cam_mirror_state *cms);
int camera_set_mirror(const char *cam, enum cam_mirror_state);
int camera_get_fps(const char *cam, int *out_fps);
int camera_set_fps(const char *cam, int fps);
int camera_get_brightness(const char *cam, int *out_bri);
int camera_set_brightness(const char *cam, int bri);
int camera_get_contrast(const char *cam, int *out_con);
int camera_set_contrast(const char *cam, int con);
int camera_get_saturation(const char *cam, int *out_sat);
int camera_set_saturation(const char *cam, int sat);
int camera_get_sharpness(const char *cam, int *out_sp);
int camera_set_sharpness(const char *cam, int sp);
int camera_get_antifog(const char *cam);
int camera_set_antifog(const char *cam, int level);
int camera_get_antiflicker(const char *cam, int *out_freq);
int camera_set_antiflicker(const char *cam, int freq);
int camera_set_wb(const char *cam, enum cam_wb_mode mode);
int camera_get_wb(const char *cam, enum cam_wb_mode *out_mode);
int camera_set_scenario(const char *cam, enum cam_scenario);
int camera_get_scenario(const char *cam, enum cam_scenario *out_sno);
int camera_get_hue(const char *cam, int *out_hue);
int camera_set_hue(const char *cam, int hue);
int camera_get_brightnessvalue(const char *cam, int *out_bv);
int camera_get_ISOLimit(const char *cam, int *out_iso);
int camera_set_ISOLimit(const char *cam, int iso);
int camera_get_exposure(const char *cam, int *out_level);
int camera_set_exposure(const char *cam, int level);

/* features on/off */
int camera_monochrome_is_enabled(const char *cam);
int camera_monochrome_enable(const char *cam, int en);
int camera_ae_is_enabled(const char *cam);
int camera_ae_enable(const char *cam, int en);
int camera_wdr_is_enabled(const char *cam);
int camera_wdr_enable(const char *cam, int en);

/* focus functions */
int camera_af_is_enabled(const char *cam);
int camera_af_enable(const char *cam, int en);
int camera_reset_lens(const char *cam);
int camera_focus_is_locked(const char *cam);
int camera_focus_lock(const char *cam, int lock);
int camera_get_focus(const char *cam);
int camera_set_focus(const char *cam, int distance);
int camera_get_zoom(const char *cam, struct cam_zoom_info *info);
int camera_set_zoom(const char *cam, struct cam_zoom_info *info);
int camera_get_snap_res(const char *cam, cam_reslist_t *preslist);
int camera_set_snap_res(const char *cam, cam_res_t res);
int camera_snap_one_shot(const char *cam);
int camera_snap_exit(const char *cam);

int camera_v4l2_snap_one_shot(const char *cam);
int camera_v4l2_snap_exit(const char *cam);

int camera_get_sensor_exposure(const char *cam, int *out_usec);
int camera_set_sensor_exposure(const char *cam, int usec);

int camera_get_dpf_attr(const char *cam, cam_dpf_attr_t *attr);
int camera_set_dpf_attr(const char *cam, const cam_dpf_attr_t *attr);

int camera_get_dns_attr(const char *cam, cam_dns_attr_t *attr);
int camera_set_dns_attr(const char *cam, const cam_dns_attr_t *attr);

int camera_get_sha_dns_attr(const char *cam, cam_sha_dns_attr_t *attr);
int camera_set_sha_dns_attr(const char *cam, const cam_sha_dns_attr_t *attr);

int camera_get_wb_attr(const char *cam, cam_wb_attr_t *attr);
int camera_set_wb_attr(const char *cam, const cam_wb_attr_t *attr);

int camera_get_3d_dns_attr(const char *cam, cam_3d_dns_attr_t *attr);
int camera_set_3d_dns_attr(const char *cam, const cam_3d_dns_attr_t *attr);

int camera_get_yuv_gamma_attr(const char *cam, cam_yuv_gamma_attr_t *attr);
int camera_set_yuv_gamma_attr(const char *cam, const cam_yuv_gamma_attr_t *attr);

int camera_get_fps_range(const char *cam, cam_fps_range_t *fps_range);
int camera_set_fps_range(const char *cam, const cam_fps_range_t *fps_range);

int camera_get_day_mode(const char *cam, enum cam_day_mode *out_day_mode);
int camera_set_gamcurve(const char *cam, cam_gamcurve_t *curve);
int camera_set_index(const char *cam, int idx);
int camera_get_index(const char *cam, int *idx);
int camera_save_data(const char *cam, const cam_save_data_t *save_data);

int camera_get_iq(const char *cam, void *pdata, int len);
int camera_set_iq(const char *cam, const void *pdata, int len);
int camera_save_isp_parameters(const char *cam, const cam_save_data_t *save_data);

int camera_get_ispost(const char *cam, void *pdata, int len);
int camera_set_ispost(const char *cam, const void *pdata, int len);

int camera_create_and_fire_fcedata(const char *cam, cam_fcedata_param_t *fcedata);
int camera_load_and_fire_fcedata(const char *cam, cam_fcefile_param_t *fcefile);
int camera_set_fce_mode(const char *cam, int mode);
int camera_get_fce_mode(const char *cam, int *mode);
int camera_get_fce_totalmodes(const char *cam, int *mode_count);
int camera_save_fcedata(const char *cam, cam_save_fcedata_param_t *param);
int camera_get_sensors(const char *cam, cam_list_t *list);
#endif /* __VB_CAMERA_API_H__ */
