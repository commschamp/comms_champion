IF "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2015" (
    set TOOLCHAIN=msvc14
    IF "%PLATFORM%"=="x86" (
        echo Performing x86 build in VS2015
        call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86
    ) ELSE (
        echo Performing amd64 build in VS2015
        call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
    )
) ELSE IF "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2017" (
    set TOOLCHAIN=msvc15
    IF "%PLATFORM%"=="x86" (
        echo Performing x86 build in VS2017
        call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
    ) ELSE (
        echo Performing amd64 build in VS2017
        call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
    )
) ELSE (
    echo Toolchain %TOOLCHAIN% is not supported
    exit -1
)

set QTDIR_PREFIX=C:\Qt\%QT_VER%
set QT_DIR_SUFFIX=
IF "%PLATFORM%"=="x64" (
    set QT_DIR_SUFFIX=_64
)    

set QTDIR=%QTDIR_PREFIX%\%TOOLCHAIN%%QT_DIR_SUFFIX%
IF NOT EXIST %QTDIR% (
    set QTDIR=%QTDIR_PREFIX%\msvc215%QT_DIR_SUFFIX%
)

echo Using Qt5 from %QTDIR%
