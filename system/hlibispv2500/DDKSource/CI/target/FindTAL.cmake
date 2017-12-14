cmake_minimum_required (VERSION 2.8)

# ----------------------------------------------------------------------
# Locate and configure the TAL library.
#
# Possible options:
#
#   TAL_TYPE				- The type of TAL to build, normal, portfwrk or 
#							  light (default: normal)
#   TAL_NEW_APIS_ONLY		- To select using the new TAL APIs ONLY 
#							  (default: FALSE)
#	TAL_NO_FAKE_DEVICES		- Ignore fake devices in target config file - devices
#							  were the name starts with "FAKE".  (default: FALSE)
#
# Possible options for "normal" TAL:
#
#   TAL_USING_DEVIF			- If TAL should use DeviceInterface or not, 
#							  default: FALSE
#   TAL_CALLBACK_SUPPORT	- Enable the callback support for register accesses 
#							  made via the TAL (default: FALSE)
#   TAL_USE_OSA				- Use OSA library to make TAL thread safe (default: FALSE)
#                             Mutual exclusive with TAL_USE_MEOS
#   TAL_USE_MEOS            - Use MeOS library to make TAL thread safe (default: FALSE)
#                             Mutual exclusive with TAL_USE_OSA
#   TAL_DEFINE_DEVIF1_FUNCS - Define DEV1 funcs (default: FALSE)
#   TAL_DEINIT_ON_EXIT		- Deinitialise TAL on exit from main thread (default: TRUE) 
# 	TAL_EXCLUDE_DEVIF		- By default the normal TAL will be built with all
#							  devif interfaces enabled.	Any devif interfaces listed 
#							  in TAL_EXCLUDE_DEVIF will be excluded.
# 							  Possible values are:
#									pci, bmem, socket, direct, transif, dash, pdump1, null
#	TAL_MAX_MEMSPACES		- No of memory space control blocks to statically 
#							  allocate in TAL light. Also size of initial allocation 
#							  in other TALs (realloc used to resize)
#
# Possible options for "portfwrk" TAL:
#
#   TAL_BRIDGING			- If we should enable BRIDGING (default FALSE)
#   TAL_PORTFWRK_USER_MODE	- If the TAL Portablity Framework version is to 
#							  be used in user mode (default: FALSE)
#   TAL_USING_DEVIF			- If TAL should use DeviceInterface (default: FALSE)
#							  Should only be used when TAL_PORTFWRK_USER_MODE is
#							  set.
#
# Possible options for "light" TAL:
#
#	TAL_ADD_UTILS			- Builds utils.
#
# Defines the following variables:
#
#   TAL_FOUND			- Found the library
#   TAL_PREFIX			- The directory where this file is located
#   TAL_INCLUDE_DIRS	- The directories needed on the include paths 
#						  for this library
#   TAL_DEFINITIONS	- The definitions to apply for this library
#   TAL_LIBRARIES		- The libraries to link to
#	TAL_SUB_DIRS		- The sub-directories to be added to the build 
#						  for this library
#   TAL_BUILD_KO        - The file that can be included to build the TAL in a kernel module
# ----------------------------------------------------------------------

# ----------------------------------------------------------------------
# Set option defaults
# ----------------------------------------------------------------------

if (NOT DEFINED TAL_TYPE)
	set(TAL_TYPE normal)
endif()
	
if (NOT DEFINED TAL_PORTFWRK_USER_MODE)
	set(TAL_PORTFWRK_USER_MODE FALSE)
endif()

if (NOT DEFINED TAL_BRIDGING)
	set(TAL_BRIDGING FALSE)
endif()

if (NOT DEFINED TAL_NEW_APIS_ONLY)
	set(TAL_NEW_APIS_ONLY FALSE)
endif()

if (NOT DEFINED TAL_USING_DEVIF)
	set(TAL_USING_DEVIF FALSE)
endif()

if (NOT DEFINED TAL_CALLBACK_SUPPORT)
	set(TAL_CALLBACK_SUPPORT FALSE)
endif()

if (NOT DEFINED TAL_USE_OSA)
	set(TAL_USE_OSA FALSE)
endif()

if (NOT DEFINED TAL_USE_MEOS)
	set(TAL_USE_MEOS FALSE)
endif()

if (NOT DEFINED TAL_DEFINE_DEVIF1_FUNCS)
	set(TAL_DEFINE_DEVIF1_FUNCS FALSE)
endif()

if (NOT DEFINED TAL_DEINIT_ON_EXIT)
	set(TAL_DEINIT_ON_EXIT TRUE)
endif()

if (NOT DEFINED TAL_NO_FAKE_DEVICES)
	set(TAL_NO_FAKE_DEVICES FALSE)
endif()

if(TAL_PORTFWRK_USER_MODE)
    set(TAL_TYPE portfwrk)
endif()

if(TAL_USING_PORTFWRK MATCHES true)
	set(TAL_TYPE portfwrk)
endif()

# ----------------------------------------------------------------------
# Find and include DEVIF package
# ----------------------------------------------------------------------
find_package(DEVIF REQUIRED)


# ----------------------------------------------------------------------
# Set package variables
# ----------------------------------------------------------------------
get_filename_component(TAL_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
string(REPLACE "${CMAKE_HOME_DIRECTORY}/"  "" TAL_SUB_DIRS ${TAL_PREFIX})
set(TAL_FOUND TRUE)
set(TAL_INCLUDE_DIRS ${TAL_PREFIX}/include ${DEVIF_INCLUDE_DIRS})
set(TAL_LIBRARIES TAL)
set(TAL_DEFINITIONS ${DEVIF_DEFINITIONS})
set(TAL_BUILD_KO ${TAL_PREFIX}/TAL_kernel.cmake) # file used to compile the TAL in the Kernel

if(TAL_DEFINE_DEVIF1_FUNCS)
    set(TAL_DEFINITIONS ${TAL_DEFINITIONS} -DDEVIF_DEFINE_DEVIF1_FUNCS)
endif()


if(${TAL_TYPE} MATCHES "portfwrk")
	set(TAL_DEFINITIONS ${TAL_DEFINITIONS} -DTAL_PORT_FWRK)
elseif(${TAL_TYPE} MATCHES "normal")
	set(TAL_DEFINITIONS ${TAL_DEFINITIONS} -DTAL_NORMAL -DTAL64)
elseif(${TAL_TYPE} MATCHES "light")
	set(TAL_DEFINITIONS ${TAL_DEFINITIONS} -DTAL_LIGHT)
endif()

if (TAL_USE_MEOS AND TAL_USE_OSA)
	message(FATAL_ERROR "TAL_USE_MEOS and TAL_USE_OSA cannot be both defined")
endif()


mark_as_advanced(TAL_INCLUDE_DIRS TAL_DEFINITIONS TAL_LIBRARIES TAL_SUB_DIRS)


