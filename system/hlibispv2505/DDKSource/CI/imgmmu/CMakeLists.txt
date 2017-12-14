cmake_minimum_required(VERSION 2.8)
#
# CMake options:
# - IMGMMU_TESTS TRUE|FALSE build unit tests if gtest is found (default: FALSE)
# - IMGMMU_SIZE int (see compilation options)
# - IMGMMU_TALHEAP TRUE|FALSE build the device virtual allocation using TAL functions - also includes the TALALLOC alllocator (default: TRUE)
#
# Compilation options (see mmu_defs.h for details)
# - IMGMMU_SIZE physical address size of the MMU in bits (default: 40)
# - IMGMMU_PAGE_SIZE page size in bytes (code default: 4096) - not changed from CMake at the moment
# If IMGMMU_PAGE_SIZE is defined the following HAVE TO be defined as well:
# - IMGMMU_PAGE_SHIFT as log2(IMGMMU_PAGE_SIZE) (code default: 12) - used in virtual address to dertimine the position of the page offset
# - IMGMMU_DIR_SHIFT as log2(IMGMMU_PAGE_SIZE)*2-2 (code default: 22) - used in virtual address to determine the position of the directory offset
#
# Uses IMG_KERNEL_MODULE to know if currently in kernel code or not
#

project(IMGMMU)

find_package(ImgIncludes REQUIRED)
find_package(IMGMMU REQUIRED)
find_package(GTest QUIET) # optional
find_package(GenDoxyfile QUIET) # optional, to have doxygen aliases and generate documentation
find_package(Doxygen QUIET) # find doxygen executable

if(NOT DEFINED IMGMMU_TESTS)
	set (IMGMMU_TESTS FALSE)
endif()

if(NOT DEFINED IMGMMU_TALHEAP)
	set(IMGMMU_TALHEAP TRUE)
endif()

if (DEFINED IMGMMU_SIZE)
	add_definitions(-DIMGMMU_PHYS_SIZE=${IMGMMU_SIZE})
endif()

message(STATUS "-- MMU library")

include_directories(
	${IMGINCLUDES_INCLUDE_DIRS}
	${IMGMMU_INCLUDE_DIRS}
	source/ # for the internal header mmu_defs.h
)
add_definitions(
	${IMGINCLUDES_DEFINITIONS}
	${IMGMMU_DEFINITIONS}
)

# potential compilation options

set (HEADERS
	include/mmulib/mmu.h
	include/mmulib/heap.h
)
set (SOURCES
	source/imgmmu.c
	source/mmu_defs.h # should be used at compilation time only
)

if (IMGMMU_TALHEAP)
	find_package(TAL REQUIRED)
	
	include_directories(${TAL_INCLUDE_DIRS})
	add_definitions(${TAL_DEFINITIONS}) # can also define -DTAL_INT_VAR=<int> to choose which TAL internal variable to use
    
    if (DEFINED IMGMMU_TALMEM_MMU_NAME)
        add_definitions(-DTAL_MEM_MMU_NAME="${IMGMMU_TALMEM_MMU_NAME}")
    endif()
    if (DEFINED IMGMMU_TALMEM_NAME)
        add_definitions(-DTAL_MEM_NAME="${IMGMMU_TALMEM_NAME}")
    endif()
	
	set(HEADERS ${HEADERS}
		include/talalloc/talalloc.h
	)
	set(SOURCES ${SOURCES}
		source/tal_heap.c # heap implementation
		source/tal_memory.cpp # function to use TAL for memory operation (can be used in MMU interface)
	)
	set(LINKWITH ${TAL_LIBRARIES})
endif()

add_library(${IMGMMU_NAME} STATIC ${SOURCES} ${HEADERS})
if (DEFINED LINKWITH)
	target_link_libraries(${IMGMMU_NAME} ${LINKWITH})
endif()

if (GTEST_FOUND AND IMGMMU_TESTS) # build unit tests

	add_subdirectory(test)
	
endif()

if (GENDOXYFILE_FOUND) # search for at previous level

	message (STATUS "    generating ${IMGMMU_NAME} Doxyfile")
	set (DOC_GEN 
		${CMAKE_CURRENT_SOURCE_DIR}/include/
		${CMAKE_CURRENT_SOURCE_DIR}/source/
		${IMGINCLUDES_INCLUDE_DIRS}
	)
	
	#
	# doxygen related outputs
	#
	set(DOXY_IMAGE_PATH ${IMGMMU_PREFIX}/doc) # if using doxygen this path has to be added to the IMAGE_PATH
		
	set (DOC_DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/IMGMMU.Doxyfile)

	GenerateDoxyfile (${DOXYGEN_HTML_TEMPLATE} "${DOC_DOXYFILE}"
		"IMG MMU library"
		"${CMAKE_BINARY_DIR}/doc/IMGMMU"
		FALSE
		"${DOC_GEN}"
	)

	if (DOXYGEN_FOUND MATCHES "YES")
		message (STATUS "    ${IMGMMU_NAME} documentation generation target")
		add_custom_target(${IMGMMU_NAME}_doc
		  COMMAND ${DOXYGEN} ${DOC_DOXYFILE}
		  COMMENT "Generate ${IMGMMU_NAME} doxygen documentation"
		  DEPENDS ${IMGMMU_NAME}
		)
	endif()

endif()
