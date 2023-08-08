Remove-Item $PSScriptRoot/build -Recurse -Force -ErrorAction:SilentlyContinue -Confirm:$False | Out-Null
& cmake --build build --preset=REL -DPLUGIN_MODE:BOOL=TRUE --config Release