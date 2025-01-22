@echo off

SET PATH=%PATH%;"%ProgramFiles(x86)%\Git";"%ProgramFiles%\Git";"%ProgramFiles(x86)%\SmartGit\git\bin";"%ProgramFiles%\SmartGit\git\bin";

if exist ..\..\Compiler (
    cd ..\..\Compiler
	git pull --rebase
) else (
	git clone https://github.com/We-the-People-civ4col-mod/Compiler.git ..\..\Compiler
    cd ..\..\Compiler
)

if not exist ..\..\tbb.dll (
	echo Copying tbb.dll to Colonization.exe
	copy DLL_for_exe\tbb.dll ..\..\
)
if not exist ..\..\tbbmalloc.dll (
	echo Copying tbbmalloc.dll to Colonization.exe
	copy DLL_for_exe\tbbmalloc.dll ..\..\
)

rem test is perl.exe is in PATH
rem  where /q perl.exe
rem  if ERRORLEVEL 1 (
rem  	echo No perl found
rem  	echo Perl is required for the compiler to work properly.
rem  	echo Please install the 64 bit version of Strawberry perl. Hit any key to open the download URL.
rem  	echo The compiler should be fully installed and working once perl is installed.
rem  ) else (
rem  	echo "ok"
rem  )
