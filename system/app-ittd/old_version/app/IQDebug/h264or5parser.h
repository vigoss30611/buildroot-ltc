
#ifndef _H264OR5PARSER_H_
#define _H264OR5PARSER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*    H264 I、P 帧，参数集     */
#define H264_I_TYPE 0x05
#define H264_P_TYPE 0x01
#define H264_SPS_TYPE 0x07
#define H264_PPS_TYPE 0x08
#define H264_SEI_TYPE 0x06

/*    H265 I、P 帧，参数集     */
#define H265_I_TYPE 0x26
#define H265_P_TYPE 0x02
#define H265_VPS_TYPE 0x40
#define H265_SPS_TYPE 0x42
#define H265_PPS_TYPE 0x44
#define H265_SEI_TYPE 0x4E

typedef struct {
	unsigned char u5Type:5;
	unsigned char:0;
} h264_type_t;

/*
	H265 解析SPS(获取高和宽)

	buf      [in]:
	nLen    [in]:
	Width   [out]:
	Height  [out]:
*/
int h265_decode_sps(char *sps, int len, int *width, int *height);


/*
	H264 解析SPS(获取高和宽)

	buf      [in]:
	nLen    [in]:
	Width   [out]:
	Height  [out]:
*/
int h264_decode_sps(char *sps, unsigned int nLen,int *width,int *height);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _H264OR5PARSER_H_ */
