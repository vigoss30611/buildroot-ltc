cmake_minimum_required (VERSION 3.2)

find_package(ImgIncludes REQUIRED)

project(VisionTuning)

find_package(VisionTuning REQUIRED)
find_package(DynCMD REQUIRED)
find_package(FelixCommon REQUIRED)
find_package(GTest QUIET) # optional

# GUI part...
find_package(Qt5Widgets REQUIRED)
find_package(Qwt REQUIRED)
find_package(QtExtra REQUIRED)
find_package(SimImage REQUIRED)
#find_package(OpenCV 2.4.9 REQUIRED) # done in FindVisionTuning.cmake
find_package(ISPC2 REQUIRED)

# CMP0020 NEW: link automatically to qtmain.lib when linking with QtCore for WIN32
# CMP0020 OLD: do not link with qtmain.lib automatically for WIN32
#cmake_policy(SET CMP0020 OLD)

if (NOT DEFINED TUNE_CCM_DGB) # add some additional displays with opencv
	set(TUN_CCM_DBG FALSE)
endif()
if (NOT DEFINED TUNE_CCM_ERROR_EVOL) # display the evolution of the error on a graph
    set(TUNE_CCM_ERROR_EVOL FALSE)
endif()

set (QT_USE_QTGUI TRUE)
set (QT_USE_QTMULTIMEDIA TRUE)
#include(${QT_USE_FILE})

PerforceFound(P4_FOUND)
if (${P4_FOUND})
	PerforceChangelist(${CMAKE_CURRENT_SOURCE_DIR} GUI_CHANGELIST)
	PerforceUpdateDate(${CMAKE_CURRENT_SOURCE_DIR} GUI_DATE)
else()
	set(GUI_CHANGELIST "unknown-version")
	set(GUI_DATE "unkown-date")
endif()

set (GUI_NAME VisionTuning)
set (INSTALL_GUI_PATH ./GUI/${GUI_NAME})
set (GENERATED_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/include/vision_about.hpp)
configure_file(vision_about.cmakein ${GENERATED_HEADERS})

include_directories(
	${VISIONTUNING_INCLUDE_DIRS}
	${IMGINCLUDES_INCLUDE_DIRS}
	${DYNCMD_INCLUDE_DIRS}
	${SIMIMAGE_INCLUDE_DIRS}
	${FELIXCOMMON_INCLUDE_DIRS}
	${ISPC2_INCLUDE_DIRS}
	
	# gui part
	${Qt5Widgets_INCLUDES}
	${QTEXTRA_INCLUDE_DIRS}
	${QWT_INCLUDE_DIRS}
	#${CMAKE_CURRENT_BINARY_DIR}/include # done in FindVisionTuning.cmake
  )

add_definitions(
	${IMGINCLUDES_DEFINITIONS}
	${DYNCMD_DEFINITIONS}
	${FELIXCOMMON_DEFINITIONS}
	${ISPC2_DEFINITIONS}
	
	# gui part
	${Qt5Widgets_DEFINITIONS}
	${QTEXTRA_DEFINITIONS}
	${QWT_DEFINITIONS}
	${SIMIMAGE_DEFINITIONS}
	
	# CCM option - add to show the evolution of the error
	#-DTUNE_CCM_WBC_TARGET_AVG
	    
    ${VISIONTUNING_DEFINES}
  )

if (TUNE_CCM_DBG)
	add_definitions(-DTUNE_CCM_DBG) 
endif()

if (TUNE_CCM_ERROR_EVOL)
    add_definitions(-DTUNE_CCM_ERROR_EVOL) 
endif()

set (CCM_HEADERS
	include/ccm/tuning.hpp
	include/ccm/compute.hpp
	include/ccm/config.hpp
  )

set (CCM_SOURCES
	ccm_src/ccmtuning.cpp
	ccm_src/ccmcompute.cpp
	ccm_src/ccmconfig.cpp
  )

set (CCM_FORMS
	forms/ccmconfig.ui
	forms/ccmconfigadv.ui
	forms/ccmtuning.ui
  )

source_group(CCM FILES ${CCM_SOURCES})
source_group(CCM_H FILES ${CCM_HEADERS})

set (LSH_HEADERS
	include/lsh/tuning.hpp
	include/lsh/compute.hpp
	include/lsh/config.hpp
	include/lsh/gridview.hpp
	include/lsh/configadv.hpp
  )
set (LSH_SOURCES
	lsh_src/lshcompute.cpp
	lsh_src/lshtuning.cpp
	lsh_src/lshconfig.cpp
	lsh_src/lshgridview.cpp
	lsh_src/lshconfigadv.cpp
  )

set (LSH_FORMS
	forms/lshconfig.ui
	forms/lshtuning.ui
	forms/lshgridview.ui
	forms/lshconfigadv.ui
  )

source_group(LSH FILES ${LSH_SOURCES})
source_group(LSH_H FILES ${LSH_HEADERS})

set (LCA_HEADERS
	include/lca/tuning.hpp
	include/lca/compute.hpp
	include/lca/config.hpp
	include/lca/preview.hpp
  )
set (LCA_SOURCES
	lca_src/lcacompute.cpp
	lca_src/lcatuning.cpp
	lca_src/lcaconfig.cpp
	lca_src/lcapreview.cpp
  )

set (LCA_FORMS
	forms/lcaconfig.ui
	forms/lcatuning.ui
	forms/lcapreview.ui
  )

source_group(LCA FILES ${LCA_SOURCES})
source_group(LCA_H FILES ${LCA_HEADERS})

set (LBC_HEADERS
	include/lbc/tuning.hpp
	include/lbc/config.hpp

  )
set (LBC_SOURCES
	lbc_src/lbctuning.cpp
	lbc_src/lbcconfig.cpp

  )

set (LBC_FORMS
	forms/lbctuning.ui
	forms/lbcwidget.ui
  )

source_group(LBC FILES ${LBC_SOURCES})
source_group(LBC_H FILES ${LBC_HEADERS})

set (GMA_HEADERS
	include/gma/tuning.hpp
	include/gma/widget.hpp

  )
set (GMA_SOURCES
	gma_src/gmatuning.cpp
	gma_src/gmawidget.cpp

  )

set (GMA_FORMS
	forms/gmatuning.ui
	forms/gmawidget.ui
  )

source_group(GMA FILES ${GMA_SOURCES})
source_group(GMA_H FILES ${GMA_HEADERS})

set(GUI_FORMS
	${CCM_FORMS}
	${LSH_FORMS}
	${LCA_FORMS}
	${LBC_FORMS}
	${GMA_FORMS}
	forms/imageview.ui
	forms/visiontuning.ui
	forms/genconfig.ui
  )

set(RESOURCES)
set(QRC_FILES )

set(GUI_HEADERS
	include/imageview.hpp
	include/visiontuning.hpp
	include/genconfig.hpp
	include/computebase.hpp # not really a GUI but need to create moc
  )
set(HEADERS_MOC) # clean
QT5_WRAP_CPP(HEADERS_MOC ${GUI_HEADERS} ${CCM_HEADERS} ${LSH_HEADERS} ${LCA_HEADERS} ${LBC_HEADERS} ${GMA_HEADERS})
QtWrapUI(FORMS_HEADERS "${GUI_FORMS}" ${CMAKE_CURRENT_BINARY_DIR}/include/ui)
QT5_ADD_RESOURCES(RESSOURCE_RCC ${QRC_FILES})

set(GUI_SOURCES
	src/imageview.cpp
	src/genconfig.cpp
	src/computebase.cpp
  )

set(SOURCES
	${GENERATED_HEADERS}
 
	${CCM_SOURCES}
	${CCM_HEADERS}
	
	${LSH_SOURCES}
	${LSH_HEADERS}
	
	${LCA_SOURCES}
	${LCA_HEADERS}
	
	${LBC_SOURCES}
	${LBC_HEADERS}
	
	${GMA_SOURCES}
	${GMA_HEADERS}
	
	${CLIENT_SOURCES}
	
	${HEADERS_MOC}
	${GUI_HEADERS}
	${GUI_SOURCES}
	${FORMS_HEADERS}
	${RESSOURCE_RCC}
	
	include/buffer.hpp
	include/imageUtils.h
		
	src/buffer.cpp
	src/imageUtils.cpp
	src/visiontuning.cpp
	
  )
add_library(${VISIONTUNING_NAME} ${SOURCES})
set_source_files_properties(${HEADERS_MOC} ${FORMS_HEADERS} ${RESSOURCE_RCC} ${GENERATED_HEADERS} PROPERTIES GENERATED TRUE)
source_group(MOC FILES ${HEADERS_MOC})
source_group(UI_H FILES ${FORMS_HEADERS})
source_group(UI FILES ${GUI_FORMS})
source_group(RCC FILES ${RESSOURCE_RCC})

target_link_libraries(${VISIONTUNING_NAME} ${DYNCMD_LIBRARIES} ${FELIXCOMMON_LIBRARIES} ${ISPC2_LIBRARIES})
target_link_libraries(${VISIONTUNING_NAME} Qt5::Widgets ${QTEXTRA_LIBRARIES} ${SIMIMAGE_LIBRARIES} ${QWT_LIBRARIES} ${OpenCV_LIBS}) # gui part

add_executable(${GUI_NAME} 	src/main.cpp)
target_link_libraries(${GUI_NAME} ${VISIONTUNING_NAME})
add_dependencies(${GUI_NAME} ${QWT_NAME})

message(STATUS "    using OpenCV ${OpenCV_CONFIG_PATH}")

if (WIN32)
	set(DEBUG_DLLS) # empty
	GetQt5OtherDLLs(DEBUG_DLLS) # find all dlls not being Qt modules but needed
	set(RELEASE_DLLS ${DEBUG_DLLS})
	# now both debug and release have by default all dlls that are not modules

	GetQt5DLLs(Qt5::Core DEBUG_DLLS RELEASE_DLLS)
	GetQt5DLLs(Qt5::Widgets DEBUG_DLLS RELEASE_DLLS)
	GetQt5DLLs(Qt5::Gui DEBUG_DLLS RELEASE_DLLS)
	CopyDLLs(DEBUG_DLLS, RELEASE_DLLS, CMAKE_CURRENT_BINARY_DIR)
		
	install(FILES ${RELEASE_DLLS} ${EXTRA_COPY} DESTINATION ${INSTALL_GUI_PATH} CONFIGURATIONS Release)
    install(FILES ${DEBUG_DLLS} ${EXTRA_COPY} DESTINATION ${INSTALL_GUI_PATH} CONFIGURATIONS Debug)
		
	#
	# this is disabled for debug only (so signal/slots connect failures are seen)!
	#
	# this property is used to remove the prompt window when running the GUI from the explorer on WIN32
	# doesn't have effect on linux 
	#
	set_target_properties(${GUI_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if (DEFINED FELIXAPI_INSTALL)
	# when using ISPC to get information we may link against CI dependencies which need some DDL on windows
	file(COPY ${FELIXAPI_INSTALL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
	install(FILES ${FELIXAPI_INSTALL} DESTINATION ${INSTALL_GUI_PATH})
endif()

foreach(file ${RESOURCES})
	GET_FILENAME_COMPONENT(outfile ${file} NAME)
    GET_FILENAME_COMPONENT(infile ${file} ABSOLUTE)
	file(COPY ${infile} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)
endforeach()
set_target_properties(${VISIONTUNING_NAME} PROPERTIES RESOURCE "${RESOURCES}")

#
# install only the release build to avoid having all the debug libs
#
install(TARGETS ${GUI_NAME}
	RUNTIME DESTINATION ${INSTALL_GUI_PATH} #CONFIGURATIONS Release
	#RESOURCE DESTINATION ${INSTALL_GUI_PATH}/resources/ CONFIGURATIONS Release
  )


if (GTEST_FOUND AND BUILD_UNIT_TESTS)
	message(STATUS "   -- unit tests")
	add_subdirectory(test)
endif()
