find_package(GenDoxyfile QUIET) # to have doxygen aliases and generate documentation
find_package(Doxygen QUIET) # find doxygen executable

if (GENDOXYFILE_FOUND)
  # defaults
  set(FELIXAPI_FOUND FALSE)
  set(ISPC2_FOUND FALSE)
  set(SENSORAPI_FOUND FALSE)

  find_package(ImgIncludes REQUIRED)
  find_package(FelixAPI QUIET) # optional
  find_package(SensorApi QUIET) # optional
  find_package(ISPC2 QUIET) # optional
  find_package(FelixCommon QUIET)

  message (STATUS "== Generating general Doxyfile ==")

  message(STATUS "    CI ${FELIXAPI_FOUND}")
  message(STATUS "    Sensor ${SENSORAPI_FOUND}")
  message(STATUS "    ISPC2 ${ISPC2_FOUND}")

  if (DEFINED DOC_THEME_FOLDER)

    message(STATUS "    using ${DOC_THEME_FOLDER} as theme folder")
    file(GLOB_RECURSE extra_files "${DOC_THEME_FOLDER}/*")

    foreach(found ${extra_files})
      set (DOXY_HTML_EXTRA_FILES "${DOXY_HTML_EXTRA_FILES} \"${found}\"")

      get_filename_component(FILE_name ${found} NAME_WE)
      get_filename_component(FILE_ext ${found} EXT)
      if (${FILE_name} STREQUAL "header" )
        set(found_header ${found})
        #message(STATUS "    - found header ${found_header}")
      elseif(${FILE_name} STREQUAL "footer" )
        set(found_footer ${found})
        #message(STATUS "    - found footer ${found_footer}")
      endif()
      if(${FILE_ext} STREQUAL ".css" )
        set(found_css ${found})
        #message(STATUS "    - found CSS ${found_css}")
      endif()
    endforeach()

    if(DEFINED found_header AND NOT DEFINED DOXY_HTML_HEADER)
      message(STATUS "    - using header ${found_header} (if incorrect define DOXY_HTML_HEADER)")
      set(DOXY_HTML_HEADER ${found_header})
    endif()
    if(DEFINED found_footer AND NOT DEFINED DOXY_HTML_FOOTER)
      message(STATUS "    - using footer ${found_footer} (if incorrect define DOXY_HTML_FOOTER)")
      set(DOXY_HTML_FOOTER ${found_footer})
    endif()
    if(DEFINED found_css AND NOT DEFINED DOXY_HTML_CSS)
      message(STATUS "    - using css ${found_css} (if incorrect define DOXY_HTML_CSS)")
      set(DOXY_HTML_CSS ${found_css})
    endif()

  endif()

  set (DOC_GEN
    ${IMGINCLUDES_INCLUDE_DIRS}
    ${FELIXAPI_INCLUDE_DIRS}
    ${FELIXDG_INCLUDE_DIRS} # should it be optional?
    ${SENSORAPI_INCLUDE_DIRS}
    ${ISPC2_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR} # for the pages
    ${FELIXCOMMON_INCLUDE_DIRS}
    )

  set(PREDEFINED
    ${IMGINCLUDES_DEFINITIONS}
    ${FELIXAPI_DEFINITIONS}
    ${FELIXDG_DEFINITIONS}
    ${SENSORAPI_DEFINITIONS}
    ${ISPC2_DEFINITIONS}
    ${FELIXCOMMON_DEFINITIONS}
    )
	
 if (EXISTS "${ANDROID_CAMERA_DIR}")
       message(STATUS "    android camera dir ${ANDROID_CAMERA_DIR}")
	set (DOC_GEN ${DOC_GEN}
		${ANDROID_CAMERA_DIR} # if android is present can be added to the doxygen
	)
	# can add -D here
	#set(PREDEFINED ${PREDEFINED})
 endif()

  # remove the leading -D
  if(NOT ("${PREDEFINED}" STREQUAL ""))
    list(REMOVE_DUPLICATES PREDEFINED)
    foreach(def ${PREDEFINED})
      string(REPLACE "-D" "" def2 ${def})
      set(DOXY_PREDEFINED ${DOXY_PREDEFINED} ${def2})
    endforeach(def)
  else()
    message("PREDEFINED variable is empty!")
  endif()

  set(DOXY_EXCLUDE_PATH
    ${REGDEF_BINARY_INCLUDE} # add here the path to the register files to avoid all the warnings when generating the doc
    )

  #set(DOXY_EXAMPLE_PATH
  #     ${CMAKE_CURRENT_SOURCE_DIR}/example
  #)

  set (DOXY_IMAGE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
  if(NOT DEFINED DOXY_PROJ_NAME)
    set (DOXY_PROJ_NAME "V2500 APIs documentation")
  endif()
  set (DOC_DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/V2500_API.Doxyfile)

  GenerateDoxyfile (${DOXYGEN_HTML_TEMPLATE} "${DOC_DOXYFILE}"
    ${DOXY_PROJ_NAME}
    "${CMAKE_BINARY_DIR}/doc/V2500"
    FALSE
    "${DOC_GEN}"
    )

  if (DOXYGEN_FOUND MATCHES "YES")
    message (STATUS "    ${DOXY_PROJ_NAME} generation target")
    set(DOXY_HAVE_DOT ${DOXYGEN_HAVE_DOT})
    set(DOXY_DOT_PATH ${DOXYGEN_DOT_PATH})
    add_custom_target(doc
      COMMAND ${DOXYGEN} ${DOC_DOXYFILE}
      COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}/doc ${CMAKE_COMMAND} -E tar czf V2500-html_doc.tgz V2500/
      COMMENT "Generate ${DOXY_PROJ_NAME}"
      DEPENDS ${FELIXAPI_DEPENDENCIES}
      )

    install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/V2500 DESTINATION ./docs OPTIONAL)
  endif()

endif()
