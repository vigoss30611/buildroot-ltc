cmake_minimum_required(VERSION 2.8)

project(ISPCONTROL_LIB2)

find_package(ImgIncludes REQUIRED)
find_package(FelixAPI REQUIRED)
find_package(ISPC2 REQUIRED)
find_package(SimImage REQUIRED)
if(CI_EXT_DATA_GENERATOR)
  find_package(FelixDG REQUIRED)
  add_definitions(-DISPC_EXT_DATA_GENERATOR)
endif()
find_package(FelixCommon REQUIRED) # LOG
find_package(GTest QUIET) # optional

#find_package(SensorApi REQUIRED) # found in FindISPC2.cmake

message (STATUS "-- ISPC2 lib")

# use CMAKE_CURRENT_SOURCE_DIR because also used in unit tests
set(ISPC_MODULES_CPP
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleIIF.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleEXS.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleBLC.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleRLT.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleLSH.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleWBC.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleFOS.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleDNS.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleLCA.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleCCM.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleMGM.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleGMA.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleWBS.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleHIS.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleR2Y.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleMIE.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleTNM.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleFLD.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleSHA.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleESC.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleDSC.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleY2R.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleDGM.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleDPF.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleOUT.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleVIB.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/ModuleENS.cpp 
)

set(ISPC_AUXILIAR_CPP
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Auxiliar/Matrix.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Auxiliar/TemperatureCorrection.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Auxiliar/ColorCorrection.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Auxiliar/ParameterFileParser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Auxiliar/LightCorrection.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Auxiliar/Save.cpp
)

set(ISPC_CONTROL_CPP
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Controls/ControlAE.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Controls/ControlAWB.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Controls/ControlAWB_PID.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Controls/ControlTNM.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Controls/ControlDNS.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Controls/ControlLBC.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Controls/ControlAF.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Controls/ControlCMC.cpp
)

set(ISPC_CPP
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Camera.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CameraFactory.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/DGCamera.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Pipeline.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Sensor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Shot.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Control.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Connection.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Parameter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ParameterList.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Module.cpp
  ${ISPC_AUXILIAR_CPP}
  ${ISPC_MODULES_CPP}
  ${ISPC_CONTROL_CPP}
)

include_directories(
  ${FELIXAPI_INCLUDE_DIRS}
  ${IMGINCLUDES_INCLUDE_DIRS}
  ${ISPC2_INCLUDE_DIRS}
  ${SENSORAPI_INCLUDE_DIRS}
  ${SIMIMAGE_INCLUDE_DIRS}
  ${FELIXCOMMON_INCLUDE_DIRS}
  )

add_definitions(
  ${FELIXAPI_DEFINITIONS}
  ${IMGINCLUDES_DEFINITIONS}
  ${ISPC2_DEFINITIONS}
  ${SENSORAPI_DEFINITIONS}
  ${SIMIMAGE_DEFINITIONS}
  ${FELIXCOMMON_DEFINITIONS}
  #-DVERBOSE_CONTROL_MODULES=1
  #-DISPC_PARAM_FOUND_VERBOSE=1
  )

set(ISPC_MODULES_H
  include/Modules/ispc/ModuleIIF.h
  include/Modules/ispc/ModuleEXS.h
  include/Modules/ispc/ModuleBLC.h
  include/Modules/ispc/ModuleRLT.h
  include/Modules/ispc/ModuleLSH.h
  include/Modules/ispc/ModuleWBC.h
  include/Modules/ispc/ModuleFOS.h
  include/Modules/ispc/ModuleDNS.h
  include/Modules/ispc/ModuleLCA.h
  include/Modules/ispc/ModuleCCM.h
  include/Modules/ispc/ModuleMGM.h
  include/Modules/ispc/ModuleGMA.h
  include/Modules/ispc/ModuleWBS.h
  include/Modules/ispc/ModuleHIS.h
  include/Modules/ispc/ModuleR2Y.h
  include/Modules/ispc/ModuleMIE.h
  include/Modules/ispc/ModuleTNM.h
  include/Modules/ispc/ModuleFLD.h
  include/Modules/ispc/ModuleSHA.h
  include/Modules/ispc/ModuleESC.h
  include/Modules/ispc/ModuleDSC.h
  include/Modules/ispc/ModuleY2R.h
  include/Modules/ispc/ModuleDGM.h
  include/Modules/ispc/ModuleDPF.h
  include/Modules/ispc/ModuleOUT.h
  include/Modules/ispc/ModuleVIB.h
  include/Modules/ispc/ModuleENS.h 
)

set(ISPC_AUXILIAR_H
  include/Auxiliar/ispc/Matrix.h
  include/Auxiliar/ispc/TemperatureCorrection.h
  include/Auxiliar/ispc/ColorCorrection.h
  include/Auxiliar/ispc/ParameterFileParser.h
  include/Auxiliar/ispc/LightCorrection.h
  include/Auxiliar/ispc/Save.h
)

set(ISPC_CONTROL_H
  include/Controls/ispc/ControlAE.h
  include/Controls/ispc/ControlAWB.h
  include/Controls/ispc/ControlAWB_PID.h
  include/Controls/ispc/ControlTNM.h
  include/Controls/ispc/ControlDNS.h
  include/Controls/ispc/ControlLBC.h
  include/Controls/ispc/ControlAF.h
  include/Controls/ispc/ControlCMC.h
)
  
set(ISPC_H
  include/ispc/ISPCDefs.h
  include/ispc/Camera.h
  include/ispc/Connection.h
  include/ispc/Pipeline.h
  include/ispc/Sensor.h
  include/ispc/Shot.h
  include/ispc/Control.h
  include/ispc/Module.h
  include/ispc/Parameter.h
  include/ispc/ParameterList.h
  #include/ispc/integral.h
  #include/ispc/ldws.h
  #include/ispc/fcws.h

  ${ISPC_MODULES_H}
  ${ISPC_AUXILIAR_H}
  ${ISPC_CONTROL_H}
)

add_library(${ISPC2_LIBRARIES} ${ISPC_CPP} ${ISPC_H})
target_link_libraries(${ISPC2_LIBRARIES}     
    ${FELIXAPI_LIBRARIES}
    ${SENSORAPI_LIBRARIES}
    ${SIMIMAGE_LIBRARIES}
    ${FELIXCOMMON_LIBRARIES}   
    hlibcamsensor
)

source_group(Modules_H FILES ${ISPC_MODULES_H})
source_group(Modules_CPP FILES ${ISPC_MODULES_CPP})

source_group(Auxiliar_H FILES ${ISPC_AUXILIAR_H})
source_group(Auxiliar_CPP FILES ${ISPC_AUXILIAR_CPP})

source_group(Control_H FILES ${ISPC_CONTROL_H})
source_group(Control_CPP FILES ${ISPC_CONTROL_CPP})

add_dependencies(${ISPC2_LIBRARIES} ${FELIXAPI_DEPENDENCIES})

if(${BUILD_TEST_APPS})
    add_subdirectory(ispc2test)
endif()

if(GTEST_FOUND AND ISPC_TESTS)
    add_subdirectory(test)
endif()
