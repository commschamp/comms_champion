IF "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2015" (
    set TOOLCHAIN=msvc14
    IF "%PLATFORM%"=="x86" (
        echo Performing x86 build in VS2015
        call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86
        set QTDIR="C:\Qt\%QT_VER%\msvc2015"
    ) ELSE (
        echo Performing amd64 build in VS2015
        call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
        set QTDIR="C:\Qt\%QT_VER%\msvc2015_64"    
    )
) ELSE IF "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2017" (
    set TOOLCHAIN=msvc15
    IF "%PLATFORM%"=="x86" (
        echo Performing x86 build in VS2017
        call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
        set QTDIR="C:\Qt\%QT_VER%\msvc2015"
    ) ELSE (
        echo Performing amd64 build in VS2017
        call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
        set QTDIR="C:\Qt\%QT_VER%\msvc2015_64"    
    )
) ELSE (
    echo Toolchain %TOOLCHAIN% is not supported
    exit -1
)
