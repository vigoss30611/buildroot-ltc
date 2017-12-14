#
# this file defines the source and include files needed if a kernel module
# as to be build using this library
#
# module_KO_SOURCES - sources files to add to the kernel module
# module_KO_INCLUDE_DIRS - external headers path that could be needed
# module_KO_DEFINITIONS
#

set(FELIXDG_KO_INCLUDE_DIRS)
set(FELIXDG_KO_DEFINITIONS
    #-DDEBUG_FIRST_LINE # dump some convertion info
)
set(FELIXDG_KO_SOURCES
  ${FELIXDG_PREFIX}/kernel/dg_io_km.c
  ${FELIXDG_PREFIX}/kernel/dg_connection.c
  ${FELIXDG_PREFIX}/kernel/dg_camera.c
  ${FELIXDG_PREFIX}/kernel/dg_internal.c
  )
