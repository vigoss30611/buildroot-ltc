cmake_minimum_required(VERSION 2.8)

project(DmaBuf)

find_package (DmaBufExporter QUIET)

if(DMABUF_EXPORTER_FOUND)
  message(STATUS "-- Userspace dma-buf allocator")

  find_package (DmaBuf REQUIRED)
  find_package (ImgIncludes REQUIRED)
  find_package (FelixCommon REQUIRED)

  include_directories(${DMABUF_INCLUDE_DIRS} ${DMABUF_EXPORTER_INCLUDE_DIRS} ${IMGINCLUDES_INCLUDE_DIRS} ${FELIXCOMMON_INCLUDE_DIRS})
  add_definitions(${DMABUF_DEFINITIONS} ${DMABUF_EXPORTER_DEFINITIONS} ${IMGINCLUDES_DEFINITIONS})

  set(SOURCES
	${DMABUF_PREFIX}/code/dmabuf.c
)

  set(HEADERS
	include/dmabuf/dmabuf.h
)

  add_library(${DMABUF_NAME} ${SOURCES} ${HEADERS})
else()
  message(STATUS "dmabuf_exporter not found in cmake tree, not building userspace dmabuf library")
endif()


