cmake_minimum_required(VERSION 2.8)

#
# included from the TAL's CMakeLists.txt
#
# build the TAL light
#
set(EXTRA ${EXTRA} light.cmake)

set(SOURCES
	libraries/tal/common/code/tal_interrupt.c
	libraries/tal/common/code/tal_test.c
	
    libraries/tal/light/code/tal.c
    libraries/target_config/code/target_light.c
)

add_library(${TAL_LIBRARIES} STATIC ${SOURCES} ${HEADERS} ${EXTRA})

if(DEFINED TAL_TARGET_HEADER_NAME)
  message("WARNING: TAL Light was modified and do not use TAL_TARGET_HEADER_NAME anymore! The configuration structure is given in the code.")
endif()

#
# TAL light should not link with anything
#
