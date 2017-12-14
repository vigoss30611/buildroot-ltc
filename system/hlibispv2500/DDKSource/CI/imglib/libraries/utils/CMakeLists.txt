cmake_minimum_required(VERSION 2.8)

# ----------------------------------------------------------------------
# Find this library
# ----------------------------------------------------------------------
find_package(ImgLib_UTILS REQUIRED)

# ----------------------------------------------------------------------
# External dependencies
# ----------------------------------------------------------------------
find_package(ImgIncludes REQUIRED)
#find_package(MeOSAL MEOSAL REQUIRED)

#
include_directories(
	${IMGLIB_UTILS_INCLUDE_DIRS}
	${IMGINCLUDES_INCLUDE_DIRS}
	#${MEOSAL_INCLUDE_DIRS}
)

add_definitions(
	${IMGLIB_UTILS_DEFINITIONS}
	${IMGINCLUDES_DEFINITIONS}
	#${MEOSAL_DEFINITIONS}
)

# ----------------------------------------------------------------------
# List of sources
# ----------------------------------------------------------------------
set(HEADERS
	include/img_utils.h
)

set(EXTRA
	FindImgLib_UTILS.cmake
)

set(SOURCES
	code/imglib_misc_utils.c
)

if(NOT IMG_KERNEL_MODULE)
	# ----------------------------------------------------------------------
	# Build the library
	# ----------------------------------------------------------------------
	add_library(ImgLib_UTILS STATIC ${SOURCES} ${HEADERS} ${EXTRA})
	#target_link_libraries(ImgLib_UTILS ${MEOSAL_LIBRARIES})
else()
	# ----------------------------------------------------------------------
	# Add information needed for KM build
	# ----------------------------------------------------------------------
	IMG_KM_ADD_INCLUDE_DIRS()
	IMG_KM_ADD_DEFINITIONS()
	IMG_KM_ADD_SOURCE("${SOURCES}")
#	IMG_KM_ADD_RPC_SERVER_FILES("${IMG_RPC_SERVER_FILES}")
endif()
