#
# this file defines the source and include files needed if a kernel module
# as to be build using this library
#
# module_KO_SOURCES - sources files to add to the kernel module
# module_KO_INCLUDE_DIRS - external headers path that could be needed
# module_KO_DEFINITIONS
#
find_package(ImgIncludes REQUIRED)

# defines the needed headers so that we don't need to copy the whole folder into the kernel module

set (IMGINCLUDES_KO_SOURCES
  ${IMGINCLUDES_PREFIX}/img_defs.h
  ${IMGINCLUDES_PREFIX}/img_errors.h
  ${IMGINCLUDES_PREFIX}/img_include.h
  ${IMGINCLUDES_PREFIX}/img_pixfmts.h
  ${IMGINCLUDES_PREFIX}/img_structs.h
  ${IMGINCLUDES_PREFIX}/img_types.h
  # linux kernel specific one
    ${IMGINCLUDES_PREFIX}/linux-kernel/img_sysdefs.h
    ${IMGINCLUDES_PREFIX}/linux-kernel/img_systypes.h
)

set (IMGINCLUDES_KO_INCLUDE_DIRS) # empty

set (IMGINCLUDES_KO_DEFINITIONS ${IMGINCLUDES_DEFINITIONS})
