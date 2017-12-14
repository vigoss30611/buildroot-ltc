
#ifndef VCPAPI_H
#define VCPAPI_H

/* ==================================================================== */
/*  File:       vcpAPI.h                                                */
/*  Purpose:    Alango VCP object integration interface                 */
/*                                                                      */
/*  Description:                                                        */
/*              This file contains VCP API definitions and description. */
/* ==================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/* ==================================================================== */
/*                                                                      */
/*  VCP CONSTANTS                                                       */
/*                                                                      */
/* ==================================================================== */


#define VCP_NUM_MEM_REGIONS		1
#define VCP_PROFILE_VER         36

#define VCP_VERSION_STRING	"Alango Voice Communication Package generation Seven (VCP7G)"


/* ---- VCP ID of processing blocks ----                                      */
		/* === DEBUG BLOCKS === */
#define VCP_BLOCK_ID_LOGGER				(0)    /* VCP Block Logger               */
#define VCP_BLOCK_ID_DCD				(1)	   /* VCP Block DCD                  */
        /* === PRIMARY (TX) SIGNAL === */
#define VCP_BLOCK_ID_TX_INDRC			(2)	 	/* VCP TX Block Input DRC         */
#define VCP_BLOCK_ID_TX_ADM				(3)	   /* VCP TX Block ADM               */
#define VCP_BLOCK_ID_TX_ADMOUT			(4)	   /* VCP TX Block ADM direct output */
#define VCP_BLOCK_ID_TX_ADM_FT			(5)	   /* VCP TX Block ADM Far Talk      */
#define VCP_BLOCK_ID_TX_ADM_NT			(6)	   /* VCP TX Block ADM Near Talk     */
#define VCP_BLOCK_ID_TX_ADM_BS			(7)	   /* VCP TX Block ADM Broad Side    */
#define VCP_BLOCK_ID_TX_AF				(8)	   /* VCP TX Block Adaptive Filters  */
#define VCP_BLOCK_ID_TX_ES				(9)	 	/* VCP TX Block Echo Suppressor   */
#define VCP_BLOCK_ID_TX_NS				(10)   /* VCP TX Block Noice Suppressor  */
#define VCP_BLOCK_ID_TX_WRNS 			(11)   /* VCP TX Block Wide Band Residual Noice Suppressor  */
#define VCP_BLOCK_ID_TX_QPFS			(12)   /* VCP TX Block Frequency Shift   */
#define VCP_BLOCK_ID_TX_EQ				(13) 	/* VCP TX Block Equalizer         */
#define VCP_BLOCK_ID_TX_EQREF			(14) 	/* VCP TX Block REF Equalizer     */
#define VCP_BLOCK_ID_TX_OUT_DRC_AGC		(15)	/* VCP TX Block output DRC1, AGC, DRC2 */	
#define VCP_BLOCK_ID_TX_HG				(16)	/* VCP TX Block Harmonic Generator*/	
#define VCP_BLOCK_ID_TX_ST				(17)	/* VCP TX Block Side Tone Mixer   */	
#define VCP_BLOCK_ID_TX_DM				(18)   /* VCP TX Block Dynamic Managment */
        /* === REFERENCE (RX) SIGNAL === */
#define VCP_BLOCK_ID_RX_PROC			(19)   /* VCP RX Processing              */
#define VCP_BLOCK_ID_RX_BAND_PROC		(21)   /* VCP RX Band Processing         */
#define VCP_BLOCK_ID_RX_NS				(22)   /* VCP RX Block Noice Suppressor  */
#define VCP_BLOCK_ID_RX_WRNS 			(23)   /* VCP RX Block Wide Band Residual Noice Suppressor  */
#define VCP_BLOCK_ID_RX_DTGC			(24)   /* VCP RX Block DTGC              */
#define VCP_BLOCK_ID_RX_NDVC			(25)   /* VCP RX Block NDVC              */
#define VCP_BLOCK_ID_RX_DRC_AGC	        (26) 	/* VCP RX Block DRC, AGC          */
#define VCP_BLOCK_ID_RX_AVQ				(27)   /* VCP RX Block AVQ               */
#define VCP_BLOCK_ID_RX_EL				(28)   /* VCP RX Block EL                */
#define VCP_BLOCK_ID_RX_EQ				(29)   /* VCP RX Block Equalizer         */
#define VCP_BLOCK_ID_RX_DM				(30)   /* VCP RX Block Dynamic Managment */
											   

/* ---- VCP bit-flags for VCP_ID_FLAGS0, 1/0 (on/off) ----  */
#define VCP_AF_ON                   0x0001    /*  bit 0:  AFflag     - AF block, default 1 */
#define VCP_ES_ON                   0x0002    /*  bit 1:  ESflag     - ES block, default 1 */
#define VCP_CN_ON                   0x0004    /*  bit 2:  CNflag     - CN block, default 1 */
#define VCP_FOST_ON                 0x0008    /*  bit 3:  FOSTflag   - AF output sample type - 1/0 (1 - last filtering stage result, 0 - minimal for two stages) default 0 */
#define VCP_TXDCR_ON				0x0010	  /*  bit 4:  TX_DCRflag - remove TX DC at inputs */
#define VCP_RXDCR_ON				0x0020	  /*  bit 5:  RX_DCRflag - remove RX DC at input  */
#define VCP_INDRC_ON                0x0040    /*  bit 6:  INDRCflag  - Input DRC, default 1 */
#define VCP_OUTDRC1_ON              0x0080    /*  bit 7:  OUTDRC1flag- Output 1st DRC, default 1 */
#define VCP_OUTDRC2_ON              0x0100    /*  bit 8:  OUTDRC2flag- Output 2nd DRC, default 0 */
#define VCP_NDVC_ON                 0x0200    /*  bit 9:  NDVCflag   - NDVC block, default 0 */
#define VCP_RX_ON                   0x0400    /*  bit 10: RXflag     - Basic RX processing, default 1 */
#define VCP_RX_BANDS_ON             0x0800    /*  bit 11: RXBflag    - Subband RX signal processing blocks (NS, EQ), default 1 */
#define VCP_RX_AVQ_ON               0x1000    /*  bit 12: RXAVQflag  - AVQ block, default 0, when enabled, RXflag and RXBflag should be also enabled */
#define VCP_RX_EL_ON                0x2000    /*  bit 13: RXELflag   - EL block, default 0 */
#define VCP_TX_ESNT_ON              0x4000    /*  bit 14: ESNTflag   - apply special ES parameters for near talk, default 0 */
#define VCP_DREQ_ON                 0x8000    /*  bit 15: DREQflag   - dynamic reference signal equalizer, default 0 */

/* ---- VCP bit-flags for VCP_ID_FLAGS1, 1/0 (on/off) ----  */
#define VCP_TX_NS_ON                0x0001    /*  bit 0:  NSflag     - TX NS block, default 1 */
#define VCP_RX_NS_ON                0x0002    /*  bit 1:  RXNSflag   - RX NS block, default 1 */
#define VCP_ESRC_ON                 0x0004 	  /*  bit 2:  ESRCflag   - ES reference control, default 1 */
#define VCP_RX_DISABLE				0x0008    /*  bit 3:  RXDflag    - RX disable, default 0, for evcp mode */
#define VCP_AF_TSF				    0x0010    /*  bit 4:  TSFflag    - two stages AF flag, default 1 */
#define VCP_TX_AGC_ON               0x0020    /*  bit 5:  AGCflag    - TX AGC block, default 1 */
#define VCP_RX_AGC_ON               0x0040    /*  bit 6:  RXAGCflag  - RX AGC block, default 1 */
#define VCP_RX_DRC_ON               0x0080    /*  bit 7:  RXDRCflag  - RX DRC block, default 1 */
#define VCP_TX_GDNR_ON              0x0100    /*  bit 8:  GDNRflag   - TX channel, Gain Dependant Noise Reduction, default 1 */
#define VCP_RX_GDNR_ON              0x0200    /*  bit 9:  GDNRflag   - RX channel, Gain Dependant Noise Reduction, default 1 */
#define VCP_PM_ON                   0x0400    /*  bit 10: PMflag     - AF Protected mode on/off, default 1 */
#define VCP_TX_WRNS_ON              0x0800    /*  bit 11: WRNSflag   - TX WRNS block, default 1 */
#define VCP_RX_WRNS_ON              0x1000    /*  bit 12  RXWRNSflag - RX WRNS block, default 1 */
#define VCP_DTGC_ON                 0x2000    /*  bit 13: RXDTGCflag - RX DTGC block, default 0 */
#define VCP_ND_ON                   0x4000    /*  bit 14: NDflag     - ES Nonlinear distortions (ND) block,  default 1 */
#define VCP_BCTEQ_ON                0x8000    /*  bit 15: BCTflag    - ES Individual BCT settings for each frequency band, default 0 */

/* ---- VCP bit-flags for VCP_ID_FLAGS2, 1/0 (on/off) ----  */
#define VCP_TX_DM_ON                0x0001    /*  bit 0:  TX_DMflag  - TX dynamic management flag, default 1 */
#define VCP_DCD_ON                  0x0004    /*  bit 2:  DCDflag    - delay change detector flag, default 0 */
#define VCP_LOG_ON                  0x0008    /*  bit 3:  LOGflag    - logger flag, default 0 */
#define VCP_TX_SB_ON                0x0010    /*  bit 4:  TX_OBflag    TX channel, Saturation Beep flag, default 0 */
#define VCP_RX_SB_ON                0x0020    /*  bit 5:  RX_OBflag    RX channel, Saturation Beep flag, default 0 */
#define VCP_IL_ON                   0x0040    /*  bit 6:  interleaving flag, for input samples, default 0 */
#define VCP_ST_ON                   0x0080    /*  bit 7:  start flag (at the work begining: bypass while input signal too small), default 1 */
#define VCP_AFAH_ON                 0x0100    /*  bit 8:  AFAHflag   - AF block, activity hold flag, default 0 */
#define VCP_AFST_ON                 0x0200    /*  bit 9:  AFSTflag   - AF stereo reference signal flag, default 0 */
#define VCP_TX_LFNF_ON              0x0400    /*  bit 10: TX NS Low Frequencies Noise Flattening, default 0 */
#define VCP_RX_LFNF_ON              0x0800    /*  bit 11: TX NS Low Frequencies Noise Flattening, default 0 */
#define VCP_TX_AFCCA_ON             0x1000    /*  bit 12: TX AF, AF correlation-controlled adaptation, default 0 */

/* ---- VCP bit-flags for VCP_ID_TX_ADMFLAGS, 1/0 (on/off) ----  */
#define VCP_ADM_ON                  0x0001    /*  bit  0: ADMflag    - ADM block, default 0 */
#define VCP_ADMOUT_ON               0x0002    /*  bit  1: ADMOUTflag - output of ADM block writes after TX output frame, default 0 */
#define VCP_ADMNLP_ON               0x0004    /*  bit  2: ADMNLPflag - ADM NLP block, default 1 */
#define VCP_ADMLIM_ON               0x0008    /*  bit  3: ADMLIMflag - ADM Limiter block, default 1 */
#define VCP_ADMSENS_ON              0x0010    /*  bit  4: ADMSENSflag - ADM mic. sensitivity equalization, default 0 */
#define VCP_ADMAM_ON                0x0020    /*  bit  5: ADMAMflag  - ADM activity mode, default 0 */
#define VCP_ADMES_ON                0x0040    /*  bit  6: ADMESflag  - ADM  echo suppressor mode, default 0 */
#define VCP_ADMFRC_ON               0x0080    /*  bit  7: ADMFRCflag - ADM frequency compensation for far talk, default 1 */
#define VCP_ADMREVM_ON              0x0100    /*  bit  8: ADMREVMflag - ADM input reverse microphones flag, default 0 */
#define VCP_ADMREVO_ON              0x0200    /*  bit  9: ADMREVOflag - ADM reverse output signal flag, default 0 */
#define VCP_ADMAFRC_ON              0x0400    /*  bit 10: ADMAFRCflag - ADM adaptive frequency compensation for far talk, default 1 */
#define VCP_ADMRM_ON                0x0800    /*  bit 11: ADMRMflag   - ADM reference mode, default 0 */

/* ---- VCP error codes ---- */
#define VCP_NO_ERROR                 0        /* no error */
#define VCP_ERROR_OUT_OF_MEMORY     -1        /* Not enough memory for profile */
#define VCP_ERROR_IN_PROGRESS       -2        /* Attempt to change a critical data while processing */
#define VCP_ERROR_NOT_INIT_OBJ      -3        /* The object can't be used yet (VCP is not initialized) */
#define VCP_ERROR_NULL_POINTER      -4        /* Object memory is absent */
#define VCP_ERROR_NO_SUCH_PARAM     -5        /* Unsupported parameter number */
#define VCP_ERROR_PROF_NOT_EXIST    -6        /* Unsupported profile number */
#define VCP_ERROR_TOO_BIG           -7        /* Parameter value is above its range */
#define VCP_ERROR_TOO_SMALL         -8        /* Parameter value is below its range */
#define VCP_ERROR_INVALID_VALUE     -9        /* Parameter value is unaccaptable */
#define VCP_ERROR_WRONG_DATA_LEN   -10        /* Data length is not divisible by frame length */
#define VCP_ERROR_CHANGE_DENIED    -11        /* Parameter value cannot be changed (VCP reinit. is required) */
#define VCP_ERROR_OBJECT_CORRUPTED -12        /* VCP object corrupted */
#define	VCP_ERROR_API_ARG_OUT_OF_RANGE  -13   /* Error in parameter(s) of VCP API function - argument value is out of range */
#define	VCP_ERROR_WRONG_PROFILE    -14        /* Supplied VCP profile structure does not correspond to the VCP lib version */
#define	VCP_ERROR_MODE_NOT_SUPPORTED    -15   /* Mode not supprorted in current VCP version of compilation */
#define	VCP_ERROR_MODE_NOT_INIT    -16        /* Mode not initialized or not started */
#define	VCP_ERROR_CRC              -17        /* wrong value of profile CRC checksum */
#define	VCP_ERROR_FEATURE_NOT_AVAILABLE  -18  /* DSP feature or processing block unavailable by compilation conditions */
#define VCP_ERROR_INVALID_LICENSE	-19		  /* Invalid license. VCP object will not be created */
#define VCP_ERROR_DEMO_END			-20		  /* Demo period expired */
#define VCP_ERROR_STEREO_REF_PROCESSING  -21  /* RX processsing disabled, if reference is stereo signal */
#define VCP_ERROR_WRONG_DATA_TYPE   -22       /* variable has wrong data type */

/* ---- VCP I/O debug options (controlled by VCP_ID_CONNECT)  ---- */
#define VCP_CONNECT_NORMAL           0        /* normal mode            */
#define VCP_CONNECT_BYPASS           1        /* algorithm bypass mode  */
#define VCP_CONNECT_SINE             2        /* sine generation        */
#define VCP_CONNECT_ZERO             3        /* zero-signal generation */
#define VCP_CONNECT_SWEEP            4        /* sweep generation       */
#define VCP_CONNECT_WHITE_NOISE      5        /* white noise            */

#define VCP_CONNECT_MAX				 5

/* ---- VCP channel identifiers ---- */
#define VCP_RX_CHANNEL               0        /* Rx, Reference channel  */
#define VCP_TX_CHANNEL               1        /* Tx, Primary channel    */

/* ---- VCP initialization methods ----*/
#define VCP_INIT_STANDARD            1        /* VCP object in one memory segment */
#define VCP_INIT_EXTENDED            2        /* VCP object in two memory segments */

/* ---- VCP identifiers for data items ---- */
#define VCP_ID_DATA_VERSION_NUMBER   0        /* VCP version number     */
#define VCP_ID_DATA_VERSION          1        /* VCP version identifiers*/
#define VCP_ID_DATA_TX_VAD           2        /* VAD value on TX channel*/
#define VCP_ID_DATA_RX_VAD           3        /* VAD value on RX channel*/
#define VCP_ID_DATA_SCRATCH_MEM      7        /* scratch memory size, bytes, for functions vcpObjMemSizeExt(), vcpObjMemSizeEx1() */

/* ---- VCP EQ array packed size ----*/
#define EQC_PACKED                  (128/4)
#define EQRXC_PACKED                (256/4)

/* ---- VCP logger parametes array ----*/
#define LOG_SIZE                     16		   /* size in 16-bit words */
#define UD_SIZE                      8		   /* size in 16-bit words */

/* -------------------------------------------------------------------- */
/*  VCP identifiers for profile items                                   */
/* -------------------------------------------------------------------- */

#define VCP_ID_VER                   0  /* VCP profile version */

/* === General VCP parameters === */
#define VCP_ID_FSF                   1  /* Sampling frequency factor */
#define VCP_ID_FLAGS0                2  /* VCP bit-flags, 1/0 (on/off)
												bit 0:  AFflag     - AF block, default 1
												bit 1:  ESflag     - ES block, default 1
												bit 2:  CNflag     - CN block, default 1
												bit 3:  FOSTflag   - AF output sample type - 1/0 (1 - last filtering stage result, 0 - minimal for two stages) default 0
									            bit 4:  TX_DCRflag - Remove DC at TX inputs
									            bit 5:  RX_DCRflag - Remove DC at RX input
												bit 6:  INDRCflag  - Input DRC, default 1
												bit 7:  OUTDRC1flag- Output 1st DRC, default 1
												bit 8:  OUTDRC2flag- Output 2nd DRC, default 0
												bit 9:  NDVCflag   - NDVC block, default 0
												bit 10: RXflag     - Basic RX processing, default 1
												bit 11: RXBflag    - Subband RX signal processing blocks (NS, EQ), default 1
												bit 12: RXAVQflag  - AVQ block, default 0, when enabled, RXflag and RXBflag should be also enabled
												bit 13: RXELflag   - Easy Listen block, default 0
												bit 14: ESALTflag  - apply special ES parameters for near talk, default 0
												bit 15: DREQflag   - dynamic reference signal equalizer, default 0
										*/
#define VCP_ID_FLAGS1                3  /* VCP bit-flags, 1/0 (on/off)
												bit 0:  NSflag     - NS block, default 1
												bit 1:  RXNSflag   - RX NS block, default 1
												bit 2:  ESRCflag   - ES reference control, default 1
												bit 3:  RXDflag    - RX disable, default 0, for evcp mode
									            bit 4:  TSFflag    - two stages AF flag, default 1
												bit 5:  AGCflag    - TX channel, AGC block, default 1
												bit 6:  RXAGCflag  - RX channel, AGC block, default 1
												bit 7:  RXDRCflag  - RX channel, DRC, default 1
												bit 8:  GDNRflag   - TX channel, Gain Dependant Noise Reduction, default 1
												bit 9:  RXGDNRflag - RX channel, Gain Dependant Noise Reduction, default 1
												bit 10: PMflag     - AF Protected Mode, default 1
												bit 11: WRNSflag   - TX WRNS block, default 1
												bit 12: RXWRNSflag - RX WRNS block, default 1
												bit 13: RXDTGCflag - RX DTGC block, default 0
												bit 14: NDflag     - ES Nonlinear distortions (ND) block, default 1
												bit 15: BCTflag    - ES Individual BCT settings for each frequency band, default 0
										*/
#define VCP_ID_FLAGS2                4  /* VCP bit-flags, 1/0 (on/off)
												bit 0:  TX_DMflag     - TX dynamic management flag, default 1
												bit 1:  RX_DMflag     - RX dynamic management flag, default 1
									            bit 2:  DCDflag       - delay change detector flag, default 0
									            bit 3:  LOGflag       - logger flag, default 0
									            bit 4:  TX_SBflag     - TX channel, Saturation Beep, default 0
									            bit 5:  RX_SBflag     - RX channel, Saturation Beep, default 0
												bit 6:  TX_ILfag      - interleaving flag, for input TX samples, default 0
												bit 7:  STflag        - start flag (at the work begining: bypass while input signal too small), default 1
												bit 8:  TX_AFAHflag   - AF Activity Hold flag, default 0
												bit 9:  TX_AFSTflag   - AF stereo reference signal flag, default 0
												bit 10: TX_LFNFflag   - TX NS Low Frequencies Noise Flattening
												bit 11: RX_LFNFflag   - RX NS Low Frequencies Noise Flattening
												bit 12: AFCCAflag     - AF correlation-controlled adaptation, default 0
												bit 13:
												bit 14:
												bit 15:
										*/
#define VCP_ID_CONNECT               5  /* VCP connection configuration (16 bit word):
                                           struct {
                                                  short TXin: 3;
                                                  short PF: 1;
                                                  short TXout: 4;
                                                  short RXin: 4;
                                                  short RXout: 4;
                                           }
                                          * TXin, TXout, RXin, RXout can be VCP_CONNECT_NORMAL, VCP_CONNECT_BYPASS, VCP_CONNECT_SINE, VCP_CONNECT_ZERO, VCP_CONNECT_SWEEP, VCP_CONNECT_WHITE_NOISE
                                          0 - VCP_CONNECT_NORMAL - normal connection mode;
                                          1 - VCP_CONNECT_BYPASS - VCP bypass mode;
                                          2 - VCP_CONNECT_SINE   - sine tone generation (1000 Hz, -6 dB);
                                          3 - VCP_CONNECT_ZERO   - zero signal generation;
                                          4 - VCP_CONNECT_SWEEP  - sweep tone signal generation;
                                          5 - VCP_CONNECT_WHITE_NOISE - pulse of white noise signal generation
                                          * PF is a flag denoting VCP processing 1/0 (enabled/disabled)
                                         */
#define VCP_ID_RPD                   6   /* Reference (Rx) to Primary (Tx) Delay, ms */
#define VCP_ID_UDF                   7   /* Upper Duplex Frequency, Hz */
#define VCP_ID_TX_HDTIME             8   /* Begin half-duplex/compression time, ms */

        /* === PRIMARY (TX) SIGNAL === */

        /* --- Input DRC config --- */
#define VCP_ID_TX_INDRC               9   /* Input DRC saturation threshold, dB */
#define VCP_ID_TX_INDRCBEND          10  /* Input DRC bend threshold, dB */
#define VCP_ID_TX_INDRCNGIL          11  /* Input DRC noise gate input level, dB */
#define VCP_ID_TX_INDRCNGOL          12  /* Input DRC noise gate output level, dB */
#define VCP_ID_TX_IG                 13  /* Gain after the Input DRC, dB */
        /* --- EC (adaptive filters) config --- */
#define VCP_ID_TX_AFL1               14  /* Effective length of adaptive filter, ms */
#define VCP_ID_TX_AFL2               15  /* Effective length of adaptive filter for high frequencies, ms */
#define VCP_ID_TX_ARF                16  /* Adaptation rate factor for adaptive filter */
#define VCP_ID_TX_AFUF               17  /* Adaptive filter upper frequency for length AFL1, hz */
#define VCP_ID_TX_REFG               18  /* Rx reference signal gain (external volume control), dB */
#define VCP_ID_TX_PMTHR              19  /* Protected mode threshold for adaptive filter 1, dB */
#define VCP_ID_TX_PMUF               20  /* Protected mode upper frequency, Hz */
#define VCP_ID_TX_ERLIF              21  /* Protected mode ERL increase factor  */

        /* --- ES - general config --- */
#define VCP_ID_TX_BCT                22  /* Band Cancellation Threshold, dB */
#define VCP_ID_TX_GCT                23  /* Global Cancellation Threshold, dB */
#define VCP_ID_TX_BCT_ALT            24  /* Band Cancellation Threshold for near talk, dB */
#define VCP_ID_TX_GCT_ALT            25  /* Global Cancellation Threshold for near talk, dB */
#define VCP_ID_TX_OBT                26  /* Open bands threshold, defines minimal allowed amount of open bands in ES */
#define VCP_ID_TX_ESOTHR             27  /* ES output low level signal threshold */
#define VCP_ID_TX_RRT                28  /* Room reverberation time, ms */
#define VCP_ID_TX_NBITSC             29  /* Number of bits for scaling signal after ec */
#define VCP_ID_TX_NSHG		         30  /* Near speach harmonics generation factor */
#define VCP_ID_TX_CNG		         31  /* Comfort noise gain, dB */
#define VCP_ID_TX_UDFTHR             32  /* Reference signal activity level threshold for bands above udf, dB */
        /* --- ES - non-linear distortions config --- */
#define VCP_ID_TX_NDF1               33  /* Bottom frequency producing non-linear distortions, Hz */
#define VCP_ID_TX_NDF2               34  /* Top frequency producing non-linear distortions, Hz */
#define VCP_ID_TX_NDF3               35  /* Top non-linear distortions region frequency, Hz */
#define VCP_ID_TX_NDC                36  /* Non-linear distortions coefficient */
#define VCP_ID_TX_NDC_ALT            37   /* Non-linear distortions coefficient for near talk */
#define VCP_ID_TX_NDCR               38  /* NDC release (in time) coefficient */
#define VCP_ID_TX_RSL                39  /* Reference (Rx) saturation level, dB */
#define VCP_ID_TX_RSNDC              40  /* Special NDC value for oversaturated Reference (Rx) */
#define VCP_ID_TX_RSNDC_ALT          41   /* Special NDC value for oversaturated Reference (Rx) for near talk*/
#define VCP_ID_TX_NDCS               42  /* NDC decrease step in case of Reference signal decrease */
#define VCP_ID_TX_PSL                43  /* Primary (Tx) saturation level, dB */
#define VCP_ID_TX_REFTHR             44  /* Reference signal activity level threshold, dB */
        /* --- Noise Suppressor config --- */
#define VCP_ID_TX_NSPR               45  /* Noise Suppression Processing  0-full, 1-moderate, 2-lite */
#define VCP_ID_TX_SNS                46  /* Stationary Noise Suppression, dB */
#define VCP_ID_TX_WBNGG              47  /* Wide band noise gate gain, dB */
#define VCP_ID_TX_NSTSMG             48  /* Tone Suppression minimal gain, dB */
#define VCP_ID_TX_NSTDATHR           49  /* Tone detection absolute threshold, dB */
#define VCP_ID_TX_NSTDRTHR	         50  /* Tone detection relative threshold, dB */
#define VCP_ID_TX_BNL                51  /* Band noise level threshold, dB */
#define VCP_ID_TX_BNL_MING           52  /* Minimal allowed BNL gain, dB */
#define VCP_ID_TX_BNLEF              53  /* BNL spectrum curvature factor, dB */
#define VCP_ID_TX_SNAT               54  /* Stationary Noise Adaptation Time, ms */
#define VCP_ID_TX_TNAT               55  /* Transient Noise Adaptation Time, ms */
#define VCP_ID_TX_NTF1               56  /* Noise Threshold Factor, first band, float */
#define VCP_ID_TX_NTF2               57  /* Noise Threshold Factor, last band,  float */
#define VCP_ID_TX_NSS                58  /* Noise Suppression Smoothness, float */
#define VCP_ID_TX_NSHRUF             59  /* TX Noise Suppression High Resolution Upper Frequency, Hz */
#define VCP_ID_TX_NSUBF              60  /* TX Noise Suppression Upper Band Frequency, Hz */
#define VCP_ID_TX_SHER               61  /* TX Noise Suppression Speech Harmonics Enhancement and Restoration factor, float */

        /* --- Output DRC/AGC/gain config --- */
#define VCP_ID_TX_OUTDRC1            62  /* Output 1st DRC factor (saturation or boost), dB */
#define VCP_ID_TX_OUTDRC1BEND        63  /* Output 1st DRC bend threshold, dB */
#define VCP_ID_TX_OUTDRC1NGIL        64  /* Output 1st DRC noise gate input level, dB */
#define VCP_ID_TX_OUTDRC1NGOL        65  /* Output 1st DRC noise gate output level, dB */
#define VCP_ID_TX_OUTDRC2            66  /* Output 2nd DRC factor (saturation or boost), dB */
#define VCP_ID_TX_OUTDRC2BEND        67  /* Output 2nd DRC bend threshold, dB */
#define VCP_ID_TX_OUTDRC2NGIL        68  /* Output 2nd DRC noise gate input level, dB */
#define VCP_ID_TX_OUTDRC2NGOL        69  /* Output 2nd DRC noise gate output level, dB */
#define VCP_ID_TX_AGC                70  /* Automatic Gain Control (AGC) maximal gain, dB */
#define VCP_ID_TX_AGCT               71  /* Time to double AGC gain, ms */
#define VCP_ID_TX_AGCSAT             72  /* AGC saturation signal level, dB */
#define VCP_ID_TX_AGCTAR             73  /* AGC target signal level, dB */
#define VCP_ID_TX_AGCTHR             74  /* AGC actuation signal level threshold, dB */
#define VCP_ID_TX_AGCFADE            75  /* AGC fade factor, sec */
#define VCP_ID_TX_AGCVADTHR          76  /* AGC VAD treshold */
#define VCP_ID_TX_OG                 77  /* TX output gain, dB */
#define VCP_ID_TX_WRNS               78  /* Wideband residual noise suppressor threshold, dB */
        /* --- Side Tone, Frequency Shift --- */
#define VCP_ID_STG		             79  /* Side tone gain, dB */
#define VCP_ID_FRQSH                 80  /* Frequency Shift, percent */
        /* --- Dynamic control config --- */
#define VCP_ID_TX_AFTHR		         81  /* AF reference threshold, dB */
#define VCP_ID_TX_ESTHR              82  /* ES reference threshold, dB */

        /* === REFERENCE (RX) SIGNAL === */

        /* --- General parameters --- */
#define VCP_ID_RX_IG                 83  /* Rx input gain, dB */
#define VCP_ID_RX_DTGCATT            84  /* Rx gain attenuation value, dB */
#define VCP_ID_RX_DTGCDT             85  /* Gain decrease time, ms */
#define VCP_ID_RX_DTGCIT             86  /* Gain increase time, ms */
#define VCP_ID_RX_DTGCHT             87  /* Attenuated gain hold time, ms */ 
#define VCP_ID_RX_DTGCHP	         88  /* DTGC high pass gain, dB */
#define VCP_ID_RX_DTGCTHR            89  /* DTGC threshold , dB */
#define VCP_ID_RX_VOL		         90  /* RX volume, dB */
        /* --- AVQ config --- */
#define VCP_ID_RX_AVQNMIN            91  /* Minimal noise level, dB */
#define VCP_ID_RX_AVQNMAX            92  /* Maximal noise level, dB */
#define VCP_ID_RX_AVQRANGE           93  /* AVQ range (defines maximal gain), dB */
#define VCP_ID_RX_AVQIFAC            94  /* Smoothing factor for raising gain, ms */
#define VCP_ID_RX_AVQDFAC            95  /* Smoothing factor for falling gain, ms */
        /* --- Noise Suppressor config --- */
#define VCP_ID_RX_NSPR               96  /* Noise Suppression Processing  0-full, 1-moderate, 2-lite */
#define VCP_ID_RX_SNS                97  /* Stationary Noise Suppression, dB */
#define VCP_ID_RX_WBNGG              98  /* Wide band noise gate gain, dB */
#define VCP_ID_RX_NSTSMG             99  /* Tone Suppression minimal gain, dB */
#define VCP_ID_RX_NSTDATHR          100  /* Tone detection absolute threshold, dB */
#define VCP_ID_RX_NSTDRTHR	        101  /* Tone detection relative threshold, dB */
#define VCP_ID_RX_BNL               102  /* Band noise level threshold, dB */
#define VCP_ID_RX_BNL_MING          103  /* Minimal allowed BNL gain, dB */
#define VCP_ID_RX_BNLEF             104  /* BNL spectrum curvature factor, dB */
#define VCP_ID_RX_SNAT              105  /* Stationary Noise Adaptation Time, ms */
#define VCP_ID_RX_TNAT              106  /* Transient Noise Adaptation Time, ms */
#define VCP_ID_RX_NTF1              107  /* Noise Threshold Factor, first band, float */
#define VCP_ID_RX_NTF2              108  /* Noise Threshold Factor, last band,  float */
#define VCP_ID_RX_NSS               109  /* Noise Suppression Smoothness, float */
#define VCP_ID_RX_NSHRUF            110  /* RX Noise Suppression High Resolution Upper Frequency, Hz */
#define VCP_ID_RX_NSUBF             111  /* RX Noise Suppression Upper Band Frequency, Hz */
#define VCP_ID_RX_SHER              112  /* RX Noise Suppression Speech Harmonics Enhancement and Restoration factor, float */

        /* --- Output DRC/AGC/gain config --- */
#define VCP_ID_RX_DRC               113  /* DRC factor (saturation or boost), dB */
#define VCP_ID_RX_DRCBEND           114  /* DRC bend threshold, dB */
#define VCP_ID_RX_DRCNGIL           115  /* DRC noise gate input level, dB */
#define VCP_ID_RX_DRCNGOL           116  /* DRC noise gate output level, dB */
#define VCP_ID_RX_AGC               117  /* Automatic Gain Control (AGC) maximal gain, dB */
#define VCP_ID_RX_AGCT              118  /* Time to double AGC gain, ms */
#define VCP_ID_RX_AGCSAT            119  /* AGC saturation signal level, dB */
#define VCP_ID_RX_AGCTAR            120  /* AGC target signal level, dB */
#define VCP_ID_RX_AGCTHR            121  /* AGC actuation signal level threshold, dB */
#define VCP_ID_RX_AGCFADE           122  /* AGC fade factor, sec */
#define VCP_ID_RX_AGCVADTHR         123  /* AGC VAD treshold */
#define VCP_ID_RX_CLIP              124  /* RX clip factor, dB */
#define VCP_ID_RX_OG                125  /* Rx output gain, db */
#define VCP_ID_RX_WRNS              126  /* Wideband residual noise suppressor threshold, dB */
        /* --- NDVC config --- */
#define VCP_ID_RX_NDVCNMIN          127  /* Minimal noise level, dB */
#define VCP_ID_RX_NDVCNMAX          128  /* Maximal noise level, dB */
#define VCP_ID_RX_NDVCRANGE         129  /* NDVC range (defines maximal gain), dB */
#define VCP_ID_RX_NDVCIFAC          130  /* Smoothing factor for raising gain, ms */
#define VCP_ID_RX_NDVCDFAC          131  /* Smoothing factor for falling gain, ms */
		/* Easy Listen config */
#define VCP_ID_RX_ELMAD             132  /* EL maximum accumulated delay, ms */
#define VCP_ID_RX_ELMSF             133  /* EL maximum stretch factor, persent */
#define VCP_ID_RX_ELPCT             134  /* EL pitch confirmation time, ms */
#define VCP_ID_RX_ELTHRV            135  /* EL threshold voice - signal with level above threshold, may be stretched, dB */
#define VCP_ID_RX_ELTHRN            136  /* EL threshold noise - signal with level below threshold, is consider as noise, dB */

    /* === EXTENSION FOR PRIMARY (TX) SIGNAL === */
    /* --- Adaptive Directional Microphone --- */
#define VCP_ID_TX_ADMFLAGS          137  /* VCP ADM bit-flags, 1/0 (on/off)
	                                      bit  0: ADMflag - ADM block, defualt 0
	                                      bit  1: ADMOUTflag - output of ADM block writes after TX output frame, default 0
	                                      bit  2: ADMNLPflag - ADM NLP, default 1
                                          bit  3: ADMLIMflag  - ADM Limiter, default 1
										  bit  4: ADMSENSflag - ADM mic. sensitivity equalization, default 0
										  bit  5: ADMAMflag - ADM activity mode, default 0
										  bit  6: ADMESflag - ADM echo suppressor mode, default 0
										  bit  7: ADMFRCflag - ADM frequency compensation for far talk, default 1
										  bit  8: ADMREVMflag - ADM input reverse microphones flag, default 0
										  bit  9: ADMREVOflag - ADM reverse rear mic phase flag, default 0
										  bit 10: ADMAFRCflag - ADM adaptive frequency compensation for far talk, default 0
										  bit 11: ADMRMflag   - ADM reference mode, default 0
                                         */
#define VCP_ID_TX_ADMCNF            138  /* ADM input type: 0-all mics, -1 - diff(fr-rear), 1..4 - single mic (1-front, 2-rear, 3-3rd mic, 4-4th mic) */
#define VCP_ID_TX_ADMFG             139  /* ADM front mic. gain, dB */
#define VCP_ID_TX_ADMRG             140  /* ADM rear mic. gain, dB */
#define VCP_ID_TX_ADMMODE           141  /* ADM mode: 0-far field, 1-near field, 2-broad side, car 3-broad side, headphone, 4 - case 3 mic., 5 - case 4 mic. */
#define VCP_ID_TX_ADMDIST1          142  /* Input mic. distance1, mm */
#define VCP_ID_TX_ADMDIST2          143  /* Input mic. distance2, mm */
#define VCP_ID_TX_ADMMAA            144  /* Minimal attenuation angle, degree */
#define VCP_ID_TX_ADMSTR            145  /* Steer angle for broad side, degree */
#define VCP_ID_TX_ADMMPE            146  /* Maximal Phase Error, degree */
#define VCP_ID_TX_ADMARFE           147  /* Adaptation Rate Factor for angle, echo */
#define VCP_ID_TX_ADMARFN           148  /* Adaptation Rate Factor for angle, ech */
#define VCP_ID_TX_ADMLDF            149  /* Lower Directionality Frequency, Hz */
#define VCP_ID_TX_ADMUDF            150  /* Upper Directionality Frequency, Hz */
#define VCP_ID_TX_ADMNLPG           151  /* NLP minimal gain, dB */
#define VCP_ID_TX_ADMMEG            152  /* Enhancer maximal gain  */
#define VCP_ID_TX_ADMWNG            153  /* Wind noise reduction minimal gain, dB */
		/* Reserved parameters */
#define VCP_ID_PAR0                 154  /* Reserved parameter */
#define VCP_ID_PAR1                 155
#define VCP_ID_PAR2                 156
#define VCP_ID_PAR3                 157
#define VCP_ID_PAR4                 158

        /* --- Auxiliary data --- */
#define VCP_ID_DATE                 159  /* Time stamp of profile generation - date */
/* bits: 0–4 day, 5–8 month, 9–15 year offset from 1980 */
#define VCP_ID_TIME                 160  /* time stamp of profile generation - time  */
                                         /* bits 0–4 sec div. by 2, 5–10 min (0–59), 11–15 hour (0–23) */
#define VCP_ID_CRC                  161  /* VCP parameters check sum */
        /* --- Equalizers config --- */
#define VCP_ID_TX_EQ                162  /* TX equalizer coefficients */
#define VCP_ID_RX_EQ                (VCP_ID_TX_EQ + EQC_PACKED)  /* RX eqaulizer coefficients (packed into 4 bits per band) */
#define VCP_ID_RF_EQ                (VCP_ID_RX_EQ + EQRXC_PACKED)  /* ReFerence eqaulizer coefficients (packed into 4 bits per band) */
#define VCP_ID_BCT_EQ               (VCP_ID_RF_EQ + EQC_PACKED)  /* Define individual BCT setting for each bands (packed into 4 bits per band) */
#define VCP_ID_REAR_EQ              (VCP_ID_BCT_EQ+ EQC_PACKED)  /* TX ADM rear microphone eqaulizer coefficients (packed into 4 bits per band) */
#define VCP_ID_GCT_EQ               (VCP_ID_REAR_EQ + EQC_PACKED) /* GCT equalizer coefficients */
#define VCP_ID_NDVC_EQ              (VCP_ID_GCT_EQ  + EQC_PACKED) /* NDVC equalizer coefficients */

        /* --- Auxiliary parameters --- */
#define VCP_ID_TX1_HWG		    (VCP_ID_NDVC_EQ + EQC_PACKED + 0)	/* Hardware gain: input TX1 */
#define VCP_ID_TX2_HWG		    (VCP_ID_NDVC_EQ + EQC_PACKED + 1)	/* Hardware gain: input TX2 */
#define VCP_ID_TXOUT_HWG	    (VCP_ID_NDVC_EQ + EQC_PACKED + 2)	/* Hardware gain: output TX */
#define VCP_ID_RXIN_HWG		    (VCP_ID_NDVC_EQ + EQC_PACKED + 3)	/* Hardware gain: input RX  */
#define VCP_ID_RXOUT_HWG	    (VCP_ID_NDVC_EQ + EQC_PACKED + 4)	/* Hardware gain: output RX */
#define VCP_ID_AUXP0		    (VCP_ID_NDVC_EQ + EQC_PACKED + 5)	/* Auxiliary parameter 0 */
#define VCP_ID_AUXP1		    (VCP_ID_NDVC_EQ + EQC_PACKED + 6)	/* Auxiliary parameter 1 */
#define VCP_ID_AUXP2		    (VCP_ID_NDVC_EQ + EQC_PACKED + 7)	/* Auxiliary parameter 2 */
#define VCP_ID_LOG              (VCP_ID_NDVC_EQ + EQC_PACKED + 8)   /* Logger parameters     */
        /* --- User parameter(s) --- */
#define VCP_ID_USERDATA         (VCP_ID_LOG + LOG_SIZE)             /* (INFO) additional user data  */ 

/* ==================================================================== */
/*                                                                      */
/*  VCP TYPES AND STRUCTURES                                            */
/*                                                                      */
/* ==================================================================== */

/* ---- VCP profile structure advanced declaration ---- */
typedef struct vcpProfile_tag * vcpProfile_t;

/* ---- VCP object structure advanced declaration ---- */
typedef struct vcpObject_tag * vcpTObj;

/* ---- Integer types redefinitiion ---- */
typedef char            vcpTInt8;
typedef unsigned char   vcpTUint8;
typedef short           vcpTInt16;
typedef unsigned short  vcpTUint16;
typedef int             vcpTInt32;
typedef unsigned int    vcpTUint32;

/* ---- VCP version identifier structure  ---- */
typedef struct {
        vcpTUint32      CodeRevisionNumber;     /* VCP code revision identifier  */
        vcpTUint32      CompileOptionsFlags;    /* Bits flags of compile options */
        vcpTInt16       NarrowBandsTx;          /* Number of narrow bands, TX    */
        vcpTInt16       NarrowBandsRx;          /* Number of narrow bands, RX    */
        vcpTInt8        BuildDate[16];          /* VCP build date string         */
        vcpTInt8        BuildTime[16];          /* VCP build time string         */
} vcpTVersionID;


/* ---- Error message types ---- */

/* ---- Compatible error type for other cases ---- */
typedef vcpTInt16 vcpTErrorCode;

/* Structure to return information on parameter errors */
typedef struct
{
vcpTErrorCode  ErrCode;        /* Error type    */
vcpTInt16      ErrInfo;        /* Error details */
} vcpTError;


/* ----------------------------------------------------------------------- */
/*  VCP PROFILE STRUCTURE                                                  */
/* ----------------------------------------------------------------------- */
typedef struct vcpSProfile
{
vcpTUint16  m_VER;                  /* VCP profile version */

/* === General VCP parameters === */
vcpTUint16  m_FSF;                  /* Sampling frequency factor */
vcpTUint16  m_FLAGS0;               /* VCP bit-flags, 1/0 (on/off)
									   bit 0:  TX_AFflag     - AF block, default 1
									   bit 1:  TX_ESflag     - ES block, default 1
									   bit 2:  TX_CNflag     - CN block, default 1
									   bit 3:  TX_FOSTflag   - AF output sample type - 1/0 (1 - last filtering stage result, 0 - minimal for two stages) default 0
									   bit 4:  TX_DCRflag	 - Remove DC at TX inputs
									   bit 5:  RX_DCRflag	 - Remove DC at RX input
									   bit 6:  TX_INDRCflag  - Input DRC, default 1
									   bit 7:  TX_OUTDRC1flag- Output 1st DRC, default 1
									   bit 8:  TX_OUTDRC2flag- Output 2nd DRC, default 0
									   bit 9:  RX_NDVCflag   - NDVC block, default 0
									   bit 10: RXflag        - Basic RX processing, default 1
									   bit 11: RXBflag       - Subband RX signal processing blocks (NS, EQ), default 1
									   bit 12: RX_AVQflag    - AVQ block, default 0, when enabled, RXflag and RXBflag should be also enabled
									   bit 13: RX_ELflag     - Easy Listen block, default 0
									   bit 14: TX_ESALTflag  - apply special ES parameters for near talk, default 0
                                    */
vcpTUint16  m_FLAGS1;               /* VCP bit-flags, 1/0 (on/off)
									   bit 0:  TX_NSflag     - NS block, default 1
									   bit 1:  RX_NSflag     - RX NS block, default 1
									   bit 2:  TX_ESRCflag   - ES reference control, default 1
									   bit 3:  RXDflag       - RX disable, default 0, for evcp mode
									   bit 4:  TX_TSFflag    - two stages AF flag, default 1
									   bit 5:  TX_AGCflag    - TX channel, AGC block, default 1
									   bit 6:  RX_AGCflag    - RX channel, AGC block, default 1
									   bit 7:  RX_DRCflag    - RX channel, DRC, default 1
									   bit 8:  TX_GDNRflag   - TX channel, Gain Dependant Noise Reduction, default 1
									   bit 9:  RX_GDNRflag   - RX channel, Gain Dependant Noise Reduction, default 1
									   bit 10: TX_PMflag     - AF Protected Mode, default 1
									   bit 11: TX_WRNSflag   - TX WRNS block, default 1
									   bit 12: RX_WRNSflag   - RX WRNS block, default 1
									   bit 13: RX_DTGCflag   - RX DTGC block, default 0
									   bit 14: TX_NDflag     - ES Nonlinear distortions (ND) block, default 1
									   bit 15: TX_BCT_EQflag - ES Individual BCT settings for each frequency band, default 0
									*/
vcpTUint16  m_FLAGS2;               /* VCP bit-flags, 1/0 (on/off)
									   bit 0:  TX_DMflag     - TX dynamic management flag, default 1
									   bit 1:  RX_DMflag     - RX dynamic management flag, default 1
									   bit 2:  DCDflag       - delay change detector flag, default 0
									   bit 3:  LOGflag       - logger flag, default 0
									   bit 4:  TX_SBflag     - TX channel, Saturation Beep, default 0
									   bit 5:  RX_SBflag     - RX channel, Saturation Beep, default 0
									   bit 6:  TX_ILflag     - interleaving flag, for input TX samples, default 0
									   bit 7:  STflag        - start flag (at the work begining: bypass while input signal too small), default 1
									   bit 8:  TX_AFAHflag   - AF Activity Hold flag, default 0
									   bit 9:  TX_AFSTflag   - AF stereo reference signal flag, default 0
									   bit 10: TX_LFNFflag   - TX NS Low Frequencies Noise Flattening
									   bit 11: RX_LFNFflag   - RX NS Low Frequencies Noise Flattening
									   bit 12: AFCCAflag     - AF correlation-controlled adaptation, default 0
									   bit 13:
									   bit 14:
									   bit 15:
									*/
vcpTUint16  m_CONNECT;              /* VCP connection configuration (16 bit word):
                                      struct {
                                              short TXin:  3;
                                              short PF:    1;
                                              short TXout: 4;
                                              short RXin:  4;
                                              short RXout: 4;
                                      }
                                      * TXin, TXout, RXin, RXout can be VCP_CONNECT_NORMAL, VCP_CONNECT_BYPASS, VCP_CONNECT_SINE, VCP_CONNECT_ZERO, VCP_CONNECT_SWEEP, VCP_CONNECT_WHITE_NOISE
                                      0 - VCP_CONNECT_NORMAL - normal connection mode;
                                      1 - VCP_CONNECT_BYPASS - VCP bypass mode;
                                      2 - VCP_CONNECT_SINE   - sine tone generation (1000 Hz, -6 dB);
                                      3 - VCP_CONNECT_ZERO   - zero signal generation;
                                      4 - VCP_CONNECT_SWEEP  - sweep tone signal generation;
                                      5 - VCP_CONNECT_WHITE_NOISE - pulse of white noise signal generation
                                      * PF is a flag denoting VCP processing 1/0 (enabled/disabled)
                                    */
vcpTUint16  m_RPD;                  /* Reference (Rx) to Primary (Tx) Delay, ms */
vcpTUint16  m_UDF;                  /* Upper Duplex Frequency, Hz */
vcpTUint16  m_TX_HDTIME;            /* Begin half-duplex/compression time, ms */

        /* === PRIMARY (TX) SIGNAL === */

        /* --- Input DRC config --- */
vcpTUint16  m_TX_INDRC;             /* Input DRC saturation threshold, dB */
vcpTUint16  m_TX_INDRCBEND;         /* Input DRC bend threshold, dB */
vcpTUint16  m_TX_INDRCNGIL;         /* Input DRC noise gate input level, dB */
vcpTUint16  m_TX_INDRCNGOL;         /* Input DRC noise gate output level, dB */
vcpTUint16  m_TX_IG;                /* Gain after the Input DRC, dB */
        /* --- EC (adaptive filters) config --- */
vcpTUint16  m_TX_AFL1;              /* Effective length of adaptive filter, ms */
vcpTUint16  m_TX_AFL2;              /* Effective length of adaptive filter for high frequencies, ms */
vcpTUint16  m_TX_ARF;               /* Adaptation rate factor for adaptive filter */
vcpTUint16  m_TX_AFUF;              /* Adaptive filter upper frequency for length AFL1, hz */
vcpTUint16  m_TX_REFG;              /* Rx reference signal gain (external volume control), dB */
vcpTUint16  m_TX_PMTHR;             /* Protected mode threshold for adaptive filter 1, dB */
vcpTUint16  m_TX_PMUF;              /* Protected mode upper frequency, Hz */
vcpTUint16  m_TX_ERLIF;             /* Protected mode ERL increase factor  */

        /* --- ES - general config --- */
vcpTUint16  m_TX_BCT;               /* Band Cancellation Threshold, dB */
vcpTUint16  m_TX_GCT;               /* Global Cancellation Threshold, dB */
vcpTUint16  m_TX_BCT_ALT;            /* Band Cancellation Threshold for near talk, dB */
vcpTUint16  m_TX_GCT_ALT;            /* Global Cancellation Threshold for near talk, dB */
vcpTUint16  m_TX_OBT;               /* Open bands threshold, defines minimal allowed amount of open bands in ES */
vcpTUint16  m_TX_ESOTHR;            /* ES output low level signal threshold */
vcpTUint16  m_TX_RRT;               /* Room reverberation time, ms */
vcpTUint16  m_TX_NBITSC;            /* Number of bits for scaling signal after ec */
vcpTUint16  m_TX_NSHG;		        /* Near speach harmonics generation factor */
vcpTUint16  m_TX_CNG;		        /* Comfort noise gain, dB */
vcpTUint16  m_TX_UDFTHR;            /* Reference signal activity level threshold for bands above udf, dB */
        /* --- ES - non-linear distortions config --- */
vcpTUint16  m_TX_NDF1;              /* Bottom frequency producing non-linear distortions, Hz */
vcpTUint16  m_TX_NDF2;              /* Top frequency producing non-linear distortions, Hz */
vcpTUint16  m_TX_NDF3;              /* Top non-linear distortions region frequency, Hz */
vcpTUint16  m_TX_NDC;               /* Non-linear distortions coefficient */
vcpTUint16  m_TX_NDC_ALT;            /* Non-linear distortions coefficient for near talk */
vcpTUint16  m_TX_NDCR;              /* NDC release (in time) coefficient */
vcpTUint16  m_TX_RSL;               /* Reference (Rx) saturation level, dB */
vcpTUint16  m_TX_RSNDC;             /* Special NDC value for oversaturated Reference (Rx) */
vcpTUint16  m_TX_RSNDC_ALT;          /* Special NDC value for oversaturated Reference (Rx) for near talk */
vcpTUint16  m_TX_NDCS;              /* NDC decrease step in case of Reference signal decrease */
vcpTUint16  m_TX_PSL;               /* Primary (Tx) saturation level, dB */
vcpTUint16  m_TX_REFTHR;            /* Reference signal activity level threshold, dB */
        /* --- Noise Suppressor config --- */
vcpTUint16  m_TX_NSPR;              /* Noise Suppression Processing  0-full, 1-moderate, 2-lite */
vcpTUint16  m_TX_SNS;               /* Stationary Noise Suppression, dB */
vcpTUint16  m_TX_WBNGG;             /* Wide band noise gate gain, dB */
vcpTUint16  m_TX_NSTSMG;            /* Tone Suppression minimal gain, dB */
vcpTUint16  m_TX_NSTDATHR;          /* Tone detection absolute threshold, dB */
vcpTUint16  m_TX_NSTDRTHR;			/* Tone detection relative threshold, dB */
vcpTUint16  m_TX_BNL;               /* Band noise level threshold, dB */
vcpTUint16  m_TX_BNL_MING;          /* Minimal allowed BNL gain, dB */
vcpTUint16  m_TX_BNLEF;             /* BNL spectrum curvature factor, dB */
vcpTUint16  m_TX_SNAT;              /* Stationary Noise Adaptation Time, ms */
vcpTUint16  m_TX_TNAT;              /* Transient Noise Adaptation Time, ms */
vcpTUint16  m_TX_NTF1;              /* Noise Threshold Factor, first band, float */
vcpTUint16  m_TX_NTF2;              /* Noise Threshold Factor, last band,  float */
vcpTUint16  m_TX_NSS;               /* Noise Suppression Smoothness, float */
vcpTUint16  m_TX_NSHRUF;            /* TX Noise Suppression High Resolution Upper Frequency, Hz */
vcpTUint16  m_TX_NSUBF;             /* TX Noise Suppression Upper Band Frequency, Hz */
vcpTUint16  m_TX_SHER;              /* TX Noise Suppression Speech Harmonics Enhancement and Restoration factor, float */

        /* --- Output DRC/AGC/gain config --- */
vcpTUint16  m_TX_OUTDRC1;           /* Output 1st DRC saturation threshold, dB */
vcpTUint16  m_TX_OUTDRC1BEND;       /* Output 1st DRC bend threshold, dB */
vcpTUint16  m_TX_OUTDRC1NGIL;       /* Output 1st DRC noise gate input level, dB */
vcpTUint16  m_TX_OUTDRC1NGOL;       /* Output 1st DRC noise gate output level, dB */
vcpTUint16  m_TX_OUTDRC2;           /* Output 2nd DRC saturation threshold, dB */
vcpTUint16  m_TX_OUTDRC2BEND;       /* Output 2nd DRC bend threshold, dB */
vcpTUint16  m_TX_OUTDRC2NGIL;       /* Output 2nd DRC noise gate input level, dB */
vcpTUint16  m_TX_OUTDRC2NGOL;       /* Output 2nd DRC noise gate output level, dB */
vcpTUint16  m_TX_AGC;               /* Automatic Gain Control (AGC) maximal gain */
vcpTUint16  m_TX_AGCT;              /* Time to double AGC gain, ms */
vcpTUint16  m_TX_AGCSAT;            /* AGC saturation signal level, dB */
vcpTUint16  m_TX_AGCTAR;            /* AGC target signal level, dB */
vcpTUint16  m_TX_AGCTHR;            /* AGC actuation signal level threshold, dB */
vcpTUint16  m_TX_AGCFADE;           /* AGC fade factor */
vcpTUint16  m_TX_AGCVADTHR;         /* AGC VAD treshold */
vcpTUint16  m_TX_OG;                /* TX output gain, dB */
vcpTUint16  m_TX_WRNS;              /* Wideband residual noise suppressor threshold, dB */
        /* --- Side Tone, Frequency Shift --- */
vcpTUint16  m_STG;			        /* Side tone gain, dB */
vcpTUint16  m_FRQSH;                /* Frequency Shift, percent */
        /* --- Dynamic control config --- */
vcpTUint16  m_TX_AFTHR;				/* AF reference threshold, dB */
vcpTUint16  m_TX_ESTHR;             /* ES reference threshold, dB */

        /* === REFERENCE (RX) SIGNAL === */

        /* --- General parameters --- */
vcpTUint16  m_RX_IG;                /* Rx input gain, dB */
vcpTUint16  m_RX_DTGCATT;           /* Rx gain attenuation value, dB */
vcpTUint16  m_RX_DTGCDT;            /* Gain decrease time, ms */
vcpTUint16  m_RX_DTGCIT;            /* Gain increase time, ms */
vcpTUint16  m_RX_DTGCHT;            /* Attenuated gain hold time, ms */
vcpTUint16  m_RX_DTGCHP;			/* DTGC high pass gain, dB */
vcpTUint16  m_RX_DTGCTHR;           /* DTGC threshold , dB */
vcpTUint16  m_RX_VOL;				/* RX volume, dB */
        /* --- AVQ config --- */
vcpTUint16  m_RX_AVQNMIN;           /* Minimal noise level, dB */
vcpTUint16  m_RX_AVQNMAX;           /* Maximal noise level, dB */
vcpTUint16  m_RX_AVQRANGE;          /* AVQ range (defines maximal gain), dB */
vcpTUint16  m_RX_AVQIFAC;           /* Smoothing factor for raising gain */
vcpTUint16  m_RX_AVQDFAC;           /* Smoothing factor for falling gain */
        /* --- Noise Suppressor config --- */
vcpTUint16  m_RX_NSPR;              /* Noise Suppression Processing 0-full, 1-moderate, 2-lite */
vcpTUint16  m_RX_SNS;               /* Stationary Noise Suppression, dB */
vcpTUint16  m_RX_WBNGG;             /* Wide band noise gate gain, dB */
vcpTUint16  m_RX_NSTSMG;            /* Tone Suppression minimal gain, dB */
vcpTUint16  m_RX_NSTDATHR;          /* Tone detection absolute threshold, dB */
vcpTUint16  m_RX_NSTDRTHR;			/* Tone detection relative threshold, dB */
vcpTUint16  m_RX_BNL;               /* Band noise level threshold, dB */
vcpTUint16  m_RX_BNL_MING;          /* Minimal allowed BNL gain, dB */
vcpTUint16  m_RX_BNLEF;             /* BNL spectrum curvature factor, dB */
vcpTUint16  m_RX_SNAT;              /* Stationary Noise Adaptation Time, ms */
vcpTUint16  m_RX_TNAT;              /* Transient Noise Adaptation Time, ms */
vcpTUint16  m_RX_NTF1;              /* Noise Threshold Factor, first band, float */
vcpTUint16  m_RX_NTF2;              /* Noise Threshold Factor, last band,  float */
vcpTUint16  m_RX_NSS;               /* Noise Suppression Smoothness, float */
vcpTUint16  m_RX_NSHRUF;            /* RX Noise Suppression High Resolution Upper Frequency, Hz */
vcpTUint16  m_RX_NSUBF;             /* RX Noise Suppression Upper Band Frequency, Hz */
vcpTUint16  m_RX_SHER;              /* RX Noise Suppression Speech Harmonics Enhancement and Restoration factor, float */

        /* --- Output DRC/AGC/gain config --- */
vcpTUint16  m_RX_DRC;               /* DRC saturation threshold, dB */
vcpTUint16  m_RX_DRCBEND;           /* DRC bend threshold, dB */
vcpTUint16  m_RX_DRCNGIL;           /* DRC noise gate input level, dB */
vcpTUint16  m_RX_DRCNGOL;           /* DRC noise gate output level, dB */
vcpTUint16  m_RX_AGC;               /* Automatic Gain Control (AGC) maximal gain, dB */
vcpTUint16  m_RX_AGCT;              /* Time to double AGC gain, ms */
vcpTUint16  m_RX_AGCSAT;            /* AGC saturation signal level, dB */
vcpTUint16  m_RX_AGCTAR;            /* AGC target signal level, dB */
vcpTUint16  m_RX_AGCTHR;            /* AGC actuation signal level threshold, dB */
vcpTUint16  m_RX_AGCFADE;           /* AGC fade factor, sec */
vcpTUint16  m_RX_AGCVADTHR;         /* AGC VAD treshold */
vcpTUint16  m_RX_CLIP;              /* RX clip factor, dB */
vcpTUint16  m_RX_OG;                /* Rx output gain, db */
vcpTUint16  m_RX_WRNS;              /* Wideband residual noise suppressor threshold, dB */
        /* --- NDVC config --- */
vcpTUint16  m_RX_NDVCNMIN;          /* Minimal noise level, dB */
vcpTUint16  m_RX_NDVCNMAX;          /* Maximal noise level, dB */
vcpTUint16  m_RX_NDVCRANGE;         /* NDVC range (defines maximal gain), dB */
vcpTUint16  m_RX_NDVCIFAC;          /* Smoothing factor for raising gain, ms */
vcpTUint16  m_RX_NDVCDFAC;          /* Smoothing factor for falling gain, ms */
		/* Easy Listen config */
vcpTUint16  m_RX_ELMAD;             /* EL maximum accumulated delay, ms */
vcpTUint16  m_RX_ELMSF;             /* EL maximum stretch factor, persent */
vcpTUint16  m_RX_ELPCT;             /* EL pitch confirmation time, ms */
vcpTUint16  m_RX_ELTHRV;            /* EL threshold voice - signal with level above threshold, may be stretched, dB */
vcpTUint16  m_RX_ELTHRN;            /* EL threshold noise - signal with level below threshold, is consider as noise, dB */

    /* === EXTENSION FOR PRIMARY (TX) SIGNAL === */
    /* --- Adaptive Directional Microphone --- */
vcpTUint16  m_TX_ADMFLAGS;          /* VCP ADM bit-flags, 1/0 (on/off)
	                                 bit  0: ADMflag - ADM block, defualt 0
	                                 bit  1: ADMOUTflag - output of ADM block writes after TX output frame, default 0
	                                 bit  2: ADMNLPflag - ADM NLP, default 1
                                     bit  3: AMLIMflag  - ADM Limiter, default 1
									 bit  4: ADMSENSflag - ADM mic. sensitivity equalization, default 0
									 bit  5: ADMAMflag - ADM activity mode, default 0
								     bit  6: ADMESflag - ADM echo suppressor mode, default 0
									 bit  7: ADMFRCflag - ADM frequency compensation for far talk, default 1
								     bit  8: ADMREVMflag - ADM input reverse microphones flag, default 0
									 bit  9: ADMREVOflag - ADM reverse rear mic phase flag, default 0
									 bit 10: ADMAFRCflag - ADM adaptive frequency compensation for far talk, default 0
									 bit 11: ADMRMflag   - ADM reference mode, default 0
                                    */
vcpTUint16  m_TX_ADMCNF;            /* ADM input type: 0-all mics, -1 - diff(fr-rear), 1..4 - single mic (1-front, 2-rear, 3-3rd mic, 4-4th mic) */
vcpTUint16  m_TX_ADMFG;             /* ADM front mic. gain, dB */
vcpTUint16  m_TX_ADMRG;             /* ADM rear mic. gain, dB */
vcpTUint16  m_TX_ADMMODE;           /* ADM mode: 0-far field, 1-near field, 2-broad side, car, 3-broad side, headphone, 4 - case 3 mic., 5 - case 4 mic. */
vcpTUint16  m_TX_ADMDIST1;          /* Input mic. distance1, mm */
vcpTUint16  m_TX_ADMDIST2;          /* Input mic. distance2, mm */
vcpTUint16  m_TX_ADMMAA;            /* Minimal attenuation angle, degree */
vcpTUint16  m_TX_ADMSTR;            /* Steer angle for broad side, degree */
vcpTUint16  m_TX_ADMMPE;            /* Maximal Phase Error, degree */
vcpTUint16  m_TX_ADMARFE;           /* Adaptation Rate Factor for angle, echo */
vcpTUint16  m_TX_ADMARFN;           /* Adaptation Rate Factor for angle, noise */
vcpTUint16  m_TX_ADMLDF;            /* Lower Directionality Frequency, Hz */
vcpTUint16  m_TX_ADMUDF;	        /* Upper Directionality Frequency, Hz */
vcpTUint16  m_TX_ADMNLPG;           /* NLP minimal gain, dB */
vcpTUint16  m_TX_ADMMEG;            /* Enhancer maximal gain */
vcpTUint16  m_TX_ADMWNG;	        /* Wind noise reduction minimal gain, dB */
        /* --- Reserved parameters --- */
vcpTUint16  m_PAR0;	                /* Reserved parameter */
vcpTUint16  m_PAR1;
vcpTUint16  m_PAR2;
vcpTUint16  m_PAR3;
vcpTUint16  m_PAR4;

        /* --- Auxiliary data --- */
vcpTUint16  m_DATE;                 /* Time stamp of profile generation - date  */
                                    /* bits: 0–4 day, 5–8 month, 9–15 year offset from 1980 */
vcpTUint16  m_TIME;                 /* time stamp of profile generation - time  */
                                    /* bits 0–4 sec div. by 2, 5–10 min (0–59), 11–15 hour (0–23) */
vcpTUint16  m_CRC16;                /* VCP parameters check sum                             */
        /* --- Equalizers config --- */
vcpTUint16  m_TX_EQ[ EQC_PACKED ];  /* TX equalizer coefficients */
vcpTUint16  m_RX_EQ[ EQRXC_PACKED ];  /* RX eqaulizer coefficients (packed into 4 bits per band) */
vcpTUint16  m_REF_EQ[ EQC_PACKED ]; /* ReFerence eqaulizer coefficients (packed into 4 bits per band) */
vcpTUint16  m_BCT_EQ[ EQC_PACKED ]; /* Define individual BCT setting for each bands (packed into 4 bits per band) */
vcpTUint16  m_REAR_EQ[ EQC_PACKED ];/* TX ADM rear microphone eqaulizer coefficients (packed into 4 bits per band) */
vcpTUint16  m_GCT_EQ[ EQC_PACKED ];	 /* GCT equalizer coefficients */
vcpTUint16  m_NDVC_EQ[ EQC_PACKED ]; /* NDVC equalizer coefficients */
        /* --- Auxiliary parameters --- */
vcpTUint16  m_TX1IN_HWG;            /* Hardware gain: input TX1 */
vcpTUint16  m_TX2IN_HWG;            /* Hardware gain: input TX2 */
vcpTUint16  m_TXOUT_HWG;	    /* Hardware gain: output TX */
vcpTUint16  m_RXIN_HWG;		    /* Hardware gain: input RX  */
vcpTUint16  m_RXOUT_HWG;	    /* Hardware gain: output RX */
vcpTUint16  m_AUXP0;		    /* Auxiliary parameter 0    */
vcpTUint16  m_AUXP1;		    /* Auxiliary parameter 1    */
vcpTUint16  m_AUXP2;		    /* Auxiliary parameter 2    */
vcpTUint16  m_LOG[ LOG_SIZE ];	    /* logger parameters buffer */
        /* --- User parameter(s) --- */
vcpTUint16  m_USERDATA[ UD_SIZE ];  /* (INFO) additional user data  */ 
} vcpTProfile;



/* ----------------------------------------------------------------------- */
/*  VCP DEBUG DATA STRUCTURE                                               */
/* ----------------------------------------------------------------------- */
typedef struct vcpSDebugStruct
{
	vcpTUint16		StructVersion;
	vcpTUint16		WordSize;
	vcpTUint32		FrameNo;
	vcpTUint16      Tx1in_amp;
	vcpTUint16      Tx2in_amp;
	vcpTUint16      Txout_amp;
	vcpTUint16		Rxin_amp;
	vcpTUint16		Rxout_amp;
	vcpTUint16		Flags;		   /* TxVAD(bit 0), RXVAD(bit 1), TxTD(bit 2), TxOutTD(bit 3), RxTD(bit 4) */

} *vcpPDebugStruct, vcpTDebugStruct;

struct vcp_mem_reg_s
{
	void	*mem;		// memory provided
	int		size;		// overall size of the region
};

typedef struct vcp_mem_reg_s vcpTMemRegStruct;

/* ==================================================================== */
/*                                                                      */
/*  VCP INTERFACE FUNCTIONS                                             */
/*                                                                      */
/* ==================================================================== */

/* -------------------------------------------------------------------- */
/*  Function:    vcpInitObj                                             */
/*                                                                      */
/*  Description: The function sets up a VCP object instance and returns */
/*               an error code. The memory regions are initialized with */
/*               parameters of *pProfile that becomes the current       */
/*               profile. The function requires synchronized access     */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR. VCP_ERROR_NULL_POINTER,                  */
/*               VCP_ERROR_OUT_OF_MEMORY, VCP_ERROR_IN_PROGRESS         */
/*               VCP_ERROR_INVALID_VALUE, VCP_ERROR_FEATURE_NOT_AVAILABLE */
/*               ErrInfo contains an ID of an error parameter           */
/* -------------------------------------------------------------------- */
vcpTError vcpInitObj(vcpTProfile *pProfile, vcpTMemRegStruct *r);

#if VCP_NUM_MEM_REGIONS == 1
/* -------------------------------------------------------------------- */
/*  Function:    vcpInitObjFlat - conforms old versions                 */
/*                                                                      */
/*  Description: The function sets up a VCP object instance and returns */
/*               an error code. The object is initialized with          */
/*               parameters of *pProfile that becomes the current       */
/*               profile. The function requires synchronized access     */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR. VCP_ERROR_NULL_POINTER,                  */
/*               VCP_ERROR_OUT_OF_MEMORY, VCP_ERROR_IN_PROGRESS         */
/*               VCP_ERROR_INVALID_VALUE, VCP_ERROR_FEATURE_NOT_AVAILABLE */
/*               ErrInfo contains an ID of an error parameter           */
/* -------------------------------------------------------------------- */
vcpTError vcpInitObjFlat(vcpTObj *pVcp, vcpTUint32 piObjSize, vcpTProfile *pProfile);
#endif

/* -------------------------------------------------------------------- */
/*  Function:    vcpProcess                                             */
/*                                                                      */
/*  Description: The function performs signal processing.               */
/*               The function deals with four linear buffers, two       */
/*               input and two output as audio data source and          */
/*               destination for VCP processing                         */
/*                                                                      */
/*         Note: Special VCP I/O debug features are ignored by this     */
/*               function.                                              */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR, VCP_ERROR_NULL_POINTER,                  */
/*               VCP_ERROR_NOT_INIT_OBJ, VCP_ERROR_WRONG_DATA_LEN,      */
/*               VCP_ERROR_OBJECT_CORRUPTED                             */
/* -------------------------------------------------------------------- */
vcpTErrorCode vcpProcess(
        void *hook,                  /* VCP Object                 */
        /* ---- Receive input channel buffer (from cellphone)           */
        vcpTInt16 *psRxInpAddr,        /* Data address                  */
        /* ---- Transmit input channel buffer (microphone)              */
        vcpTInt16 *psTxInpAddr,        /* Data address                  */
        /* ---- Receive output channel buffer (to loudspeaker)          */
        vcpTInt16 *psRxOutAddr,        /* Data address                  */
        /* ---- Transmit output channel buffer (to cellphone)           */
        vcpTInt16 *psTxOutAddr,        /* Data address                  */
        vcpTInt16  NumOfSamples        /* Channel data length to process*/
    );


/* -------------------------------------------------------------------- */
/*  Function:    vcpProcessTx                                           */
/*                                                                      */
/*  Description: The function deals with three linear buffers, two      */
/*               input and one output as audio data source and          */
/*               destination for VCP processing                         */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR. VCP_ERROR_NULL_POINTER,                  */
/*               VCP_ERROR_NOT_INIT_OBJ, VCP_ERROR_WRONG_DATA_LEN,      */
/*               VCP_ERROR_OBJECT_CORRUPTED                             */
/* -------------------------------------------------------------------- */
vcpTErrorCode vcpProcessTx(
    void *hook,                    /* VCP Object                        */
    /* ---- Reference input channel buffer                              */
    vcpTInt16 *psRfInpAddr,        /* Data address                      */
    /* ---- Transmitt input channel buffer (microphone)                 */
    vcpTInt16 *psTxInpAddr,        /* Data address                      */
    /* ---- Transmitt output channel buffer (to cellphone)              */
    vcpTInt16 *psTxOutAddr,        /* Data address                      */
    vcpTInt16  NumOfSamples        /* Channel data length to process    */
);

/* -------------------------------------------------------------------- */
/*  Function:    vcpProcessRx                                           */
/*                                                                      */
/*  Description: The function deals with two linear buffers, one        */
/*               input and one output as audio data source and          */
/*               destination for VCP processing                         */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR. VCP_ERROR_NULL_POINTER,                  */
/*               VCP_ERROR_NOT_INIT_OBJ, VCP_ERROR_WRONG_DATA_LEN,      */
/*               VCP_ERROR_OBJECT_CORRUPTED                             */
/* -------------------------------------------------------------------- */
vcpTErrorCode vcpProcessRx(
    void *hook,                    /* VCP Object                        */
    /* ---- Receive input channel buffer (from cellphone)               */
    vcpTInt16 *psRxInpAddr,        /* Data address                      */
    /* ---- Receive output channel buffer (to loudspeaker)              */
    vcpTInt16 *psRxOutAddr,        /* Data address                      */
    vcpTInt16  NumOfSamples        /* Channel data length to process    */
);

/* -------------------------------------------------------------------- */
/*  Function:    vcpGetParam                                            */
/*                                                                      */
/*  Description: The function returns a value of paramNo parameter      */
/*               from a current profile in *wParamVal.                  */
/*                                                                      */
/*               For iParamID: VCP_ID_TX_EQ, VCP_ID_RF_EQ,              */
/*               and VCP_ID_RX_EQ (if equalizer included in RX channel) */
/*               function returns array wParamVal[ nbands ],            */
/*               where nbands - number of frequency bands               */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR, VCP_ERROR_NULL_POINTER,                  */
/*               VCP_ERROR_NO_SUCH_PARAM, VCP_ERROR_NOT_INIT_OBJ        */
/* -------------------------------------------------------------------- */
vcpTErrorCode vcpGetParam(void *hook, int iParamID, vcpTUint16 *wParamVal);



/* -------------------------------------------------------------------- */
/*  Function:    vcpSetParam                                            */
/*                                                                      */
/*  Description: The function changes value of ParamID parameter of a   */
/*               current profile to a new *wParamVal value.             */
/*                                                                      */
/*               For iParamID: VCP_ID_TX_EQ                             */
/*               and VCP_ID_RX_EQ (if equalizer included in RX channel) */
/*               function sets equalizers coefficients wParamVal[nbands]*/
/*               where nbands - number of frequency bands               */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR. VCP_ERROR_NULL_POINTER,                  */
/*               VCP_ERROR_NO_SUCH_PARAM, VCP_ERROR_CHANGE_DENIED,      */
/*               VCP_ERROR_NOT_INIT_OBJ, VCP_ERROR_IN_PROGRESS          */
/*                                                                      */
/*  Note:        VCP_ERROR_CHANGE_DENIED is returned only if iParamID   */
/*               parameter value change requires a re-initialization    */
/* -------------------------------------------------------------------- */
vcpTErrorCode vcpSetParam(void *hook, int iParamID, vcpTUint16 *wParamVal);


/* -------------------------------------------------------------------- */
/*  Function:    vcpGetData                                             */
/*                                                                      */
/*  Description: The function returns a data buffer dst[]               */
/*               depend on data identifiers idataID                     */
/*               if dst=0, then size_bytes return the necessary size    */
/*               for buffer dst[]                                       */
/*                                                                      */
/*  data ID:     VCP_ID_DATA_VERSION_NUMBER (pVcp may be equal to 0)    */
/*               VCP_ID_DATA_VERSION        (pVcp may be equal to 0)    */
/*               VCP_ID_DATA_SCRATCH_MEM    (pVcp may be equal to 0)    */
/*               VCP_ID_DATA_TX_VAD, VCP_ID_DATA_RX_VAD,                */
/*               VCP_ID_DATA_TX_TONE, VCP_ID_DATA_RX_TONE,              */
/*               VCP_ID_DATA_TX_TONE_OUT                                */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR, VCP_ERROR_NULL_POINTER,                  */
/*               VCP_ERROR_NO_SUCH_PARAM, VCP_ERROR_NOT_INIT_OBJ        */
/* -------------------------------------------------------------------- */
vcpTErrorCode vcpGetData(void *hook, int idataID, void *dst, int *size_bytes);


/* -------------------------------------------------------------------- */
/*  Function:    vcpGetVersion                                          */
/*                                                                      */
/*  Description: The function returns version of VCP library            */
/*               including build date and time.                         */
/*                                                                      */
/*  Error codes: none                                                   */
/* -------------------------------------------------------------------- */
vcpTVersionID vcpGetVersion();


/* -------------------------------------------------------------------- */
/*  Function:    vcpMute                                                */
/*                                                                      */
/*  Description: The function to mute or unmute output of microphone    */
/*               or speaker channels                                    */
/*               iChannel specifies receiving or transmitting channel.  */
/*               bMute=1 - mute switch on, bMute=0 - mute switch off    */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR, VCP_ERROR_NULL_POINTER,                  */
/*               VCP_ERROR_NOT_INIT_OBJ, VCP_ERROR_INPUT_PARAM          */
/*                                                                      */
/* -------------------------------------------------------------------- */
vcpTErrorCode vcpMute(void *hook, int iChannel, int bMute);

/* -------------------------------------------------------------------- */
/*                                                                      */
/*  Service functions                                                   */
/*                                                                      */
/* -------------------------------------------------------------------- */

/* -------------------------------------------------------------------- */
/*  Function:    vcpObjMemSize                                          */
/*                                                                      */
/*  Description: The function calculates regions sizes   that are       */
/*               required for *pProfile in persistent memory and        */
/*               fills 'vcpTMemRegStruct' array.                        */
/*               It also tests all parameter values and returns         */
/*               vcpTError error code if a parameter wrong              */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR, VCP_ERROR_NULL_POINTER,                  */
/*               VCP_ERROR_TOO_BIG, VCP_ERROR_TOO_SMALL,                */
/*               VCP_ERROR_INVALID_VALUE,                               */
/*               VCP_ERROR_FEATURE_NOT_AVAILABLE                        */
/*               ErrInfo contains an ID of an error parameter           */
/* -------------------------------------------------------------------- */
vcpTError vcpObjMemSize(vcpTProfile *pProfile, vcpTMemRegStruct *reg, void *mem);

#if VCP_NUM_MEM_REGIONS == 1
/* -------------------------------------------------------------------- */
/*  Function:    vcpObjMemSizeFlat                                      */
/*                                                                      */
/*  Description: The same as above, but conforms old versions           */
/*               Computes the memory amount that is						*/
/*               required for *pProfile in persistent memory and        */
/*               puts it to *piObjSize.                                 */
/*               It also tests all parameter values and returns         */
/*               vcpTError error code if a parameter wrong              */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR, VCP_ERROR_NULL_POINTER,                  */
/*               VCP_ERROR_TOO_BIG, VCP_ERROR_TOO_SMALL,                */
/*               VCP_ERROR_INVALID_VALUE,                               */
/*               VCP_ERROR_FEATURE_NOT_AVAILABLE                        */
/*               ErrInfo contains an ID of an error parameter           */
/* -------------------------------------------------------------------- */
vcpTError vcpObjMemSizeFlat(vcpTProfile *pProfile,  vcpTUint32 *piObjSize);
#endif

/* -------------------------------------------------------------------- */
/*  Function:    vcpConnect                                             */
/*                                                                      */
/*  Description: The function directs some standard signals to input    */
/*               and from output of the algorithm for debugging purpose */
/*                                                                      */
/*               For arguments iRxInpState and iTxInpState available    */
/*               states are VCP_CONNECT_NORMAL and VCP_CONNECT_ZERO.    */
/*                                                                      */
/*               For arguments iRxOutState and iTxOutState available    */
/*               states are VCP_CONNECT_NORMAL, VCP_CONNECT_BYPASS,     */
/*               VCP_CONNECT_SINE, VCP_CONNECT_ZERO                     */
/*                                                                      */
/*               Boolean argument bProcessEnabled defines processing of */
/*               signal or bypass of processing                         */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR, VCP_ERROR_NULL_POINTER,                  */
/*               VCP_ERROR_INVALID_VALUE, VCP_ERROR_NOT_INIT_OBJ        */
/* -------------------------------------------------------------------- */
vcpTErrorCode vcpConnect(void *hook, int iRxInpState, int iTxInpState,
                         int iRxOutState, int iTxOutState, int bProcessEnabled);


/* -------------------------------------------------------------------- */
/*  Function:    vcpProcessDebug                                        */
/*                                                                      */
/*  Description: The function - implements special VCP I/O debug        */
/*               functionality defined by VCP_ID_CONNECT flag.          */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR. VCP_ERROR_NULL_POINTER,                  */
/*               VCP_ERROR_NOT_INIT_OBJ, VCP_ERROR_WRONG_DATA_LEN,      */
/*               VCP_ERROR_PROCESSING                                   */
/* -------------------------------------------------------------------- */
vcpTErrorCode vcpProcessDebug(
        void *hook,                  /* VCP Object                       */
        /* ---- Receive input channel circular buffer (from cellphone)      */
        vcpTInt16 *psRxInpAddr,        /* Data address                      */
        /* ---- Transmitt input channel circular buffer (microphone)        */
        vcpTInt16 *psTxInpAddr,        /* Data address                      */
        /* ---- Receive output channel circular buffer (to loudspeaker)     */
        vcpTInt16 *psRxOutAddr,        /* Data address                      */
        /* ---- Transmitt output channel circular buffer (to cellphone)     */
        vcpTInt16 *psTxOutAddr,        /* Data address                      */
        vcpTInt16  NumOfSamples        /* Channel data length to process    */
    );


/* -------------------------------------------------------------------- */
/*  Function:    vcpProcessTxDebug                                      */
/*                                                                      */
/*  Description: The function - implements commutation functionality    */
/*               defined with vcpConnect, deals with three linear       */
/*               buffers, two input and one output as audio data source */
/*               and destination for VCP processing                     */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR. VCP_ERROR_NULL_POINTER,                  */
/*               VCP_ERROR_NOT_INIT_OBJ, VCP_ERROR_WRONG_DATA_LEN,      */
/*               VCP_ERROR_PROCESSING                                   */
/* -------------------------------------------------------------------- */
vcpTErrorCode vcpProcessTxDebug(
    void *hook,                    /* VCP Object                        */
    /* ---- Reference input channel buffer                              */
    vcpTInt16 *psRfInpAddr,        /* Data address                      */
    /* ---- Transmitt input channel buffer (microphone)                 */
    vcpTInt16 *psTxInpAddr,        /* Data address                      */
    /* ---- Transmitt output channel buffer (to cellphone)              */
    vcpTInt16 *psTxOutAddr,        /* Data address                      */
    vcpTInt16  NumOfSamples        /* Channel data length to process    */
  );


/* -------------------------------------------------------------------- */
/*  Function:    vcpProcessRxDebug                                      */
/*                                                                      */
/*  Description: The function - implements commutation functionality    */
/*               defined with vcpConnect, deals with two linear buffers,*/
/*               one input and one output as audio data source and      */
/*               destination for VCP processing                         */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR. VCP_ERROR_NULL_POINTER,                  */
/*               VCP_ERROR_NOT_INIT_OBJ, VCP_ERROR_WRONG_DATA_LEN,      */
/*               VCP_ERROR_OBJECT_CORRUPTED                             */
/* -------------------------------------------------------------------- */
vcpTErrorCode vcpProcessRxDebug(
    void *hook,                    /* VCP Object                        */
    /* ---- Receive input channel buffer (from cellphone)               */
    vcpTInt16 *psRxInpAddr,        /* Data address                      */
    /* ---- Receive output channel buffer (to loudspeaker)              */
    vcpTInt16 *psRxOutAddr,        /* Data address                      */
    vcpTInt16  NumOfSamples        /* Channel data length to process    */
);

/* -------------------------------------------------------------------- */
/*  Function:    vcpReinit                                              */
/*                                                                      */
/*  Description: The function performs reinitialization of adaptive     */
/*               filters and echo supressor, for case when              */
/*               synchronization of reference and primary signals       */
/*               is lost                                                */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR. VCP_ERROR_NULL_POINTER,                  */
/*               VCP_ERROR_NOT_INIT_OBJ                                 */
/* -------------------------------------------------------------------- */
vcpTErrorCode vcpReinit(void *hook);                   /* VCP Object    */

/* -------------------------------------------------------------------- */
/*  Function:    vcpCheckParam                                          */
/*                                                                      */
/*  Description: The function check wParamVal value of ParamID parameter*/
/*               if  wParamVal value out of ParamID parameter range,    */
/*               the function return error                              */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR,                                          */
/*               VCP_ERROR_NO_SUCH_PARAM, VCP_ERROR_TOO_BIG,            */
/*               VCP_ERROR_TOO_SMALL, VCP_ERROR_INVALID_VALUE,          */
/*                                                                      */
/* -------------------------------------------------------------------- */
vcpTErrorCode vcpCheckParam(int iParamID, vcpTInt16 wParamVal);


/* -------------------------------------------------------------------- */
/*  Function:    vcpCheckProfile                                        */
/*                                                                      */
/*  Description: The function check profile  *pProfile                  */
/*               the function return error                              */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR, VCP_ERROR_NULL_POINTER,                  */
/*               VCP_ERROR_NO_SUCH_PARAM, VCP_ERROR_TOO_BIG,            */
/*               VCP_ERROR_TOO_SMALL, VCP_ERROR_INVALID_VALUE,          */
/*               VCP_ERROR_FEATURE_NOT_AVAILABLE                        */
/*                                                                      */
/* -------------------------------------------------------------------- */
vcpTError vcpCheckProfile(vcpTProfile *pProfile);


/* -------------------------------------------------------------------- */
/*  Function:    vcpGetCompileOptions                                   */
/*                                                                      */
/*  Description: The function										    */
/*               the function				                            */
/*                                                                      */
/*                                                                      */
/* -------------------------------------------------------------------- */
const char *vcpGetCompileOptions(void);


/* -------------------------------------------------------------------- */
/*  Function:    vcpGetProfile                                          */
/*                                                                      */
/*  Description: The function to write current profile to               */
/*               *pProfile                                              */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR. VCP_ERROR_NULL_POINTER,                  */
/*               VCP_ERROR_NOT_INIT_OBJ                                 */
/* -------------------------------------------------------------------- */
vcpTErrorCode vcpGetProfile
	(void *pVcpObjHandle,          /* VCP Object                        */
	vcpTProfile *pProfile);        /* Pointer to write profile data     */

/* -------------------------------------------------------------------- */
/*  Function:    vcpGetComponentFlags                                   */
/*                                                                      */
/*  Description: The function return VCP compiled component bit flags   */
/*                                                                      */
/* -------------------------------------------------------------------- */
vcpTUint32 vcpGetComponentFlags(void);

/* -------------------------------------------------------------------- */
/*  Function:    vcpCheckComponents                                     */
/*                                                                      */
/*  Description: The function to check profile flags, which control     */
/*               VCP processing blocks                                  */
/*               *pProfile                                              */
/*                                                                      */
/*  Error codes: VCP_NO_ERROR. VCP_ERROR_NULL_POINTER,                  */
/*               VCP_ERROR_INVALID_VALUE, VCP_ERROR_FEATURE_NOT_AVAILABLE */
/* -------------------------------------------------------------------- */
vcpTError vcpCheckComponents(vcpTProfile *p);


int vcpGetHookSize(void);

/* -------------------------------------------------------------------- */
/* Logger related														*/
/* -------------------------------------------------------------------- */

/* -------------------------------------------------------------------- */
/*  Function:    vcpLogBuffer											*/
/*                                                                      */
/*  Description: returns a pointer to logger's buffer				    */
/*               or zero in case of error                               */
/* -------------------------------------------------------------------- */

void *vcpLogBuffer(void *hook);


/* -------------------------------------------------------------------- */
/*  Function:    vcpLogBufferSize										*/
/*                                                                      */
/*  Description: returns logger's buffer size							*/
/*										                                */
/* -------------------------------------------------------------------- */

int   vcpLogBufferSize(void *hook);

/* ---------------------------------------------------------------- */
/*  Function:    vcpGetRevision										*/
/*                                                                  */
/*  Description: returns vcp lib revision number		     		*/
/*										                            */
/* ---------------------------------------------------------------- */
int vcpGetRevision(void);

/* -------------------------------------------------------------------- */
/*  Function:    vcpAddLicense										    */
/*                                                                      */
/*  Description: adds license data to VCP instance						*/
/*										                                */
/* -------------------------------------------------------------------- */
void  vcpAddLicense(unsigned char *lic);

/* -------------------------------------------------------------------- */
/*  Function:    vcpGetPointerToUserData                                */
/*                                                                      */
/*  Description: The function return pointer to array of user data      */
/*               in profile *pProfile                                   */
/*               User Data we consider as 16-bit unsigned int array     */
/*               with dimension UD_SIZE                                 */
/* -------------------------------------------------------------------- */
vcpTUint16 * vcpGetPointerToUserData
    (vcpTProfile *pProfile);       /* Pointer to profile  */



#ifdef __cplusplus
}
#endif

#endif /* VCPAPI_H */

