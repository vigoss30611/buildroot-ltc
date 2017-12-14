cmake_minimum_required (VERSION 3.2)

project(EasyPlot)
message (STATUS "-- EasyPlot")

find_package(Qt5Widgets REQUIRED)
find_package(EasyPlot REQUIRED)

# CMP0020 NEW: link automatically to qtmain.lib when linking with QtCore for WIN32
# CMP0020 OLD: do not link with qtmain.lib automatically for WIN32
#cmake_policy(SET CMP0020 OLD)

include_directories(
  include/
  ${CMAKE_CURRENT_BINARY_DIR}
  )

add_definitions(

  )
  
set(HEADERS
  include/area.h
  include/axis.h
  include/common.h
  include/component.h
  include/configwidget.h
  include/curve.h
  include/curvearea.h
  include/legend.h
  include/marker.h
  include/plot.h
  include/extplot.h
  include/title.h
  include/grid.h
  )
  
set(SOURCES
  source/area.cpp
  source/axis.cpp
  source/component.cpp
  source/configwidget.cpp
  source/curve.cpp
  source/curvearea.cpp
  source/legend.cpp
  source/marker.cpp
  source/plot.cpp
  source/extplot.cpp
  source/title.cpp
  source/grid.cpp
  )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(MOC_HEADERS)

qt5_wrap_cpp(MOC_HEADERS ${HEADERS})
#QtWrapUI(UI_HEADERS "${FORMS}" ${CMAKE_CURRENT_SOURCE_DIR}/include/ui)

add_library(${EASYPLOT_NAME} 
  ${SOURCES} 
  ${HEADERS} 
  ${MOC_HEADERS} 
  )
  
set_source_files_properties(${MOC_HEADERS} PROPERTIES GENERATED TRUE)

source_group(includes FILES ${HEADERS})

source_group(sources FILES ${SOURCES})

source_group(MOC FILES ${MOC_HEADERS})

qt5_use_modules(${EASYPLOT_NAME} 
  Widgets 
  Core 
  )
  
  