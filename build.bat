@echo off
for /f "tokens=*" %%i in ('dir /s /b "C:\Program Files\Microsoft Visual Studio\*vswhere.exe" 2^>nul') do (
    for /f "tokens=*" %%j in ('"%%i" -latest -property InstallPath 2^>nul') do set "VSINSTALL=%%j"
)
if defined VSINSTALL (
    call "%VSINSTALL%VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
    msbuild "C:\Users\Subway\Desktop\Comprehensive-Programming-Design-Experiment-master\计费管理系统.sln" /p:Configuration=Debug /p:Platform=x64 /v:minimal
) else (
    echo VS not found
)
