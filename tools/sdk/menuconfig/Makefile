SRCTREE	:= $(CURDIR)
export SRCTREE

MAKE := make -s
export MAKE

mode := $(shell [ -e .config ] && cat .config | grep "CONFIG_UBOOT_BOOT_DEVICE_EMMC=y")

ifneq ($(mode),)
    uboot_mode := emmc
else
	uboot_mode := spi
endif

all:
	@./tools/qsdk.compile

config-target := 0

ifneq ($(filter config %config, $(MAKECMDGOALS)),)
	config-target := 1
endif

ifeq ($(config-target), 1)
config:
	@$(MAKE) -f $(SRCTREE)/tools/scripts/Makefile $@
	@./tools/qsdk.libs
%config:
	$(shell cp .config .config.tmp 2>/dev/null)
	@$(MAKE) -f $(SRCTREE)/tools/scripts/Makefile $@
	@./tools/qsdk.libs
else
.PHONY += help
help:
	@echo  'make - compile the system'
	@echo  'make menuconfig - select the config choice'
	@echo  'make kernel - compile the linux kernel'
	@echo  'make kernel-rebuild - recompile the linux kernel'
	@echo  'make kernel-menuconfig - config the linux kernel'
	@echo  'make uboot0 - compile the bootloader'
	@echo  'make uboot0-rebuild - recompile the bootloader'
	@echo  'make busybox - compile the busybox'
	@echo  'make busybox-rebuild - recompile the busybox'
	@echo  'make busybox-menuconfig - config the busybox'
	@echo  'clean - Remove most generated files but keep the config'
	@$(MAKE) -f $(SRCTREE)/tools/scripts/Makefile help
endif

clean:
	@echo "make clean"
	@tools/qsdk.init clean
	@$(MAKE) -f $(SRCTREE)/tools/scripts/Makefile clean

kernel-menuconfig:
	@echo "make kenerl-menuconfig"
	@tools/qsdk.kernel menuconfig

.PHONY:kernel
kernel:
	@echo "make kernel"
	@tools/qsdk.kernel clean
	@tools/qsdk.kernel config  output/product/configs/linux_defconfig
	@tools/qsdk.kernel make
	@tools/qsdk.kernel install

kernel-rebuild:
	@echo "make kernel-rebuild"
	@tools/qsdk.kernel make
	@tools/qsdk.kernel install

uboot0:
	@echo "make uboot0"
	@tools/qsdk.uboot make clean
	@tools/qsdk.uboot make uboot0 $(uboot_mode)
	@tools/qsdk.uboot install

uboot0-rebuild:
	@echo "make uboot0-rebuild"
	@tools/qsdk.uboot make uboot0 $(uboot_mode)
	@tools/qsdk.uboot install

busybox-menuconfig:
	@echo "make system-busybox-menuconfig"
	@tools/qsdk.busybox output/system  menuconfig
	@tools/qsdk.busybox output/ramdisk  menuconfig

busybox:
	@echo "make busybox"
	@tools/qsdk.busybox output/system  clean
	@tools/qsdk.busybox output/ramdisk  clean
	@tools/qsdk.busybox output/system  config  output/product/configs/busybox_defconfig
	@tools/qsdk.busybox output/system  make
	@tools/qsdk.busybox output/system  install
	@tools/qsdk.busybox output/ramdisk  config  output/product/configs/busybox_defconfig
	@tools/qsdk.busybox output/ramdisk  make
	@tools/qsdk.busybox output/ramdisk  install

busybox-rebuild:
	@echo "make busybox-rebuild"
	@tools/qsdk.busybox output/system  make
	@tools/qsdk.busybox output/system  install
	@tools/qsdk.busybox output/ramdisk  make
	@tools/qsdk.busybox output/ramdisk  install
