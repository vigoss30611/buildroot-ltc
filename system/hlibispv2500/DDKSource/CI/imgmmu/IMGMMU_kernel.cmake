#
# this file defines the source and include files needed if a kernel module
# as to be build using this library
#
# module_KO_SOURCES - sources files to add to the kernel module
# module_KO_INCLUDE_DIRS - external headers path that could be needed
# module_KO_DEFINITIONS
#

set (IMGMMU_KO_INCLUDE_DIRS ${IMGMMU_INCLUDE_DIRS})
set (IMGMMU_DEFINITIONS)

if (DEFINED IMGMMU_SIZE)
	set(IMGMMU_DEFINITIONS -DIMGMMU_PHYS_SIZE=${IMGMMU_SIZE})
endif()

set (IMGMMU_KO_SOURCES 
	${IMGMMU_PREFIX}/source/imgmmu.c
	${IMGMMU_PREFIX}/source/kernel_heap.c
	${IMGMMU_PREFIX}/source/mmu_defs.h # should be used at compilation time only
)
