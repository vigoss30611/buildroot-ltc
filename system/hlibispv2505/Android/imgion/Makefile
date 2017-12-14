
ifndef KERNEL_PATH
$(error "KERNEL_PATH environment variable not found, please define")
endif

MODULE_PATH=$(PWD)

# check wehre the heeader lies and define the flags for ko
ifeq ($(wildcard $(KERNEL_PATH)/drivers/staging/android/ion/ion.h),)
ccflags-y += -DIMG_KERNEL_ION_HEADER='<linux/ion.h>'
ccflags-y += -DIMG_KERNEL_ION_PRIV_HEADER="<$(srctree)/drivers/gpu/ion/ion_priv.h>"
else
ccflags-y += -DIMG_KERNEL_ION_HEADER="<$(srctree)/drivers/staging/android/ion/ion.h>"
ccflags-y += -DIMG_KERNEL_ION_PRIV_HEADER="<$(srctree)/drivers/staging/android/ion/ion_priv.h>"
endif

obj-m += imgion.o

module: 
	$(MAKE) -C $(KERNEL_PATH) M=$(MODULE_PATH) modules

all: module 
	

install: all
	@INSTALL_MOD_PATH=$(ANDROID_PRODUCT_OUT)/system $(MAKE) -C $(KERNEL_PATH) M=$(MODULE_PATH) modules_install

clean: 
	@$(MAKE) -C $(KERNEL_PATH) M=$(MODULE_PATH) clean

.PHONY: all install clean
