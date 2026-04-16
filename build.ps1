$ErrorActionPreference = "SilentlyContinue"
$vswhere = "${env:ProgramFiles}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) {
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
}
if (Test-Path $vswhere) {
    $installPath = & $vswhere -latest -property InstallPath
    if ($installPath) {
        $vcvars = Join-Path $installPath "VC\Auxiliary\Build\vcvarsall.bat"
        if (Test-Path $vcvars) {
            Write-Host "Found VS at: $installPath"
            & cmd /c " `"$vcvars`" x64 && msbuild `"C:\Users\Subway\Desktop\Comprehensive-Programming-Design-Experiment-master\计费管理系统.sln`" /p:Configuration=Debug /p:Platform=x64 /v:minimal "
            exit $LASTEXITCODE
        }
    }
}
Write-Host "VS not found"
