cmake_minimum_required(VERSION 2.8)

set(IMGINCLUDES_FORCE_KERNEL FALSE) # this part is not in a kernel module - make sure of it
find_package(ImgIncludes REQUIRED)
find_package(FelixAPI REQUIRED) # felix internal finds felix API
#finds RegDefs
find_package(LinkedList REQUIRED)
#find_package(FelixCommon REQUIRED) # done when finding FelixAPI
find_package(SimImage REQUIRED)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ci_version.h.cmakein)
  # if the cmake template exists the ci_version is generated
  set(VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/include/ci/ci_version.h)
  configure_file(ci_version.h.cmakein ${VERSION_HEADER}) # uses CI_CHANGELIST and CI_DATE
endif()

include_directories(
  ${IMGINCLUDES_INCLUDE_DIRS}
  ${FELIXAPI_INCLUDE_DIRS}
  ${FelixLib_SOURCE_DIR}/kernel/include # include dirs is needed for the ioctl
  ${LINKEDLIST_INCLUDE_DIRS}
  ${FELIXCOMMON_INCLUDE_DIRS}
  ${SIMIMAGE_INCLUDE_DIRS}
  )

add_definitions(
  ${IMGINCLUDES_DEFINITIONS}
  ${LINKEDLIST_DEFINITIONS}
  ${FELIXAPI_DEFINITIONS}
  ${FELIXCOMMON_DEFINITIONS}
  ${SIMIMAGE_DEFINITIONS}
  )

set(MC_SOURCES
  source/mc_config.c
  source/mc_convert.c
  source/mc_pipeline.c
  source/mc_extract.c
  )
# this list is duplicated in unit tests
set(SOURCES
  source/ci_api.c
  source/ci_pipeline.c
  source/ci_intdg.c
  #source/ci_gasket.c
  source/ci_modulesdefaults.c
  ${MC_SOURCES}
  )

set(MC_HEADERS
  include/mc/module_config.h
  include/mc/mc_structs.h
  )
set(HEADERS
  include/ci/ci_api.h
  include/ci/ci_api_structs.h
  include/ci/ci_modules.h
  include/ci/ci_modules_structs.h
  include/ci/ci_api_internal.h 
  include/sys/sys_userio.h
  ${VERSION_HEADER}
  ${MC_HEADERS}
  )

if (NOT CI_BUILD_KERNEL_MODULE)
  set(SOURCES ${SOURCES} source/sys_userio_fake.c)
  set(HEADERS ${HEADERS} include/sys/sys_userio_fake.h)
else()
  #set(SOURCES ${SOURCES} source/sys_userio.c)
endif()
link_libraries(hlibcamsensor)
add_library(${FELIXAPI_NAME}_User ${SOURCES} ${HEADERS})

if (${MATH_NEON_ON} MATCHES "ON")
	target_link_libraries(${FELIXAPI_NAME}_User mneon ${FELIXINTERNAL_LIBRARIES} ${LINKEDLIST_LIBRARIES} ${FELIXCOMMON_LIBRARIES} ${SIMIMAGE_LIBRARIES})
else()
	target_link_libraries(${FELIXAPI_NAME}_User ${FELIXINTERNAL_LIBRARIES} ${LINKEDLIST_LIBRARIES} ${FELIXCOMMON_LIBRARIES} ${SIMIMAGE_LIBRARIES})
endif()
add_dependencies(${FELIXAPI_NAME}_User ${FELIXAPI_DEPENDENCIES})

source_group(MC_H FILES ${MC_HEADERS})
source_group(MC FILES ${MC_SOURCES})

#install(TARGETS ${FELIXAPI_NAME}_User DESTINATION ./CI/lib)
#install(FILES ${HEADERS} DESTINATION ./CI/include)

if(NOT CI_BUILD_KERNEL_MODULE)
  target_link_libraries(${FELIXAPI_NAME}_User ${FELIXAPI_NAME}_Kernel) # should not be needed as we put both in FELIXAPI_LIBRARIES

endif()
