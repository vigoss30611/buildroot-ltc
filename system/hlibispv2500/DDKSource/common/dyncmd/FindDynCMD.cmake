# Locate and configure the Dynamic command line library.
#
# Internals:
#
#   DYNCMD_PREFIX - the directory where Skeleton App is located in the project
#   DYNCMD_NAME - Librarie's name
#   DYNCMD_TESTS - used at compilation time to add the test executable if gtest if found in CMAKE_MODULE_PATH
#
# Defines the following variables:
#
#   DYNCMD_FOUND - Found the Skeleton App
#   DYNCMD_INCLUDE_DIRS - The directories needed on the include paths
#   DYNCMD_DEFINITIONS
#   DYNCMD_LIBRARIES
#

# set DYNCMD_PREFIX to directory where the current file file is located
get_filename_component(DYNCMD_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)

set (DYNCMD_FOUND TRUE)
set (DYNCMD_INCLUDE_DIRS ${DYNCMD_PREFIX}/include)
set (DYNCMD_DEFINITIONS)
set (DYNCMD_NAME ImgLib_DynamicCommand)
set (DYNCMD_LIBRARIES ${DYNCMD_NAME})
