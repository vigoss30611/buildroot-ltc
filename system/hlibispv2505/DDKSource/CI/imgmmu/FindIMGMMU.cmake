#
# Find the include directory and library name for the IMG MMU library
#
# see CMakeLists for compilation options
#
cmake_minimum_required(VERSION 2.8)

get_filename_component(IMGMMU_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)

set(IMGMMU_INCLUDE_DIRS ${IMGMMU_PREFIX}/include)
set(IMGMMU_DEFINITIONS)
set(IMGMMU_DOXYGEN_SRC ${IMGMMU_PREFIX}/source ${IMGMMU_PREFIX}/doc)
set(IMGMMU_NAME IMGMMULib)
set(IMGMMU_LIBRARIES ${IMGMMU_NAME})
set(IMGMMU_BUILD_KO ${IMGMMU_PREFIX}/IMGMMU_kernel.cmake)

mark_as_advanced(IMGMMU_INCLUDE_DIRS IMGMMU_DEFINITIONS IMGMMU_NAME IMGMMU_LIBRARIES)
