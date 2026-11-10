@echo off
setlocal

set "ROOT=%~dp0"
set "MSBUILD=C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\amd64\MSBuild.exe"
set "WSL_ROOT=/mnt/d/enterprise/programming_sciences/codebases/Gatewatch/bundle_galaxy_hell/gpk/gpk_transfer"
set "WSL_DEMO=/tmp/gpk-transfer-demo-lls"
set "PORT=37941"
set "SOURCE_ROOT=%ROOT%demo\windows\source"
set "RECEIVE_ROOT=%ROOT%demo\windows\returned"
set "WINDOWS_PEER=%ROOT%build\x64.Release\gpk_transfer.peer.exe"

if /i "%~1"=="--server" goto run_server

if not exist "%ROOT%demo" mkdir "%ROOT%demo"

echo Building the WSL peer...
wsl.exe -d Ubuntu -- make -C "%WSL_ROOT%" -f Makefile.wsl release
if errorlevel 1 exit /b 1

echo Building the Windows peer...
"%MSBUILD%" "%ROOT%gpk_transfer.peer.vcxproj" /t:Build /p:Configuration=Release /p:Platform=x64 /m:1 /nologo
if errorlevel 1 exit /b 1

call :roundtrip transfer-150KB.bin 150000
if errorlevel 1 exit /b 1
call :roundtrip transfer-1.50MB.bin 1500000
if errorlevel 1 exit /b 1
call :roundtrip transfer-15.0MB.bin 15000000
if errorlevel 1 exit /b 1
call :roundtrip transfer-150.0MB.bin 150000000
if errorlevel 1 exit /b 1

echo PASS: all four Windows -^> WSL -^> Windows transfers are byte-identical.
exit /b 0

:roundtrip
set "FILE_NAME=%~1"
set "FILE_SIZE=%~2"
set "RESOURCE=incoming/%FILE_NAME%"
set "SOURCE_FILE=%SOURCE_ROOT%\incoming\%FILE_NAME%"
set "RETURNED_FILE=%RECEIVE_ROOT%\incoming\%FILE_NAME%"
set "SERVER_LOG=%ROOT%demo\wsl-%FILE_NAME%.log"

echo Creating %FILE_NAME% with %FILE_SIZE% bytes...
"%WINDOWS_PEER%" --generate "%SOURCE_FILE%" %FILE_SIZE%
if errorlevel 1 exit /b 1

echo Starting the WSL receiver/return peer for %FILE_NAME%...
if exist "%SERVER_LOG%" del /q "%SERVER_LOG%"
start "" /b cmd.exe /d /c call "%~f0" --server "%FILE_NAME%"

for /l %%W in (1,1,30) do (
	findstr /c:"WSL peer listening" "%SERVER_LOG%" >nul 2>&1 && goto server_ready
	ping.exe -n 2 127.0.0.1 >nul
)
echo FAIL: the WSL peer did not become ready for %FILE_NAME%.
exit /b 1

:server_ready

echo Sending %FILE_NAME% Windows -^> WSL -^> Windows...
"%WINDOWS_PEER%" --connect 127.0.0.1 %PORT% "%SOURCE_ROOT%" "%RECEIVE_ROOT%" "%RESOURCE%"
if errorlevel 1 exit /b 1

fc /b "%SOURCE_FILE%" "%RETURNED_FILE%" >nul
if errorlevel 1 exit /b 1

wsl.exe -d Ubuntu -- cmp -s "%WSL_ROOT%/demo/windows/source/incoming/%FILE_NAME%" "%WSL_DEMO%/incoming/%FILE_NAME%"
if errorlevel 1 exit /b 1

echo PASS: %FILE_NAME% is byte-identical at all three locations.
exit /b 0

:run_server
wsl.exe -d Ubuntu -- %WSL_ROOT%/build/wsl/gpk_transfer.peer --listen %PORT% %WSL_DEMO% > "%ROOT%demo\wsl-%~2.log" 2>&1
exit /b %errorlevel%
