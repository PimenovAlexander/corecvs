@echo off

pushd %~dp0

set config=Release

set build_sln=ON
if /i '%1' == 'OFF' set build_sln=OFF

set configs=Release
set blddir=build

call cmake\find_cmake.bat

if not exist %blddir% mkdir %blddir%

pushd %blddir%

call "%MSVS_LOCATION%..\..\VC\Auxiliary\Build\vcvarsall.bat" x64

echo Generate CoreCVS sln

cmake ..\ -G%generator_string% -Thost=x64 -DCMAKE_CONFIGURATION_TYPES=%configs%
if not %errorlevel% == 0 (
    echo CMake CoreCVS generation error!
    popd
    goto ERROR
)

:BUILD
if '%build_sln%' == 'OFF' GOTO SUCCESS
echo CoreCVS is building
cmake --build . --config %config% -- /m /verbosity:normal
if not %errorlevel%==0 (
    echo CoreCVS build failed
    popd
    goto ERROR
)

popd

:SUCCESS
popd
exit /b 0

:USAGE
echo usage %~n0%~x0   [ON^|OFF] 
echo arg1:  ON=generate and build solution, OFF=generate solution only without build

:ERROR
    popd
    exit /b 1