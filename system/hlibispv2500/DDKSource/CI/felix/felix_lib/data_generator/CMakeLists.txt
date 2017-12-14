cmake_minimum_required(VERSION 2.8)

find_package(FelixDG REQUIRED)
find_package(ImgIncludes REQUIRED)
find_package(SimImage REQUIRED)
find_package(TAL REQUIRED)
find_package(RegDefs REQUIRED)
find_package(RegDefsUtils REQUIRED)
find_package(IMGMMU REQUIRED)

include_directories(
  ${SIMIMAGE_INCLUDE_DIRS}
  ${FELIXDG_INCLUDE_DIRS}
  ${IMGINCLUDES_INCLUDE_DIRS}
  ${TAL_INCLUDE_DIRS}
  ${REGDEF_INCLUDE_DIRS}
  ${REGDEFSUTILS_INCLUDE_DIRS}
  ${FelixLib_SOURCE_DIR}/kernel/include # needed for toErrno
  ${IMGMMU_INCLUDE_DIRS}
  )
add_definitions (
  ${SIMIMAGE_DEFINITIONS}
  ${IMGINCLUDES_DEFINITIONS}
  ${FELIXDG_DEFINITIONS}
  ${TAL_DEFINITIONS}
  ${REGDEFSUTILS_DEFINITIONS}
  ${LINKEDLIST_DEFINITIONS}
  ${IMGMMU_DEFINITIONS}
  #-DDG_DUMP_BUFFER # dump converted buffers into .dat
  #-DDG_DUMP_IMAGE # dump PGM of given image before convertion
  #-DDG_WRITE_GASKETS # write gasket registers as copy of DG ones
  #-DDEBUG_FIRST_LINE # write 1st line into files
  )

if (FELIX_SYSTEM_BIGENDIAN)
  #
  # PGM files are stored in big endian - no conversion needed?
  #
  message("    big endian system found!")
  add_definitions(-DDG_SYS_IS_BIG_ENDIAN)
endif()

#
# new datagen user-size
#

set (DG_UM_SOURCE
  user/dg_api.c
  user/dg_converter.c
  )

set (DG_UM_HEADERS
  include/dg/dg_api.h
  include/dg/dg_api_structs.h
  include/dg/dg_api_internal.h
  )

add_library(${FELIXDG_NAME}_User ${DG_UM_SOURCE} ${DG_UM_HEADERS})
target_link_libraries(${FELIXDG_NAME}_User ${SIMIMAGE_LIBRARIES} ${LINKEDLIST_LIBRARIES} ${FELIXAPI_NAME}_User)

#
# new datagen kernel-side
#

if (NOT CI_BUILD_KERNEL_MODULE)

  include(${FELIXDG_BUILD_KO}) # defines FELIXDG_KO_SOURCES FELIXDG_KO_DEFINITIONS

  include_directories(${FELIXDG_KO_INCLUDE_DIRS})

  add_definitions(${FELIXDG_KO_DEFINITIONS})
  set (FELIXDG_KO_HEADERS
    include/dg_kernel/dg_ioctl.h
    include/dg_kernel/dg_connection.h
    include/dg_kernel/dg_camera.h
    include/dg_kernel/dg_debug.h
    )

  set (FELIXDG_KO_SOURCES ${FELIXDG_KO_SOURCES}
    kernel/dg_debug.c
    )

  set (DG_LINKS ${FELIXLIB_NAME} ${TAL_LIBRARIES} ${LINKEDLIST_LIBRARIES})

  if (CI_DEBUG_FUNCTIONS)
    find_package(ImgLib_GZIPIO REQUIRED)

    message(STATUS "    DG Debug Functions")

    include_directories(${IMGLIB_GZIPIO_INCLUDE_DIRS})
    add_definitions(${IMGLIB_GZIPIO_DEFINITIONS}) # CI_DEBUG_FCT defined by finding FelixAPI
    set (DG_LINKS ${DG_LINKS} ${IMGLIB_GZIPIO_LIBRARIES})
  endif()

  add_library(${FELIXDG_NAME}_Kernel ${FELIXDG_KO_SOURCES} ${FELIXDG_KO_HEADERS})

  target_link_libraries(${FELIXDG_NAME}_Kernel ${DG_LINKS})
  add_dependencies(${FELIXDG_NAME}_Kernel ${REGDEF_NAME})

  target_link_libraries(${FELIXDG_NAME}_User ${FELIXDG_NAME}_Kernel)
endif()

