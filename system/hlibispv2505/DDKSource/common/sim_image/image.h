/*! 
******************************************************************************
@file   image.h
@brief  Base class for holding a single image in memory + set of derived classes
        for reading/writing the image in various file formats.
@Author Juraj Mlynar
@date   27/11/2012
        <b>Copyright 2012 by Imagination Technologies Limited.</b>\n
        All rights reserved.  No part of this software, either
        material or conceptual may be copied or distributed,
        transmitted, transcribed, stored in a retrieval system
        or translated into any human or computer language in any
        form by any means, electronic, mechanical, manual or
        other-wise, or disclosed to third parties without the
        express written permission of Imagination Technologies
        Limited, Unit 8, HomePark Industrial Estate,
        King's Langley, Hertfordshire, WD4 8LZ, U.K.

******************************************************************************/
#ifndef _IMAGE_H_INCLUDED_
#define _IMAGE_H_INCLUDED_

#include <stdio.h> // for FILE
#include <img_types.h>

class CImageFlx;
class CImagePxm;
class CImageYuv;
class CImagePlRaw;
class CImageNRaw;
class CImageAptinaRaw;
class CImageBmp;

/*-------------------------------------------------------------------------------
image meta data storage*/

class CMetaData {
public:
  typedef struct metaDataItemStr {
    char *name;  //new-allocated
    char *sVal;  //new-allocated
    struct metaDataItemStr *next;
    } MDITEM;
  typedef enum whatIfExistsEnum { IFEX_FAIL,IFEX_REPLACE,IFEX_APPEND,IFEX_IGNORE } IFEX_xxx;
public:
  MDITEM *list;
  CMetaData();
  ~CMetaData() { Unload(); }
  void Unload();
  IMG_INT32 GetNItems();
  bool Add(const char *name,IMG_INT32 nameLen,const char *value,IMG_INT32 valueLen,IFEX_xxx whatIfExists,const char *separatorOnAppend);
  bool UpdateSubItem(const char *name,IMG_INT32 nameLen,IMG_INT subIndex,const char *value,IMG_INT32 valueLen,char separator=' ');
  bool Add(MDITEM *metaDataItem);
  void Del(MDITEM *metaDataItem);
  bool Rename(MDITEM *metaDataItem,const char *newName);
  bool UpdateStr(const char *fieldName,const char *newValue,IFEX_xxx whatIfExists=IFEX_REPLACE);
  bool UpdateStrFmt(const char *fieldName,IFEX_xxx whatIfExists,const char *newValueFmt,...);
  bool UpdateInt(const char *fieldName,IMG_INT newValue,IFEX_xxx whatIfExists=IFEX_REPLACE);
  bool UpdateDouble(const char *fieldName,double newValue,IFEX_xxx whatIfExists=IFEX_REPLACE);
  bool UpdateSubStr(const char *fieldName,IMG_INT32 subIndex,const char *newValue,char separator=' ');
  bool UpdateSubStrFmt(const char *fieldName,IMG_INT32 subIndex,char separator,const char *newValueFmt,...);
  bool UpdateSubInt(const char *fieldName,IMG_INT32 subIndex,IMG_INT newValue,char separator=' ');
  bool UpdateSubDouble(const char *fieldName,IMG_INT32 subIndex,double newValue,char separator=' ');
  bool Rename(const char *fieldName,const char *newName);
  const char *AddFromString(const char *str,IFEX_xxx whatIfExists,const char *separatorOnAppend);
  const char *GetMetaStr(const char *name,const char *defVal=NULL);
  const char *GetMetaSubStr(const char *name,const char *defVal,IMG_INT32 index,IMG_INT32 *pLen,char separator=' ');
  IMG_INT32 GetMetaInt(const char *name,IMG_INT32 defVal,IMG_INT32 index=0,bool extendLast=false);
  IMG_INT32 GetMetaValCount(const char *name);
  IMG_INT32 GetMetaInt(const char *name); // Gets just a single integer value
  double GetMetaDouble(const char *name,double defVal,IMG_INT32 index=0,bool extendLast=false);
  IMG_INT32 GetMetaEnum(const char *name,IMG_INT32 defVal,const char *enumStrings);
  const char *GetMetaName(IMG_INT32 index);
  MDITEM *GetMetaAt(IMG_INT32 index);
  MDITEM *Find(const char *name,IMG_INT32 prefixLen=-1);
  bool IsDefined(const char *name) { return (Find(name) == NULL) ? false : true; }
  bool CopyFrom(CMetaData *pOther);
  bool MergeFrom(CMetaData *pOther,IFEX_xxx whatIfExists);
  void SwapWith(CMetaData *pOther);
  };

/*-------------------------------------------------------------------------------
base class for image data storage*/

#define MAX_COLOR_CHANNELS   4   //max. #of supported color channels

#ifndef PIXELSIZE
#define PIXELSIZE    4
#endif

#if PIXELSIZE==2
typedef IMG_INT16 PIXEL;  //pixel value (right aligned, i.e. in LSBits)
#elif PIXELSIZE==4
typedef IMG_INT32 PIXEL;  //pixel value (right aligned, i.e. in LSBits)
#else
#error Invalid PIXELSIZE.
#endif

class CImageBase {
public:
  typedef enum colorModelEnum {
    CM_UNDEF,   //image data not loaded
    CM_GRAY,    //grayscale image, one color channel (luma)
    CM_RGB,     //3 color channels (red,green,blue)
    CM_RGBA,    //4 color channels (red,green,blue,alpha)
    CM_RGGB,    //4 color channels (red,green1,green2,blue), type of bayer cell given by subsampling and phase offsets
    CM_RGBW,    //4 color channels (red,green,blue,white), type of bayer cell given by subsampling and phase offsets
    CM_YUV,     //3 color channels (luma,chroma U,chroma V), U,V are signed
    CM_YUVA,    //4 color channels (luma,chroma U,chroma V,alpha), U,V are signed
    //where A=alpha/transparency channel (0=opaque, max=transparent)
    } CM_xxx;
  typedef struct colorModelInfoStr {
    CM_xxx model;
    IMG_UINT8 Nchannels;
    bool isBayer;  //true if color model is used for Bayer images (CM_RGGB,CM_RGBW)
    bool isSubsSupport;  //subsampling and perhaps phase offsets are supported
    const char **channelNames;
    } COLORMODELINFO;
  typedef enum subsamplingModeEnum {  //common subsampling cases
    SUBS_UNDEF,  //image not loaded, subsampling does not apply to the current color model or it is not any of the common cases below
    SUBS_444, SUBS_422, SUBS_420,   //for CM_YUV
    SUBS_RGGB, SUBS_GRBG, SUBS_GBRG, SUBS_BGGR,   //for CM_RGGB
    } SUBS_xxx;
  typedef struct colorChannelDataStr {
    PIXEL *data;    //pixel values in raster scan order (subsampled as defined by subsMode)
    IMG_INT32 chnlWidth;   //= #of pixels per line in this color channel buffer (subsampled image width)
    IMG_INT32 chnlHeight;   //= #of lines in this color channel buffer (subsampled image height)
    IMG_UINT8 bitDepth;    //#of bits per component
    bool isSigned;    //are pixel values signed or not? if yes, bitdepth includes the sign bit
    } COLCHANNEL;
  typedef enum imagePropertiesEnum {
    PROPERTY_SUPPORT_VIDEO,  //1 if the image format supports video (multiple frames), 0 otherwise
    PROPERTY_IS_VIDEO,  //1 if the currently loaded file is a video (as opposed to a still image), 0 otherwise
    } PROPERTY_xxx;
public:
  IMG_INT32 width,height;   //image dimensions in pixels
  COLCHANNEL chnl[MAX_COLOR_CHANNELS];  //data is stored in separate buffer for each color channel
  CM_xxx colorModel;
  SUBS_xxx subsMode;
  char *fileName;  //(new-allocated)
public:
  CImageBase();
  virtual ~CImageBase();
  //info query methods
  static const COLORMODELINFO *GetColorModelInfo(CImageBase::CM_xxx model);
  const COLORMODELINFO *GetColorModelInfo() { return GetColorModelInfo(colorModel); }
  static IMG_UINT8 GetBayerFlip(CImageBase::SUBS_xxx bayerSubsampling);
  static IMG_UINT8 GetBayerFlip(CImageBase::SUBS_xxx srcBayerSubsampling,CImageBase::SUBS_xxx dstBayerSubsampling);
  IMG_UINT8 GetBayerFlip() { return GetBayerFlip(subsMode); }
  virtual bool IsHeaderLoaded() { return (colorModel == CM_UNDEF) ? false : true; }  //header
  virtual bool IsDataLoaded();  //image data
  virtual void UnloadHeader();
  virtual void UnloadData();
  void Unload();
  IMG_INT32 GetNColChannels() const;
  IMG_INT32 GetXSampling(IMG_INT32 colChannel) const;
  IMG_INT32 GetYSampling(IMG_INT32 colChannel) const;
  IMG_INT32 GetChMin(IMG_INT32 channel) { return chnl[channel].isSigned?(-(1<<(chnl[channel].bitDepth-1))):0; }
  IMG_INT32 GetChMax(IMG_INT32 channel) { return chnl[channel].isSigned?(+(1<<(chnl[channel].bitDepth-1))-1):((1<<chnl[channel].bitDepth)-1); }
  virtual IMG_INT32 GetProperty(PROPERTY_xxx prop) { return 0; }
  virtual IMG_INT32 GetNFrames() { return 1; }
  inline PIXEL *GetPixelPtr(IMG_INT32 channel,IMG_INT32 x,IMG_INT32 y);
  //load from file (for comments see .cpp file)
  virtual const char *LoadFileHeader(const char *filename,void *pExtra);
  virtual const char *LoadFileData(IMG_INT32 frameIndex);
  //save to file (for comments see .cpp file)
  virtual const char *SaveFileStart(const char *filename,void *pExtra,void **pSC);
  virtual const char *SaveFileHeader(void *pSC);
  virtual const char *SaveFileData(void *pSC);
  virtual const char *SaveFileEnd(void *pSC);
  const char *SaveSingleFrameImage(const char *filename,void *pExtra);
  //some basic operations on image data
  bool CreateNewImage(IMG_INT32 width,IMG_INT32 height,CM_xxx colorModel,SUBS_xxx subsampling,IMG_INT8 *chnlBitDepths,PIXEL *chnlDefVal);
  IMG_INT32 ConvertFrom(CImageBase *pSrc,CM_xxx newColorModel,SUBS_xxx newSubsMode,bool onlyCheckIfAvail);  //NOT virtual to enforce pSrc type = this object type
  void SwapWith(CImageBase *pOther);  //NOT virtual to enforce pOther = this object type
  IMG_INT32 ChBitDepth(IMG_INT32 p,IMG_INT8 oldBitDepth,IMG_INT8 newBitDepth);
  void ChBitDepth(IMG_INT32 *pSrc,IMG_INT32 *pDst,IMG_INT32 Npixels,IMG_INT8 oldBitDepth,IMG_INT8 newBitDepth);  //pDst=pSrc is allowed
  void ChangeChannelBitDepth(IMG_INT32 channel,IMG_INT8 newBitDepth);
  void OffsetChannelValues(IMG_INT32 channel,IMG_INT32 addValue);
  IMG_UINT32 ClipChannelValues(IMG_INT32 channel,IMG_INT32 minVal,IMG_INT32 maxVal,bool countClipOnly);
  IMG_UINT32 ClipChannelValuesToBD(IMG_INT32 channel,bool countClipOnly);
  };

inline PIXEL *CImageBase::GetPixelPtr(IMG_INT32 channel,IMG_INT32 x,IMG_INT32 y) {
//get pointer to a pixel value in specified color channel
  return chnl[channel].data+(y/GetYSampling(channel))*chnl[channel].chnlWidth+(x/GetXSampling(channel));
}

/*-------------------------------------------------------------------------------
Felix file format*/

#define FLXMETA_SUPPORT_DEPRECATED   1

//meta data items
//when saving an FLX file, these will be written by the saving routine explicitly based on format
#define FLXMETA_WIDTH          "WIDTH"                  //[int] image width in pixels
#define FLXMETA_HEIGHT         "HEIGHT"                 //[int] image height in pixels
#define FLXMETA_COLOR_FORMAT   "COLOUR_FORMAT"          //one of: Gray,RGB,RGBA,RGGB,RGBW,YUV,YUVA
#define FLXMETA_SUBS_H         "SUBSAMPLING_HOR"        //[ints] horizontal subsampling step (one value or per channel)
#define FLXMETA_SUBS_V         "SUBSAMPLING_VER"        //[ints] vertical subsampling step (one value or per channel)
#define FLXMETA_PHASE_OFF_H    "PHASE_OFFSET_HOR"       //[ints] horizontal channel phase offset in pixels (per channel)
#define FLXMETA_PHASE_OFF_V    "PHASE_OFFSET_VER"       //[ints] vertical channel phase offset in pixels (per channel)
#define FLXMETA_BITDEPTH       "BITDEPTH"               //[ints] bits per color component (one value or per channel)
#define FLXMETA_SIGNED         "SIGNED"                 //[ints] color component signed/unsigned (0/1, one value or per channel)
#define FLXMETA_PIXEL_FORMAT   "PIXEL_FORMAT"           //one of: unpackedR,unpackedL,grouppacked #; packing mode
#define FLXMETA_PLANE_FORMAT   "PLANE_FORMAT"           //[ints] number of color channels in each plane
#define FLXMETA_LINE_ALIGN     "LINE_ALIGN"             //[ints] alignment for one line of pixels (one value or per file plane)
#define FLXMETA_FRAME_SIZE     "FRAME_SIZE"             //[ints] size of frame in bytes (one value or per each frame)
//when saving an FLX file, these will be written only if set in meta data block by caller before saving is invoked
#define FLXMETA_FRAME_RATE     "FRAME_RATE"             //[float] video playback speed in frames per second
#define FLXMETA_CAPT_ISO       "CAPTURE_ISO"            //[int] ISO setting used to capture image
#define FLXMETA_CAPT_SHUTTER   "CAPTURE_SHUTTER"        //[float] shutter speed [ms]
#define FLXMETA_CAPT_APERTURE  "CAPTURE_APERTURE"       //[float] aperture size (the 'x' in f-stop number 'f/x')
#define FLXMETA_CAPT_FOCALLEN  "CAPTURE_FOCAL_LENGTH"   //[int] focal length [mm]
#define FLXMETA_BORDER_OBP     "BORDER_OPTICAL_BLACK"   //[ints] left right top bottom optical black border sizes
#define FLXMETA_CH_FULL_MIN    "CHANNEL_FULL_MIN"       //[int] channel full range minimum
#define FLXMETA_CH_FULL_MAX    "CHANNEL_FULL_MAX"       //[int] channel full range maximum
#define FLXMETA_CH_DISP_MIN    "CHANNEL_DISPLAY_MIN"    //[ints] channel display range minimum (black point)
#define FLXMETA_CH_DISP_MAX    "CHANNEL_DISPLAY_MAX"    //[ints] channel display range maximum (white point)
#define FLXMETA_GAMMA_APPLIED  "GAMMA_APPLIED"          //[float] gamma correction applied to image data
#define FLXMETA_CHANNEL_GAIN   "CHANNEL_GAIN"           //[floats] gain (multiplier) applied to image data (one value or per channel)
#define FLXMETA_COL_TEMP       "COLOUR_TEMPERATURE"     //[int] temperature of illuminant in degrees Kelvin
#define FLXMETA_PRIMARIES      "PRIMARY_COLOURS"        //one of: native,OV8810,9M114,NTSC,BT601,BT709,AdobeRGB
#define FLXMETA_RGBYCC_MTX     "RGB_YCC_MATRIX"         //one of: BT601,BT709,JFIF; matrix used in RGB->YUV conversion (for YUV[A])
#define FLXMETA_SENSOR_TYPE    "SENSOR_TYPE"            //[str] type of sensor used to capture image
#define FLXMETA_SENSOR_BITS    "SENSOR_BITDEPTH"        //[ints] bits per channel of the sensor (informative) (one value or per channel)
#define FLXMETA_SENSOR_BASEISO "SENSOR_BASE_ISO"        //[int] base ISO of the sensor
#define FLXMETA_SENSOR_PIXSIZE "SENSOR_PIXEL_SIZE"      //[float] size of image sensor pixel in microns
#define FLXMETA_SENSOR_PIXDEAD "SENSOR_PIXEL_DEAD_SPACE" //[float] size of pixel dead space in microns
#define FLXMETA_SENSOR_WELLD   "SENSOR_WELL_DEPTH"      //[int] sensor well depth in electrons
#define FLXMETA_SENSOR_ERN     "SENSOR_EST_READ_NOISE"  //[int] sensor estimated read noise (std.dev in electrons)
#define FLXMETA_SENSOR_QEFF    "SENSOR_QUANTUM_EFFICIENCY" //[float] sensor quantum efficiency (range <0,1>, one value or per channel)
#define FLXMETA_SENSOR_BLACK_PT "SENSOR_BLACK_POINT"    //[int] sensor black point (one value or per channel)
#define FLXMETA_TIME_CREATED   "IMAGE_CREATE_TIME"      //[dd/mm/yy [hh:mm:ss]] date & time when image data was created
#define FLXMETA_AUTHOR         "IMAGE_CREATED_BY"       //[str] name of author or source where image comes from
#define FLXMETA_DESC           "IMAGE_DESC"             //[str] image description (what the image depicts)
#define FLXMETA_COMMENT        "IMAGE_COMMENT"          //[str] any comments
//deprecated
#if FLXMETA_SUPPORT_DEPRECATED
#define FLXMETA_SUBSAMPLING    "SUBSAMPLING"            //[str] replaced by SUBSAMPLING_HOR,_VER and PHASE_OFFSET_HOR,_VER
#define FLXMETA_ISO_GAIN       "ISO_GAIN"               //[float] gain applied in analog path of sensor (compensated by well depth)
#define FLXMETA_SENSOR_DENSITY "SENSOR_DENSITY"         //[int] density of image sensor in electrons per square micron
#define FLXMETA_GAMMA_BLACK_PT "GAMMA_BLACK_POINT"      //[int] color component value for gamma black (0) point
#define FLXMETA_GAMMA_WHITE_PT "GAMMA_WHITE_POINT"      //[int] color component value for gamma white (unity) point
#define FLXMETA_NOMINAL_RANGE  "CHANNEL_NOMINAL_RANGE"  //[ints] min1,max1,min2,max2,... (one pair or pair per channel)
#endif

/*IMPORTANT
FLX format supports multiple segments with multiple frames per segment. All frames in a segment share their own meta data.
Therefore, each segment is like a separate file with its own format. At most one frame is held in memory at any time and
normally the 'current' segment (pCurrSeg) points to the segment which holds the frame's meta data.

-- LOADING --
When a file is loaded, LoadFileHeader() scans the file and loads information about all segments into pSegInfo list.
ApplyMetaForFrame() is used to set the 'current' segment as appropriate for the particular frame and a subsequent
LoadFileData() call will load frame into memory (it relies on the correct meta data, i.e. current segment, to be selected).

-- SAVING --
If CImageFlx object has segment information available, it can be used to save the file, i.e. ApplyMetaForFrame() can be
called to select an appropriate segment before calling SaveFileHeader() and SaveFileData(). In this case FLXSAVEFORMAT.pMD
can be NULL and saving will pick up meta data from the current segment.
Alternatively, FLXSAVEFORMAT.pMD can point to separately created meta data, or meta data of another CImageFlx object.
The FLXSAVEFORMAT.pImData pointer can point to another image from which pixel data is being saved. Useful for conversions.
*/

class CImageFlx: public CImageBase {
public:
  typedef enum packingModeEnum {
    PACKM_UNDEF,
    PACKM_UNPACKEDR,  //right-aligned unpacked, each color component value is stored separately in 1,2 or 4 bytes
                      //using LSBits with sign extension to full number of bits required for storage
                      //pixelFormatParam[] is not used
    PACKM_UNPACKEDL,  //left-aligned unpacked, each color component value is stored separately in 1,2 or 4 bytes
                      //using MSBits with 0 padding in unused LSBits
                      //pixelFormatParam[] is not used
    PACKM_GROUPPACK,  //group-packed; color components are bit-packed in groups (starting with LSBits) and each group is
                      //padded with 0 bits to the nearest byte boundary
                      //pixelFormatParam[] specifies group sizes (in #of components per group for each file plane)
                      //(unused values in the array can be left undefined)
    } PACKM_xxx;
  typedef struct flxSaveFormatStr {  //user-defined format parameters when saving a file
    PACKM_xxx pixelFormat;
    IMG_INT32 pixelFormatParam[MAX_COLOR_CHANNELS];  //see PACKM_xxx
    IMG_INT8 planeFormat[MAX_COLOR_CHANNELS];  //for each file plane number of color channels to store in that plane
                            //number of valid values is implicitly given by adding them until number of color channels is reached
    IMG_INT32 lineAlign;  //line alignment (if > 0 then pad each line of pixels to a multiple of this many bytes)
    CMetaData *pMD;  //FLX meta data to save - can point at this object's segment meta or into a different CImageFlx object
    CImageBase *pImData;  //image data to be saved - can be this object or another one
    } FLXSAVEFORMAT;
protected:
  typedef enum soTableEntriesEnum {  //offsets and sizes come in pairs (offsets on even indexes, sizes on odd indexes)
    SOTE_METAOFFS=0,
    SOTE_METASIZE,
    SOTE_PIXELSOFFS,
    SOTE_PIXELSSIZE,
    } SOTE_xxx;
  typedef struct flxSegmentInfoStr {  //information about one segment of the FLX file
    IMG_INT64 segmentOffset;  //absolute offset of the beginning of the segment in FLX file
    IMG_INT64 segmentSize;  //size of the segment in bytes (header + all sections)
    //frameOffsets and NframeOffsets:
    //The frameOffsets[] array only holds offsets for as many frames as there are frame sizes specified
    //in meta data, +1 extra value for the offset 'just after'. If Nframes>NframeOffsets then the extra
    //frames are assumed to have the same size as the last explicity frame, i.e.
    //  frameOffsets[NframeOffsets]-frameOffsets[NframeOffsets-1]
    //If all frames have the same size, NframeOffsets=1 and frameOffsets[] contains 2 entries.
    IMG_INT64 *frameOffsets;  //offset of the beginning of each frame relative to segmentOffset
    IMG_INT32 NframeOffsets;  //#of offsets in frameOffsets[] array (excluding the last extra one)
    IMG_INT32 Nframes;        //#of frames in this segment in total, >= NframeOffsets
    CMetaData meta;           //meta data valid for this segment
    struct flxSegmentInfoStr *next;
    } FLXSEGMENTINFO;
  typedef struct flxSaveContextStr {  //context structure used internally during save process
    FILE *f;
    IMG_INT64 soTableSave[4];  //file offsets and sizes in the header, indexed by SOTE_xxx
    FLXSEGMENTINFO seg;  //info collected on the current segment as it is being saved
    FLXSAVEFORMAT *pExtra;
    } FLXSAVECONTEXT;
public:
  //the following items are set based on meta data of the current segment when the segment is selected
  PACKM_xxx pixelFormat;
  IMG_INT32 pixelFormatParam[MAX_COLOR_CHANNELS];  //extra parameter for pixelFormat (see PACKM_xxx)
protected:
  FLXSEGMENTINFO *pSegInfo;  //list of all segments
  FLXSEGMENTINFO *pCurrSeg;  //pointer to the 'current' segment. Current object members should correspond
                             //to meta data in this segment.
protected:
  //helper query functions
  static SUBS_xxx DetectFlxSubsampling(CImageBase *pImData,CMetaData *pMD);
  IMG_INT32 GetLineSize(IMG_INT32 plane,FLXSAVEFORMAT *pFormat=NULL);
  IMG_INT32 GetFrameSize(FLXSAVEFORMAT *pFormat=NULL);
  //frames & segments
  static FLXSEGMENTINFO *NewSegment();
  void AddSegment(FLXSEGMENTINFO *pSeg);
  FLXSEGMENTINFO *GetSegment(IMG_INT32 index,bool isFrameIndex);
  IMG_INT32 GetSegmentIndex(IMG_INT32 frameIndex,IMG_INT32 *pFrameWithinSeg,FLXSEGMENTINFO **ppSeg);
  //meta data functions
  const char *ApplyMetaData(FLXSEGMENTINFO *pSeg);
  //load
  const char *LoadFlxSegmentHeader(FILE *f,FLXSEGMENTINFO *pSeg,IMG_INT64 fsize);
  //save  
  const char *SaveFlxMetaData(FLXSAVECONTEXT *pCtx);
  const char *SaveFlxFrameData(FLXSAVECONTEXT *pCtx);
  const char *SaveFlxSegmentStart(FLXSAVECONTEXT *pCtx);
  const char *SaveFlxSegmentEnd(FLXSAVECONTEXT *pCtx);
public:
  CImageFlx();
  virtual ~CImageFlx();
  //helper query functions
  IMG_UINT32 GetNFilePlanes(FLXSAVEFORMAT *pFormat=NULL);
  IMG_INT32 GetNChannelsInPlane(IMG_INT32 plane,FLXSAVEFORMAT *pFormat=NULL);
  IMG_INT32 GetPlaneBaseChannel(IMG_INT32 plane,FLXSAVEFORMAT *pFormat=NULL);
  SUBS_xxx GetSubsampling(CMetaData *pMD=NULL);
  //meta data functions
  CMetaData *GetMeta() { return (pCurrSeg == NULL)? NULL: &(pCurrSeg->meta); }
  CMetaData *GetMetaForFrame(IMG_INT32 frameIndex);
  const char *ApplyMetaForFrame(IMG_INT32 frameIndex,bool force=false);
  void UpgradeMetaData(CMetaData *pMD);
  bool CheckValidMetaPlaneFormat(FLXSAVEFORMAT *pFormat=NULL);
  void RebuildBaseMetaData(FLXSAVEFORMAT *pFormat=NULL,CMetaData *pMD=NULL);
  IMG_INT32 ConvertFrom(CImageFlx *pSrc,CM_xxx newColorModel,SUBS_xxx newSubsMode,bool onlyCheckIfAvail);  //NOT virtual to enforce pSrc type = this object type
  void SwapWith(CImageFlx *pOther);  //NOT virtual to enforce pOther = this object type
  //frames & segments
  IMG_INT32 GetNSegments();
  IMG_INT32 GetNFrames(IMG_INT32 segmentIndex,IMG_INT32 *pFirstFrame=NULL);  //-1=total across all segments
  IMG_INT32 GetSegmentIndex(IMG_INT32 frameIndex,IMG_INT32 *pFrameWithinSeg) { return GetSegmentIndex(frameIndex,pFrameWithinSeg,NULL); }
  IMG_INT64 GetFrameFilePos(IMG_INT32 frameIndex,IMG_INT32 *pFrameSize);
  bool AddNewSegment(IMG_INT32 Nframes);
  //load from file
  virtual IMG_INT32 GetProperty(PROPERTY_xxx prop);
  virtual IMG_INT32 GetNFrames() { return GetNFrames(-1); }
  virtual void UnloadHeader();
  virtual const char *LoadFileHeader(const char *filename,void *pExtra);  //pExtra=NULL
  virtual const char *LoadFileData(IMG_INT32 frameIndex);
  //save to file
  //1) call SaveFileStart()
  //2) prepare meta data (FLXSAVEFORMAT.pMD) and call StartSegment()
  //3) call SaveFileHeader (1x) and SaveFileData (for each frame) - header can be saved before or after frame data
  //   (meta data must not be modified during this step, all frames must be the same size)
  //4) if more segments, goto 2)
  //3) call SaveFileEnd()
  virtual const char *SaveFileStart(const char *filename,void *pExtra,void **pSC);  //pExtra=FLXSAVEFORMAT
  const char *StartSegment(void *pSC,bool inherit,bool force);  //FLX-specific, starts a new segment [if required]
  virtual const char *SaveFileHeader(void *pSC);  //meta data
  virtual const char *SaveFileData(void *pSC);  //frame
  virtual const char *SaveFileEnd(void *pSC);
  static const char *GetSaveFormat(FLXSAVEFORMAT *pFmt,CImageBase *pIm);
  static const char *GetSaveFormat(FLXSAVEFORMAT *pFmt,CImageFlx *pIm);
  static const char *GetSaveFormat(FLXSAVEFORMAT *pFmt,CImagePxm *pIm);
  static const char *GetSaveFormat(FLXSAVEFORMAT *pFmt,CImageYuv *pIm);
  static const char *GetSaveFormat(FLXSAVEFORMAT *pFmt,CImagePlRaw *pIm);
  static const char *GetSaveFormat(FLXSAVEFORMAT *pFmt,CImageNRaw *pIm);
  static const char *GetSaveFormat(FLXSAVEFORMAT *pFmt,CImageAptinaRaw *pIm);
  static const char *GetSaveFormat(FLXSAVEFORMAT *pFmt,CImageBmp *pIm);
  };

/*-------------------------------------------------------------------------------
PGM/PPM file format*/

class CImagePxm: public CImageBase {
public:
  typedef enum pxmTypeEnum {
    PPTYPE_UNDEF,
    PPTYPE_PPMB,  //PPM color binary
    PPTYPE_PPMT,  //PPM color text
    PPTYPE_PGMB,  //PGM grayscale binary
    PPTYPE_PGMT,  //PGM grayscale text
    } PPTYPE_xxx;
  typedef enum pxmLoadBitDepthEnum {  //how to determine bitdepth of PGM/PPM images and handling of 16-bit unsigned values
    LOADBD_SIGNED,    //bitdepth is given by maxval, convert values to 16-bit signed (any values >32767 will be negative) (default)
    LOADBD_DOWNSIZE,  //bitdepth is given by maxval, if maxval>32767, image will be downscaled by 1 bit (pixels div by 2)
    } LOADBD_xxx;
  typedef struct pxmLoadFormatStr {
    CM_xxx forceColorModel;  //color model override (CM_UNDEF=choose automatically)
    SUBS_xxx forceSubsampling;  //only applies for color models which support subsampling (SUBS_UNDEF=choose automatically)
    bool useBayerCells;  //used only if forceColorModel is CM_RGGB; true=assume the values in file are spatially
                         //organized as the Bayer CFA cells (rather than all channels interleaved in raster scan order)
    LOADBD_xxx loadBd;      //how to calculate bitdepth of the image and how to handle 16-bit unsigned (unsupported) values.
    } PXMLOADFORMAT;
  typedef struct pxmSaveFormatStr {  //user-defined format parameters when saving a file
    PPTYPE_xxx fileFormat;
    bool useBayerCells;  //used only if colorModel=CM_RGGB; true=store channel values spatially organized as
                         //the Bayer CFA given by subsMode (rather than all channels interleaved within each row)
    } PXMSAVEFORMAT;
protected:
  typedef struct pxmSaveContextStr {  //context structure used internally during save process
    FILE *f;
    PXMSAVEFORMAT *pExtra;
    IMG_INT32 maxVal;
    } PXMSAVECONTEXT;
protected:
  IMG_INT32 foffsetData;  //file offset of the first byte of image data
  void *pExtra;  //saved pExtra pointer from LoadFileHeader() and SaveFileStart()
  IMG_INT8 pixelOnLoadResize;  //bitshift applied to pixel values during loading (>0=upsize, <0=downsize)
public:
  PPTYPE_xxx fileFormat;  //format of the file from which image was loaded
  CMetaData meta;  //from comments in the header

public:
  CImagePxm();
  virtual ~CImagePxm();
  virtual void UnloadHeader();
  void ParseComment(FILE *f,char **ppMetaItemBuf);
  //load from file
  virtual const char *LoadFileHeader(const char *filename,void *pExtra);  //pExtra=NULL or PXMLOADFORMAT
  virtual const char *LoadFileData(IMG_INT32 frameIndex);
  //save to file
  //1) call SaveFileStart()
  //2) call SaveFileHeader (1x) and SaveFileData (for each frame, >1x supported only for binary formats)
  //3) call SaveFileEnd()
  virtual const char *SaveFileStart(const char *filename,void *pExtra,void **pSC);  //pExtra=PXMSAVEFORMAT
  virtual const char *SaveFileHeader(void *pSC);
  virtual const char *SaveFileData(void *pSC);
  virtual const char *SaveFileEnd(void *pSC);
  static const char *GetSaveFormat(PXMSAVEFORMAT *pFmt,CImageBase *pIm);
  static const char *GetSaveFormat(PXMSAVEFORMAT *pFmt,CImageFlx *pIm);
  static const char *GetSaveFormat(PXMSAVEFORMAT *pFmt,CImagePxm *pIm);
  static const char *GetSaveFormat(PXMSAVEFORMAT *pFmt,CImageYuv *pIm);
  static const char *GetSaveFormat(PXMSAVEFORMAT *pFmt,CImagePlRaw *pIm);
  static const char *GetSaveFormat(PXMSAVEFORMAT *pFmt,CImageNRaw *pIm);
  static const char *GetSaveFormat(PXMSAVEFORMAT *pFmt,CImageAptinaRaw *pIm);
  static const char *GetSaveFormat(PXMSAVEFORMAT *pFmt,CImageBmp *pIm);
  };

/*-------------------------------------------------------------------------------
YUV file format*/

class CImageYuv: public CImageBase {
public:
  /*
  Supported formats are:
  - 4:4:4 planar and interleaved
  - 4:2:2 planar and interleaved YUYV,YVYU,UYVY,VYUY
  - 4:2:0 planar
  unpacked formats of any bit depth up to 15 unsigned (if PIXEL=16-bit) or 16 unsigned (if PIXEL=32-bit)
  both left- and right-aligned (1 byte used for <=8 bits per pixel, 2 bytes for >8 bits per pixel)
  These formats are NOT supported:
  - 10-bit packed formats (3x10-bit in 32 bits)
  - planar PL12 formats
  - YUVA
  */
  typedef enum planeFormatsStr {
    PLANEFMT_PLANAR,
    PLANEFMT_INTER_YUV,   //4:4:4 YUV
    PLANEFMT_INTER_YUYV,  //4:2:2 YUYV
    PLANEFMT_INTER_YVYU,  //4:2:2 YVYU
    PLANEFMT_INTER_UYVY,  //4:2:2 UYVY
    PLANEFMT_INTER_VYUY,  //4:2:2 VYUY
    } PLANEFMT_xxx;
  typedef struct yuvLoadFormatStr {  //user-defined format parameters when loading a file
    IMG_INT32 width,height;  //frame dimensions in pixels
    SUBS_xxx subsMode;
    IMG_INT8 bitDepth;
    PLANEFMT_xxx planeFormat;
    bool msbAligned;  //support both left- and right-aligned uncompressed files
    } YUVLOADFORMAT;
  typedef struct yuvSaveFormatStr {  //user-defined format parameters when saving a file
    PLANEFMT_xxx planeFormat;
    bool msbAligned;  //support both left- and right-aligned uncompressed files
    } YUVSAVEFORMAT;
protected:
  typedef struct flxSaveContextStr {  //context structure used internally during save process
    FILE *f;
    YUVSAVEFORMAT *pExtra;
    } YUVSAVECONTEXT;
protected:
  IMG_INT64 frameSize;  //size of one entire frame in bytes
  IMG_INT32 Nframes;  //#of frames in YUV file
public:
  PLANEFMT_xxx planeFormat;  //planar/interleaved - format of the loaded file
  bool msbAligned;  //value alignment - format of the loaded file
public:
  CImageYuv();
  virtual ~CImageYuv();
  virtual void UnloadHeader();
  virtual IMG_INT32 GetProperty(PROPERTY_xxx prop);
  virtual IMG_INT32 GetNFrames() { return Nframes; }
  //load from file
  virtual const char *LoadFileHeader(const char *filename,void *pExtra);  //pExtra=YUVLOADFORMAT
  virtual const char *LoadFileData(IMG_INT32 frameIndex);
  //save to file
  //1) call SaveFileStart()
  //2) call SaveFileHeader() (actually does nothing for YUV) and SaveFileData() for each frame
  //3) call SaveFileEnd()
  virtual const char *SaveFileStart(const char *filename,void *pExtra,void **pSC);  //pExtra=YUVSAVEFORMAT
  virtual const char *SaveFileHeader(void *pSC);
  virtual const char *SaveFileData(void *pSC);
  virtual const char *SaveFileEnd(void *pSC);
  static const char *GetLoadFormat(YUVLOADFORMAT *pFmt,const char *filename,const char **pFormatPart);
  const char *GetFormatString(YUVSAVEFORMAT *pFmt);
  static const char *GetSaveFormat(YUVSAVEFORMAT *pFmt,CImageBase *pIm);
  static const char *GetSaveFormat(YUVSAVEFORMAT *pFmt,CImageFlx *pIm);
  static const char *GetSaveFormat(YUVSAVEFORMAT *pFmt,CImagePxm *pIm);
  static const char *GetSaveFormat(YUVSAVEFORMAT *pFmt,CImageYuv *pIm);
  static const char *GetSaveFormat(YUVSAVEFORMAT *pFmt,CImagePlRaw *pIm);
  static const char *GetSaveFormat(YUVSAVEFORMAT *pFmt,CImageNRaw *pIm);
  static const char *GetSaveFormat(YUVSAVEFORMAT *pFmt,CImageAptinaRaw *pIm);
  static const char *GetSaveFormat(YUVSAVEFORMAT *pFmt,CImageBmp *pIm);
  };

/*-------------------------------------------------------------------------------
RAW (plain data) file format*/

class CImagePlRaw: public CImageBase {
public:
  /*
  Supported formats are:
  - interleaved R,G,B
  - bayer RGGB,GRBG,GBRG,BGGR
  values in the file are always unsigned
  */
  typedef struct plrawLoadFormatStr {  //user-defined format parameters when loading a file
    IMG_INT32 width,height;  //frame dimensions in pixels
    IMG_INT8 bitDepth;
    CM_xxx colorModel;
    SUBS_xxx subsMode;
    } PLRAWLOADFORMAT;
  typedef struct plrawSaveFormatStr {  //user-defined format parameters when saving a file
    //(current color model & subsampling is used)
    IMG_INT8 bitDepth;  //output file bit depth
    IMG_INT32 rangeMin[MAX_COLOR_CHANNELS];  //scale image pixels such that this becomes 0 in the output file...
    IMG_INT32 rangeMax[MAX_COLOR_CHANNELS];  //...and this becomes maximum in the output file
    } PLRAWSAVEFORMAT;
protected:
  typedef struct plrawSaveContextStr {  //context structure used internally during save process
    FILE *f;
    PLRAWSAVEFORMAT *pExtra;
    } PLRAWSAVECONTEXT;
protected:
  IMG_INT64 frameSize;  //size of one entire frame in bytes
public:
  CImagePlRaw();
  virtual ~CImagePlRaw();
  virtual void UnloadHeader();
  //load from file
  virtual const char *LoadFileHeader(const char *filename,void *pExtra);  //pExtra=PLRAWLOADFORMAT
  virtual const char *LoadFileData(IMG_INT32 frameIndex);
  //save to file
  //1) call SaveFileStart()
  //2) call SaveFileHeader() (actually does nothing for YUV) and SaveFileData() for each frame
  //3) call SaveFileEnd()
  virtual const char *SaveFileStart(const char *filename,void *pExtra,void **pSC);  //pExtra=PLRAWSAVEFORMAT
  virtual const char *SaveFileHeader(void *pSC);
  virtual const char *SaveFileData(void *pSC);
  virtual const char *SaveFileEnd(void *pSC);
  static const char *GetLoadFormat(PLRAWLOADFORMAT *pFmt,const char *filename,const char **pFormatPart);
  const char *GetFormatString(PLRAWSAVEFORMAT *pFmt);
  static const char *GetSaveFormat(PLRAWSAVEFORMAT *pFmt,CImageBase *pIm);
  static const char *GetSaveFormat(PLRAWSAVEFORMAT *pFmt,CImageFlx *pIm);
  static const char *GetSaveFormat(PLRAWSAVEFORMAT *pFmt,CImagePxm *pIm);
  static const char *GetSaveFormat(PLRAWSAVEFORMAT *pFmt,CImageYuv *pIm);
  static const char *GetSaveFormat(PLRAWSAVEFORMAT *pFmt,CImagePlRaw *pIm);
  static const char *GetSaveFormat(PLRAWSAVEFORMAT *pFmt,CImageNRaw *pIm);
  static const char *GetSaveFormat(PLRAWSAVEFORMAT *pFmt,CImageBmp *pIm);
  };

/*-------------------------------------------------------------------------------
NRAW file format
(Nethra internal RAW)*/

#define NRAWMETA_SENSOR_CFG     "SENSOR_CFG_NUM"         //[uint16] same as firmware make file
#define NRAWMETA_CHIP_ID        "CHIP_ID"                //[uint32] current chip id
#define NRAWMETA_FW_VERSION     "FW_VERSION"             //[str] directly from camera app code
#define NRAWMETA_CAMERA_MAKE    "CAMERA_MAKE"            //[str]
#define NRAWMETA_CAMERA_MODEL   "CAMERA_MODEL"           //[str]
#define NRAWMETA_NFRAMES        "NUM_FRAMES"             //[uint16] number of frames
#define NRAWMETA_OPT_BLACK      "OPTICAL_BLACK"          //[4x uint16] optical black border sizes (T,B,L,R)

class CImageNRaw: public CImageBase {
public:
  typedef enum imageTypesEnum { IT_BAYER=0,IT_YUV422,IT_RGB565,IT_RGB444,IT_RGB888,IT_UNDEF=0xffff/*internal*/ } IT_xxx;
protected:
  typedef enum bayerTypesEnum { BT_RGGB=0,BT_GRBG,BT_GBRG,BT_BGGR,BT_UNDEF=0xffff } BT_xxx;
  typedef struct nrawHeaderStr {
    char signature[4];  //"NRAW"
    IMG_UINT16 width,height;  //image size in pixels
    IMG_UINT16 bitDepth;  //8,10,12,14 or 16
    IMG_UINT16 bytesPerSample;  //1 or 2
    IMG_UINT16 bayerType;  //BT_xxx
    IMG_UINT16 headerVersion;  //0x100 or 0x200 ((major<<8)|minor ?)
    IMG_UINT16 sensorCfgNum;  //same as in firmware make file
    IMG_UINT16 imageType;  //IT_xxx
    char reserved[12];
    IMG_UINT32 chipID;  //current chip ID
    char fwVersion[14];  //firmware version directly from camera app code
    char make[30];  //camera?
    char model[32];  //camera?
    IMG_UINT16 numComps;  //number of image components
    IMG_UINT16 numFrames;  //number of frames
    IMG_UINT16 opticalBlack[4];  //optical black border size (top,bottom,left,right)
    //pad with 0s up to 1024 bytes
    } NRAWHEADER;
public:
  CMetaData meta;  //translated from header
  IMG_INT32 frameSize;  //bytes per frame
  IMG_INT32 fileOffsetFrame1;  //file offset of 1st frame
  IT_xxx nrawImageType;  //from loaded header
  IMG_UINT8 bytesPerSample;  //from loaded header
public:
  CImageNRaw();
  virtual ~CImageNRaw();
  virtual void UnloadHeader();
  //load from file
  static bool DetectFormat(const char *filename);
  virtual const char *LoadFileHeader(const char *filename,void *pExtra);  //pExtra=NULL
  virtual const char *LoadFileData(IMG_INT32 frameIndex);
  //save to file NOT SUPPORTED
  virtual const char *SaveFileStart(const char *filename,void *pExtra,void **pSC) { return "Not supported"; }
  virtual const char *SaveFileHeader(void *pSC) { return "Not supported"; }
  virtual const char *SaveFileData(void *pSC) { return "Not supported"; }
  virtual const char *SaveFileEnd(void *pSC) { return "Not supported"; }
  };

/*-------------------------------------------------------------------------------
Aptina RAW file format
(From Aptina sensor capture software)*/

class CImageAptinaRaw: public CImageBase {
public:
  typedef enum rawPixelFormats { RPF_UNDEF,RPF_BAYER10 } RPF_xxx;
protected:
  IMG_INT32 Nframes;  //#of frames in YUV file
  IMG_INT32 frameSize;  //bytes per frame
  RPF_xxx pixelFormat;
public:
  CImageAptinaRaw();
  virtual ~CImageAptinaRaw();
  virtual void UnloadHeader();
  virtual IMG_INT32 GetProperty(PROPERTY_xxx prop);
  virtual IMG_INT32 GetNFrames() { return Nframes; }
  //load from file
  static bool DetectFormat(const char *filename);
  virtual const char *LoadFileHeader(const char *filename,void *pExtra);  //pExtra=NULL
  virtual const char *LoadFileData(IMG_INT32 frameIndex);
  //save to file NOT SUPPORTED
  virtual const char *SaveFileStart(const char *filename,void *pExtra,void **pSC) { return "Not supported"; }
  virtual const char *SaveFileHeader(void *pSC) { return "Not supported"; }
  virtual const char *SaveFileData(void *pSC) { return "Not supported"; }
  virtual const char *SaveFileEnd(void *pSC) { return "Not supported"; }
  };

/*-------------------------------------------------------------------------------
BMP file format*/

class CImageBmp: public CImageBase {
public:
  typedef enum compressionTypeEnum {  //type of bitmap compression
    COMPR_NONE=0,  //uncompressed
    COMPR_RLE8=1,  //8-bit RLE
    COMPR_RLE4=2,  //4-bit RLE
    } COMPR_xxx;
  typedef struct bmpLoadFormatStr {
    char dummy;
    } BMPLOADFORMAT;
  typedef struct bmpSaveFormatStr {  //user-defined format parameters when saving a file
    //the following need to be set up to scale current pixel values to 8-bit unsigned
    //  bmpPixel = clip(dataPixel*mul+off)
    double mul[3];    //multiplier applied to each channel
    IMG_INT off[3];   //offset applied to each channel
    } BMPSAVEFORMAT;
protected:
#pragma pack(push,1)
  typedef struct bmpFileHead {
    IMG_UINT16 wType;	      //bitmap id ('BM' or 0x4D42)
    IMG_UINT32 dwSize;        //file size in bytes
    IMG_UINT16 wRes1,wRes2;   //reserved
    IMG_UINT32 dwOffBits;     //byte offset from this struct to bitmap data
    } BMPFILEHEAD;
  typedef struct bmpInfoHead {
    IMG_UINT32 dwSize;          //size of this structure [bytes]
    IMG_UINT32 dwWidth;         //image width [pixels]
    IMG_UINT32 dwHeight;        //image height [pixels]
    IMG_UINT16 wPlanes;         //number of planes (=1)
    IMG_UINT16 wBitCount;       //bits per pixel (=1,4,8,24)
    IMG_UINT32 dwCompression;   //compression (COMPR_xxx)
    IMG_UINT32 dwSizeImage;     //size of compressed bitmap [bytes], can be 0 if using COMPR_NONE
                                //otherwise dwSizeImage=BMPFILEHEAD.dwSize-BMPFILEHEAD.dwOffBits
    IMG_UINT32 dwXPelsPerMeter; //horizontal resolution [pixels per meter]
    IMG_UINT32 dwYPelsPerMeter; //vertical resolution [pixels per meter]
    IMG_UINT32 dwClrUsed;       //#of colors used, 0=entire palette (1<<bpp) otherwise size of color optim.table
    IMG_UINT32 dwClrImportant;  //#of important colors, 0=all
    } BMPINFOHEAD;
#pragma pack(pop)
  typedef struct bmpSaveContextStr {  //context structure used internally during save process
    FILE *f;
    BMPSAVEFORMAT *pExtra;
    } BMPSAVECONTEXT;
protected:
  IMG_UINT8 *pPalette;  //used during file loading process, otherwise NULL
  BMPFILEHEAD fhdr;  //as loaded from file
public:
  BMPINFOHEAD ihdr;  //as loaded from file
protected:
  static bool GetChannelMapToRGB(CM_xxx colorModel,IMG_UINT8 *pMap3);
public:
  CImageBmp();
  virtual ~CImageBmp();
  virtual void UnloadHeader();
  //load from file
  virtual const char *LoadFileHeader(const char *filename,void *pExtra);  //pExtra=NULL or BMPLOADFORMAT
  virtual const char *LoadFileData(IMG_INT32 frameIndex);
  //save to file
  //1) call SaveFileStart()
  //2) call SaveFileHeader (1x) and SaveFileData
  //3) call SaveFileEnd()
  virtual const char *SaveFileStart(const char *filename,void *pExtra,void **pSC);  //pExtra=BMPSAVEFORMAT
  virtual const char *SaveFileHeader(void *pSC);
  virtual const char *SaveFileData(void *pSC);
  virtual const char *SaveFileEnd(void *pSC);
  static const char *GetSaveFormat(BMPSAVEFORMAT *pFmt,CImageBase *pIm);
  static const char *GetSaveFormat(BMPSAVEFORMAT *pFmt,CImageFlx *pIm);
  static const char *GetSaveFormat(BMPSAVEFORMAT *pFmt,CImagePxm *pIm);
  static const char *GetSaveFormat(BMPSAVEFORMAT *pFmt,CImageYuv *pIm);
  static const char *GetSaveFormat(BMPSAVEFORMAT *pFmt,CImagePlRaw *pIm);
  static const char *GetSaveFormat(BMPSAVEFORMAT *pFmt,CImageNRaw *pIm);
  static const char *GetSaveFormat(BMPSAVEFORMAT *pFmt,CImageAptinaRaw *pIm);
  };

#endif	//_IMAGE_H_INCLUDED_
