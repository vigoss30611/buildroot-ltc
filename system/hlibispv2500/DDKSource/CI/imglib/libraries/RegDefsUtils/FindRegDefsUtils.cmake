# Locate and configure the Register Definitions Utils headers
#
# Possible options:
#
# Defines the following variables:
#
#   REGDEFSUTILS_PREFIX - the directory where Skeleton App is located in the project
#
#   REGDEFSUTILS_FOUND - Found the library
#   REGDEFSUTILS_INCLUDE_DIRS - The directories needed on the include paths
#   REGDEFSUTILS_DEFINITIONS - The needed CFlags for the headers
#

get_filename_component(REGDEFSUTILS_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)

set (REGDEFSUTILS_FOUND TRUE)
set (REGDEFSUTILS_INCLUDE_DIRS ${REGDEFSUTILS_PREFIX}/include)
set (REGDEFSUTILS_DEFINITIONS)
