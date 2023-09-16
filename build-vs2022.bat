@ECHO OFF
REM no SETLOCAL to stay in the build dir so that "test" works
where /q devenv.exe || ECHO Could not find the devenv.exe program. Check that it is in the path. && EXIT /B 1
REM see https://cmake.org/cmake/help/latest/generator/Visual%20Studio%2017%202022.html
SET MY_CMAKE_GENERATOR="Visual Studio 17 2022" -A x64 -T host=x64
SET BUILD_DIR=build
SET BIN_DIR=bin\Debug
rem CALL build.bat
SETLOCAL
IF EXIST %BUILD_DIR% (
  ECHO build dir exists
  ECHO RD /Q /S %BUILD_DIR%
  RD /Q /S %BUILD_DIR%
)
ECHO MD %BUILD_DIR%
MD %BUILD_DIR%
PUSHD %BUILD_DIR%
ECHO cmake -G %MY_CMAKE_GENERATOR% %~dp0
cmake -G %MY_CMAKE_GENERATOR% %~dp0
POPD

@ECHO OFF
IF ERRORLEVEL 1 ECHO Error building the pokerReviewerModulaire Visual Studio project file. && EXIT /B 1
PUSHD %BUILD_DIR%
ENDLOCAL
@ECHO ON