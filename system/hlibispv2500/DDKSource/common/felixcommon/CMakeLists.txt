
find_package(ImgIncludes REQUIRED)
find_package(FelixCommon REQUIRED)
find_package(GTest QUIET)

if (NOT DEFINED FELIXCOMMON_TESTS)
  set(FELIXCOMMON_TESTS FALSE)
endif()

message (STATUS "-- Felix Common")

include_directories(
	${IMGINCLUDES_INCLUDE_DIRS}
	${FELIXCOMMON_INCLUDE_DIRS}
)

add_definitions(
	${INGINCLUDES_DEFINITIONS}
	${FELIXCOMMON_DEFINITIONS}
)

set (SOURCES
	source/pixel_transform.c
	source/lshgrid.c
	source/dpfmap.c
    source/userlog.c
    source/ci_alloc_info.c
)

set (HEADERS
	include/felixcommon/pixel_format.h
	include/felixcommon/lshgrid.h
	include/felixcommon/dpfmap.h
    include/felixcommon/userlog.h
    include/felixcommon/ci_alloc_info.h
)

add_library(${FELIXCOMMON_NAME} ${SOURCES} ${HEADERS})

if (GTEST_FOUND AND ${FELIXCOMMON_TESTS})

    file(COPY testdata DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/tests/)
    add_subdirectory(tests)
endif()