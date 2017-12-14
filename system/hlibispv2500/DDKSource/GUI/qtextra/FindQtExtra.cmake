cmake_minimum_required (VERSION 3.2)

get_filename_component (QTEXTRA_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)

set (QTEXTRA_FOUND TRUE)
set (QTEXTRA_INCLUDE_DIRS ${QTEXTRA_PREFIX}/include/)
set (QTEXTRA_DEFINES)
set (QTEXTRA_NAME QtExtra)
set (QTEXTRA_LIBRARIES ${QTEXTRA_NAME})

if (WIN32)
MACRO(GetQt4DLLs DEBUG_NAME RELEASE_NAME)

FOREACH(module QT3SUPPORT QTOPENGL QTASSISTANT QTDESIGNER QTMOTIF QTNSPLUGIN
               QAXSERVER QAXCONTAINER QTDECLARATIVE QTSCRIPT QTSVG QTUITOOLS QTHELP
               QTWEBKIT PHONON QTSCRIPTTOOLS QTMULTIMEDIA QTGUI QTTEST QTDBUS QTXML QTSQL
               QTXMLPATTERNS QTNETWORK QTCORE)

	if (QT_USE_${module} OR QT_USE_${module}_DEPENDS)
	
		string(REPLACE ".lib" ".dll" QT_${module}_DLL "${QT_${module}_LIBRARY_DEBUG}")
		set (${DEBUG_NAME} ${${DEBUG_NAME}} ${QT_${module}_DLL})
		
		string(REPLACE ".lib" ".dll" QT_${module}_DLL "${QT_${module}_LIBRARY_RELEASE}")
		set (${RELEASE_NAME} ${${RELEASE_NAME}} ${QT_${module}_DLL})
	
	endif()
  
ENDFOREACH(module)

ENDMACRO()

function(GetQt5DLLs QT_MODULE_NAME DEBUG_NAME RELEASE_NAME)
	
	get_target_property(REL_DLL ${QT_MODULE_NAME} LOCATION_Release)
	get_target_property(DBG_DLL ${QT_MODULE_NAME} LOCATION_Debug)
	
	set(${DEBUG_NAME} ${${DEBUG_NAME}} ${DBG_DLL} PARENT_SCOPE)
	set(${RELEASE_NAME} ${${RELEASE_NAME}} ${REL_DLL} PARENT_SCOPE)
  
endfunction()

function(GetQt5OtherDLLs OTHER_NAME)
	if (NOT DEFINED QT_BINARY_DIR)
		message("QT_BINARY_DIR is not defined, cannot get additional Qt DLLs")
	else()
		file(GLOB found_dlls ${QT_BINARY_DIR}/*.dll)
		file(GLOB found_qt_dlls ${QT_BINARY_DIR}/Qt5*.dll)
		
		list(REMOVE_ITEM found_dlls ${found_qt_dlls})
		
		set(${OTHER_NAME} ${${OTHER_NAME}} ${found_dlls} PARENT_SCOPE)
	endif()
endfunction()

MACRO(CopyDLLs DEBUG_DLL_NAME RELEASE_DLL_NAME DEST_DIR_NAME)

if (${CMAKE_GENERATOR} MATCHES "Visual Studio 11")
		# visual studio 12 expects the DLLs in the executable folder.
		# but not the resources!
		# can be changed into the environment property of the project to include the project's directory
		set (DLL_TO_DBG ${${DEST_DIR_NAME}}/Debug)
		set (DLL_TO_RELEASE ${${DEST_DIR_NAME}}/Release)
	else()
		# for other version of visual studio the DLLs are expected into the project folder
		set (DLL_TO_DBG ${${DEST_DIR_NAME}})
		set (DLL_TO_RELEASE ${${DEST_DIR_NAME}})
	endif()
	
	foreach(dll ${DEBUG_DLLS})
		file(COPY ${dll} DESTINATION ${DLL_TO_DBG})
	endforeach()
	
	foreach(dll ${RELEASE_DLLS})
		file(COPY ${dll} DESTINATION ${DLL_TO_RELEASE})
	endforeach()

ENDMACRO()
endif(WIN32)

MACRO (QtWrapUI VAROUTFILES UIFILES OUTDIR)
  #QT4_EXTRACT_OPTIONS(ui_files ui_options ${UIFILES})

  if (NOT EXISTS ${OUTDIR})
	file(MAKE_DIRECTORY ${OUTDIR})
  endif()
  
  #FOREACH (it ${UIFILES})
  #  GET_FILENAME_COMPONENT(outfile ${it} NAME_WE)
  #  GET_FILENAME_COMPONENT(infile ${it} ABSOLUTE)
  #  SET(outfile ${OUTDIR}/ui_${outfile}.h)
  # Qt4:uic was ${QT_UIC_EXECUTABLE}
  #  ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
  #    COMMAND Qt4::uic
  #    ARGS -o ${outfile} ${infile}
  #    MAIN_DEPENDENCY ${infile})
  #  SET(${VAROUTFILES} ${${VAROUTFILES}} ${outfile})
  #ENDFOREACH (it)
  
  foreach (it ${UIFILES})
	#message("generate command ${it}")
    get_filename_component(outfile ${it} NAME_WE)
    get_filename_component(infile ${it} ABSOLUTE)
    set(outfile ${OUTDIR}/ui_${outfile}.h)
    add_custom_command(OUTPUT ${outfile}
      COMMAND Qt5::uic
      ARGS ${ui_options} -o ${outfile} ${infile}
      MAIN_DEPENDENCY ${infile} VERBATIM)
    set(${VAROUTFILES} ${${VAROUTFILES}} ${outfile})
  endforeach ()

ENDMACRO()
