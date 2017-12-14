#ifndef __IMAP_GPS_H__
#define __IMAP_GPS_H__

#define	ADRAM_EN                (0x00000) /* GPS data ram enable                                */
#define ASTACQ                  (0x00004) /* start acquisition flag                             */
#define AINTSTAT                (0x00008) /* interrupt stat of acquisition operation            */
#define AINTEN                  (0x0000c) /* interrupt enable of acquisition                    */
#define ADRAMSTAT               (0x00010) /* GPS data RAM status control register               */
#define ACRTHRD                 (0x00014) /* Acquisition carrier gen quantize threshold for sin */

/* acquisition instructino regester, n for circular correlation instruction 0 ~ 5 */
#define ASATIDn(x)              (0x00040 + 4 * x) /* GPS acquisition satellite ID                    */
#define ACAINCn(x)              (0x00080 + 4 * x) /* c/a generator increment                         */
#define ACAPHSn(x)              (0x000c0 + 4 * x) /* c/a generator initial phase                     */
#define ACRINCn(x)              (0x00100 + 4 * x) /* carrier generator increment                     */
#define ACRPHSn(x)              (0x00140 + 4 * x) /* carrier generator phase                         */
#define ADFFTBEXPn(x)           (0x00180 + 4 * x) /* FFT block exponent for data FFT                 */
#define AIFFTBEXPn(x)           (0x001c0 + 4 * x) /* FFT block exponent for invert FFT               */
#define ACUTCFGn(x)             (0x00200 + 4 * x) /* multiply effective bit cut configuration        */
#define AFFTSCALEn(x)           (0x00240 + 4 * x) /* scale schedule used by invert fft in CC         */
#define AINSTRn(x)              (0x00300 + 4 * x) /* acquisition instruction register                */
#define ACORMAXn(x)             (0x00380 + 4 * x) /* coherent/incoherent integrate max result        */
#define ACORSMAXn(x)            (0x00400 + 4 * x) /* coherent/incoherent integrate second max result */
#define ACORMEANn(x)            (0x00480 + 4 * x) /* coherent/incoherent integrate mean result       */
#define ACORMAXOFFn(x)          (0x00500 + 4 * x) /* code offset accord to max/smax integrate result */

/* tracking unit (for channel n, offset add 0x00 n00, n: 0 11) */
#define TCAGENn(x)              (0x01000 + 0x100 * x) /* tracking channel n c/a generator enable     */
#define TCRGENn(x)              (0x01004 + 0x100 * x) /* tracking channel n carrier generator enable */
#define TSATIDn(x)              (0x01008 + 0x100 * x) /* tracking channel n satellite ID             */  
#define TCRSEL(x, y)            (0x0100c + 0x100 * x + 4 * y) /* tracking channel n correlate y(3 *) carrier selection */
#define TCRSEL02n(x)            (0x0100c + 0x100 * x) /* tracking channel n correlate 0-1-2 carrier selection */           
#define TCRSEL35n(x)            (0x01010 + 0x100 * x) /* tracking channel n correlate 3-4-5 carrier selection */
#define TCRSEL68n(x)            (0x01014 + 0x100 * x) /* tracking channel n correlate 6-7-8 carrier selection */
#define TDATASET(x, y, z)       (0x01018 + 0x100 * x + 0x14 * y + 4 * z  ) /* tracking channel n correlator y , z value */
#define TCAOFFn(x, y)           (0x01018 + 0x100 * x + 0x14 * y) /* tracking channel n correlator y c/a increment     */
#define TCAINCn(x, y)           (0x0101c + 0x100 * x + 0x14 * y) /* tracking channel n correlator y c/a offset        */
#define TCAPHSn(x, y)           (0x01020 + 0x100 * x + 0x14 * y) /* tracking channel n correlator y c/a phase         */
#define TCRINCn(x, y)           (0x01024 + 0x100 * x + 0x14 * y) /* tracking channel n correlator y carrier increment */
#define TCRPHSn(x, y)           (0x01028 + 0x100 * x + 0x14 * y) /* tracking channel n correlator y carrier phase     */
#define TCAOFF0n(x)             (0x01018 + 0x100 * x) /* tracking channel n correlator 0 c/a increment     */
#define TCAINC0n(x)             (0x0101c + 0x100 * x) /* tracking channel n correlator 0 c/a offset        */
#define TCAPHS0n(x)             (0x01020 + 0x100 * x) /* tracking channel n correlator 0 c/a phase         */
#define TCRINC0n(x)             (0x01024 + 0x100 * x) /* tracking channel n correlator 0 carrier increment */
#define TCRPHS0n(x)             (0x01028 + 0x100 * x) /* tracking channel n correlator 0 carrier phase     */
#define TCAOFF1n(x)             (0x0102c + 0x100 * x) /* tracking channel n correlator 1 c/a increment     */
#define TCAINC1n(x)             (0x01030 + 0x100 * x) /* tracking channel n correlator 1 c/a offset        */
#define TCAPHS1n(x)             (0x01034 + 0x100 * x) /* tracking channel n correlator 1 c/a phase         */
#define TCRINC1n(x)             (0x01038 + 0x100 * x) /* tracking channel n correlator 1 carrier increment */
#define TCRPHS1n(x)             (0x0103c + 0x100 * x) /* tracking channel n correlator 1 carrier phase     */
#define TCAOFF2n(x)             (0x01040 + 0x100 * x) /* tracking channel n correlator 2 c/a increment     */
#define TCAINC2n(x)             (0x01044 + 0x100 * x) /* tracking channel n correlator 2 c/a offset        */
#define TCAPHS2n(x)             (0x01048 + 0x100 * x) /* tracking channel n correlator 2 c/a phase         */
#define TCRINC2n(x)             (0x0104c + 0x100 * x) /* tracking channel n correlator 2 carrier increment */
#define TCRPHS2n(x)             (0x01050 + 0x100 * x) /* tracking channel n correlator 2 carrier phase     */
#define TIACC0n(x, y)           (0x01054 + 0x100 * x + 4 * y) /* tracking channel n 1 channel */
#define TIACC00n(x)             (0x01054 + 0x100 * x) /* tracking channel n 1 channel */
#define TIACC01n(x)             (0x01058 + 0x100 * x) /* tracking channel n 1 channel */
#define TIACC02n(x)             (0x0105c + 0x100 * x) /* tracking channel n 1 channel */
#define TIACC03n(x)             (0x01060 + 0x100 * x) /* tracking channel n 1 channel */
#define TIACC04n(x)             (0x01064 + 0x100 * x) /* tracking channel n 1 channel */
#define TIACC05n(x)             (0x01068 + 0x100 * x) /* tracking channel n 1 channel */
#define TIACC06n(x)             (0x0106c + 0x100 * x) /* tracking channel n 1 channel */
#define TIACC07n(x)             (0x01070 + 0x100 * x) /* tracking channel n 1 channel */
#define TIACC08n(x)             (0x01074 + 0x100 * x) /* tracking channel n 1 channel */
#define TQACC0n(x, y)           (0x01078 + 0x100 * x + 4 * y) /* tracking channel n q channel */
#define TQACC00n(x)             (0x01078 + 0x100 * x) /* tracking channel n q channel */
#define TQACC01n(x)             (0x0107c + 0x100 * x) /* tracking channel n q channel */
#define TQACC02n(x)             (0x01080 + 0x100 * x) /* tracking channel n q channel */
#define TQACC03n(x)             (0x01084 + 0x100 * x) /* tracking channel n q channel */
#define TQACC04n(x)             (0x01088 + 0x100 * x) /* tracking channel n q channel */
#define TQACC05n(x)             (0x0108c + 0x100 * x) /* tracking channel n q channel */
#define TQACC06n(x)             (0x01090 + 0x100 * x) /* tracking channel n q channel */
#define TQACC07n(x)             (0x01094 + 0x100 * x) /* tracking channel n q channel */
#define TQACC08n(x)             (0x01098 + 0x100 * x) /* tracking channel n q channel */ 
#define TTCNTn(x, y)            (0x0109c + 0x100 * x + 4 * y) /* tracking channel n correlator y timer count */
#define TTCNT0n(x)              (0x0109c + 0x100 * x) /* tracking channel n correlator 0 timer count */
#define TTCNT1n(x)              (0x010a0 + 0x100 * x) /* tracking channel n correlator 1 timer count */
#define TTCNT2n(x)              (0x010a4 + 0x100 * x) /* tracking channel n correlator 2 timer count */
#define TCAPHSOn(x, y)          (0x010a8 + 0x100 * x + 4 * y) /* tracking channel n correlator y ca phase out */
#define TCAPHSO0n(x)            (0x010a8 + 0x100 * x) /* tracking channel n correlator 0 ca phase out */
#define TCAPHSO1n(x)            (0x010ac + 0x100 * x) /* tracking channel n correlator 1 ca phase out */
#define TCAPHSO2n(x)            (0x010b0 + 0x100 * x) /* tracking channel n correlator 2 ca phase out */
#define TCRPHSOn(x, y)          (0x010b4 + 0x100 * x + 4 * y) /* tracking channel n correlator y carrier phase out */
#define TCRPHSO0n(x)            (0x010b4 + 0x100 * x) /* tracking channel n correlator 0 carrier phase out */
#define TCRPHSO1n(x)            (0x010b8 + 0x100 * x) /* tracking channel n correlator 1 carrier phase out */
#define TCRPHSO2n(x)            (0x010bc + 0x100 * x) /* tracking channel n correlator 2 carrier phase out */
#define TDMACTRL                (0x01c00) /* tracking module dma control register         */
#define TDMAHADDR               (0x01c04) /* tracking module dma host address register    */
#define TINTSTAT                (0x01c08) /* millisecond interrupt status register        */
#define TINTEN                  (0x01c0c) /* millisecond interrupt enable register        */
#define TCRTHRD                 (0x01c10) /* track carrier gen quantize threshold for sin */

/* timne scale */
#define PPSCNT                  (0x02000) /* PPS count register             */
#define PPSSET                  (0x02004) /* PPS signal set enable register */
#define PPSNUM                  (0x02008) /* PPS number register            */
#define PPSTYPE                 (0x0200c) /* PPS type register              */
#define PPSWID                  (0x02010) /* PPS signal width register      */

/* 
 * GPS data memory space
 *  
 * 0x04000 ~ 0x07ffc 4k * 32 
 */
#define DRAM(x)                 (0x04000 + 4 * x) /* GPS data memory */

/* memory space 
 * 
 * 4k * 32 
 * 
 * A 0x40000 ~ 0x43ffc
 * B 0x44000 ~ 0x47ffc
 * C 0x48000 ~ 0x4bffc
 * D 0x4c000 ~ 0x4fffc
 * E 0x50000 ~ 0x53ffc
 * F 0x54000 ~ 0x57ffc
 * G 0x58000 ~ 0x5bffc
 * H 0x5c000 ~ 0x5fffc
 *     */
#define CRAMA(x)                (0x40000 + 4 * x) /* coherent/incoherent memory A */
#define CRAMB(x)                (0x44000 + 4 * x) /* coherent/incoherent memory B */
#define CRAMC(x)                (0x48000 + 4 * x) /* coherent/incoherent memory C */
#define CRAMD(x)                (0x4c000 + 4 * x) /* coherent/incoherent memory D */
#define CRAME(x)                (0x50000 + 4 * x) /* coherent/incoherent memory E */
#define CRAMF(x)                (0x54000 + 4 * x) /* coherent/incoherent memory F */
#define CRAMG(x)                (0x58000 + 4 * x) /* coherent/incoherent memory G */
#define CRAMH(x)                (0x5c000 + 4 * x) /* coherent/incoherent memory H */

#define CRAM(x, y)              (0x40000 + 0x4000 * x + 4 * y) /* coherent/incoherent memory */

#endif /* imap-gps.h */
