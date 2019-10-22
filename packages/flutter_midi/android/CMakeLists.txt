cmake_minimum_required(VERSION 3.4.1)

set (OBOE_DIR ./oboe)
add_subdirectory(${OBOE_DIR} ./oboe)
include_directories(${OBOE_DIR}/include)

set (TSF_DIR ./TinySoundFont)
include_directories(${TSF_DIR})

add_library(
    android_tsf_player
    SHARED
    ./src/main/cpp/player.cpp)

find_library( # Sets the name of the path variable.
          android-lib

          # Specifies the name of the NDK library that
          # you want CMake to locate.
          android )

target_link_libraries(android_tsf_player oboe ${android-lib})

