/*------------------------------------------------------------------------------
--                                                                                                                               --
--       This software is confidential and proprietary and may be used                                   --
--        only as expressly authorized by a licensing agreement from                                     --
--                                                                                                                               --
--                            Verisilicon.                                                                                    --
--                                                                                                                               --
--                   (C) COPYRIGHT 2014 VERISILICON                                                            --
--                            ALL RIGHTS RESERVED                                                                    --
--                                                                                                                               --
--                 The entire notice above must be reproduced                                                  --
--                  on all copies and should not be removed.                                                     --
--                                                                                                                                --
--------------------------------------------------------------------------------
--
--  Abstract : HEVC Encoder API
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
       Version Information
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> /* for pow */
#include <unistd.h>

#include "hevcencapi.h"

#include "base_type.h"
#include "error.h"
#include "instance.h"
#include "sw_parameter_set.h"
#include "rate_control_picture.h"
#include "sw_slice.h"
#include "tools.h"
#include "enccommon.h"
#include "hevcApiVersion.h"
#include "enccfg.h"



#ifdef INTERNAL_TEST
#include "sw_test_id.h"
#endif

#ifdef TEST_DATA
#include "enctrace.h"
#endif

/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------

#define EVALUATION_LIMIT 300    max number of frames to encode

--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/
#define HEVCENC_SW_BUILD ((HEVCENC_BUILD_MAJOR * 1000000) + \
  (HEVCENC_BUILD_MINOR * 1000) + HEVCENC_BUILD_REVISION)

#define PRM_SET_BUFF_SIZE 1024    /* Parameter sets max buffer size */
#define HEVCENC_MAX_BITRATE             (100000*1000)    /* Level 6 main tier limit */

/* HW ID check. HEVCEncInit() will fail if HW doesn't match. */
#define HW_ID_MASK  0xFFFF0000
#define HW_ID       0x48320000

#define HEVCENC_MIN_ENC_WIDTH       64 //132     /* 144 - 12 pixels overfill */
#define HEVCENC_MAX_ENC_WIDTH       4096
#define HEVCENC_MIN_ENC_HEIGHT      64 //96
#define HEVCENC_MAX_ENC_HEIGHT      8192
#define HEVCENC_MAX_LEVEL           HEVCENC_LEVEL_6

#define HEVCENC_MAX_CTUS_PER_PIC     (4096*4096/(64*64))   /* 4096x2176 */

#define HEVCENC_DEFAULT_QP          26

#define HEVCENC_MAX_PP_INPUT_WIDTH      8192
#define HEVCENC_MAX_PP_INPUT_HEIGHT     8192

#define HEVCENCSTRMSTART_MIN_BUF        1024*11
#define HEVCENCSTRMENCODE_MIN_BUF       1024*11

#define HEVCENC_MAX_USER_DATA_SIZE      2048


#define HEVC_BUS_ADDRESS_VALID(bus_address)  (((bus_address) != 0) && \
                                              ((bus_address & 0x0F) == 0))

#define HEVC_BUS_CH_ADDRESS_VALID(bus_address)  (((bus_address) != 0) && \
                                              ((bus_address & 0x07) == 0))

typedef enum
{
  RCROIMODE_RC_ROIMAP_ROIAREA_DIABLE = 0,
  RCROIMODE_ROIAREA_ENABLE = 1,
  RCROIMODE_ROIMAP_ENABLE = 2,
  RCROIMODE_ROIMAP_ROIAREA_ENABLE = 3,
  RCROIMODE_RC_ENABLE = 4,
  RCROIMODE_RC_ROIAREA_ENABLE=5,
  RCROIMODE_RC_ROIMAP_ENABLE=6,
  RCROIMODE_RC_ROIMAP_ROIAREA_ENABLE=7

} RCROIMODEENABLE;

/* Tracing macro */
//#define HEVCENC_TRACE

#define  TempTrace(x)  if(1){printf("%s\n", x);};
#ifdef HEVCENC_TRACE

#define APITRACE(str) TempTrace(str)
#define APITRACEPARAM_X(str, val) \
  { char tmpstr[255]; sprintf(tmpstr, "  %s: 0x%x", str, (u32)val); TempTrace(tmpstr); }
#define APITRACEPARAM(str, val) \
  { char tmpstr[255]; sprintf(tmpstr, "  %s: %d", str, (int)val); TempTrace(tmpstr); }

#else
#define APITRACE(str)
#define APITRACEPARAM_X(str, val)
#define APITRACEPARAM(str, val)
#endif


#if 0
/* Tracing macro */
#ifdef HEVCENC_TRACE

#define APITRACE(str) HEVCEncTrace(str)
#define APITRACEPARAM_X(str, val) \
  { char tmpstr[255]; sprintf(tmpstr, "  %s: 0x%x", str, (u32)val); HEVCEncTrace(tmpstr); }
#define APITRACEPARAM(str, val) \
  { char tmpstr[255]; sprintf(tmpstr, "  %s: %d", str, (int)val); HEVCEncTrace(tmpstr); }

#else
#define APITRACE(str)
#define APITRACEPARAM_X(str, val)
#define APITRACEPARAM(str, val)
#endif


#endif


const u32 HEVCMaxSBPS[13] =
{
  552960, 3686400, 7372800, 16588800, 33177600, 66846720, 133693440, 267386880,
  534773760, 1069547520, 1069547520, 2139095040, 4278190080
};
const u32 HEVCMaxCPBS[13] =
{
  350000, 1500000, 3000000, 6000000, 10000000, 12000000, 20000000,
  25000000, 40000000, 60000000, 60000000, 120000000, 240000000
};

const u32 HEVCMaxBR[13] =
{
  350000, 1500000, 3000000, 6000000, 10000000, 12000000, 20000000,
  25000000, 40000000, 60000000, 60000000, 120000000, 240000000
};

const u32 HEVCMaxFS[13] = { 36864, 122880, 245760, 552960, 983040, 2228224, 2228224, 8912896,
                            8912896, 8912896, 35651584, 35651584, 35651584
                          };

static const u32 HEVCIntraPenalty[52] =
{
  24, 24, 24, 26, 27, 30, 32, 35, 39, 43, 48, 53, 58, 64, 71, 78,
  85, 93, 102, 111, 121, 131, 142, 154, 167, 180, 195, 211, 229,
  248, 271, 296, 326, 361, 404, 457, 523, 607, 714, 852, 1034,
  1272, 1588, 2008, 2568, 3318, 4323, 5672, 7486, 9928, 13216,
  17648
};//max*3=16bit


/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/
static i32 out_of_memory(struct hevc_buffer *n, u32 size);
static i32 get_buffer(struct buffer *, struct hevc_buffer *, i32 size, i32 reset);
static i32 init_buffer(struct hevc_buffer *, struct hevc_instance *);
static i32 set_parameter(struct hevc_instance *hevc_instance, const HEVCEncIn *pEncIn,
                         struct vps *v, struct sps *s, struct pps *p);
static bool_e hevcCheckCfg(const HEVCEncConfig *pEncCfg);
static bool_e SetParameter(struct hevc_instance *inst, const HEVCEncConfig   *pEncCfg);
static i32 hevc_set_ref_pic_set(struct hevc_instance *hevc_instance);
static i32 hevc_ref_pic_sets(struct hevc_instance *hevc_instance, HEVCGopConfig *gopCfg);
static i32 HEVCGetAllowedWidth(i32 width, HEVCEncPictureType inputType);
static void IntraLamdaQp(int qp, u32 *lamda_sad, enum slice_type type, int poc, double dQPFactor, bool depth0);
static void InterLamdaQp(int qp, unsigned int *lamda_sse, unsigned int *lamda_sad, enum slice_type type, int poc, double dQPFactor, bool depth0);
static void FillIntraFactor(regValues_s *regs);
static void LamdaTableQp(regValues_s *regs, int qp, enum slice_type type, int poc, double dQPFactor, bool depth0, bool ctbRc);
static void HEVCEncSetQuantParameters(asicData_s *asic, double qp_factor,
                                      enum slice_type type, bool is_depth0,
                                      true_e enable_ctu_rc);
static void HevcSetNewFrame(HEVCEncInst inst);

/* DeNoise Filter */
static void HEVCEncDnfInit(struct hevc_instance *hevc_instance);
static void HEVCEncDnfSetParameters(struct hevc_instance *hevc_instance, const HEVCEncCodingCtrl *pCodeParams);
static void HEVCEncDnfGetParameters(struct hevc_instance *hevc_instance, HEVCEncCodingCtrl *pCodeParams);
static void HEVCEncDnfPrepare(struct hevc_instance *hevc_instance);
static void HEVCEncDnfUpdate(struct hevc_instance *hevc_instance);


/*------------------------------------------------------------------------------

    Function name : HEVCEncGetApiVersion
    Description   : Return the API version info

    Return type   : HEVCEncApiVersion
    Argument      : void
------------------------------------------------------------------------------*/
HEVCEncApiVersion HEVCEncGetApiVersion(void)
{
  HEVCEncApiVersion ver;

  ver.major = HEVCENC_MAJOR_VERSION;
  ver.minor = HEVCENC_MINOR_VERSION;

  APITRACE("HEVCEncGetApiVersion# OK");
  return ver;
}

/*------------------------------------------------------------------------------
    Function name : HEVCEncGetBuild
    Description   : Return the SW and HW build information

    Return type   : HEVCEncBuild
    Argument      : void
------------------------------------------------------------------------------*/
HEVCEncBuild HEVCEncGetBuild(void)
{
  HEVCEncBuild ver;

  ver.swBuild = HEVCENC_SW_BUILD;
  ver.hwBuild = EWLReadAsicID();

  APITRACE("HEVCEncGetBuild# OK");

  return (ver);
}
/*------------------------------------------------------------------------------
  hevc_init initializes the encoder and creates new encoder instance.
------------------------------------------------------------------------------*/
void ShowHevcEncInit(HEVCEncIn *in)
{
  printf("############Show Hevc Enc Init##########\n");

}
void ShowHevcEncIn(const HEVCEncIn *cfg)
{

  #if 0
  typedef struct
  {
    u32 busLuma;         /* Bus address for input picture
                              * planar format: luminance component
                              * semiplanar format: luminance component
                              * interleaved format: whole picture
                              */
    u32 busChromaU;      /* Bus address for input chrominance
                              * planar format: cb component
                              * semiplanar format: both chrominance
                              * interleaved format: not used
                              */
    u32 busChromaV;      /* Bus address for input chrominance
                              * planar format: cr component
                              * semiplanar format: not used
                              * interleaved format: not used
                              */
    u32 timeIncrement;   /* The previous picture duration in units
                              * of 1/frameRateNum. 0 for the very first picture
                              * and typically equal to frameRateDenom for the rest.
                              */
    u32 *pOutBuf;        /* Pointer to output stream buffer */
    u32 busOutBuf;       /* Bus address of output stream buffer */
    u32 outBufSize;      /* Size of output stream buffer in bytes */
    HEVCEncPictureCodingType codingType;    /* Proposed picture coding type,
                                                 * INTRA/PREDICTED
                                                 */
    i32 poc;      /* Picture display order count */
    HEVCGopConfig gopConfig; /* GOP configuration*/
    i32 gopSize;  /* current GOP size*/
    i32 gopPicIdx;   /* encoded order count of current picture within its GOP, shoule be in the range of [0, gopSize-1] */
    u32 roiMapDeltaQpAddr;   /* Pointer of QpDelta map   */
  } HEVCEncIn;
    typedef struct
  {
    u32 poc; /* picture order count within a GOP */
    i32 QpOffset; /* QP offset */
    double QpFactor; /* QP Factor */
    i32 temporalId;
    HEVCEncPictureCodingType codingType; /* picture coding type */
    u32 numRefPics; /* the number of reference pictures kept for this picture, the value should be within [0, HEVCENC_MAX_REF_FRAMES] */
    HEVCGopPicRps refPics[HEVCENC_MAX_REF_FRAMES]; /* reference picture sets for this picture*/
  } HEVCGopPicConfig;
 #endif
  printf("############Show Hevc Enc In  ##########\n");
  printf("bus ->y:%8X u:%8X v:%8X\n", cfg->busLuma, cfg->busChromaU, cfg->busChromaV);
  printf("timeIncrement   ->%8d\n", cfg->timeIncrement);
  printf("busOutBuf   ->%8X\n", cfg->busOutBuf);
  printf("outBufSize   ->%8d\n", cfg->outBufSize);
  printf("codingType   ->%8d\n", cfg->codingType);
  printf("gopSize   ->%8d\n", cfg->gopSize);
  printf("gopPicIdx   ->%8d\n", cfg->gopPicIdx);
  printf("roiMapDeltaQpAddr   ->%8d\n", cfg->roiMapDeltaQpAddr);
  printf("gop config ->size :%d  id:%d\n", cfg->gopConfig.size, cfg->gopConfig.id);
  HEVCGopPicConfig *pGopPicCfg = cfg->gopConfig.pGopPicCfg;
  for(int i = 0;i < cfg->gopConfig.size; i++ ){
    printf("\tpoc   -> %d\n", pGopPicCfg->poc  );
    printf("\tQpOffset   -> %d\n", pGopPicCfg->QpOffset  );
    printf("\tQpFactor   -> %2.4f\n", pGopPicCfg->QpFactor  );
    printf("\ttemporalId   -> %d\n", pGopPicCfg->temporalId  );
    printf("\tcodingType   -> %d\n", pGopPicCfg->codingType  );
    printf("\tnumRefPics   -> %d\n", pGopPicCfg->numRefPics  );
    for(int j = 0;j < pGopPicCfg->numRefPics;j++){
      printf("\t\t %d ->%d\n", j, pGopPicCfg->refPics[j]);
    }
  }
}

HEVCEncRet HEVCEncInit(HEVCEncConfig *config, HEVCEncInst *instAddr)
{
  struct instance *instance;
  const void *ewl = NULL;
  EWLInitParam_t param;
  struct hevc_instance *hevc_inst;
  struct container *c;

  regValues_s *regs;
  preProcess_s *pPP;
  struct vps *v;
  struct sps *s;
  struct pps *p;
  HEVCEncRet ret;

  APITRACE("HEVCEncInit#");

  APITRACEPARAM("streamType", config->streamType);
  APITRACEPARAM("profile", config->profile);
  APITRACEPARAM("level", config->level);
  APITRACEPARAM("width", config->width);
  APITRACEPARAM("height", config->height);


  APITRACEPARAM("frameRateNum", config->frameRateNum);
  APITRACEPARAM("frameRateDenom", config->frameRateDenom);
  APITRACEPARAM("refFrameAmount", config->refFrameAmount);
  APITRACEPARAM("strongIntraSmoothing", config->strongIntraSmoothing);
  APITRACEPARAM("compressor", config->compressor);
  APITRACEPARAM("interlacedFrame", config->interlacedFrame);
  APITRACEPARAM("bitDepthLuma", config->bitDepthLuma);
  APITRACEPARAM("bitDepthChroma", config->bitDepthChroma);

  /* check that right shift on negative numbers is performed signed */
  /*lint -save -e* following check causes multiple lint messages */
#if (((-1) >> 1) != (-1))
#error Right bit-shifting (>>) does not preserve the sign
#endif
  /*lint -restore */

  /* Check for illegal inputs */
  if (config == NULL || instAddr == NULL)
  {
    APITRACE("HEVCEncInit: ERROR Null argument");
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for correct HW */
  if ((EWLReadAsicID() & HW_ID_MASK) != HW_ID)
  {
    APITRACE("HEVCEncInit: ERROR Invalid HW ID");
    return HEVCENC_ERROR;
  }

  /* Check that configuration is valid */
  if (hevcCheckCfg(config) == ENCHW_NOK)
  {
    APITRACE("HEVCEncInit: ERROR Invalid configuration");
    return HEVCENC_INVALID_ARGUMENT;
  }


  param.clientType = EWL_CLIENT_TYPE_HEVC_ENC;

  /* Init EWL */
  if ((ewl = EWLInit(&param)) == NULL)
  {
    APITRACE("HEVCEncInit: EWL Initialization failed");
    return HEVCENC_EWL_ERROR;
  }


  *instAddr = NULL;
  if (!(instance = EWLcalloc(1, sizeof(struct instance))))
  {
    ret = HEVCENC_MEMORY_ERROR;
    goto error;
  }

  instance->instance = (struct hevc_instance *)instance;
  *instAddr = (HEVCEncInst)instance;
  ASSERT(instance == (struct instance *)&instance->hevc_instance);
  hevc_inst = (struct hevc_instance *)instance;
  if (!(c = get_container(hevc_inst)))
  {
    ret = HEVCENC_MEMORY_ERROR;
    goto error;
  }

  /* Create parameter sets */
  v = (struct vps *)create_parameter_set(VPS_NUT);
  s = (struct sps *)create_parameter_set(SPS_NUT);
  p = (struct pps *)create_parameter_set(PPS_NUT);

  /* Replace old parameter set with new ones */
  remove_parameter_set(c, VPS_NUT, 0);
  remove_parameter_set(c, SPS_NUT, 0);
  remove_parameter_set(c, PPS_NUT, 0);

  queue_put(&c->parameter_set, (struct node *)v);
  queue_put(&c->parameter_set, (struct node *)s);
  queue_put(&c->parameter_set, (struct node *)p);
  hevc_inst->sps = s;
  hevc_inst->vps = v;
  hevc_inst->interlaced = config->interlacedFrame;

  /* Initialize ASIC */

  hevc_inst->asic.ewl = ewl;
  (void) EncAsicControllerInit(&hevc_inst->asic);

  hevc_inst->width = config->width;
  hevc_inst->height = config->height / (hevc_inst->interlaced + 1);

  /* Allocate internal SW/HW shared memories */
  if (EncAsicMemAlloc_V2(&hevc_inst->asic,
                         config->width,
                         config->height / (hevc_inst->interlaced + 1),
                         0,
                         0,
                         ASIC_HEVC, config->refFrameAmount,
                         config->refFrameAmount,
                         config->compressor,
                         config->bitDepthLuma,
                         config->bitDepthChroma) != ENCHW_OK)
  {

    ret = HEVCENC_EWL_MEMORY_ERROR;
    goto error;
  }

  /* Set parameters depending on user config */
  if (SetParameter(hevc_inst, config) != ENCHW_OK)
  {
    ret = HEVCENC_INVALID_ARGUMENT;
    goto error;
  }
  hevc_inst->rateControl.monitorFrames = config->frameRateNum / config->frameRateDenom;
  hevc_inst->rateControl.picRc = ENCHW_NO;
  hevc_inst->rateControl.picSkip = ENCHW_NO;
  hevc_inst->rateControl.qpHdr = -1<<QP_FRACTIONAL_BITS;
  hevc_inst->rateControl.qpMin = 0<<QP_FRACTIONAL_BITS;
  hevc_inst->rateControl.qpMax = 51<<QP_FRACTIONAL_BITS;
  hevc_inst->rateControl.hrd == ENCHW_NO;
  hevc_inst->rateControl.virtualBuffer.bitRate = 1000000;
  hevc_inst->rateControl.virtualBuffer.bufferSize = 1000000;
  hevc_inst->rateControl.gopLen = 150;

  hevc_inst->rateControl.intraQpDelta = -5 << QP_FRACTIONAL_BITS;
  hevc_inst->rateControl.fixedIntraQp = 0 << QP_FRACTIONAL_BITS;
  hevc_inst->rateControl.tolMovingBitRate = 2000;
  hevc_inst->rateControl.f_tolMovingBitRate = 2000.0;

  hevc_inst->rateControl.maxPicSizeI = HevcCalculate(hevc_inst->rateControl.virtualBuffer.bitRate, hevc_inst->rateControl.outRateDenom, hevc_inst->rateControl.outRateNum)*(1+20) ;
  hevc_inst->rateControl.maxPicSizeP = HevcCalculate(hevc_inst->rateControl.virtualBuffer.bitRate, hevc_inst->rateControl.outRateDenom, hevc_inst->rateControl.outRateNum)*(1+20);
  hevc_inst->rateControl.maxPicSizeB = HevcCalculate(hevc_inst->rateControl.virtualBuffer.bitRate, hevc_inst->rateControl.outRateDenom, hevc_inst->rateControl.outRateNum)*(1+20) ;

  hevc_inst->rateControl.minPicSizeI = HevcCalculate(hevc_inst->rateControl.virtualBuffer.bitRate, hevc_inst->rateControl.outRateDenom, hevc_inst->rateControl.outRateNum)/(1+20) ;
  hevc_inst->rateControl.minPicSizeP = HevcCalculate(hevc_inst->rateControl.virtualBuffer.bitRate, hevc_inst->rateControl.outRateDenom, hevc_inst->rateControl.outRateNum)/(1+20) ;
  hevc_inst->rateControl.minPicSizeB = HevcCalculate(hevc_inst->rateControl.virtualBuffer.bitRate, hevc_inst->rateControl.outRateDenom, hevc_inst->rateControl.outRateNum)/(1+20) ;
  hevc_inst->blockRCSize = 0;
  if (HevcInitRc(&hevc_inst->rateControl, 1) != ENCHW_OK)
  {
    ret = HEVCENC_INVALID_ARGUMENT;
    goto error;
  }
  hevc_inst->rateControl.sei.enabled = ENCHW_NO;
  hevc_inst->sps->vui.videoFullRange = ENCHW_NO;
  hevc_inst->disableDeblocking = 0;
  hevc_inst->tc_Offset = -2;
  hevc_inst->beta_Offset = 5;
  hevc_inst->enableSao = 1;
  hevc_inst->enableScalingList = 0;
  hevc_inst->sarWidth = 0;
  hevc_inst->sarHeight = 0;

  hevc_inst->max_cu_size = 64;   /* Max coding unit size in pixels */
  hevc_inst->min_cu_size = 8;   /* Min coding unit size in pixels */
  hevc_inst->max_tr_size = 16;   /* Max transform size in pixels */
  hevc_inst->min_tr_size = 4;   /* Min transform size in pixels */

  HEVCEncDnfInit(hevc_inst);

  hevc_inst->tr_depth_intra = 2;   /* Max transform hierarchy depth */
  hevc_inst->tr_depth_inter = (hevc_inst->max_cu_size == 64) ? 4 : 3;   /* Max transform hierarchy depth */

  hevc_inst->fieldOrder = 0;
  hevc_inst->chromaQpOffset = 0;
  hevc_inst->rateControl.sei.insertRecoveryPointMessage = 0;
  hevc_inst->rateControl.sei.recoveryFrameCnt = 0;

  regs = &hevc_inst->asic.regs;

  regs->cabac_init_flag = 0;
  regs->cirStart = 0;
  regs->cirInterval = 0;
  regs->intraAreaTop = regs->intraAreaLeft =
                         regs->intraAreaBottom = regs->intraAreaRight = 255;


  regs->roi1Top = regs->roi1Left =
                    regs->roi1Bottom = regs->roi1Right = 255;
  hevc_inst->roi1Enable = 0;

  regs->roi2Top = regs->roi2Left =
                    regs->roi2Bottom = regs->roi2Right = 255;
  hevc_inst->roi2Enable = 0;
  regs->roi1DeltaQp = 0;
  regs->roi2DeltaQp = 0;

  hevc_inst->roiMapEnable = 0;
  hevc_inst->ctbRCEnable = 0;
  regs->rcRoiEnable = 0x00;
  regs->diffCuQpDeltaDepth = 0;
  /* Status == INIT   Initialization succesful */
  hevc_inst->encStatus = HEVCENCSTAT_INIT;
  hevc_inst->created_pic_num = 0;

  hevc_inst->inst = hevc_inst;
  APITRACE("HEVCEncInit: OK");
  return HEVCENC_OK;

error:
  if (instance != NULL)
    EWLfree(instance);
  if (ewl != NULL)
    (void) EWLRelease(ewl);

  APITRACE("HEVCEncInit: ERROR Initialization failed");
  return ret;

}
/*------------------------------------------------------------------------------

    HEVCShutdown

    Function frees the encoder instance.

    Input   inst    Pointer to the encoder instance to be freed.
                            After this the pointer is no longer valid.

------------------------------------------------------------------------------*/
void HEVCShutdown(HEVCEncInst inst)
{
  struct hevc_instance *pEncInst = (struct hevc_instance *) inst;
  const void *ewl;

  ASSERT(inst);

  ewl = pEncInst->asic.ewl;

  EncAsicMemFree_V2(&pEncInst->asic);


  EWLfree(pEncInst);

  (void) EWLRelease(ewl);
}

/*------------------------------------------------------------------------------

    Function name : HEVCEncRelease
    Description   : Releases encoder instance and all associated resource

    Return type   : HEVCEncRet
    Argument      : inst - the instance to be released
------------------------------------------------------------------------------*/
HEVCEncRet HEVCEncRelease(HEVCEncInst inst)
{
  struct hevc_instance *pEncInst = (struct hevc_instance *) inst;
  struct container *c;

  APITRACE("HEVCEncRelease#");

  /* Check for illegal inputs */
  if (pEncInst == NULL)
  {
    APITRACE("HEVCEncRelease: ERROR Null argument");
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (pEncInst->inst != inst)
  {
    APITRACE("HEVCEncRelease: ERROR Invalid instance");
    return HEVCENC_INSTANCE_ERROR;
  }


  if (!(c = get_container(pEncInst))) return HEVCENC_ERROR;

  sw_free_pictures(c);  /* Call this before free_parameter_sets() */
  free_parameter_sets(c);

  HEVCShutdown((struct instance *)pEncInst);
  APITRACE("HEVCEncRelease: OK");
  return HEVCENC_OK;
}
/*------------------------------------------------------------------------------

    HEVCGetPerformance

    Function frees the encoder instance.

    Input   inst    Pointer to the encoder instance to be freed.
                            After this the pointer is no longer valid.

------------------------------------------------------------------------------*/
u32 HEVCGetPerformance(HEVCEncInst inst)
{
  struct hevc_instance *pEncInst = (struct hevc_instance *) inst;
  const void *ewl;
  u32 performanceData;
  ASSERT(inst);
  APITRACE("HEVCGetPerformance#");
  /* Check for illegal inputs */
  if (pEncInst == NULL)
  {
    APITRACE("HEVCGetPerformance: ERROR Null argument");
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (pEncInst->inst != inst)
  {
    APITRACE("HEVCGetPerformance: ERROR Invalid instance");
    return HEVCENC_INSTANCE_ERROR;
  }

  ewl = pEncInst->asic.ewl;
  performanceData = EncAsicGetPerformance(ewl);
  APITRACE("HEVCGetPerformance:OK");
  return performanceData;
}

/*------------------------------------------------------------------------------
  get_container
------------------------------------------------------------------------------*/
struct container *get_container(struct hevc_instance *hevc_instance)
{
  struct instance *instance = (struct instance *)hevc_instance;

  if (!instance || (instance->instance != hevc_instance)) return NULL;

  return &instance->container;
}

/*------------------------------------------------------------------------------
  hevc_set_parameter
------------------------------------------------------------------------------*/
HEVCEncRet HEVCEncSetCodingCtrl(HEVCEncInst instAddr, const HEVCEncCodingCtrl *pCodeParams)
{
  struct hevc_instance *pEncInst = (struct hevc_instance *) instAddr;
  regValues_s *regs = &pEncInst->asic.regs;

  APITRACE("HEVCEncSetCodingCtrl#");
  APITRACEPARAM("sliceSize", pCodeParams->sliceSize);
  APITRACEPARAM("seiMessages", pCodeParams->seiMessages);
  APITRACEPARAM("videoFullRange", pCodeParams->videoFullRange);
  APITRACEPARAM("disableDeblockingFilter", pCodeParams->disableDeblockingFilter);
  APITRACEPARAM("tc_Offset", pCodeParams->tc_Offset);
  APITRACEPARAM("beta_Offset", pCodeParams->beta_Offset);
  APITRACEPARAM("enableDeblockOverride", pCodeParams->enableDeblockOverride);
  APITRACEPARAM("deblockOverride", pCodeParams->deblockOverride);
  APITRACEPARAM("enableSao", pCodeParams->enableSao);
  APITRACEPARAM("enableScalingList", pCodeParams->enableScalingList);
  APITRACEPARAM("sampleAspectRatioWidth", pCodeParams->sampleAspectRatioWidth);
  APITRACEPARAM("sampleAspectRatioHeight", pCodeParams->sampleAspectRatioHeight);
  APITRACEPARAM("cabacInitFlag", pCodeParams->cabacInitFlag);
  APITRACEPARAM("cirStart", pCodeParams->cirStart);
  APITRACEPARAM("cirInterval", pCodeParams->cirInterval);
  APITRACEPARAM("intraArea.enable", pCodeParams->intraArea.enable);
  APITRACEPARAM("intraArea.top", pCodeParams->intraArea.top);
  APITRACEPARAM("intraArea.bottom", pCodeParams->intraArea.bottom);
  APITRACEPARAM("intraArea.left", pCodeParams->intraArea.left);
  APITRACEPARAM("intraArea.right", pCodeParams->intraArea.right);
  APITRACEPARAM("roi1Area.enable", pCodeParams->roi1Area.enable);
  APITRACEPARAM("roi1Area.top", pCodeParams->roi1Area.top);
  APITRACEPARAM("roi1Area.bottom", pCodeParams->roi1Area.bottom);
  APITRACEPARAM("roi1Area.left", pCodeParams->roi1Area.left);
  APITRACEPARAM("roi1Area.right", pCodeParams->roi1Area.right);
  APITRACEPARAM("roi2Area.enable", pCodeParams->roi2Area.enable);
  APITRACEPARAM("roi2Area.top", pCodeParams->roi2Area.top);
  APITRACEPARAM("roi2Area.bottom", pCodeParams->roi2Area.bottom);
  APITRACEPARAM("roi2Area.left", pCodeParams->roi2Area.left);
  APITRACEPARAM("roi2Area.right", pCodeParams->roi2Area.right);
  APITRACEPARAM("roi1DeltaQp", pCodeParams->roi1DeltaQp);
  APITRACEPARAM("roi2DeltaQp", pCodeParams->roi2DeltaQp);

  APITRACEPARAM("chroma_qp_offset", pCodeParams->chroma_qp_offset);
  APITRACEPARAM("fieldOrder", pCodeParams->fieldOrder);
  APITRACEPARAM("gdrDuration", pCodeParams->gdrDuration);
  APITRACEPARAM("roiMapDeltaQpEnable", pCodeParams->roiMapDeltaQpEnable);
  APITRACEPARAM("roiMapDeltaQpBlockUnit", pCodeParams->roiMapDeltaQpBlockUnit);

  /* Check for illegal inputs */
  if ((pEncInst == NULL) || (pCodeParams == NULL))
  {
    APITRACE("HEVCEncSetCodingCtrl: ERROR Null argument");
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (pEncInst->inst != pEncInst)
  {
    APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid instance");
    return HEVCENC_INSTANCE_ERROR;
  }
  {
    //EWLHwConfig_t cfg = EWLReadAsicConfig();  /add by bright
    //if (cfg.deNoiseEnabled == EWL_HW_CONFIG_NOT_SUPPORTED &&
    if (
        pCodeParams->noiseReductionEnable == 1)
    {
      APITRACE("HEVCEncSetCodingCtrl: ERROR noise reduction not supported");
      return HEVCENC_INVALID_ARGUMENT;
    }

    if ((regs->asicHwId < 0x48320200 /* H2V1 */)&&(pCodeParams->roiMapDeltaQpEnable == 1))
    {
      APITRACE("HEVCEncSetCodingCtrl: ERROR ROI MAP not supported");
      return HEVCENC_INVALID_ARGUMENT;
    }

    if ((pEncInst->encStatus >= HEVCENCSTAT_START_STREAM)&&(pEncInst->roiMapEnable != pCodeParams->roiMapDeltaQpEnable))
    {
      //roi can never set if enable this check
      /*
      APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid encoding status to config roiMapDeltaQpEnable");
      return HEVCENC_INVALID_ARGUMENT;
      */
    }
  }
  /* Check for invalid values */
  if (pCodeParams->sliceSize > (u32)pEncInst->ctbPerCol)
  {
    APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid sliceSize");
    return HEVCENC_INVALID_ARGUMENT;
  }
  if ((pEncInst->encStatus >= HEVCENCSTAT_START_STREAM)&&(pEncInst->gdrEnabled != (pCodeParams->gdrDuration > 0)))
  {
    APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid encoding status to config gdrDuration");
    return HEVCENC_INVALID_ARGUMENT;
  }
  if (pCodeParams->cirStart > (u32)pEncInst->ctbPerFrame ||
      pCodeParams->cirInterval > (u32)pEncInst->ctbPerFrame)
  {
    APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid CIR value");
    return HEVCENC_INVALID_ARGUMENT;
  }
  if (pCodeParams->intraArea.enable)
  {
    if (!(pCodeParams->intraArea.top <= pCodeParams->intraArea.bottom &&
          pCodeParams->intraArea.bottom < (u32)pEncInst->ctbPerCol &&
          pCodeParams->intraArea.left <= pCodeParams->intraArea.right &&
          pCodeParams->intraArea.right < (u32)pEncInst->ctbPerRow) || (pCodeParams->gdrDuration > 0))
    {
      APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid intraArea");
      return HEVCENC_INVALID_ARGUMENT;
    }
  }
  if((pEncInst->encStatus >= HEVCENCSTAT_START_STREAM)&&(pEncInst->roi1Enable != pCodeParams->roi1Area.enable))
  {
    //roi can never set if enable this check
    /*
    APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid encoding status to config roi1Area");
    return HEVCENC_INVALID_ARGUMENT;
    */
  }
  if (pCodeParams->roi1Area.enable)
  {
    if (!(pCodeParams->roi1Area.top <= pCodeParams->roi1Area.bottom &&
          pCodeParams->roi1Area.bottom < (u32)pEncInst->ctbPerCol &&
          pCodeParams->roi1Area.left <= pCodeParams->roi1Area.right &&
          pCodeParams->roi1Area.right < (u32)pEncInst->ctbPerRow) || (pCodeParams->gdrDuration > 0))
    {
      APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid roi1Area");
      return HEVCENC_INVALID_ARGUMENT;
    }
  }

  if((pEncInst->encStatus >= HEVCENCSTAT_START_STREAM)&&(pEncInst->roi2Enable != pCodeParams->roi2Area.enable))
  {
    //roi can never set if enable this check
    /*
    APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid encoding status to config roi2Area");
    eturn HEVCENC_INVALID_ARGUMENT;
    */
  }

  if (pCodeParams->roi2Area.enable)
  {
    if (!(pCodeParams->roi2Area.top <= pCodeParams->roi2Area.bottom &&
          pCodeParams->roi2Area.bottom < (u32)pEncInst->ctbPerCol &&
          pCodeParams->roi2Area.left <= pCodeParams->roi2Area.right &&
          pCodeParams->roi2Area.right < (u32)pEncInst->ctbPerRow))
    {
      APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid roi2Area");
      return HEVCENC_INVALID_ARGUMENT;
    }
  }

  if (pCodeParams->roi1DeltaQp < -30 ||
      pCodeParams->roi1DeltaQp > 0 ||
      pCodeParams->roi2DeltaQp < -30 ||
      pCodeParams->roi2DeltaQp > 0 /*||
     pCodeParams->adaptiveRoi < -51 ||
     pCodeParams->adaptiveRoi > 0*/)
  {
    APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid ROI delta QP");
    return HEVCENC_INVALID_ARGUMENT;
  }



  /* Check status, only slice size, CIR & ROI allowed to change after start */
  if (pEncInst->encStatus != HEVCENCSTAT_INIT)
  {
    goto set_slice_size;
  }


  if (pCodeParams->enableSao > 1 || pCodeParams->enableSao > 1 || pCodeParams->roiMapDeltaQpEnable > 1 ||
      pCodeParams->disableDeblockingFilter > 1 ||
      pCodeParams->seiMessages > 1 || pCodeParams->videoFullRange > 1)
  {
    APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid enable/disable");
    return HEVCENC_INVALID_ARGUMENT;
  }

  if (pCodeParams->sampleAspectRatioWidth > 65535 ||
      pCodeParams->sampleAspectRatioHeight > 65535)
  {
    APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid sampleAspectRatio");
    return HEVCENC_INVALID_ARGUMENT;
  }


  if (pCodeParams->cabacInitFlag > 1)
  {
    APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid cabacInitIdc");
    return HEVCENC_INVALID_ARGUMENT;
  }

  pEncInst->chromaQpOffset = pCodeParams->chroma_qp_offset;
  pEncInst->fieldOrder = pCodeParams->fieldOrder ? 1 : 0;
  pEncInst->disableDeblocking = pCodeParams->disableDeblockingFilter;
  pEncInst->enableDeblockOverride = pCodeParams->enableDeblockOverride;
  if (pEncInst->enableDeblockOverride)
  {
    regs->slice_deblocking_filter_override_flag = pCodeParams->deblockOverride;
  }
  else
  {
    regs->slice_deblocking_filter_override_flag = 0;
  }
  pEncInst->tc_Offset = pCodeParams->tc_Offset;
  pEncInst->beta_Offset = pCodeParams->beta_Offset;
  pEncInst->enableSao = pCodeParams->enableSao;

  HEVCEncDnfSetParameters(pEncInst, pCodeParams);

  regs->cabac_init_flag = pCodeParams->cabacInitFlag;
  pEncInst->sarWidth = pCodeParams->sampleAspectRatioWidth;
  pEncInst->sarHeight = pCodeParams->sampleAspectRatioHeight;


  pEncInst->videoFullRange = pCodeParams->videoFullRange;

  /* SEI messages are written in the beginning of each frame */
  if (pCodeParams->seiMessages)
    pEncInst->rateControl.sei.enabled = ENCHW_YES;
  else
    pEncInst->rateControl.sei.enabled = ENCHW_NO;


set_slice_size:

  regs->sliceSize = pCodeParams->sliceSize;
  regs->sliceNum = (regs->sliceSize == 0) ?  1 : ((pEncInst->ctbPerCol + (regs->sliceSize - 1)) / regs->sliceSize);
  pEncInst->enableScalingList = pCodeParams->enableScalingList;
  if (pCodeParams->roiMapDeltaQpEnable)
  {
    pEncInst->min_qp_size = 1 << (6 - pCodeParams->roiMapDeltaQpBlockUnit);
  }
  else
    pEncInst->min_qp_size = 64;

  /* Set CIR, intra forcing and ROI parameters */
  regs->cirStart = pCodeParams->cirStart;
  regs->cirInterval = pCodeParams->cirInterval;

  if (pCodeParams->intraArea.enable)
  {
    regs->intraAreaTop = pCodeParams->intraArea.top;
    regs->intraAreaLeft = pCodeParams->intraArea.left;
    regs->intraAreaBottom = pCodeParams->intraArea.bottom;
    regs->intraAreaRight = pCodeParams->intraArea.right;
  }
  else
  {
    regs->intraAreaTop = regs->intraAreaLeft = regs->intraAreaBottom =
                           regs->intraAreaRight = 255;
  }

  if (pCodeParams->roi1Area.enable)
  {
    regs->roi1Top = pCodeParams->roi1Area.top;
    regs->roi1Left = pCodeParams->roi1Area.left;
    regs->roi1Bottom = pCodeParams->roi1Area.bottom;
    regs->roi1Right = pCodeParams->roi1Area.right;
    pEncInst->roi1Enable = 1;
  }
  else
  {
    regs->roi1Top = regs->roi1Left = regs->roi1Bottom =
                                       regs->roi1Right = 255;
    pEncInst->roi1Enable = 0;
  }
  if (pCodeParams->roi2Area.enable)
  {
    regs->roi2Top = pCodeParams->roi2Area.top;
    regs->roi2Left = pCodeParams->roi2Area.left;
    regs->roi2Bottom = pCodeParams->roi2Area.bottom;
    regs->roi2Right = pCodeParams->roi2Area.right;
    pEncInst->roi2Enable = 1;
  }
  else
  {
    regs->roi2Top = regs->roi2Left = regs->roi2Bottom =
                                       regs->roi2Right = 255;
    pEncInst->roi2Enable = 0;
  }
  regs->roi1DeltaQp = -pCodeParams->roi1DeltaQp;
  regs->roi2DeltaQp = -pCodeParams->roi2DeltaQp;
  regs->roiUpdate   = 1;    /* ROI has changed from previous frame. */

  pEncInst->roiMapEnable = 0;
#ifndef CTBRC_STRENGTH
  if (pCodeParams->roiMapDeltaQpEnable == 1)
  {
    pEncInst->roiMapEnable = 1;
    regs->rcRoiEnable = RCROIMODE_ROIMAP_ENABLE;
  }
  else if (pCodeParams->roi1Area.enable || pCodeParams->roi2Area.enable)
    regs->rcRoiEnable = RCROIMODE_ROIAREA_ENABLE;
  else
    regs->rcRoiEnable = RCROIMODE_RC_ROIMAP_ROIAREA_DIABLE;
#else
    /*rcRoiEnable : bit2: rc control bit, bit1: roi map control bit, bit 0: roi area control bit*/
    if(pCodeParams->roiMapDeltaQpEnable == 1 &&( pCodeParams->roi1Area.enable ==0 && pCodeParams->roi2Area.enable ==0))
    {
      pEncInst->roiMapEnable = 1;
      regs->rcRoiEnable = 0x02;
    }
    else if((pCodeParams->roiMapDeltaQpEnable == 0) &&( pCodeParams->roi1Area.enable !=0 || pCodeParams->roi2Area.enable !=0))
    {
      regs->rcRoiEnable = 0x01;
    }
    else if((pCodeParams->roiMapDeltaQpEnable != 0) && ( pCodeParams->roi1Area.enable !=0 || pCodeParams->roi2Area.enable !=0))
    {
      pEncInst->roiMapEnable = 1;
      regs->rcRoiEnable = 0x03;
    }
    else
      regs->rcRoiEnable = 0x00;
#endif
  pEncInst->rateControl.sei.insertRecoveryPointMessage = ENCHW_NO;
  pEncInst->rateControl.sei.recoveryFrameCnt = pCodeParams->gdrDuration;
  if (pEncInst->encStatus < HEVCENCSTAT_START_FRAME)
  {
    pEncInst->gdrFirstIntraFrame = (1 + pEncInst->interlaced);
  }
  pEncInst->gdrEnabled = (pCodeParams->gdrDuration > 0);
  pEncInst->gdrDuration = pCodeParams->gdrDuration;
  if (pEncInst->gdrEnabled == 1)
  {
    pEncInst->gdrStart = 0;
    pEncInst->gdrCount = 0;



    pEncInst->gdrAverageMBRows = (pEncInst->ctbPerCol - 1) / pEncInst->gdrDuration;
    pEncInst->gdrMBLeft = pEncInst->ctbPerCol - 1 - pEncInst->gdrAverageMBRows * pEncInst->gdrDuration;



    if (pEncInst->gdrAverageMBRows == 0)
    {
      pEncInst->rateControl.sei.recoveryFrameCnt = pEncInst->gdrMBLeft;
      pEncInst->gdrDuration = pEncInst->gdrMBLeft;
    }
  }


  APITRACE("HEVCEncSetCodingCtrl: OK");
  return HEVCENC_OK;
}

/*-----------------------------------------------------------------------------
check reference lists modification
-------------------------------------------------------------------------------*/
i32 check_ref_lists_modification(const HEVCEncIn *pEncIn)
{
  int i;
  bool lowdelayB = false;
  HEVCGopConfig *gopCfg = (HEVCGopConfig *)(&(pEncIn->gopConfig));

  for (i = 0; i < gopCfg->size; i ++)
  {
    HEVCGopPicConfig *cfg = &(gopCfg->pGopPicCfg[i]);
    if (cfg->codingType == HEVCENC_BIDIR_PREDICTED_FRAME)
    {
      u32 iRefPic;
      lowdelayB = true;
      for (iRefPic = 0; iRefPic < cfg->numRefPics; iRefPic ++)
      {
        if (cfg->refPics[iRefPic].used_by_cur && cfg->refPics[iRefPic].ref_pic > 0)
          lowdelayB = false;
      }
      if (lowdelayB)
        break;
    }
  }
  return lowdelayB ? 1 : 0;
}

/*------------------------------------------------------------------------------
  set_parameter
------------------------------------------------------------------------------*/
i32 set_parameter(struct hevc_instance *hevc_instance, const HEVCEncIn *pEncIn,
                  struct vps *v, struct sps *s, struct pps *p)
{
  struct container *c;
  struct hevc_buffer source;
  i32 tmp;

  if (!(c = get_container(hevc_instance))) goto out;
  if (!v || !s || !p) goto out;

  /* Initialize stream buffers */
  if (init_buffer(&source, hevc_instance)) goto out;
  if (get_buffer(&v->ps.b, &source, PRM_SET_BUFF_SIZE, true)) goto out;
#ifdef TEST_DATA
  Enc_open_stream_trace(&v->ps.b);
#endif

  if (get_buffer(&s->ps.b, &source, PRM_SET_BUFF_SIZE, true)) goto out;
#ifdef TEST_DATA
  Enc_open_stream_trace(&s->ps.b);
#endif

  if (get_buffer(&p->ps.b, &source, PRM_SET_BUFF_SIZE, true)) goto out;
#ifdef TEST_DATA
  Enc_open_stream_trace(&p->ps.b);
#endif


  /* Coding unit sizes */
  if (log2i(hevc_instance->min_cu_size, &tmp)) goto out;
  if (check_range(tmp, 3, 3)) goto out;
  s->log2_min_cb_size = tmp;

  if (log2i(hevc_instance->max_cu_size, &tmp)) goto out;
  if (check_range(tmp, 6, 6)) goto out;
  s->log2_diff_cb_size = tmp - s->log2_min_cb_size;
  p->log2_ctb_size = s->log2_min_cb_size + s->log2_diff_cb_size;
  p->ctb_size = 1 << p->log2_ctb_size;
  ASSERT(p->ctb_size == hevc_instance->max_cu_size);

  /* Transform size */
  if (log2i(hevc_instance->min_tr_size, &tmp)) goto out;
  if (check_range(tmp, 2, 2)) goto out;
  s->log2_min_tr_size = tmp;

  if (log2i(hevc_instance->max_tr_size, &tmp)) goto out;
  if (check_range(tmp, 4, 4))
    goto out;
  s->log2_diff_tr_size = tmp - s->log2_min_tr_size;
  p->log2_max_tr_size = s->log2_min_tr_size + s->log2_diff_tr_size;
  ASSERT(1 << p->log2_max_tr_size == hevc_instance->max_tr_size);

  /* Max transform hierarchy depth intra/inter */
  tmp = p->log2_ctb_size - s->log2_min_tr_size;
  if (check_range(hevc_instance->tr_depth_intra, 0, tmp)) goto out;
  s->max_tr_hierarchy_depth_intra = hevc_instance->tr_depth_intra;

  if (check_range(hevc_instance->tr_depth_inter, 0, tmp)) goto out;
  s->max_tr_hierarchy_depth_inter = hevc_instance->tr_depth_inter;

  s->scaling_list_enable_flag = hevc_instance->enableScalingList;

  /* Parameter set id */
  if (check_range(hevc_instance->vps_id, 0, 15)) goto out;
  v->ps.id = hevc_instance->vps_id;

  if (check_range(hevc_instance->sps_id, 0, 31)) goto out;
  s->ps.id = hevc_instance->sps_id;
  s->vps_id = v->ps.id;

  if (check_range(hevc_instance->pps_id, 0, 255)) goto out;
  p->ps.id = hevc_instance->pps_id;
  p->sps_id = s->ps.id;

  /* TODO image cropping parameters */
  if (!(hevc_instance->width > 0)) goto out;
  if (!(hevc_instance->height > 0)) goto out;
  s->width = hevc_instance->width;
  s->height = hevc_instance->height;

  /* strong_intra_smoothing_enabled_flag */
  //if (check_range(s->strong_intra_smoothing_enabled_flag, 0, 1)) goto out;
  s->strong_intra_smoothing_enabled_flag = hevc_instance->strong_intra_smoothing_enabled_flag;
  ASSERT((s->strong_intra_smoothing_enabled_flag == 0) || (s->strong_intra_smoothing_enabled_flag == 1));

  /* Default quantization parameter */
  if (check_range(hevc_instance->rateControl.qpHdr >> QP_FRACTIONAL_BITS, 0, 51)) goto out;
  p->init_qp = hevc_instance->rateControl.qpHdr >> QP_FRACTIONAL_BITS;

  /* Picture level rate control TODO... */
  if (check_range(hevc_instance->rateControl.picRc, 0, 1)) goto out;

  p->cu_qp_delta_enabled_flag = ((hevc_instance->asic.regs.rcRoiEnable != 0) || hevc_instance->gdrDuration != 0) ? 1 : 0;
#ifdef INTERNAL_TEST
  p->cu_qp_delta_enabled_flag = (hevc_instance->testId == TID_ROI) ? 1 : p->cu_qp_delta_enabled_flag;
#endif

  //CTB_RC
  //printf("*******hevc_instance->rateControl.ctbRc=%d**********\n",hevc_instance->rateControl.ctbRc);
  if (p->cu_qp_delta_enabled_flag == 0 && hevc_instance->rateControl.ctbRc)
  {
    p->cu_qp_delta_enabled_flag = 1;
  }

  if (log2i(hevc_instance->min_qp_size, &tmp)) goto out;
#if 1
  /* Get the qp size from command line. */
  p->diff_cu_qp_delta_depth = p->log2_ctb_size - tmp;
#else
  /* Make qp size the same as ctu now. FIXME: when qp size is different. */
  p->diff_cu_qp_delta_depth = 0;
#endif

  if (check_range(tmp, s->log2_min_cb_size, p->log2_ctb_size)) goto out;

  /* Init the rest. TODO: tiles are not supported yet. How to get
   * slice/tiles from test_bench.c ? */
  if (init_parameter_set(s, p)) goto out;
  p->deblocking_filter_disabled_flag = hevc_instance->disableDeblocking;
  p->tc_offset = hevc_instance->tc_Offset * 2;
  p->beta_offset = hevc_instance->beta_Offset * 2;
  p->deblocking_filter_override_enabled_flag = hevc_instance->enableDeblockOverride;


  p->cb_qp_offset = hevc_instance->chromaQpOffset;
  p->cr_qp_offset = hevc_instance->chromaQpOffset;

  s->sao_enabled_flag = hevc_instance->enableSao;

  s->frameCropLeftOffset = hevc_instance->preProcess.frameCropLeftOffset;
  s->frameCropRightOffset = hevc_instance->preProcess.frameCropRightOffset;
  s->frameCropTopOffset = hevc_instance->preProcess.frameCropTopOffset;
  s->frameCropBottomOffset = hevc_instance->preProcess.frameCropBottomOffset;

  v->hevcStrmMode = hevc_instance->asic.regs.hevcStrmMode;
  s->hevcStrmMode = hevc_instance->asic.regs.hevcStrmMode;
  p->hevcStrmMode = hevc_instance->asic.regs.hevcStrmMode;

  v->general_level_idc = hevc_instance->level;
  s->general_level_idc = hevc_instance->level;


  v->general_profile_idc = hevc_instance->profile;
  s->general_profile_idc = hevc_instance->profile;

  hevc_instance->asic.regs.outputBitWidthLuma = s->bit_depth_luma_minus8;
  hevc_instance->asic.regs.outputBitWidthChroma = s->bit_depth_chroma_minus8;

  p->lists_modification_present_flag = check_ref_lists_modification(pEncIn);
  return HEVCENC_OK;

out:
  free_parameter_set((struct ps *)v);
  free_parameter_set((struct ps *)s);
  free_parameter_set((struct ps *)p);

  return HEVCENC_ERROR;
}


HEVCEncRet HEVCEncGetCodingCtrl(HEVCEncInst inst,
                                HEVCEncCodingCtrl *pCodeParams)
{
  struct hevc_instance  *pEncInst = (struct hevc_instance *) inst;
  regValues_s *regs = &pEncInst->asic.regs;

  APITRACE("HEVCEncGetCodingCtrl#");

  /* Check for illegal inputs */
  if ((pEncInst == NULL) || (pCodeParams == NULL))
  {
    APITRACE("HEVCEncGetCodingCtrl: ERROR Null argument");
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (pEncInst->inst != pEncInst)
  {
    APITRACE("HEVCEncGetCodingCtrl: ERROR Invalid instance");
    return HEVCENC_INSTANCE_ERROR;
  }

  pCodeParams->seiMessages =
    (pEncInst->rateControl.sei.enabled == ENCHW_YES) ? 1 : 0;

  pCodeParams->disableDeblockingFilter = pEncInst->disableDeblocking;
  pCodeParams->fieldOrder = pEncInst->fieldOrder;

  pCodeParams->tc_Offset = pEncInst->tc_Offset;
  pCodeParams->beta_Offset = pEncInst->beta_Offset;
  pCodeParams->enableSao = pEncInst->enableSao;
  pCodeParams->enableScalingList = pEncInst->enableScalingList;
  pCodeParams->videoFullRange =
    (pEncInst->sps->vui.videoFullRange == ENCHW_YES) ? 1 : 0;
  pCodeParams->sampleAspectRatioWidth = pEncInst->sarWidth;
  pCodeParams->sampleAspectRatioHeight = pEncInst->sarHeight;
  pCodeParams->sliceSize = regs->sliceSize;

  pCodeParams->cabacInitFlag = regs->cabac_init_flag;

  pCodeParams->cirStart = regs->cirStart;
  pCodeParams->cirInterval = regs->cirInterval;
  pCodeParams->intraArea.top    = regs->intraAreaTop;
  pCodeParams->intraArea.left   = regs->intraAreaLeft;
  pCodeParams->intraArea.bottom = regs->intraAreaBottom;
  pCodeParams->intraArea.right  = regs->intraAreaRight;
  if ((pCodeParams->intraArea.top <= pCodeParams->intraArea.bottom &&
       pCodeParams->intraArea.bottom < (u32)pEncInst->ctbPerCol &&
       pCodeParams->intraArea.left <= pCodeParams->intraArea.right &&
       pCodeParams->intraArea.right < (u32)pEncInst->ctbPerRow))
  {
    pCodeParams->intraArea.enable = 1;
  }
  else
  {
    pCodeParams->intraArea.enable = 0;
  }

  pCodeParams->roi1Area.top    = regs->roi1Top;
  pCodeParams->roi1Area.left   = regs->roi1Left;
  pCodeParams->roi1Area.bottom = regs->roi1Bottom;
  pCodeParams->roi1Area.right  = regs->roi1Right;
  if (pCodeParams->roi1Area.top <= pCodeParams->roi1Area.bottom &&
      pCodeParams->roi1Area.bottom < (u32)pEncInst->ctbPerCol &&
      pCodeParams->roi1Area.left <= pCodeParams->roi1Area.right &&
      pCodeParams->roi1Area.right < (u32)pEncInst->ctbPerRow)
  {
    pCodeParams->roi1Area.enable = 1;
  }
  else
  {
    pCodeParams->roi1Area.enable = 0;
  }

  pCodeParams->roi2Area.top    = regs->roi2Top;
  pCodeParams->roi2Area.left   = regs->roi2Left;
  pCodeParams->roi2Area.bottom = regs->roi2Bottom;
  pCodeParams->roi2Area.right  = regs->roi2Right;
  if ((pCodeParams->roi2Area.top <= pCodeParams->roi2Area.bottom &&
       pCodeParams->roi2Area.bottom < (u32)pEncInst->ctbPerCol &&
       pCodeParams->roi2Area.left <= pCodeParams->roi2Area.right &&
       pCodeParams->roi2Area.right < (u32)pEncInst->ctbPerRow))
  {
    pCodeParams->roi2Area.enable = 1;
  }
  else
  {
    pCodeParams->roi2Area.enable = 0;
  }


  pCodeParams->roi1DeltaQp = -regs->roi1DeltaQp;
  pCodeParams->roi2DeltaQp = -regs->roi2DeltaQp;


  pCodeParams->enableDeblockOverride = pEncInst->enableDeblockOverride;

  pCodeParams->deblockOverride = regs->slice_deblocking_filter_override_flag;

  pCodeParams->chroma_qp_offset = pEncInst->chromaQpOffset;

  pCodeParams->roiMapDeltaQpEnable = pEncInst->roiMapEnable;
  pCodeParams->roiMapDeltaQpBlockUnit = regs->diffCuQpDeltaDepth;

  pCodeParams->gdrDuration = pEncInst->gdrEnabled ? pEncInst->gdrDuration : 0;

  HEVCEncDnfGetParameters(pEncInst, pCodeParams);

  APITRACE("HEVCEncGetCodingCtrl: OK");
  return HEVCENC_OK;
}

/*------------------------------------------------------------------------------

    Function name : HEVCEncSetRateCtrl
    Description   : Sets rate control parameters

    Return type   : HEVCEncRet
    Argument      : inst - the instance in use
                    pRateCtrl - user provided parameters
------------------------------------------------------------------------------*/
HEVCEncRet HEVCEncSetRateCtrl(HEVCEncInst inst, const HEVCEncRateCtrl *pRateCtrl)
{
  struct hevc_instance *hevc_instance = (struct hevc_instance *)inst;
  hevcRateControl_s *rc;
  u32 i, tmp;
  i32 prevBitrate;
  regValues_s *regs = &hevc_instance->asic.regs;

  APITRACE("HEVCEncSetRateCtrl#");
  APITRACEPARAM("pictureRc", pRateCtrl->pictureRc);
  APITRACEPARAM("pictureSkip", pRateCtrl->pictureSkip);
  APITRACEPARAM("qpHdr", pRateCtrl->qpHdr);
  APITRACEPARAM("qpMin", pRateCtrl->qpMin);
  APITRACEPARAM("qpMax", pRateCtrl->qpMax);
  APITRACEPARAM("bitPerSecond", pRateCtrl->bitPerSecond);
  APITRACEPARAM("hrd", pRateCtrl->hrd);
  APITRACEPARAM("hrdCpbSize", pRateCtrl->hrdCpbSize);
  APITRACEPARAM("gopLen", pRateCtrl->gopLen);
  APITRACEPARAM("intraQpDelta", pRateCtrl->intraQpDelta);
  APITRACEPARAM("fixedIntraQp", pRateCtrl->fixedIntraQp);
  APITRACEPARAM("bitVarRangeI", pRateCtrl->bitVarRangeI);
  APITRACEPARAM("bitVarRangeP", pRateCtrl->bitVarRangeP);
  APITRACEPARAM("bitVarRangeB", pRateCtrl->bitVarRangeB);
  APITRACEPARAM("ctbRc", pRateCtrl->ctbRc);
  APITRACEPARAM("blockRCSize", pRateCtrl->blockRCSize);

  /* Check for illegal inputs */
  if ((hevc_instance == NULL) || (pRateCtrl == NULL))
  {
    APITRACE("HEVCEncSetRateCtrl: ERROR Null argument");
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (hevc_instance->inst != hevc_instance)
  {
    APITRACE("HEVCEncSetRateCtrl: ERROR Invalid instance");
    return HEVCENC_INSTANCE_ERROR;
  }

  rc = &hevc_instance->rateControl;

  /* after stream was started with HRD ON,
   * it is not allowed to change RC params */
  if (hevc_instance->encStatus == HEVCENCSTAT_START_FRAME && rc->hrd == ENCHW_YES && pRateCtrl->hrd == ENCHW_YES)
  {
    APITRACE("HEVCEncSetRateCtrl: ERROR Stream started with HRD ON. Not allowed to change any parameters");
    return HEVCENC_INVALID_STATUS;
  }

  if ((hevc_instance->encStatus >= HEVCENCSTAT_START_STREAM)&&(hevc_instance->ctbRCEnable != pRateCtrl->ctbRc))
  {
    APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid encoding status to config ctbRc");
    return HEVCENC_INVALID_ARGUMENT;
  }
  if ((regs->asicHwId < 0x48320200 /* H2V1 */) &&
      pRateCtrl->ctbRc == 1)
  {
    APITRACE("HEVCEncSetRateCtrl: ERROR ctb RC not supported");
    return HEVCENC_INVALID_ARGUMENT;
  }
  /* Check for invalid input values */
  if (pRateCtrl->pictureRc > 1 ||
      pRateCtrl->pictureSkip > 1 || pRateCtrl->hrd > 1)
  {
    APITRACE("HEVCEncSetRateCtrl: ERROR Invalid enable/disable value");
    return HEVCENC_INVALID_ARGUMENT;
  }

  if (pRateCtrl->qpHdr > 51 ||
      pRateCtrl->qpMin > 51 ||
      pRateCtrl->qpMax > 51 ||
      pRateCtrl->qpMax < pRateCtrl->qpMin)
  {
    APITRACE("HEVCEncSetRateCtrl: ERROR Invalid QP");
    return HEVCENC_INVALID_ARGUMENT;
  }

  if ((pRateCtrl->qpHdr != -1) &&
      (pRateCtrl->qpHdr < (i32)pRateCtrl->qpMin ||
       pRateCtrl->qpHdr > (i32)pRateCtrl->qpMax))
  {
    APITRACE("HEVCEncSetRateCtrl: ERROR QP out of range");
    return HEVCENC_INVALID_ARGUMENT;
  }
  if ((u32)(pRateCtrl->intraQpDelta + 51) > 102)
  {
    APITRACE("HEVCEncSetRateCtrl: ERROR intraQpDelta out of range");
    return HEVCENC_INVALID_ARGUMENT;
  }

  if (pRateCtrl->fixedIntraQp > 51)
  {
    APITRACE("HEVCEncSetRateCtrl: ERROR fixedIntraQp out of range");
    return HEVCENC_INVALID_ARGUMENT;
  }
  if (pRateCtrl->gopLen < 1 || pRateCtrl->gopLen > 300)
  {
    APITRACE("HEVCEncSetRateCtrl: ERROR Invalid GOP length");
    return HEVCENC_INVALID_ARGUMENT;
  }
  if (pRateCtrl->monitorFrames < 10 || pRateCtrl->monitorFrames > 120)
  {
    APITRACE("HEVCEncSetRateCtrl: ERROR Invalid monitorFrames");
    return HEVCENC_INVALID_ARGUMENT;
  }

  if (pRateCtrl->blockRCSize > 2 || pRateCtrl->ctbRc >1)
  {
    APITRACE("HEVCEncSetRateCtrl: ERROR Invalid blockRCSize");
    return HEVCENC_INVALID_ARGUMENT;
  }

  /* Bitrate affects only when rate control is enabled */
  if ((pRateCtrl->pictureRc ||
       pRateCtrl->pictureSkip || pRateCtrl->hrd) &&
      (pRateCtrl->bitPerSecond < 10000 ||
       pRateCtrl->bitPerSecond > HEVCENC_MAX_BITRATE))
  {
    APITRACE("HEVCEncSetRateCtrl: ERROR Invalid bitPerSecond");
    return HEVCENC_INVALID_ARGUMENT;
  }

  {
    u32 cpbSize = pRateCtrl->hrdCpbSize;
    u32 bps = pRateCtrl->bitPerSecond;
    u32 level = hevc_instance->levelIdx;

    /* Limit maximum bitrate based on resolution and frame rate */
    /* Saturates really high settings */
    /* bits per unpacked frame */
    tmp = 3 * 8 * hevc_instance->ctbPerFrame * hevc_instance->max_cu_size * hevc_instance->max_cu_size / 2;
    /* bits per second */
    tmp = HevcCalculate(tmp, rc->outRateNum, rc->outRateDenom);
    if (bps > (tmp * 5 / 3))
      bps = tmp * 5 / 3;

    if (cpbSize == 0)
      cpbSize = HEVCMaxCPBS[level];
    else if (cpbSize == (u32)(-1))
      cpbSize = bps;

    /* Limit minimum CPB size based on average bits per frame */
    tmp = HevcCalculate(bps, rc->outRateDenom, rc->outRateNum);
    cpbSize = MAX(cpbSize, tmp);

    /* cpbSize must be rounded so it is exactly the size written in stream */
    i = 0;
    tmp = cpbSize;
    while (4095 < (tmp >> (4 + i++)));

    cpbSize = (tmp >> (4 + i)) << (4 + i);

    /* if HRD is ON we have to obay all its limits */
    if (pRateCtrl->hrd != 0)
    {
      if (cpbSize > HEVCMaxCPBS[level])
      {
        APITRACE("HEVCEncSetRateCtrl: ERROR. HRD is ON. hrdCpbSize higher than maximum allowed for stream level");
        return HEVCENC_INVALID_ARGUMENT;
      }

      if (bps > HEVCMaxBR[level])
      {
        APITRACE("HEVCEncSetRateCtrl: ERROR. HRD is ON. bitPerSecond higher than maximum allowed for stream level");
        return HEVCENC_INVALID_ARGUMENT;
      }
    }

    rc->virtualBuffer.bufferSize = cpbSize;

    /* Set the parameters to rate control */
    if (pRateCtrl->pictureRc != 0)
      rc->picRc = ENCHW_YES;
    else
      rc->picRc = ENCHW_NO;

    //CTB_RC
    if (pRateCtrl->ctbRc != 0)
    {
      rc->ctbRc = ENCHW_YES;
      hevc_instance->ctbRCEnable = 1;
      //rc->picRc = ENCHW_YES;  // disable picRc becauseof vbr bitrate is ng
      hevc_instance->blockRCSize = pRateCtrl->blockRCSize;
#ifdef CTBRC_STRENGTH

      if(hevc_instance->min_qp_size > (64 >> hevc_instance->blockRCSize))
         hevc_instance->min_qp_size = (64 >> hevc_instance->blockRCSize);
#endif
    }
    else
    {
      rc->ctbRc = ENCHW_NO;
      hevc_instance->ctbRCEnable = 0;
    }

    if (pRateCtrl->pictureSkip != 0)
      rc->picSkip = ENCHW_YES;
    else
      rc->picSkip = ENCHW_NO;

    if (pRateCtrl->hrd != 0)
    {
      rc->hrd = ENCHW_YES;
      rc->picRc = ENCHW_YES;
    }
    else
      rc->hrd = ENCHW_NO;

    rc->qpHdr = pRateCtrl->qpHdr << QP_FRACTIONAL_BITS;
    rc->qpMin = pRateCtrl->qpMin << QP_FRACTIONAL_BITS;
    rc->qpMax = pRateCtrl->qpMax << QP_FRACTIONAL_BITS;
    prevBitrate = rc->virtualBuffer.bitRate;
    rc->virtualBuffer.bitRate = bps;
    rc->gopLen = pRateCtrl->gopLen;
    rc->maxPicSizeI = HevcCalculate(bps, rc->outRateDenom, rc->outRateNum) / 100 * (100 + pRateCtrl->bitVarRangeI) ;
    rc->maxPicSizeP = HevcCalculate(bps, rc->outRateDenom, rc->outRateNum) / 100 * (100 + pRateCtrl->bitVarRangeP);
    rc->maxPicSizeB = HevcCalculate(bps, rc->outRateDenom, rc->outRateNum) / 100 * (100 + pRateCtrl->bitVarRangeB) ;

    rc->minPicSizeI = HevcCalculate(bps, rc->outRateDenom, rc->outRateNum) * 100 / (100 + pRateCtrl->bitVarRangeI) ;
    rc->minPicSizeP = HevcCalculate(bps, rc->outRateDenom, rc->outRateNum) * 100 / (100 + pRateCtrl->bitVarRangeP) ;
    rc->minPicSizeB = HevcCalculate(bps, rc->outRateDenom, rc->outRateNum) * 100 / (100 + pRateCtrl->bitVarRangeB) ;
    rc->tolMovingBitRate = pRateCtrl->tolMovingBitRate;
    rc->f_tolMovingBitRate = (float)rc->tolMovingBitRate;
    rc->monitorFrames = pRateCtrl->monitorFrames;
  }

  rc->intraQpDelta = pRateCtrl->intraQpDelta << QP_FRACTIONAL_BITS;
  rc->fixedIntraQp = pRateCtrl->fixedIntraQp << QP_FRACTIONAL_BITS;
  rc->frameQpDelta = 0 << QP_FRACTIONAL_BITS ;


  /* New parameters checked already so ignore return value.
  * Reset RC bit counters when changing bitrate. */
  (void) HevcInitRc(rc, (hevc_instance->encStatus == HEVCENCSTAT_INIT) ||
                    (rc->virtualBuffer.bitRate != prevBitrate) || rc->hrd ||pRateCtrl->u32NewStream);
  APITRACE("HEVCEncSetRateCtrl: OK");
  return HEVCENC_OK;
}

/*------------------------------------------------------------------------------

    Function name : HEVCEncGetRateCtrl
    Description   : Return current rate control parameters

    Return type   : HEVCEncRet
    Argument      : inst - the instance in use
                    pRateCtrl - place where parameters are returned
------------------------------------------------------------------------------*/
HEVCEncRet HEVCEncGetRateCtrl(HEVCEncInst inst, HEVCEncRateCtrl *pRateCtrl)
{
  struct hevc_instance *hevc_instance = (struct hevc_instance *)inst;
  hevcRateControl_s *rc;

  APITRACE("HEVCEncGetRateCtrl#");

  /* Check for illegal inputs */
  if ((hevc_instance == NULL) || (pRateCtrl == NULL))
  {
    APITRACE("HEVCEncGetRateCtrl: ERROR Null argument");
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (hevc_instance->inst != hevc_instance)
  {
    APITRACE("HEVCEncGetRateCtrl: ERROR Invalid instance");
    return HEVCENC_INSTANCE_ERROR;
  }

  /* Get the values */
  rc = &hevc_instance->rateControl;

  pRateCtrl->pictureRc = rc->picRc == ENCHW_NO ? 0 : 1;
  //CTB_RC
  pRateCtrl->ctbRc = rc->ctbRc == ENCHW_NO ? 0 : 1;
  pRateCtrl->pictureSkip = rc->picSkip == ENCHW_NO ? 0 : 1;
  pRateCtrl->qpHdr = rc->qpHdr >> QP_FRACTIONAL_BITS;
  pRateCtrl->qpMin = rc->qpMin >> QP_FRACTIONAL_BITS;
  pRateCtrl->qpMax = rc->qpMax >> QP_FRACTIONAL_BITS;
  pRateCtrl->bitPerSecond = rc->virtualBuffer.bitRate;
  if (rc->virtualBuffer.bitPerPic)
  {
    pRateCtrl->bitVarRangeI = rc->maxPicSizeI * 100 / rc->virtualBuffer.bitPerPic - 100;
    pRateCtrl->bitVarRangeP = rc->maxPicSizeP * 100 / rc->virtualBuffer.bitPerPic - 100;
    pRateCtrl->bitVarRangeB = rc->maxPicSizeB * 100 / rc->virtualBuffer.bitPerPic - 100;
  }
  else
  {
    pRateCtrl->bitVarRangeI = 2000;
    pRateCtrl->bitVarRangeP = 2000;
    pRateCtrl->bitVarRangeB = 2000;
  }
  pRateCtrl->hrd = rc->hrd == ENCHW_NO ? 0 : 1;
  pRateCtrl->gopLen = rc->gopLen;

  pRateCtrl->hrdCpbSize = (u32) rc->virtualBuffer.bufferSize;
  pRateCtrl->intraQpDelta = rc->intraQpDelta >> QP_FRACTIONAL_BITS;
  pRateCtrl->fixedIntraQp = rc->fixedIntraQp >> QP_FRACTIONAL_BITS;
  pRateCtrl->monitorFrames = rc->monitorFrames;
  pRateCtrl->tolMovingBitRate = rc->tolMovingBitRate;

  pRateCtrl->blockRCSize = hevc_instance->blockRCSize ;

  APITRACE("HEVCEncGetRateCtrl: OK");
  return HEVCENC_OK;
}


/*------------------------------------------------------------------------------
    Function name   : HEVCEncSetPreProcessing
    Description     : Sets the preprocessing parameters
    Return type     : HEVCEncRet
    Argument        : inst - encoder instance in use
    Argument        : pPreProcCfg - user provided parameters
------------------------------------------------------------------------------*/
HEVCEncRet HEVCEncSetPreProcessing(HEVCEncInst inst, const HEVCEncPreProcessingCfg *pPreProcCfg)
{
  struct hevc_instance *hevc_instance = (struct hevc_instance *)inst;
  preProcess_s pp_tmp;
  asicData_s *asic = &hevc_instance->asic;


  APITRACE("HEVCEncSetPreProcessing#");
  APITRACEPARAM("origWidth", pPreProcCfg->origWidth);
  APITRACEPARAM("origHeight", pPreProcCfg->origHeight);
  APITRACEPARAM("xOffset", pPreProcCfg->xOffset);
  APITRACEPARAM("yOffset", pPreProcCfg->yOffset);
  APITRACEPARAM("inputType", pPreProcCfg->inputType);
  APITRACEPARAM("rotation", pPreProcCfg->rotation);
  //APITRACEPARAM("videoStabilization", pPreProcCfg->videoStabilization);
  APITRACEPARAM("colorConversion.type", pPreProcCfg->colorConversion.type);
  APITRACEPARAM("colorConversion.coeffA", pPreProcCfg->colorConversion.coeffA);
  APITRACEPARAM("colorConversion.coeffB", pPreProcCfg->colorConversion.coeffB);
  APITRACEPARAM("colorConversion.coeffC", pPreProcCfg->colorConversion.coeffC);
  APITRACEPARAM("colorConversion.coeffE", pPreProcCfg->colorConversion.coeffE);
  APITRACEPARAM("colorConversion.coeffF", pPreProcCfg->colorConversion.coeffF);
  APITRACEPARAM("scaledWidth", pPreProcCfg->scaledWidth);
  APITRACEPARAM("scaledHeight", pPreProcCfg->scaledHeight);
  APITRACEPARAM("scaledOutput", pPreProcCfg->scaledOutput);
  APITRACEPARAM("virtualAddressScaledBuff", pPreProcCfg->virtualAddressScaledBuff);
  APITRACEPARAM("busAddressScaledBuff", pPreProcCfg->busAddressScaledBuff);
  APITRACEPARAM("sizeScaledBuff", pPreProcCfg->sizeScaledBuff);

  /* Check for illegal inputs */
  if ((inst == NULL) || (pPreProcCfg == NULL))
  {
    APITRACE("HEVCEncSetPreProcessing: ERROR Null argument");
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (hevc_instance->inst != hevc_instance)
  {
    APITRACE("HEVCEncSetPreProcessing: ERROR Invalid instance");
    return HEVCENC_INSTANCE_ERROR;
  }

  /* check HW limitations */
  {
   EWLHwConfig_t cfg = EWLReadAsicConfig();// encoded with RGBA8888 is OK
   if (cfg.rgbEnabled == EWL_HW_CONFIG_NOT_SUPPORTED &&
        pPreProcCfg->inputType >= HEVCENC_RGB565 &&
        pPreProcCfg->inputType <= HEVCENC_BGR101010)
    {
      APITRACE("HEVCEncSetPreProcessing: ERROR RGB input not supported");
      return HEVCENC_INVALID_ARGUMENT;
    }
   // if (cfg.scalingEnabled == EWL_HW_CONFIG_NOT_SUPPORTED &&  //add by bright
    if (
        pPreProcCfg->scaledOutput != 0)
    {
      APITRACE("HEVCEncSetPreProcessing: WARNING Scaling not supported, disabling output");
    }
  }

  if (pPreProcCfg->origWidth > HEVCENC_MAX_PP_INPUT_WIDTH ||
      pPreProcCfg->origHeight > HEVCENC_MAX_PP_INPUT_HEIGHT)
  {
    APITRACE("HEVCEncSetPreProcessing: ERROR Too big input image");
    return HEVCENC_INVALID_ARGUMENT;
  }
#if 1
  /* Scaled image width limits, multiple of 4 (YUYV) and smaller than input */
  if ((pPreProcCfg->scaledWidth > (u32)hevc_instance->width) ||
      (pPreProcCfg->scaledWidth & 0x1) != 0)
  {
    APITRACE("HEVCEncSetPreProcessing: Invalid scaledWidth");
    return ENCHW_NOK;
  }

  if (((i32)pPreProcCfg->scaledHeight > hevc_instance->height) ||
      (pPreProcCfg->scaledHeight & 0x1) != 0)
  {
    APITRACE("HEVCEncSetPreProcessing: Invalid scaledHeight");
    return ENCHW_NOK;
  }
#endif
  if (pPreProcCfg->inputType > HEVCENC_YUV420_10BIT_PACKED_Y0L2)
  {
    APITRACE("HEVCEncSetPreProcessing: ERROR Invalid YUV type");
    return HEVCENC_INVALID_ARGUMENT;
  }
  if ((pPreProcCfg->inputType == HEVCENC_YUV420_PLANAR_10BIT_I010
  ||pPreProcCfg->inputType == HEVCENC_YUV420_PLANAR_10BIT_P010
  ||pPreProcCfg->inputType == HEVCENC_YUV420_PLANAR_10BIT_PACKED_PLANAR
  ||pPreProcCfg->inputType == HEVCENC_YUV420_10BIT_PACKED_Y0L2)&&(asic->regs.asicHwId<0x48320401))
  {
    APITRACE("HEVCEncSetPreProcessing: ERROR Invalid YUV type, HW not support I010 format.");
    return HEVCENC_INVALID_ARGUMENT;
  }

  if (pPreProcCfg->rotation > HEVCENC_ROTATE_180R)
  {
    APITRACE("HEVCEncSetPreProcessing: ERROR Invalid rotation");
    return HEVCENC_INVALID_ARGUMENT;
  }

  pp_tmp = hevc_instance->preProcess;

  {
    pp_tmp.horOffsetSrc = pPreProcCfg->xOffset  >> 1 << 1;
    pp_tmp.verOffsetSrc = pPreProcCfg->yOffset >> 1 << 1;
  }

  /* Encoded frame resolution as set in Init() */
  //pp_tmp = pEncInst->preProcess;    //here is a bug? if enabling this line.

  pp_tmp.lumWidthSrc  = pPreProcCfg->origWidth;
  pp_tmp.lumHeightSrc = pPreProcCfg->origHeight;
  pp_tmp.rotation     = pPreProcCfg->rotation;
  pp_tmp.inputFormat  = pPreProcCfg->inputType;
  pp_tmp.videoStab    =  0;
  pp_tmp.scaledWidth  = pPreProcCfg->scaledWidth;
  pp_tmp.scaledHeight = pPreProcCfg->scaledHeight;
  asic->scaledImage.busAddress = pPreProcCfg->busAddressScaledBuff;
  asic->scaledImage.virtualAddress = pPreProcCfg->virtualAddressScaledBuff;
  asic->scaledImage.size = pPreProcCfg->sizeScaledBuff;
  asic->regs.scaledLumBase = asic->scaledImage.busAddress;

  pp_tmp.scaledOutput = (pPreProcCfg->scaledOutput) ? 1 : 0;
  if (pPreProcCfg->scaledWidth *pPreProcCfg->scaledHeight == 0)
    pp_tmp.scaledOutput = 0;
  /* Check for invalid values */
  if (EncPreProcessCheck(&pp_tmp) != ENCHW_OK)
  {
    APITRACE("HEVCEncSetPreProcessing: ERROR Invalid cropping values");
    return HEVCENC_INVALID_ARGUMENT;
  }
#if 1
  pp_tmp.frameCropping = ENCHW_NO;
  pp_tmp.frameCropLeftOffset = 0;
  pp_tmp.frameCropRightOffset = 0;
  pp_tmp.frameCropTopOffset = 0;
  pp_tmp.frameCropBottomOffset = 0;
  /* Set cropping parameters if required */
  if (hevc_instance->preProcess.lumWidth % 8 || hevc_instance->preProcess.lumHeight % 8)
  {
    u32 fillRight = (hevc_instance->preProcess.lumWidth + 7) / 8 * 8 -
                    hevc_instance->preProcess.lumWidth;
    u32 fillBottom = (hevc_instance->preProcess.lumHeight + 7) / 8 * 8 -
                     hevc_instance->preProcess.lumHeight;
    pp_tmp.frameCropping = ENCHW_YES;

    if (pPreProcCfg->rotation == HEVCENC_ROTATE_0)     /* No rotation */
    {
      pp_tmp.frameCropRightOffset = fillRight / 2;
      pp_tmp.frameCropBottomOffset = fillBottom / 2;
    }
    else if (pPreProcCfg->rotation == HEVCENC_ROTATE_90R)        /* Rotate right */
    {
      pp_tmp.frameCropLeftOffset = fillRight / 2;
      pp_tmp.frameCropBottomOffset = fillBottom / 2;
    }
    else if (pPreProcCfg->rotation == HEVCENC_ROTATE_90L)        /* Rotate left */
    {
      pp_tmp.frameCropRightOffset = fillRight / 2;
      pp_tmp.frameCropTopOffset = fillBottom / 2;
    }
    else if (pPreProcCfg->rotation == HEVCENC_ROTATE_180R)        /* Rotate 180 degree left */
    {
      pp_tmp.frameCropLeftOffset = fillRight / 2;
      pp_tmp.frameCropTopOffset = fillBottom / 2;
    }
  }
#endif
  pp_tmp.colorConversionType = pPreProcCfg->colorConversion.type;
  pp_tmp.colorConversionCoeffA = pPreProcCfg->colorConversion.coeffA;
  pp_tmp.colorConversionCoeffB = pPreProcCfg->colorConversion.coeffB;
  pp_tmp.colorConversionCoeffC = pPreProcCfg->colorConversion.coeffC;
  pp_tmp.colorConversionCoeffE = pPreProcCfg->colorConversion.coeffE;
  pp_tmp.colorConversionCoeffF = pPreProcCfg->colorConversion.coeffF;
  EncSetColorConversion(&pp_tmp, &hevc_instance->asic);

  hevc_instance->preProcess = pp_tmp;
  APITRACE("HEVCEncSetPreProcessing: OK");

  return HEVCENC_OK;
}

/*------------------------------------------------------------------------------
    Function name   : HEVCEncGetPreProcessing
    Description     : Returns current preprocessing parameters
    Return type     : HEVCEncRet
    Argument        : inst - encoder instance
    Argument        : pPreProcCfg - place where the parameters are returned
------------------------------------------------------------------------------*/
HEVCEncRet HEVCEncGetPreProcessing(HEVCEncInst inst,
                                   HEVCEncPreProcessingCfg *pPreProcCfg)
{
  struct hevc_instance *hevc_instance = (struct hevc_instance *)inst;
  preProcess_s *pPP;
  APITRACE("HEVCEncGetPreProcessing#");
  /* Check for illegal inputs */
  if ((inst == NULL) || (pPreProcCfg == NULL))
  {
    APITRACE("HEVCEncGetPreProcessing: ERROR Null argument");
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (hevc_instance->inst != hevc_instance)
  {
    APITRACE("HEVCEncGetPreProcessing: ERROR Invalid instance");
    return HEVCENC_INSTANCE_ERROR;
  }

  pPP = &hevc_instance->preProcess;

  pPreProcCfg->origHeight = pPP->lumHeightSrc;
  pPreProcCfg->origWidth = pPP->lumWidthSrc;
  pPreProcCfg->xOffset = pPP->horOffsetSrc;
  pPreProcCfg->yOffset = pPP->verOffsetSrc;

  pPreProcCfg->rotation = (HEVCEncPictureRotation) pPP->rotation;
  pPreProcCfg->inputType = (HEVCEncPictureType) pPP->inputFormat;

  pPreProcCfg->scaledOutput       = pPP->scaledOutput;
  pPreProcCfg->scaledWidth       = pPP->scaledWidth;
  pPreProcCfg->scaledHeight       = pPP->scaledHeight;


  pPreProcCfg->colorConversion.type =
    (HEVCEncColorConversionType) pPP->colorConversionType;
  pPreProcCfg->colorConversion.coeffA = pPP->colorConversionCoeffA;
  pPreProcCfg->colorConversion.coeffB = pPP->colorConversionCoeffB;
  pPreProcCfg->colorConversion.coeffC = pPP->colorConversionCoeffC;
  pPreProcCfg->colorConversion.coeffE = pPP->colorConversionCoeffE;
  pPreProcCfg->colorConversion.coeffF = pPP->colorConversionCoeffF;

  APITRACE("HEVCEncGetPreProcessing: OK");
  return HEVCENC_OK;
}

/*------------------------------------------------------------------------------
    Function name : HEVCEncSetSeiUserData
    Description   : Sets user data SEI messages
    Return type   : HEVCEncRet
    Argument      : inst - the instance in use
                    pUserData - pointer to userData, this is used by the
                                encoder so it must not be released before
                                disabling user data
                    userDataSize - size of userData, minimum size 16,
                                   maximum size HEVCENC_MAX_USER_DATA_SIZE
                                   not valid size disables userData sei messages
------------------------------------------------------------------------------*/
HEVCEncRet HEVCEncSetSeiUserData(HEVCEncInst inst, const u8 *pUserData,
                                 u32 userDataSize)
{
  struct hevc_instance *hevc_instance = (struct hevc_instance *)inst;

  APITRACE("HEVCEncSetSeiUserData#");
  APITRACEPARAM("userDataSize", userDataSize);

  /* Check for illegal inputs */
  if ((hevc_instance == NULL) || (userDataSize != 0 && pUserData == NULL))
  {
    APITRACE("HEVCEncSetSeiUserData: ERROR Null argument");
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (hevc_instance->inst != hevc_instance)
  {
    APITRACE("HEVCEncSetSeiUserData: ERROR Invalid instance");
    return HEVCENC_INSTANCE_ERROR;
  }

  /* Disable user data */
  if ((userDataSize < 16) || (userDataSize > HEVCENC_MAX_USER_DATA_SIZE))
  {
    hevc_instance->rateControl.sei.userDataEnabled = ENCHW_NO;
    hevc_instance->rateControl.sei.pUserData = NULL;
    hevc_instance->rateControl.sei.userDataSize = 0;
  }
  else
  {
    hevc_instance->rateControl.sei.userDataEnabled = ENCHW_YES;
    hevc_instance->rateControl.sei.pUserData = pUserData;
    hevc_instance->rateControl.sei.userDataSize = userDataSize;
  }

  APITRACE("HEVCEncSetSeiUserData: OK");
  return HEVCENC_OK;

}
/*------------------------------------------------------------------------------

    Function name : HEVCAddNaluSize
    Description   : Adds the size of a NAL unit into NAL size output buffer.

    Return type   : void
    Argument      : pEncOut - encoder output structure
    Argument      : naluSizeBytes - size of the NALU in bytes
------------------------------------------------------------------------------*/
void HEVCAddNaluSize(HEVCEncOut *pEncOut, u32 naluSizeBytes)
{
  if (pEncOut->pNaluSizeBuf != NULL)
  {
    pEncOut->pNaluSizeBuf[pEncOut->numNalus++] = naluSizeBytes;
    pEncOut->pNaluSizeBuf[pEncOut->numNalus] = 0;
  }
}

/*------------------------------------------------------------------------------
  parameter_set

  RPS:
  poc[n*2  ] = delta_poc
  poc[n*2+1] = used_by_curr_pic
  poc[n*2  ] = 0, termination
  For example:
  { 0, 0},    No reference pictures
  {-1, 1},    Only one previous pictures
  {-1, 1, -2, 1},   Two previous pictures
  {-1, 1, -2, 0},   Two previous ictures, 2'nd not used
  {-1, 1, -2, 1, 1, 1}, Two previous and one future picture
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Function name : HEVCEncStrmStart
    Description   : Starts a new stream
    Return type   : HEVCEncRet
    Argument      : inst - encoder instance
    Argument      : pEncIn - user provided input parameters
                    pEncOut - place where output info is returned
------------------------------------------------------------------------------*/
HEVCEncRet HEVCEncStrmStart(HEVCEncInst inst, const HEVCEncIn *pEncIn,
                            HEVCEncOut *pEncOut)
{

  struct hevc_instance *hevc_instance = (struct hevc_instance *)inst;
  struct container *c;
  struct vps *v;
  struct sps *s;
  struct pps *p;
  i32 ret = HEVCENC_ERROR;

  APITRACE("HEVCEncStrmStart#");
  APITRACEPARAM_X("busLuma", pEncIn->busLuma);
  APITRACEPARAM_X("busChromaU", pEncIn->busChromaU);
  APITRACEPARAM_X("busChromaV", pEncIn->busChromaV);
  APITRACEPARAM("timeIncrement", pEncIn->timeIncrement);
  APITRACEPARAM_X("pOutBuf", pEncIn->pOutBuf);
  APITRACEPARAM_X("busOutBuf", pEncIn->busOutBuf);
  APITRACEPARAM("outBufSize", pEncIn->outBufSize);
  APITRACEPARAM("codingType", pEncIn->codingType);
  APITRACEPARAM("poc", pEncIn->poc);
  APITRACEPARAM("gopSize", pEncIn->gopSize);
  APITRACEPARAM("gopPicIdx", pEncIn->gopPicIdx);
  APITRACEPARAM_X("roiMapDeltaQpAddr", pEncIn->roiMapDeltaQpAddr);



  /* Check for illegal inputs */
  if ((hevc_instance == NULL) || (pEncIn == NULL) || (pEncOut == NULL))
  {
    APITRACE("HEVCEncStrmStart: ERROR Null argument");
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (hevc_instance->inst != hevc_instance)
  {
    APITRACE("HEVCEncStrmStart: ERROR Invalid instance");
    return HEVCENC_INSTANCE_ERROR;
  }


  /* Check status */
  if ((hevc_instance->encStatus != HEVCENCSTAT_INIT) &&
      (hevc_instance->encStatus != HEVCENCSTAT_START_FRAME))
  {
    APITRACE("HEVCEncStrmStart: ERROR Invalid status");
    return HEVCENC_INVALID_STATUS;
  }

  /* Check for invalid input values */
  if ((pEncIn->pOutBuf == NULL) ||
      (pEncIn->outBufSize < HEVCENCSTRMSTART_MIN_BUF))
  {
    APITRACE("HEVCEncStrmStart: ERROR Invalid input. Stream buffer");
    return HEVCENC_INVALID_ARGUMENT;
  }

  /* Check for invalid input values */
  //if ((EWLReadAsicID() <= 0x48320101) && (pEncIn->gopConfig.size > 1))//add by bright
  //if (pEncIn->gopConfig.size > 1)
  //{
  //  APITRACE("HEVCEncStrmStart: ERROR Invalid input. gopConfig");
  //  return HEVCENC_INVALID_ARGUMENT;
  //}
  if (!(c = get_container(hevc_instance))) return HEVCENC_ERROR;


  hevc_instance->vps_id = 0;
  hevc_instance->sps_id = 0;
  hevc_instance->pps_id = 0;

  hevc_instance->stream.stream = (u8 *)pEncIn->pOutBuf;
  hevc_instance->stream.stream_bus = pEncIn->busOutBuf;
  hevc_instance->stream.size = pEncIn->outBufSize;

  pEncOut->pNaluSizeBuf = (u32 *)hevc_instance->asic.sizeTbl.virtualAddress;


  hevc_instance->temp_size = 1024 * 10;
  hevc_instance->temp_buffer = hevc_instance->stream.stream + hevc_instance->stream.size - hevc_instance->temp_size;

  pEncOut->streamSize = 0;
  pEncOut->numNalus = 0;

  /* set RPS */
  if (hevc_ref_pic_sets(hevc_instance, (HEVCGopConfig *)(&(pEncIn->gopConfig))))
  {
    Error(2, ERR, "hevc_ref_pic_sets() fails");
    return NOK;
  }

  v = (struct vps *)get_parameter_set(c, VPS_NUT, 0);
  s = (struct sps *)get_parameter_set(c, SPS_NUT, 0);
  p = (struct pps *)get_parameter_set(c, PPS_NUT, 0);

  /* Initialize parameter sets */
  if (set_parameter(hevc_instance, pEncIn, v, s, p)) goto out;


  /* init VUI */

  HEVCSpsSetVuiAspectRatio(s,
                           hevc_instance->sarWidth,
                           hevc_instance->sarHeight);
  HEVCSpsSetVuiVideoInfo(s,
                         hevc_instance->videoFullRange);
  {
    const hevcVirtualBuffer_s *vb = &hevc_instance->rateControl.virtualBuffer;

    HEVCSpsSetVuiTimigInfo(s,
                           vb->timeScale, vb->unitsInTic);
  }

  /* update VUI */
  if (hevc_instance->rateControl.sei.enabled == ENCHW_YES)
  {
    HEVCSpsSetVuiPictStructPresentFlag(s, 1);
  }

  HEVCSpsSetVui_field_seq_flag(s, hevc_instance->preProcess.interlacedFrame);
  if (hevc_instance->rateControl.hrd == ENCHW_YES)
  {
    HEVCSpsSetVuiHrd(s, 1);

    HEVCSpsSetVuiHrdBitRate(s,
                            hevc_instance->rateControl.virtualBuffer.bitRate);

    HEVCSpsSetVuiHrdCpbSize(s,
                            hevc_instance->rateControl.virtualBuffer.bufferSize);
  }
  /* Init SEI */
  HevcInitSei(&hevc_instance->rateControl.sei, s->hevcStrmMode == ASIC_HEVC_BYTE_STREAM,
              hevc_instance->rateControl.hrd, hevc_instance->rateControl.outRateNum, hevc_instance->rateControl.outRateDenom);

  /* Create parameter set nal units */
  v->ps.b.stream = hevc_instance->stream.stream;
  video_parameter_set(v, inst);
  //APITRACEPARAM("vps size", *v->ps.b.cnt);
  printf("vps size=%d\n", *v->ps.b.cnt);
  pEncOut->streamSize += *v->ps.b.cnt;
  HEVCAddNaluSize(pEncOut, *v->ps.b.cnt);
  hevc_instance->stream.stream += *v->ps.b.cnt;
  s->ps.b.stream = hevc_instance->stream.stream;
  sequence_parameter_set(c, s, inst);
  //APITRACEPARAM("sps size", *s->ps.b.cnt);
  printf("sps size=%d\n", *s->ps.b.cnt);
  pEncOut->streamSize += *s->ps.b.cnt;
  HEVCAddNaluSize(pEncOut, *s->ps.b.cnt);
  hevc_instance->stream.stream += *s->ps.b.cnt;
  p->ps.b.stream = hevc_instance->stream.stream;
  picture_parameter_set(p);
  //APITRACEPARAM("pps size", *p->ps.b.cnt);
  printf("pps size=%d\n", *p->ps.b.cnt);
  pEncOut->streamSize += *p->ps.b.cnt;
  HEVCAddNaluSize(pEncOut, *p->ps.b.cnt);
  /* Status == START_STREAM   Stream started */
  hevc_instance->encStatus = HEVCENCSTAT_START_STREAM;

  if (hevc_instance->rateControl.hrd == ENCHW_YES)
  {
    /* Update HRD Parameters to RC if needed */
    u32 bitrate = HEVCSpsGetVuiHrdBitRate(s);
    u32 cpbsize = HEVCSpsGetVuiHrdCpbSize(s);

    if ((hevc_instance->rateControl.virtualBuffer.bitRate != (i32)bitrate) ||
        (hevc_instance->rateControl.virtualBuffer.bufferSize != (i32)cpbsize))
    {
      hevc_instance->rateControl.virtualBuffer.bitRate = bitrate;
      hevc_instance->rateControl.virtualBuffer.bufferSize = cpbsize;
      (void) HevcInitRc(&hevc_instance->rateControl, 1);
    }
  }

  APITRACE("HEVCEncStrmStart: OK");
  ret = HEVCENC_OK;

out:
#ifdef TEST_DATA
  Enc_close_stream_trace();
#endif

  return ret;

}

/*------------------------------------------------------------------------------
select RPS according to coding type and interlace option
--------------------------------------------------------------------------------*/
static HEVCEncPictureCodingType get_rps_id(struct hevc_instance *hevc_instance, const HEVCEncIn *pEncIn, struct sps *s)
{
  HEVCGopConfig *gopCfg = (HEVCGopConfig *)(&(pEncIn->gopConfig));
  HEVCEncPictureCodingType codingType = pEncIn->codingType;

  if ((hevc_instance->gdrEnabled == 1) && (hevc_instance->encStatus == HEVCENCSTAT_START_FRAME) && (hevc_instance->gdrFirstIntraFrame == 0))
  {

    if (pEncIn->codingType == HEVCENC_INTRA_FRAME)
    {
      hevc_instance->gdrStart++ ;
      codingType = HEVCENC_PREDICTED_FRAME;
    }
  }
  //Intra
  if (codingType == HEVCENC_INTRA_FRAME)
  {
    hevc_instance->rps_id = s->num_short_term_ref_pic_sets;
  }
  //work around for interlaced, only for gopSize = 1
  else if (hevc_instance->interlaced && pEncIn->gopSize == 1)
  {
    hevc_instance->rps_id = hevc_instance->poc == 1 ? s->num_short_term_ref_pic_sets - 1 : s->num_short_term_ref_pic_sets - 2;
    codingType = HEVCENC_PREDICTED_FRAME;
  }
  //normal case
  else
  {
    hevc_instance->rps_id = gopCfg->id;

    //for lowdelayB, force IBBBB... to IPBBB...
    u32 i;
    HEVCGopPicConfig *cfg = &(gopCfg->pGopPicCfg[hevc_instance->rps_id]);
    for (i = 0; i < cfg->numRefPics; i ++)
    {
      if (cfg->refPics[i].used_by_cur && (hevc_instance->poc + cfg->refPics[i].ref_pic) < 0)
      {
        hevc_instance->rps_id = s->num_short_term_ref_pic_sets - 1; //IPPPPPP
        codingType = HEVCENC_PREDICTED_FRAME;
      }
    }
  }
  return codingType;
}
#ifdef CTBRC_STRENGTH

static i32 float2fixpoint(float data)
{
    i32 i = 0;
    i32 result = 0;
    float pow2=2.0;
    /*0.16 format*/
    float base = 0.5;
    for(i= 0; i<16 ;i++)
    {
        result <<= 1;
        if(data >= base)
        {
           result |= 1;
           data -= base;

        }

        pow2 *= 2;
        base = 1.0/pow2;

    }
    return result;

}
static i32 float2fixpoint8(float data)
{
    i32 i = 0;
    i32 result = 0;
    float pow2=2.0;
    /*0.16 format*/
    float base = 0.5;
    for(i= 0; i<8 ;i++)
    {
        result <<= 1;
        if(data >= base)
        {
           result |= 1;
           data -= base;

        }

        pow2 *= 2;
        base = 1.0/pow2;

    }
    return result;

}
#endif

/*------------------------------------------------------------------------------

    Function name : HEVCEncStrmEncode
    Description   : Encodes a new picture
    Return type   : HEVCEncRet
    Argument      : inst - encoder instance
    Argument      : pEncIn - user provided input parameters
                    pEncOut - place where output info is returned
------------------------------------------------------------------------------*/


HEVCEncRet HEVCEncStrmEncode(HEVCEncInst inst, const HEVCEncIn *pEncIn,
                             HEVCEncOut *pEncOut,
                             HEVCEncSliceReadyCallBackFunc sliceReadyCbFunc,
                             void *pAppData)
{
  struct hevc_instance *hevc_instance = (struct hevc_instance *)inst;
  asicData_s *asic = &hevc_instance->asic;
  HEVCGopConfig *gopCfg = (HEVCGopConfig *)(&(pEncIn->gopConfig));
  struct container *c;
  struct hevc_buffer source;
  struct vps *v;
  struct sps *s;
  struct pps *p;
  struct rps *r;
  struct node *n;
  struct sw_picture *pic;
  struct slice *slice;
  i32 tmp, i;
  i32 ret = HEVCENC_ERROR;
  u32 status = ASIC_STATUS_ERROR;
  int  sliceNum;
  HEVCEncSliceReady slice_callback;
  HEVCEncPictureCodingType codingType;
  i32 top_pos, bottom_pos;

#ifdef CTBRC_STRENGTH
  i32 qpDeltaGain;
  float strength = 1.6f;
  float qpDeltaGainFloat;
#endif
  APITRACE("HEVCEncStrmEncode#");
  APITRACEPARAM_X("busLuma", pEncIn->busLuma);
  APITRACEPARAM_X("busChromaU", pEncIn->busChromaU);
  APITRACEPARAM_X("busChromaV", pEncIn->busChromaV);
  APITRACEPARAM("timeIncrement", pEncIn->timeIncrement);
  APITRACEPARAM_X("pOutBuf", pEncIn->pOutBuf);
  APITRACEPARAM_X("busOutBuf", pEncIn->busOutBuf);
  APITRACEPARAM("outBufSize", pEncIn->outBufSize);
  APITRACEPARAM("codingType", pEncIn->codingType);
  APITRACEPARAM("poc", pEncIn->poc);
  APITRACEPARAM("gopSize", pEncIn->gopSize);
  APITRACEPARAM("gopPicIdx", pEncIn->gopPicIdx);
  APITRACEPARAM_X("roiMapDeltaQpAddr", pEncIn->roiMapDeltaQpAddr);




  /* Check for illegal inputs */
  if ((hevc_instance == NULL) || (pEncIn == NULL) || (pEncOut == NULL))
  {
    APITRACE("HEVCEncStrmEncode: ERROR Null argument");
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (hevc_instance->inst != hevc_instance)
  {
    APITRACE("HEVCEncStrmEncode: ERROR Invalid instance");
    return HEVCENC_INSTANCE_ERROR;
  }
  /* Check status, INIT and ERROR not allowed */
  if ((hevc_instance->encStatus != HEVCENCSTAT_START_STREAM) &&
      (hevc_instance->encStatus != HEVCENCSTAT_START_FRAME))
  {
    APITRACE("HEVCEncStrmEncode: ERROR Invalid status");
    return HEVCENC_INVALID_STATUS;
  }

  /* Check for invalid input values */
  //if ((EWLReadAsicID() <= 0x48320101) && (pEncIn->gopSize > 1))//add by bright
  //if ( (pEncIn->gopSize > 1))
  //{
  //  APITRACE("HEVCEncStrmEncode: ERROR Invalid input. gopSize");
  //  return HEVCENC_INVALID_ARGUMENT;
  //}

  /* Check for invalid input values */
  if ((!HEVC_BUS_ADDRESS_VALID(pEncIn->busOutBuf)) ||
      (pEncIn->pOutBuf == NULL) ||
      (pEncIn->outBufSize < HEVCENCSTRMENCODE_MIN_BUF) ||
      (pEncIn->codingType > HEVCENC_NOTCODED_FRAME))
  {
    APITRACE("HEVCEncStrmEncode: ERROR Invalid input. Output buffer");
    return HEVCENC_INVALID_ARGUMENT;
  }

  if ((hevc_instance->gdrEnabled) && (pEncIn->codingType == HEVCENC_BIDIR_PREDICTED_FRAME))
  {
    APITRACE("HEVCEncSetCodingCtrl: ERROR gdr not support B frame");
    return HEVCENC_INVALID_ARGUMENT;
  }
  switch (hevc_instance->preProcess.inputFormat)
  {
    case HEVCENC_YUV420_PLANAR:
    case HEVCENC_YUV420_PLANAR_10BIT_I010:
    case HEVCENC_YUV420_PLANAR_10BIT_PACKED_PLANAR:
      if (!HEVC_BUS_CH_ADDRESS_VALID(pEncIn->busChromaV))
      {
        APITRACE("HEVCEncStrmEncode: ERROR Invalid input busChromaU");
        return HEVCENC_INVALID_ARGUMENT;
      }
      /* fall through */
    case HEVCENC_YUV420_SEMIPLANAR:
    case HEVCENC_YUV420_SEMIPLANAR_VU:
    case HEVCENC_YUV420_PLANAR_10BIT_P010:
      if (!HEVC_BUS_ADDRESS_VALID(pEncIn->busChromaU))
      {
        APITRACE("HEVCEncStrmEncode: ERROR Invalid input busChromaU");
        return HEVCENC_INVALID_ARGUMENT;
      }
      /* fall through */
    case HEVCENC_YUV422_INTERLEAVED_YUYV:
    case HEVCENC_YUV422_INTERLEAVED_UYVY:
    case HEVCENC_RGB565:
    case HEVCENC_BGR565:
    case HEVCENC_RGB555:
    case HEVCENC_BGR555:
    case HEVCENC_RGB444:
    case HEVCENC_BGR444:
    case HEVCENC_RGB888:
    case HEVCENC_BGR888:
    case HEVCENC_RGB101010:
    case HEVCENC_BGR101010:
    case HEVCENC_YUV420_10BIT_PACKED_Y0L2:
      if (!HEVC_BUS_ADDRESS_VALID(pEncIn->busLuma))
      {
        APITRACE("HEVCEncStrmEncode: ERROR Invalid input busLuma");
        return HEVCENC_INVALID_ARGUMENT;
      }
      break;
    default:
      APITRACE("HEVCEncStrmEncode: ERROR Invalid input format");
      return HEVCENC_INVALID_ARGUMENT;
  }

#ifdef INTERNAL_TEST
  /* Configure the encoder instance according to the test vector */
  HevcConfigureTestBeforeFrame(hevc_instance);
#endif
  /* Try to reserve the HW resource */
  if (EWLReserveHw(hevc_instance->asic.ewl) == EWL_ERROR)
  {
    APITRACE("HEVCEncStrmEncode: ERROR HW unavailable");
    return HEVCENC_HW_RESERVED;
  }
  {
    #if 0  //add by Bright Jiang
    EWLHwConfig_t cfg = EWLReadAsicConfig();

    if ((cfg.bFrameEnabled == EWL_HW_CONFIG_NOT_SUPPORTED) && ((pEncIn->codingType) == HEVCENC_BIDIR_PREDICTED_FRAME))
    {
      APITRACE("HEVCEncStrmEncode: Invalid coding format, B frame not supported by HW");
      return ENCHW_NOK;
    }
    #endif
  }
  asic->regs.roiMapDeltaQpAddr = pEncIn->roiMapDeltaQpAddr;
  hevc_instance->stream.stream = (u8 *)pEncIn->pOutBuf;
  hevc_instance->stream.stream_bus = pEncIn->busOutBuf;
  hevc_instance->stream.size = pEncIn->outBufSize;
  hevc_instance->stream.cnt = &hevc_instance->stream.byteCnt;

  pEncOut->pNaluSizeBuf = (u32 *)hevc_instance->asic.sizeTbl.virtualAddress;

  pEncOut->streamSize = 0;
  pEncOut->numNalus = 0;
  pEncOut->codingType = HEVCENC_NOTCODED_FRAME;


  hevc_instance->sliceReadyCbFunc = sliceReadyCbFunc;
  hevc_instance->pAppData = pAppData;

  /* Clear the NAL unit size table */
  if (pEncOut->pNaluSizeBuf != NULL)
    pEncOut->pNaluSizeBuf[0] = 0;

  hevc_instance->stream.byteCnt = 0;

  asic->regs.outputStrmBase = (u32)hevc_instance->stream.stream_bus;
  asic->regs.outputStrmSize = hevc_instance->stream.size;

  if (!(c = get_container(hevc_instance))) return HEVCENC_ERROR;

  hevc_instance->poc = pEncIn->poc;

  pEncOut->busScaledLuma = hevc_instance->asic.scaledImage.busAddress;
  pEncOut->scaledPicture = (u8 *)hevc_instance->asic.scaledImage.virtualAddress;
  /* Get parameter sets */
  v = (struct vps *)get_parameter_set(c, VPS_NUT, 0);
  s = (struct sps *)get_parameter_set(c, SPS_NUT, 0);
  p = (struct pps *)get_parameter_set(c, PPS_NUT, 0);

  codingType = get_rps_id(hevc_instance, pEncIn, s);
  r = (struct rps *)get_parameter_set(c, RPS,     hevc_instance->rps_id);
  if (!v || !s || !p || !r) return HEVCENC_ERROR;

  /* Initialize before/after/follow poc list */
  init_poc_list(r, hevc_instance->poc);
  /* mark unused picture, so that you can reuse those. TODO: IDR flush */
  if (ref_pic_marking(c, r))
  {
    Error(2, ERR, "RPS error: reference picture(s) not available");
    return HEVCENC_ERROR;
  }

  /* Get free picture (if any) and remove it from picture store */
  if (!(pic = get_picture(c, -1)))
  {
    if (!(pic = create_picture_ctrlsw(hevc_instance, v, s, p, asic->regs.sliceSize, hevc_instance->preProcess.lumWidthSrc, hevc_instance->preProcess.lumHeightSrc))) return HEVCENC_ERROR;
  }
  create_slices_ctrlsw(pic, p, asic->regs.sliceSize);

  queue_remove(&c->picture, (struct node *)pic);

  //store input addr
  pic->input.lum = pEncIn->busLuma;
  pic->input.cb = pEncIn->busChromaU;
  pic->input.cr = pEncIn->busChromaV;

  if (pic->picture_memeory_init == 0)
  {
    pic->recon.lum = asic->internalreconLuma[pic->picture_memeory_id].busAddress;
    pic->recon.cb = asic->internalreconChroma[pic->picture_memeory_id].busAddress;
    pic->recon.cr = pic->recon.cb + asic->regs.recon_chroma_half_size;
    pic->recon_4n_base = asic->internalreconLuma_4n[pic->picture_memeory_id].busAddress;

    // for compress
    {
      u32 lumaTblSize = asic->regs.recon_luma_compress ?
                        (((pic->sps->width + 63) / 64) * ((pic->sps->height + 63) / 64) * 8) : 0;
      lumaTblSize = ((lumaTblSize + 15) >> 4) << 4;
      pic->recon_compress.lumaTblBase = asic->compressTbl[pic->picture_memeory_id].busAddress;
      pic->recon_compress.chromaTblBase = (pic->recon_compress.lumaTblBase + lumaTblSize);
    }
    pic->picture_memeory_init = 1;
  }

  /* Activate reference paremater sets and reference picture set,
   * TODO: if parameter sets if id differs... */
  pic->rps = r;
  ASSERT(pic->vps == v);
  ASSERT(pic->sps == s);
  ASSERT(pic->pps == p);
  pic->poc     = hevc_instance->poc;

  //0=INTER. 1=INTRA(IDR). 2=MVC-INTER. 3=MVC-INTER(ref mod).
  if ((hevc_instance->gdrEnabled == 1) && (hevc_instance->encStatus == HEVCENCSTAT_START_FRAME) && (hevc_instance->gdrFirstIntraFrame == 0))
  {
    asic->regs.intraAreaTop = asic->regs.intraAreaLeft = asic->regs.intraAreaBottom =
                                asic->regs.intraAreaRight = 255;
    asic->regs.roi1Top = asic->regs.roi1Left = asic->regs.roi1Bottom =
                           asic->regs.roi1Right = 255;
    asic->regs.roi1DeltaQp = 0;
    if (pEncIn->codingType == HEVCENC_INTRA_FRAME)
    {
      //hevc_instance->gdrStart++ ;
      codingType = HEVCENC_PREDICTED_FRAME;
    }
    if (hevc_instance->gdrStart)
    {
      if (hevc_instance->gdrCount == 0)
        hevc_instance->rateControl.sei.insertRecoveryPointMessage = ENCHW_YES;
      else
        hevc_instance->rateControl.sei.insertRecoveryPointMessage = ENCHW_NO;
      top_pos = (hevc_instance->gdrCount / (1 + hevc_instance->interlaced)) * hevc_instance->gdrAverageMBRows;
      bottom_pos = 0;
      if (hevc_instance->gdrMBLeft)
      {
        if ((hevc_instance->gdrCount / (1 + (i32)hevc_instance->interlaced)) < hevc_instance->gdrMBLeft)
        {
          top_pos += (hevc_instance->gdrCount / (1 + (i32)hevc_instance->interlaced));
          bottom_pos += 1;
        }
        else
        {
          top_pos += hevc_instance->gdrMBLeft;
        }
      }
      bottom_pos += top_pos + hevc_instance->gdrAverageMBRows;
      if (bottom_pos > ((i32)hevc_instance->ctbPerCol - 1))
      {
        bottom_pos = hevc_instance->ctbPerCol - 1;
      }

      asic->regs.intraAreaTop = top_pos;
      asic->regs.intraAreaLeft = 0;
      asic->regs.intraAreaBottom = bottom_pos;
      asic->regs.intraAreaRight = hevc_instance->ctbPerRow - 1;

      //to make video quality in intra area is close to inter area.
      asic->regs.roi1Top = top_pos;
      asic->regs.roi1Left = 0;
      asic->regs.roi1Bottom = bottom_pos;
      asic->regs.roi1Right = hevc_instance->ctbPerRow - 1;

      asic->regs.roi1DeltaQp = 3;

      asic->regs.rcRoiEnable = 0x01;

    }

    asic->regs.roiUpdate   = 1;    /* ROI has changed from previous frame. */

  }

  if (codingType == HEVCENC_INTRA_FRAME)
  {
    asic->regs.frameCodingType = 1;
    asic->regs.nal_unit_type = IDR_W_RADL;
    pic->sliceInst->type = I_SLICE;
    pEncOut->codingType = HEVCENC_INTRA_FRAME;
  }
  else if (codingType == HEVCENC_PREDICTED_FRAME)
  {
    asic->regs.frameCodingType = 0;
    asic->regs.nal_unit_type = TRAIL_R;
    pic->sliceInst->type = P_SLICE;
    pEncOut->codingType = HEVCENC_PREDICTED_FRAME;
  }
  else if (codingType == HEVCENC_BIDIR_PREDICTED_FRAME)
  {
    asic->regs.frameCodingType = 2;
    asic->regs.nal_unit_type = TRAIL_R;
    pic->sliceInst->type = B_SLICE;
    pEncOut->codingType = HEVCENC_BIDIR_PREDICTED_FRAME;
  }



  /* Generate reference picture list of current picture using active
   * reference picture set */
  reference_picture_list(c, pic);
#ifdef TEST_DATA
  EncTraceReferences(c, pic);
#endif
  if (pic->sliceInst->type != I_SLICE)
  {
    int ref_cnt = r->before_cnt + r->after_cnt;
    //H2V2 limit:
    //List lengh is no more than 2
    //number of active ref is no more than 1
    ASSERT(ref_cnt <= 2);
    ASSERT(pic->sliceInst->active_l0_cnt <= 1);
    ASSERT(pic->sliceInst->active_l1_cnt <= 1);

    asic->regs.l0_used_by_curr_pic[0] = 0;
    asic->regs.l0_used_by_curr_pic[1] = 0;
    asic->regs.l1_used_by_curr_pic[0] = 0;
    asic->regs.l1_used_by_curr_pic[1] = 0;

    for (i = 0; i < ref_cnt; i ++)
      asic->regs.l0_used_by_curr_pic[i] = 1;

    for (i = 0; i < pic->sliceInst->active_l0_cnt; i++)
    {
      asic->regs.pRefPic_recon_l0[0][i] = pic->rpl[0][i]->recon.lum;
      asic->regs.pRefPic_recon_l0[1][i] = pic->rpl[0][i]->recon.cb;
      asic->regs.pRefPic_recon_l0[2][i] = pic->rpl[0][i]->recon.cr;
      asic->regs.pRefPic_recon_l0_4n[i] =  pic->rpl[0][i]->recon_4n_base;

      asic->regs.l0_delta_poc[i] = pic->rpl[0][i]->poc - pic->poc;

      //compress
      asic->regs.ref_l0_luma_compressed[i] = pic->rpl[0][i]->recon_compress.lumaCompressed;
      asic->regs.ref_l0_luma_compress_tbl_base[i] = pic->rpl[0][i]->recon_compress.lumaTblBase;

      asic->regs.ref_l0_chroma_compressed[i] = pic->rpl[0][i]->recon_compress.chromaCompressed;
      asic->regs.ref_l0_chroma_compress_tbl_base[i] = pic->rpl[0][i]->recon_compress.chromaTblBase;
    }

    if (pic->sliceInst->type == B_SLICE)
    {
      for (i = 0; i < ref_cnt; i ++)
        asic->regs.l1_used_by_curr_pic[i] = 1;

      for (i = 0; i < pic->sliceInst->active_l1_cnt; i++)
      {
        asic->regs.pRefPic_recon_l1[0][i] = (u32)pic->rpl[1][i]->recon.lum;
        asic->regs.pRefPic_recon_l1[1][i] = (u32)pic->rpl[1][i]->recon.cb;
        asic->regs.pRefPic_recon_l1[2][i] = (u32)pic->rpl[1][i]->recon.cr;
        asic->regs.pRefPic_recon_l1_4n[i] = (u32)pic->rpl[1][i]->recon_4n_base;

        asic->regs.l1_delta_poc[i] = pic->rpl[1][i]->poc - pic->poc;

        //compress
        asic->regs.ref_l1_luma_compressed[i] = pic->rpl[1][i]->recon_compress.lumaCompressed;
        asic->regs.ref_l1_luma_compress_tbl_base[i] = pic->rpl[1][i]->recon_compress.lumaTblBase;

        asic->regs.ref_l1_chroma_compressed[i] = pic->rpl[1][i]->recon_compress.chromaCompressed;
        asic->regs.ref_l1_chroma_compress_tbl_base[i] = pic->rpl[1][i]->recon_compress.chromaTblBase;
      }
    }
  }

  asic->regs.recon_luma_compress_tbl_base = pic->recon_compress.lumaTblBase;
  asic->regs.recon_chroma_compress_tbl_base = pic->recon_compress.chromaTblBase;

  asic->regs.active_l0_cnt = pic->sliceInst->active_l0_cnt;
  asic->regs.active_l1_cnt = pic->sliceInst->active_l1_cnt;
  asic->regs.active_override_flag = pic->sliceInst->active_override_flag;

  asic->regs.lists_modification_present_flag = pic->pps->lists_modification_present_flag;
  asic->regs.ref_pic_list_modification_flag_l0 = pic->sliceInst->ref_pic_list_modification_flag_l0;
  asic->regs.list_entry_l0[0] = pic->sliceInst->list_entry_l0[0];
  asic->regs.list_entry_l0[1] = pic->sliceInst->list_entry_l0[1];
  asic->regs.ref_pic_list_modification_flag_l1 = pic->sliceInst->ref_pic_list_modification_flag_l1;
  asic->regs.list_entry_l1[0] = pic->sliceInst->list_entry_l1[0];
  asic->regs.list_entry_l1[1] = pic->sliceInst->list_entry_l1[1];

  hevc_instance->rateControl.hierarchial_bit_allocation_GOP_size = pEncIn->gopSize;
  /* Rate control */
  if (pic->sliceInst->type == I_SLICE)
  {
    hevc_instance->rateControl.gopPoc = 0;
    hevc_instance->rateControl.encoded_frame_number = 0;
  }
  else
  {
    hevc_instance->rateControl.frameQpDelta = gopCfg->pGopPicCfg[gopCfg->id].QpOffset << QP_FRACTIONAL_BITS;
    hevc_instance->rateControl.gopPoc       = gopCfg->pGopPicCfg[gopCfg->id].poc;
    hevc_instance->rateControl.encoded_frame_number = pEncIn->gopPicIdx;
    if (hevc_instance->rateControl.gopPoc > 0)
      hevc_instance->rateControl.gopPoc -= 1;

    if (pEncIn->gopSize > 8)
    {
      hevc_instance->rateControl.hierarchial_bit_allocation_GOP_size = 1;
      hevc_instance->rateControl.gopPoc = 0;
      hevc_instance->rateControl.encoded_frame_number = 0;
    }
  }

  //CTB_RC
  hevc_instance->rateControl.ctbRcBitsMin = asic->regs.ctbBitsMin;
  hevc_instance->rateControl.ctbRcBitsMax = asic->regs.ctbBitsMax;
  hevc_instance->rateControl.ctbRctotalLcuBit = asic->regs.totalLcuBits;
  if (hevc_instance->rateControl.seqStart)
  {
    hevc_instance->rateControl.ctbMemCurAddr = asic->ctbRCCur.busAddress;
    hevc_instance->rateControl.ctbMemPreAddr = asic->ctbRCPre.busAddress;
    hevc_instance->rateControl.ctbMemCurVirtualAddr = asic->ctbRCCur.virtualAddress; /* CTB_RC_0701   */
    hevc_instance->rateControl.ctbMemPreVirtualAddr = asic->ctbRCPre.virtualAddress; /* CTB_RC_0701   */
  }
#ifdef CTBRC_STRENGTH
  asic->regs.qpMin = hevc_instance->rateControl.qpMin >> QP_FRACTIONAL_BITS;
  asic->regs.qpMax = hevc_instance->rateControl.qpMax >> QP_FRACTIONAL_BITS;
  if(hevc_instance->rateControl.ctbRc)
  {
      asic->regs.rcQpDeltaRange = 10;
  }
  else
  {
      asic->regs.rcQpDeltaRange = 0;
  }
#endif

  HevcBeforePicRc(&hevc_instance->rateControl, pEncIn->timeIncrement,
                  pic->sliceInst->type);
  //CTB_RC
  {
    u32 val = hevc_instance->rateControl.ctbMemCurAddr;
    hevc_instance->rateControl.ctbMemCurAddr = hevc_instance->rateControl.ctbMemPreAddr;
    hevc_instance->rateControl.ctbMemPreAddr = val;
    //CTB_RC_0701
    u32 *pval = hevc_instance->rateControl.ctbMemCurVirtualAddr;
    hevc_instance->rateControl.ctbMemCurVirtualAddr = hevc_instance->rateControl.ctbMemPreVirtualAddr;
    hevc_instance->rateControl.ctbMemPreVirtualAddr = pval;
    asic->regs.ctbRcBitMemAddrCur = hevc_instance->rateControl.ctbMemCurAddr;
    asic->regs.ctbRcBitMemAddrPre = hevc_instance->rateControl.ctbMemPreAddr;
  }
  asic->regs.bitsRatio    = hevc_instance->rateControl.bitsRatio;
  asic->regs.ctbRcThrdMin = hevc_instance->rateControl.ctbRcThrdMin;
  asic->regs.ctbRcThrdMax = hevc_instance->rateControl.ctbRcThrdMax;
#ifndef CTBRC_STRENGTH
  if (hevc_instance->rateControl.ctbRc && hevc_instance->asic.regs.rcRoiEnable == RCROIMODE_BOTH_DIABLE)
  {
    hevc_instance->asic.regs.rcRoiEnable = RCROIMODE_ONLY_RC_ENABLE;
  }
  if (hevc_instance->rateControl.ctbRc && (hevc_instance->asic.regs.rcRoiEnable == RCROIMODE_ROI_AREA_ENABLE || hevc_instance->asic.regs.rcRoiEnable == RCROIMODE_ROI_MAP_ENABLE))
  {
    hevc_instance->rateControl.ctbRc = 0;
  }
#else
  if(hevc_instance->rateControl.ctbRc)
  {
    hevc_instance->asic.regs.rcRoiEnable |= 0x04;
  }
  else
  {
         hevc_instance->asic.regs.rcRoiEnable&= (~0x04);
  }
#endif
#if 0
  printf("/***********sw***********/\nhevc_instance->rateControl.ctbRc=%d,asic->regs.ctbRcBitMemAddrCur=0x%x,asic->regs.ctbRcBitMemAddrPre=0x%x\n", hevc_instance->rateControl.ctbRc, asic->regs.ctbRcBitMemAddrCur, asic->regs.ctbRcBitMemAddrPre);
  printf("asic->regs.bitsRatio=%d,asic->regs.ctbRcThrdMin=%d,asic->regs.ctbRcThrdMax=%d,asic->regs.rcRoiEnable=%d\n", asic->regs.bitsRatio, asic->regs.ctbRcThrdMin, asic->regs.ctbRcThrdMax, asic->regs.rcRoiEnable);
  printf("asic->regs.ctbBitsMin=%d,asic->regs.ctbBitsMax=%d,asic->regs.totalLcuBits=%d\n", asic->regs.ctbBitsMin, asic->regs.ctbBitsMax, asic->regs.totalLcuBits);
#endif

  /* time stamp updated */
  HevcUpdateSeiTS(&hevc_instance->rateControl.sei, pEncIn->timeIncrement);
  /* Rate control may choose to skip the frame */
  if (hevc_instance->rateControl.frameCoded == ENCHW_NO)
  {
    APITRACE("HEVCEncStrmEncode: OK, frame skipped");


    //pSlice->frameNum = pSlice->prevFrameNum;    /* restore frame_num */


    EWLReleaseHw(asic->ewl);
    queue_put(&c->picture, (struct node *)pic);

    return HEVCENC_FRAME_READY;
  }

  asic->regs.targetPicSize = hevc_instance->rateControl.targetPicSize;
  if (codingType == HEVCENC_INTRA_FRAME)
  {
    asic->regs.minPicSize = hevc_instance->rateControl.minPicSizeI;
    asic->regs.maxPicSize = hevc_instance->rateControl.maxPicSizeI;
    //printf("I frame targetPicSize=%d,minPicSize=%d,maxPicSize=%d\n",asic->regs.targetPicSize,asic->regs.minPicSize,asic->regs.maxPicSize);
  }
  else if (codingType == HEVCENC_PREDICTED_FRAME)
  {
    asic->regs.minPicSize = hevc_instance->rateControl.minPicSizeP;
    asic->regs.maxPicSize = hevc_instance->rateControl.maxPicSizeP;
    //printf("P frame targetPicSize=%d,minPicSize=%d,maxPicSize=%d\n",asic->regs.targetPicSize,asic->regs.minPicSize,asic->regs.maxPicSize);
  }
  else if (codingType == HEVCENC_BIDIR_PREDICTED_FRAME)
  {
    asic->regs.minPicSize = hevc_instance->rateControl.minPicSizeB;
    asic->regs.maxPicSize = hevc_instance->rateControl.maxPicSizeB;
    //printf("B frame targetPicSize=%d,minPicSize=%d,maxPicSize=%d\n",asic->regs.targetPicSize,asic->regs.minPicSize,asic->regs.maxPicSize);
  }


  asic->regs.inputLumBase = pic->input.lum;
  asic->regs.inputCbBase = pic->input.cb;
  asic->regs.inputCrBase = pic->input.cr;



  asic->regs.minCbSize = pic->sps->log2_min_cb_size;
  asic->regs.maxCbSize = pic->pps->log2_ctb_size;
  asic->regs.minTrbSize = pic->sps->log2_min_tr_size;
  asic->regs.maxTrbSize = pic->pps->log2_max_tr_size;

  asic->regs.picWidth = pic->sps->width_min_cbs;
  asic->regs.picHeight = pic->sps->height_min_cbs;

  asic->regs.pps_deblocking_filter_override_enabled_flag = pic->pps->deblocking_filter_override_enabled_flag;


  /* Enable slice ready interrupts if defined by config and slices in use */
  asic->regs.sliceReadyInterrupt =
    ENCH2_SLICE_READY_INTERRUPT & ((asic->regs.sliceNum > 1));
  asic->regs.picInitQp = pic->pps->init_qp;
#ifndef CTBRC_STRENGTH
  asic->regs.qp = hevc_instance->rateControl.qpHdr;
#else
    //printf("float qp=%f\n",hevc_instance->rateControl.qpHdr);


    //printf("float qp=%f\n",inst->rateControl.qpHdr);
    asic->regs.qp = (hevc_instance->rateControl.qpHdr >> QP_FRACTIONAL_BITS);

    asic->regs.qpfrac = hevc_instance->rateControl.qpHdr - (asic->regs.qp << QP_FRACTIONAL_BITS);
    //printf("float qp=%f, int qp=%d,qpfrac=0x%x\n",hevc_instance->rateControl.qpHdr,asic->regs.qp, asic->regs.qpfrac);



    //#define BLOCK_SIZE_MIN_RC  32 , 2=16x16,1= 32x32, 0=64x64
    asic->regs.rcBlockSize = hevc_instance->blockRCSize;

    {
        qpDeltaGainFloat = (float)hevc_instance->rateControl.rcPicComplexity / (hevc_instance->rateControl.ctbPerPic*8*8);
        if(qpDeltaGainFloat < 14.0)
        {
            qpDeltaGainFloat = 14.0;
        }
        //printf("average complexity= %f\n",qpDeltaGainFloat);
        qpDeltaGainFloat = qpDeltaGainFloat*qpDeltaGainFloat*strength/256;
       //qpDeltaGainFloat = qpDeltaGainFloat;
        qpDeltaGain = (((i32)qpDeltaGainFloat)<<8)+ float2fixpoint8(qpDeltaGainFloat - (float)((i32)qpDeltaGainFloat) );

        if(asic->regs.frameCodingType == 1)
        {
            asic->regs.qpDeltaMBGain = qpDeltaGain ;
            asic->regs.offsetMBComplexity = 15;
        }
        else
        {
            asic->regs.qpDeltaMBGain = qpDeltaGain ;
            asic->regs.offsetMBComplexity = 15;
        }
    }


    //printf("regs->offsetMBComplexity = 0x%x,regs->qpDeltaMBGain=0x%x\n",regs->offsetMBComplexity,regs->qpDeltaMBGain);



#endif

  asic->regs.qpMax = hevc_instance->rateControl.qpMax >> QP_FRACTIONAL_BITS;
  asic->regs.qpMin = hevc_instance->rateControl.qpMin >> QP_FRACTIONAL_BITS;

  pic->pps->qp = asic->regs.qp;
  pic->pps->qpMin = asic->regs.qpMin;
  pic->pps->qpMax = asic->regs.qpMax;

  asic->regs.diffCuQpDeltaDepth = pic->pps->diff_cu_qp_delta_depth;

  asic->regs.cbQpOffset = pic->pps->cb_qp_offset;
  asic->regs.saoEnable = pic->sps->sao_enabled_flag;
  asic->regs.maxTransHierarchyDepthInter = pic->sps->max_tr_hierarchy_depth_inter;
  asic->regs.maxTransHierarchyDepthIntra = pic->sps->max_tr_hierarchy_depth_intra;

  asic->regs.cuQpDeltaEnabled = pic->pps->cu_qp_delta_enabled_flag;
  asic->regs.log2ParellelMergeLevel = pic->pps->log2_parallel_merge_level;
  asic->regs.numShortTermRefPicSets = pic->sps->num_short_term_ref_pic_sets;

  asic->regs.reconLumBase = pic->recon.lum;
  asic->regs.reconCbBase = pic->recon.cb;
  asic->regs.reconCrBase = pic->recon.cr;

  asic->regs.reconL4nBase = pic->recon_4n_base;

  /* Map the reset of instance parameters */
  asic->regs.poc = pic->poc;
  asic->regs.rpsId = pic->rps->ps.id;
  asic->regs.numNegativePics = pic->rps->num_negative_pics;
  asic->regs.numPositivePics = pic->rps->num_positive_pics;

  asic->regs.filterDisable = hevc_instance->disableDeblocking;
  asic->regs.tc_Offset = hevc_instance->tc_Offset * 2;
  asic->regs.beta_Offset = hevc_instance->beta_Offset * 2;

  /* strong_intra_smoothing_enabled_flag */
  asic->regs.strong_intra_smoothing_enabled_flag = pic->sps->strong_intra_smoothing_enabled_flag;

  /* constrained_intra_pred_flag */
  asic->regs.constrained_intra_pred_flag = pic->pps->constrained_intra_pred_flag;


  /* HW base address for NAL unit sizes is affected by start offset
   * and SW created NALUs. */
  asic->regs.sizeTblBase = asic->sizeTbl.busAddress;
  /*clip qp*/
#ifdef CTBRC_STRENGTH

	asic->regs.offsetSliceQp = 0;
	if( asic->regs.qp >= 35)
	{
		asic->regs.offsetSliceQp = 35 - asic->regs.qp;
	}
	if(asic->regs.qp <= 15)
	{
		asic->regs.offsetSliceQp = 15 - asic->regs.qp;
	}

  /*
      if ROI update asic->regs.roi1DeltaQp and asic->regs.roi2DeltaQp already updates in HEVCEncSetCodingCtrl
      Here Just check and refine to meaningful value, remove ctbrc check
  */
   //if((hevc_instance->asic.regs.rcRoiEnable&0x04) == 0)
  {
    if(hevc_instance->asic.regs.rcRoiEnable &0x3)
    {
      asic->regs.roi1DeltaQp = CLIP3(0 ,15 - asic->regs.offsetSliceQp,asic->regs.roi1DeltaQp);
      asic->regs.roi2DeltaQp = CLIP3(0 ,15 - asic->regs.offsetSliceQp,asic->regs.roi2DeltaQp);

      if(((i32)asic->regs.qp - (i32)asic->regs.qpMin) < asic->regs.roi1DeltaQp)
      {
        asic->regs.roi1DeltaQp = (i32)asic->regs.qp - (i32)asic->regs.qpMin;
      }

      if(((i32)asic->regs.qp - (i32)asic->regs.qpMin) < asic->regs.roi2DeltaQp)
      {
        asic->regs.roi2DeltaQp = (i32)asic->regs.qp - (i32)asic->regs.qpMin;
      }
    }
  }
#endif


  /* HW Base must be 64-bit aligned */
  ASSERT(asic->regs.sizeTblBase % 8 == 0);

  /*inter prediction parameters*/
  double dQPFactor = 0.8;
  bool depth0 = true;

  asic->regs.nuh_temporal_id = 0;

  if (pic->sliceInst->type != I_SLICE)
  {
    dQPFactor = gopCfg->pGopPicCfg[gopCfg->id].QpFactor;
    depth0 = (gopCfg->pGopPicCfg[gopCfg->id].poc % pEncIn->gopSize) ? false : true;
    asic->regs.nuh_temporal_id = gopCfg->pGopPicCfg[gopCfg->id].temporalId;
    if(asic->regs.nuh_temporal_id)
      asic->regs.nal_unit_type = TSA_R;
  }

  HEVCEncSetQuantParameters(asic, dQPFactor, pic->sliceInst->type, depth0,
                            hevc_instance->rateControl.ctbRc);

  asic->regs.skip_chroma_dc_threadhold = 2;

  /*tuning on intra cu cost bias*/
  asic->regs.bits_est_bias_intra_cu_8 = 22; //25;
  asic->regs.bits_est_bias_intra_cu_16 = 40; //48;
  asic->regs.bits_est_bias_intra_cu_32 = 86; //108;
  asic->regs.bits_est_bias_intra_cu_64 = 38 * 8; //48*8;

  asic->regs.bits_est_1n_cu_penalty = 5;
  asic->regs.bits_est_tu_split_penalty = 3;
  asic->regs.inter_skip_bias = 124;

  /*small resolution, smaller CU/TU prefered*/
  if (pic->sps->width <= 832 && pic->sps->height <= 480)
  {
    asic->regs.bits_est_1n_cu_penalty = 0;
    asic->regs.bits_est_tu_split_penalty = 2;
  }

  // scaling list enable
  asic->regs.scaling_list_enabled_flag = pic->sps->scaling_list_enable_flag;

  HEVCEncDnfPrepare(hevc_instance);

#ifdef INTERNAL_TEST
  /* Configure the ASIC penalties according to the test vector */
  HevcConfigureTestBeforeStart(hevc_instance);
#endif


  hevc_instance->preProcess.bottomField = (hevc_instance->poc % 2) == hevc_instance->fieldOrder;
  HevcUpdateSeiPS(&hevc_instance->rateControl.sei, hevc_instance->preProcess.interlacedFrame, hevc_instance->preProcess.bottomField);
  EncPreProcess(asic, &hevc_instance->preProcess);
  /* SEI message */
  {
    sei_s *sei = &hevc_instance->rateControl.sei;

    if (sei->enabled == ENCHW_YES || sei->userDataEnabled == ENCHW_YES || sei->insertRecoveryPointMessage == ENCHW_YES)
    {

      if (sei->activated_sps == 0)
      {

        tmp = hevc_instance->stream.byteCnt;
        HevcNalUnitHdr(&hevc_instance->stream, PREFIX_SEI_NUT, sei->byteStream);
        HevcActiveParameterSetsSei(&hevc_instance->stream, sei, &s->vui);
        rbsp_trailing_bits(&hevc_instance->stream);

        sei->nalUnitSize = hevc_instance->stream.byteCnt;
        printf(" activated_sps sei size=%d\n", hevc_instance->stream.byteCnt);
        //pEncOut->streamSize+=hevc_instance->stream.byteCnt;
        HEVCAddNaluSize(pEncOut, hevc_instance->stream.byteCnt - tmp);
      }

      if (sei->enabled == ENCHW_YES)
      {
        if ((pic->sliceInst->type == I_SLICE) && (sei->hrd == ENCHW_YES))
        {
          tmp = hevc_instance->stream.byteCnt;
          HevcNalUnitHdr(&hevc_instance->stream, PREFIX_SEI_NUT, sei->byteStream);
          HevcBufferingSei(&hevc_instance->stream, sei, &s->vui);
          rbsp_trailing_bits(&hevc_instance->stream);

          sei->nalUnitSize = hevc_instance->stream.byteCnt;
          printf("BufferingSei sei size=%d\n", hevc_instance->stream.byteCnt);
          //pEncOut->streamSize+=hevc_instance->stream.byteCnt;
          HEVCAddNaluSize(pEncOut, hevc_instance->stream.byteCnt - tmp);
        }
        tmp = hevc_instance->stream.byteCnt;
        HevcNalUnitHdr(&hevc_instance->stream, PREFIX_SEI_NUT, sei->byteStream);
        HevcPicTimingSei(&hevc_instance->stream, sei, &s->vui);
        rbsp_trailing_bits(&hevc_instance->stream);

        sei->nalUnitSize = hevc_instance->stream.byteCnt;
        printf("PicTiming sei size=%d\n", hevc_instance->stream.byteCnt);
        //pEncOut->streamSize+=hevc_instance->stream.byteCnt;
        HEVCAddNaluSize(pEncOut, hevc_instance->stream.byteCnt - tmp);
      }

      if (sei->userDataEnabled == ENCHW_YES)
      {
        tmp = hevc_instance->stream.byteCnt;
        HevcNalUnitHdr(&hevc_instance->stream, PREFIX_SEI_NUT, sei->byteStream);
        HevcUserDataUnregSei(&hevc_instance->stream, sei);
        rbsp_trailing_bits(&hevc_instance->stream);

        sei->nalUnitSize = hevc_instance->stream.byteCnt;
        printf("UserDataUnreg sei size=%d\n", hevc_instance->stream.byteCnt);
        //pEncOut->streamSize+=hevc_instance->stream.byteCnt;
        HEVCAddNaluSize(pEncOut, hevc_instance->stream.byteCnt - tmp);
      }


      if (sei->insertRecoveryPointMessage == ENCHW_YES)
      {
        tmp = hevc_instance->stream.byteCnt;
        HevcNalUnitHdr(&hevc_instance->stream, PREFIX_SEI_NUT, sei->byteStream);
        HevcRecoveryPointSei(&hevc_instance->stream, sei);
        rbsp_trailing_bits(&hevc_instance->stream);

        sei->nalUnitSize = hevc_instance->stream.byteCnt;
        printf("RecoveryPoint sei size=%d\n", hevc_instance->stream.byteCnt);
        //pEncOut->streamSize+=hevc_instance->stream.byteCnt;
        HEVCAddNaluSize(pEncOut, hevc_instance->stream.byteCnt - tmp);
      }

#if 0
      //rbsp_trailing_bits(&hevc_instance->stream);
      sei->nalUnitSize = hevc_instance->stream.byteCnt;
      printf("sei size=%d\n", hevc_instance->stream.byteCnt);
      //pEncOut->streamSize+=hevc_instance->stream.byteCnt;
      HEVCAddNaluSize(pEncOut, hevc_instance->stream.byteCnt);
#endif
    }
  }
  hevc_instance->numNalus = pEncOut->numNalus;


  /* Reset callback struct */
  slice_callback.slicesReadyPrev = 0;
  slice_callback.slicesReady = 0;
  slice_callback.sliceSizes = (u32 *) hevc_instance->asic.sizeTbl.virtualAddress;
  //slice_callback.sliceSizes += hevc_instance->numNalus;
  slice_callback.nalUnitInfoNum = hevc_instance->numNalus;
  slice_callback.pOutBuf = pEncIn->pOutBuf;
  slice_callback.pAppData = hevc_instance->pAppData;

  HevcSetNewFrame(hevc_instance);
  hevc_instance->output_buffer_over_flow = 0;
  EncAsicFrameStart(asic->ewl, &asic->regs);

  do
  {
    /* Encode one frame */
    i32 ewl_ret;

    /* Wait for IRQ for every slice or for complete frame */
    if ((asic->regs.sliceNum > 1) && hevc_instance->sliceReadyCbFunc)
      ewl_ret = EWLWaitHwRdy(asic->ewl, &slice_callback.slicesReady, asic->regs.sliceNum, &status);
    else
      ewl_ret = EWLWaitHwRdy(asic->ewl, NULL, asic->regs.sliceNum, &status);

    if (ewl_ret != EWL_OK)
    {
      status = ASIC_STATUS_ERROR;

      if (ewl_ret == EWL_ERROR)
      {
        /* IRQ error => Stop and release HW */
        ret = HEVCENC_SYSTEM_ERROR;
      }
      else    /*if(ewl_ret == EWL_HW_WAIT_TIMEOUT) */
      {
        /* IRQ Timeout => Stop and release HW */
        ret = HEVCENC_HW_TIMEOUT;
      }
      APITRACE("HEVCEncStrmEncode: ERROR Fatal system error ewl_ret != EWL_OK.");
      EncAsicStop(asic->ewl);
      /* Release HW so that it can be used by other codecs */
      EWLReleaseHw(asic->ewl);

    }
    else
    {
      /* Check ASIC status bits and possibly release HW */
      status = EncAsicCheckStatus_V2(asic, status);

      switch (status)
      {
        case ASIC_STATUS_ERROR:
          APITRACE("HEVCEncStrmEncode: ERROR status error");
          ret = HEVCENC_ERROR;
          break;
        case ASIC_STATUS_HW_TIMEOUT:
          APITRACE("HEVCEncStrmEncode: ERROR HW/IRQ timeout");
          ret = HEVCENC_HW_TIMEOUT;
          break;
        case ASIC_STATUS_SLICE_READY:

          ret = HEVCENC_OK;
#if 1

          /* Issue callback to application telling how many slices
           * are available. */
#if 0
          if (slice_callback.slicesReady != (slice_callback.slicesReadyPrev + 1))
          {
            //this is used for hardware interrupt.
            slice_callback.slicesReady++;

          }
#endif
          if (hevc_instance->sliceReadyCbFunc &&
              (slice_callback.slicesReadyPrev < slice_callback.slicesReady) && hevc_instance->rateControl.hrd == ENCHW_NO)
            hevc_instance->sliceReadyCbFunc(&slice_callback);
          slice_callback.slicesReadyPrev = slice_callback.slicesReady;
#endif
          break;
        case ASIC_STATUS_BUFF_FULL:
          APITRACE("HEVCEncStrmEncode: ERROR buffer full.");
          hevc_instance->output_buffer_over_flow = 1;
          ret = HEVCENC_OK;
          //inst->stream.overflow = ENCHW_YES;
          break;
        case ASIC_STATUS_HW_RESET:
          APITRACE("HEVCEncStrmEncode: ERROR HW bus access error");
          ret = HEVCENC_HW_RESET;
          break;
        case ASIC_STATUS_FRAME_READY:
          if (asic->regs.sliceReadyInterrupt)
          {
            if (slice_callback.slicesReady != asic->regs.sliceNum)
            {
              //this is used for hardware interrupt.
              slice_callback.slicesReady = asic->regs.sliceNum;
            }
            if (hevc_instance->sliceReadyCbFunc &&
                (slice_callback.slicesReadyPrev < slice_callback.slicesReady) && hevc_instance->rateControl.hrd == ENCHW_NO)
              hevc_instance->sliceReadyCbFunc(&slice_callback);
            slice_callback.slicesReadyPrev = slice_callback.slicesReady;
          }
          sliceNum = 0;
          for (n = pic->slice.tail; n; n = n->next)
          {
            //slice = (struct slice *)pic->sliceInst;

            //*slice->cabac.b.cnt += ((u32 *)(asic->regs.sizeTblBase))[sliceNum];
            //hevc_instance->stream.byteCnt+=((u32 *)(asic->regs.sizeTblBase))[sliceNum];
            //printf("POC %3d slice %d size=%d\n", hevc_instance->poc, sliceNum, ((asic->sizeTbl.virtualAddress))[pEncOut->numNalus]);
            sliceNum++;
            pEncOut->numNalus++;

#if 0
            /* Stream header remainder ie. last not full 64-bit address
             * is counted in HW data. */
            const u32 hw_offset = inst->stream.byteCnt & (0x07U);

            inst->stream.byteCnt +=
              asic->regs.outputStrmSize - hw_offset;
            inst->stream.stream +=
              asic->regs.outputStrmSize - hw_offset;
#endif
          }

          ((asic->sizeTbl.virtualAddress))[pEncOut->numNalus] = 0;
          asic->regs.outputStrmSize =
            EncAsicGetRegisterValue(asic->ewl, asic->regs.regMirror, HWIF_ENC_OUTPUT_STRM_BUFFER_LIMIT);
          hevc_instance->stream.byteCnt += asic->regs.outputStrmSize;
#ifdef CTBRC_STRENGTH
          asic->regs.sumOfQP =
            EncAsicGetRegisterValue(asic->ewl, asic->regs.regMirror, HWIF_ENC_QP_SUM);
          asic->regs.sumOfQPNumber =
            EncAsicGetRegisterValue(asic->ewl, asic->regs.regMirror, HWIF_ENC_QP_NUM);

          asic->regs.picComplexity =
            EncAsicGetRegisterValue(asic->ewl, asic->regs.regMirror, HWIF_ENC_PIC_COMPLEXITY);
#else
asic->regs.averageQP =
            EncAsicGetRegisterValue(asic->ewl, asic->regs.regMirror, HWIF_ENC_AVERAGEQP);
          asic->regs.nonZeroCount =
            EncAsicGetRegisterValue(asic->ewl, asic->regs.regMirror, HWIF_ENC_NONZEROCOUNT);
#endif

          //for adaptive GOP
          asic->regs.intraCu8Num =
            EncAsicGetRegisterValue(asic->ewl, asic->regs.regMirror, HWIF_ENC_INTRACU8NUM);

          asic->regs.skipCu8Num =
            EncAsicGetRegisterValue(asic->ewl, asic->regs.regMirror, HWIF_ENC_SKIPCU8NUM);

          asic->regs.PBFrame4NRdCost =
            EncAsicGetRegisterValue(asic->ewl, asic->regs.regMirror, HWIF_ENC_PBFRAME4NRDCOST);

          //CTB_RC
          asic->regs.ctbBitsMin =
            EncAsicGetRegisterValue(asic->ewl, asic->regs.regMirror, HWIF_ENC_CTBBITSMIN);
          asic->regs.ctbBitsMax =
            EncAsicGetRegisterValue(asic->ewl, asic->regs.regMirror, HWIF_ENC_CTBBITSMAX);
          asic->regs.totalLcuBits =
            EncAsicGetRegisterValue(asic->ewl, asic->regs.regMirror, HWIF_ENC_TOTALLCUBITS);
          hevc_instance->iThreshSigmaCalcd = asic->regs.nrThreshSigmaCalced =
                                               EncAsicGetRegisterValue(asic->ewl, asic->regs.regMirror, HWIF_ENC_THRESH_SIGMA_CALCED);
          hevc_instance->iSigmaCalcd = asic->regs.nrFrameSigmaCalced =
                                         EncAsicGetRegisterValue(asic->ewl, asic->regs.regMirror, HWIF_ENC_FRAME_SIGMA_CALCED);


          asic->regs.SSEDivide256 =
            EncAsicGetRegisterValue(asic->ewl, asic->regs.regMirror, HWIF_ENC_SSE_DIV_256);
          EWLReleaseHw(asic->ewl);
          ret = HEVCENC_OK;
          break;

        default:
          APITRACE("HEVCEncStrmEncode: ERROR Fatal system error");
          /* should never get here */
          ASSERT(0);
          ret = HEVCENC_ERROR;
      }
    }
  }
  while (status == ASIC_STATUS_SLICE_READY);

  if (ret != HEVCENC_OK)
    goto out;

  //set/clear picture compress flag
  pic->recon_compress.lumaCompressed = asic->regs.recon_luma_compress ? 1 : 0;
  pic->recon_compress.chromaCompressed = asic->regs.recon_chroma_compress ? 1 : 0;

  if (hevc_instance->output_buffer_over_flow == 1)
  {
    hevc_instance->stream.byteCnt = 0;
    pEncOut->streamSize = hevc_instance->stream.byteCnt;
    pEncOut->numNalus = 0;
    pEncOut->pNaluSizeBuf[0] = 0;
    hevc_instance->encStatus = HEVCENCSTAT_START_FRAME;
    ret = HEVCENC_OUTPUT_BUFFER_OVERFLOW;
    APITRACE("HEVCEncStrmEncode: ERROR Output buffer too small");
    goto out;

  }

  {
    i32 stat, j, rows, totalrows;
    float psnr;
    rows = 0;
    totalrows = 0;
    for (j = 0 ; j < (i32)asic->regs.picHeight; j++)
    {
      rows++;
      totalrows++;
      if (rows == 64 - 8)
      {
        rows = 0;
        j += 8;
      }
    }
    if (asic->regs.SSEDivide256 == 0)
    {
      psnr = 999999.0;
    }
    else
      psnr = 10.0 * log10f((float)((256 << asic->regs.outputBitWidthLuma) - 1) * ((256 << asic->regs.outputBitWidthLuma) - 1) * asic->regs.picWidth * totalrows / (float)(asic->regs.SSEDivide256 * ((256 << asic->regs.outputBitWidthLuma) << asic->regs.outputBitWidthLuma)));
#ifdef TEST_DATA
    //if (asic->regs.asicHwId > 0x48320101)
      //printf("partial psnr=%.4f\n", psnr);
#endif
#ifdef CTBRC_STRENGTH
    hevc_instance->rateControl.rcPicComplexity = asic->regs.picComplexity;

#endif

    stat = HevcAfterPicRc(&hevc_instance->rateControl, 0, hevc_instance->stream.byteCnt, asic->regs.sumOfQP,asic->regs.sumOfQPNumber);

    /* After HRD overflow discard the coded frame and go back old time,
        * just like not coded frame. But if only one reference frame
        * buffer is in use we can't discard the frame unless the next frame
        * is coded as intra. */
    if ((stat == HEVCRC_OVERFLOW))
    {
      hevc_instance->stream.byteCnt = 0;

#if 1
      pEncOut->numNalus = 0;
      pEncOut->pNaluSizeBuf[0] = 0;
#endif
      //queue_put(&c->picture, (struct node *)pic);
      APITRACE("HEVCEncStrmEncode: OK, Frame discarded (HRD overflow)");
      //return HEVCENC_FRAME_READY;
    }
    else
      hevc_instance->rateControl.sei.activated_sps = 1;
  }

  hevc_instance->frameCnt++;  // update frame count after one frame is ready.
  if (hevc_instance->gdrEnabled == 1)
  {
    if (hevc_instance->gdrFirstIntraFrame != 0)
    {
      hevc_instance->gdrFirstIntraFrame--;
    }
    if (hevc_instance->gdrStart)
      hevc_instance->gdrCount++;

    if (hevc_instance->gdrCount == (hevc_instance->gdrDuration *(1 + (i32)hevc_instance->interlaced)))
    {
      hevc_instance->gdrStart--;
      hevc_instance->gdrCount = 0;
      asic->regs.rcRoiEnable = 0x00;
    }
  }

  HEVCEncDnfUpdate(hevc_instance);

  hevc_instance->encStatus = HEVCENCSTAT_START_FRAME;
  ret = HEVCENC_FRAME_READY;

  APITRACE("HEVCEncStrmEncode: OK");

out:
  /* Put picture back to store */
  queue_put(&c->picture, (struct node *)pic);
  pEncOut->streamSize = hevc_instance->stream.byteCnt;
  return ret;
}

/*------------------------------------------------------------------------------

    Function name : HEVCEncStrmEnd
    Description   : Ends a stream
    Return type   : HEVCEncRet
    Argument      : inst - encoder instance
    Argument      : pEncIn - user provided input parameters
                    pEncOut - place where output info is returned
------------------------------------------------------------------------------*/
HEVCEncRet HEVCEncStrmEnd(HEVCEncInst inst, const HEVCEncIn *pEncIn,
                          HEVCEncOut *pEncOut)
{

  /* Status == INIT   Stream ended, next stream can be started */
  struct hevc_instance *hevc_instance = (struct hevc_instance *)inst;

  APITRACE("HEVCEncStrmEnd#");
  APITRACEPARAM_X("busLuma", pEncIn->busLuma);
  APITRACEPARAM_X("busChromaU", pEncIn->busChromaU);
  APITRACEPARAM_X("busChromaV", pEncIn->busChromaV);
  APITRACEPARAM("timeIncrement", pEncIn->timeIncrement);
  APITRACEPARAM_X("pOutBuf", pEncIn->pOutBuf);
  APITRACEPARAM_X("busOutBuf", pEncIn->busOutBuf);
  APITRACEPARAM("outBufSize", pEncIn->outBufSize);
  APITRACEPARAM("codingType", pEncIn->codingType);
  APITRACEPARAM("poc", pEncIn->poc);
  APITRACEPARAM("gopSize", pEncIn->gopSize);
  APITRACEPARAM("gopPicIdx", pEncIn->gopPicIdx);
  APITRACEPARAM_X("roiMapDeltaQpAddr", pEncIn->roiMapDeltaQpAddr);


  /* Check for illegal inputs */
  if ((hevc_instance == NULL) || (pEncIn == NULL) || (pEncOut == NULL))
  {
    APITRACE("HEVCEncStrmEnd: ERROR Null argument");
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (hevc_instance->inst != hevc_instance)
  {
    APITRACE("HEVCEncStrmEnd: ERROR Invalid instance");
    return HEVCENC_INSTANCE_ERROR;
  }

  /* Check status, this also makes sure that the instance is valid */
  if ((hevc_instance->encStatus != HEVCENCSTAT_START_FRAME) &&
      (hevc_instance->encStatus != HEVCENCSTAT_START_STREAM))
  {
    APITRACE("HEVCEncStrmEnd: ERROR Invalid status");
    return HEVCENC_INVALID_STATUS;
  }

  hevc_instance->stream.stream = (u8 *)pEncIn->pOutBuf;
  hevc_instance->stream.stream_bus = pEncIn->busOutBuf;
  hevc_instance->stream.size = pEncIn->outBufSize;
  hevc_instance->stream.cnt = &hevc_instance->stream.byteCnt;

  pEncOut->streamSize = 0;
  hevc_instance->stream.byteCnt = 0;

  /* Set pointer to the beginning of NAL unit size buffer */
  pEncOut->pNaluSizeBuf = (u32 *) hevc_instance->asic.sizeTbl.virtualAddress;
  pEncOut->numNalus = 0;

  /* Clear the NAL unit size table */
  if (pEncOut->pNaluSizeBuf != NULL)
    pEncOut->pNaluSizeBuf[0] = 0;



  /* Write end-of-stream code */

  endofsequence(&hevc_instance->stream, hevc_instance->asic.regs.hevcStrmMode);
  pEncOut->streamSize = hevc_instance->stream.byteCnt;
  pEncOut->numNalus = 1;
  pEncOut->pNaluSizeBuf[0] = hevc_instance->stream.byteCnt;
  pEncOut->pNaluSizeBuf[1] = 0;

  /* Status == INIT   Stream ended, next stream can be started */

  hevc_instance->encStatus = HEVCENCSTAT_INIT;
  APITRACE("HEVCEncStrmEnd: OK");
  return HEVCENC_OK;
}

/*------------------------------------------------------------------------------
    Function name : HEVCEncGetBitsPerPixel
    Description   : Returns the amount of bits per pixel for given format.
    Return type   : u32
------------------------------------------------------------------------------*/
u32 HEVCEncGetBitsPerPixel(HEVCEncPictureType type)
{
  switch (type)
  {
    case HEVCENC_YUV420_PLANAR:
    case HEVCENC_YUV420_SEMIPLANAR:
    case HEVCENC_YUV420_SEMIPLANAR_VU:
      return 12;
    case HEVCENC_YUV422_INTERLEAVED_YUYV:
    case HEVCENC_YUV422_INTERLEAVED_UYVY:
    case HEVCENC_RGB565:
    case HEVCENC_BGR565:
    case HEVCENC_RGB555:
    case HEVCENC_BGR555:
    case HEVCENC_RGB444:
    case HEVCENC_BGR444:
      return 16;
    case HEVCENC_RGB888:
    case HEVCENC_BGR888:
    case HEVCENC_RGB101010:
    case HEVCENC_BGR101010:
      return 32;
    case HEVCENC_YUV420_PLANAR_10BIT_I010:
    case HEVCENC_YUV420_PLANAR_10BIT_P010:
      return 24;
    case HEVCENC_YUV420_PLANAR_10BIT_PACKED_PLANAR:
      return 15;
    case HEVCENC_YUV420_10BIT_PACKED_Y0L2:
      return 16;
    default:
      return 0;
  }
}

/*------------------------------------------------------------------------------
    Function name : HEVCEncSetTestId
    Description   : Sets the encoder configuration according to a test vector
    Return type   : HEVCEncRet
    Argument      : inst - encoder instance
    Argument      : testId - test vector ID
------------------------------------------------------------------------------*/
HEVCEncRet HEVCEncSetTestId(HEVCEncInst inst, u32 testId)
{
  struct hevc_instance *pEncInst = (struct hevc_instance *) inst;
  (void) pEncInst;
  (void) testId;

  APITRACE("HEVCEncSetTestId#");

#ifdef INTERNAL_TEST
  pEncInst->testId = testId;

  APITRACE("HEVCEncSetTestId# OK");
  return HEVCENC_OK;
#else
  /* Software compiled without testing support, return error always */
  APITRACE("HEVCEncSetTestId# ERROR, testing disabled at compile time");
  return HEVCENC_ERROR;
#endif
}

/*------------------------------------------------------------------------------
    Function name : HEVCEncGetMbInfo
    Description   : Set the motionVectors field of HEVCEncOut structure to
                    point macroblock mbNum
    Return type   : HEVCEncRet
    Argument      : inst - encoder instance
    Argument      : mbNum - macroblock number
------------------------------------------------------------------------------*/
HEVCEncRet HEVCEncGetCTUInfo(HEVCEncInst inst)
{
  (void)inst;
  return HEVCENC_OK;
}

/*------------------------------------------------------------------------------
  out_of_memory
------------------------------------------------------------------------------*/
i32 out_of_memory(struct hevc_buffer *n, u32 size)
{
  return (size > n->cnt);
}

/*------------------------------------------------------------------------------
  init_buffer
------------------------------------------------------------------------------*/
i32 init_buffer(struct hevc_buffer *hevc_buffer,
                struct hevc_instance *hevc_instance)
{
  if (!hevc_instance->temp_buffer) return NOK;
  hevc_buffer->next   = NULL;
  hevc_buffer->buffer = hevc_instance->temp_buffer;
  hevc_buffer->cnt    = hevc_instance->temp_size;
  hevc_buffer->busaddr = hevc_instance->temp_bufferBusAddress;
  return OK;
}

/*------------------------------------------------------------------------------
  get_buffer
------------------------------------------------------------------------------*/
i32 get_buffer(struct buffer *buffer, struct hevc_buffer *source, i32 size,
               i32 reset)
{
  struct hevc_buffer *hevc_buffer;

  if (size < 0) return NOK;
  memset(buffer, 0, sizeof(struct buffer));

  /* New hevc_buffer node */
  if (out_of_memory(source, sizeof(struct hevc_buffer))) return NOK;
  hevc_buffer = (struct hevc_buffer *)source->buffer;
  if (reset) memset(hevc_buffer, 0, sizeof(struct hevc_buffer));
  source->buffer += sizeof(struct hevc_buffer);
  source->busaddr += sizeof(struct hevc_buffer);
  source->cnt    -= sizeof(struct hevc_buffer);

  /* Connect previous hevc_buffer node */
  if (source->next) source->next->next = hevc_buffer;
  source->next = hevc_buffer;

  /* Map internal buffer and user space hevc_buffer, NOTE that my chunk
   * size is sizeof(struct hevc_buffer) */
  size = (size / sizeof(struct hevc_buffer)) * sizeof(struct hevc_buffer);
  if (out_of_memory(source, size)) return NOK;
  hevc_buffer->buffer = source->buffer;
  hevc_buffer->busaddr = source->busaddr;
  buffer->stream      = source->buffer;
  buffer->stream_bus  = source->busaddr;
  buffer->size        = size;
  buffer->cnt         = &hevc_buffer->cnt;
  source->buffer     += size;
  source->busaddr    += size;
  source->cnt        -= size;

  return OK;
}

/*------------------------------------------------------------------------------
  hevc_set_ref_pic_set
------------------------------------------------------------------------------*/
i32 hevc_set_ref_pic_set(struct hevc_instance *hevc_instance)
{
  struct container *c;
  struct hevc_buffer source;
  struct rps *r;

  if (!(c = get_container(hevc_instance))) return NOK;

  /* Create new reference picture set */
  if (!(r = (struct rps *)create_parameter_set(RPS))) return NOK;

  /* We will read hevc_instance->buffer */
  if (init_buffer(&source, hevc_instance)) goto out;
  if (get_buffer(&r->ps.b, &source, PRM_SET_BUFF_SIZE, false)) goto out;

  /* Initialize parameter set id */
  r->ps.id = hevc_instance->rps_id;
  r->sps_id = hevc_instance->sps_id;

  if (set_reference_pic_set(r)) goto out;

  /* Replace old reference picture set with new one */
  remove_parameter_set(c, RPS, hevc_instance->rps_id);
  queue_put(&c->parameter_set, (struct node *)r);

  return OK;

out:
  free_parameter_set((struct ps *)r);
  return NOK;
}

/*------------------------------------------------------------------------------
  hevc_ref_pic_sets
------------------------------------------------------------------------------*/
i32 hevc_ref_pic_sets(struct hevc_instance *hevc_instance, HEVCGopConfig *gopCfg)
{
  struct hevc_buffer *hevc_buffer;
  i32 *poc;
  i32 rps_id = 0;

  /* Initialize tb->instance->buffer so that we can cast it to hevc_buffer
   * by calling hevc_set_parameter(), see api.c TODO...*/
  hevc_instance->rps_id = -1;
  hevc_set_ref_pic_set(hevc_instance);
  hevc_buffer = (struct hevc_buffer *)hevc_instance->temp_buffer;
  poc = (i32 *)hevc_buffer->buffer;

  //RPS based on user GOP configuration
  int i;
  rps_id = 0;
  for (i = 0; i < gopCfg->size; i ++)
  {
    HEVCGopPicConfig *cfg = &(gopCfg->pGopPicCfg[i]);
    u32 iRefPic, idx = 0;
    for (iRefPic = 0; iRefPic < cfg->numRefPics; iRefPic ++)
    {
      poc[idx ++] = cfg->refPics[iRefPic].ref_pic;
      poc[idx ++] = cfg->refPics[iRefPic].used_by_cur;
    }
    poc[idx] = 0; //end
    hevc_instance->rps_id = rps_id;
    if (hevc_set_ref_pic_set(hevc_instance))
    {
      Error(2, ERR, "hevc_set_ref_pic_set() fails");
      return NOK;
    }
    rps_id++;
  }
  //add some special RPS configuration
  if (hevc_instance->interlaced)
  {
    //interlaced with gopSize = 1
    poc[0] = -1;
    poc[1] = 0;
    poc[2] = -2;
    poc[3] = 1;
    poc[4] = 0;
    hevc_instance->rps_id = rps_id;
    if (hevc_set_ref_pic_set(hevc_instance))
    {
      Error(2, ERR, "hevc_set_ref_pic_set() fails");
      return NOK;
    }
    rps_id++;
  }

  //P frame for sequence starting
  poc[0] = -1;
  poc[1] = 1;
  poc[2] = 0;
  hevc_instance->rps_id = rps_id;
  if (hevc_set_ref_pic_set(hevc_instance))
  {
    Error(2, ERR, "hevc_set_ref_pic_set() fails");
    return NOK;
  }
  rps_id++;

  //Intra
  poc[0] = 0;
  hevc_instance->rps_id = rps_id;
  if (hevc_set_ref_pic_set(hevc_instance))
  {
    Error(2, ERR, "hevc_set_ref_pic_set() fails");
    return NOK;
  }
  return OK;
}

/*------------------------------------------------------------------------------
  hevc_get_ref_pic_set
------------------------------------------------------------------------------*/
i32 hevc_get_ref_pic_set(struct hevc_instance *hevc_instance)
{
  struct container *c;
  struct hevc_buffer source;
  struct ps *p;

  if (!(c = get_container(hevc_instance))) return NOK;
  if (!(p = get_parameter_set(c, RPS, hevc_instance->rps_id))) return NOK;
  if (init_buffer(&source, hevc_instance)) return NOK;
  if (get_buffer(&p->b, &source, PRM_SET_BUFF_SIZE, true)) return NOK;
  if (get_reference_pic_set((struct rps *)p)) return NOK;

  return OK;
}

/*------------------------------------------------------------------------------

    hevcCheckCfg

    Function checks that the configuration is valid.

    Input   pEncCfg Pointer to configuration structure.

    Return  ENCHW_OK      The configuration is valid.
            ENCHW_NOK     Some of the parameters in configuration are not valid.

------------------------------------------------------------------------------*/
bool_e hevcCheckCfg(const HEVCEncConfig *pEncCfg)
{
  i32 height = pEncCfg->height;

  ASSERT(pEncCfg);


  if ((pEncCfg->streamType != HEVCENC_BYTE_STREAM) &&
      (pEncCfg->streamType != HEVCENC_NAL_UNIT_STREAM))
  {
    APITRACE("hevcCheckCfg: Invalid stream type");
    return ENCHW_NOK;
  }

  /* Encoded image width limits, multiple of 2 */
  if (pEncCfg->width < HEVCENC_MIN_ENC_WIDTH ||
      pEncCfg->width > HEVCENC_MAX_ENC_WIDTH || (pEncCfg->width & 0x1) != 0)
  {
    APITRACE("hevcCheckCfg: Invalid width");
    return ENCHW_NOK;
  }

  /* Encoded image height limits, multiple of 2 */
  if (height < HEVCENC_MIN_ENC_HEIGHT ||
      height > HEVCENC_MAX_ENC_HEIGHT || (height & 0x1) != 0)
  {
    APITRACE("hevcCheckCfg: Invalid height");
    return ENCHW_NOK;
  }

  /* Encoded image height*width limits */
  if (pEncCfg->width * height > HEVCENC_MAX_ENC_WIDTH * HEVCENC_MAX_ENC_HEIGHT)
  {
    APITRACE("hevcCheckCfg: Invalid area");
    return ENCHW_NOK;
  }

  /* total macroblocks per picture limit */
  if (((height + 63) / 64) *((pEncCfg->width + 63) / 64) >
      HEVCENC_MAX_CTUS_PER_PIC)
  {
    APITRACE("hevcCheckCfg: Invalid max resolution");
    return ENCHW_NOK;
  }

  /* Check frame rate */
  if (pEncCfg->frameRateNum < 1 || pEncCfg->frameRateNum > ((1 << 20) - 1))
  {
    APITRACE("hevcCheckCfg: Invalid frameRateNum");
    return ENCHW_NOK;
  }

  if (pEncCfg->frameRateDenom < 1)
  {
    APITRACE("hevcCheckCfg: Invalid frameRateDenom");
    return ENCHW_NOK;
  }

  /* special allowal of 1000/1001, 0.99 fps by customer request */
  if (pEncCfg->frameRateDenom > pEncCfg->frameRateNum &&
      !(pEncCfg->frameRateDenom == 1001 && pEncCfg->frameRateNum == 1000))
  {
    APITRACE("hevcCheckCfg: Invalid frameRate");
    return ENCHW_NOK;
  }

  /* check profile */
  #if 0  //add by bright
  {
    u32 asicID = EWLReadAsicID();
    if ((pEncCfg->profile > HEVCENC_MAIN_10_PROFILE) ||
        ((i32)pEncCfg->profile < HEVCENC_MAIN_PROFILE) ||
        ((asicID < 0x48320401) && (pEncCfg->profile == HEVCENC_MAIN_10_PROFILE)))
    {
      APITRACE("hevcCheckCfg: Invalid profile");
      return ENCHW_NOK;
    }
  }
  #endif
  /* check bit depth for main8 and main still profile */
  if ((pEncCfg->profile < HEVCENC_MAIN_10_PROFILE)
      && (pEncCfg->bitDepthLuma != 8 || pEncCfg->bitDepthChroma != 8))
  {
    APITRACE("hevcCheckCfg: Invalid bit depth");
    return ENCHW_NOK;
  }

  /* check bit depth for main10 profile */
  if ((pEncCfg->profile == HEVCENC_MAIN_10_PROFILE)
      && (pEncCfg->bitDepthLuma < 8  || pEncCfg->bitDepthLuma > 10
          || pEncCfg->bitDepthChroma < 8 || pEncCfg->bitDepthChroma > 10))
  {
    APITRACE("hevcCheckCfg: Invalid bit depth for main10 profile");
    return ENCHW_NOK;
  }


  /* check level */
  if ((pEncCfg->level > HEVCENC_MAX_LEVEL) ||
      (pEncCfg->level < HEVCENC_LEVEL_1))
  {
    APITRACE("hevcCheckCfg: Invalid level");
    return ENCHW_NOK;
  }

  if (pEncCfg->refFrameAmount > HEVCENC_MAX_REF_FRAMES)
  {
    APITRACE("hevcCheckCfg: Invalid refFrameAmount");
    return ENCHW_NOK;
  }


  /* check HW limitations */
  {
    EWLHwConfig_t cfg = EWLReadAsicConfig();
    /* is HEVC encoding supported */
    if (cfg.hevcEnabled == EWL_HW_CONFIG_NOT_SUPPORTED)
    {
      APITRACE("hevcCheckCfg: Invalid format, hevc not supported by HW");
      return ENCHW_NOK;
    }

    /* max width supported */
    if (cfg.maxEncodedWidth < pEncCfg->width)
    {
      APITRACE("hevcCheckCfg: Invalid width, not supported by HW");
      return ENCHW_NOK;
    }
  }

  return ENCHW_OK;
}
static i32 getlevelIdx(i32 level)
{
  switch (level)
  {
    case HEVCENC_LEVEL_1:
      return 0;
    case HEVCENC_LEVEL_2:
      return 1;
    case HEVCENC_LEVEL_2_1:
      return 2;
    case HEVCENC_LEVEL_3:
      return 3;
    case HEVCENC_LEVEL_3_1:
      return 4;
    case HEVCENC_LEVEL_4:
      return 5;
    case HEVCENC_LEVEL_4_1:
      return 6;
    case HEVCENC_LEVEL_5:
      return 7;
    case HEVCENC_LEVEL_5_1:
      return 8;
    case HEVCENC_LEVEL_5_2:
      return 9;
    case HEVCENC_LEVEL_6:
      return 10;
    case HEVCENC_LEVEL_6_1:
      return 11;
    case HEVCENC_LEVEL_6_2:
      return 12;
    default:
      ASSERT(0);
  }
  return 0;
}

/*------------------------------------------------------------------------------

    SetParameter

    Set all parameters in instance to valid values depending on user config.

------------------------------------------------------------------------------*/
bool_e SetParameter(struct hevc_instance *inst, const HEVCEncConfig   *pEncCfg)
{
  i32 width, height, bps;
  EWLHwConfig_t cfg;
  ASSERT(inst);

  //inst->width = pEncCfg->width;
  //inst->height = pEncCfg->height;

  /* Internal images, next macroblock boundary */
  width = /*pEncCfg->ctb_size*/64 * ((inst->width + /*pEncCfg->ctb_size*/64 - 1) / 64/*pEncCfg->ctb_size*/);
  height = /*pEncCfg->ctb_size*/64 * ((inst->height + /*pEncCfg->ctb_size*/64 - 1) / 64/*pEncCfg->ctb_size*/);

  /* stream type */
  if (pEncCfg->streamType == HEVCENC_BYTE_STREAM)
  {
    inst->asic.regs.hevcStrmMode = ASIC_HEVC_BYTE_STREAM;
    //        inst->rateControl.sei.byteStream = ENCHW_YES;
  }
  else
  {
    inst->asic.regs.hevcStrmMode = ASIC_HEVC_NAL_UNIT;
    //        inst->rateControl.sei.byteStream = ENCHW_NO;
  }

  /* ctb */
  inst->ctbPerRow = width / 64/*pEncCfg->ctb_size*/;
  inst->ctbPerCol = height / 64/*pEncCfg->ctb_size*/;
  inst->ctbPerFrame = inst->ctbPerRow * inst->ctbPerCol;


  /* Disable intra and ROI areas by default */
  inst->asic.regs.intraAreaTop = inst->asic.regs.intraAreaBottom = inst->ctbPerCol;
  inst->asic.regs.intraAreaLeft = inst->asic.regs.intraAreaRight = inst->ctbPerRow;
  inst->asic.regs.roi1Top = inst->asic.regs.roi1Bottom = inst->ctbPerCol;
  inst->asic.regs.roi1Left = inst->asic.regs.roi1Right = inst->ctbPerRow;
  inst->asic.regs.roi2Top = inst->asic.regs.roi2Bottom = inst->ctbPerCol;
  inst->asic.regs.roi2Left = inst->asic.regs.roi2Right = inst->ctbPerRow;

  /* Slice num */
  inst->asic.regs.sliceSize = 0;
  inst->asic.regs.sliceNum = 1;

  /* compressor */
  inst->asic.regs.recon_luma_compress = (pEncCfg->compressor & 1) ? 1 : 0;
  inst->asic.regs.recon_chroma_compress = (pEncCfg->compressor & 2) ? 1 : 0;

  /* Sequence parameter set */
  inst->level = pEncCfg->level;
  inst->levelIdx = getlevelIdx(inst->level);

  switch (pEncCfg->profile)
  {
    case 0:
      //main profile
      inst->profile = 1;
      break;
    case 1:
      //main still picture profile.
      inst->profile = 3;
      break;
    case 2:
      //main 10 profile.
      inst->profile = 2;
      break;
    default:
      break;
  }


  /* enforce maximum frame size in level */
  if ((u32)(inst->width * inst->height) > HEVCMaxFS[inst->levelIdx])
  {
    return ENCHW_NOK;
  }

  /* enforce Max luma sample rate in level */
  {
    u32 sample_rate =
      (pEncCfg->frameRateNum * inst->width * inst->height) /
      pEncCfg->frameRateDenom;

    if (sample_rate > HEVCMaxSBPS[inst->levelIdx])
    {
      return ENCHW_NOK;
    }
  }

  /* intra */
  inst->constrained_intra_pred_flag = 0;
  inst->strong_intra_smoothing_enabled_flag = pEncCfg->strongIntraSmoothing;


  inst->vps->max_dec_pic_buffering[0] = pEncCfg->refFrameAmount + 1;
  inst->sps->max_dec_pic_buffering[0] = pEncCfg->refFrameAmount + 1;


  inst->sps->bit_depth_luma_minus8 = pEncCfg->bitDepthLuma - 8;
  inst->sps->bit_depth_chroma_minus8 = pEncCfg->bitDepthChroma - 8;

  inst->vps->max_num_sub_layers = pEncCfg->maxTLayers;
  inst->vps->temporal_id_nesting_flag =1;
  for(int i = 0; i < inst->vps->max_num_sub_layers; i++)
  {
    inst->vps->max_dec_pic_buffering[i] =inst->vps->max_dec_pic_buffering[0];
  }

  inst->sps->max_num_sub_layers = pEncCfg->maxTLayers;
  inst->sps->temporal_id_nesting_flag =1;
  for(int i = 0; i < inst->sps->max_num_sub_layers; i++)
  {
    inst->sps->max_dec_pic_buffering[i] =inst->sps->max_dec_pic_buffering[0];
  }

  /* Rate control setup */

  /* Maximum bitrate for the specified level */
  bps = HEVCMaxBR[inst->levelIdx];

  {
    hevcRateControl_s *rc = &inst->rateControl;

    rc->outRateDenom = pEncCfg->frameRateDenom;
    rc->outRateNum = pEncCfg->frameRateNum;
    rc->picArea = ((inst->width + 7) & (~7)) * ((inst->height + 7) & (~7));
    rc->ctbPerPic = inst->ctbPerFrame;
    rc->ctbRows = inst->ctbPerCol;
    rc->ctbSize = 64;//pEncCfg->ctb_size;

    {
      hevcVirtualBuffer_s *vb = &rc->virtualBuffer;

      vb->bitRate = bps;
      vb->unitsInTic = pEncCfg->frameRateDenom;
      vb->timeScale = pEncCfg->frameRateNum * (inst->interlaced + 1);
      vb->bufferSize = HEVCMaxCPBS[inst->levelIdx];
    }

    rc->hrd = ENCHW_NO;
    rc->picRc = ENCHW_NO;
    rc->ctbRc = ENCHW_NO;
    rc->picSkip = ENCHW_NO;

    rc->qpHdr = HEVCENC_DEFAULT_QP << QP_FRACTIONAL_BITS;
    rc->qpMin = 0 << QP_FRACTIONAL_BITS;
    rc->qpMax = 51 << QP_FRACTIONAL_BITS;

    rc->frameCoded = ENCHW_YES;
    rc->sliceTypeCur = I_SLICE;
    rc->sliceTypePrev = P_SLICE;
    rc->gopLen = 150;

    /* Default initial value for intra QP delta */
    rc->intraQpDelta = -5 << QP_FRACTIONAL_BITS;
    rc->fixedIntraQp = 0 << QP_FRACTIONAL_BITS;
    rc->frameQpDelta = 0 << QP_FRACTIONAL_BITS;
    rc->gopPoc       = 0;
  }

  /* no SEI by default */
  inst->rateControl.sei.enabled = ENCHW_NO;

  /* Pre processing */
  inst->preProcess.lumWidth = pEncCfg->width;
  inst->preProcess.lumWidthSrc =
    HEVCGetAllowedWidth(pEncCfg->width, HEVCENC_YUV420_PLANAR);

  inst->preProcess.lumHeight = pEncCfg->height / ((inst->interlaced + 1));
  inst->preProcess.lumHeightSrc = pEncCfg->height;

  inst->preProcess.horOffsetSrc = 0;
  inst->preProcess.verOffsetSrc = 0;

  inst->preProcess.rotation = ROTATE_0;
  inst->preProcess.inputFormat = HEVCENC_YUV420_PLANAR;
  inst->preProcess.videoStab = 0;

  inst->preProcess.scaledWidth    = 0;
  inst->preProcess.scaledHeight   = 0;
  inst->preProcess.scaledOutput   = 0;
  inst->preProcess.interlacedFrame = inst->interlaced;
  inst->asic.scaledImage.busAddress = 0;
  inst->asic.scaledImage.virtualAddress = NULL;
  inst->asic.scaledImage.size = 0;
  inst->preProcess.adaptiveRoi = 0;
  inst->preProcess.adaptiveRoiColor  = 0;
  inst->preProcess.adaptiveRoiMotion = -5;

  inst->preProcess.colorConversionType = 0;
  EncSetColorConversion(&inst->preProcess, &inst->asic);

  return ENCHW_OK;
}

/*------------------------------------------------------------------------------

    Round the width to the next multiple of 8 or 16 depending on YUV type.

------------------------------------------------------------------------------*/
i32 HEVCGetAllowedWidth(i32 width, HEVCEncPictureType inputType)
{
  if (inputType == HEVCENC_YUV420_PLANAR)
  {
    /* Width must be multiple of 16 to make
     * chrominance row 64-bit aligned */
    return ((width + 15) / 16) * 16;
  }
  else
  {
    /* HEVCENC_YUV420_SEMIPLANAR */
    /* HEVCENC_YUV422_INTERLEAVED_YUYV */
    /* HEVCENC_YUV422_INTERLEAVED_UYVY */
    return ((width + 7) / 8) * 8;
  }
}

void IntraLamdaQp(int qp, u32 *lamda_sad, enum slice_type type, int poc, double dQPFactor, bool depth0)
{
  double recalQP = (double)qp;
  double lambda;

  // pre-compute lambda and QP values for all possible QP candidates
  {
    // compute lambda value
    //Int    NumberBFrames = 0;
    //double dLambda_scale = 1.0 - MAX( 0.0, MIN( 0.5, 0.05*(double)NumberBFrames ));
    Int    SHIFT_QP = 12;
    Int    g_bitDepth = 8;
    Int    bitdepth_luma_qp_scale = 6 * (g_bitDepth - 8);
    double qp_temp = (double) recalQP + bitdepth_luma_qp_scale - SHIFT_QP;

    // Case #1: I or P-slices (key-frame)
    //double dQPFactor = type == B_SLICE ? 0.68 : 0.8;//0.442;//0.576;//0.8;

    lambda = dQPFactor * pow(2.0, qp_temp / 3.0);
    //if (type == B_SLICE)
    if (!depth0)
      lambda *= CLIP3(2.0, 4.0, (qp - 12) / 6.0); //* 0.8/0.576;
    //clip
    //    if (((unsigned int)lambda) > ((1<<14)-1))
    //      lambda = (double)((1<<14)-1);
  }

  // store lambda
  lambda = sqrt(lambda);

  ASSERT(lambda < 255);

  *lamda_sad = (u32)(lambda * (1 << 6) + 0.5);
}

void InterLamdaQp(int qp, unsigned int *lamda_sse, unsigned int *lamda_sad, enum slice_type type, int poc, double dQPFactor, bool depth0)
{
  double recalQP = (double)qp;
  double lambda;

  // pre-compute lambda and QP values for all possible QP candidates
  {
    // compute lambda value
    //Int    NumberBFrames = 0;
    //double dLambda_scale = 1.0 - MAX( 0.0, MIN( 0.5, 0.05*(double)NumberBFrames ));
    Int    SHIFT_QP = 12;
    Int    g_bitDepth = 8;
    Int    bitdepth_luma_qp_scale = 6 * (g_bitDepth - 8);
    double qp_temp = (double) recalQP + bitdepth_luma_qp_scale - SHIFT_QP;

    // Case #1: I or P-slices (key-frame)
    //double dQPFactor = type == B_SLICE ? 0.68 : 0.8;//0.442;//0.576;//0.8;

    lambda = dQPFactor * pow(2.0, qp_temp / 3.0);
    //if (type == B_SLICE)
    if (!depth0)
      lambda *= CLIP3(2.0, 4.0, (qp - 12) / 6.0); //* 0.8/0.576;
    //clip
    if (((unsigned int)lambda) > ((1 << 14) - 1))
      lambda = (double)((1 << 14) - 1);
  }

  // store lambda
  double m_dLambda           = lambda;
  double m_sqrtLambda        = sqrt(m_dLambda);
  UInt m_uiLambdaMotionSAD = ((UInt)floor(65536.0 * m_sqrtLambda)) >> 16;

  *lamda_sse = (unsigned int)lambda;
  *lamda_sad = m_uiLambdaMotionSAD;
}

//Set LamdaTable for all qp
void LamdaTableQp(regValues_s *regs, int qp, enum slice_type type, int poc, double dQPFactor, bool depth0, bool ctbRc)
{
  int qpoffset, lamdaIdx;
#ifndef CTBRC_STRENGTH

  //setup LamdaTable dependent on ctbRc enable
  if (ctbRc)
  {
    for (qpoffset = -2, lamdaIdx = 14; qpoffset <= +2; qpoffset++, lamdaIdx++)
    {
      IntraLamdaQp(qp - qpoffset, regs->lambda_satd_ims + (lamdaIdx & 0xf), type, poc, dQPFactor, depth0);
      InterLamdaQp(qp - qpoffset, regs->lambda_sse_me + (lamdaIdx & 0xf),  regs->lambda_satd_me + (lamdaIdx & 0xf),    type, poc, dQPFactor, depth0);
    }
  }
  else
  {
    //for ROI-window, ROI-Map
    for (qpoffset = 0, lamdaIdx = 0; qpoffset <= 15; qpoffset++, lamdaIdx++)
    {
      IntraLamdaQp(qp - qpoffset, regs->lambda_satd_ims + lamdaIdx, type, poc, dQPFactor, depth0);
      InterLamdaQp(qp - qpoffset, regs->lambda_sse_me + lamdaIdx,   regs->lambda_satd_me + lamdaIdx,    type, poc, dQPFactor, depth0);
    }
  }
#else

	regs->offsetSliceQp = 0;
	if( qp >= 35)
	{
		regs->offsetSliceQp = 35 - qp;
	}
	if(qp <= 15)
	{
		regs->offsetSliceQp = 15 - qp;
	}

  for (qpoffset = -16, lamdaIdx = 16  /*- regs->offsetSliceQp*/; qpoffset <= +15; qpoffset++, lamdaIdx++)
  {
    IntraLamdaQp(CLIP3(0,51,qp + regs->offsetSliceQp - qpoffset), regs->lambda_satd_ims + (lamdaIdx & 0x1f), type, poc, dQPFactor, depth0);
    InterLamdaQp(CLIP3(0,51,qp  + regs->offsetSliceQp - qpoffset), regs->lambda_sse_me + (lamdaIdx & 0x1f),  regs->lambda_satd_me + (lamdaIdx & 0x1f),    type, poc, dQPFactor, depth0);
  }

#endif
}

void FillIntraFactor(regValues_s *regs)
{
#if 0
  const double weight = 0.57 / 0.8;
  const double size_coeff[4] =
  {
    30.0,
    30.0,
    42.0,
    42.0
  };
  const u32 mode_coeff[3] =
  {
    0x6276 + 1 * 0x8000,
    0x6276 + 2 * 0x8000,
    0x6276 + 5 * 0x8000,
  };

  regs->intra_size_factor[0] = double_to_u6q4(sqrt(weight) * (1 / 0.8) * size_coeff[0]);
  regs->intra_size_factor[1] = double_to_u6q4(sqrt(weight) * (1 / 0.8) * size_coeff[1]);
  regs->intra_size_factor[2] = double_to_u6q4(sqrt(weight) * (1 / 0.8) * size_coeff[2]);
  regs->intra_size_factor[3] = double_to_u6q4(sqrt(weight) * (1 / 0.8) * size_coeff[3]);

  regs->intra_mode_factor[0] = double_to_u6q4(sqrt(weight) * (double) mode_coeff[0] / 32768.0);
  regs->intra_mode_factor[1] = double_to_u6q4(sqrt(weight) * (double) mode_coeff[1] / 32768.0);
  regs->intra_mode_factor[2] = double_to_u6q4(sqrt(weight) * (double) mode_coeff[2] / 32768.0);
#else
  regs->intra_size_factor[0] = 506;
  regs->intra_size_factor[1] = 506;
  regs->intra_size_factor[2] = 709;
  regs->intra_size_factor[3] = 709;

  regs->intra_mode_factor[0] = 24;
  regs->intra_mode_factor[1] = 37;
  regs->intra_mode_factor[2] = 78;
#endif

  return;
}

/*------------------------------------------------------------------------------

    Set QP related parameters at the beginning of a new frame.

------------------------------------------------------------------------------*/
static void HEVCEncSetQuantParameters(asicData_s *asic, double qp_factor,
                                      enum slice_type type, bool is_depth0,
                                      true_e enable_ctu_rc)
{
  /* Intra Penalty for TU 4x4 vs. 8x8 */
  static unsigned short HEVCIntraPenaltyTu4[52] =
  {
    7,    7,    8,   10,   11,   12,   14,   15,   17,   20,
    22,   25,   28,   31,   35,   40,   44,   50,   56,   63,
    71,   80,   89,  100,  113,  127,  142,  160,  179,  201,
    226,  254,  285,  320,  359,  403,  452,  508,  570,  640,
    719,  807,  905, 1016, 1141, 1281, 1438, 1614, 1811, 2033,
    2282, 2562
  };//max*3=13bit

  /* Intra Penalty for TU 8x8 vs. 16x16 */
  static unsigned short HEVCIntraPenaltyTu8[52] =
  {
    7,    7,    8,   10,   11,   12,   14,   15,   17,   20,
    22,   25,   28,   31,   35,   40,   44,   50,   56,   63,
    71,   80,   89,  100,  113,  127,  142,  160,  179,  201,
    226,  254,  285,  320,  359,  403,  452,  508,  570,  640,
    719,  807,  905, 1016, 1141, 1281, 1438, 1614, 1811, 2033,
    2282, 2562
  };//max*3=13bit

  /* Intra Penalty for TU 16x16 vs. 32x32 */
  static unsigned short HEVCIntraPenaltyTu16[52] =
  {
    9,   11,   12,   14,   15,   17,   19,   22,   24,   28,
    31,   35,   39,   44,   49,   56,   62,   70,   79,   88,
    99,  112,  125,  141,  158,  177,  199,  224,  251,  282,
    317,  355,  399,  448,  503,  564,  634,  711,  799,  896,
    1006, 1129, 1268, 1423, 1598, 1793, 2013, 2259, 2536, 2847,
    3196, 3587
  };//max*3=14bit

  /* Intra Penalty for TU 32x32 vs. 64x64 */
  static unsigned short HEVCIntraPenaltyTu32[52] =
  {
    9,   11,   12,   14,   15,   17,   19,   22,   24,   28,
    31,   35,   39,   44,   49,   56,   62,   70,   79,   88,
    99,  112,  125,  141,  158,  177,  199,  224,  251,  282,
    317,  355,  399,  448,  503,  564,  634,  711,  799,  896,
    1006, 1129, 1268, 1423, 1598, 1793, 2013, 2259, 2536, 2847,
    3196, 3587
  };//max*3=14bit

  /* Intra Penalty for Mode a */
  static unsigned short HEVCIntraPenaltyModeA[52] =
  {
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    1,    1,    1,    1,    1,    1,    2,    2,    2,    2,
    3,    3,    4,    4,    5,    5,    6,    7,    8,    9,
    10,   11,   13,   15,   16,   19,   21,   23,   26,   30,
    33,   38,   42,   47,   53,   60,   67,   76,   85,   95,
    107,  120
  };//max*3=9bit

  /* Intra Penalty for Mode b */
  static unsigned short HEVCIntraPenaltyModeB[52] =
  {
    0,    0,    0,    0,    0,    0,    1,    1,    1,    1,
    1,    1,    2,    2,    2,    2,    3,    3,    4,    4,
    5,    5,    6,    7,    8,    9,   10,   11,   13,   14,
    16,   18,   21,   23,   26,   29,   33,   37,   42,   47,
    53,   59,   66,   75,   84,   94,  106,  119,  133,  150,
    168,  189
  };////max*3=10bit

  /* Intra Penalty for Mode c */
  static unsigned short HEVCIntraPenaltyModeC[52] =
  {
    1,    1,    1,    1,    1,    1,    2,    2,    2,    3,
    3,    3,    4,    4,    5,    6,    6,    7,    8,    9,
    10,   12,   13,   15,   17,   19,   21,   24,   27,   31,
    34,   39,   43,   49,   55,   62,   69,   78,   87,   98,
    110,  124,  139,  156,  175,  197,  221,  248,  278,  312,
    351,  394
  };//max*3=11bit

  switch (asic->regs.asicHwId)
  {
    case 0x48320101: /* H2V1 rev01 */
      asic->regs.intraPenaltyPic4x4  = HEVCIntraPenaltyTu4[asic->regs.qp];
      asic->regs.intraPenaltyPic8x8  = HEVCIntraPenaltyTu8[asic->regs.qp];
      asic->regs.intraPenaltyPic16x16 = HEVCIntraPenaltyTu16[asic->regs.qp];
      asic->regs.intraPenaltyPic32x32 = HEVCIntraPenaltyTu32[asic->regs.qp];

      asic->regs.intraMPMPenaltyPic1 = HEVCIntraPenaltyModeA[asic->regs.qp];
      asic->regs.intraMPMPenaltyPic2 = HEVCIntraPenaltyModeB[asic->regs.qp];
      asic->regs.intraMPMPenaltyPic3 = HEVCIntraPenaltyModeC[asic->regs.qp];

      u32 roi1_qp = CLIP3(0, 51, ((int)asic->regs.qp - (int)asic->regs.roi1DeltaQp));
      asic->regs.intraPenaltyRoi14x4 = HEVCIntraPenaltyTu4[roi1_qp];
      asic->regs.intraPenaltyRoi18x8 = HEVCIntraPenaltyTu8[roi1_qp];
      asic->regs.intraPenaltyRoi116x16 = HEVCIntraPenaltyTu16[roi1_qp];
      asic->regs.intraPenaltyRoi132x32 = HEVCIntraPenaltyTu32[roi1_qp];

      asic->regs.intraMPMPenaltyRoi11 = HEVCIntraPenaltyModeA[roi1_qp];
      asic->regs.intraMPMPenaltyRoi12 = HEVCIntraPenaltyModeB[roi1_qp];
      asic->regs.intraMPMPenaltyRoi13 = HEVCIntraPenaltyModeC[roi1_qp];

      u32 roi2_qp = CLIP3(0, 51, ((int)asic->regs.qp - (int)asic->regs.roi2DeltaQp));
      asic->regs.intraPenaltyRoi24x4 = HEVCIntraPenaltyTu4[roi2_qp];
      asic->regs.intraPenaltyRoi28x8 = HEVCIntraPenaltyTu8[roi2_qp];
      asic->regs.intraPenaltyRoi216x16 = HEVCIntraPenaltyTu16[roi2_qp];
      asic->regs.intraPenaltyRoi232x32 = HEVCIntraPenaltyTu32[roi2_qp];

      asic->regs.intraMPMPenaltyRoi21 = HEVCIntraPenaltyModeA[roi2_qp]; //need to change
      asic->regs.intraMPMPenaltyRoi22 = HEVCIntraPenaltyModeB[roi2_qp]; //need to change
      asic->regs.intraMPMPenaltyRoi23 = HEVCIntraPenaltyModeC[roi2_qp];  //need to change

      /*inter prediction parameters*/
      InterLamdaQp(asic->regs.qp, &asic->regs.lamda_motion_sse, &asic->regs.lambda_motionSAD,
                   type, asic->regs.poc, qp_factor, is_depth0);
      InterLamdaQp(asic->regs.qp - asic->regs.roi1DeltaQp, &asic->regs.lamda_motion_sse_roi1,
                   &asic->regs.lambda_motionSAD_ROI1, type, asic->regs.poc,
                   qp_factor, is_depth0);
      InterLamdaQp(asic->regs.qp - asic->regs.roi2DeltaQp, &asic->regs.lamda_motion_sse_roi2,
                   &asic->regs.lambda_motionSAD_ROI2, type, asic->regs.poc,
                   qp_factor, is_depth0);

      asic->regs.lamda_SAO_luma = asic->regs.lamda_motion_sse;
      asic->regs.lamda_SAO_chroma = 0.75 * asic->regs.lamda_motion_sse;

      break;
    case 0x48320201: /* H2V2 rev01 */
    case 0x48320301: /* H2V3 rev01 */
      LamdaTableQp(&asic->regs, asic->regs.qp, type, asic->regs.poc,
                   qp_factor, is_depth0, enable_ctu_rc);
      FillIntraFactor(&asic->regs);
      asic->regs.lamda_SAO_luma = asic->regs.lambda_sse_me[0];
      asic->regs.lamda_SAO_chroma = 0.75 * asic->regs.lambda_sse_me[0];
      break;
    case 0x48320401: /* H2V4 rev01 */
      LamdaTableQp(&asic->regs, asic->regs.qp, type, asic->regs.poc,
                   qp_factor, is_depth0, enable_ctu_rc);
      FillIntraFactor(&asic->regs);
      asic->regs.lamda_SAO_luma = asic->regs.lambda_sse_me[0];
      asic->regs.lamda_SAO_chroma = 0.75 * asic->regs.lambda_sse_me[0];
      break;
    default:
      ASSERT(0);
  }
}

/*------------------------------------------------------------------------------

    Set encoding parameters at the beginning of a new frame.

------------------------------------------------------------------------------*/
void HevcSetNewFrame(HEVCEncInst inst)
{
  struct hevc_instance *hevc_instance = (struct hevc_instance *)inst;
  asicData_s *asic = &hevc_instance->asic;
  regValues_s *regs = &hevc_instance->asic.regs;
  i32 qpHdr, qpMin, qpMax;
  i32 qp[4];
  i32 s, i, aroiDeltaQ = 0, tmp;

  regs->outputStrmSize -= hevc_instance->stream.byteCnt;
  //regs->outputStrmSize /= 8;  /* 64-bit addresses */
  //regs->outputStrmSize &= (~0x07);    /* 8 multiple size */

  /* 64-bit aligned stream base address */
  regs->outputStrmBase += hevc_instance->stream.byteCnt;


  /* Enable slice ready interrupts if defined by config and slices in use */
  regs->sliceReadyInterrupt =
    ENCH2_SLICE_READY_INTERRUPT & (regs->sliceNum > 1);



  /* HW base address for NAL unit sizes is affected by start offset
   * and SW created NALUs. */
  regs->sizeTblBase = asic->sizeTbl.busAddress + hevc_instance->numNalus * 4;

  /* HW Base must be 64-bit aligned */
  //ASSERT(regs->sizeTblBase%8 == 0);


}

/* DeNoise Filter */
static void HEVCEncDnfInit(struct hevc_instance *hevc_instance)
{
  hevc_instance->uiNoiseReductionEnable = 0;

#if USE_TOP_CTRL_DENOISE
  hevc_instance->iNoiseL = 10 << FIX_POINT_BIT_WIDTH;
  hevc_instance->iSigmaCur = hevc_instance->iFirstFrameSigma = 11 << FIX_POINT_BIT_WIDTH;
  //printf("init seq noiseL = %d, SigCur = %d, first = %d\n\n\n\n\n",pEncInst->iNoiseL, regs->nrSigmaCur,  pCodeParams->firstFrameSigma);
#endif
}

static void HEVCEncDnfSetParameters(struct hevc_instance *inst, const HEVCEncCodingCtrl *pCodeParams)
{
  regValues_s *regs = &inst->asic.regs;

  //wiener denoise paramter set
  regs->noiseReductionEnable = inst->uiNoiseReductionEnable = pCodeParams->noiseReductionEnable;//0: disable noise reduction; 1: enable noise reduction
  regs->noiseLow = pCodeParams->noiseLow;//0: use default value; valid value range: [1, 30]

#if USE_TOP_CTRL_DENOISE
  inst->iNoiseL = pCodeParams->noiseLow << FIX_POINT_BIT_WIDTH;
  regs->nrSigmaCur = inst->iSigmaCur = inst->iFirstFrameSigma = pCodeParams->firstFrameSigma << FIX_POINT_BIT_WIDTH;
  //printf("init seq noiseL = %d, SigCur = %d, first = %d\n\n\n\n\n",pEncInst->iNoiseL, regs->nrSigmaCur,  pCodeParams->firstFrameSigma);
#endif
}

static void HEVCEncDnfGetParameters(struct hevc_instance *inst, HEVCEncCodingCtrl *pCodeParams)
{
  pCodeParams->noiseReductionEnable = inst->uiNoiseReductionEnable;
#if USE_TOP_CTRL_DENOISE
  pCodeParams->noiseLow =  inst->iNoiseL >> FIX_POINT_BIT_WIDTH;
  pCodeParams->firstFrameSigma = inst->iFirstFrameSigma >> FIX_POINT_BIT_WIDTH;
#endif
}

// run before HW run, set register's value according to coding settings
static void HEVCEncDnfPrepare(struct hevc_instance *hevc_instance)
{
  asicData_s *asic = &hevc_instance->asic;
#if USE_TOP_CTRL_DENOISE
  asic->regs.nrSigmaCur = hevc_instance->iSigmaCur;
  asic->regs.nrThreshSigmaCur = hevc_instance->iThreshSigmaCur;
  asic->regs.nrSliceQPPrev = hevc_instance->iSliceQPPrev;
#endif
}

// run after HW finish one frame, update collected parameters
static void HEVCEncDnfUpdate(struct hevc_instance *hevc_instance)
{
#if USE_TOP_CTRL_DENOISE
  const int KK=102;
  unsigned int j = 0;
  int CurSigmaTmp = 0;
  unsigned int SigmaSmoothFactor[5] = {1024, 512, 341, 256, 205};
  int dSumFrmNoiseSigma = 0;
  int QpSlc = hevc_instance->asic.regs.qp;
  int FrameCodingType = hevc_instance->asic.regs.frameCodingType;
  unsigned int uiFrmNum = hevc_instance->uiFrmNum++;

  // check pre-conditions
  if (hevc_instance->uiNoiseReductionEnable == 0 || hevc_instance->stream.byteCnt == 0)
    return;

  if (uiFrmNum == 0)
    hevc_instance->FrmNoiseSigmaSmooth[0] = hevc_instance->iFirstFrameSigma; //(pHevc_instance->asic.regs.firstFrameSigma<< FIX_POINT_BIT_WIDTH);//pHevc_instance->iFirstFrmSigma;//(double)((51-QpSlc-5)/2);
  int iFrmSigmaRcv = hevc_instance->iSigmaCalcd;
  //int iThSigmaRcv = hevc_instance->iThreshSigmaCalcd;
  int iThSigmaRcv = (FrameCodingType != 1) ? hevc_instance->iThreshSigmaCalcd : hevc_instance->iThreshSigmaPrev;
  //printf("iFrmSigRcv = %d\n\n\n", iFrmSigmaRcv);
  if (iFrmSigmaRcv == 0xFFFF)
  {
    iFrmSigmaRcv = hevc_instance->iFirstFrameSigma;
    //printf("initial to %d\n", iFrmSigmaRcv);
  }
  iFrmSigmaRcv = (iFrmSigmaRcv * KK) >> 7;
  if (iFrmSigmaRcv < hevc_instance->iNoiseL) iFrmSigmaRcv = 0;
  hevc_instance->FrmNoiseSigmaSmooth[(uiFrmNum+1)%SIGMA_SMOOTH_NUM] = iFrmSigmaRcv;
  for (j = 0; j < (MIN(SIGMA_SMOOTH_NUM - 1, uiFrmNum + 1) + 1); j++)
  {
    dSumFrmNoiseSigma += hevc_instance->FrmNoiseSigmaSmooth[j];
    //printf("FrmNoiseSig %d = %d\n", j, pHevc_instance->FrmNoiseSigmaSmooth[j]);
  }
  CurSigmaTmp = (dSumFrmNoiseSigma * SigmaSmoothFactor[MIN(SIGMA_SMOOTH_NUM-1, uiFrmNum+1)]) >> 10;
  hevc_instance->iSigmaCur = CLIP3(0, (SIGMA_MAX << FIX_POINT_BIT_WIDTH), CurSigmaTmp);
  hevc_instance->iThreshSigmaCur = hevc_instance->iThreshSigmaPrev = iThSigmaRcv;
  hevc_instance->iSliceQPPrev = QpSlc;
  //printf("TOP::uiFrmNum = %d, FrmSig=%d, Th=%d, QP=%d, QP2=%d\n", uiFrmNum,pHevc_instance->asic.regs.nrSigmaCur, iThSigmaRcv, QpSlc, QpSlc2 );
#endif
}

/*------------------------------------------------------------------------------
    Function name : HEVCEncCalculateInitialQp
    Description   : Calcalute initial Qp, decide by bitrate framerate resolution
    Return type   : HEVCEncRet
    Argument      : inst - encoder instance
    Argument      : s32QpHdr - result to store
    Argument      : s32BitRate - TargetBitrate
------------------------------------------------------------------------------*/
HEVCEncRet HEVCEncCalculateInitialQp(HEVCEncInst inst, i32 *s32QpHdr, i32 s32BitRate)
{
    struct hevc_instance *hevc_instance = (struct hevc_instance *)inst;

    *s32QpHdr =  HevcCalculateInitialQp(&hevc_instance->rateControl, s32BitRate);
    return HEVCENC_OK;
}

/*------------------------------------------------------------------------------
    Function name : HEVCEncGetRealQp
    Description   : Get RealQp, qp that set to register
    Return type   : HEVCEncRet
    Argument      : inst - encoder instance
    Argument      : s32QpHdr - result to store
------------------------------------------------------------------------------*/
HEVCEncRet HEVCEncGetRealQp(HEVCEncInst inst, i32 *s32QpHdr)
{
    struct hevc_instance *hevc_instance = (struct hevc_instance *)inst;

    *s32QpHdr =  hevc_instance->asic.regs.qp;
    return HEVCENC_OK;

}
