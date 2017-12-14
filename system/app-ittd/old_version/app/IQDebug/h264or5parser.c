

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

typedef struct {
	long width, height;
	unsigned int profile, level;
	unsigned int nal_length_size;
}vc_params_t;

static unsigned char * m_data;
static int m_len;
static int m_idx;
static int m_bits;
static unsigned char m_byte;
static int m_zeros;

static unsigned char GetBYTE()
{
	unsigned char b;
	if ( m_idx >= m_len )
		return 0;
	b = m_data[m_idx++];
	// to avoid start-code emulation, a byte 0x03 is inserted
	// after any 00 00 pair. Discard that here.
	if ( b == 0 )
	{
		m_zeros++;
		if ( (m_idx < m_len) && (m_zeros == 2) && (m_data[m_idx] == 0x03) )
		{
			m_idx++;
			m_zeros=0;
		}
	}
	else
	{
		m_zeros = 0;
	}
	return b;
};

static unsigned int GetBit()
{
	if (m_bits == 0) 
	{
		m_byte = GetBYTE();
		m_bits = 8;
	}
	m_bits--;
	return (m_byte >> m_bits) & 0x1;
}

static unsigned int GetWord(int bits)
{
	unsigned int u = 0;
	while ( bits > 0 )
	{
		u <<= 1;
		u |= GetBit();
		bits--;
	}
	return u;
}

static unsigned int GetUE()
{
	// Exp-Golomb entropy coding: leading zeros, then a one, then
	// the data bits. The number of leading zeros is the number of
	// data bits, counting up from that number of 1s as the base.
	// That is, if you see
	// ? ? ?0001010
	// You have three leading zeros, so there are three data bits (010)
	// counting up from a base of 111: thus 111 + 010 = 1001 = 9
	int zeros = 0;
	while (m_idx < m_len && GetBit() == 0 ) zeros++;
	return GetWord(zeros) + ((1 << zeros) - 1);
}

static int GetSE()
{
	// same as UE but signed.
	// basically the unsigned numbers are used as codes to indicate signed numbers in pairs
	// in increasing value. Thus the encoded values
	// ? ? ?0, 1, 2, 3, 4
	// mean
	// ? ? ?0, 1, -1, 2, -2 etc
	unsigned int UE = GetUE();
	bool positive = UE & 1;
	int SE = (UE + 1) >> 1;
	if ( !positive )
	{
		SE = -SE;
	}
	return SE;
};

static void Init(unsigned char *data, int len)
{
	m_data = data; 
	m_len = len; 
	m_idx = 0; 
	m_bits = 0; 
	m_byte = 0; 
	m_zeros = 0; 
};

/*
	H265 解析SPS(获取高和宽)

	buf      [in]:
	nLen    [in]:
	Width   [out]:
	Height  [out]:
*/
int h265_decode_sps(char *sps, int len, int *width, int *height)
{
	int i, sps_max_sub_layers_minus1;
	unsigned int sps_seq_parameter_set_id;
	unsigned int chroma_format_idc;
	unsigned int bit_depth_luma_minus8;
	unsigned int bit_depth_chroma_minus8;
	if (len < 20)
	{
		return -1;
	}
	Init((unsigned char *)sps,  len);
	
	GetWord(4);// sps_video_parameter_set_id
	
	sps_max_sub_layers_minus1 = GetWord(3); // "The value of sps_max_sub_layers_minus1 shall be in the range of 0 to 6, inclusive."
	if (sps_max_sub_layers_minus1 > 6)
	{
		return -2;
	}
	GetWord(1);// sps_temporal_id_nesting_flag
	// profile_tier_level( sps_max_sub_layers_minus1 )
	{
		unsigned char sub_layer_profile_present_flag[6] = {0};
		unsigned char sub_layer_level_present_flag[6]  = {0};
		GetWord(2);// general_profile_space
		GetWord(1);// general_tier_flag
		GetWord(5);// general_profile_idc
		GetWord(32);// general_profile_compatibility_flag[32]
		GetWord(1);// general_progressive_source_flag
		GetWord(1);// general_interlaced_source_flag
		GetWord(1);// general_non_packed_constraint_flag
		GetWord(1);// general_frame_only_constraint_flag
		GetWord(44);// general_reserved_zero_44bits
		GetWord(8);// general_level_idc
		for (i = 0; i < sps_max_sub_layers_minus1; i++) 
		{
			sub_layer_profile_present_flag[i] = GetWord(1);
			sub_layer_level_present_flag[i]   = GetWord(1);
		}
		if (sps_max_sub_layers_minus1 > 0) 
		{
			for (i = sps_max_sub_layers_minus1; i < 8; i++) 
			{
				GetWord(2); /* reserved_zero_2bits */
			}
		}
		for (i = 0; i < sps_max_sub_layers_minus1; i++) 
		{
			if (sub_layer_profile_present_flag[i])
			{
				GetWord(2);// sub_layer_profile_space[i]
				GetWord(1);// sub_layer_tier_flag[i]
				GetWord(5);// sub_layer_profile_idc[i]
				GetWord(32);// sub_layer_profile_compatibility_flag[i][32]
				GetWord(1);// sub_layer_progressive_source_flag[i]
				GetWord(1);// sub_layer_interlaced_source_flag[i]
				GetWord(1);// sub_layer_non_packed_constraint_flag[i]
				GetWord(1);// sub_layer_frame_only_constraint_flag[i]
				GetWord(44);// sub_layer_reserved_zero_44bits[i]
			}
			if (sub_layer_level_present_flag[i]) 
			{
				GetWord(8);// sub_layer_level_idc[i]
			}
		}
	}
	sps_seq_parameter_set_id = GetUE(); // "The ?value ?of sps_seq_parameter_set_id shall be in the range of 0 to 15, inclusive."
	if (sps_seq_parameter_set_id > 15)
	{
		return -3;
	}
	chroma_format_idc = GetUE(); // "The value of chroma_format_idc shall be in the range of 0 to 3, inclusive."
	if (sps_seq_parameter_set_id > 3)
	{
		return -4;
	}
	if (chroma_format_idc == 3) 
	{
		GetWord(1);// separate_colour_plane_flag
	}
	*width  = GetUE(); // pic_width_in_luma_samples
	*height  = GetUE(); // pic_height_in_luma_samples
	if (GetWord(1))
	{// conformance_window_flag
		GetUE();  // conf_win_left_offset
		GetUE();  // conf_win_right_offset
		GetUE();  // conf_win_top_offset
		GetUE();  // conf_win_bottom_offset
	}
	bit_depth_luma_minus8 = GetUE();
	bit_depth_chroma_minus8 = GetUE();
	if (bit_depth_luma_minus8 != bit_depth_chroma_minus8)
	{
		return -4;
	}
	//...

	return 0;
}

static int Ue(char *pBuff, unsigned int nLen, unsigned int *nStartBit)
{
	/*计算0bit的个数*/
	int i,dwRet = 0;	
	unsigned int nZeroNum = 0,curr = *nStartBit;
	
	while (curr < nLen * 8)
	{
		if (pBuff[curr / 8] & (0x80 >> (curr % 8))) /*&:按位与，%取余*/
		{
			break;
		}
		nZeroNum++;
		curr++;
	}
	curr ++;

	/*计算结果*/

	for (i=0; i<nZeroNum; i++)
	{
		dwRet <<= 1;
		if (pBuff[curr / 8] & (0x80 >> (curr % 8)))
		{
			dwRet += 1;
		}
		curr++;
	}
	*nStartBit = curr;
	
	return (1 << nZeroNum) - 1 + dwRet;
}


static int Se(char *pBuff, unsigned int nLen, unsigned int *nStartBit)
{
	int UeVal = Ue(pBuff,nLen,nStartBit);
	double k = UeVal;
	int nValue = ceil(k/2);/*ceil函数：ceil函数的作用是求不小于给定实数的最小整数。ceil(2)=ceil(1.2)=cei(1.5)=2.00*/

	if ((UeVal % 2) ==0)
		nValue = -nValue;
	
	return nValue;
}


static int u(unsigned int BitCount,char * buf,unsigned int *nStartBit)
{
	int i,dwRet = 0;
	unsigned int curr = *nStartBit;
	
	for (i=0; i<BitCount; i++)
	{
		dwRet <<= 1;
		if (buf[curr / 8] & (0x80 >> (curr % 8)))
		{
			dwRet += 1;
		}
		curr++;
	}
	*nStartBit = curr;
	
	return dwRet;
}


/*
	H264 解析SPS(获取高和宽)

	buf      [in]:
	nLen    [in]:
	Width   [out]:
	Height  [out]:
*/
int h264_decode_sps(char *sps, unsigned int nLen,int *width,int *height)
{
	int i, r = -1;
	unsigned int StartBit=0; 
	int nal_unit_type;

	u(1,sps,&StartBit); /* forbidden_zero_bit */
	u(2,sps,&StartBit); /* nal_ref_idc */
	nal_unit_type = u(5,sps,&StartBit); /* nal_unit_type */
	if (nal_unit_type == 7)
	{
		int pic_order_cnt_type;
		int pic_width_in_mbs_minus1;
		int pic_height_in_map_units_minus1;
		int profile_idc = u(8,sps,&StartBit); /* profile_idc */
		u(1,sps,&StartBit);/*(sps[1] & 0x80)>>7; constraint_set0_flag */
		u(1,sps,&StartBit); /* (sps[1] & 0x40)>>6; constraint_set1_flag */
		u(1,sps,&StartBit);/*(sps[1] & 0x20)>>5; constraint_set2_flag */
		u(1,sps,&StartBit);/*(sps[1] & 0x10)>>4; constraint_set3_flag */
		u(4,sps,&StartBit); /* reserved_zero_4bits */
		u(8,sps,&StartBit); /* level_idc */
		Ue(sps,nLen,&StartBit); /* seq_parameter_set_id */

		if (profile_idc == 100 || profile_idc == 110 ||
			profile_idc == 122 || profile_idc == 144 )
		{
			int seq_scaling_matrix_present_flag;
			int chroma_format_idc = Ue(sps,nLen,&StartBit);
			if ( chroma_format_idc == 3 )
			{
				 u(1,sps,&StartBit); /* residual_colour_transform_flag */
			}
			Ue(sps,nLen,&StartBit); /* bit_depth_luma_minus8 */
			Ue(sps,nLen,&StartBit); /* bit_depth_chroma_minus8 */
			u(1,sps,&StartBit); /* qpprime_y_zero_transform_bypass_flag */
			seq_scaling_matrix_present_flag = u(1,sps,&StartBit); /* seq_scaling_matrix_present_flag */
			if( seq_scaling_matrix_present_flag )
			{
				int seq_scaling_list_present_flag[12];
				for(i=0; i<((chroma_format_idc != 3)?8:12); i++)
				{
					seq_scaling_list_present_flag[i] = u(1,sps,&StartBit);
					if (seq_scaling_list_present_flag[i])
					{
						int j,tmp;
						const int size_of_scaling_list = (i < 6 ) ? 16 : 64;
						int lastscale = 8;
						int nextscale = 8;
						for (j = 0; j < size_of_scaling_list; j++ )
						{
							if (nextscale != 0 )
							{
								tmp = Se(sps, nLen,&StartBit);
								nextscale = (lastscale + tmp + 256 ) % 256;
							}
							lastscale = (nextscale == 0 ) ? lastscale : nextscale;
						}
					}
				}
			}
		}
		Ue(sps,nLen,&StartBit); /* log2_max_frame_num_minus4 */
		pic_order_cnt_type = Ue(sps,nLen,&StartBit); /* pic_order_cnt_type */
		if ( pic_order_cnt_type == 0 )
		{
			Ue(sps,nLen,&StartBit); /* log2_max_pic_order_cnt_lsb_minus4 */
		}
		else if ( pic_order_cnt_type == 1 )
		{
			int num_ref_frames_in_pic_order_cnt_cycle;
			u(1,sps,&StartBit); /* delta_pic_order_always_zero_flag */
			Se(sps,nLen,&StartBit); /* offset_for_non_ref_pic */
			Se(sps,nLen,&StartBit); /* offset_for_top_to_bottom_field */
			num_ref_frames_in_pic_order_cnt_cycle = Ue(sps,nLen,&StartBit); /* num_ref_frames_in_pic_order_cnt_cycle */
			//if (num_ref_frames_in_pic_order_cnt_cycle >= 0)
			{
				//int offset_for_ref_frame[num_ref_frames_in_pic_order_cnt_cycle];
				for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
				{
					Se(sps,nLen,&StartBit); /* offset_for_ref_frame[i] */
				}
			}
		}
		Ue(sps,nLen,&StartBit); /* num_ref_frames */
		u(1,sps,&StartBit); /* gaps_in_frame_num_value_allowed_flag */
		pic_width_in_mbs_minus1 = Ue(sps,nLen,&StartBit);  /* pic_width_in_mbs_minus1 */
		pic_height_in_map_units_minus1 = Ue(sps,nLen,&StartBit); /* pic_height_in_map_units_minus1 */

		*width = (pic_width_in_mbs_minus1+1)*16;
		*height = (pic_height_in_map_units_minus1+1)*16;
		r = 0;
	}

	return r;
}
