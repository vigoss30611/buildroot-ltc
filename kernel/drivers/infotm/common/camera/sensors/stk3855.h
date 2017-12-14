#ifndef __STK8355_H
#define __STK8355_H

#define GETWORDLSB(w)       (w & 0xff)
#define GETWORDMSB(w)       (w >> 8)

/*unsigned char ucMatrixTypeMT; for sensor matrix mode, reg 04H[11]*/
#define MT_BAYLOR_INPUT 	0
#define MT_YUV_INPUT		1

/*unsigned char ucModeCIGYUVMix;  for YUV mix sequence ex:VYUY reg04h[15:12]*/ 
#define MODE_YUVMix_UYVY	0
#define MODE_YUVMix_VYUY	1
#define MODE_YUVMix_YUYV	2
#define MODE_YUVMix_YVYU	3

/*for test mode selection, reg a8H[1:0]	unsigned char ucTM*/
#define TM_MODE_SENSOR0 0
#define TM_MODE_SENSOR1	1
#define TM_MODE_NORMAL	3

#define SENSOR0	0
#define SENSOR1	1

#define RGB_R	0
#define RGB_G	1
#define RGB_B	2

/*for selection of sensor type reg04H[4:5]   ucSnrSourceSel */
enum CsiBridgeSourceSelect {
	SNRSRC_SNR0BUF0 = 0,/*sensor0 in buffer0, sensor 1 in buffer 1*/
	SNRSRC_SNR0BUF1,	/*sensor0 in buffer0, sensor 1 in buffer 1*/
	SNRSRC_AUTO			/*auto select, leading sensor in buf1, later sensor in buf0*/
};

/*for sensor type selection, reg 4H[1:0]	 unsigned char ucSensortype;*/
enum CsiBridgeSensorType {
	SENSORTYPE_HN_VN = 0, 
	SENSORTYPE_HN_VP,
	SENSORTYPE_HP_VN,
	SENSORTYPE_HP_VP
};

/*unsigned char ucSBUmodesMD; for SBU modes, reg 04H[10:8]*/
enum CsiBridgeMode {
	SBU_MODE_BYPASS = 0,
	SBU_MODE_PIXELMIX,
	SBU_MODE_FRAMEBYFRAME,
	SBU_MODE_SIDEBYSIDE,
	SBU_MODE_YUVMIX
};

// CSI properties.
typedef struct _STCsiEngineInfo {
	unsigned char ucFirstPixelColor;	// first pixel color of sensor deliveryed is a frame.
	unsigned char ucControl0;
	unsigned char ucControl1;
	unsigned char ucImageFormat;		// Baylor 8 or 10
	unsigned char ucFifoAndThreshold;	// register 0x28a
	unsigned short usSrcStartX;
	unsigned short usSrcStartY;
	unsigned short usSrcEndX;		// end position is inclusive
	unsigned short usSrcEndY;		// end position is inclusive
	unsigned short usSrcWidth;		// # of horizontal pixel captured into CSI
	unsigned short usSrcHeight;		// # of vertical lines captured into CSI
	unsigned char ucSrcXDecimation;		// 0 => no decimation, 1 => 1/2, 2 => 1/3,..., 15 => 1/16
	unsigned char ucSrcYDecimation;		// 0 => no decimation, 1 => 1/2, 2 => 1/3,..., 15 => 1/16
	unsigned char ucFrameDecimation;	// 0 => no decimation, 1 => 1/2, 2 => 1/3,..., 7 => 1/8
	unsigned short usRawStride;		// buffer stride
	unsigned short usStrideCount;		// stride count in warp around mode
	unsigned short usIntStart;		// interrupt position
	unsigned char ucIntLength;		// interrupt period in # of lines
} STCsiEngineInfo;

struct sensor_drv_context {
	int reset_pads;
};

#endif
