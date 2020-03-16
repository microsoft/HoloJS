echo OFF

if "%~1"=="" goto MISSING_CONFIGURATION_PARAM

if "%~2"=="" goto MISSING_PLATFORM_PARAM

if not %1==Release (if not %1==Debug goto BAD_CONFIGURATION)

if not %2==Win32 (if not %2==x64 goto BAD_PLATFORM)

echo Initializing ANGLE build ...

pushd angle-google

mkdir out\%1-%2
pushd out\%1-%2
if %2==Win32 ( echo target_cpu = "x86" > args.gn ) else ( echo target_cpu = "x64" > args.gn )
if %1 ==Release ( echo is_debug = false >> args.gn ) else ( echo is_debug = true >> args.gn )
echo is_clang = false >> args.gn
popd
call gn gen out\%1-%2

mkdir out\%1-%2-uwp
pushd out\%1-%2-uwp
if %2==Win32 ( echo target_cpu = "x86" > args.gn ) else ( echo target_cpu = "x64" > args.gn )
if %1 ==Release ( echo is_debug = false >> args.gn ) else ( echo is_debug = true >> args.gn )
echo is_clang = false >> args.gn
echo target_os = "winuwp" >> args.gn
popd
call gn gen out\%1-%2-uwp

popd

echo Initializing ZXING build ...

pushd zxing-cpp
mkdir out\%2
pushd out\%2
cmake ..\..\core -G "Visual Studio 16 2019" -A %2
popd
popd

echo Initializing LabSound build ...

pushd LabSound
mkdir out\%2
pushd out\%2
cmake ..\.. -G "Visual Studio 16 2019" -A %2
popd
popd

exit /B

:MISSING_CONFIGURATION_PARAM
echo Missing Configuration parameter. Can be either "Release" or "Debug"
exit /B

:MISSING_PLATFORM_PARAM
echo Missing Platform parameter. Can be either "Win32" or "x64"
exit /B

:BAD_CONFIGURATION
echo Unknown Configuration. Can be either "Release" or "Debug"
exit /B

:BAD_PLATFORM
echo Unknown Platform. Can be either "Win32" or "x64"
exit /B