cmake_minimum_required (VERSION 3.2)

project(VisionLive2)

#find_package(Qt5 COMPONENTS Widgets Core Network)
find_package(Qt5Widgets REQUIRED)

find_package(Qwt REQUIRED)
find_package(QtExtra REQUIRED)

find_package(ImgIncludes REQUIRED)
find_package(ParamSocket2 REQUIRED)

find_package(SimImage REQUIRED)
find_package(FelixAPI REQUIRED)
find_package(FelixCommon REQUIRED)
find_package(VisionTuning REQUIRED)

find_package(ISPC2 REQUIRED)

find_package(DynCMD REQUIRED)

# CMP0020 NEW: link automatically to qtmain.lib when linking with QtCore for WIN32
# CMP0020 OLD: do not link with qtmain.lib automatically for WIN32
#cmake_policy(SET CMP0020 OLD)

set (GUI_EXE_NAME VisionLive)

include_directories(
  include/
  include/common
  include/global
  include/module
  ${CMAKE_CURRENT_BINARY_DIR}
  ${QWT_INCLUDE_DIRS}
  ${QTEXTRA_INCLUDE_DIRS}
  ${IMGINCLUDES_INCLUDE_DIRS}
  ${PARAMSOCKET2_INCLUDE_DIRS}
  ${SIMIMAGE_INCLUDE_DIRS}
  ${FELIXAPI_INCLUDE_DIRS}
  ${FELIXCOMMON_INCLUDE_DIRS}
  ${VISIONTUNING_INCLUDE_DIRS}
  ${ISPC2_INCLUDE_DIRS}
  ${DYNCMD_INCLUDE_DIRS}
  )

add_definitions(
  ${QWT_DEFINITIONS}
  ${QTEXTRA_DEFINITIONS}
  ${IMGINLCUDES_DEFINITIONS}
  ${PARAMSOCKET2_DEFINITIONS}
  ${SIMIMAGE_DEFINITIONS}
  ${FELIXAPI_DEFINITIONS}
  ${FELIXCOMMON_DEFINES}
  ${VISIONTUNING_DEFINES}
  ${ISPC2_DEFINITIONS}
  ${DYNCMD_DEFINITIONS}
  )

set (COMMON_HEADERS
  include/common/css.hpp
  include/common/names.hpp
  include/common/algorithms.hpp
  include/common/customgraphicsview.hpp
  include/common/doubleslider.hpp
  )
  
set (GLOBAL_HEADERS
  include/global/mainwindow.hpp
  include/global/tablogview.hpp
  include/global/livefeedview.hpp
  include/global/imagedata.hpp
  include/global/objectregistry.hpp
  include/global/proxyhandler.hpp
  include/global/imagehandler.hpp
  include/global/commandhandler.hpp
  include/global/test.hpp
  include/global/testrunner.hpp
  include/global/hwinfo.hpp
  include/global/histogram.hpp
  include/global/capturegallery.hpp
  include/global/capturegalleryitem.hpp
  include/global/capturepreview.hpp
  include/global/vectorscope.hpp
  include/global/lineview.hpp
  )
  
set (MODULE_HEADERS
  include/module/modulebase.hpp
  include/module/out.hpp
  include/module/exposure.hpp
  include/module/focus.hpp
  include/module/blc.hpp
  include/module/noise.hpp
  include/module/r2y.hpp
  include/module/y2r.hpp
  include/module/mgm.hpp
  include/module/dgm.hpp
  include/module/ens.hpp
  include/module/wbc.hpp
  include/module/ccmpatch.hpp
  include/module/lsh.hpp
  include/module/dpf.hpp
  include/module/tnm.hpp
  include/module/tnmcurve.hpp
  include/module/vib.hpp
  include/module/vibcurve.hpp
  include/module/lbc.hpp
  include/module/lbcconfig.hpp
  include/module/gma.hpp
  include/module/mie.hpp
  include/module/miewidget.hpp
  include/module/lca.hpp
  )
  
set (HEADERS
  ${COMMON_HEADERS}
  ${GLOBAL_HEADERS}
  ${MODULE_HEADERS}
  )
  
set (COMMON_SOURCES
  source/common/algorithms.cpp
  source/common/customgraphicsview.cpp
  source/common/doubleslider.cpp
  )
  
set (GLOBAL_SOURCES
  source/global/main.cpp
  source/global/mainwindow.cpp
  source/global/tablogview.cpp
  source/global/livefeedview.cpp
  source/global/imagedata.cpp
  source/global/objectregistry.cpp
  source/global/proxyhandler.cpp
  source/global/imagehandler.cpp
  source/global/commandhandler.cpp
  source/global/test.cpp
  source/global/testrunner.cpp
  source/global/hwinfo.cpp
  source/global/histogram.cpp
  source/global/capturegallery.cpp
  source/global/capturegalleryitem.cpp
  source/global/capturepreview.cpp
  source/global/vectorscope.cpp
  source/global/lineview.cpp
  )
  
set (MODULE_SOURCES
  source/module/modulebase.cpp
  source/module/out.cpp
  source/module/exposure.cpp
  source/module/focus.cpp
  source/module/blc.cpp
  source/module/noise.cpp
  source/module/r2y.cpp
  source/module/y2r.cpp
  source/module/mgm.cpp
  source/module/dgm.cpp
  source/module/ens.cpp
  source/module/wbc.cpp
  source/module/ccmpatch.cpp
  source/module/lsh.cpp
  source/module/dpf.cpp
  source/module/tnm.cpp
  source/module/tnmcurve.cpp
  source/module/vib.cpp
  source/module/vibcurve.cpp
  source/module/lbc.cpp
  source/module/lbcconfig.cpp
  source/module/gma.cpp
  source/module/mie.cpp
  source/module/miewidget.cpp
  source/module/lca.cpp
  )
  
set (SOURCES
  ${COMMON_SOURCES}
  ${GLOBAL_SOURCES}
  ${MODULE_SOURCES}
  )
  
set (COMMON_FORMS
  )
  
set (GLOBAL_FORMS
  forms/global/mainwindow.ui
  forms/global/log.ui
  forms/global/test.ui
  forms/global/hwinfo.ui
  forms/global/capturegallerywidget.ui
  forms/global/capturepreview.ui
  )
  
set (MODULE_FORMS
  forms/module/out.ui
  forms/module/exposure.ui
  forms/module/focus.ui
  forms/module/blc.ui
  forms/module/noise.ui
  forms/module/r2y.ui
  forms/module/y2r.ui
  forms/module/mgm.ui
  forms/module/dgm.ui
  forms/module/ens.ui
  forms/module/wbc.ui
  forms/module/ccmpatch.ui
  forms/module/lsh.ui
  forms/module/dpf.ui
  forms/module/tnm.ui
  forms/module/tnmcurve.ui
  forms/module/vib.ui
  forms/module/vibcurve.ui
  forms/module/lbc.ui
  forms/module/lbcconfig.ui
  forms/module/gma.ui
  forms/module/mie.ui
  forms/module/miewidget.ui
  forms/module/lca.ui
  )
  
set (FORMS
  ${COMMON_FORMS}
  ${GLOBAL_FORMS}
  ${MODULE_FORMS}
  )

include_directories(${CMAKE_CURRENT_BINARY_DIR})
  
set(MOC_HEADERS)
set(UI_HEADERS)
set(RESOURCES_RCC)
qt5_wrap_cpp(MOC_HEADERS ${HEADERS})
qt5_wrap_ui(UI_HEADERS ${FORMS})
#QtWrapUI(UI_HEADERS "${FORMS}" ${CMAKE_CURRENT_SOURCE_DIR}/include/ui)
qt5_add_resources(RESOURCES_RCC ${CMAKE_CURRENT_SOURCE_DIR}/resources/recource.qrc)

add_executable(${GUI_EXE_NAME} 
  ${SOURCES} 
  ${HEADERS} 
  ${MOC_HEADERS} 
  ${UI_HEADERS} 
  ${RESOURCES_RCC}
  )

set_source_files_properties(${MOC_HEADERS} ${UI_HEADERS} ${RESOURCES_RCC} PROPERTIES GENERATED TRUE)

source_group(Common_H FILES ${COMMON_HEADERS})
source_group(Global_H FILES ${GLOBAL_HEADERS})
source_group(Module_H FILES ${MODULE_HEADERS})

source_group(Common_CPP FILES ${COMMON_SOURCES})
source_group(Global_CPP FILES ${GLOBAL_SOURCES})
source_group(Module_CPP FILES ${MODULE_SOURCES})

source_group(Common_UI FILES ${COMMON_FORMS})
source_group(Global_UI FILES ${GLOBAL_FORMS})
source_group(Module_UI FILES ${MODULE_FORMS})

source_group(UI_H FILES ${UI_HEADERS})

source_group(MOC FILES ${MOC_HEADERS})

source_group(RCC FILES ${RESOURCES_RCC})

qt5_use_modules(${GUI_EXE_NAME} 
  Widgets 
  Core 
  Network
  )

target_link_libraries(${GUI_EXE_NAME}
  ${QWT_LIBRARIES}
  ${QTEXTRA_LIBRARIES}
  ${PARAMSOCKET2_LIBRARIES}
  ${SIMIMAGE_LIBRARIES} 
  ${FELIXAPI_LIBRARIES}
  ${FELIXCOMMON_LIBRARIES} 
  ${VISIONTUNING_LIBRARIES}
  ${ISPC2_LIBRARIES}
  ${DYNCMD_LIBRARIES}
  )

#################################################################################################################################
  
# hack: does not link with FELIXAPI_LIBRARIES the GUI includes the needed file in its sources
add_dependencies(${GUI_EXE_NAME} ${QWT_NAME} ${FELIXAPI_DEPENDENCIES}) # qwt is built as an external library

set (RESOURCES)

set (INSTALL_FELIXDEMOGUI_PATH ./GUI/VisionLive)

if(NOT DEFINED FELIXDEMOGUI_WIN32EXE)
  set(FELIXDEMOGUI_WIN32EXE TRUE) # so it can be defined as FALSE when debugging
endif()

if (WIN32)
	set(DEBUG_DLLS) # empty
	GetQt5OtherDLLs(DEBUG_DLLS) # find all dlls not being Qt modules but needed
	set(RELEASE_DLLS ${DEBUG_DLLS})

	# now both debug and release have by default all dlls that are not modules
	GetQt5DLLs(Qt5::Core DEBUG_DLLS RELEASE_DLLS)
	GetQt5DLLs(Qt5::Widgets DEBUG_DLLS RELEASE_DLLS)
	GetQt5DLLs(Qt5::Gui DEBUG_DLLS RELEASE_DLLS)
	CopyDLLs(DEBUG_DLLS, RELEASE_DLLS, CMAKE_CURRENT_BINARY_DIR)

	install(FILES ${RELEASE_DLLS} DESTINATION ${INSTALL_FELIXDEMOGUI_PATH} CONFIGURATIONS Release)
	install(FILES ${DEBUG_DLLS} DESTINATION ${INSTALL_FELIXDEMOGUI_PATH} CONFIGURATIONS Debug)

	# this property is used to remove the prompt window when running the GUI from the explorer on WIN32
	# doesn't have effect on linux
	set_target_properties(${GUI_EXE_NAME} PROPERTIES WIN32_EXECUTABLE ${FELIXDEMOGUI_WIN32EXE})
endif()

if (DEFINED FELIXAPI_INSTALL)
  # when using ISPC to get information we may link against CI dependencies which need some DDL on windows
  file(COPY ${FELIXAPI_INSTALL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  install(FILES ${FELIXAPI_INSTALL} DESTINATION ${INSTALL_FELIXDEMOGUI_PATH})
endif()

foreach(file ${RESOURCES})
  GET_FILENAME_COMPONENT(outfile ${file} NAME)
  GET_FILENAME_COMPONENT(infile ${file} ABSOLUTE)
  file(COPY ${infile} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)
endforeach()

set_target_properties(${GUI_EXE_NAME} PROPERTIES RESOURCE "${RESOURCES}")

#
# install only the release build to avoid having all the debug libs
#
install(TARGETS ${GUI_EXE_NAME}
  RUNTIME DESTINATION ${INSTALL_FELIXDEMOGUI_PATH} #CONFIGURATIONS Release
  RESOURCE DESTINATION ${INSTALL_FELIXDEMOGUI_PATH}/resources/ #CONFIGURATIONS Release
  )
