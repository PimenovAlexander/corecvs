@echo off
rem Locates CMake and updates path variable. Sets generator_string and generator_string_x86.
rem Set MSVS_LOCATION used to locate correct version of VS when building VS solutions
rem Avoids updating path if correct CMake version is already on path.
rem Prefer cmake version 3.14.5 identified by the environment variable CMAKE_3_14_5.
rem This is particularly important on builds servers.
rem If CMAKE_3_14_5 is unset, use what is on path or what is installed in standard locations.

if not defined CMAKE_3_14_5 (
    goto use_heuristics_to_locate_cmake
)

rem Check if any cmake version is already on path.
where cmake
if %ERRORLEVEL% equ 1 (
    rem No cmake found on path, add it
    goto add_cmake_to_path
)
rem At least one cmake version found. Check if the first occurrence is equivalent to CMAKE_3_14_5.
for /f "delims=" %%A in ('where cmake') DO (
    set FIRST_CMAKE_ON_PATH=%%A
    goto :exit_loop_on_first_occurence
)
:exit_loop_on_first_occurence
if "%CMAKE_3_14_5%\cmake.exe" == "%FIRST_CMAKE_ON_PATH%" (
    rem CMAKE_3_14_5 is already on path.
    goto callcmake
)
:add_cmake_to_path
rem Add CMAKE_3_14_5 to path as first version to hide any other versions.
set "PATH=%CMAKE_3_14_5%;%PATH%"
goto callcmake

:use_heuristics_to_locate_cmake
rem If CMake is on path use this version
where cmake
if %ERRORLEVEL% equ 0 (
    goto callcmake
)

rem Attempt to locate CMake at the standard installed locations
set "PATH=C:\Program Files\CMake\bin;C:\Program Files (x86)\CMake\bin;%PATH%"

:callcmake
rem Output cmake version used
cmake --version

rem Specify Visual Studio version. Use this variable to make it easier to change Visual Studio version
set generator_string="Visual Studio 15 2017 Win64"

rem Specify Visual Studio version. Use this variable to make it easier to change Visual Studio version
set generator_string_x86="Visual Studio 15 2017"

rem Ensure MSVS_LOCATION is set for solutions where we don't use CMake
if defined MSVS_LOCATION goto done
set MSVS_LOCATION=C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\Tools\
if not exist "%MSVS_LOCATION%" set MSVS_LOCATION=C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\Common7\Tools\
:done
