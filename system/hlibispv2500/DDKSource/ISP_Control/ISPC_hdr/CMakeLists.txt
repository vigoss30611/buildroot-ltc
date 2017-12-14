cmake_minimum_required(VERSION 2.8)

find_package(ImgIncludes REQUIRED)
find_package(ISPC2 REQUIRED)
find_package(DynCMD REQUIRED)

include_directories(
  ${IMGINCLUDES_INCLUDE_DIRS}
  ${ISPC2_INCLUDE_DIRS}
  ${ISPC2TEST_INCLUDE_DIRS}
  ${DYNCMD_INCLUDE_DIRS}
)

add_definitions(
  ${IMGINCLUDES_DEFINITIONS}
  ${ISPC2_DEFINITIONS}
  ${ISPC2TEST_DEFINITIONS}
  ${DYNCMD_DEFINITIONS}
)

set(LINK_LIBS ${ISPC2_LIBRARIES} ${ISPC2TEST_LIBRARIES} ${DYNCMD_LIBRARIES})

message(STATUS "Build ISPC_hdr")

add_executable(ISPC_hdr ISPC_hdr.cpp)
target_link_libraries(ISPC_hdr ${LINK_LIBS})

#install(TARGETS ISPC_hdr DESTINATION ./ISPC)
