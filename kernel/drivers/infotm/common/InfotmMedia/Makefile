#
# Makefile for the kernel multimedia device drivers.
#


include drivers/infotm/common/InfotmMedia/env.mk

#
# conflict check
#
#################imapx800######################
ifeq ($(SOC), imapx800)
endif

##################imapx900#####################
ifeq ($(SOC), imapx900)
endif


IM_EXTERNAL_PROJECT_ROOT := external/project

ifeq ($(CONFIG_INFOTM_MEDIA_SUPPORT), y)
obj-y += foundations/
obj-y += utlz_lib/source/
obj-y += $(IM_EXTERNAL_PROJECT_ROOT)/venc_h265_v4/
obj-y += $(IM_EXTERNAL_PROJECT_ROOT)/jenc/
obj-y += $(IM_EXTERNAL_PROJECT_ROOT)/fpga_venc_264_h1/
obj-y += $(IM_EXTERNAL_PROJECT_ROOT)/fpga_venc_265_h2/
endif

ifeq ($(CONFIG_INFOTM_MEDIA_DMMU_SUPPORT), y)
obj-y += $(IM_EXTERNAL_PROJECT_ROOT)/dmmu_drv/
obj-y += dmmu_lib/source/
endif

ifeq ($(CONFIG_INFOTM_MEDIA_VDEC_G1V0_SUPPORT), y)
obj-y += $(IM_EXTERNAL_PROJECT_ROOT)/vdec_drv_g1/
endif

ifeq ($(CONFIG_INFOTM_MEDIA_VENC_8270_SUPPORT), y)
#obj-y += $(IM_EXTERNAL_PROJECT_ROOT)/venc_drv_8270/
endif

ifeq ($(CONFIG_INFOTM_MEDIA_VDEC_G1_SUPPORT), y)
obj-m += $(IM_EXTERNAL_PROJECT_ROOT)/fpga_vdec_264_g1/
endif
ifeq ($(CONFIG_INFOTM_MEDIA_VDEC_G2_SUPPORT), y)
obj-m += $(IM_EXTERNAL_PROJECT_ROOT)/fpga_vdec_265_g2/
endif
