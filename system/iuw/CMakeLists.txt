project(iuw)
cmake_minimum_required(VERSION 2.8)


set(CMAKE_CXX_FLAGS "-Wall -Werror -Wl,--as-needed")
include_directories(${iuw_SOURCE_DIR}/include)
link_libraries(ssl crypto)

# build iuw
add_executable(iuw
   main.c
   genisi.c
   mkius.c
   mkburn.c
   lib/crc32.c
   lib/md5.c
   lib/hash.c
   lib/ius.c
   lib/ixlparser.c
   lib/items.c
)

# install
install(TARGETS iuw DESTINATION bin)

