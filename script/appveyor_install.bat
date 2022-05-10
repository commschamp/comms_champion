IF "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2015" (
    set TOOLCHAIN=msvc14
    set QT_SUBDIR=msvc2015
    set QT_VER=5.6
    IF "%PLATFORM%"=="x86" (
        echo Performing x86 build in VS2015
        call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86
    ) ELSE (
        echo Performing amd64 build in VS2015
        call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
    )
) ELSE IF "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2017" (
    set TOOLCHAIN=msvc15
    set QT_SUBDIR=msvc2017
    set QT_VER=5.11
    IF "%PLATFORM%"=="x86" (
        echo Performing x86 build in VS2017
        call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
    ) ELSE (
        echo Performing amd64 build in VS2017
        call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
    )
    
) ELSE IF "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2019" (
    set TOOLCHAIN=msvc16
    set QT_SUBDIR=msvc2019
    set QT_VER=5.15.2
    IF "%PLATFORM%"=="x86" (
        echo Performing x86 build in VS2019
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"
    ) ELSE (
        echo Performing amd64 build in VS2019
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
    )
) ELSE IF "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2022" (
    set TOOLCHAIN=msvc17
    set QT_SUBDIR=msvc2019
    set QT_VER=5.15.2
    IF "%PLATFORM%"=="x86" (
        echo Performing x86 build in VS2022
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"
    ) ELSE (
        echo Performing amd64 build in VS2022
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    )  
) ELSE (
    echo Toolchain %TOOLCHAIN% is not supported
    exit -1
)

set QTDIR_PREFIX=C:/Qt/%QT_VER%
IF "%PLATFORM%"=="x86" (
    set QTDIR_SUFFIX=
) ELSE (
    set QTDIR_SUFFIX=_64
)

set QTDIR=%QTDIR_PREFIX%/%QT_SUBDIR%%QTDIR_SUFFIX%
IF NOT EXIST %QTDIR% (
    echo WARNING: %QTDIR% does not exist!!!
    set QTDIR=%QTDIR_PREFIX%/msvc2015%QTDIR_SUFFIX%
)

echo Using Qt5 from %QTDIR%
