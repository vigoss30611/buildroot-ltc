# This file is designed to be included in a top level CMakeLists.txt like so:
#
#     include(path/to/here/UsePackage.cmake)
#
# It will add to the module path all subdirectories with a FindXXX.cmake file

# Do everything inside a function so as not to clash with any existing variables
function(AddDirsToModulePath)
   get_filename_component(CURRENT_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
   
   include(${CURRENT_DIR}/felix/UsePackage.cmake)
   include(${CURRENT_DIR}/imglib/UsePackage.cmake)
   include(${CURRENT_DIR}/target/UsePackage.cmake)
   if(NOT CI_BUILD_KERNEL_MODULE)
     # transif support to run test apps against simulator loaded with a dll
     set(TRANSIF_BUILD_SLAVE FALSE PARENT_SCOPE)
     set(TRANSIF_BUILD_MASTER TRUE PARENT_SCOPE) # master is the one that imports the library
     set(TRANSIF_NO_PDUMP_PLAYER TRUE PARENT_SCOPE)

     include(${CURRENT_DIR}/transif/UsePackage.cmake)
   endif()
   set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
     ${CURRENT_DIR}/felix/sim_image
     ${CURRENT_DIR}/imgmmu
	 ${CURRENT_DIR}/osa
     PARENT_SCOPE
     )

   set(FELIXDIR ${CURRENT_DIR} PARENT_SCOPE)
endfunction()

if(DEBUG_MODULE)
  message("CMAKE_MODULE_PATH before: ${CMAKE_MODULE_PATH}")
endif()

AddDirsToModulePath()
include(${FELIXDIR}/felix/project.cmake) # project options - not in the function to have the variables available

if(DEBUG_MODULE)
  message("CMAKE_MODULE_PATH  after: ${CMAKE_MODULE_PATH}")
endif()