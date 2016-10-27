#--------------------------------------------------------------------------
# camera project
#--------------------------------------------------------------------------

PROJECT(camera)
FILE(GLOB camera_headers *.h)
FILE(GLOB camera_sources *.cpp)

SET(files_camera
	${camera_headers} 
	${camera_sources})

SOURCE_GROUP("camera" FILES ${files_camera})

ADD_LIBRARY(camera STATIC ${files_camera})
TARGET_LINK_LIBRARIES(camera mymathlib)
SET_TARGET_PROPERTIES(camera PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(camera PROPERTIES FOLDER "MyLibs")
TARGET_INCLUDE_DIRECTORIES(camera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})