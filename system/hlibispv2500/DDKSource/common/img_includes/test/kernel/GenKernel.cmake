
if(NOT DEFINED GENKERN_PREFIX)
  # set FELIXLIB_PREFIX to directory where the current file file is located
  get_filename_component(GENKERN_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
endif()

#
# Transform a cmake list (; separated) into space separated with a prefix (e.g. -I)
#
function(GenSpaceList LIST PREFIX PARENT_SCOPE_LIST_NAME)

  set (TMP)
  foreach(v ${LIST})
    set (TMP "${TMP} ${PREFIX}${v}")
  endforeach()

  set (${PARENT_SCOPE_LIST_NAME} ${TMP} PARENT_SCOPE)

endfunction()

#
# Generate a kernel module Makefile usable with Kbuild
#
# KO_MODULE_NAME is the name of the kernel target
# KO_CFLAGS list of cflags as a string with -D and separated with spaces
# KO_INCLUDES list of includes as string with -I and separated with spaces
# KO_SOURCES is a list of files (normal CMake list work) to compile for the module
# KO_OUTDIR is the place where all C files will be copied and the Makefile generated
#
# Note you can use GenSpaceList and get_directory_property to get the spaced lists:
# e.g.
#  get_directory_property(KO_INCLUDES INCLUDE_DIRECTORIES)
#  GenSpaceList("${KO_INCLUDES}" "-I" KO_INCLUDES)
#
function(GenKernel_Makefile KO_MODULE_NAME KO_CFLAGS KO_SOURCES KO_INCLUDES KO_OUTDIR)

  if (NOT EXISTS ${KO_OUTDIR})
    file(MAKE_DIRECTORY ${KO_OUTDIR})
  endif()
  if (NOT EXISTS ${KO_OUTDIR}/include)
    file(MAKE_DIRECTORY ${KO_OUTDIR}/include)
  endif()

  set (KO_MODE "m")
#  set (TMP) # reset
#  foreach(flag ${KO_CFLAGS})
#    set (TMP "${TMP} -D${flag}")
#  endforeach()
#  set (KO_CFLAGS ${TMP})

#  set (TMP) # reset
#  foreach(dir ${KO_INCLUDES})
#    set (TMP "${TMP} -I${dir}")
#  endforeach()
#  set (KO_INCLUDES ${TMP})
  set(TO_CLEAN
    ${KO_OUTDIR}/${KO_MODULE_NAME}.ko
    ${KO_OUTDIR}/${KO_MODULE_NAME}.o
    ${KO_OUTDIR}/${KO_MODULE_NAME}.mod.c
    ${KO_OUTDIR}/${KO_MODULE_NAME}.mod.o
    ${KO_OUTDIR}/modules.order
    ${KO_OUTDIR}/Module.symvers
    ${KO_OUTDIR}/built-in.o
    )

  set (KO_INCLUDES "${KO_INCLUDES} -I${KO_OUTDIR}/include")
  set (TMP) # reset
  foreach(file ${KO_SOURCES})
    # copy file to local dir - c files in local, h files in ./include
    # extract filename without extention
    get_filename_component (filename ${file} NAME_WE)
    get_filename_component (extension ${file} EXT)

    # for a module called MyMod.ko a file MyMod.c is needed to define the point of entry
    # and other things but shouldn't be in the sources
    if (${extension} MATCHES ".c" )

      execute_process(COMMAND ln -s ${file} ${KO_OUTDIR}/${filename}${extension})
      #if (NOT ${filename} MATCHES ${KO_MODULE_NAME})
	set (TMP "${TMP} ${filename}.o")
	set (TO_CLEAN ${TO_CLEAN} ${KO_OUTDIR}/${filename}.o)
      #endif()

    elseif (${extension} MATCHES ".h" )
 
      execute_process(COMMAND ln -s ${file} ${KO_OUTDIR}/include/${filename}${extension})     

    else()
      
      message ("unkown extension for file ${file}")

    endif()
  endforeach()
  set (KO_SOURCES ${TMP})

  message (STATUS "generating ${KO_OUTDIR}/Makefile")
  configure_file(${GENKERN_PREFIX}/kernel_makefile.cmakein ${KO_OUTDIR}/Makefile)
  
  set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${TO_CLEAN}")

endfunction()

#
# Generate a simple Kconfig file
#
# KO_MODULE_NAME
# KO_BUILD_MODULE - if yes set the KO_BUILD to tristate (kernel module, inside kernel or not built) if no set to bool (inside kernel or not built)
# KO_BRIEF - brief description of the module
# KO_DEFAULT - default mode (y for inside kernel, m for module - if KO_BUILD_MODULE=YES - or n for not built)
# KO_HELP - description of the module
# KO_OUTDIR
# KO_DEPENDS - string that defines the dependencies e.g. "depends on V4L && !DMA"
#
function(GenKernel_Kconfig KO_MODULE_NAME KO_BUILD_MODULE KO_BRIEF KO_BUILD_DEFAULT KO_HELP KO_OUTDIR KO_DEPENDS)
  
  if (NOT EXISTS ${KO_OUTDIR})
    file(MAKE_DIRECTORY ${KO_OUTDIR})
  endif()

  set(KO_DEFAULT "n")
  
  if (KO_BUILD_MODULE)
    set(KO_BUILD "tristate")
    if (${KO_BUILD_DEFAULT})
      set(KO_DEFAULT "m")
    endif()
  else()
    set(KO_BUILD "bool")
    if (${KO_BUILD_DEFAULT})
      set(KO_DEFAULT "y")
    endif()
  endif()
    

  set(KO_EXTRA ${KO_DEPENDS})

  message (STATUS "generating ${KO_OUTDIR}/Kconfig")
  configure_file(${GENKERN_PREFIX}/kernel_kconfig.cmakein ${KO_OUTDIR}/Kconfig)

endfunction()

#
# Add custom target ${KO_MODULE_NAME}
#
# KO_MODULE_NAME is the name of the kernel target
# KO_OUTDIR is the place where all C files will be copied and the Makefile generated
#
# Note you can use GenSpaceList and get_directory_property to get the spaced lists:
# e.g.
#  get_directory_property(KO_INCLUDES INCLUDE_DIRECTORIES)
#  GenSpaceList("${KO_INCLUDES}" "-I" KO_INCLUDES)
#
function(GenKernel_addTarget KO_MODULE_NAME KO_OUTDIR)

  if(NOT DEFINED LINUX_KERNEL_BUILD_DIR)
    message(FATAL_ERROR "LINUX_KERNEL_BUILD_DIR should be defined")
  endif()

  add_custom_target(${KO_MODULE_NAME} ALL 
    COMMAND ${CMAKE_MAKE_PROGRAM} -C ${LINUX_KERNEL_BUILD_DIR} M=${KO_OUTDIR} v=1
    WORKING_DIRECTORY ${KO_OUTDIR}
    COMMENT "Building the kernel module: ${KO_MODULE_NAME}"
    )
endfunction()