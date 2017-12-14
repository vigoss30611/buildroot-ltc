project(target)

FILE(GLOB sources_target RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

FILE(GLOB sources_pdump  "${IMG_WORKROOT}/target/libraries/pdump/common/code/*.c")

set(sources
  ${sources_target}
  ${sources_pdump}
  ${IMG_WORKROOT}/target/frc/${OS_NAME}/code/target.c
  ${IMG_WORKROOT}/target/libraries/tal/normal/code/tal.c
  ${IMG_WORKROOT}/target/libraries/mem/stubs/code/mem_api.c
  ${IMG_WORKROOT}/target/libraries/pdump/file/code/pdump.c
  ${IMG_WORKROOT}/target/libraries/addr_alloc/code/addr_alloc.c
  ${IMG_WORKROOT}/target/libraries/target_config/code/target_config.c
  ${IMG_WORKROOT}/target/libraries/wrap/pdump_tools/code/pagemem.c
  ${IMG_WORKROOT}/target/libraries/wrap/pdump_tools/code/sgxmmu.c
  ${IMG_WORKROOT}/target/libraries/wrap/pdump_tools/code/wrap.c
)


add_definitions(
  ${cpp_flags_csim}
  ${cpp_flags_common}
)

# cpp_flags_common needs to be set to something like this to build
# on linux but that should be done in a higher level CMakeLists.txt
#
#set(cpp_flags_common
#  -D__IMG_PRIVATE_APIS__ 
#  -DCYGWIN 
#  -D__linux__
#  -D_FILE_OFFSET_BITS=64 
#  -DNDEBUG 
#  -D__RELEASE_RELEASE__ 
#  -D__RELEASE__=\"release\" 
#  -D__FRC_HW__=\"frc\" 
#  -D__IMG_PROJ__=\"FRC\" 
#  -D__TALMMU_USE_MALLOC__ 
#  -D__TAL_MALLOC_MEM_CBS__ 
#  -D__TAL_EXCLUDE_MEM_API__ 
#  -D__TAL_MAKE_THREADSAFE__ 
#  -D__EXCLUDE_TARGETDLL_API__ 
#  -DNO_REGIO_CHECK_FIELD_VALUE 
#  -DCMD_DONT_ASSERT_ON_ERROR 
#  -DOSAL_TIMER_TICKS=1000 
#  -DTIM_TICK_PERIOD=1
#  -DINCLUDED_PDUMP_CMDS 
#  -DINCLUDED_PDUMP_CMDS_KM 
#  -DINCLUDED_MMU_API 
#  -DENABLE_SOCKET_DEVIF 
#  -DEXCLUDE_FWRKUTIL
#  -D_CRT_SECURE_NO_WARNINGS 
#  -D_SCL_SECURE_NO_WARNINGS 
#)


include_directories(
  ${PROJECT_SOURCE_DIR}

  ${IMG_WORKROOT}/include
  ${IMG_WORKROOT}/include/${OS_NAME}
  ${IMG_WORKROOT}/apis/frc/include
  ${IMG_WORKROOT}/register_includes/frc
  ${IMG_WORKROOT}/apis/common/include
  ${IMG_WORKROOT}/imglib/include
  ${IMG_WORKROOT}/target/include
  ${IMG_WORKROOT}/system/frc/include
  ${IMG_WORKROOT}/system/frc/${OS_NAME}/include
  ${IMG_WORKROOT}/meosal/meos4.3/meosal/include
  ${IMG_WORKROOT}/meosal/meos4.3/meosal/include/meos

  ${IMG_WORKROOT}/target/libraries/pdump/file/code
)

add_library(target ${sources})
