@echo off
cd /d "C:\Users\Subway\Desktop\Comprehensive-Programming-Design-Experiment-master"
msbuild "计费管理系统.sln" /p:Configuration=Debug /p:Platform=x64 /v:minimal
