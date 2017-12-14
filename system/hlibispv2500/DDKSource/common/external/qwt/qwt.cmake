#
# Build Qwt library as an external project
#
# May requiere QT_BINARY_DIR to be defined if your Qt installation is not in the PATH
#
# Qwt has several build options:
# QWT_FRAMEWORK				- add lib_bundle to Qt Config = not supported
# QWT_PLOT TRUE|FALSE		- build the plot widgets
# QWT_SVG TRUE|FALSE		- build the svg support
# QWT_OPENGL TRUE|FALSE		- build OpenGL support
# QWT_WIDGETS TRUE|FALSE	- build the base widgets support
#
cmake_minimum_required(VERSION 2.8)

find_package(Qwt REQUIRED) # to have QWT_PREFIX and QWT_INSTALL_DIR

include(ExternalProject)

set(QWT_RELEASE 6.1.2)
set(QWT_MD5 9c88db1774fa7e3045af063bbde44d7d)
set(QWT_URL ${QWT_PREFIX}/qwt-${QWT_RELEASE}.tar.bz2)
set(QWT_DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/libraries/qwt/download)
set(QWT_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/libraries/qwt/source)
set(QWT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/libraries/qwt)

# QWT_INSTALL_DIR is defined in FindGTest.cmake
set(QWT_CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${QWT_INSTALL_DIR})
#set(QWT_CMAKE_ARGS ${QWT_CMAKE_ARGS} -DSKIP_INSTALL_FILES:BOOL=ON)
set(QWT_PATCH_COMMAND "")
set(QWT_CMAKE_CACHE_ARGS "")

#
# WARNING: Qwt was never cross compiler
# WARNING: Qwt was never compiled on MINGW (i.e. cygwin) nor android nor darwin
#

if (DEFINED QT_BINARY_DIR)

	if(NOT DEFINED Qt5Widgets_DIR)
		set(Qt5Widgets_DIR ${QT_BINARY_DIR}/../lib/cmake/Qt5Widgets)
		message("Qt5Widgets_DIR is not defined: guessing location from QT_BINARY_DIR: ${Qt5Widgets_DIR} (if incorrect define this variable to the correct location)")
	endif()
	if(NOT DEFINED Qt5PrintSupport_DIR)
		set(Qt5PrintSupport_DIR ${QT_BINARY_DIR}/../lib/cmake/Qt5PrintSupport)
		message("Qt5PrintSupport_DIR is not defined: guessing location from QT_BINARY_DIR: ${Qt5PrintSupport_DIR} (if incorrect define this variable to the correct location)")
	endif()
	if(NOT DEFINED DQt5Concurrent_DIR)
		set(Qt5Concurrent_DIR ${QT_BINARY_DIR}/../lib/cmake/Qt5Concurrent)
		message("Qt5Concurrent_DIR is not defined: guessing location from QT_BINARY_DIR: ${Qt5Concurrent_DIR} (if incorrect define this variable to the correct location)")
	endif()
	
endif()

set(QWT_CMAKE_ARGS ${QWT_CMAKE_ARGS}
	-DQT_BINARY_DIR:PATH=${QT_BINARY_DIR}
	-DQt5Widgets_DIR:PATH=${Qt5Widgets_DIR}
	-DQt5PrintSupport_DIR:PATH=${Qt5PrintSupport_DIR}
	-DQt5Concurrent_DIR:PATH=${Qt5Concurrent_DIR}
)

if (DEFINED QWT_FRAMEWORK) 
	message(FATAL_ERROR "this option is not supported by the CMake version of QWT yet")
endif()
if (NOT DEFINED QWT_PLOT)
	set(QWT_PLOT TRUE) 
endif()
if (NOT DEFINED QWT_SVG)
	set(QWT_SVG FALSE) 
endif()
if (NOT DEFINED QWT_OPENGL)
	set(QWT_OPENGL FALSE) 
endif()
if (NOT DEFINED QWT_WIDGETS)
	set(QWT_WIDGETS FALSE) 
endif()
if (NOT DEFINED QWT_DLL)
	set(QWT_DLL FALSE) 
endif()

set(QWT_CMAKE_ARGS ${QWT_CMAKE_ARGS}
	-DQWT_PLOT:BOOL=${QWT_PLOT}
	-DQWT_SVG:BOOL=${QWT_SVG}
	-DQWT_OPENGL:BOOL=${QWT_OPENGL}
	-DQWT_WIDGETS:BOOL=${QWT_WIDGETS}
	-DQWT_DLL:BOOL=${QWT_DLL}
)

if(MSVC)

  # Force /MT and /MTd
  set(QWT_CMAKE_CACHE_ARGS ${QWT_CMAKE_CACHE_ARGS}
	-DCMAKE_C_FLAGS_DEBUG:STRING=${CMAKE_C_FLAGS_DEBUG}
	-DCMAKE_C_FLAGS_MINSIZEREL:STRING=${CMAKE_C_FLAGS_MINSIZEREL}
	-DCMAKE_C_FLAGS_RELEASE:STRING=${CMAKE_C_FLAGS_RELEASE}
	-DCMAKE_C_FLAGS_RELWITHDEBINFO:STRING=${CMAKE_C_FLAGS_RELWITHDEBINFO}

	-DCMAKE_CXX_FLAGS_DEBUG:STRING=${CMAKE_CXX_FLAGS_DEBUG}
	-DCMAKE_CXX_FLAGS_MINSIZEREL:STRING=${CMAKE_CXX_FLAGS_MINSIZEREL}
	-DCMAKE_CXX_FLAGS_RELEASE:STRING=${CMAKE_CXX_FLAGS_RELEASE}
	-DCMAKE_CXX_FLAGS_RELWITHDEBINFO:STRING=${CMAKE_CXX_FLAGS_RELWITHDEBINFO}
  )
  
  # Force /MACHINE
  set(QWT_CMAKE_CACHE_ARGS ${QWT_CMAKE_CACHE_ARGS} -DCMAKE_EXE_LINKER_FLAGS:STRING=${CMAKE_EXE_LINKER_FLAGS})
endif()

if(MINGW) 
  set(QWT_CMAKE_CACHE_ARGS ${QWT_CMAKE_CACHE_ARGS} 
	-DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
	-DCMAKE_EXE_LINKER_FLAGS:STRING=${CMAKE_EXE_LINKER_FLAGS}
	-DCMAKE_SHARED_LINKER_FLAGS:STRING=${CMAKE_SHARED_LINKER_FLAGS}
	-DCMAKE_SHARED_MODULE_FLAGS:STRING=${CMAKE_MODULE_LINKER_FLAGS}
  )
endif()

if(ANDROID)
  set(QWT_CMAKE_CACHE_ARGS ${QWT_CMAKE_CACHE_ARGS}
	-DANDROID_TARGET_PRODUCT:STRING=${ANDROID_TARGET_PRODUCT}
	-DANDROID_TARGET_OUT:STRING=${ANDROID_TARGET_OUT}
	-DANDROID_ROOT:STRING=${ANDROID_ROOT}
  )
endif()
  
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" OR ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

  if(${FORCE_32BIT_BUILD} MATCHES "ON")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    set(QWT_CMAKE_CACHE_ARGS ${QWT_CMAKE_CACHE_ARGS} -DFORCE_32BIT_BUILD:BOOL=ON)
  endif()
  
  set(QWT_CMAKE_CACHE_ARGS ${QWT_CMAKE_CACHE_ARGS}
	-DCMAKE_EXE_LINKER_FLAGS:STRING=${CMAKE_EXE_LINKER_FLAGS}
	-DCMAKE_SHARED_LINKER_FLAGS:STRING=${CMAKE_SHARED_LINKER_FLAGS}

	-DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
	-DQWT_WITH_PTHREAD:BOOL=ON
  )

endif()

# Pass toolchain
if(NOT "${CMAKE_TOOLCHAIN_FILE}" STREQUAL "")
  set(QWT_CMAKE_CACHE_ARGS ${QWT_CMAKE_CACHE_ARGS} -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE})
endif()

# Need to set the build type
set(QWT_CMAKE_CACHE_ARGS ${QWT_CMAKE_CACHE_ARGS} -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE})

# Pass any non default configuration
if(CMAKE_CONFIGURATION_TYPES)
  foreach(CONFIG_NAME ${CMAKE_CONFIGURATION_TYPES})
    if(NOT "${CONFIG_NAME}" STREQUAL "Debug" AND
       NOT "${CONFIG_NAME}" STREQUAL "Release" AND
       NOT "${CONFIG_NAME}" STREQUAL "MinSizeRel" AND
       NOT "${CONFIG_NAME}" STREQUAL "RelWithDebInfo")

      string(TOUPPER ${CONFIG_NAME} CONFIG_NAME)
      # Set the flags for this configuration
      foreach(FLAGS_NAME CXX_FLAGS C_FLAGS) #Compiler flags
        set(QWT_CMAKE_CACHE_ARGS ${QWT_CMAKE_CACHE_ARGS} -DCMAKE_${FLAGS_NAME}_${CONFIG_NAME}:STRING=${CMAKE_${FLAGS_NAME}_${CONFIG_NAME}})
      endforeach()
      
      foreach(FLAGS_NAME EXE_LINKER_FLAGS MODULE_LINKER_FLAGS SHARED_LINKER_FLAGS) #Linker flags
        set(QWT_CMAKE_CACHE_ARGS ${QWT_CMAKE_CACHE_ARGS} -DCMAKE_${FLAGS_NAME}_${CONFIG_NAME}:STRING=${CMAKE_${FLAGS_NAME}_${CONFIG_NAME}})
      endforeach()
    endif()
  endforeach()

  set(QWT_CMAKE_CACHE_ARGS ${QWT_CMAKE_CACHE_ARGS} -DCMAKE_CONFIGURATION_TYPES:STRING=${CMAKE_CONFIGURATION_TYPES})
endif()

# Copy CMakeList.txt to source directory
set(QWT_PATCH_COMMAND cmake -E copy_directory ${QWT_PREFIX}/patch_q5 <SOURCE_DIR>)

if ( CMAKE_VERSION VERSION_GREATER "2.8.3" )
  # ExternalProject_Add has CMAKE_CACHE_ARGS and URL_MD5
  ExternalProject_Add(
    ${QWT_NAME}
    URL ${QWT_URL}
    URL_MD5 ${QWT_MD5}
    DOWNLOAD_DIR ${QWT_DOWNLOAD_DIR}
    SOURCE_DIR ${QWT_SOURCE_DIR}
    BINARY_DIR ${QWT_BINARY_DIR}
    INSTALL_DIR ${QWT_INSTALL_DIR}
    CMAKE_ARGS ${QWT_CMAKE_ARGS}
    CMAKE_CACHE_ARGS ${QWT_CMAKE_CACHE_ARGS}
    UPDATE_COMMAND ""
    PATCH_COMMAND ${QWT_PATCH_COMMAND}
  )
elseif ( CMAKE_VERSION VERSION_GREATER "2.8.1" )
  # ExternalProject_Add has URL_MD5
  # CMAKE_CACHE_ARGS option in ExternalProject_Add is only implemented in
  # cmake starting in version 2.8.4. Use CMAKE_ARGS for now (this limits
  # the max command line to 8k chars on windows).
  set(QWT_CMAKE_ARGS ${QWT_CMAKE_ARGS} ${QWT_CMAKE_CACHE_ARGS})
  
  ExternalProject_Add(
    ${QWT_NAME}
    URL ${QWT_URL}
    URL_MD5 ${QWT_MD5}
    DOWNLOAD_DIR ${QWT_DOWNLOAD_DIR}
    SOURCE_DIR ${QWT_SOURCE_DIR}
    BINARY_DIR ${QWT_BINARY_DIR}
    INSTALL_DIR ${QWT_INSTALL_DIR}
    CMAKE_ARGS ${QWT_CMAKE_ARGS}
    UPDATE_COMMAND ""
    PATCH_COMMAND ${QWT_PATCH_COMMAND}
  )
else()
  # CMAKE_CACHE_ARGS option in ExternalProject_Add is only implemented in
  # cmake starting in version 2.8.4. Use CMAKE_ARGS for now (this limits
  # the max command line to 8k chars on windows).
  set(QWT_CMAKE_ARGS ${QWT_CMAKE_ARGS} ${QWT_CMAKE_CACHE_ARGS})
  
  ExternalProject_Add(
    ${QWT_NAME}
    URL ${QWT_URL}
    DOWNLOAD_DIR ${QWT_DOWNLOAD_DIR}
    SOURCE_DIR ${QWT_SOURCE_DIR}
    BINARY_DIR ${QWT_BINARY_DIR}
    INSTALL_DIR ${QWT_INSTALL_DIR}
    CMAKE_ARGS ${QWT_CMAKE_ARGS}
    UPDATE_COMMAND ""
    PATCH_COMMAND ${QWT_PATCH_COMMAND}
  )
endif()
