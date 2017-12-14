#ifndef __LIBGRIDTOOLS_H__
#define __LIBGRIDTOOLS_H__


enum FISHEYE_MODE
{
	FISHEYE_MODE_DOWNVIEW,		//0
	FISHEYE_MODE_FRONTVIEW,		//1
	FISHEYE_MODE_CENTERSCALE,	//2
	FISHEYE_MODE_ORIGINSCALE,	//3
	FISHEYE_MODE_ROTATE,		//4
	FISHEYE_MODE_UPVIEW,		//5
	FISHEYE_MODE_MERCATOR,		//6
	FISHEYE_MODE_PERSPECTIVE,	//7
	FISHEYE_MODE_CYLINDER,		//8
//	FISHEYE_MODE_PANORAMA,		//9
	FISHEYE_MODE_MAXIMUM
};

typedef struct
{
	int  mode;
	unsigned char *yBuf;
	int inWidth;
	int inHeight;
	int inStride;
	int ctX;
	int ctY;
	int histThreshold;
	int blackValue;
	int findCnt;
	int shift;
	int pixelThreshold;
	int errorThreshold;	// if more than this value, FceFindCenter fail
	int debugInfo;
} TFceFindCenterParam;

typedef struct
{
	int gridSize;
	int inWidth;
	int inHeight;
	int outWidth;
	int outHeight;
	int gridBufLen;
	int geoBufLen;
} TFceCalcGDSizeParam;

typedef struct
{
	int fisheyeMode;
	int fisheyeGridSize;

	double fisheyeStartTheta;  // Longitude
	double fisheyeEndTheta;    // Longitude
	double fisheyeStartPhi;    // Latitude
	double fisheyeEndPhi;      // Latitude

	int fisheyeRadius;

	int fisheyeImageWidth;	//input
	int fisheyeImageHeight;
	int fisheyeCenterX;
	int fisheyeCenterY;

	int rectImageWidth;		//output
	int rectImageHeight;
	int rectCenterX;
	int rectCenterY;

	int fisheyeFlipMirror;
	double scalingWidth;
	double scalingHeight;
	double fisheyeRotateAngle;
	double fisheyeRotateScale;

	double fisheyeHeadingAngle;
	double fisheyePitchAngle;
	double fisheyeFovAngle;

	double coefficientHorizontalCurve;
	double coefficientVerticalCurve;

	//For panorama parameters
	double fisheyeTheta1;	//horizontal rotation
	double fisheyeTheta2;	//optical axis rotation
	double fisheyeTranslation1;	//3d translation
	double fisheyeTranslation2;	//3d translation
	double fisheyeTranslation3;	//3d translation
	double fisheyeCenterX2;	//optical center x of camera 2
	double fisheyeCenterY2;	//optical center y of camera 2
	double fisheyeRotateAngle2; //rotate of camera 2

	int gridBufLen;
	int geoBufLen;
	unsigned char *fisheyeGridbuf;
	unsigned char *fisheyeGeobuf;
	int debugInfo;
} TFceGenGridDataParam;

unsigned long FceGetVersion(void);
int FceFindCenter(void *pParam);
int FceCalGDSize(void *pParam);
int FceGenGridData(void *pParam);

int fisheye_start();
int griddata_start();

#endif
