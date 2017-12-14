#ifndef __IMAPX800_ISPOST_Q3F_REGS_H__
#define __IMAPX800_ISPOST_Q3F_REGS_H__


#if defined(__cplusplus)
extern "C" {
#endif //__cplusplus


// ISPOST base address
//#define ISPOST_PERI_BASE_ADDR                  ISPOST_BASE_ADDR


//==========================================================
// ISP post processor registers offset.
//----------------------------------------------------------
// ISPCTRL0 - ISPOST Control Register 0
#define ISPOST_ISPCTRL0_OFFSET                 (0x0000)
#define ISPOST_ISPCTRL0                        (ISPOST_PERI_BASE_ADDR + ISPOST_ISPCTRL0_OFFSET)

/*
 * ISPOST, ISPCTRL0, ALL_INT
 * [31:24] ALL_INT - All of Interrupt state.
 */
#define ISPOST_ISPCTRL0_ALL_INT_MASK          (0xFF000000)
#define ISPOST_ISPCTRL0_ALL_INT_LSBMASK       (0x000000FF)
#define ISPOST_ISPCTRL0_ALL_INT_SHIFT         (24)
#define ISPOST_ISPCTRL0_ALL_INT_LENGTH        (8)
#define ISPOST_ISPCTRL0_ALL_INT_DEFAULT       (0)
#define ISPOST_ISPCTRL0_ALL_INT_SIGNED_FIELD  (0)

/*
 * ISPOST, ISPCTRL0, LBERRINT
 * [27] LBERRINT - LC Line Buffer Error Interrupt state.
 */
#define ISPOST_ISPCTRL0_LBERRINT_MASK          (0x08000000)
#define ISPOST_ISPCTRL0_LBERRINT_LSBMASK       (0x00000001)
#define ISPOST_ISPCTRL0_LBERRINT_SHIFT         (27)
#define ISPOST_ISPCTRL0_LBERRINT_LENGTH        (1)
#define ISPOST_ISPCTRL0_LBERRINT_DEFAULT       (0)
#define ISPOST_ISPCTRL0_LBERRINT_SIGNED_FIELD  (0)

/*
 * ISPOST, ISPCTRL0, VSFWINT
 * [26] VSFWINT - VS Frame Write Interrupt state.
 */
#define ISPOST_ISPCTRL0_VSFWINT_MASK           (0x04000000)
#define ISPOST_ISPCTRL0_VSFWINT_LSBMASK        (0x00000001)
#define ISPOST_ISPCTRL0_VSFWINT_SHIFT          (26)
#define ISPOST_ISPCTRL0_VSFWINT_LENGTH         (1)
#define ISPOST_ISPCTRL0_VSFWINT_DEFAULT        (0)
#define ISPOST_ISPCTRL0_VSFWINT_SIGNED_FIELD   (0)

/*
 * ISPOST, ISPCTRL0, VSINT
 * [25] VSINT - VS Interrupt state.
 */
#define ISPOST_ISPCTRL0_VSINT_MASK             (0x02000000)
#define ISPOST_ISPCTRL0_VSINT_LSBMASK          (0x00000001)
#define ISPOST_ISPCTRL0_VSINT_SHIFT            (25)
#define ISPOST_ISPCTRL0_VSINT_LENGTH           (1)
#define ISPOST_ISPCTRL0_VSINT_DEFAULT          (0)
#define ISPOST_ISPCTRL0_VSINT_SIGNED_FIELD     (0)

/*
 * ISPOST, ISPCTRL0, INT
 * [24] INT - Interrupt state.
 */
#define ISPOST_ISPCTRL0_INT_MASK               (0x01000000)
#define ISPOST_ISPCTRL0_INT_LSBMASK            (0x00000001)
#define ISPOST_ISPCTRL0_INT_SHIFT              (24)
#define ISPOST_ISPCTRL0_INT_LENGTH             (1)
#define ISPOST_ISPCTRL0_INT_DEFAULT            (0)
#define ISPOST_ISPCTRL0_INT_SIGNED_FIELD       (0)

/*
 * ISPOST, ISPCTRL0, RSTVENCL
 * [23] RSTVENCL - Reset/initialize Video Encoder Link.
 */
#define ISPOST_ISPCTRL0_RSTVENCL_MASK          (0x00800000)
#define ISPOST_ISPCTRL0_RSTVENCL_LSBMASK       (0x00000001)
#define ISPOST_ISPCTRL0_RSTVENCL_SHIFT         (23)
#define ISPOST_ISPCTRL0_RSTVENCL_LENGTH        (1)
#define ISPOST_ISPCTRL0_RSTVENCL_DEFAULT       (0)
#define ISPOST_ISPCTRL0_RSTVENCL_SIGNED_FIELD  (0)

/*
 * ISPOST, ISPCTRL0, ENDNWR
 * [21] ENDNWR - Enable Denoise Write.
 */
#define ISPOST_ISPCTRL0_ENDNWR_MASK            (0x00200000)
#define ISPOST_ISPCTRL0_ENDNWR_LSBMASK         (0x00000001)
#define ISPOST_ISPCTRL0_ENDNWR_SHIFT           (21)
#define ISPOST_ISPCTRL0_ENDNWR_LENGTH          (1)
#define ISPOST_ISPCTRL0_ENDNWR_DEFAULT         (0)
#define ISPOST_ISPCTRL0_ENDNWR_SIGNED_FIELD    (0)

/*
 * ISPOST, ISPCTRL0, ENUO
 * [20] ENUO - Enable Unscaled Output.
 */
#define ISPOST_ISPCTRL0_ENUO_MASK              (0x00100000)
#define ISPOST_ISPCTRL0_ENUO_LSBMASK           (0x00000001)
#define ISPOST_ISPCTRL0_ENUO_SHIFT             (20)
#define ISPOST_ISPCTRL0_ENUO_LENGTH            (1)
#define ISPOST_ISPCTRL0_ENUO_DEFAULT           (0)
#define ISPOST_ISPCTRL0_ENUO_SIGNED_FIELD      (0)

/*
 * ISPOST, ISPCTRL0, ENLCLB
 * [16] ENLCLB - Enable LC Line Buffer.
 */
#define ISPOST_ISPCTRL0_ENLCLB_MASK            (0x00010000)
#define ISPOST_ISPCTRL0_ENLCLB_LSBMASK         (0x00000001)
#define ISPOST_ISPCTRL0_ENLCLB_SHIFT           (16)
#define ISPOST_ISPCTRL0_ENLCLB_LENGTH          (1)
#define ISPOST_ISPCTRL0_ENLCLB_DEFAULT         (0)
#define ISPOST_ISPCTRL0_ENLCLB_SIGNED_FIELD    (0)

/*
 * ISPOST, ISPCTRL0, ENVENCL
 * [15] ENVENCL - Enable Video Encoder Link.
 */
#define ISPOST_ISPCTRL0_ENVENCL_MASK           (0x00008000)
#define ISPOST_ISPCTRL0_ENVENCL_LSBMASK        (0x00000001)
#define ISPOST_ISPCTRL0_ENVENCL_SHIFT          (15)
#define ISPOST_ISPCTRL0_ENVENCL_LENGTH         (1)
#define ISPOST_ISPCTRL0_ENVENCL_DEFAULT        (0)
#define ISPOST_ISPCTRL0_ENVENCL_SIGNED_FIELD   (0)

/*
 * ISPOST, ISPCTRL0, ENVSR
 * [14] ENVSR - Enable Video Stabilization Image Registration (Frame Buffer Input).
 */
#define ISPOST_ISPCTRL0_ENVSR_MASK             (0x00004000)
#define ISPOST_ISPCTRL0_ENVSR_LSBMASK          (0x00000001)
#define ISPOST_ISPCTRL0_ENVSR_SHIFT            (14)
#define ISPOST_ISPCTRL0_ENVSR_LENGTH           (1)
#define ISPOST_ISPCTRL0_ENVSR_DEFAULT          (0)
#define ISPOST_ISPCTRL0_ENVSR_SIGNED_FIELD     (0)

/*
 * ISPOST, ISPCTRL0, ENVSRC
 * [13] ENVSRC - Enable Video Stabilization Image Registration (Core).
 */
#define ISPOST_ISPCTRL0_ENVSRC_MASK            (0x00002000)
#define ISPOST_ISPCTRL0_ENVSRC_LSBMASK         (0x00000001)
#define ISPOST_ISPCTRL0_ENVSRC_SHIFT           (13)
#define ISPOST_ISPCTRL0_ENVSRC_LENGTH          (1)
#define ISPOST_ISPCTRL0_ENVSRC_DEFAULT         (0)
#define ISPOST_ISPCTRL0_ENVSRC_SIGNED_FIELD    (0)

/*
 * ISPOST, ISPCTRL0, ENSS1
 * [12] ENSS1 - Enable Scaled Stream 1.
 */
#define ISPOST_ISPCTRL0_ENSS1_MASK             (0x00001000)
#define ISPOST_ISPCTRL0_ENSS1_LSBMASK          (0x00000001)
#define ISPOST_ISPCTRL0_ENSS1_SHIFT            (12)
#define ISPOST_ISPCTRL0_ENSS1_LENGTH           (1)
#define ISPOST_ISPCTRL0_ENSS1_DEFAULT          (0)
#define ISPOST_ISPCTRL0_ENSS1_SIGNED_FIELD     (0)

/*
 * ISPOST, ISPCTRL0, ENSS0
 * [11] ENSS0 - Enable Scaled Stream 0.
 */
#define ISPOST_ISPCTRL0_ENSS0_MASK             (0x00000800)
#define ISPOST_ISPCTRL0_ENSS0_LSBMASK          (0x00000001)
#define ISPOST_ISPCTRL0_ENSS0_SHIFT            (11)
#define ISPOST_ISPCTRL0_ENSS0_LENGTH           (1)
#define ISPOST_ISPCTRL0_ENSS0_DEFAULT          (0)
#define ISPOST_ISPCTRL0_ENSS0_SIGNED_FIELD     (0)

/*
 * ISPOST, ISPCTRL0, ENDN
 * [10] ENDN - Enable Denoise module.
 */
#define ISPOST_ISPCTRL0_ENDN_MASK              (0x00000400)
#define ISPOST_ISPCTRL0_ENDN_LSBMASK           (0x00000001)
#define ISPOST_ISPCTRL0_ENDN_SHIFT             (10)
#define ISPOST_ISPCTRL0_ENDN_LENGTH            (1)
#define ISPOST_ISPCTRL0_ENDN_DEFAULT           (0)
#define ISPOST_ISPCTRL0_ENDN_SIGNED_FIELD      (0)

/*
 * ISPOST, ISPCTRL0, ENOV
 * [9] ENOV - Enable Overlay module.
 */
#define ISPOST_ISPCTRL0_ENOV_MASK              (0x00000200)
#define ISPOST_ISPCTRL0_ENOV_LSBMASK           (0x00000001)
#define ISPOST_ISPCTRL0_ENOV_SHIFT             (9)
#define ISPOST_ISPCTRL0_ENOV_LENGTH            (1)
#define ISPOST_ISPCTRL0_ENOV_DEFAULT           (0)
#define ISPOST_ISPCTRL0_ENOV_SIGNED_FIELD      (0)

/*
 * ISPOST, ISPCTRL0, ENLC
 * [8] ENLC - Enable Lens Correction module.
 */
#define ISPOST_ISPCTRL0_ENLC_MASK              (0x00000100)
#define ISPOST_ISPCTRL0_ENLC_LSBMASK           (0x00000001)
#define ISPOST_ISPCTRL0_ENLC_SHIFT             (8)
#define ISPOST_ISPCTRL0_ENLC_LENGTH            (1)
#define ISPOST_ISPCTRL0_ENLC_DEFAULT           (0)
#define ISPOST_ISPCTRL0_ENLC_SIGNED_FIELD      (0)

/*
 * ISPOST, ISPCTRL0, RSTISPL
 * [7] RSTISPL - Reset/initialize ISP Link.
 */
#define ISPOST_ISPCTRL0_RSTISPL_MASK           (0x00000080)
#define ISPOST_ISPCTRL0_RSTISPL_LSBMASK        (0x00000001)
#define ISPOST_ISPCTRL0_RSTISPL_SHIFT          (7)
#define ISPOST_ISPCTRL0_RSTISPL_LENGTH         (1)
#define ISPOST_ISPCTRL0_RSTISPL_DEFAULT        (0)
#define ISPOST_ISPCTRL0_RSTISPL_SIGNED_FIELD   (0)

/*
 * ISPOST, ISPCTRL0, ENISPL
 * [6] ENISPL - Enable ISP Post processor ISP Link.
 */
#define ISPOST_ISPCTRL0_ENISPL_MASK            (0x00000040)
#define ISPOST_ISPCTRL0_ENISPL_LSBMASK         (0x00000001)
#define ISPOST_ISPCTRL0_ENISPL_SHIFT           (6)
#define ISPOST_ISPCTRL0_ENISPL_LENGTH          (1)
#define ISPOST_ISPCTRL0_ENISPL_DEFAULT         (0)
#define ISPOST_ISPCTRL0_ENISPL_SIGNED_FIELD    (0)

/*
 * ISPOST, ISPCTRL0, RSTVS
 * [5] RSTVS - Reset/initialize module group: VS LMV (necessary before enabling).
 */
#define ISPOST_ISPCTRL0_RSTVS_MASK            (0x00000020)
#define ISPOST_ISPCTRL0_RSTVS_LSBMASK         (0x00000001)
#define ISPOST_ISPCTRL0_RSTVS_SHIFT           (5)
#define ISPOST_ISPCTRL0_RSTVS_LENGTH          (1)
#define ISPOST_ISPCTRL0_RSTVS_DEFAULT         (0)
#define ISPOST_ISPCTRL0_RSTVS_SIGNED_FIELD    (0)

/*
 * ISPOST, ISPCTRL0, ENVS
 * [4] ENVS - Enable ISP Post processor module group: VS LMV.
 */
#define ISPOST_ISPCTRL0_ENVS_MASK             (0x00000010)
#define ISPOST_ISPCTRL0_ENVS_LSBMASK          (0x00000001)
#define ISPOST_ISPCTRL0_ENVS_SHIFT            (4)
#define ISPOST_ISPCTRL0_ENVS_LENGTH           (1)
#define ISPOST_ISPCTRL0_ENVS_DEFAULT          (0)
#define ISPOST_ISPCTRL0_ENVS_SIGNED_FIELD     (0)

/*
 * ISPOST, ISPCTRL0, RSTILC
 * [3] RSTILC - Reset/initialize module group: LC (necessary before enabling).
 */
#define ISPOST_ISPCTRL0_RSTILC_MASK           (0x00000008)
#define ISPOST_ISPCTRL0_RSTILC_LSBMASK        (0x00000001)
#define ISPOST_ISPCTRL0_RSTILC_SHIFT          (3)
#define ISPOST_ISPCTRL0_RSTILC_LENGTH         (1)
#define ISPOST_ISPCTRL0_RSTILC_DEFAULT        (0)
#define ISPOST_ISPCTRL0_RSTILC_SIGNED_FIELD   (0)

/*
 * ISPOST, ISPCTRL0, ENILC
 * [2] ENILC - Enable ISP Post processor module group: LC.
 */
#define ISPOST_ISPCTRL0_ENILC_MASK            (0x00000004)
#define ISPOST_ISPCTRL0_ENILC_LSBMASK         (0x00000001)
#define ISPOST_ISPCTRL0_ENILC_SHIFT           (2)
#define ISPOST_ISPCTRL0_ENILC_LENGTH          (1)
#define ISPOST_ISPCTRL0_ENILC_DEFAULT         (0)
#define ISPOST_ISPCTRL0_ENILC_SIGNED_FIELD    (0)

/*
 * ISPOST, ISPCTRL0, RST
 * [1] RST - Reset/initialize module group: VS IR, OV, O, SS0, SS1, VO, VENC link (necessary before enabling).
 */
#define ISPOST_ISPCTRL0_RST_MASK               (0x00000002)
#define ISPOST_ISPCTRL0_RST_LSBMASK            (0x00000001)
#define ISPOST_ISPCTRL0_RST_SHIFT              (1)
#define ISPOST_ISPCTRL0_RST_LENGTH             (1)
#define ISPOST_ISPCTRL0_RST_DEFAULT            (0)
#define ISPOST_ISPCTRL0_RST_SIGNED_FIELD       (0)

/*
 * ISPOST, ISPCTRL0, EN
 * [0] EN- Enable ISP Post processor module group: VS IR, OV, O, SS0, SS1, VO, VENC link.
 */
#define ISPOST_ISPCTRL0_EN_MASK                (0x00000001)
#define ISPOST_ISPCTRL0_EN_LSBMASK             (0x00000001)
#define ISPOST_ISPCTRL0_EN_SHIFT               (0)
#define ISPOST_ISPCTRL0_EN_LENGTH              (1)
#define ISPOST_ISPCTRL0_EN_DEFAULT             (0)
#define ISPOST_ISPCTRL0_EN_SIGNED_FIELD        (0)


//----------------------------------------------------------
// ISPSTAT0 - ISPOST Status Register 0.
#define ISPOST_ISPSTAT0_OFFSET                 (0x0004)
#define ISPOST_ISPSTAT0                        (ISPOST_PERI_BASE_ADDR + ISPOST_ISPSTAT0_OFFSET)

/*
 * ISPOST, ISPSTAT0, LBERR
 * [8] LBERR - Video stabilization frame output run status.
 */
#define ISPOST_ISPSTAT0_LBERR_MASK             (0x00000100)
#define ISPOST_ISPSTAT0_LBERR_LSBMASK          (0x00000001)
#define ISPOST_ISPSTAT0_LBERR_SHIFT            (8)
#define ISPOST_ISPSTAT0_LBERR_LENGTH           (1)
#define ISPOST_ISPSTAT0_LBERR_DEFAULT          (0)
#define ISPOST_ISPSTAT0_LBERR_SIGNED_FIELD     (0)

/*
 * ISPOST, ISPSTAT0, STAT_VO
 * [6] STAT_VO - Video Output run status.
 */
#define ISPOST_ISPSTAT0_STAT_VO_MASK           (0x00000040)
#define ISPOST_ISPSTAT0_STAT_VO_LSBMASK        (0x00000001)
#define ISPOST_ISPSTAT0_STAT_VO_SHIFT          (6)
#define ISPOST_ISPSTAT0_STAT_VO_LENGTH         (1)
#define ISPOST_ISPSTAT0_STAT_VO_DEFAULT        (0)
#define ISPOST_ISPSTAT0_STAT_VO_SIGNED_FIELD   (0)

/*
 * ISPOST, ISPSTAT0, STAT2
 * [5] STAT2 - Video stabilization frame output run status.
 */
#define ISPOST_ISPSTAT0_STAT2_MASK             (0x00000020)
#define ISPOST_ISPSTAT0_STAT2_LSBMASK          (0x00000001)
#define ISPOST_ISPSTAT0_STAT2_SHIFT            (5)
#define ISPOST_ISPSTAT0_STAT2_LENGTH           (1)
#define ISPOST_ISPSTAT0_STAT2_DEFAULT          (0)
#define ISPOST_ISPSTAT0_STAT2_SIGNED_FIELD     (0)

/*
 * ISPOST, ISPSTAT0, STAT1
 * [4] STAT1 - Video stabilization motion vector estimation run status.
 */
#define ISPOST_ISPSTAT0_STAT1_MASK             (0x00000010)
#define ISPOST_ISPSTAT0_STAT1_LSBMASK          (0x00000001)
#define ISPOST_ISPSTAT0_STAT1_SHIFT            (4)
#define ISPOST_ISPSTAT0_STAT1_LENGTH           (1)
#define ISPOST_ISPSTAT0_STAT1_DEFAULT          (0)
#define ISPOST_ISPSTAT0_STAT1_SIGNED_FIELD     (0)

/*
 * ISPOST, ISPSTAT0, STAT_SS1
 * [3] STAT_SS1 - Scaled Stream 1 run status.
 */
#define ISPOST_ISPSTAT0_STAT_SS1_MASK          (0x00000008)
#define ISPOST_ISPSTAT0_STAT_SS1_LSBMASK       (0x00000001)
#define ISPOST_ISPSTAT0_STAT_SS1_SHIFT         (3)
#define ISPOST_ISPSTAT0_STAT_SS1_LENGTH        (1)
#define ISPOST_ISPSTAT0_STAT_SS1_DEFAULT       (0)
#define ISPOST_ISPSTAT0_STAT_SS1_SIGNED_FIELD  (0)

/*
 * ISPOST, ISPSTAT0, STAT_SS0
 * [2] STAT_SS0 - Scaled Stream 0 run status.
 */
#define ISPOST_ISPSTAT0_STAT_SS0_MASK          (0x00000004)
#define ISPOST_ISPSTAT0_STAT_SS0_LSBMASK       (0x00000001)
#define ISPOST_ISPSTAT0_STAT_SS0_SHIFT         (2)
#define ISPOST_ISPSTAT0_STAT_SS0_LENGTH        (1)
#define ISPOST_ISPSTAT0_STAT_SS0_DEFAULT       (0)
#define ISPOST_ISPSTAT0_STAT_SS0_SIGNED_FIELD  (0)

/*
 * ISPOST, ISPSTAT0, STAT_UO
 * [1] STAT_UO - Unscaled Output run status.
 */
#define ISPOST_ISPSTAT0_STAT_UO_MASK           (0x00000002)
#define ISPOST_ISPSTAT0_STAT_UO_LSBMASK        (0x00000001)
#define ISPOST_ISPSTAT0_STAT_UO_SHIFT          (1)
#define ISPOST_ISPSTAT0_STAT_UO_LENGTH         (1)
#define ISPOST_ISPSTAT0_STAT_UO_DEFAULT        (0)
#define ISPOST_ISPSTAT0_STAT_UO_SIGNED_FIELD   (0)

/*
 * ISPOST, ISPSTAT0, STAT0
 * [0] STAT0 - ISPOST run status.
 */
#define ISPOST_ISPSTAT0_STAT0_MASK             (0x00000001)
#define ISPOST_ISPSTAT0_STAT0_LSBMASK          (0x00000001)
#define ISPOST_ISPSTAT0_STAT0_SHIFT            (0)
#define ISPOST_ISPSTAT0_STAT0_LENGTH           (1)
#define ISPOST_ISPSTAT0_STAT0_DEFAULT          (0)
#define ISPOST_ISPSTAT0_STAT0_SIGNED_FIELD     (0)



//==========================================================
// Lens distortion correction registers offset.
//----------------------------------------------------------
// LCSRCAY - LC Source Image Y Plane Start Address Register.
#define ISPOST_LCSRCAY_OFFSET                  (0x0008)
#define ISPOST_LCSRCAY                         (ISPOST_PERI_BASE_ADDR + ISPOST_LCSRCAY_OFFSET)

/*
 * ISPOST, LCSRCAY, SAD
 * [31:3] SAD - Base address in QWORD (64bit) boundary.
 */
#define ISPOST_LCSRCAY_SAD_MASK                (0xFFFFFFF8)
#define ISPOST_LCSRCAY_SAD_LSBMASK             (0xFFFFFFF8)
#define ISPOST_LCSRCAY_SAD_SHIFT               (0)
#define ISPOST_LCSRCAY_SAD_LENGTH              (32)
#define ISPOST_LCSRCAY_SAD_DEFAULT             (0)
#define ISPOST_LCSRCAY_SAD_SIGNED_FIELD        (0)


//----------------------------------------------------------
// LCSRCAUV - LC Source Image UV Plane Start Address Register.
#define ISPOST_LCSRCAUV_OFFSET                 (0x0010)
#define ISPOST_LCSRCAUV                        (ISPOST_PERI_BASE_ADDR + ISPOST_LCSRCAUV_OFFSET)

/*
 * ISPOST, LCSRCAUV, SAD
 * [31:3] SAD - Base address in QWORD (64bit) boundary.
 */
#define ISPOST_LCSRCAUV_SAD_MASK               (0xFFFFFFF8)
#define ISPOST_LCSRCAUV_SAD_LSBMASK            (0xFFFFFFF8)
#define ISPOST_LCSRCAUV_SAD_SHIFT              (0)
#define ISPOST_LCSRCAUV_SAD_LENGTH             (32)
#define ISPOST_LCSRCAUV_SAD_DEFAULT            (0)
#define ISPOST_LCSRCAUV_SAD_SIGNED_FIELD       (0)


//----------------------------------------------------------
// LCSRCS - LC Source Image Stride Register.
#define ISPOST_LCSRCS_OFFSET                   (0x0014)
#define ISPOST_LCSRCS                          (ISPOST_PERI_BASE_ADDR + ISPOST_LCSRCS_OFFSET)

/*
 * ISPOST, LCSRCS, SD
 * [12:3] SD - Image stride (QWORD granularity).
 */
#define ISPOST_LCSRCS_SD_MASK                  (0x00001FF8)
#define ISPOST_LCSRCS_SD_LSBMASK               (0x00001FF8)
#define ISPOST_LCSRCS_SD_SHIFT                 (0)
#define ISPOST_LCSRCS_SD_LENGTH                (13)
#define ISPOST_LCSRCS_SD_DEFAULT               (0)
#define ISPOST_LCSRCS_SD_SIGNED_FIELD          (0)


//----------------------------------------------------------
// LCSRCSZ - LC Source Image Size Register.
#define ISPOST_LCSRCSZ_OFFSET                  (0x0018)
#define ISPOST_LCSRCSZ                         (ISPOST_PERI_BASE_ADDR + ISPOST_LCSRCSZ_OFFSET)

/*
 * ISPOST, LCSRCSZ, W
 * [28:16] W - Image width (pixel granularity).
 */
#define ISPOST_LCSRCSZ_W_MASK                  (0x1FFF0000)
#define ISPOST_LCSRCSZ_W_LSBMASK               (0x00001FFF)
#define ISPOST_LCSRCSZ_W_SHIFT                 (16)
#define ISPOST_LCSRCSZ_W_LENGTH                (13)
#define ISPOST_LCSRCSZ_W_DEFAULT               (0)
#define ISPOST_LCSRCSZ_W_SIGNED_FIELD          (0)

/*
 * ISPOST, LCSRCSZ, H
 * [12:0] H - Image height (line granularity).
 */
#define ISPOST_LCSRCSZ_H_MASK                  (0x00001FFF)
#define ISPOST_LCSRCSZ_H_LSBMASK               (0x00001FFF)
#define ISPOST_LCSRCSZ_H_SHIFT                 (0)
#define ISPOST_LCSRCSZ_H_LENGTH                (13)
#define ISPOST_LCSRCSZ_H_DEFAULT               (0)
#define ISPOST_LCSRCSZ_H_SIGNED_FIELD          (0)


//----------------------------------------------------------
// LCSRCBFC - LC Source Image Back Fill Color Register.
#define ISPOST_LCSRCBFC_OFFSET                 (0x001C)
#define ISPOST_LCSRCBFC                        (ISPOST_PERI_BASE_ADDR + ISPOST_LCSRCBFC_OFFSET)

/*
 * ISPOST, LCSRCBFC, Y
 * [23:16] Y - Source Back Fill Y component value. (0-255).
 */
#define ISPOST_LCSRCBFC_Y_MASK                 (0x00FF0000)
#define ISPOST_LCSRCBFC_Y_LSBMASK              (0x000000FF)
#define ISPOST_LCSRCBFC_Y_SHIFT                (16)
#define ISPOST_LCSRCBFC_Y_LENGTH               (8)
#define ISPOST_LCSRCBFC_Y_DEFAULT              (0)
#define ISPOST_LCSRCBFC_Y_SIGNED_FIELD         (0)

/*
 * ISPOST, LCSRCBFC, CB
 * [15:8] CB - Source Back Fill CB component value (0-255).
 */
#define ISPOST_LCSRCBFC_CB_MASK                (0x0000FF00)
#define ISPOST_LCSRCBFC_CB_LSBMASK             (0x000000FF)
#define ISPOST_LCSRCBFC_CB_SHIFT               (8)
#define ISPOST_LCSRCBFC_CB_LENGTH              (8)
#define ISPOST_LCSRCBFC_CB_DEFAULT             (0)
#define ISPOST_LCSRCBFC_CB_SIGNED_FIELD        (0)

/*
 * ISPOST, LCSRCBFC, CR
 * [7:0] CR - Source Back Fill CR component value (0-255).
 */
#define ISPOST_LCSRCBFC_CR_MASK                (0x000000FF)
#define ISPOST_LCSRCBFC_CR_LSBMASK             (0x000000FF)
#define ISPOST_LCSRCBFC_CR_SHIFT               (0)
#define ISPOST_LCSRCBFC_CR_LENGTH              (8)
#define ISPOST_LCSRCBFC_CR_DEFAULT             (0)
#define ISPOST_LCSRCBFC_CR_SIGNED_FIELD        (0)


//----------------------------------------------------------
// LCGBA - LC Grid Buffer Start Address Register.
#define ISPOST_LCGBA_OFFSET                    (0x0020)
#define ISPOST_LCGBA                           (ISPOST_PERI_BASE_ADDR + ISPOST_LCGBA_OFFSET)

/*
 * ISPOST, LCGBA, SAD
 * [31:3] SAD - Base address in QWORD (64bit) boundary.
 */
#define ISPOST_LCGBA_SAD_MASK                  (0xFFFFFFF8)
#define ISPOST_LCGBA_SAD_LSBMASK               (0xFFFFFFF8)
#define ISPOST_LCGBA_SAD_SHIFT                 (0)
#define ISPOST_LCGBA_SAD_LENGTH                (32)
#define ISPOST_LCGBA_SAD_DEFAULT               (0)
#define ISPOST_LCGBA_SAD_SIGNED_FIELD          (0)


//----------------------------------------------------------
// LCGBS - LC Grid Buffer Stride Register.
#define ISPOST_LCGBS_OFFSET                    (0x0024)
#define ISPOST_LCGBS                           (ISPOST_PERI_BASE_ADDR + ISPOST_LCGBS_OFFSET)

/*
 * ISPOST, LCGBS, SD
 * [12:3] SD - Image stride (QWORD granularity).
 */
#define ISPOST_LCGBS_SD_MASK                   (0x00001FF8)
#define ISPOST_LCGBS_SD_LSBMASK                (0x00001FF8)
#define ISPOST_LCGBS_SD_SHIFT                  (0)
#define ISPOST_LCGBS_SD_LENGTH                 (13)
#define ISPOST_LCGBS_SD_DEFAULT                (0)
#define ISPOST_LCGBS_SD_SIGNED_FIELD           (0)


//----------------------------------------------------------
// LCGBSZ - LC Grid Size Register.
#define ISPOST_LCGBSZ_OFFSET                   (0x0028)
#define ISPOST_LCGBSZ                          (ISPOST_PERI_BASE_ADDR + ISPOST_LCGBSZ_OFFSET)

/*
 * ISPOST, LCGBSZ, LBEN
 * [8] LBEN - Grid line buffer enable.
 */
#define ISPOST_LCGBSZ_LBEN_MASK                (0x00000100)
#define ISPOST_LCGBSZ_LBEN_LSBMASK             (0x00000001)
#define ISPOST_LCGBSZ_LBEN_SHIFT               (8)
#define ISPOST_LCGBSZ_LBEN_LENGTH              (1)
#define ISPOST_LCGBSZ_LBEN_DEFAULT             (0)
#define ISPOST_LCGBSZ_LBEN_SIGNED_FIELD        (0)

/*
 * ISPOST, LCGBSZ, SIZE
 * [1:0] SIZE - LC grid size.
 *       00 : 8x8 pixels.
 *       01 : 16x16 pixels.
 *       10 : 32x32 pixels.
 *       11 : Reserved.
 */
#define ISPOST_LCGBSZ_SIZE_MASK                (0x00000003)
#define ISPOST_LCGBSZ_SIZE_LSBMASK             (0x00000003)
#define ISPOST_LCGBSZ_SIZE_SHIFT               (0)
#define ISPOST_LCGBSZ_SIZE_LENGTH              (2)
#define ISPOST_LCGBSZ_SIZE_DEFAULT             (0)
#define ISPOST_LCGBSZ_SIZE_SIGNED_FIELD        (0)


//----------------------------------------------------------
// LCLBERR - LC Line Buffer Error Register.
#define ISPOST_LCLBERR_OFFSET                  (0x002C)
#define ISPOST_LCLBERR                         (ISPOST_PERI_BASE_ADDR + ISPOST_LCLBERR_OFFSET)

/*
 * ISPOST, LCLBERR, ERROR
 * [31:0] ERROR - Line Buffer error information.
 */
#define ISPOST_LCLBERR_ERROR_MASK              (0xFFFFFFFF)
#define ISPOST_LCLBERR_ERROR_LSBMASK           (0xFFFFFFFF)
#define ISPOST_LCLBERR_ERROR_SHIFT             (0)
#define ISPOST_LCLBERR_ERROR_LENGTH            (32)
#define ISPOST_LCLBERR_ERROR_DEFAULT           (0)
#define ISPOST_LCLBERR_ERROR_SIGNED_FIELD      (0)


//----------------------------------------------------------
// LCPCM - LC Pixel Cache, Line Buffer Mode Register.
#define ISPOST_LCPCM_OFFSET                    (0x0030)
#define ISPOST_LCPCM                           (ISPOST_PERI_BASE_ADDR + ISPOST_LCPCM_OFFSET)

/*
 * ISPOST, LCPCM, WLBE
 * [11] WLBE - Wait for Line Buffer End Image before LC signals done interrupt.
 *             0 : STACK.
 *                 When stack is full, the new error info will not be pushed, to store first 8 error info entries
 *             1 : FIFO
 *                 When fifo is full, the earliest error info will be dropped, to store latest 8 error info entries
 */
#define ISPOST_LCPCM_WLBE_MASK                 (0x00000800)
#define ISPOST_LCPCM_WLBE_LSBMASK              (0x00000001)
#define ISPOST_LCPCM_WLBE_SHIFT                (11)
#define ISPOST_LCPCM_WLBE_LENGTH               (1)
#define ISPOST_LCPCM_WLBE_DEFAULT              (0)
#define ISPOST_LCPCM_WLBE_SIGNED_FIELD         (0)

/*
 * ISPOST, LCPCM, FIFO_TYPE
 * [10] FIFO_TYPE - Line Buffer Error FIFO Type.
 */
#define ISPOST_LCPCM_FIFO_TYPE_MASK            (0x00000400)
#define ISPOST_LCPCM_FIFO_TYPE_LSBMASK         (0x00000001)
#define ISPOST_LCPCM_FIFO_TYPE_SHIFT           (10)
#define ISPOST_LCPCM_FIFO_TYPE_LENGTH          (1)
#define ISPOST_LCPCM_FIFO_TYPE_DEFAULT         (0)
#define ISPOST_LCPCM_FIFO_TYPE_SIGNED_FIELD    (0)

/*
 * ISPOST, LCPCM, LBFMT
 * [9:8] LBFMT - Line Buffer Internal Format.
 */
#define ISPOST_LCPCM_LBFMT_MASK                (0x00000300)
#define ISPOST_LCPCM_LBFMT_LSBMASK             (0x00000003)
#define ISPOST_LCPCM_LBFMT_SHIFT               (8)
#define ISPOST_LCPCM_LBFMT_LENGTH              (2)
#define ISPOST_LCPCM_LBFMT_DEFAULT             (0)
#define ISPOST_LCPCM_LBFMT_SIGNED_FIELD        (0)

/*
 * ISPOST, LCPCM, PREF
 * [6:2] PREF - Cache controller cfg value.
 */
#define ISPOST_LCPCM_PREF_MASK                 (0x0000007C)
#define ISPOST_LCPCM_PREF_LSBMASK              (0x0000001F)
#define ISPOST_LCPCM_PREF_SHIFT                (2)
#define ISPOST_LCPCM_PREF_LENGTH               (5)
#define ISPOST_LCPCM_PREF_DEFAULT              (0)
#define ISPOST_LCPCM_PREF_SIGNED_FIELD         (0)

/*
 * ISPOST, LCPCM, CDSS
 * [6:5] CDSS - cache data shape select.
 *              Allowed values are from 0 to 3, for fisheye cfg = 2 or cfg = 3 is better.
 */
#define ISPOST_LCPCM_CDSS_MASK                 (0x00000060)
#define ISPOST_LCPCM_CDSS_LSBMASK              (0x00000003)
#define ISPOST_LCPCM_CDSS_SHIFT                (5)
#define ISPOST_LCPCM_CDSS_LENGTH               (2)
#define ISPOST_LCPCM_CDSS_DEFAULT              (0)
#define ISPOST_LCPCM_CDSS_SIGNED_FIELD         (0)

/*
 * ISPOST, LCPCM, BLS
 * [4] BLS - Burst length select.
 *           0 : 4QW.
 *           1 : 8QW.
 *           Suggestion: For barrel distortion uses 8QW, fisheye uses 4QW
 */
#define ISPOST_LCPCM_BLS_MASK                  (0x00000010)
#define ISPOST_LCPCM_BLS_LSBMASK               (0x00000001)
#define ISPOST_LCPCM_BLS_SHIFT                 (4)
#define ISPOST_LCPCM_BLS_LENGTH                (1)
#define ISPOST_LCPCM_BLS_DEFAULT               (0)
#define ISPOST_LCPCM_BLS_SIGNED_FIELD          (0)

/*
 * ISPOST, LCPCM, PREFCTRL
 * Bit[3:2] PREFCTRL - Pre-fetch control.
 *                     Bit3: Disable main pixel fifo function. Value =1 means disable.
 *                     Bit2: Disable pre-fetch function. Value =1 means disable.
 */
#define ISPOST_LCPCM_PREFCTRL_MASK             (0x0000000C)
#define ISPOST_LCPCM_PREFCTRL_LSBMASK          (0x00000003)
#define ISPOST_LCPCM_PREFCTRL_SHIFT            (2)
#define ISPOST_LCPCM_PREFCTRL_LENGTH           (2)
#define ISPOST_LCPCM_PREFCTRL_DEFAULT          (0)
#define ISPOST_LCPCM_PREFCTRL_SIGNED_FIELD     (0)

/*
 * ISPOST, LCPCM, CBCRSWAP
 * [0] CBCRSWAP - CB/CR swap - 0: No swap, 1: swap.
 */
#define ISPOST_LCPCM_CBCRSWAP_MASK             (0x00000001)
#define ISPOST_LCPCM_CBCRSWAP_LSBMASK          (0x00000001)
#define ISPOST_LCPCM_CBCRSWAP_SHIFT            (0)
#define ISPOST_LCPCM_CBCRSWAP_LENGTH           (1)
#define ISPOST_LCPCM_CBCRSWAP_DEFAULT          (0)
#define ISPOST_LCPCM_CBCRSWAP_SIGNED_FIELD     (0)


//----------------------------------------------------------
// LCPGM - LC Pixel Coordinate Generator Mode Register.
#define ISPOST_LCPGM_OFFSET                    (0x0034)
#define ISPOST_LCPGM                           (ISPOST_PERI_BASE_ADDR + ISPOST_LCPGM_OFFSET)

/*
 * ISPOST, LCPGM, SCANW
 * [9:8] SCANW - scan width.
 *               00 : 8.
 *               01 : 16.
 *               10 : 32.
 *               11 : Reserved.
 */
#define ISPOST_LCPGM_SCANW_MASK                (0x00000300)
#define ISPOST_LCPGM_SCANW_LSBMASK             (0x00000003)
#define ISPOST_LCPGM_SCANW_SHIFT               (8)
#define ISPOST_LCPGM_SCANW_LENGTH              (2)
#define ISPOST_LCPGM_SCANW_DEFAULT             (0)
#define ISPOST_LCPGM_SCANW_SIGNED_FIELD        (0)

/*
 * ISPOST, LCPGM, SCANH
 * [5:4] SCANH - scan height.
 *               00 : 8.
 *               01 : 16.
 *               10 : 32.
 *               11 : 2.
 *               NOTE: Use SCANH=16 for compatibility with direct link to video codec when VSIR frame buffer is disabled,
 *                         SCANH=2 is required if VS LMV block is active,
 *                         SCANH=2 can be used with line buffer enabled and VSIR frame buffer for best performance.
 */
#define ISPOST_LCPGM_SCANH_MASK                (0x00000030)
#define ISPOST_LCPGM_SCANH_LSBMASK             (0x00000003)
#define ISPOST_LCPGM_SCANH_SHIFT               (4)
#define ISPOST_LCPGM_SCANH_LENGTH              (2)
#define ISPOST_LCPGM_SCANH_DEFAULT             (0)
#define ISPOST_LCPGM_SCANH_SIGNED_FIELD        (0)

/*
 * ISPOST, LCPGM, SCANM
 * [2:0] SCANM - scan mode.
 *               000 : LR (left → right), TB (top → bottom).
 *               001 : LR, BT.
 *               010 : RL, TB.
 *               011 : RL, BT.
 *               100 : TB (top → bottom), LR (left → right).
 *               101 : BT, LR.
 *               110 : TB, RL.
 *               111 : BT, RL.
 */
#define ISPOST_LCPGM_SCANM_MASK                (0x00000007)
#define ISPOST_LCPGM_SCANM_LSBMASK             (0x00000007)
#define ISPOST_LCPGM_SCANM_SHIFT               (0)
#define ISPOST_LCPGM_SCANM_LENGTH              (3)
#define ISPOST_LCPGM_SCANM_DEFAULT             (0)
#define ISPOST_LCPGM_SCANM_SIGNED_FIELD        (0)


//----------------------------------------------------------
// LCPGSZ - LC Pixel Coordinate Generator Destination Size Register.
#define ISPOST_LCPGSZ_OFFSET                   (0x0038)
#define ISPOST_LCPGSZ                          (ISPOST_PERI_BASE_ADDR + ISPOST_LCPGSZ_OFFSET)

/*
 * ISPOST, LCPGSZ, W
 * [28:16] W - Destination image width (pixel granularity).
 */
#define ISPOST_LCPGSZ_W_MASK                   (0x1FFF0000)
#define ISPOST_LCPGSZ_W_LSBMASK                (0x00001FFF)
#define ISPOST_LCPGSZ_W_SHIFT                  (16)
#define ISPOST_LCPGSZ_W_LENGTH                 (13)
#define ISPOST_LCPGSZ_W_DEFAULT                (0)
#define ISPOST_LCPGSZ_W_SIGNED_FIELD           (0)

/*
 * ISPOST, LCPGSZ, H
 * [12:0] H - Destination image width (pixel granularity).
 */
#define ISPOST_LCPGSZ_H_MASK                   (0x00001FFF)
#define ISPOST_LCPGSZ_H_LSBMASK                (0x00001FFF)
#define ISPOST_LCPGSZ_H_SHIFT                  (0)
#define ISPOST_LCPGSZ_H_LENGTH                 (13)
#define ISPOST_LCPGSZ_H_DEFAULT                (0)
#define ISPOST_LCPGSZ_H_SIGNED_FIELD           (0)


//----------------------------------------------------------
// LCOAY - LC Output Image Y Plane Start Address.
#define ISPOST_LCOAY_OFFSET                    (0x0040)
#define ISPOST_LCOAY                           (ISPOST_PERI_BASE_ADDR + ISPOST_LCOAY_OFFSET)

/*
 * ISPOST, LCOAY, SAD
 * [31:3] SAD - Base address in QWORD (64bit) boundary.
 */
#define ISPOST_LCOAY_SAD_MASK                  (0xFFFFFFF8)
#define ISPOST_LCOAY_SAD_LSBMASK               (0xFFFFFFF8)
#define ISPOST_LCOAY_SAD_SHIFT                 (0)
#define ISPOST_LCOAY_SAD_LENGTH                (32)
#define ISPOST_LCOAY_SAD_DEFAULT               (0)
#define ISPOST_LCOAY_SAD_SIGNED_FIELD          (0)


//----------------------------------------------------------
// LCOAUV - LC Output Image UV Plane Start Address Register.
#define ISPOST_LCOAUV_OFFSET                   (0x0044)
#define ISPOST_LCOAUV                          (ISPOST_PERI_BASE_ADDR + ISPOST_LCOAUV_OFFSET)

/*
 * ISPOST, LCOAUV, SAD
 * [31:3] SAD - Base address in QWORD (64bit) boundary.
 */
#define ISPOST_LCOAUV_SAD_MASK                 (0xFFFFFFF8)
#define ISPOST_LCOAUV_SAD_LSBMASK              (0xFFFFFFF8)
#define ISPOST_LCOAUV_SAD_SHIFT                (0)
#define ISPOST_LCOAUV_SAD_LENGTH               (32)
#define ISPOST_LCOAUV_SAD_DEFAULT              (0)
#define ISPOST_LCOAUV_SAD_SIGNED_FIELD         (0)


//----------------------------------------------------------
// LCOS - LC Output Image Stride Register.
#define ISPOST_LCOS_OFFSET                     (0x0048)
#define ISPOST_LCOS                            (ISPOST_PERI_BASE_ADDR + ISPOST_LCOS_OFFSET)

/*
 * ISPOST, LCOS, SD
 * [12:3] SD - Image stride (QWORD granularity).
 */
#define ISPOST_LCOS_SD_MASK                    (0x00001FF8)
#define ISPOST_LCOS_SD_LSBMASK                 (0x00001FF8)
#define ISPOST_LCOS_SD_SHIFT                   (0)
#define ISPOST_LCOS_SD_LENGTH                  (13)
#define ISPOST_LCOS_SD_DEFAULT                 (0)
#define ISPOST_LCOS_SD_SIGNED_FIELD            (0)


//----------------------------------------------------------
// LCLBGEOIDX - LC Line Buffer Line Geometry Index Register.
#define ISPOST_LCLBGEOIDX_OFFSET               (0x004C)
#define ISPOST_LCLBGEOIDX                      (ISPOST_PERI_BASE_ADDR + ISPOST_LCLBGEOIDX_OFFSET)

/*
 * ISPOST, LCLBGEOIDX, AINC
 * [12] AINC - Autoincrement enable.
 */
#define ISPOST_LCLBGEOIDX_AINC_MASK            (0x00001000)
#define ISPOST_LCLBGEOIDX_AINC_LSBMASK         (0x00000001)
#define ISPOST_LCLBGEOIDX_AINC_SHIFT           (12)
#define ISPOST_LCLBGEOIDX_AINC_LENGTH          (1)
#define ISPOST_LCLBGEOIDX_AINC_DEFAULT         (0)
#define ISPOST_LCLBGEOIDX_AINC_SIGNED_FIELD    (0)

/*
 * ISPOST, LCLBGEOIDX, IDX
 * [11:0] IDX - LCLBGEO index register.
 */
#define ISPOST_LCLBGEOIDX_IDX_MASK             (0x00000FFF)
#define ISPOST_LCLBGEOIDX_IDX_LSBMASK          (0x00000FFF)
#define ISPOST_LCLBGEOIDX_IDX_SHIFT            (0)
#define ISPOST_LCLBGEOIDX_IDX_LENGTH           (12)
#define ISPOST_LCLBGEOIDX_IDX_DEFAULT          (0)
#define ISPOST_LCLBGEOIDX_IDX_SIGNED_FIELD     (0)


//----------------------------------------------------------
// LCLBGEO - LC Line Buffer Line Geometry Register.
#define ISPOST_LCLBGEO_OFFSET                  (0x0050)
#define ISPOST_LCLBGEO                         (ISPOST_PERI_BASE_ADDR + ISPOST_LCLBGEO_OFFSET)

/*
 * ISPOST, LCLBGEO, LEND
 * [27:16] LEND - Line end.
 */
#define ISPOST_LCLBGEO_LEND_MASK               (0x0FFF0000)
#define ISPOST_LCLBGEO_LEND_LSBMASK            (0x00000FFF)
#define ISPOST_LCLBGEO_LEND_SHIFT              (16)
#define ISPOST_LCLBGEO_LEND_LENGTH             (12)
#define ISPOST_LCLBGEO_LEND_DEFAULT            (0)
#define ISPOST_LCLBGEO_LEND_SIGNED_FIELD       (0)

/*
 * ISPOST, LCLBGEO, LSTART
 * [11:0] LSTART - Line start.
 */
#define ISPOST_LCLBGEO_LSTART_MASK             (0x00000FFF)
#define ISPOST_LCLBGEO_LSTART_LSBMASK          (0x00000FFF)
#define ISPOST_LCLBGEO_LSTART_SHIFT            (0)
#define ISPOST_LCLBGEO_LSTART_LENGTH           (12)
#define ISPOST_LCLBGEO_LSTART_DEFAULT          (0)
#define ISPOST_LCLBGEO_LSTART_SIGNED_FIELD     (0)


//----------------------------------------------------------
// LCAXI - LC AXI Control Register.
#define ISPOST_LCAXI_OFFSET                    (0x005C)
#define ISPOST_LCAXI                           (ISPOST_PERI_BASE_ADDR + ISPOST_LCAXI_OFFSET)

/*
 * ISPOST, LCAXI, SRCRDL
 * [22:16] SRCRDL - Source Image AXI Read Request Limit.
 * Total number of QW requested by ISPOST for grid buffer by will be 64 - SRCRDL. Valid range: 56-4
 */
#define ISPOST_LCAXI_SRCRDL_MASK               (0x007F0000)
#define ISPOST_LCAXI_SRCRDL_LSBMASK            (0x0000007F)
#define ISPOST_LCAXI_SRCRDL_SHIFT              (16)
#define ISPOST_LCAXI_SRCRDL_LENGTH             (7)
#define ISPOST_LCAXI_SRCRDL_DEFAULT            (0)
#define ISPOST_LCAXI_SRCRDL_SIGNED_FIELD       (0)

/*
 * ISPOST, LCAXI, GBID
 * [15:8] GBID - Grid Buffer AXI Read ID.
 * (Please note: ALL AXI Read Ids must be unique).
 */
#define ISPOST_LCAXI_GBID_MASK                 (0x0000FF00)
#define ISPOST_LCAXI_GBID_LSBMASK              (0x000000FF)
#define ISPOST_LCAXI_GBID_SHIFT                (8)
#define ISPOST_LCAXI_GBID_LENGTH               (8)
#define ISPOST_LCAXI_GBID_DEFAULT              (0)
#define ISPOST_LCAXI_GBID_SIGNED_FIELD         (0)

/*
 * ISPOST, LCAXI, SRCID
 * [7:0] SRCID - Source Image AXI Read ID.
 * (Please note: ALL AXI Read Ids must be unique).
 */
#define ISPOST_LCAXI_SRCID_MASK                (0x000000FF)
#define ISPOST_LCAXI_SRCID_LSBMASK             (0x000000FF)
#define ISPOST_LCAXI_SRCID_SHIFT               (0)
#define ISPOST_LCAXI_SRCID_LENGTH              (8)
#define ISPOST_LCAXI_SRCID_DEFAULT             (0)
#define ISPOST_LCAXI_SRCID_SIGNED_FIELD        (0)



//==========================================================
// Overlay registers offset.
//----------------------------------------------------------
// OVM - OV Mode Register.
#define ISPOST_OVM_OFFSET                      (0x0060)
#define ISPOST_OVM                             (ISPOST_PERI_BASE_ADDR + ISPOST_OVM_OFFSET)

/*
 * ISPOST, OVM, ENOV7
 * [15] ENOV7 - Enable Overlay Layer 7.
 */
#define ISPOST_OVM_ENOV7_MASK                  (0x00008000)
#define ISPOST_OVM_ENOV7_LSBMASK               (0x00000001)
#define ISPOST_OVM_ENOV7_SHIFT                 (15)
#define ISPOST_OVM_ENOV7_LENGTH                (1)
#define ISPOST_OVM_ENOV7_DEFAULT               (0)
#define ISPOST_OVM_ENOV7_SIGNED_FIELD          (0)

/*
 * ISPOST, OVM, ENOV6
 * [14] ENOV6 - Enable Overlay Layer 6.
 */
#define ISPOST_OVM_ENOV6_MASK                  (0x00004000)
#define ISPOST_OVM_ENOV6_LSBMASK               (0x00000001)
#define ISPOST_OVM_ENOV6_SHIFT                 (14)
#define ISPOST_OVM_ENOV6_LENGTH                (1)
#define ISPOST_OVM_ENOV6_DEFAULT               (0)
#define ISPOST_OVM_ENOV6_SIGNED_FIELD          (0)

/*
 * ISPOST, OVM, ENOV5
 * [13] ENOV5 - Enable Overlay Layer 5.
 */
#define ISPOST_OVM_ENOV5_MASK                  (0x00002000)
#define ISPOST_OVM_ENOV5_LSBMASK               (0x00000001)
#define ISPOST_OVM_ENOV5_SHIFT                 (13)
#define ISPOST_OVM_ENOV5_LENGTH                (1)
#define ISPOST_OVM_ENOV5_DEFAULT               (0)
#define ISPOST_OVM_ENOV5_SIGNED_FIELD          (0)

/*
 * ISPOST, OVM, ENOV4
 * [12] ENOV4 - Enable Overlay Layer 4.
 */
#define ISPOST_OVM_ENOV4_MASK                  (0x00001000)
#define ISPOST_OVM_ENOV4_LSBMASK               (0x00000001)
#define ISPOST_OVM_ENOV4_SHIFT                 (12)
#define ISPOST_OVM_ENOV4_LENGTH                (1)
#define ISPOST_OVM_ENOV4_DEFAULT               (0)
#define ISPOST_OVM_ENOV4_SIGNED_FIELD          (0)

/*
 * ISPOST, OVM, ENOV3
 * [11] ENOV3 - Enable Overlay Layer 3.
 */
#define ISPOST_OVM_ENOV3_MASK                  (0x00000800)
#define ISPOST_OVM_ENOV3_LSBMASK               (0x00000001)
#define ISPOST_OVM_ENOV3_SHIFT                 (11)
#define ISPOST_OVM_ENOV3_LENGTH                (1)
#define ISPOST_OVM_ENOV3_DEFAULT               (0)
#define ISPOST_OVM_ENOV3_SIGNED_FIELD          (0)

/*
 * ISPOST, OVM, ENOV2
 * [10] ENOV2 - Enable Overlay Layer 2.
 */
#define ISPOST_OVM_ENOV2_MASK                  (0x00000400)
#define ISPOST_OVM_ENOV2_LSBMASK               (0x00000001)
#define ISPOST_OVM_ENOV2_SHIFT                 (10)
#define ISPOST_OVM_ENOV2_LENGTH                (1)
#define ISPOST_OVM_ENOV2_DEFAULT               (0)
#define ISPOST_OVM_ENOV2_SIGNED_FIELD          (0)

/*
 * ISPOST, OVM, ENOV1
 * [9] ENOV1 - Enable Overlay Layer 1.
 */
#define ISPOST_OVM_ENOV1_MASK                  (0x00000200)
#define ISPOST_OVM_ENOV1_LSBMASK               (0x00000001)
#define ISPOST_OVM_ENOV1_SHIFT                 (9)
#define ISPOST_OVM_ENOV1_LENGTH                (1)
#define ISPOST_OVM_ENOV1_DEFAULT               (0)
#define ISPOST_OVM_ENOV1_SIGNED_FIELD          (0)

/*
 * ISPOST, OVM, ENOV0
 * [8] ENOV0 - Enable Overlay Layer 0.
 */
#define ISPOST_OVM_ENOV0_MASK                  (0x00000100)
#define ISPOST_OVM_ENOV0_LSBMASK               (0x00000001)
#define ISPOST_OVM_ENOV0_SHIFT                 (8)
#define ISPOST_OVM_ENOV0_LENGTH                (1)
#define ISPOST_OVM_ENOV0_DEFAULT               (0)
#define ISPOST_OVM_ENOV0_SIGNED_FIELD          (0)

/*
 * ISPOST, OVM, BCO
 * [4] BCO - Background color override.
 *           0 : Disabled.
 *               Color of pixels with overlay index =0 will be determined by overlay image.
 *           1 : Enabled.
 *               Color of pixels with overlay index =0 will be set by register OVBC (090H).
 */
#define ISPOST_OVM_BCO_MASK                    (0x00000010)
#define ISPOST_OVM_BCO_LSBMASK                 (0x00000001)
#define ISPOST_OVM_BCO_SHIFT                   (4)
#define ISPOST_OVM_BCO_LENGTH                  (1)
#define ISPOST_OVM_BCO_DEFAULT                 (0)
#define ISPOST_OVM_BCO_SIGNED_FIELD            (0)

/*
 * ISPOST, OVM, OVM
 * [1:0] OVM - Overlay mode.
 *             00 : UV.
 *             01 : Y.
 *             10 : YUV.
 *             11 : Reserved.
 */
#define ISPOST_OVM_OVM_MASK                    (0x00000003)
#define ISPOST_OVM_OVM_LSBMASK                 (0x00000003)
#define ISPOST_OVM_OVM_SHIFT                   (0)
#define ISPOST_OVM_OVM_LENGTH                  (2)
#define ISPOST_OVM_OVM_DEFAULT                 (0)
#define ISPOST_OVM_OVM_SIGNED_FIELD            (0)


//----------------------------------------------------------
// OVRIAY - OV Overlay Image Y Plane Start Address Register.
#define ISPOST_OVRIAY_OFFSET                   (0x0064)
#define ISPOST_OVRIAY                          (ISPOST_PERI_BASE_ADDR + ISPOST_OVRIAY_OFFSET)

/*
 * ISPOST, OVRIAY, SAD
 * [31:3] SAD - Base address in QWORD (64bit) boundary.
 *              This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVRIAY_SAD_MASK                 (0xFFFFFFF8)
#define ISPOST_OVRIAY_SAD_LSBMASK              (0xFFFFFFF8)
#define ISPOST_OVRIAY_SAD_SHIFT                (0)
#define ISPOST_OVRIAY_SAD_LENGTH               (32)
#define ISPOST_OVRIAY_SAD_DEFAULT              (0)
#define ISPOST_OVRIAY_SAD_SIGNED_FIELD         (0)


//----------------------------------------------------------
// OVRIAUV - OV Overlay Image UV Plane Start Address Register.
#define ISPOST_OVRIAUV_OFFSET                  (0x0068)
#define ISPOST_OVRIAUV                         (ISPOST_PERI_BASE_ADDR + ISPOST_OVRIAUV_OFFSET)

/*
 * ISPOST, OVRIAUV, SAD
 * [31:3] SAD - Base address in QWORD (64bit) boundary.
 */
#define ISPOST_OVRIAUV_SAD_MASK                (0xFFFFFFF8)
#define ISPOST_OVRIAUV_SAD_LSBMASK             (0xFFFFFFF8)
#define ISPOST_OVRIAUV_SAD_SHIFT               (0)
#define ISPOST_OVRIAUV_SAD_LENGTH              (32)
#define ISPOST_OVRIAUV_SAD_DEFAULT             (0)
#define ISPOST_OVRIAUV_SAD_SIGNED_FIELD        (0)


//----------------------------------------------------------
// OVRIS - OV Overlay Image Stride Register.
#define ISPOST_OVRIS_OFFSET                    (0x006C)
#define ISPOST_OVRIS                           (ISPOST_PERI_BASE_ADDR + ISPOST_OVRIS_OFFSET)

/*
 * ISPOST, OVRIS, SD
 * [12:3] SD - Image stride (QWORD granularity).
 *             This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVRIS_SD_MASK                   (0x00001FF8)
#define ISPOST_OVRIS_SD_LSBMASK                (0x00001FF8)
#define ISPOST_OVRIS_SD_SHIFT                  (0)
#define ISPOST_OVRIS_SD_LENGTH                 (13)
#define ISPOST_OVRIS_SD_DEFAULT                (0)
#define ISPOST_OVRIS_SD_SIGNED_FIELD           (0)


//----------------------------------------------------------
// OVISZ - OV Overlay Image Size Register.
#define ISPOST_OVISZ_OFFSET                    (0x0070)
#define ISPOST_OVISZ                           (ISPOST_PERI_BASE_ADDR + ISPOST_OVISZ_OFFSET)

/*
 * ISPOST, OVISZ, W
 * [28:16] W - Overlay image width (pixel granularity).
 *             This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVISZ_W_MASK                    (0x1FFF0000)
#define ISPOST_OVISZ_W_LSBMASK                 (0x00001FFF)
#define ISPOST_OVISZ_W_SHIFT                   (16)
#define ISPOST_OVISZ_W_LENGTH                  (13)
#define ISPOST_OVISZ_W_DEFAULT                 (0)
#define ISPOST_OVISZ_W_SIGNED_FIELD            (0)

/*
 * ISPOST, OVISZ, H
 * [12:0] H - Overlay image height (line granularity).
 *            This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVISZ_H_MASK                    (0x00001FFF)
#define ISPOST_OVISZ_H_LSBMASK                 (0x00001FFF)
#define ISPOST_OVISZ_H_SHIFT                   (0)
#define ISPOST_OVISZ_H_LENGTH                  (13)
#define ISPOST_OVISZ_H_DEFAULT                 (0)
#define ISPOST_OVISZ_H_SIGNED_FIELD            (0)


//----------------------------------------------------------
// OVOFF - OV Overlay Offset Register.
#define ISPOST_OVOFF_OFFSET                    (0x0074)
#define ISPOST_OVOFF                           (ISPOST_PERI_BASE_ADDR + ISPOST_OVOFF_OFFSET)

/*
 * ISPOST, OVOFF, X
 * [28:16] X - Overlay image horizontal offset (in relation to destination image).
 *             This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVOFF_X_MASK                    (0x1FFF0000)
#define ISPOST_OVOFF_X_LSBMASK                 (0x00001FFF)
#define ISPOST_OVOFF_X_SHIFT                   (16)
#define ISPOST_OVOFF_X_LENGTH                  (13)
#define ISPOST_OVOFF_X_DEFAULT                 (0)
#define ISPOST_OVOFF_X_SIGNED_FIELD            (0)

/*
 * ISPOST, OVOFF, Y
 * [12:0] Y - Overlay image vertical offset (in relation to destination image).
 *            This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVOFF_Y_MASK                    (0x00001FFF)
#define ISPOST_OVOFF_Y_LSBMASK                 (0x00001FFF)
#define ISPOST_OVOFF_Y_SHIFT                   (0)
#define ISPOST_OVOFF_Y_LENGTH                  (13)
#define ISPOST_OVOFF_Y_DEFAULT                 (0)
#define ISPOST_OVOFF_Y_SIGNED_FIELD            (0)


//----------------------------------------------------------
// OVALPHA0 - OV Overlay Alpha Value 0 Register.
#define ISPOST_OVALPHA0_OFFSET                 (0x0078)
#define ISPOST_OVALPHA0                        (ISPOST_PERI_BASE_ADDR + ISPOST_OVALPHA0_OFFSET)

/*
 * ISPOST, OVALPHA0, ALPHA3
 * [31:24] ALPHA3 - Overlay image alpha value register. (IDX=0)
 *                  Overlay image Color 0 Alpha value register. (IDX=1-7)
 *                  This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVALPHA0_ALPHA3_MASK            (0xFF000000)
#define ISPOST_OVALPHA0_ALPHA3_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA0_ALPHA3_SHIFT           (24)
#define ISPOST_OVALPHA0_ALPHA3_LENGTH          (8)
#define ISPOST_OVALPHA0_ALPHA3_DEFAULT         (0)
#define ISPOST_OVALPHA0_ALPHA3_SIGNED_FIELD    (0)

/*
 * ISPOST, OVALPHA0, ALPHA2
 * [23:16] ALPHA2 - Overlay image alpha value register. (IDX=0)
 *                  Overlay image Color 0 Y value register. (IDX=1-7)
 *                  This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVALPHA0_ALPHA2_MASK            (0x00FF0000)
#define ISPOST_OVALPHA0_ALPHA2_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA0_ALPHA2_SHIFT           (16)
#define ISPOST_OVALPHA0_ALPHA2_LENGTH          (8)
#define ISPOST_OVALPHA0_ALPHA2_DEFAULT         (0)
#define ISPOST_OVALPHA0_ALPHA2_SIGNED_FIELD    (0)

/*
 * ISPOST, OVALPHA0, ALPHA1
 * [15:8] ALPHA1 - Overlay image alpha value register. (IDX=0)
 *                 Overlay image Color 0 CB value register. (IDX=1-7)
 *                 This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVALPHA0_ALPHA1_MASK            (0x0000FF00)
#define ISPOST_OVALPHA0_ALPHA1_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA0_ALPHA1_SHIFT           (8)
#define ISPOST_OVALPHA0_ALPHA1_LENGTH          (8)
#define ISPOST_OVALPHA0_ALPHA1_DEFAULT         (0)
#define ISPOST_OVALPHA0_ALPHA1_SIGNED_FIELD    (0)

/*
 * ISPOST, OVALPHA0, ALPHA0
 * [7:0] ALPHA0 - Overlay image alpha value register. (IDX=0)
 *                Overlay image Color 0 CR value register. (IDX=1-7)
 *                This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVALPHA0_ALPHA0_MASK            (0x000000FF)
#define ISPOST_OVALPHA0_ALPHA0_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA0_ALPHA0_SHIFT           (0)
#define ISPOST_OVALPHA0_ALPHA0_LENGTH          (8)
#define ISPOST_OVALPHA0_ALPHA0_DEFAULT         (0)
#define ISPOST_OVALPHA0_ALPHA0_SIGNED_FIELD    (0)


//----------------------------------------------------------
// OVALPHA1 - OV Overlay Alpha Value 1 Register.
#define ISPOST_OVALPHA1_OFFSET                 (0x007C)
#define ISPOST_OVALPHA1                        (ISPOST_PERI_BASE_ADDR + ISPOST_OVALPHA1_OFFSET)

/*
 * ISPOST, OVALPHA1, ALPHA7
 * [31:24] ALPHA7 - Overlay image alpha value register. (IDX=0)
 *                  Overlay image Color 1 Alpha value register. (IDX=1-7)
 *                  This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVALPHA1_ALPHA7_MASK            (0xFF000000)
#define ISPOST_OVALPHA1_ALPHA7_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA1_ALPHA7_SHIFT           (24)
#define ISPOST_OVALPHA1_ALPHA7_LENGTH          (8)
#define ISPOST_OVALPHA1_ALPHA7_DEFAULT         (0)
#define ISPOST_OVALPHA1_ALPHA7_SIGNED_FIELD    (0)

/*
 * ISPOST, OVALPHA1, ALPHA6
 * [23:16] ALPHA6 - Overlay image alpha value register. (IDX=0)
 *                  Overlay image Color 1 Y value register. (IDX=1-7)
 *                  This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVALPHA1_ALPHA6_MASK            (0x00FF0000)
#define ISPOST_OVALPHA1_ALPHA6_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA1_ALPHA6_SHIFT           (16)
#define ISPOST_OVALPHA1_ALPHA6_LENGTH          (8)
#define ISPOST_OVALPHA1_ALPHA6_DEFAULT         (0)
#define ISPOST_OVALPHA1_ALPHA6_SIGNED_FIELD    (0)

/*
 * ISPOST, OVALPHA1, ALPHA5
 * [15:8] ALPHA5 - Overlay image alpha value register. (IDX=0)
 *                 Overlay image Color 1 CB value register. (IDX=1-7)
 *                 This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVALPHA1_ALPHA5_MASK            (0x0000FF00)
#define ISPOST_OVALPHA1_ALPHA5_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA1_ALPHA5_SHIFT           (8)
#define ISPOST_OVALPHA1_ALPHA5_LENGTH          (8)
#define ISPOST_OVALPHA1_ALPHA5_DEFAULT         (0)
#define ISPOST_OVALPHA1_ALPHA5_SIGNED_FIELD    (0)

/*
 * ISPOST, OVALPHA1, ALPHA4
 * [7:0] ALPHA4 - Overlay image alpha value register. (IDX=0)
 *                Overlay image Color 1 CR value register. (IDX=1-7)
 *                This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVALPHA1_ALPHA4_MASK            (0x000000FF)
#define ISPOST_OVALPHA1_ALPHA4_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA1_ALPHA4_SHIFT           (0)
#define ISPOST_OVALPHA1_ALPHA4_LENGTH          (8)
#define ISPOST_OVALPHA1_ALPHA4_DEFAULT         (0)
#define ISPOST_OVALPHA1_ALPHA4_SIGNED_FIELD    (0)


//----------------------------------------------------------
// OVALPHA2 - OV Overlay Alpha Value 2 Register.
#define ISPOST_OVALPHA2_OFFSET                 (0x0080)
#define ISPOST_OVALPHA2                        (ISPOST_PERI_BASE_ADDR + ISPOST_OVALPHA2_OFFSET)

/*
 * ISPOST, OVALPHA2, ALPHA11
 * [31:24] ALPHA11 - Overlay image alpha value register. (IDX=0)
 *                   Overlay image Color 2 Alpha value register. (IDX=1-7)
 *                   This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVALPHA2_ALPHA11_MASK           (0xFF000000)
#define ISPOST_OVALPHA2_ALPHA11_LSBMASK        (0x000000FF)
#define ISPOST_OVALPHA2_ALPHA11_SHIFT          (24)
#define ISPOST_OVALPHA2_ALPHA11_LENGTH         (8)
#define ISPOST_OVALPHA2_ALPHA11_DEFAULT        (0)
#define ISPOST_OVALPHA2_ALPHA11_SIGNED_FIELD   (0)

/*
 * ISPOST, OVALPHA2, ALPHA10
 * [23:16] ALPHA10 - Overlay image alpha value register. (IDX=0)
 *                   Overlay image Color 2 Y value register. (IDX=1-7)
 *                   This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVALPHA2_ALPHA10_MASK           (0x00FF0000)
#define ISPOST_OVALPHA2_ALPHA10_LSBMASK        (0x000000FF)
#define ISPOST_OVALPHA2_ALPHA10_SHIFT          (16)
#define ISPOST_OVALPHA2_ALPHA10_LENGTH         (8)
#define ISPOST_OVALPHA2_ALPHA10_DEFAULT        (0)
#define ISPOST_OVALPHA2_ALPHA10_SIGNED_FIELD   (0)

/*
 * ISPOST, OVALPHA2, ALPHA9
 * [15:8] ALPHA9 - Overlay image alpha value register. (IDX=0)
 *                 Overlay image Color 2 CB value register. (IDX=1-7)
 *                 This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVALPHA2_ALPHA9_MASK            (0x0000FF00)
#define ISPOST_OVALPHA2_ALPHA9_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA2_ALPHA9_SHIFT           (8)
#define ISPOST_OVALPHA2_ALPHA9_LENGTH          (8)
#define ISPOST_OVALPHA2_ALPHA9_DEFAULT         (0)
#define ISPOST_OVALPHA2_ALPHA9_SIGNED_FIELD    (0)

/*
 * ISPOST, OVALPHA2, ALPHA8
 * [7:0] ALPHA8 - Overlay image alpha value register. (IDX=0)
 *                Overlay image Color 2 CR value register. (IDX=1-7)
 *                This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVALPHA2_ALPHA8_MASK            (0x000000FF)
#define ISPOST_OVALPHA2_ALPHA8_LSBMASK         (0x000000FF)
#define ISPOST_OVALPHA2_ALPHA8_SHIFT           (0)
#define ISPOST_OVALPHA2_ALPHA8_LENGTH          (8)
#define ISPOST_OVALPHA2_ALPHA8_DEFAULT         (0)
#define ISPOST_OVALPHA2_ALPHA8_SIGNED_FIELD    (0)


//----------------------------------------------------------
// OVALPHA3 - OV Overlay Alpha Value 3 Register.
#define ISPOST_OVALPHA3_OFFSET                 (0x0084)
#define ISPOST_OVALPHA3                        (ISPOST_PERI_BASE_ADDR + ISPOST_OVALPHA3_OFFSET)

/*
 * ISPOST, OVALPHA3, ALPHA15
 * [31:24] ALPHA15 - Overlay image alpha value register. (IDX=0)
 *                   Overlay image Color 3 Alpha value register. (IDX=1-7)
 *                   This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVALPHA3_ALPHA15_MASK           (0xFF000000)
#define ISPOST_OVALPHA3_ALPHA15_LSBMASK        (0x000000FF)
#define ISPOST_OVALPHA3_ALPHA15_SHIFT          (24)
#define ISPOST_OVALPHA3_ALPHA15_LENGTH         (8)
#define ISPOST_OVALPHA3_ALPHA15_DEFAULT        (0)
#define ISPOST_OVALPHA3_ALPHA15_SIGNED_FIELD   (0)

/*
 * ISPOST, OVALPHA3, ALPHA14
 * [23:16] ALPHA14 - Overlay image alpha value register. (IDX=0)
 *                   Overlay image Color 3 Y value register. (IDX=1-7)
 *                   This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVALPHA3_ALPHA14_MASK           (0x00FF0000)
#define ISPOST_OVALPHA3_ALPHA14_LSBMASK        (0x000000FF)
#define ISPOST_OVALPHA3_ALPHA14_SHIFT          (16)
#define ISPOST_OVALPHA3_ALPHA14_LENGTH         (8)
#define ISPOST_OVALPHA3_ALPHA14_DEFAULT        (0)
#define ISPOST_OVALPHA3_ALPHA14_SIGNED_FIELD   (0)

/*
 * ISPOST, OVALPHA3, ALPHA13
 * [15:8] ALPHA13 - Overlay image alpha value register. (IDX=0)
 *                  Overlay image Color 3 CB value register. (IDX=1-7)
 *                  This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVALPHA3_ALPHA13_MASK           (0x0000FF00)
#define ISPOST_OVALPHA3_ALPHA13_LSBMASK        (0x000000FF)
#define ISPOST_OVALPHA3_ALPHA13_SHIFT          (8)
#define ISPOST_OVALPHA3_ALPHA13_LENGTH         (8)
#define ISPOST_OVALPHA3_ALPHA13_DEFAULT        (0)
#define ISPOST_OVALPHA3_ALPHA13_SIGNED_FIELD   (0)

/*
 * ISPOST, OVALPHA3, ALPHA12
 * [7:0] ALPHA12 - Overlay image alpha value register. (IDX=0)
 *                 Overlay image Color 3 CR value register. (IDX=1-7)
 *                 This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 */
#define ISPOST_OVALPHA3_ALPHA12_MASK           (0x000000FF)
#define ISPOST_OVALPHA3_ALPHA12_LSBMASK        (0x000000FF)
#define ISPOST_OVALPHA3_ALPHA12_SHIFT          (0)
#define ISPOST_OVALPHA3_ALPHA12_LENGTH         (8)
#define ISPOST_OVALPHA3_ALPHA12_DEFAULT        (0)
#define ISPOST_OVALPHA3_ALPHA12_SIGNED_FIELD   (0)


//----------------------------------------------------------
// OVAXI - OV AXI Control Register.
#define ISPOST_OVAXI_OFFSET                    (0x0088)
#define ISPOST_OVAXI                           (ISPOST_PERI_BASE_ADDR + ISPOST_OVAXI_OFFSET)

/*
 * ISPOST, OVAXI, OVID
 * [7:0] OVID - OV Image AXI Read ID.
 *              This register is indexed by OVIDX[IDX] (08CH). Valid index values: 0-7.
 * (Please note: ALL AXI Read Ids must be unique).
 */
#define ISPOST_OVAXI_OVID_MASK                 (0x000000FF)
#define ISPOST_OVAXI_OVID_LSBMASK              (0x000000FF)
#define ISPOST_OVAXI_OVID_SHIFT                (0)
#define ISPOST_OVAXI_OVID_LENGTH               (8)
#define ISPOST_OVAXI_OVID_DEFAULT              (0)
#define ISPOST_OVAXI_OVID_SIGNED_FIELD         (0)


//----------------------------------------------------------
// OVIDX - OV Index Register.
#define ISPOST_OVIDX_OFFSET                    (0x008C)
#define ISPOST_OVIDX                           (ISPOST_PERI_BASE_ADDR + ISPOST_OVIDX_OFFSET)

/*
 * ISPOST, OVIDX, IDX
 * [2:0] IDX - OV Index.
 */
#define ISPOST_OVIDX_IDX_MASK                  (0x00000007)
#define ISPOST_OVIDX_IDX_LSBMASK               (0x00000007)
#define ISPOST_OVIDX_IDX_SHIFT                 (0)
#define ISPOST_OVIDX_IDX_LENGTH                (3)
#define ISPOST_OVIDX_IDX_DEFAULT               (0)
#define ISPOST_OVIDX_IDX_SIGNED_FIELD          (0)


//----------------------------------------------------------
// OVBC - OV Overlay Background Color Register.
#define ISPOST_OVBC_OFFSET                     (0x0090)
#define ISPOST_OVBC                            (ISPOST_PERI_BASE_ADDR + ISPOST_OVBC_OFFSET)

/*
 * ISPOST, OVBC, Y
 * [23:16] Y - Backgound Y component value. (0-255).
 */
#define ISPOST_OVBC_Y_MASK                     (0x00FF0000)
#define ISPOST_OVBC_Y_LSBMASK                  (0x000000FF)
#define ISPOST_OVBC_Y_SHIFT                    (16)
#define ISPOST_OVBC_Y_LENGTH                   (8)
#define ISPOST_OVBC_Y_DEFAULT                  (0)
#define ISPOST_OVBC_Y_SIGNED_FIELD             (0)

/*
 * ISPOST, OVBC, CB
 * [15:8] CB - Backgound CB component value. (0-255).
 */
#define ISPOST_OVBC_CB_MASK                    (0x0000FF00)
#define ISPOST_OVBC_CB_LSBMASK                 (0x000000FF)
#define ISPOST_OVBC_CB_SHIFT                   (8)
#define ISPOST_OVBC_CB_LENGTH                  (8)
#define ISPOST_OVBC_CB_DEFAULT                 (0)
#define ISPOST_OVBC_CB_SIGNED_FIELD            (0)

/*
 * ISPOST, OVBC, CR
 * [7:0] CR - Backgound CR component value. (0-255).
 */
#define ISPOST_OVBC_CR_MASK                    (0x000000FF)
#define ISPOST_OVBC_CR_LSBMASK                 (0x000000FF)
#define ISPOST_OVBC_CR_SHIFT                   (0)
#define ISPOST_OVBC_CR_LENGTH                  (8)
#define ISPOST_OVBC_CR_DEFAULT                 (0)
#define ISPOST_OVBC_CR_SIGNED_FIELD            (0)



//==========================================================
// Unscaled output interface registers offset.
//----------------------------------------------------------
// UOAY - Unscaled Output Image Y Plane Start Address Register.
#define ISPOST_UOAY_OFFSET                     (0x00AC)
#define ISPOST_UOAY                            (ISPOST_PERI_BASE_ADDR + ISPOST_UOAY_OFFSET)

/*
 * ISPOST, UOAY, SAD
 * [31:3] SAD - Base address in QWORD (64bit) boundary.
 */
#define ISPOST_UOAY_SAD_MASK                   (0xFFFFFFF8)
#define ISPOST_UOAY_SAD_LSBMASK                (0xFFFFFFF8)
#define ISPOST_UOAY_SAD_SHIFT                  (0)
#define ISPOST_UOAY_SAD_LENGTH                 (32)
#define ISPOST_UOAY_SAD_DEFAULT                (0)
#define ISPOST_UOAY_SAD_SIGNED_FIELD           (0)


//----------------------------------------------------------
// UOAUV - Unscaled Output Image UV Plane Start Address Register.
#define ISPOST_UOAUV_OFFSET                    (0x00B0)
#define ISPOST_UOAUV                           (ISPOST_PERI_BASE_ADDR + ISPOST_UOAUV_OFFSET)

/*
 * ISPOST, UOAUV, SAD
 * [31:3] SAD - Base address in QWORD (64bit) boundary.
 */
#define ISPOST_UOAUV_SAD_MASK                  (0xFFFFFFF8)
#define ISPOST_UOAUV_SAD_LSBMASK               (0xFFFFFFF8)
#define ISPOST_UOAUV_SAD_SHIFT                 (0)
#define ISPOST_UOAUV_SAD_LENGTH                (32)
#define ISPOST_UOAUV_SAD_DEFAULT               (0)
#define ISPOST_UOAUV_SAD_SIGNED_FIELD          (0)


//----------------------------------------------------------
// UOS - Unscaled Output Image Stride Register.
#define ISPOST_UOS_OFFSET                      (0x00B4)
#define ISPOST_UOS                             (ISPOST_PERI_BASE_ADDR + ISPOST_UOS_OFFSET)

/*
 * ISPOST, UOS, SD
 * [12:3] SD - Image stride (QWORD granularity).
 */
#define ISPOST_UOS_SD_MASK                     (0x00001FF8)
#define ISPOST_UOS_SD_LSBMASK                  (0x00001FF8)
#define ISPOST_UOS_SD_SHIFT                    (0)
#define ISPOST_UOS_SD_LENGTH                   (13)
#define ISPOST_UOS_SD_DEFAULT                  (0)
#define ISPOST_UOS_SD_SIGNED_FIELD             (0)


//----------------------------------------------------------
// UOPGM - Unscaled Output Pixel Coordinate Generator Mode Register.
#define ISPOST_UOPGM_OFFSET                    (0x0270)
#define ISPOST_UOPGM                           (ISPOST_PERI_BASE_ADDR + ISPOST_UOPGM_OFFSET)

/*
 * ISPOST, UOPGM, SCANH
 * [5:4] SCANH - scan height.
 *               00 : 8.
 *               10 : 16.
 *               01 : 32.
 *               11 : 2.
 *               NOTE: only scan height 16 is compatible with direct link to video codec
 */
#define ISPOST_UOPGM_SCANH_MASK                (0x00000030)
#define ISPOST_UOPGM_SCANH_LSBMASK             (0x00000003)
#define ISPOST_UOPGM_SCANH_SHIFT               (4)
#define ISPOST_UOPGM_SCANH_LENGTH              (2)
#define ISPOST_UOPGM_SCANH_DEFAULT             (0)
#define ISPOST_UOPGM_SCANH_SIGNED_FIELD        (0)


//----------------------------------------------------------
// UOAXI - Unscaled Output AXI Control Register.
#define ISPOST_UOAXI_OFFSET                    (0x0274)
#define ISPOST_UOAXI                           (ISPOST_PERI_BASE_ADDR + ISPOST_UOAXI_OFFSET)

/*
 * ISPOST, UOAXI, REFWID
 * [15:8] REFWID - Reference Image AXI Write ID.
 */
#define ISPOST_UOAXI_REFWID_MASK               (0x0000FF00)
#define ISPOST_UOAXI_REFWID_LSBMASK            (0x000000FF)
#define ISPOST_UOAXI_REFWID_SHIFT              (8)
#define ISPOST_UOAXI_REFWID_LENGTH             (8)
#define ISPOST_UOAXI_REFWID_DEFAULT            (0)
#define ISPOST_UOAXI_REFWID_SIGNED_FIELD       (0)



//==========================================================
// 3D Denoise registers offset.
//----------------------------------------------------------
// DNRIAY - DN Reference Input Image Y Plane Start Address Register.
#define ISPOST_DNRIAY_OFFSET                   (0x00A0)
#define ISPOST_DNRIAY                          (ISPOST_PERI_BASE_ADDR + ISPOST_DNRIAY_OFFSET)

/*
 * ISPOST, DNRIAY, SAD
 * [31:3] SAD - Base address in QWORD (64bit) boundary.
 */
#define ISPOST_DNRIAY_SAD_MASK                 (0xFFFFFFF8)
#define ISPOST_DNRIAY_SAD_LSBMASK              (0xFFFFFFF8)
#define ISPOST_DNRIAY_SAD_SHIFT                (0)
#define ISPOST_DNRIAY_SAD_LENGTH               (32)
#define ISPOST_DNRIAY_SAD_DEFAULT              (0)
#define ISPOST_DNRIAY_SAD_SIGNED_FIELD         (0)


//----------------------------------------------------------
// DNRIAUV - DN Reference Input Image UV Plane Start Address Register.
#define ISPOST_DNRIAUV_OFFSET                  (0x00A4)
#define ISPOST_DNRIAUV                         (ISPOST_PERI_BASE_ADDR + ISPOST_DNRIAUV_OFFSET)

/*
 * ISPOST, DNRIAUV, SAD
 * [31:3] SAD - Base address in QWORD (64bit) boundary.
 */
#define ISPOST_DNRIAUV_SAD_MASK                (0xFFFFFFF8)
#define ISPOST_DNRIAUV_SAD_LSBMASK             (0xFFFFFFF8)
#define ISPOST_DNRIAUV_SAD_SHIFT               (0)
#define ISPOST_DNRIAUV_SAD_LENGTH              (32)
#define ISPOST_DNRIAUV_SAD_DEFAULT             (0)
#define ISPOST_DNRIAUV_SAD_SIGNED_FIELD        (0)


//----------------------------------------------------------
// DNRIS - DN Reference Input Image Stride Register.
#define ISPOST_DNRIS_OFFSET                    (0x00A8)
#define ISPOST_DNRIS                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNRIS_OFFSET)

/*
 * ISPOST, DNRIS, SD
 * [12:3] SD - Image stride (QWORD granularity).
 */
#define ISPOST_DNRIS_SD_MASK                   (0x00001FF8)
#define ISPOST_DNRIS_SD_LSBMASK                (0x00001FF8)
#define ISPOST_DNRIS_SD_SHIFT                  (0)
#define ISPOST_DNRIS_SD_LENGTH                 (13)
#define ISPOST_DNRIS_SD_DEFAULT                (0)
#define ISPOST_DNRIS_SD_SIGNED_FIELD           (0)


//----------------------------------------------------------
// DNROAY - DN Reference Output Image Y Plane Start Address Register.
#define ISPOST_DNROAY_OFFSET                   (0x0280)
#define ISPOST_DNROAY                          (ISPOST_PERI_BASE_ADDR + ISPOST_DNROAY_OFFSET)

/*
 * ISPOST, DNROAY, SAD
 * [31:3] SAD - Base address in QWORD (64bit) boundary.
 */
#define ISPOST_DNROAY_SAD_MASK                 (0xFFFFFFF8)
#define ISPOST_DNROAY_SAD_LSBMASK              (0xFFFFFFF8)
#define ISPOST_DNROAY_SAD_SHIFT                (0)
#define ISPOST_DNROAY_SAD_LENGTH               (32)
#define ISPOST_DNROAY_SAD_DEFAULT              (0)
#define ISPOST_DNROAY_SAD_SIGNED_FIELD         (0)


//----------------------------------------------------------
// DNROAUV - DN Reference Output Image UV Plane Start Address Register.
#define ISPOST_DNROAUV_OFFSET                  (0x0284)
#define ISPOST_DNROAUV                         (ISPOST_PERI_BASE_ADDR + ISPOST_DNROAUV_OFFSET)

/*
 * ISPOST, DNROAUV, SAD
 * [31:3] SAD - Base address in QWORD (64bit) boundary.
 */
#define ISPOST_DNROAUV_SAD_MASK                (0xFFFFFFF8)
#define ISPOST_DNROAUV_SAD_LSBMASK             (0xFFFFFFF8)
#define ISPOST_DNROAUV_SAD_SHIFT               (0)
#define ISPOST_DNROAUV_SAD_LENGTH              (32)
#define ISPOST_DNROAUV_SAD_DEFAULT             (0)
#define ISPOST_DNROAUV_SAD_SIGNED_FIELD        (0)


//----------------------------------------------------------
// DNROS - DN Reference Output Image Stride Register.
#define ISPOST_DNROS_OFFSET                    (0x0288)
#define ISPOST_DNROS                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNROS_OFFSET)

/*
 * ISPOST, DNROS, SD
 * [12:3] SD - Image stride (QWORD granularity).
 */
#define ISPOST_DNROS_SD_MASK                   (0x00001FF8)
#define ISPOST_DNROS_SD_LSBMASK                (0x00001FF8)
#define ISPOST_DNROS_SD_SHIFT                  (0)
#define ISPOST_DNROS_SD_LENGTH                 (13)
#define ISPOST_DNROS_SD_DEFAULT                (0)
#define ISPOST_DNROS_SD_SIGNED_FIELD           (0)


//----------------------------------------------------------
// DNMC0 - DN Mask Curve 0 Register.
#define ISPOST_DNMC0_OFFSET                    (0x00B8)
#define ISPOST_DNMC0                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC0_OFFSET)

/*
 * ISPOST, DNMC0, S
 * [31:20] S - Slope of mask value for mask curve. (format 0.0.12).
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC0_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC0_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC0_S_SHIFT                   (20)
#define ISPOST_DNMC0_S_LENGTH                  (12)
#define ISPOST_DNMC0_S_DEFAULT                 (0)
#define ISPOST_DNMC0_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC0, R
 * [18:16] R - Absolute difference value range exponent: RANGE = 2^R.
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC0_R_MASK                    (0x00070000)
#define ISPOST_DNMC0_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC0_R_SHIFT                   (16)
#define ISPOST_DNMC0_R_LENGTH                  (3)
#define ISPOST_DNMC0_R_DEFAULT                 (0)
#define ISPOST_DNMC0_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC0, Y
 * [15:8] Y - Mask value for mask curve. (format 0.0.8).
 *            This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC0_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC0_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC0_Y_SHIFT                   (8)
#define ISPOST_DNMC0_Y_LENGTH                  (8)
#define ISPOST_DNMC0_Y_DEFAULT                 (0)
#define ISPOST_DNMC0_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC0, X
 * [7:0] X - Absolute difference value for mask curve. (format 0.8.0).
 *           This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
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


//----------------------------------------------------------
// DNMC1 - DN Mask Curve 1 Register.
#define ISPOST_DNMC1_OFFSET                    (0x00BC)
#define ISPOST_DNMC1                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC1_OFFSET)

/*
 * ISPOST, DNMC1, S
 * [31:20] S - Slope of mask value for mask curve. (format 0.0.12).
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC1_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC1_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC1_S_SHIFT                   (20)
#define ISPOST_DNMC1_S_LENGTH                  (12)
#define ISPOST_DNMC1_S_DEFAULT                 (0)
#define ISPOST_DNMC1_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC1, R
 * [18:16] R - Absolute difference value range exponent: RANGE = 2^R.
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC1_R_MASK                    (0x00070000)
#define ISPOST_DNMC1_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC1_R_SHIFT                   (16)
#define ISPOST_DNMC1_R_LENGTH                  (3)
#define ISPOST_DNMC1_R_DEFAULT                 (0)
#define ISPOST_DNMC1_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC1, Y
 * [15:8] Y - Mask value for mask curve. (format 0.0.8).
 *            This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC1_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC1_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC1_Y_SHIFT                   (8)
#define ISPOST_DNMC1_Y_LENGTH                  (8)
#define ISPOST_DNMC1_Y_DEFAULT                 (0)
#define ISPOST_DNMC1_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC1, X
 * [7:0] X - Absolute difference value for mask curve. (format 0.8.0).
 *           This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
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


//----------------------------------------------------------
// DNMC2 - DN Mask Curve 2 Register.
#define ISPOST_DNMC2_OFFSET                    (0x00C0)
#define ISPOST_DNMC2                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC2_OFFSET)

/*
 * ISPOST, DNMC2, S
 * [31:20] S - Slope of mask value for mask curve. (format 0.0.12).
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC2_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC2_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC2_S_SHIFT                   (20)
#define ISPOST_DNMC2_S_LENGTH                  (12)
#define ISPOST_DNMC2_S_DEFAULT                 (0)
#define ISPOST_DNMC2_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC2, R
 * [18:16] R - Absolute difference value range exponent: RANGE = 2^R.
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC2_R_MASK                    (0x00070000)
#define ISPOST_DNMC2_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC2_R_SHIFT                   (16)
#define ISPOST_DNMC2_R_LENGTH                  (3)
#define ISPOST_DNMC2_R_DEFAULT                 (0)
#define ISPOST_DNMC2_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC2, Y
 * [15:8] Y - Mask value for mask curve. (format 0.0.8).
 *            This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC2_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC2_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC2_Y_SHIFT                   (8)
#define ISPOST_DNMC2_Y_LENGTH                  (8)
#define ISPOST_DNMC2_Y_DEFAULT                 (0)
#define ISPOST_DNMC2_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC2, X
 * [7:0] X - Absolute difference value for mask curve. (format 0.8.0).
 *           This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
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


//----------------------------------------------------------
// DNMC3 - DN Mask Curve 3 Register.
#define ISPOST_DNMC3_OFFSET                    (0x00C4)
#define ISPOST_DNMC3                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC3_OFFSET)

/*
 * ISPOST, DNMC3, S
 * [31:20] S - Slope of mask value for mask curve. (format 0.0.12).
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC3_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC3_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC3_S_SHIFT                   (20)
#define ISPOST_DNMC3_S_LENGTH                  (12)
#define ISPOST_DNMC3_S_DEFAULT                 (0)
#define ISPOST_DNMC3_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC3, R
 * [18:16] R - Absolute difference value range exponent: RANGE = 2^R.
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC3_R_MASK                    (0x00070000)
#define ISPOST_DNMC3_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC3_R_SHIFT                   (16)
#define ISPOST_DNMC3_R_LENGTH                  (3)
#define ISPOST_DNMC3_R_DEFAULT                 (0)
#define ISPOST_DNMC3_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC3, Y
 * [15:8] Y - Mask value for mask curve. (format 0.0.8).
 *            This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC3_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC3_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC3_Y_SHIFT                   (8)
#define ISPOST_DNMC3_Y_LENGTH                  (8)
#define ISPOST_DNMC3_Y_DEFAULT                 (0)
#define ISPOST_DNMC3_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC3, X
 * [7:0] X - Absolute difference value for mask curve. (format 0.8.0).
 *           This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
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


//----------------------------------------------------------
// DNMC4 - DN Mask Curve 4 Register.
#define ISPOST_DNMC4_OFFSET                    (0x00C8)
#define ISPOST_DNMC4                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC4_OFFSET)

/*
 * ISPOST, DNMC4, S
 * [31:20] S - Slope of mask value for mask curve. (format 0.0.12).
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC4_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC4_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC4_S_SHIFT                   (20)
#define ISPOST_DNMC4_S_LENGTH                  (12)
#define ISPOST_DNMC4_S_DEFAULT                 (0)
#define ISPOST_DNMC4_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC4, R
 * [18:16] R - Absolute difference value range exponent: RANGE = 2^R.
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC4_R_MASK                    (0x00070000)
#define ISPOST_DNMC4_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC4_R_SHIFT                   (16)
#define ISPOST_DNMC4_R_LENGTH                  (3)
#define ISPOST_DNMC4_R_DEFAULT                 (0)
#define ISPOST_DNMC4_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC4, Y
 * [15:8] Y - Mask value for mask curve. (format 0.0.8).
 *            This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC4_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC4_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC4_Y_SHIFT                   (8)
#define ISPOST_DNMC4_Y_LENGTH                  (8)
#define ISPOST_DNMC4_Y_DEFAULT                 (0)
#define ISPOST_DNMC4_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC4, X
 * [7:0] X - Absolute difference value for mask curve. (format 0.8.0).
 *           This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
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


//----------------------------------------------------------
// DNMC5 - DN Mask Curve 5 Register.
#define ISPOST_DNMC5_OFFSET                    (0x00CC)
#define ISPOST_DNMC5                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC5_OFFSET)

/*
 * ISPOST, DNMC5, S
 * [31:20] S - Slope of mask value for mask curve. (format 0.0.12).
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC5_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC5_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC5_S_SHIFT                   (20)
#define ISPOST_DNMC5_S_LENGTH                  (12)
#define ISPOST_DNMC5_S_DEFAULT                 (0)
#define ISPOST_DNMC5_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC5, R
 * [18:16] R - Absolute difference value range exponent: RANGE = 2^R.
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC5_R_MASK                    (0x00070000)
#define ISPOST_DNMC5_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC5_R_SHIFT                   (16)
#define ISPOST_DNMC5_R_LENGTH                  (3)
#define ISPOST_DNMC5_R_DEFAULT                 (0)
#define ISPOST_DNMC5_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC5, Y
 * [15:8] Y - Mask value for mask curve. (format 0.0.8).
 *            This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC5_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC5_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC5_Y_SHIFT                   (8)
#define ISPOST_DNMC5_Y_LENGTH                  (8)
#define ISPOST_DNMC5_Y_DEFAULT                 (0)
#define ISPOST_DNMC5_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC5, X
 * [7:0] X - Absolute difference value for mask curve. (format 0.8.0).
 *           This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
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


//----------------------------------------------------------
// DNMC6 - DN Mask Curve 6 Register.
#define ISPOST_DNMC6_OFFSET                    (0x00D0)
#define ISPOST_DNMC6                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC6_OFFSET)

/*
 * ISPOST, DNMC6, S
 * [31:20] S - Slope of mask value for mask curve. (format 0.0.12).
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC6_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC6_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC6_S_SHIFT                   (20)
#define ISPOST_DNMC6_S_LENGTH                  (12)
#define ISPOST_DNMC6_S_DEFAULT                 (0)
#define ISPOST_DNMC6_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC6, R
 * [18:16] R - Absolute difference value range exponent: RANGE = 2^R.
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC6_R_MASK                    (0x00070000)
#define ISPOST_DNMC6_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC6_R_SHIFT                   (16)
#define ISPOST_DNMC6_R_LENGTH                  (3)
#define ISPOST_DNMC6_R_DEFAULT                 (0)
#define ISPOST_DNMC6_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC6, Y
 * [15:8] Y - Mask value for mask curve. (format 0.0.8).
 *            This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC6_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC6_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC6_Y_SHIFT                   (8)
#define ISPOST_DNMC6_Y_LENGTH                  (8)
#define ISPOST_DNMC6_Y_DEFAULT                 (0)
#define ISPOST_DNMC6_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC6, X
 * [7:0] X - Absolute difference value for mask curve. (format 0.8.0).
 *           This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
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


//----------------------------------------------------------
// DNMC7 - DN Mask Curve 7 Register.
#define ISPOST_DNMC7_OFFSET                    (0x00D4)
#define ISPOST_DNMC7                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC7_OFFSET)

/*
 * ISPOST, DNMC7, S
 * [31:20] S - Slope of mask value for mask curve. (format 0.0.12).
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC7_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC7_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC7_S_SHIFT                   (20)
#define ISPOST_DNMC7_S_LENGTH                  (12)
#define ISPOST_DNMC7_S_DEFAULT                 (0)
#define ISPOST_DNMC7_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC7, R
 * [18:16] R - Absolute difference value range exponent: RANGE = 2^R.
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC7_R_MASK                    (0x00070000)
#define ISPOST_DNMC7_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC7_R_SHIFT                   (16)
#define ISPOST_DNMC7_R_LENGTH                  (3)
#define ISPOST_DNMC7_R_DEFAULT                 (0)
#define ISPOST_DNMC7_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC7, Y
 * [15:8] Y - Mask value for mask curve. (format 0.0.8).
 *            This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC7_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC7_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC7_Y_SHIFT                   (8)
#define ISPOST_DNMC7_Y_LENGTH                  (8)
#define ISPOST_DNMC7_Y_DEFAULT                 (0)
#define ISPOST_DNMC7_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC7, X
 * [7:0] X - Absolute difference value for mask curve. (format 0.8.0).
 *           This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
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


//----------------------------------------------------------
// DNMC8 - DN Mask Curve 8 Register.
#define ISPOST_DNMC8_OFFSET                    (0x00D8)
#define ISPOST_DNMC8                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC8_OFFSET)

/*
 * ISPOST, DNMC8, S
 * [31:20] S - Slope of mask value for mask curve. (format 0.0.12).
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC8_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC8_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC8_S_SHIFT                   (20)
#define ISPOST_DNMC8_S_LENGTH                  (12)
#define ISPOST_DNMC8_S_DEFAULT                 (0)
#define ISPOST_DNMC8_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC8, R
 * [18:16] R - Absolute difference value range exponent: RANGE = 2^R.
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC8_R_MASK                    (0x00070000)
#define ISPOST_DNMC8_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC8_R_SHIFT                   (16)
#define ISPOST_DNMC8_R_LENGTH                  (3)
#define ISPOST_DNMC8_R_DEFAULT                 (0)
#define ISPOST_DNMC8_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC8, Y
 * [15:8] Y - Mask value for mask curve. (format 0.0.8).
 *            This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC8_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC8_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC8_Y_SHIFT                   (8)
#define ISPOST_DNMC8_Y_LENGTH                  (8)
#define ISPOST_DNMC8_Y_DEFAULT                 (0)
#define ISPOST_DNMC8_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC8, X
 * [7:0] X - Absolute difference value for mask curve. (format 0.8.0).
 *           This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
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


//----------------------------------------------------------
// DNMC9 - DN Mask Curve 9 Register.
#define ISPOST_DNMC9_OFFSET                    (0x00DC)
#define ISPOST_DNMC9                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC9_OFFSET)

/*
 * ISPOST, DNMC9, S
 * [31:20] S - Slope of mask value for mask curve. (format 0.0.12).
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC9_S_MASK                    (0xFFF00000)
#define ISPOST_DNMC9_S_LSBMASK                 (0x00000FFF)
#define ISPOST_DNMC9_S_SHIFT                   (20)
#define ISPOST_DNMC9_S_LENGTH                  (12)
#define ISPOST_DNMC9_S_DEFAULT                 (0)
#define ISPOST_DNMC9_S_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC9, R
 * [18:16] R - Absolute difference value range exponent: RANGE = 2^R.
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC9_R_MASK                    (0x00070000)
#define ISPOST_DNMC9_R_LSBMASK                 (0x00000007)
#define ISPOST_DNMC9_R_SHIFT                   (16)
#define ISPOST_DNMC9_R_LENGTH                  (3)
#define ISPOST_DNMC9_R_DEFAULT                 (0)
#define ISPOST_DNMC9_R_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC9, Y
 * [15:8] Y - Mask value for mask curve. (format 0.0.8).
 *            This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC9_Y_MASK                    (0x0000FF00)
#define ISPOST_DNMC9_Y_LSBMASK                 (0x000000FF)
#define ISPOST_DNMC9_Y_SHIFT                   (8)
#define ISPOST_DNMC9_Y_LENGTH                  (8)
#define ISPOST_DNMC9_Y_DEFAULT                 (0)
#define ISPOST_DNMC9_Y_SIGNED_FIELD            (0)

/*
 * ISPOST, DNMC9, X
 * [7:0] X - Absolute difference value for mask curve. (format 0.8.0).
 *           This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
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


//----------------------------------------------------------
// DNMC10 - DN Mask Curve 10 Register.
#define ISPOST_DNMC10_OFFSET                   (0x00E0)
#define ISPOST_DNMC10                          (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC10_OFFSET)

/*
 * ISPOST, DNMC10, S
 * [31:20] S - Slope of mask value for mask curve. (format 0.0.12).
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC10_S_MASK                   (0xFFF00000)
#define ISPOST_DNMC10_S_LSBMASK                (0x00000FFF)
#define ISPOST_DNMC10_S_SHIFT                  (20)
#define ISPOST_DNMC10_S_LENGTH                 (12)
#define ISPOST_DNMC10_S_DEFAULT                (0)
#define ISPOST_DNMC10_S_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC10, R
 * [18:16] R - Absolute difference value range exponent: RANGE = 2^R.
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC10_R_MASK                   (0x00070000)
#define ISPOST_DNMC10_R_LSBMASK                (0x00000007)
#define ISPOST_DNMC10_R_SHIFT                  (16)
#define ISPOST_DNMC10_R_LENGTH                 (3)
#define ISPOST_DNMC10_R_DEFAULT                (0)
#define ISPOST_DNMC10_R_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC10, Y
 * [15:8] Y - Mask value for mask curve. (format 0.0.8).
 *            This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC10_Y_MASK                   (0x0000FF00)
#define ISPOST_DNMC10_Y_LSBMASK                (0x000000FF)
#define ISPOST_DNMC10_Y_SHIFT                  (8)
#define ISPOST_DNMC10_Y_LENGTH                 (8)
#define ISPOST_DNMC10_Y_DEFAULT                (0)
#define ISPOST_DNMC10_Y_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC10, X
 * [7:0] X - Absolute difference value for mask curve. (format 0.8.0).
 *           This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
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


//----------------------------------------------------------
// DNMC11 - DN Mask Curve 11 Register.
#define ISPOST_DNMC11_OFFSET                   (0x00E4)
#define ISPOST_DNMC11                          (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC11_OFFSET)

/*
 * ISPOST, DNMC11, S
 * [31:20] S - Slope of mask value for mask curve. (format 0.0.12).
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC11_S_MASK                   (0xFFF00000)
#define ISPOST_DNMC11_S_LSBMASK                (0x00000FFF)
#define ISPOST_DNMC11_S_SHIFT                  (20)
#define ISPOST_DNMC11_S_LENGTH                 (12)
#define ISPOST_DNMC11_S_DEFAULT                (0)
#define ISPOST_DNMC11_S_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC11, R
 * [18:16] R - Absolute difference value range exponent: RANGE = 2^R.
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC11_R_MASK                   (0x00070000)
#define ISPOST_DNMC11_R_LSBMASK                (0x00000007)
#define ISPOST_DNMC11_R_SHIFT                  (16)
#define ISPOST_DNMC11_R_LENGTH                 (3)
#define ISPOST_DNMC11_R_DEFAULT                (0)
#define ISPOST_DNMC11_R_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC11, Y
 * [15:8] Y - Mask value for mask curve. (format 0.0.8).
 *            This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC11_Y_MASK                   (0x0000FF00)
#define ISPOST_DNMC11_Y_LSBMASK                (0x000000FF)
#define ISPOST_DNMC11_Y_SHIFT                  (8)
#define ISPOST_DNMC11_Y_LENGTH                 (8)
#define ISPOST_DNMC11_Y_DEFAULT                (0)
#define ISPOST_DNMC11_Y_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC11, X
 * [7:0] X - Absolute difference value for mask curve. (format 0.8.0).
 *           This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
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


//----------------------------------------------------------
// DNMC12 - DN Mask Curve 12 Register.
#define ISPOST_DNMC12_OFFSET                   (0x00E8)
#define ISPOST_DNMC12                          (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC12_OFFSET)

/*
 * ISPOST, DNMC12, S
 * [31:20] S - Slope of mask value for mask curve. (format 0.0.12).
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC12_S_MASK                   (0xFFF00000)
#define ISPOST_DNMC12_S_LSBMASK                (0x00000FFF)
#define ISPOST_DNMC12_S_SHIFT                  (20)
#define ISPOST_DNMC12_S_LENGTH                 (12)
#define ISPOST_DNMC12_S_DEFAULT                (0)
#define ISPOST_DNMC12_S_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC12, R
 * [18:16] R - Absolute difference value range exponent: RANGE = 2^R.
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC12_R_MASK                   (0x00070000)
#define ISPOST_DNMC12_R_LSBMASK                (0x00000007)
#define ISPOST_DNMC12_R_SHIFT                  (16)
#define ISPOST_DNMC12_R_LENGTH                 (3)
#define ISPOST_DNMC12_R_DEFAULT                (0)
#define ISPOST_DNMC12_R_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC12, Y
 * [15:8] Y - Mask value for mask curve. (format 0.0.8).
 *            This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC12_Y_MASK                   (0x0000FF00)
#define ISPOST_DNMC12_Y_LSBMASK                (0x000000FF)
#define ISPOST_DNMC12_Y_SHIFT                  (8)
#define ISPOST_DNMC12_Y_LENGTH                 (8)
#define ISPOST_DNMC12_Y_DEFAULT                (0)
#define ISPOST_DNMC12_Y_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC12, X
 * [7:0] X - Absolute difference value for mask curve. (format 0.8.0).
 *           This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
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


//----------------------------------------------------------
// DNMC13 - DN Mask Curve 13 Register.
#define ISPOST_DNMC13_OFFSET                   (0x00EC)
#define ISPOST_DNMC13                          (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC13_OFFSET)

/*
 * ISPOST, DNMC13, S
 * [31:20] S - Slope of mask value for mask curve. (format 0.0.12).
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC13_S_MASK                   (0xFFF00000)
#define ISPOST_DNMC13_S_LSBMASK                (0x00000FFF)
#define ISPOST_DNMC13_S_SHIFT                  (20)
#define ISPOST_DNMC13_S_LENGTH                 (12)
#define ISPOST_DNMC13_S_DEFAULT                (0)
#define ISPOST_DNMC13_S_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC13, R
 * [18:16] R - Absolute difference value range exponent: RANGE = 2^R.
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC13_R_MASK                   (0x00070000)
#define ISPOST_DNMC13_R_LSBMASK                (0x00000007)
#define ISPOST_DNMC13_R_SHIFT                  (16)
#define ISPOST_DNMC13_R_LENGTH                 (3)
#define ISPOST_DNMC13_R_DEFAULT                (0)
#define ISPOST_DNMC13_R_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC13, Y
 * [15:8] Y - Mask value for mask curve. (format 0.0.8).
 *            This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC13_Y_MASK                   (0x0000FF00)
#define ISPOST_DNMC13_Y_LSBMASK                (0x000000FF)
#define ISPOST_DNMC13_Y_SHIFT                  (8)
#define ISPOST_DNMC13_Y_LENGTH                 (8)
#define ISPOST_DNMC13_Y_DEFAULT                (0)
#define ISPOST_DNMC13_Y_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC13, X
 * [7:0] X - Absolute difference value for mask curve. (format 0.8.0).
 *           This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
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


//----------------------------------------------------------
// DNMC14 - DN Mask Curve 14 Register.
#define ISPOST_DNMC14_OFFSET                   (0x00F0)
#define ISPOST_DNMC14                          (ISPOST_PERI_BASE_ADDR + ISPOST_DNMC14_OFFSET)

/*
 * ISPOST, DNMC14, S
 * [31:20] S - Slope of mask value for mask curve. (format 0.0.12).
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC14_S_MASK                   (0xFFF00000)
#define ISPOST_DNMC14_S_LSBMASK                (0x00000FFF)
#define ISPOST_DNMC14_S_SHIFT                  (20)
#define ISPOST_DNMC14_S_LENGTH                 (12)
#define ISPOST_DNMC14_S_DEFAULT                (0)
#define ISPOST_DNMC14_S_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC14, R
 * [18:16] R - Absolute difference value range exponent: RANGE = 2^R.
 *             This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC14_R_MASK                   (0x00070000)
#define ISPOST_DNMC14_R_LSBMASK                (0x00000007)
#define ISPOST_DNMC14_R_SHIFT                  (16)
#define ISPOST_DNMC14_R_LENGTH                 (3)
#define ISPOST_DNMC14_R_DEFAULT                (0)
#define ISPOST_DNMC14_R_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC14, Y
 * [15:8] Y - Mask value for mask curve. (format 0.0.8).
 *            This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
 */
#define ISPOST_DNMC14_Y_MASK                   (0x0000FF00)
#define ISPOST_DNMC14_Y_LSBMASK                (0x000000FF)
#define ISPOST_DNMC14_Y_SHIFT                  (8)
#define ISPOST_DNMC14_Y_LENGTH                 (8)
#define ISPOST_DNMC14_Y_DEFAULT                (0)
#define ISPOST_DNMC14_Y_SIGNED_FIELD           (0)

/*
 * ISPOST, DNMC14, X
 * [7:0] X - Absolute difference value for mask curve. (format 0.8.0).
 *           This register is indexed by DNCTRL0[IDX] (0F8H). Valid index values: 0-2.
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


//----------------------------------------------------------
// DNCTRL0 - DN Control Register 0.
#define ISPOST_DNCTRL0_OFFSET                  (0x00F8)
#define ISPOST_DNCTRL0                         (ISPOST_PERI_BASE_ADDR + ISPOST_DNCTRL0_OFFSET)

/*
 * ISPOST, DNCTRL0, IDX
 * [25:24] IDX - DN mask curve index.
 *               0 : Y mask curve.
 *               1 : U mask curve.
 *               2 : V mask curve.
 *               3 : Reserved.
 */
#define ISPOST_DNCTRL0_IDX_MASK                (0x03000000)
#define ISPOST_DNCTRL0_IDX_LSBMASK             (0x00000003)
#define ISPOST_DNCTRL0_IDX_SHIFT               (24)
#define ISPOST_DNCTRL0_IDX_LENGTH              (2)
#define ISPOST_DNCTRL0_IDX_DEFAULT             (0)
#define ISPOST_DNCTRL0_IDX_SIGNED_FIELD        (0)


//----------------------------------------------------------
// DNAXI - DN AXI Control Register.
#define ISPOST_DNAXI_OFFSET                    (0x00FC)
#define ISPOST_DNAXI                           (ISPOST_PERI_BASE_ADDR + ISPOST_DNAXI_OFFSET)

/*
 * ISPOST, DNAXI, REFWID
 * [15:8] REFWID - Reference Image AXI Write ID.
 *                 (Please note: ALL AXI Read Ids must be unique).
 */
#define ISPOST_DNAXI_REFWID_MASK               (0x0000FF00)
#define ISPOST_DNAXI_REFWID_LSBMASK            (0x000000FF)
#define ISPOST_DNAXI_REFWID_SHIFT              (8)
#define ISPOST_DNAXI_REFWID_LENGTH             (8)
#define ISPOST_DNAXI_REFWID_DEFAULT            (0)
#define ISPOST_DNAXI_REFWID_SIGNED_FIELD       (0)

/*
 * ISPOST, DNAXI, REFRID
 * [7:0] REFRID - Reference Image AXI Read ID.
 *                (Please note: ALL AXI Read Ids must be unique).
 */
#define ISPOST_DNAXI_REFRID_MASK               (0x000000FF)
#define ISPOST_DNAXI_REFRID_LSBMASK            (0x000000FF)
#define ISPOST_DNAXI_REFRID_SHIFT              (0)
#define ISPOST_DNAXI_REFRID_LENGTH             (8)
#define ISPOST_DNAXI_REFRID_DEFAULT            (0)
#define ISPOST_DNAXI_REFRID_SIGNED_FIELD       (0)



//==========================================================
// Scaled output registers offset.
//----------------------------------------------------------
// SS0AY - SS0 Output Image Y Plane Start Address Register.
#define ISPOST_SS0AY_OFFSET                    (0x0100)
#define ISPOST_SS0AY                           (ISPOST_PERI_BASE_ADDR + ISPOST_SS0AY_OFFSET)

/*
 * ISPOST, SS0AY, SAD
 * [31:3] SAD - Base address in QWORD (64bit) boundary.
 */
#define ISPOST_SS0AY_SAD_MASK                  (0xFFFFFFF8)
#define ISPOST_SS0AY_SAD_LSBMASK               (0xFFFFFFF8)
#define ISPOST_SS0AY_SAD_SHIFT                 (0)
#define ISPOST_SS0AY_SAD_LENGTH                (32)
#define ISPOST_SS0AY_SAD_DEFAULT               (0)
#define ISPOST_SS0AY_SAD_SIGNED_FIELD          (0)


//----------------------------------------------------------
// SS0AUV - SS0 Output Image UV Plane Start Address Register.
#define ISPOST_SS0AUV_OFFSET                   (0x0104)
#define ISPOST_SS0AUV                          (ISPOST_PERI_BASE_ADDR + ISPOST_SS0AUV_OFFSET)

/*
 * ISPOST, SS0AUV, SAD
 * [31:3] SAD - Base address in QWORD (64bit) boundary.
 */
#define ISPOST_SS0AUV_SAD_MASK                 (0xFFFFFFF8)
#define ISPOST_SS0AUV_SAD_LSBMASK              (0xFFFFFFF8)
#define ISPOST_SS0AUV_SAD_SHIFT                (0)
#define ISPOST_SS0AUV_SAD_LENGTH               (32)
#define ISPOST_SS0AUV_SAD_DEFAULT              (0)
#define ISPOST_SS0AUV_SAD_SIGNED_FIELD         (0)


//----------------------------------------------------------
// SS0S - SS0 Output Image Stride Register.
#define ISPOST_SS0S_OFFSET                     (0x0108)
#define ISPOST_SS0S                            (ISPOST_PERI_BASE_ADDR + ISPOST_SS0S_OFFSET)

/*
 * ISPOST, SS0S, SD
 * [12:3] SD - Image stride (QWORD granularity).
 */
#define ISPOST_SS0S_SD_MASK                    (0x00001FF8)
#define ISPOST_SS0S_SD_LSBMASK                 (0x00001FF8)
#define ISPOST_SS0S_SD_SHIFT                   (0)
#define ISPOST_SS0S_SD_LENGTH                  (13)
#define ISPOST_SS0S_SD_DEFAULT                 (0)
#define ISPOST_SS0S_SD_SIGNED_FIELD            (0)


//----------------------------------------------------------
// SS0HF - SS0 H Scaling Factor Register.
#define ISPOST_SS0HF_OFFSET                    (0x010C)
#define ISPOST_SS0HF                           (ISPOST_PERI_BASE_ADDR + ISPOST_SS0HF_OFFSET)

/*
 * ISPOST, SS0HF, SF
 * [27:16] SF - Scaling factor (format 0.0.12).
 */
#define ISPOST_SS0HF_SF_MASK                   (0x0FFF0000)
#define ISPOST_SS0HF_SF_LSBMASK                (0x00000FFF)
#define ISPOST_SS0HF_SF_SHIFT                  (16)
#define ISPOST_SS0HF_SF_LENGTH                 (12)
#define ISPOST_SS0HF_SF_DEFAULT                (0)
#define ISPOST_SS0HF_SF_SIGNED_FIELD           (0)

/*
 * ISPOST, SS0HF, SM
 * [1:0] SM - scaling mode.
 *            00 : Scale down.
 *            01 : Reserved.
 *            10 : No scaling.
 *            11 : Reserved.
 */
#define ISPOST_SS0HF_SM_MASK                   (0x00000003)
#define ISPOST_SS0HF_SM_LSBMASK                (0x00000003)
#define ISPOST_SS0HF_SM_SHIFT                  (0)
#define ISPOST_SS0HF_SM_LENGTH                 (2)
#define ISPOST_SS0HF_SM_DEFAULT                (0)
#define ISPOST_SS0HF_SM_SIGNED_FIELD           (0)


//----------------------------------------------------------
// SS0VF - SS0 V Scaling Factor Register.
#define ISPOST_SS0VF_OFFSET                    (0x0110)
#define ISPOST_SS0VF                           (ISPOST_PERI_BASE_ADDR + ISPOST_SS0VF_OFFSET)

/*
 * ISPOST, SS0VF, SF
 * [27:16] SF - Scaling factor (format 0.0.12)  Minimum vertical scaling factor is x1/8.
 */
#define ISPOST_SS0VF_SF_MASK                   (0x0FFF0000)
#define ISPOST_SS0VF_SF_LSBMASK                (0x00000FFF)
#define ISPOST_SS0VF_SF_SHIFT                  (16)
#define ISPOST_SS0VF_SF_LENGTH                 (12)
#define ISPOST_SS0VF_SF_DEFAULT                (0)
#define ISPOST_SS0VF_SF_SIGNED_FIELD           (0)

/*
 * ISPOST, SS0VF, SM
 * [1:0] SM - scaling mode.
 *            00 : Scale down.
 *            01 : Reserved.
 *            10 : No scaling.
 *            11 : Reserved.
 */
#define ISPOST_SS0VF_SM_MASK                   (0x00000003)
#define ISPOST_SS0VF_SM_LSBMASK                (0x00000003)
#define ISPOST_SS0VF_SM_SHIFT                  (0)
#define ISPOST_SS0VF_SM_LENGTH                 (2)
#define ISPOST_SS0VF_SM_DEFAULT                (0)
#define ISPOST_SS0VF_SM_SIGNED_FIELD           (0)


//----------------------------------------------------------
// SS0IW - SS0 Output Image Size Register.
#define ISPOST_SS0IW_OFFSET                    (0x0114)
#define ISPOST_SS0IW                           (ISPOST_PERI_BASE_ADDR + ISPOST_SS0IW_OFFSET)

/*
 * ISPOST, SS0IW, W
 * [28:16] W - Output image width (pixel granularity).
 */
#define ISPOST_SS0IW_W_MASK                    (0x1FFF0000)
#define ISPOST_SS0IW_W_LSBMASK                 (0x00001FFF)
#define ISPOST_SS0IW_W_SHIFT                   (16)
#define ISPOST_SS0IW_W_LENGTH                  (13)
#define ISPOST_SS0IW_W_DEFAULT                 (0)
#define ISPOST_SS0IW_W_SIGNED_FIELD            (0)

/*
 * ISPOST, SS0IW, H
 * [12:0] H - Output image height (line granularity).
 */
#define ISPOST_SS0IW_H_MASK                    (0x00001FFF)
#define ISPOST_SS0IW_H_LSBMASK                 (0x00001FFF)
#define ISPOST_SS0IW_H_SHIFT                   (0)
#define ISPOST_SS0IW_H_LENGTH                  (13)
#define ISPOST_SS0IW_H_DEFAULT                 (0)
#define ISPOST_SS0IW_H_SIGNED_FIELD            (0)


//----------------------------------------------------------
// SS1AY - SS1 Output Image Y Plane Start Address Register.
#define ISPOST_SS1AY_OFFSET                    (0x0118)
#define ISPOST_SS1AY                           (ISPOST_PERI_BASE_ADDR + ISPOST_SS1AY_OFFSET)

/*
 * ISPOST, SS1AY, SAD
 * [31:3] SAD - Base address in QWORD (64bit) boundary.
 */
#define ISPOST_SS1AY_SAD_MASK                  (0xFFFFFFF8)
#define ISPOST_SS1AY_SAD_LSBMASK               (0xFFFFFFF8)
#define ISPOST_SS1AY_SAD_SHIFT                 (0)
#define ISPOST_SS1AY_SAD_LENGTH                (32)
#define ISPOST_SS1AY_SAD_DEFAULT               (0)
#define ISPOST_SS1AY_SAD_SIGNED_FIELD          (0)


//----------------------------------------------------------
// SS1AUV - SS1 Output Image UV Plane Start Address Register.
#define ISPOST_SS1AUV_OFFSET                   (0x011C)
#define ISPOST_SS1AUV                          (ISPOST_PERI_BASE_ADDR + ISPOST_SS1AUV_OFFSET)

/*
 * ISPOST, SS1AUV, SAD
 * [31:3] SAD - Base address in QWORD (64bit) boundary.
 */
#define ISPOST_SS1AUV_SAD_MASK                 (0xFFFFFFF8)
#define ISPOST_SS1AUV_SAD_LSBMASK              (0xFFFFFFF8)
#define ISPOST_SS1AUV_SAD_SHIFT                (0)
#define ISPOST_SS1AUV_SAD_LENGTH               (32)
#define ISPOST_SS1AUV_SAD_DEFAULT              (0)
#define ISPOST_SS1AUV_SAD_SIGNED_FIELD         (0)


//----------------------------------------------------------
// SS1S - SS1 Output Image Stride Register.
#define ISPOST_SS1S_OFFSET                     (0x0120)
#define ISPOST_SS1S                            (ISPOST_PERI_BASE_ADDR + ISPOST_SS1S_OFFSET)

/*
 * ISPOST, SS1S, SD
 * [12:3] SD - Image stride (QWORD granularity).
 */
#define ISPOST_SS1S_SD_MASK                    (0x00001FF8)
#define ISPOST_SS1S_SD_LSBMASK                 (0x00001FF8)
#define ISPOST_SS1S_SD_SHIFT                   (0)
#define ISPOST_SS1S_SD_LENGTH                  (13)
#define ISPOST_SS1S_SD_DEFAULT                 (0)
#define ISPOST_SS1S_SD_SIGNED_FIELD            (0)


//----------------------------------------------------------
// SS1HF - SS1 H Scaling Factor Register.
#define ISPOST_SS1HF_OFFSET                    (0x0124)
#define ISPOST_SS1HF                           (ISPOST_PERI_BASE_ADDR + ISPOST_SS1HF_OFFSET)

/*
 * ISPOST, SS1HF, SF
 * [27:16] SF - Scaling factor (format 0.0.12).
 */
#define ISPOST_SS1HF_SF_MASK                   (0x0FFF0000)
#define ISPOST_SS1HF_SF_LSBMASK                (0x00000FFF)
#define ISPOST_SS1HF_SF_SHIFT                  (16)
#define ISPOST_SS1HF_SF_LENGTH                 (12)
#define ISPOST_SS1HF_SF_DEFAULT                (0)
#define ISPOST_SS1HF_SF_SIGNED_FIELD           (0)

/*
 * ISPOST, SS1HF, SM
 * [1:0] SM - scaling mode.
 *            00 : Scale down.
 *            01 : Reserved.
 *            10 : No scaling.
 *            11 : Reserved.
 */
#define ISPOST_SS1HF_SM_MASK                   (0x00000003)
#define ISPOST_SS1HF_SM_LSBMASK                (0x00000003)
#define ISPOST_SS1HF_SM_SHIFT                  (0)
#define ISPOST_SS1HF_SM_LENGTH                 (2)
#define ISPOST_SS1HF_SM_DEFAULT                (0)
#define ISPOST_SS1HF_SM_SIGNED_FIELD           (0)


//----------------------------------------------------------
// SS1VF - SS1 V Scaling Factor Register.
#define ISPOST_SS1VF_OFFSET                    (0x0128)
#define ISPOST_SS1VF                           (ISPOST_PERI_BASE_ADDR + ISPOST_SS1VF_OFFSET)

/*
 * ISPOST, SS1VF, SF
 * [27:16] SF - Scaling factor (format 0.0.12)  Minimum vertical scaling factor is x1/8.
 */
#define ISPOST_SS1VF_SF_MASK                   (0x0FFF0000)
#define ISPOST_SS1VF_SF_LSBMASK                (0x00000FFF)
#define ISPOST_SS1VF_SF_SHIFT                  (16)
#define ISPOST_SS1VF_SF_LENGTH                 (12)
#define ISPOST_SS1VF_SF_DEFAULT                (0)
#define ISPOST_SS1VF_SF_SIGNED_FIELD           (0)

/*
 * ISPOST, SS1VF, SM
 * [1:0] SM - scaling mode.
 *            00 : Scale down.
 *            01 : Reserved.
 *            10 : No scaling.
 *            11 : Reserved.
 */
#define ISPOST_SS1VF_SM_MASK                   (0x00000003)
#define ISPOST_SS1VF_SM_LSBMASK                (0x00000003)
#define ISPOST_SS1VF_SM_SHIFT                  (0)
#define ISPOST_SS1VF_SM_LENGTH                 (2)
#define ISPOST_SS1VF_SM_DEFAULT                (0)
#define ISPOST_SS1VF_SM_SIGNED_FIELD           (0)


//----------------------------------------------------------
// SS1IW - SS1 Output Image Size Register.
#define ISPOST_SS1IW_OFFSET                    (0x012C)
#define ISPOST_SS1IW                           (ISPOST_PERI_BASE_ADDR + ISPOST_SS1IW_OFFSET)

/*
 * ISPOST, SS1IW, W
 * [28:16] W - Output image width (pixel granularity).
 */
#define ISPOST_SS1IW_W_MASK                    (0x1FFF0000)
#define ISPOST_SS1IW_W_LSBMASK                 (0x00001FFF)
#define ISPOST_SS1IW_W_SHIFT                   (16)
#define ISPOST_SS1IW_W_LENGTH                  (13)
#define ISPOST_SS1IW_W_DEFAULT                 (0)
#define ISPOST_SS1IW_W_SIGNED_FIELD            (0)

/*
 * ISPOST, SS1IW, H
 * [12:0] H - Output image height (line granularity).
 */
#define ISPOST_SS1IW_H_MASK                    (0x00001FFF)
#define ISPOST_SS1IW_H_LSBMASK                 (0x00001FFF)
#define ISPOST_SS1IW_H_SHIFT                   (0)
#define ISPOST_SS1IW_H_LENGTH                  (13)
#define ISPOST_SS1IW_H_DEFAULT                 (0)
#define ISPOST_SS1IW_H_SIGNED_FIELD            (0)


//----------------------------------------------------------
// SSAXI - SS AXI Control Register.
#define ISPOST_SSAXI_OFFSET                    (0x013C)
#define ISPOST_SSAXI                           (ISPOST_PERI_BASE_ADDR + ISPOST_SSAXI_OFFSET)

/*
 * ISPOST, SSAXI, SS1WID
 * [15:8] SS1WID - SS1 Image AXI Write ID.
 *                 (Please note: ALL AXI Read Ids must be unique).
 */
#define ISPOST_SSAXI_SS1WID_MASK               (0x0000FF00)
#define ISPOST_SSAXI_SS1WID_LSBMASK            (0x000000FF)
#define ISPOST_SSAXI_SS1WID_SHIFT              (8)
#define ISPOST_SSAXI_SS1WID_LENGTH             (8)
#define ISPOST_SSAXI_SS1WID_DEFAULT            (0)
#define ISPOST_SSAXI_SS1WID_SIGNED_FIELD       (0)


/*
 * ISPOST, SSAXI, SS0WID
 * [7:0] SS0WID - SS1 Image AXI Write ID.
 *               (Please note: ALL AXI Read Ids must be unique).
 */
#define ISPOST_SSAXI_SS0WID_MASK               (0x000000FF)
#define ISPOST_SSAXI_SS0WID_LSBMASK            (0x000000FF)
#define ISPOST_SSAXI_SS0WID_SHIFT              (0)
#define ISPOST_SSAXI_SS0WID_LENGTH             (8)
#define ISPOST_SSAXI_SS0WID_DEFAULT            (0)
#define ISPOST_SSAXI_SS0WID_SIGNED_FIELD       (0)



//==========================================================
// Video stabilization registers offset.
//----------------------------------------------------------
// VSCTRL0 - Video Stabilization Control Register 0.
#define ISPOST_VSCTRL0_OFFSET                  (0x0140)
#define ISPOST_VSCTRL0                         (ISPOST_PERI_BASE_ADDR + ISPOST_VSCTRL0_OFFSET)

/*
 * ISPOST, VSCTRL0, FLIPH
 * [5] FLIPH - Horizontal Flip enable.
 */
#define ISPOST_VSCTRL0_FLIPH_MASK              (0x00000020)
#define ISPOST_VSCTRL0_FLIPH_LSBMASK           (0x00000001)
#define ISPOST_VSCTRL0_FLIPH_SHIFT             (5)
#define ISPOST_VSCTRL0_FLIPH_LENGTH            (1)
#define ISPOST_VSCTRL0_FLIPH_DEFAULT           (0)
#define ISPOST_VSCTRL0_FLIPH_SIGNED_FIELD      (0)

/*
 * ISPOST, VSCTRL0, FLIPV
 * [4] FLIPV - Vertical Flip enable.
 */
#define ISPOST_VSCTRL0_FLIPV_MASK              (0x00000010)
#define ISPOST_VSCTRL0_FLIPV_LSBMASK           (0x00000001)
#define ISPOST_VSCTRL0_FLIPV_SHIFT             (4)
#define ISPOST_VSCTRL0_FLIPV_LENGTH            (1)
#define ISPOST_VSCTRL0_FLIPV_DEFAULT           (0)
#define ISPOST_VSCTRL0_FLIPV_SIGNED_FIELD      (0)

/*
 * ISPOST, VSCTRL0, ROT
 * [3:2] ROT - Rotation Mode.
 *             00 : No rotation.
 *             01 : Reserved.
 *             10 : Rotate 90deg.
 *             11 : Rotate 270deg.
 */
#define ISPOST_VSCTRL0_ROT_MASK                (0x0000000C)
#define ISPOST_VSCTRL0_ROT_LSBMASK             (0x00000003)
#define ISPOST_VSCTRL0_ROT_SHIFT               (2)
#define ISPOST_VSCTRL0_ROT_LENGTH              (2)
#define ISPOST_VSCTRL0_ROT_DEFAULT             (0)
#define ISPOST_VSCTRL0_ROT_SIGNED_FIELD        (0)

/*
 * ISPOST, VSCTRL0, REN
 * [0] REN - Rolling shutter compensation enable.
 */
#define ISPOST_VSCTRL0_REN_MASK                (0x00000001)
#define ISPOST_VSCTRL0_REN_LSBMASK             (0x00000001)
#define ISPOST_VSCTRL0_REN_SHIFT               (0)
#define ISPOST_VSCTRL0_REN_LENGTH              (1)
#define ISPOST_VSCTRL0_REN_DEFAULT             (0)
#define ISPOST_VSCTRL0_REN_SIGNED_FIELD        (0)


//----------------------------------------------------------
// VSWC0 - Video Stabilization Window Coordinate Register 0.
#define ISPOST_VSWC0_OFFSET                    (0x0150)
#define ISPOST_VSWC0                           (ISPOST_PERI_BASE_ADDR + ISPOST_VSWC0_OFFSET)

/*
 * ISPOST, VSWC0, COLX
 * [28:16] COLX - X coordinate of the upper left corner pixel of the first window in the column 0. (refer to figure below)
 */
#define ISPOST_VSWC0_COLX_MASK                 (0x1FFF0000)
#define ISPOST_VSWC0_COLX_LSBMASK              (0x00001FFF)
#define ISPOST_VSWC0_COLX_SHIFT                (16)
#define ISPOST_VSWC0_COLX_LENGTH               (13)
#define ISPOST_VSWC0_COLX_DEFAULT              (0)
#define ISPOST_VSWC0_COLX_SIGNED_FIELD         (0)

/*
 * ISPOST, VSWC0, ROWY
 * [12:0] ROWY - Y coordinate of the upper left corner of the first window in the row 0. (refer to figure below)
 */
#define ISPOST_VSWC0_ROWY_MASK                 (0x00001FFF)
#define ISPOST_VSWC0_ROWY_LSBMASK              (0x00001FFF)
#define ISPOST_VSWC0_ROWY_SHIFT                (0)
#define ISPOST_VSWC0_ROWY_LENGTH               (13)
#define ISPOST_VSWC0_ROWY_DEFAULT              (0)
#define ISPOST_VSWC0_ROWY_SIGNED_FIELD         (0)


//----------------------------------------------------------
// VSWC1 - Video Stabilization Window Coordinate Register 1.
#define ISPOST_VSWC1_OFFSET                    (0x0154)
#define ISPOST_VSWC1                           (ISPOST_PERI_BASE_ADDR + ISPOST_VSWC1_OFFSET)

/*
 * ISPOST, VSWC1, COLX
 * [28:16] COLX - X coordinate of the upper left corner pixel of the first window in the column 1. (refer to figure below)
 */
#define ISPOST_VSWC1_COLX_MASK                 (0x1FFF0000)
#define ISPOST_VSWC1_COLX_LSBMASK              (0x00001FFF)
#define ISPOST_VSWC1_COLX_SHIFT                (16)
#define ISPOST_VSWC1_COLX_LENGTH               (13)
#define ISPOST_VSWC1_COLX_DEFAULT              (0)
#define ISPOST_VSWC1_COLX_SIGNED_FIELD         (0)

/*
 * ISPOST, VSWC1, ROWY
 * [12:0] ROWY - Y coordinate of the upper left corner of the first window in the row 1. (refer to figure below)
 */
#define ISPOST_VSWC1_ROWY_MASK                 (0x00001FFF)
#define ISPOST_VSWC1_ROWY_LSBMASK              (0x00001FFF)
#define ISPOST_VSWC1_ROWY_SHIFT                (0)
#define ISPOST_VSWC1_ROWY_LENGTH               (13)
#define ISPOST_VSWC1_ROWY_DEFAULT              (0)
#define ISPOST_VSWC1_ROWY_SIGNED_FIELD         (0)


//----------------------------------------------------------
// VSWC2 - Video Stabilization Window Coordinate Register 2.
#define ISPOST_VSWC2_OFFSET                    (0x0158)
#define ISPOST_VSWC2                           (ISPOST_PERI_BASE_ADDR + ISPOST_VSWC2_OFFSET)

/*
 * ISPOST, VSWC2, COLX
 * [28:16] COLX - X coordinate of the upper left corner pixel of the first window in the column 2. (refer to figure below)
 */
#define ISPOST_VSWC2_COLX_MASK                 (0x1FFF0000)
#define ISPOST_VSWC2_COLX_LSBMASK              (0x00001FFF)
#define ISPOST_VSWC2_COLX_SHIFT                (16)
#define ISPOST_VSWC2_COLX_LENGTH               (13)
#define ISPOST_VSWC2_COLX_DEFAULT              (0)
#define ISPOST_VSWC2_COLX_SIGNED_FIELD         (0)

/*
 * ISPOST, VSWC2, ROWY
 * [12:0] ROWY - Y coordinate of the upper left corner of the first window in the row 2. (refer to figure below)
 */
#define ISPOST_VSWC2_ROWY_MASK                 (0x00001FFF)
#define ISPOST_VSWC2_ROWY_LSBMASK              (0x00001FFF)
#define ISPOST_VSWC2_ROWY_SHIFT                (0)
#define ISPOST_VSWC2_ROWY_LENGTH               (13)
#define ISPOST_VSWC2_ROWY_DEFAULT              (0)
#define ISPOST_VSWC2_ROWY_SIGNED_FIELD         (0)


//----------------------------------------------------------
// VSWC3 - Video Stabilization Window Coordinate Register 3.
#define ISPOST_VSWC3_OFFSET                    (0x015C)
#define ISPOST_VSWC3                           (ISPOST_PERI_BASE_ADDR + ISPOST_VSWC3_OFFSET)

/*
 * ISPOST, VSWC3, COLX
 * [28:16] COLX - X coordinate of the upper left corner pixel of the first window in the column 3. (refer to figure below)
 */
#define ISPOST_VSWC3_COLX_MASK                 (0x1FFF0000)
#define ISPOST_VSWC3_COLX_LSBMASK              (0x00001FFF)
#define ISPOST_VSWC3_COLX_SHIFT                (16)
#define ISPOST_VSWC3_COLX_LENGTH               (13)
#define ISPOST_VSWC3_COLX_DEFAULT              (0)
#define ISPOST_VSWC3_COLX_SIGNED_FIELD         (0)


//----------------------------------------------------------
// VSLMV00 - Video Stabilization Local Motion Vector 00 Register.
#define ISPOST_VSLMV00_OFFSET                  (0x0160)
#define ISPOST_VSLMV00                         (ISPOST_PERI_BASE_ADDR + ISPOST_VSLMV00_OFFSET)

/*
 * ISPOST, VSLMV00, X
 * [23:16] X - Local motion vector X coordinate.
 */
#define ISPOST_VSLMV00_X_MASK                  (0x00FF0000)
#define ISPOST_VSLMV00_X_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV00_X_SHIFT                 (16)
#define ISPOST_VSLMV00_X_LENGTH                (8)
#define ISPOST_VSLMV00_X_DEFAULT               (0)
#define ISPOST_VSLMV00_X_SIGNED_FIELD          (0)

/*
 * ISPOST, VSLMV00, Y
 * [7:0] Y - Local motion vector Y coordinate.
 */
#define ISPOST_VSLMV00_Y_MASK                  (0x000000FF)
#define ISPOST_VSLMV00_Y_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV00_Y_SHIFT                 (0)
#define ISPOST_VSLMV00_Y_LENGTH                (8)
#define ISPOST_VSLMV00_Y_DEFAULT               (0)
#define ISPOST_VSLMV00_Y_SIGNED_FIELD          (0)


//----------------------------------------------------------
// VSLMV10 - Video Stabilization Local Motion Vector 10 Register.
#define ISPOST_VSLMV10_OFFSET                  (0x0164)
#define ISPOST_VSLMV10                         (ISPOST_PERI_BASE_ADDR + ISPOST_VSLMV10_OFFSET)

/*
 * ISPOST, VSLMV10, X
 * [23:16] X - Local motion vector X coordinate.
 */
#define ISPOST_VSLMV10_X_MASK                  (0x00FF0000)
#define ISPOST_VSLMV10_X_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV10_X_SHIFT                 (16)
#define ISPOST_VSLMV10_X_LENGTH                (8)
#define ISPOST_VSLMV10_X_DEFAULT               (0)
#define ISPOST_VSLMV10_X_SIGNED_FIELD          (0)

/*
 * ISPOST, VSLMV10, Y
 * [7:0] Y - Local motion vector Y coordinate.
 */
#define ISPOST_VSLMV10_Y_MASK                  (0x000000FF)
#define ISPOST_VSLMV10_Y_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV10_Y_SHIFT                 (0)
#define ISPOST_VSLMV10_Y_LENGTH                (8)
#define ISPOST_VSLMV10_Y_DEFAULT               (0)
#define ISPOST_VSLMV10_Y_SIGNED_FIELD          (0)


//----------------------------------------------------------
// VSLMV20 - Video Stabilization Local Motion Vector 20 Register.
#define ISPOST_VSLMV20_OFFSET                  (0x0168)
#define ISPOST_VSLMV20                         (ISPOST_PERI_BASE_ADDR + ISPOST_VSLMV20_OFFSET)

/*
 * ISPOST, VSLMV20, X
 * [23:16] X - Local motion vector X coordinate.
 */
#define ISPOST_VSLMV20_X_MASK                  (0x00FF0000)
#define ISPOST_VSLMV20_X_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV20_X_SHIFT                 (16)
#define ISPOST_VSLMV20_X_LENGTH                (8)
#define ISPOST_VSLMV20_X_DEFAULT               (0)
#define ISPOST_VSLMV20_X_SIGNED_FIELD          (0)

/*
 * ISPOST, VSLMV20, Y
 * [7:0] Y - Local motion vector Y coordinate.
 */
#define ISPOST_VSLMV20_Y_MASK                  (0x000000FF)
#define ISPOST_VSLMV20_Y_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV20_Y_SHIFT                 (0)
#define ISPOST_VSLMV20_Y_LENGTH                (8)
#define ISPOST_VSLMV20_Y_DEFAULT               (0)
#define ISPOST_VSLMV20_Y_SIGNED_FIELD          (0)


//----------------------------------------------------------
// VSLMV30 - Video Stabilization Local Motion Vector 30 Register.
#define ISPOST_VSLMV30_OFFSET                  (0x016C)
#define ISPOST_VSLMV30                         (ISPOST_PERI_BASE_ADDR + ISPOST_VSLMV30_OFFSET)

/*
 * ISPOST, VSLMV30, X
 * [23:16] X - Local motion vector X coordinate.
 */
#define ISPOST_VSLMV30_X_MASK                  (0x00FF0000)
#define ISPOST_VSLMV30_X_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV30_X_SHIFT                 (16)
#define ISPOST_VSLMV30_X_LENGTH                (8)
#define ISPOST_VSLMV30_X_DEFAULT               (0)
#define ISPOST_VSLMV30_X_SIGNED_FIELD          (0)

/*
 * ISPOST, VSLMV30, Y
 * [7:0] Y - Local motion vector Y coordinate.
 */
#define ISPOST_VSLMV30_Y_MASK                  (0x000000FF)
#define ISPOST_VSLMV30_Y_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV30_Y_SHIFT                 (0)
#define ISPOST_VSLMV30_Y_LENGTH                (8)
#define ISPOST_VSLMV30_Y_DEFAULT               (0)
#define ISPOST_VSLMV30_Y_SIGNED_FIELD          (0)


//----------------------------------------------------------
// VSLMV01 - Video Stabilization Local Motion Vector 01 Register.
#define ISPOST_VSLMV01_OFFSET                  (0x0170)
#define ISPOST_VSLMV01                         (ISPOST_PERI_BASE_ADDR + ISPOST_VSLMV01_OFFSET)

/*
 * ISPOST, VSLMV01, X
 * [23:16] X - Local motion vector X coordinate.
 */
#define ISPOST_VSLMV01_X_MASK                  (0x00FF0000)
#define ISPOST_VSLMV01_X_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV01_X_SHIFT                 (16)
#define ISPOST_VSLMV01_X_LENGTH                (8)
#define ISPOST_VSLMV01_X_DEFAULT               (0)
#define ISPOST_VSLMV01_X_SIGNED_FIELD          (0)

/*
 * ISPOST, VSLMV01, Y
 * [7:0] Y - Local motion vector Y coordinate.
 */
#define ISPOST_VSLMV01_Y_MASK                  (0x000000FF)
#define ISPOST_VSLMV01_Y_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV01_Y_SHIFT                 (0)
#define ISPOST_VSLMV01_Y_LENGTH                (8)
#define ISPOST_VSLMV01_Y_DEFAULT               (0)
#define ISPOST_VSLMV01_Y_SIGNED_FIELD          (0)


//----------------------------------------------------------
// VSLMV11 - Video Stabilization Local Motion Vector 11 Register.
#define ISPOST_VSLMV11_OFFSET                  (0x0174)
#define ISPOST_VSLMV11                         (ISPOST_PERI_BASE_ADDR + ISPOST_VSLMV11_OFFSET)

/*
 * ISPOST, VSLMV11, X
 * [23:16] X - Local motion vector X coordinate.
 */
#define ISPOST_VSLMV11_X_MASK                  (0x00FF0000)
#define ISPOST_VSLMV11_X_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV11_X_SHIFT                 (16)
#define ISPOST_VSLMV11_X_LENGTH                (8)
#define ISPOST_VSLMV11_X_DEFAULT               (0)
#define ISPOST_VSLMV11_X_SIGNED_FIELD          (0)

/*
 * ISPOST, VSLMV11, Y
 * [7:0] Y - Local motion vector Y coordinate.
 */
#define ISPOST_VSLMV11_Y_MASK                  (0x000000FF)
#define ISPOST_VSLMV11_Y_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV11_Y_SHIFT                 (0)
#define ISPOST_VSLMV11_Y_LENGTH                (8)
#define ISPOST_VSLMV11_Y_DEFAULT               (0)
#define ISPOST_VSLMV11_Y_SIGNED_FIELD          (0)


//----------------------------------------------------------
// VSLMV21 - Video Stabilization Local Motion Vector 21 Register.
#define ISPOST_VSLMV21_OFFSET                  (0x0178)
#define ISPOST_VSLMV21                         (ISPOST_PERI_BASE_ADDR + ISPOST_VSLMV21_OFFSET)

/*
 * ISPOST, VSLMV21, X
 * [23:16] X - Local motion vector X coordinate.
 */
#define ISPOST_VSLMV21_X_MASK                  (0x00FF0000)
#define ISPOST_VSLMV21_X_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV21_X_SHIFT                 (16)
#define ISPOST_VSLMV21_X_LENGTH                (8)
#define ISPOST_VSLMV21_X_DEFAULT               (0)
#define ISPOST_VSLMV21_X_SIGNED_FIELD          (0)

/*
 * ISPOST, VSLMV21, Y
 * [7:0] Y - Local motion vector Y coordinate.
 */
#define ISPOST_VSLMV21_Y_MASK                  (0x000000FF)
#define ISPOST_VSLMV21_Y_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV21_Y_SHIFT                 (0)
#define ISPOST_VSLMV21_Y_LENGTH                (8)
#define ISPOST_VSLMV21_Y_DEFAULT               (0)
#define ISPOST_VSLMV21_Y_SIGNED_FIELD          (0)


//----------------------------------------------------------
// VSLMV31 - Video Stabilization Local Motion Vector 31 Register.
#define ISPOST_VSLMV31_OFFSET                  (0x017C)
#define ISPOST_VSLMV31                         (ISPOST_PERI_BASE_ADDR + ISPOST_VSLMV31_OFFSET)

/*
 * ISPOST, VSLMV31, X
 * [23:16] X - Local motion vector X coordinate.
 */
#define ISPOST_VSLMV31_X_MASK                  (0x00FF0000)
#define ISPOST_VSLMV31_X_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV31_X_SHIFT                 (16)
#define ISPOST_VSLMV31_X_LENGTH                (8)
#define ISPOST_VSLMV31_X_DEFAULT               (0)
#define ISPOST_VSLMV31_X_SIGNED_FIELD          (0)

/*
 * ISPOST, VSLMV31, Y
 * [7:0] Y - Local motion vector Y coordinate.
 */
#define ISPOST_VSLMV31_Y_MASK                  (0x000000FF)
#define ISPOST_VSLMV31_Y_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV31_Y_SHIFT                 (0)
#define ISPOST_VSLMV31_Y_LENGTH                (8)
#define ISPOST_VSLMV31_Y_DEFAULT               (0)
#define ISPOST_VSLMV31_Y_SIGNED_FIELD          (0)


//----------------------------------------------------------
// VSLMV02 - Video Stabilization Local Motion Vector 02 Register.
#define ISPOST_VSLMV02_OFFSET                  (0x0180)
#define ISPOST_VSLMV02                         (ISPOST_PERI_BASE_ADDR + ISPOST_VSLMV02_OFFSET)

/*
 * ISPOST, VSLMV02, X
 * [23:16] X - Local motion vector X coordinate.
 */
#define ISPOST_VSLMV02_X_MASK                  (0x00FF0000)
#define ISPOST_VSLMV02_X_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV02_X_SHIFT                 (16)
#define ISPOST_VSLMV02_X_LENGTH                (8)
#define ISPOST_VSLMV02_X_DEFAULT               (0)
#define ISPOST_VSLMV02_X_SIGNED_FIELD          (0)

/*
 * ISPOST, VSLMV02, Y
 * [7:0] Y - Local motion vector Y coordinate.
 */
#define ISPOST_VSLMV02_Y_MASK                  (0x000000FF)
#define ISPOST_VSLMV02_Y_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV02_Y_SHIFT                 (0)
#define ISPOST_VSLMV02_Y_LENGTH                (8)
#define ISPOST_VSLMV02_Y_DEFAULT               (0)
#define ISPOST_VSLMV02_Y_SIGNED_FIELD          (0)


//----------------------------------------------------------
// VSLMV12 - Video Stabilization Local Motion Vector 12 Register.
#define ISPOST_VSLMV12_OFFSET                  (0x0184)
#define ISPOST_VSLMV12                         (ISPOST_PERI_BASE_ADDR + ISPOST_VSLMV12_OFFSET)

/*
 * ISPOST, VSLMV12, X
 * [23:16] X - Local motion vector X coordinate.
 */
#define ISPOST_VSLMV12_X_MASK                  (0x00FF0000)
#define ISPOST_VSLMV12_X_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV12_X_SHIFT                 (16)
#define ISPOST_VSLMV12_X_LENGTH                (8)
#define ISPOST_VSLMV12_X_DEFAULT               (0)
#define ISPOST_VSLMV12_X_SIGNED_FIELD          (0)

/*
 * ISPOST, VSLMV12, Y
 * [7:0] Y - Local motion vector Y coordinate.
 */
#define ISPOST_VSLMV12_Y_MASK                  (0x000000FF)
#define ISPOST_VSLMV12_Y_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV12_Y_SHIFT                 (0)
#define ISPOST_VSLMV12_Y_LENGTH                (8)
#define ISPOST_VSLMV12_Y_DEFAULT               (0)
#define ISPOST_VSLMV12_Y_SIGNED_FIELD          (0)


//----------------------------------------------------------
// VSLMV22 - Video Stabilization Local Motion Vector 22 Register.
#define ISPOST_VSLMV22_OFFSET                  (0x0188)
#define ISPOST_VSLMV22                         (ISPOST_PERI_BASE_ADDR + ISPOST_VSLMV22_OFFSET)

/*
 * ISPOST, VSLMV22, X
 * [23:16] X - Local motion vector X coordinate.
 */
#define ISPOST_VSLMV22_X_MASK                  (0x00FF0000)
#define ISPOST_VSLMV22_X_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV22_X_SHIFT                 (16)
#define ISPOST_VSLMV22_X_LENGTH                (8)
#define ISPOST_VSLMV22_X_DEFAULT               (0)
#define ISPOST_VSLMV22_X_SIGNED_FIELD          (0)

/*
 * ISPOST, VSLMV22, Y
 * [7:0] Y - Local motion vector Y coordinate.
 */
#define ISPOST_VSLMV22_Y_MASK                  (0x000000FF)
#define ISPOST_VSLMV22_Y_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV22_Y_SHIFT                 (0)
#define ISPOST_VSLMV22_Y_LENGTH                (8)
#define ISPOST_VSLMV22_Y_DEFAULT               (0)
#define ISPOST_VSLMV22_Y_SIGNED_FIELD          (0)


//----------------------------------------------------------
// VSLMV32 - Video Stabilization Local Motion Vector 32 Register.
#define ISPOST_VSLMV32_OFFSET                  (0x018C)
#define ISPOST_VSLMV32                         (ISPOST_PERI_BASE_ADDR + ISPOST_VSLMV32_OFFSET)

/*
 * ISPOST, VSLMV32, X
 * [23:16] X - Local motion vector X coordinate.
 */
#define ISPOST_VSLMV32_X_MASK                  (0x00FF0000)
#define ISPOST_VSLMV32_X_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV32_X_SHIFT                 (16)
#define ISPOST_VSLMV32_X_LENGTH                (8)
#define ISPOST_VSLMV32_X_DEFAULT               (0)
#define ISPOST_VSLMV32_X_SIGNED_FIELD          (0)

/*
 * ISPOST, VSLMV32, Y
 * [7:0] Y - Local motion vector Y coordinate.
 */
#define ISPOST_VSLMV32_Y_MASK                  (0x000000FF)
#define ISPOST_VSLMV32_Y_LSBMASK               (0x000000FF)
#define ISPOST_VSLMV32_Y_SHIFT                 (0)
#define ISPOST_VSLMV32_Y_LENGTH                (8)
#define ISPOST_VSLMV32_Y_DEFAULT               (0)
#define ISPOST_VSLMV32_Y_SIGNED_FIELD          (0)


//----------------------------------------------------------
// VSCT00 - Video Stabilization Contrast 00 Register.
#define ISPOST_VSCT00_OFFSET                   (0x0190)
#define ISPOST_VSCT00                          (ISPOST_PERI_BASE_ADDR + ISPOST_VSCT00_OFFSET)

/*
 * ISPOST, VSCT00, X
 * [31:16] X - Contrast measure X coordinate.
 */
#define ISPOST_VSCT00_X_MASK                   (0xFFFF0000)
#define ISPOST_VSCT00_X_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT00_X_SHIFT                  (16)
#define ISPOST_VSCT00_X_LENGTH                 (16)
#define ISPOST_VSCT00_X_DEFAULT                (0)
#define ISPOST_VSCT00_X_SIGNED_FIELD           (0)

/*
 * ISPOST, VSCT00, Y
 * [15:0] Y - Contrast measure Y coordinate.
 */
#define ISPOST_VSCT00_Y_MASK                   (0x0000FFFF)
#define ISPOST_VSCT00_Y_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT00_Y_SHIFT                  (0)
#define ISPOST_VSCT00_Y_LENGTH                 (16)
#define ISPOST_VSCT00_Y_DEFAULT                (0)
#define ISPOST_VSCT00_Y_SIGNED_FIELD           (0)


//----------------------------------------------------------
// VSCT10 - Video Stabilization Contrast 10 Register.
#define ISPOST_VSCT10_OFFSET                   (0x0194)
#define ISPOST_VSCT10                          (ISPOST_PERI_BASE_ADDR + ISPOST_VSCT10_OFFSET)

/*
 * ISPOST, VSCT10, X
 * [31:16] X - Contrast measure X coordinate.
 */
#define ISPOST_VSCT10_X_MASK                   (0xFFFF0000)
#define ISPOST_VSCT10_X_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT10_X_SHIFT                  (16)
#define ISPOST_VSCT10_X_LENGTH                 (16)
#define ISPOST_VSCT10_X_DEFAULT                (0)
#define ISPOST_VSCT10_X_SIGNED_FIELD           (0)

/*
 * ISPOST, VSCT10, Y
 * [15:0] Y - Contrast measure Y coordinate.
 */
#define ISPOST_VSCT10_Y_MASK                   (0x0000FFFF)
#define ISPOST_VSCT10_Y_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT10_Y_SHIFT                  (0)
#define ISPOST_VSCT10_Y_LENGTH                 (16)
#define ISPOST_VSCT10_Y_DEFAULT                (0)
#define ISPOST_VSCT10_Y_SIGNED_FIELD           (0)


//----------------------------------------------------------
// VSCT20 - Video Stabilization Contrast 20 Register.
#define ISPOST_VSCT20_OFFSET                   (0x0198)
#define ISPOST_VSCT20                          (ISPOST_PERI_BASE_ADDR + ISPOST_VSCT20_OFFSET)

/*
 * ISPOST, VSCT20, X
 * [31:16] X - Contrast measure X coordinate.
 */
#define ISPOST_VSCT20_X_MASK                   (0xFFFF0000)
#define ISPOST_VSCT20_X_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT20_X_SHIFT                  (16)
#define ISPOST_VSCT20_X_LENGTH                 (16)
#define ISPOST_VSCT20_X_DEFAULT                (0)
#define ISPOST_VSCT20_X_SIGNED_FIELD           (0)

/*
 * ISPOST, VSCT20, Y
 * [15:0] Y - Contrast measure Y coordinate.
 */
#define ISPOST_VSCT20_Y_MASK                   (0x0000FFFF)
#define ISPOST_VSCT20_Y_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT20_Y_SHIFT                  (0)
#define ISPOST_VSCT20_Y_LENGTH                 (16)
#define ISPOST_VSCT20_Y_DEFAULT                (0)
#define ISPOST_VSCT20_Y_SIGNED_FIELD           (0)


//----------------------------------------------------------
// VSCT30 - Video Stabilization Contrast 30 Register.
#define ISPOST_VSCT30_OFFSET                   (0x019C)
#define ISPOST_VSCT30                          (ISPOST_PERI_BASE_ADDR + ISPOST_VSCT30_OFFSET)

/*
 * ISPOST, VSCT30, X
 * [31:16] X - Contrast measure X coordinate.
 */
#define ISPOST_VSCT30_X_MASK                   (0xFFFF0000)
#define ISPOST_VSCT30_X_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT30_X_SHIFT                  (16)
#define ISPOST_VSCT30_X_LENGTH                 (16)
#define ISPOST_VSCT30_X_DEFAULT                (0)
#define ISPOST_VSCT30_X_SIGNED_FIELD           (0)

/*
 * ISPOST, VSCT30, Y
 * [15:0] Y - Contrast measure Y coordinate.
 */
#define ISPOST_VSCT30_Y_MASK                   (0x0000FFFF)
#define ISPOST_VSCT30_Y_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT30_Y_SHIFT                  (0)
#define ISPOST_VSCT30_Y_LENGTH                 (16)
#define ISPOST_VSCT30_Y_DEFAULT                (0)
#define ISPOST_VSCT30_Y_SIGNED_FIELD           (0)


//----------------------------------------------------------
// VSCT01 - Video Stabilization Contrast 01 Register.
#define ISPOST_VSCT01_OFFSET                   (0x01A0)
#define ISPOST_VSCT01                          (ISPOST_PERI_BASE_ADDR + ISPOST_VSCT01_OFFSET)

/*
 * ISPOST, VSCT01, X
 * [31:16] X - Contrast measure X coordinate.
 */
#define ISPOST_VSCT01_X_MASK                   (0xFFFF0000)
#define ISPOST_VSCT01_X_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT01_X_SHIFT                  (16)
#define ISPOST_VSCT01_X_LENGTH                 (16)
#define ISPOST_VSCT01_X_DEFAULT                (0)
#define ISPOST_VSCT01_X_SIGNED_FIELD           (0)

/*
 * ISPOST, VSCT01, Y
 * [15:0] Y - Contrast measure Y coordinate.
 */
#define ISPOST_VSCT01_Y_MASK                   (0x0000FFFF)
#define ISPOST_VSCT01_Y_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT01_Y_SHIFT                  (0)
#define ISPOST_VSCT01_Y_LENGTH                 (16)
#define ISPOST_VSCT01_Y_DEFAULT                (0)
#define ISPOST_VSCT01_Y_SIGNED_FIELD           (0)


//----------------------------------------------------------
// VSCT11 - Video Stabilization Contrast 11 Register.
#define ISPOST_VSCT11_OFFSET                   (0x01A4)
#define ISPOST_VSCT11                          (ISPOST_PERI_BASE_ADDR + ISPOST_VSCT11_OFFSET)

/*
 * ISPOST, VSCT11, X
 * [31:16] X - Contrast measure X coordinate.
 */
#define ISPOST_VSCT11_X_MASK                   (0xFFFF0000)
#define ISPOST_VSCT11_X_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT11_X_SHIFT                  (16)
#define ISPOST_VSCT11_X_LENGTH                 (16)
#define ISPOST_VSCT11_X_DEFAULT                (0)
#define ISPOST_VSCT11_X_SIGNED_FIELD           (0)

/*
 * ISPOST, VSCT11, Y
 * [15:0] Y - Contrast measure Y coordinate.
 */
#define ISPOST_VSCT11_Y_MASK                   (0x0000FFFF)
#define ISPOST_VSCT11_Y_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT11_Y_SHIFT                  (0)
#define ISPOST_VSCT11_Y_LENGTH                 (16)
#define ISPOST_VSCT11_Y_DEFAULT                (0)
#define ISPOST_VSCT11_Y_SIGNED_FIELD           (0)


//----------------------------------------------------------
// VSCT21 - Video Stabilization Contrast 21 Register.
#define ISPOST_VSCT21_OFFSET                   (0x01A8)
#define ISPOST_VSCT21                          (ISPOST_PERI_BASE_ADDR + ISPOST_VSCT21_OFFSET)

/*
 * ISPOST, VSCT21, X
 * [31:16] X - Contrast measure X coordinate.
 */
#define ISPOST_VSCT21_X_MASK                   (0xFFFF0000)
#define ISPOST_VSCT21_X_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT21_X_SHIFT                  (16)
#define ISPOST_VSCT21_X_LENGTH                 (16)
#define ISPOST_VSCT21_X_DEFAULT                (0)
#define ISPOST_VSCT21_X_SIGNED_FIELD           (0)

/*
 * ISPOST, VSCT21, Y
 * [15:0] Y - Contrast measure Y coordinate.
 */
#define ISPOST_VSCT21_Y_MASK                   (0x0000FFFF)
#define ISPOST_VSCT21_Y_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT21_Y_SHIFT                  (0)
#define ISPOST_VSCT21_Y_LENGTH                 (16)
#define ISPOST_VSCT21_Y_DEFAULT                (0)
#define ISPOST_VSCT21_Y_SIGNED_FIELD           (0)


//----------------------------------------------------------
// VSCT31 - Video Stabilization Contrast 31 Register.
#define ISPOST_VSCT31_OFFSET                   (0x01AC)
#define ISPOST_VSCT31                          (ISPOST_PERI_BASE_ADDR + ISPOST_VSCT31_OFFSET)

/*
 * ISPOST, VSCT31, X
 * [31:16] X - Contrast measure X coordinate.
 */
#define ISPOST_VSCT31_X_MASK                   (0xFFFF0000)
#define ISPOST_VSCT31_X_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT31_X_SHIFT                  (16)
#define ISPOST_VSCT31_X_LENGTH                 (16)
#define ISPOST_VSCT31_X_DEFAULT                (0)
#define ISPOST_VSCT31_X_SIGNED_FIELD           (0)

/*
 * ISPOST, VSCT31, Y
 * [15:0] Y - Contrast measure Y coordinate.
 */
#define ISPOST_VSCT31_Y_MASK                   (0x0000FFFF)
#define ISPOST_VSCT31_Y_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT31_Y_SHIFT                  (0)
#define ISPOST_VSCT31_Y_LENGTH                 (16)
#define ISPOST_VSCT31_Y_DEFAULT                (0)
#define ISPOST_VSCT31_Y_SIGNED_FIELD           (0)


//----------------------------------------------------------
// VSCT02 - Video Stabilization Contrast 02 Register.
#define ISPOST_VSCT02_OFFSET                   (0x01B0)
#define ISPOST_VSCT02                          (ISPOST_PERI_BASE_ADDR + ISPOST_VSCT02_OFFSET)

/*
 * ISPOST, VSCT02, X
 * [31:16] X - Contrast measure X coordinate.
 */
#define ISPOST_VSCT02_X_MASK                   (0xFFFF0000)
#define ISPOST_VSCT02_X_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT02_X_SHIFT                  (16)
#define ISPOST_VSCT02_X_LENGTH                 (16)
#define ISPOST_VSCT02_X_DEFAULT                (0)
#define ISPOST_VSCT02_X_SIGNED_FIELD           (0)

/*
 * ISPOST, VSCT02, Y
 * [15:0] Y - Contrast measure Y coordinate.
 */
#define ISPOST_VSCT02_Y_MASK                   (0x0000FFFF)
#define ISPOST_VSCT02_Y_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT02_Y_SHIFT                  (0)
#define ISPOST_VSCT02_Y_LENGTH                 (16)
#define ISPOST_VSCT02_Y_DEFAULT                (0)
#define ISPOST_VSCT02_Y_SIGNED_FIELD           (0)


//----------------------------------------------------------
// VSCT12 - Video Stabilization Contrast 12 Register.
#define ISPOST_VSCT12_OFFSET                   (0x01B4)
#define ISPOST_VSCT12                          (ISPOST_PERI_BASE_ADDR + ISPOST_VSCT12_OFFSET)

/*
 * ISPOST, VSCT12, X
 * [31:16] X - Contrast measure X coordinate.
 */
#define ISPOST_VSCT12_X_MASK                   (0xFFFF0000)
#define ISPOST_VSCT12_X_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT12_X_SHIFT                  (16)
#define ISPOST_VSCT12_X_LENGTH                 (16)
#define ISPOST_VSCT12_X_DEFAULT                (0)
#define ISPOST_VSCT12_X_SIGNED_FIELD           (0)

/*
 * ISPOST, VSCT12, Y
 * [15:0] Y - Contrast measure Y coordinate.
 */
#define ISPOST_VSCT12_Y_MASK                   (0x0000FFFF)
#define ISPOST_VSCT12_Y_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT12_Y_SHIFT                  (0)
#define ISPOST_VSCT12_Y_LENGTH                 (16)
#define ISPOST_VSCT12_Y_DEFAULT                (0)
#define ISPOST_VSCT12_Y_SIGNED_FIELD           (0)


//----------------------------------------------------------
// VSCT22 - Video Stabilization Contrast 22 Register.
#define ISPOST_VSCT22_OFFSET                   (0x01B8)
#define ISPOST_VSCT22                          (ISPOST_PERI_BASE_ADDR + ISPOST_VSCT22_OFFSET)

/*
 * ISPOST, VSCT22, X
 * [31:16] X - Contrast measure X coordinate.
 */
#define ISPOST_VSCT22_X_MASK                   (0xFFFF0000)
#define ISPOST_VSCT22_X_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT22_X_SHIFT                  (16)
#define ISPOST_VSCT22_X_LENGTH                 (16)
#define ISPOST_VSCT22_X_DEFAULT                (0)
#define ISPOST_VSCT22_X_SIGNED_FIELD           (0)

/*
 * ISPOST, VSCT22, Y
 * [15:0] Y - Contrast measure Y coordinate.
 */
#define ISPOST_VSCT22_Y_MASK                   (0x0000FFFF)
#define ISPOST_VSCT22_Y_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT22_Y_SHIFT                  (0)
#define ISPOST_VSCT22_Y_LENGTH                 (16)
#define ISPOST_VSCT22_Y_DEFAULT                (0)
#define ISPOST_VSCT22_Y_SIGNED_FIELD           (0)


//----------------------------------------------------------
// VSCT32 - Video Stabilization Contrast 32 Register.
#define ISPOST_VSCT32_OFFSET                   (0x01BC)
#define ISPOST_VSCT32                          (ISPOST_PERI_BASE_ADDR + ISPOST_VSCT32_OFFSET)

/*
 * ISPOST, VSCT32, X
 * [31:16] X - Contrast measure X coordinate.
 */
#define ISPOST_VSCT32_X_MASK                   (0xFFFF0000)
#define ISPOST_VSCT32_X_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT32_X_SHIFT                  (16)
#define ISPOST_VSCT32_X_LENGTH                 (16)
#define ISPOST_VSCT32_X_DEFAULT                (0)
#define ISPOST_VSCT32_X_SIGNED_FIELD           (0)

/*
 * ISPOST, VSCT32, Y
 * [15:0] Y - Contrast measure Y coordinate.
 */
#define ISPOST_VSCT32_Y_MASK                   (0x0000FFFF)
#define ISPOST_VSCT32_Y_LSBMASK                (0x0000FFFF)
#define ISPOST_VSCT32_Y_SHIFT                  (0)
#define ISPOST_VSCT32_Y_LENGTH                 (16)
#define ISPOST_VSCT32_Y_DEFAULT                (0)
#define ISPOST_VSCT32_Y_SIGNED_FIELD           (0)


//----------------------------------------------------------
// VSSIDX - Video Stabilization Curve Access Index Register.
#define ISPOST_VSSIDX_OFFSET                   (0x01C0)
#define ISPOST_VSSIDX                          (ISPOST_PERI_BASE_ADDR + ISPOST_VSSIDX_OFFSET)

/*
 * ISPOST, VSSIDX, CRDMD
 * [19:18] CRDMD - Contrast read mode.
 *                 VSCT00 – VSCT32 register RD mode.
 *                 00 : X,Y.
 *                      (16 bits, 16 bits with saturation)
 *                 01 : X.
 *                      (24 bits)
 *                 10 : Y.
 *                      (24 bits)
 *                 11 : Reserved
 */
#define ISPOST_VSSIDX_CRDMD_MASK               (0x000C0000)
#define ISPOST_VSSIDX_CRDMD_LSBMASK            (0x00000003)
#define ISPOST_VSSIDX_CRDMD_SHIFT              (18)
#define ISPOST_VSSIDX_CRDMD_LENGTH             (2)
#define ISPOST_VSSIDX_CRDMD_DEFAULT            (0)
#define ISPOST_VSSIDX_CRDMD_SIGNED_FIELD       (0)

/*
 * ISPOST, VSSIDX, SAINC
 * [17] SAINC - Automatically increment index VSSIDX[IDX] on VSSACC read.
 */
#define ISPOST_VSSIDX_SAINC_MASK               (0x00020000)
#define ISPOST_VSSIDX_SAINC_LSBMASK            (0x00000001)
#define ISPOST_VSSIDX_SAINC_SHIFT              (17)
#define ISPOST_VSSIDX_SAINC_LENGTH             (1)
#define ISPOST_VSSIDX_SAINC_DEFAULT            (0)
#define ISPOST_VSSIDX_SAINC_SIGNED_FIELD       (0)

/*
 * ISPOST, VSSIDX, SADAINC
 * [16] SADAINC - Automatically increment index VSSIDX[IDX] on VSCYSADACC read.
 */
#define ISPOST_VSSIDX_SADAINC_MASK             (0x00010000)
#define ISPOST_VSSIDX_SADAINC_LSBMASK          (0x00000001)
#define ISPOST_VSSIDX_SADAINC_SHIFT            (16)
#define ISPOST_VSSIDX_SADAINC_LENGTH           (1)
#define ISPOST_VSSIDX_SADAINC_DEFAULT          (0)
#define ISPOST_VSSIDX_SADAINC_SIGNED_FIELD     (0)

/*
 * ISPOST, VSSIDX, XBLOCKIDX
 * [13:12] XBLOCKIDX - The X Block index for the Signature and SAD curves to be read.
 *                     Valid XBLOCKIDX values: 0-3.
 */
#define ISPOST_VSSIDX_XBLOCKIDX_MASK           (0x00003000)
#define ISPOST_VSSIDX_XBLOCKIDX_LSBMASK        (0x00000003)
#define ISPOST_VSSIDX_XBLOCKIDX_SHIFT          (12)
#define ISPOST_VSSIDX_XBLOCKIDX_LENGTH         (2)
#define ISPOST_VSSIDX_XBLOCKIDX_DEFAULT        (0)
#define ISPOST_VSSIDX_XBLOCKIDX_SIGNED_FIELD   (0)

/*
 * ISPOST, VSSIDX, YBLOCKIDX
 * [9:8] YBLOCKIDX - The Y Block index for the Signature and SAD curves to be read.
 *                   Valid XBLOCKIDX values: 0-2.
 */
#define ISPOST_VSSIDX_YBLOCKIDX_MASK           (0x00000300)
#define ISPOST_VSSIDX_YBLOCKIDX_LSBMASK        (0x00000003)
#define ISPOST_VSSIDX_YBLOCKIDX_SHIFT          (8)
#define ISPOST_VSSIDX_YBLOCKIDX_LENGTH         (2)
#define ISPOST_VSSIDX_YBLOCKIDX_DEFAULT        (0)
#define ISPOST_VSSIDX_YBLOCKIDX_SIGNED_FIELD   (0)

/*
 * ISPOST, VSSIDX, IDX
 * [7:0] IDX - Signature and SAD curves index.
 *             SAD index will be automatically increased one after CPU read both of the CXSAD[index] and the CYSAD[index], i.e. 1C8h and 1CCh.
 *             CPU read data as follows : CXSAD[index] , CYSAD[index], CXSAD[index+1] , CYSAD[index+1], CXSAD[index+2], CYSAD[index+2],......,CXSAD[index+n] , CYSAD[index+n], ......., etc.
 */
#define ISPOST_VSSIDX_IDX_MASK                 (0x000000FF)
#define ISPOST_VSSIDX_IDX_LSBMASK              (0x000000FF)
#define ISPOST_VSSIDX_IDX_SHIFT                (0)
#define ISPOST_VSSIDX_IDX_LENGTH               (8)
#define ISPOST_VSSIDX_IDX_DEFAULT              (0)
#define ISPOST_VSSIDX_IDX_SIGNED_FIELD         (0)


//----------------------------------------------------------
// VSSACC - Video Stabilization Signature Curve Access Data Register.
#define ISPOST_VSSACC_OFFSET                   (0x01C4)
#define ISPOST_VSSACC                          (ISPOST_PERI_BASE_ADDR + ISPOST_VSSACC_OFFSET)

/*
 * ISPOST, VSSACC, CYSC
 * [31:16] CYSC - Cy Signature curve data.
 *                This register is indexed by VSSIDX[IDX] and VSSIDX[XBLOCKIDX,YBLOCKIDX] (1C0H).
 *                Valid IDX index values: 0-159.
 *                Valid XBLOCKIDX index values : 0-3.
 *                Valid YBLOCKIDX index values : 0-2.
 */
#define ISPOST_VSSACC_CYSC_MASK                (0xFFFF0000)
#define ISPOST_VSSACC_CYSC_LSBMASK             (0x0000FFFF)
#define ISPOST_VSSACC_CYSC_SHIFT               (16)
#define ISPOST_VSSACC_CYSC_LENGTH              (16)
#define ISPOST_VSSACC_CYSC_DEFAULT             (0)
#define ISPOST_VSSACC_CYSC_SIGNED_FIELD        (0)

/*
 * ISPOST, VSSACC, CXSC
 * [15:0] CXSC - Cx Signature curve data.
 *               This register is indexed by VSSIDX[IDX] and VSSIDX[XBLOCKIDX,YBLOCKIDX] (1C0H).
 *               Valid IDX index values: 0-159.
 *               Valid XBLOCKIDX index values : 0-3.
 *               Valid YBLOCKIDX index values : 0-2.
 */
#define ISPOST_VSSACC_CXSC_MASK                (0x0000FFFF)
#define ISPOST_VSSACC_CXSC_LSBMASK             (0x0000FFFF)
#define ISPOST_VSSACC_CXSC_SHIFT               (0)
#define ISPOST_VSSACC_CXSC_LENGTH              (16)
#define ISPOST_VSSACC_CXSC_DEFAULT             (0)
#define ISPOST_VSSACC_CXSC_SIGNED_FIELD        (0)


//----------------------------------------------------------
// VSCXSADACC - Video Stabilization SAD Access Data Register.
#define ISPOST_VSCXSADACC_OFFSET               (0x01C8)
#define ISPOST_VSCXSADACC                      (ISPOST_PERI_BASE_ADDR + ISPOST_VSCXSADACC_OFFSET)

/*
 * ISPOST, VSCXSADACC, CXSAD
 * [31:0] CXSAD - SAD curve data for CX component.
 *                This register is indexed by VSSIDX[IDX] and VSSIDX[XBLOCKIDX,YBLOCKIDX] (1C0H).
 *                Valid IDX index values: 0-97.
 *                Valid XBLOCKIDX index values : 0-3.
 *                Valid YBLOCKIDX index values : 0-2.
 */
#define ISPOST_VSCXSADACC_CXSAD_MASK           (0xFFFFFFFF)
#define ISPOST_VSCXSADACC_CXSAD_LSBMASK        (0xFFFFFFFF)
#define ISPOST_VSCXSADACC_CXSAD_SHIFT          (0)
#define ISPOST_VSCXSADACC_CXSAD_LENGTH         (32)
#define ISPOST_VSCXSADACC_CXSAD_DEFAULT        (0)
#define ISPOST_VSCXSADACC_CXSAD_SIGNED_FIELD   (0)


//----------------------------------------------------------
// VSCYSADACC - Video Stabilization SAD Access Data Register.
#define ISPOST_VSCYSADACC_OFFSET               (0x01CC)
#define ISPOST_VSCYSADACC                      (ISPOST_PERI_BASE_ADDR + ISPOST_VSCYSADACC_OFFSET)

/*
 * ISPOST, VSCYSADACC, CYSAD
 * [31:0] CYSAD - SAD curve data for CY component.
 *                This register is indexed by VSSIDX[IDX] and VSSIDX[XBLOCKIDX,YBLOCKIDX] (1C0H).
 *                Valid IDX index values: 0-97.
 *                Valid XBLOCKIDX index values : 0-3.
 *                Valid YBLOCKIDX index values : 0-2.
 */
#define ISPOST_VSCYSADACC_CYSAD_MASK           (0xFFFFFFFF)
#define ISPOST_VSCYSADACC_CYSAD_LSBMASK        (0xFFFFFFFF)
#define ISPOST_VSCYSADACC_CYSAD_SHIFT          (0)
#define ISPOST_VSCYSADACC_CYSAD_LENGTH         (32)
#define ISPOST_VSCYSADACC_CYSAD_DEFAULT        (0)
#define ISPOST_VSCYSADACC_CYSAD_SIGNED_FIELD   (0)


//----------------------------------------------------------
// VSRCAY - VS Source Image Y Plane Start Address Register.
#define ISPOST_VSRCAY_OFFSET                   (0x0208)
#define ISPOST_VSRCAY                          (ISPOST_PERI_BASE_ADDR + ISPOST_VSRCAY_OFFSET)

/*
 * ISPOST, VSRCAY, SAD
 * [31:3] SAD - Base address in QWORD (64bit) boundary.
 */
#define ISPOST_VSRCAY_SAD_MASK                 (0xFFFFFFF8)
#define ISPOST_VSRCAY_SAD_LSBMASK              (0xFFFFFFF8)
#define ISPOST_VSRCAY_SAD_SHIFT                (0)
#define ISPOST_VSRCAY_SAD_LENGTH               (32)
#define ISPOST_VSRCAY_SAD_DEFAULT              (0)
#define ISPOST_VSRCAY_SAD_SIGNED_FIELD         (0)


//----------------------------------------------------------
// VSRCAUV - VS Source Image UV Plane Start Address Register.
#define ISPOST_VSRCAUV_OFFSET                  (0x0210)
#define ISPOST_VSRCAUV                         (ISPOST_PERI_BASE_ADDR + ISPOST_VSRCAUV_OFFSET)

/*
 * ISPOST, VSRCAUV, SAD
 * [31:3] SAD - Base address in QWORD (64bit) boundary.
 */
#define ISPOST_VSRCAUV_SAD_MASK                (0xFFFFFFF8)
#define ISPOST_VSRCAUV_SAD_LSBMASK             (0xFFFFFFF8)
#define ISPOST_VSRCAUV_SAD_SHIFT               (0)
#define ISPOST_VSRCAUV_SAD_LENGTH              (32)
#define ISPOST_VSRCAUV_SAD_DEFAULT             (0)
#define ISPOST_VSRCAUV_SAD_SIGNED_FIELD        (0)


//----------------------------------------------------------
// VSRCS - VS Source Image Stride Register.
#define ISPOST_VSRCS_OFFSET                    (0x0214)
#define ISPOST_VSRCS                           (ISPOST_PERI_BASE_ADDR + ISPOST_VSRCS_OFFSET)

/*
 * ISPOST, VSRCS, SD
 * [12:3] SD - Image stride (QWORD granularity).
 */
#define ISPOST_VSRCS_SD_MASK                   (0x00001FF8)
#define ISPOST_VSRCS_SD_LSBMASK                (0x00001FF8)
#define ISPOST_VSRCS_SD_SHIFT                  (0)
#define ISPOST_VSRCS_SD_LENGTH                 (13)
#define ISPOST_VSRCS_SD_DEFAULT                (0)
#define ISPOST_VSRCS_SD_SIGNED_FIELD           (0)


//----------------------------------------------------------
// VSRSZ - VS Source Image Size Register.
#define ISPOST_VSRSZ_OFFSET                    (0x0218)
#define ISPOST_VSRSZ                           (ISPOST_PERI_BASE_ADDR + ISPOST_VSRSZ_OFFSET)

/*
 * ISPOST, VSRSZ, W
 * [28:16] W - Source image width (pixel granularity).
 */
#define ISPOST_VSRSZ_W_MASK                    (0x1FFF0000)
#define ISPOST_VSRSZ_W_LSBMASK                 (0x00001FFF)
#define ISPOST_VSRSZ_W_SHIFT                   (16)
#define ISPOST_VSRSZ_W_LENGTH                  (13)
#define ISPOST_VSRSZ_W_DEFAULT                 (0)
#define ISPOST_VSRSZ_W_SIGNED_FIELD            (0)

/*
 * ISPOST, VSRSZ, H
 * [12:0] H - Source image height (line granularity).
 */
#define ISPOST_VSRSZ_H_MASK                    (0x00001FFF)
#define ISPOST_VSRSZ_H_LSBMASK                 (0x00001FFF)
#define ISPOST_VSRSZ_H_SHIFT                   (0)
#define ISPOST_VSRSZ_H_LENGTH                  (13)
#define ISPOST_VSRSZ_H_DEFAULT                 (0)
#define ISPOST_VSRSZ_H_SIGNED_FIELD            (0)


//----------------------------------------------------------
// VSROFF - VS Registration Offset, Rolling Shutter Coeff Register.
#define ISPOST_VSROFF_OFFSET                   (0x021C)
#define ISPOST_VSROFF                          (ISPOST_PERI_BASE_ADDR + ISPOST_VSROFF_OFFSET)

/*
 * ISPOST, VSROFF, ROLL
 * [29:16] ROLL - Rolling shutter correction factor – signed value (format 1.1.12) See figure below.
 */
#define ISPOST_VSROFF_ROLL_MASK                (0x3FFF0000)
#define ISPOST_VSROFF_ROLL_LSBMASK             (0x00003FFF)
#define ISPOST_VSROFF_ROLL_SHIFT               (16)
#define ISPOST_VSROFF_ROLL_LENGTH              (14)
#define ISPOST_VSROFF_ROLL_DEFAULT             (0)
#define ISPOST_VSROFF_ROLL_SIGNED_FIELD        (0)

/*
 * ISPOST, VSROFF, HOFF
 * [12:0] HOFF - Horizontal registration offset (pixel granularity). See figure below.
 */
#define ISPOST_VSROFF_HOFF_MASK                (0x00001FFF)
#define ISPOST_VSROFF_HOFF_LSBMASK             (0x00001FFF)
#define ISPOST_VSROFF_HOFF_SHIFT               (0)
#define ISPOST_VSROFF_HOFF_LENGTH              (13)
#define ISPOST_VSROFF_HOFF_DEFAULT             (0)
#define ISPOST_VSROFF_HOFF_SIGNED_FIELD        (0)


//----------------------------------------------------------
// VSRVOFF - VS Registration Vertical Offset Register.
#define ISPOST_VSRVOFF_OFFSET                  (0x0220)
#define ISPOST_VSRVOFF                         (ISPOST_PERI_BASE_ADDR + ISPOST_VSRVOFF_OFFSET)

/*
 * ISPOST, VSRVOFF, VOFF
 * [0] VOFF - Vertical registration offset (line granularity).
 *            Majority of vertical offset is provided by start address adjustment.
 *            For 4:2:0 formats where UV granularity provided by address adjustment would be 2 lines the VOFF provides 1 line offset.
 */
#define ISPOST_VSRVOFF_VOFF_MASK               (0x00000001)
#define ISPOST_VSRVOFF_VOFF_LSBMASK            (0x00000001)
#define ISPOST_VSRVOFF_VOFF_SHIFT              (0)
#define ISPOST_VSRVOFF_VOFF_LENGTH             (1)
#define ISPOST_VSRVOFF_VOFF_DEFAULT            (0)
#define ISPOST_VSRVOFF_VOFF_SIGNED_FIELD       (0)


//----------------------------------------------------------
// VSAXI - VS AXI ID Register.
#define ISPOST_VSAXI_OFFSET                    (0x0224)
#define ISPOST_VSAXI                           (ISPOST_PERI_BASE_ADDR + ISPOST_VSAXI_OFFSET)

/*
 * ISPOST, VSAXI, FWID
 * [15:8] FWID - FWID VS Frame write AXI Write ID.
 */
#define ISPOST_VSAXI_FWID_MASK                 (0x0000FF00)
#define ISPOST_VSAXI_FWID_LSBMASK              (0x000000FF)
#define ISPOST_VSAXI_FWID_SHIFT                (8)
#define ISPOST_VSAXI_FWID_LENGTH               (8)
#define ISPOST_VSAXI_FWID_DEFAULT              (0)
#define ISPOST_VSAXI_FWID_SIGNED_FIELD         (0)

/*
 * ISPOST, VSAXI, RDID
 * [7:0] RDID - RDID VS Image registration AXI Read ID.
 */
#define ISPOST_VSAXI_RDID_MASK                 (0x000000FF)
#define ISPOST_VSAXI_RDID_LSBMASK              (0x000000FF)
#define ISPOST_VSAXI_RDID_SHIFT                (0)
#define ISPOST_VSAXI_RDID_LENGTH               (8)
#define ISPOST_VSAXI_RDID_DEFAULT              (0)
#define ISPOST_VSAXI_RDID_SIGNED_FIELD         (0)



//==========================================================
// Video link registers offset.
//----------------------------------------------------------
// VLINK - Video Link Control Register.
#define ISPOST_VLINK_OFFSET                    (0x0240)
#define ISPOST_VLINK                           (ISPOST_PERI_BASE_ADDR + ISPOST_VLINK_OFFSET)

/*
 * ISPOST, VLINK, CTRL
 * [31:0] CTRL - Control value.
 *               Default value: 00000001H
 */
#define ISPOST_VLINK_CTRL_MASK                 (0xFFFFFFFF)
#define ISPOST_VLINK_CTRL_LSBMASK              (0xFFFFFFFF)
#define ISPOST_VLINK_CTRL_SHIFT                (0)
#define ISPOST_VLINK_CTRL_LENGTH               (32)
#define ISPOST_VLINK_CTRL_DEFAULT              (0)
#define ISPOST_VLINK_CTRL_SIGNED_FIELD         (0)

/*
 * ISPOST, VLINK, RSRV
 * [31:12] RSRV - Reserved - Future debugging use.
 */
#define ISPOST_VLINK_RSRV_MASK                 (0xFFFFF000)
#define ISPOST_VLINK_RSRV_LSBMASK              (0x000FFFFF)
#define ISPOST_VLINK_RSRV_SHIFT                (12)
#define ISPOST_VLINK_RSRV_LENGTH               (20)
#define ISPOST_VLINK_RSRV_DEFAULT              (0)
#define ISPOST_VLINK_RSRV_SIGNED_FIELD         (0)

/*
 * ISPOST, VLINK, DATA_SELECT
 * [11] DATA_SELECT - The bit specifies the vlink input data coming from scaler 0 or unscaled output.
 *                    0: means unscaled output.
 *                    1: means scaler 0.
 */
#define ISPOST_VLINK_DATA_SELECT_MASK          (0x00000800)
#define ISPOST_VLINK_DATA_SELECT_LSBMASK       (0x00000001)
#define ISPOST_VLINK_DATA_SELECT_SHIFT         (11)
#define ISPOST_VLINK_DATA_SELECT_LENGTH        (1)
#define ISPOST_VLINK_DATA_SELECT_DEFAULT       (0)
#define ISPOST_VLINK_DATA_SELECT_SIGNED_FIELD  (0)

/*
 * ISPOST, VLINK, CMD_FIFO_THRESHOLD
 * [10:6] CMD_FIFO_THRESHOLD - Command FIFO threshold.
 */
#define ISPOST_VLINK_CMD_FIFO_THRESHOLD_MASK         (0x000007C0)
#define ISPOST_VLINK_CMD_FIFO_THRESHOLD_LSBMASK      (0x0000001F)
#define ISPOST_VLINK_CMD_FIFO_THRESHOLD_SHIFT        (6)
#define ISPOST_VLINK_CMD_FIFO_THRESHOLD_LENGTH       (5)
#define ISPOST_VLINK_CMD_FIFO_THRESHOLD_DEFAULT      (0)
#define ISPOST_VLINK_CMD_FIFO_THRESHOLD_SIGNED_FIELD (0)

/*
 * ISPOST, VLINK, HMB
 * [5] HMB - The bit specifies the image height can be divided by 16 (MB height) or not. One means yes.
 *           0 : Can not be divided by 16.
 *           1 : Can be divided by 16.
 */
#define ISPOST_VLINK_HMB_MASK                  (0x00000020)
#define ISPOST_VLINK_HMB_LSBMASK               (0x00000001)
#define ISPOST_VLINK_HMB_SHIFT                 (5)
#define ISPOST_VLINK_HMB_LENGTH                (1)
#define ISPOST_VLINK_HMB_DEFAULT               (0)
#define ISPOST_VLINK_HMB_SIGNED_FIELD          (0)

/*
 * ISPOST, VLINK, FIFO_THRESHOLD
 * [4:0] FIFO_THRESHOLD - Data FIFO threshold. 
 *                        Default value: 00000001H
 */
#define ISPOST_VLINK_FIFO_THRESHOLD_MASK         (0x0000001F)
#define ISPOST_VLINK_FIFO_THRESHOLD_LSBMASK      (0x0000001F)
#define ISPOST_VLINK_FIFO_THRESHOLD_SHIFT        (0)
#define ISPOST_VLINK_FIFO_THRESHOLD_LENGTH       (5)
#define ISPOST_VLINK_FIFO_THRESHOLD_DEFAULT      (0)
#define ISPOST_VLINK_FIFO_THRESHOLD_SIGNED_FIELD (0)


//----------------------------------------------------------
// VLINKSA - Start Address Register.
#define ISPOST_VLINKSA_OFFSET                  (0x0244)
#define ISPOST_VLINKSA                         (ISPOST_PERI_BASE_ADDR + ISPOST_VLINKSA_OFFSET)

/*
 * ISPOST, VLINKSA, SAD
 * [31:3] SAD - Luminance plane base address in QWORD (64bit) boundary.
 *              Chrominance plane must directly follow the luminance plane.
 */
#define ISPOST_VLINKSA_SAD_MASK                (0xFFFFFFF8)
#define ISPOST_VLINKSA_SAD_LSBMASK             (0xFFFFFFF8)
#define ISPOST_VLINKSA_SAD_SHIFT               (0)
#define ISPOST_VLINKSA_SAD_LENGTH              (32)
#define ISPOST_VLINKSA_SAD_DEFAULT             (0)
#define ISPOST_VLINKSA_SAD_SIGNED_FIELD        (0)


//----------------------------------------------------------
// ILINKSAM - ISP Link Start Address MSW Register.
#define ISPOST_ILINKSAM_OFFSET                 (0x0250)
#define ISPOST_ILINKSAM                        (ISPOST_PERI_BASE_ADDR + ISPOST_ILINKSAM_OFFSET)

/*
 * ISPOST, ILINKSAM, SADM
 * [7:0] SADM - ISP Link start address bits [39:32].
 */
#define ISPOST_ILINKSAM_SADM_MASK              (0x000000FF)
#define ISPOST_ILINKSAM_SADM_LSBMASK           (0x000000FF)
#define ISPOST_ILINKSAM_SADM_SHIFT             (0)
#define ISPOST_ILINKSAM_SADM_LENGTH            (8)
#define ISPOST_ILINKSAM_SADM_DEFAULT           (0)
#define ISPOST_ILINKSAM_SADM_SIGNED_FIELD      (0)


//----------------------------------------------------------
// ILINKSAL - ISP Link Start Address LSW Register.
#define ISPOST_ILINKSAL_OFFSET                 (0x0254)
#define ISPOST_ILINKSAL                        (ISPOST_PERI_BASE_ADDR + ISPOST_ILINKSAL_OFFSET)

/*
 * ISPOST, ILINKSAL, SADL
 * [31:4] SADL - ISP Link start address bits [31:4].
 */
#define ISPOST_ILINKSAL_SADL_MASK              (0xFFFFFFF0)
#define ISPOST_ILINKSAL_SADL_LSBMASK           (0xFFFFFFF0)
#define ISPOST_ILINKSAL_SADL_SHIFT             (0)
#define ISPOST_ILINKSAL_SADL_LENGTH            (32)
#define ISPOST_ILINKSAL_SADL_DEFAULT           (0)
#define ISPOST_ILINKSAL_SADL_SIGNED_FIELD      (0)


//----------------------------------------------------------
// ILINKCTRL0 - ISP Link Control Register 0.
#define ISPOST_ILINKCTRL0_OFFSET               (0x0258)
#define ISPOST_ILINKCTRL0                      (ISPOST_PERI_BASE_ADDR + ISPOST_ILINKCTRL0_OFFSET)

/*
 * ISPOST, ILINKCTRL0, CTRL
 * [31:0] CTRL - Control value.
 */
#define ISPOST_ILINKCTRL0_CTRL_MASK            (0xFFFFFFFF)
#define ISPOST_ILINKCTRL0_CTRL_LSBMASK         (0xFFFFFFFF)
#define ISPOST_ILINKCTRL0_CTRL_SHIFT           (0)
#define ISPOST_ILINKCTRL0_CTRL_LENGTH          (32)
#define ISPOST_ILINKCTRL0_CTRL_DEFAULT         (0)
#define ISPOST_ILINKCTRL0_CTRL_SIGNED_FIELD    (0)


//----------------------------------------------------------
// ILINKCTRL1 - ISP Link Control Register 1.
#define ISPOST_ILINKCTRL1_OFFSET               (0x025C)
#define ISPOST_ILINKCTRL1                      (ISPOST_PERI_BASE_ADDR + ISPOST_ILINKCTRL1_OFFSET)

/*
 * ISPOST, ILINKCTRL1, CTRL
 * [31:0] CTRL - Control value.
 */
#define ISPOST_ILINKCTRL1_CTRL_MASK            (0xFFFFFFFF)
#define ISPOST_ILINKCTRL1_CTRL_LSBMASK         (0xFFFFFFFF)
#define ISPOST_ILINKCTRL1_CTRL_SHIFT           (0)
#define ISPOST_ILINKCTRL1_CTRL_LENGTH          (32)
#define ISPOST_ILINKCTRL1_CTRL_DEFAULT         (0)
#define ISPOST_ILINKCTRL1_CTRL_SIGNED_FIELD    (0)

/*
 * ISPOST, ILINKCTRL1, INV
 * [31] INV - invert ISP address (enable link).
 */
#define ISPOST_ILINKCTRL1_INV_MASK             (0x80000000)
#define ISPOST_ILINKCTRL1_INV_LSBMASK          (0x00000001)
#define ISPOST_ILINKCTRL1_INV_SHIFT            (31)
#define ISPOST_ILINKCTRL1_INV_LENGTH           (1)
#define ISPOST_ILINKCTRL1_INV_DEFAULT          (0)
#define ISPOST_ILINKCTRL1_INV_SIGNED_FIELD     (0)

/*
 * ISPOST, ILINKCTRL1, BURST4W
 * [30] BURST4W - pad all bursts to 4w.
 */
#define ISPOST_ILINKCTRL1_BURST4W_MASK         (0x40000000)
#define ISPOST_ILINKCTRL1_BURST4W_LSBMASK      (0x00000001)
#define ISPOST_ILINKCTRL1_BURST4W_SHIFT        (30)
#define ISPOST_ILINKCTRL1_BURST4W_LENGTH       (1)
#define ISPOST_ILINKCTRL1_BURST4W_DEFAULT      (0)
#define ISPOST_ILINKCTRL1_BURST4W_SIGNED_FIELD (0)

/*
 * ISPOST, ILINKCTRL1, ENBURST
 * [29] ENBURST - insert bursts enable.
 */
#define ISPOST_ILINKCTRL1_ENBURST_MASK         (0x20000000)
#define ISPOST_ILINKCTRL1_ENBURST_LSBMASK      (0x00000001)
#define ISPOST_ILINKCTRL1_ENBURST_SHIFT        (29)
#define ISPOST_ILINKCTRL1_ENBURST_LENGTH       (1)
#define ISPOST_ILINKCTRL1_ENBURST_DEFAULT      (0)
#define ISPOST_ILINKCTRL1_ENBURST_SIGNED_FIELD (0)

/*
 * ISPOST, ILINKCTRL1, ENREC
 * [28] ENREC - reorder enable.
 */
#define ISPOST_ILINKCTRL1_ENREC_MASK           (0x10000000)
#define ISPOST_ILINKCTRL1_ENREC_LSBMASK        (0x00000001)
#define ISPOST_ILINKCTRL1_ENREC_SHIFT          (28)
#define ISPOST_ILINKCTRL1_ENREC_LENGTH         (1)
#define ISPOST_ILINKCTRL1_ENREC_DEFAULT        (0)
#define ISPOST_ILINKCTRL1_ENREC_SIGNED_FIELD   (0)

/*
 * ISPOST, ILINKCTRL1, RSLAST
 * [23:16] RSLAST - reorder sequence last, set bit to 1 for last sel0, sel1 in burst sequence.
 */
#define ISPOST_ILINKCTRL1_RSLAST_MASK          (0x00FF0000)
#define ISPOST_ILINKCTRL1_RSLAST_LSBMASK       (0x000000FF)
#define ISPOST_ILINKCTRL1_RSLAST_SHIFT         (16)
#define ISPOST_ILINKCTRL1_RSLAST_LENGTH        (8)
#define ISPOST_ILINKCTRL1_RSLAST_DEFAULT       (0)
#define ISPOST_ILINKCTRL1_RSLAST_SIGNED_FIELD  (0)

/*
 * ISPOST, ILINKCTRL1, RSSEL1
 * [15:8] RSSEL1 - reorder sequence sel1.
 *                 Y0: {sel1,sel0}=00, Y1: {sel1,sel0}=01, UV: {sel1,sel0}=10.
 *                 For YUV420: ISP burst sequence: UV, Y0, Y1(last)
 */
#define ISPOST_ILINKCTRL1_RSSEL1_MASK          (0x0000FF00)
#define ISPOST_ILINKCTRL1_RSSEL1_LSBMASK       (0x000000FF)
#define ISPOST_ILINKCTRL1_RSSEL1_SHIFT         (8)
#define ISPOST_ILINKCTRL1_RSSEL1_LENGTH        (8)
#define ISPOST_ILINKCTRL1_RSSEL1_DEFAULT       (0)
#define ISPOST_ILINKCTRL1_RSSEL1_SIGNED_FIELD  (0)

/*
 * ISPOST, ILINKCTRL1, RSSEL0
 * [7:0] RSSEL0 - reorder sequence sel0.
 *                Y0: {sel1,sel0}=00, Y1: {sel1,sel0}=01, UV: {sel1,sel0}=10.
 *                For YUV420: ISP burst sequence: UV, Y0, Y1(last)
 */
#define ISPOST_ILINKCTRL1_RSSEL0_MASK          (0x000000FF)
#define ISPOST_ILINKCTRL1_RSSEL0_LSBMASK       (0x000000FF)
#define ISPOST_ILINKCTRL1_RSSEL0_SHIFT         (0)
#define ISPOST_ILINKCTRL1_RSSEL0_LENGTH        (8)
#define ISPOST_ILINKCTRL1_RSSEL0_DEFAULT       (0)
#define ISPOST_ILINKCTRL1_RSSEL0_SIGNED_FIELD  (0)


//----------------------------------------------------------
// ILINKSTAT0 - ISP Link Status Register 0.
#define ISPOST_ILINKSTAT0_OFFSET               (0x0260)
#define ISPOST_ILINKSTAT0                      (ISPOST_PERI_BASE_ADDR + ISPOST_ILINKSTAT0_OFFSET)

/*
 * ISPOST, ILINKSTAT0, STAT
 * [31:0] STAT - Status value.
 */
#define ISPOST_ILINKSTAT0_STAT_MASK            (0xFFFFFFFF)
#define ISPOST_ILINKSTAT0_STAT_LSBMASK         (0xFFFFFFFF)
#define ISPOST_ILINKSTAT0_STAT_SHIFT           (0)
#define ISPOST_ILINKSTAT0_STAT_LENGTH          (32)
#define ISPOST_ILINKSTAT0_STAT_DEFAULT         (0)
#define ISPOST_ILINKSTAT0_STAT_SIGNED_FIELD    (0)


//----------------------------------------------------------
// ILINKCTRL2 - ISP Link Control Register 2.
#define ISPOST_ILINKCTRL2_OFFSET               (0x0264)
#define ISPOST_ILINKCTRL2                      (ISPOST_PERI_BASE_ADDR + ISPOST_ILINKCTRL2_OFFSET)

/*
 * ISPOST, ILINKCTRL2, CTRL
 * [31:0] CTRL - Control value.
 */
#define ISPOST_ILINKCTRL2_CTRL_MASK            (0xFFFFFFFF)
#define ISPOST_ILINKCTRL2_CTRL_LSBMASK         (0xFFFFFFFF)
#define ISPOST_ILINKCTRL2_CTRL_SHIFT           (0)
#define ISPOST_ILINKCTRL2_CTRL_LENGTH          (32)
#define ISPOST_ILINKCTRL2_CTRL_DEFAULT         (0)
#define ISPOST_ILINKCTRL2_CTRL_SIGNED_FIELD    (0)

/*
 * ISPOST, ILINKCTRL2, BURSTL
 * [23:16] BURSTL - burst_insert_len - insert burst_insert_len 4word bursts.
 */
#define ISPOST_ILINKCTRL2_BURSTL_MASK          (0x00FF0000)
#define ISPOST_ILINKCTRL2_BURSTL_LSBMASK       (0x000000FF)
#define ISPOST_ILINKCTRL2_BURSTL_SHIFT         (16)
#define ISPOST_ILINKCTRL2_BURSTL_LENGTH        (8)
#define ISPOST_ILINKCTRL2_BURSTL_DEFAULT       (0)
#define ISPOST_ILINKCTRL2_BURSTL_SIGNED_FIELD  (0)

/*
 * ISPOST, ILINKCTRL2, BURSTW
 * [11:0] BURSTW - burst_insert_w - insert burst_insert_len 4word bursts after burst_insert_w input bursts.
 *                 For YUV 420: 3 bursts = 64 pixels.
 *                 For YUV 422: 2 bursts = 64 pixels.
 */
#define ISPOST_ILINKCTRL2_BURSTW_MASK          (0x00000FFF)
#define ISPOST_ILINKCTRL2_BURSTW_LSBMASK       (0x00000FFF)
#define ISPOST_ILINKCTRL2_BURSTW_SHIFT         (0)
#define ISPOST_ILINKCTRL2_BURSTW_LENGTH        (12)
#define ISPOST_ILINKCTRL2_BURSTW_DEFAULT       (0)
#define ISPOST_ILINKCTRL2_BURSTW_SIGNED_FIELD  (0)


//----------------------------------------------------------
// ILINKCTRL3 - ISP Link Control Register 3.
#define ISPOST_ILINKCTRL3_OFFSET               (0x0268)
#define ISPOST_ILINKCTRL3                      (ISPOST_PERI_BASE_ADDR + ISPOST_ILINKCTRL3_OFFSET)

/*
 * ISPOST, ILINKCTRL3, CTRL
 * [31:0] CTRL - Control value.
 */
#define ISPOST_ILINKCTRL3_CTRL_MASK            (0xFFFFFFFF)
#define ISPOST_ILINKCTRL3_CTRL_LSBMASK         (0xFFFFFFFF)
#define ISPOST_ILINKCTRL3_CTRL_SHIFT           (0)
#define ISPOST_ILINKCTRL3_CTRL_LENGTH          (32)
#define ISPOST_ILINKCTRL3_CTRL_DEFAULT         (0)
#define ISPOST_ILINKCTRL3_CTRL_SIGNED_FIELD    (0)



//==========================================================
//===== Dump Image to Memory Register =====
//----------------------------------------------------------
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


//----------------------------------------------------------
// Dump Image to Memory : Start Address Register.
#define ISPOST_DUMPA_OFFSET                    (0x0104)
#define ISPOST_DUMPA                           (ISPOST_PERI_BASE_ADDR + ISPOST_DUMPA_OFFSET)

/*
 * ISPOST, DUMPA, SAD
 * [31:3] Base address in QWORD (64bit) boundary.
 */
#define ISPOST_DUMPA_SAD_MASK                  (0xFFFFFFF8)
#define ISPOST_DUMPA_SAD_LSBMASK               (0xFFFFFFF8)
#define ISPOST_DUMPA_SAD_SHIFT                 (0)
#define ISPOST_DUMPA_SAD_LENGTH                (32)
#define ISPOST_DUMPA_SAD_DEFAULT               (0)
#define ISPOST_DUMPA_SAD_SIGNED_FIELD          (0)


//----------------------------------------------------------
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

//----------------------------------------------------------
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
#endif //__cplusplus


#endif //__IMAPX800_ISPOST_Q3F_REGS_H__
