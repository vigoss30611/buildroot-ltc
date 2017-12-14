#ifndef _H264_NALU_H_
#define _H264_NALU_H_

typedef struct tagSPS_PPS_VALUE
{
    int SPS_LEN;
	int PPS_LEN;
	int SEL_LEN;
	int SPS_PPS_BASE64_LEN;
	unsigned char SPS_VALUE[128];
	unsigned char PPS_VALUE[64];
	unsigned char SEI_VALUE[64];
	char SPS_PPS_BASE64[256];
	unsigned char profile_level_id[8];
}SPS_PPS_VALUE;

int base64encode_ex(const unsigned char *input, int input_length, unsigned char *output, int output_length);
void sprintf_hexa(char *s, unsigned char *p_data, int i_data);
#endif

