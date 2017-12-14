/*
sps,
pps
做BASE64编码时使用的函数
*/

char* MP4ToBase64(const HI_U8* pData, HI_U32 dataSize)
{
    if(pData==NULL||dataSize==0)
    {
        printf("MP4ToBase64 return NULL\n" );
        return NULL;
    }

	static const char encoding[64] = {
		'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
		'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
		'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
		'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
	};

	char* s = (char*)malloc((((dataSize + 2) * 4) / 3) + 1);

	const HI_U8* src = pData;
	char* dest = s;
	HI_U32 numGroups = dataSize / 3;

    HI_U32 i;
	for (i = 0; i < numGroups; i++) {
		*dest++ = encoding[src[0] >> 2];
		*dest++ = encoding[((src[0] & 0x03) << 4) | (src[1] >> 4)];
		*dest++ = encoding[((src[1] & 0x0F) << 2) | (src[2] >> 6)];
		*dest++ = encoding[src[2] & 0x3F];
		src += 3;
	}

	if (dataSize % 3 == 1) {
		*dest++ = encoding[src[0] >> 2];
		*dest++ = encoding[((src[0] & 0x03) << 4)];
		*dest++ = '=';
		*dest++ = '=';
	} else if (dataSize % 3 == 2) {
		*dest++ = encoding[src[0] >> 2];
		*dest++ = encoding[((src[0] & 0x03) << 4) | (src[1] >> 4)];
		*dest++ = encoding[((src[1] & 0x0F) << 2)];
		*dest++ = '=';
	}
	*dest = '\0';
	return s;	/* N.B. caller is responsible for free'ing s */
}
