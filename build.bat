@ECHO OFF
SETLOCAL
IF NOT DEFINED COMPILER ECHO need to set the COMPILER environment variable && EXIT /B 1
IF NOT DEFINED CMAKE_GENERATOR ECHO call build-*.bat instead && EXIT /B 1
IF NOT DEFINED BUILD_DIR ECHO call build-*.bat instead && EXIT /B 1
IF NOT DEFINED BIN_DIR ECHO call build-*.bat instead && EXIT /B 1

REM the *_DIR environment variables are hints to help CMake finding the libraries and include dirs
REM one letter for the loop variable
SETLOCAL enabledelayedexpansion
FOR %%i IN (Boost_DIR FLTK_DIR frozen_DIR Microsoft.GSL_DIR SCRIPTS_DIR) DO (
  IF NOT DEFINED %%i ECHO need to set the %%i environment variable && EXIT /B 1
  FOR /F %%j IN ('ECHO %%i') DO (
    IF NOT EXIST !%%j!\NUL ECHO %%i refers to a non existing directory '!%%j!' && EXIT /B 1
  )
)
ENDLOCAL

IF NOT EXIST %SCRIPTS_DIR%\timecmd.bat ECHO the script 'timecmd.bat' could not be found && EXIT /B 1

IF EXIST %BUILD_DIR% (RMDIR /Q /S %BUILD_DIR%)
MKDIR %BUILD_DIR%
PUSHD %BUILD_DIR%
@ECHO ON
cmake -G %CMAKE_GENERATOR% %~dp0
@ECHO OFF
POPD
ECHO ^pushd %CD% ^&^& ^format ^&^& ^popd > %BUILD_DIR%\format.bat
ECHO ^color ^&^& ^cls ^&^& ^%BIN_DIR%\unitTests.exe %%* > %BUILD_DIR%\test.bat
ECHO ^color ^&^& ^cls ^&^& ^%BIN_DIR%\unitTests.exe --run_test=%%* -l all > %BUILD_DIR%\testOne.bat
ECHO ^%SCRIPTS_DIR%\timecmd.bat ^%BIN_DIR%\unitTests.exe > %BUILD_DIR%\measure.bat
ECHO ^%BIN_DIR%\reviewer.exe > %BUILD_DIR%\reviewer.bat
REM ENDLOCAL
@ECHO ON