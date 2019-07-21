md ..\..\Win32.Release
md ..\..\Win32.Debug
md ..\..\x64.Release
md ..\..\x64.Debug

copy gpk_config.json ..\..\Win32.Release\*.*
copy gpk_config.json ..\..\Win32.Debug\*.*
copy gpk_config.json ..\..\x64.Release\*.*
copy gpk_config.json ..\..\x64.Debug\*.*
