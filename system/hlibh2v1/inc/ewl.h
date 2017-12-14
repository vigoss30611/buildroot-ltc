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
--                 The entire notice above must be reproduced                                                 --
--                  on all copies and should not be removed.                                                    --
--                                                                                                                               --
--------------------------------------------------------------------------------
--
--  Abstract : H2 Encoder Wrapper Layer for OS services
--
------------------------------------------------------------------------------*/

#ifndef __EWL_H__
#define __EWL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "base_type.h"

  /* Return values */
#define EWL_OK                      0
#define EWL_ERROR                  -1

#define EWL_HW_WAIT_OK              EWL_OK
#define EWL_HW_WAIT_ERROR           EWL_ERROR
#define EWL_HW_WAIT_TIMEOUT         1

  /* HW configuration values */
#define EWL_HW_BUS_TYPE_UNKNOWN     0
#define EWL_HW_BUS_TYPE_AHB         1
#define EWL_HW_BUS_TYPE_OCP         2
#define EWL_HW_BUS_TYPE_AXI         3
#define EWL_HW_BUS_TYPE_PCI         4

#define EWL_HW_BUS_WIDTH_UNKNOWN     0
#define EWL_HW_BUS_WIDTH_32BITS      1
#define EWL_HW_BUS_WIDTH_64BITS      2
#define EWL_HW_BUS_WIDTH_128BITS     3

#define EWL_HW_SYNTHESIS_LANGUAGE_UNKNOWN     0
#define EWL_HW_SYNTHESIS_LANGUAGE_VHDL        1
#define EWL_HW_SYNTHESIS_LANGUAGE_VERILOG     2

#define EWL_HW_CONFIG_NOT_SUPPORTED    0
#define EWL_HW_CONFIG_ENABLED          1

  /* Hardware configuration description */
  typedef struct EWLHwConfig
  {
    u32 maxEncodedWidth; /* Maximum supported width for video encoding  */
    u32 hevcEnabled;     /* HW supports HEVC */
    u32 vp9Enabled;      /* HW supports VP9 */
    u32 vsEnabled;       /* HW supports video stabilization */
    u32 rgbEnabled;      /* HW supports RGB input */
    u32 searchAreaSmall; /* HW search area reduced */
    u32 scalingEnabled;  /* HW supports down-scaling */
    u32 busType;         /* HW bus type in use */
    u32 busWidth;
    u32 synthesisLanguage;
  } EWLHwConfig_t;


  /* Allocated linear memory area information */
  typedef struct EWLLinearMem
  {
    u32 *virtualAddress;
    u32 busAddress;
    u32 size;
    u32 uid;
  } EWLLinearMem_t;

  /* h2_EWLInitParam is used to pass parameters when initializing the EWL */
  typedef struct h2_EWLInitParam
  {
    u32 clientType;
  } h2_EWLInitParam_t;

#define EWL_CLIENT_TYPE_HEVC_ENC         1U
#define EWL_CLIENT_TYPE_VP9_ENC          2U

  /*------------------------------------------------------------------------------
      4.  Function prototypes
  ------------------------------------------------------------------------------*/

  /* Read and return the HW ID register value, static implementation */
  u32 h2_EWLReadAsicID(void);

  /* Read and return HW configuration info, static implementation */
  EWLHwConfig_t h2_EWLReadAsicConfig(void);

  /* Initialize the EWL instance
   * Returns a wrapper instance or NULL for error
   * h2_EWLInit is called when the encoder instance is initialized */
  const void *h2_EWLInit(h2_EWLInitParam_t *param);

	int h2_EWLOff(const void *inst);
	int h2_EWLOn(const void *inst);	
	int h2_EWLInitOn(void);	
	
  /* Release the EWL instance
   * Returns EWL_OK or EWL_ERROR
   * h2_EWLRelease is called when the encoder instance is released */
  i32 h2_EWLRelease(const void *inst);

  /* Reserve the HW resource for one codec instance
   * h2_EWLReserveHw is called when beginning a frame encoding
   * The function may block until the resource is available.
   * Returns EWL_OK if the resource was successfully reserved for this instance
   * or EWL_ERROR if unable to reserve the resource. */
  i32 h2_EWLReserveHw(const void *inst);

  /* Release the HW resource
   * h2_EWLReleaseHw is called when the HW has finished the frame encoding.
   * The codec SW will continue the frame encoding but the HW can
   * be used by another codec.*/
  void h2_EWLReleaseHw(const void *inst);

  /* Frame buffers memory */
  i32 h2_EWLMallocRefFrm(const void *inst, u32 size, EWLLinearMem_t *info);
  void h2_EWLFreeRefFrm(const void *inst, EWLLinearMem_t *info);

  /* SW/HW shared memory */
  i32 h2_EWLMallocLinear(const void *inst, u32 size, EWLLinearMem_t *info);
  void h2_EWLFreeLinear(const void *inst, EWLLinearMem_t *info);

  /* D-Cache coherence *//* Not in use currently */
  void h2_EWLDCacheRangeFlush(const void *instance, EWLLinearMem_t *info);
  void h2_EWLDCacheRangeRefresh(const void *instance, EWLLinearMem_t *info);

  /* Write value to a HW register
   * All registers are written at once at the beginning of frame encoding
   * Offset is relative to the the HW ID register (#0) in bytes
   * Enable indicates when the HW is enabled. If shadow registers are used then
   * they must be flushed to the HW registers when enable is '1' before
   * writing the register that enables the HW */
  void h2_EWLWriteReg(const void *inst, u32 offset, u32 val);

  /* Read and return the value of a HW register
   * The status register is read after every macroblock encoding by SW
   * The other registers which may be updated by the HW are read after
   * BUFFER_FULL or FRAME_READY interrupt
   * Offset is relative to the the HW ID register (#0) in bytes */
  u32 h2_EWLReadReg(const void *inst, u32 offset);

  /* Writing all registers in one call *//*Not in use currently */
  void h2_EWLWriteRegAll(const void *inst, const u32 *table, u32 size);
  /* Reading all registers in one call *//*Not in use currently */
  void h2_EWLReadRegAll(const void *inst, u32 *table, u32 size);

  /* HW enable/disable. This will write <val> to register <offset> and by */
  /* this enablig/disabling the hardware. */
  void h2_EWLEnableHW(const void *inst, u32 offset, u32 val);
  void h2_EWLDisableHW(const void *inst, u32 offset, u32 val);

  /* Synchronize SW with HW
   * Returns EWL_HW_WAIT_OK, EWL_HW_WAIT_ERROR or EWL_HW_WAIT_TIMEOUT
   * h2_EWLWaitHwRdy is called after enabling the HW to wait for IRQ from HW.
   * If slicesReady pointer is given, at input it should contain the number
   * of slicesReady received. The function will return when the HW has finished
   * encoding next slice. Upon return the slicesReady pointer will contain
   * the number of slices that are ready and available in the HW output buffer.
   */
  
  i32 h2_EWLWaitHwRdy(const void *inst, u32 *slicesReady,u32 totalsliceNumber,u32* status_register);
  

  /* SW/SW shared memory handling */
  void *h2_EWLmalloc(u32 n);
  void *h2_EWLcalloc(u32 n, u32 s);
  void h2_EWLfree(void *p);
  void *h2_EWLmemcpy(void *d, const void *s, u32 n);
  void *h2_EWLmemset(void *d, i32 c, u32 n);
  int h2_EWLmemcmp(const void *s1, const void *s2, u32 n);

#ifdef __cplusplus
}
#endif
#endif /*__EWL_H__*/
