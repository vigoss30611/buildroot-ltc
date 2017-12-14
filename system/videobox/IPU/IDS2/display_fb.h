#ifndef _DISPLAY_FB_H
#define _DISPLAY_FB_H


#define FBIO_GET_OSD_BACKGROUND	_IOR('F', 0x60, unsigned int)
#define FBIO_SET_OSD_BACKGROUND	_IOW('F', 0x60, unsigned int)
#define FBIO_GET_OSD_SIZE					_IOR('F', 0x61, unsigned int)
#define FBIO_SET_OSD_SIZE					_IOW('F', 0x61, unsigned int)
#define FBIO_GET_PERIPHERAL_SIZE		_IOR('F', 0x62, unsigned int)

/* only variable resolution device can change resolution. e.g. HDMI */
#define FBIO_GET_PERIPHERAL_VIC		_IOR('F', 0x63, unsigned int)
#define FBIO_SET_PERIPHERAL_VIC		_IOW('F', 0x63, unsigned int)

/* get path enable state. use FBIOBLANK to set path enable state */
#define FBIO_GET_BLANK_STATE			_IOR('F', 0x64, unsigned int)

#define FBIO_SET_BUFFER_ADDR			_IOW('F', 0x65, unsigned int)

/* only used when path is enabled */
#define FBIO_SET_BUFFER_ENABLE		_IOW('F', 0x66, unsigned int)

#define FBIO_SET_MAPCOLOR_ENABLE	_IOW('F', 0x67, unsigned int)
#define FBIO_SET_MAPCOLOR					_IOW('F', 0x68, unsigned int)

#define FBIO_SET_CHROMA_KEY_ENABLE			_IOW('F', 0x69, unsigned int)
#define FBIO_SET_CHROMA_KEY_VALUE			_IOW('F', 0x6A, unsigned int)
#define FBIO_SET_CHROMA_KEY_ALPHA_NONKEY_AREA		_IOW('F', 0x6B, unsigned int)
#define FBIO_SET_CHROMA_KEY_ALPHA_KEY_AREA			_IOW('F', 0x6C, unsigned int)

#define FBIO_SET_ALPHA_TYPE				_IOW('F', 0x6D, unsigned int)
#define FBIO_SET_ALPHA_VALUE			_IOW('F', 0x6E, unsigned int)

#define FBIO_SET_LCDVCLK_CDOWN				_IOW('F', 0x6F, unsigned int)
#define FBIO_SET_LCDVCLK_RFRM_NUM		_IOW('F', 0x70, unsigned int)


/* only for the condition that one path has multiple peripherals */
#define FBIO_SET_PERIPHERAL_DEVICE		_IOW('F', 0x71, unsigned long)


struct display_fb_win_size {
	unsigned int width;
	unsigned int height;
};

#ifndef DTD_TYPE
#define DTD_TYPE
typedef struct {
	unsigned short mCode;	// VIC
	unsigned short mPixelRepetitionInput;
	unsigned int mPixelClock;		// in KHz (different with hdmi)
	unsigned char mInterlaced;
	unsigned short mHActive;
	unsigned short mHBlanking;
	unsigned short mHBorder;
	unsigned short mHImageSize;
	unsigned short mHSyncOffset;
	unsigned short mHSyncPulseWidth;
	unsigned char mHSyncPolarity;
	unsigned char mVclkInverse;
	unsigned short mVActive;
	unsigned short mVBlanking;
	unsigned short mVBorder;
	unsigned short mVImageSize;
	unsigned short mVSyncOffset;
	unsigned short mVSyncPulseWidth;
	unsigned char mVSyncPolarity;
} dtd_t;
#endif

#endif /* _DISPLAY_FB_H */
