call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
devenv ..\PalmInput\PalmInput.sln  /rebuild "Release|x86"
devenv ..\PalmInput\PalmInput.sln  /rebuild "Release|x64"
mkdir x86
mkdir x64
copy ..\PalmInput\Release\PalmInput.dll    .\x86\
copy ..\PalmInput\x64\Release\PalmInput.dll    .\x64\
"C:\Program Files (x86)\NSIS\makensis.exe" installer.nsi
pause
