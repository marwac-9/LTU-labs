#--------------------------------------------------------------------------
# obj project
#--------------------------------------------------------------------------

PROJECT(obj)
FILE(GLOB obj_headers *.h)
FILE(GLOB obj_sources *.cpp)

SET(files_obj
	${obj_headers} 
	${obj_sources})

SOURCE_GROUP("obj" FILES ${files_obj})

ADD_LIBRARY(obj STATIC ${files_obj})
TARGET_LINK_LIBRARIES(obj mymathlib)
SET_TARGET_PROPERTIES(obj PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(obj PROPERTIES FOLDER "MyLibs")
TARGET_INCLUDE_DIRECTORIES(obj PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})