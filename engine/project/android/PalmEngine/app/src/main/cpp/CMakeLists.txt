# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)

# Declares and names the project.

project("palmengine")

include_directories(../../../../../../../include)
include_directories(../../../../../../../src/pinyin)

SET(LIB_PINYIN_SRC
        ../../../../../../../src/pinyin/PDLexBeijing.c
        ../../../../../../../src/pinyin/PDLexBeijing.h
        ../../../../../../../src/pinyin/PDLexCore.c
        ../../../../../../../src/pinyin/PDLexName.c
        ../../../../../../../src/pinyin/PDLexName.h
        ../../../../../../../src/pinyin/PYCand.c
        ../../../../../../../src/pinyin/PYCand.h
        ../../../../../../../src/pinyin/PYConst.h
        ../../../../../../../src/pinyin/PYDyn.c
        ../../../../../../../src/pinyin/PYDyn.h
        ../../../../../../../src/pinyin/PYDynEx.c
        ../../../../../../../src/pinyin/PYEngine.c
        ../../../../../../../src/pinyin/PYEngineEx.c
        ../../../../../../../src/pinyin/PYFix.c
        ../../../../../../../src/pinyin/PYFix.h
        ../../../../../../../src/pinyin/PYGraph.c
        ../../../../../../../src/pinyin/PYGraph.h
        ../../../../../../../src/pinyin/PYHan.c
        ../../../../../../../src/pinyin/PYHan.h
        ../../../../../../../src/pinyin/PYHanData.c
        ../../../../../../../src/pinyin/PYHanData.h
        ../../../../../../../src/pinyin/PYHanDef.h
        ../../../../../../../src/pinyin/PYKey.c
        ../../../../../../../src/pinyin/PYKey.h
        ../../../../../../../src/pinyin/PYLex.c
        ../../../../../../../src/pinyin/PYLex.h
        ../../../../../../../src/pinyin/PYLexEx.cpp
        ../../../../../../../src/pinyin/PYLexEx.h
        ../../../../../../../src/pinyin/PYName.c
        ../../../../../../../src/pinyin/PYName.h
        ../../../../../../../src/pinyin/PYPredict.c
        ../../../../../../../src/pinyin/PYPredict.h
        ../../../../../../../src/pinyin/PYPriDef.h
        ../../../../../../../src/pinyin/PYUtils.c
        ../../../../../../../src/pinyin/PYUtils.h

        )

add_library( # Sets the name of the library.
        pinyin

        # Sets the library as a shared library.
        STATIC

        # Provides a relative path to your source file(s).
        ${LIB_PINYIN_SRC}
        )

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             palmengine-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             PYAdapter.c
             PYAdapter.h
             PYProvider.cpp
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

set(OUTPUT_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../out)
set_target_properties(palmengine-lib PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY
        ${OUTPUT_LIBS_DIR}/${ANDROID_ABI})

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        palmengine-lib
        android
        pinyin
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib} )