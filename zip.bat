@ECHO OFF
SETLOCAL
WHERE 7z
IF %ERRORLEVEL% NEQ 0 ECHO The command 7z does not exist. Did you install 7-Zip ?
SET ZIP_NAME=prm.7z
IF EXIST "%ZIP_NAME%" REN %ZIP_NAME% %ZIP_NAME%_old
7z a -mhe=on -t7z -pgo %ZIP_NAME% *.txt *.bat src
IF EXIST "%ZIP_NAME%_old" DEL %ZIP_NAME%_old
@ECHO ON