cmake_minimum_required(VERSION 2.8)

# ----------------------------------------------------------------------
# Specific Include List Given
# ----------------------------------------------------------------------

if(DEFINED IMGLIB_BUILD_LIST)
	foreach(lib ${IMGLIB_BUILD_LIST})
		if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libraries/${lib})
		    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/libraries/${lib}/CMakeLists.txt)
				#message(STATUS "-- ${lib}")
		        add_subdirectory(libraries/${lib})
		    endif()
		endif()
	endforeach()
else()

# ----------------------------------------------------------------------
# Build ImgLib sublibraries by searching the file structure
# ----------------------------------------------------------------------
	file(GLOB sublibs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} libraries/*)
	
	# Remove any excluded libs
	if(DEFINED IMGLIB_DONT_BUILD_LIST)
		foreach(exclude ${IMGLIB_DONT_BUILD_LIST})
			list(REMOVE_ITEM sublibs libraries/${exclude})	  
		endforeach()
	endif()

	foreach(lib ${sublibs})
		if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${lib})
		    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${lib}/CMakeLists.txt)
		        add_subdirectory(${lib})
		    endif()
		endif()
	endforeach()

endif()
