cmake_minimum_required(VERSION 3.6)
project(HockeyPlayersRecognizer)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# Current directory as working directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(SOURCE_FILES main.cpp RinkBackgroundSubtractor.cpp RinkBackgroundSubtractor.h VideoHandler.cpp VideoHandler.h Utils/RegionChooser.cpp Utils/RegionChooser.h Utils/OcclusionHandler.cpp Utils/OcclusionHandler.h Developing/Tracker.cpp Developing/Tracker.h Developing/MotionModel.cpp Developing/MotionModel.h Utils/defines.h Developing/DataAssociationAlgorithm.cpp Developing/DataAssociationAlgorithm.h Detector/MovingObjectDetector.cpp Detector/MovingObjectDetector.h)
add_executable(HockeyPlayersRecognizer ${SOURCE_FILES})

# OpenCV 3 is required.
find_package(OpenCV REQUIRED)
target_link_libraries(HockeyPlayersRecognizer ${OpenCV_LIBS})