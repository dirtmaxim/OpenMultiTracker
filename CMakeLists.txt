cmake_minimum_required(VERSION 3.6)
project(HockeyPlayersRecognizer)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Specific libraries.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lopencv_core -lopencv_features2d -lopencv_calib3d -lopencv_highgui")

# Image libraries.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lopencv_imgproc -lopencv_imgcodecs")

# Video libraries.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lopencv_videoio -lopencv_video -lopencv_videostab")

# Current directory as working directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(SOURCE_FILES main.cpp RinkBackgroundSubtractor.cpp RinkBackgroundSubtractor.h VideoHandler.cpp VideoHandler.h RegionChooser.cpp RegionChooser.h)
add_executable(HockeyPlayersRecognizer ${SOURCE_FILES})

# BGS library. But you also need to include "package_bgs" and "config" directories. Library was built for Mac OS X.
if (APPLE)
    target_link_libraries(HockeyPlayersRecognizer ${CMAKE_CURRENT_SOURCE_DIR}/libbgs.dylib)
endif (APPLE)