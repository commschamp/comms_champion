if NOT %TOOLCHAIN%=="msvc14" (
    echo Toolchain %TOOLCHAIN% is not supported
    exit -1
)

if %PLATFORM%=="x86" (
    call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86
    set QTDIR="C:\Qt\5.7\msvc2015"
)
else (
    call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
    set QTDIR="C:\Qt\5.7\msvc2015_64"    
)

