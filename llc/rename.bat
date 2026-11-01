setlocal EnableExtensions EnableDelayedExpansion

if "%~1"=="" goto usage
if "%~2"=="" goto usage

set "SOURCE_PREFIX=%~1"
set "TARGET_PREFIX=%~2"

if "%SOURCE_PREFIX%"=="%TARGET_PREFIX%" (
	echo Source and target prefixes are identical: "%SOURCE_PREFIX%".
	exit /b 1
)

for /f "delims=" %%T in ('powershell.exe -NoLogo -NoProfile -Command "Get-Date -Format yyMMddHHmmss"') do set "TIMESTAMP=%%T"
if not defined TIMESTAMP (
	echo Failed to generate the run timestamp.
	exit /b 1
)
set "LOG_FILE=%CD%\rename_!TIMESTAMP!.log"

> "!LOG_FILE!" echo Root: "%CD%"
>>"!LOG_FILE!" echo Source prefix: "%SOURCE_PREFIX%"
>>"!LOG_FILE!" echo Target prefix: "%TARGET_PREFIX%"
>>"!LOG_FILE!" echo.

set /a PREFIX_LENGTH=0
set "PREFIX_REMAINDER=%SOURCE_PREFIX%"

:count_prefix
if defined PREFIX_REMAINDER (
	set "PREFIX_REMAINDER=!PREFIX_REMAINDER:~1!"
	set /a PREFIX_LENGTH+=1
	goto count_prefix
)

set /a RENAMED_COUNT=0
set /a SKIPPED_COUNT=0
set /a ERROR_COUNT=0

for /r "%CD%" %%F in (*.h *.cpp) do (
	set "FILE_NAME=%%~nxF"
	if "!FILE_NAME:~0,%PREFIX_LENGTH%!"=="%SOURCE_PREFIX%" (
		set "TARGET_NAME=%TARGET_PREFIX%!FILE_NAME:~%PREFIX_LENGTH%!"
		if exist "%%~dpF!TARGET_NAME!" (
			echo Collision: "%%~fF" ^-^> "%%~dpF!TARGET_NAME!"
			>>"!LOG_FILE!" echo SKIPPED collision: "%%~fF" ^-^> "%%~dpF!TARGET_NAME!"
			set /a SKIPPED_COUNT+=1
			set /a ERROR_COUNT+=1
		) else (
			echo ren "%%~fF" "!TARGET_NAME!"
			ren "%%~fF" "!TARGET_NAME!"
			if errorlevel 1 (
				>>"!LOG_FILE!" echo SKIPPED rename-error: "%%~fF" ^-^> "%%~dpF!TARGET_NAME!"
				set /a SKIPPED_COUNT+=1
				set /a ERROR_COUNT+=1
			) else (
				>>"!LOG_FILE!" echo RENAMED: "%%~fF" ^-^> "%%~dpF!TARGET_NAME!"
				set /a RENAMED_COUNT+=1
			)
		)
	)
)

>>"!LOG_FILE!" echo.
>>"!LOG_FILE!" echo Renamed: !RENAMED_COUNT! file(s). Skipped: !SKIPPED_COUNT! file(s). Errors: !ERROR_COUNT!.
echo Renamed: !RENAMED_COUNT! file(s). Skipped: !SKIPPED_COUNT! file(s). Errors: !ERROR_COUNT!.
echo Log: "!LOG_FILE!"
if !ERROR_COUNT! neq 0 exit /b 1
exit /b 0

:usage
echo Usage: %~nx0 ^<source-prefix^> ^<target-prefix^>
echo Runs recursively from the current working directory.
exit /b 2
