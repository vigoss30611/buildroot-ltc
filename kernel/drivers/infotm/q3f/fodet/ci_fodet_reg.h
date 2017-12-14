#ifndef __IMAPX800_FODET_REG_H__
#define __IMAPX800_FODET_REG_H__


#if defined(__cplusplus)
extern "C" {
#endif


// FODET base address
#if 0
#define FODET_PERI_BASE_ADDR                   FODET_BASE_ADDR
#else
#if defined(CONFIG_COMPILE_RTL) || defined(CONFIG_COMPILE_FPGA)
#define FODET_PERI_BASE_ADDR                   IMAP_FODET_BASE
#else
#define FODET_PERI_BASE_ADDR                   0x00000000
#endif
#endif


// Face Object Detection processor registers offset
//==========================================================
// FODET FOD Integral Image Control Register.
#define FODET_FODIMCTRL_OFFSET                 (0x0000)
#define FODET_FODIMCTRL                        (FODET_PERI_BASE_ADDR + FODET_FODIMCTRL_OFFSET)

/*
 * FODET, FODIMCTRL, SRCFM
 * Source Image Format.
 */
#define FODET_FODIMCTRL_SRCFM_MASK             (0x00001000)
#define FODET_FODIMCTRL_SRCFM_LSBMASK          (0x00000001)
#define FODET_FODIMCTRL_SRCFM_SHIFT            (12)
#define FODET_FODIMCTRL_SRCFM_LENGTH           (1)
#define FODET_FODIMCTRL_SRCFM_DEFAULT          (0)
#define FODET_FODIMCTRL_SRCFM_SIGNED_FIELD     (0)

/*
 * FODET, FODIMCTRL, INTM
 * INTM - Integral Image Mode.
 */
#define FODET_FODIMCTRL_INTM_MASK              (0x00000300)
#define FODET_FODIMCTRL_INTM_LSBMASK           (0x00000003)
#define FODET_FODIMCTRL_INTM_SHIFT             (8)
#define FODET_FODIMCTRL_INTM_LENGTH            (2)
#define FODET_FODIMCTRL_INTM_DEFAULT           (0)
#define FODET_FODIMCTRL_INTM_SIGNED_FIELD      (0)

/*
 * FODET, FODIMCTRL, EN_BUSY
 * EN_BUSY - Enable/Busy.
 */
#define FODET_FODIMCTRL_EN_BUSY_MASK           (0x00000001)
#define FODET_FODIMCTRL_EN_BUSY_LSBMASK        (0x00000001)
#define FODET_FODIMCTRL_EN_BUSY_SHIFT          (0)
#define FODET_FODIMCTRL_EN_BUSY_LENGTH         (1)
#define FODET_FODIMCTRL_EN_BUSY_DEFAULT        (0)
#define FODET_FODIMCTRL_EN_BUSY_SIGNED_FIELD   (0)


// FODET FOD Integral Image Source Address Register.
#define FODET_FODIMSRC_OFFSET                  (0x0004)
#define FODET_FODIMSRC                         (FODET_PERI_BASE_ADDR + FODET_FODIMSRC_OFFSET)

/*
 * FODET, FODIMSRC, SAD
 * SAD - Start Address.
 */
#define FODET_FODIMSRC_SAD_MASK                (0xFFFFFFFF)
#define FODET_FODIMSRC_SAD_LSBMASK             (0xFFFFFFFF)
#define FODET_FODIMSRC_SAD_SHIFT               (0)
#define FODET_FODIMSRC_SAD_LENGTH              (32)
#define FODET_FODIMSRC_SAD_DEFAULT             (0)
#define FODET_FODIMSRC_SAD_SIGNED_FIELD        (0)


// FODET FOD Integral Image Source Stride Register.
#define FODET_FODIMSS_OFFSET                   (0x0008)
#define FODET_FODIMSS                          (FODET_PERI_BASE_ADDR + FODET_FODIMSS_OFFSET)

/*
 * FODET, FODIMSS, SD
 * SD - Image stride (QWORD granularity).
 */
#define FODET_FODIMSS_SD_MASK                  (0x0000FFFF)
#define FODET_FODIMSS_SD_LSBMASK               (0x0000FFFF)
#define FODET_FODIMSS_SD_SHIFT                 (0)
#define FODET_FODIMSS_SD_LENGTH                (16)
#define FODET_FODIMSS_SD_DEFAULT               (0)
#define FODET_FODIMSS_SD_SIGNED_FIELD          (0)


// FODET FOD Integral Image Output Address Register.
#define FODET_FODIMOA_OFFSET                   (0x000C)
#define FODET_FODIMOA                          (FODET_PERI_BASE_ADDR + FODET_FODIMOA_OFFSET)

/*
 * FODET, FODIMOA, SAD
 * SAD - Start Address.
 */
#define FODET_FODIMOA_SAD_MASK                 (0xFFFFFFFF)
#define FODET_FODIMOA_SAD_LSBMASK              (0xFFFFFFFF)
#define FODET_FODIMOA_SAD_SHIFT                (0)
#define FODET_FODIMOA_SAD_LENGTH               (32)
#define FODET_FODIMOA_SAD_DEFAULT              (0)
#define FODET_FODIMOA_SAD_SIGNED_FIELD         (0)


// FODET FOD Integral Image Output Stride Register.
#define FODET_FODIMOS_OFFSET                   (0x0010)
#define FODET_FODIMOS                          (FODET_PERI_BASE_ADDR + FODET_FODIMOS_OFFSET)

/*
 * FODET, FODIMOS, IIS
 * IIS - Integral Image Stride.
 */
#define FODET_FODIMOS_IIS_MASK                 (0x000F0000)
#define FODET_FODIMOS_IIS_LSBMASK              (0x0000000F)
#define FODET_FODIMOS_IIS_SHIFT                (16)
#define FODET_FODIMOS_IIS_LENGTH               (4)
#define FODET_FODIMOS_IIS_DEFAULT              (0)
#define FODET_FODIMOS_IIS_SIGNED_FIELD         (0)

/*
 * FODET, FODIMOS, IIO
 * IIO - Integral Image Offset.
 */
#define FODET_FODIMOS_IIO_MASK                 (0x000007FF)
#define FODET_FODIMOS_IIO_LSBMASK              (0x000007FF)
#define FODET_FODIMOS_IIO_SHIFT                (0)
#define FODET_FODIMOS_IIO_LENGTH               (11)
#define FODET_FODIMOS_IIO_DEFAULT              (0)
#define FODET_FODIMOS_IIO_SIGNED_FIELD         (0)


// FODET FOD Integral Image Source Size Register.
#define FODET_FODIMSZ_OFFSET                   (0x0014)
#define FODET_FODIMSZ                          (FODET_PERI_BASE_ADDR + FODET_FODIMSZ_OFFSET)

/*
 * FODET, FODIMSZ, H
 * H - Height.
 * Height for source image. The max value supported is 240(0xf0).
 */
#define FODET_FODIMSZ_H_MASK                   (0x00FF0000)
#define FODET_FODIMSZ_H_LSBMASK                (0x000000FF)
#define FODET_FODIMSZ_H_SHIFT                  (16)
#define FODET_FODIMSZ_H_LENGTH                 (8)
#define FODET_FODIMSZ_H_DEFAULT                (0)
#define FODET_FODIMSZ_H_SIGNED_FIELD           (0)

/*
 * FODET, FODIMSZ, W
 * W - Width.
 * Width for source image. The max value supported is 320(0x140).
 */
#define FODET_FODIMSZ_W_MASK                   (0x000001FF)
#define FODET_FODIMSZ_W_LSBMASK                (0x000001FF)
#define FODET_FODIMSZ_W_SHIFT                  (0)
#define FODET_FODIMSZ_W_LENGTH                 (9)
#define FODET_FODIMSZ_W_DEFAULT                (0)
#define FODET_FODIMSZ_W_SIGNED_FIELD           (0)


// FODET FOD Integral Image Bus Control Register.
#define FODET_FODIMBCTRL_OFFSET                (0x0018)
#define FODET_FODIMBCTRL                       (FODET_PERI_BASE_ADDR + FODET_FODIMBCTRL_OFFSET)

/*
 * FODET, FODIMBCTRL, MWB
 * MWB - Memory Write Burst Size.
 * Burst size for memory write will be equal or less than MWB. 0 means unlimited.
 */
#define FODET_FODIMBCTRL_MWB_MASK              (0x00000F00)
#define FODET_FODIMBCTRL_MWB_LSBMASK           (0x0000000F)
#define FODET_FODIMBCTRL_MWB_SHIFT             (8)
#define FODET_FODIMBCTRL_MWB_LENGTH            (4)
#define FODET_FODIMBCTRL_MWB_DEFAULT           (0)
#define FODET_FODIMBCTRL_MWB_SIGNED_FIELD      (0)

/*
 * FODET, FODIMBCTRL, MWT
 * MWT - Memory Write Threshold.
 * Request for memory write is issued when data number of FIFO is larger than MWT.
 */
#define FODET_FODIMBCTRL_MWT_MASK              (0x000000F0)
#define FODET_FODIMBCTRL_MWT_LSBMASK           (0x0000000F)
#define FODET_FODIMBCTRL_MWT_SHIFT             (4)
#define FODET_FODIMBCTRL_MWT_LENGTH            (4)
#define FODET_FODIMBCTRL_MWT_DEFAULT           (0)
#define FODET_FODIMBCTRL_MWT_SIGNED_FIELD      (0)

/*
 * FODET, FODIMBCTRL, MRT
 * MRT - Memory Read Threshold.
 * Request for memory read is issued when empty number of FIFO is larger than MRT.
 * Burst size for memory read will be larger than MRT.
 */
#define FODET_FODIMBCTRL_MRT_MASK              (0x0000000F)
#define FODET_FODIMBCTRL_MRT_LSBMASK           (0x0000000F)
#define FODET_FODIMBCTRL_MRT_SHIFT             (0)
#define FODET_FODIMBCTRL_MRT_LENGTH            (4)
#define FODET_FODIMBCTRL_MRT_DEFAULT           (0)
#define FODET_FODIMBCTRL_MRT_SIGNED_FIELD      (0)


// FODET FOD Classifier Control Register.
#define FODET_FODCCTRL_OFFSET                  (0x0020)
#define FODET_FODCCTRL                         (FODET_PERI_BASE_ADDR + FODET_FODCCTRL_OFFSET)

/*
 * FODET, FODCCTRL, X
 * X - X Coordinate(Read Only).
 * Valid when detected register is set.
 */
#define FODET_FODCCTRL_X_MASK                  (0x01FF0000)
#define FODET_FODCCTRL_X_LSBMASK               (0x000001FF)
#define FODET_FODCCTRL_X_SHIFT                 (16)
#define FODET_FODCCTRL_X_LENGTH                (9)
#define FODET_FODCCTRL_X_DEFAULT               (0)
#define FODET_FODCCTRL_X_SIGNED_FIELD          (0)

/*
 * FODET, FODCCTRL, Y
 * Y - Y Coordinate(Read Only).
 * Valid when detected register is set.
 */
#define FODET_FODCCTRL_Y_MASK                  (0x0000FF00)
#define FODET_FODCCTRL_Y_LSBMASK               (0x000000FF)
#define FODET_FODCCTRL_Y_SHIFT                 (8)
#define FODET_FODCCTRL_Y_LENGTH                (8)
#define FODET_FODCCTRL_Y_DEFAULT               (0)
#define FODET_FODCCTRL_Y_SIGNED_FIELD          (0)

/*
 * FODET, FODCCTRL, DG
 * DG - Debug.
 */
#define FODET_FODCCTRL_DG_MASK                 (0x00000008)
#define FODET_FODCCTRL_DG_LSBMASK              (0x00000001)
#define FODET_FODCCTRL_DG_SHIFT                (3)
#define FODET_FODCCTRL_DG_LENGTH               (1)
#define FODET_FODCCTRL_DG_DEFAULT              (0)
#define FODET_FODCCTRL_DG_SIGNED_FIELD         (0)

/*
 * FODET, FODCCTRL, RD
 * RD - Reset/Detected.
 */
#define FODET_FODCCTRL_RD_MASK                 (0x00000004)
#define FODET_FODCCTRL_RD_LSBMASK              (0x00000001)
#define FODET_FODCCTRL_RD_SHIFT                (2)
#define FODET_FODCCTRL_RD_LENGTH               (1)
#define FODET_FODCCTRL_RD_DEFAULT              (0)
#define FODET_FODCCTRL_RD_SIGNED_FIELD         (0)

/*
 * FODET, FODCCTRL, RH
 * RH - Resume/Hold.
 */
#define FODET_FODCCTRL_RH_MASK                 (0x00000002)
#define FODET_FODCCTRL_RH_LSBMASK              (0x00000001)
#define FODET_FODCCTRL_RH_SHIFT                (1)
#define FODET_FODCCTRL_RH_LENGTH               (1)
#define FODET_FODCCTRL_RH_DEFAULT              (0)
#define FODET_FODCCTRL_RH_SIGNED_FIELD         (0)

/*
 * FODET, FODCCTRL, EN_BUSY
 * EN_BUSY - Enable/Busy.
 */
#define FODET_FODCCTRL_EN_BUSY_MASK            (0x00000001)
#define FODET_FODCCTRL_EN_BUSY_LSBMASK         (0x00000001)
#define FODET_FODCCTRL_EN_BUSY_SHIFT           (0)
#define FODET_FODCCTRL_EN_BUSY_LENGTH          (1)
#define FODET_FODCCTRL_EN_BUSY_DEFAULT         (0)
#define FODET_FODCCTRL_EN_BUSY_SIGNED_FIELD    (0)


// FODET FOD Classifier Cascade Address Register.
#define FODET_FODCCA_OFFSET                    (0x0024)
#define FODET_FODCCA                           (FODET_PERI_BASE_ADDR + FODET_FODCCA_OFFSET)

/*
 * FODET, FODCCA, SAD
 * SAD - Start Address.
 */
#define FODET_FODCCA_SAD_MASK                  (0xFFFFFFFF)
#define FODET_FODCCA_SAD_LSBMASK               (0xFFFFFFFF)
#define FODET_FODCCA_SAD_SHIFT                 (0)
#define FODET_FODCCA_SAD_LENGTH                (32)
#define FODET_FODCCA_SAD_DEFAULT               (0)
#define FODET_FODCCA_SAD_SIGNED_FIELD          (0)


// FODET FOD Classifier HID-Cascade Address Register.
#define FODET_FODCHCA_OFFSET                   (0x0028)
#define FODET_FODCHCA                          (FODET_PERI_BASE_ADDR + FODET_FODCHCA_OFFSET)

/*
 * FODET, FODCHCA, SAD
 * SAD - Start Address.
 */
#define FODET_FODCHCA_SAD_MASK                  (0xFFFFFFFF)
#define FODET_FODCHCA_SAD_LSBMASK               (0xFFFFFFFF)
#define FODET_FODCHCA_SAD_SHIFT                 (0)
#define FODET_FODCHCA_SAD_LENGTH                (32)
#define FODET_FODCHCA_SAD_DEFAULT               (0)
#define FODET_FODCHCA_SAD_SIGNED_FIELD          (0)


// FODET FOD Classifier Integral Image Address Register.
#define FODET_FODCIMCA_OFFSET                  (0x002C)
#define FODET_FODCIMCA                         (FODET_PERI_BASE_ADDR + FODET_FODCIMCA_OFFSET)

/*
 * FODET, FODCIMCA, SAD
 * SAD - Start Address.
 */
#define FODET_FODCIMCA_SAD_MASK                (0xFFFFFFFF)
#define FODET_FODCIMCA_SAD_LSBMASK             (0xFFFFFFFF)
#define FODET_FODCIMCA_SAD_SHIFT               (0)
#define FODET_FODCIMCA_SAD_LENGTH              (32)
#define FODET_FODCIMCA_SAD_DEFAULT             (0)
#define FODET_FODCIMCA_SAD_SIGNED_FIELD        (0)


// FODET FOD Classifier Integral Image Stride Register.
#define FODET_FODCIMS_OFFSET                   (0x0030)
#define FODET_FODCIMS                          (FODET_PERI_BASE_ADDR + FODET_FODCIMS_OFFSET)

/*
 * FODET, FODCIMS, IIS
 * IIS - Integral Image Stride.
 * Stride for target sum is 2(SS+2).
 */
#define FODET_FODCIMS_IIS_MASK                 (0x000F0000)
#define FODET_FODCIMS_IIS_LSBMASK              (0x0000000F)
#define FODET_FODCIMS_IIS_SHIFT                (16)
#define FODET_FODCIMS_IIS_LENGTH               (4)
#define FODET_FODCIMS_IIS_DEFAULT              (0)
#define FODET_FODCIMS_IIS_SIGNED_FIELD         (0)

/*
 * FODET, FODCIMS, IIO
 * IIO - Integral Image Offset.
 * Offset for target integral image. 4 byte aligned.
 */
#define FODET_FODCIMS_IIO_MASK                 (0x000007FF)
#define FODET_FODCIMS_IIO_LSBMASK              (0x000007FF)
#define FODET_FODCIMS_IIO_SHIFT                (0)
#define FODET_FODCIMS_IIO_LENGTH               (11)
#define FODET_FODCIMS_IIO_DEFAULT              (0)
#define FODET_FODCIMS_IIO_SIGNED_FIELD         (0)


// FODET FOD Classifier Bus Control Register.
#define FODET_FODCBCTRL_OFFSET                 (0x0034)
#define FODET_FODCBCTRL                        (FODET_PERI_BASE_ADDR + FODET_FODCBCTRL_OFFSET)

/*
 * FODET, FODCBCTRL, MWB
 * MWB - Memory Write Burst Size.
 * Burst size for memory write will be equal or less than MWB. 0 means unlimited.
 */
#define FODET_FODCBCTRL_MWB_MASK               (0x00000F00)
#define FODET_FODCBCTRL_MWB_LSBMASK            (0x0000000F)
#define FODET_FODCBCTRL_MWB_SHIFT              (8)
#define FODET_FODCBCTRL_MWB_LENGTH             (4)
#define FODET_FODCBCTRL_MWB_DEFAULT            (0)
#define FODET_FODCBCTRL_MWB_SIGNED_FIELD       (0)

/*
 * FODET, FODCBCTRL, MWT
 * MWT - Memory Write Threshold.
 * Request for memory write is issued when data number of FIFO is larger than MWT.
 */
#define FODET_FODCBCTRL_MWT_MASK               (0x000000F0)
#define FODET_FODCBCTRL_MWT_LSBMASK            (0x0000000F)
#define FODET_FODCBCTRL_MWT_SHIFT              (4)
#define FODET_FODCBCTRL_MWT_LENGTH             (4)
#define FODET_FODCBCTRL_MWT_DEFAULT            (0)
#define FODET_FODCBCTRL_MWT_SIGNED_FIELD       (0)

/*
 * FODET, FODCBCTRL, MRT
 * MRT - Memory Read Threshold.
 * Request for memory read is issued when empty number of FIFO is larger than MRT.
 * Burst size for memory read will be larger than MRT.
 */
#define FODET_FODCBCTRL_MRT_MASK               (0x0000000F)
#define FODET_FODCBCTRL_MRT_LSBMASK            (0x0000000F)
#define FODET_FODCBCTRL_MRT_SHIFT              (0)
#define FODET_FODCBCTRL_MRT_LENGTH             (4)
#define FODET_FODCBCTRL_MRT_DEFAULT            (0)
#define FODET_FODCBCTRL_MRT_SIGNED_FIELD       (0)


// FODET FOD Classifier Scale and Size Register.
#define FODET_FODCSSZ_OFFSET                   (0x0038)
#define FODET_FODCSSZ                          (FODET_PERI_BASE_ADDR + FODET_FODCSSZ_OFFSET)

/*
 * FODET, FODCSSZ, EWH
 * EWH - EqWin Height.
 * Height for eqWin.
 */
#define FODET_FODCSSZ_EWH_MASK                 (0xFF000000)
#define FODET_FODCSSZ_EWH_LSBMASK              (0x000000FF)
#define FODET_FODCSSZ_EWH_SHIFT                (24)
#define FODET_FODCSSZ_EWH_LENGTH               (8)
#define FODET_FODCSSZ_EWH_DEFAULT              (0)
#define FODET_FODCSSZ_EWH_SIGNED_FIELD         (0)

/*
 * FODET, FODCSSZ, EWW
 * EWW - EqWin Width.
 * Width for eqWin.
 */
#define FODET_FODCSSZ_EWW_MASK                 (0x00FF0000)
#define FODET_FODCSSZ_EWW_LSBMASK              (0x000000FF)
#define FODET_FODCSSZ_EWW_SHIFT                (16)
#define FODET_FODCSSZ_EWW_LENGTH               (8)
#define FODET_FODCSSZ_EWW_DEFAULT              (0)
#define FODET_FODCSSZ_EWW_SIGNED_FIELD         (0)

/*
 * FODET, FODCSSZ, SF
 * SF - Scaling Factor(0.4.10).
 * The max value supported is 12(0x3000).
 */
#define FODET_FODCSSZ_SF_MASK                  (0x00003FFF)
#define FODET_FODCSSZ_SF_LSBMASK               (0x00003FFF)
#define FODET_FODCSSZ_SF_SHIFT                 (0)
#define FODET_FODCSSZ_SF_LENGTH                (14)
#define FODET_FODCSSZ_SF_DEFAULT               (0)
#define FODET_FODCSSZ_SF_SIGNED_FIELD          (0)


// FODET FOD Classifier Window Limit Register.
#define FODET_FODCWL_OFFSET                    (0x003C)
#define FODET_FODCWL                           (FODET_PERI_BASE_ADDR + FODET_FODCWL_OFFSET)

/*
 * FODET, FODCWL, SH
 * SH - Stop Height.
 * Vertical limit for window shift.
 */
#define FODET_FODCWL_SH_MASK                   (0x7F000000)
#define FODET_FODCWL_SH_LSBMASK                (0x0000007F)
#define FODET_FODCWL_SH_SHIFT                  (24)
#define FODET_FODCWL_SH_LENGTH                 (7)
#define FODET_FODCWL_SH_DEFAULT                (0)
#define FODET_FODCWL_SH_SIGNED_FIELD           (0)

/*
 * FODET, FODCWL, SW
 * SW - Stop Width.
 * Horizontal limit for window shift.
 */
#define FODET_FODCWL_SW_MASK                   (0x00FF0000)
#define FODET_FODCWL_SW_LSBMASK                (0x000000FF)
#define FODET_FODCWL_SW_SHIFT                  (16)
#define FODET_FODCWL_SW_LENGTH                 (8)
#define FODET_FODCWL_SW_DEFAULT                (0)
#define FODET_FODCWL_SW_SIGNED_FIELD           (0)

/*
 * FODET, FODCWL, SP
 * SP - Step(0.4.10).
 * Step for window shift on both horizontal and vertical directions.
 */
#define FODET_FODCWL_SP_MASK                   (0x00003FFF)
#define FODET_FODCWL_SP_LSBMASK                (0x00003FFF)
#define FODET_FODCWL_SP_SHIFT                  (0)
#define FODET_FODCWL_SP_LENGTH                 (14)
#define FODET_FODCWL_SP_DEFAULT                (0)
#define FODET_FODCWL_SP_SIGNED_FIELD           (0)


// FODET FOD Classifier Reciprocal Register.
#define FODET_FODCRC_OFFSET                    (0x0040)
#define FODET_FODCRC                           (FODET_PERI_BASE_ADDR + FODET_FODCRC_OFFSET)

/*
 * FODET, FODCRC, RWA
 * RWA - Reciprocal of Window Area (0.11.0).
 * RecWindowArea = (2^20)/(eqWinWidth*eqWinHeight).
 */
#define FODET_FODCRC_RWA_MASK                  (0x00000FFF)
#define FODET_FODCRC_RWA_LSBMASK               (0x00000FFF)
#define FODET_FODCRC_RWA_SHIFT                 (0)
#define FODET_FODCRC_RWA_LENGTH                (12)
#define FODET_FODCRC_RWA_DEFAULT               (0)
#define FODET_FODCRC_RWA_SIGNED_FIELD          (0)


// FODET FOD Classifier Status Index Register.
#define FODET_FODCSIDX_OFFSET                  (0x0044)
#define FODET_FODCSIDX                         (FODET_PERI_BASE_ADDR + FODET_FODCSIDX_OFFSET)

/*
 * FODET, FODCSIDX, IDX
 * IDX - Index.
 */
#define FODET_FODCSIDX_IDX_MASK                (0x00000007)
#define FODET_FODCSIDX_IDX_LSBMASK             (0x00000007)
#define FODET_FODCSIDX_IDX_SHIFT               (0)
#define FODET_FODCSIDX_IDX_LENGTH              (3)
#define FODET_FODCSIDX_IDX_DEFAULT             (0)
#define FODET_FODCSIDX_IDX_SIGNED_FIELD        (0)


// FODET FOD Classifier Status Value Register.
#define FODET_FODCSVAL_OFFSET                  (0x0048)
#define FODET_FODCSVAL                         (FODET_PERI_BASE_ADDR + FODET_FODCSVAL_OFFSET)

/*
 * FODET, FODCSVAL, VAL
 * VAL - Value.
 * Value for debug mode.
 */
#define FODET_FODCSVAL_VAL_MASK                (0xFFFFFFFF)
#define FODET_FODCSVAL_VAL_LSBMASK             (0xFFFFFFFF)
#define FODET_FODCSVAL_VAL_SHIFT               (0)
#define FODET_FODCSVAL_VAL_LENGTH              (32)
#define FODET_FODCSVAL_VAL_DEFAULT             (0)
#define FODET_FODCSVAL_VAL_SIGNED_FIELD        (0)


// FODET FOD AXI Control 0 Register.
#define FODET_FODAXI0_OFFSET                   (0x0080)
#define FODET_FODAXI0                          (FODET_PERI_BASE_ADDR + FODET_FODAXI0_OFFSET)

/*
 * FODET, FODAXI0, IMCID
 * IMCID - Integral Image Cache AXI Read ID.
 */
#define FODET_FODAXI0_IMCID_MASK               (0xFF000000)
#define FODET_FODAXI0_IMCID_LSBMASK            (0x000000FF)
#define FODET_FODAXI0_IMCID_SHIFT              (24)
#define FODET_FODAXI0_IMCID_LENGTH             (8)
#define FODET_FODAXI0_IMCID_DEFAULT            (0)
#define FODET_FODAXI0_IMCID_SIGNED_FIELD       (0)

/*
 * FODET, FODAXI0, HCID
 * HCID - HID-Cascade AXI Read ID.
 */
#define FODET_FODAXI0_HCID_MASK                (0x00FF0000)
#define FODET_FODAXI0_HCID_LSBMASK             (0x000000FF)
#define FODET_FODAXI0_HCID_SHIFT               (16)
#define FODET_FODAXI0_HCID_LENGTH              (8)
#define FODET_FODAXI0_HCID_DEFAULT             (0)
#define FODET_FODAXI0_HCID_SIGNED_FIELD        (0)

/*
 * FODET, FODAXI0, CID
 * CID - Cascade AXI Read ID.
 */
#define FODET_FODAXI0_CID_MASK                 (0x0000FF00)
#define FODET_FODAXI0_CID_LSBMASK              (0x000000FF)
#define FODET_FODAXI0_CID_SHIFT                (8)
#define FODET_FODAXI0_CID_LENGTH               (8)
#define FODET_FODAXI0_CID_DEFAULT              (0)
#define FODET_FODAXI0_CID_SIGNED_FIELD         (0)

/*
 * FODET, FODAXI0, SRCID
 * SRCID - Source Image AXI Read ID.
 */
#define FODET_FODAXI0_SRCID_MASK               (0x000000FF)
#define FODET_FODAXI0_SRCID_LSBMASK            (0x000000FF)
#define FODET_FODAXI0_SRCID_SHIFT              (0)
#define FODET_FODAXI0_SRCID_LENGTH             (8)
#define FODET_FODAXI0_SRCID_DEFAULT            (0)
#define FODET_FODAXI0_SRCID_SIGNED_FIELD       (0)


// FODET FOD AXI Control 1 Register.
#define FODET_FODAXI1_OFFSET                   (0x0084)
#define FODET_FODAXI1                          (FODET_PERI_BASE_ADDR + FODET_FODAXI1_OFFSET)

/*
 * FODET, FODAXI1, FWID
 * FWID - FODET AXI Write ID.
 */
#define FODET_FODAXI1_FWID_MASK                (0x0000FF00)
#define FODET_FODAXI1_FWID_LSBMASK             (0x000000FF)
#define FODET_FODAXI1_FWID_SHIFT               (8)
#define FODET_FODAXI1_FWID_LENGTH              (8)
#define FODET_FODAXI1_FWID_DEFAULT             (0)
#define FODET_FODAXI1_FWID_SIGNED_FIELD        (0)

/*
 * FODET, FODAXI1, HCCID
 * HCCID - HID-Cascade Cache AXI Read ID.
 */
#define FODET_FODAXI1_HCCID_MASK               (0x000000FF)
#define FODET_FODAXI1_HCCID_LSBMASK            (0x000000FF)
#define FODET_FODAXI1_HCCID_SHIFT              (0)
#define FODET_FODAXI1_HCCID_LENGTH             (8)
#define FODET_FODAXI1_HCCID_DEFAULT            (0)
#define FODET_FODAXI1_HCCID_SIGNED_FIELD       (0)


// FODET FOD Cache Control Register.
#define FODET_FODCHCTRL_OFFSET                 (0x0090)
#define FODET_FODCHCTRL                        (FODET_PERI_BASE_ADDR + FODET_FODCHCTRL_OFFSET)

/*
 * FODET, FODCHCTRL, INT
 * INT - Interrupt state.
 */
#define FODET_FODCHCTRL_INT_MASK               (0x01000000)
#define FODET_FODCHCTRL_INT_LSBMASK            (0x00000001)
#define FODET_FODCHCTRL_INT_SHIFT              (24)
#define FODET_FODCHCTRL_INT_LENGTH             (1)
#define FODET_FODCHCTRL_INT_DEFAULT            (0)
#define FODET_FODCHCTRL_INT_SIGNED_FIELD       (0)

/*
 * FODET, FODCHCTRL, HAR
 * HAR - Hold after Renew for classifier run (write zero to resume).
 */
#define FODET_FODCHCTRL_HAR_MASK               (0x00004000)
#define FODET_FODCHCTRL_HAR_LSBMASK            (0x00000001)
#define FODET_FODCHCTRL_HAR_SHIFT              (14)
#define FODET_FODCHCTRL_HAR_LENGTH             (1)
#define FODET_FODCHCTRL_HAR_DEFAULT            (0)
#define FODET_FODCHCTRL_HAR_SIGNED_FIELD       (0)

/*
 * FODET, FODCHCTRL, RHC
 * RHC - Reset HID-Cascade Cache.
 */
#define FODET_FODCHCTRL_RHC_MASK               (0x00002000)
#define FODET_FODCHCTRL_RHC_LSBMASK            (0x00000001)
#define FODET_FODCHCTRL_RHC_SHIFT              (13)
#define FODET_FODCHCTRL_RHC_LENGTH             (1)
#define FODET_FODCHCTRL_RHC_DEFAULT            (0)
#define FODET_FODCHCTRL_RHC_SIGNED_FIELD       (0)

/*
 * FODET, FODCHCTRL, RIC
 * RIC - Reset Integral Image Cache.
 */
#define FODET_FODCHCTRL_RIC_MASK               (0x00001000)
#define FODET_FODCHCTRL_RIC_LSBMASK            (0x00000001)
#define FODET_FODCHCTRL_RIC_SHIFT              (12)
#define FODET_FODCHCTRL_RIC_LENGTH             (1)
#define FODET_FODCHCTRL_RIC_DEFAULT            (0)
#define FODET_FODCHCTRL_RIC_SIGNED_FIELD       (0)

/*
 * FODET, FODCHCTRL, LDHC
 * LDHC - Load HID-Cascade Cache.
 */
#define FODET_FODCHCTRL_LDHC_MASK              (0x00000800)
#define FODET_FODCHCTRL_LDHC_LSBMASK           (0x00000001)
#define FODET_FODCHCTRL_LDHC_SHIFT             (11)
#define FODET_FODCHCTRL_LDHC_LENGTH            (1)
#define FODET_FODCHCTRL_LDHC_DEFAULT           (0)
#define FODET_FODCHCTRL_LDHC_SIGNED_FIELD      (0)

/*
 * FODET, FODCHCTRL, LDIC
 * LDIC - Load Integral Image Cache.
 */
#define FODET_FODCHCTRL_LDIC_MASK              (0x00000400)
#define FODET_FODCHCTRL_LDIC_LSBMASK           (0x00000001)
#define FODET_FODCHCTRL_LDIC_SHIFT             (10)
#define FODET_FODCHCTRL_LDIC_LENGTH            (1)
#define FODET_FODCHCTRL_LDIC_DEFAULT           (0)
#define FODET_FODCHCTRL_LDIC_SIGNED_FIELD      (0)

/*
 * FODET, FODCHCTRL, ENHC
 * ENHC - Enable HID-Cascade Cache.
 */
#define FODET_FODCHCTRL_ENHC_MASK              (0x00000200)
#define FODET_FODCHCTRL_ENHC_LSBMASK           (0x00000001)
#define FODET_FODCHCTRL_ENHC_SHIFT             (9)
#define FODET_FODCHCTRL_ENHC_LENGTH            (1)
#define FODET_FODCHCTRL_ENHC_DEFAULT           (0)
#define FODET_FODCHCTRL_ENHC_SIGNED_FIELD      (0)

/*
 * FODET, FODCHCTRL, ENIC
 * ENIC - Enable Integral Image Cache.
 */
#define FODET_FODCHCTRL_ENIC_MASK              (0x00000100)
#define FODET_FODCHCTRL_ENIC_LSBMASK           (0x00000001)
#define FODET_FODCHCTRL_ENIC_SHIFT             (8)
#define FODET_FODCHCTRL_ENIC_LENGTH            (1)
#define FODET_FODCHCTRL_ENIC_DEFAULT           (0)
#define FODET_FODCHCTRL_ENIC_SIGNED_FIELD      (0)

/*
 * FODET, FODCHCTRL, FRST
 * FRST - FODET Master Reset.
 */
#define FODET_FODCHCTRL_FRST_MASK              (0x00000002)
#define FODET_FODCHCTRL_FRST_LSBMASK           (0x00000001)
#define FODET_FODCHCTRL_FRST_SHIFT             (1)
#define FODET_FODCHCTRL_FRST_LENGTH            (1)
#define FODET_FODCHCTRL_FRST_DEFAULT           (0)
#define FODET_FODCHCTRL_FRST_SIGNED_FIELD      (0)

/*
 * FODET, FODCHCTRL, FEN
 * FEN - FODET Master Enable.
 */
#define FODET_FODCHCTRL_FEN_MASK               (0x00000001)
#define FODET_FODCHCTRL_FEN_LSBMASK            (0x00000001)
#define FODET_FODCHCTRL_FEN_SHIFT              (0)
#define FODET_FODCHCTRL_FEN_LENGTH             (1)
#define FODET_FODCHCTRL_FEN_DEFAULT            (0)
#define FODET_FODCHCTRL_FEN_SIGNED_FIELD       (0)


// FODET FOD Integral Image Cache Address Register.
#define FODET_FODIMCA_OFFSET                   (0x0094)
#define FODET_FODIMCA                          (FODET_PERI_BASE_ADDR + FODET_FODIMCA_OFFSET)

/*
 * FODET, FODIMCA, SAD
 * SAD - Start Address.
 */
#define FODET_FODIMCA_SAD_MASK                 (0xFFFFFFFF)
#define FODET_FODIMCA_SAD_LSBMASK              (0xFFFFFFFF)
#define FODET_FODIMCA_SAD_SHIFT                (0)
#define FODET_FODIMCA_SAD_LENGTH               (32)
#define FODET_FODIMCA_SAD_DEFAULT              (0)
#define FODET_FODIMCA_SAD_SIGNED_FIELD         (0)


// FODET FOD Integral Image Cache Stride Register.
#define FODET_FODIMCS_OFFSET                   (0x0098)
#define FODET_FODIMCS                          (FODET_PERI_BASE_ADDR + FODET_FODIMCS_OFFSET)

/*
 * FODET, FODIMCS, IIS
 * IIS - Integral Image Stride.
 * Stride for target sum is 2(SS+2).
 */
#define FODET_FODIMCS_IIS_MASK                 (0x000F0000)
#define FODET_FODIMCS_IIS_LSBMASK              (0x0000000F)
#define FODET_FODIMCS_IIS_SHIFT                (16)
#define FODET_FODIMCS_IIS_LENGTH               (4)
#define FODET_FODIMCS_IIS_DEFAULT              (0)
#define FODET_FODIMCS_IIS_SIGNED_FIELD         (0)

/*
 * FODET, FODIMCS, IIO
 * IIO - Integral Image Offset.
 * Offset for target integral image. 4 byte aligned.
 */
#define FODET_FODIMCS_IIO_MASK                 (0x000007FF)
#define FODET_FODIMCS_IIO_LSBMASK              (0x000007FF)
#define FODET_FODIMCS_IIO_SHIFT                (0)
#define FODET_FODIMCS_IIO_LENGTH               (11)
#define FODET_FODIMCS_IIO_DEFAULT              (0)
#define FODET_FODIMCS_IIO_SIGNED_FIELD         (0)


// FODET FOD Integral Image Cache Load Address Register.
#define FODET_FODIMCLA_OFFSET                  (0x009C)
#define FODET_FODIMCLA                         (FODET_PERI_BASE_ADDR + FODET_FODIMCLA_OFFSET)

/*
 * FODET, FODIMCLA, SAD
 * SAD - Start Address.
 */
#define FODET_FODIMCLA_SAD_MASK                 (0xFFFFFFFF)
#define FODET_FODIMCLA_SAD_LSBMASK              (0xFFFFFFFF)
#define FODET_FODIMCLA_SAD_SHIFT                (0)
#define FODET_FODIMCLA_SAD_LENGTH               (32)
#define FODET_FODIMCLA_SAD_DEFAULT              (0)
#define FODET_FODIMCLA_SAD_SIGNED_FIELD         (0)


// FODET FOD Integral Image Cache Load Local Address Register.
#define FODET_FODIMCLLA_OFFSET                 (0x00A0)
#define FODET_FODIMCLLA                        (FODET_PERI_BASE_ADDR + FODET_FODIMCLLA_OFFSET)

/*
 * FODET, FODIMCLLA, LAD
 * LAD - Start Address.
 */
#define FODET_FODIMCLLA_LAD_MASK               (0x000FFFFF)
#define FODET_FODIMCLLA_LAD_LSBMASK            (0x000FFFFF)
#define FODET_FODIMCLLA_LAD_SHIFT              (0)
#define FODET_FODIMCLLA_LAD_LENGTH             (20)
#define FODET_FODIMCLLA_LAD_DEFAULT            (0)
#define FODET_FODIMCLLA_LAD_SIGNED_FIELD       (0)


// FODET FOD Integral Image Cache Load Local Length Register.
#define FODET_FODIMCLLL_OFFSET                 (0x00A4)
#define FODET_FODIMCLLL                        (FODET_PERI_BASE_ADDR + FODET_FODIMCLLL_OFFSET)

/*
 * FODET, FODIMCLLL, LEN
 * LEN - Load Length.
 */
#define FODET_FODIMCLLL_LEN_MASK               (0x000FFFFF)
#define FODET_FODIMCLLL_LEN_LSBMASK            (0x000FFFFF)
#define FODET_FODIMCLLL_LEN_SHIFT              (0)
#define FODET_FODIMCLLL_LEN_LENGTH             (20)
#define FODET_FODIMCLLL_LEN_DEFAULT            (0)
#define FODET_FODIMCLLL_LEN_SIGNED_FIELD       (0)


// FODET FOD HID-Cascade Cache Address Register.
#define FODET_FODHCCA_OFFSET                   (0x00B0)
#define FODET_FODHCCA                          (FODET_PERI_BASE_ADDR + FODET_FODHCCA_OFFSET)

/*
 * FODET, FODHCCA, SAD
 * SAD - Start Address.
 */
#define FODET_FODHCCA_SAD_MASK               (0xFFFFFFFF)
#define FODET_FODHCCA_SAD_LSBMASK            (0xFFFFFFFF)
#define FODET_FODHCCA_SAD_SHIFT              (0)
#define FODET_FODHCCA_SAD_LENGTH             (32)
#define FODET_FODHCCA_SAD_DEFAULT            (0)
#define FODET_FODHCCA_SAD_SIGNED_FIELD       (0)


/*!
******************************************************************************

 @Macro	FODET_REGIO_READ_FIELD

 @Title     This macro is used to extract a field from a register.

 @Input		ui32RegValue: 		The register value.

 @Input		group: 		The name of the group containing the register from which
						the field is to be extracted.

 @Input		reg: 		The name of the register from which the field is to
						be extracted.

 @Input		field: 		The name of the field to be extracted.

 @Return	IMG_UINT32:	The value of the field - right aligned.

******************************************************************************/

#if 1
#define FODET_REGIO_READ_FIELD(ui32RegValue, group, reg, field)             \
    ((ui32RegValue & group##_##reg##_##field##_MASK) >> group##_##reg##_##field##_SHIFT)

#else

#define FODET_REGIO_READ_FIELD(ui32RegValue, group, reg, field)             \
    ((ui32RegValue >> group##_##reg##_##field##_SHIFT) & group##_##reg##_##field##_LSBMASK)

#endif

#if ( defined WIN32 || defined __linux__ ) && !defined NO_REGIO_CHECK_FIELD_VALUE
/* Only provide register field range checking for Windows and Linux builds */

/* Simple range check that ensures that if bits outside the valid field		*/
/* range are set, that the provided value is at least consistent with a		*/
/* negative value (i.e.: all top bits are set to 1).						*/

/* Cannot perform more comprehensive testing without knowing whether field	*/
/* should be interpreted as signed or unsigned.								*/

#ifdef IMG_KERNEL_MODULE
#define FODET_REG_PRINT(...) printk(KERN_DEBUG __VA_ARGS__)
#else
#define FODET_REG_PRINT printf
#endif

#define FODET_REGIO_CHECK_VALUE_FITS_WITHIN_FIELD(This_Group,This_Reg,This_Field,This_Value)                                \
{                                                                                                                           \
    if (((FODET_UINT32)(FODET_INT32)This_Value) > (This_Group##_##This_Reg##_##This_Field##_LSBMASK) &&                     \
        (((FODET_UINT32)(FODET_INT32)This_Value) & (FODET_UINT32)~(This_Group##_##This_Reg##_##This_Field##_LSBMASK)) !=    \
        (FODET_UINT32)~(This_Group##_##This_Reg##_##This_Field##_LSBMASK))                                                  \
    {                                                                                                                       \
       FODET_REG_PRINT("%s: %d does not fit in %s:%s:%s\n", __FUNCTION__, This_Value, #This_Group, #This_Reg, #This_Field); \
    }                                                                                                                       \
}

#else
#define FODET_REGIO_CHECK_VALUE_FITS_WITHIN_FIELD(This_Group,This_Reg,This_Field,This_Value)
#endif

/*!
******************************************************************************

 @Macro	FODET_REGIO_WRITE_FIELD

 @Title     This macro is used to update the value of a field in a register.

 @Input		ui32RegValue: 	The register value - which gets updated.

 @Input		group: 		The name of the group containing the register into which
						the field is to be written.

 @Input		reg: 		The name of the register into which the field is to
						be written.

 @Input		field: 		The name of the field to be updated.

 @Input		ui32Value: 	The value to be written to the field - right aligned.

 @Return	None.

******************************************************************************/
#define FODET_REGIO_WRITE_FIELD(ui32RegValue, group, reg, field, ui32Value)                                     \
{                                                                                                               \
    FODET_REGIO_CHECK_VALUE_FITS_WITHIN_FIELD(group,reg,field,ui32Value);                                       \
    (ui32RegValue) =                                                                                            \
        ((ui32RegValue) & ~(group##_##reg##_##field##_MASK)) |                                                  \
        (((FODET_UINT32) (ui32Value) << (group##_##reg##_##field##_SHIFT)) & (group##_##reg##_##field##_MASK)); \
}

/*!
******************************************************************************

 @Macro	FODET_REGIO_WRITE_FIELD_LITE

 @Title     This macro is used to update the value of a field in a register.

 @note      It assumes that ui32RegValue is initially zero and thus no masking is required.

 @Input		ui32RegValue: 	The register value - which gets updated.

 @Input		group: 		The name of the group containing the register into which
						the field is to be written.

 @Input		reg: 		The name of the register into which the field is to
						be written.

 @Input		field: 		The name of the field to be updated.

 @Input		ui32Value: 	The value to be written to the field - right aligned.

 @Return	None.

******************************************************************************/
#define FODET_REGIO_WRITE_FIELD_LITE(ui32RegValue, group, reg, field, ui32Value)            \
{                                                                                           \
    FODET_REGIO_CHECK_VALUE_FITS_WITHIN_FIELD(group,reg,field,ui32Value);                   \
    (ui32RegValue) |= ( (FODET_UINT32) (ui32Value) << (group##_##reg##_##field##_SHIFT) );  \
}

/*!
******************************************************************************

 @Macro	FODET_REGIO_ENCODE_FIELD

 @Description

 This macro shifts a value to its correct position in a register. This is used
 for statically defining register values

 @Input		group: 		The name of the group containing the register into which
						the field is to be written.

 @Input		reg: 		The name of the register into which the field is to
						be written.

 @Input		field: 		The name of the field to be updated.
 @Input		ui32Value: 	The value to be written to the field - right aligned.

 @Return	None.

******************************************************************************/
#define FODET_REGIO_ENCODE_FIELD(group, reg, field, ui32Value)              \
    ( (FODET_UINT32) (ui32Value) << (group##_##reg##_##field##_SHIFT) )


#if defined(__cplusplus)
}
#endif


#endif //__IMAPX800_FODET_REG_H__
