@echo off
setlocal EnableDelayedExpansion
if [%PLATFORM%]==[] set PLATFORM=x86
if [%PREFERREDTOOLARCHITECTURE%]==[x64] set PLATFORM=x64
set PLATFORM=%PLATFORM:X=x%

rem if exist build_msvc12_%PLATFORM%\tests (cd build_msvc12_%PLATFORM%\tests) else (
rem   if exist ..\build_%PLATFORM%\tests (cd ..\build_msvc12_%PLATFORM%\tests) else (
rem     echo unable to find test dir
rem     exit /b 1
rem   )
rem )

for /f %%T in (testlist.txt) do (
  set test=%%T
  set output=test: !test!....................................................................
  echo !target!
  set target=%%~nT.exe
  if not exist !target! SET result=NotFound
  call !target! && (SET result=Passed) || (SET result=Failed)
  
  rem > last_test.out 2> last_test.err
  rem set /p testerr=<last_test.err
  rem set /p testout=<last_test.out
  
  echo !output:~0,70!!result!
  
  rem if defined testerr echo error: !testerr!
  rem if defined testout echo msg: !testout!
  
  if defined APPVEYOR (
    appveyor AddTest "!test! %PLATFORM%" -Outcome !result! -Framework Custom -Filename %%~nT.exe -Duration 0
  )
)

exit /b 0