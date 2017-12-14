#include <stdio.h>     /*  for printf */
#include <stdlib.h>    /*  for exit */
#include <unistd.h>
#include  <getopt.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <qsdk/videobox.h>
#include <qsdk/rtsp_server.h>


#include <hlibvideodebug/video_debug.h> /*for video debug system*/

/*
*start stop cam video have the same priority and the highest priority
*if cam is on then get other command followed
*if video is on then get other command followed
*/

#define TEST_DEBUG 0
#if TEST_DEBUG
    #define CTRLVB_DBG(arg...)    printf("[ctrlvb]" arg)
#else
    #define CTRLVB_DBG(arg...)
#endif

char *channel = NULL;
char *channel_value = NULL;
char *output_path = NULL;
char set_flag = 0;
char channel_flag = 0;
char  overlay_fmt[128] = "%t%Y/%M/%D  %H:%m:%S";
static char info_flag = 0;
static char capture_flag = 0;
static char snapshot_flag = 0;
static char thumbnail_flag = 0;
static char quit = 0;
static char set_res_flag = 0;
static int height = 0, width = 0;
static int frame_mode = 0;
static int s_s32StressCount = 0;
static int s_s32PhotoResStress = 0;
static int s_s32SEIMode = 0;

char **tmp_argv;


#define VB_OK     0
#define VB_FAIL   -1

#define DEFAULT_VIDEO_FAULT  -2
#define DEFAULT_CAM_FAULT    -2

#define SNAPSHOT_LENGTH    0X100000
#define HEADER_LENGTH      128
#define TEST               0
#define TEST_DEFAULT_VALUE 50

struct font_attr font_attr;

typedef struct
{
    int (*camera_func)(const char *cam, void *ptr);
    void *param;
    int size;
}cam_test_info_t;

static int get_cam_list(const char *cam);
static void print_usage(const char *program_name);
static int set_camera_direction(const char *cam,  char *value);
static int get_camera_brightness(const char *cam);
static int set_camera_brightness(const char *cam,  char *value);
static int get_camera_contrast(const char *cam);
static int set_camera_contrast(const char *cam,  char *value);
static int get_camera_ae(const char *cam);
static int set_camera_ae(const char *cam, char *value );
static int set_camera_af(const char *cam, char *value);
static int set_camera_wb(const char *cam, char *value);
static int get_camera_wb(const char *cam);
static int set_camera_zoom(const char *cam, char *value);
static int set_camera_antifog(const char *cam, char *value);
static int get_camera_saturation(const char *cam);
static int set_camera_saturation(const char *cam, char *value);
static int get_camera_sharpness(const char *cam);
static int set_camera_sharpness(const char *cam, char *value);
static int get_camera_wdr(const char *cam);
static int set_camera_wdr(const char *cam, char *value);
static int get_camera_monochrome(const char *cam);
static int set_camera_monochrome(const char *cam, char *value);
static int set_camera_antiflicker(const char *cam, char *value);
static int get_camera_antiflicker(const char *cam);
static int set_camera_lc(const char *cam, char *value);
static int set_camera_lock(const char *cam, char *value);
static int set_camera_fps(const char *cam, char *value);
static int set_camera_focus(const char *cam, char *value);
static int set_camera_hue(const char *cam,  char *value);
static int get_camera_hue(const char *cam);
static int set_camera_gamcurve(const char *cam,  char *value);
static int set_camera_mirror(const char *cam,  char *value);
static int get_camera_mirror(const char *cam);
static int set_camera_scenario(const char *cam,  char *value);
static int get_camera_scenario(const char *cam);
static int set_camera_exposure(const char *cam,  char *value);
static int get_camera_exposure(const char *cam);
static int set_camera_iso(const char *cam,  char *value);
static int get_camera_iso(const char *cam);
static int set_v4l2_snapcapture(const char *cam, char *value);
static int set_camera_snapcapture(const char *cam, char *value);
static int get_camera_sensor_exposure(const char *cam);
static int set_camera_sensor_exposure(const char *cam,  char *value);
static int set_camera_dpf_attr(const char *cam,  char *value);
static int set_camera_dns_attr(const char *cam,  char *value);
static int set_camera_sha_dns_attr(const char *cam,  char *value);
static int set_camera_wb_attr(const char *cam,  char *value);
static int set_camera_3d_dns_attr(const char *cam,  char *value);
static int get_camera_3d_dns_attr(const char *cam);
static int set_camera_yuv_gamma_attr(const char *cam,  char *value);
static int get_camera_yuv_gamma_attr(const char *cam);
static int set_camera_fps_range(const char *cam,  char *value);
static int get_camera_fps_range(const char *cam);
static int set_camera_save_data(const char *cam, char *value);
static int set_camera_create_fcedata(const char *cam);
static int set_camera_load_fcedata(const char *cam, char *value);
static int get_envbrightness(const char *cam);
static int video_thumbnail(const char *channel, char *path);
static int get_camera_day_mode(const char *cam);
static int set_camera_index(const char *cam,  char *value);
static int get_camera_index(const char *cam);
static int set_camera_fce_mode(const char *cam, char *value);
static int get_camera_fce_mode(const char *cam);
static int get_camera_fce_totalmodes(const char *cam);
static int set_camera_save_fcedata(const char *cam, char *value);
static int set_camera_qts_4_grid(const char *cam, char *value);
static int set_camera_loop_fisheye(const char *cam, char *value);
static int set_camera_dgov_fisheye(const char *cam, char *value);
static int test_photo_change_resolution(char * ps8Channel, char *ps8Path, int s32Count);



const char *dir[] = {
    "up",
    "down",
    "right",
    "left"
};

const char *state[] = {
    "off",
    "on",
};

const char *lock_state[] = {
    "locked",
    "unlocked",
};

const char *mirror_state[] = {
    "none",
    "mirror_horizontal",
    "flip_vertical",
    "mirror+flip(H+V)",
};

const char *scenario_state[] = {
    "day",
    "night"
};

const char *wb_mode[] = {
    "auto_wb",
    "2500K",
    "4000K",
    "5300K",
    "6800K"
};

const char *profile_name[] = {
    "baseline",
    "main",
    "high",
};

const char *stream_type[] = {
    "venc_bytestream",
    "venc_nailstream",
};

struct cam_opt_list {
    struct option long_option;
    char *show_name;
    char *show_range; //val range
    union {
    int (*func_param_ptr)(const char *cam,  char *val);
    int (*func_ptr)(const char *cam);
    };
    char *description;
};

struct cam_opt_list s_cam_opts_list[] = {
    {
		{"help",       no_argument,        NULL, 'h'},
		NULL, NULL, NULL,
		"help for command.",
    },
    {
		{"list",       no_argument,        NULL, 'l'},
		NULL, NULL, get_cam_list,
		"list the camera snesor channels info.",
    },
    {
		{"info",       no_argument,        NULL, 'i'},
		NULL, NULL, NULL,
		"show camera info.",
    },
    {
		{"channel",    required_argument,  NULL, 'c'},
		NULL, NULL, NULL,
		"set camera sensor channel.",
    },
    {
        {"get_ae_enable",         no_argument,  NULL, 0},
        NULL, NULL, get_camera_ae,
        "get camera ae status : enable or disable.",
    },
    {
		{"ae",         required_argument,  NULL, 'a'},
		"set ae", "on/off", set_camera_ae,
		"set camera ae to enable or disable.",
    },
    {
		{"awb",        required_argument,  NULL, 'w'},
		NULL, "0:auto, 1:2500K, 2:4000K, 3:5300K, 4:6800K", set_camera_wb,
		"set camera wb mode.",
    },
    {
        {"get_awb_mode",        no_argument,  NULL, 0},
        NULL, "0:auto, 1:2500K, 2:4000K, 3:5300K, 4:6800K", get_camera_wb,
        "set camera wb mode.",
    },

    {
		{"get_brightness", no_argument,  NULL,  0},
		NULL, NULL, get_camera_brightness,
		"get image brightness",
    },
    {
		{"set_brightness", required_argument, NULL, 'b'},
		NULL, "0--255", set_camera_brightness,
		"set image to increase or decrease brightness",
    },
    {
		{"get_contrast",   no_argument,  NULL,  0},
		NULL, NULL, get_camera_contrast,
		"get image contrast",
    },
    {
		{"set_contrast",   required_argument,  NULL,  't'},
		NULL, "0--255", set_camera_contrast,
		"set image to increase or decrease contrast",
    },
    {
		{"get_saturation", no_argument,  NULL,   0},
		NULL, NULL, get_camera_saturation,
		"get image saturation",
    },
    {
		{"set_saturation", required_argument,  NULL,   0},
		NULL, "0--255", set_camera_saturation,
		"set image to increase or decrease saturation",
    },
    {
		{"get_sharpness",  no_argument,  NULL,   0},
		NULL, NULL, get_camera_sharpness,
		"get image sharpness",
    },
    {
		{"set_sharpness",  required_argument,  NULL,   0},
		NULL, "0--255", set_camera_sharpness,
		"set image to increase or decrease sharpness",
    },
    {
		{"get_hue", no_argument,  NULL,  0},
		NULL, NULL, get_camera_hue,
		"get image hue",
    },
    {
		{"set_hue",   required_argument,  NULL,  0},
		NULL, "0--255", set_camera_hue,
		"set image to increase or decrease hue",
    },
    {
		{"set_gamcurve",   required_argument,  NULL,  0},
		NULL, "1--3", set_camera_gamcurve,
		"set camera gamcurve",
    },
    {
        {"get_monochrome_status", no_argument,  NULL,   0},
        NULL, NULL, get_camera_monochrome,
        "get camera monochrome status: enable or disable",
    },
    {
		{"monochrome", required_argument,  NULL,   0},
		NULL, "on/off", set_camera_monochrome,
		"set camera monochrome to enable or disable",
    },
    {
		{"set_antiflicker",required_argument,  NULL,   0},
		NULL, "0--255", set_camera_antiflicker,
		"set camera antiflicker value.",
    },
    {
        {"get_antiflicker",no_argument,  NULL,   0},
        NULL, "0--255", get_camera_antiflicker,
        "get camera antiflicker value.",
    },
    {
		{"fps",        required_argument,  NULL,   0},
		NULL, NULL, set_camera_fps,
		"set sensor fps",
    },
    {
        {"get_wdr_enable",      no_argument,  NULL,   0},
        NULL, NULL, get_camera_wdr,
        "get camera wdr mode status enable or disable.",
    },
    {
		{"wdr",      required_argument,  NULL,   0},
		NULL, "on/off", set_camera_wdr,
		"set camera wdr mode to enable or disable.",
    },
    {
		{"set_mirror",      required_argument,  NULL,   0},
		NULL, "0:none, 1:mirror, 2:flip, 3:mirror+flip", set_camera_mirror,
		"set sensor mirror or flip.",
    },

    {
        {"get_mirror",      no_argument,  NULL,   0},
        NULL, "0:none, 1:mirror, 2:flip, 3:mirror+flip", get_camera_mirror,
        "get sensor mirror or flip.",
    },

    {
		{"set_scenario",      required_argument,  NULL,   0},
		NULL, "0:day, 1:night", set_camera_scenario,
		"set camera scenario.",
    },

    {
        {"get_scenario",      no_argument,  NULL,   0},
        NULL, "0:day, 1:night", get_camera_scenario,
        "get camera scenario.",
    },

    {
		{"set_exposure",      required_argument,  NULL,   0},
		NULL, "-4 ~ 4", set_camera_exposure,
		"set camera exposure compensation.",
    },
    {
        {"get_exposure",      no_argument,  NULL,   0},
        NULL, "-4 ~ 4", get_camera_exposure,
        "get camera exposure compensation.",
    },
    {
        {"get_envbrigtness",      no_argument,  NULL,   0},
        NULL, "0 ~ 255", get_envbrightness,
        "get camera statistics lumination value.",
    },
    {
		{"set_iso",      required_argument,  NULL,   0},
		NULL, "0--255", set_camera_iso,
		"set camera ISO.",
    },
    {
        {"get_iso",      no_argument,  NULL,   0},
        NULL, "1--255", get_camera_iso,
        "get camera ISO.",
    },
    {
		{"v4l2_snap",      required_argument,  NULL,   0},
		NULL, "one#file path",
		set_v4l2_snapcapture,
		"v4l2 snap a camera frame.",

    },
    {
		{"snapcapture",      required_argument,  NULL,   0},
		NULL, "one/mult/simple/mode0/mode1/mode2/mode3#file path",
		set_camera_snapcapture,
		"snap a camera frame.",

    },
    {
        {"get_sensor_exposure", no_argument,  NULL,   0},
        NULL, NULL,      get_camera_sensor_exposure,
        "get sensor exposure time (microseconds).",
    },
    {
		{"set_sensor_exposure", required_argument,  NULL,   0},
		NULL, "XX(us)",      set_camera_sensor_exposure,
		"set sensor exposure time (microseconds).",
    },
    {
		{"set_dpf_attr", required_argument,  NULL,   0},
		NULL, "0/1",      set_camera_dpf_attr,
		"set camera defective pixels fixing attribute.",
    },
    {
		{"set_dns_attr", required_argument,  NULL,   0},
		NULL, "0/1",      set_camera_dns_attr,
		"set camera denoiser attribute.",
    },
    {
		{"set_sha_dns_attr", required_argument,  NULL,   0},
		NULL, "0/1",      set_camera_sha_dns_attr,
		"set camera sharpening denoise attribute.",
    },
    {
		{"set_wb_attr", required_argument,  NULL,   0},
		NULL, "0/1",      set_camera_wb_attr,
		"set camera white balance attribute.",
    },
    {
		{"set_3d_dns_attr", required_argument,  NULL,   0},
		NULL, "0/1",      set_camera_3d_dns_attr,
		"set camera 3d denoiser attribute.",
    },
    {
		{"get_3d_dns_attr", no_argument,  NULL,   0},
		NULL, NULL,      get_camera_3d_dns_attr,
		"get camera 3d denoiser attribute.",
    },
    {
		{"set_yuv_gamma_attr", required_argument,  NULL,   0},
		NULL, "0/1",      set_camera_yuv_gamma_attr,
		"set camera y gamma attribute.",
    },
    {
		{"get_yuv_gamma_attr", no_argument,  NULL,   0},
		NULL, NULL,      get_camera_yuv_gamma_attr,
		"get camera y gamma attribute.",
    },
    {
		{"set_fps_range", required_argument,  NULL,   0},
		NULL, "0/1",      set_camera_fps_range,
		"set camera fps range.",
    },
    {
		{"get_fps_range", no_argument,  NULL,   0},
		NULL, NULL,      get_camera_fps_range,
		"get camera fps range.",
    },
    {
		{"set_save_data", required_argument,  NULL,   0},
		NULL, "file path",      set_camera_save_data,
		"save camera data to file.",
    },
    {
		{"set_create_fcedata", no_argument,  NULL,   0},
		NULL, NULL,      set_camera_create_fcedata,
		"create grid data.",
    },
    {
		{"set_load_fcedata", required_argument,  NULL,   0},
		NULL, "file path",      set_camera_load_fcedata,
		"load grid file.",
    },
    {
		{"set_fce_mode", required_argument,  NULL,   0},
		NULL, "0--3",    set_camera_fce_mode,
		"set fce mode",
    },
    {
		{"get_fce_mode", no_argument,  NULL,   0},
		NULL, NULL,      get_camera_fce_mode,
		"get fce mode",
    },
    {
		{"get_fce_totalmodes", no_argument,  NULL,   0},
		NULL, NULL,      get_camera_fce_totalmodes,
		"get fce total modes",
    },
    {
		{"set_save_fcedata", required_argument,  NULL,   0},
		NULL, "file path",      set_camera_save_fcedata,
		"save fce data",
    },
    {
		{"get_day_mode", no_argument,  NULL,   0},
		NULL, NULL,      get_camera_day_mode,
		"get day mode",
    },
    {
		{"set_index", required_argument,  NULL,   0},
		NULL, "0/1",      set_camera_index,
		"set camera index",
    },
    {
		{"get_index", no_argument,  NULL,   0},
		NULL, NULL,      get_camera_index,
		"get camera index",
    },
    {
		{"set_qts_4_grid", required_argument,  NULL,   0},
		NULL, "file path",      set_camera_qts_4_grid,
		"show 4 grid for QTS.",
    },
    {
		{"set_loop_fisheye", required_argument,  NULL,   0},
		NULL, "file path",      set_camera_loop_fisheye,
		"switch fisheye mode ",
    },
    {
		{"set_dgov_fisheye", required_argument,  NULL,   0},
		NULL, "file path",      set_camera_dgov_fisheye,
		"show dg overlay and fisheye mode",
    },
#if 0
	{
		{"direction",  required_argument,  NULL, 'd'},
		NULL, "up/down/left/right", set_camera_direction,
		"set camera sensor direction.",
	},
	{
		{"antifog",    required_argument,  NULL,   0},
		NULL, "on/off", set_camera_antifog,
		"set camera antifog to enable or disable.",
	},
	{
		{"lens",       required_argument,  NULL,   0},
		NULL, "reset", set_camera_lc,
		"reset camera sensor lens value.",
	},
	{
		{"af",         required_argument,  NULL, 'f'},
		NULL, "on/off", set_camera_af,
		"set camera af to enable or disable.",
	},
	{
		{"lock",       required_argument,  NULL,   0},
		NULL, "on/off", set_camera_lock,
		"set camera focus lock mode to enable or disable",
	},
	{
		{"focus",      required_argument,  NULL,   0},
		NULL, NULL, set_camera_focus,
		NULL,
	},
	{
		{"zoom",       required_argument,  NULL, 'z'},
		NULL, "100--300", set_camera_zoom,
		"set camera to zooming",
	},
#endif
    {
		{0,            0,                  NULL,   0},
		NULL, NULL, NULL,
		NULL
    }
};

const char *video_short_opts = "hlsiqc:b:p:o:r:t:";
const struct option video_long_options[] = {
    {"help",       no_argument,        NULL,   'h'},
    {"list",       no_argument,        NULL,   'l'},
    {"info",       no_argument,        NULL,   'i'},
    {"set",        no_argument,        NULL,   's'},
    {"channel",    required_argument,  NULL,   'c'},
    {"bitrate",    required_argument,  NULL,   'b'},
    {"pframe",     required_argument,  NULL,   'p'},
    {"streamtype",     required_argument,  NULL,   't'},
    {"output",     required_argument,  NULL,   'o'},

    {"qp-max",     required_argument,  NULL,     0},
    {"qp-min",     required_argument,  NULL,     0},
    {"qp-hdr",     required_argument,  NULL,     0},
    {"gop-length", required_argument,  NULL,     0},
    {"profile",    required_argument,  NULL,     0},
    {"fps",  required_argument,  NULL,     0},
    {"height",     required_argument,  NULL,     0},
    {"width",      required_argument,  NULL,     0},
    {"resolution", no_argument,        NULL,     0},
    {"enable",     no_argument,        NULL,     0},
    {"disable",    no_argument,        NULL,     0},
    {"day",     no_argument,        NULL,     0},
    {"night",    no_argument,        NULL,     0},
    {"capture",    no_argument,        NULL,     0},
    {"roi",    required_argument,        NULL,     0},
    {"snapshot",   no_argument,        NULL,     0},
    {"set-snapshot", required_argument,  NULL,     0},
    {"trigger-keyframe", no_argument,  NULL,     0},
    {"frc", required_argument,  NULL,     0},
    {"pip", required_argument, NULL,      0},
    {"thumbnail", no_argument, NULL,      0},
    {"set-thumb-res", required_argument,  NULL, 0},
    {"set-pic-quality", required_argument,  NULL, 0},
    {"nvrmode", required_argument,  NULL, 0},
    {"frame-mode", required_argument,   NULL,   0},
    {"photo-res-stress", no_argument, NULL, 0},
    {"stress-count", required_argument, NULL, 0},
    {"set-sei", required_argument, NULL, 0},
    {"set-sei-mode", required_argument, NULL, 0},
    {"set-slice-height", required_argument, NULL, 0},
    {"set-pic-panoramic", required_argument, NULL, 0},
    {0,            0,                  NULL,     0}
};

const char *overlay_short_opts = "a:r:s:c:n:b:f:p:";
const struct option overlay_long_options[] = {
    {"set",        required_argument,        NULL,   's'},
    {"channel",    required_argument,  NULL,   'c'},
    {"font",       required_argument,  NULL,   'f'},
    {"fmt",       required_argument,  NULL,   0},
    {"align",       required_argument,  NULL,   'a'},
    {"color",       required_argument,  NULL,   0},
    {"resize",       required_argument,  NULL,   0},
    {"position",       required_argument,  NULL,   'p'},
    {"random",       required_argument,  NULL,   'r'},
    {"nv12",       required_argument,  NULL,   'n'},
    {"bmp",       required_argument,  NULL,   'b'},
    {"string",       required_argument,  NULL,   0},
    {"shelter",       0,  NULL,   0},
    {0,            0,                  NULL,     0}
};

const char *mv_short_opts = "sc:b:l:t:f:r:";

static void print_cam_usage(void)
{
    int long_list_num = 0;
    int i = 0;
    char line_str[600];
    char short_opt[20];
    char long_opt[256];


    printf("cam     test camera api,the parameter is follow.\n");

    long_list_num = sizeof(s_cam_opts_list) / sizeof(s_cam_opts_list[0]);
    for (i = 0; i < long_list_num; i++)	{
		memset(line_str, 0, sizeof(line_str));
		memset(short_opt, 0, sizeof(short_opt));
		memset(long_opt, 0, sizeof(long_opt));

		if (s_cam_opts_list[i].long_option.val) {
			sprintf(short_opt, "-%c", s_cam_opts_list[i].long_option.val);
			strcat(line_str, short_opt);
		}
		if (s_cam_opts_list[i].long_option.name) {
			if (strlen(line_str))
				strcat(line_str, " ");

			if (s_cam_opts_list[i].show_range)
				sprintf(long_opt, "--%s=(%s)", s_cam_opts_list[i].long_option.name,
								s_cam_opts_list[i].show_range);
			else
				sprintf(long_opt, "--%s", s_cam_opts_list[i].long_option.name);

			strcat(line_str, long_opt);
		}
		if (strlen(line_str)) {
			printf("\t%-30s    %-s \n", line_str, s_cam_opts_list[i].description);
		}
    }

}

static void print_video_usage(void)
{
    printf("video                        test video api,the parameter is follow.\n");
    printf("\t-l --list                  list the video channels info. \n");
    printf("\t-i --info                  get the videobox info. \n");
    printf("\t-s --set                   set the videobox info. \n");
    printf("\t-c --channel               set videobox channels. \n");
    printf("\t-q --quest                 get kinds of info.\n");
    printf("\t-b --bitrate=(0--20.0)     set video bitrate, this must be used together with -s option. \n");
    printf("\t-p --pframe=(0--60)        set video p frame count, this must be used together with -s option. \n");
    printf("\t-t --streamtype=[0, 1]     set video stream type, this must be used together with -s option. \n");
    printf("\t-o --output                set video output file path. \n");
    printf("\t--qp-max                   set video max QP value, this must be used together with -s option. \n");
    printf("\t--qp-min                   set video minimum QP value, this must be used together with -s option. \n");
    printf("\t--qp-hdr                   set video QP hdr value, this must be used together with -s option. \n");
    printf("\t--gop-length               set video max GOP value, this must be used together with -s option. \n");
    printf("\t--profile                  set video encoding profile(main/high/baseline).\n");
    printf("\t--capture                  start capture video frame into a file,The default file will be ./vbctrl_frames, the \n output file can be specified by -o option. The capture procedure can be canceled by Ctrl-C. \n");
    printf("\t--snapshot                 get a snapshot from videobox,This command will create a JPEG file, the default file \n will be ./vbctrl_snap.jpgthe output file can be specified by -o option. \n");
    printf("\t--nvrmode	          set display mode and enable display channel\n");
}

static void print_vam_usage(void)
{
    printf("vam                          access vam features\n");
    printf("\t-i                         list vam status\n");
    printf("\t-s                         set sensitivity, 0 ~ 2\n");
    printf("\t-c                         specify va channel\n");
    printf("\t-f                         specify va fps\n");
    printf("stop                         stop videobox\n");
    printf("ctrl IPU CODE PARA           control videobox common features\n");
}

static void print_mv_usage(void)
{
    printf("mv                          access mv features\n");
    printf("\t-s                         set sensitivity, Tlmv, Tdlmv, Tcmv\n");
    printf("\t-c                         specify mv channel\n");
    printf("\t-f                         specify mv fps\n");
}

static void print_usage(const char *program_name)
{
    printf("%s 1.0.1 (2016-04-14)/n", program_name);
    printf("This is a program test camera and videobox API \n");
    printf("Usage: %s start/stop/cam/video -s/ic  [<channel_name>] -d/b.... [<setvalue>] \n", program_name);
    printf("rebind                       port, target_port\n");
    printf("start                        start videobox service with or without a json file.\n");
    printf("stop                         stop videobox service.\n");

    print_cam_usage();
    print_video_usage();
    print_vam_usage();
    print_mv_usage();
}

static int get_envbrightness(const char *cam)
{
    int env = 0;
    int ret = 0;


    if (NULL != cam) {
#if !TEST
        ret = camera_get_brightnessvalue(cam, &env);
        if(-1 == ret) {
            CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
            return VB_FAIL;
        }
        printf("Get statistics lumination value %d\n", env);
#else
        printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif
    } else {
        return VB_FAIL;
    }
    return VB_OK;
}

//cam command functions
static int get_cam_list(const char *cam)
{
    int i = 0;
#if !TEST
    cam_list_t vlist;

    memset((void *)&vlist, 0, sizeof(vlist));

    camera_get_sensors(cam, &vlist);
#else
    char *list[] = {
		"cam-yuv",
		"cam-dvp",
		"cam-mipi",
		"cam-usb",
		NULL,
    };
#endif

    for(i = 0;i < 13; i++)
    {
	    printf("%s\n", vlist.name[i]);
    }

    return VB_OK;
}

static int get_cam_info(const char *cam)
{
    int ret = 0;
    int direction = 0;
    int fps = 0;
    int lockstate = 0;
    enum cam_mirror_state flip = 0;
    int brightness = 0;
    int contrast = 0;
    int saturation = 0;
    int antifog = 0;
    int sharpness = 0;
    int wdr_state = 0;
    int monochrome_state = 0;
    int antiflicker = 0;
    int focus = 0;
    int ae_state = 0;
    int af_state = 0;
    enum cam_wb_mode wb_mode = 0;
    int hue = 0;
    int brightness_value = 0;
    enum cam_scenario scen = 0;
    int iso = 0;
    int exp_comp = 0;
    struct cam_zoom_info zoom;
    int sensor_exp = 0;
    cam_dpf_attr_t isp_dpf_attr;
    cam_sha_dns_attr_t isp_sha_dns_attr;
    cam_dns_attr_t isp_dns_attr;
    cam_wb_attr_t isp_wb_attr;
    cam_3d_dns_attr_t isp_3d_dns_attr;
    cam_yuv_gamma_attr_t isp_y_gamma_attr;
    cam_fps_range_t fps_range;


    const int line_count = 10;
    char temp_str[30];
    int i = 0;
    int max_cnt = 0;
    cam_test_info_t *p_item = NULL;
    cam_test_info_t func_list[]= {
		{(int (*)(const char *, void *))camera_get_mirror, &flip, sizeof(flip)},
		{(int (*)(const char *, void *))camera_get_fps, &fps, sizeof(fps)},
		{(int (*)(const char *, void *))camera_get_brightness, &brightness, sizeof(brightness)},
		{(int (*)(const char *, void *))camera_get_contrast, &contrast, sizeof(contrast)},
		{(int (*)(const char *, void *))camera_get_saturation, &saturation, sizeof(saturation)},
		{(int (*)(const char *, void *))camera_get_sharpness, &sharpness, sizeof(sharpness)},
		{(int (*)(const char *, void *))camera_get_hue, &hue, sizeof(hue)},
		{(int (*)(const char *, void *))camera_get_antiflicker, &antiflicker, sizeof(antiflicker)},
		{(int (*)(const char *, void *))camera_get_wb, &wb_mode, sizeof(wb_mode)},
		{(int (*)(const char *, void *))camera_get_brightnessvalue, &brightness_value, sizeof(brightness_value)},
		{(int (*)(const char *, void *))camera_get_scenario, &scen, sizeof(scen)},
		{(int (*)(const char *, void *))camera_get_exposure, &exp_comp, sizeof(exp_comp)},
		{(int (*)(const char *, void *))camera_get_ISOLimit, &iso, sizeof(iso)},
		{(int (*)(const char *, void *))camera_get_sensor_exposure, &sensor_exp, sizeof(sensor_exp)},
		{(int (*)(const char *, void *))camera_get_dpf_attr, &isp_dpf_attr, sizeof(isp_dpf_attr)},
		{(int (*)(const char *, void *))camera_get_sha_dns_attr, &isp_sha_dns_attr, sizeof(isp_sha_dns_attr)},
		{(int (*)(const char *, void *))camera_get_dns_attr, &isp_dns_attr, sizeof(isp_dns_attr)},
		{(int (*)(const char *, void *))camera_get_wb_attr, &isp_wb_attr, sizeof(isp_wb_attr)},
		{(int (*)(const char *, void *))camera_get_3d_dns_attr, &isp_3d_dns_attr, sizeof(isp_3d_dns_attr)},
		{(int (*)(const char *, void *))camera_get_yuv_gamma_attr, &isp_y_gamma_attr, sizeof(isp_y_gamma_attr)},
		{(int (*)(const char *, void *))camera_get_fps_range, &fps_range, sizeof(fps_range)},
		{NULL, NULL}
    };

#if !TEST
    if(NULL != cam) {

		monochrome_state = camera_monochrome_is_enabled(cam);
		if(-1 == monochrome_state) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		} else if(DEFAULT_CAM_FAULT == monochrome_state) {
			monochrome_state = 0;
		}
		ae_state = camera_ae_is_enabled(cam);
		if(-1 == ae_state) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}

		max_cnt = sizeof(func_list) / sizeof(func_list[0]);
		for (i = 0; i < max_cnt - 1; ++i) {
			p_item = &func_list[i];
			if (p_item && p_item->camera_func && p_item->param) {
				memset(p_item->param, 0, p_item->size);
				ret = p_item->camera_func(cam, p_item->param);
				if (ret) {
					printf("(%s:%d)...i=%d is failed !!!....\n", __FUNCTION__, __LINE__, i);
					return VB_FAIL;
				}
			}
		}

#if 0
		wdr_state = camera_wdr_is_enabled(cam);
		if(-1 == wdr_state) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
		antifog = camera_get_antifog(cam);
		if(-1 == antifog) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		} else if(DEFAULT_CAM_FAULT == antifog) {
			antifog = 0;
		}
		af_state = camera_af_is_enabled(cam);
		if(-1 == af_state) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		} else if(DEFAULT_CAM_FAULT == af_state) {
			af_state = 0;
		}
		lockstate = camera_focus_is_locked(cam);
		if(-1 == lockstate) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		} else if(DEFAULT_CAM_FAULT == lockstate) {
			lockstate = 0;
		}
		focus = camera_get_focus(cam);
		if(-1 == focus) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		} else if(DEFAULT_CAM_FAULT == focus) {
			focus = 0;
		}
		ret = camera_get_zoom(cam, &zoom);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		} else if(DEFAULT_CAM_FAULT == ret) {
			zoom.a_multiply = zoom.d_multiply = 0;
		}
#endif
    } else {
		return VB_FAIL;
    }
#else
    direction = 1;
    fps = TEST_DEFAULT_VALUE;
    focus = TEST_DEFAULT_VALUE;
    lockstate = 1;
    flip = 2;
    brightness = TEST_DEFAULT_VALUE;
    contrast = TEST_DEFAULT_VALUE;
    saturation = TEST_DEFAULT_VALUE;
    antifog = TEST_DEFAULT_VALUE;
    sharpness = TEST_DEFAULT_VALUE;
    wdr_state = 1;
    monochrome_state = 1;
    antiflicker = TEST_DEFAULT_VALUE;
    ae_state = 1;
    af_state = 1;
    wb_mode = 1;
    zoom.a_multiply = zoom.d_multiply = 100;
#endif

    printf("AE:               %s  \n", (char *)state[ae_state]);
    printf("WB mode:          %d  \n", wb_mode);
    printf("BrightnessValue:  %d  \n", brightness_value);
    printf("fps:              %d  \n", fps);
    printf("antiflicker:      %d  \n", antiflicker);
    printf("sensor mirror:    %s  \n", mirror_state[flip]);
    printf("scenario:         %s  \n", scenario_state[scen]);

    printf("exp comp(EV): %d   \n", exp_comp);
    printf("ISO        : %d   \n", iso);
    printf("brightness : %d   \n", brightness);
    printf("saturation : %d   \n", saturation);
    printf("contrast   : %d   \n", contrast);
    printf("sharpness  : %d   \n", sharpness);
    printf("hue        : %d   \n", hue);
    printf("monochrome : %s   \n", state[monochrome_state]);

    printf("sensor exposure(us) : %d   \n", sensor_exp);
    printf("isp dpf attr : en=%d mode=%d threshold=%d weight=%f   \n",
		isp_dpf_attr.cmn.mdl_en, isp_dpf_attr.cmn.mode,
		isp_dpf_attr.threshold, isp_dpf_attr.weight);

    printf("isp sha dns attr : en=%d  mode=%d strength=%f smooth=%f   \n",
		isp_sha_dns_attr.cmn.mdl_en, isp_sha_dns_attr.cmn.mode,
		isp_sha_dns_attr.strength, isp_sha_dns_attr.smooth);

    printf("isp dns attr : en=%d mode=%d is_combine_green=%d strength=%f greyscale_thr=%f\n",
		isp_dns_attr.cmn.mdl_en, isp_dns_attr.cmn.mode, isp_dns_attr.is_combine_green,
		isp_dns_attr.strength, isp_dns_attr.greyscale_threshold);
    printf("               sensor_gain=%f bitdepth=%d well_depth=%d read_noise=%f  \n",
		isp_dns_attr.sensor_gain, isp_dns_attr.sensor_bitdepth,
		isp_dns_attr.sensor_well_depth, isp_dns_attr.sensor_read_noise);

    printf("isp wb attr : en=%d  mode=%d r=%f g=%f b=%f \n",
		isp_wb_attr.cmn.mdl_en, isp_wb_attr.cmn.mode,
		isp_wb_attr.man.rgb.r, isp_wb_attr.man.rgb.g,
		isp_wb_attr.man.rgb.b);

    printf("isp 3d dns attr : en=%d  mode=%d y_thr=%d u_thr=%d v_thr=%d w=%d \n",
		isp_3d_dns_attr.cmn.mdl_en, isp_3d_dns_attr.cmn.mode,
		isp_3d_dns_attr.y_threshold, isp_3d_dns_attr.u_threshold,
		isp_3d_dns_attr.v_threshold, isp_3d_dns_attr.weight);

    printf("isp y gamma attr : en=%d  mode=%d curve: \n",
		isp_y_gamma_attr.cmn.mdl_en, isp_y_gamma_attr.cmn.mode);
    for (i = 0; i < CAM_ISP_YUV_GAMMA_COUNT; ++i) {
		if ((0 == (i % line_count))
			&& (i != 0) )
			printf("\n");
		if (0 == (i % line_count))
			printf("             ");

		sprintf(temp_str, "%f", isp_y_gamma_attr.curve[i]);
		printf("%8s  ", temp_str);
    }
    printf("\n");

    printf("fps range : min_fps=%f  max_fps=%f \n", fps_range.min_fps, fps_range.max_fps);

    return VB_OK;
}

static int set_camera_direction(const char *cam,  char *value)
{
    int direction;
    int ret = VB_OK;
    if((NULL != cam) && (NULL != value)) {
		direction = atoi(value);
#if !TEST
		ret = camera_set_direction(cam, direction);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int get_camera_brightness(const char *cam)
{
    int brightness = 0;
    int ret;

    if (NULL != cam) {
#if !TEST
		ret = camera_get_brightness(cam, &brightness);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
		printf("%s-----%s-----brightness=%d----\n", __FUNCTION__, cam, brightness);
#else
		printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_brightness(const char *cam,  char *value)
{
    int brightness;
    int ret;
    if((NULL != cam) && (NULL != value)) {
		brightness = atoi(value);
#if !TEST
		ret = camera_set_brightness(cam, brightness);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int get_camera_contrast(const char *cam)
{
    int contrast = 0;
    int ret = 0;


    if (NULL != cam) {
#if !TEST
		ret = camera_get_contrast(cam, &contrast);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
		printf("%s-----%s-----contrast=%d----\n", __FUNCTION__, cam, contrast);
#else
		printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int set_camera_contrast(const char *cam,  char *value)
{
    int contrast;
    int ret;
    if((NULL != cam) && (NULL != value)) {
		contrast = atoi(value);
#if !TEST
		ret = camera_set_contrast(cam, contrast);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int get_camera_ae(const char *cam)
{
    int ae_state = 0;
    int ret;
    if((NULL != cam)) {

#if !TEST
        ae_state = camera_ae_is_enabled(cam);
        if(-1 == ae_state) {
            CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
            return VB_FAIL;
        }

        printf("Get camera ae state = %d\n", ae_state);
#else
        printf("%s-----%s----\n", __FUNCTION__, cam);
#endif
    } else {
        return VB_FAIL;
    }
    return VB_OK;
}

static int set_camera_ae(const char *cam, char *value )
{
    int ret;
    if((NULL != cam) && (NULL != value)) {
#if !TEST
		if(0 == strcmp(value, "on")) {
			ret = camera_ae_enable(cam, 1);
		} else {
			ret = camera_ae_enable(cam, 0);
		}
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int set_camera_af(const char *cam, char *value)
{
    int ret;
    if((NULL != cam) && (NULL != value)) {
#if !TEST
		if(0 == strcmp(value, "on")) {
			ret = camera_af_enable(cam, 1);
		} else {
			ret = camera_af_enable(cam, 0);
		}
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int set_camera_wb(const char *cam, char *value)
{
    int ret = 0;

    if((NULL != cam) && (NULL != value)) {
#if !TEST

		ret = camera_set_wb(cam, atoi(value));
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int get_camera_wb(const char *cam)
{
    int ret = 0;
    int mode;
    if((NULL != cam)) {
#if !TEST

        ret = camera_get_wb(cam,(enum cam_wb_mode *) &mode);
        if(-1 == ret) {
            CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
            return VB_FAIL;
        }

        printf("Get awb mode %s\n", wb_mode[mode]);
#else
        printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    } else {
        return VB_FAIL;
    }
    return VB_OK;
}

static int set_camera_zoom(const char *cam, char *value)
{
    struct cam_zoom_info info;
    int ret;
    if((NULL != cam) && (NULL != value)){
#if !TEST
		info.a_multiply = info.d_multiply = atoi(value);
		ret = camera_set_zoom(cam, &info);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    } else {
		return VB_FAIL;
    }
    return;
}

static int set_camera_antifog(const char *cam, char *value)
{
    int antifog;
    int ret;
    if((NULL != cam) && (NULL != value)) {
		antifog = atoi(value);
#if !TEST
		ret = camera_set_antifog(cam, antifog);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int get_camera_saturation(const char *cam)
{
    int saturation = 0;
    int ret = 0;


    if (NULL != cam) {
#if !TEST
		ret = camera_get_saturation(cam, &saturation);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
		printf("%s-----%s-----saturation=%d----\n", __FUNCTION__, cam, saturation);
#else
		printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int set_camera_saturation(const char *cam, char *value)
{
    int saturation;
    int ret;
    if((NULL != cam) && (NULL != value)) {
		saturation = atoi(value);
#if !TEST
		ret = camera_set_saturation(cam, saturation);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int get_camera_sharpness(const char *cam)
{
    int sharpness = 0;
    int ret = 0;


    if (NULL != cam) {
#if !TEST
		ret = camera_get_sharpness(cam, &sharpness);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
		printf("%s-----%s-----sharpness=%d----\n", __FUNCTION__, cam, sharpness);
#else
		printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int set_camera_sharpness(const char *cam, char *value)
{
    int sharpness;
    int ret;
    if((NULL != cam) && (NULL != value)) {
		sharpness = atoi(value);
#if !TEST
		ret = camera_set_sharpness(cam, sharpness);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int get_camera_wdr(const char *cam)
{
    int wdr_state = 0;
    int ret = 0;

    if (NULL != cam) {
#if !TEST
        wdr_state = camera_wdr_is_enabled(cam);
        if(-1 == wdr_state) {
            CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
            return VB_FAIL;
        }
        printf("%s-----%s-----camera_wdr_is_enabled=%d----\n", __FUNCTION__, cam, wdr_state);
#else
        printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif
    } else {
        return VB_FAIL;
    }
    return VB_OK;
}


static int set_camera_wdr(const char *cam, char *value)
{
    int ret;
    if((NULL != cam) && (NULL != value)) {
#if !TEST
		if(0 == strcmp(value, "on")) {
			ret = camera_wdr_enable(cam, 1);
		} else {
			ret = camera_wdr_enable(cam, 0);
		}
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int get_camera_monochrome(const char *cam)
{
    int monochrome_state = 0;
    int ret;
    if((NULL != cam)) {

#if !TEST
        monochrome_state = camera_monochrome_is_enabled(cam);
        if(-1 == monochrome_state) {
            CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
            return VB_FAIL;
        }

        printf("Get camera monochrome state = %d\n", monochrome_state);
#else
        printf("%s-----%s----\n", __FUNCTION__, cam);
#endif
    } else {
        return VB_FAIL;
    }
    return VB_OK;
}


static int set_camera_monochrome(const char *cam, char *value)
{
    int ret;
    struct fr_buf_info buf = FR_BUFINITIALIZER;

    if((NULL != cam) && (NULL != value)) {
#if !TEST
		if(0 == strcmp(value, "on")) {
			ret = camera_monochrome_enable(cam, 1);

		} else {
			ret = camera_monochrome_enable(cam, 0);

		}
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int set_v4l2_snapcapture(const char *cam, char *value)
{
    int ret;
    int file_fd = -1;
    struct fr_buf_info buf = FR_BUFINITIALIZER;

    char name[128];
    int h, w;
    char *path = NULL;

    if((NULL != cam) && (NULL != value))
    {
#if !TEST
         path = strchr(value, '#');
         if (path) {
             path += 1;
             if ('/' == path[strlen(path) - 1]) {
                 path[strlen(path) - 1] = '\0';
             }
         }

        if(0 == strncmp(value, "one", 3)) {
            video_get_resolution("jpeg-in", &w, &h);
            printf("get the width = %d, height = %d\n ", w, h);
            video_set_resolution("jpeg-in", w, h);

            memset(name, '\0', sizeof(name));
            sprintf(name, "%s/%s%d%s", path, "snap_capture_mode", 0, ".jpg");
            printf("%s\n", name);

            ret = camera_v4l2_snap_one_shot("v4l2");
            if (ret < 0)
            {
                return VB_FAIL;
            }

            ret = video_get_snap("jpeg-out", &buf);
            if (ret < 0)
            {
                camera_v4l2_snap_exit("v4l2");
                return VB_FAIL;
            }

            file_fd = open(name, O_CREAT | O_TRUNC | O_WRONLY, 0666);
            if(-1 != file_fd)
            {
                ret = write(file_fd, buf.virt_addr, buf.size);
                if(ret <0)
                printf("write snapshot data to file fail, %d\n",ret );
                close(file_fd);
            }
            else
                printf("create file failed\n");

            video_put_snap("jpeg-out", &buf);
            usleep(50000);//for one frame not correct when continue snap no delay.

            ret = camera_v4l2_snap_exit("v4l2");
        }

        if(ret < 0)
        {
            CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
            return VB_FAIL;
        }
#else
        printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    } else {
        return VB_FAIL;
    }
    return VB_OK;

}

static int set_camera_snapcapture(const char *cam, char *value)
{
    int ret;
    int file_fd = -1;
    struct fr_buf_info buf = FR_BUFINITIALIZER;

    char name[128];
    int h, w;
    int o_h, o_w;
    int i = 0;
    char *path = NULL;

    if((NULL != cam) && (NULL != value))
    {
#if !TEST
		path = strchr(value, '#');
		if (path) {
			path += 1;
			if ('/' == path[strlen(path) - 1]) {
				path[strlen(path) - 1] = '\0';
			}
		}
		if(0 == strncmp(value, "one", 3))
		{
			video_get_resolution("jpeg-in", &w, &h);
			printf("get the width = %d, height = %d\n ", w, h);
			while(1)
			{
				video_get_resolution("jpeg-in", &o_w, &o_h);
				printf("get the width = %d, height = %d\n ", o_w, o_h);

				//note: q3 dual sensor not support change resolution
				// in videobox path, should be removed.
				if (i % 2 != 0)
				{
					if ((w/2)%64 == 0 && (h/2)%2 ==0)
						video_set_resolution("jpeg-in", w/2, h/2);
					else if ((w/2)%64 == 0 && (h)%2 ==0)
						video_set_resolution("jpeg-in", w/2, h);
					else if ((w)%64 == 0 && (h/2)%2 ==0)
						video_set_resolution("jpeg-in", w, h/2);
				}
				else
					video_set_resolution("jpeg-in", w, h);

				memset(name, '\0', sizeof(name));
				sprintf(name, "%s/%s_%d%s", path, "snap_one_capture", i++, ".jpg");
				printf("%s\n", name);

                ret = camera_snap_one_shot(cam);
                if (ret < 0)
                {
                    printf("isp trigger frame fail!\n");
                    break;
                }
                ret = video_get_snap("jpeg-out", &buf);
                if (ret < 0)
                {
                    camera_snap_exit(cam);
                    if (i == 5)
                    {
                       break;
                    }
                    else
                    {
                       continue;
                    }
                }
                file_fd = open(name, O_CREAT | O_TRUNC | O_WRONLY, 0666);
				if(-1 != file_fd)
				{
					ret = write(file_fd, buf.virt_addr, buf.size);
					if(ret <0)
						printf("write snapshot data to file fail, %d\n",ret );
					close(file_fd);
				}
				else
					printf("create file failed\n");

				video_put_snap("jpeg-out", &buf);
				usleep(50000);//for one frame not correct when continue snap no delay.
				ret = camera_snap_exit(cam);
				if (5 == i)
					break;
			}
		}
		else if(0 == strncmp(value, "mult", 4))
		{
			int cnt = 50;
			while(cnt)
			{
				for (i = 0; i < 5; i++)
				{
					memset(name, '\0', sizeof(name));
					ret = camera_snap_one_shot(cam);
                    if (ret < 0)
                    {
                        break;
                    }
                    ret = video_get_snap("jpeg-out", &buf);
                    if (ret < 0)
                    {
                        camera_snap_exit(cam);
                        break;
                    }
					sprintf(name, "%s/%s_%d_%d%s", path, "snap_mult_capture", i, cnt, ".jpg");
					printf("%s\n", name);
					file_fd = open(name, O_CREAT | O_TRUNC | O_WRONLY, 0666);
					if(-1 != file_fd)
					{
						ret = write(file_fd, buf.virt_addr, buf.size);
						if(ret <0)
							printf("write snapshot data to file fail, %d\n",ret );
					}
					else
						printf("write  file failed\n");

					close(file_fd);
					video_put_snap("jpeg-out", &buf);
				}
				cnt--;
			}
			ret = camera_snap_exit(cam);
		}
		else if(0 == strncmp(value, "simple", 6))
		{
			video_get_resolution("jpeg-in", &w, &h);
			printf("get the width = %d, height = %d\n ", w, h);
			video_set_resolution("jpeg-in", w, h);

			for (i = 0; i < 5; i++)
			{
				memset(name, '\0', sizeof(name));
				sprintf(name, "%s/%s%d%s", path, "snap_capture", i, ".jpg");
				printf("%s\n", name);

                ret = camera_snap_one_shot(cam);
                if (ret < 0)
                {
                    return VB_FAIL;
                }
                ret = video_get_snap("jpeg-out", &buf);
                if (ret < 0)
                {
                    camera_snap_exit(cam);
                    return VB_FAIL;
                }
                file_fd = open(name, O_CREAT | O_TRUNC | O_WRONLY, 0666);
				if(-1 != file_fd)
				{
					ret = write(file_fd, buf.virt_addr, buf.size);
					if(ret <0)
						printf("write snapshot data to file fail, %d\n",ret );
					close(file_fd);
				}
				else
					printf("create file failed\n");

				video_put_snap("jpeg-out", &buf);
				usleep(50000);//for one frame not correct when continue snap no delay.
			}

			ret = camera_snap_exit(cam);
		}
		else if(0 == strncmp(value, "mode0", 5))
		{
			video_get_resolution("jpeg-in", &w, &h);
			printf("get the width = %d, height = %d\n ", w, h);
			video_set_resolution("jpeg-in", 4096, 1152);

			memset(name, '\0', sizeof(name));
			sprintf(name, "%s/%s%d%s", path, "snap_capture_mode", 0, ".jpg");
			printf("%s\n", name);

            ret = camera_snap_one_shot(cam);
            if (ret < 0)
            {
                return VB_FAIL;
            }
            ret = video_get_snap("jpeg-out", &buf);
            if (ret < 0)
            {
                camera_snap_exit(cam);
                return VB_FAIL;
            }
            file_fd = open(name, O_CREAT | O_TRUNC | O_WRONLY, 0666);
			if(-1 != file_fd)
			{
				ret = write(file_fd, buf.virt_addr, buf.size);
				if(ret <0)
					printf("write snapshot data to file fail, %d\n",ret );
				close(file_fd);
			}
			else
				printf("create file failed\n");

			video_put_snap("jpeg-out", &buf);
			usleep(50000);//for one frame not correct when continue snap no delay.

			ret = camera_snap_exit(cam);
		}
		else if(0 == strncmp(value, "mode1", 5))
		{
			video_get_resolution("jpeg-in", &w, &h);
			printf("get the width = %d, height = %d\n ", w, h);
			video_set_resolution("jpeg-in", 2560, 720);

			memset(name, '\0', sizeof(name));
			sprintf(name, "%s/%s%d%s", path, "snap_capture_mode", 1, ".jpg");
			printf("%s\n", name);

            ret = camera_snap_one_shot(cam);
            if (ret < 0)
            {
                return VB_FAIL;
            }
            ret = video_get_snap("jpeg-out", &buf);
            if (ret < 0)
            {
                camera_snap_exit(cam);
                return VB_FAIL;
            }

            file_fd = open(name, O_CREAT | O_TRUNC | O_WRONLY, 0666);
			if(-1 != file_fd)
			{
				ret = write(file_fd, buf.virt_addr, buf.size);
				if(ret <0)
					printf("write snapshot data to file fail, %d\n",ret );
				close(file_fd);
			}
			else
				printf("create file failed\n");

			video_put_snap("jpeg-out", &buf);
			usleep(50000);//for one frame not correct when continue snap no delay.

			ret = camera_snap_exit(cam);
		}
		else if(0 == strncmp(value, "mode2", 5))
		{
			video_get_resolution("jpeg-in", &w, &h);
			printf("get the width = %d, height = %d\n ", w, h);
			video_set_resolution("jpeg-in", 3840, 1920);

			memset(name, '\0', sizeof(name));
			sprintf(name, "%s/%s%d%s", path, "snap_capture_mode", 2, ".jpg");
			printf("%s\n", name);

            ret = camera_snap_one_shot(cam);
            if (ret < 0)
            {
                return VB_FAIL;
            }
            ret = video_get_snap("jpeg-out", &buf);

            if (ret < 0)
            {
                camera_snap_exit(cam);
                return VB_FAIL;
            }
            file_fd = open(name, O_CREAT | O_TRUNC | O_WRONLY, 0666);

			if(-1 != file_fd)
			{
				ret = write(file_fd, buf.virt_addr, buf.size);
				if(ret <0)
					printf("write snapshot data to file fail, %d\n",ret );
				close(file_fd);
			}
			else
				printf("create file failed\n");

			video_put_snap("jpeg-out", &buf);
			usleep(50000);//for one frame not correct when continue snap no delay.

			ret = camera_snap_exit(cam);
		}

		else if(0 == strncmp(value, "mode3", 5))
		{
			video_get_resolution("jpeg-in", &w, &h);
			printf("get current width = %d, height = %d\n ", w, h);
			while(1)
			{
				video_get_resolution("jpeg-in", &o_w, &o_h);
				printf("get current width = %d, height = %d\n ", o_w, o_h);

				if (i % 3 == 0)
					video_set_resolution("jpeg-in", 2560, 720);
				else if (i%3 == 1)
					video_set_resolution("jpeg-in", 3840, 1920);
				else
					video_set_resolution("jpeg-in", 4096, 1152);

				memset(name, '\0', sizeof(name));
				sprintf(name, "%s/%s_%d%s", path, "snap_capture_mode3", i++, ".jpg");
				printf("%s\n", name);


                ret = camera_snap_one_shot(cam);
                if (ret < 0)
                {
                    break;
                }
                ret = video_get_snap("jpeg-out", &buf);
                if (ret < 0)
                {
                    camera_snap_exit(cam);
                    if (i == 3)
                    {
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }
                file_fd = open(name, O_CREAT | O_TRUNC | O_WRONLY, 0666);
				if(-1 != file_fd)
				{
					ret = write(file_fd, buf.virt_addr, buf.size);
					if(ret <0)
						printf("write snapshot data to file fail, %d\n",ret );
					close(file_fd);
				}
				else
					printf("create file failed\n");

				video_put_snap("jpeg-out", &buf);
				usleep(50000);//for one frame not correct when continue snap no delay.
				ret = camera_snap_exit(cam);
				if (3 == i)
					break;
			}
		}

        if(ret < 0)
		{
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    }
    else
    {
		return VB_FAIL;
    }
    return VB_OK;
}

static int set_camera_antiflicker(const char *cam, char *value)
{
 	int antiflicker;
 	int ret;
    if((NULL != cam) && (NULL != value)) {
		antiflicker = atoi(value);
#if !TEST
		ret = camera_set_antiflicker(cam, antiflicker);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int get_camera_antiflicker(const char *cam)
{
    int antiflicker;
    int ret;
    if((NULL != cam)) {

#if !TEST
        ret = camera_get_antiflicker(cam, &antiflicker);
        if(-1 == ret) {
            CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
            return VB_FAIL;
        }

        printf("Get camera antiflicker = %d\n", antiflicker);
#else
        printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    } else {
        return VB_FAIL;
    }
    return VB_OK;
}

static int set_camera_lc(const char *cam, char *value)
{
    int ret;
    if((NULL == cam) || (NULL == value)) {
		return VB_FAIL;
    }
    if((!strcmp(value, "R")) || (!strcmp(value, "r")) || (!strcmp(value, "reset"))) {
		ret = camera_reset_lens(cam);
		if(-1 == ret){
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
    }
    return VB_OK;
}

static int set_camera_lock(const char *cam, char *value)
{
    int ret;
    if((NULL != cam) && (NULL != value)) {
#if !TEST
	 	if(0 == (strcmp(value, "on"))) {
	 		ret = camera_focus_lock(cam, 1);
	 	} else {
			ret = camera_focus_lock(cam, 0);
		}
		if(-1 == ret) {
		 	CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int set_camera_fps(const char *cam, char *value)
{
    int fps = 0;
    int ret = 0;;

    if((NULL != cam) && (NULL != value)) {
		fps = atoi(value);
#if !TEST
		ret = camera_set_fps(cam, fps);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int set_camera_focus(const char *cam, char *value)
{
    int focus;
    int ret;
    if((NULL != cam) && (NULL != value)) {
		focus = atoi(value);
#if !TEST
		ret = camera_set_focus(cam, focus);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int set_camera_hue(const char *cam,  char *value)
{
    int hue = 0;
    int ret = 0;


    if ((NULL != cam) && (NULL != value)) {
		hue = atoi(value);
#if !TEST
		ret = camera_set_hue(cam, hue);
		if (ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int get_camera_hue(const char *cam)
{
    int hue = 0;
    int ret = 0;


    if (NULL != cam) {
#if !TEST
		ret = camera_get_hue(cam, &hue);
		if (ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
		printf("%s:%s, hue=%d\n", __FUNCTION__, cam, hue);
#else
		printf("%s-----%s\n", __FUNCTION__, cam);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_gamcurve(const char *cam,  char *value)
{
    int curve = 0;
    int ret = 0;
    int i = 0;
    cam_gamcurve_t gamcurve;
    memset(&gamcurve, 0, sizeof(gamcurve));

    if ((NULL != cam) && (NULL != value)) {
        curve = atoi(value);
#if !TEST
{
    uint16_t curve1[63] =
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1
    };

    uint16_t curve2[63] =
    {
        600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
        600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
        600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
        600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
        600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
        600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
        600, 600, 600
    };

    uint16_t curve3[63] =
    {
          0,     8,   94,  132,  174,  226,  281,  354,  428,  478,
        525,   592,  652,  708,  760,  809,	 855,  898,  938,  976,
        1013, 1049, 1083, 1139, 1191, 1240, 1286, 1330, 1373, 1413,
        1452, 1490, 1527, 1563, 1598, 1628, 1657, 1687, 1715, 1744,
        1772, 1801, 1829, 1883, 1937, 1992, 2048, 2112, 2176, 2240,
        2304, 2368, 2432, 2496, 2560, 2624, 2688, 2752, 2816, 2880,
        2944, 3008, 3072
    };

    if (curve == 1) {
        memcpy(gamcurve.curve, curve1, sizeof(curve1));
        camera_set_gamcurve(cam, &gamcurve);
    } else if (curve == 2){
        memcpy(gamcurve.curve, curve2, sizeof(curve2));
        camera_set_gamcurve(cam, &gamcurve);
    } else if (curve == 3) {
        memcpy(gamcurve.curve, curve3, sizeof(curve3));
        camera_set_gamcurve(cam, &gamcurve);
    } else {
        printf("set_camera_gamcurve set value error\n");
    }

    printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
}
#else
        printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif
    } else {
        return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_mirror(const char *cam,  char *value)
{
    enum cam_mirror_state st = 0;
    int ret = 0;


    if ((NULL != cam) && (NULL != value)) {
		st = atoi(value);
#if !TEST
		ret = camera_set_mirror(cam, st);
		if (ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int get_camera_mirror(const char *cam)
{
    enum cam_mirror_state st = 0;
    int ret = 0;


    if ((NULL != cam)) {
#if !TEST
        ret = camera_get_mirror(cam, &st);
        if (ret) {
            CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
            return VB_FAIL;
        }

        printf("Get camera flip&mirror status:%s\n", mirror_state[st]);
#else
        printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
        return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_scenario(const char *cam,  char *value)
{
    enum cam_scenario scen = 0;
    int ret = 0;


    if ((NULL != cam) && (NULL != value)) {
		scen = atoi(value);
#if !TEST
		ret = camera_set_scenario(cam, scen);
		if (-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int get_camera_scenario(const char *cam)
{
    enum cam_scenario scen = 0;
    int ret = 0;


    if ((NULL != cam)) {

#if !TEST
        ret = camera_get_scenario(cam, &scen);
        if (-1 == ret) {
            CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
            return VB_FAIL;
        }
        printf("Get scenario %s \n", scenario_state[scen]);
#else
        printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
        return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_exposure(const char *cam,  char *value)
{
    int exp = 0;
    int ret = 0;


    if ((NULL != cam) && (NULL != value)) {
		exp = atoi(value);
#if !TEST
		ret = camera_set_exposure(cam, exp);
		if (-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int get_camera_exposure(const char *cam)
{
    int exp = 0;
    int ret = 0;


    if ((NULL != cam)) {

#if !TEST
        ret = camera_get_exposure(cam, &exp);
        if (-1 == ret) {
            CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
            return VB_FAIL;
        }
        printf("Get exposure level %d\n", exp);
#else
        printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
        return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_iso(const char *cam,  char *value)
{
    int iso = 0;
    int ret = 0;


    if ((NULL != cam) && (NULL != value)) {
		iso = atoi(value);
#if !TEST
		ret = camera_set_ISOLimit(cam, iso);
		if (-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int get_camera_iso(const char *cam)
{
    int iso = 0;
    int ret = 0;


    if ((NULL != cam)) {

#if !TEST
        ret = camera_get_ISOLimit(cam, &iso);
        if (-1 == ret) {
            CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
            return VB_FAIL;
        }
        printf("Get camera iso : %d\n", iso);
#else
        printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
        return VB_FAIL;
    }

    return VB_OK;
}

//video command functions
static int get_video_list()
{
    int i = 0;
#if !TEST
    char **list = NULL;
    list = (char **)video_get_channels();
#else
    char *list[] = {
		"stream-480p",
		"stream-720p",
		"stream-1080p",
		NULL,
    };
#endif
    if(NULL != list) {
		while(list[i]) {
			printf("%s  \n", list[i]);
			i++;
		}
    } else {
		printf("video_get_channels return NULL....\n");
		return VB_FAIL;
    }
    return VB_OK;
}

static int get_video_info(const char *channel)
{
    struct v_bitrate_info bri;
    struct v_basic_info basic;
    struct v_roi_info roi_info;
    int w,h;
    int fps;
    int ret;
    int roi_count;
    int i = 0;
    if(NULL != channel) {
#if !TEST
		ret = video_get_bitrate(channel, &bri);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		} else if(DEFAULT_VIDEO_FAULT == ret){
			memset(&bri, 0, sizeof(struct v_bitrate_info));
		}
		ret = video_get_basicinfo(channel, &basic);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		} else if(DEFAULT_VIDEO_FAULT == ret){
			memset(&basic, 0, sizeof(struct v_basic_info));
		}
		roi_count = video_get_roi_count(channel);
		if(-1 == roi_count) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		} else if(DEFAULT_VIDEO_FAULT == roi_count) {
			roi_count = 0;
		}
		ret = video_get_roi(channel, &roi_info);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		} else if(DEFAULT_VIDEO_FAULT == ret) {
			memset(&roi_info, 0, sizeof(struct v_roi_info));
		}
		fps = video_get_fps(channel);
		if(-1 == fps) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		} else if(DEFAULT_VIDEO_FAULT == fps) {
			fps = 0;
		}
		ret = video_get_resolution(channel, &w, &h);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		} else if(DEFAULT_VIDEO_FAULT == ret) {
			w = 0;
			h = 0;
		}
#else
    w = 1920;
    h = 1080;
    fps = 60;
    bri.bitrate = 10;
    bri.qp_max = 60;
    bri.qp_min = 40;
    bri.gop_length = 44;
    basic.profile = VENC_MAIN;
    basic.stream = VENC_BYTESTREAM;
    roi_count = 2;
    roi_info.roi[0].x = roi_info.roi[1].x = 0;
    roi_info.roi[0].y = roi_info.roi[1].y = 0;
    roi_info.roi[0].w = roi_info.roi[1].w = 1920;
    roi_info.roi[0].h = roi_info.roi[1].h = 1080;
    roi_info.roi[0].qp_max = roi_info.roi[1].qp_max = 60;
    roi_info.roi[0].qp_min = roi_info.roi[1].qp_min = 40;
#endif
    } else {
		return VB_FAIL;
    }

    printf("video info:    \n");
    printf(" %d * %d  @%dfps enc_profile: %s     \n", w, h, fps, profile_name[basic.profile]);
    printf("stream_type:%s   \n", stream_type[basic.stream_type]);
    printf("bitrate:   \n");
    printf("\t%d Mbs  QP: %d~%d  GOP: %d    \n", bri.bitrate, bri.qp_min, bri.qp_max, bri.gop_length);
    printf("\tQP_delta:%d   QP_hdr:%d   picture_skip:%d   \n", bri.qp_delta, bri.qp_hdr, bri.picture_skip);
    printf("ROI info:    \n");
    printf("\tcount: %d \n", roi_count);
    for(i = 0; i < roi_count; i++) {
	    printf("\t %d   \n", i);
	    printf("\t\tenable:%d \n", roi_info.roi[i].enable);
		printf("\t\tx:%d \n", roi_info.roi[i].x);
		printf("\t\ty:%d \n", roi_info.roi[i].y);
		printf("\t\tw:%d \n", roi_info.roi[i].w);
		printf("\t\th:%d \n", roi_info.roi[i].h);
		printf("\t\tqp_offset:%d \n", roi_info.roi[i].qp_offset);
    }

    return VB_OK;
}
static void dump_video_bitrate_info(struct v_bitrate_info *bri){
    printf("bitrate info ->\n");
    printf("\trc_mode ->%d\n", bri->rc_mode);
    printf("\tbitrate ->%d\n", bri->bitrate);
    printf("\tqp_max ->%d\n", bri->qp_max);
    printf("\tqp_min ->%d\n", bri->qp_min);
    printf("\tqp_delta ->%d\n", bri->qp_delta);
    printf("\tqp_hdr ->%d\n", bri->qp_hdr);
    printf("\tgop_length ->%d\n", bri->gop_length);
    printf("\tpicture_skip ->%d\n", bri->picture_skip);
    printf("\tp_frame ->%d\n", bri->p_frame);
}
static int set_video_bitrate(const char *channel, char *value)
{
/*
struct v_bitrate_info {
    enum v_rate_ctrl_type rc_mode; //rc_mode = 1(VBR Mode) rc_mode = 0(CBR Mode)
    int bitrate;
    int qp_max;
    int qp_min;
    int qp_delta; //will affect i frame size
    int qp_hdr;   //when rc_mode is 0,affect first I frame qp,when rc_mode is 1,affect whole frame qp
    int gop_length;
    int picture_skip;
    int p_frame; //I/P frame interval
};
*/
    struct v_bitrate_info bri ;
    int ret;
    assert(NULL != channel);
    assert(NULL != value);
    printf("value format ->\n");
    printf("\t  vbr/cbr:bitrate:qp_max:qp_min:qp_delta:qp_hdr:gop_length:picture_skip:p_frame\n");
    printf("\t  for example: b:12000:50:20:0:-1:30:0:30\n");
    char *p = value;
    if(*p == 'v'){
        printf("vbr mode\n");
        bri.rc_mode = 1;
    }
    else if(*p == 'c'){
        printf("cbr mode\n");
        bri.rc_mode = 0;
    }
    else{
        printf("wrong rc mode \n");
        return VB_FAIL;
    }
    p++;
    if(*p != ':'){
        printf("wrong split char ,please use :");
        return VB_FAIL;
    }
    p++;
    bri.bitrate =  atoi(p);
    printf("bit rate ->%d\n", bri.bitrate);
    //get qp_max
    p = strstr(p, ":");
    if( p == NULL){
        printf("parse qp max error->%s\n", p);
        return VB_FAIL;
    }
    p++;
    bri.qp_max = atoi(p);
    printf("qp_max ->%d\n", bri.qp_max);
    //get qp_min
    p = strstr(p, ":");
    if( p == NULL){
        printf("parse qp min error->%s\n", p);
        return VB_FAIL;
    }
    p++;
    bri.qp_min = atoi(p);
    printf("qp_min ->%d\n", bri.qp_min);
    //get qp_delta
    p = strstr(p, ":");
    if( p == NULL){
        printf("parse qp delta error->%s\n", p);
        return VB_FAIL;
    }
    p++;
    bri.qp_delta = atoi(p);
    printf("qp_delta ->%d\n", bri.qp_delta);
    //get qp_hdr
    p = strstr(p, ":");
    if( p == NULL){
        printf("parse qp hdr error->%s\n", p);
        return VB_FAIL;
    }
    p++;
    bri.qp_hdr = atoi(p);
    printf("qp_hdr ->%d\n", bri.qp_hdr);
    //get gop_length
    p = strstr(p, ":");
    if( p == NULL){
        printf("parse gop_length error->%s\n", p);
        return VB_FAIL;
    }
    p++;
    bri.gop_length = atoi(p);
    printf("gop_length ->%d\n", bri.gop_length);
    //get picture_skip
    p = strstr(p, ":");
    if( p == NULL){
        printf("parse picture_skip error->%s\n", p);
        return VB_FAIL;
    }
    p++;
    bri.picture_skip = atoi(p);
    printf("picture_skip ->%d\n", bri.picture_skip);
    //get p_frame
    p = strstr(p, ":");
    if( p == NULL){
        printf("parse p_frame error->%s\n", p);
        return VB_FAIL;
    }
    p++;
    bri.p_frame = atoi(p);
    printf("p_frame ->%d\n", bri.p_frame);
    printf("set video bitrate ->\n");

    dump_video_bitrate_info(&bri);
    ret = video_set_bitrate(channel, &bri);
    CTRLVB_DBG("%s-----%s-----%d----\n", __FUNCTION__, channel, bri.bitrate);
    if(-1 == ret) {
        CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
        return VB_FAIL;
    }
    CTRLVB_DBG("%s.......%d.......set birtate success\n", __FUNCTION__, __LINE__);
    ret = video_get_bitrate(channel, &bri);
    printf("get video bitrate ->\n");
    dump_video_bitrate_info(&bri);
    return VB_OK;
}
static int get_video_bitrate(const char *channel)
{
    int ret = 0;
    struct v_bitrate_info bri;
    assert(channel != NULL);
    printf("get %s bitrate\n", channel);
    ret = video_get_bitrate(channel, &bri);
    if(ret < 0 ){
        printf("get video bitrate error\n");
        return VB_FAIL;
    }
    printf("get video bitrate ->\n");
    dump_video_bitrate_info(&bri);
    return VB_OK;

}

static int set_video_roi(const char *channel, char *value)
{
    char *p = NULL;
    char *str = NULL;
    char *str_temp = NULL;
    struct v_roi_info roi_info;
    int i = 0;
    int ret = 0;
    if(channel == NULL || value == NULL)
        return VB_FAIL;
    for(i = 0, str = value; ;i++, str = NULL) {
        p = strtok_r(str, ":", &str_temp);
        if(p == NULL)
            break;
        *(((int *)&roi_info) + i) = atoi(p);
    }
    if(i != 12) {
        printf("Please check your command \n");
        printf("ie: video -sc name --roi");
        printf("roi0_enable:roi0_x:roi0_y:roi0_w:roi0_h:roi0_qp:");
        printf("roi1_enable:roi1_x:roi1_y:roi1_w:roi1_h:roi1_qp\n");
    }

    ret = video_set_roi(channel, &roi_info);
    if(ret < 0) {
        return VB_FAIL;
    }
    return VB_OK;

}

static int get_video_roi(const char *channel)
{
    int ret = 0;
    struct v_roi_info roi_info;
    if(channel == NULL)
        return VB_FAIL;
    ret = video_get_roi(channel, &roi_info);
    if(ret < 0) {
        printf("get video roi info error\n");
        return VB_FAIL;
    }
    printf("roi0 en: \t%d\n", roi_info.roi[0].enable);
    printf("roi0 x: \t%d\n", roi_info.roi[0].x);
    printf("roi0 y: \t%d\n", roi_info.roi[0].y);
    printf("roi0 w: \t%d\n", roi_info.roi[0].w);
    printf("roi0 h: \t%d\n", roi_info.roi[0].h);
    printf("roi0 qp: \t%d\n", roi_info.roi[0].qp_offset);
    printf("roi1 en: \t%d\n", roi_info.roi[1].enable);
    printf("roi1 x: \t%d\n", roi_info.roi[1].x);
    printf("roi1 y: \t%d\n", roi_info.roi[1].y);
    printf("roi1 w: \t%d\n", roi_info.roi[1].w);
    printf("roi1 h: \t%d\n", roi_info.roi[1].h);
    printf("roi1 qp: \t%d\n", roi_info.roi[1].qp_offset);
    return VB_OK;
}

static int set_video_ratectrl(const char *channel, char *value)
{
    char *p = NULL;
    char *str = NULL;
    char *str_temp = NULL;
    struct v_rate_ctrl_info rate_ctrl;
    struct v_rate_ctrl_info *prc;
    int rc_mode = 0;
    int i = 0;
    int ret = 0;
    prc = &rate_ctrl;
    prc = (struct v_rate_ctrl_info *)((int)prc +32);

    if(channel == NULL || value == NULL)
        return VB_FAIL;
    rate_ctrl.rc_mode = VENC_VBR_MODE;
    for(i = 0, str = value; ;i++, str = NULL) {
        p = strtok_r(str, ":", &str_temp);
        if(p == NULL)
            break;
        *(((int *)prc) + i) = atoi(p);
        if(!i)
            rc_mode = atoi(p);
    }
    switch (rc_mode) {
        case VENC_CBR_MODE:
            if(i != 14) {
                printf("Please check your command \n");
                printf("ie: video -sc name -r ");
                printf("rc_mode:gop_length:pic_skip:idr_interval:hrd:hrd_cpbsize:refresh_interval:");
                printf("mbrc:mb_qp_adjustment:qp_max:qp_min:bitrate:qp_delta:qp_hdr\n");
                return VB_FAIL;
            }
            break;
        case VENC_VBR_MODE:
            if(i != 14) {
                printf("Please check your command \n");
                printf("ie: video -sc name -r ");
                printf("rc_mode:gop_length:pic_skip:idr_interval:hrd:hrd_cpbsize:refresh_interval:");
                printf("mbrc:mb_qp_adjustment:qp_max:qp_min:max_bitrate:threshold:qp_delta\n");
                return VB_FAIL;
            }
            break;
        case VENC_FIXQP_MODE:
            if(i != 10) {
                printf("Please check your command \n");
                printf("ie: video -sc name -r ");
                printf("rc_mode:gop_length:pic_skip:idr_interval:hrd:hrd_cpbsize:refresh_interval:");
                printf("mbrc:mb_qp_adjustment:qp_fix\n");
                return VB_FAIL;
            }
            break;
        default:
            printf("Wrong rc_mode, please check your command \n");
            printf("ie: video -sc name -r ");
            printf("rc_mode:gop_length:pic_skip:idr_interval:hrd:hrd_cpbsize:refresh_interval:");
            printf("mbrc:mb_qp_adjustment:qp_max:qp_min:bitrate:qp_delta:qp_hdr\n");
            return VB_FAIL;
            break;
    }
    ret = video_set_ratectrl(channel, &rate_ctrl);
    if(ret < 0) {
        return VB_FAIL;
    }
    return VB_OK;
}

static int get_video_ratectrl(const char *channel)
{
    int ret = 0;
    struct v_rate_ctrl_info rate_ctrl;
    if(channel == NULL)
        return VB_FAIL;
    ret = video_get_ratectrl(channel, &rate_ctrl);
    if(ret < 0) {
        printf("get video vbr info error\n");
        return VB_FAIL;
    }
    printf("videobox ratctrl info:\n");
    printf("\tgop_length:\t\t%d\n", rate_ctrl.gop_length);
    printf("\tpicture_skip:\t\t%d\n", rate_ctrl.picture_skip);
    printf("\tidr_internal:\t\t%d\n", rate_ctrl.idr_interval);
    printf("\thrd:\t\t\t%d\n", rate_ctrl.hrd);
    printf("\thrd_cpbsize:\t\t%d\n", rate_ctrl.hrd_cpbsize);
    printf("\trefresh_interval:\t%d\n", rate_ctrl.refresh_interval);
    printf("\tmbrc: \t\t\t%d\n", rate_ctrl.mbrc);
    printf("\tmbQpAjustment: \t\t%d\n", rate_ctrl.mb_qp_adjustment);
    switch (rate_ctrl.rc_mode) {
        case VENC_CBR_MODE:
            printf("\trc_mode:\t\tCBR mode\n");
            printf("\tbitrate:\t\t%d\n", rate_ctrl.cbr.bitrate);
            printf("\tqp_max:\t\t\t%d\n", rate_ctrl.cbr.qp_max);
            printf("\tqp_min:\t\t\t%d\n", rate_ctrl.cbr.qp_min);
            printf("\tqp_delta:\t\t%d\n", rate_ctrl.cbr.qp_delta);
            printf("\tqp_hdr:\t\t\t%d\n", rate_ctrl.cbr.qp_hdr);
            break;
        case VENC_VBR_MODE:
            printf("\trc_mode:\t\tVBR mode\n");
            printf("\tqp_max:\t\t\t%d\n", rate_ctrl.vbr.qp_max);
            printf("\tqp_min:\t\t\t%d\n", rate_ctrl.vbr.qp_min);
            printf("\tqp_delta:\t\t\t%d\n", rate_ctrl.vbr.qp_delta);

            printf("\tmax_bitrate:\t\t%d\n", rate_ctrl.vbr.max_bitrate);
            printf("\tthreshold:\t\t%d\n", rate_ctrl.vbr.threshold);
            break;
        case VENC_FIXQP_MODE:
            printf("\trc_mode:\t\tFIXQP mode\n");
            printf("\tqp_fix:\t\t\t%d\n", rate_ctrl.fixqp.qp_fix);
            break;
        default:
            break;
    }
    return VB_OK;
}

static int set_video_pframe(const char *channel, char *value)
{
    struct v_basic_info basic;
    int ret;
    if((NULL != channel) && (NULL != value)) {
#if !TEST
		ret = video_get_basicinfo(channel, &basic);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
		CTRLVB_DBG("%s-----%s-----%d----\n", __FUNCTION__, channel);
		ret = video_set_basicinfo(channel, &basic);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, channel, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int set_video_streamtype(const char *channel, char *value)
{
    struct v_basic_info basic;
    int ret;
    if((NULL != channel) && (NULL != value)) {
#if !TEST
		ret = video_get_basicinfo(channel, &basic);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
		basic.stream_type = atoi(value);
		CTRLVB_DBG("%s-----%s-----%d----\n", __FUNCTION__, channel);
		ret = video_set_basicinfo(channel, &basic);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, channel, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int set_video_qp_max(const char *channel, char *value)
{
    struct v_bitrate_info bri;
    int ret;
    if((NULL != channel) && (NULL != value)) {
#if !TEST
		ret = video_get_bitrate(channel, &bri);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
		bri.qp_max = atoi(value);
		CTRLVB_DBG("%s-----%s-----%d----\n", __FUNCTION__, channel, bri.qp_max);
		ret = video_set_bitrate(channel, &bri);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, channel, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int set_video_qp_min(const char *channel, char *value)
{
    struct v_bitrate_info bri;
    int ret;
    if((NULL != channel) && (NULL != value)) {
#if !TEST
		ret = video_get_bitrate(channel, &bri);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
		bri.qp_min = atoi(value);
		CTRLVB_DBG("%s-----%s-----%d----\n", __FUNCTION__, channel, bri.qp_min);
		ret = video_set_bitrate(channel, &bri);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, channel, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int set_video_qp_hdr(const char *channel, char *value)
{
    struct v_bitrate_info bri;
    int ret;
    if((NULL != channel) && (NULL != value)) {
#if !TEST
		ret = video_get_bitrate(channel, &bri);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
		bri.qp_hdr = atoi(value);
		CTRLVB_DBG("%s-----%s-----%d----\n", __FUNCTION__, channel, bri.qp_hdr);
		ret = video_set_bitrate(channel, &bri);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, channel, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int set_video_gop_length(const char *channel, char *value)
{
    struct v_bitrate_info bri;
    int ret;
    if((NULL != channel) && (NULL != value)) {
#if !TEST
		ret = video_get_bitrate(channel, &bri);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
		bri.gop_length = atoi(value);
		CTRLVB_DBG("%s-----%s-----%d----\n", __FUNCTION__, channel, bri.gop_length);
		ret = video_set_bitrate(channel, &bri);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, channel, value);
#endif
    } else {
		return VB_FAIL;
    }
    return VB_OK;
}

static int set_video_profile(const char *channel, char *value)
{
    struct v_basic_info basic;
    int ret;
    if((NULL != channel) && (NULL != value)) {
#if !TEST
		ret = video_get_basicinfo(channel, &basic);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
		basic.profile = atoi(value);
		CTRLVB_DBG("%s-----%s-----%d----\n", __FUNCTION__, channel, basic.profile);
		ret = video_set_basicinfo(channel, &basic);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, channel, value);
#endif
    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int set_video_framerate(const char *channel, char *value)
{
    int fps;
    int ret;
    if((NULL != channel) && (NULL != value)) {
#if !TEST
		fps = atoi(value);
		CTRLVB_DBG("%s-----%s-----%d----\n", __FUNCTION__, channel, fps);
		ret = video_set_fps(channel, fps);
		if(-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, channel, value);
#endif
    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

uint64_t  get_timestamp(void){
    struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return  ts.tv_sec * 1000ull + ts.tv_nsec / 1000000;
}
static int video_capture(const char *channel, char *path)
{
    int ret;
    char *buf = NULL;
    int file_fd = -1;
    long total_size = 0;
    int size = 0 ;

    struct fr_buf_info cap_frame = FR_BUFINITIALIZER;
#if !TEST
    buf = malloc(HEADER_LENGTH);
    if(NULL != buf) {
		size = video_get_header(channel, buf, HEADER_LENGTH);
    }
    if(NULL != path) {
		file_fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    } else {
		file_fd = open("vbctrl_frames", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    }


    if(-1 == file_fd) {
		printf("create %s/vbctrl_frames file failed  \n", path);
		return;
    }
    ret = write(file_fd, buf, size); //将其写入文件中
    if(ret <0){
		printf("write snapshot data to file fail, %d\n",ret );
    }

    total_size =  0;
    int counter = 0 ;
    int average  =  0 ;
    int fps = video_get_fps(channel);
    printf("video fps ->%d\n",fps);
    struct v_frc_info frc;
    ret = video_get_frc(channel, &frc);
    printf("video frc ->%2d:%2d   %d\n", frc.framerate, frc.framebase, ret);
    uint64_t new ,old ;
    old = get_timestamp();
    int firstFrame =  0;
    while(!quit) {
        if(video_test_frame(channel, &cap_frame) <= 0) {
            continue;
        }
		if(video_get_frame(channel, &cap_frame) < 0 ){
			printf("get frame error\n");
		}
#if 1
		else {
			if(firstFrame == 0 ){
				if(cap_frame.priv != VIDEO_FRAME_I ){
					video_put_frame(channel , &cap_frame );
					continue ;
				}
				else {
					firstFrame = 1 ;
				}
			}
			ret = write(file_fd, cap_frame.virt_addr, cap_frame.size); //将其写入文件中
			if(ret <0) {
				printf("write capture data to file fail, %d\n",ret );
				break;
			}
		}
#endif
		total_size += cap_frame.size;
		counter++;
		new= get_timestamp();
		int check_interval = 10;
		if(( (new - old )/1000) >= check_interval){
			printf("\n++++++++++video average rate->%4dkbps  %4d  %d\n\n",total_size*8/check_interval/1024,total_size/counter,counter/check_interval);
			counter =  0 ;
			total_size =  0 ;
			old= new ;
		}
		video_put_frame(channel, &cap_frame);
		if(cap_frame.priv == VIDEO_FRAME_I){
			printf("Capture is running, get size is I : %4d(K).6%d %8lld serial:%d\n", cap_frame.size/1024, cap_frame.size%1024, cap_frame.timestamp, cap_frame.serial);
		}
		else if(cap_frame.priv == VIDEO_FRAME_P){
			printf("Capture is running, get size is P : %4d(K).6%d %8lld serial:%d\n", cap_frame.size/1024, cap_frame.size%1024, cap_frame.timestamp, cap_frame.serial);
		}
		else {
			printf("Capture is running, get size is ? : %4d(K).6%d serial:%d\n", cap_frame.size/1024, cap_frame.size%1024, cap_frame.timestamp, cap_frame.serial);
		}
		fflush(stdout);
	}
//	video_get_tail(channel, buf, HEADER_LENGTH);
    ret = write(file_fd, buf, HEADER_LENGTH); //将其写入文件中
    if(ret <0) {
		printf("write capture data to file fail, %d\n",ret );
    }
    free(buf);
    buf = NULL;
    close(file_fd);
#else
    printf("%s-----%s-----%s----\n", __FUNCTION__, channel, path);
#endif
}

static int video_snapshot(const char *channel, char *path)
{
    int file_fd = -1;
    int ret;
    struct fr_buf_info buf = FR_BUFINITIALIZER;
#if !TEST
    ret = video_get_snap(channel, &buf);

    if (ret < 0)
    {
        printf("Take picture fail! ret:%d\n", ret);
        return ret;
    }

    if(NULL != path) {
		file_fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    } else {
		file_fd = open("vbctrl_snap.jpg", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    }

    if(-1 != file_fd) {
		ret = write(file_fd, buf.virt_addr, buf.size);
		if(ret <0) {
			printf("write snapshot data to file fail, %d\n",ret );
		}
		close(file_fd);
    } else {
		printf("create %s/vbctrl_snap.jpg file failed  \n", path);
    }

    video_put_snap(channel, &buf);
#else
    printf("%s-----%s-----%s----\n", __FUNCTION__, channel, path);
#endif
    return ret;
}

static int video_thumbnail(const char *channel, char *path)
{
    int file_fd = -1;
    int ret;
    struct fr_buf_info buf = FR_BUFINITIALIZER;
#if !TEST
    ret = video_get_thumbnail(channel, &buf);
    if(ret < 0) {
        printf("err:video get thumbnail fail ret:%d!\n", ret);
        return ret;
    }
    if(NULL != path) {
        file_fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    } else {
        file_fd = open("vbctrl_snap.jpg", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    }

    if(-1 != file_fd) {
        ret = write(file_fd, buf.virt_addr, buf.size); //将其写入文件中
        if(ret <0) {
            printf("write snapshot data to file fail, %d\n",ret );
        }
        close(file_fd);
    } else {
        printf("create %s/vbctrl_snap.jpg file failed  \n", path);
    }
    video_put_thumbnail(channel, &buf);
#else
    printf("%s-----%s-----%s----\n", __FUNCTION__, channel, path);
#endif
}

static int video_set_thumb_resolution(const char *channel, int widht, int height) {
    int ret = 0;
    ret = thumbnail_set_resolution(channel, widht, height);
    return ret;
}

static int cam_do_func(char *program_name, int index)
{
    int ret = -1;
    char print_string[512];
    int is_process = 0;

//	printf("%s L%d, index=%d   \n", __FUNCTION__, __LINE__, index);
    if (channel_flag) {
		memset(print_string, 0, sizeof(print_string));


		if (required_argument == s_cam_opts_list[index].long_option.has_arg) {
			if (optarg) {
				if (s_cam_opts_list[index].func_param_ptr) {
					ret = s_cam_opts_list[index].func_param_ptr(channel, optarg);
					is_process = 1;
				}
			} else {
				printf("please check your command \n");
				printf("ie:%s cam -c name -a string \n", program_name);
				return -1;
			}
		} else if (s_cam_opts_list[index].func_ptr) {
				ret = s_cam_opts_list[index].func_ptr(channel);
				is_process = 1;
		}

		if (is_process) {
			if (s_cam_opts_list[index].show_name)
				strcat(print_string, s_cam_opts_list[index].show_name);
			else
				strcat(print_string, s_cam_opts_list[index].long_option.name);

			if(VB_OK == ret) {
				strcat(print_string, " is success");
			} else {
				strcat(print_string, " is failed");
			}
			printf("%s \n", print_string);
			return ret;
		}
    }

    return ret;
}

static int cam_long_option(char *program_name, int index)
{
    int ret = -1;


    if (channel_flag) {
		ret = cam_do_func(program_name, index);
    } else {
		printf("Please check your command \n");
		printf("ie:%s cam -c name --%s string \n", program_name, s_cam_opts_list[index].long_option.name);
		return -1;
    }

    return ret;
}

static int cam_short_option(char *program_name, int opt,int max)
{
    int ret = 0;
    int i = 0;
    int is_out = 1;


    for (i = 0; i < max; i++) {
		is_out = 1;

		switch (opt) {
		case 'c':
			channel_flag = 1;
			channel = optarg;
			if (info_flag)
				get_cam_info(channel);
			break;
		case 'i':
			info_flag = 1;
			if (channel_flag)
				get_cam_info(channel);
			break;
		default:
			 if (opt == s_cam_opts_list[i].long_option.val) {
				if (channel_flag) {
					ret = cam_do_func(program_name, i);
				} else {
					printf("Please check your command \n");
					printf("ie:%s cam -c name -s string \n", program_name);
				}
			} else {
				is_out = 0;
			}
			break;
		}

		if (is_out)
			break;
    }

    if (i == max)
    {
		printf("Please check your command \n");
		ret = -1;
    }

    return ret;
}

static int sub_cam(char *program_name, int argc, char **argv)
{
    int option_index = 0;
    int ret = 0;
    int c = 0;
    struct option  *long_option_ptr = NULL;
    int long_list_num = 0;
    int i = 0;
    char short_opts[256];


    memset(short_opts, 0, sizeof(short_opts));
    long_list_num = sizeof(s_cam_opts_list) / sizeof(s_cam_opts_list[0]);
    long_option_ptr = calloc(long_list_num, sizeof(*long_option_ptr));
    for (i = 0; i < long_list_num; i++) {
		if (s_cam_opts_list[i].long_option.val) {
			strncat(short_opts, (char*)&s_cam_opts_list[i].long_option.val, 1);
			if (s_cam_opts_list[i].long_option.has_arg)
				strncat(short_opts, ":", 1);
		}
		memcpy(long_option_ptr + i, &s_cam_opts_list[i].long_option, sizeof(s_cam_opts_list[i].long_option));
    }
    while ((c = getopt_long(argc, argv, short_opts, long_option_ptr, &option_index)) != -1) {
        switch ( c ) {
        	case 'h' :
        		print_usage(argv[0]);
        		break;
			case 0 :
				ret = cam_long_option(program_name, option_index);
				break;
			case '?' :
				printf("Please check your command or get help by %s start/stop/cam/video--help  \n", program_name);
				break;
			default :
				ret = cam_short_option(program_name, c, long_list_num);
				break;
        }
    }

    return ret;
}
static int video_update_frc(char *chn, char *arg){
    struct v_frc_info frc;
    int len = strlen(arg);
    char *p = strstr(arg, ":");
    if(len >= 32  || p == NULL){
		printf("frc data error ,please set in framerate:framebase format\n");
		return -1;
    }
    frc.framerate = atoi(arg);
    frc.framebase = atoi(p+1);
    printf("frc info ->%d:%d\n", frc.framerate, frc.framebase);
    int ret = video_set_frc(chn, &frc);
    if(ret < 0){
		printf("set frc error \n");
		return -1;
    }
    sleep(2);
    ret = video_get_frc(chn, &frc);
    if(ret < 0 ){
		printf("get frc error\n");
		return -1;
    }
    printf("frc get result ->%d:%d\n", frc.framerate, frc.framebase);
    return 1;
}

static int video_parse_long_options(char *program_name, int index)
{
    int ret;
    if(0 == strcmp(video_long_options[index].name, "bitrate")) {
		if((1 == set_flag) && (1 == channel_flag) && (NULL != optarg)) {
			ret = set_video_qp_max(channel, optarg);
			if(VB_OK == ret) {
				printf("set video qp_max success     \n");
				return 0;
			} else {
				printf("set video qp_max fail     \n");
				return -1;
			}
		} else {
			printf("Please check your command \n");
			printf("ie:%s video -sc name -qp-max string \n", program_name);
			return -1;
		}
    } else if(0 == strcmp(video_long_options[index].name, "qp-min")) {
		if((1 == set_flag) && (1 == channel_flag) && (NULL != optarg)) {
			ret = set_video_qp_min(channel, optarg);
			if(VB_OK == ret) {
				printf("set video qp_min success     \n");
				return 0;
			} else {
				printf("set video qp_min fail     \n");
				return -1;
			}
		} else {
			printf("Please check your command \n");
			printf("ie:%s video -sc name -qp-min string \n", program_name);
			return -1;
		}
    } else if(0 == strcmp(video_long_options[index].name, "qp-hdr")) {
		if((1 == set_flag) && (1 == channel_flag) && (NULL != optarg)) {
			ret = set_video_qp_hdr(channel, optarg);
			if(VB_OK == ret) {
				printf("set video qp-hdr success     \n");
				return 0;
			} else {
				printf("set video qp-hdr fail     \n");
				return -1;
			}
		} else {
			printf("Please check your command \n");
			printf("ie:%s video -sc name --qp-hdr string \n", program_name);
			return -1;
		}
    } else if(0 == strcmp(video_long_options[index].name, "gop-length")) {
		if((1 == set_flag) && (1 == channel_flag) && (NULL != optarg)) {
			ret = set_video_gop_length(channel, optarg);
			if(VB_OK == ret) {
				printf("set video gop_max success     \n");
				return 0;
			} else {
				printf("set video gop_max fail     \n");
				return -1;
			}
		} else {
			printf("Please check your command \n");
			printf("ie:%s video -sc name -gop-max string \n", program_name);
			return -1;
		}
    } else if(0 == strcmp(video_long_options[index].name, "profile")) {
		if((1 == set_flag) && (1 == channel_flag) && (NULL != optarg)) {
			ret = set_video_profile(channel, optarg);
			if(VB_OK == ret) {
				printf("set video profile success     \n");
				return 0;
			} else {
				printf("set video profile fail     \n");
				return -1;
			}
		} else {
			printf("Please check your command \n");
			printf("ie:%s video -sc name -profile string \n", program_name);
			return -1;
		}
    } else if(0 == strcmp(video_long_options[index].name, "fps")) {
		if((1 == set_flag) && (1 == channel_flag) && (NULL != optarg)) {
			ret = set_video_framerate(channel, optarg);
			if(VB_OK == ret) {
				printf("set video framerate success     \n");
				return 0;
			} else {
				printf("set video framerate fail     \n");
				return -1;
			}
		} else {
			printf("Please check your command \n");
			printf("ie:%s video -sc name -profile string \n", program_name);
			return -1;
		}
    } else if(0 == strcmp(video_long_options[index].name, "roi")) {
		if((set_flag) && (1 == channel_flag) && (NULL != optarg)) {
            if(set_flag == 1) {
                ret = set_video_roi(channel, optarg);
                if(VB_OK == ret) {
                    printf("set video roi info success     \n");
                    return 0;
                } else {
                    printf("set video roi info fail     \n");
                    return -1;
                }
            } else {
                ret = get_video_roi(channel);
            }
		} else {
			printf("Please check your command \n");
			printf("ie:%s video -sc name --roi\n", program_name);
			return -1;
		}
    } else if(0 == strcmp(video_long_options[index].name, "capture")) {
		if((1 == set_flag) && (1 == channel_flag)) {
			capture_flag = 1;
		} else {
			printf("Please check your command \n");
			printf("ie:%s video -sc name -capture -o string \n", program_name);
			return -1;
		}
    } else if(0 == strcmp(video_long_options[index].name, "snapshot")) {
		if((1 == set_flag) && (1 == channel_flag)) {
			snapshot_flag = 1;
		} else {
			printf("Please check your command \n");
			printf("ie:%s video -sc name -snapshot -o string \n", program_name);
			return -1;
		}
    } else if(0 == strcmp(video_long_options[index].name, "height")) {
		height = atoi(optarg);
    } else if(0 == strcmp(video_long_options[index].name, "width")) {
		width = atoi(optarg);
    } else if(0 == strcmp(video_long_options[index].name, "resolution")) {
		set_res_flag = 1;
    } else if(0 == strcmp(video_long_options[index].name, "enable")) {
		video_enable_channel(channel);
    } else if(0 == strcmp(video_long_options[index].name, "disable")) {
		video_disable_channel(channel);
    } else if(0 == strcmp(video_long_options[index].name, "day")) {
		printf("set result ->%d\n", video_set_scenario(channel, VIDEO_DAY_MODE));
		sleep(2);
		printf("video set scenario day result ->%d\n", video_get_scenario(channel));
    } else if(0 == strcmp(video_long_options[index].name, "night")) {
		printf("set result ->%d\n", video_set_scenario(channel, VIDEO_NIGHT_MODE));
		sleep(2);
		printf("video set scenario night result ->%d\n", video_get_scenario(channel));
    } else if(0 == strcmp(video_long_options[index].name, "trigger-keyframe")) {
        video_trigger_key_frame(channel);
        printf("video trigger key ok\n");
    } else if(0 == strcmp(video_long_options[index].name, "frame-mode")) {
        frame_mode = atoi(optarg);
        if (( frame_mode < VENC_HEADER_NO_IN_FRAME_MODE) ||
            (frame_mode > VENC_HEADER_IN_FRAME_MODE))
        {
            printf("frame mode:%d is incorrect.\n", frame_mode);
            return -1;
        }
        video_set_frame_mode(channel, frame_mode);
        printf("video set frame mode:%d\n", frame_mode);
        printf("video get frame mode:%d\n", video_get_frame_mode(channel));
    } else if(0 == strcmp(video_long_options[index].name, "frc")){
		video_update_frc(channel, (char *)optarg);
    } else if(0 == strcmp(video_long_options[index].name, "set-snapshot")) {
        struct v_pip_info pip_info;
        int s32Ret = 0;
        printf("optarg:%s\n",optarg);
        printf("optarg:%s\n",tmp_argv[optind]);
        printf("optarg:%s\n",tmp_argv[optind + 1]);
        printf("optarg:%s\n",tmp_argv[optind + 2]);
        printf("optarg:%s\n",tmp_argv[optind + 3]);
        printf("optarg:%s\n",tmp_argv[optind + 4]);

        pip_info.pic_w = atoi(optarg);
        pip_info.pic_h = atoi(tmp_argv[optind]);
        pip_info.x = atoi(tmp_argv[optind + 1]);
        pip_info.y = atoi(tmp_argv[optind + 2]);
        pip_info.w = atoi(tmp_argv[optind + 3]);
        pip_info.h = atoi(tmp_argv[optind + 4]);

        s32Ret = video_set_snap(channel, &pip_info);
        if (s32Ret < 0)
        {
            printf("set crop args fail,ret:%d!\n",s32Ret);
        }
    } else if(0 == strcmp(video_long_options[index].name, "pip")) {
		struct v_pip_info pip_info;
		if((1 == set_flag) && (1 == channel_flag)) {
			strncpy(pip_info.portname, optarg, 16);
			pip_info.x = atoi(tmp_argv[optind]);
			pip_info.y = atoi(tmp_argv[optind + 1]);

			if (tmp_argv[optind + 2] && tmp_argv[optind + 3]) {
				pip_info.w = atoi(tmp_argv[optind + 2]);
				pip_info.h = atoi(tmp_argv[optind + 3]);
			}else {
				pip_info.w = pip_info.h = 0;
			}

			printf("vbctrl set pip \n");
			printf("postname:%s\n",optarg);
			printf("pip: x:%d\n",pip_info.x);
			printf("pip: y:%d\n",pip_info.y);
			printf("pip: w:%d\n",pip_info.w);
			printf("pip: h:%d\n",pip_info.h);

			video_set_pip_info(channel, &pip_info);
		} else if (1 == info_flag) {
			strncpy(pip_info.portname, optarg, 16);
			video_get_pip_info(channel, &pip_info);
			printf("vbctrl get pip \n");
			printf("pip: portname:%s\n",pip_info.portname);
			printf("pip: x:%d\n",pip_info.x);
			printf("pip: y:%d\n",pip_info.y);
			printf("pip: w:%d\n",pip_info.w);
			printf("pip: h:%d\n",pip_info.h);
		} else {
			printf("Please check your command \n");
			printf("ie:%s video -sc name --pip [portname] [x] [y] [w] [h]\n", program_name);
			printf("ie:%s video -ic name --pip [portname]\n", program_name);
			return -1;
		}
    } else if (0 == strcmp(video_long_options[index].name, "thumbnail")) {
        thumbnail_flag = 1;
    } else if (0 == strcmp(video_long_options[index].name, "set-thumb-res")) {
        thumbnail_set_resolution(channel, atoi(optarg), atoi(tmp_argv[optind]));
    } else if (0 == strcmp(video_long_options[index].name, "set-pic-quality")) {
        video_set_snap_quality(channel, atoi(optarg));
    }else if (0 == strcmp(video_long_options[index].name, "set-pic-panoramic")) {
        printf("vbctrl set-pic-panoramic info enable:%d\n",atoi(optarg));
        video_set_panoramic(channel, atoi(optarg));
    }
    else if(0 == strcmp(video_long_options[index].name, "nvrmode")) {
		struct v_nvr_info nvr_mode;
		nvr_mode.mode = atoi(optarg);
		nvr_mode.channel_enable[0]= atoi(tmp_argv[optind]);
		nvr_mode.channel_enable[1] = atoi(tmp_argv[optind + 1]);
		nvr_mode.channel_enable[2] = atoi(tmp_argv[optind + 2]);
		nvr_mode.channel_enable[3] = atoi(tmp_argv[optind + 3]);
		printf("vbctrl set nvr_mode\n");
		printf("nvr_mode.mode:%d\n",nvr_mode.mode);
		printf("nvr_mode.channel_enable[0]:%d\n",nvr_mode.channel_enable[0]);
		printf("nvr_mode.channel_enable[1]:%d\n",nvr_mode.channel_enable[1]);
		printf("nvr_mode.channel_enable[2]:%d\n",nvr_mode.channel_enable[2]);
		printf("nvr_mode.channel_enable[3]:%d\n",nvr_mode.channel_enable[3]);
		video_set_nvr_mode(channel,&nvr_mode);
    }
    else if (0 == strcmp(video_long_options[index].name, "photo-res-stress"))
    {
        s_s32PhotoResStress = 1;
    }
    else if (0 == strcmp(video_long_options[index].name, "stress-count"))
    {
        s_s32StressCount = atoi(optarg);
    }
    else if (0 == strcmp(video_long_options[index].name, "set-sei-mode"))
    {
        s_s32SEIMode = atoi(optarg);
    }
    else if (0 == strcmp(video_long_options[index].name, "set-sei"))
    {
        if (optarg && (strlen(optarg) > 0))
        {
            printf("SEI data:%s, mode:%d\n", optarg, s_s32SEIMode);
            video_set_sei_user_data(channel, optarg, strlen(optarg), s_s32SEIMode);
        }
    }
    else if (0 == strcmp(video_long_options[index].name, "set-slice-height"))
    {
        int s32SliceHeight = 0;
        s32SliceHeight = atoi(optarg);
        printf("vbctrl set-slice-height:%d\n",s32SliceHeight);
        ret = video_set_slice_height(channel,s32SliceHeight);
        if (ret != VBOK)
        {
            printf("video_set_slice_height fail Ret:%d\n",ret);
        }
        else
        {
            ret = video_get_slice_height(channel,&s32SliceHeight) ;
            if (ret != VBOK)
            {
                printf("video_get_slice_height fail Ret:%d\n",ret);
            }
            else
            {
                printf("video_get_slice_height:%d\n",s32SliceHeight);
            }
        }
    }
    else
    {
        printf("Please check your command \n");
        printf("Try `ctrlvb' for more information.\n");
        return -1;
    }
}

static int sub_video(char *program_name, int argc, char **argv)
{
    int ret = 0;
    int option_index = 0;
    int c;
    tmp_argv = argv;
    while ((c = getopt_long(argc, argv, video_short_opts, video_long_options, &option_index)) != -1) {
        switch ( c ) {
        	case 'h' :
        		print_usage(argv[0]);
        		return 0;
        		break;
        	case 'l' :
        		ret = get_video_list();
        		if(VB_OK == ret) {
					printf("get video list success     \n");
					return 0;
				} else {
					printf("get video list fail     \n");
					return -1;
				}
        		break;
        	case 's' :
        		set_flag = 1;
        		break;
            case 'q' :
                set_flag = 2;
                break;
        	case 'i' :
        		info_flag = 1;
        		if(argc < 3) {
        			printf("please check your command \n");
        			printf("ie:%s video -ic string \n", program_name);
        			return -1;
        		}
        		break;
            case 'c' :
                if(set_flag) {
                    channel_flag = 1;
                } else if((1 == info_flag) && (NULL != optarg)) {
                    ret = get_video_info(optarg);
                }else {
                    printf("please check your command \n");
                    printf("ie:%s video -sc string \n", program_name);
                    return -1;
                }
                channel = optarg;
                break;
        	case 'b' :
        		if((set_flag) && (1 == channel_flag) && (NULL != optarg)) {
                    if(set_flag == 1){
        			ret = set_video_bitrate(channel, optarg);
        			if(VB_OK == ret) {
						printf("set video bitrate success     \n");
						return 0;
					} else {
						printf("set video bitrate fail     \n");
						return -1;
					}
                    }
                    else{
                        ret = get_video_bitrate(channel);
                    }
        		} else {
        			printf("please check your command \n");
                    printf("ie:%s video -sc string \n", program_name);
        			return -1;
        		}
        		break;
            case 'r' :
                if((set_flag) && (1 == channel_flag) && (NULL != optarg)) {
                    if(set_flag == 1) {
                        ret = set_video_ratectrl(channel, optarg);
                        if(VB_OK == ret) {
                            printf("set video rate ctrl success     \n");
                            return 0;
                        } else {
                            printf("set video rate ctrl fail     \n");
                            return -1;
                        }
                    } else {
                        ret = get_video_ratectrl(channel);
                    }
                } else {
                    printf("please check your command \n");
                    printf("ie:%s video -sc/qc string \n", program_name);
                    return -1;
                }
                break;
            case 't' :
                if((1 == set_flag) && (1 == channel_flag) && (NULL != optarg)) {
                    ret = set_video_streamtype(channel, optarg);
                    if(VB_OK == ret) {
						printf("set video stream type success     \n");
						return 0;
					} else {
						printf("set video stream type fail     \n");
						return -1;
					}
                } else {
                    printf("please check your command \n");
                    printf("ie:%s video -sc name -t streamtype\n", program_name);
                    return -1;
                }
                break;
            case 'p' :
                if((1 == set_flag) && (1 == channel_flag) && (NULL != optarg)) {
                    ret = set_video_pframe(channel, optarg);
                    if(VB_OK == ret) {
						printf("set video pframe success     \n");
						return 0;
					} else {
						printf("set video pframe fail     \n");
						return -1;
					}
        		} else {
        			printf("please check your command \n");
        			printf("ie:%s video -sc name -b string \n", program_name);
        			return -1;
        		}
        		break;
        	case 'o' :
                if ((1 == set_flag) && (1 == channel_flag))
                {
                    if(1 == capture_flag)
                    {
                        video_capture(channel, optarg);
                        return 0;
                    }
                    else if(1 == snapshot_flag)
                    {
                        ret = video_snapshot(channel, optarg);
                        return ret;
                    }
                    else if(1 == thumbnail_flag)
                    {
                        ret = video_thumbnail(channel, optarg);
                        return ret;
                    }
                    else if (s_s32PhotoResStress > 0)
                    {
                        ret = test_photo_change_resolution(channel, optarg, s_s32StressCount);
                        return ret;
                    }
                }
                else {
                    printf("please check your command \n");
                    printf("ie:%s video -sc name --capture/snapshot -t string \n", program_name);
                    return -1;
                }
				break;
			case 0 :
				video_parse_long_options(program_name, option_index);
				break;
			case '?' :
				printf("Please check your command or get help by %s start/stop/cam/video--help  \n", program_name);
				break;
			default :
				printf("Try `ctrlvb' for more information.\n");
				return -1;
        }
    }

    if(set_res_flag) {
		if((0 == width) || (0 == height)) {
			printf("please check your command \n");
			printf("i:%s video -sc name --width string --height string --resolution \n", program_name);
			return -1;
		}
		video_set_resolution(channel, width, height);
    }
    return ret;
}

static int sub_mv(char *program_name, int argc, char **argv)
{
    int i, c, mv_channelflag = 0, mv_setflag = 0;
    int fps = -1;
    int level = 0;
    int option_index = 0;
    const char *mv = NULL;
    char *p = NULL;
    char *str = NULL;
    char *str_temp = NULL;
    struct MBThreshold st;
    struct blkInfo_t blkInfo;
    struct roiInfo_t roi;

    while ((c = getopt_long(argc, argv, mv_short_opts, NULL, &option_index)) != -1) {
        switch ( c ) {
            case 's' :
                mv_setflag = 1;
                break;
            case 'c' :
                mv = optarg;
                mv_channelflag = 1;
                printf("mv: %s\n", mv);
                break;
            case 'b':
                if(!mv_channelflag) {
                    printf("pleae specify channel name\n");
                    break;
                }
                if(mv_setflag) {
                    for(i = 0, str = optarg; ;i++, str = NULL) {
                        p = strtok_r(str, ":", &str_temp);
                        if(p == NULL)
                            break;
                        *(((int *)&blkInfo) + i) = atoi(p);
                    }
                    if(i != 2) {
                        printf("eg: vbctrl mv cs mv0 -b blk_x:blk_y\n");
                        break;
                    }
                    printf("mv set grid block w: %d, h: %d\n", blkInfo.blk_x, blkInfo.blk_y);
                    va_mv_set_blkinfo(mv, &blkInfo);
                } else {
                    va_mv_get_blkinfo(mv, &blkInfo);
                    printf("mv set grid block w: %d, h: %d\n", blkInfo.blk_x, blkInfo.blk_y);
                }
                break;
            case 'l':
                if(!mv_channelflag) {
                    printf("pleae specify channel name\n");
                    break;
                }
                if(mv_setflag) {
                    level = atoi(optarg);
                    printf("mv set senlevel: %d\n", level);
                    va_mv_set_senlevel(mv, level);
                } else {
                    printf("mv set senlevel: %d\n", va_mv_get_senlevel(mv));
                }
                break;
            case 't':
                if(!mv_channelflag) {
                    printf("pleae specify channel name\n");
                    break;
                }
                if(mv_setflag) {
                    for(i = 0, str = optarg; ;i++, str = NULL) {
                        p = strtok_r(str, ":", &str_temp);
                        if(p == NULL)
                            break;
                        *(((int *)&st) + i) = atoi(p);
                    }
                    if(i != 3) {
                        printf("eg: vbctrl mv cs mv0 -t tlmv:tdlmv:tcmv\n");
                        break;
                    }
                    printf("Tlmv: %d, Tdlmv: %d, Tcmv: %d\n",
                            st.tlmv, st.tdlmv, st.tcmv);
                    va_mv_set_sensitivity(mv, &st);
                } else {
                    va_mv_get_sensitivity(mv, &st);
                    printf("Tlmv: %d, Tdlmv: %d, Tcmv: %d\n",
                            st.tlmv, st.tdlmv, st.tcmv);
                }
                break;
            case 'f' :
                if(!mv_channelflag) {
                    printf("pleae specify channel name\n");
                    break;
                }
                if(mv_setflag) {
                    fps = atoi(optarg);
                    printf("mv sample fps set to %d\n", fps);
                    va_mv_set_sample_fps(mv, fps);
                } else {
                    printf("mv get sample fps: %d\n", va_mv_get_sample_fps(mv));
                }
                break;
            case 'r':
                if(!mv_channelflag) {
                    printf("pleae specify channel name\n");
                    break;
                }
                if(mv_setflag) {
                    for(i = 0, str = optarg; ;i++, str = NULL) {
                        p = strtok_r(str, ":", &str_temp);
                        if(p == NULL)
                            break;
                        *(((int *)&roi) + i) = atoi(p);
                    }
                    if(i != 6) {
                        printf("eg: vbctrl mv cs mv0 -r x:y:w:h:0:roi_num\n");
                        break;
                    }
                    printf("mv set roi info-> x: %d, y: %d, w: %d, h: %d, mvNumb: %d, roi_num: %d\n",
                            roi.mv_roi.x, roi.mv_roi.y, roi.mv_roi.w, roi.mv_roi.h,
                            roi.mv_roi.mv_mb_num, roi.roi_num);
                    va_mv_set_roi(mv, &roi);
                } else {
                    roi.roi_num = atoi(optarg);
                    va_mv_get_roi(mv, &roi);
                    printf("mv get roi info -> x: %d, y: %d, w: %d, h: %d, mvNumb: %d, roi_num: %d\n",
                            roi.mv_roi.x, roi.mv_roi.y, roi.mv_roi.w, roi.mv_roi.h,
                            roi.mv_roi.mv_mb_num, roi.roi_num);
                }
                break;
            default :
                return -1;
        }
    }
    return 0;
}

static int sub_vam(char *program_name, int argc, char **argv)
{
    int ret, sens = -1, c;
    int fps = -1;
    int option_index = 0;
    const char *va = NULL;

    while ((c = getopt_long(argc, argv, "s:c:i:f:", NULL, &option_index)) != -1) {
        switch ( c ) {
        	case 's' :
				sens = atoi(optarg);
				break;
        	case 'c' :
        		va = optarg;
				printf("va: %s\n", va);
        		break;
        	case 'i' :
				sens = -1;
				break;
			case 'f' :
				fps = atoi(optarg);
				printf("fps : %d\n", fps);
				break;
			default :
				return -1;
        }
    }

    if(sens >= 0) {
		va_move_set_sensitivity(va, sens);
		printf("va sensitivity set to %d\n", va_move_get_sensitivity(va));
    } else
		printf("va status: %d: %d\n", va_move_get_sensitivity(va),
		 va_move_get_status(va, 0, 0));

    if(fps >= 0) {
		va_move_set_sample_fps(va, fps);
		printf("va sample fps set to %d\n", va_move_get_sample_fps(va));
    }

    return 0;
}

static void sub_overlay(char *program_name, int argc, char **argv)
{
    int ret;
    int option_index = 0;
    int c;
    int s32Width,s32Height;
    struct v_pip_info stPipInfo;
    font_attr.font_color = 0x00ffffff;
    font_attr.back_color = 0x20000000;
    strcpy(font_attr.ttf_name,"arial");
    tmp_argv = argv;
    while ((c = getopt_long(argc, argv, overlay_short_opts, overlay_long_options, &option_index)) != -1) {
        switch ( c ) {
            case 'c' :
                channel_flag = 1;
                channel = optarg;
                break;
            case 's' :
                if (channel_flag == 1)
                {
                    int enable = atoi(optarg);
                    if (enable == 1)
                    {
                        set_flag = 1;
                        marker_enable(channel);
                        marker_set_mode(channel, "auto", overlay_fmt, &font_attr);
                    }
                    else if (enable == 0)
                    {
                        set_flag = 0;
                        marker_disable(channel);
                    }
                }
                break;
            case 'f' :
                strncpy((char *)font_attr.ttf_name, optarg, strlen(optarg) + 1);
                marker_set_mode(channel, "auto", overlay_fmt, &font_attr);
                break;
            case 'a' :
                if (strncmp(optarg,"MiD",3))
                {
                    font_attr.mode = MIDDLE;
                }
                else if (strncmp(optarg,"LEFT",4))
                {
                    font_attr.mode = LEFT;
                }
                else if (strncmp(optarg,"RIGHT",5))
                {
                    font_attr.mode = RIGHT;
                }
                marker_set_mode(channel, "auto", overlay_fmt, &font_attr);
                break;
            case 'p' :
                memset(&stPipInfo, 0, sizeof(struct v_pip_info));
                strncpy(stPipInfo.portname, optarg, strlen(optarg));
                stPipInfo.x = atoi(tmp_argv[optind]);
                stPipInfo.y = atoi(tmp_argv[optind + 1]);
                ret = video_set_pip_info("ispost", &stPipInfo);
                if(VB_OK == ret) {
                    printf("set overlay position success     \n");
                    return;
                } else {
                    printf("set overlay position fail     \n");
                    return;
                }
                break;
            case 'n' :
                if((1 == set_flag) && (1 == channel_flag) && (NULL != optarg)) {
                    marker_set_mode(channel, "manual", overlay_fmt, &font_attr);
                    marker_set_picture(channel,optarg);
                } else {
                    printf("please check your command \n");
                    printf("ie:%s overlay -n string \n", program_name);
                    return;
                }
                break;
            case 'b' :
                if((1 == set_flag) && (1 == channel_flag) && (NULL != optarg)) {
                    overlay_set_bmp(channel,optarg);
                } else {
                    printf("please check your command \n");
                    printf("ie:%s overlay -b string \n", program_name);
                    return;
                }
                break;
            case 'r' :
                s32Width = s32Height = 0;
                int count = 0;
                int limit = 100;
                memset(&stPipInfo, 0, sizeof(struct v_pip_info));

                srand((unsigned)time(NULL));

                video_get_resolution("isp-out", &s32Width,&s32Height);

                strncpy(stPipInfo.portname, optarg, strlen(optarg));
                if(tmp_argv[optind])
                {
                    limit = atoi(tmp_argv[optind]);
                }

                while(count++ < limit)
                {
                    stPipInfo.x = rand() % s32Width;
                    stPipInfo.y = rand() % s32Height;

                    // set marker position
                    ret = video_set_pip_info("ispost", &stPipInfo);
                    if (ret < 0)
                    {
                        printf("video_set_pip_info error,err num:%d\n", ret);
                        return -1;
                    }
                    usleep(500*1000);
                }
                break;
            case 0 :
                overlay_parse_long_options(program_name, option_index);
                break;
            case '?' :
                printf("Please check your command or get help by %s start/stop/cam/video/overlay--help  \n", program_name);
                break;
            default :
                printf("Try `ctrlvb' for more information.\n");
            return;
        }
    }
}

static int sub_rtsp(char *program_name, int argc, char **argv)
{
    int ret, sens = -1, c;
    int fps = -1;
    int option_index = 0;
    const char *va = NULL;

    while ((c = getopt_long(argc, argv, "lp:", NULL, &option_index)) != -1) {
        switch ( c ) {
            case 'l' :
                rtsp_server_init(0,NULL,NULL);
                rtsp_server_start();
                while(!quit)
                {
                    usleep(200*1000);
                }
                rtsp_server_stop();
                break;
                case 'p' :
                rtsp_server_init(0,NULL,NULL);
                rtsp_server_set_file_location(optarg);
                rtsp_server_start();
                while(!quit)
                {
                    usleep(200*1000);
                }
                rtsp_server_stop();
                break;
                default :
                return -1;
        }
    }

    return 0;
}


const char *debug_short_opts = "m:";
const struct option debug_long_options[] = {
    {"mode",        required_argument,        NULL,   'm'},
    {"isp",        no_argument,        NULL,   'i'},
    {"venc",        no_argument,        NULL,   'v'},
    {0,            0,                  NULL,     0}
};

static void sub_debug(char *program_name, int argc, char **argv)
{
    int ret;
    int option_index = 0;
    int c;
    int mode = 0;
    int isp_mode = 0;
    int venc_mode = 0;

    while ((c = getopt_long(argc, argv,
						debug_short_opts,
						debug_long_options,
						&option_index)) != -1) {
		switch ( c ) {
		case 'm':
			mode = atoi(optarg);
			break;

		case 'i':
			isp_mode  = 1;
			break;

		case 'v':
			venc_mode  = 1;
			break;

		case '?' :
			printf("Please check your command or get help by %s "
				"start/stop/cam/video/overlay/debug--help  \n", program_name);
			break;
		default :
			printf("Try `ctrlvb' for more information.\n");
			return;
		}
    }

    if (isp_mode)
		video_dbg_print_info(VIDEO_DEBUG_ISP_UE_MODE);
    else if (venc_mode)
		video_dbg_print_info(VIDEO_DEBUG_VENC_MODE);
    else
		video_dbg_print_info(mode);

}

static void signal_handler(int sig)
{
    if(SIGINT == sig) {
    	quit = 1;
    }

}

/*
* init signal
*/
static void init_signal()
{
    signal(SIGINT, signal_handler);
}

int main(int argc, char **argv)
{

    init_signal();

    if(argc > 1) {
    if(0 == strcmp(argv[1], "start")) {
		videobox_start(argc < 3? NULL: argv[2]);
    } else if(0 == strcmp(argv[1], "stop")) {
		videobox_stop();
    } else if(0 == strcmp(argv[1], "cam")) {
		sub_cam(argv[0], argc-1, argv+1);
    }  else if(0 == strcmp(argv[1], "debug")) {
		sub_debug(argv[0], argc-1, argv+1);
    }  else if(0 == strcmp(argv[1], "video")) {
		sub_video(argv[0], argc-1, argv+1);
		} else if(0 == strcmp(argv[1], "overlay")) {
			sub_overlay(argv[0], argc-1, argv+1);
		} else if(0 == strcmp(argv[1], "vam")) {
			sub_vam(argv[0], argc-1, argv+1);
		} else if(0 == strcmp(argv[1], "mv")) {
			sub_mv(argv[0], argc-1, argv+1);
		} else if(0 == strcmp(argv[1], "rebind")) {
			if(videobox_rebind(argv[2], argv[3]) < 0)
				printf("rebind %s to %s failed\n", argv[2], argv[3]);
		} else if(0 == strcmp(argv[1], "repath")) {
			if(videobox_repath(argv[2]) < 0)
				printf("repath %s failed\n", argv[2]);
		} else if(0 == strcmp(argv[1], "ctrl")) {
			if(argc < 5) {
				printf("please check command parameters(usage:vbctrl ctrl IPU CODE PARA)\n");
				return -1;
			} else {
				int ret = videobox_control(argv[2], atoi(argv[3]), atoi(argv[4]));
				printf("return value: %d\n", ret);
			}
		} else if(0 == strcmp(argv[1], "rtsp")) {
			sub_rtsp(argv[0], argc-1, argv+1);
		} else {
			printf("please check your command \n");
    		printf("Usage: %s start/stop/cam/video -s/ic  [<channel_name>] -d/b.... [<setvalue>] \n", argv[0]);
    	}
    } else {
    	print_usage(argv[0]);

    }

    exit(EXIT_SUCCESS);
}

static int get_camera_sensor_exposure(const char *cam)
{
    int usec = 0;
    int ret = 0;

    if (NULL != cam) {
#if !TEST
        ret = camera_get_sensor_exposure(cam, &usec);
        if(-1 == ret) {
            CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
            return VB_FAIL;
        }
        printf("%s-----%s-----sensor exposure(us) = %d----\n", __FUNCTION__, cam, usec);
#else
        printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif
    } else {
        return VB_FAIL;
    }

    return VB_OK;
}


static int set_camera_sensor_exposure(const char *cam,  char *value)
{
    int exp = 0;
    int ret = 0;


    if ((NULL != cam) && (NULL != value)) {
		exp = atoi(value);
#if !TEST
		ret = camera_set_sensor_exposure(cam, exp);
		if (-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_dpf_attr(const char *cam,  char *value)
{
    int en = 0;
    int ret = 0;


    if ((NULL != cam) && (NULL != value)) {
		en = atoi(value);
#if !TEST
{
		cam_dpf_attr_t attr;
		memset(&attr, 0, sizeof(attr));
		camera_get_dpf_attr(cam, &attr);

		attr.cmn.mdl_en = en;
		attr.threshold = 0;
		attr.weight = 0.0;
		ret = camera_set_dpf_attr(cam, &attr);
		if (-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_dns_attr(const char *cam,  char *value)
{
    int mode = 0;
    int ret = 0;


    if ((NULL != cam) && (NULL != value)) {
		mode = atoi(value);
#if !TEST
{
		cam_dns_attr_t attr;
		memset(&attr, 0, sizeof(attr));
		camera_get_dns_attr(cam, &attr);

		attr.cmn.mdl_en = 1;
		attr.cmn.mode = mode;
		attr.strength = 6.0;
		ret = camera_set_dns_attr(cam, &attr);
		if (-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_sha_dns_attr(const char *cam,  char *value)
{
    int mode = 0;
    int ret = 0;


    if ((NULL != cam) && (NULL != value)) {
		mode = atoi(value);
#if !TEST
{
		cam_sha_dns_attr_t attr;
		memset(&attr, 0, sizeof(attr));
		camera_get_sha_dns_attr(cam, &attr);

		attr.cmn.mdl_en = 1;
		attr.cmn.mode = mode;
		attr.strength = 1.0;
		attr.smooth = 1.0;
		ret = camera_set_sha_dns_attr(cam, &attr);
		if (-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_wb_attr(const char *cam,  char *value)
{
    int mode = 0;
    int ret = 0;


    if ((NULL != cam) && (NULL != value)) {
		mode = atoi(value);
#if !TEST
{
		cam_wb_attr_t attr;
		memset(&attr, 0, sizeof(attr));
		camera_get_wb_attr(cam, &attr);

		attr.cmn.mdl_en = 1;
		attr.cmn.mode = mode;
		if (CAM_CMN_MANUAL == attr.cmn.mode) {
			attr.man.rgb.r = 6.5;
			attr.man.rgb.g = 1.0;
			attr.man.rgb.b = 0.1;
		}
		ret = camera_set_wb_attr(cam, &attr);
		if (-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_3d_dns_attr(const char *cam,  char *value)
{
    int mode = 0;
    int ret = 0;


    if ((NULL != cam) && (NULL != value)) {
		mode = atoi(value);
#if !TEST
{
		typedef struct
		{
			int t1;
			int t2;
			int t3;
			int w;
		}th_w_t;

		th_w_t list[]=
		{
#if 0
			/*same values with ispostv2.cpp */
			{20, 20, 20, 8},
			{20, 20, 20, 5},
			{20, 20, 20, 4},
			{20, 20, 20, 2},
			{34, 34, 34, 2},
			{14, 14, 14, 5},
			{14, 24, 24, 4},
			{14, 24, 24, 3},
			{14, 30, 30, 3},
			{14, 34, 34, 3}
#else
#if 0
			{48, 48, 48, 3},
			{48, 48, 48, 4},
			{48, 48, 48, 5},
			{48, 48, 48, 6},
			{48, 48, 48, 7},
			{48, 48, 48, 8},
			{14, 14, 14, 5},
			{20, 20, 20, 5},
			{24, 24, 24, 5},
			{30, 30, 30, 5},
			{34, 34, 34, 5},
#endif
			{48, 48, 48, 3},
#endif
		};

		int i = 0;
		int max = 0;


        max = sizeof(list) / sizeof(list[0]);
		cam_3d_dns_attr_t attr;
		memset(&attr, 0, sizeof(attr));
		camera_get_3d_dns_attr(cam, &attr);

        for (i = 0; i < max; i++)
        {
            attr.cmn.mdl_en = 1;
            attr.cmn.mode = mode;
            if (CAM_CMN_MANUAL == attr.cmn.mode) {
                attr.y_threshold = list[i].t1;
                attr.u_threshold = list[i].t2;
                attr.v_threshold = list[i].t3;
                attr.weight = list[i].w;
            }

            ret = camera_set_3d_dns_attr(cam, &attr);
            if (-1 == ret) {
                CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
                return VB_FAIL;
            }

            usleep(1000*1000);
        }
}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int get_camera_3d_dns_attr(const char *cam)
{
    int ret = 0;


    if ((NULL != cam)) {
#if !TEST
{
		cam_3d_dns_attr_t attr;


		memset(&attr, 0, sizeof(attr));
		ret = camera_get_3d_dns_attr(cam, &attr);
		printf("isp 3d dns attr : en=%d  mode=%d y_thr=%d u_thr=%d v_thr=%d w=%d \n",
			attr.cmn.mdl_en, attr.cmn.mode,
			attr.y_threshold, attr.u_threshold,
			attr.v_threshold, attr.weight);
}
#else
		printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_yuv_gamma_attr(const char *cam,  char *value)
{
    int mode = 0;
    int ret = 0;


    if ((NULL != cam) && (NULL != value)) {
		mode = atoi(value);
#if !TEST
{
		float curve[] =
		{
			1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
			1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
			1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
			1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
			1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
			1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
			1.0, 1.0, 1.0
		};
		cam_yuv_gamma_attr_t attr;


		memset(&attr, 0, sizeof(attr));
		camera_get_yuv_gamma_attr(cam, &attr);

		attr.cmn.mdl_en = 1;
		attr.cmn.mode = mode;
		if (CAM_CMN_MANUAL == attr.cmn.mode) {
			memcpy(attr.curve, curve, sizeof(curve));
		}
		ret = camera_set_yuv_gamma_attr(cam, &attr);
		if (-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int get_camera_yuv_gamma_attr(const char *cam)
{
    int ret = 0;


    if ((NULL != cam)) {
#if !TEST
{
		const int line_count = 10;
		char temp_str[30];
		int i = 0;
		cam_yuv_gamma_attr_t attr;


		memset(&attr, 0, sizeof(attr));
		camera_get_yuv_gamma_attr(cam, &attr);

		printf("isp y gamma attr : en=%d  mode=%d curve: \n",
			attr.cmn.mdl_en, attr.cmn.mode);
		for (i = 0; i < CAM_ISP_YUV_GAMMA_COUNT; ++i) {
			if ((0 == (i % line_count))
				&& (i != 0) )
				printf("\n");
			if (0 == (i % line_count))
				printf("             ");

			sprintf(temp_str, "%f", attr.curve[i]);
			printf("%8s  ", temp_str);
		}
		printf("\n");
}
#else
		printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}


static int set_camera_fps_range(const char *cam,  char *value)
{
    int ret = 0;
    int mode = 0;

    if ((NULL != cam) && (NULL != value)) {
		mode = atoi(value);
#if !TEST
{
		cam_fps_range_t fps;
		memset(&fps, 0, sizeof(fps));

		camera_ae_enable(cam, mode);

		fps.min_fps = 10;
		fps.max_fps = 25;
		ret = camera_set_fps_range(cam, &fps);
		if (-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int get_camera_fps_range(const char *cam)
{
    int ret = 0;
    int mode = 0;


    if ((NULL != cam)) {
#if !TEST
{
		cam_fps_range_t fps;
		memset(&fps, 0, sizeof(fps));

		camera_ae_enable(cam, mode);

		ret = camera_get_fps_range(cam, &fps);
		printf("fps range : min_fps=%f  max_fps=%f \n", fps.min_fps, fps.max_fps);
}
#else
		printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_save_data(const char *cam, char *value)
{
    int ret = 0;
    char *path = NULL;

    if (NULL != cam) {
#if !TEST
{
		cam_save_data_t save_data;
		int i = 0;

		path = value;
		if ('/' == path[strlen(path) - 1]) {
			path[strlen(path) - 1] = '\0';
		}
		memset(&save_data, 0, sizeof(save_data));
		while (i < 2)
		{
			save_data.fmt = CAM_DATA_FMT_BAYER_FLX | CAM_DATA_FMT_YUV |CAM_DATA_FMT_BAYER_RAW;
			sprintf(save_data.file_path, "%s/", path);
			if (1 == i)
				sprintf(save_data.file_name, "myfilename");
			camera_save_data(cam, &save_data);
			if (-1 == ret) {
				CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
				return VB_FAIL;
			}
			i++;
		}
}
#else
		printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_create_fcedata(const char *cam)
{
    int ret = 0;

    if (NULL != cam) {
#if !TEST
{
		cam_fcedata_param_t param;

		memset(&param, 0, sizeof(param));
		param.common.out_width = 1920;
		param.common.out_height = 1080;
		param.common.grid_size = 0;
		param.common.fisheye_center_x = 1080 / 2;
		param.common.fisheye_center_y = 1080 / 2;

    {
		double org_pitch_angle = 0.0;
		double org_rotate_angle = 0.0;
		int i,j,cnt;

		param.fec.fisheye_mode = CAM_FE_MODE_PERSPECTIVE;
		param.fec.fisheye_radius = -5;
		// 0, 4..., 352, 356 (90 sets)
		//This part we could use for-loop to generate 90 sets grid data every 4 degree
		param.fec.fisheye_rotate_angle = 0;
		param.fec.fisheye_heading_angle = 90;
		// 65, 62, 59 ..., 41, 38 (10 sets)
		//This part we could use for-loop to generate 10 sets grid data every 3 degree
		param.fec.fisheye_pitch_angle = 65;
		param.fec.fisheye_fov_angle = 65;

		org_pitch_angle = param.fec.fisheye_pitch_angle;
		org_rotate_angle = param.fec.fisheye_rotate_angle;
		cnt = 0;
		for (i = 0; i < 9; ++i) {
			param.fec.fisheye_rotate_angle = org_rotate_angle + i * 4 * 10;

			for (j = 0; j < 10; ++j, ++cnt) {
				param.fec.fisheye_pitch_angle = org_pitch_angle - j * 3;

				printf("grid_param.fisheyePitchAngle=%f,rotate:%f \n",\
					   param.fec.fisheye_pitch_angle, param.fec.fisheye_rotate_angle);
				camera_create_and_fire_fcedata(cam, &param);
				sleep(1);
			}
		}
		printf("%s:%d cnt=%d \n", __func__, __LINE__, cnt);
    }
}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_load_fcedata(const char *cam, char *value)
{
    int ret = 0;
    char *path = NULL;

    if (NULL != cam) {
#if !TEST
{
		cam_fcefile_param_t param;
		cam_fcefile_mode_t *mode_list;
		cam_fcefile_file_t *file_list;
		cam_fcefile_port_t  port;
		cam_position_t  pos;
		int mode_nb = 1;
		int file_nb = 1;
		int mode_index = 0;
		int i = 0;
		int j = 0;

		path = value;
		if ('/' == path[strlen(path) - 1]) {
			path[strlen(path) - 1] = '\0';
		}
		memset(&param, 0, sizeof(param));

		param.mode_number = 3;
		mode_list = param.mode_list;
		for (i = 0; i < param.mode_number; ++i, mode_list++)
		{
			switch(i)
			{
			case 0:
				mode_list->file_number = 4;
				sprintf(mode_list->file_list[0].file_grid, "%s/lc_down_view_0-0_hermite32_1088x1088-960x544_fisheyes_downview_0.bin", path);
#if 0
				mode_list->file_list[0].outport_number = 4;

				pos.x = 0; pos.y = 0; pos.width = 320; pos.height = 240;
				port.type = CAM_FCE_PORT_SS1;
				port.pos = pos;
				mode_list->file_list[0].outport_list[0] = port;

				pos.x = 0; pos.y = 0; pos.width = 960; pos.height = 540;
				port.type = CAM_FCE_PORT_DN;
				port.pos = pos;
				mode_list->file_list[0].outport_list[1] = port;

				pos.x = 0; pos.y = 0; pos.width = 960; pos.height = 540;
				port.type = CAM_FCE_PORT_UO;
				port.pos = pos;
				mode_list->file_list[0].outport_list[2] = port;

				pos.x = 0; pos.y = 0; pos.width = 320; pos.height = 240;
				port.type = CAM_FCE_PORT_SS0;
				port.pos = pos;
				mode_list->file_list[0].outport_list[3] = port;
#endif
				sprintf(mode_list->file_list[1].file_grid, "%s/lc_down_view_0-0_hermite32_1088x1088-960x544_fisheyes_downview_1.bin", path);
#if 0
				mode_list->file_list[1].outport_number = 4;
				pos.x = 320; pos.y = 0; pos.width = 320; pos.height = 240;
				port.type = CAM_FCE_PORT_SS1;
				port.pos = pos;
				mode_list->file_list[1].outport_list[0] = port;

				pos.x = 960; pos.y = 0; pos.width = 960; pos.height = 540;
				port.type = CAM_FCE_PORT_DN;
				port.pos = pos;
				mode_list->file_list[1].outport_list[1] = port;

				pos.x = 960; pos.y = 0; pos.width = 960; pos.height = 540;
				port.type = CAM_FCE_PORT_UO;
				port.pos = pos;
				mode_list->file_list[1].outport_list[2] = port;

				pos.x = 320; pos.y = 0; pos.width = 320; pos.height = 240;
				port.type = CAM_FCE_PORT_SS0;
				port.pos = pos;
				mode_list->file_list[1].outport_list[3] = port;
#endif
				sprintf(mode_list->file_list[2].file_grid, "%s/lc_down_view_0-0_hermite32_1088x1088-960x544_fisheyes_downview_2.bin", path);
#if 0
				mode_list->file_list[2].outport_number = 4;

				pos.x = 0; pos.y = 240; pos.width = 320; pos.height = 240;
				port.type = CAM_FCE_PORT_SS1;
				port.pos = pos;
				mode_list->file_list[2].outport_list[0] = port;

				pos.x = 0; pos.y = 540; pos.width = 960; pos.height = 540;
				port.type = CAM_FCE_PORT_DN;
				port.pos = pos;
				mode_list->file_list[2].outport_list[1] = port;

				pos.x = 0; pos.y = 540; pos.width = 960; pos.height = 540;
				port.type = CAM_FCE_PORT_UO;
				port.pos = pos;
				mode_list->file_list[2].outport_list[2] = port;

				pos.x = 0; pos.y = 240; pos.width = 320; pos.height = 240;
				port.type = CAM_FCE_PORT_SS0;
				port.pos = pos;
				mode_list->file_list[2].outport_list[3] = port;
#endif
				sprintf(mode_list->file_list[3].file_grid, "%s/lc_down_view_0-0_hermite32_1088x1088-960x544_fisheyes_downview_3.bin", path);
#if 0
				mode_list->file_list[3].outport_number = 4;
				pos.x = 320; pos.y = 240; pos.width = 320; pos.height = 240;
				port.type = CAM_FCE_PORT_SS1;
				port.pos = pos;
				mode_list->file_list[3].outport_list[0] = port;

				pos.x = 960; pos.y = 540; pos.width = 960; pos.height = 540;
				port.type = CAM_FCE_PORT_DN;
				port.pos = pos;
				mode_list->file_list[3].outport_list[1] = port;

				pos.x = 960; pos.y = 540; pos.width = 960; pos.height = 540;
				port.type = CAM_FCE_PORT_UO;
				port.pos = pos;
				mode_list->file_list[3].outport_list[2] = port;

				pos.x = 320; pos.y = 240; pos.width = 320; pos.height = 240;
				port.type = CAM_FCE_PORT_SS0;
				port.pos = pos;
				mode_list->file_list[3].outport_list[3] = port;
#endif
				break;
			case 1:
				mode_list->file_number = 2;
				sprintf(mode_list->file_list[0].file_grid, "%s/lc_up_2p360_0-0_hermite32_1088x1088-1920x544_fisheyes_panorama_0.bin", path);
#if 0
				mode_list->file_list[0].outport_number = 2;

				pos.x = 0; pos.y = 0; pos.width = 640; pos.height = 240;
				port.type = CAM_FCE_PORT_SS0;
				port.pos = pos;
				mode_list->file_list[0].outport_list[0] = port;

				pos.x = 0; pos.y = 0; pos.width = 1920; pos.height = 540;
				port.type = CAM_FCE_PORT_DN;
				port.pos = pos;
				mode_list->file_list[0].outport_list[1] = port;
#endif
				sprintf(mode_list->file_list[1].file_grid, "%s/lc_up_2p360_0-0_hermite32_1088x1088-1920x544_fisheyes_panorama_1.bin", path);
#if 0
				mode_list->file_list[1].outport_number = 2;
				pos.x = 0; pos.y = 240; pos.width = 640; pos.height = 240;
				port.type = CAM_FCE_PORT_SS0;
				port.pos = pos;
				mode_list->file_list[1].outport_list[0] = port;

				pos.x = 0; pos.y = 540; pos.width = 1920; pos.height = 540;
				port.type = CAM_FCE_PORT_DN;
				port.pos = pos;
				mode_list->file_list[1].outport_list[1] = port;
#endif
				break;
			case 2:
				mode_list->file_number = 1;
				sprintf(mode_list->file_list[0].file_grid, "%s/lc_Barrel_0-0_hermite32_1088x1088-1920x1080_fisheyes_mercator_0.bin", path);
#if 0
				mode_list->file_list[0].outport_number = 2;

				pos.x = 0; pos.y = 0; pos.width = 640; pos.height = 480;
				port.type = CAM_FCE_PORT_SS0;
				port.pos = pos;
				mode_list->file_list[0].outport_list[0] = port;

				pos.x = 0; pos.y = 0; pos.width = 1920; pos.height = 1080;
				port.type = CAM_FCE_PORT_DN;
				port.pos = pos;
				mode_list->file_list[0].outport_list[1] = port;
#endif
				break;
			}
		}

		camera_load_and_fire_fcedata(cam, &param);
#if 0
		sleep(2);
		camera_get_fce_mode(cam, &mode_index);
		printf("%s:%d mode_index=%d\n", __func__, __LINE__, mode_index);

		for (i = 0; i < param.mode_number; ++i)
		{
			camera_set_fce_mode(cam, i);
			sleep(2);
		}
#endif
}
#else
		printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_fce_mode(const char *cam, char *value)
{
    int ret = 0;
    int mode = 0;


    if ((NULL != cam) && (NULL != value)) {
		mode = atoi(value);
#if !TEST
{
		camera_set_fce_mode(cam, mode);
}
#else
		printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int get_camera_fce_mode(const char *cam)
{
    int ret = 0;
    int mode = 0;


    if ((NULL != cam)) {
#if !TEST
{
		camera_get_fce_mode(cam, &mode);
		printf("mode=%d \n", mode);
}
#else
		printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int get_camera_fce_totalmodes(const char *cam)
{
    int ret = 0;
    int mode = 0;


    if ((NULL != cam)) {
#if !TEST
{
		camera_get_fce_totalmodes(cam, &mode);
		printf("total modes=%d \n", mode);
}
#else
		printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_save_fcedata(const char *cam, char *value)
{
    int ret = 0;
    int mode = 0;
    char *path = NULL;

    if ((NULL != cam)) {
#if !TEST
{
		cam_save_fcedata_param_t save_param;

		path = value;
		if ('/' == path[strlen(path) - 1]) {
			path[strlen(path) - 1] = '\0';
		}
		sprintf(save_param.file_name, "%s/mygrid", value);
		camera_save_fcedata(cam, &save_param);
}
#else
		printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int get_camera_day_mode(const char *cam)
{
    int ret = 0;

    if ((NULL != cam)) {
#if !TEST
{
		enum cam_day_mode mode;


		ret = camera_get_day_mode(cam, &mode);
		printf("day_mode=%d \n", mode);
}
#else
		printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_index(const char *cam,  char *value)
{
    int ret = 0;
    int mode = 0;

    if ((NULL != cam) && (NULL != value)) {
		mode = atoi(value);
#if !TEST
{
		int index;

		index = mode;
		ret = camera_set_index(cam, index);
		if (-1 == ret) {
			CTRLVB_DBG("%s.......%d.......\n", __FUNCTION__, __LINE__);
			return VB_FAIL;
		}
}
#else
		printf("%s-----%s-----%s----\n", __FUNCTION__, cam, value);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int get_camera_index(const char *cam)
{
    int ret = 0;

    if ((NULL != cam)) {
#if !TEST
{
		int index;


		ret = camera_get_index(cam, &index);
		printf("index=%d \n", index);
}
#else
		printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_qts_4_grid(const char *cam, char *value)
{
    int ret = 0;
    char *path = NULL;

    if (NULL != cam) {
#if !TEST
{
		cam_fcefile_param_t param;
		cam_fcefile_mode_t *mode_list;
		cam_fcefile_file_t *file_list;
		cam_fcefile_port_t  port;
		cam_position_t  pos;
		int mode_nb = 1;
		int file_nb = 1;
		int mode_index = 0;
		int i = 0;
		int j = 0;

		path = value;
		if ('/' == path[strlen(path) - 1]) {
			path[strlen(path) - 1] = '\0';
		}
		memset(&param, 0, sizeof(param));

		param.mode_number = 1;
		mode_list = param.mode_list;
		for (i = 0; i < param.mode_number; ++i, mode_list++)
		{
			switch(i)
			{
			case 0:
				mode_list->file_number = 4;
				sprintf(mode_list->file_list[0].file_grid, "%s/lc_down_view_0-0_hermite32_1088x1088-960x544_fisheyes_downview_0.bin", path);
				sprintf(mode_list->file_list[1].file_grid, "%s/lc_down_view_0-0_hermite32_1088x1088-960x544_fisheyes_downview_1.bin", path);
				sprintf(mode_list->file_list[2].file_grid, "%s/lc_down_view_0-0_hermite32_1088x1088-960x544_fisheyes_downview_2.bin", path);
				sprintf(mode_list->file_list[3].file_grid, "%s/lc_down_view_0-0_hermite32_1088x1088-960x544_fisheyes_downview_3.bin", path);
				break;
#if 0
			case 1:
				mode_list->file_number = 2;

				sprintf(mode_list->file_list[0].file_grid, "/mnt/sd0/testcase_data/camera/data/lc_up_2p360_0-0_hermite32_1088x1088-1920x544_fisheyes_panorama_0.bin");

				sprintf(mode_list->file_list[1].file_grid, "/mnt/sd0/testcase_data/camera/data/lc_up_2p360_0-0_hermite32_1088x1088-1920x544_fisheyes_panorama_1.bin");
				break;
			case 2:
				mode_list->file_number = 1;
				sprintf(mode_list->file_list[0].file_grid, "/mnt/sd0/testcase_data/camera/data/lc_Barrel_0-0_hermite32_1088x1088-1920x1080_fisheyes_mercator_0.bin");
				break;
#endif
			}
		}

		camera_load_and_fire_fcedata(cam, &param);
}
#else
		printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

void KillVbctrl(int signo)
{
	system("killall vbctrl");
    exit(0);
}

typedef void (*sighandler_t)(int);
static int set_camera_loop_fisheye(const char *cam, char *value)
{
    int ret = 0;
    char *path = NULL;
    char json[300];

    if ((NULL != cam)) {
#if !TEST
{
		sighandler_t old_handler = NULL;

		path = value;
		if ('/' == path[strlen(path) - 1]) {
			path[strlen(path) - 1] = '\0';
		}
		old_handler = signal(SIGCHLD, SIG_DFL);
		system("videoboxd");
		signal(SIGCHLD, old_handler);

		signal(SIGINT, KillVbctrl); // ctrl-c
		signal(SIGTERM, KillVbctrl); // kill
		signal(SIGQUIT, KillVbctrl); // ctrl-slash

		while (1) {
			sprintf(json, "%s/ispost-dg-4-fisheye-uo.json", path);
			ret = videobox_repath(json);
			sleep(1);
			sprintf(json, "%s/ispost-dg-2-fisheye-uo.json", path);
			ret = videobox_repath(json);
			sleep(1);
			sprintf(json, "%s/ispost-dg-fisheye.json", path);
			ret = videobox_repath(json);
			sleep(1);
		}
}
#else
		printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int set_camera_dgov_fisheye(const char *cam, char *value)
{
    int ret = 0;
    char *path = NULL;

    if (NULL != cam) {
#if !TEST
{
		cam_fcefile_param_t param;
		cam_fcefile_mode_t *mode_list;
		cam_fcefile_file_t *file_list;
		cam_fcefile_port_t  port;
		cam_position_t  pos;
		int mode_nb = 1;
		int file_nb = 1;
		int mode_index = 0;
		int i = 0;
		int j = 0;

		path = value;
		if ('/' == path[strlen(path) - 1]) {
			path[strlen(path) - 1] = '\0';
		}
		memset(&param, 0, sizeof(param));

		param.mode_number = 1;
		mode_list = param.mode_list;
		for (i = 0; i < param.mode_number; ++i, mode_list++)
		{
			switch(i)
			{
			case 0:
				mode_list->file_number = 2;
				sprintf(mode_list->file_list[0].file_grid, "%s/lc_up_2p360_0-0_hermite32_1088x1088-1920x544_fisheyes_panorama_0.bin", path);
				sprintf(mode_list->file_list[1].file_grid, "%s/lc_up_2p360_0-0_hermite32_1088x1088-1920x544_fisheyes_panorama_1.bin", path);
				break;
			default:
				break;
			}
		}
		camera_load_and_fire_fcedata(cam, &param);

		struct v_pip_info info;

		memset(&info, 0, sizeof(info));
		sprintf(info.portname,"ov0");
		info.x = 200;
		info.y = 400;
		video_set_pip_info(cam, &info);
}
#else
		printf("%s-----%s-----\n", __FUNCTION__, cam);
#endif

    } else {
		return VB_FAIL;
    }

    return VB_OK;
}

static int test_photo_change_resolution(char * ps8Channel, char *ps8Path, int s32Count)
{
    // Note:jpeg-in need bind to isp-cap, test resolution cases restrict in real sensor's max width & height
    FILE *pstFile = NULL;
    struct fr_buf_info stJencBuf = FR_BUFINITIALIZER;
    vbres_t pstTestArray[3] =
            {
                {1920, 1080},
                {1280, 720},
                {640, 320}
            };
    char s8ArraySize = sizeof(pstTestArray)/sizeof(vbres_t);
    const int s32PortLength = 32;
    const int s32PathLength = 128;
    char ps8PortIn[s32PortLength], ps8PortOut[s32PortLength];
    char ps8PathName[s32PathLength];
    char *ps8Cam = "isp";
    char *ps8PortInSuffix = "-in";
    char *ps8PortOutSuffix = "-out";
    char *ps8Pos = NULL;
    int s32Width = 0, s32Height = 0, s32Offset = 0, s32ArraySeed = 0, s32Index = 0, s32Ret = 0;

    if (s32Count <= 0)
    {
        s32Count = 10;
    }

    if (ps8Channel == NULL || ps8Path == NULL)
    {
        return -1;
    }

    ps8Pos = strchr(ps8Channel, '-');

    if (ps8Pos)
    {
        s32Offset = ps8Pos - ps8Channel;
    }
    else
    {
        s32Offset = strlen(ps8Channel);
    }

    if (s32PortLength < (s32Offset + strlen(ps8PortOutSuffix) + 1))
    {
        printf("Error:channel name too long!\n");
        return -1;
    }

    snprintf(ps8PortIn, s32Offset + 1, "%s", ps8Channel);
    snprintf(ps8PortIn + s32Offset, strlen(ps8PortInSuffix) + 1, "%s", "-in");

    snprintf(ps8PortOut, s32Offset + 1, "%s", ps8Channel);
    snprintf(ps8PortOut + s32Offset, strlen(ps8PortOutSuffix) + 1, "%s", "-out");

    printf("ps8PortIn:%s ps8PortOut:%s path:%s count:%d\n",ps8PortIn, ps8PortOut, ps8Path, s32Count);

    while(s32Index < s32Count)
    {
        video_get_resolution(ps8PortIn, &s32Width, &s32Height);
        printf("Current resolution(%d, %d)-->",s32Width, s32Height);
        s32ArraySeed = random()%s8ArraySize;
        s32Width = pstTestArray[s32ArraySeed].w;
        s32Height = pstTestArray[s32ArraySeed].h;
        if (s32Width <= 0 || s32Height <= 0)
        {
            continue;
        }
        printf("Target resolution(%d, %d),Index:%d, s32ArraySeed:%d\n",s32Width, s32Height, s32Index, s32ArraySeed);
        video_set_resolution(ps8PortIn, s32Width, s32Height);

        s32Ret = camera_snap_one_shot(ps8Cam);
        if (s32Ret < 0)
        {
            printf("error:isp trigger picture fail!\n");
            return -1;
        }

        s32Ret = video_get_snap(ps8PortOut, &stJencBuf);
        if (s32Ret < 0)
        {
            printf("error:Jpeg encoder fail!\n");
            video_put_snap(ps8PortOut, &stJencBuf);
            camera_snap_exit(ps8Cam);
            return -1;
        }

        snprintf(ps8PathName, s32PathLength, "%s/Pic_%d_%d_%d.jpg",ps8Path, s32Index, s32Width, s32Height);
        pstFile = fopen(ps8PathName, "wb+");
        if (pstFile == NULL)
        {
            printf("error:creat %s fail!\n",ps8PathName);
            video_put_snap(ps8PortOut, &stJencBuf);
            camera_snap_exit(ps8Cam);
            return -1;
        }
        s32Ret = fwrite(stJencBuf.virt_addr, stJencBuf.size, 1, pstFile);
        if (s32Ret != 1)
        {
            printf("error:save %s fail!\n",ps8PathName);
            video_put_snap(ps8PortOut, &stJencBuf);
            camera_snap_exit(ps8Cam);
            fclose(pstFile);
            return -1;
        }

        video_put_snap(ps8PortOut, &stJencBuf);
        camera_snap_exit(ps8Cam);
        fclose(pstFile);

        usleep(50000);

        s32Index++;
    }

    return 0;
}
