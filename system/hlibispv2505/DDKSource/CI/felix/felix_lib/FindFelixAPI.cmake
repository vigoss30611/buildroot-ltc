
#
# needs the FelixLib project to exists before called!
#

get_filename_component(FELIXAPI_ROOT "${CMAKE_CURRENT_LIST_FILE}" PATH)
set(FELIXAPI_PREFIX ${FELIXAPI_ROOT}/user)

find_package(RegDefs REQUIRED) # regdefs/felix_config_pack.h
find_package(FelixCommon REQUIRED)

set(FELIXAPI_FOUND TRUE)
set(FELIXAPI_INCLUDE_DIRS 
  ${FELIXAPI_PREFIX}/include 
  ${FelixLib_BINARY_DIR}/user/include # used when ci_version.h is generated
  ${REGDEF_INCLUDE_DIRS}
  ${FELIXCOMMON_INCLUDE_DIRS}
)
set(FELIXAPI_DEFINITIONS
	${FELIXCOMMON_DEFINITIONS}
)
set(FELIXAPI_NAME CI) # used internally
set(FELIXAPI_LIBRARIES ${FELIXAPI_NAME}_User)
set(FELIXAPI_DEPENDENCIES ${REGDEF_NAME}) # for generated files
set(FELIXAPI_INSTALL) # files to copy to the run folder - on windows dlls

if(NOT CI_BUILD_KERNEL_MODULE)
	find_package(LinkedList REQUIRED)
	if(WIN32)
	  find_package(PThreadWin32 REQUIRED)
	  set(FELIXAPI_INCLUDE_DIRS ${FELIXAPI_INCLUDE_DIRS} ${PTHREADWIN32_INCLUDE_DIRS})
	  set(FELIXAPI_DEFINITIONS ${FELIXAPI_DEFINITIONS} ${PTHREADWIN32_DEFINITIONS})
	  set(FELIXAPI_LIBRARIES ${FELIXAPI_LIBRARIES} ${PTHREADWIN32_LIBRARIES})
	  set(FELIXAPI_INSTALL ${FELIXAPI_INSTALL} ${PTHREADWIN32_INSTALL})
	else()
	  set(FELIXAPI_LIBRARIES ${FELIXAPI_LIBRARIES} -lpthread)
	endif()

	# then the path is needed for external libraries to init the module
	set(FELIXAPI_INCLUDE_DIRS 
	  ${FELIXAPI_INCLUDE_DIRS} 
	  ${FELIXAPI_ROOT}/kernel/include
	  ${FELIXAPI_ROOT}/kernel/fake_include
	  ${LINKEDLIST_INCLUDE_DIRS}
	  )

	set(FELIXAPI_LIBRARIES ${FELIXAPI_LIBRARIES} ${FELIXAPI_NAME}_Kernel)

	set(FELIXAPI_DEFINITIONS ${FELIXAPI_DEFINITIONS} ${LINKEDLIST_DEFINITIONS} -DFELIX_FAKE)

	if (CI_DEBUG_FUNCTIONS) # in kernel the cmake option enables CI_DEBUGFS, a compile time option
	  set(FELIXAPI_DEFINITIONS ${FELIXAPI_DEFINITIONS} -DCI_DEBUG_FCT)
	endif()
    if (CI_EXT_DATA_GENERATOR)
        if (NOT FELIXDG_FOUND)
            find_package(FelixDG REQUIRED)
        endif()
        set(FELIXAPI_LIBRARIES ${FELIXAPI_LIBRARIES} ${FELIXDG_LIBRARIES}) # because interrupt handle calls DG interrupt handler when running fake interface
    endif()
	
	set(FELIXAPI_DOXYGEN_SRC
		${FELIXAPI_ROOT}/kernel/kernel_src
		${FELIXAPI_ROOT}/kernel/platform_src
		${FELIXAPI_ROOT}/user/source
		)
    
    if(${CAPTUREINTERFACE_TESTS})
        set(CITEST_FOUND TRUE)
        
        set(CITEST_INCLUDE_DIRS ${FELIXAPI_ROOT}/test/include)
        set(CITEST_DEFINITIONS -DFELIX_UNIT_TESTS)
        if (CI_EXT_DATA_GENERATOR)
            set(CITEST_DEFINITIONS ${CITEST_DEFINITIONS}
                -DFELIX_HAS_DG
            )
        endif()
        set(CITEST_NAME ${FELIXAPI_NAME}_test_lib)
        set(CITEST_LIBRARIES ${CITEST_NAME})
		set(CITEST_DATA ${FELIXAPI_ROOT}/testdata)
    else()
        set(CITEST_FOUND FALSE)
    endif()
endif()

if (DEBUG_MODULES)
  message("FELIXAPI_ROOT=${FELIXAPI_ROOT}")
  message ("FELIXAPI_INCLUDE_DIRS=${FELIXAPI_INCLUDE_DIRS}")
  message ("FELIXAPI_DEFINITIONS=${FELIXAPI_DEFINITIONS}")
  message ("FELIXAPI_LIBRARIES=${FELIXAPI_LIBRARIES}")
endif()

mark_as_advanced(FELIXAPI_ROOT FELIXAPI_INCLUDE_DIRS FELIXAPI_DEFINITIONS FELIXAPI_NAME FELIXAPI_LIBRARIES)
