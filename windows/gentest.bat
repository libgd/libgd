SETLOCAL ENABLEDELAYEDEXPANSION
@echo off
set testsdir=%~1
set builddir=%~2
set CFLAGS=%~3
set LDFLAGS=%~4
set CC=%~5
set LD=%~6
set TESTMK=%~7
set TESTLIST=%~8
echo %*
echo recieved !testsdir! !builddir! !CFLAGS! !LDFLAGS! !CC!

copy NUL !TESTMK! > nul
copy NUL !TESTLIST! > nul

for /D %%d in (!testsdir!/*) do (
	if NOT "%%d"=="fontconfig" if NOT "%%d"=="gdtest" for %%f in (!testsdir!/%%d/*.c) do (
		echo TEST_EXES=!builddir!\tests\%%d_%%~nf.exe ^$^(TEST_EXES^) >> !TESTMK!
	)
)

for /D %%d in (!testsdir!/*) do (
    if NOT "%%d"=="fontconfig" if NOT "%%d"=="gdtest" for %%f in (!testsdir!/%%d/*.c) do (
		echo !builddir!\%%d_%%~nf.obj: !testsdir!\%%d\%%f; ^$^(CC^) ^$^(TEST_CFLAGS^) /c ^$** /Fd$*.pdb /Fo:$@  >> !TESTMK!
	)
)

for /D %%d in (!testsdir!/*) do (
    if NOT "%%d"=="fontconfig" if NOT "%%d"=="gdtest" for %%f in (!testsdir!/%%d/*.c) do (
		echo !builddir!\tests\%%d_%%~nf.exe: !builddir!\%%d_%%~nf.obj; !LD! !LDFLAGS! $** /out:$@ >> !TESTMK!
		echo %%d_%%~nf.exe >> !TESTLIST!
	)
)
ENDLOCAL
