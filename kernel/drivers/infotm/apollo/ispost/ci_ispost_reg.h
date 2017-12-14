#ifndef __IMAPX800_ISPOST_REG_H__
#define __IMAPX800_ISPOST_REG_H__


#if defined(__cplusplus)
extern "C" {
#endif


// ISP post processor registers offset.
//==========================================================
// ISPOST Control Register 0.
#define ISPOST_ISPCTRL0_OFFSET                 (0x0000)
#define ISPOST_ISPCTRL0                        (ISPOST_PERI_BASE_ADDR + ISPOST_ISPCTRL0_OFFSET)

/*
 * ISPOST, ISPCTRL0, INT
 * Interrupt state.
 */
#define ISPOST_ISPCTRL0_INT_MASK               (0x01000000)
#define ISPOST_ISPCTRL0_INT_LSBMASK            (0x00000001)
#define ISPOST_ISPCTRL0_INT_SHIFT              (24)
#define ISPOST_ISPCTRL0_INT_LENGTH             (1)
#define ISPOST_ISPCTRL0_INT_DEFAULT            (0)
#define ISPOST_ISPCTRL0_INT_SIGNED_FIELD       (0)

/*
 * ISPOST, ISPCTRL0, ENSS1
 * Enable Scaled Stream 1.
 */
#define ISPOST_ISPCTRL0_ENSS1_MASK             (0x00001000)
#define ISPOST_ISPCTRL0_ENSS1_LSBMASK          (0x00000001)
#define ISPOST_ISPCTRL0_ENSS1_SHIFT            (12)
#define ISPOST_ISPCTRL0_ENSS1_LENGTH           (1)
#define ISPOST_ISPCTRL0_ENSS1_DEFAULT          (0)
#define ISPOST_ISPCTRL0_ENSS1_SIGNED_FIELD     (0)

/*
 * ISPOST, ISPCTRL0, ENSS0
 * Enable Scaled Stream 0.
 */
#define ISPOST_ISPCTRL0_ENSS0_MASK             (0x00000800)
#define ISPOST_ISPCTRL0_ENSS0_LSBMASK          (0x00000001)
#define ISPOST_ISPCTRL0_ENSS0_SHIFT            (11)
#define ISPOST_ISPCTRL0_ENSS0_LENGTH           (1)
#define ISPOST_ISPCTRL0_ENSS0_DEFAULT          (0)
#define ISPOST_ISPCTRL0_ENSS0_SIGNED_FIELD     (0)

/*
 * ISPOST, ISPCTRL0, ENDN
 * Enable Denoise module.
 */
#define ISPOST_ISPCTRL0_ENDN_MASK              (0x00000400)
#define ISPOST_ISPCTRL0_ENDN_LSBMASK           (0x00000001)
#define ISPOST_ISPCTRL0_ENDN_SHIFT             (10)
#define ISPOST_ISPCTRL0_ENDN_LENGTH            (1)
#define ISPOST_ISPCTRL0_ENDN_DEFAULT           (0)
#define ISPOST_ISPCTRL0_ENDN_SIGNED_FIELD      (0)

/*
 * ISPOST, ISPCTRL0, ENOV
 * Enable Overlay module.
 */
#define ISPOST_ISPCTRL0_ENOV_MASK              (0x00000200)
#define ISPOST_ISPCTRL0_ENOV_LSBMASK           (0x00000001)
#define ISPOST_ISPCTRL0_ENOV_SHIFT             (9)
#define ISPOST_ISPCTRL0_ENOV_LENGTH            (1)
#define ISPOST_ISPCTRL0_ENOV_DEFAULT           (0)
#define ISPOST_ISPCTRL0_ENOV_SIGNED_FIELD      (0)

/*
 * ISPOST, ISPCTRL0, ENLC
 * Enable Lens Correction module.
 */
#define ISPOST_ISPCTRL0_ENLC_MASK              (0x00000100)
#define ISPOST_ISPCTRL0_ENLC_LSBMASK           (0x00000001)
#define ISPOST_ISPCTRL0_ENLC_SHIFT             (8)
#define ISPOST_ISPCTRL0_ENLC_LENGTH            (1)
#define ISPOST_ISPCTRL0_ENLC_DEFAULT           (0)
#define ISPOST_ISPCTRL0_ENLC_SIGNED_FIELD      (0)

/*
 * ISPOST, ISPCTRL0, RST
 * Reset/initialize module (necessary before enabling).
 */
#define ISPOST_ISPCTRL0_RST_MASK               (0x00000002)
#define ISPOST_ISPCTRL0_RST_LSBMASK            (0x00000001)
#define ISPOST_ISPCTRL0_RST_SHIFT              (1)
#define ISPOST_ISPCTRL0_RST_LENGTH             (1)
#define ISPOST_ISPCTRL0_RST_DEFAULT            (0)
#define ISPOST_ISPCTRL0_RST_SIGNED_FIELD       (0)

/*
 * ISPOST, ISPCTRL0, EN
 * Enable ISP Post processor module.
 */
#define ISPOST_ISPCTRL0_EN_MASK                (0x00000001)
#define ISPOST_ISPCTRL0_EN_LSBMASK             (0x00000001)
#define ISPOST_ISPCTRL0_EN_SHIFT               (0)
#define ISPOST_ISPCTRL0_EN_LENGTH              (1)
#define ISPOST_ISPCTRL0_EN_DEFAULT             (0)
#define ISPOST_ISPCTRL0_EN_SIGNED_FIELD        (0)


// ISPOST Status Register 0.
#define ISPOST_ISPSTAT0_OFFSET                 (0x0004)
#define ISPOST_ISPSTAT0                        (ISPOST_PERI_BASE_ADDR + ISPOST_ISPSTAT0_OFFSET)

/*
 * ISPOST, ISPSTAT0, STAT_SS1
 * Scaled Stream 1 run status.
 */
#define ISPOST_ISPSTAT0_STAT_SS1_MASK          (0x00000008)
#define ISPOST_ISPSTAT0_STAT_SS1_LSBMASK       (0x00000001)
#define ISPOST_ISPSTAT0_STAT_SS1_SHIFT         (3)
#define ISPOST_ISPSTAT0_STAT_SS1_LENGTH        (1)
#define ISPOST_ISPSTAT0_STAT_SS1_DEFAULT       (0)
#define ISPOST_ISPSTAT0_STAT_SS1_SIGNED_FIELD  (0)

/*
 * ISPOST, ISPSTAT0, STAT_SS0
 * Scaled Stream 0 run status.
 */
#define ISPOST_ISPSTAT0_STAT_SS0_MASK          (0x00000004)
#define ISPOST_ISPSTAT0_STAT_SS0_LSBMASK       (0x00000001)
#define ISPOST_ISPSTAT0_STAT_SS0_SHIFT         (2)
#define ISPOST_ISPSTAT0_STAT_SS0_LENGTH        (1)
#define ISPOST_ISPSTAT0_STAT_SS0_DEFAULT       (0)
#define ISPOST_ISPSTAT0_STAT_SS0_SIGNED_FIELD  (0)

/*
 * ISPOST, ISPSTAT0, STAT_DN
 * Denoise module run status.
 */
#define ISPOST_ISPSTAT0_STAT_DN_MASK           (0x00000002)
#define ISPOST_ISPSTAT0_STAT_DN_LSBMASK        (0x00000001)
#define ISPOST_ISPSTAT0_STAT_DN_SHIFT          (1)
#define ISPOST_ISPSTAT0_STAT_DN_LENGTH         (1)
#define ISPOST_ISPSTAT0_STAT_DN_DEFAULT        (0)
#define ISPOST_ISPSTAT0_STAT_DN_SIGNED_FIELD   (0)

/*
 * ISPOST, ISPSTAT0, STAT0
 * ISPOST run status.
 */
#define ISPOST_ISPSTAT0_STAT0_MASK             (0x00000001)
#define ISPOST_ISPSTAT0_STAT0_LSBMASK          (0x00000001)
#define ISPOST_ISPSTAT0_STAT0_SHIFT            (0)
#define ISPOST_ISPSTAT0_STAT0_LENGTH           (1)
#define ISPOST_ISPSTAT0_STAT0_DEFAULT          (0)
#define ISPOST_ISPSTAT0_STAT0_SIGNED_FIELD     (0)



// Lens distortion correction registers offset.
//==========================================================
// LC Source Image Y Plane Start Address Register.
#define ISPOST_LCSRCAY_OFFSET                  (0x0008)
#define ISPOST_LCSRCAY                         (ISPOST_PERI_BASE_ADDR + ISPOST_LCSRCAY_OFFSET)

/*
 * ISPOST, LCSRCAY, SAD
 * Base address in QWORD (64bit) boundary.
 */
#define ISPOST_LCSRCAY_SAD_MASK                (0xFFFFFFF8)
#define ISPOST_LCSRCAY_SAD_LSBMASK             (0x1FFFFFFF)
#define ISPOST_LCSRCAY_SAD_SHIFT               (3)
#define ISPOST_LCSRCAY_SAD_LENGTH              (29)
#define ISPOST_LCSRCAY_SAD_DEFAULT             (0)
#define ISPOST_LCSRCAY_SAD_SIGNED_FIELD        (0)


// LC Source Image UV Plane Start Address Register.
#define ISPOST_LCSRCAUV_OFFSET                 (0x0010)
#define ISPOST_LCSRCAUV                        (ISPOST_PERI_BASE_ADDR + ISPOST_LCSRCAUV_OFFSET)

/*
 * ISPOST, LCSRCAUV, SAD
 * Base address in QWORD (64bit) boundary.
 */
#define ISPOST_LCSRCAUV_SAD_MASK               (0xFFFFFFF8)
#define ISPOST_LCSRCAUV_SAD_LSBMASK            (0x1FFFFFFF)
#define ISPOST_LCSRCAUV_SAD_SHIFT              (3)
#define ISPOST_LCSRCAUV_SAD_LENGTH             (29)
#define ISPOST_LCSRCAUV_SAD_DEFAULT            (0)
#define ISPOST_LCSRCAUV_SAD_SIGNED_FIELD       (0)


// LC Source Image Stride Register.
#define ISPOST_LCSRCS_OFFSET                   (0x0014)
#define ISPOST_LCSRCS                          (ISPOST_PERI_BASE_ADDR + ISPOST_LCSRCS_OFFSET)

/*
 * ISPOST, LCSRCS, SD
 * Image stride (QWORD granularity).
 */
#define ISPOST_LCSRCS_SD_MASK                  (0x00001FF8)
#define ISPOST_LCSRCS_SD_LSBMASK               (0x000003FF)
#define ISPOST_LCSRCS_SD_SHIFT                 (3)
#define ISPOST_LCSRCS_SD_LENGTH                (10)
#define ISPOST_LCSRCS_SD_DEFAULT               (0)
#define ISPOST_LCSRCS_SD_SIGNED_FIELD          (0)


// LC Source Image Size Register.
#define ISPOST_LCSRCSZ_OFFSET                  (0x0018)
#define ISPOST_LCSRCSZ                         (ISPOST_PERI_BASE_ADDR + ISPOST_LCSRCSZ_OFFSET)

/*
 * ISPOST, LCSRCSZ, W
 * Image width (pixel granularity).
 */
#define ISPOST_LCSRCSZ_W_MASK                  (0x1FFF0000)
#define ISPOST_LCSRCSZ_W_LSBMASK               (0x00001FFF)
#define ISPOST_LCSRCSZ_W_SHIFT                 (16)
#define ISPOST_LCSRCSZ_W_LENGTH                (13)
#define ISPOST_LCSRCSZ_W_DEFAULT               (0)
#define ISPOST_LCSRCSZ_W_SIGNED_FIELD          (0)

/*
 * ISPOST, LCSRCSZ, H
 * Image height (line granularity).
 */
#define ISPOST_LCSRCSZ_H_MASK                  (0x00001FFF)
#define ISPOST_LCSRCSZ_H_LSBMASK               (0x00001FFF)
#define ISPOST_LCSRCSZ_H_SHIFT                 (0)
#define ISPOST_LCSRCSZ_H_LENGTH                (13)
#define ISPOST_LCSRCSZ_H_DEFAULT               (0)
#define ISPOST_LCSRCSZ_H_SIGNED_FIELD          (0)


// LC Source Image Back Fill Color Register.
#define ISPOST_LCSRCBFC_OFFSET                 (0x001C)
#define ISPOST_LCSRCBFC                        (ISPOST_PERI_BASE_ADDR + ISPOST_LCSRCBFC_OFFSET)

/*
 * ISPOST, LCSRCBFC, Y
 * Source Back Fill Y component value. (0-255).
 */
#define ISPOST_LCSRCBFC_Y_MASK                 (0x00FF0000)
#define ISPOST_LCSRCBFC_Y_LSBMASK              (0x000000FF)
#define ISPOST_LCSRCBFC_Y_SHIFT                (16)
#define ISPOST_LCSRCBFC_Y_LENGTH               (8)
#define ISPOST_LCSRCBFC_Y_DEFAULT              (0)
#define ISPOST_LCSRCBFC_Y_SIGNED_FIELD         (0)

/*
 * ISPOST, LCSRCBFC, CB
 * Source Back Fill CB component value (0-255).
 */
#define ISPOST_LCSRCBFC_CB_MASK                (0x0000FF00)
#define ISPOST_LCSRCBFC_CB_LSBMASK             (0x000000FF)
#define ISPOST_LCSRCBFC_CB_SHIFT               (8)
#define ISPOST_LCSRCBFC_CB_LENGTH              (8)
#define ISPOST_LCSRCBFC_CB_DEFAULT             (0)
#define ISPOST_LCSRCBFC_CB_SIGNED_FIELD        (0)

/*
 * ISPOST, LCSRCBFC, CR
 * Source Back Fill CR component value (0-255).
 */
#define ISPOST_LCSRCBFC_CR_MASK                (0x000000FF)
#define ISPOST_LCSRCBFC_CR_LSBMASK             (0x000000FF)
#define ISPOST_LCSRCBFC_CR_SHIFT               (0)
#define ISPOST_LCSRCBFC_CR_LENGTH              (8)
#define ISPOST_LCSRCBFC_CR_DEFAULT             (0)
#define ISPOST_LCSRCBFC_CR_SIGNED_FIELD        (0)


// LC Grid Buffer Start Address Register.
#define ISPOST_LCGBA_OFFSET                    (0x0020)
#define ISPOST_LCGBA                           (ISPOST_PERI_BASE_ADDR + ISPOST_LCGBA_OFFSET)

/*
 * ISPOST, LCGBA, SAD
 * Base address in QWORD (64bit) boundary.
 */
#define ISPOST_LCGBA_SAD_MASK                  (0xFFFFFFF8)
#define ISPOST_LCGBA_SAD_LSBMASK               (0x1FFFFFFF)
#define ISPOST_LCGBA_SAD_SHIFT                 (3)
#define ISPOST_LCGBA_SAD_LENGTH                (29)
#define ISPOST_LCGBA_SAD_DEFAULT               (0)
#define ISPOST_LCGBA_SAD_SIGNED_FIELD          (0)


// LC Grid Buffer Stride Register.
#define ISPOST_LCGBS_OFFSET                    (0x0024)
#define ISPOST_LCGBS                           (ISPOST_PERI_BASE_ADDR + ISPOST_LCGBS_OFFSET)

/*
 * ISPOST, LCGBS, SD
 * Image stride (QWORD granularity).
 */
#define ISPOST_LCGBS_SD_MASK                   (0x00001FF8)
#define ISPOST_LCGBS_SD_LSBMASK                (0x000003FF)
#define ISPOST_LCGBS_SD_SHIFT                  (3)
#define ISPOST_LCGBS_SD_LENGTH                 (10)
#define ISPOST_LCGBS_SD_DEFAULT                (0)
#define ISPOST_LCGBS_SD_SIGNED_FIELD           (0)


// LC Grid Size Register.
#define ISPOST_LCGBSZ_OFFSET                   (0x0028)
#define ISPOST_LCGBSZ                          (ISPOST_PERI_BASE_ADDR + ISPOST_LCGBSZ_OFFSET)

/*
 * ISPOST, LCGBSZ, LBEN
 * Grid line buffer enable.
 */
#define ISPOST_LCGBSZ_LBEN_MASK                (0x00000100)
#define ISPOST_LCGBSZ_LBEN_LSBMASK             (0x00000001)
#define ISPOST_LCGBSZ_LBEN_SHIFT               (8)
#define ISPOST_LCGBSZ_LBEN_LENGTH              (1)
#define ISPOST_LCGBSZ_LBEN_DEFAULT             (0)
#define ISPOST_LCGBSZ_LBEN_SIGNED_FIELD        (0)

/*
 * ISPOST, LCGBSZ, SIZE
 * GRID size.
 */
#define ISPOST_LCGBSZ_SIZE_MASK                (0x00000003)
#define ISPOST_LCGBSZ_SIZE_LSBMASK             (0x00000003)
#define ISPOST_LCGBSZ_SIZE_SHIFT               (0)
#define ISPOST_LCGBSZ_SIZE_LENGTH              (2)
#define ISPOST_LCGBSZ_SIZE_DEFAULT             (0)
#define ISPOST_LCGBSZ_SIZE_SIGNED_FIELD        (0)


// LC Pixel Cache Mode Register.
#define ISPOST_LCPCM_OFFSET                    (0x0030)
#define ISPOST_LCPCM                           (ISPOST_PERI_BASE_ADDR + ISPOST_LCPCM_OFFSET)

/*
 * ISPOST, LCPCM, PREF
 * PREF - Cache controller cfg value.
 */
#define ISPOST_LCPCM_PREF_MASK                 (0x00000070)
#define ISPOST_LCPCM_PREF_LSBMASK              (0x00000007)
#define ISPOST_LCPCM_PREF_SHIFT                (4)
#define ISPOST_LCPCM_PREF_LENGTH               (3)
#define ISPOST_LCPCM_PREF_DEFAULT              (0)
#define ISPOST_LCPCM_PREF_SIGNED_FIELD         (0)

/*
 * ISPOST, LCPCM, CBCRSWAP
 * CB/CR swap - 0: No swap, 1: swap.
 */
#define ISPOST_LCPCM_CBCRSWAP_MASK             (0x00000001)
#define ISPOST_LCPCM_CBCRSWAP_LSBMASK          (0x00000001)
#define ISPOST_LCPCM_CBCRSWAP_SHIFT            (0)
#define ISPOST_LCPCM_CBCRSWAP_LENGTH           (1)
#define ISPOST_LCPCM_CBCRSWAP_DEFAULT          (0)
#define ISPOST_LCPCM_CBCRSWAP_SIGNED_FIELD     (0)


// LC Pixel Coordinate Generator Mode Register.
#define ISPOST_LCPGM_OFFSET                    (0x0034)
#define ISPOST_LCPGM                           (ISPOST_PERI_BASE_ADDR + ISPOST_LCPGM_OFFSET)

/*
 * ISPOST, LCPGM, SCANW
 * SCANW - scan width.
 */
#define ISPOST_LCPGM_SCANW_MASK                (0x00000300)
#define ISPOST_LCPGM_SCANW_LSBMASK             (0x00000003)
#define ISPOST_LCPGM_SCANW_SHIFT               (8)
#define ISPOST_LCPGM_SCANW_LENGTH              (2)
#define ISPOST_LCPGM_SCANW_DEFAULT             (0)
#define ISPOST_LCPGM_SCANW_SIGNED_FIELD        (0)

/*
 * ISPOST, LCPGM, SCANH
 * SCANH - scan height.
 */
#define ISPOST_LCPGM_SCANH_MASK                (0x00000030)
#define ISPOST_LCPGM_SCANH_LSBMASK             (0x00000003)
#define ISPOST_LCPGM_SCANH_SHIFT               (4)
#define ISPOST_LCPGM_SCANH_LENGTH              (2)
#define ISPOST_LCPGM_SCANH_DEFAULT             (0)
#define ISPOST_LCPGM_SCANH_SIGNED_FIELD        (0)

/*
 * ISPOST, LCPGM, SCANM
 * SCANM - scan mode.
 */
#define ISPOST_LCPGM_SCANM_MASK                (0x00000007)
#define ISPOST_LCPGM_SCANM_LSBMASK             (0x00000007)
#define ISPOST_LCPGM_SCANM_SHIFT               (0)
#define ISPOST_LCPGM_SCANM_LENGTH              (3)
#define ISPOST_LCPGM_SCANM_DEFAULT             (0)
#define ISPOST_LCPGM_SCANM_SIGNED_FIELD        (0)


// LC Pixel Coordinate Generator Destination Size Register.
#define ISPOST_LCPGSZ_OFFSET                   (0x0038)
#define ISPOST_LCPGSZ                          (ISPOST_PERI_BASE_ADDR + ISPOST_LCPGSZ_OFFSET)

/*
 * ISPOST, LCPGSZ, W
 * Destination image width (pixel granularity).
 */
#define ISPOST_LCPGSZ_W_MASK                   (0x1FFF0000)
#define ISPOST_LCPGSZ_W_LSBMASK                (0x00001FFF)
#define ISPOST_LCPGSZ_W_SHIFT                  (16)
#define ISPOST_LCPGSZ_W_LENGTH                 (13)
#define ISPOST_LCPGSZ_W_DEFAULT                (0)
#define ISPOST_LCPGSZ_W_SIGNED_FIELD           (0)

/*
 * ISPOST, LCPGSZ, H
 * Destination image width (pixel granularity).
 */
#define ISPOST_LCPGSZ_H_MASK                   (0x00001FFF)
#define ISPOST_LCPGSZ_H_LSBMASK                (0x00001FFF)
#define ISPOST_LCPGSZ_H_SHIFT                  (0)
#define ISPOST_LCPGSZ_H_LENGTH                 (13)
#define ISPOST_LCPGSZ_H_DEFAULT                (0)
#define ISPOST_LCPGSZ_H_SIGNED_FIELD           (0)


// LC Mirror and Rotation Mode Register.
#define ISPOST_LCMRM_OFFSET                    (0x0040)
#define ISPOST_LCMRM                           (ISPOST_PERI_BASE_ADDR + ISPOST_LCMRM_OFFSET)

/*
 * ISPOST, LCMRM, MRMODE
 * MRMODE – Mirror and Rotation mode.
 */
#define ISPOST_LCMRM_MRMODE_MASK               (0x0000000F)
#define ISPOST_LCMRM_MRMODE_LSBMASK            (0x0000000F)
#define ISPOST_LCMRM_MRMODE_SHIFT              (0)
#define ISPOST_LCMRM_MRMODE_LENGTH             (4)
#define ISPOST_LCMRM_MRMODE_DEFAULT            (0)
#define ISPOST_LCMRM_MRMODE_SIGNED_FIELD       (0)


// LC Mirror and Rotation Pre Offset Register.
#define ISPOST_LCMRPREO_OFFSET                 (0x0044)
#define ISPOST_LCMRPREO                        (ISPOST_PERI_BASE_ADDR + ISPOST_LCMRPREO_OFFSET)

/*
 * ISPOST, LCMRPREO, XCPRE
 * XCPRE – prerotation horizontal offset.
 * Horizontal offset applied after rotation (1.14.1 format).
 */
#define ISPOST_LCMRPREO_XCPRE_MASK             (0xFFFF0000)
#define ISPOST_LCMRPREO_XCPRE_LSBMASK          (0x0000FFFF)
#define ISPOST_LCMRPREO_XCPRE_SHIFT            (16)
#define ISPOST_LCMRPREO_XCPRE_LENGTH           (16)
#define ISPOST_LCMRPREO_XCPRE_DEFAULT          (0)
#define ISPOST_LCMRPREO_XCPRE_SIGNED_FIELD     (0)

/*
 * ISPOST, LCMRPREO, YCPRE
 * YCPRE – prerotation vertical offset.
 * Vertical offset applied after rotation (1.14.1 format).
 */
#define ISPOST_LCMRPREO_YCPRE_MASK             (0x0000FFFF)
#define ISPOST_LCMRPREO_YCPRE_LSBMASK          (0x0000FFFF)
#define ISPOST_LCMRPREO_YCPRE_SHIFT            (0)
#define ISPOST_LCMRPREO_YCPRE_LENGTH           (16)
#define ISPOST_LCMRPREO_YCPRE_DEFAULT          (0)
#define ISPOST_LCMRPREO_YCPRE_SIGNED_FIELD     (0)


// LC Mirror and Rotation Post Offset Register.
#define ISPOST_LCMRPOSTO_OFFSET                (0x0048)
#define ISPOST_LCMRPOSTO                       (ISPOST_PERI_BASE_ADDR + ISPOST_LCMRPOSTO_OFFSET)

/*
 * ISPOST, LCMRPOSTO, XCPRE
 * XCPRE – postrotation horizontal offset.
 * Horizontal offset applied after rotation (1.14.1 format).
 */
#define ISPOST_LCMRPOSTO_XCPRE_MASK            (0xFFFF0000)
#define ISPOST_LCMRPOSTO_XCPRE_LSBMASK         (0x0000FFFF)
#define ISPOST_LCMRPOSTO_XCPRE_SHIFT           (16)
#define ISPOST_LCMRPOSTO_XCPRE_LENGTH          (16)
#define ISPOST_LCMRPOSTO_XCPRE_DEFAULT         (0)
#define ISPOST_LCMRPOSTO_XCPRE_SIGNED_FIELD    (0)

/*
 * ISPOST, LCMRPOSTO, YCPRE
 * YCPRE – postrotation vertical offset.
 * Vertical offset applied after rotation (1.14.1 format).
 */
#define ISPOST_LCMRPOSTO_YCPRE_MASK            (0x0000FFFF)
#define ISPOST_LCMRPOSTO_YCPRE_LSBMASK         (0x0000FFFF)
#define ISPOST_LCMRPOSTO_YCPRE_SHIFT           (0)
#define ISPOST_LCMRPOSTO_YCPRE_LENGTH          (16)
#define ISPOST_LCMRPOSTO_YCPRE_DEFAULT         (0)
#define ISPOST_LCMRPOSTO_YCPRE_SIGNED_FIELD    (0)


// LC AXI Control Register.
#define ISPOST_LCAXI_OFFSET                    (0x005C)
#define ISPOST_LCAXI                           (ISPOST_PERI_BASE_ADDR + ISPOST_LCAXI_OFFSET)

/*
 * ISPOST, LCAXI, SRCRDL
 * SRCRDL – Source Image AXI Read Request Limit.
 * Total number of QW requested by ISPOST for grid buffer by will be 64 – SRCRDL. Valid range: 56-4
 */
#define ISPOST_LCAXI_SRCRDL_MASK               (0x007F0000)
#define ISPOST_LCAXI_SRCRDL_LSBMASK            (0x0000007F)
#define ISPOST_LCAXI_SRCRDL_SHIFT              (16)
#define ISPOST_LCAXI_SRCRDL_LENGTH             (7)
#define ISPOST_LCAXI_SRCRDL_DEFAULT            (0)
#define ISPOST_LCAXI_SRCRDL_SIGNED_FIELD       (0)

/*
 * ISPOST, LCAXI, GBID
 * GBID – Grid Buffer AXI Read ID.
 * (Please note: ALL AXI Read Ids must be unique).
 */
#define ISPOST_LCAXI_GBID_MASK                 (0x00000300)
#define ISPOST_LCAXI_GBID_LSBMASK              (0x00000003)
#define ISPOST_LCAXI_GBID_SHIFT                (8)
#define ISPOST_LCAXI_GBID_LENGTH               (2)
#define ISPOST_LCAXI_GBID_DEFAULT              (0)
#define ISPOST_LCAXI_GBID_SIGNED_FIELD         (0)

/*
 * ISPOST, LCAXI, SRCID
 * SRCID – Source Image AXI Read ID.
 * (Please note: ALL AXI Read Ids must be unique).
 */
#define ISPOST_LCAXI_SRCID_MASK                (0x00000003)
#define ISPOST_LCAXI_SRCID_LSBMASK             (0x00000003)
#define ISPOST_LCAXI_SRCID_SHIFT               (0)
#define ISPOST_LCAXI_SRCID_LENGTH              (2)
#define ISPOST_LCAXI_SRCID_DEFAULT             (0)
#define ISPOST_LCAXI_SRCID_SIGNED_FIELD        (0)



// Overlay registers offset.
//==========================================================
// OV Mode Register.
#define ISPOST_OVM_OFFSET                      (0x0060)
#define ISPOST_OVM                             (ISPOST_PERI_BASE_ADDR + ISPOST_OVM_OFFSET)

/*
 * ISPOST, OVM, OVM
 * OVM - Overlay mode.
 */
#define ISPOST_OVM_OVM_MASK                    (0x00000003)
#define ISPOST_OVM_OVM_LSBMASK                 (0x00000003)
#define ISPOST_OVM_OVM_SHIFT                   (0)
#define ISPOST_OVM_OVM_LENGTH                  (2)
#define ISPOST_OVM_OVM_DEFAULT                 (0)
#define ISPOST_OVM_OVM_SIGNED_FIELD            (0)


// OV Overlay Image Y Plane Start Address Register.
#define ISPOST_OVRIAY_OFFSET                   (0x0064)
#define ISPOST_OVRIAY                          (ISPOST_PERI_BASE_ADDR + ISPOST_OVRIAY_OFFSET)

/*
 * ISPOST, OVRIAY, SAD
 * Base address in QWORD (64bit) boundary.
 */
#define ISPOST_OVRIAY_SAD_MASK                 (0xFFFFFFF8)
#define ISPOST_OVRIAY_SAD_LSBMASK              (0x1FFFFFFF)
#define ISPOST_OVRIAY_SAD_SHIFT                (3)
#define ISPOST_OVRIAY_SAD_LENGTH               (29)
#define ISPOST_OVRIAY_SAD_DEFAULT              (0)
#define ISPOST_OVRIAY_SAD_SIGNED_FIELD         (0)


// OV Overlay Image UV Plane Start Address Register Read/Write at Offset.
#define ISPOST_OVRIAUV_OFFSET                  (0x0068)
#define ISPOST_OVRIAUV                         (ISPOST_PERI_BASE_ADDR + ISPOST_OVRIAUV_OFFSET)

/*
 * ISPOST, OVRIAUV, SAD
 * Base address in QWORD (64bit) boundary.
 */
#define ISPOST_OVRIAUV_SAD_MASK                (0xFFFFFFF8)
#define ISPOST_OVRIAUV_SAD_LSBMASK             (0x1FFFFFFF)
#define ISPOST_OVRIAUV_SAD_SHIFT               (3)
#define ISPOST_OVRIAUV_SAD_LENGTH              (29)
#define ISPOST_OVRIAUV_SAD_DEFAULT             (0)
#define ISPOST_OVRIAUV_SAD_SIGNED_FIELD        (0)


// OV Overlay Image Stride Register.
#define ISPOST_OVRIS_OFFSET                    (0x006C)
#define ISPOST_OVRIS                           (ISPOST_PERI_BASE_ADDR + ISPOST_OVRIS_OFFSET)

/*
 * ISPOST, OVRIS, SD
 * Image stride (QWORD granularity).
 */
#define ISPOST_OVRIS_SD_MASK                   (0x00001FF8)
#define ISPOST_OVRIS_SD_LSBMASK                (0x000003FF)
#define ISPOST_OVRIS_SD_SHIFT                  (3)
#define ISPOST_OVRIS_SD_LENGTH                 (10)
#define ISPOST_OVRIS_SD_DEFAULT                (0)
#define ISPOST_OVRIS_SD_SIGNED_FIELD           (0)


// OV Overlay Image Size Register.
#define ISPOST_OVISZ_OFFSET                    (0x0070)
#define ISPOST_OVISZ                           (ISPOST_PERI_BASE_ADDR + ISPOST_OVISZ_OFFSET)

/*
 * ISPOST, OVISZ, W
 * Overlay image width (pixel granularity).
 */
#define ISPOST_OVISZ_W_MASK                    (0x1FFF0000)
#define ISPOST_OVISZ_W_LSBMASK                 (0x00001FFF)
#define ISPOST_OVISZ_W_SHIFT                   (16)
#define ISPOST_OVISZ_W_LENGTH                  (13)
#define ISPOST_OVISZ_W_DEFAULT                 (0)
#define ISPOST_OVISZ_W_SIGNED_FIELD            (0)

/*
 * ISPOST, OVISZ, H
 * Overlay image height (line granularity).
 */
#define ISPOST_OVISZ_H_MASK                    (0x00001FFF)
#define ISPOST_OVISZ_H_LSBMASK                 (0x00001FFF)
#define ISPOST_OVISZ_H_SHIFT                   (0)
#define ISPOST_OVISZ_H_LENGTH                  (13)
#define ISPOST_OVISZ_H_DEFAULT                 (0)
#define ISPOST_OVISZ_H_SIGNED_FIELD            (0)


// OV Overlay Offset Register.
#define ISPOST_OVOFF_OFFSET                    (0x0074)
#define ISPOST_OVOFF                           (ISPOST_PERI_BASE_ADDR + ISPOST_OVOFF_OFFSET)

/*
 * ISPOST, OVOFF, X
 * Overlay image horizontal offset (in relation to destination image).
 */
#define ISPOST_OVOFF_X_MASK                    (0x1FFF0000)
#define ISPOST_OVOFF_X_LSBMASK                 (0x00001FFF)
#define ISPOST_OVOFF_X_SHIFT                   (16)
#define ISPOST_OVOFF_X_LENGTH                  (13)
#define ISPOST_OVOFF_X_DEFAULT                 (0)
#define ISPOST_OVOFF_X_SIGNED_FIELD            (0)

/*
 * ISPOST, OVOFF, Y
 * Overlay image vertical offset (in relation to destination image).
 */
#define ISPOST_OVOFF_Y_MASK                    (0x00001FFF)
#define ISPOST_OVOFF_Y_LSBMASK                 (0x00001FFF)
#define ISPOST_OVOFF_Y_SHIFT                   (0)
#define ISPOST_OVOFF_Y_LENGTH                  (13)
#define ISPOST_OVOFF_Y_DEFAULT                 (0)
#define ISPOST_OVOFF_Y_SIGNED_FIELD            (0)


// OV Overlay Alpha Value 0 Register.
#define ISPOST_OVALPHA0_OFFSET                 (0x0078)
#define ISPOST_OVALPHA0                        (ISPOST_PERI_BASE_ADDR + ISPOST_OVALPHA0_OFFSET)

/*
 * ISPOST, OVALPHA0, ALPHA3
 * Overlay image alpha value register.
 */
#define ISPOST_OVALPHA0_ALPHA3_MASK            (0xFF000000)
#define ISPOST_OVALPHA0_ALPHA3_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA0_ALPHA3_SHIFT           (24)
#define ISPOST_OVALPHA0_ALPHA3_LENGTH          (8)
#define ISPOST_OVALPHA0_ALPHA3_DEFAULT         (0)
#define ISPOST_OVALPHA0_ALPHA3_SIGNED_FIELD    (0)

/*
 * ISPOST, OVALPHA0, ALPHA2
 * Overlay image alpha value register.
 */
#define ISPOST_OVALPHA0_ALPHA2_MASK            (0x00FF0000)
#define ISPOST_OVALPHA0_ALPHA2_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA0_ALPHA2_SHIFT           (16)
#define ISPOST_OVALPHA0_ALPHA2_LENGTH          (8)
#define ISPOST_OVALPHA0_ALPHA2_DEFAULT         (0)
#define ISPOST_OVALPHA0_ALPHA2_SIGNED_FIELD    (0)

/*
 * ISPOST, OVALPHA0, ALPHA1
 * Overlay image alpha value register.
 */
#define ISPOST_OVALPHA0_ALPHA1_MASK            (0x0000FF00)
#define ISPOST_OVALPHA0_ALPHA1_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA0_ALPHA1_SHIFT           (8)
#define ISPOST_OVALPHA0_ALPHA1_LENGTH          (8)
#define ISPOST_OVALPHA0_ALPHA1_DEFAULT         (0)
#define ISPOST_OVALPHA0_ALPHA1_SIGNED_FIELD    (0)

/*
 * ISPOST, OVALPHA0, ALPHA0
 * Overlay image alpha value register.
 */
#define ISPOST_OVALPHA0_ALPHA0_MASK            (0x000000FF)
#define ISPOST_OVALPHA0_ALPHA0_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA0_ALPHA0_SHIFT           (0)
#define ISPOST_OVALPHA0_ALPHA0_LENGTH          (8)
#define ISPOST_OVALPHA0_ALPHA0_DEFAULT         (0)
#define ISPOST_OVALPHA0_ALPHA0_SIGNED_FIELD    (0)


// OV Overlay Alpha Value 1 Register.
#define ISPOST_OVALPHA1_OFFSET                 (0x007C)
#define ISPOST_OVALPHA1                        (ISPOST_PERI_BASE_ADDR + ISPOST_OVALPHA1_OFFSET)

/*
 * ISPOST, OVALPHA1, ALPHA7
 * Overlay image alpha value register.
 */
#define ISPOST_OVALPHA1_ALPHA7_MASK            (0xFF000000)
#define ISPOST_OVALPHA1_ALPHA7_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA1_ALPHA7_SHIFT           (24)
#define ISPOST_OVALPHA1_ALPHA7_LENGTH          (8)
#define ISPOST_OVALPHA1_ALPHA7_DEFAULT         (0)
#define ISPOST_OVALPHA1_ALPHA7_SIGNED_FIELD    (0)

/*
 * ISPOST, OVALPHA1, ALPHA6
 * Overlay image alpha value register.
 */
#define ISPOST_OVALPHA1_ALPHA6_MASK            (0x00FF0000)
#define ISPOST_OVALPHA1_ALPHA6_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA1_ALPHA6_SHIFT           (16)
#define ISPOST_OVALPHA1_ALPHA6_LENGTH          (8)
#define ISPOST_OVALPHA1_ALPHA6_DEFAULT         (0)
#define ISPOST_OVALPHA1_ALPHA6_SIGNED_FIELD    (0)

/*
 * ISPOST, OVALPHA1, ALPHA5
 * Overlay image alpha value register.
 */
#define ISPOST_OVALPHA1_ALPHA5_MASK            (0x0000FF00)
#define ISPOST_OVALPHA1_ALPHA5_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA1_ALPHA5_SHIFT           (8)
#define ISPOST_OVALPHA1_ALPHA5_LENGTH          (8)
#define ISPOST_OVALPHA1_ALPHA5_DEFAULT         (0)
#define ISPOST_OVALPHA1_ALPHA5_SIGNED_FIELD    (0)

/*
 * ISPOST, OVALPHA1, ALPHA4
 * Overlay image alpha value register.
 */
#define ISPOST_OVALPHA1_ALPHA4_MASK            (0x000000FF)
#define ISPOST_OVALPHA1_ALPHA4_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA1_ALPHA4_SHIFT           (0)
#define ISPOST_OVALPHA1_ALPHA4_LENGTH          (8)
#define ISPOST_OVALPHA1_ALPHA4_DEFAULT         (0)
#define ISPOST_OVALPHA1_ALPHA4_SIGNED_FIELD    (0)


// OV Overlay Alpha Value 2 Register.
#define ISPOST_OVALPHA2_OFFSET                 (0x0080)
#define ISPOST_OVALPHA2                        (ISPOST_PERI_BASE_ADDR + ISPOST_OVALPHA2_OFFSET)

/*
 * ISPOST, OVALPHA2, ALPHA11
 * Overlay image alpha value register.
 */
#define ISPOST_OVALPHA2_ALPHA11_MASK           (0xFF000000)
#define ISPOST_OVALPHA2_ALPHA11_LSBMASK        (0x000000FF)
#define ISPOST_OVALPHA2_ALPHA11_SHIFT          (24)
#define ISPOST_OVALPHA2_ALPHA11_LENGTH         (8)
#define ISPOST_OVALPHA2_ALPHA11_DEFAULT        (0)
#define ISPOST_OVALPHA2_ALPHA11_SIGNED_FIELD   (0)

/*
 * ISPOST, OVALPHA2, ALPHA10
 * Overlay image alpha value register.
 */
#define ISPOST_OVALPHA2_ALPHA10_MASK           (0x00FF0000)
#define ISPOST_OVALPHA2_ALPHA10_LSBMASK        (0x000000FF)
#define ISPOST_OVALPHA2_ALPHA10_SHIFT          (16)
#define ISPOST_OVALPHA2_ALPHA10_LENGTH         (8)
#define ISPOST_OVALPHA2_ALPHA10_DEFAULT        (0)
#define ISPOST_OVALPHA2_ALPHA10_SIGNED_FIELD   (0)

/*
 * ISPOST, OVALPHA2, ALPHA9
 * Overlay image alpha value register.
 */
#define ISPOST_OVALPHA2_ALPHA9_MASK            (0x0000FF00)
#define ISPOST_OVALPHA2_ALPHA9_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA2_ALPHA9_SHIFT           (8)
#define ISPOST_OVALPHA2_ALPHA9_LENGTH          (8)
#define ISPOST_OVALPHA2_ALPHA9_DEFAULT         (0)
#define ISPOST_OVALPHA2_ALPHA9_SIGNED_FIELD    (0)

/*
 * ISPOST, OVALPHA2, ALPHA8
 * Overlay image alpha value register.
 */
#define ISPOST_OVALPHA2_ALPHA8_MASK            (0x000000FF)
#define ISPOST_OVALPHA2_ALPHA8_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA2_ALPHA8_SHIFT           (0)
#define ISPOST_OVALPHA2_ALPHA8_LENGTH          (8)
#define ISPOST_OVALPHA2_ALPHA8_DEFAULT         (0)
#define ISPOST_OVALPHA2_ALPHA8_SIGNED_FIELD    (0)


// OV Overlay Alpha Value 3 Register.
#define ISPOST_OVALPHA3_OFFSET                 (0x0084)
#define ISPOST_OVALPHA3                        (ISPOST_PERI_BASE_ADDR + ISPOST_OVALPHA3_OFFSET)

/*
 * ISPOST, OVALPHA3, ALPHA15
 * Overlay image alpha value register.
 */
#define ISPOST_OVALPHA3_ALPHA15_MASK           (0xFF000000)
#define ISPOST_OVALPHA3_ALPHA15_LSBMASK        (0x000000FF)
#define ISPOST_OVALPHA3_ALPHA15_SHIFT          (24)
#define ISPOST_OVALPHA3_ALPHA15_LENGTH         (8)
#define ISPOST_OVALPHA3_ALPHA15_DEFAULT        (0)
#define ISPOST_OVALPHA3_ALPHA15_SIGNED_FIELD   (0)

/*
 * ISPOST, OVALPHA3, ALPHA14
 * Overlay image alpha value register.
 */
#define ISPOST_OVALPHA3_ALPHA14_MASK           (0x00FF0000)
#define ISPOST_OVALPHA3_ALPHA14_LSBMASK        (0x000000FF)
#define ISPOST_OVALPHA3_ALPHA14_SHIFT          (16)
#define ISPOST_OVALPHA3_ALPHA14_LENGTH         (8)
#define ISPOST_OVALPHA3_ALPHA14_DEFAULT        (0)
#define ISPOST_OVALPHA3_ALPHA14_SIGNED_FIELD   (0)

/*
 * ISPOST, OVALPHA3, ALPHA13
 * Overlay image alpha value register.
 */
#define ISPOST_OVALPHA3_ALPHA13_MASK           (0x0000FF00)
#define ISPOST_OVALPHA3_ALPHA13_LSBMASK        (0x000000FF)
#define ISPOST_OVALPHA3_ALPHA13_SHIFT          (8)
#define ISPOST_OVALPHA3_ALPHA13_LENGTH         (8)
#define ISPOST_OVALPHA3_ALPHA13_DEFAULT        (0)
#define ISPOST_OVALPHA3_ALPHA13_SIGNED_FIELD   (0)

/*
 * ISPOST, OVALPHA3, ALPHA12
 * Overlay image alpha value register.
 */
#define ISPOST_OVALPHA3_ALPHA12_MASK           (0x000000FF)
#define ISPOST_OVALPHA3_ALPHA12_LSBMASK        (0x000000FF)
#define ISPOST_OVALPHA3_ALPHA12_SHIFT          (0)
#define ISPOST_OVALPHA3_ALPHA12_LENGTH         (8)
#define ISPOST_OVALPHA3_ALPHA12_DEFAULT        (0)
#define ISPOST_OVALPHA3_ALPHA12_SIGNED_FIELD   (0)


// OV AXI Control Register.
#define ISPOST_OVAXI_OFFSET                    (0x0088)
#define ISPOST_OVAXI                           (ISPOST_PERI_BASE_ADDR + ISPOST_OVAXI_OFFSET)

/*
 * ISPOST, OVAXI, OVID
 * OV Image AXI Read ID.
 * (Please note: ALL AXI Read Ids must be unique).
 */
#define ISPOST_OVAXI_OVID_MASK                 (0x00000003)
#define ISPOST_OVAXI_OVID_LSBMASK              (0x00000003)
#define ISPOST_OVAXI_OVID_SHIFT                (0)
#define ISPOST_OVAXI_OVID_LENGTH               (2)
#define ISPOST_OVAXI_OVID_DEFAULT              (0)
#define ISPOST_OVAXI_OVID_SIGNED_FIELD         (0)



// 3D Denoise registers offset.
//==========================================================
// DN Reference Input Image Y Plane Start Address Register.
#define ISPOST_DNRIAY_OFFSET                   (0x00A0)
#define ISPOST_DNRIAY                          (ISPOST_PERI_BASE_ADDR + ISPOST_DNRIAY_OFFSET)

/*
 * ISPOST, DNRIAY, SAD
 * Base address in QWORD (64bit) boundary.
 */
#define ISPOST_DNRIAY_SAD_MASK                 (0xFFFFFFF8)
#define ISPOST_DNRIAY_SAD_LSBMASK              (0x1FFFFFFF)
#define ISPOST_DNRIAY_SAD_SHIFT                (3)
#define ISPOST_DNRIAY_SAD_LENGTH               (29)
#define ISPOST_DNRIAY_SAD_DEFAULT              (0)
#define ISPOST_DNRIAY_SAD_SIGNED_FIELD         (0)


// DN Reference Input Image UV Plane Start Address Register.
#define ISPOST_DNRIAUV_OFFSET                  (0x00A4)
#define ISPOST_DNRIAUV                         (ISPOST_PERI_BASE_ADDR + ISPOST_DNRIAUV_OFFSET)

/*
 * ISPOST, DNRIAUV, SAD
 * Base address in QWORD (64bit) boundary.
 */
#define ISPOST_DNRIAUV_SAD_MASK                (0xFFFFFFF8)
#define ISPOST_DNRIAUV_SAD_LSBMASK             (0x1FFFFFFF)
#define ISPOST_DNRIAUV_SAD_SHIFT               (3)
#define ISPOST_DNRIAUV_SAD_LENGTH              (29)
#define ISPOST_DNRIAUV_SAD_DEFAULT             (0)
#define ISPOST_DNRIAUV_SAD_SIGNED_FIELD        (0)


// DN Reference Input Image Stride Register.
#define ISPOST_DNRIS_OFFSET                    (0x00A8)
#define ISPOST_DNRIS                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNRIS_OFFSET)

/*
 * ISPOST, DNRIS, SD
 * Image stride (QWORD granularity).
 */
#define ISPOST_DNRIS_SD_MASK                   (0x00001FF8)
#define ISPOST_DNRIS_SD_LSBMASK                (0x000003FF)
#define ISPOST_DNRIS_SD_SHIFT                  (3)
#define ISPOST_DNRIS_SD_LENGTH                 (10)
#define ISPOST_DNRIS_SD_DEFAULT                (0)
#define ISPOST_DNRIS_SD_SIGNED_FIELD           (0)


// DN Reference Output Image Y Plane Start Address Register.
#define ISPOST_DNROAY_OFFSET                   (0x00AC)
#define ISPOST_DNROAY                          (ISPOST_PERI_BASE_ADDR + ISPOST_DNROAY_OFFSET)

/*
 * ISPOST, DNROAY, SAD
 * Base address in QWORD (64bit) boundary.
 */
#define ISPOST_DNROAY_SAD_MASK                 (0xFFFFFFF8)
#define ISPOST_DNROAY_SAD_LSBMASK              (0x1FFFFFFF)
#define ISPOST_DNROAY_SAD_SHIFT                (3)
#define ISPOST_DNROAY_SAD_LENGTH               (29)
#define ISPOST_DNROAY_SAD_DEFAULT              (0)
#define ISPOST_DNROAY_SAD_SIGNED_FIELD         (0)


// DN Reference Output Image UV Plane Start Address Register.
#define ISPOST_DNROAUV_OFFSET                  (0x00B0)
#define ISPOST_DNROAUV                         (ISPOST_PERI_BASE_ADDR + ISPOST_DNROAUV_OFFSET)

/*
 * ISPOST, DNROAUV, SAD
 * Base address in QWORD (64bit) boundary.
 */
#define ISPOST_DNROAUV_SAD_MASK                (0xFFFFFFF8)
#define ISPOST_DNROAUV_SAD_LSBMASK             (0x1FFFFFFF)
#define ISPOST_DNROAUV_SAD_SHIFT               (3)
#define ISPOST_DNROAUV_SAD_LENGTH              (29)
#define ISPOST_DNROAUV_SAD_DEFAULT             (0)
#define ISPOST_DNROAUV_SAD_SIGNED_FIELD        (0)


// DN Reference Output Image Stride Register.
#define ISPOST_DNROS_OFFSET                    (0x00B4)
#define ISPOST_DNROS                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNROS_OFFSET)

/*
 * ISPOST, DNROS, SD
 * Image stride (QWORD granularity).
 */
#define ISPOST_DNROS_SD_MASK                   (0x00001FF8)
#define ISPOST_DNROS_SD_LSBMASK                (0x000003FF)
#define ISPOST_DNROS_SD_SHIFT                  (3)
#define ISPOST_DNROS_SD_LENGTH                 (10)
#define ISPOST_DNROS_SD_DEFAULT                (0)
#define ISPOST_DNROS_SD_SIGNED_FIELD           (0)


// DN Mask Curve 0 Register.
#define ISPOST_DNMC0_OFFSET                    (0x00B8)
#define ISPOST_DNMC0                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC0_OFFSET)

/*
 * ISPOST, DNMC0, S
 * Slope of mask value for mask curve. (format 0.0.12).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC0_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC0_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC0_S_SHIFT                   (20)
#define ISPOST_DNMC0_S_LENGTH                  (12)
#define ISPOST_DNMC0_S_DEFAULT                 (0)
#define ISPOST_DNMC0_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC0, R
 * Absolute difference value range exponent: RANGE = 2^R.
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC0_R_MASK                    (0x00070000)
#define ISPOST_DNMC0_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC0_R_SHIFT                   (16)
#define ISPOST_DNMC0_R_LENGTH                  (3)
#define ISPOST_DNMC0_R_DEFAULT                 (0)
#define ISPOST_DNMC0_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC0, Y
 * Mask value for mask curve. (format 0.0.8).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC0_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC0_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC0_Y_SHIFT                   (8)
#define ISPOST_DNMC0_Y_LENGTH                  (8)
#define ISPOST_DNMC0_Y_DEFAULT                 (0)
#define ISPOST_DNMC0_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC0, X
 * Absolute difference value for mask curve. (format 0.8.0).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 * -------------------------------------------------------------------------
 * NOTE: the difference of next point X and this point X values must match
 * this point range value R (RANGE(N) = 2^R(N) = X(N+1) - X(N))
 * -------------------------------------------------------------------------
 */
#define ISPOST_DNMC0_X_MASK                    (0x000000FF)
#define ISPOST_DNMC0_X_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC0_X_SHIFT                   (0)
#define ISPOST_DNMC0_X_LENGTH                  (8)
#define ISPOST_DNMC0_X_DEFAULT                 (0)
#define ISPOST_DNMC0_X_SIGNED_FIELD            (0)


// DN Mask Curve 1 Register.
#define ISPOST_DNMC1_OFFSET                    (0x00BC)
#define ISPOST_DNMC1                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC1_OFFSET)

/*
 * ISPOST, DNMC1, S
 * Slope of mask value for mask curve. (format 0.0.12).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC1_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC1_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC1_S_SHIFT                   (20)
#define ISPOST_DNMC1_S_LENGTH                  (12)
#define ISPOST_DNMC1_S_DEFAULT                 (0)
#define ISPOST_DNMC1_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC1, R
 * Absolute difference value range exponent: RANGE = 2^R.
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC1_R_MASK                    (0x00070000)
#define ISPOST_DNMC1_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC1_R_SHIFT                   (16)
#define ISPOST_DNMC1_R_LENGTH                  (3)
#define ISPOST_DNMC1_R_DEFAULT                 (0)
#define ISPOST_DNMC1_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC1, Y
 * Mask value for mask curve. (format 0.0.8).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC1_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC1_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC1_Y_SHIFT                   (8)
#define ISPOST_DNMC1_Y_LENGTH                  (8)
#define ISPOST_DNMC1_Y_DEFAULT                 (0)
#define ISPOST_DNMC1_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC1, X
 * Absolute difference value for mask curve. (format 0.8.0).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 * -------------------------------------------------------------------------
 * NOTE: the difference of next point X and this point X values must match
 * this point range value R (RANGE(N) = 2^R(N) = X(N+1) - X(N))
 * -------------------------------------------------------------------------
 */
#define ISPOST_DNMC1_X_MASK                    (0x000000FF)
#define ISPOST_DNMC1_X_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC1_X_SHIFT                   (0)
#define ISPOST_DNMC1_X_LENGTH                  (8)
#define ISPOST_DNMC1_X_DEFAULT                 (0)
#define ISPOST_DNMC1_X_SIGNED_FIELD            (0)


// DN Mask Curve 2 Register.
#define ISPOST_DNMC2_OFFSET                    (0x00C0)
#define ISPOST_DNMC2                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC2_OFFSET)

/*
 * ISPOST, DNMC2, S
 * Slope of mask value for mask curve. (format 0.0.12).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC2_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC2_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC2_S_SHIFT                   (20)
#define ISPOST_DNMC2_S_LENGTH                  (12)
#define ISPOST_DNMC2_S_DEFAULT                 (0)
#define ISPOST_DNMC2_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC2, R
 * Absolute difference value range exponent: RANGE = 2^R.
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC2_R_MASK                    (0x00070000)
#define ISPOST_DNMC2_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC2_R_SHIFT                   (16)
#define ISPOST_DNMC2_R_LENGTH                  (3)
#define ISPOST_DNMC2_R_DEFAULT                 (0)
#define ISPOST_DNMC2_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC2, Y
 * Mask value for mask curve. (format 0.0.8).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC2_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC2_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC2_Y_SHIFT                   (8)
#define ISPOST_DNMC2_Y_LENGTH                  (8)
#define ISPOST_DNMC2_Y_DEFAULT                 (0)
#define ISPOST_DNMC2_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC2, X
 * Absolute difference value for mask curve. (format 0.8.0).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 * -------------------------------------------------------------------------
 * NOTE: the difference of next point X and this point X values must match
 * this point range value R (RANGE(N) = 2^R(N) = X(N+1) - X(N))
 * -------------------------------------------------------------------------
 */
#define ISPOST_DNMC2_X_MASK                    (0x000000FF)
#define ISPOST_DNMC2_X_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC2_X_SHIFT                   (0)
#define ISPOST_DNMC2_X_LENGTH                  (8)
#define ISPOST_DNMC2_X_DEFAULT                 (0)
#define ISPOST_DNMC2_X_SIGNED_FIELD            (0)


// DN Mask Curve 3 Register.
#define ISPOST_DNMC3_OFFSET                    (0x00C4)
#define ISPOST_DNMC3                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC3_OFFSET)

/*
 * ISPOST, DNMC3, S
 * Slope of mask value for mask curve. (format 0.0.12).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC3_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC3_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC3_S_SHIFT                   (20)
#define ISPOST_DNMC3_S_LENGTH                  (12)
#define ISPOST_DNMC3_S_DEFAULT                 (0)
#define ISPOST_DNMC3_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC3, R
 * Absolute difference value range exponent: RANGE = 2^R.
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC3_R_MASK                    (0x00070000)
#define ISPOST_DNMC3_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC3_R_SHIFT                   (16)
#define ISPOST_DNMC3_R_LENGTH                  (3)
#define ISPOST_DNMC3_R_DEFAULT                 (0)
#define ISPOST_DNMC3_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC3, Y
 * Mask value for mask curve. (format 0.0.8).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC3_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC3_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC3_Y_SHIFT                   (8)
#define ISPOST_DNMC3_Y_LENGTH                  (8)
#define ISPOST_DNMC3_Y_DEFAULT                 (0)
#define ISPOST_DNMC3_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC3, X
 * Absolute difference value for mask curve. (format 0.8.0).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 * -------------------------------------------------------------------------
 * NOTE: the difference of next point X and this point X values must match
 * this point range value R (RANGE(N) = 2^R(N) = X(N+1) - X(N))
 * -------------------------------------------------------------------------
 */
#define ISPOST_DNMC3_X_MASK                    (0x000000FF)
#define ISPOST_DNMC3_X_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC3_X_SHIFT                   (0)
#define ISPOST_DNMC3_X_LENGTH                  (8)
#define ISPOST_DNMC3_X_DEFAULT                 (0)
#define ISPOST_DNMC3_X_SIGNED_FIELD            (0)


// DN Mask Curve 4 Register.
#define ISPOST_DNMC4_OFFSET                    (0x00C8)
#define ISPOST_DNMC4                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC4_OFFSET)

/*
 * ISPOST, DNMC4, S
 * Slope of mask value for mask curve. (format 0.0.12).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC4_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC4_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC4_S_SHIFT                   (20)
#define ISPOST_DNMC4_S_LENGTH                  (12)
#define ISPOST_DNMC4_S_DEFAULT                 (0)
#define ISPOST_DNMC4_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC4, R
 * Absolute difference value range exponent: RANGE = 2^R.
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC4_R_MASK                    (0x00070000)
#define ISPOST_DNMC4_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC4_R_SHIFT                   (16)
#define ISPOST_DNMC4_R_LENGTH                  (3)
#define ISPOST_DNMC4_R_DEFAULT                 (0)
#define ISPOST_DNMC4_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC4, Y
 * Mask value for mask curve. (format 0.0.8).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC4_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC4_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC4_Y_SHIFT                   (8)
#define ISPOST_DNMC4_Y_LENGTH                  (8)
#define ISPOST_DNMC4_Y_DEFAULT                 (0)
#define ISPOST_DNMC4_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC4, X
 * Absolute difference value for mask curve. (format 0.8.0).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 * -------------------------------------------------------------------------
 * NOTE: the difference of next point X and this point X values must match
 * this point range value R (RANGE(N) = 2^R(N) = X(N+1) - X(N))
 * -------------------------------------------------------------------------
 */
#define ISPOST_DNMC4_X_MASK                    (0x000000FF)
#define ISPOST_DNMC4_X_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC4_X_SHIFT                   (0)
#define ISPOST_DNMC4_X_LENGTH                  (8)
#define ISPOST_DNMC4_X_DEFAULT                 (0)
#define ISPOST_DNMC4_X_SIGNED_FIELD            (0)


// DN Mask Curve 5 Register.
#define ISPOST_DNMC5_OFFSET                    (0x00CC)
#define ISPOST_DNMC5                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC5_OFFSET)

/*
 * ISPOST, DNMC5, S
 * Slope of mask value for mask curve. (format 0.0.12).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC5_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC5_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC5_S_SHIFT                   (20)
#define ISPOST_DNMC5_S_LENGTH                  (12)
#define ISPOST_DNMC5_S_DEFAULT                 (0)
#define ISPOST_DNMC5_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC5, R
 * Absolute difference value range exponent: RANGE = 2^R.
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC5_R_MASK                    (0x00070000)
#define ISPOST_DNMC5_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC5_R_SHIFT                   (16)
#define ISPOST_DNMC5_R_LENGTH                  (3)
#define ISPOST_DNMC5_R_DEFAULT                 (0)
#define ISPOST_DNMC5_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC5, Y
 * Mask value for mask curve. (format 0.0.8).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC5_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC5_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC5_Y_SHIFT                   (8)
#define ISPOST_DNMC5_Y_LENGTH                  (8)
#define ISPOST_DNMC5_Y_DEFAULT                 (0)
#define ISPOST_DNMC5_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC5, X
 * Absolute difference value for mask curve. (format 0.8.0).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 * -------------------------------------------------------------------------
 * NOTE: the difference of next point X and this point X values must match
 * this point range value R (RANGE(N) = 2^R(N) = X(N+1) - X(N))
 * -------------------------------------------------------------------------
 */
#define ISPOST_DNMC5_X_MASK                    (0x000000FF)
#define ISPOST_DNMC5_X_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC5_X_SHIFT                   (0)
#define ISPOST_DNMC5_X_LENGTH                  (8)
#define ISPOST_DNMC5_X_DEFAULT                 (0)
#define ISPOST_DNMC5_X_SIGNED_FIELD            (0)


// DN Mask Curve 6 Register.
#define ISPOST_DNMC6_OFFSET                    (0x00D0)
#define ISPOST_DNMC6                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC6_OFFSET)

/*
 * ISPOST, DNMC6, S
 * Slope of mask value for mask curve. (format 0.0.12).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC6_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC6_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC6_S_SHIFT                   (20)
#define ISPOST_DNMC6_S_LENGTH                  (12)
#define ISPOST_DNMC6_S_DEFAULT                 (0)
#define ISPOST_DNMC6_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC6, R
 * Absolute difference value range exponent: RANGE = 2^R.
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC6_R_MASK                    (0x00070000)
#define ISPOST_DNMC6_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC6_R_SHIFT                   (16)
#define ISPOST_DNMC6_R_LENGTH                  (3)
#define ISPOST_DNMC6_R_DEFAULT                 (0)
#define ISPOST_DNMC6_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC6, Y
 * Mask value for mask curve. (format 0.0.8).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC6_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC6_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC6_Y_SHIFT                   (8)
#define ISPOST_DNMC6_Y_LENGTH                  (8)
#define ISPOST_DNMC6_Y_DEFAULT                 (0)
#define ISPOST_DNMC6_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC6, X
 * Absolute difference value for mask curve. (format 0.8.0).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 * -------------------------------------------------------------------------
 * NOTE: the difference of next point X and this point X values must match
 * this point range value R (RANGE(N) = 2^R(N) = X(N+1) - X(N))
 * -------------------------------------------------------------------------
 */
#define ISPOST_DNMC6_X_MASK                    (0x000000FF)
#define ISPOST_DNMC6_X_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC6_X_SHIFT                   (0)
#define ISPOST_DNMC6_X_LENGTH                  (8)
#define ISPOST_DNMC6_X_DEFAULT                 (0)
#define ISPOST_DNMC6_X_SIGNED_FIELD            (0)


// DN Mask Curve 7 Register.
#define ISPOST_DNMC7_OFFSET                    (0x00D4)
#define ISPOST_DNMC7                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC7_OFFSET)

/*
 * ISPOST, DNMC7, S
 * Slope of mask value for mask curve. (format 0.0.12).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC7_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC7_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC7_S_SHIFT                   (20)
#define ISPOST_DNMC7_S_LENGTH                  (12)
#define ISPOST_DNMC7_S_DEFAULT                 (0)
#define ISPOST_DNMC7_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC7, R
 * Absolute difference value range exponent: RANGE = 2^R.
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC7_R_MASK                    (0x00070000)
#define ISPOST_DNMC7_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC7_R_SHIFT                   (16)
#define ISPOST_DNMC7_R_LENGTH                  (3)
#define ISPOST_DNMC7_R_DEFAULT                 (0)
#define ISPOST_DNMC7_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC7, Y
 * Mask value for mask curve. (format 0.0.8).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC7_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC7_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC7_Y_SHIFT                   (8)
#define ISPOST_DNMC7_Y_LENGTH                  (8)
#define ISPOST_DNMC7_Y_DEFAULT                 (0)
#define ISPOST_DNMC7_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC7, X
 * Absolute difference value for mask curve. (format 0.8.0).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 * -------------------------------------------------------------------------
 * NOTE: the difference of next point X and this point X values must match
 * this point range value R (RANGE(N) = 2^R(N) = X(N+1) - X(N))
 * -------------------------------------------------------------------------
 */
#define ISPOST_DNMC7_X_MASK                    (0x000000FF)
#define ISPOST_DNMC7_X_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC7_X_SHIFT                   (0)
#define ISPOST_DNMC7_X_LENGTH                  (8)
#define ISPOST_DNMC7_X_DEFAULT                 (0)
#define ISPOST_DNMC7_X_SIGNED_FIELD            (0)


// DN Mask Curve 8 Register.
#define ISPOST_DNMC8_OFFSET                    (0x00D8)
#define ISPOST_DNMC8                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC8_OFFSET)

/*
 * ISPOST, DNMC8, S
 * Slope of mask value for mask curve. (format 0.0.12).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC8_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC8_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC8_S_SHIFT                   (20)
#define ISPOST_DNMC8_S_LENGTH                  (12)
#define ISPOST_DNMC8_S_DEFAULT                 (0)
#define ISPOST_DNMC8_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC8, R
 * Absolute difference value range exponent: RANGE = 2^R.
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC8_R_MASK                    (0x00070000)
#define ISPOST_DNMC8_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC8_R_SHIFT                   (16)
#define ISPOST_DNMC8_R_LENGTH                  (3)
#define ISPOST_DNMC8_R_DEFAULT                 (0)
#define ISPOST_DNMC8_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC8, Y
 * Mask value for mask curve. (format 0.0.8).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC8_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC8_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC8_Y_SHIFT                   (8)
#define ISPOST_DNMC8_Y_LENGTH                  (8)
#define ISPOST_DNMC8_Y_DEFAULT                 (0)
#define ISPOST_DNMC8_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC8, X
 * Absolute difference value for mask curve. (format 0.8.0).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 * -------------------------------------------------------------------------
 * NOTE: the difference of next point X and this point X values must match
 * this point range value R (RANGE(N) = 2^R(N) = X(N+1) - X(N))
 * -------------------------------------------------------------------------
 */
#define ISPOST_DNMC8_X_MASK                    (0x000000FF)
#define ISPOST_DNMC8_X_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC8_X_SHIFT                   (0)
#define ISPOST_DNMC8_X_LENGTH                  (8)
#define ISPOST_DNMC8_X_DEFAULT                 (0)
#define ISPOST_DNMC8_X_SIGNED_FIELD            (0)


// DN Mask Curve 9 Register.
#define ISPOST_DNMC9_OFFSET                    (0x00DC)
#define ISPOST_DNMC9                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC9_OFFSET)

/*
 * ISPOST, DNMC9, S
 * Slope of mask value for mask curve. (format 0.0.12).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC9_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC9_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC9_S_SHIFT                   (20)
#define ISPOST_DNMC9_S_LENGTH                  (12)
#define ISPOST_DNMC9_S_DEFAULT                 (0)
#define ISPOST_DNMC9_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC9, R
 * Absolute difference value range exponent: RANGE = 2^R.
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC9_R_MASK                    (0x00070000)
#define ISPOST_DNMC9_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC9_R_SHIFT                   (16)
#define ISPOST_DNMC9_R_LENGTH                  (3)
#define ISPOST_DNMC9_R_DEFAULT                 (0)
#define ISPOST_DNMC9_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC9, Y
 * Mask value for mask curve. (format 0.0.8).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC9_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC9_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC9_Y_SHIFT                   (8)
#define ISPOST_DNMC9_Y_LENGTH                  (8)
#define ISPOST_DNMC9_Y_DEFAULT                 (0)
#define ISPOST_DNMC9_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC9, X
 * Absolute difference value for mask curve. (format 0.8.0).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 * -------------------------------------------------------------------------
 * NOTE: the difference of next point X and this point X values must match
 * this point range value R (RANGE(N) = 2^R(N) = X(N+1) - X(N))
 * -------------------------------------------------------------------------
 */
#define ISPOST_DNMC9_X_MASK                    (0x000000FF)
#define ISPOST_DNMC9_X_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC9_X_SHIFT                   (0)
#define ISPOST_DNMC9_X_LENGTH                  (8)
#define ISPOST_DNMC9_X_DEFAULT                 (0)
#define ISPOST_DNMC9_X_SIGNED_FIELD            (0)


// DN Mask Curve 10 Register.
#define ISPOST_DNMC10_OFFSET                   (0x00E0)
#define ISPOST_DNMC10                          (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC10_OFFSET)

/*
 * ISPOST, DNMC10, S
 * Slope of mask value for mask curve. (format 0.0.12).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC10_S_MASK                   (0xFFF00000)
#define ISPOST_DNMC10_S_LSBMASK                (0x00000FFF)
#define ISPOST_DNMC10_S_SHIFT                  (20)
#define ISPOST_DNMC10_S_LENGTH                 (12)
#define ISPOST_DNMC10_S_DEFAULT                (0)
#define ISPOST_DNMC10_S_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC10, R
 * Absolute difference value range exponent: RANGE = 2^R.
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC10_R_MASK                   (0x00070000)
#define ISPOST_DNMC10_R_LSBMASK                (0x00000007)
#define ISPOST_DNMC10_R_SHIFT                  (16)
#define ISPOST_DNMC10_R_LENGTH                 (3)
#define ISPOST_DNMC10_R_DEFAULT                (0)
#define ISPOST_DNMC10_R_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC10, Y
 * Mask value for mask curve. (format 0.0.8).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC10_Y_MASK                   (0x0000FF00)
#define ISPOST_DNMC10_Y_LSBMASK                (0x000000FF)
#define ISPOST_DNMC10_Y_SHIFT                  (8)
#define ISPOST_DNMC10_Y_LENGTH                 (8)
#define ISPOST_DNMC10_Y_DEFAULT                (0)
#define ISPOST_DNMC10_Y_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC10, X
 * Absolute difference value for mask curve. (format 0.8.0).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 * -------------------------------------------------------------------------
 * NOTE: the difference of next point X and this point X values must match
 * this point range value R (RANGE(N) = 2^R(N) = X(N+1) - X(N))
 * -------------------------------------------------------------------------
 */
#define ISPOST_DNMC10_X_MASK                   (0x000000FF)
#define ISPOST_DNMC10_X_LSBMASK                (0x000000FF)
#define ISPOST_DNMC10_X_SHIFT                  (0)
#define ISPOST_DNMC10_X_LENGTH                 (8)
#define ISPOST_DNMC10_X_DEFAULT                (0)
#define ISPOST_DNMC10_X_SIGNED_FIELD           (0)


// DN Mask Curve 11 Register.
#define ISPOST_DNMC11_OFFSET                   (0x00E4)
#define ISPOST_DNMC11                          (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC11_OFFSET)

/*
 * ISPOST, DNMC11, S
 * Slope of mask value for mask curve. (format 0.0.12).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC11_S_MASK                   (0xFFF00000)
#define ISPOST_DNMC11_S_LSBMASK                (0x00000FFF)
#define ISPOST_DNMC11_S_SHIFT                  (20)
#define ISPOST_DNMC11_S_LENGTH                 (12)
#define ISPOST_DNMC11_S_DEFAULT                (0)
#define ISPOST_DNMC11_S_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC11, R
 * Absolute difference value range exponent: RANGE = 2^R.
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC11_R_MASK                   (0x00070000)
#define ISPOST_DNMC11_R_LSBMASK                (0x00000007)
#define ISPOST_DNMC11_R_SHIFT                  (16)
#define ISPOST_DNMC11_R_LENGTH                 (3)
#define ISPOST_DNMC11_R_DEFAULT                (0)
#define ISPOST_DNMC11_R_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC11, Y
 * Mask value for mask curve. (format 0.0.8).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC11_Y_MASK                   (0x0000FF00)
#define ISPOST_DNMC11_Y_LSBMASK                (0x000000FF)
#define ISPOST_DNMC11_Y_SHIFT                  (8)
#define ISPOST_DNMC11_Y_LENGTH                 (8)
#define ISPOST_DNMC11_Y_DEFAULT                (0)
#define ISPOST_DNMC11_Y_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC11, X
 * Absolute difference value for mask curve. (format 0.8.0).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 * -------------------------------------------------------------------------
 * NOTE: the difference of next point X and this point X values must match
 * this point range value R (RANGE(N) = 2^R(N) = X(N+1) - X(N))
 * -------------------------------------------------------------------------
 */
#define ISPOST_DNMC11_X_MASK                   (0x000000FF)
#define ISPOST_DNMC11_X_LSBMASK                (0x000000FF)
#define ISPOST_DNMC11_X_SHIFT                  (0)
#define ISPOST_DNMC11_X_LENGTH                 (8)
#define ISPOST_DNMC11_X_DEFAULT                (0)
#define ISPOST_DNMC11_X_SIGNED_FIELD           (0)


// DN Mask Curve 12 Register.
#define ISPOST_DNMC12_OFFSET                   (0x00E8)
#define ISPOST_DNMC12                          (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC12_OFFSET)

/*
 * ISPOST, DNMC12, S
 * Slope of mask value for mask curve. (format 0.0.12).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC12_S_MASK                   (0xFFF00000)
#define ISPOST_DNMC12_S_LSBMASK                (0x00000FFF)
#define ISPOST_DNMC12_S_SHIFT                  (20)
#define ISPOST_DNMC12_S_LENGTH                 (12)
#define ISPOST_DNMC12_S_DEFAULT                (0)
#define ISPOST_DNMC12_S_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC12, R
 * Absolute difference value range exponent: RANGE = 2^R.
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC12_R_MASK                   (0x00070000)
#define ISPOST_DNMC12_R_LSBMASK                (0x00000007)
#define ISPOST_DNMC12_R_SHIFT                  (16)
#define ISPOST_DNMC12_R_LENGTH                 (3)
#define ISPOST_DNMC12_R_DEFAULT                (0)
#define ISPOST_DNMC12_R_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC12, Y
 * Mask value for mask curve. (format 0.0.8).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC12_Y_MASK                   (0x0000FF00)
#define ISPOST_DNMC12_Y_LSBMASK                (0x000000FF)
#define ISPOST_DNMC12_Y_SHIFT                  (8)
#define ISPOST_DNMC12_Y_LENGTH                 (8)
#define ISPOST_DNMC12_Y_DEFAULT                (0)
#define ISPOST_DNMC12_Y_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC12, X
 * Absolute difference value for mask curve. (format 0.8.0).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 * -------------------------------------------------------------------------
 * NOTE: the difference of next point X and this point X values must match
 * this point range value R (RANGE(N) = 2^R(N) = X(N+1) - X(N))
 * -------------------------------------------------------------------------
 */
#define ISPOST_DNMC12_X_MASK                   (0x000000FF)
#define ISPOST_DNMC12_X_LSBMASK                (0x000000FF)
#define ISPOST_DNMC12_X_SHIFT                  (0)
#define ISPOST_DNMC12_X_LENGTH                 (8)
#define ISPOST_DNMC12_X_DEFAULT                (0)
#define ISPOST_DNMC12_X_SIGNED_FIELD           (0)


// DN Mask Curve 13 Register.
#define ISPOST_DNMC13_OFFSET                   (0x00EC)
#define ISPOST_DNMC13                          (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC13_OFFSET)

/*
 * ISPOST, DNMC13, S
 * Slope of mask value for mask curve. (format 0.0.12).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC13_S_MASK                   (0xFFF00000)
#define ISPOST_DNMC13_S_LSBMASK                (0x00000FFF)
#define ISPOST_DNMC13_S_SHIFT                  (20)
#define ISPOST_DNMC13_S_LENGTH                 (12)
#define ISPOST_DNMC13_S_DEFAULT                (0)
#define ISPOST_DNMC13_S_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC13, R
 * Absolute difference value range exponent: RANGE = 2^R.
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC13_R_MASK                   (0x00070000)
#define ISPOST_DNMC13_R_LSBMASK                (0x00000007)
#define ISPOST_DNMC13_R_SHIFT                  (16)
#define ISPOST_DNMC13_R_LENGTH                 (3)
#define ISPOST_DNMC13_R_DEFAULT                (0)
#define ISPOST_DNMC13_R_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC13, Y
 * Mask value for mask curve. (format 0.0.8).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC13_Y_MASK                   (0x0000FF00)
#define ISPOST_DNMC13_Y_LSBMASK                (0x000000FF)
#define ISPOST_DNMC13_Y_SHIFT                  (8)
#define ISPOST_DNMC13_Y_LENGTH                 (8)
#define ISPOST_DNMC13_Y_DEFAULT                (0)
#define ISPOST_DNMC13_Y_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC13, X
 * Absolute difference value for mask curve. (format 0.8.0).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 * -------------------------------------------------------------------------
 * NOTE: the difference of next point X and this point X values must match
 * this point range value R (RANGE(N) = 2^R(N) = X(N+1) - X(N))
 * -------------------------------------------------------------------------
 */
#define ISPOST_DNMC13_X_MASK                   (0x000000FF)
#define ISPOST_DNMC13_X_LSBMASK                (0x000000FF)
#define ISPOST_DNMC13_X_SHIFT                  (0)
#define ISPOST_DNMC13_X_LENGTH                 (8)
#define ISPOST_DNMC13_X_DEFAULT                (0)
#define ISPOST_DNMC13_X_SIGNED_FIELD           (0)


// DN Mask Curve 14 Register.
#define ISPOST_DNMC14_OFFSET                   (0x00F0)
#define ISPOST_DNMC14                          (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC14_OFFSET)

/*
 * ISPOST, DNMC14, S
 * Slope of mask value for mask curve. (format 0.0.12).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC14_S_MASK                   (0xFFF00000)
#define ISPOST_DNMC14_S_LSBMASK                (0x00000FFF)
#define ISPOST_DNMC14_S_SHIFT                  (20)
#define ISPOST_DNMC14_S_LENGTH                 (12)
#define ISPOST_DNMC14_S_DEFAULT                (0)
#define ISPOST_DNMC14_S_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC14, R
 * Absolute difference value range exponent: RANGE = 2^R.
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC14_R_MASK                   (0x00070000)
#define ISPOST_DNMC14_R_LSBMASK                (0x00000007)
#define ISPOST_DNMC14_R_SHIFT                  (16)
#define ISPOST_DNMC14_R_LENGTH                 (3)
#define ISPOST_DNMC14_R_DEFAULT                (0)
#define ISPOST_DNMC14_R_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC14, Y
 * Mask value for mask curve. (format 0.0.8).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC14_Y_MASK                   (0x0000FF00)
#define ISPOST_DNMC14_Y_LSBMASK                (0x000000FF)
#define ISPOST_DNMC14_Y_SHIFT                  (8)
#define ISPOST_DNMC14_Y_LENGTH                 (8)
#define ISPOST_DNMC14_Y_DEFAULT                (0)
#define ISPOST_DNMC14_Y_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC14, X
 * Absolute difference value for mask curve. (format 0.8.0).
 * This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 * -------------------------------------------------------------------------
 * NOTE: the difference of next point X and this point X values must match
 * this point range value R (RANGE(N) = 2^R(N) = X(N+1) - X(N))
 * -------------------------------------------------------------------------
 */
#define ISPOST_DNMC14_X_MASK                   (0x000000FF)
#define ISPOST_DNMC14_X_LSBMASK                (0x000000FF)
#define ISPOST_DNMC14_X_SHIFT                  (0)
#define ISPOST_DNMC14_X_LENGTH                 (8)
#define ISPOST_DNMC14_X_DEFAULT                (0)
#define ISPOST_DNMC14_X_SIGNED_FIELD           (0)


// DN Control Register 0.
#define ISPOST_DNCTRL0_OFFSET                  (0x00F8)
#define ISPOST_DNCTRL0                         (ISPOST_PERI_BASE_ADDR + ISPOST_DNCTRL0_OFFSET)

/*
 * ISPOST, DNCTRL0, IDX
 * IDX – DN mask curve index.
 */
#define ISPOST_DNCTRL0_IDX_MASK                (0x03000000)
#define ISPOST_DNCTRL0_IDX_LSBMASK             (0x00000003)
#define ISPOST_DNCTRL0_IDX_SHIFT               (24)
#define ISPOST_DNCTRL0_IDX_LENGTH              (2)
#define ISPOST_DNCTRL0_IDX_DEFAULT             (0)
#define ISPOST_DNCTRL0_IDX_SIGNED_FIELD        (0)


// DN AXI Control Register.
#define ISPOST_DNAXI_OFFSET                    (0x00FC)
#define ISPOST_DNAXI                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNAXI_OFFSET)

/*
 * ISPOST, DNAXI, REFWID
 * REFWID – Reference Image AXI Write ID.
 * (Please note: ALL AXI Read Ids must be unique).
 */
#define ISPOST_DNAXI_REFWID_MASK               (0x00000300)
#define ISPOST_DNAXI_REFWID_LSBMASK            (0x00000003)
#define ISPOST_DNAXI_REFWID_SHIFT              (8)
#define ISPOST_DNAXI_REFWID_LENGTH             (2)
#define ISPOST_DNAXI_REFWID_DEFAULT            (0)
#define ISPOST_DNAXI_REFWID_SIGNED_FIELD       (0)

/*
 * ISPOST, DNAXI, REFRID
 * REFRID – Reference Image AXI Read ID.
 * (Please note: ALL AXI Read Ids must be unique).
 */
#define ISPOST_DNAXI_REFRID_MASK               (0x00000003)
#define ISPOST_DNAXI_REFRID_LSBMASK            (0x00000003)
#define ISPOST_DNAXI_REFRID_SHIFT              (0)
#define ISPOST_DNAXI_REFRID_LENGTH             (2)
#define ISPOST_DNAXI_REFRID_DEFAULT            (0)
#define ISPOST_DNAXI_REFRID_SIGNED_FIELD       (0)



// Scaled output registers offset.
//==========================================================
// SS0 Output Image Y Plane Start Address Register.
#define ISPOST_SS0AY_OFFSET                    (0x0100)
#define ISPOST_SS0AY                           (ISPOST_PERI_BASE_ADDR + ISPOST_SS0AY_OFFSET)

/*
 * ISPOST, SS0AY, SAD
 * Base address in QWORD (64bit) boundary.
 */
#define ISPOST_SS0AY_SAD_MASK                  (0xFFFFFFF8)
#define ISPOST_SS0AY_SAD_LSBMASK               (0x1FFFFFFF)
#define ISPOST_SS0AY_SAD_SHIFT                 (3)
#define ISPOST_SS0AY_SAD_LENGTH                (29)
#define ISPOST_SS0AY_SAD_DEFAULT               (0)
#define ISPOST_SS0AY_SAD_SIGNED_FIELD          (0)


// SS0 Output Image UV Plane Start Address Register.
#define ISPOST_SS0AUV_OFFSET                   (0x0104)
#define ISPOST_SS0AUV                          (ISPOST_PERI_BASE_ADDR + ISPOST_SS0AUV_OFFSET)

/*
 * ISPOST, SS0AUV, SAD
 * Base address in QWORD (64bit) boundary.
 */
#define ISPOST_SS0AUV_SAD_MASK                 (0xFFFFFFF8)
#define ISPOST_SS0AUV_SAD_LSBMASK              (0x1FFFFFFF)
#define ISPOST_SS0AUV_SAD_SHIFT                (3)
#define ISPOST_SS0AUV_SAD_LENGTH               (29)
#define ISPOST_SS0AUV_SAD_DEFAULT              (0)
#define ISPOST_SS0AUV_SAD_SIGNED_FIELD         (0)


// SS0 Output Image Stride Register.
#define ISPOST_SS0S_OFFSET                     (0x0108)
#define ISPOST_SS0S                            (ISPOST_PERI_BASE_ADDR + ISPOST_SS0S_OFFSET)

/*
 * ISPOST, SS0S, SD
 * Image stride (QWORD granularity).
 */
#define ISPOST_SS0S_SD_MASK                    (0x00001FF8)
#define ISPOST_SS0S_SD_LSBMASK                 (0x000003FF)
#define ISPOST_SS0S_SD_SHIFT                   (3)
#define ISPOST_SS0S_SD_LENGTH                  (10)
#define ISPOST_SS0S_SD_DEFAULT                 (0)
#define ISPOST_SS0S_SD_SIGNED_FIELD            (0)


// SS0 H Scaling Factor Register.
#define ISPOST_SS0HF_OFFSET                    (0x010C)
#define ISPOST_SS0HF                           (ISPOST_PERI_BASE_ADDR + ISPOST_SS0HF_OFFSET)

/*
 * ISPOST, SS0HF, SF
 * Scaling factor (format 0.0.12).
 */
#define ISPOST_SS0HF_SF_MASK                   (0x0FFF0000)
#define ISPOST_SS0HF_SF_LSBMASK                (0x00000FFF)
#define ISPOST_SS0HF_SF_SHIFT                  (16)
#define ISPOST_SS0HF_SF_LENGTH                 (12)
#define ISPOST_SS0HF_SF_DEFAULT                (0)
#define ISPOST_SS0HF_SF_SIGNED_FIELD           (0)

/*
 * ISPOST, SS0HF, SM
 * SM – scaling mode.
 */
#define ISPOST_SS0HF_SM_MASK                   (0x00000003)
#define ISPOST_SS0HF_SM_LSBMASK                (0x00000003)
#define ISPOST_SS0HF_SM_SHIFT                  (0)
#define ISPOST_SS0HF_SM_LENGTH                 (2)
#define ISPOST_SS0HF_SM_DEFAULT                (0)
#define ISPOST_SS0HF_SM_SIGNED_FIELD           (0)


// SS0 V Scaling Factor Register.
#define ISPOST_SS0VF_OFFSET                    (0x0110)
#define ISPOST_SS0VF                           (ISPOST_PERI_BASE_ADDR + ISPOST_SS0VF_OFFSET)

/*
 * ISPOST, SS0VF, SF
 * Scaling factor (format 0.0.12)  Minimum vertical scaling factor is x1/8.
 */
#define ISPOST_SS0VF_SF_MASK                   (0x0FFF0000)
#define ISPOST_SS0VF_SF_LSBMASK                (0x00000FFF)
#define ISPOST_SS0VF_SF_SHIFT                  (16)
#define ISPOST_SS0VF_SF_LENGTH                 (12)
#define ISPOST_SS0VF_SF_DEFAULT                (0)
#define ISPOST_SS0VF_SF_SIGNED_FIELD           (0)

/*
 * ISPOST, SS0VF, SM
 * SM – scaling mode.
 */
#define ISPOST_SS0VF_SM_MASK                   (0x00000003)
#define ISPOST_SS0VF_SM_LSBMASK                (0x00000003)
#define ISPOST_SS0VF_SM_SHIFT                  (0)
#define ISPOST_SS0VF_SM_LENGTH                 (2)
#define ISPOST_SS0VF_SM_DEFAULT                (0)
#define ISPOST_SS0VF_SM_SIGNED_FIELD           (0)


// SS0 Output Image Size Register.
#define ISPOST_SS0IW_OFFSET                    (0x0114)
#define ISPOST_SS0IW                           (ISPOST_PERI_BASE_ADDR + ISPOST_SS0IW_OFFSET)

/*
 * ISPOST, SS0IW, W
 * Output image width (pixel granularity).
 */
#define ISPOST_SS0IW_W_MASK                    (0x1FFF0000)
#define ISPOST_SS0IW_W_LSBMASK                 (0x00001FFF)
#define ISPOST_SS0IW_W_SHIFT                   (16)
#define ISPOST_SS0IW_W_LENGTH                  (13)
#define ISPOST_SS0IW_W_DEFAULT                 (0)
#define ISPOST_SS0IW_W_SIGNED_FIELD            (0)

/*
 * ISPOST, SS0IW, H
 * Output image height (line granularity).
 */
#define ISPOST_SS0IW_H_MASK                    (0x00001FFF)
#define ISPOST_SS0IW_H_LSBMASK                 (0x00001FFF)
#define ISPOST_SS0IW_H_SHIFT                   (0)
#define ISPOST_SS0IW_H_LENGTH                  (13)
#define ISPOST_SS0IW_H_DEFAULT                 (0)
#define ISPOST_SS0IW_H_SIGNED_FIELD            (0)


// SS1 Output Image Y Plane Start Address Register.
#define ISPOST_SS1AY_OFFSET                    (0x0118)
#define ISPOST_SS1AY                           (ISPOST_PERI_BASE_ADDR + ISPOST_SS1AY_OFFSET)

/*
 * ISPOST, SS1AY, SAD
 * Base address in QWORD (64bit) boundary.
 */
#define ISPOST_SS1AY_SAD_MASK                  (0xFFFFFFF8)
#define ISPOST_SS1AY_SAD_LSBMASK               (0x1FFFFFFF)
#define ISPOST_SS1AY_SAD_SHIFT                 (3)
#define ISPOST_SS1AY_SAD_LENGTH                (29)
#define ISPOST_SS1AY_SAD_DEFAULT               (0)
#define ISPOST_SS1AY_SAD_SIGNED_FIELD          (0)


// SS1 Output Image UV Plane Start Address Register.
#define ISPOST_SS1AUV_OFFSET                   (0x011C)
#define ISPOST_SS1AUV                          (ISPOST_PERI_BASE_ADDR + ISPOST_SS1AUV_OFFSET)

/*
 * ISPOST, SS1AUV, SAD
 * Base address in QWORD (64bit) boundary.
 */
#define ISPOST_SS1AUV_SAD_MASK                 (0xFFFFFFF8)
#define ISPOST_SS1AUV_SAD_LSBMASK              (0x1FFFFFFF)
#define ISPOST_SS1AUV_SAD_SHIFT                (3)
#define ISPOST_SS1AUV_SAD_LENGTH               (29)
#define ISPOST_SS1AUV_SAD_DEFAULT              (0)
#define ISPOST_SS1AUV_SAD_SIGNED_FIELD         (0)


// SS1 Output Image Stride Register.
#define ISPOST_SS1S_OFFSET                     (0x0120)
#define ISPOST_SS1S                            (ISPOST_PERI_BASE_ADDR + ISPOST_SS1S_OFFSET)

/*
 * ISPOST, SS1S, SD
 * Image stride (QWORD granularity).
 */
#define ISPOST_SS1S_SD_MASK                    (0x00001FF8)
#define ISPOST_SS1S_SD_LSBMASK                 (0x000003FF)
#define ISPOST_SS1S_SD_SHIFT                   (3)
#define ISPOST_SS1S_SD_LENGTH                  (10)
#define ISPOST_SS1S_SD_DEFAULT                 (0)
#define ISPOST_SS1S_SD_SIGNED_FIELD            (0)


// SS1 H Scaling Factor Register.
#define ISPOST_SS1HF_OFFSET                    (0x0124)
#define ISPOST_SS1HF                           (ISPOST_PERI_BASE_ADDR + ISPOST_SS1HF_OFFSET)

/*
 * ISPOST, SS1HF, SF
 * Scaling factor (format 0.0.12).
 */
#define ISPOST_SS1HF_SF_MASK                   (0x0FFF0000)
#define ISPOST_SS1HF_SF_LSBMASK                (0x00000FFF)
#define ISPOST_SS1HF_SF_SHIFT                  (16)
#define ISPOST_SS1HF_SF_LENGTH                 (12)
#define ISPOST_SS1HF_SF_DEFAULT                (0)
#define ISPOST_SS1HF_SF_SIGNED_FIELD           (0)

/*
 * ISPOST, SS1HF, SM
 * SM – scaling mode.
 */
#define ISPOST_SS1HF_SM_MASK                   (0x00000003)
#define ISPOST_SS1HF_SM_LSBMASK                (0x00000003)
#define ISPOST_SS1HF_SM_SHIFT                  (0)
#define ISPOST_SS1HF_SM_LENGTH                 (2)
#define ISPOST_SS1HF_SM_DEFAULT                (0)
#define ISPOST_SS1HF_SM_SIGNED_FIELD           (0)


// SS1 V Scaling Factor Register.
#define ISPOST_SS1VF_OFFSET                    (0x0128)
#define ISPOST_SS1VF                           (ISPOST_PERI_BASE_ADDR + ISPOST_SS1VF_OFFSET)

/*
 * ISPOST, SS1VF, SF
 * Scaling factor (format 0.0.12)  Minimum vertical scaling factor is x1/8.
 */
#define ISPOST_SS1VF_SF_MASK                   (0x0FFF0000)
#define ISPOST_SS1VF_SF_LSBMASK                (0x00000FFF)
#define ISPOST_SS1VF_SF_SHIFT                  (16)
#define ISPOST_SS1VF_SF_LENGTH                 (12)
#define ISPOST_SS1VF_SF_DEFAULT                (0)
#define ISPOST_SS1VF_SF_SIGNED_FIELD           (0)

/*
 * ISPOST, SS1VF, SM
 * SM – scaling mode.
 */
#define ISPOST_SS1VF_SM_MASK                   (0x00000003)
#define ISPOST_SS1VF_SM_LSBMASK                (0x00000003)
#define ISPOST_SS1VF_SM_SHIFT                  (0)
#define ISPOST_SS1VF_SM_LENGTH                 (2)
#define ISPOST_SS1VF_SM_DEFAULT                (0)
#define ISPOST_SS1VF_SM_SIGNED_FIELD           (0)


// SS1 Output Image Size Register.
#define ISPOST_SS1IW_OFFSET                    (0x012C)
#define ISPOST_SS1IW                           (ISPOST_PERI_BASE_ADDR + ISPOST_SS1IW_OFFSET)

/*
 * ISPOST, SS1IW, W
 * Output image width (pixel granularity).
 */
#define ISPOST_SS1IW_W_MASK                    (0x1FFF0000)
#define ISPOST_SS1IW_W_LSBMASK                 (0x00001FFF)
#define ISPOST_SS1IW_W_SHIFT                   (16)
#define ISPOST_SS1IW_W_LENGTH                  (13)
#define ISPOST_SS1IW_W_DEFAULT                 (0)
#define ISPOST_SS1IW_W_SIGNED_FIELD            (0)

/*
 * ISPOST, SS1IW, H
 * Output image height (line granularity).
 */
#define ISPOST_SS1IW_H_MASK                    (0x00001FFF)
#define ISPOST_SS1IW_H_LSBMASK                 (0x00001FFF)
#define ISPOST_SS1IW_H_SHIFT                   (0)
#define ISPOST_SS1IW_H_LENGTH                  (13)
#define ISPOST_SS1IW_H_DEFAULT                 (0)
#define ISPOST_SS1IW_H_SIGNED_FIELD            (0)


// SS AXI Control Register.
#define ISPOST_SSAXI_OFFSET                    (0x013C)
#define ISPOST_SSAXI                           (ISPOST_PERI_BASE_ADDR + ISPOST_SSAXI_OFFSET)

/*
 * ISPOST, SSAXI, SS1WID
 * SS1WID – SS1 Image AXI Write ID.
 * (Please note: ALL AXI Read Ids must be unique).
 */
#define ISPOST_SSAXI_SS1WID_MASK               (0x0000FF00)
#define ISPOST_SSAXI_SS1WID_LSBMASK            (0x000000FF)
#define ISPOST_SSAXI_SS1WID_SHIFT              (8)
#define ISPOST_SSAXI_SS1WID_LENGTH             (8)
#define ISPOST_SSAXI_SS1WID_DEFAULT            (0)
#define ISPOST_SSAXI_SS1WID_SIGNED_FIELD       (0)


/*
 * ISPOST, SSAXI, SS0WID
 * SS0WID – SS1 Image AXI Write ID.
 * (Please note: ALL AXI Read Ids must be unique).
 */
#define ISPOST_SSAXI_SS0WID_MASK               (0x000000FF)
#define ISPOST_SSAXI_SS0WID_LSBMASK            (0x000000FF)
#define ISPOST_SSAXI_SS0WID_SHIFT              (0)
#define ISPOST_SSAXI_SS0WID_LENGTH             (8)
#define ISPOST_SSAXI_SS0WID_DEFAULT            (0)
#define ISPOST_SSAXI_SS0WID_SIGNED_FIELD       (0)


//===== Dump Image to Memory Register =====
//==========================================================
// Dump Image to Memory : Trigger Register.
#define ISPOST_DUMPTRIG_OFFSET                 (0x0100)
#define ISPOST_DUMPTRIG                        (ISPOST_PERI_BASE_ADDR + ISPOST_DUMPTRIG_OFFSET)

/*
 * ISPOST, FILEID, TR
 */
#define ISPOST_DUMPTRIG_TR_MASK                (0x00000002)
#define ISPOST_DUMPTRIG_TR_LSBMASK             (0x00000001)
#define ISPOST_DUMPTRIG_TR_SHIFT               (1)
#define ISPOST_DUMPTRIG_TR_LENGTH              (1)
#define ISPOST_DUMPTRIG_TR_DEFAULT             (0)
#define ISPOST_DUMPTRIG_TR_SIGNED_FIELD        (0)


// Dump Image to Memory : Start Address Register.
#define ISPOST_DUMPA_OFFSET                    (0x0104)
#define ISPOST_DUMPA                           (ISPOST_PERI_BASE_ADDR + ISPOST_DUMPA_OFFSET)

/*
 * ISPOST, DUMPA, SAD
 * Base address in QWORD (64bit) boundary.
 */
#define ISPOST_DUMPA_SAD_MASK                  (0xFFFFFFF8)
#define ISPOST_DUMPA_SAD_LSBMASK               (0x1FFFFFFF)
#define ISPOST_DUMPA_SAD_SHIFT                 (3)
#define ISPOST_DUMPA_SAD_LENGTH                (29)
#define ISPOST_DUMPA_SAD_DEFAULT               (0)
#define ISPOST_DUMPA_SAD_SIGNED_FIELD          (0)


// Dump Image to Memory : File ID Register.
#define ISPOST_DUMPFID_OFFSET                  (0x0108)
#define ISPOST_DUMPFID                         (ISPOST_PERI_BASE_ADDR + ISPOST_DUMPFID_OFFSET)

/*
 * ISPOST, DUMPFID, ID
 */
#define ISPOST_DUMPFID_ID_MASK                  (0x0000000F)
#define ISPOST_DUMPFID_ID_LSBMASK               (0x0000000F)
#define ISPOST_DUMPFID_ID_SHIFT                 (0)
#define ISPOST_DUMPFID_ID_LENGTH                (4)
#define ISPOST_DUMPFID_ID_DEFAULT               (0)
#define ISPOST_DUMPFID_ID_SIGNED_FIELD          (0)

// Dump Image to Memory : Size Register.
#define ISPOST_DUMPSZ_OFFSET                   (0x010C)
#define ISPOST_DUMPSZ                          (ISPOST_PERI_BASE_ADDR + ISPOST_DUMPSZ_OFFSET)

/*
 * ISPOST, DUMPSZ, SZ
 */
#define ISPOST_DUMPSZ_SZ_MASK                  (0xFFFFFFFF)
#define ISPOST_DUMPSZ_SZ_LSBMASK               (0xFFFFFFFF)
#define ISPOST_DUMPSZ_SZ_SHIFT                 (0)
#define ISPOST_DUMPSZ_SZ_LENGTH                (32)
#define ISPOST_DUMPSZ_SZ_DEFAULT               (0)
#define ISPOST_DUMPSZ_SZ_SIGNED_FIELD          (0)
//===== Dump Image to Memory Register End =====


/*!
******************************************************************************

 @Macro	ISPOST_REGIO_READ_FIELD

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
#define ISPOST_REGIO_READ_FIELD(ui32RegValue, group, reg, field)				\
	((ui32RegValue & group##_##reg##_##field##_MASK) >> group##_##reg##_##field##_SHIFT)

#else

#define ISPOST_REGIO_READ_FIELD(ui32RegValue, group, reg, field)				\
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
#define REG_PRINT(...) printk(KERN_DEBUG __VA_ARGS__)
#else
#define REG_PRINT printf
#endif

    #define ISPOST_REGIO_CHECK_VALUE_FITS_WITHIN_FIELD(This_Group,This_Reg,This_Field,This_Value) \
    { \
	if ( ((ISPOST_UINT32)(ISPOST_INT32)This_Value) > (This_Group##_##This_Reg##_##This_Field##_LSBMASK) && \
	     (((ISPOST_UINT32)(ISPOST_INT32)This_Value) & (ISPOST_UINT32)~(This_Group##_##This_Reg##_##This_Field##_LSBMASK)) !=  \
	     (ISPOST_UINT32)~(This_Group##_##This_Reg##_##This_Field##_LSBMASK) )		\
           { \
	       REG_PRINT("%s: %d does not fit in %s:%s:%s\n", __FUNCTION__, This_Value, #This_Group, #This_Reg, #This_Field); \
	   } \
    }

#else
	#define ISPOST_REGIO_CHECK_VALUE_FITS_WITHIN_FIELD(This_Group,This_Reg,This_Field,This_Value)
#endif

/*!
******************************************************************************

 @Macro	ISPOST_REGIO_WRITE_FIELD

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
#define ISPOST_REGIO_WRITE_FIELD(ui32RegValue, group, reg, field, ui32Value)									\
{																												\
	ISPOST_REGIO_CHECK_VALUE_FITS_WITHIN_FIELD(group,reg,field,ui32Value);										\
	(ui32RegValue) =																							\
	((ui32RegValue) & ~(group##_##reg##_##field##_MASK)) |														\
		(((ISPOST_UINT32) (ui32Value) << (group##_##reg##_##field##_SHIFT)) & (group##_##reg##_##field##_MASK));	\
}

/*!
******************************************************************************

 @Macro	ISPOST_REGIO_WRITE_FIELD_LITE

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
#define ISPOST_REGIO_WRITE_FIELD_LITE(ui32RegValue, group, reg, field, ui32Value)			\
{																							\
	ISPOST_REGIO_CHECK_VALUE_FITS_WITHIN_FIELD(group,reg,field,ui32Value);					\
	(ui32RegValue) |= ( (ISPOST_UINT32) (ui32Value) << (group##_##reg##_##field##_SHIFT) );	\
}

/*!
******************************************************************************

 @Macro	ISPOST_REGIO_ENCODE_FIELD

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
#define ISPOST_REGIO_ENCODE_FIELD(group, reg, field, ui32Value)    \
                ( (ISPOST_UINT32) (ui32Value) << (group##_##reg##_##field##_SHIFT) )


#if defined(__cplusplus)
}
#endif


#endif //__IMAPX800_ISPOST_REG_H__
