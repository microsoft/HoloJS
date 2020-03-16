echo OFF

if "%~1"=="" goto MISSING_CONFIGURATION_PARAM

if "%~2"=="" goto MISSING_PLATFORM_PARAM

if not %1==Release (if not %1==Debug goto BAD_CONFIGURATION)

if not %2==Win32 (if not %2==x64 goto BAD_PLATFORM)

pushd angle-google
call autoninja -C out\%1-%2-uwp
call autoninja -C out\%1-%2
popd

pushd ziplib
msbuild ZipLib.sln /t:Build /p:Configuration="%1";Platform="%2";PlatformToolset=v142
popd

pushd zxing-cpp\out\%2
msbuild Project.sln /t:Build /p:Configuration="%1";PlatformToolset=v142
popd

pushd LabSound\out\%2
msbuild LabSoundExamples.sln /t:Build /p:Configuration="%1";PlatformToolset=v142
popd

pushd Win2D
msbuild Win2D.uap.sln /t:Build /p:Configuration="%1";Platform=%2;WindowsTargetPlatformVersion=10.0.17134.0;PlatformToolset=v142
popd

pushd ChakraCore\Build
msbuild Chakra.Core.sln /t:Build /p:Configuration="%1";Platform=%2;PlatformToolset=v142
popd

pushd ChakraCore-Debugger
..\..\windows\nuget-package-spec\nuget.exe restore
msbuild ChakraCore.Debugger.sln /t:Build /p:Configuration="%1";Platform=%2;PlatformToolset=v142
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