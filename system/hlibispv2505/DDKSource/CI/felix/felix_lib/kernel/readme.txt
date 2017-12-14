This is the kernel module for the V2500 drivers copyright by Imagination 
Technologies Ltd under MIT and/or GPL license provided in MIT_COPYING and 
GPLHEADER.

The provided Makefile is an example of linux build. Using other build options 
and platform may generate different CFLAGS and files to be used as source when 
using the DDK options.

Build as a kernel module as `make -C <kernel build> M=$PWD/km modules`

It contains sources for different platform support (refer to Platform Guide 
delivered with DDK for more details - only 1 of them should be compiled):
- sys_device_pci_kernel.c implementation for imagination FPGA system over PCI
- sys_device_memmapped.c untested implementation for memory mapped device, will 
need alteration to contain correct addresses
- sys_device_imgvideo.c implementation using imagination FPGA system over PCI 
bus using imgvideo kernel module
- sys_device_transif_kernel.c transif implementation when using C-Simulator in 
QEmu systems

It also contains different allocator support (only 1 of them should be 
compiled):
- sys_mem_carveout.c implementation of carved-out memory reservation
- sys_mem_pagealloc.c implementation using global memory page by page
- sys_mem_imgvideo.c implementation using imgvideo kernel module (most be with 
imgvideo device)
- sys_mem_ion.c using for android systems 

If using Imagination's SCB block the i2c-img.o should be added to the objects
and -DIMG_SCB_DRIVER and -DSCB_REF_CLK=<clock in MHz>
e.g.
ccflags-y += -DIMG_SCB_DRIVER -DSCB_REF_CLK=40
Felix-objs += i2c-img.o

If using Imagination's FPGA and the PCI implementation it is possible to enable
PDP support by adding the -DCI_PDP ccflags.

DebugFS can be enabled with -DCI_DEBUGFS added to the ccflags. Removing that
flag will just disable DebugFS support.

See the Getting Started Guide provided with the DDK for insmod parameters.

Copyright Imagination Technologies Ltd. All Rights Reserved. 

Imagination Technologies Limited,  
Unit 8, HomePark Industrial Estate, 
King's Langley, Hertfordshire, 
WD4 8LZ, U.K. 
