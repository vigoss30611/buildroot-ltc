#
# this file defines the source and include files needed if a kernel module
# as to be build using this library
#
# module_KO_SOURCES - sources files to add to the kernel module
# module_KO_INCLUDE_DIRS - external headers path that could be needed
# module_KO_DEFINITIONS
#
find_package(TAL REQUIRED)

if(${TAL_TYPE} MATCHES "light")

  #message("building TAL light in a kernel module is experimental")

  # TAL_LIGHT defined in FindTAL already
  #set(TAL_KO_DEFINITIONS ${TAL_KO_DEFINITIONS} -DTAL_LIGHT)

  # add the headers to the source so that they are linked into the kernel module folder instead of added as -I
  set(TAL_KO_SOURCES ${TAL_KO_SOURCES}

    # common sources
    ${TAL_PREFIX}/libraries/tal/common/code/tal_interrupt.c
    ${TAL_PREFIX}/libraries/tal/common/code/tal_test.c

    # tal light with no options
    ${TAL_PREFIX}/libraries/tal/light/code/tal.c
    ${TAL_PREFIX}/libraries/target_config/code/target_light.c

    # headers from sources
    ${TAL_PREFIX}/include/tal.h
    ${TAL_PREFIX}/libraries/tal/include/tal_intdefs.h
    ${TAL_PREFIX}/include/target.h

    # headers from headers
    ${TAL_PREFIX}/include/tal_defs.h
    ${TAL_PREFIX}/include/tal_reg.h
    ${TAL_PREFIX}/include/tal_mem.h
    ${TAL_PREFIX}/include/tal_vmem.h
    ${TAL_PREFIX}/include/tal_debug.h
    ${TAL_PREFIX}/include/tal_intvar.h
    ${TAL_PREFIX}/include/tal_pdump.h
    ${TAL_PREFIX}/include/tal_setup.h
    ${TAL_PREFIX}/include/tal_internal.h
    ${TAL_PREFIX}/include/pdump_cmds.h
    ${TAL_PREFIX}/include/binlite.h
    )

  if (TAL_ADD_UTILS)
    set(TAL_KO_SOURCES ${TAL_KO_SOURCES}
      ${TAL_PREFIX}/libraries/addr_alloc/code/addr_alloc.c
      ${TAL_PREFIX}/libraries/utils/code/hash.c
      ${TAL_PREFIX}/libraries/utils/code/pool.c
      ${TAL_PREFIX}/libraries/utils/code/trace.c
      ${TAL_PREFIX}/libraries/utils/code/ra.c

      ${TAL_PREFIX}/include/addr_alloc.h
      ${TAL_PREFIX}/include/ra.h
      ${TAL_PREFIX}/include/hash.h
      ${TAL_PREFIX}/include/pool.h
      ${TAL_PREFIX}/include/utils.h
      ${TAL_PREFIX}/include/trace.h
      )
  endif()

endif() # TAL Light in kernel
