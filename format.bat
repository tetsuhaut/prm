@echo off
where /q AStyle || ECHO Could not find the AStyle command. Make sure it is in the path. && EXIT /B 1
AStyle --recursive --options=%~dp0\astyle-command-line.txt --quiet %~dp0/src/main/cpp/*.*pp
rem AStyle --recursive --options=%~dp0\astyle-command-line.txt --quiet %~dp0/src/test/cpp/*.*pp
@echo on