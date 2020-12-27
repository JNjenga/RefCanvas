@echo off
rm -r bin\assets
cp -r assets bin
:: devenv.com /SafeMode workspace\ReferencePal.sln /Build
MsBuild workspace\ReferencePal.sln -v:m
if %ERRORLEVEL% EQU 0 call .\bin\referencepal.exe

exit

