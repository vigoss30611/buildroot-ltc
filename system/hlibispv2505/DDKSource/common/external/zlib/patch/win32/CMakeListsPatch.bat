@echo off

echo. >> CMakeLists.txt
echo if(MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 8) >> CMakeLists.txt
echo   set_target_properties(zlibstatic PROPERTIES STATIC_LIBRARY_FLAGS "/MACHINE:X64") >> CMakeLists.txt
echo else() >> CMakeLists.txt
echo   set_target_properties(zlibstatic PROPERTIES STATIC_LIBRARY_FLAGS "/MACHINE:X86") >> CMakeLists.txt
echo endif() >> CMakeLists.txt
