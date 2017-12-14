/***************************************************************************** 
** 
** Copyright (c) 2012~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
**      
** Revision History: 
** ----------------- 
** v1.0.1	leo@2012/03/15: first commit.
** v1.3.1	leo@2012/05/15: add str2i() to support get integer from hex.
**
*****************************************************************************/ 

#include <InfotmMedia.h>


#define VALID_OPT(str)		((str[0] == (IM_TCHAR)'-')?IM_TRUE:IM_FALSE)
#define VALID_SHORT_OPT(str)	(((str[0] == (IM_TCHAR)'-') && (str[2] == (IM_TCHAR)'='))?IM_TRUE:IM_FALSE)
#define VALID_LONG_OPT(str)	(((str[0] == (IM_TCHAR)'-') && (str[1] == (IM_TCHAR)'-') && (str[2] != (IM_TCHAR)'-'))?IM_TRUE:IM_FALSE)


static IM_INT32 find_short_opt(IM_TCHAR match_c, IM_INT32 argc, IM_TCHAR *argv[], IM_TCHAR **str)
{
	IM_INT32 i;
	IM_TCHAR *pstr;

	for(i=0; i<argc; i++){
		pstr = argv[i];
		if(VALID_SHORT_OPT(pstr)){
			if(match_c == pstr[1]){
				*str = &(pstr[3]);
				return i;
			}
		}
	}

	return -1;
}

static IM_INT32 find_long_opt(IM_TCHAR *match_s, IM_INT32 argc, IM_TCHAR *argv[], IM_TCHAR **str)
{
	IM_INT32 i, j;
	IM_TCHAR *pstr, opt[64];

	for(i=0; i<argc; i++){
		pstr = argv[i];
		if(VALID_LONG_OPT(pstr)){
			j = 2;
			while(pstr[j] != (IM_TCHAR)'='){
				opt[j-2] = pstr[j];
				if(pstr[j] == (IM_TCHAR)'\0'){
					return -1;
				}
				j++;
			}
			
			opt[j-2] = (IM_TCHAR)'\0';
			if(strcmp(opt, match_s) == 0){
				j++;
				*str = &(pstr[j]);
				return i;
			}
		}
	}

	return -1;
}

static IM_INT32 str2i(IM_TCHAR *str)
{
	IM_INT32 i, val=0;
	
	if((str[0] == '0') && (str[1] == 'x')){
		for(i=2; str[i] != '\0'; i++){
			val <<= 4;
			if((str[i] >= '0') && (str[i] <= '9')){
				val += str[i] - '0';
			}else if((str[i] >= 'a') && (str[i] <= 'f')){
				val += str[i] - 'a' + 10;
			}else if((str[i] >= 'A') && (str[i] <= 'F')){
				val += str[i] - 'A' + 10;
			}else{
				val = 0;
				break;
			}
		}
	}else{
		val = atoi(str);
	}

	return val;
}

IM_RET argp_get_key_int(IN argp_key_table_entry_t *kte, OUT IM_INT32 *val, IN IM_INT32 argc, IN IM_TCHAR *argv[])
{
	IM_TCHAR *sopt_str=IM_NULL, *lopt_str=IM_NULL, *str;
	IM_INT32 sopt_index=-1, lopt_index=-1;
	
	if(kte->shortOpt != 0){
		sopt_index = find_short_opt(kte->shortOpt, argc, argv, &sopt_str);
	}
	if(kte->longOpt != IM_NULL){
		lopt_index = find_long_opt(kte->longOpt, argc, argv, &lopt_str);
	}

	if((sopt_index != -1) && (lopt_index != -1)){
		if(sopt_index < lopt_index){
			str = sopt_str; 
		}else{
			str = lopt_str;
		}
	}else if(sopt_index != -1){
		str = sopt_str; 
	}else if(lopt_index != -1){
		str = lopt_str;
	}else{
		return IM_RET_FAILED;
	}

	*val = str2i(str);

	return IM_RET_OK;

}

IM_RET argp_get_key_string(IN argp_key_table_entry_t *kte, OUT IM_TCHAR **str, IN IM_INT32 argc, IN IM_TCHAR *argv[])
{
	IM_TCHAR *sopt_str=IM_NULL, *lopt_str=IM_NULL;
	IM_INT32 sopt_index=-1, lopt_index=-1;
	
	if(kte->shortOpt != 0){
		sopt_index = find_short_opt(kte->shortOpt, argc, argv, &sopt_str);
	}
	if(kte->longOpt != IM_NULL){
		lopt_index = find_long_opt(kte->longOpt, argc, argv, &lopt_str);
	}

	if((sopt_index != -1) && (lopt_index != -1)){
		if(sopt_index < lopt_index){
			*str = sopt_str; 
		}else{
			*str = lopt_str;
		}
	}else if(sopt_index != -1){
		*str = sopt_str; 
	}else if(lopt_index != -1){
		*str = lopt_str;
	}else{
		return IM_RET_FAILED;
	}

	return IM_RET_OK;
}



