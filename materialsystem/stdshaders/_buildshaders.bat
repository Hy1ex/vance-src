@echo off

@REM == Setup path to nmake.exe ==
if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" (
	for /f "usebackq tokens=1* delims=: " %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.VisualStudio.Workload.NativeDesktop`) do (
		if /i "%%i"=="installationPath" (
			set VSDIR=%%j
			call "!VSDIR!\Common7\Tools\VsDevCmd.bat" >nul
			goto :start
		)
	)	
) else if exist "%VS140COMNTOOLS%vsvars32.bat" (
	call "%VS140COMNTOOLS%vsvars32.bat"
	echo Using Visual Studio 2015 nmake
	
) else if exist "%VS120COMNTOOLS%vsvars32.bat" (
	call "%VS120COMNTOOLS%vsvars32.bat"
	echo Using Visual Studio 2013 nmake
	
) else if exist "%VS100COMNTOOLS%vsvars32.bat" (
	call "%VS100COMNTOOLS%vsvars32.bat"
	echo Using Visual Studio 2010 nmake
	
) else (
	echo.
	echo Install Either Visual Studio Version 2010 through 2019
	pause
	exit
)

:start
set TTEXE=..\..\devtools\bin\timeprecise.exe
if not exist %TTEXE% goto no_ttexe
goto no_ttexe_end

:no_ttexe
set TTEXE=time /t
:no_ttexe_end

@REM echo ==================== buildshaders %* ==================
%TTEXE% -cur-Q
set tt_start=%ERRORLEVEL%
set tt_chkpt=%tt_start%


@REM --------------------------------------------------------
@REM usage: buildshaders <shaderProjectName>
@REM --------------------------------------------------------

setlocal
set arg_filename=%1
set shadercompilecommand=ShaderCompile.exe
set SrcDirBase=..\..
set shaderDir=shaders

if "%1" == "" goto usage
set inputbase=%1

@REM TODO: replace this slow af dx_proxy with one from quiver

set DIRECTX_SDK_VER=pc09.00
set DIRECTX_SDK_BIN_DIR=dx9sdk\utilities

if /i "%7" == "-dx9_30" goto dx_sdk_dx9_30
goto dx_sdk_end
:dx_sdk_dx9_30
			set DIRECTX_SDK_VER=pc09.30
			set DIRECTX_SDK_BIN_DIR=dx10sdk\Utilities\dx9_30
			goto dx_sdk_end
:dx_sdk_end

if /i "%8" == "-force30" goto set_force30_arg
goto set_force_end
:set_force30_arg
			set DIRECTX_FORCE_MODEL=30
			goto set_force_end
:set_force_end

if /i "%2" == "-game" goto set_mod_args
if /i "%6" == "1" set dynamic_shaders=1
goto build_shaders

@REM --------------------------------------------------------
@REM USAGE
@REM --------------------------------------------------------
:usage
echo.
echo "usage: buildshaders <shaderProjectName> [-dx10 or -game] [gameDir if -game was specified] [-source sourceDir]"
echo "       gameDir is where gameinfo.txt is (where it will store the compiled shaders)."
echo "       sourceDir is where the source code is (where it will find scripts and compilers)."
echo "ex   : buildshaders myshaders"
echo "ex   : buildshaders myshaders -game c:\steam\steamapps\sourcemods\mymod -source c:\mymod\src"
goto :end

@REM --------------------------------------------------------
@REM MOD ARGS - look for -game or the vproject environment variable
@REM --------------------------------------------------------
:set_mod_args

if not exist "%ENGINEDIR%\bin\shadercompile.exe" goto NoShaderCompile

if /i "%4" NEQ "-source" goto NoSourceDirSpecified
set "SrcDirBase=%~5"

REM ** use the -game parameter to tell us where to put the files
set targetdir=%~3\shaders
set SDKArgs=-nompi -nop4 -game "%~3"

if not exist "%~3\gameinfo.txt" goto InvalidGameDirectory
goto build_shaders

@REM --------------------------------------------------------
@REM ERRORS
@REM --------------------------------------------------------
:InvalidGameDirectory
echo -
echo Error: "%~3" is not a valid game directory.
echo (The -game directory must have a gameinfo.txt file)
echo -
goto end

:NoSourceDirSpecified
echo ERROR: If you specify -game on the command line, you must specify -source.
goto usage
goto end

:NoShaderCompile
echo -
echo - ERROR: shadercompile.exe doesn't exist in %ENGINEDIR%\bin
echo -
goto end

@REM --------------------------------------------------------
@REM BUILD SHADERS
@REM --------------------------------------------------------
:build_shaders

@REM echo ------------------------------------------------------------
@REM echo %inputbase%
@REM echo ------------------------------------------------------------
@REM make sure that target dirs exist
@REM files will be built in these targets and copied to their final destination
if not exist %shaderDir% mkdir %shaderDir%
if not exist %shaderDir%\fxc mkdir %shaderDir%\fxc

@REM Nuke some files that we will add to later.
if exist filelist.txt del /f /q filelist.txt
if exist filestocopy.txt del /f /q filestocopy.txt
if exist filelistgen.txt del /f /q filelistgen.txt
if exist inclist.txt del /f /q inclist.txt
if exist vcslist.txt del /f /q vcslist.txt
if exist uniquefilestocopy.txt del /f /q uniquefilestocopy.txt
if exist makefile.%inputbase% del /f /q makefile.%inputbase%

@REM --------------------------------------------------------
@REM Generate a makefile for the shader project
@REM creates vcslist.txt and and inclist.txt
@REM --------------------------------------------------------
perl "%SrcDirBase%\devtools\bin\updateshaders.pl" -source "%SrcDirBase%" %inputbase%

@REM --------------------------------------------------------
@REM Run the makefile, generating minimal work/build list for fxc files, go ahead and compile vsh and psh files.
@REM --------------------------------------------------------
@REM nmake /S /C -f makefile.%inputbase% clean > clean.txt 2>&1
@REM echo Building inc files, asm vcs files, and VMPI worklist for %inputbase%...
echo Creating makefile for %inputbase%...
nmake /S /C -f ./makefile.%inputbase%

@REM --------------------------------------------------------
@REM Copy the inc files to their target
@REM --------------------------------------------------------
if exist "inclist.txt" (
	echo Publishing shader inc files to target...
	perl %SrcDirBase%\devtools\bin\copyshaderincfiles.pl inclist.txt
)

@REM --------------------------------------------------------
@REM Add the executables to the worklist.
@REM --------------------------------------------------------

if /i "%DIRECTX_SDK_VER%" == "pc09.00" (
	@REM echo "Copy extra files for dx 9 std
)
if /i "%DIRECTX_SDK_VER%" == "pc09.30" (
	echo %SrcDirBase%\devtools\bin\d3dx9_33.dll >> filestocopy.txt
)
if /i "%DIRECTX_SDK_VER%" == "pc10.00" (
	echo %SrcDirBase%\devtools\bin\d3dx10_33.dll >> filestocopy.txt
)

echo %SrcDirBase%\%DIRECTX_SDK_BIN_DIR%\dx_proxy.dll >> filestocopy.txt

echo %ENGINEDIR%\bin\shadercompile.exe >> filestocopy.txt
echo %ENGINEDIR%\bin\shadercompile_dll.dll >> filestocopy.txt
echo %ENGINEDIR%\bin\vstdlib.dll >> filestocopy.txt
echo %ENGINEDIR%\bin\tier0.dll >> filestocopy.txt

@REM --------------------------------------------------------
@REM Cull duplicate entries in work/build list
@REM --------------------------------------------------------
if exist filestocopy.txt type filestocopy.txt | perl "%SrcDirBase%\devtools\bin\uniqifylist.pl" > uniquefilestocopy.txt
if exist filelistgen.txt if not "%dynamic_shaders%" == "1" (
    echo Generating action list...
    copy filelistgen.txt filelist.txt >nul
)

@REM --------------------------------------------------------
@REM Execute distributed process on work/build list
@REM --------------------------------------------------------
set shader_path_cd=%cd%
if exist "filelist.txt" if exist "uniquefilestocopy.txt" if not "%dynamic_shaders%" == "1" (
	@REM checking if shader compile is running
	call _kill_shadercompiler.bat
	
	echo Building shaders...
	cd /D "%ENGINEDIR%\bin"
	
	@REM %shadercompilecommand% -mpi_MaxWorkers %shadercompileworkers% -shaderpath "%shader_path_cd:/=\%" -allowdebug
	@REM -verbose -subprocess X
	echo.
	@REM use all threads on your cpu
	%shadercompilecommand% %SDKArgs% -threads %NUMBER_OF_PROCESSORS% -shaderpath "%shader_path_cd:/=\%" -verbose -verbose2 -verbose_preprocessor -verbose_errors
	
	echo.
	cd /D %shader_path_cd%
)

@REM delete the temporary files
if exist filelist.txt del /f /q filelist.txt
if exist filestocopy.txt del /f /q filestocopy.txt
if exist filelistgen.txt del /f /q filelistgen.txt
if exist inclist.txt del /f /q inclist.txt
if exist vcslist.txt del /f /q vcslist.txt
if exist uniquefilestocopy.txt del /f /q uniquefilestocopy.txt
if exist makefile.%inputbase% del /f /q makefile.%inputbase%

@REM --------------------------------------------------------
@REM PC Shader copy
@REM Publish the generated files to the output dir using XCOPY
@REM This batch file may have been invoked standalone or slaved (master does final smart mirror copy)
@REM --------------------------------------------------------
:DoXCopy
if not "%dynamic_shaders%" == "1" (
	if not exist "%targetdir%" md "%targetdir%"	
	xcopy "%cd%\shaders" "%targetdir%" /q /e /y	
)
goto end

@REM --------------------------------------------------------
@REM END
@REM --------------------------------------------------------
:end


%TTEXE% -diff %tt_start%