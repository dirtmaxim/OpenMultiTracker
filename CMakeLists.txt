cmake_minimum_required(VERSION 3.6)
project(HockeyPlayersRecognizer)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Current directory as working directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(SOURCE_FILES main.cpp RinkBackgroundSubtractor.cpp RinkBackgroundSubtractor.h VideoHandler.cpp VideoHandler.h RegionChooser.cpp RegionChooser.h)
add_executable(HockeyPlayersRecognizer ${SOURCE_FILES})

# OpenCV 3 is required.
find_package(OpenCV REQUIRED)
target_link_libraries(HockeyPlayersRecognizer ${OpenCV_LIBS})

# BGS library. But you also need to include "package_bgs" and "config" directories. Library was built for Mac OS X.
if (APPLE)
    target_link_libraries(HockeyPlayersRecognizer ${CMAKE_CURRENT_SOURCE_DIR}/libbgs.dylib)
endif (APPLE)