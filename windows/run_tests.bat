@echo off
setlocal EnableDelayedExpansion
if [%PLATFORM%]==[] set PLATFORM=x86
if [%PREFERREDTOOLARCHITECTURE%]==[x64] set PLATFORM=x64
set PLATFORM=%PLATFORM:X=x%

set total=0
set total_passed=0
set total_failed=0
set total_missing=0

for /f %%T in (testlist.txt) do (
  set test=%%T
  set output=test: !test!....................................................................
  set target=%%~nT.exe
  if not exist !target! SET result=NotFound
  call !target! && (SET result=Passed) || (SET result=Failed)

  rem > last_test.out 2> last_test.err
  rem set /p testerr=<last_test.err
  rem set /p testout=<last_test.out

  echo !output:~0,70!!result!

  rem if defined testerr echo error: !testerr!
  rem if defined testout echo msg: !testout!

  set /a total=total+1
  if "!result!"=="Passed" set /a total_passed=total_passed+1
  if "!result!"=="Failed" set /a total_failed=total_failed+1
  if "!result!"=="NotFound" set /a total_missing=total_missing+1

  if defined APPVEYOR (
    appveyor AddTest "!test! %PLATFORM%" -Outcome !result! -Framework Custom -Filename %%~nT.exe -Duration 0
  )
)
echo.
echo Tests:   !total!
echo Passed:  !total_passed!
echo Failed:  !total_failed!
echo Missing: !total_missing!

exit /b 0
