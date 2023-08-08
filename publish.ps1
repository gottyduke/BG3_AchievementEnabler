if (!(Test-Path $PSScriptRoot\dist\bink2w64.dll) -or 
    !(Test-Path $PSScriptRoot\dist\bink2w64_original.dll) -or 
    !(Test-Path $PSScriptRoot\build\Release\*)) {
    exit
}

Remove-Item $PSScriptRoot\NativeMods -Force -Recurse -ErrorAction:SilentlyContinue | Out-Null
Remove-Item $PSScriptRoot\*.zip -Force -Recurse -ErrorAction:SilentlyContinue | Out-Null
New-Item -ItemType Directory -Path $PSScriptRoot\NativeMods | Out-Null
Copy-Item $PSScriptRoot\build\Release\* $PSScriptRoot\NativeMods\
Compress-Archive -Path $PSScriptRoot\dist\bink2w64.dll, $PSScriptRoot\dist\bink2w64_original.dll -DestinationPath $PSScriptRoot\Part-1-NativeModLoader.zip -Force
Compress-Archive -Path $PSScriptRoot\NativeMods\ -DestinationPath $PSScriptRoot\Part-2-BG3AE.zip -Force
Remove-Item $PSScriptRoot\NativeMods -Force -Recurse  -ErrorAction:SilentlyContinue | Out-Null