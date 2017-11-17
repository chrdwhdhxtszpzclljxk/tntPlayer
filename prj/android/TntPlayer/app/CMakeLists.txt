# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

ADD_DEFINITIONS(-DFIXED_POINT)
ADD_DEFINITIONS(-DUSE_KISS_FFT)



#include_directories(../../../../crossany/external/curl/include/android)


#add_subdirectory(src/main/cpp/)
#add_subdirectory(../../../../crossany/external/curl-7.55.1 curl.out)

#message(${CROSSANY_SRC})

add_subdirectory(src/main/cpp/libspeex)
AUX_SOURCE_DIRECTORY(src/main/cpp/libspeex LIBSPEEX)

add_subdirectory(src/main/cpp/android)
AUX_SOURCE_DIRECTORY(src/main/cpp/android LIBANDROID)

add_subdirectory(src/main/cpp/cryptopp565)
AUX_SOURCE_DIRECTORY(src/main/cpp/cryptopp565 LIBCRYPTOPP565)

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-lib.cpp
             src/main/cpp/httpclient.cpp
             src/main/cpp/httpsdownloader.cpp
             src/main/cpp/crossanyapp.cpp
             src/main/cpp/gtmvreader.cpp
             src/main/cpp/basesocket.cpp
             src/main/cpp/speex_jni.cpp
             ${LIBSPEEX}
             ${LIBANDROID}
             ${LIBCRYPTOPP565}
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

find_library( # Sets the name of the path variable.
          android-lib

          # Specifies the name of the NDK library that
          # you want CMake to locate.
          android )


find_library( # Sets the name of the path variable.
          OpenSLES-lib

          # Specifies the name of the NDK library that
          # you want CMake to locate.
          OpenSLES )


# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                        ${android-lib}
                        ${OpenSLES-lib}
                       )