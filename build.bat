@echo off
rm -r bin\assets
cp -r assets bin
devenv workspace\ReferencePal.sln /Build
call .\bin\referencepal.exe
