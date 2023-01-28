IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(ARCH_BITS 64)
ELSE()
    SET(ARCH_BITS 32)
ENDIF()

if( ANDROID )
    # android platform
    set( TARGET_ARCH ${CMAKE_ANDROID_ARCH_ABI} )
    if( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fdeclspec -g -Wall -pthread")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fdeclspec -pthread")
        add_link_options("-Wl,--build-id=sha1")
        set(TARGET_ARCH ${CMAKE_ANDROID_ARCH_ABI} )
    else()
        message("why not clang!?!?")
    endif()
    # add basic NDK native library
    find_library( log log )
    find_library( android android )
elseif( CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall -pthread")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -pthread")
elseif( APPLE )
    if( IOS )
        set( TARGET_ARCH iOS )                 # iOS platform
    else()
        set( TARGET_ARCH macOS )               # macOS platform
    endif()
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++14 -g -Wall")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -std=c99 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++14 -O2 -Wunused-function")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -std=c99 -O2 -pthread")
elseif( WIN32 )
    # windows platform
    if( DEFINED TARGET_ARCH )
        if( ${TARGET_ARCH} STREQUAL x86 )
        elseif( ${TARGET_ARCH} STREQUAL x64 )
        else()
            set( TARGET_ARCH x64 )
            message("Invalid Win32 Architechture, set architecture to 'x64'...")
        endif()
    else()
        set( TARGET_ARCH x64 )
        message("Architechture was not set, set architecture to 'x64'...")
    endif()
    # MingW Compilers
    if( MINGW )
        message("compiler: mingw")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++14 -g -Wall -pthread")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -std=c99 -g -pthread")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++14 -O2 -pthread -Wunused-function")
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -pthread")
        #
        if( GX_BITS STREQUAL 64 )
            set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -m64")
            set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -m64")
        elseif(GX_BITS STREQUAL 32)
            set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
        endif()
    endif()
    # Microsoft Visual C++ Compilers
    if(MSVC)
        message("compiler: msvc")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /bigobj")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /std:c++17 /bigobj")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /std:c++17 /bigobj /fp:fast /Gy /Oi /Oy /O2 /Ot /Zi /EHsc ")
        ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
        IF(CMAKE_CL_64)
            set( TARGET_ARCH "x64" )
        ELSE(CMAKE_CL_64)
            set( TARGET_ARCH "x86" )
        ENDIF(CMAKE_CL_64)
    elseif( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
        message("compiler: clang")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++14 -g -Wall -pthread")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -std=c99 -g -pthread")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++14 -O2 -pthread -Wunused-function")
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -pthread")
        # set(TARGET_ARCH ${CMAKE_ANDROID_ARCH_ABI} )
    endif()
endif()

message( "target platform : ${CMAKE_SYSTEM_NAME}")

set( SOLUTION_DIR ${CMAKE_CURRENT_SOURCE_DIR} )