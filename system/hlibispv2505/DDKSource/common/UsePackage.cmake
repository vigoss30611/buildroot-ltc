# This file is designed to be included in a top level CMakeLists.txt like so:
#
#     include(path/to/here/UsePackage.cmake)
#
# It will add to the module path all subdirectories with a FindXXX.cmake file

# Do everything inside a function so as not to clash with any existing variables
function(AddDirsToModulePath)
get_filename_component(CURRENT_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

if (NOT ("${CI_ALLOC}" STREQUAL "IMGVIDEO"))
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    ${CURRENT_DIR}/img_includes/)
endif()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
  ${CURRENT_DIR}/felixcommon/
  ${CURRENT_DIR}/paramsocket2/
  ${CURRENT_DIR}/external/gtest
  ${CURRENT_DIR}/external/qwt
  ${CURRENT_DIR}/external/zlib
  ${CURRENT_DIR}/external/mxml
  ${CURRENT_DIR}/external/pthread_win32
  ${CURRENT_DIR}/HDRLibs/
  ${CURRENT_DIR}/sim_image
  ${CURRENT_DIR}/linkedlist
  ${CURRENT_DIR}/dyncmd
  ${CURRENT_DIR}/dmabuf
  PARENT_SCOPE
  )
endfunction()

if(DEBUG_MODULE)
  message("CMAKE_MODULE_PATH before: ${CMAKE_MODULE_PATH}")
endif()

AddDirsToModulePath()

if(DEBUG_MODULE)
  message("CMAKE_MODULE_PATH  after: ${CMAKE_MODULE_PATH}")
endif()
