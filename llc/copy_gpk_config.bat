md ..\..\Win32.Release
md ..\..\Win32.Debug
md ..\..\x64.Release
md ..\..\x64.Debug

copy *.json ..\..\Win32.Release\*.*
copy *.json ..\..\Win32.Debug\*.*
copy *.json ..\..\x64.Release\*.*
copy *.json ..\..\x64.Debug\*.*
