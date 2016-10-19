#--------------------------------------------------------------------------
# ai2_pathfinding project
#--------------------------------------------------------------------------


cmake_minimum_required(VERSION 2.8.11)

project(ai2_pathfinding)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
#Runs UIC if ui_XXX.h is included in headers and expects XXX.ui
SET(CMAKE_AUTOUIC ON)
#QT Resource files
SET(CMAKE_AUTORCC ON)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

FIND_PACKAGE(Qt5Widgets REQUIRED)

INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDES})			

FILE(GLOB HEAD "code/*.h")
FILE(GLOB SRC "code/*.cpp")
LIST(APPEND CODE ${HEAD} ${SRC})
SOURCE_GROUP("code" FILES ${CODE})

FILE(GLOB UI "code/*.ui")
SOURCE_GROUP("ui" FILES ${UI})

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
ADD_EXECUTABLE(ai2_pathfinding ${CODE} ${UI})

SET_TARGET_PROPERTIES(ai2_pathfinding PROPERTIES FOLDER "LABS")
TARGET_LINK_LIBRARIES(ai2_pathfinding Qt5::Widgets mymathlib dirent)

#MESSAGE( STATUS "CMAKE_BINARY_DIR:         " ${FORMS} )
ADD_CUSTOM_COMMAND(TARGET ai2_pathfinding PRE_BUILD
					COMMAND ${CMAKE_COMMAND} -E make_directory
						${CMAKE_CURRENT_SOURCE_DIR}/bin)

#copy maps to bin
ADD_CUSTOM_COMMAND(TARGET ai2_pathfinding POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_directory
						${CMAKE_CURRENT_SOURCE_DIR}/code/Maps ${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug/Maps)