/******************************************************************************
******************************************************************************/

#if !defined(AFX_TMFILE_FORMAT_H__)
#define AFX_TMFILE_FORMAT_H__

#define MAXLEN_STREAMFORMAT	128	//use in STREAMDATA.private
#define MAX_CHANNEL_NAME_LEN	32
#define MAX_VIDEO_NAME_LEN				60


#define		XVIDFLAG	0XB2010000
#define		DIVXFLAG	0X00000001
#define		INDEXFLAG	0XDDDDDDDD

#define		TM4KFLAG	0x4b344d54  // TM4K

typedef unsigned char u_int8;
typedef unsigned short u_int16;
typedef unsigned int u_int32;

typedef struct screen_format{

}SCREENFORMAT;

typedef struct tagBITMAPINFOHEADER{
	DWORD      biSize;
	LONG       biWidth;
	LONG       biHeight;
	WORD       biPlanes;
	WORD       biBitCount;
	DWORD      biCompression;
	DWORD      biSizeImage;
	LONG       biXPelsPerMeter;
	LONG       biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
} BITMAPINFOHEADER;

typedef struct tWAVEFORMATEX
{
    WORD    wFormatTag;        /* format type */
    WORD    nChannels;         /* number of channels (i.e. mono, stereo...) */
    DWORD   nSamplesPerSec;    /* sample rate */
    DWORD   nAvgBytesPerSec;   /* for buffer estimation */
    WORD    nBlockAlign;       /* block size of data */
    WORD    wBitsPerSample;    /* Number of bits per sample of mono data */
    WORD    cbSize;            /* The count in bytes of the size of
                                   extra information (after cbSize) */
} WAVEFORMATEX;

typedef struct video_format{
	u_int16 wLength;
	u_int8	cFps;
	BITMAPINFOHEADER	bmpHeader;
	char	VideoName[MAX_VIDEO_NAME_LEN];
} VIDEOFORMAT;

typedef struct audio_format{
	u_int16 wLength;
	WAVEFORMATEX	waveFormat;
} AUDIOFORMAT;

typedef struct tagJBNV_CHANNEL_INFO
{
	DWORD   dwSize;
	DWORD   dwStream1Height;
	DWORD   dwStream1Width;
	DWORD   dwStream1CodecID;	//MPEG4Ϊ0JPEG2000Ϊ1,H264Ϊ2
	DWORD   dwStream2Height;
	DWORD   dwStream2Width;
	DWORD   dwStream2CodecID;	//MPEG4Ϊ0JPEG2000Ϊ1,H264Ϊ2
	DWORD   dwAudioChannels;
	DWORD   dwAudioBits;
	DWORD   dwAudioSamples;
	DWORD   dwAudioFormatTag;	//MP3Ϊ0x55G722Ϊ0x65
	char	csChannelName[32];
}JBNV_CHANNEL_INFO,*PJBNV_CHANNEL_INFO;

#endif // !defined(AFX_TMFILE_FORMAT_H__)
