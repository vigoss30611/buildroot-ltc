cmake_minimum_required(VERSION 2.8)

enable_language(C)

if(NOT WIN32)
  find_package(Threads)
endif()

include(CheckTypeSize)
include(CheckFunctionExists)
include(CheckIncludeFiles)

project(libmxml C)

set(MXML_VERSION "Mini-XML v2.7")

check_type_size("long long int" HAVE_LONG_LONG)

check_function_exists(snprintf HAVE_SNPRINTF)
check_function_exists(vsnprintf HAVE_VSNPRINTF)
check_function_exists(strdup HAVE_STRDUP)

if(NOT WIN32)
  check_include_files(pthread.h HAVE_PTHREAD_H)
endif()

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmakeconfig.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

# SET LIBNAME
set(MXML_LIB_NAME mxml)
if(MSVC)
	set(MXML_LIB_NAME_STATIC ${MXML_LIB_NAME}_static)
	set(CMAKE_DEBUG_POSTFIX "d")
else()
	set(MXML_LIB_NAME_STATIC ${MXML_LIB_NAME})
endif()
# to distinguish between debug and release lib


set(SOURCES
  ./mxml-attr.c
  ./mxml-entity.c
  ./mxml-file.c
  ./mxml-get.c
  ./mxml-index.c
  ./mxml-node.c
  ./mxml-search.c
  ./mxml-set.c
  ./mxml-private.c
  ./mxml-string.c
)

add_library(${MXML_LIB_NAME_STATIC} STATIC ${SOURCES})

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
  # msvc does not append 'lib' - do it here to have consistent name
  set_target_properties(${MXML_LIB_NAME_STATIC} PROPERTIES PREFIX "lib")
endif()

if(CMAKE_USE_PTHREADS_INIT)
  set(PTHREAD_FLAGS "-D_THREAD_SAFE -D_REENTRANT")
  
  if(CMAKE_SYSTEM MATCHES "SunOS.*")
    set(PTHREAD_FLAGS "${PTHREAD_FLAGS} -D_POSIX_PTHREAD_SEMANTICS")
  endif()   

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PTHREAD_FLAGS}")
  
  target_link_libraries(${MXML_LIB_NAME_STATIC} ${CMAKE_THREAD_LIBS_INIT})
endif()

set(prefix      ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${CMAKE_INSTALL_PREFIX})
set(libdir      ${CMAKE_INSTALL_PREFIX}/lib)
set(includedir  ${CMAKE_INSTALL_PREFIX}/include)

# INSTALL
if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL)
  install(TARGETS ${MXML_LIB_NAME_STATIC}
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib)
endif()

if(NOT SKIP_INSTALL_HEADERS AND NOT SKIP_INSTALL_ALL)
  install(FILES mxml.h DESTINATION include)
endif()

