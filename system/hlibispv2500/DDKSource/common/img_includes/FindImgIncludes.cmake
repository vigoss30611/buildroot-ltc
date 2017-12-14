cmake_minimum_required (VERSION 2.8)

# ----------------------------------------------------------------------
# Locate and configure the ImgIncludes library.
#
# Defines the following variables:
#
#   IMGINCLUDES_FOUND        - Found the library
#   IMGINCLUDES_PREFIX       - The directory where this file is located
#   IMGINCLUDES_INCLUDE_DIRS - The directories needed on the include paths
#                              for this library
#   IMGINCLUDES_DEFINITIONS  - The definitions to apply for this library
#   IMGINCLUDES_LIBRARIES    - The libraries to link to
#   IMGINCLUDES_BUILD_KO     - The file to include to know which files are needed when building a kernel module
# ----------------------------------------------------------------------

get_filename_component(IMGINCLUDES_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)

#set (ImgIncludes_VERSION "2")
#message ("version def=${ImgIncludes_VERSION} pck_find=${PACKAGE_FIND_VERSION_MAJOR} pck=${PACKAGE_VERSION}")

if (NOT DEFINED IMGINCLUDES_FORCE_WIN32)
    set (IMGINCLUDES_FORCE_WIN32 FALSE)
endif()
if (NOT DEFINED IMGINCLUDES_FORCE_C99)
    set (IMGINCLUDES_FORCE_C99 FALSE)
endif()
if (NOT DEFINED IMGINCLUDES_FORCE_KERNEL)
    set (IMGINCLUDES_FORCE_KERNEL FALSE)
endif()
if (NOT DEFINED IMGINCLUDES_EXIT_ON_ASSERT)
    set (IMGINCLUDES_EXIT_ON_ASSERT TRUE)
endif()
if (NOT DEFINED IMGINCLUDES_MALLOC_TEST)
    set (IMGINCLUDES_MALLOC_TEST FALSE)
endif()
if (NOT DEFINED IMGINCLUDES_MALLOC_CHECK)
    set (IMGINCLUDES_MALLOC_CHECK FALSE)
endif()

set(IMGINCLUDES_INCLUDE_DIRS ${IMGINCLUDES_PREFIX})
set(IMGINCLUDES_DEFINITIONS)
set(IMGINCLUDES_LIBRARIES ImgIncludes) #Used for error to string function
set(IMGINCLUDES_FOUND TRUE)
set(IMGINCLUDES_BUILD_KO ${IMGINCLUDES_PREFIX}/ImgIncludes_kernel.cmake)

if (IMGINCLUDES_EXIT_ON_ASSERT)
    set (IMGINCLUDES_DEFINITIONS ${IMGINCLUDES_DEFINITIONS} -DEXIT_ON_ASSERT)
endif()
if (IMGINCLUDES_MALLOC_TEST)
	if (DEBUG_MODULES)
		message ("IMG Includes uses IMG_MALLOC_TEST")
	endif()
	set (IMGINCLUDES_DEFINITIONS ${IMGINCLUDES_DEFINITIONS} -DIMG_MALLOC_TEST)
endif()
if (IMGINCLUDES_MALLOC_CHECK)
	if (DEBUG_MODULES)
		message ("IMG Includes uses IMG_MALLOC_CHECK")
	endif()
	set (IMGINCLUDES_DEFINITIONS ${IMGINCLUDES_DEFINITIONS} -DIMG_MALLOC_CHECK)
endif()

if (WIN32 OR IMGINCLUDES_FORCE_WIN32)

	set (IMGINCLUDES_INCLUDE_DIRS ${IMGINCLUDES_INCLUDE_DIRS}
		${IMGINCLUDES_PREFIX}/ms
	)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux"         OR
       ${CMAKE_SYSTEM_NAME} MATCHES "Linux-Android" OR
       ${CMAKE_SYSTEM_NAME} MATCHES "Darwin"        OR
       ${CMAKE_SYSTEM_NAME} MATCHES "MTX"           OR
       IMGINCLUDES_FORCE_C99                        OR
       IMGINCLUDES_FORCE_KERNEL)
    if (NOT IMGINCLUDES_FORCE_KERNEL)
        set (IMGINCLUDES_INCLUDE_DIRS ${IMGINCLUDES_INCLUDE_DIRS}
            ${IMGINCLUDES_PREFIX}/c99
        )
    else() # kernel
        set (IMGINCLUDES_INCLUDE_DIRS ${IMGINCLUDES_INCLUDE_DIRS}
            ${IMGINCLUDES_PREFIX}/linux-kernel
        )
    endif()
else ( )
    message(FATAL_ERROR "Target system '${CMAKE_SYSTEM_NAME}' not suported by ImgIncludes!")
endif()

if (IMGINCLUDES_OLD_VARS)
    set (IMGINC_INCLUDE_DIRS ${IMGINCLUDES_INCLUDE_DIRS})
    set (IMGINC_DEFINITIONS ${IMGINCLUDES_DEFINITIONS})
    set (IMGINC_FOUND TRUE)
    mark_as_advanced(IMGINC_INCLUDE_DIRS IMGINC_DEFINITIONS)
    message("IMG includes: IMGINCLDUES_OLD_VARS should not be used")
endif()

if (DEBUG_MODULES)
    message ("IMGINCLUDES_INCLUDE_DIRS=${IMGINCLUDES_INCLUDE_DIRS}")
    message ("IMGINCLUDES_DEFINITIONS=${IMGINCLUDES_DEFINITIONS}")
endif()

mark_as_advanced(IMGINCLUDES_INCLUDE_DIRS IMGINCLUDES_DEFINITIONS IMGINCLUDES_LIBRARIES)
