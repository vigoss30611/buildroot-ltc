cmake_minimum_required(VERSION 2.8)

project(ImgLib_ListUtils)
message(STATUS "list_utils")

# ----------------------------------------------------------------------
# Find this library
# ----------------------------------------------------------------------
find_package(ImgLib_ListUtils REQUIRED)
find_package(ImgIncludes REQUIRED)

include_directories(
	${IMGLIB_LISTUTILS_INCLUDE_DIRS}
	${IMGINCLUDES_INCLUDE_DIRS}
)

add_definitions(
	${IMGLIB_LISTUTILS_DEFINITIONS}
	${IMGINCLUDES_DEFINITIONS}
)

# ----------------------------------------------------------------------
# List of headers
# ----------------------------------------------------------------------
set(INCLUDES
	include/lst.h
	include/tre.h
	include/dq.h    
)
# ----------------------------------------------------------------------
# List of sources
# ----------------------------------------------------------------------
set(SOURCES ${SOURCES}
	src/dq/dq.c
	src/lst/lst.c
	src/trees/addchild.c
	src/trees/addsib.c
	src/trees/copysub.c
	src/trees/finalsib.c
	src/trees/firstchild.c
	src/trees/init.c
	src/trees/isinsub.c
	src/trees/nextsib.c
	src/trees/prevsib.c
	src/trees/remleaf.c
	src/trees/remlsub.c
	src/trees/remsub.c
	src/trees/subnext.c
	)

if(NOT IMG_KERNEL_MODULE)
	# ----------------------------------------------------------------------
	# Build the library
	# ----------------------------------------------------------------------
	add_library(${IMGLIB_LISTUTILS_LIBRARIES} STATIC ${SOURCES}  ${INCLUDES})

else()
	# ----------------------------------------------------------------------
	# Add information needed for KM build - used in viddec
	# ----------------------------------------------------------------------
	IMG_KM_ADD_INCLUDE_DIRS()
	IMG_KM_ADD_DEFINITIONS()
	IMG_KM_ADD_SOURCE("${SOURCES}")
endif()


