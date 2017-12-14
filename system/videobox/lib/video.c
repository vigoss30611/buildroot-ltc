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

#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <qsdk/videobox.h>

static int remove_header(void *data, int len)
{
#define H264_SPS_BIT 0x7
#define H264_PPS_BIT 0x8
#define H264_IDR_BIT 0x5
#define H265_HEAD_BIT 0x40
#define H265_IDR_BIT 0x26
#define ALIGN_NAL_SIZE (8+5)
#define HEADER_MAX_SIZE 128

    int offset = 0, num = 0, head_len = 0;;
    unsigned char *ptr = NULL;

    ptr = data;
    head_len = HEADER_MAX_SIZE;

    if(len < head_len || len < ALIGN_NAL_SIZE)
        return 0;

    while(num < ALIGN_NAL_SIZE){
        if((*ptr==0x0) && (*(ptr+1)==0x0) && (*(ptr+2)==0x1) && (*(ptr+3) == H265_HEAD_BIT)){
            head_len -= (num + 4);
            while(head_len--){
                ptr++;
                if((*ptr==0x0) && (*(ptr+1)==0x0) && (*(ptr+2)==0x1) && (*(ptr+3) == H265_IDR_BIT)){
                    offset = abs((int)ptr - (int)data);
                    if(!(*(ptr-1)))
                        offset--;
                    return offset;
                }
            }
            if(head_len <= 0)
                return 0;
        }else if(*(ptr)==0x0 && *(ptr+1)==0x0 && *(ptr+2)==0x1 && ((*(ptr+3)&H264_PPS_BIT) || (*(ptr+3)&H264_SPS_BIT))){
            head_len -= (num + 4);
            while(head_len--){
                ptr++;
                if(*(ptr)==0x0 && *(ptr+1)==0x0 && *(ptr+2)==0x1 && (*(ptr+3)&H264_IDR_BIT)){
                    offset = abs((int)ptr - (int)data);
                    if(!(*(ptr-1)))
                        offset--;
                    return offset;
                }
            }
            if(head_len <= 0)
                return 0;
        }
        ptr++;
        num ++;
    }

    return 0;
}

const char **video_get_channels(void)
{
    return NULL;
}

int video_enable_channel(const char *chn)
{
    videobox_launch_rpc_l(chn, NULL, 0);
    return _rpc.ret;
}

int video_disable_channel(const char *chn)
{
    videobox_launch_rpc_l(chn, NULL, 0);
    return _rpc.ret;
}

int video_get_basicinfo(const char *chn, struct v_basic_info *info)
{
    videobox_launch_rpc(chn, info);
    return _rpc.ret;
}

int video_set_basicinfo(const char *chn, struct v_basic_info *info)
{
    videobox_launch_rpc(chn, info);
    return _rpc.ret;
}

int video_get_bitrate(const char *chn, struct v_bitrate_info *info)
{
    videobox_launch_rpc(chn, info);
    return _rpc.ret;
}

int video_set_bitrate(const char *chn, struct v_bitrate_info *info)
{
    videobox_launch_rpc(chn, info);
    return _rpc.ret;
}

int video_get_ratectrl(const char *chn, struct v_rate_ctrl_info *info)
{
    videobox_launch_rpc(chn, info);
    return _rpc.ret;
}

int video_get_rtbps(const char *chn)
{
    videobox_launch_rpc_l(chn, NULL, 0);
    return _rpc.ret;
}

int video_set_ratectrl(const char *chn, struct v_rate_ctrl_info *info)
{
    videobox_launch_rpc(chn, info);
    return _rpc.ret;
}

int video_set_ratectrl_ex(const char *chn, struct v_rate_ctrl_info *info)
{
    videobox_launch_rpc(chn, info);
    return _rpc.ret;
}

int video_test_frame(const char *chn, struct fr_buf_info *ref)
{
    return fr_test_new_by_name(chn, ref);
}

int video_get_frame(const char *chn, struct fr_buf_info *ref)
{
    return fr_get_ref_by_name(chn, ref);
}

int video_put_frame(const char *chn, struct fr_buf_info *ref)
{
    return fr_put_ref(ref);
}

int video_write_frame(const char *chn, void *data, int len)
{
    struct fr_buf_info buf = FR_BUFINITIALIZER;
    int ret, offset = 0;

    ret = fr_get_buf_by_name(chn, &buf);
    if(ret < 0) return ret;

    if(len > buf.map_size) {
        printf("%s: size exceel limit: (0x%x > 0x%x)\n",
            __func__, len, buf.size);
        ret = VBEBADPARAM;
    } else{
        offset = remove_header(data, len);
        if (offset < 0 || offset >= len)
        {
            buf.size = 0;
            fr_put_buf(&buf);
            printf("%s error, offset:%d, len:%d\n",
                __func__, offset, len);
            return -1;
        }
        if(offset > 0)
        {
            memcpy(buf.virt_addr, data, offset);
            buf.size = offset;
            fr_put_buf(&buf);
            ret = fr_get_buf_by_name(chn, &buf);
            if(ret < 0)
            {
                return ret;
            }
            else if(len > buf.map_size)
            {
                printf("%s: size exceel limit: (0x%x > 0x%x)\n", __func__, len, buf.size);
                fr_put_buf(&buf);
                return VBEBADPARAM;
            }
        }
        memcpy(buf.virt_addr, data + offset, len - offset);
        buf.size = len - offset;
    }

    fr_put_buf(&buf);
    return ret;
}

int video_get_fps(const char *chn)
{
    videobox_launch_rpc_l(chn, NULL, 0);
    return _rpc.ret;
}

int video_set_fps(const char *chn, int fps)
{
    videobox_launch_rpc(chn, &fps);
    return _rpc.ret;
}

int video_get_resolution(const char *chn, int *w, int *h)
{
    vbres_t res;

    videobox_launch_rpc(chn, &res);
    *w = res.w;
    *h = res.h;
    return _rpc.ret;
}

int video_set_resolution(const char *chn, int w, int h)
{
    vbres_t res = { .w = w, .h = h};

    videobox_launch_rpc(chn, &res);
    return _rpc.ret;
}

int video_get_roi_count(const char *chn)
{
    videobox_launch_rpc_l(chn, NULL, 0);
    return _rpc.ret;
}

int video_get_roi(const char *chn, struct v_roi_info *roi)
{
    videobox_launch_rpc(chn, roi);
    return _rpc.ret;
}

int video_set_roi(const char *chn, struct v_roi_info *roi)
{
    videobox_launch_rpc(chn, roi);
    return _rpc.ret;
}

int video_set_snap_quality(const char *chn, int qlevel)
{
    videobox_launch_rpc(chn, &qlevel);
    return _rpc.ret;
}

int video_get_thumbnail(const char *chn, struct fr_buf_info *ref)
{
    videobox_launch_rpc_l(chn, NULL, 0);
    if(_rpc.ret >= 0) {
        fr_INITBUF(ref, NULL);
        return fr_get_ref_by_name(chn, ref);
    } else {
        printf("err:can not get thumbnail!\n");
    }
    return _rpc.ret;
}

int video_put_thumbnail(const char *chn, struct fr_buf_info *ref)
{
    int ret = fr_put_ref(ref);
    printf("video_put_thumbnail ret:%d\n",ret);
    return ret;
}

int video_get_snap(const char *chn, struct fr_buf_info *ref)
{
    videobox_launch_rpc_l(chn, NULL, 0);
    if(_rpc.ret >= 0) {
        fr_INITBUF(ref, NULL);
        return fr_get_ref_by_name(chn, ref);
    }
    return _rpc.ret;
}

int video_set_snap(const char *chn, struct v_pip_info *pip_info)
{
    videobox_launch_rpc(chn, pip_info);
    return _rpc.ret;
}

int video_put_snap(const char *chn, struct fr_buf_info *ref)
{
    return fr_put_ref(ref);
}

int video_get_header(const char *chn, char *buf, int len)
{
    if(len < VIDEO_HEADER_MAXLEN) {
        printf("buffer size %d too small to hold header\n", len);
        return -1;
    }

    videobox_launch_rpc_l(chn, buf, len);
    return _rpc.ret;
}

int video_set_header(const char *chn, char *buf, int len)
{
    struct v_header_info str_header;

    memcpy(str_header.header, buf, len);
    str_header.headerLen = len;

    videobox_launch_rpc(chn, &str_header);
    return _rpc.ret;
}

int video_trigger_key_frame(const char *chn)
{
    videobox_launch_rpc_l(chn, NULL, 0);
    return _rpc.ret;
}

int video_set_scenario(const char *channel, enum video_scenario mode)
{
    videobox_launch_rpc(channel, &mode);
    return _rpc.ret;
}

int video_set_pip_info(const char *chn, struct v_pip_info *pip_info)
{
    videobox_launch_rpc(chn, pip_info);
    return _rpc.ret;
}

int video_get_pip_info(const char *chn, struct v_pip_info *pip_info)
{
    videobox_launch_rpc(chn, pip_info);
    return _rpc.ret;
}

enum video_scenario video_get_scenario(const char *channel)
{
    videobox_launch_rpc_l(channel, NULL,0);
    return _rpc.ret;
}

int video_set_frame_mode(const char *channel, enum v_enc_frame_type mode)
{
    videobox_launch_rpc(channel, &mode);
    return _rpc.ret;
}

enum v_enc_frame_type video_get_frame_mode(const char *channel)
{
    videobox_launch_rpc_l(channel, NULL, 0);
    return _rpc.ret;
}

int va_qr_enable(const char *chn)
{
    videobox_launch_rpc_l(chn, NULL, 0);
    return _rpc.ret;
}

int va_qr_disable(const char *chn)
{
    videobox_launch_rpc_l(chn, NULL, 0);
    return _rpc.ret;
}

int va_qr_set_density(const char *chn, int x, int y)
{
    density_t dsty = { .x = x, .y = y};

    videobox_launch_rpc(chn, &dsty);
    return _rpc.ret;
}

int va_qr_get_density(const char *chn, int *x, int *y)
{
    density_t dsty;

    videobox_launch_rpc(chn, &dsty);
    *x = dsty.x;
    *y = dsty.y;

    return _rpc.ret;
}
int video_set_frc(const char *channel, struct v_frc_info *frc){
    videobox_launch_rpc(channel, frc);
    return _rpc.ret;
}
int video_get_frc(const char *channel, struct v_frc_info *frc){
    videobox_launch_rpc(channel, frc);
    return _rpc.ret;
}
int video_set_nvr_mode(const char *channel,struct v_nvr_info *nvr_mode)
{
    videobox_launch_rpc(channel, nvr_mode);
    return _rpc.ret;
}

int thumbnail_set_resolution(const char *chn, int w, int h)
{
    vbres_t res = { .w = w, .h = h};

    videobox_launch_rpc(chn, &res);
    return _rpc.ret;
}

int video_set_sei_user_data(const char *ps8Chn, char *ps8Buf, int s32Len, EN_SEI_TRIGGER_MODE enMode)
{
    ST_VIDEO_SEI_USER_DATA_INFO stSei;
    char ps8TempBuf[128] = {0};

    if ((s32Len < 0) ||
        (s32Len > VIDEO_SEI_USER_DATA_VALIDATE_LEN) ||
        (ps8Buf == NULL))
    {
        printf("data %p error length %d not in (%d, %d)\n",
            ps8Buf,
            s32Len,
            0,
            VIDEO_SEI_USER_DATA_VALIDATE_LEN);
        return -1;
    }
    memset(&stSei, 0, sizeof(stSei));
    snprintf(stSei.seiUserData, VIDEO_SEI_USER_DATA_UUID, "%s", "infotm UserData");
    memcpy(stSei.seiUserData + VIDEO_SEI_USER_DATA_UUID, ps8Buf, s32Len);
    stSei.length = s32Len + VIDEO_SEI_USER_DATA_UUID;
    stSei.enMode = enMode;

    sprintf(ps8TempBuf, "%s#%s", ps8Chn, __FUNCTION__);
    return videobox_exchange_data("ipu_data", (char *)ps8TempBuf, (char *)&stSei, sizeof(stSei));
}

int video_set_slice_height(const char *ps8Chn, int s32SliceHeight)
{
    videobox_launch_rpc(ps8Chn, &s32SliceHeight);
    return _rpc.ret;
}

int video_get_slice_height(const char *ps8Chn, int *ps32SliceHeight)
{
    videobox_launch_rpc(ps8Chn, ps32SliceHeight);
    return _rpc.ret;
}

int video_set_panoramic(const char *ps8Chn, int s32Enable)
{
    videobox_launch_rpc(ps8Chn, &s32Enable);
    return _rpc.ret;
}

EN_VIDEOBOX_RET videobox_ipu_set_property(const char *ps8IpuID, EN_VIDEOBOX_PROPERTY enProperty, void *pPropertyInfo, int s32Size) {
    ST_VIDEOBOX_ARGS_INFO info = {.enProperty = enProperty, .PropertyInfo = {0}};
    memcpy(info.PropertyInfo, (char*)pPropertyInfo, s32Size);
    videobox_launch_rpc(ps8IpuID, &info);
    return _rpc.ret;
}

EN_VIDEOBOX_RET videobox_ipu_get_property(const char *ps8IpuID, EN_VIDEOBOX_PROPERTY enProperty, void *pPropertyInfo, int s32Size) {
    ST_VIDEOBOX_ARGS_INFO info = {.enProperty = enProperty, .PropertyInfo = {0}};
    videobox_launch_rpc(ps8IpuID, &info);
    memcpy((char*)pPropertyInfo, info.PropertyInfo, s32Size);
    return _rpc.ret;
}

EN_VIDEOBOX_RET videobox_ipu_set_attribute(const char *ps8IpuID, const char *ps8PortName, ST_VIDEOBOX_PORT_ATTRIBUTE *pstPortAttribute) {
    ST_VIDEOBOX_PORT_INFO info = {.s8PortName = {0}, .stPortAttribute = *pstPortAttribute};
    memcpy(info.s8PortName, ps8PortName, strlen(ps8PortName));
    videobox_launch_rpc(ps8IpuID, &info);
    return _rpc.ret;
}

EN_VIDEOBOX_RET videobox_ipu_get_attribute(const char *ps8IpuID, const char *ps8PortName, ST_VIDEOBOX_PORT_ATTRIBUTE *pstPortAttribute) {
    ST_VIDEOBOX_PORT_INFO info = {.s8PortName = {0}, .stPortAttribute = {0}};
    memcpy(info.s8PortName, ps8PortName, strlen(ps8PortName));
    videobox_launch_rpc(ps8IpuID, &info);
    *pstPortAttribute = info.stPortAttribute;
    return _rpc.ret;
}

EN_VIDEOBOX_RET videobox_ipu_bind(const char *ps8SrcIpuID, const char *ps8SrcPortName, const char *ps8SinkIpuID, const char *ps8SinkPortName) {
    ST_VIDEOBOX_BIND_EMBEZZLE_INFO info = {.s8SrcPortName = {0}, .s8SinkIpuID = {0}, .s8SinkPortName = {0}};
    memcpy(info.s8SrcPortName, ps8SrcPortName, strlen(ps8SrcPortName));
    memcpy(info.s8SinkIpuID, ps8SinkIpuID, strlen(ps8SinkIpuID));
    memcpy(info.s8SinkPortName, ps8SinkPortName, strlen(ps8SinkPortName));
    videobox_launch_rpc(ps8SrcIpuID, &info);
    return _rpc.ret;
}

EN_VIDEOBOX_RET videobox_ipu_unbind(const char *ps8SrcIpuID, const char *ps8SrcPortName, const char *ps8SinkIpuID, const char *ps8SinkPortName) {
    ST_VIDEOBOX_BIND_EMBEZZLE_INFO info = {.s8SrcPortName = {0}, .s8SinkIpuID = {0}, .s8SinkPortName = {0}};
    memcpy(info.s8SrcPortName, ps8SrcPortName, strlen(ps8SrcPortName));
    memcpy(info.s8SinkIpuID, ps8SinkIpuID, strlen(ps8SinkIpuID));
    memcpy(info.s8SinkPortName, ps8SinkPortName, strlen(ps8SinkPortName));
    videobox_launch_rpc(ps8SrcIpuID, &info);
    return _rpc.ret;
}

EN_VIDEOBOX_RET videobox_ipu_embezzle(const char *ps8SrcIpuID, const char *ps8SrcPortName, const char *ps8SinkIpuID, const char *ps8SinkPortName) {
    ST_VIDEOBOX_BIND_EMBEZZLE_INFO info = {.s8SrcPortName = {0}, .s8SinkIpuID = {0}, .s8SinkPortName = {0}};
    memcpy(info.s8SrcPortName, ps8SrcPortName, strlen(ps8SrcPortName));
    memcpy(info.s8SinkIpuID, ps8SinkIpuID, strlen(ps8SinkIpuID));
    memcpy(info.s8SinkPortName, ps8SinkPortName, strlen(ps8SinkPortName));
    videobox_launch_rpc(ps8SrcIpuID, &info);
    return _rpc.ret;
}

EN_VIDEOBOX_RET videobox_ipu_unembezzle(const char *ps8SrcIpuID, const char *ps8SrcPortName, const char *ps8SinkIpuID, const char *ps8SinkPortName) {
    ST_VIDEOBOX_BIND_EMBEZZLE_INFO info = {.s8SrcPortName = {0}, .s8SinkIpuID = {0}, .s8SinkPortName = {0}};
    memcpy(info.s8SrcPortName, ps8SrcPortName, strlen(ps8SrcPortName));
    memcpy(info.s8SinkIpuID, ps8SinkIpuID, strlen(ps8SinkIpuID));
    memcpy(info.s8SinkPortName, ps8SinkPortName, strlen(ps8SinkPortName));
    videobox_launch_rpc(ps8SrcIpuID, &info);
    return _rpc.ret;
}

EN_VIDEOBOX_RET videobox_create_path(void) {
    int ret = system("videoboxd");
    if(ret == -1 || ret == 127 || ret == 126) { //for bin or shell fail
        return -1;
    }
    ret = WEXITSTATUS(ret);
    return ret > 127 ? ret | (~0xFF) : ret; //set high bit if the number is negtive number
}

EN_VIDEOBOX_RET videobox_delete_path(void) {
    videobox_launch_rpc_l(NULL, NULL, 0);
    return _rpc.ret;
}

EN_VIDEOBOX_RET videobox_start_path(void) {
    videobox_launch_rpc_l(NULL, NULL, 0);
    return _rpc.ret;
}

EN_VIDEOBOX_RET videobox_stop_path(void) {
    videobox_launch_rpc_l(NULL, NULL, 0);
    return _rpc.ret;
}

EN_VIDEOBOX_RET videobox_jsonshow(void) {
    videobox_launch_rpc_l(NULL, NULL, 0);
    return _rpc.ret;
}

EN_VIDEOBOX_RET videobox_trigger(void) {
    videobox_launch_rpc_l(NULL, NULL, 0);
    return _rpc.ret;
}

EN_VIDEOBOX_RET videobox_suspend(void) {
    videobox_launch_rpc_l(NULL, NULL, 0);
    return _rpc.ret;
}

EN_VIDEOBOX_RET videobox_ipu_create(const char *ps8IpuID) {
    videobox_launch_rpc_l(ps8IpuID, NULL, 0);
    return _rpc.ret;
}

EN_VIDEOBOX_RET videobox_ipu_delete(const char *ps8IpuID) {
    videobox_launch_rpc_l(ps8IpuID, NULL, 0);
    return _rpc.ret;
}
