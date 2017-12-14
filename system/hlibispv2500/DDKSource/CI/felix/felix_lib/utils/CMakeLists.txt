
find_package(ImgIncludes REQUIRED)
find_package(FelixAPI REQUIRED) # to get the HW information header
find_package(DynCMD REQUIRED)

include_directories(
	${IMGINCLUDES_INCLUDE_DIRS}
	${FELIXAPI_INCLUDE_DIRS}
	${DYNCMD_INCLUDE_DIRS}
	${FelixLib_SOURCE_DIR}/kernel/include # to access ioctl
)

if (CI_EXT_DATA_GENERATOR)
    include_directories(${FelixLib_SOURCE_DIR}/data_generator/include) # to access ioctl
endif()

add_definitions(
	${IMGINCLUDES_DEFINITIONS}
	${FELIXAPI_DEFINITIONS}
	${DYNCMD_DEFINITIONS}
)
#
# additional tools
#

add_executable(dpf_conv dpf_converter.c)
target_link_libraries(dpf_conv ${DYNCMD_LIBRARIES})
add_dependencies(dpf_conv ${FELIXAPI_DEPENDENCIES}) # for generated files

add_executable(ci_ioctl_printer ci_ioctl_printer.c)
add_dependencies(ci_ioctl_printer ${FELIXAPI_DEPENDENCIES}) # for generated files

#install(TARGETS dpf_conv ci_ioctl_printer DESTINATION ./utils/)


