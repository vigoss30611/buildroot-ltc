project(libzbar)
cmake_minimum_required(VERSION 2.8)

include_directories(${libzbar_SOURCE_DIR} ${libzbar_SOURCE_DIR}/include)
add_definitions(-DHAVE_SYS_TIME_H -DENABLE_QRCODE -Os -fvisibility=hidden)

# svg debug
#link_libraries(png)
#add_definitions(-DDEBUG_SVG)
link_libraries(iconv)

# --> files
set(libzbarfiles img_scanner.c error.c scanner.c
	symbol.c decoder.c image.c refcnt.c decoder/qr_finder.c)

set(libzbarfiles ${libzbarfiles}
	qrcode/bch15_5.c qrcode/binarize.c qrcode/isaac.c
	qrcode/qrdec.c qrcode/qrdectxt.c qrcode/rs.c qrcode/util.c)

add_library(zbar SHARED ${libzbarfiles})
add_library(zbar_static STATIC ${libzbarfiles})
set_target_properties(zbar_static PROPERTIES OUTPUT_NAME zbar CLEAN_DIRECT_OUTPUT 1)

install(TARGETS zbar DESTINATION lib)
install(TARGETS zbar_static DESTINATION lib)

