vcpTProfile profile =
{ /* Auto generated profile */
  0x0024,  /*             VER=36           (INFO) VCP profile version */
  0x1F40,  /*             FSF=8000         (GEN) Sampling frequency, 8000/16000/24000/32000, Hz */
/* FLAGS: value == 0x080F */
 (1 << 0 ) | /*     TX_AFflag=1   (TX AF) Adaptive Filters (AF), flag (0-off, 1-on) */
 (1 << 1 ) | /*     TX_ESflag=1   (TX ES) ES block, flag (0-off,1-on) */
 (1 << 2 ) | /*     TX_CNflag=1   (TX ES) Comfort noise, flag(0-off,1-on) */
 (1 << 3 ) | /*   TX_FOSTflag=1   (TX AF) AF output sample type (0 - min after two stages, 1 - last stage result) */
 (0 << 4 ) | /*    TX_DCRflag=0   (GEN) TX - remove DC at inputs, flag (0-off,1-on) */
 (0 << 5 ) | /*    RX_DCRflag=0   (GEN) RX - remove DC at input, flag (0-off,1-on) */
 (0 << 6 ) | /*  TX_INDRCflag=0   (TX, inDRC) TX input DRC, flag (0-off, 1-on) */
 (0 << 7 ) | /*TX_OUTDRC1flag=0   (TX outDRC1) TX output 1st DRC, flag (0-off,1-on) */
 (0 << 8 ) | /*TX_OUTDRC2flag=0   (TX outDRC2) TX output 2nd DRC, flag (0-off,1-on) */
 (0 << 9 ) | /*   RX_NDVCflag=0   (RX NDVC) NDVC, flag (0-off,1-on) */
 (0 << 10) | /*        RXflag=0   (GEN) RX processing, flag (0-off,1-on) */
 (1 << 11) | /*       RXBflag=1   (GEN) RX subband processing (EQ, NR), flag (0-off,1-on) */
 (0 << 12) | /*    RX_AVQflag=0   (RX AVQ) AVQ, flag (0-off,1-on) */
 (0 << 13) | /*     RX_ELflag=0   (RX EL) EasyListen, flag (0-off,1-on) */
 (0 << 14) | /*  TX_ESALTflag=0   (TX ES) Special BCT/GCT/NDC settings, flag (0-off,1-on) */
 (0 << 15) , /*      DREQflag=0   RESERVED */
/* FLAGS: value == 0x0315 */
 (1 << 0 ) | /*     TX_NSflag=1   (TX NS) Noise Suppression (NS) block, flag (0-off,1-on) */
 (0 << 1 ) | /*     RX_NSflag=0   (RX NS) Noise Suppression (NS), flag (0-off,1-on) */
 (1 << 2 ) | /*   TX_ESRCflag=1   (TX ES) ES reference signal control, flag (0-off,1-on) */
 (0 << 3 ) | /*       RXDflag=0   RESERVED */
 (1 << 4 ) | /*    TX_TSFflag=1   (TX AF) Two stage adaptive filtering, flag (0-one stage, 1-two stages) */
 (0 << 5 ) | /*    TX_AGCflag=0   (TX AGC) TX AGC, flag (0-off,1-on) */
 (0 << 6 ) | /*    RX_AGCflag=0   (RX AGC) RX Automatic Gain Control (AGC), flag (0-off,1-on) */
 (0 << 7 ) | /*    RX_DRCflag=0   (RX DRC) RX DRC, flag (0-off,1-on) */
 (1 << 8 ) | /*   TX_GDNRflag=1   (TX NS) Gain dependant noise reduction, flag (0-off,1-on) */
 (1 << 9 ) | /*   RX_GDNRflag=1   (RX NS) Gain dependant noise reduction, flag (0-off,1-on) */
 (0 << 10) | /*     TX_PMflag=0   (TX AF) AF Protected Mode (0-off, 1-on) */
 (0 << 11) | /*   TX_WRNSflag=0   (TX NS WRNS) TX wide band residual noise suppression, flag (0-off,1-on) */
 (0 << 12) | /*   RX_WRNSflag=0   (RX NS WRNS) RX wideband residual noise suppressor, flag (0-off,1-on) */
 (0 << 13) | /*   RX_DTGCflag=0   (RX DTGC) DTGC flag (0-off,1-on) */
 (0 << 14) | /*     TX_NDflag=0   (TX ES ND) Non-linear Distortions (ND), flag (0-off,1-on) */
 (0 << 15) , /* TX_BCT_EQflag=0   (TX ES) Allows defining individual BCT settings for each frequency band, flag (0-off,1-on) */
/* FLAGS: value == 0x1100 */
 (0 << 0 ) | /*     TX_DMflag=0   (DM TX) TX dynamic management, flag (0-off,1-on) */
 (0 << 1 ) | /*     RX_DMflag=0   RESERVED */
 (0 << 2 ) | /*       DCDflag=0   RESERVED */
 (0 << 3 ) | /*       LOGflag=0   (DEB) Logger flag (0-off, 1-on) */
 (0 << 4 ) | /*     TX_SBflag=0   (TX)  Saturation Beep flag (0-off, 1-on) */
 (0 << 5 ) | /*     RX_SBflag=0   (RX)  Saturation Beep flag (0-off, 1-on) */
 (0 << 6 ) | /*     TX_ILflag=0   (TX, ADM) microphones audio data interleaved samples flag, (0-off,1-on) */
 (0 << 7 ) | /*        STflag=0   (GEN) Start flag (at the work begining: bypass while input signal too small), flag (0-off,1-on) */
 (1 << 8 ) | /*   TX_AFAHflag=1   (TX AF) Adaptive Filters (AF) Activity Hold flag (0-off, 1-on) */
 (0 << 9 ) | /*   TX_AFSTflag=0   (TX AF) Adaptive Filters (AF) stereo reference signal flag, default 0 (0-off, 1-on) */
 (0 << 10) | /*   TX_LFNFflag=0   (TX NS) Low Frequencies Noise Flattening,flag (0-off,1-on) */
 (0 << 11) | /*   RX_LFNFflag=0   (RX NS) Low Frequencies Noise Flattening,flag (0-off,1-on) */
 (1 << 12) , /*  TX_AFCCAflag=1   (TX AF) Adaptive Filters (AF) correlation-controlled adaptation, default 0 (0-off, 1-on) */
/* FLAGS: value == 0x0008 */
 (0 << 0 ) | /*          CTXI=0   (DEB) TXin debug mode 0,2,3,4,5 as (norm/sine/zero/sweep/noise) */
 (1 << 3 ) | /*            PF=1   (DEB) Signal processing (off/on), flag */
 (0 << 4 ) | /*          CTXO=0   (DEB) TXout debug mode [0..5] as (norm/bypass/sine/zero/sweep/noise) */
 (0 << 8 ) | /*          CRXI=0   (DEB) RXin debug mode 0,2,3,4,5 as (norm/sine/zero/sweep/noise) */
 (0 << 12) , /*          CRXO=0   (DEB) RXout debug mode [0..5] as (norm/bypass/sine/zero/sweep/noise) */
  0x0000,  /*             RPD=0            (Ref) Reference (RXout) to Primary (TXin) delay (bulk delay compensation), ms */
  0x0FA0,  /*             UDF=4000         (GEN) Upper duplex frequency (half-duplex above it), Hz */
  0x01F4,  /*       TX_HDTIME=500          (TX ES) Half-duplex time after VCP init, ms */
  0x0000,  /*        TX_INDRC=0            (TX, inDRC) TX input DRC saturation level, int, dB */
  0x0001,  /*    TX_INDRCBEND=-90.31       (TX, inDRC) TX input DRC bend threshold, dB */
  0x0001,  /*    TX_INDRCNGIL=-90.31       (TX, inDRC) TX input DRC noise gate input level, dB */
  0x0001,  /*    TX_INDRCNGOL=-90.31       (TX, inDRC) TX input DRC noise gate output level, dB */
  0x0800,  /*           TX_IG=0.00         (TX) TX input digital gain (applied after the input DRC), dB */
  0x0040,  /*         TX_AFL1=64           (TX AF) Effective length of adaptive filter, ms */
  0x0040,  /*         TX_AFL2=64           (TX AF) Effective length of adaptive filter for high frequencies, ms */
  0x1999,  /*          TX_ARF=0.799951     (TX AF) Adaptation rate factor for adaptive filter, float */
  0x0FA0,  /*         TX_AFUF=4000         (TX AF) Adaptive filter upper frequency for length AFL1, Hz  */
  0x0000,  /*            REFG=0            (Ref) Reference signal gain (for external volume control), dB */
  0x0813,  /*        TX_PMTHR=-24.00       (TX AF) 1st adaptive filter protected mode threshold, dB */
  0x07D0,  /*         TX_PMUF=2000         (TX AF) Protected mode upper frequency (no PM above), Hz */
  0x0CCD,  /*        TX_ERLIF=0.100009     (TX AF) Protected mode ERL increase factor (1 + TX_ERLIF), float */
  0x32F5,  /*          TX_BCT=-8.00        (TX ES) Band cancellation threshold, dB */
  0x101D,  /*          TX_GCT=-18.00       (TX ES) Global cancellation threshold, dB */
  0x32F5,  /*      TX_BCT_ALT=-8.00        (TX ES) Alternative band cancellation threshold, dB */
  0x101D,  /*      TX_GCT_ALT=-18.00       (TX ES) Alternative global cancellation threshold, dB */
  0x0000,  /*          TX_OBT=0            (TX ES) Defines minimal allowed percent of open sub-bands in ES. */
  0x0001,  /*       TX_ESOTHR=-90.31       (TX ES) Minimal ES output signal level allowed during RX activity, dB */
  0x0000,  /*          TX_RRT=0            (TX ES) Room reverberation time (use only if adaptive filter is not long enough), ms */
  0x0000,  /*       TX_NBITSC=0            (TX ES) Scaling factor for adaptive filter output signal (in bits), int */
  0x0000,  /*         TX_NSHG=0            (TX ES) Near speech harmonic generation factor (activated in double talk), float */
  0x2000,  /*          TX_CNG=0.00         (TX ES) Comfort noise gain, dB */
  0x00A4,  /*       TX_UDFTHR=-46.01       (TX ES) Reference signal actuation threshold above UDF, dB */
  0x0000,  /*         TX_NDF1=0            (TX ES ND) Bottom frequency producing non-linear distortions, Hz */
  0x07D0,  /*         TX_NDF2=2000         (TX ES ND) Top frequency producing non-linear distortions (> NDF1), Hz */
  0x0FA0,  /*         TX_NDF3=4000         (TX ES ND) Top non-linear distortions region frequency (> NDF2), Hz */
  0x0000,  /*          TX_NDC=0            (TX ES ND) Non-linear distortions factor, float */
  0x0000,  /*      TX_NDC_ALT=0            (TX ES ND) Alternative NDC value, float */
  0x6CCC,  /*         TX_NDCR=0.850002     (TX ES ND) NDC release factor, float */
  0x7FFF,  /*          TX_RSL=0.00         (TX ES ND) Reference signal saturation level, dB */
  0x0000,  /*        TX_RSNDC=0            (TX ES ND) Special NDC value for over-saturated Reference, float */
  0x0000,  /*    TX_RSNDC_ALT=0            (TX ES ND) Alternative RSNDC value, float */
  0x4000,  /*         TX_NDCS=0.500015     (TX ES ND) NDC decrease step in case of Reference signal level decrease, float */
  0x7FFF,  /*          TX_PSL=0.00         (TX ES) TX input saturation level (to close ES bands during Far End activity), dB */
  0x00A4,  /*       TX_REFTHR=-46.01       (TX ES) Reference signal actuation threshold, dB */
  0x0000,  /*         TX_NSPR=0            (TX NS) Noise Suppression Processing  0-full, 1-moderate, 2-lite */
  0x4026,  /*          TX_SNS=-6.00        (TX NS) Stationary Noise Suppression gain, dB */
  0x5A9D,  /*        TX_WBNGG=-3.00        (TX NS) Wide band noise gate gain, dB */
  0x7FFF,  /*       TX_NSTSMG=0.00         (TX NS) Tone Suppression minimal gain, dB */
  0x7FFF,  /*     TX_NSTDATHR=0.00         (TX NS) Tone detection absolute threshold, dB */
  0x7FFF,  /*     TX_NSTDRTHR=0.00         (TX NS) Tone detection relative threshold, dB */
  0x7FFF,  /*          TX_BNL=0.00         (TX NS) Band noise level threshold for SNS decrease, dB */
  0x101D,  /*     TX_BNL_MING=-18.00       (TX NS) Minimal allowed BNL gain, dB */
  0x2CFC,  /*        TX_BNLEF=15.00        (TX NS) BNL spectrum curvature factor, dB */
  0x02BC,  /*         TX_SNAT=700          (TX NS) Adaptation time to stationary noises, ms */
  0x02BC,  /*         TX_TNAT=700          (TX NS) Adaptation time to transient noises, ms */
  0x0000,  /*         TX_NTF1=0            (TX NS) Noise threshold factor (defines NS sensitivity), float */
  0x0000,  /*         TX_NTF2=0            (TX NS) Noise Threshold Factor for high frequencies, float */
  0x4000,  /*          TX_NSS=0.500015     (TX NS) Noise Suppression Smoothness, float */
  0x0DAC,  /*       TX_NSHRUF=3500         (TX NS) Noise Suppressor High Resolution Upper Frequency, Hz */
  0x3E80,  /*        TX_NSUBF=16000        (TX NS) Noise Suppressor Upper Band Frequency, Hz */
  0x4000,  /*         TX_SHER=0.500015     (TX NS) Noise Suppression Speech Harmonics Enhancement and Restoration factor, float */
  0x0000,  /*      TX_OUTDRC1=0            (TX outDRC1) TX output 1st DRC saturation level, dB */
  0x0001,  /*  TX_OUTDRC1BEND=-90.31       (TX outDRC1) TX output 1st DRC bend threshold, dB */
  0x0001,  /*  TX_OUTDRC1NGIL=-90.31       (TX outDRC1) TX output 1st DRC noise gate input level, dB */
  0x0001,  /*  TX_OUTDRC1NGOL=-90.31       (TX outDRC1) TX output 1st DRC noise gate output level, dB */
  0x0000,  /*      TX_OUTDRC2=0            (TX outDRC2) TX output 2nd DRC saturation threshold, dB */
  0x0001,  /*  TX_OUTDRC2BEND=-90.31       (TX outDRC2) TX output 2nd DRC bend threshold, dB */
  0x0001,  /*  TX_OUTDRC2NGIL=-90.31       (TX outDRC2) TX output 2nd DRC noise gate input level, dB */
  0x0001,  /*  TX_OUTDRC2NGOL=-90.31       (TX outDRC2) TX output 2nd DRC noise gate output level, dB */
  0x0800,  /*          TX_AGC=0.00         (TX AGC) TX Automatic Gain Control (AGC) maximal gain, dB */
  0x012C,  /*         TX_AGCT=300          (TX AGC) Time to double TX AGC gain, ms */
  0x7FFF,  /*       TX_AGCSAT=0.00         (TX AGC) TX AGC saturation level, dB */
  0x7FFF,  /*       TX_AGCTAR=0.00         (TX AGC) TX AGC target level, dB */
  0x0148,  /*       TX_AGCTHR=-39.99       (TX AGC) TX AGC actuation level, dB */
  0x0014,  /*      TX_AGCFADE=20           (TX AGC) TX AGC decrease time (by -6 dB), sec */
  0x2000,  /*    TX_AGCVADTHR=0.250008     (TX AGC) TX AGC VAD actuation value, float */
  0x0800,  /*           TX_OG=0.00         (TX) TX output digital gain, dB */
  0x0001,  /*         TX_WRNS=-90.31       (TX NS WRNS) Wideband residual noise suppressor threshold, dB */
  0x0000,  /*             STG=-120.00      (GEN) TXout -> RXout side tone gain, dB */
  0x0000,  /*        TX_FRQSH=0            (TX) Quasi-proportional frequency shift for TX output signal, Hz */
  0x0001,  /*        TX_AFTHR=-90.31       (DM TX AF) Signal level threshold for dynamic management of AF, dB */
  0x0001,  /*        TX_ESTHR=-90.31       (DM TX ES) Signal level threshold for dynamic management of ES, dB */
  0x0100,  /*           RX_IG=0.00         (RX) RX input digital gain, dB */
  0x7FFF,  /*      RX_DTGCATT=0.00         (RX DTGC) DTGC attenuation gain in Double Talk, dB */
  0x001E,  /*       RX_DTGCDT=30           (RX DTGC) DTGC gain decrease time, ms */
  0x00FA,  /*       RX_DTGCIT=250          (RX DTGC) DTGC gain increase time, ms */
  0x00C8,  /*       RX_DTGCHT=200          (RX DTGC) DTGC attenuated gain hold time, ms */
  0x7FFF,  /*       RX_DTGCHP=0.00         (RX DTGC) DTGC high-pass filter gain at 1000 Hz in Double Talk, dB */
  0x0003,  /*      RX_DTGCTHR=-80.77       (RX DTGC) DTGC actuation threshold, dB */
  0x7FFF,  /*          RX_VOL=0.00         (RX) RX digital volume, dB */
  0x0068,  /*      RX_AVQNMIN=-49.97       (RX AVQ) AVQ minimal noise level to react on, dB */
  0x0CCD,  /*      RX_AVQNMAX=-20.00       (RX AVQ) AVQ maximal noise level to react on, dB */
  0x0100,  /*     RX_AVQRANGE=0.00         (RX AVQ) Allowed volume (maximal gain increase), dB */
  0x0028,  /*      RX_AVQIFAC=40           (RX AVQ) Gain increase (+6 dB) time, ms */
  0x000F,  /*      RX_AVQDFAC=15           (RX AVQ) Gain decrease (+6 dB) time, ms */
  0x0000,  /*         RX_NSPR=0            (RX NS) Noise Suppression Processing  0-full, 1-moderate, 2-lite */
  0x4026,  /*          RX_SNS=-6.00        (RX NS) Stationary Noise Suppression gain, dB */
  0x5A9D,  /*        RX_WBNGG=-3.00        (RX NS) Wide band noise gate gain, dB */
  0x7FFF,  /*       RX_NSTSMG=0.00         (RX NS) Tone Suppression minimal gain, dB */
  0x7FFF,  /*     RX_NSTDATHR=0.00         (RX NS) Tone detection absolute threshold, dB */
  0x7FFF,  /*     RX_NSTDRTHR=0.00         (RX NS) Tone detection relative threshold, dB */
  0x7FFF,  /*          RX_BNL=0.00         (RX NS) Band noise level threshold for SNS decrease, dB */
  0x101D,  /*     RX_BNL_MING=-18.00       (RX NS) Minimal allowed BNL gain, dB */
  0x2CFC,  /*        RX_BNLEF=15.00        (RX NS) BNL spectrum curvature factor, dB */
  0x02BC,  /*         RX_SNAT=700          (RX NS) Adaptation time to stationary noises, ms */
  0x02BC,  /*         RX_TNAT=700          (RX NS) Adaptation time to transient noises, ms */
  0x0000,  /*         RX_NTF1=0            (RX NS) Noise threshold factor (defines NS sensitivity), float */
  0x0000,  /*         RX_NTF2=0            (RX NS) Noise threshold factor for high frequencies, float */
  0x4000,  /*          RX_NSS=0.500015     (RX NS) Noise Suppression Smoothness, float */
  0x0DAC,  /*       RX_NSHRUF=3500         (RX NS) Noise Suppressor High Resolution Upper Frequency, Hz */
  0x3E80,  /*        RX_NSUBF=16000        (RX NS) Noise Suppressor Upper Band Frequency, Hz */
  0x4000,  /*         RX_SHER=0.500015     (RX NS) Noise Suppression Speech Harmonics Enhancement and Restoration factor, float */
  0x0000,  /*          RX_DRC=0            (RX DRC) RX DRC saturation threshold, dB */
  0x0001,  /*      RX_DRCBEND=-90.31       (RX DRC) RX DRC bend threshold, dB */
  0x0001,  /*      RX_DRCNGIL=-90.31       (RX DRC) RX DRC noise gate input level, dB */
  0x0001,  /*      RX_DRCNGOL=-90.31       (RX DRC) RX DRC noise gate output level, dB */
  0x0800,  /*          RX_AGC=0.00         (RX AGC) RX Automatic Gain Control (AGC) maximal gain, dB */
  0x012C,  /*         RX_AGCT=300          (RX AGC) Time to double RX AGC gain, ms */
  0x7FFF,  /*       RX_AGCSAT=0.00         (RX AGC) RX AGC saturation level, dB */
  0x7FFF,  /*       RX_AGCTAR=0.00         (RX AGC) RX AGC target level, dB */
  0x040C,  /*       RX_AGCTHR=-30.00       (RX AGC) RX AGC actuation level, dB */
  0x0014,  /*      RX_AGCFADE=20           (RX AGC) RX AGC decrease time (by -6 dB), sec */
  0x2000,  /*    RX_AGCVADTHR=0.250008     (RX AGC) RX AGC VAD actuation value, float */
  0x0100,  /*         RX_CLIP=0.00         (RX) RX digital clipping gain, dB */
  0x0800,  /*           RX_OG=0.00         (RX) RX output digital gain, dB */
  0x0001,  /*         RX_WRNS=-90.31       (RX NS WRNS) RX wideband residual noise suppressor threshold, dB */
  0x0068,  /*     RX_NDVCNMIN=-49.97       (RX NDVC) Minimal noise level to react on, dB */
  0x0CCD,  /*     RX_NDVCNMAX=-20.00       (RX NDVC) Maximal noise level to react on, dB */
  0x0100,  /*    RX_NDVCRANGE=0.00         (RX NDVC) Allowed volume (maximal gain increase), dB */
  0x0028,  /*     RX_NDVCIFAC=40           (RX NDVC) Gain increase (+6 dB) time, ms */
  0x000F,  /*     RX_NDVCDFAC=15           (RX NDVC) Gain decrease (+6 dB) time, ms */
  0x03E8,  /*        RX_ELMAD=1000         (RX EL) Maximal accumulated delay, ms */
  0x0028,  /*        RX_ELMSF=40           (RX EL) Maximal time stretch factor, % */
  0x0000,  /*        RX_ELPCT=0            (RX EL) Time for pitch confirmation (for simplified algorithm), float */
  0x0148,  /*       RX_ELTHRV=-39.99       (RX EL) EL threshold voice - signal with level above threshold can be stretched, dB */
  0x0021,  /*       RX_ELTHRN=-59.94       (RX EL) EL threshold noise - signal with level below threshold is considered as noise, dB */
/* FLAGS: value == 0x008C */
 (0 << 0 ) | /*    TX_ADMflag=0   (TX, ADM) ADM, flag (0-off,1-on) */
 (0 << 1 ) | /* TX_ADMOUTflag=0   RESERVED */
 (1 << 2 ) | /* TX_ADMNLPflag=1   (TX, ADM) ADM non-linear processing (NLP), flag (0-off,1-on) */
 (1 << 3 ) | /* TX_ADMLIMflag=1   (TX, ADM) ADM limiter, flag (0-off,1-on) */
 (0 << 4 ) | /*TX_ADMSENSflag=0   (TX, ADM) ADM auto sensitivity adjustment, flag (0-off,1-on) */
 (0 << 5 ) | /*  TX_ADMAMflag=0   (TX, ADM) ADM adaptation during Far Talk only, flag (0-yes,1-no) */
 (0 << 6 ) | /*  TX_ADMESflag=0   (TX, ADM) Take into account ADM attenuation in ES, flag (0-off,1-on) */
 (1 << 7 ) | /* TX_ADMFRCflag=1   (TX, ADM) ADM low frequency compensation, flag (0-off,1-on) */
 (0 << 8 ) | /*TX_ADMREVMflag=0   (TX, ADM) ADM input microphones exchange flag, (0-off,1-on) */
 (0 << 9 ) | /*TX_ADMREVOflag=0   (TX, ADM) ADM reverse rear mic phase flag, (0-off,1-on) */
 (0 << 10) | /*TX_ADMAFRCflag=0   (TX, ADM) ADM frequency compensation type, flag (0-static,1-dynamic) */
 (0 << 11) , /*  TX_ADMRMflag=0   (TX, ADM) ADM reference mode, different adaptation during Far Talk(activity) in band/global activity/no echo/, flag (0-yes,1-no) */
  0x0001,  /*       TX_ADMCNF=1            (TX, ADM) ADM input type: 0-all mics, -1-difference, 1..4-single mics */
  0x0800,  /*        TX_ADMFG=0.00         (TX, ADM) ADM front mic gain, dB */
  0x0800,  /*        TX_ADMRG=0.00         (TX, ADM) ADM rear mic gain, dB */
  0x0000,  /*      TX_ADMMODE=0            (TX, ADM) ADM mode: 0-far field, 1-near field, 2-broad side (car), 3-broad side (headset), 4 - 3mic, 5 - 4mic */
  0x0028,  /*     TX_ADMDIST1=40           (TX, ADM) Distance between microphones, mm */
  0x0004,  /*     TX_ADMDIST2=4            (TX, ADM) Distance between microphones, mm */
  0x0050,  /*       TX_ADMMAA=80           (TX, ADM) Signal acceptance angle, degree */
  0x0000,  /*       TX_ADMSTR=0            (TX, ADM) Steering angle, degree */
  0x0000,  /*       TX_ADMMPE=0            (TX, ADM) Maximal phase error, degree */
  0x7FFF,  /*      TX_ADMARFE=1            (TX, ADM) Adaptation rate factor for echo, float */
  0x7FFF,  /*      TX_ADMARFN=1            (TX, ADM) Adaptation rate factor for noise, float */
  0x0000,  /*       TX_ADMLDF=0            (TX, ADM) Lower directional frequency (ADM is bypassed below it), Hz */
  0x1F40,  /*       TX_ADMUDF=8000         (TX, ADM) Upper directional frequency, Hz */
  0x4026,  /*      TX_ADMNLPG=-6.00        (TX, ADM) ADM NLP minimal gain, dB */
  0x0000,  /*       TX_ADMMEG=0            (TX, ADM) ADM maximal amplitude compensation factor, float */
  0x7FFF,  /*       TX_ADMWNG=0.00         (TX, ADM) Wind noise reduction, minimal gain, dB */
  0x0000,  /*            PAR0=0            (AUX) Reserved parameter 0, int [-32767, 32767] */
  0x0000,  /*            PAR1=0            (AUX) Reserved parameter 1, int [-32767, 32767] */
  0x0000,  /*            PAR2=0            (AUX) Reserved parameter 2, int [-32767, 32767] */
  0x0000,  /*            PAR3=0            (AUX) Reserved parameter 3, int [-32767, 32767] */
  0x0000,  /*            PAR4=0            (AUX) Reserved parameter 4, int [-32767, 32767] */
  0x4668,  /* Date: Wed Apr 08 14:19:24 2015 */
  0x726C,  /* Time: Wed Apr 08 14:19:24 2015 */
  0x0E72,  /* CRC */
  { /* TX_EQ */
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555 
  }, /* TX_EQ end */
  { /* RX_EQ */
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555 
  }, /* RX_EQ end */
  { /* REF_EQ */
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555 
  }, /* REF_EQ end */
  { /* TX_BCT_EQ */
    0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,
    0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,
    0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,
    0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444 
  }, /* TX_BCT_EQ end */
  { /* TX_REAR_EQ */
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555 
  }, /* TX_REAR_EQ end */
  { /* GCT_EQ */
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555 
  }, /* GCT_EQ end */
  { /* NDVC_EQ */
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555 
  }, /* NDVC_EQ end */
  0x0000,  /*       TX1IN_HWG=0            (AUX) TX mic1 input hardware (codec) gain */
  0x0000,  /*       TX2IN_HWG=0            (AUX) TX mic2 input hardware (codec) gain */
  0x0000,  /*       TXOUT_HWG=0            (AUX) TX output hardware (codec) gain */
  0x0000,  /*        RXIN_HWG=0            (AUX) RX input hardware (codec) gain */
  0x0000,  /*       RXOUT_HWG=0            (AUX) RX output hardware (codec) gain */
  0x0000,  /*           AUXP0=0            (AUX) Auxiliary parameter 0, int [-32767, 32767] */
  0x0000,  /*           AUXP1=0            (AUX) Auxiliary parameter 1, int [-32767, 32767] */
  0x0000,  /*           AUXP2=0            (AUX) Auxiliary parameter 2, int [-32767, 32767] */
  { /* LOG */
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
  }, /* LOG end  */
  { /* USERDATA */
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
  }, /* USERDATA end  */
}; /* vcpTProfile profile */
