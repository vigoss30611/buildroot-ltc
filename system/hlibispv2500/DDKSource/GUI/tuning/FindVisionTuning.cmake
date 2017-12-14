#
# needs the VisionTuning project to exists before called!
#

cmake_minimum_required (VERSION 2.8)

get_filename_component (VISIONTUNING_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)

find_package(OpenCV 2.4.9 REQUIRED)

set (VISIONTUNING_FOUND TRUE)
set (VISIONTUNING_INCLUDE_DIRS 
    ${VISIONTUNING_PREFIX}/include/ 
    ${OpenCV_INCLUDE_DIRS}
    ${VisionTuning_BINARY_DIR}/include
)
set (VISIONTUNING_DEFINES ${OpenCV_DEFINITIONS})
set (VISIONTUNING_NAME VisionTuning_lib)
set (VISIONTUNING_LIBRARIES ${VISIONTUNING_NAME})
