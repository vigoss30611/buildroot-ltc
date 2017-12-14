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
#define HEVCENC_SW_BUILD ((HEVCENC_MAJOR_VERSION * 1000000) + \
  (HEVCENC_MINOR_VERSION * 1000) + HEVCENC_BUILD_REVISION)

#define PRM_SET_BUFF_SIZE 1024    /* Parameter sets max buffer size */
#define HEVCENC_MAX_BITRATE             (60000*1000)    /* Level 6 main tier limit */

/* HW ID check. HEVCEncInit() will fail if HW doesn't match. */
#define HW_ID_MASK  0xFFFF0000
#define HW_ID       0x48320000

#define HEVCENC_MIN_ENC_WIDTH       64 //132     /* 144 - 12 pixels overfill */
#define HEVCENC_MAX_ENC_WIDTH       4096
#define HEVCENC_MIN_ENC_HEIGHT      64 //96
#define HEVCENC_MAX_ENC_HEIGHT      8192
#define HEVCENC_MAX_REF_FRAMES      2
#define HEVCENC_MAX_LEVEL           HEVCENC_LEVEL_6
#define HEVCENC_MAX_REF_FRAMES      2

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

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/
static i32 out_of_memory(struct hevc_buffer *n, u32 size);
static i32 get_buffer(struct buffer *, struct hevc_buffer *, i32 size,
                      i32 reset);
static i32 init_buffer(struct hevc_buffer *, struct hevc_instance *);
static i32 set_parameter(struct hevc_instance *hevc_instance, struct vps *v,
                         struct sps *s, struct pps *p);

static bool_e hevcCheckCfg(const HEVCEncConfig *pEncCfg);
static bool_e SetParameter(struct hevc_instance *inst, const HEVCEncConfig   *pEncCfg);
static i32 hevc_set_ref_pic_set(struct hevc_instance *hevc_instance);
static i32 HEVCGetAllowedWidth(i32 width, HEVCEncPictureType inputType);
static void LamdaQp(int qp, unsigned int *lamda_sse, unsigned int *lamda_sad);
static void HevcSetNewFrame(HEVCEncInst inst);


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

  //    APITRACE("HEVCEncGetApiVersion# OK");
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
  ver.hwBuild = h2_EWLReadAsicID();

  //   APITRACE("HEVCEncGetBuild# OK");

  return (ver);
}

HEVCEncRet HEVCEncOff(HEVCEncInst inst)
{
    struct hevc_instance *pEncInst = (struct hevc_instance *) inst;
    if(pEncInst == NULL)
    {
        printf("HEVCEncOff: ERROR Null argument\n");
        return HEVCENC_NULL_ARGUMENT;
    }
    h2_EWLOff(pEncInst->asic.ewl);
    return HEVCENC_OK;
}

HEVCEncRet HEVCEncOn(HEVCEncInst inst)
{
    struct hevc_instance *pEncInst = (struct hevc_instance *) inst;
    if(pEncInst == NULL)
    {
        printf("HEVCEncOn: ERROR Null argument\n");
        return HEVCENC_NULL_ARGUMENT;
    }
    h2_EWLOn(pEncInst->asic.ewl);
    return HEVCENC_OK;
}

HEVCEncRet HEVCEncInitOn(void)
{
    h2_EWLInitOn();
    return HEVCENC_OK;
}

/*------------------------------------------------------------------------------
  hevc_init initializes the encoder and creates new encoder instance.
------------------------------------------------------------------------------*/
HEVCEncRet HEVCEncInit(HEVCEncConfig *config, HEVCEncInst *instAddr)
{
  struct instance *instance;
  const void *ewl = NULL;
  h2_EWLInitParam_t param;
  struct hevc_instance *hevc_inst;
  struct container *c;

  struct vps *v;
  struct sps *s;
  struct pps *p;

  //APITRACE("HEVCEncInit#");

  /* check that right shift on negative numbers is performed signed */
  /*lint -save -e* following check causes multiple lint messages */
#if (((-1) >> 1) != (-1))
#error Right bit-shifting (>>) does not preserve the sign
#endif
  /*lint -restore */

  /* Check for illegal inputs */
  if (config == NULL || instAddr == NULL)
  {
    printf("HEVCEncInit: ERROR Null argument");
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for correct HW */
  if ((h2_EWLReadAsicID() & HW_ID_MASK) != HW_ID)
  {
    printf("HEVCEncInit: ERROR Invalid HW ID");
    return HEVCENC_ERROR;
  }

  /* Check that configuration is valid */
  if (hevcCheckCfg(config) == ENCHW_NOK)
  {
    printf("hevcCheckCfg fail. \n");
    return HEVCENC_INVALID_ARGUMENT;
  }


  param.clientType = EWL_CLIENT_TYPE_HEVC_ENC;

  /* Init EWL */
  if ((ewl = h2_EWLInit(&param)) == NULL){
    printf("h2_EWLInit error\n");
    return HEVCENC_EWL_ERROR;
    }


  *instAddr = NULL;
  if (!(instance = h2_EWLcalloc(1, sizeof(struct instance)))) {
    printf("h2_EWLmalloc error\n");
    goto error;
    }

  instance->instance = (struct hevc_instance *)instance;
  *instAddr = (HEVCEncInst)instance;
  ASSERT(instance == (struct instance *)&instance->hevc_instance);
  hevc_inst = (struct hevc_instance *)instance;
  if (!(c = get_container(hevc_inst))) {
        printf("get_container error\n");
        return HEVCENC_ERROR;

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
  hevc_inst->interlaced = config->interlacedFrame;

  /* Initialize ASIC */

  hevc_inst->asic.ewl = ewl;
  (void) h2_EncAsicControllerInit(&hevc_inst->asic);

  hevc_inst->width = config->width;
  hevc_inst->height = config->height / (hevc_inst->interlaced + 1);

  /* Allocate internal SW/HW shared memories */
  if (h2_EncAsicMemAlloc_V2(&hevc_inst->asic,
                         config->width,
                         config->height / (hevc_inst->interlaced + 1),
                         0,
                         0,
                         ASIC_HEVC, config->refFrameAmount,
                         config->refFrameAmount,
                         config->compressor) != ENCHW_OK)
  {

    //ret = HEVCENC_EWL_MEMORY_ERROR;
    printf("h2_EncAsicMemAlloc_V2 error\n");
    goto error;
  }

  /* Set parameters depending on user config */
  if (SetParameter(hevc_inst, config) != ENCHW_OK)
  {
    printf("SetParameter error\n");
    goto error;
  }

  if (HevcInitRc(&hevc_inst->rateControl, 1) != ENCHW_OK)
  {
    printf("HevcInitRc fail. \n");
    return HEVCENC_INVALID_ARGUMENT;
  }

  hevc_inst->created_pic_num = 0;  //will.he backport to fix multi-instance issue

  /* Status == INIT   Initialization succesful */
  hevc_inst->encStatus = HEVCENCSTAT_INIT;

  hevc_inst->inst = hevc_inst;
  return HEVCENC_OK;

error:
  if (instance != NULL)
    h2_EWLfree(instance);
  if (ewl != NULL)
    (void) h2_EWLRelease(ewl);
  return HEVCENC_ERROR;

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

  h2_EncAsicMemFree_V2(&pEncInst->asic);


  h2_EWLfree(pEncInst);

  (void) h2_EWLRelease(ewl);
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

  //APITRACE("HEVCEncRelease#");

  /* Check for illegal inputs */
  if (pEncInst == NULL)
  {
    //APITRACE("HEVCEncRelease: ERROR Null argument");
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (pEncInst->inst != inst)
  {
    //APITRACE("HEVCEncRelease: ERROR Invalid instance");
    return HEVCENC_INSTANCE_ERROR;
  }


  if (!(c = get_container(pEncInst))) return HEVCENC_ERROR;

  sw_free_pictures(c);  /* Call this before free_parameter_sets() */
  free_parameter_sets(c);

  HEVCShutdown((struct instance *)pEncInst);

  return HEVCENC_OK;
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

  //APITRACE("HEVCEncSetCodingCtrl#");

  /* Check for illegal inputs */
  if ((pEncInst == NULL) || (pCodeParams == NULL))
  {
    //APITRACE("HEVCEncSetCodingCtrl: ERROR Null argument");
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (pEncInst->inst != pEncInst)
  {
    //APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid instance");
    return HEVCENC_INSTANCE_ERROR;
  }
#if 0
  /* Check for invalid values */
  if (pCodeParams->sliceSize > pEncInst->ctbPerCol)
  {
    //APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid sliceSize");
    return HEVCENC_INVALID_ARGUMENT;
  }
#endif
  if (pCodeParams->cirStart > (u32)pEncInst->ctbPerFrame ||
      pCodeParams->cirInterval > (u32)pEncInst->ctbPerFrame)
  {
    //APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid CIR value");
    return HEVCENC_INVALID_ARGUMENT;
  }
  if (pCodeParams->intraArea.enable)
  {
    if (!(pCodeParams->intraArea.top <= pCodeParams->intraArea.bottom &&
          pCodeParams->intraArea.bottom < (u32)pEncInst->ctbPerCol &&
          pCodeParams->intraArea.left <= pCodeParams->intraArea.right &&
          pCodeParams->intraArea.right < (u32)pEncInst->ctbPerRow))
    {
      //APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid intraArea");
      return HEVCENC_INVALID_ARGUMENT;
    }
  }

  if (pCodeParams->roi1Area.enable)
  {
    if (!(pCodeParams->roi1Area.top <= pCodeParams->roi1Area.bottom &&
          pCodeParams->roi1Area.bottom < (u32)pEncInst->ctbPerCol &&
          pCodeParams->roi1Area.left <= pCodeParams->roi1Area.right &&
          pCodeParams->roi1Area.right < (u32)pEncInst->ctbPerRow))
    {
      //APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid roi1Area");
      return HEVCENC_INVALID_ARGUMENT;
    }
  }

  if (pCodeParams->roi2Area.enable)
  {
    if (!(pCodeParams->roi2Area.top <= pCodeParams->roi2Area.bottom &&
          pCodeParams->roi2Area.bottom < (u32)pEncInst->ctbPerCol &&
          pCodeParams->roi2Area.left <= pCodeParams->roi2Area.right &&
          pCodeParams->roi2Area.right < (u32)pEncInst->ctbPerRow))
    {
      //APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid roi2Area");
      return HEVCENC_INVALID_ARGUMENT;
    }
  }

  if (pCodeParams->roi1DeltaQp < -15 ||
      pCodeParams->roi1DeltaQp > 0 ||
      pCodeParams->roi2DeltaQp < -15 ||
      pCodeParams->roi2DeltaQp > 0 /*||
     pCodeParams->adaptiveRoi < -51 ||
     pCodeParams->adaptiveRoi > 0*/)
  {
    //APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid ROI delta QP");
    return HEVCENC_INVALID_ARGUMENT;
  }

  /* Check status, only slice size, CIR & ROI allowed to change after start */
  if (pEncInst->encStatus != HEVCENCSTAT_INIT)
  {
    goto set_slice_size;
  }


  if (/*pCodeParams->constrainedIntraPrediction > 1 ||*/
    pCodeParams->disableDeblockingFilter > 2 || pCodeParams->insertrecoverypointmessage > 1 ||
    pCodeParams->seiMessages > 1 || pCodeParams->videoFullRange > 1)
  {
    //APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid enable/disable");
    return HEVCENC_INVALID_ARGUMENT;
  }

  if (pCodeParams->sampleAspectRatioWidth > 65535 ||
      pCodeParams->sampleAspectRatioHeight > 65535)
  {
    //APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid sampleAspectRatio");
    return HEVCENC_INVALID_ARGUMENT;
  }


  if (pCodeParams->cabacInitFlag > 1)
  {
    //APITRACE("HEVCEncSetCodingCtrl: ERROR Invalid cabacInitIdc");
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
#if 0
  /* Slice size is set in macroblock rows => convert to macroblocks */
  pEncInst->slice.sliceSize = pCodeParams->sliceSize * pEncInst->ctbPerRow;
  pEncInst->slice.fieldOrder = pCodeParams->fieldOrder ? 1 : 0;

  pEncInst->slice.quarterPixelMv = pCodeParams->quarterPixelMv;
#else
  regs->sliceSize = pCodeParams->sliceSize;
#endif

  pEncInst->max_cu_size = pCodeParams->max_cu_size;   /* Max coding unit size in pixels */
  pEncInst->min_cu_size = pCodeParams->min_cu_size;   /* Min coding unit size in pixels */
  pEncInst->max_tr_size = pCodeParams->max_tr_size;   /* Max transform size in pixels */
  pEncInst->min_tr_size = pCodeParams->min_tr_size;   /* Min transform size in pixels */
  pEncInst->tr_depth_intra = pCodeParams->tr_depth_intra;   /* Max transform hierarchy depth */
  pEncInst->tr_depth_inter = pCodeParams->tr_depth_inter;   /* Max transform hierarchy depth */
  pEncInst->enableScalingList = pCodeParams->enableScalingList;

  if (!pEncInst->min_qp_size)
    pEncInst->min_qp_size = pEncInst->min_cu_size;

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

  pEncInst->rateControl.sei.insertrecoverypointmessage = pCodeParams->insertrecoverypointmessage;

  pEncInst->rateControl.sei.recoverypointpoc = pCodeParams->recoverypointpoc;

  if (pCodeParams->roi1Area.enable)
  {
    regs->roi1Top = pCodeParams->roi1Area.top;
    regs->roi1Left = pCodeParams->roi1Area.left;
    regs->roi1Bottom = pCodeParams->roi1Area.bottom;
    regs->roi1Right = pCodeParams->roi1Area.right;
  }
  else
  {
    regs->roi1Top = regs->roi1Left = regs->roi1Bottom =
                                       regs->roi1Right = 255;
  }
  if (pCodeParams->roi2Area.enable)
  {
    regs->roi2Top = pCodeParams->roi2Area.top;
    regs->roi2Left = pCodeParams->roi2Area.left;
    regs->roi2Bottom = pCodeParams->roi2Area.bottom;
    regs->roi2Right = pCodeParams->roi2Area.right;
  }
  else
  {
    regs->roi2Top = regs->roi2Left = regs->roi2Bottom =
                                       regs->roi2Right = 255;
  }
  regs->roi1DeltaQp = -pCodeParams->roi1DeltaQp;
  regs->roi2DeltaQp = -pCodeParams->roi2DeltaQp;
  regs->roiUpdate   = 1;    /* ROI has changed from previous frame. */



  //APITRACE("HEVCEncSetCodingCtrl: OK");
  return HEVCENC_OK;
}

/*------------------------------------------------------------------------------
  set_parameter
------------------------------------------------------------------------------*/
i32 set_parameter(struct hevc_instance *hevc_instance, struct vps *v,
                  struct sps *s, struct pps *p)
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
  if (check_range(tmp, 3, 6)) goto out;
  s->log2_min_cb_size = tmp;

  if (log2i(hevc_instance->max_cu_size, &tmp)) goto out;
  if (check_range(tmp, s->log2_min_cb_size, 6)) goto out;
  s->log2_diff_cb_size = tmp - s->log2_min_cb_size;
  p->log2_ctb_size = s->log2_min_cb_size + s->log2_diff_cb_size;
  p->ctb_size = 1 << p->log2_ctb_size;
  ASSERT(p->ctb_size == hevc_instance->max_cu_size);

  /* Transform size */
  if (log2i(hevc_instance->min_tr_size, &tmp)) goto out;
  if (check_range(tmp, 2, s->log2_min_cb_size - 1)) goto out;
  s->log2_min_tr_size = tmp;

  if (log2i(hevc_instance->max_tr_size, &tmp)) goto out;
  if (check_range(tmp, s->log2_min_tr_size, MIN(p->log2_ctb_size, 5)))
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
  if (check_range(hevc_instance->rateControl.qpHdr, 0, 51)) goto out;
  p->init_qp = hevc_instance->rateControl.qpHdr;

  /* Picture level rate control TODO... */
  if (check_range(hevc_instance->rateControl.picRc, 0, 1)) goto out;

  p->cu_qp_delta_enabled_flag = ((hevc_instance->asic.regs.roi1Top != 255) || (hevc_instance->asic.regs.roi2Top != 255)) ? 1 : 0;
#ifdef INTERNAL_TEST
  p->cu_qp_delta_enabled_flag = (hevc_instance->testId == TID_ROI) ? 1 : p->cu_qp_delta_enabled_flag;
#endif

  if (log2i(hevc_instance->min_qp_size, &tmp)) goto out;
  if (check_range(tmp, s->log2_min_cb_size, p->log2_ctb_size)) goto out;
#if 0
  /* Get the qp size from command line. */
  p->diff_cu_qp_delta_depth = p->log2_ctb_size - tmp;
#else
  /* Make qp size the same as ctu now. FIXME: when qp size is different. */
  p->diff_cu_qp_delta_depth = 0;
#endif

  /* Init the rest. TODO: tiles are not supported yet. How to get
   * slice/tiles from test_bench.c ? */
  if (init_parameter_set(s, p)) goto out;
  p->deblocking_filter_disabled_flag = hevc_instance->disableDeblocking;
  p->tc_offset = hevc_instance->tc_Offset * 2;
  p->beta_offset = hevc_instance->beta_Offset * 2;
  p->deblocking_filter_override_enabled_flag = hevc_instance->enableDeblockOverride;

#if 0 // ENABLE_CONSTRAINED_INTRA_PRED NOT SUPPORT NOW.
  /* constrained_intra_pred_flag */
  p->constrained_intra_pred_flag = hevc_instance->constrained_intra_pred_flag;
  ASSERT((p->constrained_intra_pred_flag == 0) || (p->constrained_intra_pred_flag == 1));
#endif /* ENABLE_CONSTRAINED_INTRA_PRED */

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

  //APITRACE("HEVCEncGetCodingCtrl#");

  /* Check for illegal inputs */
  if ((pEncInst == NULL) || (pCodeParams == NULL))
  {
    //APITRACE("HEVCEncGetCodingCtrl: ERROR Null argument");
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (pEncInst->inst != pEncInst)
  {
    //APITRACE("HEVCEncGetCodingCtrl: ERROR Invalid instance");
    return HEVCENC_INSTANCE_ERROR;
  }

  pCodeParams->seiMessages =
    (pEncInst->rateControl.sei.enabled == ENCHW_YES) ? 1 : 0;


  pCodeParams->fieldOrder = pEncInst->fieldOrder;

  pCodeParams->videoFullRange =
    (pEncInst->sps->vui.videoFullRange == ENCHW_YES) ? 1 : 0;
  pCodeParams->sampleAspectRatioWidth =
    pEncInst->sps->vui.sarWidth;
  pCodeParams->sampleAspectRatioHeight =
    pEncInst->sps->vui.sarHeight;
  pCodeParams->sliceSize = regs->sliceSize;

  pCodeParams->cabacInitFlag = regs->cabac_init_flag;

  pCodeParams->cirStart = regs->cirStart;
  pCodeParams->cirInterval = regs->cirInterval;
  pCodeParams->intraArea.enable = regs->intraAreaTop < (u32)pEncInst->ctbPerCol ? 1 : 0;
  pCodeParams->intraArea.top    = regs->intraAreaTop;
  pCodeParams->intraArea.left   = regs->intraAreaLeft;
  pCodeParams->intraArea.bottom = regs->intraAreaBottom;
  pCodeParams->intraArea.right  = regs->intraAreaRight;
  pCodeParams->roi1Area.enable = regs->roi1Top < (u32)pEncInst->ctbPerCol ? 1 : 0;
  pCodeParams->roi1Area.top    = regs->roi1Top;
  pCodeParams->roi1Area.left   = regs->roi1Left;
  pCodeParams->roi1Area.bottom = regs->roi1Bottom;
  pCodeParams->roi1Area.right  = regs->roi1Right;
  pCodeParams->roi2Area.enable = regs->roi2Top < (u32)pEncInst->ctbPerCol ? 1 : 0;
  pCodeParams->roi2Area.top    = regs->roi2Top;
  pCodeParams->roi2Area.left   = regs->roi2Left;
  pCodeParams->roi2Area.bottom = regs->roi2Bottom;
  pCodeParams->roi2Area.right  = regs->roi2Right;
  pCodeParams->roi1DeltaQp = -regs->roi1DeltaQp;
  pCodeParams->roi2DeltaQp = -regs->roi2DeltaQp;
  pCodeParams->max_cu_size = (1 << regs->maxCbSize);
  pCodeParams->min_cu_size = (1 << regs->minCbSize);
  pCodeParams->max_tr_size = (1 << regs->maxTrbSize);
  pCodeParams->min_tr_size = (1 << regs->minTrbSize);


  pCodeParams->roi1DeltaQp = -regs->roi1DeltaQp;
  pCodeParams->roi2DeltaQp = -regs->roi2DeltaQp;

  pCodeParams->tr_depth_intra = regs->maxTransHierarchyDepthIntra;
  pCodeParams->tr_depth_inter = regs->maxTransHierarchyDepthInter;

  pCodeParams->enableDeblockOverride = pEncInst->enableDeblockOverride;

  pCodeParams->deblockOverride = regs->slice_deblocking_filter_override_flag;

  pCodeParams->chroma_qp_offset = pEncInst->chromaQpOffset;

  pCodeParams->insertrecoverypointmessage = pEncInst->rateControl.sei.insertrecoverypointmessage;

  pCodeParams->recoverypointpoc = pEncInst->rateControl.sei.recoverypointpoc;

  //APITRACE("HEVCEncGetCodingCtrl: OK");
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

  //APITRACE("HEVCEncSetRateCtrl#");

  /* Check for illegal inputs */
  if ((hevc_instance == NULL) || (pRateCtrl == NULL))
  {
    //APITRACE("HEVCEncSetRateCtrl: ERROR Null argument");
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (hevc_instance->inst != hevc_instance)
  {
    //APITRACE("HEVCEncSetRateCtrl: ERROR Invalid instance");
    return HEVCENC_INSTANCE_ERROR;
  }

  rc = &hevc_instance->rateControl;

  /* after stream was started with HRD ON,
   * it is not allowed to change RC params */
  if (hevc_instance->encStatus == HEVCENCSTAT_START_FRAME && rc->hrd == ENCHW_YES && pRateCtrl->hrd == ENCHW_YES)
  {
    //APITRACE
    //  ("HEVCEncSetRateCtrl: ERROR Stream started with HRD ON. Not allowed to change any parameters");
    return HEVCENC_INVALID_STATUS;
  }

  /* Check for invalid input values */
  if (pRateCtrl->pictureRc > 1 ||
      pRateCtrl->pictureSkip > 1 || pRateCtrl->hrd > 1)
  {
    //APITRACE("HEVCEncSetRateCtrl: ERROR Invalid enable/disable value");
    return HEVCENC_INVALID_ARGUMENT;
  }

  if (pRateCtrl->qpHdr > 51 ||
      pRateCtrl->qpMin > 51 ||
      pRateCtrl->qpMax > 51 ||
      pRateCtrl->qpMax < pRateCtrl->qpMin)
  {
    //APITRACE("HEVCEncSetRateCtrl: ERROR Invalid QP");
    return HEVCENC_INVALID_ARGUMENT;
  }

  if ((pRateCtrl->qpHdr != -1) &&
      (pRateCtrl->qpHdr < (i32)pRateCtrl->qpMin ||
       pRateCtrl->qpHdr > (i32)pRateCtrl->qpMax))
  {
    //APITRACE("HEVCEncSetRateCtrl: ERROR QP out of range");
    return HEVCENC_INVALID_ARGUMENT;
  }
  if ((u32)(pRateCtrl->intraQpDelta + 51) > 102)
  {
    //APITRACE("HEVCEncSetRateCtrl: ERROR intraQpDelta out of range");
    return HEVCENC_INVALID_ARGUMENT;
  }
  if (pRateCtrl->fixedIntraQp > 51)
  {
    //APITRACE("HEVCEncSetRateCtrl: ERROR fixedIntraQp out of range");
    return HEVCENC_INVALID_ARGUMENT;
  }
  if (pRateCtrl->gopLen < 1 || pRateCtrl->gopLen > 300)
  {
    //APITRACE("HEVCEncSetRateCtrl: ERROR Invalid GOP length");
    return HEVCENC_INVALID_ARGUMENT;
  }

  /* Bitrate affects only when rate control is enabled */
  if ((pRateCtrl->pictureRc ||
       pRateCtrl->pictureSkip || pRateCtrl->hrd) &&
      (pRateCtrl->bitPerSecond < 10000 ||
       pRateCtrl->bitPerSecond > HEVCENC_MAX_BITRATE))
  {
    //APITRACE("HEVCEncSetRateCtrl: ERROR Invalid bitPerSecond");
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
        //APITRACE
        //  ("HEVCEncSetRateCtrl: ERROR. HRD is ON. hrdCpbSize higher than maximum allowed for stream level");
        return HEVCENC_INVALID_ARGUMENT;
      }

      if (bps > HEVCMaxBR[level])
      {
        //APITRACE
        //  ("HEVCEncSetRateCtrl: ERROR. HRD is ON. bitPerSecond higher than maximum allowed for stream level");
        return HEVCENC_INVALID_ARGUMENT;
      }
    }

    rc->virtualBuffer.bufferSize = cpbSize;

    /* Set the parameters to rate control */
    if (pRateCtrl->pictureRc != 0)
      rc->picRc = ENCHW_YES;
    else
      rc->picRc = ENCHW_NO;

    rc->ctbRc = ENCHW_NO;

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

    rc->qpHdr = pRateCtrl->qpHdr;
    rc->qpMin = pRateCtrl->qpMin;
    rc->qpMax = pRateCtrl->qpMax;
    prevBitrate = rc->virtualBuffer.bitRate;
    rc->virtualBuffer.bitRate = bps;
    rc->gopLen = pRateCtrl->gopLen;
  }

  rc->intraQpDelta = pRateCtrl->intraQpDelta;
  rc->fixedIntraQp = pRateCtrl->fixedIntraQp;

  /* New parameters checked already so ignore return value.
  * Reset RC bit counters when changing bitrate. */
  (void) HevcInitRc(rc, (hevc_instance->encStatus == HEVCENCSTAT_INIT) ||
                    (rc->virtualBuffer.bitRate != prevBitrate) || rc->hrd ||pRateCtrl->u32NewStream);

  //APITRACE("HEVCEncSetRateCtrl: OK");
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

  //APITRACE("HEVCEncGetRateCtrl#");

  /* Check for illegal inputs */
  if ((hevc_instance == NULL) || (pRateCtrl == NULL))
  {
    //APITRACE("HEVCEncGetRateCtrl: ERROR Null argument");
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (hevc_instance->inst != hevc_instance)
  {
    //APITRACE("HEVCEncGetRateCtrl: ERROR Invalid instance");
    return HEVCENC_INSTANCE_ERROR;
  }

  /* Get the values */
  rc = &hevc_instance->rateControl;

  pRateCtrl->pictureRc = rc->picRc == ENCHW_NO ? 0 : 1;
  pRateCtrl->pictureSkip = rc->picSkip == ENCHW_NO ? 0 : 1;
  pRateCtrl->qpHdr = rc->qpHdr;
  pRateCtrl->qpMin = rc->qpMin;
  pRateCtrl->qpMax = rc->qpMax;
  pRateCtrl->bitPerSecond = rc->virtualBuffer.bitRate;
  pRateCtrl->hrd = rc->hrd == ENCHW_NO ? 0 : 1;
  pRateCtrl->gopLen = rc->gopLen;

  pRateCtrl->hrdCpbSize = (u32) rc->virtualBuffer.bufferSize;
  pRateCtrl->intraQpDelta = rc->intraQpDelta;
  pRateCtrl->fixedIntraQp = rc->fixedIntraQp;

  //APITRACE("HEVCEncGetRateCtrl: OK");
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
  /* Check for illegal inputs */
  if ((inst == NULL) || (pPreProcCfg == NULL))
  {
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (hevc_instance->inst != hevc_instance)
  {
    return HEVCENC_INSTANCE_ERROR;
  }

  /* check HW limitations */
  {
    EWLHwConfig_t cfg = h2_EWLReadAsicConfig();

    /* is video stabilization supported? */
    if (cfg.vsEnabled == EWL_HW_CONFIG_NOT_SUPPORTED &&
        pPreProcCfg->videoStabilization != 0)
    {
      return HEVCENC_INVALID_ARGUMENT;
    }
    if (cfg.rgbEnabled == EWL_HW_CONFIG_NOT_SUPPORTED &&
        pPreProcCfg->inputType >= HEVCENC_RGB565 &&
        pPreProcCfg->inputType <= HEVCENC_BGR101010)
    {
      return HEVCENC_INVALID_ARGUMENT;
    }
    if (cfg.scalingEnabled == EWL_HW_CONFIG_NOT_SUPPORTED &&
        pPreProcCfg->scaledOutput != 0)
    {
      ;
    }
  }

  if (pPreProcCfg->origWidth > HEVCENC_MAX_PP_INPUT_WIDTH ||
      pPreProcCfg->origHeight > HEVCENC_MAX_PP_INPUT_HEIGHT)
  {
    return HEVCENC_INVALID_ARGUMENT;
  }

  /* Scaled image width limits, multiple of 4 (YUYV) and smaller than input */
  if ((pPreProcCfg->scaledWidth > (u32)hevc_instance->width) ||
      (pPreProcCfg->scaledWidth & 0x7) != 0)
  {
    //APITRACE("hevcCheckCfg: Invalid scaledWidth");
    return ENCHW_NOK;
  }

  if (((i32)pPreProcCfg->scaledHeight > hevc_instance->height) ||
      (pPreProcCfg->scaledHeight & 0x1) != 0)
  {
    //APITRACE("hevcCheckCfg: Invalid scaledHeight");
    return ENCHW_NOK;
  }

  if (pPreProcCfg->inputType > HEVCENC_BGR101010)
  {
    return HEVCENC_INVALID_ARGUMENT;
  }

  if (pPreProcCfg->rotation > HEVCENC_ROTATE_90L)
  {
    return HEVCENC_INVALID_ARGUMENT;
  }

  pp_tmp = hevc_instance->preProcess;

  if (pPreProcCfg->videoStabilization == 0)
  {
    pp_tmp.horOffsetSrc = pPreProcCfg->xOffset >> 1 << 1;
    pp_tmp.verOffsetSrc = pPreProcCfg->yOffset >> 1 << 1;
  }
  else
  {
    pp_tmp.horOffsetSrc = pp_tmp.verOffsetSrc = 0;
  }

  /* Encoded frame resolution as set in Init() */
  //pp_tmp = pEncInst->preProcess;    //here is a bug? if enabling this line.

  pp_tmp.lumWidthSrc  = pPreProcCfg->origWidth;
  pp_tmp.lumHeightSrc = pPreProcCfg->origHeight;
  pp_tmp.rotation     = pPreProcCfg->rotation;
  pp_tmp.inputFormat  = pPreProcCfg->inputType;
  pp_tmp.videoStab    = (pPreProcCfg->videoStabilization != 0) ? 1 : 0;
  pp_tmp.interlacedFrame = (pPreProcCfg->interlacedFrame != 0) ? 1 : 0;
  pp_tmp.scaledWidth  = pPreProcCfg->scaledWidth;
  pp_tmp.scaledHeight = pPreProcCfg->scaledHeight;
  asic->scaledImage.busAddress = pPreProcCfg->busAddressScaledBuff;
  asic->scaledImage.virtualAddress = pPreProcCfg->virtualAddressScaledBuff;
  asic->scaledImage.size = pPreProcCfg->sizeScaledBuff;
  asic->regs.scaledLumBase = asic->scaledImage.busAddress;

  pp_tmp.scaledOutput = (pPreProcCfg->scaledOutput) ? 1 : 0;
  if (pPreProcCfg->scaledWidth * pPreProcCfg->scaledHeight == 0)
    pp_tmp.scaledOutput = 0;
  else
    pp_tmp.scaledOutput = 1;
  /* Check for invalid values */
  if (h265_EncPreProcessCheck(&pp_tmp) != ENCHW_OK)
  {
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
    else        /* Rotate left */
    {
      pp_tmp.frameCropRightOffset = fillRight / 2;
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
  h265_EncSetColorConversion(&pp_tmp, &hevc_instance->asic);

  hevc_instance->preProcess = pp_tmp;

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

  /* Check for illegal inputs */
  if ((inst == NULL) || (pPreProcCfg == NULL))
  {
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (hevc_instance->inst != hevc_instance)
  {
    return HEVCENC_INSTANCE_ERROR;
  }

  pPP = &hevc_instance->preProcess;

  pPreProcCfg->origHeight = pPP->lumHeightSrc;
  pPreProcCfg->origWidth = pPP->lumWidthSrc;
  pPreProcCfg->xOffset = pPP->horOffsetSrc;
  pPreProcCfg->yOffset = pPP->verOffsetSrc;

  pPreProcCfg->rotation = (HEVCEncPictureRotation) pPP->rotation;
  pPreProcCfg->inputType = (HEVCEncPictureType) pPP->inputFormat;

  pPreProcCfg->interlacedFrame    = pPP->interlacedFrame;
  pPreProcCfg->videoStabilization = pPP->videoStab;
  {
    EWLHwConfig_t cfg = h2_EWLReadAsicConfig();
    if (cfg.scalingEnabled)
      pPreProcCfg->scaledOutput       = 1;
  }
  pPreProcCfg->scaledWidth       = pPP->scaledWidth;
  pPreProcCfg->scaledHeight       = pPP->scaledHeight;


  pPreProcCfg->colorConversion.type =
    (HEVCEncColorConversionType) pPP->colorConversionType;
  pPreProcCfg->colorConversion.coeffA = pPP->colorConversionCoeffA;
  pPreProcCfg->colorConversion.coeffB = pPP->colorConversionCoeffB;
  pPreProcCfg->colorConversion.coeffC = pPP->colorConversionCoeffC;
  pPreProcCfg->colorConversion.coeffE = pPP->colorConversionCoeffE;
  pPreProcCfg->colorConversion.coeffF = pPP->colorConversionCoeffF;


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

  /* Check for illegal inputs */
  if ((hevc_instance == NULL) || (userDataSize != 0 && pUserData == NULL))
  {
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (hevc_instance->inst != hevc_instance)
  {
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
  struct hevc_buffer *hevc_buffer;
  i32 *poc;
  i32 rps_id = 0;
  i32 ret = HEVCENC_ERROR;
  /* Check for illegal inputs */
  if ((hevc_instance == NULL) || (pEncIn == NULL) || (pEncOut == NULL))
  {
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (hevc_instance->inst != hevc_instance)
  {
    return HEVCENC_INSTANCE_ERROR;
  }


  /* Check status */
  if ((hevc_instance->encStatus != HEVCENCSTAT_INIT) &&
      (hevc_instance->encStatus != HEVCENCSTAT_START_FRAME))
  {
    return HEVCENC_INVALID_STATUS;
  }

  /* Check for invalid input values */
  if ((pEncIn->pOutBuf == NULL) ||
      (pEncIn->outBufSize < HEVCENCSTRMSTART_MIN_BUF))
  {
    return HEVCENC_INVALID_ARGUMENT;
  }

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


  /* Initialize tb->instance->buffer so that we can cast it to hevc_buffer
   * by calling hevc_set_parameter(), see api.c TODO...*/
  hevc_instance->rps_id = -1;
  hevc_set_ref_pic_set(hevc_instance);
  hevc_buffer = (struct hevc_buffer *)hevc_instance->temp_buffer;
  poc = (i32 *)hevc_buffer->buffer;

  /* RPS 0 */
  poc[0] = -1;
  poc[1] = 1;
  poc[2] = 0;
  hevc_instance->rps_id = 0;
  if (hevc_set_ref_pic_set(hevc_instance))
  {
    Error(2, ERR, "hevc_set_ref_pic_set() fails");
    return NOK;
  }

  rps_id++;
  if (hevc_instance->interlaced)
  {
    /* RPS 2 */
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

  /* RPS 1 for intra only  */
  poc[0] = 0;
  hevc_instance->rps_id = rps_id;
  if (hevc_set_ref_pic_set(hevc_instance))
  {
    Error(2, ERR, "hevc_set_ref_pic_set() fails");
    return NOK;
  }

  v = (struct vps *)get_parameter_set(c, VPS_NUT, 0);
  s = (struct sps *)get_parameter_set(c, SPS_NUT, 0);
  p = (struct pps *)get_parameter_set(c, PPS_NUT, 0);

  /* Initialize parameter sets */
  if (set_parameter(hevc_instance, v, s, p)) goto out;


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
  pEncOut->streamSize += *v->ps.b.cnt;
  HEVCAddNaluSize(pEncOut, *v->ps.b.cnt);
  hevc_instance->stream.stream += *v->ps.b.cnt;
  s->ps.b.stream = hevc_instance->stream.stream;
  sequence_parameter_set(c, s, inst);
  pEncOut->streamSize += *s->ps.b.cnt;
  HEVCAddNaluSize(pEncOut, *s->ps.b.cnt);
  hevc_instance->stream.stream += *s->ps.b.cnt;
  p->ps.b.stream = hevc_instance->stream.stream;
  picture_parameter_set(p);
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


  ret = HEVCENC_OK;

out:
#ifdef TEST_DATA
  Enc_close_stream_trace();
#endif

  return ret;

}


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
  /* Check for illegal inputs */
  if ((hevc_instance == NULL) || (pEncIn == NULL) || (pEncOut == NULL))
  {
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (hevc_instance->inst != hevc_instance)
  {
    return HEVCENC_INSTANCE_ERROR;
  }
  /* Check status, INIT and ERROR not allowed */
  if ((hevc_instance->encStatus != HEVCENCSTAT_START_STREAM) &&
      (hevc_instance->encStatus != HEVCENCSTAT_START_FRAME))
  {

    return HEVCENC_INVALID_STATUS;
  }

  /* Check for invalid input values */
  if ((!HEVC_BUS_ADDRESS_VALID(pEncIn->busOutBuf)) ||
      (pEncIn->pOutBuf == NULL) ||
      (pEncIn->outBufSize < HEVCENCSTRMENCODE_MIN_BUF) ||
      (pEncIn->codingType > HEVCENC_NOTCODED_FRAME))
  {

    return HEVCENC_INVALID_ARGUMENT;
  }

  switch (hevc_instance->preProcess.inputFormat)
  {
    case HEVCENC_YUV420_PLANAR:
      if (!HEVC_BUS_CH_ADDRESS_VALID(pEncIn->busChromaV))
      {
        return HEVCENC_INVALID_ARGUMENT;
      }
      /* fall through */
    case HEVCENC_YUV420_SEMIPLANAR:
    case HEVCENC_YUV420_SEMIPLANAR_VU:
      if (!HEVC_BUS_ADDRESS_VALID(pEncIn->busChromaU))
      {
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
      if (!HEVC_BUS_ADDRESS_VALID(pEncIn->busLuma))
      {
        return HEVCENC_INVALID_ARGUMENT;
      }
      break;
    default:
      return HEVCENC_INVALID_ARGUMENT;
  }

#ifdef INTERNAL_TEST
  /* Configure the encoder instance according to the test vector */
  HevcConfigureTestBeforeFrame(hevc_instance);
#endif

#if 1   //will.he TODO: workaround, will fix later...
  /* Try to reserve the HW resource */
  if ( h2_EWLReserveHw(asic->ewl) == EWL_ERROR )
  {
      return HEVCENC_HW_RESERVED;
  }
#endif

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

  // select RPS according to coding type and interlace option
  if (hevc_instance->interlaced)
  {
    if (pEncIn->codingType == HEVCENC_INTRA_FRAME)
    {
      hevc_instance->rps_id = 2;  // the last RPS is fake for Intra
    }
    else
    {
      hevc_instance->rps_id = hevc_instance->poc == 1 ? 0 : 1;
    }
  }
  else
  {
    if (pEncIn->codingType == HEVCENC_INTRA_FRAME)
    {
      hevc_instance->rps_id = 1;  // the last RPS is fake for Intra
    }
    else
    {
      hevc_instance->rps_id = 0;
    }
  }
  pEncOut->busScaledLuma = hevc_instance->asic.scaledImage.busAddress;
  pEncOut->scaledPicture = (u8 *)hevc_instance->asic.scaledImage.virtualAddress;

  /* Get parameter sets */
  v = (struct vps *)get_parameter_set(c, VPS_NUT, 0);
  s = (struct sps *)get_parameter_set(c, SPS_NUT, 0);
  p = (struct pps *)get_parameter_set(c, PPS_NUT, 0);
  r = (struct rps *)get_parameter_set(c, RPS,     hevc_instance->rps_id);
  if (!v || !s || !p || !r) return HEVCENC_ERROR;


  /* Initialize before/after/follow poc list and mark unused picture, so
   * that you can reuse those TODO IDR flush */
  init_poc_list(r, hevc_instance->poc);
  if(ref_pic_marking(c, r)) {
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

  pic->picture_memeory_init = 0; // temperary solution to fix still pic sequence. 20150623 arthur feng

  if (pic->picture_memeory_init == 0)
  {

    pic->input.lum = pEncIn->busLuma;
    pic->input.cb = pEncIn->busChromaU;
    pic->input.cr = pEncIn->busChromaV;


    pic->recon.lum = asic->internalreconLuma[pic->picture_memeory_id].busAddress;
    pic->recon.cb = asic->internalreconChroma[pic->picture_memeory_id].busAddress;
    pic->recon.cr = pic->recon.cb + asic->regs.recon_chroma_half_size;
    pic->recon_4n_base = asic->internalreconLuma_4n[pic->picture_memeory_id].busAddress;

    // for compress
    {
      u32 lumaTblSize = asic->regs.recon_luma_compress ?
                        (((pic->sps->width + 63) / 64) * ((pic->sps->height + 63) / 64) * 8) : 0;
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

  /* Generate reference picture list of current picture using active
   * reference picture set */
  reference_picture_list(c, pic);


  for (i = 0; i < pic->sliceInst->active_l0_cnt; i++)
  {
    asic->regs.pRefPic_recon_l0[0][i] = pic->rpl[0][i]->recon.lum;
    asic->regs.pRefPic_recon_l0[1][i] = pic->rpl[0][i]->recon.cb;
    asic->regs.pRefPic_recon_l0[2][i] = pic->rpl[0][i]->recon.cr;
    asic->regs.pRefPic_recon_l0_4n[i] =  pic->rpl[0][i]->recon_4n_base;

    //compress
    asic->regs.ref_l0_luma_compressed[i] = pic->rpl[0][i]->recon_compress.lumaCompressed;
    asic->regs.ref_l0_luma_compress_tbl_base[i] = pic->rpl[0][i]->recon_compress.lumaTblBase;

    asic->regs.ref_l0_chroma_compressed[i] = pic->rpl[0][i]->recon_compress.chromaCompressed;
    asic->regs.ref_l0_chroma_compress_tbl_base[i] = pic->rpl[0][i]->recon_compress.chromaTblBase;
  }
  asic->regs.recon_luma_compress_tbl_base = pic->recon_compress.lumaTblBase;
  asic->regs.recon_chroma_compress_tbl_base = pic->recon_compress.chromaTblBase;

  asic->regs.active_l0_cnt = pic->sliceInst->active_l0_cnt;
  asic->regs.active_override_flag = pic->sliceInst->active_override_flag;


  //0=INTER. 1=INTRA(IDR). 2=MVC-INTER. 3=MVC-INTER(ref mod).
  if (pEncIn->codingType == HEVCENC_INTRA_FRAME)
  {
    asic->regs.frameCodingType = 1;
    pic->sliceInst->type = I_SLICE;
    pEncOut->codingType = HEVCENC_INTRA_FRAME;
  }
  else if (pEncIn->codingType == HEVCENC_PREDICTED_FRAME)
  {
    asic->regs.frameCodingType = 0;
    pic->sliceInst->type = P_SLICE;
    pEncOut->codingType = HEVCENC_PREDICTED_FRAME;
  }
  if (hevc_instance->rateControl.sei.insertrecoverypointmessage)
    hevc_instance->rateControl.firstsecond_intra_bit_error = 0;
  /* Rate control */
  HevcBeforePicRc(&hevc_instance->rateControl, pEncIn->timeIncrement,
                  pic->sliceInst->type);

  /* time stamp updated */
  HevcUpdateSeiTS(&hevc_instance->rateControl.sei, pEncIn->timeIncrement);
  /* Rate control may choose to skip the frame */
  if (hevc_instance->rateControl.frameCoded == ENCHW_NO)
  {
    //APITRACE("HEVCEncStrmEncode: OK, frame skipped");


    //pSlice->frameNum = pSlice->prevFrameNum;    /* restore frame_num */


    h2_EWLReleaseHw(asic->ewl);
    queue_put(&c->picture, (struct node *)pic);

    return HEVCENC_FRAME_READY;
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
    ENCH2_SLICE_READY_INTERRUPT & ((asic->regs.sliceNum>1));
  asic->regs.picInitQp = pic->pps->init_qp;
  asic->regs.qp = hevc_instance->rateControl.qpHdr;

  asic->regs.qpMax = hevc_instance->rateControl.qpMax;
  asic->regs.qpMin = hevc_instance->rateControl.qpMin;

  pic->pps->qp = asic->regs.qp;
  pic->pps->qpMin = asic->regs.qpMin;
  pic->pps->qpMax = asic->regs.qpMax;

  //asic->regs.diffCuQpDeltaDepth = pic->pps->diff_cu_qp_delta_depth;

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
  pic->poc                 = hevc_instance->poc;
  asic->regs.poc = pic->poc;
  asic->regs.rpsId = pic->rps->ps.id;
  asic->regs.numNegativePics = pic->rps->num_negative_pics;
  asic->regs.numPositivePics = pic->rps->num_positive_pics;

  if (pic->rps->num_negative_pics > 0)
  {
    asic->regs.delta_poc0 = pic->rps->ref_pic_s0[0].delta_poc;
    asic->regs.used_by_curr_pic0 = pic->rps->ref_pic_s0[0].used_by_curr_pic;
  }


  if (pic->rps->num_negative_pics > 1)
  {
    asic->regs.delta_poc1 = pic->rps->ref_pic_s0[1].delta_poc;
    asic->regs.used_by_curr_pic1 = pic->rps->ref_pic_s0[1].used_by_curr_pic;
  }
  asic->regs.filterDisable = hevc_instance->disableDeblocking;
  asic->regs.tc_Offset = hevc_instance->tc_Offset * 2;
  asic->regs.beta_Offset = hevc_instance->beta_Offset * 2;

  /* strong_intra_smoothing_enabled_flag */
  asic->regs.strong_intra_smoothing_enabled_flag = pic->sps->strong_intra_smoothing_enabled_flag;

  /* constrained_intra_pred_flag */
  asic->regs.constrained_intra_pred_flag = pic->pps->constrained_intra_pred_flag;

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

  /* HW base address for NAL unit sizes is affected by start offset
   * and SW created NALUs. */
  asic->regs.sizeTblBase = asic->sizeTbl.busAddress;

  /* HW Base must be 64-bit aligned */
  ASSERT(asic->regs.sizeTblBase % 8 == 0);

  /*inter prediction parameters*/
  LamdaQp(asic->regs.qp, &asic->regs.lamda_motion_sse, &asic->regs.lambda_motionSAD);
  LamdaQp(asic->regs.qp - asic->regs.roi1DeltaQp, &asic->regs.lamda_motion_sse_roi1, &asic->regs.lambda_motionSAD_ROI1);
  LamdaQp(asic->regs.qp - asic->regs.roi2DeltaQp, &asic->regs.lamda_motion_sse_roi2, &asic->regs.lambda_motionSAD_ROI2);
  asic->regs.skip_chroma_dc_threadhold = 2;
  asic->regs.lamda_SAO_luma = asic->regs.lamda_motion_sse;
  asic->regs.lamda_SAO_chroma = 0.75 * asic->regs.lamda_motion_sse;

  /*tuning on intra cu cost bias*/
  asic->regs.bits_est_bias_intra_cu_8 = 22; //25;
  asic->regs.bits_est_bias_intra_cu_16 = 40; //48;
  asic->regs.bits_est_bias_intra_cu_32 = 86; //108;
  asic->regs.bits_est_bias_intra_cu_64 = 38 * 8; //48*8;

  asic->regs.bits_est_1n_cu_penalty = 5;
  asic->regs.bits_est_tu_split_penalty = 3;
  asic->regs.inter_skip_bias = 124;

  /*small resolution, smaller CU/TU prefered*/
  if (pic->sps->width < 832 && pic->sps->height < 480)
  {
    asic->regs.bits_est_1n_cu_penalty = 0;
    asic->regs.bits_est_tu_split_penalty = 2;
  }

  // scaling list enable
  asic->regs.scaling_list_enabled_flag = pic->sps->scaling_list_enable_flag;


#ifdef INTERNAL_TEST
  /* Configure the ASIC penalties according to the test vector */
  HevcConfigureTestBeforeStart(hevc_instance);
#endif


  hevc_instance->preProcess.bottomField = (hevc_instance->poc % 2) == hevc_instance->fieldOrder;
  HevcUpdateSeiPS(&hevc_instance->rateControl.sei, hevc_instance->preProcess.interlacedFrame, hevc_instance->preProcess.bottomField);
  h265_EncPreProcess(asic, &hevc_instance->preProcess);
  /* SEI message */
  {
    sei_s *sei = &hevc_instance->rateControl.sei;

    if (sei->enabled == ENCHW_YES || sei->userDataEnabled == ENCHW_YES || sei->insertrecoverypointmessage == ENCHW_YES)
    {

      if (sei->activated_sps == 0)
      {

        tmp=hevc_instance->stream.byteCnt;
        HevcNalUnitHdr(&hevc_instance->stream, PREFIX_SEI_NUT, sei->byteStream);
        HevcActiveParameterSetsSei(&hevc_instance->stream, sei, &s->vui);
        rbsp_trailing_bits(&hevc_instance->stream);

        sei->nalUnitSize = hevc_instance->stream.byteCnt;
        printf(" activated_sps sei size=%d\n", hevc_instance->stream.byteCnt);
        //pEncOut->streamSize+=hevc_instance->stream.byteCnt;
        HEVCAddNaluSize(pEncOut, hevc_instance->stream.byteCnt-tmp);
      }

      if (sei->enabled == ENCHW_YES)
      {
        if ((pic->sliceInst->type == I_SLICE) && (sei->hrd == ENCHW_YES))
        {
          tmp=hevc_instance->stream.byteCnt;
          HevcNalUnitHdr(&hevc_instance->stream, PREFIX_SEI_NUT, sei->byteStream);
          HevcBufferingSei(&hevc_instance->stream, sei, &s->vui);
          rbsp_trailing_bits(&hevc_instance->stream);

          sei->nalUnitSize = hevc_instance->stream.byteCnt;
          printf("BufferingSei sei size=%d\n", hevc_instance->stream.byteCnt);
          //pEncOut->streamSize+=hevc_instance->stream.byteCnt;
          HEVCAddNaluSize(pEncOut, hevc_instance->stream.byteCnt-tmp);
        }
        tmp=hevc_instance->stream.byteCnt;
        HevcNalUnitHdr(&hevc_instance->stream, PREFIX_SEI_NUT, sei->byteStream);
        HevcPicTimingSei(&hevc_instance->stream, sei, &s->vui);
        rbsp_trailing_bits(&hevc_instance->stream);

        sei->nalUnitSize = hevc_instance->stream.byteCnt;
        printf("PicTiming sei size=%d\n", hevc_instance->stream.byteCnt);
        //pEncOut->streamSize+=hevc_instance->stream.byteCnt;
        HEVCAddNaluSize(pEncOut, hevc_instance->stream.byteCnt-tmp);
      }

      if (sei->userDataEnabled == ENCHW_YES)
      {
        tmp=hevc_instance->stream.byteCnt;
        HevcNalUnitHdr(&hevc_instance->stream, PREFIX_SEI_NUT, sei->byteStream);
        HevcUserDataUnregSei(&hevc_instance->stream, sei);
        rbsp_trailing_bits(&hevc_instance->stream);

        sei->nalUnitSize = hevc_instance->stream.byteCnt;
        printf("UserDataUnreg sei size=%d\n", hevc_instance->stream.byteCnt);
        //pEncOut->streamSize+=hevc_instance->stream.byteCnt;
        HEVCAddNaluSize(pEncOut, hevc_instance->stream.byteCnt-tmp);
      }


      if (sei->insertrecoverypointmessage == ENCHW_YES)
      {
        tmp=hevc_instance->stream.byteCnt;
        HevcNalUnitHdr(&hevc_instance->stream, PREFIX_SEI_NUT, sei->byteStream);
        HevcRecoveryPointSei(&hevc_instance->stream, sei);
        rbsp_trailing_bits(&hevc_instance->stream);

        sei->nalUnitSize = hevc_instance->stream.byteCnt;
        printf("RecoveryPoint sei size=%d\n", hevc_instance->stream.byteCnt);
        //pEncOut->streamSize+=hevc_instance->stream.byteCnt;
        HEVCAddNaluSize(pEncOut, hevc_instance->stream.byteCnt-tmp);
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
  slice_callback.nalUnitInfoNum=hevc_instance->numNalus;
  slice_callback.pOutBuf = pEncIn->pOutBuf;
  slice_callback.pAppData = hevc_instance->pAppData;

  HevcSetNewFrame(hevc_instance);
  hevc_instance->output_buffer_over_flow = 0;
  h2_EncAsicFrameStart(asic->ewl, &asic->regs);

  do
  {
    /* Encode one frame */
    i32 ewl_ret;

#if 1   //will.he TODO: workaround, will fix later...
    //printf("#9999 waiting hardware ready\n");

    /* Wait for IRQ for every slice or for complete frame */
    if ((asic->regs.sliceNum > 1) && hevc_instance->sliceReadyCbFunc)
      ewl_ret = h2_EWLWaitHwRdy(asic->ewl, &slice_callback.slicesReady,asic->regs.sliceNum,&status);
    else
      ewl_ret = h2_EWLWaitHwRdy(asic->ewl, NULL,asic->regs.sliceNum,&status);

    //printf("#9999 hardware ready, ewl_ret:%d\n", ewl_ret);
#else
    ewl_ret = EWL_OK;
#endif

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

      h2_EncAsicStop(asic->ewl);
      /* Release HW so that it can be used by other codecs */
      h2_EWLReleaseHw(asic->ewl);

    }
    else
    {
      /* Check ASIC status bits and possibly release HW */
      status = h2_EncAsicCheckStatus_V2(asic,status);

      switch (status)
      {
        case ASIC_STATUS_ERROR:
          ret = HEVCENC_ERROR;
          break;
        case ASIC_STATUS_HW_TIMEOUT:
          ret = HEVCENC_HW_TIMEOUT;
          break;
        case ASIC_STATUS_SLICE_READY:
          ret = HEVCENC_OK;
#if 1

          /* Issue callback to application telling how many slices
           * are available. */
#if 0
          if(slice_callback.slicesReady!=(slice_callback.slicesReadyPrev+1))
          {
            //this is used for hardware interrupt.
            slice_callback.slicesReady++;

          }
#endif
          if (hevc_instance->sliceReadyCbFunc &&
              (slice_callback.slicesReadyPrev < slice_callback.slicesReady)&&hevc_instance->rateControl.hrd == ENCHW_NO)
            hevc_instance->sliceReadyCbFunc(&slice_callback);
          slice_callback.slicesReadyPrev = slice_callback.slicesReady;
#endif
          break;
        case ASIC_STATUS_BUFF_FULL:

          hevc_instance->output_buffer_over_flow = 1;
          ret = HEVCENC_OK;
          //inst->stream.overflow = ENCHW_YES;
          break;
        case ASIC_STATUS_HW_RESET:
          ret = HEVCENC_HW_RESET;
          break;
        case ASIC_STATUS_FRAME_READY:
          if(asic->regs.sliceReadyInterrupt)
          {
            if(slice_callback.slicesReady!=asic->regs.sliceNum)
            {
              //this is used for hardware interrupt.
              slice_callback.slicesReady=asic->regs.sliceNum;
            }
            if (hevc_instance->sliceReadyCbFunc &&
              (slice_callback.slicesReadyPrev < slice_callback.slicesReady)&&hevc_instance->rateControl.hrd == ENCHW_NO)
              hevc_instance->sliceReadyCbFunc(&slice_callback);
              slice_callback.slicesReadyPrev = slice_callback.slicesReady;
          }
          sliceNum = 0;
          for (n = pic->slice.tail; n; n = n->next)
          {
            //slice = (struct slice *)pic->sliceInst;

            //*slice->cabac.b.cnt += ((u32 *)(asic->regs.sizeTblBase))[sliceNum];
            //hevc_instance->stream.byteCnt+=((u32 *)(asic->regs.sizeTblBase))[sliceNum];
            //printf("POC %d slice %d size=%d\n", hevc_instance->poc, sliceNum, ((asic->sizeTbl.virtualAddress))[pEncOut->numNalus]);
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
            h2_EncAsicGetRegisterValue(asic->ewl, asic->regs.regMirror, HWIF_ENC_OUTPUT_STRM_BUFFER_LIMIT);
          hevc_instance->stream.byteCnt += asic->regs.outputStrmSize;
          h2_EWLReleaseHw(asic->ewl);  //will.he
          ret = HEVCENC_OK;
          break;

        default:
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
    goto out;
    //queue_put(&c->picture, (struct node *)pic);
    //APITRACE("HEVCEncStrmEncode: OK, Frame discarded (HRD overflow)");
    //return HEVCENC_FRAME_READY;
  }

  {
    i32 stat;

    stat = HevcAfterPicRc(&hevc_instance->rateControl, 0, hevc_instance->stream.byteCnt, 0);

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
      //APITRACE("HEVCEncStrmEncode: OK, Frame discarded (HRD overflow)");
      //return HEVCENC_FRAME_READY;
    }
    else
      hevc_instance->rateControl.sei.activated_sps = 1;
  }

  hevc_instance->frameCnt++;  // update frame count after one frame is ready.

  hevc_instance->encStatus = HEVCENCSTAT_START_FRAME;
  ret = HEVCENC_FRAME_READY;

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

  /* Check for illegal inputs */
  if ((hevc_instance == NULL) || (pEncIn == NULL) || (pEncOut == NULL))
  {
    return HEVCENC_NULL_ARGUMENT;
  }

  /* Check for existing instance */
  if (hevc_instance->inst != hevc_instance)
  {
    return HEVCENC_INSTANCE_ERROR;
  }

  /* Check status, this also makes sure that the instance is valid */
  if ((hevc_instance->encStatus != HEVCENCSTAT_START_FRAME) &&
      (hevc_instance->encStatus != HEVCENCSTAT_START_STREAM))
  {
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

  //APITRACE("HEVCEncSetTestId#");

#ifdef INTERNAL_TEST
  pEncInst->testId = testId;

  //APITRACE("HEVCEncSetTestId# OK");
  return HEVCENC_OK;
#else
  /* Software compiled without testing support, return error always */
  //APITRACE("HEVCEncSetTestId# ERROR, testing disabled at compile time");
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
static i32 out_of_memory(struct hevc_buffer *n, u32 size)
{
  return (size > n->cnt);
}

/*------------------------------------------------------------------------------
  init_buffer
------------------------------------------------------------------------------*/
static i32 init_buffer(struct hevc_buffer *hevc_buffer,
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
static i32 get_buffer(struct buffer *buffer, struct hevc_buffer *source, i32 size,
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
    //APITRACE("hevcCheckCfg: Invalid stream type");
    printf("hevcCheckCfg error. streamType:%d\n", pEncCfg->streamType);
    return ENCHW_NOK;
  }

  /* Encoded image width limits, multiple of 2 */
  if (pEncCfg->width < HEVCENC_MIN_ENC_WIDTH ||
      pEncCfg->width > HEVCENC_MAX_ENC_WIDTH || (pEncCfg->width & 0x1) != 0)
  {
    //APITRACE("hevcCheckCfg: Invalid width");
    printf("pEncCfg->width:%d, HEVCENC_MIN_ENC_WIDTH:%d, HEVCENC_MAX_ENC_WIDTH:%d\n",
        pEncCfg->width,HEVCENC_MIN_ENC_WIDTH, HEVCENC_MAX_ENC_WIDTH);
    return ENCHW_NOK;
  }

  /* Encoded image height limits, multiple of 2 */
  if (height < HEVCENC_MIN_ENC_HEIGHT ||
      height > HEVCENC_MAX_ENC_HEIGHT || (height & 0x1) != 0)
  {
    //APITRACE("hevcCheckCfg: Invalid height");
    printf("height:%d, HEVCENC_MIN_ENC_HEIGHT:%d, HEVCENC_MAX_ENC_HEIGHT:%d \n",
            height, HEVCENC_MIN_ENC_HEIGHT, HEVCENC_MAX_ENC_HEIGHT);
    return ENCHW_NOK;
  }

  /* Encoded image height*width limits */
  if (pEncCfg->width * height > 4096 * 4096)
  {
    //APITRACE("hevcCheckCfg: Invalid height");
    printf("hevcCheckCfg: Invalid height/wid\n");
    return ENCHW_NOK;
  }
#if 0

  /* Scaled image width limits, multiple of 4 (YUYV) and smaller than input */
  if ((pEncCfg->scaledWidth > pEncCfg->width) ||
      (pEncCfg->scaledWidth & 0x7) != 0)
  {
    //APITRACE("hevcCheckCfg: Invalid scaledWidth");
    return ENCHW_NOK;
  }

  if (((i32)pEncCfg->scaledHeight > height) ||
      (pEncCfg->scaledHeight & 0x1) != 0)
  {
    //APITRACE("hevcCheckCfg: Invalid scaledHeight");
    return ENCHW_NOK;
  }
  if ((pEncCfg->scaledWidth == pEncCfg->width) &&
      (pEncCfg->scaledHeight == height))
  {
    APITRACE("hevcCheckCfg: Invalid scaler output, no downscaling");
    return ENCHW_NOK;
  }
#endif

  /* total macroblocks per picture limit */
  if (((height + 63) / 64) * ((pEncCfg->width + 63) / 64) >
      HEVCENC_MAX_CTUS_PER_PIC)
  {
    //APITRACE("hevcCheckCfg: Invalid max resolution");
    printf("hevcCheckCfg: Invalid max resolution");
    return ENCHW_NOK;
  }

  /* Check frame rate */
  if (pEncCfg->frameRateNum < 1 || pEncCfg->frameRateNum > ((1 << 20) - 1))
  {
    printf("hevcCheckCfg: Invalid frameRateNum");
    return ENCHW_NOK;
  }

  if (pEncCfg->frameRateDenom < 1)
  {
    printf("hevcCheckCfg: Invalid frameRateDenom");
    return ENCHW_NOK;
  }

  /* special allowal of 1000/1001, 0.99 fps by customer request */
  if (pEncCfg->frameRateDenom > pEncCfg->frameRateNum &&
      !(pEncCfg->frameRateDenom == 1001 && pEncCfg->frameRateNum == 1000))
  {
    printf("hevcCheckCfg: Invalid frameRate");
    return ENCHW_NOK;
  }

  /* check level */
  if ((pEncCfg->level > HEVCENC_MAX_LEVEL) ||
      (pEncCfg->level < HEVCENC_LEVEL_1))
  {
    printf("hevcCheckCfg: Invalid level");
    return ENCHW_NOK;
  }

  if (pEncCfg->refFrameAmount < 1 ||
      pEncCfg->refFrameAmount > HEVCENC_MAX_REF_FRAMES)
  {
    printf("hevcCheckCfg: Invalid refFrameAmount");
    return ENCHW_NOK;
  }

  /* check on max slice num */
  if ((pEncCfg->sliceSize > (u32)((height + (/*pEncCfg->ctb_size*/64 - 1)) / 64/*pEncCfg->ctb_size*/)))
  {
    printf("sliceSize error. =%d\n", pEncCfg->sliceSize);
    return ENCHW_NOK;
  }

  /* check HW limitations */
  {
    EWLHwConfig_t cfg = h2_EWLReadAsicConfig();
    /* is HEVC encoding supported */
    if (cfg.hevcEnabled == EWL_HW_CONFIG_NOT_SUPPORTED)
    {
      printf("hevcCheckCfg: Invalid format, hevc not supported by HW");
      return ENCHW_NOK;
    }

    /* max width supported */
    if (cfg.maxEncodedWidth < pEncCfg->width)
    {
      printf("hevcCheckCfg: Invalid width, not supported by HW. cfg.maxEncodedWidth(%d) < pEncCfg->width(%d). but cont\n",
        cfg.maxEncodedWidth , pEncCfg->width);
      cfg.maxEncodedWidth = pEncCfg->width;
      //return ENCHW_NOK;
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
static bool_e SetParameter(struct hevc_instance *inst, const HEVCEncConfig   *pEncCfg)
{
  i32 width, height, bps;
  EWLHwConfig_t cfg = h2_EWLReadAsicConfig();
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
  inst->asic.regs.sliceSize = pEncCfg->sliceSize;
  inst->asic.regs.sliceNum = (pEncCfg->sliceSize == 0) ?  1 : ((inst->ctbPerCol + (pEncCfg->sliceSize - 1)) / pEncCfg->sliceSize);

  /* compressor */
  inst->asic.regs.recon_luma_compress = (pEncCfg->compressor & 1) ? 1 : 0;
  inst->asic.regs.recon_chroma_compress = (pEncCfg->compressor & 2) ? 1 : 0;

  /* Sequence parameter set */
  inst->level = pEncCfg->level;
  inst->levelIdx = getlevelIdx(inst->level);

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
  inst->constrained_intra_pred_flag = pEncCfg->constrainedIntraPrediction;
  inst->strong_intra_smoothing_enabled_flag = pEncCfg->strongIntraSmoothing;

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

    rc->qpHdr = HEVCENC_DEFAULT_QP;
    rc->qpMin = 0;
    rc->qpMax = 51;

    rc->frameCoded = ENCHW_YES;
    rc->sliceTypeCur = I_SLICE;
    rc->sliceTypePrev = P_SLICE;
    rc->gopLen = 150;

    /* Default initial value for intra QP delta */
    rc->intraQpDelta = -5;
    rc->fixedIntraQp = 0;

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
#if 0
  inst->preProcess.scaledWidth    = pEncCfg->scaledWidth;
  inst->preProcess.scaledHeight   = pEncCfg->scaledHeight;

  /* Is HW scaling supported */
  if (cfg.scalingEnabled == EWL_HW_CONFIG_NOT_SUPPORTED)
    inst->preProcess.scaledWidth = inst->preProcess.scaledHeight = 0;

  inst->preProcess.scaledOutput   =
    (inst->preProcess.scaledWidth * inst->preProcess.scaledHeight ? 1 : 0);
#endif
  inst->preProcess.adaptiveRoi = 0;
  inst->preProcess.adaptiveRoiColor  = 0;
  inst->preProcess.adaptiveRoiMotion = -5;

  inst->preProcess.colorConversionType = 0;
  h265_EncSetColorConversion(&inst->preProcess, &inst->asic);

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

void LamdaQp(int qp, unsigned int *lamda_sse, unsigned int *lamda_sad)
{
  double recalQP = (double)qp;
  double lambda;

  // pre-compute lambda and QP values for all possible QP candidates
  {
    // compute lambda value
    //Int    NumberBFrames = 0;
    Int    SHIFT_QP = 12;
    //double dLambda_scale = 1.0 - MAX( 0.0, MIN( 0.5, 0.05*(double)NumberBFrames ));
    Int    g_bitDepth = 8;
    Int    bitdepth_luma_qp_scale = 6 * (g_bitDepth - 8);
    double qp_temp = (double) recalQP + bitdepth_luma_qp_scale - SHIFT_QP;

    // Case #1: I or P-slices (key-frame)
    double dQPFactor = 0.8;
    lambda = dQPFactor * pow(2.0, qp_temp / 3.0);
  }

  // store lambda
  double m_dLambda           = lambda;
  double m_sqrtLambda        = sqrt(m_dLambda);
  UInt m_uiLambdaMotionSAD = ((UInt)floor(65536.0 * m_sqrtLambda)) >> 16;

  *lamda_sse = (unsigned int)lambda;
  *lamda_sad = m_uiLambdaMotionSAD;
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
