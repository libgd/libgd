@echo off
setlocal EnableDelayedExpansion

if exist test (cd test) else (
  if exist ..\test (cd ..\test) else (
    echo unable to find test dir
    exit /b 1
  )
)

if "%PLATFORM%"=="" set PLATFORM=86

for /f %%T in (testlist.txt) do (
  set test=%%T
  set output=test: !test!....................................................................
  
  set target=..\build_x%PLATFORM%\%%~nT.exe
  call !target! && (SET result=Passed) || (SET result=Failed)
  
  echo !output:~0,73!!result!
  if defined APPVEYOR (
    appveyor AddTest "!test! x86" -Outcome !result! -Framework Custom -Filename %%~nT.exe -Duration 0
  )
)

exit /b 0