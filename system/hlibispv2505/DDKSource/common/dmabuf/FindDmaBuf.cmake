# Locate and configure the Userspace dma-buf allocator
#
# Internals:
#
#   DMABUF_PREFIX - the directory where Skeleton App is located in the project
#   DMABUF_NAME - Librarie's name
#   DMABUF_TESTS - used at compilation time to add the test executable if gtest if found in CMAKE_MODULE_PATH
#
# Defines the following variables:
#
#   DMABUF_FOUND - Found the Skeleton App
#   DMABUF_INCLUDE_DIRS - The directories needed on the include paths
#   DMABUF_DEFINITIONS
#   DMABUF_LIBRARIES
#

# set DMABUF_PREFIX to directory where the current file file is located
get_filename_component(DMABUF_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)

set (DMABUF_FOUND TRUE)
set (DMABUF_INCLUDE_DIRS ${DMABUF_PREFIX}/include)
set (DMABUF_DEFINITIONS)
set (DMABUF_NAME ImgLib_DmaBuf)
set (DMABUF_LIBRARIES ${DMABUF_NAME})
