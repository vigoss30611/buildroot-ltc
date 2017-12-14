@echo off

copy CMakeLists.txt CMakeLists.txt.ori
attrib -r CMakeLists.txt

rem Because VS 2012 has some trouble with standard C++ classes used in gtest
echo add_definitions(-D_VARIADIC_MAX=10) > CMakeLists.txt
type CMakeLists.txt.ori >> CMakeLists.txt
echo install(TARGETS gtest gtest_main DESTINATION lib) >> CMakeLists.txt
echo install(DIRECTORY ${gtest_SOURCE_DIR}/include DESTINATION .) >> CMakeLists.txt
